#ifndef CPU_H_
#define CPU_H_

#include <arch/msr.h>
#include <arch/tables/ist.h>
#include <limine_requests.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <utils/attributes.h>

#include <arch/tables/gdt.h>
#include <arch/tables/ist.h>
#include <arch/tables/tss.h>

/*
 * cpu.h
 * cpu local struct */

#define MAX_CORES 64

typedef struct cpu {
  tss_t tss;
  gdt_t gdt;
  ist_t ist;
  uintptr_t ist_stack_tops[IST_STACK_COUNT];
  u64 idx;
  u64 lapic_id;
  uintptr_t kern_stack_top;
} cpu_t;

void set_current_cpu(struct cpu *c);
void cpu_init(cpu_t *cpu);
struct cpu *current_cpu();
noreturn void ap_entry(struct limine_mp_info *info);

#endif
