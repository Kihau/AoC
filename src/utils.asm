;  utils.asm  System I/O and utility functions
;
; Assemble: nasm -f elf64 utils.asm
; Link:     ld -o aoc utils.o ...

%include "defines.inc"

BITS 64

section .bss
    ; NOTE: Won't work in a multithreaded context.
    ;       This is a reminder to change it when I decide run things in parallel.
    ; string_buffer_inv: resb 32
    ; string_buffer:     resb 32
    ; string_len:        resb 2

    ; NOTE: Won't work in a multithreaded context.
    ;       This is a reminder to change it when I decide run things in parallel.
    ; stat_buffer: resb stat_size


section .rodata
    newline_char: db 10
    debug_msg: db "DEBUG HIT", 10
    debug_len: equ $ - debug_msg
    

section .data
    ; NOTE: Won't work in a multithreaded context. Needs to be behind a lock.
    ;       This is a reminder to change it when I decide run things in parallel.
    heap_ptr: dq 0


section .text
    global print_number
    global print_newline
    global read_to_string
    global string_starts_with
    global number_to_string
    ; global number_to_string_old
    global arena_create
    global arena_destroy
    global arena_reset
    global arena_allocate
    global arena_clear
    global max


; Compare two strings and see if the first string starts with the seconds string.
; Input:
;     rdi - Pointer to string 1.
;     rsi - Pointer to string 2.
; Output:
;     rax - 0 when no match, 1 when match was found.
string_starts_with:
    ; If we iterated to the end ('\0') of the string 2, that means that we found a match.
    ; If we iterated to the end ('\0') of the string 1, that means that there is no match.
    ; If any of the compared characters don't match, that means that there is no match.

    mov r8, rdi
    mov r9, rsi

    ; Iteration counter, used as string character lookup offset.
    mov r10, 0

.substring_loop:
    ; Clear for debugging purposes.
    mov rax, 0
    mov al, [r9 + r10]
    cmp al, 0
    je .strings_matching

    ; Clear for debugging purposes.
    mov rdx, 0
    mov dl, [r8 + r10]
    cmp dl, 0
    je .strings_not_matching

    cmp al, dl
    jne .strings_not_matching

    inc r10
    jmp .substring_loop
    
.strings_not_matching:
    mov rax, 0
    ret

.strings_matching:
    mov rax, 1
    ret


; Read an entire input file to a string buffer.
; Input:
;     rdi - File input path.
;     rsi - Arena allocator
; Output:
;     rax - Null terminated input buffer. Set to 0 if the read failed.
;     rdx - Size of the input buffer.
read_to_string:
    ; Preserve
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15

    mov rbp, rsp
    sub rsp, stat_size

    mov r12, rdi
    mov rbx, rsi

    ; TODO: This can fail, handle the error.
    ; Open the input file here.
    mov rax, SYS_OPEN
    mov rdi, r12
    mov rsi, O_RDONLY
    syscall

    ; Storing the file descriptor to not overwrite it.
    mov r12, rax

    ; Get exact size of the input size (in bytes).
    mov rax, SYS_FSTAT
    mov rdi, r12
    mov rsi, rsp
    syscall

    ; Store size of the input.
    mov r14, [rsp + stat.st_size]
    inc r14 ; Add 1 in order to fit string null terminator.

    ; mov rdi, r14
    ; call bump_allocate
    mov rdi, rbx
    mov rsi, r14
    call arena_allocate

    ; Save pointer to the allocated data.
    mov r13, rax

    ; Now we read an entire input file into memory.
    mov rax, SYS_READ
    mov rdi, r12
    mov rsi, r13
    mov rdx, [rsp + stat.st_size]
    syscall

    ; Preserve byte count read.
    mov r15, rax

    ; All data is ready. Input file can be closed now.
    mov rax, SYS_CLOSE
    mov rdi, r12
    syscall

    ; Check if expected byte count was read from a file.
    cmp [rsp + stat.st_size], r15
    je .byte_count_matches
    xor rax, rax
    xor rdx, rdx
    jmp .return_result
