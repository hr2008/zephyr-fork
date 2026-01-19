/*
 * Copyright (c) 2025 Rajeev H
 * SPDX-License-Identifier: Apache-2.0
 *
 * AM335x Interrupt Controller (INTC) Support
 */

#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/irq.h>
#include <zephyr/sw_isr_table.h>

/* INTC Register Offsets */
#define INTC_BASE           0x48200000

#define INTC_REVISION       0x00
#define INTC_SYSCONFIG      0x10
#define INTC_SYSSTATUS      0x14
#define INTC_SIR_IRQ        0x40
#define INTC_SIR_FIQ        0x44
#define INTC_CONTROL        0x48
#define INTC_PROTECTION     0x4C
#define INTC_IDLE           0x50
#define INTC_IRQ_PRIORITY   0x60
#define INTC_FIQ_PRIORITY   0x64
#define INTC_THRESHOLD      0x68

/* Interrupt Mask/Enable registers (128 interrupts / 32 per register) */
#define INTC_ITR(n)         (0x80 + ((n) * 0x20))   /* IRQ status */
#define INTC_MIR(n)         (0x84 + ((n) * 0x20))   /* Mask */
#define INTC_MIR_CLEAR(n)   (0x88 + ((n) * 0x20))   /* Clear mask (enable) */
#define INTC_MIR_SET(n)     (0x8C + ((n) * 0x20))   /* Set mask (disable) */
#define INTC_ISR_SET(n)     (0x90 + ((n) * 0x20))   /* Set interrupt */
#define INTC_ISR_CLEAR(n)   (0x94 + ((n) * 0x20))   /* Clear interrupt */
#define INTC_PENDING_IRQ(n) (0x98 + ((n) * 0x20))   /* Pending IRQ */
#define INTC_PENDING_FIQ(n) (0x9C + ((n) * 0x20))   /* Pending FIQ */

/* Interrupt Line registers (128 interrupts, 4 bits per interrupt) */
#define INTC_ILR(n)         (0x100 + ((n) * 0x04))

/* ILR register bits */
#define ILR_PRIORITY_SHIFT  2
#define ILR_PRIORITY_MASK   (0x3F << ILR_PRIORITY_SHIFT)
#define ILR_FIQ_nIRQ        BIT(0)

void z_soc_irq_init(void)
{
	uint32_t i;

	/* Perform software reset */
	sys_write32(0x02, INTC_BASE + INTC_SYSCONFIG);

	/* Wait for reset to complete */
	while (!(sys_read32(INTC_BASE + INTC_SYSSTATUS) & 0x01)) {
		/* Wait */
	}

	/* Mask all interrupts */
	for (i = 0; i < 4; i++) {
		sys_write32(0xFFFFFFFF, INTC_BASE + INTC_MIR_SET(i));
	}

	/* Clear all pending interrupts */
	for (i = 0; i < 4; i++) {
		sys_write32(0xFFFFFFFF, INTC_BASE + INTC_ISR_CLEAR(i));
	}

	/* Configure all interrupts as IRQ (not FIQ) with default priority */
	for (i = 0; i < 128; i++) {
		sys_write32(0, INTC_BASE + INTC_ILR(i));
	}

	/* Enable new IRQ/FIQ agreement */
	sys_write32(0x01, INTC_BASE + INTC_CONTROL);

	/* Set protection to allow user mode access (optional) */
	sys_write32(0x01, INTC_BASE + INTC_PROTECTION);
}

void z_soc_irq_enable(unsigned int irq)
{
	uint32_t reg_num = irq / 32;
	uint32_t bit_num = irq % 32;

	if (irq >= 128) {
		return;
	}

	/* Clear mask bit to enable interrupt */
	sys_write32(BIT(bit_num), INTC_BASE + INTC_MIR_CLEAR(reg_num));
}

void z_soc_irq_disable(unsigned int irq)
{
	uint32_t reg_num = irq / 32;
	uint32_t bit_num = irq % 32;

	if (irq >= 128) {
		return;
	}

	/* Set mask bit to disable interrupt */
	sys_write32(BIT(bit_num), INTC_BASE + INTC_MIR_SET(reg_num));
}

int z_soc_irq_is_enabled(unsigned int irq)
{
	uint32_t reg_num = irq / 32;
	uint32_t bit_num = irq % 32;
	uint32_t mir;

	if (irq >= 128) {
		return 0;
	}

	/* Read mask register (0 = enabled, 1 = disabled) */
	mir = sys_read32(INTC_BASE + INTC_MIR(reg_num));

	return !(mir & BIT(bit_num));
}

void z_soc_irq_priority_set(unsigned int irq, unsigned int prio, uint32_t flags)
{
	uint32_t ilr;

	ARG_UNUSED(flags);

	if (irq >= 128) {
		return;
	}

	/* Limit priority to 6 bits (0-63) */
	prio &= 0x3F;

	/* Read current ILR value */
	ilr = sys_read32(INTC_BASE + INTC_ILR(irq));

	/* Clear priority bits and set new priority */
	ilr &= ~ILR_PRIORITY_MASK;
	ilr |= (prio << ILR_PRIORITY_SHIFT);

	/* Write back */
	sys_write32(ilr, INTC_BASE + INTC_ILR(irq));
}

void z_soc_irq_eoi(unsigned int irq)
{
	ARG_UNUSED(irq);

	/* Acknowledge interrupt - write new IRQ agreement */
	sys_write32(0x01, INTC_BASE + INTC_CONTROL);
}

/* Get active interrupt number */
unsigned int z_soc_irq_get_active(void)
{
	uint32_t sir_irq;

	/* Read active IRQ number */
	sir_irq = sys_read32(INTC_BASE + INTC_SIR_IRQ);

	/* Extract interrupt number (bits 6:0) */
	return (sir_irq & 0x7F);
}

/* Set interrupt as spurious (not used on AM335x) */
void z_soc_irq_set_spurious(unsigned int irq)
{
	ARG_UNUSED(irq);
	/* Not applicable for AM335x INTC */
}
