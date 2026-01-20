/* SPDX-License-Identifier: Apache-2.0 */
/* File #26: include/zephyr/dt-bindings/pinctrl/am335x-pinctrl.h */

#ifndef ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_AM335X_H_
#define ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_AM335X_H_

/**
 * @brief AM335x pin configuration
 *
 * Pin configuration is encoded as a 32-bit value:
 * - bits [15:0]:  pin offset from control module base (0x44E10800)
 * - bits [18:16]: mux mode (0-7)
 * - bit  19:      pull disable (1 = disable pull resistor)
 * - bit  20:      pull type (0 = pulldown, 1 = pullup)
 * - bit  21:      input enable (1 = input enabled)
 * - bit  22:      slew control (0 = fast, 1 = slow)
 */

/* Mux mode selection */
#define AM335X_PIN_MODE_SHIFT       16
#define AM335X_PIN_MODE_MASK        0x7
#define AM335X_PIN_MODE(x)          (((x) & AM335X_PIN_MODE_MASK) << AM335X_PIN_MODE_SHIFT)

#define AM335X_PIN_MODE_0           AM335X_PIN_MODE(0)
#define AM335X_PIN_MODE_1           AM335X_PIN_MODE(1)
#define AM335X_PIN_MODE_2           AM335X_PIN_MODE(2)
#define AM335X_PIN_MODE_3           AM335X_PIN_MODE(3)
#define AM335X_PIN_MODE_4           AM335X_PIN_MODE(4)
#define AM335X_PIN_MODE_5           AM335X_PIN_MODE(5)
#define AM335X_PIN_MODE_6           AM335X_PIN_MODE(6)
#define AM335X_PIN_MODE_7           AM335X_PIN_MODE(7)

/* Pull resistor configuration */
#define AM335X_PULL_DISABLE         (1 << 19)
#define AM335X_PULL_UP              (1 << 20)
#define AM335X_PULL_DOWN            (0 << 20)

/* Input enable */
#define AM335X_INPUT_EN             (1 << 21)

/* Slew rate control */
#define AM335X_SLEW_SLOW            (1 << 22)
#define AM335X_SLEW_FAST            (0 << 22)

/* Common pin configurations */
#define AM335X_PIN_OUTPUT           (AM335X_PULL_DISABLE)
#define AM335X_PIN_INPUT            (AM335X_INPUT_EN | AM335X_PULL_DISABLE)
#define AM335X_PIN_INPUT_PULLUP     (AM335X_INPUT_EN | AM335X_PULL_UP)
#define AM335X_PIN_INPUT_PULLDOWN   (AM335X_INPUT_EN | AM335X_PULL_DOWN)

/**
 * @brief Create a pin configuration value
 *
 * @param offset Register offset from control module conf base (0x44E10800)
 * @param mode Mux mode (0-7)
 * @param flags Additional flags (pull, input enable, slew)
 */
#define AM335X_PINMUX(offset, mode, flags) \
	((offset) | AM335X_PIN_MODE(mode) | (flags))

/*
 * Pin offsets for commonly used pins on AM335x
 * These offsets are relative to the Control Module conf base at 0x44E10800
 * Complete pinmux tables are in the AM335x Technical Reference Manual
 */

/* UART0 pins */
#define AM335X_PIN_UART0_RXD        0x170   /* uart0_rxd, mode 0 */
#define AM335X_PIN_UART0_TXD        0x174   /* uart0_txd, mode 0 */

/* UART1 pins */
#define AM335X_PIN_UART1_RXD        0x180   /* uart1_rxd, mode 0 */
#define AM335X_PIN_UART1_TXD        0x184   /* uart1_txd, mode 0 */
#define AM335X_PIN_UART1_CTSN       0x178   /* uart1_ctsn, mode 0 */
#define AM335X_PIN_UART1_RTSN       0x17C   /* uart1_rtsn, mode 0 */

/* UART2 pins */
#define AM335X_PIN_UART2_RXD        0x150   /* spi0_sclk, mode 1 (uart2_rxd) */
#define AM335X_PIN_UART2_TXD        0x154   /* spi0_d0, mode 1 (uart2_txd) */

