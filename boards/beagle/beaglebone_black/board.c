/* SPDX-License-Identifier: Apache-2.0 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/device.h>

static int beaglebone_black_init(void)
{
	/* Board-specific initialization */
	return 0;
}

SYS_INIT(beaglebone_black_init, PRE_KERNEL_1, CONFIG_BOARD_INIT_PRIORITY);
