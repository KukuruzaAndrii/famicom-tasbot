/* gpio.c
 * Basic GPIO driver for the F411
 */

#include "f411.h"

#define LED_GPIO GPIOC /* PC13 */

struct gpio {
	volatile unsigned int mode;   /* 0x00 */
	volatile unsigned int otype;  /* 0x04 */
	volatile unsigned int ospeed; /* 0x08 */
	volatile unsigned int pupd;   /* 0x0c */
	volatile unsigned int idata;  /* 0x10 */
	volatile unsigned int odata;  /* 0x14 */
	volatile unsigned int bsrr;   /* 0x18 */
	volatile unsigned int lock;   /* 0x1c */
	volatile unsigned int afl;    /* 0x20 */
	volatile unsigned int afh;    /* 0x24 */
};

#define GPIOA_BASE (struct gpio *) 0x40020000
#define GPIOB_BASE (struct gpio *) 0x40020400
#define GPIOC_BASE (struct gpio *) 0x40020800

static struct gpio *gpio_bases[] = { GPIOA_BASE, GPIOB_BASE, GPIOC_BASE };

/* Change alternate function setting for a pin
 * These are 4 bit fields. All initially 0.
 */
void gpio_af(int gpio, int pin, int val) {
	struct gpio *gp;
	int shift;

	gp = gpio_bases[gpio];

	if (pin < 8) {
		shift = pin * 4;
		gp->afl &= ~(0xf << shift);
		gp->afl |= val << shift;
	} else {
		shift = (pin - 8) * 4;
		gp->afh &= ~(0xf << shift);
		gp->afh |= val << shift;
	}
}

#define MODE_INPUT 0
#define MODE_OUT 1
#define MODE_AF 2
#define MODE_ANALOG 3

/* This is a 2 bit field */
void gpio_mode(int gpio, int pin, int val) {
	struct gpio *gp;
	int shift;

	gp = gpio_bases[gpio];
	shift = pin * 2;
	gp->mode &= ~(0x3 << shift);
	gp->mode |= val << shift;
}

#define PUPD_NONE 0
#define PUPD_UP 1
#define PUPD_DOWN 2
#define PUPD_XXX 3

/* This is a 2 bit field */
void gpio_pupd(int gpio, int pin, int val) {
	struct gpio *gp;
	int shift;

	gp = gpio_bases[gpio];
	shift = pin * 2;
	gp->pupd &= ~(0x3 << shift);
	gp->pupd |= val << shift;
}

/* kludge for now */
void gpio_uart(int gpio, int pin) {
	struct gpio *gp;
	int shift;

	gp = gpio_bases[gpio];

	gp->otype &= ~(1 << pin);

	shift = pin * 2;
	gp->ospeed |= (3 << shift);

	// gp->pupd &= ~(3<<shift);
	gpio_pupd(gpio, pin, PUPD_NONE);
}

void gpio_input_init(int gpio, int pin) {
	gpio_mode(gpio, pin, MODE_INPUT);
	gpio_pupd(gpio, pin, PUPD_UP);
}
void gpio_input_init_down(int gpio, int pin) {
	gpio_mode(gpio, pin, MODE_INPUT);
	gpio_pupd(gpio, pin, PUPD_DOWN);
}
void gpio_output_init(int gpio, int pin) {
	gpio_mode(gpio, pin, MODE_OUT);
	struct gpio *gp = gpio_bases[gpio];
	int shift = pin * 2;
	gp->ospeed |= (3 << shift);
}

int gpio_read(int gpio, int pin) {
	struct gpio *gp = gpio_bases[gpio];

	return gp->idata & (1 << pin);
}

void gpio_uart_init(int uart) {
	if (uart == UART1) {
		gpio_af(GPIOA, 9, 7); /* Tx */
		gpio_mode(GPIOA, 9, MODE_AF);
		gpio_uart(GPIOA, 9);

		gpio_af(GPIOA, 10, 7); /* Rx */
		gpio_mode(GPIOA, 10, MODE_AF);
		gpio_uart(GPIOA, 10);
	}
}

void pin_on(int gpio, int pin) {
	struct gpio *gp = gpio_bases[gpio];
	gp->bsrr = 1 << pin;
}

void pin_off(int gpio, int pin) {
	struct gpio *gp = gpio_bases[gpio];
	gp->bsrr = 1 << (pin + 16);
}
