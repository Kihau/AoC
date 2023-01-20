#include "all_days.h"
#include <stdio.h>
#include <string.h>

typedef struct {
    char name[3];
    int flow_rate;
    // Don't have this. Store realased flow_rates as a separeate value and increment it every minute
    // bool released;
    int connections[16];
    int travel_times[16];
    int conn_len;

} Valve;

static int valve_index(const Valve valves[], const int valves_len, char name[2]) {
    for (int i = 0; i < valves_len; i++) {
        if (valves[i].name[0] == name[0] && valves[i].name[1] == name[1])
            return i;
    }
    return -1;
}

static void parse_valve(Valve valves[], int *valves_len, char *buffer) {
    // +0 v_____v +6
    //    Valve AA .....
    buffer += 6;
    int index = valve_index(valves, *valves_len, buffer);

    // Valve not found, add a new valve
    if (index == -1) {
        Valve v = {};
        v.name[0] = buffer[0];
        v.name[1] = buffer[1];

        index = *valves_len;
        valves[(*valves_len)++] = v;
    } 

    //    +0 v_______________v +16
    // ..... AA has flow rate=0; .....
    buffer += 16;

    char num_buf[8] = {};
    int num_i = 0;
    while (*buffer++ != ';')
        num_buf[num_i++] = *buffer;
    valves[index].flow_rate = atoi(num_buf);

    //            +0 v______________________V +23
    //  ..... rate=0; tunnels lead to valves DD, II, BB
    //  ..... rate=0; tunnel leads to valve DD
    buffer += 23;
    if (*buffer == ' ')
        buffer++;
    
    while (true) {
        int con_index = valve_index(valves, *valves_len, buffer);
        if (con_index == -1) {
            Valve v = {};
            v.name[0] = buffer[0];
            v.name[1] = buffer[1];

            con_index = *valves_len;
            valves[(*valves_len)++] = v;
        } 

        Valve *v = &valves[index];
        v->travel_times[v->conn_len] = 1;
        v->connections[v->conn_len++] = con_index;

        if (buffer[2] != ',')
            break;

        buffer += 4;
    }
}

// Remove all 0 flow nodes, find all path length from all non zero flow valves to closest non zero flow valves
static void simplify_paths_closest(Valve **valves, int *valves_len, int start) {
    Valve *v = *valves;

    Valve *new_valves = calloc(64, sizeof(Valve));
    int new_len = 0;

    int *visited = malloc(64 * sizeof(int));
    int *queue = malloc(64 * sizeof(int));

    int *map = malloc(64 * sizeof(int));
    {
        int map_idx = 0;
        for (int i = 0; i < *valves_len; i++) {
            if (v[i].flow_rate > 0 || i == start) {
                map[i] = map_idx++;
            }
        }
    }

    for (int i = 0; i < *valves_len; i++) {
        if (v[i].flow_rate != 0 || i == start) {
            strcpy(new_valves[new_len].name, v[i].name);
            new_valves[new_len].flow_rate = v[i].flow_rate;
            new_len++;

            memset(visited, -1, 64 * sizeof(int));
            visited[i] = 1;

            int start = 0;
            int end = 0;

            queue[end++] = i;

            while (start != end) {
                int vdx = queue[start++];

                for (int j = 0; j < v[vdx].conn_len; j++) {
                    int con = v[vdx].connections[j];
                    if (visited[con] == -1) {
                        visited[con] = visited[vdx] + 1;
                        
                        if (v[con].flow_rate > 0) {
                            Valve *nv = &new_valves[map[i]];
                            nv->connections[nv->conn_len] = map[con];
                            nv->travel_times[nv->conn_len] = visited[vdx];
                            nv->conn_len++;
                        } else queue[end++] = con; // The search only continues when the closest
                    }                              // connected non-zero flow node wasn't reached
                }
            }
        }
    }
    
    free(map);
    free(queue);
    free(visited);
    free(v);
    *valves_len = new_len;
    *valves = new_valves;
}

