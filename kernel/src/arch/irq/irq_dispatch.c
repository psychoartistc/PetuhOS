#include <arch/irq/irq_dispatch.h>
#include <stddef.h>
#include <utils/logging.h>

extern u64 g_isrStubTable[IDT_ENTRY_COUNT];

static idt_handler_t s_entries[IDT_ENTRY_COUNT] = {0};
static void *s_args[IDT_ENTRY_COUNT] = {0};

void irq_dispatch_install() {
  for (size_t i = 0; i < IDT_ENTRY_COUNT; i++) {
    int r =
        idt_set_gate((u8)i, (idt_handler_t)g_isrStubTable[i], 0x08, 0, 0x8E);
    if (r < 0) {
      pr_error("[irq_dispatch] error %d when installing isrs", r);
      continue;
    }
  }
}

void irq_register(u8 vec, irq_handler_t handler, void *arg) {
  s_entries[vec] = handler;
  s_args[vec] = arg;
}

void irq_unregister(u8 at) {
  s_entries[at] = NULL;
  s_args[at] = NULL;
}

void irq_dispatch(interrupt_frame_t *frame) {
  u64 num = frame->int_no;
  if (num >= IDT_ENTRY_COUNT)
    return;

  irq_handler_t handler = s_entries[num];
  void *arg = s_args[num];

  if (handler)
    handler(frame, arg);
}