/* UART3 pins */
#define AM335X_PIN_UART3_RXD        0x160   /* ecap0_in_pwm0_out, mode 1 (uart3_rxd) */
#define AM335X_PIN_UART3_TXD        0x164   /* spi0_cs1, mode 1 (uart3_txd) */

/* UART4 pins */
#define AM335X_PIN_UART4_RXD        0x070   /* gpmc_wait0, mode 6 (uart4_rxd) */
#define AM335X_PIN_UART4_TXD        0x074   /* gpmc_wpn, mode 6 (uart4_txd) */

/* UART5 pins */
#define AM335X_PIN_UART5_RXD        0x0C4   /* lcd_data9, mode 4 (uart5_rxd) */
#define AM335X_PIN_UART5_TXD        0x0C0   /* lcd_data8, mode 4 (uart5_txd) */

/* GPIO pins - BeagleBone Black User LEDs */
#define AM335X_PIN_GPMC_A5          0x054   /* gpmc_a5, mode 7 = GPIO1_21 (USR0) */
#define AM335X_PIN_GPMC_A6          0x058   /* gpmc_a6, mode 7 = GPIO1_22 (USR1) */
#define AM335X_PIN_GPMC_A7          0x05C   /* gpmc_a7, mode 7 = GPIO1_23 (USR2) */
#define AM335X_PIN_GPMC_A8          0x060   /* gpmc_a8, mode 7 = GPIO1_24 (USR3) */

/* I2C0 pins */
#define AM335X_PIN_I2C0_SDA         0x188   /* i2c0_sda, mode 0 */
#define AM335X_PIN_I2C0_SCL         0x18C   /* i2c0_scl, mode 0 */

/* I2C1 pins */
#define AM335X_PIN_I2C1_SDA         0x158   /* spi0_d1, mode 2 (i2c1_sda) */
#define AM335X_PIN_I2C1_SCL         0x15C   /* spi0_cs0, mode 2 (i2c1_scl) */

/* I2C2 pins */
#define AM335X_PIN_I2C2_SDA         0x17C   /* uart1_rtsn, mode 3 (i2c2_sda) */
#define AM335X_PIN_I2C2_SCL         0x178   /* uart1_ctsn, mode 3 (i2c2_scl) */

/* SPI0 pins */
#define AM335X_PIN_SPI0_SCLK        0x150   /* spi0_sclk, mode 0 */
#define AM335X_PIN_SPI0_D0          0x154   /* spi0_d0, mode 0 (MOSI) */
#define AM335X_PIN_SPI0_D1          0x158   /* spi0_d1, mode 0 (MISO) */
#define AM335X_PIN_SPI0_CS0         0x15C   /* spi0_cs0, mode 0 */
#define AM335X_PIN_SPI0_CS1         0x160   /* spi0_cs1, mode 0 */

/* SPI1 pins */
#define AM335X_PIN_SPI1_SCLK        0x190   /* mcasp0_aclkx, mode 3 (spi1_sclk) */
#define AM335X_PIN_SPI1_D0          0x194   /* mcasp0_fsx, mode 3 (spi1_d0) */
#define AM335X_PIN_SPI1_D1          0x198   /* mcasp0_axr0, mode 3 (spi1_d1) */
#define AM335X_PIN_SPI1_CS0         0x19C   /* mcasp0_ahclkr, mode 3 (spi1_cs0) */

/* MMC0 pins (microSD on BBB) */
#define AM335X_PIN_MMC0_DAT3        0x0F0   /* mmc0_dat3, mode 0 */
#define AM335X_PIN_MMC0_DAT2        0x0F4   /* mmc0_dat2, mode 0 */
#define AM335X_PIN_MMC0_DAT1        0x0F8   /* mmc0_dat1, mode 0 */
#define AM335X_PIN_MMC0_DAT0        0x0FC   /* mmc0_dat0, mode 0 */
#define AM335X_PIN_MMC0_CLK         0x100   /* mmc0_clk, mode 0 */
#define AM335X_PIN_MMC0_CMD         0x104   /* mmc0_cmd, mode 0 */
#define AM335X_PIN_MMC0_CD          0x108   /* mmc0_cd, mode 0 (card detect) */

