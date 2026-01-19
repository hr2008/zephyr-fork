/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/irq.h>
#include <zephyr/sw_isr_table.h>

#define INTC_BASE           0x48200000

#define INTC_SYSCONFIG      (INTC_BASE + 0x010)
#define INTC_SYSSTATUS      (INTC_BASE + 0x014)
#define INTC_SIR_IRQ        (INTC_BASE + 0x040)
#define INTC_SIR_FIQ        (INTC_BASE + 0x044)
#define INTC_CONTROL        (INTC_BASE + 0x048)
#define INTC_PROTECTION     (INTC_BASE + 0x04C)
#define INTC_IDLE           (INTC_BASE + 0x050)
#define INTC_IRQ_PRIORITY   (INTC_BASE + 0x060)
#define INTC_FIQ_PRIORITY   (INTC_BASE + 0x064)
#define INTC_THRESHOLD      (INTC_BASE + 0x068)

#define INTC_MIR_BASE       (INTC_BASE + 0x084)
#define INTC_MIR_CLEAR_BASE (INTC_BASE + 0x088)
#define INTC_MIR_SET_BASE   (INTC_BASE + 0x08C)
#define INTC_ISR_SET_BASE   (INTC_BASE + 0x090)
#define INTC_ISR_CLEAR_BASE (INTC_BASE + 0x094)

#define INTC_ILR_BASE       (INTC_BASE + 0x100)

#define INTC_ACTIVEIRQ_MASK 0x7F
#define INTC_SPURIOUSIRQ    0x000000FF
#define INTC_TO_IRQ			0x00

void arch_irq_enable(unsigned int irq)
{
	uint32_t bank = irq / 32;
	uint32_t bit = irq % 32;
	volatile uint32_t *mir_clear = (volatile uint32_t *)(INTC_MIR_CLEAR_BASE + (bank * 0x20));
	
	*mir_clear = BIT(bit);
}

void arch_irq_disable(unsigned int irq)
{
	uint32_t bank = irq / 32;
	uint32_t bit = irq % 32;
	volatile uint32_t *mir_set = (volatile uint32_t *)(INTC_MIR_SET_BASE + (bank * 0x20));
	
	*mir_set = BIT(bit);
}

int arch_irq_is_enabled(unsigned int irq)
{
	uint32_t bank = irq / 32;
	uint32_t bit = irq % 32;
	volatile uint32_t *mir = (volatile uint32_t *)(INTC_MIR_BASE + (bank * 0x20));
	
	return (*mir & BIT(bit)) == 0;
}

void z_arm_irq_priority_set(unsigned int irq, unsigned int prio, uint32_t flags)
{
	volatile uint32_t *ilr = (volatile uint32_t *)(INTC_ILR_BASE + (irq * 4));
	
	/* Set priority and FIQ/IRQ routing */
	*ilr = (prio & 0x3F) | ((flags & INTC_TO_IRQ) ? BIT(1) : 0);
}

void z_soc_irq_init(void)
{
	volatile uint32_t *sysconfig = (volatile uint32_t *)INTC_SYSCONFIG;
	volatile uint32_t *sysstatus = (volatile uint32_t *)INTC_SYSSTATUS;
	volatile uint32_t *control = (volatile uint32_t *)INTC_CONTROL;
	int i;

	/* Soft reset INTC */
	*sysconfig = BIT(1);
	
	/* Wait for reset to complete */
	while ((*sysstatus & BIT(0)) == 0) {
		/* Wait */
	}

	/* Enable new IRQ agreement protocol */
	*control = 0x1;

	/* Mask all interrupts initially */
	for (i = 0; i < 4; i++) {
		volatile uint32_t *mir_set = (volatile uint32_t *)(INTC_MIR_SET_BASE + (i * 0x20));
		*mir_set = 0xFFFFFFFF;
	}
}
/*
void z_arm_interrupt_init(void)
{
	z_arm_irq_init();
}
*/
void arch_irq_handle(void)
{
	volatile uint32_t *sir_irq = (volatile uint32_t *)INTC_SIR_IRQ;
	volatile uint32_t *control = (volatile uint32_t *)INTC_CONTROL;
	uint32_t sir;
	uint32_t irq;

	sir = *sir_irq;
	irq = sir & INTC_ACTIVEIRQ_MASK;

	if (irq != INTC_SPURIOUSIRQ) {
		struct _isr_table_entry *entry = &_sw_isr_table[irq];
		
		entry->isr(entry->arg);

		/* Signal end of interrupt */
		*control = 0x1;
	}
}
