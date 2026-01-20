# Copyright (c) 2025 Rajeev H
# SPDX-License-Identifier: Apache-2.0

zephyr_library()

# SoC initialization
zephyr_library_sources(soc.c)

# Interrupt controller support
#zephyr_library_sources(irq.c)

# MMU support (if enabled)
zephyr_library_sources_ifdef(CONFIG_ARM_AARCH32_MMU mmu.c)

# Include directories
zephyr_include_directories(.)

# Set linker script
set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld CACHE INTERNAL "")
