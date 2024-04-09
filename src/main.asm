%include "defines.inc"

BITS 64

section .data
    input1_path: db "./inputs/kihau/day1.txt", 0
    input2_path: db "./inputs/kihau/day2.txt", 0
    input3_path: db "./inputs/kihau/day3.txt", 0

section .text
    global _start

    extern read_to_string
    extern solve_day1
    extern solve_day2
    extern solve_day3
    extern scratch_run

    extern arena_create
    extern arena_destroy
    extern arena_clear

_start:
    sub rsp, arena_size
    mov r15, rsp

    mov rdi, r15
    call arena_create

    ; call scratch_run

    mov rdi, input1_path
    mov rsi, r15
    call read_to_string
    mov rdi, rax
    call solve_day1

    mov rdi, input2_path
    mov rsi, r15
    call read_to_string
    mov rdi, rax
    call solve_day2

    mov rdi, input3_path
    mov rsi, r15
    call read_to_string
    mov rdi, rax
    mov rsi, rdx
    call solve_day3

    ; Exit syscall
    mov rax, SYS_EXIT
    mov rdi, EXIT_SUCCESS
    syscall
