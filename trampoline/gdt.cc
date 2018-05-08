#include "gdt.h"
#include "memory.h"


void Gdt::SetupProc() {
  uint32_t *gdt_desc = reinterpret_cast<uint32_t *>(MemoryMap::kGdt);
  Tss *tss = reinterpret_cast<Tss *>(MemoryMap::kTss);

  //TODO: impl assert
  // assert(KERNEL_CS / 8 + 1 <= Gdt::kGdtEntryNum);
  // assert(KERNEL_DS / 8 + 1 <= Gdt::kGdtEntryNum);
  // assert(USER_CS / 8 + 1 <= Gdt::kGdtEntryNum);
  // assert(USER_DS / 8 + 1 <= Gdt::kGdtEntryNum);
  // assert(KERNEL_CPU / 8 + 1 <= Gdt::kGdtEntryNum);
  // assert(TSS / 8 + 2 <= Gdt::kGdtEntryNum);

  for (int i = 0; i < kGdtEntryNum * 2; i++) {
    gdt_desc[i] = 0;
  }

  gdt_desc[(KERNEL_CS / sizeof(uint32_t))] = 0x00000000;
  gdt_desc[(KERNEL_CS / sizeof(uint32_t)) + 1] = 0x00209a00;
  gdt_desc[(KERNEL_DS / sizeof(uint32_t))] = 0x00000000;
  gdt_desc[(KERNEL_DS / sizeof(uint32_t)) + 1] = 0x00009200;
  gdt_desc[(USER_DS / sizeof(uint32_t))] = 0x00000000;
  gdt_desc[(USER_DS / sizeof(uint32_t)) + 1] = 0x0000F200;
  gdt_desc[(USER_CS / sizeof(uint32_t))] = 0x00000000;
  gdt_desc[(USER_CS / sizeof(uint32_t)) + 1] = 0x0020FA00;
  gdt_desc[(KERNEL_CPU / sizeof(uint32_t))] = 0;
  gdt_desc[(KERNEL_CPU / sizeof(uint32_t)) + 1] = 0;
  gdt_desc[(TSS / sizeof(uint32_t))] =
      MASK((sizeof(Tss) - 1), 15, 0) | (MASK(static_cast<uint64_t>(MemoryMap::kTss), 16, 0) << 16);
  gdt_desc[(TSS / sizeof(uint32_t)) + 1] =
      (MASK(static_cast<uint64_t>(MemoryMap::kTss), 23, 16) >> 16) | (9 << 8) | (3 << 13) | (1 << 15) |
      MASK((sizeof(Tss) - 1), 19, 16) | MASK(static_cast<uint64_t>(MemoryMap::kTss), 31, 24);
  gdt_desc[(TSS / sizeof(uint32_t)) + 2] = static_cast<uint64_t>(MemoryMap::kTss) >> 32;
  gdt_desc[(TSS / sizeof(uint32_t)) + 3] = 0;



  virt_addr rsp0 = static_cast<virt_addr>(MemoryMap::kIst1);
  tss->rsp0l = rsp0;
  tss->rsp0h = rsp0 >> 32;

  virt_addr rsp1 = static_cast<virt_addr>(MemoryMap::kIst1);
  tss->rsp1l = rsp1;
  tss->rsp1h = rsp1 >> 32;

  virt_addr rsp2 = static_cast<virt_addr>(MemoryMap::kIst1);
  tss->rsp2l = rsp2;
  tss->rsp2h = rsp2 >> 32;

  virt_addr dfstack = static_cast<virt_addr>(MemoryMap::kIst2);
  tss->ist1l = dfstack;
  tss->ist1h = dfstack >> 32;

  virt_addr nmistack = static_cast<virt_addr>(MemoryMap::kIst3);
  tss->ist2l = nmistack;
  tss->ist2h = nmistack >> 32;

  virt_addr debugstack = static_cast<virt_addr>(MemoryMap::kIst4);
  tss->ist3l = debugstack;
  tss->ist3h = debugstack >> 32;

  virt_addr mcestack = static_cast<virt_addr>(MemoryMap::kIst5);
  tss->ist4l = mcestack;
  tss->ist4h = mcestack >> 32;

  virt_addr genstack = static_cast<virt_addr>(MemoryMap::kIst6);
  tss->ist5l = genstack;
  tss->ist5h = genstack >> 32;

  tss->iomap = sizeof(Tss);  // no I/O permission map

  lgdt(gdt_desc, kGdtEntryNum);
  asm volatile("ltr %0;" ::"r"((uint16_t)TSS));
  // x86::wrmsr(MSR_IA32_FS_BASE, context_info_vaddr);
}
