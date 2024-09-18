;  main.asm  Program entry point

%include "utils.inc"

bits 64
default rel

section .rodata
    input1_path: db "./inputs/kihau/day1.txt", 0
    input2_path: db "./inputs/kihau/day2.txt", 0
    input3_path: db "./inputs/kihau/day3.txt", 0
    input4_path: db "./inputs/kihau/day4.txt", 0
    ;input4_path: db "./inputs/dummy/day4.txt", 0


section .text
    global main

    extern solve_day1
    extern solve_day2
    extern solve_day3
    extern solve_day4
    extern scratch_run

    extern arena_create
    ;extern arena_destroy
    extern read_entire_file
    extern print_output
    extern exit_program


main:
    push rbp
    mov rbp, rsp

    ; call scratch_run

    sub rsp, Arena_size
    mov r15, rsp

    mov rdi, r15
    call arena_create

    mov rdi, r15
    mov rsi, input1_path
    call read_entire_file
    mov rdi, rax
    mov rsi, rdx
    call solve_day1

    mov rdi, r15
    mov rsi, input2_path
    call read_entire_file
    mov rdi, rax
    mov rsi, rdx
    call solve_day2

    mov rdi, r15
    mov rsi, input3_path
    call read_entire_file 
    mov rdi, rax
    mov rsi, rdx
    call solve_day3

    mov rdi, r15
    mov rsi, input4_path
    call read_entire_file 
    mov rdi, rax
    mov rsi, rdx
    call solve_day4

    mov rdi, 0
    call exit_program

    mov rsp, rbp
    pop rbp
    ret
