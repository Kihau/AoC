; Windows ABI:
;
; Argument registers order:
;     Integer values:        rcx,  rdx,  r8,   r9
;     Floating point values: xmm0, xmm1, xmm2, xmm3
; Return registers:
;     rax
; Preserved registers:
; Volatile registers:
;
;
;    WinApi call stack setup:
; +-------------------------------+ <- rsp aligned to 16 bytes
; |  PADDING                      |    ex. 5 function input args -> 40 bytes
; +-------------------------------+        40 bytes aligned to 16 -> 48 bytes stack frame
; |        .                      |
; |        .                      |
; |        .                      |
; |  STACK ARGUMENT 2             |
; |  STACK ARGUMENT 1             |
; +-------------------------------+  -+  <- rsp + 32
; |  R9                           |   |
; +-------------------------------+   |
; |  R8                           |   |    Shadow Space:
; +-------------------------------+   |    At least 32 bytes of free space is required to be allocated
; |  RDX                          |   |    before any win api calls are made.
; +-------------------------------+   |
; |  RCX                          |   |
; +-------------------------------+  -+  <- rsp
; |  CALLER RET ADDR              | 
; +-------------------------------+ <- rsp after function call (ex. call ExitProcess)

;%define ExitProcess  _ExitProcess@4
;%define WriteConsole _WriteConsoleA@20
;%define GetStdHandle _GetStdHandle@4

;%define ExitProcess  ExitProcess@4
;%define WriteConsole WriteConsoleA@20
;%define GetStdHandle GetStdHandle@4

; Standard device types:
%define STD_INPUT_HANDLE  -10
%define STD_OUTPUT_HANDLE -11
%define STD_ERROR_HANDLE  -12

; VirtualAlloc allocation types:
%define MEM_COMMIT      0x00001000
%define MEM_RESERVE     0x00002000
%define MEM_RESET       0x00080000
%define MEM_TOP_DOWN    0x00100000
%define MEM_WRITE_WATCH 0x00200000
%define MEM_PHYSICAL    0x00400000
%define MEM_RESET_UNDO  0x01000000
%define MEM_LARGE_PAGES 0x20000000

; VirtualAlloc deallocation types:
%define MEM_DECOMMIT 0x00004000
%define MEM_RELEASE  0x00008000

; Memory protection constants:
%define PAGE_NOACCESS          0x00000001
%define PAGE_READONLY          0x00000002
%define PAGE_READWRITE         0x00000004
%define PAGE_WRITECOPY         0x00000008
%define PAGE_EXECUTE           0x00000010
%define PAGE_EXECUTE_READ      0x00000020
%define PAGE_EXECUTE_READWRITE 0x00000040
%define PAGE_EXECUTE_WRITECOPY 0x00000080
%define PAGE_TARGETS_INVALID   0x40000000
%define PAGE_TARGETS_NO_UPDATE 0x40000000

; File open flags
; %define OF_SHARE_COMPAT     0x00000000
; %define OF_READ             0x00000000
; %define OF_WRITE            0x00000001
; %define OF_READWRITE        0x00000002
; %define OF_SHARE_EXCLUSIVE  0x00000010
; %define OF_SHARE_DENY_WRITE 0x00000020
; %define OF_SHARE_DENY_READ  0x00000030
; %define OF_SHARE_DENY_NONE  0x00000040
; %define OF_PARSE            0x00000100
; %define OF_DELETE           0x00000200
; %define OF_CANCEL           0x00000800
; %define OF_CREATE           0x00001000
; %define OF_EXIST            0x00004000
; %define OF_PROMPT           0x00002000
; %define OF_REOPEN           0x00008000
; 
; struc OFSTRUCT
;     .cBytes:     resb 1
;     .fFixedDisk: resb 1
;     .nErrCode:   resb 2
;     .Reserved1:  resb 2
;     .Reserved2:  resb 2
;     .szPathName: resb 128
; endstruc


%define GENERIC_ALL     0x10000000
%define GENERIC_EXECUTE 0x20000000
%define GENERIC_WRITE   0x40000000
%define GENERIC_READ    0x80000000

%define FILE_SHARE_NONE   0x00000000
%define FILE_SHARE_READ   0x00000001
%define FILE_SHARE_WRITE  0x00000002
%define FILE_SHARE_DELETE 0x00000004

%define CREATE_NEW        1
%define CREATE_ALWAYS     2
%define OPEN_EXISTING     3
%define OPEN_ALWAYS       4
%define TRUNCATE_EXISTING 5


%define FILE_ATTRIBUTE_READONLY      0x00000001
%define FILE_ATTRIBUTE_HIDDEN        0x00000002
%define FILE_ATTRIBUTE_SYSTEM        0x00000004
%define FILE_ATTRIBUTE_ARCHIVE       0x00000020
%define FILE_ATTRIBUTE_NORMAL        0x00000080
%define FILE_ATTRIBUTE_TEMPORARY     0x00000100
%define FILE_ATTRIBUTE_ENCRYPTED     0x00004000
%define FILE_ATTRIBUTE_OFFLINE       0x00001000
%define FILE_FLAG_OPEN_NO_RECALL     0x00100000
%define FILE_FLAG_OPEN_REPARSE_POINT 0x00200000
%define FILE_FLAG_SESSION_AWARE      0x00800000
%define FILE_FLAG_SEQUENTIAL_SCAN    0x08000000
%define FILE_FLAG_POSIX_SEMANTICS    0x01000000
%define FILE_FLAG_BACKUP_SEMANTICS   0x02000000
%define FILE_FLAG_DELETE_ON_CLOSE    0x04000000
%define FILE_FLAG_NO_BUFFERING       0x20000000
%define FILE_FLAG_OVERLAPPED         0x40000000
%define FILE_FLAG_RANDOM_ACCESS      0x10000000
%define FILE_FLAG_WRITE_THROUGH      0x80000000
