%include "defines.inc"

BITS 64

section .data
    input1_path: db "./inputs/kihau/day1.txt", 0
    input1_len:  equ $-input1_path

    input2_path: db "./inputs/dummy/day2.txt", 0
    input2_len:  equ $-input2_path

section .text
    global _start

    extern solve_day1
    extern solve_day2
    extern read_to_string

_start:
    mov rdi, input1_path
    call read_to_string
    mov rdi, rax
    call solve_day1

    mov rdi, input2_path
    call read_to_string
    mov rdi, rax
    call solve_day2

    ; Exit syscall
    mov rax, SYS_EXIT
    mov rdi, EXIT_SUCCESS
    syscall
