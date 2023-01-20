#include "all_days.h"

#define BLOCKS_COUNT 2022
#define CHAMBER_WIDTH 7
#define MAX_MAP_HEIGHT BLOCKS_COUNT * 4

#define print_bits(value)                                                                          \
    do {                                                                                           \
        int size = sizeof(value);                                                                  \
        void *addr = (char *)&value + size - 1;                                                    \
        while (size > 0) {                                                                         \
            u8 byte = *(char *)addr;                                                               \
            for (int bit_count = 7; bit_count >= 0; bit_count--) {                                 \
                printf("%i", byte & (1 << bit_count) ? 1 : 0);                                     \
            }                                                                                      \
            addr = (char *)addr - 1;                                                               \
            size--;                                                                                \
        }                                                                                          \
        printf("\n");                                                                              \
    } while (0);

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

typedef unsigned char u8;

void print_map(u8 map[], int map_len) {
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

// TODO: Bounds check and collitions
//       Return true when placed (collision on y + 1 pos)
//       Return false when falling (no collision)
//       Fix out of bounds (ignore collision on x)

/*
 * pos here -> ####...
 */
bool maybe_place_hline(u8 map[], Pos *p) {
    // Don't move on x when out of bounds or collision on x
    if (p->x < 0 || one(p->y, p->x)) {
        p->x++;
    } else if (p->x + 3 > 6 || one(p->y, p->x + 3)) {
        p->x--;
    }

    if (p->y == 0 || 
        one(p->y - 1, p->x + 0) || one(p->y - 1, p->x + 1) || 
        one(p->y - 1, p->x + 2) || one(p->y - 1, p->x + 3)
    ) {

        set(p->y + 0, p->x + 0);
        set(p->y + 0, p->x + 1);
        set(p->y + 0, p->x + 2);
        set(p->y + 0, p->x + 3);
        return true;
    } 

    return false;
}

/*             .#.
 *             ###
 * pos here -> .#.
 */
bool plus_overlapping(const u8 map[], const Pos p) {
    if (p.x < 0 || one(p.y, p.x + 1) || one(p.y + 1, p.x + 0) || 
        one(p.y + 2, p.x + 1) || p.x + 2 > 6 || one(p.y + 1, p.x + 2)) {
        return true;
    }

    return false;
}
bool maybe_place_plus(u8 map[], Pos *p) {
    if (p->x < 0 || 
        one(p->y, p->x + 1) || one(p->y + 1, p->x + 0) || one(p->y + 2, p->x + 1)
    ) {
        p->x++;
    } else if (p->x + 2 > 6 ||
        one(p->y, p->x + 1) || one(p->y + 1, p->x + 2) || one(p->y + 2, p->x + 1)
    ) {
        p->x--;
    }

    if (p->y == 0 || 
        one(p->y, p->x + 0) || one(p->y - 1, p->x + 1) || one(p->y, p->x + 2)
    ) {
        set(p->y + 0, p->x + 1);
        set(p->y + 1, p->x + 0);
        set(p->y + 1, p->x + 1);
        set(p->y + 1, p->x + 2);
        set(p->y + 2, p->x + 1);
        return true;
    } 

    return false;
}

/*             #
 *             #
 *             #
 * pos here -> #
 */
bool maybe_place_vline(u8 map[], Pos *p) {
    if (p->x < -1 || 
        one(p->y + -1, p->x + 0) || one(p->y + 1, p->x + 0) || 
        one(p->y + 1, p->x + 0) || one(p->y + 3, p->x + 0)
    ) {
        p->x++;
    } else if (p->x + 1 > 6 ||
        one(p->y + -1, p->x + 1) || one(p->y + 1, p->x + 1) ||
        one(p->y + 1, p->x + 1) || one(p->y + 3, p->x + 1)
    ) {
        p->x--;
    }

    if (p->y == -1 || one(p->y - 1, p->x + 0)) {
        set(p->y + -1, p->x + 0);
        set(p->y + 0, p->x + 0);
        set(p->y + 1, p->x + 0);
        set(p->y + 2, p->x + 0);
        return true;
    } 

    return false;
}

/*               #
 *               #
 * pos here -> ###
 */
bool maybe_place_revl(u8 map[], Pos *p) {
    if (p->x < 0 || 
        one(p->y, p->x) || one(p->y + 1, p->x + 2) || one(p->y + 2, p->x + 2)
    ) {
        p->x++;
    } else if (p->x + 2 > 6 ||
        one(p->y, p->x + 2) || one(p->y + 1, p->x + 2) || one(p->y + 2, p->x + 2)
    ) {
        p->x--;
    }

    if (p->y == 0 || 
        one(p->y - 1, p->x + 0) || one(p->y - 1, p->x + 1) || one(p->y - 1, p->x + 2)
    ) {
        set(p->y + 0, p->x + 0);
        set(p->y + 0, p->x + 1);
        set(p->y + 0, p->x + 2);
        set(p->y + 1, p->x + 2);
        set(p->y + 2, p->x + 2);
        return true;
    } 

    return false;
}

/*             ##
 * pos here -> ##
 */
bool maybe_place_box(u8 map[], Pos *p) {
    if (p->x < 0 || one(p->y, p->x) || one(p->y + 1, p->x)) {
        p->x++;
    } else if (p->x + 1 > 6 || one(p->y, p->x + 1) || one(p->y + 1, p->x + 1)) {
        p->x--;
    }

    if (p->y == 0 || one(p->y - 1, p->x) || one(p->y - 1, p->x + 1)) {
        set(p->y + 0, p->x + 0);
        set(p->y + 0, p->x + 1);
        set(p->y + 1, p->x + 0);
        set(p->y + 1, p->x + 1);
        return true;
    }

    return false;
}

int find_height(u8 map[]) {
    // assert(false && "Not implemented");

    int height = 0;;
    BlockType block = HLINE;
    // int heighest_point = 0;
    // for (int i = 0; i < BLOCKS_COUNT; i++) {
    for (int i = 0; i < 20; i++) {

        // Fall down loop
        bool block_placed = false;

        Pos p = {
            .x = 0,
            // .y = MAX_MAP_HEIGHT - 4, 
            .y = 10
        };
        
        while (!block_placed) {
            char wind = '>'; // something (from input file)
            int shift = 0;
            if (wind == '>')
                shift = 1;
            else if (wind == '<')
                shift = 1;

            p.x += shift;

            bool overlapping = false;
            switch (block) {
                case HLINE: block_placed = hline_overlapping(map, p); break;
                case PLUS:  block_placed = plus_overlapping(map, p);  break;
                case REV_L: block_placed = revl_overlapping(map, p);  break;
                case VLINE: block_placed = vline_overlapping(map, p); break;
                case BOX:   block_placed = box_overlapping(map, p);   break;
                default:    assert(false && "Something went wrong");   break;
            }

            if (overlapping) {
                p.x -= shift;
            }

            switch (block) {
                case HLINE: block_placed = maybe_place_hline(map, &p); break;
                case PLUS:  block_placed = maybe_place_plus(map, &p);  break;
                case REV_L: block_placed = maybe_place_revl(map, &p);  break;
                case VLINE: block_placed = maybe_place_vline(map, &p); break;
                case BOX:   block_placed = maybe_place_box(map, &p);   break;
                default:    assert(false && "Something went wrong");   break;
            }
            
            p.y -= 1;
        }

        print_map(map, 20);

        block += 1;
        if (block > BOX)
            block = HLINE;
    }
    return 0;
}

// TODO: Full ncurses animation
void day17(FILE *input) {
    // Array of bit maps
    // 0000000(0) <- last bit is unused
    u8 map[MAX_MAP_HEIGHT] = {};
    int map_len = 20;

    // map[1] |= 1 << 0;
    //
    // map[0] |= 1 << 0;
    // map[0] |= 1 << 1;
    // map[0] |= 1 << 2;
    // map[0] |= 1 << 3;

    // place_hline(map, 2, 9);
    // place_plus(map, 0, 3);
    // place_revl(map, 1, 0);
    // place_vline(map, 6, 0);
    // place_box(map, 4, 6);
    
    find_height(map);
    print_map(map, map_len);
}
#include "all_days.h"
#include <stdio.h>

#define BLOCKS_COUNT 2022
#define CHAMBER_WIDTH 7
#define MAX_MAP_HEIGHT BLOCKS_COUNT * 4

#define print_bits(value)                                                                          \
    do {                                                                                           \
        int size = sizeof(value);                                                                  \
        void *addr = (char *)&value + size - 1;                                                    \
        while (size > 0) {                                                                         \
            u8 byte = *(char *)addr;                                                               \
            for (int bit_count = 7; bit_count >= 0; bit_count--) {                                 \
                printf("%i", byte & (1 << bit_count) ? 1 : 0);                                     \
            }                                                                                      \
            addr = (char *)addr - 1;                                                               \
            size--;                                                                                \
        }                                                                                          \
        printf("\n");                                                                              \
    } while (0);

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

typedef unsigned char u8;

void print_map(u8 map[], int map_len) {
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
bool hline_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 3 >= 7 ||
        one(p.y + 0, p.x + 0) || one(p.y + 0, p.x + 1) || 
        one(p.y + 0, p.x + 2) || one(p.y + 0, p.x + 3);

}

int maybe_place_hline(u8 map[], const Pos p) {
    if (p.y == 0 || 
        one(p.y + 0, p.x + 0) || one(p.y + 0, p.x + 1) || 
        one(p.y + 0, p.x + 2) || one(p.y + 0, p.x + 3)
    ) {
        set(p.y + 0, p.x + 0);
        set(p.y + 0, p.x + 1);
        set(p.y + 0, p.x + 2);
        set(p.y + 0, p.x + 3);
        return p.y + 1;
    } 

    return -1;
}

/*             .#.
 *             ###
 * pos here -> .#.
 */
bool plus_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 2 >= 7 ||
        one(p.y + 1, p.x + 0) || one(p.y + 0, p.x + 1) || 
        one(p.y + 2, p.x + 1) || one(p.y + 1, p.x + 2);
}

