#include <arch/control.h>
#include <arch/register_dump.h>
#include <arch/stack_trace.h>
#include <stdarg.h>
#include <utils/branching.h>
#include <utils/logging.h>
#include <utils/panic.h>
#include <utils/rdfile.h>

noreturn void vpanic(const char *fmt, va_list args) {
  irq_disable();

  pr_file("noooo.ascii");
  printk("\nkernel panic\n");
  printk("error: ");
  vprintk(fmt, args);
  printk("\n");
  register_dump();
  printk("\n");

  stack_trace();

  hcf();
  unreachable();
}

noreturn void panic(const char *fmt, ...) {
  irq_disable();
  va_list va;
  va_start(va, fmt);

  vpanic(fmt, va);
  va_end(va);
}
