; --- NAGŁÓWEK MULTIBOOT ---
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

; --- PAMIĘĆ NA STOS I TABLICE STRONICOWANIA ---
section .bss
align 4096
pml4_table: resb 4096
pdpt_table: resb 4096
pd_table:   resb 4096

stack_bottom:
resb 16384 ; 16 KB na stos
stack_top:

; --- GDT (Global Descriptor Table) ---
section .rodata
gdt64:
    dq 0 ; Zero entry
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
.data: equ $ - gdt64
    dq (1<<44) | (1<<47)
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

; --- KOD 32-BITOWY ---
section .text
bits 32
global _start
extern kernel_main

_start:
    ; 1. Stos
    mov esp, stack_top  

    ; 2. ZEROWANIE TABLIC STRONICOWANIA (To naprawia Triple Fault z powodu śmieci!)
    mov edi, pml4_table
    mov ecx, 3072       ; 3 tablice * 4096 bajtów = 12288 bajtów (czyli 3072 bloków po 4 bajty)
    xor eax, eax        ; Zerujemy rejestr eax
    rep stosd           ; Wypełnia pamięć zerami

    ; 3. Konfiguracja Stronicowania
    mov eax, pdpt_table
    or eax, 0b11
    mov [pml4_table], eax

    mov eax, pd_table
    or eax, 0b11
    mov [pdpt_table], eax

    ; 4. Mapowanie pierwszych 10 Megabajtów pamięci (5 stron po 2MB)
    ; To upewnia nas, że procesor się nie zgubi!
    mov ecx, 0
    mov eax, 0b10000011 ; Flagi: Present, Writable, Huge Page
.map_loop:
    mov [pd_table + ecx * 8], eax ; *8 bo każdy wpis zajmuje teraz 8 bajtów!
    add eax, 0x200000             ; Kolejna strona (+ 2MB)
    inc ecx
    cmp ecx, 5                    ; Mapujemy 5 stron (razem 10 MB)
    jne .map_loop

    ; 5. Ładowanie PML4 do CR3
    mov eax, pml4_table
    mov cr3, eax

    ; 6. Włączenie PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 7. Włączenie Long Mode (LME)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 8. Włączenie stronicowania (Teraz to nie wybuchnie!)
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; 9. Ładowanie GDT
    lgdt [gdt64.pointer]

    ; 10. Skok do 64 bitów
    jmp gdt64.code:long_mode_start

; --- KOD 64-BITOWY ---
bits 64
long_mode_start:
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call kernel_main
    
    cli
.hang:
    hlt
    jmp .hang