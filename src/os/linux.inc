; Linux system calls:
;
; Call register:
;     rax
; Argument registers order:
;     rdi, rsi, rdx, r10, r8, r9
; Return registers:
;     rax, rdx, (some syscalls also modify rcx and r11 registers)
; Preserved registers:
; Volatile registers:
;
;
;     Memory layout:
; +-------------------------------+  <- 0xFFFFFFFFFFFF
; |  KERNEL SPACE                 |
; +-------------------------------+  -+ Stack start
; |  STACK      |                 |   |
; |             |                 |   |    Usually around 8 MB.
; |             v                 |   |
; +-------------------------------+  -+ Stack end
; |  SHARED LIBS                  |
; +-------------------------------+
; |             ^                 |
; |             |                 |  <- Mmap (random adress in heap section).
; |  HEAP       |                 |
; +-------------------------------+  <- Heap start = brk(0)
; |  .bss                         |  <- Uninitialized data
; +-------------------------------+
; |  .data                        |  <- Initialized data
; +-------------------------------+
; |  .rodata                      |  <- Read-only data
; +-------------------------------+
; |  .text                        |  <- Executable code
; +-------------------------------+
; |  RESERVED                     |
; +-------------------------------+  <- 0x000000000000

; Linux system calls codes.
; Passed to the rax register.
%define SYS_READ   0
%define SYS_WRITE  1
%define SYS_OPEN   2
%define SYS_CLOSE  3
%define SYS_STAT   4
%define SYS_FSTAT  5
%define SYS_LSTAT  6
%define SYS_MMAP   9
%define SYS_MUNMAP 11 
%define SYS_BRK    12
%define SYS_EXIT   60

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

; SYS_MMAP
%define PROT_READ  0x01
%define PROT_WRITE 0x02

%define MAP_PRIVATE   0x02
%define MAP_ANONYMOUS 0x20

struc timespec 
    .tv_sec:  resb 8
    .tv_nsec: resb 8
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

    .st_atim:    resb timespec_size
    .st_mtim:    resb timespec_size
    .st_ctim:    resb timespec_size

    .pad1:       resb 24
endstruc
