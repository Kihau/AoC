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

    ;extern scratch_run
    extern solve_day1
    extern solve_day2
    extern solve_day3
    extern solve_day4

    extern arena_create
    ;extern arena_destroy
    extern read_entire_file
    extern print_output
    extern exit_program

; TOOD (next commit): 
;   - Error handling and hardening for utils and os layer. 
;   - AOC2022 style main logic (days array, input agruments).
;   - Individual day hardening.
;   - Day 5

main:
    sub rsp, Arena_size
    mov r15, rsp

    ;call scratch_run

    mov rax, r15
    call arena_create

    mov rax, r15
    mov rbx, input1_path
    call read_entire_file
    mov rax, r15
    mov rbx, rdi
    mov rcx, rsi
    call solve_day1

    mov rax, r15
    mov rbx, input2_path
    call read_entire_file
    mov rax, r15
    mov rbx, rdi
    mov rcx, rsi
    call solve_day2

    mov rax, r15
    mov rbx, input3_path
    call read_entire_file 
    mov rax, r15
    mov rbx, rdi
    mov rcx, rsi
    call solve_day3

    mov rax, r15
    mov rbx, input4_path
    call read_entire_file 
    mov rax, r15
    mov rbx, rdi
    mov rcx, rsi
    call solve_day4

    mov rax, 0
    call exit_program
