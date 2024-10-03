;  linux.asm  Operating System specific implementations.

%include "os/linux.inc"

bits 64
default rel

section .text
    global exit_program
    global print_output
    global print_error
    global map_memory_pages
    global unmap_memory_pages
    global open_file
    global close_file
    global get_file_size
    global read_file


exit_program:
    mov rdi, rax
    mov rax, SYS_EXIT
    syscall
    ud2


print_output:
    push r11
    push r10
    push r9
    push r8
    mov rdx, rbx
    mov rsi, rax
    mov rdi, STDOUT
    mov rax, SYS_WRITE
    syscall
    pop r8
    pop r9
    pop r10
    pop r11
    ret


print_error:
    push r11
    push r10
    push r9
    push r8
    mov rdx, rbx
    mov rsi, rax
    mov rdi, STDERR
    mov rax, SYS_WRITE
    syscall
    pop r8
    pop r9
    pop r10
    pop r11
    ret


map_memory_pages:
    push r11
    push r10
    push r9
    push r8
    mov r9,  0                            ; off_t offset
    mov r8, -1                            ; int fd
    mov r10, MAP_PRIVATE | MAP_ANONYMOUS  ; int flags
    mov rdx, PROT_READ | PROT_WRITE       ; int prot
    mov rsi, rax                          ; size_t length
    mov rdi, 0                            ; void addr[.length]
    mov rax, SYS_MMAP
    syscall
    mov rdi, rax
    pop r8
    pop r9
    pop r10
    pop r11
    ret


unmap_memory_pages:
    push r11
    push r10
    push r9
    push r8
    mov rdi, rax
    mov rax, SYS_MUNMAP
    syscall
    pop r8
    pop r9
    pop r10
    pop r11
    ret


open_file:
    push r11
    push r10
    push r9
    push r8
    mov rsi, O_RDONLY
    mov rdi, rax
    mov rax, SYS_OPEN
    syscall
    mov rdi, rax
    pop r8
    pop r9
    pop r10
    pop r11
    ret 


close_file:
    push r11
    push r10
    push r9
    push r8
    mov rdi, rax
    mov rax, SYS_CLOSE
    syscall
    pop r8
    pop r9
    pop r10
    pop r11
    ret


get_file_size:
    push rbp
    push r11
    push r10
    push r9
    push r8
    mov rbp, rsp
    sub rsp, stat_size
    mov rsi, rsp
    mov rdi, rax
    mov rax, SYS_FSTAT
    syscall
    mov rdi, [rsp + stat.st_size]
    mov rsp, rbp
    pop r8
    pop r9
    pop r10
    pop r11
    pop rbp
    ret


read_file: 
    push r11
    push r10
    push r9
    push r8
    mov rdx, rcx
    mov rsi, rbx
    mov rdi, rax
    mov rax, SYS_READ
    syscall
    mov rdi, rax
    pop r8
    pop r9
    pop r10
    pop r11
    ret
