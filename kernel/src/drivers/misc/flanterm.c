#include <drivers/api.h>
#include <drivers/misc/fterm/backends/fb.h>
#include <drivers/misc/fterm/flanterm.h>
#include <limine_requests.h>
#include <mm/kern/slub.h>
#include <utils/errno.h>
#include <utils/logging.h>
#include <utils/memory.h>
#include <utils/printf.h>

#define ANSI_RESET "\033[0m"
#define ANSI_BOLD "\033[1m"

static inline void ft_free_wrapper(void *ptr, size_t size) {
  (void)size;
  kfree(ptr);
}

static struct flanterm_context *s_ftCtx = NULL;

struct flanterm_log_color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

static const struct flanterm_log_color s_logColors[8] = {
    [(L_TRACE)] = {0xb0, 0xb0, 0xb0},    [(L_NOTE)] = {0x8a, 0x8a, 0x8a},
    [(L_DEBUG)] = {0x6f, 0xc7, 0xd9},    [(L_INFO)] = {0x9f, 0xc4, 0x5f},
    [(L_WARN)] = {0xe0, 0xb4, 0x5c},     [(L_ERROR)] = {0xe0, 0x6c, 0x6c},
    [(L_CRITICAL)] = {0xff, 0x55, 0x55}, [(L_PRINTK)] = {0xff, 0xff, 0xff},
};

static void ft_write(const char *str) {
  if (!s_ftCtx)
    return;

  flanterm_write(s_ftCtx, str, strlen(str));
}

static void ft_write_color(const struct flanterm_log_color *color) {
  char buf[32];

  int len = snprintf_(buf, sizeof(buf), "\033[38;2;%u;%u;%um", color->r,
                      color->g, color->b);

  if (len > 0)
    flanterm_write(s_ftCtx, buf, (size_t)len);
}

static void ft_putchar(char c, void *a) {
  (void)a;

  if (!s_ftCtx)
    return;

  if (c == '\n') {
    const char *crlf = "\r\n";
    flanterm_write(s_ftCtx, crlf, 2);
    return;
  }

  flanterm_write(s_ftCtx, &c, 1);
}

static void ft_log_poller(u8 level, const char *fmt, va_list va) {
  if (!s_ftCtx)
    return;

  if (level != L_PRINTK) {
    const struct flanterm_log_color *color = &s_logColors[level];
    ft_write_color(color);
    ft_write(ANSI_BOLD);
    fctprintf(ft_putchar, NULL, "[ %s ] %s", logs_level_str(level), ANSI_RESET);
  }
  vfctprintf(ft_putchar, NULL, fmt, va);
  if (level != L_PRINTK)
    ft_putchar('\n', NULL);
}

static int flanterm_entry() {
  if (framebuffer_available()) {
    struct limine_framebuffer *fb =
        framebuffer_request.response->framebuffers[0];

    s_ftCtx = flanterm_fb_init(
        kmalloc, ft_free_wrapper, fb->address, fb->width, fb->height, fb->pitch,
        fb->red_mask_size, fb->red_mask_shift, fb->green_mask_size,
        fb->green_mask_shift, fb->blue_mask_size, fb->blue_mask_shift, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, 0, 0, 1, 0, 0, 0, 0, true);
    if (unlikely(!s_ftCtx)) {
      pr_error("[flanterm] failed to create flanterm context");
      return -ENOENT;
    }

    logs_add_handler(ft_log_poller);
  }

  return 0;
}

REGISTER_DRIVER(flanterm_driver, "flanterm",
                "handles logs and polls them to flanterm", "1.0.0",
                DRVLICENSE_DEFAULT, DRV_INIT_ORDER_NORMAL, flanterm_entry,
                NULL);
