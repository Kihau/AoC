# Advent of Code 2023
Advent of Code solutions written in x86_64 Linux and Windows assembly.

## Building requirements:
- [NASM](https://nasm.us/) - Netwide Assembler (`nasm` executable needs to be in your system PATH)
- Windows:
    - [MS Build](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022) toolchain (`link.exe` linker needs to be in your system PATH)
        - alternatively [MINGW](https://www.mingw-w64.org/) build tools can be used (`ld` linker needs to be in your system PATH)
- Linux:
    - Build tools (`ld` linker needs to be in your system PATH)

## Building on windows:
Run build script in windows cmd:
```
$ .\build-windows.bat
```
alternatively run this build script in MINGW shell:
```
$ ./build-windows.sh
```
#### Notice
The `kernel32.lib` in `res/` directory is used for linking final executable with kernel32.dll  
The library was generated using a MINGW dlltool and the `kernel32.def` script.  
If you wish to use official Microsoft kernel32.lib you may edit the build script.

## Building on linux:
Run build script in any linux shell:
```
$ ./build-linux.sh
```

## Cross building windows version on linux:
If you wish to cross build you may need to install the MINGW toolchain.  
Then you need to uncomment the following line in the `build-windows.sh` script:
```
x86_64-w64-mingw32-ld --entry=main $objects res/kernel32.lib --output=build/aoc.exe
```
