#include <stdint.h>
#include "f411.h"

struct uart {
	volatile unsigned int status; /* 00 */
	volatile unsigned int data;   /* 04 */
	volatile unsigned int baud;   /* 08 */
	volatile unsigned int cr1;    /* 0c */
	volatile unsigned int cr2;    /* 10 */
	volatile unsigned int cr3;    /* 14 */
	volatile unsigned int gtp;    /* 18 - guard time and prescaler */
};

#define UART1_BASE (struct uart *) 0x40011000
#define UART2_BASE (struct uart *) 0x40004400
#define UART3_BASE (struct uart *) 0x40011400

#define UART1_IRQ 37
#define UART2_IRQ 38
#define UART3_IRQ 71

static struct uart *uart_bases[] = { UART1_BASE, UART2_BASE, UART3_BASE };

/* Bits in Cr1 */
#define CR1_ENABLE 0x2000
#define CR1_9BIT 0x1000
#define CR1_WAKE 0x0800
#define CR1_PARITY 0x0400
#define CR1_ODD 0x0200
#define CR1_PIE 0x0100
#define CR1_TXEIE 0x0080
#define CR1_TCIE 0x0040
#define CR1_RXIE 0x0020
#define CR1_IDLE_IE 0x0010
#define CR1_TXE 0x0008
#define CR1_RXE 0x0004
#define CR1_RWU 0x0002
#define CR1_BRK 0x0001

// 0x340c
#define CR1_CONSOLE (CR1_ENABLE | CR1_9BIT | CR1_PARITY | CR1_TXE | CR1_RXE)

/* bits in the status register */
#define ST_PE 0x0001
#define ST_FE 0x0002
#define ST_NE 0x0004
#define ST_OVER 0x0008
#define ST_IDLE 0x0010
#define ST_RXNE 0x0020 /* Receiver not empty */
#define ST_TC 0x0040   /* Transmission complete */
#define ST_TXE 0x0080  /* Transmitter empty */
#define ST_BREAK 0x0100
#define ST_CTS 0x0200

#define SERIAL_SEND_BUF_LEN (1000)

uint8_t serial_data[SERIAL_SEND_BUF_LEN] = { 0 };
unsigned serial_send_cursor = 0;
unsigned serial_write_cursor = 0;

static int std_serial = UART1;

/* IRQ */
void uart1_handler(void) {
	struct uart *up = UART1_BASE;

	if (up->status & ST_TXE) {
		if (serial_send_cursor != serial_write_cursor) {
			up->data = serial_data[serial_send_cursor];
			serial_send_cursor++;
			if (serial_send_cursor >= SERIAL_SEND_BUF_LEN) {
				serial_send_cursor = 0;
			}
		} else {
			up->cr1 &= ~CR1_TXEIE;
		}
	}
}

int serial_begin(int uart, int baud) {
	struct uart *up;

	gpio_uart_init(uart);

	up = uart_bases[uart];

	/* 1 start bit, even parity */
	up->cr1 = CR1_CONSOLE;

	up->cr2 = 0;
	up->cr3 = 0;
	up->gtp = 0;

	// up->cr1 |= CR1_TXEIE;

	nvic_enable(UART1_IRQ);

	up->baud = get_pclk2() / baud;
	return uart;
}


static void serial_puts(int uart, char *str) {
	struct uart *up = uart_bases[uart];

	if (serial_send_cursor == serial_write_cursor) {
		up->data = *str++;
	}
	while (*str) {
		char byte = *str;

		serial_data[serial_write_cursor] = byte;
		str++;
		serial_write_cursor++;
		if (serial_write_cursor == SERIAL_SEND_BUF_LEN) {
			serial_write_cursor = 0;
		}
		if (byte == '\n') {
			/* todo ugly */
			serial_data[serial_write_cursor] = '\r';
			serial_write_cursor++;
			if (serial_write_cursor == SERIAL_SEND_BUF_LEN) {
				serial_write_cursor = 0;
			}
		}
	}
	up->cr1 |= CR1_TXEIE;
}

void puts(char *msg) {
	serial_puts(std_serial, msg);
}
