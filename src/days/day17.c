#include "all_days.h"

// #define BLOCKS_COUNT 2022
#define BLOCKS_COUNT 10000
#define CHAMBER_WIDTH 7
#define MAX_MAP_HEIGHT BLOCKS_COUNT * 4

// Check if bit is set to one
#define one(index_y, index_x) (map[index_y] & (1 << (index_x)))
// Set the bit to one
#define set(index_y, index_x) map[index_y] |= 1 << (index_x)

typedef enum {
    HLINE = 0,
    PLUS  = 1,
    REV_L = 2,
    VLINE = 3,
    BOX   = 4
} BlockType;

typedef struct {
    int x;
    int y;
} Pos;

static void print_map(u8 map[], int map_len) {
    // Printing in reverse order
    for (int i = map_len - 1; i >= 0; i--) {
        printf("|");
        u8 byte = map[i];
        for (int bit_count = 6; bit_count >= 0; bit_count--) {
            // printf("%c", byte & (1 << bit_count) ? '#' : '.');
            printf("%c", byte & 1 ? '#' : '.');
            byte >>= 1;
        }
        printf("|\n");
        // print_bits(map[i]);
    }
    printf("+-------+\n");
}

/*
 * pos here -> ####...
 */
static bool hline_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 3 >= 7 || p.y < 0 || 
        one(p.y + 0, p.x + 0) || 
        one(p.y + 0, p.x + 1) || 
        one(p.y + 0, p.x + 2) || 
        one(p.y + 0, p.x + 3);

}

static int place_hline(u8 map[], const Pos p) {
    set(p.y + 0, p.x + 0);
    set(p.y + 0, p.x + 1);
    set(p.y + 0, p.x + 2);
    set(p.y + 0, p.x + 3);
    return p.y + 1;
}

/*             .#.
 *             ###
 * pos here -> .#.
 */
static bool plus_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 2 >= 7 || p.y < 0 ||
        one(p.y + 0, p.x + 1) || 
        one(p.y + 1, p.x + 0) ||
        one(p.y + 1, p.x + 1) || 
        one(p.y + 1, p.x + 2) ||
        one(p.y + 2, p.x + 1);
}

static int place_plus(u8 map[], const Pos p) {
    set(p.y + 0, p.x + 1);
    set(p.y + 1, p.x + 0);
    set(p.y + 1, p.x + 1);
    set(p.y + 1, p.x + 2);
    set(p.y + 2, p.x + 1);
    return p.y + 3;
}

/*             #
 *             #
 *             #
 * pos here -> #
 */
static bool vline_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 0 >= 7 || p.y < 0 ||
        one(p.y + 0, p.x + 0) || 
        one(p.y + 1, p.x + 0) ||
        one(p.y + 2, p.x + 0) || 
        one(p.y + 3, p.x + 0);
}

static int place_vline(u8 map[], const Pos p) {
    set(p.y + 0, p.x + 0);
    set(p.y + 1, p.x + 0);
    set(p.y + 2, p.x + 0);
    set(p.y + 3, p.x + 0);
    return p.y + 4;
}


/*               #
 *               #
 * pos here -> ###
 */
static bool revl_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 2 >= 7 || p.y < 0 ||
        one(p.y + 0, p.x + 0) ||
        one(p.y + 0, p.x + 1) ||
        one(p.y + 0, p.x + 2) ||
        one(p.y + 1, p.x + 2) ||
        one(p.y + 2, p.x + 2);
}

static int place_revl(u8 map[], const Pos p) {
    set(p.y + 0, p.x + 0);
    set(p.y + 0, p.x + 1);
    set(p.y + 0, p.x + 2);
    set(p.y + 1, p.x + 2);
    set(p.y + 2, p.x + 2);
    return p.y + 3;
}

/*             ##
 * pos here -> ##
 */
static bool box_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 1 >= 7 || p.y < 0 ||
        one(p.y + 0, p.x + 0) ||
        one(p.y + 0, p.x + 1) ||
        one(p.y + 1, p.x + 0) ||
        one(p.y + 1, p.x + 1);
}

