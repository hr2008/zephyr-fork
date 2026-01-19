/* SPDX-License-Identifier: Apache-2.0 */

#define DT_DRV_COMPAT ti_am335x_gpio

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_utils.h>
#include <zephyr/irq.h>
#include <zephyr/sys/util.h>

/* GPIO Register Offsets */
#define GPIO_SYSCONFIG          0x010
#define GPIO_SYSSTATUS          0x114
#define GPIO_CTRL               0x130
#define GPIO_OE                 0x134
#define GPIO_DATAIN             0x138
#define GPIO_DATAOUT            0x13C
#define GPIO_CLEARDATAOUT       0x190
#define GPIO_SETDATAOUT         0x194
#define GPIO_IRQSTATUS_0        0x02C
#define GPIO_IRQSTATUS_1        0x030
#define GPIO_IRQSTATUS_SET_0    0x034
#define GPIO_IRQSTATUS_SET_1    0x038
#define GPIO_IRQSTATUS_CLR_0    0x03C
#define GPIO_IRQSTATUS_CLR_1    0x040
#define GPIO_RISINGDETECT       0x148
#define GPIO_FALLINGDETECT      0x14C

#define GPIO_SYSCONFIG_SOFTRESET    BIT(1)
#define GPIO_SYSSTATUS_RESETDONE    BIT(0)

struct gpio_am335x_config {
	struct gpio_driver_config common;
	uint32_t base;
	uint32_t clock_reg;
	uint8_t ngpios;
};

struct gpio_am335x_data {
	struct gpio_driver_data common;
};

static inline uint32_t gpio_read(const struct device *dev, uint32_t reg)
{
	const struct gpio_am335x_config *cfg = dev->config;
	return sys_read32(cfg->base + reg);
}

static inline void gpio_write(const struct device *dev, uint32_t reg, uint32_t val)
{
	const struct gpio_am335x_config *cfg = dev->config;
	sys_write32(val, cfg->base + reg);
}

static void gpio_am335x_enable_clock(const struct device *dev)
{
	const struct gpio_am335x_config *cfg = dev->config;
	volatile uint32_t *clk_reg = (volatile uint32_t *)cfg->clock_reg;
	
	/* Enable module clock */
	*clk_reg = 0x2;
	
	/* Wait for module to be enabled */
	while ((*clk_reg & (0x3 << 16)) != 0) {
		/* Wait */
	}
}

static int gpio_am335x_configure(const struct device *dev,
				 gpio_pin_t pin,
				 gpio_flags_t flags)
{
	const struct gpio_am335x_config *cfg = dev->config;
	uint32_t oe;

	if (pin >= cfg->ngpios) {
		return -EINVAL;
	}

	oe = gpio_read(dev, GPIO_OE);

	if (flags & GPIO_OUTPUT) {
		/* Configure as output */
		oe &= ~BIT(pin);
		
		/* Set initial value if specified */
		if (flags & GPIO_OUTPUT_INIT_HIGH) {
			gpio_write(dev, GPIO_SETDATAOUT, BIT(pin));
		} else if (flags & GPIO_OUTPUT_INIT_LOW) {
			gpio_write(dev, GPIO_CLEARDATAOUT, BIT(pin));
		}
	} else if (flags & GPIO_INPUT) {
		/* Configure as input */
		oe |= BIT(pin);
	}

	gpio_write(dev, GPIO_OE, oe);

	return 0;
}

static int gpio_am335x_port_get_raw(const struct device *dev,
				    gpio_port_value_t *value)
{
	*value = gpio_read(dev, GPIO_DATAIN);
	return 0;
}

static int gpio_am335x_port_set_masked_raw(const struct device *dev,
					   gpio_port_pins_t mask,
					   gpio_port_value_t value)
{
	gpio_write(dev, GPIO_CLEARDATAOUT, mask & ~value);
	gpio_write(dev, GPIO_SETDATAOUT, mask & value);
	return 0;
}

static int gpio_am335x_port_set_bits_raw(const struct device *dev,
					 gpio_port_pins_t pins)
{
	gpio_write(dev, GPIO_SETDATAOUT, pins);
	return 0;
}

static int gpio_am335x_port_clear_bits_raw(const struct device *dev,
					   gpio_port_pins_t pins)
{
	gpio_write(dev, GPIO_CLEARDATAOUT, pins);
	return 0;
}

static int gpio_am335x_port_toggle_bits(const struct device *dev,
					gpio_port_pins_t pins)
{
	uint32_t out = gpio_read(dev, GPIO_DATAOUT);
	
	gpio_write(dev, GPIO_CLEARDATAOUT, pins & out);
	gpio_write(dev, GPIO_SETDATAOUT, pins & ~out);
	
	return 0;
}

