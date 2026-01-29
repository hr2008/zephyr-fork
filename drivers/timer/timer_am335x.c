/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 Rajeev H
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT ti_am335x_timer

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/timer/system_timer.h>
#include <zephyr/sys_clock.h>
#include <zephyr/spinlock.h>
#include <zephyr/irq.h>

/* Timer Register Offsets */
#define TIMER_TIDR      0x00
#define TIMER_TIOCP_CFG 0x10
#define TIMER_TISTAT    0x14
#define TIMER_TISR      0x18
#define TIMER_TIER      0x1C
#define TIMER_TWER      0x20
#define TIMER_TCLR      0x24
#define TIMER_TCRR      0x28
#define TIMER_TLDR      0x2C
#define TIMER_TTGR      0x30
#define TIMER_TWPS      0x34
#define TIMER_TMAR      0x38
#define TIMER_TCAR1     0x3C
#define TIMER_TSICR     0x40
#define TIMER_TCAR2     0x44

/* TCLR Register Bits */
#define TIMER_TCLR_ST  BIT(0) /* Start/Stop timer */
#define TIMER_TCLR_AR  BIT(1) /* Auto-reload */
#define TIMER_TCLR_CE  BIT(6) /* Compare enable */
#define TIMER_TCLR_PRE BIT(5) /* Prescaler enable */

/* TIER Register Bits */
#define TIMER_TIER_MAT_EN BIT(0) /* Match interrupt enable */
#define TIMER_TIER_OVF_EN BIT(1) /* Overflow interrupt enable */

/* TISR Register Bits */
#define TIMER_TISR_MAT BIT(0) /* Match interrupt status */
#define TIMER_TISR_OVF BIT(1) /* Overflow interrupt status */

#define TIMER_CLOCK_FREQ 24000000 /* 24 MHz */

#define CYC_PER_TICK (TIMER_CLOCK_FREQ / CONFIG_SYS_CLOCK_TICKS_PER_SEC)
#define MAX_CYCLES   0xFFFFFFFF
#define MAX_TICKS    ((MAX_CYCLES - CYC_PER_TICK) / CYC_PER_TICK)
#define MIN_DELAY    1

static struct k_spinlock lock;
static uint64_t last_count;

#if defined(CONFIG_TEST)
const int32_t z_sys_timer_irq_for_test = DT_IRQN(DT_NODELABEL(timer2));
#endif

static inline uint32_t timer_read(uint32_t reg)
{
	return sys_read32(DT_REG_ADDR(DT_NODELABEL(timer2)) + reg);
}

static inline void timer_write(uint32_t reg, uint32_t val)
{
	sys_write32(val, DT_REG_ADDR(DT_NODELABEL(timer2)) + reg);
}

static void timer_am335x_enable_clock(void)
{
	volatile uint32_t *clk_reg = (volatile uint32_t *)CM_PER_TIMER2_CLKCTRL;

	/* Enable module clock */
	*clk_reg = 0x2;

	/* Wait for module to be enabled */
	while ((*clk_reg & (0x3 << 16)) != 0) {
		/* Wait */
	}
}

static void timer_am335x_isr(const void *arg)
{
	ARG_UNUSED(arg);
	k_spinlock_key_t key = k_spin_lock(&lock);
	uint32_t status = timer_read(TIMER_TISR);

	/* Clear interrupt status */
	timer_write(TIMER_TISR, status);

	uint32_t count = timer_read(TIMER_TCRR);
	uint32_t ticks = (count - (uint32_t)last_count) / CYC_PER_TICK;

	last_count += ticks * CYC_PER_TICK;

	k_spin_unlock(&lock, key);

	sys_clock_announce(ticks);
}

void sys_clock_set_timeout(int32_t ticks, bool idle)
{
	ARG_UNUSED(idle);

#if defined(CONFIG_TICKLESS_KERNEL)
	k_spinlock_key_t key = k_spin_lock(&lock);
	uint32_t count = timer_read(TIMER_TCRR);
	uint32_t delta;

	if (ticks == K_TICKS_FOREVER) {
		delta = MAX_CYCLES;
	} else {
		ticks = (ticks == K_TICKS_FOREVER) ? MAX_TICKS : ticks;
		ticks = CLAMP(ticks - 1, 0, (int32_t)MAX_TICKS);
		delta = ticks * CYC_PER_TICK;
	}

	/* Set compare value */
	timer_write(TIMER_TMAR, count + delta);

	k_spin_unlock(&lock, key);
#endif
}

uint32_t sys_clock_elapsed(void)
{
	if (!IS_ENABLED(CONFIG_TICKLESS_KERNEL)) {
		return 0;
	}

	k_spinlock_key_t key = k_spin_lock(&lock);
	uint32_t count = timer_read(TIMER_TCRR);
	uint32_t elapsed = (count - (uint32_t)last_count) / CYC_PER_TICK;

	k_spin_unlock(&lock, key);
	return elapsed;
}

uint32_t sys_clock_cycle_get_32(void)
{
	return timer_read(TIMER_TCRR);
}

static int sys_clock_driver_init(void)
{
	/* Enable clock */
	timer_am335x_enable_clock();

	/* Stop timer */
	timer_write(TIMER_TCLR, 0);

	/* Reset counter */
	timer_write(TIMER_TCRR, 0);
	timer_write(TIMER_TLDR, 0);

	/* Set compare value for first tick */
	timer_write(TIMER_TMAR, CYC_PER_TICK);

	/* Enable match and overflow interrupts */
	timer_write(TIMER_TIER, TIMER_TIER_MAT_EN | TIMER_TIER_OVF_EN);

	/* Clear any pending interrupts */
	timer_write(TIMER_TISR, TIMER_TISR_MAT | TIMER_TISR_OVF);

	/* Configure and connect interrupt */
	IRQ_CONNECT(DT_IRQN(DT_NODELABEL(timer2)), DT_IRQ(DT_NODELABEL(timer2), priority),
		    timer_am335x_isr, NULL, 0);

	irq_enable(DT_IRQN(DT_NODELABEL(timer2)));

	/* Start timer: auto-reload, compare enabled */
	timer_write(TIMER_TCLR, TIMER_TCLR_ST | TIMER_TCLR_AR | TIMER_TCLR_CE);

	return 0;
}

SYS_INIT(sys_clock_driver_init, PRE_KERNEL_2, CONFIG_SYSTEM_CLOCK_INIT_PRIORITY);