// Remove all 0 flow nodes, find all path length from all non zero flow  valves to all non zero flow valves
static void simplify_paths_all(Valve **valves, int *valves_len, int start) {
    Valve *v = *valves;

    // There is no need to allocate this here. Just use the original array.
    Valve *new_valves = calloc(64, sizeof(Valve));
    int new_len = 0;

    int *visited = malloc(64 * sizeof(int));
    int *queue = malloc(64 * sizeof(int));

    int *map = malloc(64 * sizeof(int));
    {
        int map_idx = 0;
        for (int i = 0; i < *valves_len; i++) {
            if (v[i].flow_rate > 0 || i == start) {
                map[i] = map_idx++;
            }
        }
    }

    for (int i = 0; i < *valves_len; i++) {
        // Skip all 0 flow valves that are not a starting point
        if (v[i].flow_rate == 0 && i != start) {
            continue;
        }

        strcpy(new_valves[new_len].name, v[i].name);
        new_valves[new_len].flow_rate = v[i].flow_rate;
        new_len++;

        memset(visited, -1, 64 * sizeof(int));
        visited[i] = 1;

        int start = 0;
        int end = 0;

        queue[end++] = i;

        while (start != end) {
            int vdx = queue[start++];

            for (int j = 0; j < v[vdx].conn_len; j++) {
                int con = v[vdx].connections[j];
                if (visited[con] != -1) {
                    continue;
                }

                visited[con] = visited[vdx] + 1;
                queue[end++] = con;

                if (v[con].flow_rate <= 0) {
                    continue;
                }

                Valve *nv = &new_valves[map[i]];
                nv->connections[nv->conn_len] = map[con];
                nv->travel_times[nv->conn_len] = visited[vdx];
                nv->conn_len++;
            }
        }
    }

    free(map);
    free(queue);
    free(visited);
    free(v);
    *valves_len = new_len;
    *valves = new_valves;
}

// Uses graphviz to generate a visual graph
void generate_graph(const Valve valves[], int valves_len, int start, const char *output, bool bigraph) {
#define BIGRAPH false
    FILE *graph = fopen("graph.dot", "w");
    assert(graph && "Something went wrong");

    fprintf(graph, "digraph Valves {\n");
    fprintf(graph, "\tSTART [label=\"\", shape=none, height=.0, width=.0]\n");

    for (int i = 0; i < valves_len; i++) {
        Valve v = valves[i];
        if (v.flow_rate == 0)
            fprintf(graph, "\t%s [label=\"%s\"]\n", v.name, v.name);
        else fprintf(graph, "\t%s [label=\"%s = %i\" fontcolor=red]\n", v.name, v.name, v.flow_rate);
    }

    fprintf(graph, "\tSTART -> %s [label = \" START \"]\n", valves[start].name);

    bool *print_flags = calloc(valves_len, sizeof(bool));
    for (int i = 0; i < valves_len; i++) {
        Valve v = valves[i];
        for (int j = 0; j < v.conn_len; j++) {
            if (bigraph) {
                // Bidirectional arrows
                if (v.connections[j] > i) {
                    Valve c = valves[v.connections[j]];
                    if (i != 0)
                        fprintf(graph, "\t%s -> %s [dir=both label=%i]\n", v.name, c.name, v.travel_times[j]);
                    else 
                        fprintf(graph, "\t%s -> %s [label=%i]\n", v.name, c.name, v.travel_times[j]);
                }
            } else {
                // One directional arrows
                Valve c = valves[v.connections[j]];
                fprintf(graph, "\t%s -> %s [label=%i]\n", v.name, c.name, v.travel_times[j]);
            }
        }
    }
    fprintf(graph, "}");

    fclose(graph);
    free(print_flags);

    char buffer[128];
    snprintf(buffer, sizeof(buffer), "dot -Tpng graph.dot > %s", output);
    system(buffer);
    // system("dot -Tpng graph.dot > graph.png");
}

