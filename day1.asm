;  day1.asm  Advent of Code Day 1 
;
; Assemble: nasm -f elf64 day1.asm
; Link:     ld -o day1 day1.o

; Linux system calls:
;
; Call register:
;     rax
; Argument registers order:
;     rdi, rsi, rdx, r10, r8, r9
; Return registers:
;     rdx, rdx

; C calling conventions:
;
; Function paramater register order:
;     rdi, rsi, rdx, rcx, r8, r9
; Return registers:
;     rax, rdx
; Scratch registers:
;     rax, rdi, rsi, rdx, rcx, r8, r9, r10, r11
; Preserved registers:
;     rbx, rsp, rbp, r12, r13, r14, r15

BITS 64

; TODO: Split data, constants, functions and days into separate files?

; Passed to the rax register
%define SYS_READ  0
%define SYS_WRITE 1
%define SYS_OPEN  2
%define SYS_CLOSE 3
%define SYS_STAT  4
%define SYS_FSTAT 5
%define SYS_LSTAT 6
%define SYS_MMAP  9
%define SYS_BRK   12
%define SYS_EXIT  60

%define STDIN  0 
%define STDOUT 1
%define STDERR 2

%define EXIT_SUCCESS 0
%define EXIT_ERROR   1

; SYS_OPEN flags
%define O_PATH 10000000

; SYS_OPEN modes
%define O_RDONLY 0
%define O_WRONLY 1
%define O_RDWR   2

%define BREAKPOINT int3

; TODO: I'm going to need a bump or some other allocator, there is that...

global _start

section .bss
    string_buffer_inv: resb 32
    string_buffer:     resb 32
    string_len:        resb 2

    statbuf: resb stat_size
    statbuf_size: equ $-stat

section .data
    newline: db 10

    message: db "Hello, day 1!", 10
    msg_len: equ $-message

    debug_msg: db "DEBUG HIT", 10
    debug_len: equ $-debug_msg

    input_path: db "./inputs/kihau/day1.txt", 0
    input_len:  equ $-input_path

    solved_part1:     db "Part 1 result: ", 0
    solved_part1_len: equ $-solved_part1

    solved_part2:     db "Part 2 result: ", 0
    solved_part2_len: equ $-solved_part2

    string_one:   db "one",   0
    string_two:   db "two",   0
    string_three: db "three", 0
    string_four:  db "four",  0
    string_five:  db "five",  0
    string_six:   db "six",   0
    string_seven: db "seven", 0
    string_eight: db "eight", 0
    string_nine:  db "nine",  0

    digit_strings_array: dq                        \
        string_one,   string_two,   string_three,  \
        string_four,  string_five,  string_six,    \
        string_seven, string_eight, string_nine,
    digit_strings_len: equ $-digit_strings_array

    heap_ptr: dq 0

    struc timespec 
        .tv_sec: resb 16
        .tv_nsec: resb 16
    endstruc

    struc stat
        .st_dev:     resb 8
        .st_ino:     resb 8
        .st_nlink:   resb 8

        .st_mode:    resb 4
        .st_uid:     resb 4
        .st_gid:     resb 4
        .pad0:       resb 4

        .st_rdev:    resb 8
        .st_size:    resb 8
        .st_blksize: resb 8
        .st_blocks:  resb 8

        ; .st_atim:    equ $-timespec_size
        ; .st_mtim:    equ $-timespec_size
        ; .st_ctim:    equ $-timespec_size
        .st_atim:    resb 16
        .st_mtim:    resb 16
        .st_ctim:    resb 16

        .pad1:       resb 24
    endstruc


section .text

; Read an entire input file to string buffer.
; Input:
;     rax - File input path.
; Output:
;     rax - Null terminated input buffer. Set to 0 if the read failed.
;     rdx - Size of the input buffer.
read_to_string:
    ret

; Compare two strings and see if the first string starts with the seconds string.
; Input:
;     rax - pointer to string 1, day1 input buffer
;     rdi - pointer to string 2, the digit name 
; Output:
;     rax - 0 when no match, 1 when match was found.
substring_match:
    ; If we iterated to the end ('\0') of the string 2 that means that we found a match.
    ; If we either reached end of string 1 or we found new line character that means there is no match.

    mov r8, rax
    mov r9, rdi

    ; Iteration counter, used as string character lookup offset.
    mov r10, 0

.substring_loop:
    ; Clear for debugging purposes.
    mov rax, 0
    mov al, [r9 + r10]
    cmp al, 0
    je .strings_matching

    ; Clear for debugging purposes.
    mov rbx, 0
    mov bl, [r8 + r10]
    cmp bl, 0
    je .strings_not_matching
    cmp bl, 10
    je .strings_not_matching

    cmp al, bl
    jne .strings_not_matching

    inc r10
    jmp .substring_loop
    
.strings_not_matching:
    mov rax, 0
    ret

.strings_matching:
    mov rax, 1
    ret

