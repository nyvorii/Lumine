MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
FLAGS    equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
dd MAGIC
dd FLAGS
dd CHECKSUM

section .bss
align 4096
pml4_table: resb 4096
pdpt_table: resb 4096
pd_table:   resb 4096

stack_bottom:
resb 16384 ; 16 KB na stos (pamięć podręczną)
stack_top:

section .text
global _start
extern kernel_main

_start:
  mov esp, stack_top  
  call kernel_main    
  cli
.hang:
  hlt                 ; Zatrzymanie procesora, gdyby system z jakiegoś powodu z niego wyszedł
  jmp .hang