void print_valves(const Valve valves[], int valves_len) {
    for (int i = 0; i < valves_len; i++) {
        Valve v = valves[i];
        printf("name: %s, flow: %02i, conns: [", v.name, v.flow_rate);
        for (int j = 0; j < v.conn_len - 1; j++) {
            int c = v.connections[j];
            // printf("%s: %i, ", valves[c].name, v.travel_times[j]);
            printf("%s, ", valves[c].name);
            // printf("%i, ", v.connections[j]);
        }

        int c = v.connections[v.conn_len - 1];
        // printf("%s: %i]\n", valves[c].name, v.travel_times[v.conn_len - 1]);
        printf("%s]\n", valves[c].name);
        // printf("%i]\n", v.connections[v.conn_len - 1]);
    }
    printf("\n");
}

typedef struct {
    int index;
    int time;
    int opened_valves;
    int pressure;
} SearchState;

int search_path(const Valve valves[], SearchState *search_stack, int stack_size, int max_time) {
    int max_pressure = 0;

    while (stack_size != 0) {
        stack_size -= 1;
        SearchState state = search_stack[stack_size];

        if (state.time == max_time) {
            if (state.pressure > max_pressure)
                max_pressure = state.pressure;
            continue;
        }

        Valve v = valves[state.index];
        if ((state.opened_valves & (1 << state.index)) == 0) {
            state.opened_valves |= (1 << state.index);
            state.pressure += (max_time - state.time) * v.flow_rate;
        }

        int size_before = stack_size;
        for (int i = 0; i < v.conn_len; i++) {
            SearchState new_state = state;

            int con_time = v.travel_times[i] + 1;
            int con_idx = v.connections[i];

            if (con_time + new_state.time >= max_time) {
                continue;
            }

            new_state.time += con_time;
            new_state.index = con_idx;

            search_stack[stack_size++] = new_state;
        }

        if (size_before == stack_size) {
            state.time = max_time;
            search_stack[stack_size++] = state;
        }
    }

    return max_pressure;
}

int find_maximum_pressure(const Valve valves[], int start, int max_time) {
    SearchState search_stack[64];
    int stack_size = 0;

    int max_pressure = 0;

    Valve v = valves[start];
    for (int i = 0; i < v.conn_len; i++) {
        SearchState initial_state =  {
            .index = v.connections[i],
            // Travel time + valve open time, since I always want to open the valves (all-to-all)
            .time = v.travel_times[i] + 1,
            .opened_valves = 0,
            .pressure = 0,
        };

        stack_size = 0;
        search_stack[stack_size++] = initial_state;

        int pressure = search_path(valves, search_stack, stack_size, max_time);
        if (pressure > max_pressure) {
            // printf("hej: %i %i!\n", v.connections[i], v.travel_times[i] + 1);
            max_pressure = pressure;
        }

    }

    return max_pressure;
}

void trace_valves(const Valve valves[], int start, int time) {
    SearchState trace[16] = {};
    int trace_len = 1;

    int max_pressure = find_maximum_pressure(valves, start, 30);

    SearchState search_stack[64];
    int stack_size = 0;

    while (trace[trace_len - 1].time < 30) {
        SearchState current = trace[trace_len - 1];
        Valve v = valves[current.index];

        SearchState max_state = current;
        max_state.pressure = 0;

        for (int i = 0; i < v.conn_len; i++) {
            SearchState traced_state =  {
                .index = v.connections[i],
                // Travel time + valve open time, since I always want to open the valves (all-to-all)
                .time = v.travel_times[i] + 1 + current.time,
                .opened_valves = current.opened_valves,
                .pressure = 0,
            };

            stack_size = 0;
            search_stack[stack_size++] = traced_state;

            int pressure = search_path(valves, search_stack, stack_size, time);
            if (pressure > max_state.pressure) {
                max_state = current;
                max_state.pressure = pressure;
                max_state.time += traced_state.time;
                max_state.index = traced_state.index;
                max_state.opened_valves |= (1 << traced_state.index);
            }
        }

        printf("%i ", max_pressure - max_state.pressure);
        trace[trace_len++] = max_state;
    }
    printf("\n");

    printf("Valves trace = [");
    for (int i = 1; i < trace_len - 1; i++) {
        printf("%s, ", valves[trace[i].index].name);
    }
    printf("%s]\n", valves[trace[trace_len - 1].index].name);
}

