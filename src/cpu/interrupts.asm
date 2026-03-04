bits 64



global dummy_isr
global keyboard_isr
global timer_isr

extern keyboard_handler
extern timer_handler
dummy_isr:
    iretq

keyboard_isr:
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    cld    

    call keyboard_handler

    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax

    iretq

timer_isr:
    push rax
    push rcx
    push rdx
    push r8
    push r9
    push r10
    push r11

    call timer_handler

    pop r11
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rcx
    pop rax
    iretq