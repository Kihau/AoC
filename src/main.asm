%include "defines.inc"

BITS 64

section .data
    input1_path: db "./inputs/kihau/day1.txt", 0
    input1_len:  equ $-input1_path

section .text
    global _start

    extern solve_day1
    extern read_to_string

_start:
    mov rax, input1_path
    call read_to_string
    call solve_day1

    ; Exit syscall
    mov rax, SYS_EXIT
    mov rdi, EXIT_SUCCESS
    syscall
