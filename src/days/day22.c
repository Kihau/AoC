#include "all_days.h"

// TODO: Scan the input, don't hardcode it
//
// Real inputs - 50x50 chunk each
// #define CHUNK_SIZE 50
// #define MAP_WIDTH 3
// #define MAP_HEIGHT 4

// Dummy
#define CHUNK_SIZE 4
#define MAP_WIDTH 4
#define MAP_HEIGHT 3

typedef struct {
    // int start;
    int length;
    int capacity;
    char *data;
} Vec;

typedef struct {
    Vec *data;
    int length;
} Lines;

typedef enum {
    RIGHT = 0,
    DOWN  = 1,
    LEFT  = 2,
    UP    = 3,
} Direction;

typedef struct {
    int steps;
    bool rotate_clockwise;
    int cursor;
} Move;

typedef struct {
    int position_x;
    int position_y;
    Direction direction;
    int chunk_idx;
    // Move move;
} State;

Vec vec_new(int cap) {
    Vec vec = {
        // .start = 0,
        .capacity = cap,
        .length = 0,
        .data = malloc(cap),
    };
    return vec;
}

// Scan the input to determine the chunk size as well as map width and height
int get_biggest_chunk_size() {
    assert(false && "Not implemented yet");
}

void vec_free(const Vec vec) {
    free(vec.data);
}

void vec_push(Vec *vec, char c) {
    if (vec->capacity == vec->length) {
        vec->capacity *= 2;
        vec->data = realloc(vec->data, vec->capacity);
    }

    vec->data[vec->length] = c;
    vec->length += 1;
}

// Vec get_map_line(FILE *input) {
//     //            null termination because c  VVV
//     Vec vec = vec_new(CHUNK_SIZE * MAP_WIDTH + 1);
//
//     char c = 0;
//     while (c != '.' && c != '#') {
//         c = getc(input);
//         vec_push(&vec, c);
//         if (c == EOF) {
//             return vec;
//         }
//         // vec.start += 1;
//     }
//
//     // c = getc(input);
//     while (c != '\n' && c != EOF) {
//         vec_push(&vec, c);
//         c = getc(input);
//     }
//
//     // Null terminate
//     vec_push(&vec, 0);
//
//     return vec;
// }

Vec get_line(FILE *input) {
    Vec vec = vec_new(4);

    char c = getc(input);
    while (c != '\n' && c != EOF) {
        vec_push(&vec, c);
        c = getc(input);
    }

    // Null terminate
    vec_push(&vec, 0);

    return vec;
}

Lines read_map_lines(FILE *input) {
    Lines lines = {
        .length = 0,
        .data = malloc(sizeof(Vec) * MAP_HEIGHT * CHUNK_SIZE),
    };

    for (int i = 0; i < MAP_HEIGHT * CHUNK_SIZE; i++) {
        Vec line = get_line(input);
        // printf("%s\n", line.data);
        lines.data[lines.length++] = line; 
    }

    // Vec *line = get_line(input);
    // do {
    //     printf("%i %s\n", line->capacity, line->data);
    //     lines.data[lines.length++] = line;
    //     line = get_line(input);
    // } while (line != NULL);

    return lines;
}

void free_map_lines(Lines lines) {
    for (int i = 0; i < lines.length; i++) {
        vec_free(lines.data[i]);
    }
    free(lines.data);
}

// void fill_chunk_map(bool chunk_map[MAP_HEIGHT][MAP_WIDTH], const Lines *lines) {
//     for (int y = 0; y < MAP_HEIGHT; y++) {
//         for (int x = 0; x < MAP_WIDTH; x++) {
//             int y_line = y * CHUNK_SIZE;
//             int x_line = x * CHUNK_SIZE + 1;
//
//             Vec line = lines->data[y_line];
//             if (line.length > x_line && line.data[x_line] != ' ') {
//                 // printf("%i %i : %s\n", y_line, x_line, line->data);
//                 chunk_map[y][x] = true;
//             }
//         }
//     }
// }

void print_chunk_ncurses(char *chunk, int y_offset, int x_offset) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
        // move(y + y_offset, 0);
        for (int x = 0; x < CHUNK_SIZE; x++) {
            // printw("%i ", y * CHUNK_SIZE + x);
            
            // Do some coloring
            move(y + y_offset, x + x_offset);
            if (chunk) {
                char c = chunk[y * CHUNK_SIZE + x];
                if (c == '>' || c == '<' || c == '^' || c == 'V') 
                    attron(COLOR_PAIR(1));
                printw("%c", c);
                attron(COLOR_PAIR(2));
            } else printw(" ");
        }
        // printw("\n");
    }
    refresh();
}

