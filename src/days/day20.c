#include "all_days.h"

// TODO: Also try doing to by copying the data, wihout the use of linked list
typedef struct {
    long coord;
    int next;
    int prev;
} Node;

static void fill_links(Node list[], int size) {
    // Initially all adjacent indexes point to one another, later they get mixed up
    for (int i = 0; i < size; ++i) {
        list[i].prev = i - 1;
        list[i].next = i + 1;
    }

    // Wrap the list around
    list[0].prev = size - 1;
    list[size - 1].next = 0;
}

static void print_list(const Node list[], const int size) {
    int current = 0;
    for (int i = 0; i < size; i++) {
        printf("%li ", list[current].coord);
        current = list[current].next;
    }
    printf("\n");
}

static void mix_coordinates(Node list[], const int size) {
    for (int i = 0; i < size; i++) {
        // print_list(list, size);

        int move_idx = i;

        // Traverse the linked list and find the node to swap
        long steps = labs(list[i].coord % (size - 1));
        if (steps == 0) continue;

        if (list[i].coord > 0) {
            for (int j = 0; j < steps; j++)
                move_idx = list[move_idx].next;
        } else {
            for (int j = 0; j < steps + 1; j++)
                move_idx = list[move_idx].prev;
        }

        // Unlink the connection - i gets moved
        Node *i_prev = &list[list[i].prev];
        Node *i_next = &list[list[i].next];
        i_prev->next = list[i].next;
        i_next->prev = list[i].prev;

        // Move the node to the right of the move index 
        int move_next = list[move_idx].next;
        list[i].next = move_next;
        list[move_next].prev = i;
        list[move_idx].next = i;
        list[i].prev = move_idx;
    }
}

static int solve_part1(Node list[], int size) {
    mix_coordinates(list, size);

    int current = -1;
    for (int i = 0; i < size; i++) {
        if (list[i].coord == 0) {
            current = i;
            break;
        }
    }

    assert(current != -1 && "Zero coordinate not found, is the input correct (or is my code broken)?");

    int coord_sum = 0;
    for (int i = 0; i < 3; i++) {
        for (int i = 0; i < 1000; i++) {
            current = list[current].next;
        }
        coord_sum += list[current].coord;
    }
    return coord_sum;
}

static long solve_part2(Node list[], int size) {
    // Apply decription key
    for (int i = 0; i < size; i++) {
        list[i].coord *= 811589153;
    }

    // In this part list gets mixed up 10 times.
    for (int i = 0; i < 10; i++)
        mix_coordinates(list, size);

    int current = -1;
    for (int i = 0; i < size; i++) {
        if (list[i].coord == 0) {
            current = i;
            break;
        }
    }

    assert(current != -1 && "Zero value not found, is the input correct (or is my code broken)?");

    long coord_sum = 0;
    for (int i = 0; i < 3; i++) {
        for (int i = 0; i < 1000; i++) {
            current = list[current].next;
        }
        coord_sum += list[current].coord;
    }
    return coord_sum;
}

void day20(FILE *input) { 
    Node *list = malloc(sizeof(Node) * 5000);
    int list_len = 0;

    char buffer[32];
    while (fgets(buffer, sizeof(buffer), input)) {
        list[list_len].coord = atoi(buffer);
        list_len += 1;
    }
    fill_links(list, list_len);
    // print_list(list, list_len);

    Node *part1_list = list;
    Node *part2_list = malloc(sizeof(Node) * 5000);
    memcpy(part2_list, list, sizeof(Node) * 5000);

    int part1 = solve_part1(part1_list, list_len);
    long part2 = solve_part2(part2_list, list_len);
    printf("PART1: Grove coordinates = %i\n", part1);
    printf("PART2: Grove coordinates = %li\n", part2);
}
