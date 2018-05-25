#pragma once

#define KERNEL_CS (0x10)
#define KERNEL_DS (0x18)
#define USER_DS (0x23)
#define USER_CS (0x2B)
#define KERNEL_CPU (0x30)
#define TSS (0x40)

#define MSR_IA32_FS_BASE (0xC0000100)
#define MSR_IA32_GS_BASE (0xC0000101)

#ifndef ASM_FILE

#include "trampoline.h"
#include "x86.h"

extern "C" void lgdt_sub(virt_addr gdtr);

class Gdt {
 public:
  void SetupProc();
  static bool IsInitializedPerCpuStruct() {
    return x86::rdmsr(MSR_IA32_FS_BASE) != 0;
  }

 private:
  void lgdt(uint32_t *gdt_desc, int entry_num) {
    volatile uint32_t gdtr[3];
    gdtr[0] = (entry_num * 8 - 1) << 16;
    gdtr[1] = reinterpret_cast<virt_addr>(gdt_desc) & 0xFFFFFFFF;
    gdtr[2] = (reinterpret_cast<virt_addr>(gdt_desc) >> 32) & 0xFFFFFFFF;

    lgdt_sub(reinterpret_cast<virt_addr>(gdtr) + 2);
  }

  struct Tss {
    uint32_t reserved1;
    uint32_t rsp0l;
    uint32_t rsp0h;
    uint32_t rsp1l;
    uint32_t rsp1h;
    uint32_t rsp2l;
    uint32_t rsp2h;
    uint32_t reserved2;
    uint32_t reserved3;
    uint32_t ist1l;
    uint32_t ist1h;
    uint32_t ist2l;
    uint32_t ist2h;
    uint32_t ist3l;
    uint32_t ist3h;
    uint32_t ist4l;
    uint32_t ist4h;
    uint32_t ist5l;
    uint32_t ist5h;
    uint32_t ist6l;
    uint32_t ist6h;
    uint32_t ist7l;
    uint32_t ist7h;
    uint32_t reserved4;
    uint32_t reserved5;
    uint16_t reserved6;
    uint16_t iomap;
  } __attribute__((packed));
  static const int kGdtEntryNum = 10;
};
#endif  // ! ASM_FILE