.byte_count_matches:

    ; Null terminate the input buffer.
    mov rax, r14
    dec rax
    mov BYTE [r13 + rax], 0

    mov rax, r13
    mov rdx, r14

.return_result:
    mov rsp, rbp
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret


; Compare two number and return bigger one.
; Input:
;     rdi - First number.
;     rsi - Second number.
; Output:
;     rax - Bigger number.
max:
    cmp rdi, rsi
    jl .second_larger
    mov rax, rdi
    ret
.second_larger:
    mov rax, rsi
    ret


; Map virtual memory pages.
; Input:
;     rdi - Minimium number of bytes.
; Output:
;     rax - Pointer to the mapped page.
map_virtual_pages:
    ; Should rcx be aligned to the page size?
    mov rcx, rdi

    mov rax, SYS_MMAP                     ; syscall code
    mov rdi, 0                            ; address, 0 for "random" one
    mov rsi, rcx                          ; number of bytes to allocate
    mov rdx, PROT_READ | PROT_WRITE       ; protection level
    mov r10, MAP_PRIVATE | MAP_ANONYMOUS  ; flags
    mov r8, -1                            ; file descriptor
    mov r9,  0                            ; offset
    syscall

    ; TODO: Check if mmap failed?

    ret


; Unmap virtual memory pages.
; Input:
;     rdi - Pointer to mapped virtual memory.
;     rdi - Size of the mapped memory.
; Output:
;     None.
unmap_virtual_pages:
    mov rax, SYS_MUNMAP
    ; mov rdi, rdi
    ; mov rsi, rsi
    syscall
    ret


; Create new arena allocator.
; Input:
;     rdi - Pointer to uninitialized arena struct.
; Output:
;     rax - Initialized arena allocator.
arena_create:
    push r12

    %define ARENA_SIZE 2 * 1024 * 1024 * 1024
    mov rcx, ARENA_SIZE

    mov r12, rdi
    mov QWORD [r12 + arena.total_size], rcx
    mov QWORD [r12 + arena.position],   0

    mov rdi, ARENA_SIZE
    call map_virtual_pages
    mov [r12 + arena.base_ptr], rax

    mov rax, r12
    pop r12
    ret


; Destroy an arena allocator. Unmap all virtual memory pages.
; Input:
;     rdi - Pointer to arena allocator.
; Output:
;     None.
arena_destroy:
    push r12
    mov r12, rdi

    mov rdi, [r12 + arena.base_ptr]
    mov rsi, [r12 + arena.total_size]
    call unmap_virtual_pages

    mov QWORD [r12 + arena.base_ptr], 0
    mov QWORD [r12 + arena.total_size], 0
    mov QWORD [r12 + arena.position], 0

    pop r12
    ret


; Reset the arena. Unmapping all allocated pages and map new ones.
; Input:
;     rdi - Pointer to an arena allocator.
; Output:
;     None.
arena_reset:
    push rbx
    mov rbx, rdi ; Preserve
    call arena_destroy
    mov rdi, rbx
    call arena_create
    pop rbx
    ret


; Allocate rdi number of bytes.
; Input:
;     rdi - Pointer to an arena allocator.
;     rsi - Size in bytes to allocate.
; Output:
;     rax - Pointer to allocated data. Set to 0 if the allocation failed.
arena_allocate:
    mov r9,  [rdi + arena.position]
    mov r10, [rdi + arena.base_ptr]
    add r10, r9
    add r9, rsi
    cmp r9, [rdi + arena.total_size]
    jl .not_out_of_bounds
    xor rax, rax
    ret

.not_out_of_bounds:
    mov [rdi + arena.position], r9
    mov rax, r10
    ret


; Clear the arena without unmapping any allocated memory.
; Input:
;     rdi - Pointer to an arena allocator.
; Output:
;     None.
arena_clear:
    mov QWORD [rdi + arena.position], 0
    ret


