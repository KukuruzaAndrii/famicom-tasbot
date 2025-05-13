#include "f411.h"

struct timer {
	volatile int cr1;   /* 0x00 */
	volatile int cr2;   /* 0x04 */
	volatile int smcr;  /* 0x08 */
	volatile int dier;  /* 0x0C */
	volatile int sr;    /* 0x10 */
	volatile int egr;   /* 0x14 */
	volatile int ccmr1; /* 0x18 */
	volatile int ccmr2; /* 0x1C */
	volatile int ccer;  /* 0x20 */
	volatile int cnt;   /* 0x24 */
	volatile int psc;   /* 0x28 */
	volatile int arr;   /* 0x2C */
	int __pad1[1];      /* 0x30 */
	volatile int ccr1;  /* 0x34 */
	volatile int ccr2;  /* 0x38 */
	volatile int ccr3;  /* 0x3C */
	volatile int ccr4;  /* 0x40 */
	int __pad2[1];      /* 0x44 */
	volatile int dcr;   /* 0x48 */
	volatile int dmar;  /* 0x4C */
	volatile int or;    /* 0x50 */
};

/* Enable couning */
#define CR1_COUNTER_EN (BIT(0))
/* */
#define CCMR1_CC1S_INPUT_TI 0x01
/* Enable channel 1 of timer */
#define CCER_CC1E_EN (BIT(0))
/* Enable channel 1 interrupt */
#define DIER_CC1IE_EN (BIT(1))

/* Interrupt on channel 1 */
#define SR_CC1IF (BIT(1))

// 0x4000 0000 - 0x4000 03FF TIM2
#define TIM_2_BASE (struct timer *) 0x40000000

/* Alternate function - tim2 input*/
#define GPIO_ALTER_FUNC_01 1

#define TIM2_IRQ_NUM (28)

#define MODE_AF (2)

void timer_init(void) {
	gpio_af(GPIOA, 0, GPIO_ALTER_FUNC_01);

	gpio_mode(GPIOA, 0, MODE_AF);

	struct timer *tim2 = TIM_2_BASE;

	tim2->psc = 48 - 1;          // 1mks according to pclk1
	tim2->arr = 5 * 1000 * 1000; // 5s according to psc

	tim2->ccmr1 |= CCMR1_CC1S_INPUT_TI;
	tim2->ccer = CCER_CC1E_EN;

	tim2->dier |= DIER_CC1IE_EN;

	tim2->cr1 |= CR1_COUNTER_EN;

	nvic_enable(TIM2_IRQ_NUM);
}

unsigned was_first_capt = 0;
volatile unsigned ic_val_prev = 0;
volatile unsigned ic_val_now = 0;
extern unsigned f_count;
extern unsigned l_count;
extern unsigned data;

void tim2_handler(void) {
	SEGGER_SYSVIEW_RecordEnterISR();
	struct timer *tim2 = TIM_2_BASE;

	if (tim2->sr & SR_CC1IF) {
		if (!was_first_capt) {
			was_first_capt = 1;
			ic_val_prev = tim2->ccr1;
		} else {
			ic_val_now = tim2->ccr1;
			unsigned time;
			if (ic_val_now >= ic_val_prev) {
				time = ic_val_now - ic_val_prev;
			} else {
				time = ((5000000 - ic_val_prev) + ic_val_now) + 1;
			}
			// printf("s-%d\n",ic_val_prev);
			// printf("e-%d\n",ic_val_now);
			// printf("t-%d\n",time);
			ic_val_prev = ic_val_now;
			timer_cb(time);
		}
		tim2->sr &= ~SR_CC1IF;
	}
	SEGGER_SYSVIEW_RecordExitISR();
}