static int gpio_am335x_pin_interrupt_configure(const struct device *dev,
					       gpio_pin_t pin,
					       enum gpio_int_mode mode,
					       enum gpio_int_trig trig)
{
	const struct gpio_am335x_config *cfg = dev->config;
	uint32_t rising, falling;

	if (pin >= cfg->ngpios) {
		return -EINVAL;
	}

	/* Disable interrupts for this pin */
	gpio_write(dev, GPIO_IRQSTATUS_CLR_0, BIT(pin));

	if (mode == GPIO_INT_MODE_DISABLED) {
		return 0;
	}

	rising = gpio_read(dev, GPIO_RISINGDETECT);
	falling = gpio_read(dev, GPIO_FALLINGDETECT);

	if (trig & GPIO_INT_TRIG_LOW) {
		falling |= BIT(pin);
	} else {
		falling &= ~BIT(pin);
	}

	if (trig & GPIO_INT_TRIG_HIGH) {
		rising |= BIT(pin);
	} else {
		rising &= ~BIT(pin);
	}

	gpio_write(dev, GPIO_RISINGDETECT, rising);
	gpio_write(dev, GPIO_FALLINGDETECT, falling);

	/* Enable interrupt */
	gpio_write(dev, GPIO_IRQSTATUS_SET_0, BIT(pin));

	return 0;
}

static int gpio_am335x_manage_callback(const struct device *dev,
				       struct gpio_callback *callback,
				       bool set)
{
	struct gpio_am335x_data *data = dev->data;

	return gpio_manage_callback(&data->common, callback, set);
}

static void gpio_am335x_isr(const struct device *dev)
{
	struct gpio_am335x_data *data = dev->data;
	uint32_t status;

	status = gpio_read(dev, GPIO_IRQSTATUS_0);
	
	/* Clear interrupt status */
	gpio_write(dev, GPIO_IRQSTATUS_0, status);

	gpio_fire_callbacks(&data->common, dev, status);
}

static int gpio_am335x_init(const struct device *dev)
{
	uint32_t sysconfig, sysstatus;

	/* Enable clock */
	gpio_am335x_enable_clock(dev);

	/* Soft reset GPIO module */
	sysconfig = gpio_read(dev, GPIO_SYSCONFIG);
	gpio_write(dev, GPIO_SYSCONFIG, sysconfig | GPIO_SYSCONFIG_SOFTRESET);

	/* Wait for reset complete */
	do {
		sysstatus = gpio_read(dev, GPIO_SYSSTATUS);
	} while ((sysstatus & GPIO_SYSSTATUS_RESETDONE) == 0);

	/* Configure GPIO module control */
	gpio_write(dev, GPIO_CTRL, 0);

	return 0;
}

static DEVICE_API(gpio, gpio_am335x_api) = {
	.pin_configure = gpio_am335x_configure,
	.port_get_raw = gpio_am335x_port_get_raw,
	.port_set_masked_raw = gpio_am335x_port_set_masked_raw,
	.port_set_bits_raw = gpio_am335x_port_set_bits_raw,
	.port_clear_bits_raw = gpio_am335x_port_clear_bits_raw,
	.port_toggle_bits = gpio_am335x_port_toggle_bits,
	.pin_interrupt_configure = gpio_am335x_pin_interrupt_configure,
	.manage_callback = gpio_am335x_manage_callback,
};

#define GPIO_AM335X_INIT(n)						\
	static const struct gpio_am335x_config gpio_am335x_config_##n = { \
		.common = {						\
			.port_pin_mask = GPIO_PORT_PIN_MASK_FROM_NGPIOS(DT_INST_PROP(n, ngpios)), \
		},							\
		.base = DT_INST_REG_ADDR(n),				\
		.clock_reg = (n == 0) ? CM_WKUP_GPIO0_CLKCTRL :		\
			     (n == 1) ? CM_PER_GPIO1_CLKCTRL :		\
			     (n == 2) ? CM_PER_GPIO2_CLKCTRL :		\
			     CM_PER_GPIO3_CLKCTRL,			\
		.ngpios = DT_INST_PROP(n, ngpios),			\
	};								\
									\
	static struct gpio_am335x_data gpio_am335x_data_##n;		\
									\
	static void gpio_am335x_irq_config_##n(void)			\
	{								\
		IRQ_CONNECT(DT_INST_IRQN(n),				\
			    0,						\
			    gpio_am335x_isr,				\
			    DEVICE_DT_INST_GET(n),			\
			    0);						\
		irq_enable(DT_INST_IRQN(n));				\
	}								\
									\
	DEVICE_DT_INST_DEFINE(n,					\
			      gpio_am335x_init,				\
			      NULL,					\
			      &gpio_am335x_data_##n,			\
			      &gpio_am335x_config_##n,			\
			      POST_KERNEL,				\
			      CONFIG_GPIO_INIT_PRIORITY,		\
			      &gpio_am335x_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_AM335X_INIT)