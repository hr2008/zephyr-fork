# SPDX-License-Identifier: Apache-2.0

zephyr_sources(
  soc.c
  vector_table.S
)

#set(SOC_LINKER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld CACHE INTERNAL "")

zephyr_include_directories(.)
