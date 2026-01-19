/* SPDX-License-Identifier: Apache-2.0 */

#define DT_DRV_COMPAT ti_am335x_uart

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/irq.h>

/* UART Register Offsets */
#define UART_RHR        0x00  /* Receive Holding Register */
#define UART_THR        0x00  /* Transmit Holding Register */
#define UART_IER        0x04  /* Interrupt Enable Register */
#define UART_IIR        0x08  /* Interrupt Identification Register */
#define UART_FCR        0x08  /* FIFO Control Register */
#define UART_LCR        0x0C  /* Line Control Register */
#define UART_MCR        0x10  /* Modem Control Register */
#define UART_LSR        0x14  /* Line Status Register */
#define UART_MSR        0x18  /* Modem Status Register */
#define UART_SPR        0x1C  /* Scratch Pad Register */
#define UART_DLL        0x00  /* Divisor Latch Low (when LCR[7]=1) */
#define UART_DLH        0x04  /* Divisor Latch High (when LCR[7]=1) */
#define UART_MDR1       0x20  /* Mode Definition Register 1 */
#define UART_SYSC       0x54  /* System Configuration Register */
#define UART_SYSS       0x58  /* System Status Register */

/* LCR Register Bits */
#define UART_LCR_DLAB   BIT(7)  /* Divisor Latch Access Bit */
#define UART_LCR_8N1    0x03    /* 8 data bits, no parity, 1 stop bit */

/* LSR Register Bits */
#define UART_LSR_RXFIFOE BIT(0)  /* RX FIFO error */
#define UART_LSR_RXFE    BIT(0)  /* RX FIFO empty */
#define UART_LSR_THRE    BIT(5)  /* THR empty */
#define UART_LSR_TEMT    BIT(6)  /* Transmitter empty */

/* IER Register Bits */
#define UART_IER_RHR    BIT(0)  /* Receive data available interrupt */
#define UART_IER_THR    BIT(1)  /* Transmit holding register empty */

/* FCR Register Bits */
#define UART_FCR_FIFO_EN     BIT(0)  /* FIFO Enable */
#define UART_FCR_RXCLR       BIT(1)  /* RX FIFO Clear */
#define UART_FCR_TXCLR       BIT(2)  /* TX FIFO Clear */

/* MDR1 Register - Mode Definition */
#define UART_MDR1_MODE_16X   0x00    /* 16x mode */
#define UART_MDR1_MODE_DISABLE 0x07  /* Disable UART */

/* System Configuration */
#define UART_SYSC_SOFTRESET  BIT(1)
#define UART_SYSS_RESETDONE  BIT(0)


#define UART_CLK_FREQ   48000000  /* 48 MHz */

struct uart_am335x_config {
	uint32_t base;
	uint32_t clock_reg;
	uint32_t baud_rate;
	uint8_t irq;
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	void (*irq_config_func)(const struct device *dev);
#endif
};

struct uart_am335x_data {
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	uart_irq_callback_user_data_t callback;
	void *cb_data;
#endif
};

static inline uint32_t uart_read(const struct device *dev, uint32_t reg)
{
	const struct uart_am335x_config *cfg = dev->config;
	return sys_read32(cfg->base + reg);
}

static inline void uart_write(const struct device *dev, uint32_t reg, uint32_t val)
{
	const struct uart_am335x_config *cfg = dev->config;
	sys_write32(val, cfg->base + reg);
}

static void uart_am335x_enable_clock(const struct device *dev)
{
	const struct uart_am335x_config *cfg = dev->config;
	volatile uint32_t *clk_reg = (volatile uint32_t *)cfg->clock_reg;
	
	/* Enable module clock */
	*clk_reg = 0x2;
	
	/* Wait for module to be enabled */
	while ((*clk_reg & (0x3 << 16)) != 0) {
		/* Wait */
	}
}

static int uart_am335x_poll_in(const struct device *dev, unsigned char *c)
{
	uint32_t lsr = uart_read(dev, UART_LSR);

	if (lsr & UART_LSR_RXFE) {
		return -1;
	}

	*c = (unsigned char)uart_read(dev, UART_RHR);
	return 0;
}

