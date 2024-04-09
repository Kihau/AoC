;  scratch.asm  
;  Assembly testing playground

%include "defines.inc"

section .rodata
    result: db 10, "Result is: ", 0
    result_len: equ $ - result

BITS 64

section .text
    global scratch_run

    extern number_to_string
    extern print_number
    extern print_newline

scratch_run:
    push rbp
    mov rbp, rsp
    sub rsp, number_string_size

    PRINT result, result_len

    mov rdi, 10
    mov rsi, rsp
    call number_to_string
    lea r8, [rsp + number_string.buffer]
    mov r9b, [rsp + number_string.len]
    PRINTLN r8, r9

.trap:
    jmp .trap

    mov rsp, rbp
    pop rbp
    ret