int maybe_place_plus(u8 map[], const Pos p) {
    if (p.y == 0 || 
        one(p.y + 1, p.x + 0) || one(p.y + 0, p.x + 1) || one(p.y + 0, p.x + 2)
    ) {
        set(p.y + 0, p.x + 1);
        set(p.y + 1, p.x + 0);
        set(p.y + 1, p.x + 1);
        set(p.y + 1, p.x + 2);
        set(p.y + 2, p.x + 1);
        return p.y + 3;
    } 

    return -1;
}

/*             #
 *             #
 *             #
 * pos here -> #
 */
bool vline_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 1 >= 7 || 
        one(p.y + 0, p.x + 0) || one(p.y + 1, p.x + 0) ||
        one(p.y + 2, p.x + 0) || one(p.y + 3, p.x + 0);
}

int maybe_place_vline(u8 map[], const Pos p) {
    if (p.y == 0 || one(p.y + 0, p.x + 0)) {
        set(p.y + 0, p.x + 0);
        set(p.y + 1, p.x + 0);
        set(p.y + 2, p.x + 0);
        set(p.y + 3, p.x + 0);
        return p.y + 4;
    } 

    return -1;
}


/*               #
 *               #
 * pos here -> ###
 */
bool revl_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 2 >= 7 || 
        one(p.y + 0, p.x + 0) || one(p.y + 0, p.x + 2) ||
        one(p.y + 1, p.x + 2) || one(p.y + 2, p.x + 2);
}