static void uart_am335x_poll_out(const struct device *dev, unsigned char c)
{
	/* Wait for TX FIFO to be ready */
	while ((uart_read(dev, UART_LSR) & UART_LSR_THRE) == 0) {
		/* Wait */
	}

	uart_write(dev, UART_THR, c);
}

static int uart_am335x_configure(const struct device *dev,
				 const struct uart_config *cfg)
{
	const struct uart_am335x_config *config = dev->config;
	uint32_t divisor;

	/* Calculate baud rate divisor */
	divisor = UART_CLK_FREQ / (16 * config->baud_rate);

	/* Disable UART */
	uart_write(dev, UART_MDR1, UART_MDR1_MODE_DISABLE);

	/* Enable access to DLL and DLH registers */
	uart_write(dev, UART_LCR, UART_LCR_DLAB);

	/* Set baud rate */
	uart_write(dev, UART_DLL, divisor & 0xFF);
	uart_write(dev, UART_DLH, (divisor >> 8) & 0xFF);

	/* Set 8N1 mode, disable DLAB */
	uart_write(dev, UART_LCR, UART_LCR_8N1);

	/* Enable and reset FIFOs */
	uart_write(dev, UART_FCR, UART_FCR_FIFO_EN | UART_FCR_RXCLR | UART_FCR_TXCLR);

	/* Enable UART in 16x mode */
	uart_write(dev, UART_MDR1, UART_MDR1_MODE_16X);

	return 0;
}

static int uart_am335x_config_get(const struct device *dev,
				  struct uart_config *cfg)
{
	const struct uart_am335x_config *config = dev->config;

	cfg->baudrate = config->baud_rate;
	cfg->parity = UART_CFG_PARITY_NONE;
	cfg->stop_bits = UART_CFG_STOP_BITS_1;
	cfg->data_bits = UART_CFG_DATA_BITS_8;
	cfg->flow_ctrl = UART_CFG_FLOW_CTRL_NONE;

	return 0;
}

#ifdef CONFIG_UART_INTERRUPT_DRIVEN

static int uart_am335x_fifo_fill(const struct device *dev,
				 const uint8_t *tx_data, int size)
{
	int i;

	for (i = 0; i < size; i++) {
		if ((uart_read(dev, UART_LSR) & UART_LSR_THRE) == 0) {
			break;
		}
		uart_write(dev, UART_THR, tx_data[i]);
	}

	return i;
}

static int uart_am335x_fifo_read(const struct device *dev,
				 uint8_t *rx_data, const int size)
{
	int i;

	for (i = 0; i < size; i++) {
		if ((uart_read(dev, UART_LSR) & UART_LSR_RXFE) != 0) {
			break;
		}
		rx_data[i] = uart_read(dev, UART_RHR);
	}

	return i;
}

static void uart_am335x_irq_tx_enable(const struct device *dev)
{
	uint32_t ier = uart_read(dev, UART_IER);
	uart_write(dev, UART_IER, ier | UART_IER_THR);
}

static void uart_am335x_irq_tx_disable(const struct device *dev)
{
	uint32_t ier = uart_read(dev, UART_IER);
	uart_write(dev, UART_IER, ier & ~UART_IER_THR);
}

static int uart_am335x_irq_tx_ready(const struct device *dev)
{
	return (uart_read(dev, UART_LSR) & UART_LSR_THRE) != 0;
}

static void uart_am335x_irq_rx_enable(const struct device *dev)
{
	uint32_t ier = uart_read(dev, UART_IER);
	uart_write(dev, UART_IER, ier | UART_IER_RHR);
}

static void uart_am335x_irq_rx_disable(const struct device *dev)
{
	uint32_t ier = uart_read(dev, UART_IER);
	uart_write(dev, UART_IER, ier & ~UART_IER_RHR);
}

static int uart_am335x_irq_rx_ready(const struct device *dev)
{
	return (uart_read(dev, UART_LSR) & UART_LSR_RXFE) == 0;
}

static int uart_am335x_irq_is_pending(const struct device *dev)
{
	uint32_t iir = uart_read(dev, UART_IIR);
	return (iir & 0x01) == 0;
}

static int uart_am335x_irq_update(const struct device *dev)
{
	return 1;
}

static void uart_am335x_irq_callback_set(const struct device *dev,
					 uart_irq_callback_user_data_t cb,
					 void *user_data)
{
	struct uart_am335x_data *data = dev->data;

	data->callback = cb;
	data->cb_data = user_data;
}