; Map virtual memory pages.
; Input:
;     rax - Minimium number of bytes.
; Output:
;     rax - Pointer to the mapped page.
map_virtual_page:
    ; Should rcx be aligned to the page size?
    mov rcx, rax

    mov rax, SYS_MMAP  ; syscall code
    mov rdi, 0         ; address, 0 for "random" one
    mov rsi, rcx       ; number of bytes to allocate
    mov rdx, 3         ; protection level, here PROT_READ | PROT_WRITE
    mov r10, 34        ; flags, here MAP_PRIVATE | MAP_ANONYMOUS
    mov r8, -1         ; file descriptor
    mov r9,  0         ; offset
    syscall
    ret

; Allocates N number of bytes. Increases heap memory region size.
; Input:
;     rax - Number of bytes to allocate.
; Output:
;     rax - Pointer to allocated data.
bump_allocate:
    mov rsi, rax
    mov rdx, [heap_ptr]

    cmp rdx, 0
    jne .heap_initialized
    mov rax, SYS_BRK
    xor rdi, rdi
    syscall
    mov [heap_ptr], rax
.heap_initialized:

    mov rdx, [heap_ptr]
    add [heap_ptr], rsi

    mov rax, SYS_BRK
    mov rdi, [heap_ptr]
    syscall

    ; Check if the syscall failed.
    cmp rax, -1
    je .brk_failed
    mov rax, rdx
.brk_failed:
    ret

debug_hit_print:
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, debug_msg
    mov rdx, debug_len
    syscall
    ret

; Convert input number to a string.
; Input:
;     rax - The number to convert.
; Output:
;     string_buffer - Buffer with converted string.
;     string_len    - Length of the converted string.
number_to_string:
    ; Current buffer write offset
    mov r8, 0

    ; mov rdx, rax
    mov rcx, 10

.loop_convert:
    mov rdx, 0
    div rcx

    add rdx, 48
    mov [string_buffer_inv + r8], rdx
    inc r8

    cmp rax, 0
    jne .loop_convert

    ; Now we set up decrement register
    mov rax, r8

    ; Reset buffer write offset
    mov r8, 0

.loop_reverse:
    dec rax

    mov rdx, [string_buffer_inv + rax]
    mov [string_buffer + r8], rdx
    inc r8

    cmp rax, 0
    jne .loop_reverse

    ; null terminate the string
    mov rdx, 0
    mov [string_buffer + r8], rdx
    inc r8
    mov [string_len], r8
    ret

; Prints 64 bit number into STDOUT.
; Input:
;     rax - Number to be printed.
; Output:
;     none
print_number:
    call number_to_string
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, string_buffer
    mov rdx, [string_len]
    syscall
    ret

; Prints newline character into STDOUT.
; Input:
;     none
; Output:
;     none
print_newline:
    call number_to_string
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, newline
    mov rdx, 1
    syscall
    ret


; Does what it says.
; Input:
;     rax - Pointer to the input buffer.
; Output:
;     rax - Computed value.
solve_part1:
    ; Set up the stack
    add rsp, 24

    %define TOTAL_VALUE rsp + 8
    %define FIRST_DIGIT rsp + 16
    %define LAST_DIGIT  rsp + 24
    
    ; Clear out the uninitialized data
    mov QWORD [TOTAL_VALUE], 0
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT],  0

    ; r15 - Preserved input string character at index r14.
    ; r14 - Iteration counter, used for indexing the input string.
    ; r13 - Pointer to the input string.

    ; Pointer to input string
    mov r13, rax

    ; Iteration counter
    mov r14, 0

.loop_read_input:
    ; Preserve current character at index r14
    xor rax, rax
    mov al, [r13 + r14]
    mov r15, rax

    cmp r15, 0
    je .end_of_input

    cmp r15, 10
    jne .not_newline

    ; Compute the value for the current line
    mov rbx, [FIRST_DIGIT]
    mov rdi, [LAST_DIGIT]
    imul rbx, 10
    add rbx, rdi

    ; Add computed value to total value and clear the digits
    add [TOTAL_VALUE], rbx
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT], 0
.not_newline:

    ; Check if the character is an ASCII digit (codes 48-57 are digits)
    cmp r15, 48 
    jl .not_a_digit
    cmp r15, 57
    jg .not_a_digit
    mov rax, r15
    sub rax, 48
    mov [LAST_DIGIT], rax

    mov rbx, [FIRST_DIGIT]
    cmp rbx, 0
    jne .first_number_found
    mov [FIRST_DIGIT], rax
.first_number_found:
.not_a_digit:

    inc r14
    jmp .loop_read_input
.end_of_input:
    mov rax, QWORD[TOTAL_VALUE]

    ; Set up the stack
    sub rsp, 24
    ret


