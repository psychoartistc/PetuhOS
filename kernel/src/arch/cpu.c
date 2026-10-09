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

void cpu_init(cpu_t *cpu) {
  if (unlikely(!cpu))
    return;

  tss_init(&cpu->tss);
  tss_rzero_stack_set(&cpu->tss, cpu->kern_stack_top);

  for (size_t i = 0; i < IST_STACK_COUNT; ++i)
    ist_set_stack_top(&cpu->ist, i, cpu->ist_stack_tops[i]);

  ist_install(&cpu->ist, &cpu->tss);

  gdt_init(&cpu->gdt, (u64)&cpu->tss.e, sizeof(tss_entry_t) - 1);
  gdt_load(&cpu->gdt);
  tss_load(&cpu->tss, GDT_TSS_SELECTOR);
}
