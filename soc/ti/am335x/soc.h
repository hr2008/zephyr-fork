/* SPDX-License-Identifier: Apache-2.0 */

#ifndef _SOC_TI_AM335X_H_
#define _SOC_TI_AM335X_H_

#include <zephyr/sys/util.h>

#ifndef _ASMLANGUAGE

#include <zephyr/device.h>

/* Peripheral Base Addresses */
#define AM335X_L4_WKUP_BASE         0x44C00000
#define AM335X_L4_PER_BASE          0x48000000
#define AM335X_EMIF0_BASE           0x4C000000

/* Control Module */
#define AM335X_CM_BASE              0x44E10000
#define AM335X_CM_WKUP_BASE         (AM335X_CM_BASE + 0x0400)
#define AM335X_CM_PER_BASE          (AM335X_CM_BASE + 0x0000)

/* GPIO */
#define AM335X_GPIO0_BASE           0x44E07000
#define AM335X_GPIO1_BASE           0x4804C000
#define AM335X_GPIO2_BASE           0x481AC000
#define AM335X_GPIO3_BASE           0x481AE000

/* UART */
#define AM335X_UART0_BASE           0x44E09000
#define AM335X_UART1_BASE           0x48022000
#define AM335X_UART2_BASE           0x48024000
#define AM335X_UART3_BASE           0x481A6000
#define AM335X_UART4_BASE           0x481A8000
#define AM335X_UART5_BASE           0x481AA000

/* Timer */
#define AM335X_DMTIMER0_BASE        0x44E05000
#define AM335X_DMTIMER1_BASE        0x44E31000
#define AM335X_DMTIMER2_BASE        0x48040000
#define AM335X_DMTIMER3_BASE        0x48042000
#define AM335X_DMTIMER4_BASE        0x48044000
#define AM335X_DMTIMER5_BASE        0x48046000
#define AM335X_DMTIMER6_BASE        0x48048000
#define AM335X_DMTIMER7_BASE        0x4804A000

/* Interrupt Controller (INTC) */
#define AM335X_INTC_BASE            0x48200000

/* Clock Module Peripheral Registers */
#define CM_PER_GPIO1_CLKCTRL        (AM335X_CM_PER_BASE + 0xAC)
#define CM_PER_GPIO2_CLKCTRL        (AM335X_CM_PER_BASE + 0xB0)
#define CM_PER_GPIO3_CLKCTRL        (AM335X_CM_PER_BASE + 0xB4)
#define CM_PER_UART1_CLKCTRL        (AM335X_CM_PER_BASE + 0x6C)
#define CM_PER_UART2_CLKCTRL        (AM335X_CM_PER_BASE + 0x70)
#define CM_PER_UART3_CLKCTRL        (AM335X_CM_PER_BASE + 0x74)
#define CM_PER_UART4_CLKCTRL        (AM335X_CM_PER_BASE + 0x78)
#define CM_PER_UART5_CLKCTRL        (AM335X_CM_PER_BASE + 0x38)
#define CM_PER_TIMER2_CLKCTRL       (AM335X_CM_PER_BASE + 0x80)
#define CM_PER_TIMER3_CLKCTRL       (AM335X_CM_PER_BASE + 0x84)
#define CM_PER_TIMER4_CLKCTRL       (AM335X_CM_PER_BASE + 0x88)
#define CM_PER_TIMER5_CLKCTRL       (AM335X_CM_PER_BASE + 0xEC)
#define CM_PER_TIMER6_CLKCTRL       (AM335X_CM_PER_BASE + 0xF0)
#define CM_PER_TIMER7_CLKCTRL       (AM335X_CM_PER_BASE + 0x7C)

#define CM_WKUP_GPIO0_CLKCTRL       (AM335X_CM_WKUP_BASE + 0x08)
#define CM_WKUP_UART0_CLKCTRL       (AM335X_CM_WKUP_BASE + 0xB4)
#define CM_WKUP_TIMER0_CLKCTRL      (AM335X_CM_WKUP_BASE + 0x10)
#define CM_WKUP_TIMER1_CLKCTRL      (AM335X_CM_WKUP_BASE + 0xC4)

/* Module Enable */
#define CM_CLKCTRL_MODULEMODE_ENABLE    (0x2)
#define CM_CLKCTRL_IDLEST_FUNC          (0x0 << 16)

/* IRQ numbers */
#define AM335X_IRQ_UART0            72
#define AM335X_IRQ_UART1            73
#define AM335X_IRQ_UART2            74
#define AM335X_IRQ_UART3            44
#define AM335X_IRQ_UART4            45
#define AM335X_IRQ_UART5            46

#define AM335X_IRQ_GPIO0A           96
#define AM335X_IRQ_GPIO0B           97
#define AM335X_IRQ_GPIO1A           98
#define AM335X_IRQ_GPIO1B           99
#define AM335X_IRQ_GPIO2A           32
#define AM335X_IRQ_GPIO2B           33
#define AM335X_IRQ_GPIO3A           62
#define AM335X_IRQ_GPIO3B           63

#define AM335X_IRQ_TIMER0           66
#define AM335X_IRQ_TIMER1           67
#define AM335X_IRQ_TIMER2           68
#define AM335X_IRQ_TIMER3           69
#define AM335X_IRQ_TIMER4           92
#define AM335X_IRQ_TIMER5           93
#define AM335X_IRQ_TIMER6           94
#define AM335X_IRQ_TIMER7           95

#endif /* !_ASMLANGUAGE */

#endif /* _SOC_TI_AM335X_H_ */
