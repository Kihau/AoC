#include <string.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <stdbool.h>

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

void addr_top() {
    int number = 123;
    printf("number is at: %p\n", &number);
    
    char *one = malloc(1);
    printf("one is at: %p\n", one);
    printf("stack ptr to one is at: %p\n", &one);
}

#define TERA 1024 * 1024 * 1024 * 1024
#define GIGA 1024 * 1024 * 1024
#define MEGA 1024 * 1024
#define KILO 1024

void giga_memory() {
    void *mem = mmap(0, (long)GIGA * 32, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
    // memset(mem, 0, (long)GIGA * 10);
    printf("mmap ptr start: %p\n", mem);
    printf("mmap ptr end:   %p\n", mem + (long)GIGA * 32);
    getchar();
}

void looping_test() {
    void *ptr;
    while (true) {
        // int a = PROT_READ|PROT_WRITE;
        // int b = MAP_PRIVATE|MAP_ANONYMOUS;
        // void *mem = mmap(GIGA, 1, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        // printf("mmap ptr: %p\n", mem);
        // memset(mem, 0, GIGA);

        ptr = sbrk((long)GIGA * 32);
        memset(ptr, 0, 1024 * 1024);
        ptr += (long)GIGA * 32;
        printf("pointer now: %p\n", ptr);
        // int result = brk(ptr);
        // memset(ptr, 0, GIGA);
        // printf("result: %i, pointer now: %p\n", result, ptr);

        getchar();
    }
}

void mmap_testing(void) {
    while (true) {
        char *ptr = mmap(0, 33L * GIGA, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
        // for (long i = 0; i < 1024 * 1024; i++) {
        //     ptr[i * 5000] = 0;
        //     // ptr[i * 4096] = 0;
        //     // ptr[i * 2048] = 0;
        // }

        for (long i = 0; i < 4100; i++) {
            ptr[i] = 0;
            // ptr[i * 4096] = 0;
            // ptr[i * 2048] = 0;
        }

        getchar();
    }
}

int main(int argc, char **argv) {
    // addr_top();
    // giga_memory();
    // looping_test();
    mmap_testing();

    return 0;
}
