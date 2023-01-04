#include "all_days.h"
#include <stdlib.h>

// TODO Make this an INITIAL capacity for the dynamic monkey vector (or something)
#define MONKEY_COUNT 8
// #define MONKEY_COUNT 4
#define MAX_ITEM_COUNT 64
#define ROUNDS_PART1 20
#define ROUNDS_PART2 10000
#define VALUE_OLD -1

typedef enum {
    ADD, SUB, MUL,
    // DIV, /* no division needed (?) */
} OpType;

typedef struct {
    // OpType sign;
    char sign;
    bool value_old;
    int value;
} Operation;

typedef struct {
    int division;
    int passed;
    int failed;
} Test;

typedef struct {
    long items[MAX_ITEM_COUNT];
    int items_len;

    Operation op;
    Test test;
    int inspections;
} Monkey;


static Monkey parse_monkey(FILE *input) {
    Monkey monkey = {};

    char buffer[128];
    char *buf;

    // This line gets ignored
    // Monkey 0:
    fgets(buffer, sizeof(buffer), input);

    //-------------------V 19 bytes
    //   Starting items: 79, 98
    fgets(buffer, sizeof(buffer), input);
    buf = buffer + 18;
    monkey.items_len = 0;
    char *start = buf;
    while (*buf++ != '\n') {
        if (*buf == ',') {
            char num_buf[8] = {};
            memcpy(num_buf, start, buf - start);
            monkey.items[monkey.items_len++] = atol(num_buf);
            start = buf + 2;
        }
    }
    char num_buf[8] = {};
    memcpy(num_buf, start, buf - start);
    monkey.items[monkey.items_len++] = atol(num_buf);
    start = buf + 2;

    //------------------------V 24 bytes
    //   Operation: new = old * 19
    fgets(buffer, sizeof(buffer), input);
    buf = buffer + 23;
    monkey.op.sign = buf[0];
    if (buf[2] != 'o') {
        monkey.op.value_old = false;
        monkey.op.value = atoi(buf + 2);
    } else monkey.op.value_old = true;

    //----------------------V 22 bytes
    //   Test: divisible by 23
    fgets(buffer, sizeof(buffer), input);
    buf = buffer + 21;
    monkey.test.division = atoi(buf);

    //------------------------------V 30 bytes
    //     If true: throw to monkey 2
    fgets(buffer, sizeof(buffer), input);
    buf = buffer + 29;
    monkey.test.passed = atoi(buf);

    //-------------------------------V 31 bytes
    //     If false: throw to monkey 3
    fgets(buffer, sizeof(buffer), input);
    buf = buffer + 30;
    monkey.test.failed = atoi(buf);

    // Ignore new line
    fgets(buffer, sizeof(buffer), input);

    return monkey;
}

static void print_monkey(Monkey *monkey) {
    printf("Monkey equipment: ");
    for (int i = 0; i < monkey->items_len; i++) {
        printf("%li ", monkey->items[i]);
    }
    printf("\n");
    if (!monkey->op.value_old)
        printf("Operation: new = old %c %i\n", monkey->op.sign, monkey->op.value);
    else printf("Operation: new = old %c old\n", monkey->op.sign);

    printf("Test: divisible by %i\n", monkey->test.division);

    printf("    If true: throw to monkey  %i\n", monkey->test.passed);
    printf("    If false: throw to monkey %i\n", monkey->test.failed);
    printf("Number of inspections %i\n", monkey->inspections);
    printf("\n");
}

static void monkey_round_part2(Monkey *monkeys, int index, int div_product) {
    Monkey *tx = &monkeys[index];
    tx->inspections += tx->items_len;

    for (int i = 0; i < tx->items_len; i++) {
        long worry = tx->items[i] % div_product;
        // long worry = tx->items[i] % 9699690;
        // long worry = tx->items[i];
        // while (worry > 9699690)
        //     worry -= 9699690;

        long value = tx->op.value_old ? worry : tx->op.value;
        switch (tx->op.sign) {
            case '+': worry += value; break; 
            case '-': worry -= value; break; 
            case '*': worry *= value; break; 
        }

        int throw_index = worry % tx->test.division == 0 
            ? tx->test.passed : tx->test.failed;

        Monkey *rx = &monkeys[throw_index];
        rx->items[rx->items_len++] = worry;
    }
    tx->items_len = 0;
}

static void monkey_round_part1(Monkey *monkeys, int index) {
    Monkey *tx = &monkeys[index];
    tx->inspections += tx->items_len;

    for (int i = 0; i < tx->items_len; i++) {
        long worry = tx->items[i];

        long value = tx->op.value_old ? worry : tx->op.value;
        switch (tx->op.sign) {
            case '+': worry += value; break; 
            case '-': worry -= value; break; 
            case '*': worry *= value; break; 
        }
        worry /= 3;

        int throw_index = worry % tx->test.division == 0 
            ? tx->test.passed : tx->test.failed;

        Monkey *rx = &monkeys[throw_index];
        rx->items[rx->items_len++] = worry;
    }
    tx->items_len = 0;
}

static long monkey_max(Monkey *monkeys) {
    long max1 = 0, max2 = 0;
    for (int i = 0; i < MONKEY_COUNT; i++) {
        if (max1 < monkeys[i].inspections) {
            max2 = max1;
            max1 = monkeys[i].inspections;
        } else if (max2 < monkeys[i].inspections)
            max2 = monkeys[i].inspections;
    }
    return max1 * max2;
}

void day11(FILE *input) { 
    Monkey monkeys1[MONKEY_COUNT];
    for (int i = 0; i < MONKEY_COUNT; i++) {
        monkeys1[i] = parse_monkey(input);
    }

    Monkey monkeys2[MONKEY_COUNT];
    memcpy(monkeys2, monkeys1, sizeof(monkeys1));

    for (int i = 0; i < ROUNDS_PART1; i++) {
        for (int j = 0; j < MONKEY_COUNT; j++) {
            monkey_round_part1(monkeys1, j);
        }
    }

    int div_product = 1;
    for (int i = 0; i < MONKEY_COUNT; i++) { 
        div_product *= monkeys2[i].test.division;
    }
    // printf("%i\n", 12 % div_product);

    for (int i = 0; i < ROUNDS_PART2; i++) {
        for (int j = 0; j < MONKEY_COUNT; j++) {
            monkey_round_part2(monkeys2, j, div_product);
        }
    }

    // for (int i = 0; i < MONKEY_COUNT; i++) {
    //     print_monkey(monkeys2 + i);
    // }

    long part1 = monkey_max(monkeys1);
    long part2 = monkey_max(monkeys2);

    printf("Part 1: Monkey business = %li\n", part1);
    printf("Part 2: Monkey business = %li\n", part2);
}
