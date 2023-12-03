#include "all_days.h"

#define STACK_COUNT 9

typedef struct {
    int len;
    // There is less then 9 * 8 crates, so one stack can safely store all crates (and more)
    char data[128];
} Stack;

typedef struct {
    int moves;
    int src;
    int dest;
} Rearrangement;

static Stack stack_create(void) {
    Stack new_stack = {
        .data = {},
        .len = 0,
    };
    return new_stack;
}

static char stack_pop(Stack *stack) {
    stack->len--;
    return stack->data[stack->len];
}

static void stack_push(Stack *stack, char crate) {
    stack->data[stack->len] = crate;
    stack->len++;
}

static void move_crates_by_one(int count, Stack *src, Stack *dest) {
    for (int i = 0; i < count && src->len > 0; i++) {
        char crate = stack_pop(src);
        stack_push(dest, crate);
    }
}

static void move_multiple_crates(int count, Stack *src, Stack *dest) {
    char *dest_ptr = dest->data + dest->len;
    char *src_ptr = src->data + src->len - count;
    memcpy(dest_ptr, src_ptr, count);
    dest->len += count;
    src->len -= count;
}

static void parse_crates(Stack *stacks, FILE *input) {
    char line[128];
    int jumps[128];
    int index = 0;

    do {
        // Is it better to save stream positions and re-read it in reverse or just save the data?
        jumps[index] = ftell(input);
        fgets(line, 128, input);
        index++;
    } while (line[0] != '\n');

    int last = ftell(input);

    for (int i = index - 3; i >= 0; i--) {
        fseek(input, jumps[i], 0);
        fgets(line, 128, input);

        for (int i = 0, char_offset = 1; i < STACK_COUNT; i++, char_offset += 4) {
            if (line[char_offset] != ' ') {
                stack_push(stacks + i, line[char_offset]);
            } 
        }
    }

    fseek(input, last, 0);
}

static Rearrangement parse_rearrangement(char *line) {
    Rearrangement new_rea = {};
    while (*++line != ' ');

    char char_buf[8] = {};
    for (int i = 0; *++line != ' '; i++) {
        char_buf[i] = *line;
    }
    new_rea.moves = atoi(char_buf); 

    while (*++line != ' ');

    memset(char_buf, 0, sizeof(char_buf));
    for (int i = 0; *++line != ' '; i++) {
        char_buf[i] = *line;
    }
    new_rea.src = atoi(char_buf) - 1; 

    while (*++line != ' ');

    memset(char_buf, 0, sizeof(char_buf));
    for (int i = 0; *++line != '\n'; i++) {
        char_buf[i] = *line;
    }
    new_rea.dest = atoi(char_buf) - 1; 

    return new_rea;
}

void day5(FILE *input) { 
    Stack stacks1[STACK_COUNT];
    for (int i = 0; i < STACK_COUNT; i++)
        stacks1[i] = stack_create();
    parse_crates(stacks1, input);

    Stack stacks2[STACK_COUNT];
    for (int i = 0; i < STACK_COUNT; i++)
        stacks2[i] = stacks1[i];

    char line[128];
    while (fgets(line, sizeof(line), input)) {
        Rearrangement rea = parse_rearrangement(line);
        // PART 1
        move_crates_by_one(rea.moves, stacks1 + rea.src, stacks1 + rea.dest);
        // PART 2
        move_multiple_crates(rea.moves, stacks2 + rea.src, stacks2 + rea.dest);
    }

    printf("PART 1: Crates on top of the stacks = ");
    for (int i = 0; i < STACK_COUNT; i++) {
        int len = stacks1[i].len;
        printf("%c", stacks1[i].data[len - 1]);
    }
    printf("\n");

    printf("PART 2: Crates on top of the stacks = ");
    for (int i = 0; i < STACK_COUNT; i++) {
        int len = stacks2[i].len;
        printf("%c", stacks2[i].data[len - 1]);
    }
    printf("\n");
}