static int place_box(u8 map[], const Pos p) {
    set(p.y + 0, p.x + 0);
    set(p.y + 0, p.x + 1);
    set(p.y + 1, p.x + 0);
    set(p.y + 1, p.x + 1);
    return p.y + 2;
}

static bool block_overlapping(int block, const u8 map[], const Pos p) {
    bool overlapping = false;
    switch (block) {
        case HLINE: overlapping = hline_overlapping(map, p); break;
        case PLUS:  overlapping = plus_overlapping(map, p);  break;
        case REV_L: overlapping = revl_overlapping(map, p);  break;
        case VLINE: overlapping = vline_overlapping(map, p); break;
        case BOX:   overlapping = box_overlapping(map, p);   break;
        default:    assert(false && "Something went wrong"); break;
    }
    return overlapping;
}

static int place_block(int block, u8 map[], const Pos p) {
    int placed_height;
    switch (block) {
        case HLINE: placed_height = place_hline(map, p); break;
        case PLUS:  placed_height = place_plus(map, p);  break;
        case REV_L: placed_height = place_revl(map, p);  break;
        case VLINE: placed_height = place_vline(map, p); break;
        case BOX:   placed_height = place_box(map, p);   break;
        default:    assert(false && "Something went wrong");  break;
    }
    return placed_height;
}

static int find_height_blocks(u8 map[], FILE *input, int blocks, int height) {
    fseek(input, 0, SEEK_SET);
    BlockType block = HLINE;
    for (int i = 0; i < blocks; i++) {
        // Fall down loop
        int placed_height = -1;

        Pos p = {
            .x = 2,
            .y = height + 3, 
        };
        
        while (true) {
            char wind = fgetc(input);

            if (feof(input) || (wind != '>' && wind != '<')){
                printf("reapeted! b: %4i     h: %4i     i: %4i\n", block, height, i);
                // DANK
                fseek(input, 0, SEEK_SET);
                wind = fgetc(input);
            }
            int shift = 0;

            if (wind == '>')
                shift = 1;
            else if (wind == '<')
                shift = -1;

            p.x += shift;
            if (block_overlapping(block, map, p)) {
                p.x -= shift;
            }

            p.y -= 1;
            if (block_overlapping(block, map, p)) { 
                p.y += 1;
                placed_height = place_block(block, map, p);

                break;
            }
        }

        if (placed_height > height) {
            height = placed_height;
        }

        // if (height < 20) {
        //     printf("\n");
        //     print_map(map, height + 3);
        //     printf("h = %i\n", height);
        // }

        block += 1;
        if (block > BOX)
            block = HLINE;
    }
    return height;
}

static int find_height(u8 map[], FILE *input) {
    return find_height_blocks(map, input, BLOCKS_COUNT, 0);
}


// TODO: Full ncurses animation
void day17(FILE *input) {
    // Array of bit maps
    // 0000000(0) <- last bit is unused

    long max = 1000000000000;
    // long max = 2022;
    // long max = 12000;

#if 1 // Changing this to 0 displays adjust values
    // TODO: NOT COMPLETE: This only works for my input. For other inputs values have to be adjusted manually.
    // MAX_MAP_HEIGHT size should be based on the input
    u8 *map = malloc(MAX_MAP_HEIGHT);
    
    // Those need to be manually adjusted (for now)
    
    // Kihau
    const int h_start = 2642;
    const int i_start = 1709;
    const int h_rep = (5336 - 2642);
    const int i_rep = (3434 - 1709);
    
    // Dummy
    // const int h_start = 70;
    // const int i_start = 43;
    // const int h_rep = (123 - 70);
    // const int i_rep = (78 - 43);

    long h = h_start;
    long i = i_start;

    long iter_count =  max / i_rep;
    h += iter_count * h_rep;
    i += iter_count * i_rep;

    i -= i_start;
    h -= h_start;
    long i_left = max - i;

    int height_end = find_height_blocks(map, input, i_left, 0);
    printf("height: %li\n", height_end + h);

    free(map);
#else
    u8 *map = malloc(MAX_MAP_HEIGHT);
    int height = find_height_blocks(map, input, max, 0);
    printf("h = %i\n", height);
    free(map);
#endif
}