static void uart_am335x_isr(const struct device *dev)
{
	struct uart_am335x_data *data = dev->data;

	if (data->callback) {
		data->callback(dev, data->cb_data);
	}
}

#endif /* CONFIG_UART_INTERRUPT_DRIVEN */

static int uart_am335x_init(const struct device *dev)
{
	const struct uart_am335x_config *config = dev->config;
	struct uart_config uart_cfg = {
		.baudrate = config->baud_rate,
		.parity = UART_CFG_PARITY_NONE,
		.stop_bits = UART_CFG_STOP_BITS_1,
		.data_bits = UART_CFG_DATA_BITS_8,
		.flow_ctrl = UART_CFG_FLOW_CTRL_NONE,
	};

	/* Enable clock */
	uart_am335x_enable_clock(dev);

	/* Soft reset */
	uart_write(dev, UART_SYSC, UART_SYSC_SOFTRESET);
	while ((uart_read(dev, UART_SYSS) & UART_SYSS_RESETDONE) == 0) {
		/* Wait */
	}

	/* Configure UART */
	uart_am335x_configure(dev, &uart_cfg);

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	config->irq_config_func(dev);
#endif

	return 0;
}

static const struct uart_driver_api uart_am335x_api = {
	.poll_in = uart_am335x_poll_in,
	.poll_out = uart_am335x_poll_out,
	.configure = uart_am335x_configure,
	.config_get = uart_am335x_config_get,
#ifdef CONFIG_UART_INTERRUPT_DRIVEN
	.fifo_fill = uart_am335x_fifo_fill,
	.fifo_read = uart_am335x_fifo_read,
	.irq_tx_enable = uart_am335x_irq_tx_enable,
	.irq_tx_disable = uart_am335x_irq_tx_disable,
	.irq_tx_ready = uart_am335x_irq_tx_ready,
	.irq_rx_enable = uart_am335x_irq_rx_enable,
	.irq_rx_disable = uart_am335x_irq_rx_disable,
	.irq_rx_ready = uart_am335x_irq_rx_ready,
	.irq_is_pending = uart_am335x_irq_is_pending,
	.irq_update = uart_am335x_irq_update,
	.irq_callback_set = uart_am335x_irq_callback_set,
#endif
};

#ifdef CONFIG_UART_INTERRUPT_DRIVEN
#define UART_AM335X_IRQ_CONFIG(n)					\
	static void uart_am335x_irq_config_##n(const struct device *dev) \
	{								\
		IRQ_CONNECT(DT_INST_IRQN(n),				\
			    DT_INST_IRQ(n, priority),			\
			    uart_am335x_isr,				\
			    DEVICE_DT_INST_GET(n),			\
			    0);						\
		irq_enable(DT_INST_IRQN(n));				\
	}
#else
#define UART_AM335X_IRQ_CONFIG(n)
#endif

#define UART_AM335X_INIT(n)						\
	UART_AM335X_IRQ_CONFIG(n)					\
									\
	static const struct uart_am335x_config uart_am335x_config_##n = { \
		.base = DT_INST_REG_ADDR(n),				\
		.clock_reg = (n == 0) ? CM_WKUP_UART0_CLKCTRL :		\
			     (n == 1) ? CM_PER_UART1_CLKCTRL :		\
			     CM_PER_UART2_CLKCTRL,			\
		.baud_rate = DT_INST_PROP(n, current_speed),		\
		.irq = DT_INST_IRQN(n),					\
		IF_ENABLED(CONFIG_UART_INTERRUPT_DRIVEN,		\
			   (.irq_config_func = uart_am335x_irq_config_##n,)) \
	};								\
									\
	static struct uart_am335x_data uart_am335x_data_##n;		\
									\
	DEVICE_DT_INST_DEFINE(n,					\
			      uart_am335x_init,				\
			      NULL,					\
			      &uart_am335x_data_##n,			\
			      &uart_am335x_config_##n,			\
			      PRE_KERNEL_1,				\
			      CONFIG_SERIAL_INIT_PRIORITY,		\
			      &uart_am335x_api);

DT_INST_FOREACH_STATUS_OKAY(UART_AM335X_INIT)
