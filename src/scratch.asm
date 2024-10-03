;  scratch.asm  Assembly testing playground

%include "utils.inc"

section .rodata
    result: db 10, "Result is: ", 0
    result_len: equ $ - result

    my_dummy_file: db "./dummyfile.txt", 0

bits 64
default rel

section .text
    global scratch_run

    extern open_file
    extern print_number
    extern print_newline
    extern exit_program

scratch_run:
    push rbp
    mov rbp, rsp

    mov rax, my_dummy_file
    call open_file
    mov rax, rdi
    call print_number
    call print_newline
    mov rax, 0
    call exit_program

    mov rsp, rbp
    pop rbp
    ret
