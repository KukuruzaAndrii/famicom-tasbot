/* Nvic.c
 *
 * The registers for this are part of the ARM Cortex M3
 *  "System Control Space" which contains:
 *
 *  0xE000E000 - interrupt type register
 *  0xE000E010 - system timer (SysTick)
 *  0xE000E100 - NVIC (nested vectored interrupt controller)
 *  0xE000ED00 - system control block (includes CPU ID)
 *  0xE000EF00 - software trigger exception registers
 *  0xE000EFD0 - ID space
 */

#include "f411.h"

struct nvic {
	volatile unsigned int iser[8]; /* 00 set enable */
	int __pad1[24];
	volatile unsigned int icer[8]; /* 80 clear enable */
	int __pad2[24];
	volatile unsigned int ispr[8]; /* 100 set pending */
	int __pad3[24];
	volatile unsigned int icpr[8]; /* 180 clear pending */
	int __pad4[24];
	volatile unsigned int iabr[8]; /* 200 active bit */
	int __pad5[56];
	volatile unsigned char ip[240]; /* 300 priority */
	int __pad6[644];
	volatile unsigned int stir; /* EF00 - software trigger */
};

#define NVIC_BASE ((struct nvic *) 0xe000e100)

#define NUM_IRQ 68

void nvic_init(void) {
	struct nvic *np = NVIC_BASE;
}

void nvic_enable(int irq) {
	struct nvic *np = NVIC_BASE;

	if (irq >= NUM_IRQ)
		return;

	np->iser[irq / 32] = 1 << (irq % 32);
}

/* =========================================================== */

/* Here is the EXTI controller stuff for the F411
 * it isn't part of the NVIC, but this file seems
 * like an OK place for it at least for now.
 */
#define EXTI_TO_IRQ(_EXTI) ((_EXTI) + 6)
#define IRQ_EXTI0 6
#define IRQ_EXTI1 7
#define IRQ_EXTI2 8
#define IRQ_EXTI3 9
#define IRQ_EXTI4 10

struct exti {
	volatile unsigned int imask;    /* 0x00 */
	volatile unsigned int mask;     /* 0x04 */
	volatile unsigned int rtrigger; /* 0x08 */
	volatile unsigned int ftrigger; /* 0x0c */
	volatile unsigned int soft;     /* 0x10 - software trigger*/
	volatile unsigned int pending;  /* 0x14 */
};

#define EXTI_BASE (struct exti *) 0x40013c00

/* The RM also mentions configuring the SYSCFG_EXTICR1 register.
 * for events from GPIO A0 and B0,
 * and corresponding SYSCFG registers for other pins.
 * This is in section 7 of the RM.
 */

struct syscfg {
	volatile unsigned int memremap;     /* 0x00 */
	volatile unsigned int pmc;          /* 0x04 */
	volatile unsigned int exti_conf[4]; /* 0x08 .. 0x14*/
	int __pad[2];
	volatile unsigned int cmpcr; /* 0x20 */
};

#define SYSCFG_BASE (struct syscfg *) 0x40013800

/* The conf registers each have 4 fields of 4 bits.
 * conf[0] handles pins 0-3 and routes
 * them to exti lines 0-3
 */
static void exti_sysconfig(int gpio, int pin) {
	struct syscfg *sp = SYSCFG_BASE;
	int line_index;
	int line_shift;
	int gpio_index;
	int gpio_val;

	line_index = pin / 4;
	line_shift = (pin % 4) * 4;

	gpio_val = gpio & 3;

	sp->exti_conf[line_index] &= ~(0xf << line_shift);
	sp->exti_conf[line_index] |= gpio_val << line_shift;
}

#define EXTI0 0
#define EXTI1 1
#define EXTI2 2
#define EXTI3 3

void exti_setup(int gpio, int pin) {
	struct exti *ep = EXTI_BASE;

	gpio_input_init(gpio, pin);

	exti_sysconfig(gpio, pin);

	int line = pin;
	ep->ftrigger |= 1 << line;
	if (pin == 1) {
		ep->rtrigger |= 1 << line;
	}

	ep->mask |= 1 << line;
	ep->imask |= 1 << line;

	nvic_enable(EXTI_TO_IRQ(line));
}

/* =========================================================== */

void idle(void) {
	for (;;) {
		// asm volatile( "wfe" );
		// asm volatile( "nop" );
	}
}

void bogus(void) {
	printf("Unexpected interrupt!\n");
	printf("Spinning\n");
	for (;;)
		;
}

void fault(void) {
	printf("Unexpected fault!\n");
	printf("Spinning\n");
	for (;;)
		;
}

