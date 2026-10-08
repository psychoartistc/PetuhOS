#include <arch/control.h>
#include <arch/irq/exceptions.h>
#include <arch/paging.h>
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

  void *a = kzalloc(1028);
  pr_info("0x%p", a);

  idt_init();
  irq_dispatch_install();
  irq_exceptions_install();
  idt_load();
  irq_enable();

  asm volatile("ud2");
  hcf();
}
