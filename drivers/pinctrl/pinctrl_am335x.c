/* SPDX-License-Identifier: Apache-2.0 */

#define DT_DRV_COMPAT ti_am335x_pinctrl

#include <zephyr/drivers/pinctrl.h>
#include <zephyr/dt-bindings/pinctrl/am335x-pinctrl.h>

/* Control Module base address */
#define AM335X_CTRL_BASE        0x44E10000
#define AM335X_CTRL_CONF_BASE   (AM335X_CTRL_BASE + 0x800)

/* Pin configuration register bits */
#define CONF_MMODE_MASK         0x07    /* Mux mode */
#define CONF_PUDEN              BIT(3)  /* Pull disable */
#define CONF_PUTYPESEL          BIT(4)  /* Pull type select */
#define CONF_RXACTIVE           BIT(5)  /* Input enable */
#define CONF_SLEWCTRL           BIT(6)  /* Slew rate control */

struct pinctrl_am335x_config {
	uint32_t base;
};

/**
 * @brief Configure a single pin
 *
 * @param pin_config Pin configuration value containing:
 *                   - bits [15:0]: pin offset
 *                   - bits [18:16]: mux mode
 *                   - bit 19: pull disable
 *                   - bit 20: pull type (0=pulldown, 1=pullup)
 *                   - bit 21: input enable
 *                   - bit 22: slew control
 */
static void pinctrl_configure_pin(uint32_t pin_config)
{
	uint32_t offset = pin_config & 0xFFFF;
	uint32_t mode = (pin_config >> 16) & 0x7;
	uint32_t pull_disable = (pin_config >> 19) & 0x1;
	uint32_t pull_type = (pin_config >> 20) & 0x1;
	uint32_t input_en = (pin_config >> 21) & 0x1;
	uint32_t slew = (pin_config >> 22) & 0x1;
	
	uint32_t conf_val = 0;
	volatile uint32_t *conf_reg;

	/* Build configuration value */
	conf_val |= (mode & CONF_MMODE_MASK);
	
	if (pull_disable) {
		conf_val |= CONF_PUDEN;
	}
	
	if (pull_type) {
		conf_val |= CONF_PUTYPESEL;  /* Pull-up */
	}
	
	if (input_en) {
		conf_val |= CONF_RXACTIVE;
	}
	
	if (slew) {
		conf_val |= CONF_SLEWCTRL;
	}

	/* Write to configuration register */
	conf_reg = (volatile uint32_t *)(AM335X_CTRL_CONF_BASE + offset);
	*conf_reg = conf_val;
}

int pinctrl_configure_pins(const pinctrl_soc_pin_t *pins, uint8_t pin_cnt,
			    uintptr_t reg)
{
	ARG_UNUSED(reg);

	for (uint8_t i = 0; i < pin_cnt; i++) {
		pinctrl_configure_pin(pins[i]);
	}

	return 0;
}

static int pinctrl_am335x_init(void)
{
	/* No initialization needed - control module is always accessible */
	return 0;
}

SYS_INIT(pinctrl_am335x_init, PRE_KERNEL_1, CONFIG_PINCTRL_INIT_PRIORITY);
