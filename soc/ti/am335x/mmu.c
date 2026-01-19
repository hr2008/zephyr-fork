/*
 * Copyright (c) 2025 Rajeev H
 * SPDX-License-Identifier: Apache-2.0
 *
 * AM335x MMU Configuration
 */

#include <zephyr/arch/arm/mmu/arm_mmu.h>
#include <zephyr/sys/util.h>

static const struct arm_mmu_region mmu_regions[] = {

	/* DDR RAM – 512 MB, cacheable, executable */
	MMU_REGION_FLAT_ENTRY(
		"ddr_ram",
		0x80000000,
		0x20000000, /* 512 MB */
		MT_NORMAL | MPERM_R | MPERM_W | MPERM_X
	),

	/* L3 interconnect / device region */
	MMU_REGION_FLAT_ENTRY(
		"peripherals",
		0x44000000,
		0x01000000, /* 16 MB */
		MT_DEVICE | MPERM_R | MPERM_W
	),

	/* L4 peripherals */
	MMU_REGION_FLAT_ENTRY(
		"l4_peripherals",
		0x48000000,
		0x02000000, /* 32 MB */
		MT_DEVICE | MPERM_R | MPERM_W
	),

	/* EMIF (DDR controller) */
	MMU_REGION_FLAT_ENTRY(
		"emif",
		0x4C000000,
		0x01000000, /* 16 MB */
		MT_DEVICE | MPERM_R | MPERM_W
	),

	/* GPMC */
	MMU_REGION_FLAT_ENTRY(
		"gpmc",
		0x50000000,
		0x01000000, /* 16 MB */
		MT_DEVICE | MPERM_R | MPERM_W
	),
};

const struct arm_mmu_config mmu_config = {
	.num_regions = ARRAY_SIZE(mmu_regions),
	.mmu_regions = mmu_regions,
};
