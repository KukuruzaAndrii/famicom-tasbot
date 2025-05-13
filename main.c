
#include "f411.h"
#include <stddef.h>
#include "tas.inc"

#include "SEGGER_SYSVIEW_Conf.h"
#include "SEGGER_SYSVIEW.h"

// for rtt
unsigned SystemCoreClock = 96000000;

static unsigned is_started = 0;

volatile unsigned latch_count = 0;
volatile unsigned frame_count = -1;

volatile unsigned data = 0;

unsigned get_tas_data(unsigned frame_count) {
	return tas_data[frame_count * 2];
}
extern unsigned was_first_capt;
static void reset(void) {
	latch_count = 0;
	/* todo dirty */
	frame_count = -1;
	data = 0;
	was_first_capt = 0;
}

static void send_data_bit() {
	unsigned pressed = data & 1;
	if (pressed) {
		// inverted signal
		pin_off(GPIOA, 4);
	} else {
		pin_on(GPIOA, 4);
	}
	// shift to next bit
	data = data >> 1;
}

/* Handler for latch with time bitween trigering */
void timer_cb(unsigned time) {
	disable_irq;
	if (time < 1000) {
		latch_count++;
		// printf("lc %d\n", latch_count);
	} else if (time < 33260) {
		latch_count = 0;
		frame_count++;
		// printf(" \nfc %d\n", frame_count);
	} else {
		latch_count = 0;
		frame_count += time / 33260;
		// printf("fc! %d\n", frame_count);
	}

	if (latch_count == 0 || latch_count == 1) {
		// second pl data
		// data = tas_data[frame_count * 2 + 1];
		data = 0;
	} else if (latch_count == 2 || latch_count == 3) {
		// first pl data
		data = get_tas_data(frame_count);
	}
	send_data_bit();
	enable_irq;
}

static void clock_cb(void) {
	if (!is_started) {
		return;
	}
	disable_irq;

	send_data_bit();
	enable_irq;
}

struct exti {
	volatile unsigned int imask;    /* 0x00 */
	volatile unsigned int mask;     /* 0x04 */
	volatile unsigned int rtrigger; /* 0x08 */
	volatile unsigned int ftrigger; /* 0x0c */
	volatile unsigned int soft;     /* 0x10 - software trigger*/
	volatile unsigned int pending;  /* 0x14 */
};

#define EXTI_BASE (struct exti *) 0x40013c00

/* Handler for powering up console */
void exti1_handler(void) {
	SEGGER_SYSVIEW_RecordEnterISR();
	struct exti *ep = EXTI_BASE;

	ep->pending |= 1 << 1;

	if (gpio_read(GPIOA, 0)) {
		puts("start1\n");
		reset();
		is_started = 1;
	} else {
		puts("start0\n");
		is_started = 0;
		pin_off(GPIOA, 4);
	}
	SEGGER_SYSVIEW_RecordExitISR();
}

/* Handler for clock*/
void exti2_handler(void) {
	SEGGER_SYSVIEW_RecordEnterISR();
	struct exti *ep = EXTI_BASE;

	ep->pending |= 1 << 2;
	clock_cb();
	SEGGER_SYSVIEW_RecordExitISR();
}

void startup(void) {
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();
	SEGGER_SYSVIEW_OnIdle();
	serial_begin(UART1, 115200);

	puts("\n\r");
	puts("Up and running\n");

	exti_setup(GPIOA, 1);
	exti_setup(GPIOA, 2);
	gpio_output_init(GPIOA, 4);
	pin_off(GPIOA, 4);

	for (;;)
		;
}


