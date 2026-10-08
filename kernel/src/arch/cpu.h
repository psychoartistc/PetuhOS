#ifndef CPU_H_
#define CPU_H_

#include <arch/msr.h>
#include <limine_requests.h>
#include <stdint.h>
#include <stdnoreturn.h>
#include <utils/attributes.h>

/*
 * cpu.h
 * cpu local struct */

#define MAX_CORES 64

typedef struct cpu {
  u64 idx;
  u64 lapic_id;
} cpu_t;

void set_current_cpu(struct cpu *c);
struct cpu *current_cpu();
noreturn void ap_entry(struct limine_mp_info *info);

#endif
