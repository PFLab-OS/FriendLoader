
  #define ASM_FILE
  #include "gdt.h"
  .global lgdt_sub

lgdt_sub:
  pushq $KERNEL_CS
  pushq $jmp                    
  lgdt (%rdi)                   
  lretq                         
jmp:
  movw $KERNEL_DS, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %ss
  movw $0, %ax
  movw %ax, %fs
  movw %ax, %gs
  ret
