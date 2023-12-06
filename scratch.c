#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

void print_stat() {
    struct stat buffer;
    struct timespec time = buffer.st_atim;

    printf("%li\n", sizeof buffer.st_dev);
    printf("%li\n", sizeof buffer.st_ino);
    printf("%li\n", sizeof buffer.st_nlink);

    printf("%li\n", sizeof buffer.st_mode);
    printf("%li\n", sizeof buffer.st_uid);
    printf("%li\n", sizeof buffer.st_gid);
    printf("%li\n", sizeof buffer.__pad0);
    printf("%li\n", sizeof buffer.st_rdev);
    printf("%li\n", sizeof buffer.st_size);
    printf("%li\n", sizeof buffer.st_blksize);
    printf("%li\n", sizeof buffer.st_blocks);

    printf("stat: %li\n", sizeof buffer);
    printf("timespec: %li\n", sizeof time);
}

#define GIGA 1024 * 1024 * 1024
#define MEGA 1024 * 1024
#define KILO 1024

void giga_memory() {
    void *mem = mmap(0, (long)GIGA * 16, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    memset(mem, 0, (long)GIGA * 16);
    printf("mmap ptr: %p\n", mem);
    sleep(3600);
}

int main(int argc, char **argv) {
    giga_memory();
    // void *ptr = mmap(0, 1, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);

    // while (1) {
    //     // int a = PROT_READ|PROT_WRITE;
    //     // int b = MAP_PRIVATE|MAP_ANONYMOUS;
    //     void *mem = mmap(GIGA * 16, 1, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    //     printf("mmap ptr: %p\n", mem);
    //     // memset(mem, 0, GIGA);
    //
    //     // ptr = sbrk(GIGA);
    //     // ptr += GIGA;
    //     // int result = brk(ptr);
    //     // memset(ptr, 0, GIGA);
    //     // printf("result: %i, pointer now: %p\n", result, ptr);
    //
    //     getchar();
    // }

    return 0;
}
