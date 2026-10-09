#ifndef GDT_H_
#define GDT_H_

/*
 * tables/gdt.h
 * global descriptor table
 * */

#include <utils/attributes.h>
#include <utils/int.h>

#define GDT_KERN_CODE_SELECTOR 0x08
#define GDT_KERN_DATA_SELECTOR 0x10
#define GDT_TSS_SELECTOR 0x18
#define GDT_TSS_INDEX 3

typedef struct gdt_entry {
  u16 limit_low;
  u16 base_low;
  u8 base_middle;
  u8 access;
  u8 granularity;
  u8 base_high;
} __PACKED gdt_entry_t;

typedef struct gdt_ptr {
  u16 limit;
  u64 base;
} __PACKED gdt_ptr_t;

#endif
