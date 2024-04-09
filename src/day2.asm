;  day2.asm  Advent of Code Day 2
;
; Assemble: nasm -f elf64 day2.asm
; Link:     ld -o aoc day2.o ...

%include "defines.inc"

BITS 64

%define PART1_MAX_RED   12
%define PART1_MAX_GREEN 13
%define PART1_MAX_BLUE  14

section .rodata
    header: db "----- DAY 2 -----", 10, 0
    header_len: equ $ - header

    string_red:   db "red",   0
    string_green: db "green", 0
    string_blue:  db "blue",  0

    parsing_failed: db "Input parsing failed!", 0
    parsing_failed_len: equ $ - parsing_failed

    result_part1: db "Part 1 result: ", 0
    result1_len:  equ $ - result_part1
    result_part2: db "Part 2 result: ", 0
    result2_len:  equ $ - result_part2

    ; TODO: Parsing error messages


section .text
    global solve_day2

    extern print_number
    extern print_newline
    extern string_starts_with
    extern max


; Does what it says.
; Input:
;     rdi - Pointer to the input buffer.
; Output:
;     rax - Computed result for part 1.
;     rdx - Computed result for part 2.
solve_both_parts:
    push r15
    push r14
    push r13
    push r12
    push rbx
    push rbp

    mov rbp, rsp
    sub rsp, 5 * 8

    %define computed_value  rsp + 0 * 8
    %define computed_value2 rsp + 1 * 8
    %define max_red         rsp + 2 * 8
    %define max_green       rsp + 3 * 8
    %define max_blue        rsp + 4 * 8
    %define game_invalid    rsp + 5 * 8

    ; Input buffer
    mov r15, rdi

    ; String iteration index
    xor r14, r14

    ; Game / Line counter
    xor r13, r13

    ; Previous string iteration index
    ; (Only used to point out where exactly the parsing error occurred)
    ; mov r12, 0

    ; Computed values for day 2
    mov QWORD [computed_value],  0
    mov QWORD [computed_value2], 0

    ; Parse one game
    xor rax, rax


    ; 1. Parse game start (by finding the ':' character)
    ; 2. Skip space characters
    ; 3. Parse the color number
    ; 4. Skip space characters
    ; 5. Parse the color name
    ; 6. Skip space characters
    ; 7. Parse seperator:
    ;     - ';'  - goto at 3
    ;     - ','  - goto at 3
    ;     - '\n' - goto at 1

.parse_next_game:
    mov QWORD [game_invalid], 0

    mov QWORD [max_red],   0
    mov QWORD [max_green], 0
    mov QWORD [max_blue],  0

    inc r13

; --------- Parsing the game start
.parse_game_start:
    mov al, [r15 + r14]
    inc r14

    cmp al, 0
    je .end_of_input
    cmp al, ':'
    jne .parse_game_start


.parse_next_color:
; --------- Skip spaces
.skip_spaces:
    mov al, [r15 + r14]
    inc r14
    cmp al, 0
    je .end_of_input
    cmp al, ' '
    je .skip_spaces

    ; Go one step back
    dec r14


; --------- Parse number
    ; Parsed number
    mov r12, 0
.parse_number:
    mov al, [r15 + r14]
    inc r14
    cmp al, 0
    je .end_of_input
    cmp al, ' '
    je .number_parsed
    cmp al, '0'
    jl .something_went_wrong
    cmp al, '9'
    jg .something_went_wrong

    imul r12, 10
    sub rax, 48
    add r12, rax
    jmp .parse_number
.number_parsed:

    ; Go one step back
    dec r14


; --------- Skip spaces
.skip_spaces2:
    mov al, [r15 + r14]
    inc r14
    cmp al, 0
    je .end_of_input
    cmp al, ' '
    je .skip_spaces2

    ; Go one step back
    dec r14


; --------- Parse string color
    lea rdi, [r15 + r14]
    mov rsi, string_red
    call string_starts_with
    cmp rax, 0
    jne .red_color_found

    lea rdi, [r15 + r14]
    mov rsi, string_green
    call string_starts_with
    cmp rax, 0
    jne .green_color_found

    lea rdi, [r15 + r14]
    mov rsi, string_blue
    call string_starts_with
    cmp rax, 0
    jne .blue_color_found

    jmp .something_went_wrong

.red_color_found:
    mov rdi, r12
    mov rsi, [max_red]
    call max
    mov [max_red], rax

    add r14, 3

    cmp r12, PART1_MAX_RED
    jg .mark_invalid
    jmp .color_name_parsed

.green_color_found:
    mov rdi, r12
    mov rsi, [max_green]
    call max
    mov [max_green], rax

    add r14, 5

    cmp r12, PART1_MAX_GREEN
    jg .mark_invalid
    jmp .color_name_parsed

.blue_color_found:
    mov rdi, r12
    mov rsi, [max_blue]
    call max
    mov [max_blue], rax

    add r14, 4

    cmp r12, PART1_MAX_BLUE
    jg .mark_invalid
    jmp .color_name_parsed

.mark_invalid:
    mov QWORD [game_invalid], 1

.color_name_parsed:


; --------- Skip spaces
.skip_spaces3:
    mov al, [r15 + r14]
    inc r14
    cmp al, 0
    je .end_of_input
    cmp al, ' '
    je .skip_spaces3

    ; Go one step back
    dec r14


; --------- Parse seperator
    mov al, [r15 + r14]
    inc r14

    cmp al, 0
    je .end_of_input
    cmp al, ';'
    je .parse_next_color
    cmp al, ','
    je .parse_next_color
    cmp al, 10
    je .finish_current_game

    jmp .something_went_wrong

.end_of_input:
    mov rbx, [max_red]
    imul rbx, [max_green]
    imul rbx, [max_blue]

    add [computed_value2], rbx

    mov rax, [computed_value]
    mov rdx, [computed_value2]
    jmp .return_result

.finish_current_game:
    mov rbx,  [max_red]
    imul rbx, [max_green]
    imul rbx, [max_blue]

    mov rsi, [computed_value2]
    add rsi, rbx
    mov [computed_value2], rsi

    mov rax, [game_invalid]
    cmp rax, 1
    je .parse_next_game

    add [computed_value], r13
    jmp .parse_next_game

.something_went_wrong:
    mov rax, SYS_WRITE
    mov rdi, STDERR
    mov rsi, parsing_failed
    mov rdx, parsing_failed_len
    syscall
    call print_newline

    xor rax, rax
    xor rdx, rdx

.return_result:
    mov rsp, rbp

    pop rbp
    pop rbx
    pop r12
    pop r13
    pop r14
    pop r15
    ret


; Does what it says.
; Input:
;     rdi - Pointer to the input buffer.
; Output:
;     None.
solve_day2:
    push rbx
    mov rbx, rdi

    call print_newline

    PRINT header, header_len

    mov rdi, rbx
    call solve_both_parts

    mov r12, rax
    mov r13, rdx

    PRINTLN_WITH_NUMBER result_part1, result1_len, r12
    PRINTLN_WITH_NUMBER result_part2, result2_len, r13
    
    pop rbx
    ret
