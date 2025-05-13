#pragma once

#define UART1 0

/* names to index the bases array */
#define GPIOA 0
#define GPIOB 1
#define GPIOC 2

/* Handy macros */

/* These can be used as locks around critical sections */
#define enable_irq asm volatile("cpsie i" ::: "memory")
#define disable_irq asm volatile("cpsid i" ::: "memory")

#define BIT(nr) (1 << (nr))

