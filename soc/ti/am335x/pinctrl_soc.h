/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 Rajeev H
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_SOC_TI_AM335X_PINCTRL_SOC_H_
#define ZEPHYR_SOC_TI_AM335X_PINCTRL_SOC_H_

#include <stdint.h>
#include <zephyr/devicetree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Type for AM335x pin configuration.
 *
 * Each pin configuration is a 32-bit value containing:
 * - bits [15:0]:  pin offset from control module base
 * - bits [18:16]: mux mode (0-7)
 * - bit  19:      pull disable (1 = disable)
 * - bit  20:      pull type (0 = pulldown, 1 = pullup)
 * - bit  21:      input enable (1 = enabled)
 * - bit  22:      slew control (0 = fast, 1 = slow)
 */
typedef uint32_t pinctrl_soc_pin_t;

/**
 * @brief Utility macro to initialize each pin.
 *
 * @param node_id Node identifier.
 * @param prop Property name.
 * @param idx Property entry index.
 */
#define Z_PINCTRL_STATE_PIN_INIT(node_id, prop, idx) (DT_PROP_BY_IDX(node_id, prop, idx)),

/**
 * @brief Utility macro to initialize state pins contained in a given property.
 *
 * @param node_id Node identifier.
 * @param prop Property name describing state pins.
 */
#define Z_PINCTRL_STATE_PINS_INIT(node_id, prop)                                                   \
	{DT_FOREACH_PROP_ELEM(node_id, prop, Z_PINCTRL_STATE_PIN_INIT)}

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_SOC_TI_AM335X_PINCTRL_SOC_H_ */
