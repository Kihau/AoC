#include "all_days.h"

#define BLUEPRINTS_CAP 30

typedef struct {
    int ore_cost;
} OreRobot;

typedef struct {
    int ore_cost;
} ClayRobot;

typedef struct {
    int ore_cost; 
    int clay_cost;
} ObsidianRobot;

typedef struct {
    int ore_cost; 
    int obsidian_cost;
} GeodeRobot;

typedef struct {
    OreRobot ore;
    ClayRobot clay;
    ObsidianRobot obs;
    GeodeRobot geo;
} Blueprint;

typedef struct {
    u8 ore;
    u8 clay;
    u8 obs;
    u8 geo;
} Data;

typedef struct {
    Data robots;
    Data resources;
    u8 time;
} State;

#define PARSE_NEXT(number)                                                                         \
    do {                                                                                           \
        while (*buffer < '0' || *buffer > '9')                                                     \
            buffer++;                                                                              \
        char num_buf[8] = {};                                                                      \
        int num_len = 0;                                                                           \
        while (*buffer >= '0' && *buffer <= '9') {                                                 \
            num_buf[num_len++] = *buffer;                                                          \
            buffer++;                                                                              \
        }                                                                                          \
        number = atoi(num_buf);                                                                    \
    } while (0)

static Blueprint parse_blueprint(char *buffer) {
    Blueprint bp = {};

    // Skip to letter E
    while (*buffer != 'E' && *buffer)
        buffer++;

    PARSE_NEXT(bp.ore.ore_cost);
    PARSE_NEXT(bp.clay.ore_cost);

    PARSE_NEXT(bp.obs.ore_cost);
    PARSE_NEXT(bp.obs.clay_cost);

    PARSE_NEXT(bp.geo.ore_cost);
    PARSE_NEXT(bp.geo.obsidian_cost);

    return bp;
}

static void print_blueprints(const Blueprint blueprints[], int bp_len) {
    for (int i = 0; i < bp_len; ++i) {
        printf("Blueprint %i\n", i + 1);

        const Blueprint *bp = &blueprints[i];
        printf("    Each ore robot costs %i ore.\n", bp->ore.ore_cost);
        printf("    Each clay robot costs %i ore.\n", bp->clay.ore_cost);
        printf("    Each obsidian robot costs %i ore and %i clay.\n", bp->obs.ore_cost, bp->obs.clay_cost);
        printf("    Each geode robot costs %i ore and %i obsidian.\n", bp->geo.ore_cost, bp->geo.obsidian_cost);
        printf("\n");
    }
}

static inline int max(int first, int second) {
    return first > second ? first : second;
}

static State next_state(const State *state) {
    State new_state = *state;

    // One minute passes
    new_state.time += 1;

    // Mine ores
    new_state.resources.ore += new_state.robots.ore;
    new_state.resources.clay += new_state.robots.clay;
    new_state.resources.obs += new_state.robots.obs;
    new_state.resources.geo += new_state.robots.geo;

    return new_state;
}

