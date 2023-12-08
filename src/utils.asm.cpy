;  utils.asm  System I/O and utility functions
;
; Assemble: nasm -f elf64 utils.asm
; Link:     ld -o aoc utils.o ...

%include "defines.inc"

BITS 64

section .bss
    string_buffer_inv: resb 32
    string_buffer:     resb 32
    string_len:        resb 2

    stat_buffer: resb stat_size


section .rodata
    newline_char: db 10
    debug_msg: db "DEBUG HIT", 10
    debug_len: equ $-debug_msg
    

section .data
    heap_ptr: dq 0


section .text
    global print_number
    global print_newline
    global read_to_string
    global string_starts_with
    global max


; Compare two strings and see if the first string starts with the seconds string.
; Input:
;     rax - Pointer to string 1.
;     rdi - Pointer to string 2.
; Output:
;     rax - 0 when no match, 1 when match was found.
string_starts_with:
    ; If we iterated to the end ('\0') of the string 2, that means that we found a match.
    ; If we iterated to the end ('\0') of the string 1, that means that there is no match.
    ; If any of the compared characters don't match, that means that there is no match.

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


; Read an entire input file to string buffer.
; Input:
;     rax - File input path.
; Output:
;     rax - Null terminated input buffer. Set to 0 if the read failed.
read_to_string:
    ; Preserve
    push r12
    push r13

    mov r12, rax

    ; Open the input file here.
    mov rax, SYS_OPEN
    mov rdi, r12
    mov rsi, O_RDONLY
    syscall

    ; Storing the file descriptor to not overwrite it.
    mov r12, rax

    ; Get exact size of the input size (in bytes)
    mov rax, SYS_FSTAT
    mov rdi, r12
    mov rsi, stat_buffer ; I could put it on the stack
    syscall

    mov rax, [stat_buffer + stat.st_size]
    inc rax ; Add 1 to rax in order to fit string null terminator.
    call bump_allocate

    ; Save pointer to the heap.
    mov r13, rax

    ; Now we read an entire input file into memory
    mov rax, SYS_READ
    mov rdi, r12
    mov rsi, r13
    mov rdx, [stat_buffer + stat.st_size]
    syscall

    ; Should I null terminate the string or is this fine?

    ; TODO: Check if expected byte count was read from a file.

    ; All data is ready. Input file can be closed now.
    mov rax, SYS_CLOSE
    mov rdi, r12
    syscall

    mov rax, r13

    pop r13
    pop r12
    ret


; Compare two number and return bigger one.
; Input:
;     rax - First number
;     rdi - Second number
; Output:
;     rax - Bigger number
max:
    cmp rax, rdi
    jl .return_rdi
    ret
.return_rdi
    mov rax, rdi
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


; Reset the allocated data.
; Input:
;     None
; Output:
;     None
bump_reset:
    mov QWORD [heap_ptr], 0
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
    mov rsi, newline_char
    mov rdx, 1
    syscall
    ret


; Testing some stuff here
debug_testing:
    BREAKPOINT
    ret