; Does what it says.
; Input:
;     rax - Pointer to the input buffer.
; Output:
;     rax - Computed value.
solve_part2:
    ; Set up the stack
    add rsp, 24

    %define TOTAL_VALUE rsp + 8
    %define FIRST_DIGIT rsp + 16
    %define LAST_DIGIT  rsp + 24
    
    ; Clear out the uninitialized data
    mov QWORD [TOTAL_VALUE], 0
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT],  0

    ; r15 - Preserved input string character at index r14.
    ; r14 - Iteration counter, used for indexing the input string.
    ; r13 - Pointer to the input string.

    ; Pointer to input string
    mov r13, rax

    ; Iteration counter
    mov r14, 0

.loop_read_input:
    ; Preserve current character at index r14
    xor rax, rax
    mov al, [r13 + r14]
    mov r15, rax

    cmp r15, 0
    je .end_of_input

    cmp r15, 10
    jne .not_newline

    ; Compute the value for the current line
    mov rbx, [FIRST_DIGIT]
    mov rdi, [LAST_DIGIT]
    imul rbx, 10
    add rbx, rdi

    ; Add computed value to total value and clear the digits
    add [TOTAL_VALUE], rbx
    mov QWORD [FIRST_DIGIT], 0
    mov QWORD [LAST_DIGIT], 0
.not_newline:

    ; Check if the character is an ASCII digit (codes 48-57 are digits)
    cmp r15, 48 
    jl .not_a_digit
    cmp r15, 57
    jg .not_a_digit
    mov rax, r15
    sub rax, 48
    mov [LAST_DIGIT], rax

    mov rbx, [FIRST_DIGIT]
    cmp rbx, 0
    jne .first_number_found
    mov [FIRST_DIGIT], rax
.first_number_found:
.not_a_digit:

    ; Iterate over the array of string pointers.
    ; mov r12, [digit_strings_len]
    mov r12, 0
.loop_digit_string_array:
    lea rax, [r13 + r14]
    mov rdi, [digit_strings_array + r12 * 8]
    inc r12

    call substring_match
    cmp rax, 0
    je .not_a_digit_string
    mov rax, r12
    mov QWORD [LAST_DIGIT], rax
    mov rbx, [FIRST_DIGIT]
    cmp rbx, 0
    jne .first_number_found2
    mov QWORD [FIRST_DIGIT], rax
.first_number_found2:
.not_a_digit_string:

    cmp r12, 9
    jne .loop_digit_string_array

    inc r14
    jmp .loop_read_input
.end_of_input:
    mov rax, QWORD[TOTAL_VALUE]

    ; Set up the stack
    sub rsp, 24
    ret


; Testing some stuff here
debug_testing:
    BREAKPOINT
    ; Bump allocator testing
    ; mov rax, 1024
    ; call bump_allocate
    ; mov rax, 1024
    ; call bump_allocate
    ; mov rax, 1024
    ; call bump_allocate

    ; Length testing
    ; mov rax, string_one
    ; call print_number
    ; call print_newline
    ;
    ; mov rax, [digit_strings_array]
    ; call print_number
    ; call print_newline
    ;
    ; mov rax, string_nine
    ; call print_number
    ; call print_newline
    ;
    ; mov rax, digit_strings_array
    ; call print_number
    ; call print_newline
    ;
    ; mov rax, digit_strings_len2
    ; call print_number
    ; call print_newline
    ;
    ; mov rax, solved_part1_len
    ; call print_number
    ; call print_newline
    ret

_start:
    ; Write some dummy welcome message.
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, message
    mov rdx, msg_len
    syscall

    ; Open the input file here.
    mov rax, SYS_OPEN
    mov rdi, input_path
    mov rsi, O_RDONLY
    syscall

    ; Storing the file descriptor to not overwrite it.
    mov r15, rax

    ; Get exact size of the input size (in bytes)
    mov rax, SYS_FSTAT
    mov rdi, r15
    mov rsi, statbuf ; I could put it on the stack
    syscall

    mov rax, [statbuf + stat.st_size]
    inc rax ; Add 1 to rax in order to fit string null terminator.
    call bump_allocate

    ; Save pointer to the heap.
    mov rbx, rax

    ; Now we read an entire input file into memory
    mov rax, SYS_READ
    mov rdi, r15
    mov rsi, rbx
    mov rdx, [statbuf + stat.st_size]
    syscall

    ; Should I null terminate the string or is this fine?

    ; TODO: Check if expected byte count was read from a file.

    ; All data is ready. Input file can be closed now.
    mov rax, SYS_CLOSE
    mov rdi, r15
    syscall


    push rbx

    ; PART 1
    mov rax, rbx
    call solve_part1
    mov rbx, rax

    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, solved_part1
    mov rdx, solved_part1_len
    syscall

    mov rax, rbx
    call print_number
    call print_newline

    pop rbx

    ; PART 2
    mov rax, rbx
    call solve_part2
    mov rbx, rax

    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, solved_part2
    mov rdx, solved_part2_len
    syscall

    mov rax, rbx
    call print_number
    call print_newline

    ; Exit syscall
    mov rax, SYS_EXIT
    mov rdi, EXIT_SUCCESS
    syscall