static int calculate_mined_geodes(const Blueprint *bp, int mining_time) {
    int ore_limit = bp->ore.ore_cost;
    ore_limit = max(ore_limit, bp->clay.ore_cost);
    ore_limit = max(ore_limit, bp->obs.ore_cost);
    ore_limit = max(ore_limit, bp->geo.ore_cost);
    int clay_limit = bp->obs.clay_cost;
    int obs_limit = bp->geo.obsidian_cost;

    State *state_stack = malloc(100 * sizeof(State));
    int stack_len = 0;

    int max_geodes = 0;

    State initial_state = { .robots.ore = 1 };
    state_stack[stack_len++] = initial_state;

    while (stack_len > 0) {
        stack_len -= 1;
        State current = state_stack[stack_len];
        // printf(
        //     "%3i %3i %3i %3i %3i\n", 
        //     current.time, current.resources.ore, current.resources.clay, 
        //     current.resources.obs, current.resources.geo
        // );

        // Check if time is equal to 24, do the geode mined comparasion
        // if (current.time == 24) {
        if (current.time == mining_time) {
            // printf("TIME!\n");
            if (current.resources.geo > max_geodes) {
                max_geodes = current.resources.geo;
                // printf("new_max = %i\n", max_geodes);
            }
            continue;
        }

        // One minute passes
        // current.time += 1;

        // Buy all of robots (if possible)
        // State new_state;

        // Mine ores
        // current.resources.ore += current.robots.ore;
        // current.resources.clay += current.robots.clay;
        // current.resources.obs += current.robots.obs;
        // current.resources.geo += current.robots.geo;

        // bool ignore_other_states = current.robots.ore >= ore_limit && 
        //     current.robots.clay >= clay_limit && current.robots.obs >= obs_limit;
        bool ignore_other_states = true;

        // Buying the geode robot
        if (current.resources.ore >= bp->geo.ore_cost && 
            current.resources.obs >= bp->geo.obsidian_cost) {
            State new_state = next_state(&current);

            new_state.robots.geo += 1;
            new_state.resources.ore -= bp->geo.ore_cost;
            new_state.resources.obs -= bp->geo.obsidian_cost;
            state_stack[stack_len++] = new_state;

            if (ignore_other_states)
                continue;
        }

        // Buying the obsidian robot
        if (current.resources.ore >= bp->obs.ore_cost && 
            current.resources.clay >= bp->obs.clay_cost && 
            current.robots.obs < obs_limit) {
            State new_state = next_state(&current);

            new_state.robots.obs += 1;
            new_state.resources.ore -= bp->obs.ore_cost;
            new_state.resources.clay -= bp->obs.clay_cost;
            state_stack[stack_len++] = new_state;

            if (ignore_other_states)
                continue;
        }

        // Buying the clay robot
        if (current.resources.ore >= bp->clay.ore_cost && current.robots.clay < clay_limit) {
            State new_state = next_state(&current);

            new_state.robots.clay += 1;
            new_state.resources.ore -= bp->clay.ore_cost;
            state_stack[stack_len++] = new_state;
        }

        // Buying the ore robot
        if (current.resources.ore >= bp->ore.ore_cost && current.robots.ore < ore_limit) {
            State new_state = next_state(&current);

            new_state.robots.ore += 1;
            new_state.resources.ore -= bp->ore.ore_cost;
            state_stack[stack_len++] = new_state;
        }

        // TODO: Do not add this thing. Instead just add all available states to the stack and keep
        //       in mind time that padded to collect ores in order to create given state
        // Push the state current (wait state)
        state_stack[stack_len++] = next_state(&current);
    }

    free(state_stack);
    return max_geodes;
}

void day19(FILE *input) { 
    Blueprint blueprints[BLUEPRINTS_CAP];
    int bp_len = 0;
    char buffer[256];

    // Parse the blueprints from the input file
    while (fgets(buffer, sizeof(buffer), input)) {
        blueprints[bp_len++] = parse_blueprint(buffer);
    }

    print_blueprints(blueprints, bp_len);

    // Part 1
    int quality_sum_part1 = 0;
    for (int i = 0; i < bp_len; i++) {
        int ores_mined = calculate_mined_geodes(&blueprints[i], 24);
        // printf("Mined %i = %i\n", i + 1, ores_mined);
        quality_sum_part1 += ores_mined * (i + 1);
    }

    // Part 2
    // int quality_sum_part2 = 0;
    // for (int i = 0; i < 3 && i < bp_len; i++) {
    //     int ores_mined = calculate_mined_geodes(&blueprints[i], 32);
    //     // printf("Mined %i = %i\n", i + 1, ores_mined);
    //     quality_sum_part2 *= ores_mined;
    // }

    printf("PART1: Quality level of all blueprints = %i\n", quality_sum_part1);
    // printf("PART2: Quality level of all blueprints = %i\n", quality_sum_part2);
}
