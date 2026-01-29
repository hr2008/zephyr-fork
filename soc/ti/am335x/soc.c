/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 Rajeev H
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/arch/cpu.h>
#include "soc.h"

#define INTC_SYSCONFIG  (AM335X_INTC_BASE + 0x10)
#define INTC_SYSSTATUS  (AM335X_INTC_BASE + 0x14)
#define INTC_CONTROL    (AM335X_INTC_BASE + 0x48)
#define INTC_PROTECTION (AM335X_INTC_BASE + 0x4C)
#define INTC_IDLE       (AM335X_INTC_BASE + 0x50)

#define INTC_SYSCONFIG_SOFTRESET BIT(1)
#define INTC_SYSSTATUS_RESETDONE BIT(0)

static void intc_init(void)
{
	volatile uint32_t *sysconfig = (volatile uint32_t *)INTC_SYSCONFIG;
	volatile uint32_t *sysstatus = (volatile uint32_t *)INTC_SYSSTATUS;
	volatile uint32_t *control = (volatile uint32_t *)INTC_CONTROL;

	/* Soft reset INTC */
	*sysconfig = INTC_SYSCONFIG_SOFTRESET;

	/* Wait for reset to complete */
	while ((*sysstatus & INTC_SYSSTATUS_RESETDONE) == 0) {
		/* Wait */
	}

	/* Enable new interrupt agreement */
	*control = 0x1;
}

static int am335x_init(void)
{
	/* Initialize interrupt controller */
	intc_init();

	return 0;
}

SYS_INIT(am335x_init, PRE_KERNEL_1, 0);
