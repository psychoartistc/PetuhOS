#ifndef IRQ_EXCEPTIONS_H_
#define IRQ_EXCEPTIONS_H_

/*
 * irq/exceptions.h
 * cpu exceptions and their handlers */

#define CPU_EXCEPT_PAGE_FAULT 14

#include <arch/irq/irq_dispatch.h>

void irq_exceptions_install();

#endif