void print_map_ncurses(char *chunk_map[MAP_WIDTH * MAP_HEIGHT]) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            char *chunk = chunk_map[y * MAP_WIDTH + x];
            int y_offset = y * CHUNK_SIZE;
            int x_offset = x * CHUNK_SIZE;
            print_chunk_ncurses(chunk, y_offset, x_offset);
        }
    }
}

char *create_chunk(const Lines *lines, const int y_start, const int x_start) {
    char *chunk = malloc(CHUNK_SIZE * CHUNK_SIZE);
    for (int y = 0; y < CHUNK_SIZE; y++) {
        Vec line = lines->data[y + y_start];
        for (int x = 0; x < CHUNK_SIZE; x++) {
            // printf("y: %i x: %i\n", y * CHUNK_SIZE, x);
            chunk[y * CHUNK_SIZE + x] = line.data[x + x_start];
        }
    }

    return chunk;
}

void create_chunk_map(char *chunk_map[MAP_HEIGHT * MAP_WIDTH], const Lines *lines) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            int y_line = y * CHUNK_SIZE;
            int x_line = x * CHUNK_SIZE;

            Vec line = lines->data[y_line];
            if (line.length > x_line + 1 && line.data[x_line + 1] != ' ') {
                // printf("y: %i x: %i\n", y_line, x_line);
                char *chunk = create_chunk(lines, y_line, x_line);
                chunk_map[y * MAP_WIDTH + x] = chunk;
            }
        }
    }
}

void print_chunk(char *chunk) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
        for (int x = 0; x < CHUNK_SIZE; x++) {
            printf("%c", chunk[y * CHUNK_SIZE + x]);
        }
        printf("\n");
    }
}

bool next_move(Move *move, const Vec *input_moves) {
    if (move->cursor >= input_moves->length)
        return false;

    char buffer[8] = {};
    int len = 0;

    char c = input_moves->data[move->cursor]; 
    while (c != 'R' && c != 'L') {
        buffer[len++] = c;

        move->cursor += 1;

        // TODO: Last move is ignored? (it doesnt have a direction at the end)
        if (move->cursor >= input_moves->length)
            break;
        
        c = input_moves->data[move->cursor]; 
    }

    move->steps = atoi(buffer) - 1;
    // Hack
    if (move->steps == 0)
        move->steps += 1;

    // Last move does not hava a direction, so it just gets skipped.
    if (move->cursor >= input_moves->length)
        return true;

    switch (c) {
        case 'R': move->rotate_clockwise = true; break;
        case 'L': move->rotate_clockwise = false; break;
        default: assert(false && "Unreachable");
    }

    move->cursor += 1;
    return true;
}

void find_starting_chunk(State *state, const char *chunk_map[MAP_HEIGHT * MAP_WIDTH]) {
    for (int y = 0; y < MAP_HEIGHT ; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (chunk_map[y * MAP_WIDTH + x]) {
                // Can the starting point here be a wall?
                // state->position_y = y * CHUNK_SIZE;
                // state->position_x = x * CHUNK_SIZE;
                state->chunk_idx =  y * MAP_WIDTH + x;
                return;
            }
        }
    }
    assert(false && "Incorrect input (or is the code broken?)");
}

void change_state_direction(State *state, const Move *move) {
    if (move->rotate_clockwise) {
        state->direction += 1;
    } else {
        state->direction -= 1;
    }

    // Wrapping the enum
    if (state->direction > UP)
        state->direction = RIGHT;

    if (state->direction < RIGHT)
        state->direction = UP;
}