/* MMC1 pins (eMMC on BBB) */
#define AM335X_PIN_MMC1_DAT7        0x080   /* gpmc_csn1, mode 2 (mmc1_dat7) */
#define AM335X_PIN_MMC1_DAT6        0x084   /* gpmc_csn2, mode 2 (mmc1_dat6) */
#define AM335X_PIN_MMC1_DAT5        0x088   /* gpmc_csn3, mode 2 (mmc1_dat5) */
#define AM335X_PIN_MMC1_DAT4        0x08C   /* gpmc_clk, mode 2 (mmc1_dat4) */
#define AM335X_PIN_MMC1_DAT3        0x03C   /* gpmc_ad15, mode 1 (mmc1_dat3) */
#define AM335X_PIN_MMC1_DAT2        0x038   /* gpmc_ad14, mode 1 (mmc1_dat2) */
#define AM335X_PIN_MMC1_DAT1        0x034   /* gpmc_ad13, mode 1 (mmc1_dat1) */
#define AM335X_PIN_MMC1_DAT0        0x030   /* gpmc_ad12, mode 1 (mmc1_dat0) */
#define AM335X_PIN_MMC1_CLK         0x07C   /* gpmc_csn0, mode 2 (mmc1_clk) */
#define AM335X_PIN_MMC1_CMD         0x000   /* gpmc_ad0, mode 1 (mmc1_cmd) */

/* CAN pins */
#define AM335X_PIN_DCAN0_RX         0x178   /* uart1_ctsn, mode 2 (dcan0_rx) */
#define AM335X_PIN_DCAN0_TX         0x17C   /* uart1_rtsn, mode 2 (dcan0_tx) */
#define AM335X_PIN_DCAN1_RX         0x180   /* uart1_rxd, mode 2 (dcan1_rx) */
#define AM335X_PIN_DCAN1_TX         0x184   /* uart1_txd, mode 2 (dcan1_tx) */

/* PWM pins (EHRPWM) */
#define AM335X_PIN_EHRPWM0A         0x150   /* spi0_sclk, mode 3 (ehrpwm0a) */
#define AM335X_PIN_EHRPWM0B         0x154   /* spi0_d0, mode 3 (ehrpwm0b) */
#define AM335X_PIN_EHRPWM1A         0x048   /* gpmc_a2, mode 6 (ehrpwm1a) */
#define AM335X_PIN_EHRPWM1B         0x04C   /* gpmc_a3, mode 6 (ehrpwm1b) */
#define AM335X_PIN_EHRPWM2A         0x020   /* gpmc_ad8, mode 4 (ehrpwm2a) */
#define AM335X_PIN_EHRPWM2B         0x024   /* gpmc_ad9, mode 4 (ehrpwm2b) */

/* eCAP pins */
#define AM335X_PIN_ECAP0            0x164   /* ecap0_in_pwm0_out, mode 0 */
#define AM335X_PIN_ECAP1            0x1A0   /* mcasp0_axr1, mode 4 (ecap1_in_pwm1_out) */
#define AM335X_PIN_ECAP2            0x19C   /* mcasp0_ahclkr, mode 4 (ecap2_in_pwm2_out) */

/* MDIO (Ethernet management) */
#define AM335X_PIN_MDIO_DATA        0x148   /* mdio_data, mode 0 */
#define AM335X_PIN_MDIO_CLK         0x14C   /* mdio_clk, mode 0 */

