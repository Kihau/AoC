#include "all_days.h"

typedef unsigned char u8;

typedef struct {
    int x;
    int y;
    int z;
} Cube;

#define PARSE_NUMBER(number, end)                                                                  \
    do {                                                                                           \
        char num_buf[8] = {};                                                                      \
        int num_len = 0;                                                                           \
        while (*buffer != end) {                                                                   \
            num_buf[num_len++] = *buffer;                                                          \
            buffer++;                                                                              \
        }                                                                                          \
        number = atoi(num_buf);                                                                    \
        buffer++;                                                                                  \
    } while (0)

static Cube parse_cube(char *buffer) {
    Cube cube;
    PARSE_NUMBER(cube.x, ',');
    cube.x += 1;
    PARSE_NUMBER(cube.y, ',');
    cube.y += 1;
    PARSE_NUMBER(cube.z, '\n');
    cube.z += 1;
    return cube;
}

static u8 *create_cube_bitmap() {
    // Do this assert for the get and set (and disable in opt build?)
    // assert(
    //     max_size <= 31 &&
    //     "Constrains of the data structure forbid allocating more than 5 bits per cube dimention"
    // );

    // Preallocate enough data to fill all cubes (since the data set is small)
    // For the input data set, cube count is not larger than 9261 (which would be around 1kB),
    // but enought memory needs to be allocated to fit all 15 bits onto the bitmap of size 
    u8 *cubes = calloc(4096, 1);
    return cubes;
}

static inline void coords_set(u8 *cube_bitmap, const int x, const int y, const int z) {
    int cube_code = 0;

    // Packing the cube onto 15 bits (since it looks like the maximum value is no bigger than 22)
    // 16   8    4    2    1   =   31 (max)
    // 0    0    0    0    0
    cube_code |= x << 0;
    cube_code |= y << 5;
    cube_code |= z << 10;

    // Divide index by 8 (index rounded down)
    int index = cube_code >> 3;
    // Multiply back by8 to get the bit
    int bit = cube_code - (index << 3);
    // Stored in back of 8 bits
    // 14 -> 14 >> 3 = 1 -> map[1] & (1 << ) -> 
    // 19 -> 19 >> 3 = 2 -> 2 << 3 = 16 -> 19 - 16 - 1 = 2 -> map[2] & (1 << 2)
    cube_bitmap[index] |= (1 << bit);
}

static inline void cubes_set(u8 *cube_bitmap, const Cube cube) {
    coords_set(cube_bitmap, cube.x, cube.y, cube.z);
}


static inline bool coords_get(const u8 *cube_bitmap, const int x, const int y, const int z) {
    int cube_code = 0;
    // 000000000000000
    // 000000000011111 << 0
    // 000001111100000 << 5
    // 111110000000000 << 10
    cube_code |= x << 0;
    cube_code |= y << 5;
    cube_code |= z << 10;

    // Divide index by 8 (index rounded down)
    int index = cube_code >> 3;
    int bit = cube_code - (index << 3);
    // 00000100 00000000 00000000
    // 19 -> 19 >> 3 = 2 -> 2 << 3 = 16 -> 19 - 16 - 1 = 2 -> map[2] & (1 << 2)
    return cube_bitmap[index] & (1 << bit);
}

static inline bool cubes_get(const u8 *cube_bitmap, const Cube cube) {
    return coords_get(cube_bitmap, cube.x, cube.y, cube.z);
}

#define CHECK_ADD_CUBE(cube)                                                                       \
    do {                                                                                           \
        /* This may seem un-optimal, but it probably gets opt out, idk, need to check */           \
        if (cube.x <= max.x && cube.y <= max.y && cube.z <= max.z && cube.x >= min.x &&            \
            cube.y >= min.y && cube.z >= min.z) {                                                  \
            if (coords_get(cube_bitmap, cube.x, cube.y, cube.z)) {                                 \
                surface_area += 1;                                                                 \
            } else if (!coords_get(cubes_visited, cube.x, cube.y, cube.z)) {                       \
                coords_set(cubes_visited, cube.x, cube.y, cube.z);                                 \
                cube_queue[end++] = cube;                                                          \
            }                                                                                      \
        }                                                                                          \
    } while (0)