bool traverse_chunk(const char *chunk, State *state, Move *move) {
    switch (state->direction) {
        case RIGHT: while (move->steps) {
            // Right side of the chunk was reached, but the move steps still remain
            if (state->position_x == CHUNK_SIZE - 1) return true;
            // Wall was hit, there is nowhere to move now
            if (chunk[state->position_y * CHUNK_SIZE + state->position_x + 1] == '#')
                return false;
            // Move right by one step
            state->position_x += 1;
            move->steps -= 1;
        } break;

        case DOWN: while (move->steps) {
            // Bottom of the chunk was reached, but the move steps still remain
            if (state->position_y == CHUNK_SIZE - 1) return true;
            // Wall was hit, there is nowhere to move now
            if (chunk[(state->position_y + 1) * CHUNK_SIZE + state->position_x] == '#')
                return false;
            // Move down by one step
            state->position_y += 1;
            move->steps -= 1;
        } break;

        case LEFT: while (move->steps) {
            // Left side of the chunk was reached, but the move steps still remain
            if (state->position_x == 0) return true;
            // Wall was hit, there is nowhere to move now
            if (chunk[state->position_y * CHUNK_SIZE + state->position_x - 1] == '#')
                return false;
            // Move left by one step
            state->position_x -= 1;
            move->steps -= 1;
        } break;

        case UP: while (move->steps) { 
            // Top of the chunk was reached, but the move steps still remain
            if (state->position_y == 0) return true;
            // Wall was hit, there is nowhere to move now
            if (chunk[(state->position_y - 1) * CHUNK_SIZE + state->position_x] == '#')
                return false;
            // Move up by one step
            state->position_y -= 1;
            move->steps -= 1;
        } break;
        default: assert(false && "Unreachabale code detected.");
    }
    // There are no more move steps left
    return false;
}

// Returns whether there are any move steps remaining
bool traverse_chunk_ncurses(char *chunk_map[MAP_WIDTH * MAP_HEIGHT], char *chunk, State *state, Move *move) {
    // Move this loop outside (and rename the function to make_step idk)
    while (move->steps) {
        print_map_ncurses(chunk_map);
        getch();
        switch (state->direction) {
            case RIGHT: {
                chunk[state->position_y * CHUNK_SIZE + state->position_x] = '>';

                if (state->position_x == CHUNK_SIZE - 1) return true;
                if (chunk[state->position_y * CHUNK_SIZE + state->position_x + 1] == '#')
                    return false;

                state->position_x += 1;
            } break;

            case DOWN: {
                chunk[state->position_y * CHUNK_SIZE + state->position_x] = 'V';

                if (state->position_y == CHUNK_SIZE - 1) return true;
                if (chunk[(state->position_y + 1) * CHUNK_SIZE + state->position_x] == '#')
                    return false;

                state->position_y += 1;
            } break;

            case LEFT: {
                chunk[state->position_y * CHUNK_SIZE + state->position_x] = '<';

                if (state->position_x == 0) return true;
                if (chunk[state->position_y * CHUNK_SIZE + state->position_x - 1] == '#')
                    return false;
                state->position_x -= 1;
            } break;

            case UP: { 
                chunk[state->position_y * CHUNK_SIZE + state->position_x] = '^';

                if (state->position_y == 0) return true;
                if (chunk[(state->position_y - 1) * CHUNK_SIZE + state->position_x] == '#')
                    return false;

                state->position_y -= 1;
            } break;
            default: assert(false && "Unreachabale code detected.");
        }
        move->steps -= 1;
    }
    // There are no more move steps left
    return false;
}

int get_new_chunk_index(const char *chunk_map[MAP_HEIGHT * MAP_WIDTH], const State *state) {
    // TODO: this is fucked - fix
    int y_idx = state->chunk_idx / MAP_WIDTH;
    int x_idx = state->chunk_idx - y_idx * MAP_WIDTH;
    int new_idx = state->chunk_idx;

    switch (state->direction) {
        case RIGHT: do {
            x_idx = x_idx == MAP_WIDTH - 1 ? 0 : x_idx + 1;
            new_idx = y_idx * MAP_WIDTH + x_idx;
        } while (chunk_map[new_idx] == NULL); break;

        case DOWN: do {
            y_idx = y_idx == MAP_HEIGHT - 1 ? 0 : y_idx + 1;
            new_idx = y_idx * MAP_WIDTH + x_idx;
        } while (chunk_map[new_idx] == NULL); break;

        case LEFT: do {
            x_idx = x_idx == 0 ? MAP_WIDTH - 1 : x_idx - 1;
            new_idx = y_idx * MAP_WIDTH + x_idx;
        } while (chunk_map[new_idx] == NULL); break;

        case UP: do {
            y_idx = y_idx == 0 ? MAP_HEIGHT - 1 : y_idx - 1;
            new_idx = y_idx * MAP_WIDTH + x_idx;
        } while (chunk_map[new_idx] == NULL); break;
        default: assert(false && "Unreachabale code detected.");
    }

    return new_idx;
}

