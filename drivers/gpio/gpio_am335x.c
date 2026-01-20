/*
 * Copyright (c) 2026
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ti_am335x_gpio

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/gpio/gpio_utils.h>
#include <zephyr/irq.h>
#include <zephyr/sys/sys_io.h>
#include <zephyr/sys/util.h>

/* --- AM335x GPIO register offsets (simplified) --- */
#define GPIO_OE            0x134
#define GPIO_DATAIN        0x138
#define GPIO_DATAOUT       0x13C
#define GPIO_SETDATAOUT    0x194
#define GPIO_CLEARDATAOUT  0x190
#define GPIO_IRQSTATUS_0   0x02C
#define GPIO_IRQSTATUS_1   0x030
#define GPIO_IRQENABLE_0   0x034
#define GPIO_IRQENABLE_1   0x038

/* --- Driver data --- */
struct gpio_am335x_data {
	struct gpio_driver_data common;
	sys_slist_t callbacks;
};

/* --- Driver config --- */
struct gpio_am335x_config {
	uintptr_t base;
	void (*irq_config_func)(void);
};

/* --- Helpers --- */
static inline uint32_t reg_read(uintptr_t base, uint32_t offset)
{
	return sys_read32(base + offset);
}

static inline void reg_write(uintptr_t base, uint32_t offset, uint32_t val)
{
	sys_write32(val, base + offset);
}

/* --- GPIO API implementations --- */

static int gpio_am335x_configure(const struct device *dev,
				 gpio_pin_t pin,
				 gpio_flags_t flags)
{
	const struct gpio_am335x_config *cfg = dev->config;
	uint32_t oe;

	if (pin >= 32) {
		return -EINVAL;
	}

	oe = reg_read(cfg->base, GPIO_OE);

	if (flags & GPIO_OUTPUT) {
		oe &= ~BIT(pin);
	} else {
		oe |= BIT(pin);
	}

	reg_write(cfg->base, GPIO_OE, oe);
	return 0;
}

static int gpio_am335x_port_get_raw(const struct device *dev,
				    gpio_port_value_t *value)
{
	const struct gpio_am335x_config *cfg = dev->config;

	*value = reg_read(cfg->base, GPIO_DATAIN);
	return 0;
}

static int gpio_am335x_port_set_masked_raw(const struct device *dev,
					   gpio_port_pins_t mask,
					   gpio_port_value_t value)
{
	const struct gpio_am335x_config *cfg = dev->config;

	reg_write(cfg->base, GPIO_SETDATAOUT, mask & value);
	reg_write(cfg->base, GPIO_CLEARDATAOUT, mask & ~value);
	return 0;
}

static int gpio_am335x_port_set_bits_raw(const struct device *dev,
					gpio_port_pins_t pins)
{
	const struct gpio_am335x_config *cfg = dev->config;

	reg_write(cfg->base, GPIO_SETDATAOUT, pins);
	return 0;
}

static int gpio_am335x_port_clear_bits_raw(const struct device *dev,
					  gpio_port_pins_t pins)
{
	const struct gpio_am335x_config *cfg = dev->config;

	reg_write(cfg->base, GPIO_CLEARDATAOUT, pins);
	return 0;
}

static int gpio_am335x_port_toggle_bits(const struct device *dev,
				       gpio_port_pins_t pins)
{
	gpio_port_value_t val;

	gpio_am335x_port_get_raw(dev, &val);
	return gpio_am335x_port_set_masked_raw(dev, pins, ~val);
}

static int gpio_am335x_manage_callback(const struct device *dev,
				       struct gpio_callback *callback,
				       bool set)
{
	struct gpio_am335x_data *data = dev->data;

	return gpio_manage_callback(&data->callbacks, callback, set);
}

/* --- ISR --- */
static void gpio_am335x_isr(const struct device *dev)
{
	const struct gpio_am335x_config *cfg = dev->config;
	struct gpio_am335x_data *data = dev->data;
	uint32_t status;

	status = reg_read(cfg->base, GPIO_IRQSTATUS_0);
	reg_write(cfg->base, GPIO_IRQSTATUS_0, status);

	if (status) {
		gpio_fire_callbacks(&data->callbacks, dev, status);
	}
}

/* --- Driver API --- */
static const struct gpio_driver_api gpio_am335x_driver_api = {
	.pin_configure = gpio_am335x_configure,
	.port_get_raw = gpio_am335x_port_get_raw,
	.port_set_masked_raw = gpio_am335x_port_set_masked_raw,
	.port_set_bits_raw = gpio_am335x_port_set_bits_raw,
	.port_clear_bits_raw = gpio_am335x_port_clear_bits_raw,
	.port_toggle_bits = gpio_am335x_port_toggle_bits,
	.manage_callback = gpio_am335x_manage_callback,
};

/* --- Init --- */
static int gpio_am335x_init(const struct device *dev)
{
	const struct gpio_am335x_config *cfg = dev->config;
	struct gpio_am335x_data *data = dev->data;

	sys_slist_init(&data->callbacks);

	if (cfg->irq_config_func) {
		cfg->irq_config_func();
	}

	return 0;
}

/* --- DT instantiation --- */
#define GPIO_AM335X_INIT(n)                                              \
	static void gpio_am335x_irq_config_##n(void)                      \
	{                                                                  \
		IRQ_CONNECT(DT_INST_IRQN(n),                               \
			    DT_INST_IRQ(n, priority),                     \
			    gpio_am335x_isr,                              \
			    DEVICE_DT_INST_GET(n),                        \
			    0);                                           \
		irq_enable(DT_INST_IRQN(n));                              \
	}                                                                  \
                                                                           \
	static const struct gpio_am335x_config gpio_am335x_cfg_##n = {     \
		.base = DT_INST_REG_ADDR(n),                               \
		.irq_config_func = gpio_am335x_irq_config_##n,             \
	};                                                                  \
                                                                           \
	static struct gpio_am335x_data gpio_am335x_data_##n;               \
                                                                           \
	DEVICE_DT_INST_DEFINE(n,                                           \
			      gpio_am335x_init,                            \
			      NULL,                                      \
			      &gpio_am335x_data_##n,                     \
			      &gpio_am335x_cfg_##n,                      \
			      POST_KERNEL,                               \
			      CONFIG_GPIO_INIT_PRIORITY,                \
			      &gpio_am335x_driver_api);

DT_INST_FOREACH_STATUS_OKAY(GPIO_AM335X_INIT)
