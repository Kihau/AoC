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
