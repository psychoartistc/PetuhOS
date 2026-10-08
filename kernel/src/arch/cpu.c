#include <arch/control.h>
#include <arch/cpu.h>
#include <utils/branching.h>

#define MSR_GS_BASE 0xC0000101

void set_current_cpu(cpu_t *c) { wrmsr(MSR_GS_BASE, (u64)c); }

struct cpu *current_cpu() { return (struct cpu *)rdmsr(MSR_GS_BASE); }

noreturn void ap_entry(struct limine_mp_info *info) {
  (void)info;
  hcf();
  unreachable();
}
