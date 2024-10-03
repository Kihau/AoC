;  day2.asm  Advent of Code Day 2
;
; Assemble: nasm -f elf64 day2.asm
; Link:     ld -o aoc day2.o ...

%include "utils.inc"

bits 64
default rel

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

    extern print_output
    extern print_error
    extern print_number
    extern print_newline
    extern string_starts_with
    extern max


; Solves both parts of AoC day 2.
; Parameters:
;     [in] u8* - Pointer to the input buffer.
; Results:
;     [out] u64 - Part 1 solution.
;     [out] u64 - Part 2 solution.
solve_both_parts:
    push rbp
    push r15
    push r14
    push r13
    push r12
    push r11

    mov rbp, rsp
    sub rsp, 6 * 8

    %define computed_value  rsp + 0 * 8
    %define computed_value2 rsp + 1 * 8
    %define max_red         rsp + 2 * 8
    %define max_green       rsp + 3 * 8
    %define max_blue        rsp + 4 * 8
    %define game_invalid    rsp + 5 * 8

    ; Input buffer
    mov r15, rax

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
    lea rax, [r15 + r14]
    mov rbx, string_red
    call string_starts_with
    cmp rdi, 0
    jne .red_color_found

    lea rax, [r15 + r14]
    mov rbx, string_green
    call string_starts_with
    cmp rdi, 0
    jne .green_color_found

    lea rax, [r15 + r14]
    mov rbx, string_blue
    call string_starts_with
    cmp rdi, 0
    jne .blue_color_found

    jmp .something_went_wrong

.red_color_found:
    mov rax, r12
    mov rbx, [max_red]
    call max
    mov [max_red], rdi

    add r14, 3

    cmp r12, PART1_MAX_RED
    jg .mark_invalid
    jmp .color_name_parsed

.green_color_found:
    mov rax, r12
    mov rbx, [max_green]
    call max
    mov [max_green], rdi

    add r14, 5

    cmp r12, PART1_MAX_GREEN
    jg .mark_invalid
    jmp .color_name_parsed

.blue_color_found:
    mov rax, r12
    mov rbx, [max_blue]
    call max
    mov [max_blue], rdi

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
    mov r11, [max_red]
    imul r11, [max_green]
    imul r11, [max_blue]

    add [computed_value2], r11

    mov rdi, [computed_value]
    mov rsi, [computed_value2]
    jmp .return_result

.finish_current_game:
    mov r11,  [max_red]
    imul r11, [max_green]
    imul r11, [max_blue]

    mov rsi, [computed_value2]
    add rsi, r11
    mov [computed_value2], rsi

    mov rax, [game_invalid]
    cmp rax, 1
    je .parse_next_game

    add [computed_value], r13
    jmp .parse_next_game

.something_went_wrong:
    mov rax, parsing_failed
    mov rbx, parsing_failed_len
    call print_error
    call print_newline

    xor rdi, rdi
    xor rsi, rsi

.return_result:
    mov rsp, rbp

    pop r11
    pop r12
    pop r13
    pop r14
    pop r15
    pop rbp
    ret


; Solves AoC day 2.
; Parameters:
;     [in] Arena* - Pointer to the arena allocator.
;     [in] u8*    - Pointer to the input buffer.
;     [in] u64    - Input buffer size.
; Results:
;     TODO: [out] u64 - Part 1 solution.
;     TODO: [out] u64 - Part 2 solution.
solve_day2:
    push r15
    push r14

    mov rax, rbx
    call solve_both_parts
    mov r15, rdi
    mov r14, rsi

    call print_newline
    mov rax, header
    mov rbx, header_len
    call print_output

    mov rax, result_part1 
    mov rbx, result1_len
    call print_output
    mov rax, r15
    call print_number
    call print_newline

    mov rax, result_part2 
    mov rbx, result2_len
    call print_output
    mov rax, r14
    call print_number
    call print_newline
    
    pop r14
    pop r15
    ret
