#include <arch/control.h>
#include <arch/irq/exceptions.h>
#include <arch/paging.h>
#include <arch/stack/alloc.h>
#include <arch/stack/enter.h>
#include <arch/stack/utils.h>
#include <arch/tables/gdt.h>
#include <arch/tables/ist.h>
#include <drivers/api.h>
#include <limine.h>
#include <limine_requests.h>
#include <mm/kern/slub.h>
#include <mm/physical/allocator.h>
#include <mm/virtual/page_table.h>
#include <mm/virtual/vma/vma.h>
#include <scheduler/smp/cpus.h>
#include <security/crypto/rng.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <utils/logging.h>
#include <utils/memory.h>
#include <utils/panic.h>
#include <utils/symbols.h>

void trigger_double_fault_via_idt(void) {
  struct {
    unsigned short limit;
    unsigned long base;
  } __attribute__((packed)) idtr;

  idtr.limit = 0;
  idtr.base = 0x0;

  asm volatile("lidt %0" : : "m"(idtr) : "memory");
  asm volatile("int $3");
}

void bsp_entry(void *arg) {
  (void)arg;
  pr_info("hello from bsp stack! at 0x%p", read_rsp());
  trigger_double_fault_via_idt();

  hcf();
}

void kmain(void) {
  symbols_init();

  DRV_FOREACH(drv) {
    if (drv->initialized || drv->init_order != DRV_INIT_ORDER_URGENT ||
        !drv->init)
      continue;

    drv->init();

    drv->initialized = true;
  }

  rng_init();
  cpus_init();
  pmem_init();

  page_table_t *pt = kernel_pt_init();
  pt_switch(pt);
  vma_init();

  kmem_init();
  kmem_dump_caches();

  DRV_FOREACH(drv) {
    if (drv->initialized || drv->init_order != DRV_INIT_ORDER_NORMAL ||
        !drv->init)
      continue;

    drv->init();

    drv->initialized = true;
  }

  cpu_t *bsp = current_cpu();
  uintptr_t kstack = stack_alloc_kern();

  bsp->kern_stack_top = kstack + (KERN_STACK_PAGES * PAGE_SIZE);
  for (size_t i = 0; i < IST_STACK_COUNT; ++i) {
    uintptr_t istack = stack_alloc_ist();

    bsp->ist_stack_tops[i] = istack + (IST_STACK_PAGES * PAGE_SIZE);
  }

  cpu_init(bsp);

  idt_init();
  idt_load();

  irq_dispatch_install();
  irq_exceptions_install();

  idt_load();

  stack_enter(bsp->kern_stack_top, bsp_entry, NULL);
  hcf();
}