; Allocates N number of bytes. Increases heap memory region size.
; Input:
;     rdi - Number of bytes to allocate.
; Output:
;     rax - Pointer to allocated data.
bump_allocate:
    mov rsi, rdi
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
;     None.
; Output:
;     None.
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


; Convert input string to a number.
; Input:
;     rdi - Input string.
;     rsi - Size of the string.
; Output:
;     rax - Converted number.
string_to_number:
    ret

; Convert zero-terminated input string to a number.
; Input:
;     rdi - Zero-terminated input string.
; Output:
;     rax - Converted number.
cstring_to_number:
    ret

; Convert input number to a string.
; Input:
;     rdi - The number to convert.
;     rsi - Pointer to number_string struct. The output will be placed here.
; Output:
;     rsi - Parsed number into number_string struct.
number_to_string:
    ; Current buffer write offset
    xor r9, r9

    mov rax, rdi ; Dividend.
    mov rcx, 10  ; Divisor.

.loop_number_convert:
    ; Clear the dividend.
    xor rdx, rdx
    div rcx
    ; rax - Result of the divion.
    ; rdx - Divison reminder.

    add rdx, '0'
    mov [rsi + number_string.buffer + r9], dl
    inc r9

    cmp rax, 0
    jne .loop_number_convert

    ; Set string length and add null terminator.
    lea rax, [r9 + 1]
    mov [rsi + number_string.len], al
    mov BYTE [rsi + number_string.buffer + rax], 0

    ; Clear the left iterator.
    xor r8, r8
.loop_string_inverse:
    mov al, [rsi + number_string.buffer + r8]
    mov dl, [rsi + number_string.buffer + r9]
    mov [rsi + number_string.buffer + r8], dl
    mov [rsi + number_string.buffer + r9], al

    inc r8
    dec r9
    
    cmp r8, r9
    jl .loop_string_inverse
    ret

; Convert input number to a string.
; Input:
;     rdi - The number to convert.
; Output:
;     string_buffer - Buffer with converted string.
;     string_len    - Length of the converted string.
; number_to_string_old:
;     ; Current buffer write offset
;     mov r8, 0
;
;     mov rax, rdi ; Dividend.
;     mov rcx, 10  ; Divisor.
;
; .loop_convert:
;     mov rdx, 0 ; Clear the dividend.
;     div rcx
;     ; rax - Result of the divion.
;     ; rdx - Divison reminder.
;
;     add rdx, 48
;     mov [string_buffer_inv + r8], rdx
;     inc r8
;
;
;     cmp rax, 0
;     jne .loop_convert
;
;     ; Now we set up decrement register
;     mov rax, r8
;
;     ; Reset buffer write offset
;     mov r8, 0
;
; .loop_reverse:
;     dec rax
;
;     mov rdx, [string_buffer_inv + rax]
;     mov [string_buffer + r8], rdx
;     inc r8
;
;     cmp rax, 0
;     jne .loop_reverse
;
;     ; null terminate the string
;     mov rdx, 0
;     mov [string_buffer + r8], rdx
;     inc r8
;     mov [string_len], r8
;     ret


; Prints 64 bit number into STDOUT.
; Input:
;     rdi - Number to be printed.
; Output:
;     None.
print_number:
    sub rsp, number_string_size

    mov rsi, rsp
    call number_to_string
    lea r8,  [rsi + number_string.buffer]
    mov r9b, [rsi + number_string.len]

    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, r8
    mov rdx, r9
    syscall

    add rsp, number_string_size
    ret


; Prints newline character into STDOUT.
; Input:
;     None.
; Output:
;     None.
print_newline:
    mov rax, SYS_WRITE
    mov rdi, STDOUT
    mov rsi, newline_char
    mov rdx, 1
    syscall
    ret


; Testing some stuff here
debug_testing:
    PRINTLN debug_msg, debug_len
    BREAKPOINT
    ret