// Calculate surface area exposed to outside
static int total_surface_area(const u8 *cube_bitmap, const Cube min, const Cube max) {
    int surface_area = 0;

    // TODO: PERF: Don't store full cubes (2 byte compression) and make it a ring buffer
    Cube *cube_queue = malloc(10000 * sizeof(Cube));
    int start = 0;
    int end = 0;

    u8 *cubes_visited = create_cube_bitmap();

    Cube curr = { 0, 0, 0 };
    cube_queue[end++] = curr;
    coords_set(cubes_visited, curr.x, curr.y, curr.y);

    while (start != end) {
        curr = cube_queue[start++];

        Cube right = { curr.x + 1, curr.y, curr.z };
        CHECK_ADD_CUBE(right);

        Cube left  = { curr.x - 1, curr.y, curr.z };
        CHECK_ADD_CUBE(left);

        Cube up    = { curr.x, curr.y + 1, curr.z };
        CHECK_ADD_CUBE(up);

        Cube down  = { curr.x, curr.y - 1, curr.z };
        CHECK_ADD_CUBE(down);

        Cube back  = { curr.x, curr.y, curr.z + 1 };
        CHECK_ADD_CUBE(back);

        Cube front = { curr.x, curr.y, curr.z - 1 };
        CHECK_ADD_CUBE(front);
    }

    free(cubes_visited);
    free(cube_queue);
    return surface_area;
}

// Calculate surface area for one cube
static int cube_outside_area(const u8 *cube_bitmap, const Cube c) {
    int cube_area = 6;

    cube_area -= coords_get(cube_bitmap, c.x + 1, c.y, c.z);
    cube_area -= coords_get(cube_bitmap, c.x - 1, c.y, c.z);
    cube_area -= coords_get(cube_bitmap, c.x, c.y + 1, c.z);
    cube_area -= coords_get(cube_bitmap, c.x, c.y - 1, c.z);
    cube_area -= coords_get(cube_bitmap, c.x, c.y, c.z + 1);
    cube_area -= coords_get(cube_bitmap, c.x, c.y, c.z - 1);

    return cube_area;
}

static Cube cube_max(Cube cube1, Cube cube2) {
    Cube max = {
        .x = cube1.x > cube2.x ? cube1.x : cube2.x,
        .y = cube1.y > cube2.y ? cube1.y : cube2.y,
        .z = cube1.z > cube2.z ? cube1.z : cube2.z,
    };
    return max;
}

static Cube cube_min(Cube cube1, Cube cube2) {
    Cube min = {
        .x = cube1.x < cube2.x ? cube1.x : cube2.x,
        .y = cube1.y < cube2.y ? cube1.y : cube2.y,
        .z = cube1.z < cube2.z ? cube1.z : cube2.z,
    };
    return min;
}

void day18(FILE *input) { 
    int total_area = 0;
    int surface_area = 0;
    u8 *cube_bitmap = create_cube_bitmap();
    Cube max = { 0, 0, 0 };
    Cube min = { INT32_MAX, INT32_MAX, INT32_MAX };
    char buffer[20];

    while (fgets(buffer, sizeof(buffer), input)) {
        Cube cube = parse_cube(buffer);
        // TODO(?): assert to check if smaller than 32
        max = cube_max(max, cube);
        min = cube_min(min, cube);
        int area = cube_outside_area(cube_bitmap, cube);
        total_area += area * 2 - 6;
        cubes_set(cube_bitmap, cube);
    }

    max.x += 1;
    max.y += 1;
    max.z += 1;

    min.x -= 1;
    min.y -= 1;
    min.z -= 1;
    surface_area = total_surface_area(cube_bitmap, min, max);

    free(cube_bitmap);

    printf("PART1: Total area = %i\n", total_area);
    printf("PART2: Total surface area = %i\n", surface_area);
}
