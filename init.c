/* init.c
 * (c) Tom Trebisky  11-24-2020
 *
 * The First C code that runs right after startup.
 *
 *  Zero BSS area.
 *  ....
 */

#include "f411.h"

/* These are arranged in the LDS script */
/* We don't need all these, but they were
 * interesting when sorting all this out and
 * do no harm (no extra memory is used.
 */
extern char __bss_start;
extern char __bss_end;
extern char __end;

extern int __data_start;
extern int __data_end;

extern int __text_start;
extern int __text_end;

extern unsigned int __rodata_start;
extern unsigned int __rodata_end;

struct dbgmcu {
	volatile int idcode;
	volatile int cr;
	volatile int apb1_fz;
	volatile int apb2_fz;
};

#define DBGMCU_BASE (struct dbgmcu *) 0xE0042000

struct debug_cont_block {
	volatile int dhcsr;
	volatile int dcrsr;
	volatile int dsrdr;
	volatile int demcr;
};

#define DEBUG_CONT_BLOCK_BASE ((struct debug_cont_block *) 0xE000EDF0)
#define TRCENA (BIT(24))

#define DWT_CTRL (0xE0001000)
#define CYCCNTENA (BIT(0))

void stm_init(void) {
	int *p;
	int *src;

	/* Zero the BSS area */
	for (p = (int *) &__bss_start; p < (int *) &__bss_end;)
		*p++ = 0;

	/* Copy initialized data from flash */
	// src = &__rodata_start;
	src = &__text_end;

	for (p = &__data_start; p < &__data_end; p++)
		*p = *src++;

	// struct dbgmcu* dbg = DBGMCU_BASE;
	// dbg->cr = (BIT(0)) | (BIT(1)) | (BIT(2)); // Debug Sleep Mode + Stop Mode + Standby mode

	struct debug_cont_block *dcb = DEBUG_CONT_BLOCK_BASE;
	dcb->demcr |= TRCENA;

	int *dwt_ctr = DWT_CTRL;
	*dwt_ctr |= CYCCNTENA;

	rcc_init();
	nvic_init();
	timer_init();

	/* Not needed, the chip starts up with
	 * interrupts enabled
	 */
	enable_irq;

	/* Call the user code */
	startup();
}