bool try_switch_chunk(const char *new_chunk, State *state) {
    State new_state = *state;
    switch (state->direction) {
        case RIGHT: new_state.position_x = 0;              break;
        case DOWN:  new_state.position_y = 0;              break;
        case LEFT:  new_state.position_x = CHUNK_SIZE - 1; break;
        case UP:    new_state.position_y = CHUNK_SIZE - 1; break;
        default: assert(false && "Unreachabale code detected.");
    }

    if (new_chunk[new_state.position_y * CHUNK_SIZE + new_state.position_x] == '#')
        return false;

    *state = new_state;
    return true;
}

int traverse_map(const char *chunk_map[MAP_HEIGHT * MAP_WIDTH], const Vec *input_moves) {
    State state = {
        .direction = RIGHT,
        .position_x = 0,
        .position_y = 0,
    };

    // Find a starting point
    find_starting_chunk(&state, chunk_map);
    // printf("Found starting point at %i %i\n\n", state.position_y, state.position_x);

    Move move = {};
    while (next_move(&move, input_moves)) {
        // printf("\n");
        // printf("Going to move %i%c with current rotation %i\n", move.steps, move.rotate_clockwise ? 'R' : 'L', state.direction);

        // Start traversing the chunks
        while (move.steps > 0) {
            const char *chunk = chunk_map[state.chunk_idx];
            // bool out_of_bounds = traverse_chunk(chunk, &state, &move);
            bool out_of_bounds = traverse_chunk_ncurses(chunk_map, chunk, &state, &move);
            if (out_of_bounds) {

                int new_chunk_idx = get_new_chunk_index(chunk_map, &state);
                const char *new_chunk = chunk_map[new_chunk_idx];

                // If chunk changed to a wall - go to the previous one and skip the moves
                if (try_switch_chunk(new_chunk, &state)) {
                    state.chunk_idx = new_chunk_idx;
                } else move.steps = 0;
            } else move.steps = 0;
        }

        // printf("After the moves we are at %i chunk in pos %i %i\n", state.chunk_idx, state.position_y, state.position_x);
        // Change rotation after we finish moving
        change_state_direction(&state, &move);

        // printf("%i %i %i\n", move.steps, move.rotate_clockwise, move.cursor);
    }

    // Calculate the score based on the state
    int map_y_idx = state.chunk_idx / MAP_WIDTH;
    int map_x_idx = state.chunk_idx - map_y_idx * MAP_WIDTH;
    int map_pos_y = map_y_idx * CHUNK_SIZE + state.position_y + 1;
    int map_pos_x = map_x_idx * CHUNK_SIZE + state.position_x + 1;
    // printf("Final chunk pos: %i %i\n", map_pos_y, map_pos_x);
    // printf("Final global pos: %i %i\n", state.position_y, state.position_x);
    int password = map_pos_y * 1000 + map_pos_x * 4 + state.direction;
    return password;
}

void day22(FILE *input) { 
    initscr();
    noecho();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);

    Lines lines = read_map_lines(input);
    // printf("len is = %i\n", lines.length);

    // Chunk map should actually store chunk pointers of 1D array chunks?
    char *chunk_map[MAP_HEIGHT * MAP_WIDTH] = {};
    create_chunk_map(chunk_map, &lines);

    // There is an empty line after the map lines - ignore it.
    vec_free(get_line(input));

    Vec input_moves = get_line(input);
    // printf("%i : %s\n", input_moves.capacity, input_moves.data);

    int part1_password = traverse_map(chunk_map, &input_moves);
    getch();

    // bool chunk_map[MAP_HEIGHT][MAP_WIDTH] = {};
    // fill_chunk_map(chunk_map, &lines);

    // printf("\nThe chunks are:\n");
    // for (int i = 0; i < MAP_HEIGHT * MAP_WIDTH; i++) {
    //     char *chunk = chunk_map[i];
    //     if (chunk) {
    //         print_chunk(chunk);
    //         printf("\n");
    //     }
    // }
    //
    // printf("The map is:\n");
    // for (int y = 0; y < MAP_HEIGHT; y++) {
    //     for (int x = 0; x < MAP_WIDTH; x++) {
    //         char *chunk = chunk_map[y * MAP_WIDTH + x];
    //         printf("%i ", chunk ? 1 : 0);
    //     }
    //     printf("\n");
    // }
    // printf("\n");


    // TODO: More free here
    // Free all data
    vec_free(input_moves);
    free_map_lines(lines);

    endwin();

    printf("PART 1: Calculated password = %i\n", part1_password);
}