int find_maximum_pressure_old(const Valve valves[], int start, int max_time) {
    SearchState search_stack[64];
    // SearchState *search_stack = malloc(64 * sizeof(SearchState));
    int stack_len = 0;

    SearchState initial_state =  {
        .index = start,
        .time = 0,
        .pressure = 0,
        .opened_valves = 0,
    };
    search_stack[stack_len++] = initial_state;

    int max_pressure = 0;
    while (stack_len != 0) {
        // Pop the element from the stack
        stack_len -= 1;
        const SearchState state = search_stack[stack_len];

        if (state.time == max_time) {
            if (state.pressure > max_pressure)
                max_pressure = state.pressure;
            continue;
        }

        int len_before = stack_len;
        Valve v = valves[state.index];
        for (int i = 0; i < v.conn_len; i++) {
            SearchState new_state = state;

            int con_idx = v.connections[i];
            // Trave time + time required to open the valve (1 minute)
            int con_time = v.travel_times[i] + 1;
            Valve con = valves[con_idx];

            if (state.opened_valves & (1 << con_idx) || state.time + con_time >= max_time) {
                continue;
            }

            new_state.index = con_idx;
            new_state.time += con_time;
            new_state.opened_valves |= (1 << con_idx);
            new_state.pressure += (max_time - new_state.time) * con.flow_rate;

            search_stack[stack_len++] = new_state;
        }

        // If no new states were added to the stack, mark previous state as "finished" and push it
        // onto the stack
        if (stack_len == len_before) {
            SearchState new_state = state;
            new_state.time = max_time;
            search_stack[stack_len++] = new_state;
        }
    }

    return max_pressure;
}

void day16(FILE *input) {
    Valve *valves = malloc(sizeof(Valve) * 64);
    int valves_len = 0;

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), input)) {
        parse_valve(valves, &valves_len, buffer);
    }

    int start = 0;
    for (int i = 0; i < valves_len; i++) {
        Valve v = valves[i];
        if (v.name[0] == 'A' && v.name[1] == 'A') {
            start = i;
            break;
        }
    }

    // printf("Original graph:\n");
    // print_valves(valves, valves_len);
    generate_graph(valves, valves_len, start, "graph.png", true);

    printf("Simplified closest node graph:\n");
    simplify_paths_closest(&valves, &valves_len, start);
    for (int i = 0; i < valves_len; i++) {
        Valve v = valves[i];
        if (v.name[0] == 'A' && v.name[1] == 'A') {
            start = i;
            break;
        }
    }
    generate_graph(valves, valves_len, start, "graph-close.png", true);
    print_valves(valves, valves_len);

    // printf("Simplified all-to-all graph:\n");
    // simplify_paths_all(&valves, &valves_len, start);
    // for (int i = 0; i < valves_len; i++) {
    //     Valve v = valves[i];
    //     if (v.name[0] == 'A' && v.name[1] == 'A') {
    //         start = i;
    //         break;
    //     }
    // }
    // print_valves(valves, valves_len);
    // generate_graph(valves, valves_len, start, "graph-all.png", true);
    
    int part1 = find_maximum_pressure(valves, start, 30);
    // int part2 = find_maximum_pressure(valves, start, 26);
    printf("PART1: Max pressure = %i\n", part1);

    free(valves);
}