int maybe_place_revl(u8 map[], const Pos p) {
    if (p.y == 0 || 
        one(p.y + 0, p.x + 0) || one(p.y + 0, p.x + 1) || one(p.y + 0, p.x + 2)
    ) {
        set(p.y + 0, p.x + 0);
        set(p.y + 0, p.x + 1);
        set(p.y + 0, p.x + 2);
        set(p.y + 1, p.x + 2);
        set(p.y + 2, p.x + 2);
        return p.y + 3;
    } 

    return -1;
}

/*             ##
 * pos here -> ##
 */
bool box_overlapping(const u8 map[], const Pos p) {
    return p.x < 0 || p.x + 1 >= 7 || 
        one(p.y + 0, p.x + 0) || one(p.y + 0, p.x + 1) ||
        one(p.y + 1, p.x + 0) || one(p.y + 1, p.x + 1);
}

int maybe_place_box(u8 map[], const Pos p) {
    if (p.y == 0 || one(p.y + 0, p.x) || one(p.y + 0, p.x + 1)) {
        set(p.y + 0, p.x + 0);
        set(p.y + 0, p.x + 1);
        set(p.y + 1, p.x + 0);
        set(p.y + 1, p.x + 1);
        return p.y + 2;
    }

    return -1;
}

int find_height(u8 map[], FILE *input) {
    int height = 0;
    BlockType block = HLINE;
    // int heighest_point = 0;
    for (int i = 0; i < BLOCKS_COUNT; i++) {
        // Fall down loop
        int placed_height = -1;

        Pos p = {
            .x = 2,
            .y = height + 3, 
        };
        
        while (placed_height == -1) {
            char wind = fgetc(input);

            if (feof(input) || wind == '\n'){
                fseek(input, 0, SEEK_SET);
                wind = fgetc(input);
            }
            int shift = 0;

            if (wind == '>')
                shift = 1;
            else if (wind == '<')
                shift = -1;

            p.x += shift;

            bool overlapping = false;
            switch (block) {
                case HLINE: overlapping = hline_overlapping(map, p); break;
                case PLUS:  overlapping = plus_overlapping(map, p);  break;
                case REV_L: overlapping = revl_overlapping(map, p);  break;
                case VLINE: overlapping = vline_overlapping(map, p); break;
                case BOX:   overlapping = box_overlapping(map, p);   break;
                default:    assert(false && "Something went wrong"); break;
            }

            if (overlapping) {
                p.x -= shift;
            }

            switch (block) {
                case HLINE: placed_height = maybe_place_hline(map, p); break;
                case PLUS:  placed_height = maybe_place_plus(map, p);  break;
                case REV_L: placed_height = maybe_place_revl(map, p);  break;
                case VLINE: placed_height = maybe_place_vline(map, p); break;
                case BOX:   placed_height = maybe_place_box(map, p);   break;
                default:    assert(false && "Something went wrong");  break;
            }
            
            p.y -= 1;
        }

        if (placed_height > height) {
            height = placed_height;
        }

        if (height < 20 && placed_height != -1) {
            printf("\n");
            print_map(map, height + 3);
            printf("h = %i\n", height);
        }

        block += 1;
        if (block > BOX)
            block = HLINE;
    }
    return height;
}

// TODO: Full ncurses animation
void day17(FILE *input) {
    // Array of bit maps
    // 0000000(0) <- last bit is unused
    u8 map[MAX_MAP_HEIGHT] = {};
    
    int height = find_height(map, input);

    printf("\n");
    print_map(map, 50);
    // print_map(map, height + 1);
    printf("h = %i\n", height);
}