/* RGMII1 (Ethernet PHY 1) */
#define AM335X_PIN_RGMII1_TCLK      0x110   /* mii1_txclk, mode 2 (rgmii1_tclk) */
#define AM335X_PIN_RGMII1_TCTL      0x114   /* mii1_txen, mode 2 (rgmii1_tctl) */
#define AM335X_PIN_RGMII1_TD0       0x128   /* mii1_txd0, mode 2 (rgmii1_td0) */
#define AM335X_PIN_RGMII1_TD1       0x124   /* mii1_txd1, mode 2 (rgmii1_td1) */
#define AM335X_PIN_RGMII1_TD2       0x120   /* mii1_txd2, mode 2 (rgmii1_td2) */
#define AM335X_PIN_RGMII1_TD3       0x11C   /* mii1_txd3, mode 2 (rgmii1_td3) */
#define AM335X_PIN_RGMII1_RCLK      0x130   /* mii1_rxclk, mode 2 (rgmii1_rclk) */
#define AM335X_PIN_RGMII1_RCTL      0x118   /* mii1_rxdv, mode 2 (rgmii1_rctl) */
#define AM335X_PIN_RGMII1_RD0       0x140   /* mii1_rxd0, mode 2 (rgmii1_rd0) */
#define AM335X_PIN_RGMII1_RD1       0x13C   /* mii1_rxd1, mode 2 (rgmii1_rd1) */
#define AM335X_PIN_RGMII1_RD2       0x138   /* mii1_rxd2, mode 2 (rgmii1_rd2) */
#define AM335X_PIN_RGMII1_RD3       0x134   /* mii1_rxd3, mode 2 (rgmii1_rd3) */

/* USB pins */
#define AM335X_PIN_USB0_DRVVBUS     0x21C   /* usb0_drvvbus, mode 0 */
#define AM335X_PIN_USB1_DRVVBUS     0x234   /* usb1_drvvbus, mode 0 */

/* NAND/GPMC pins */
#define AM335X_PIN_GPMC_AD0         0x000   /* gpmc_ad0, mode 0 */
#define AM335X_PIN_GPMC_AD1         0x004   /* gpmc_ad1, mode 0 */
#define AM335X_PIN_GPMC_AD2         0x008   /* gpmc_ad2, mode 0 */
#define AM335X_PIN_GPMC_AD3         0x00C   /* gpmc_ad3, mode 0 */
#define AM335X_PIN_GPMC_AD4         0x010   /* gpmc_ad4, mode 0 */
#define AM335X_PIN_GPMC_AD5         0x014   /* gpmc_ad5, mode 0 */
#define AM335X_PIN_GPMC_AD6         0x018   /* gpmc_ad6, mode 0 */
#define AM335X_PIN_GPMC_AD7         0x01C   /* gpmc_ad7, mode 0 */
#define AM335X_PIN_GPMC_AD8         0x020   /* gpmc_ad8, mode 0 */
#define AM335X_PIN_GPMC_AD9         0x024   /* gpmc_ad9, mode 0 */
#define AM335X_PIN_GPMC_AD10        0x028   /* gpmc_ad10, mode 0 */
#define AM335X_PIN_GPMC_AD11        0x02C   /* gpmc_ad11, mode 0 */
#define AM335X_PIN_GPMC_AD12        0x030   /* gpmc_ad12, mode 0 */
#define AM335X_PIN_GPMC_AD13        0x034   /* gpmc_ad13, mode 0 */
#define AM335X_PIN_GPMC_AD14        0x038   /* gpmc_ad14, mode 0 */
#define AM335X_PIN_GPMC_AD15        0x03C   /* gpmc_ad15, mode 0 */

/* Additional common GPIO pins */
#define AM335X_PIN_GPIO0_7          0x164   /* ecap0_in_pwm0_out, mode 7 = GPIO0_7 */
#define AM335X_PIN_GPIO1_0          0x054   /* gpmc_a5, mode 7 = GPIO1_21 */
#define AM335X_PIN_GPIO1_12         0x030   /* gpmc_ad12, mode 7 = GPIO1_12 */
#define AM335X_PIN_GPIO1_13         0x034   /* gpmc_ad13, mode 7 = GPIO1_13 */
#define AM335X_PIN_GPIO1_14         0x038   /* gpmc_ad14, mode 7 = GPIO1_14 */
#define AM335X_PIN_GPIO1_15         0x03C   /* gpmc_ad15, mode 7 = GPIO1_15 */
#define AM335X_PIN_GPIO1_28         0x078   /* gpmc_ben1, mode 7 = GPIO1_28 */

#endif /* ZEPHYR_INCLUDE_DT_BINDINGS_PINCTRL_AM335X_H_ */
