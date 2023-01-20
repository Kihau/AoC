#include "all_days.h"
#include <stdio.h>

#define MAX_SUBPACKETS 8

typedef enum {
    RIGHT_ORDER,
    WRONG_ORDER,
    UNDETERMINED,
} COMP_RES;

enum PacketType {
    VALUE, SUBPACKET
};

union PacketContent {
    int value;
    struct Packet *subpacket;
};

typedef struct Packet {
    enum PacketType type[MAX_SUBPACKETS];
    union PacketContent content[MAX_SUBPACKETS];
    int len;
} Packet;

static Packet *parse_packet(char **buffer) { 
    Packet *packet = malloc(sizeof(Packet));
    packet->len = 0;

    char value_buf[8] = {};
    int index = 0;

    while (**buffer) {
        char c = *(*buffer)++;
        if (c == '[') {
            // Start a new sub packet 
            Packet *subpacket = parse_packet(buffer);

            packet->content[packet->len].subpacket = subpacket;
            packet->type[packet->len] = SUBPACKET;
            packet->len++;

        } else if (c >= '0' && c <= '9') {
            // Add number to number atoi buffa thing
            value_buf[index++] = c;
        } else if (c == ',' && index != 0) {
            // Append \0 and parse the number
            value_buf[index] = 0;
            index = 0;

            assert(packet->len < MAX_SUBPACKETS && "Index out of bounds");

            packet->content[packet->len].value = atoi(value_buf);
            packet->type[packet->len] = VALUE;
            packet->len++;
        } else if (c == ']') {
            if (index == 0)
                return packet;

            // Append \0, parse the last number and end the packet
            value_buf[index] = 0;

            assert(packet->len < MAX_SUBPACKETS && "Index out of bounds");

            packet->content[packet->len].value = atoi(value_buf);
            packet->type[packet->len] = VALUE;
            packet->len++;

            return packet;
        }
    }

    assert(false && "How did you even get here? Was the input correct?");
}

static void free_packet(Packet packet[]) {
    for (int i = 0; i < packet->len; i++) {
        if (packet->type[i] == SUBPACKET) {
            free_packet(packet->content[i].subpacket);
        }
    }
    free(packet);
}

/* FRISK:
[6,[6,[1,8],[8,10,0,8,5]],[[7],7]],[[],0,[[8,9,8],1,3,[]]]
[[[6,2,6,4,5],3,[0],[9]],4],[],[5,1,[[]],[3,[10,1,10,5,10]]]
*/

/*
- [[5,[[0],6,[7,8,7,5],[4,8,7,7],10],[0,9,[4,9,9,6,3],[6],4],[[8,9],3,[]]],[[]],[0,[[10,10,5,8,5],2,7,0,[3]],[2,[4,6,5,1,6],[7,10,10,4],7]]] vs [[5,0],[[[6,3,5],[3],[8,1,5],5,9],[6],[2,0],2,[10]],[]]
    - [5, [[0],6,[7,8,7,5],[4,8,7,7],10], [0,9,[4,9,9,6,3],[6],4], [[8,9],3,[]]] vs [5, 0]
        - 5 vs 5
        - 0 vs [[0],6,[7,8,7,5],[4,8,7,7],10] 
            - 0 vs [0]
            - [0] vs [0]
            - 0 vs 0
        - [5, 0] run out of items
*/

static COMP_RES compare_packets(Packet *p1, Packet *p2) { 
    int i = 0;
    while (true) {
        if (i == p1->len && i == p2->len)
            return UNDETERMINED;
        else if (i == p1->len)
            return RIGHT_ORDER;
        else if (i == p2->len)
            return WRONG_ORDER;
        
        COMP_RES res = UNDETERMINED;
        if (p1->type[i] == VALUE && p2->type[i] == VALUE) {
            if (p1->content[i].value > p2->content[i].value) {
                return WRONG_ORDER;
            } else if (p1->content[i].value < p2->content[i].value) {
                return RIGHT_ORDER;
            }
        } else if (p1->type[i] == VALUE && p2->type[i] == SUBPACKET) {
            Packet temp;
            temp.len = 1,
            temp.type[0] = VALUE;
            temp.content[0].value = p1->content[i].value;

            res = compare_packets(&temp, p2->content[i].subpacket);
        } else if (p1->type[i] == SUBPACKET && p2->type[i] == VALUE) {
            Packet temp;
            temp.len = 1,
            temp.type[0] = VALUE;
            temp.content[0].value = p2->content[i].value;

            res = compare_packets(p1->content[i].subpacket, &temp);
        } else if (p1->type[i] == SUBPACKET && p2->type[i] == SUBPACKET) {
            res = compare_packets(p1->content[i].subpacket, p2->content[i].subpacket);
        }

        if (res != UNDETERMINED)
            return res;
        i++;
    }
    assert(false && "Unreachable code detected");
}

static int perfnt_sort(Packet *packets[], int len) {
    int first_pos = len - 1;
    int second_pos = len - 2;
    for (int i = 0; i < len; i++) {
        for (int j = 0; j < len; j++) {
            if (compare_packets(packets[i], packets[j]) != WRONG_ORDER) {
                if (first_pos == i) {
                    first_pos = j;
                } else if (first_pos == j) {
                    first_pos = i;
                }

                if (second_pos == i) {
                    second_pos = j;
                } else if (second_pos == j) {
                    second_pos = i;
                }

                Packet *temp = packets[i];
                packets[i] = packets[j];
                packets[j] = temp;
            }
        }
    }
    return (first_pos + 1) * (second_pos + 1);
}

void day13(FILE *input) { 
    // Indices of packet pairs start at 1;
    int index = 1;
    int sum_of_indices = 0;

    Packet **packets = malloc(sizeof(Packet *) * 302);
    int packets_count = 0;

    char buffer[512] = {};
    do {
        fgets(buffer, sizeof(buffer), input);
        // TODO: This is just crap - instead have parse_packet and parse_subpacket?
        // Ignore first character (assumming it is the square bracket)
        char *buf_ptr = buffer + 1;
        Packet *p1 = parse_packet(&buf_ptr);

        fgets(buffer, sizeof(buffer), input);
        // Ignore first character (assumming it is the square bracket)
        buf_ptr = buffer + 1;
        Packet *p2 = parse_packet(&buf_ptr);

        packets[packets_count++] = p1;
        packets[packets_count++] = p2;

        COMP_RES result = compare_packets(p1, p2);
        if (result != WRONG_ORDER) 
            sum_of_indices += index; 
        index++;
    } while (fgets(buffer, sizeof(buffer), input));

    char *first = "[6]]";
    packets[packets_count++] = parse_packet(&first);
    char *second = "[2]]";
    packets[packets_count++] = parse_packet(&second);

    int decoder_key = perfnt_sort(packets, packets_count);
    // int decoder_key;

    for (int i = 0; i < packets_count; i++) {
        free_packet(packets[i]);
    }
    free(packets);

    printf("PART 1: Sum of indices = %i\n", sum_of_indices);
    printf("PART 2: Decoder key = %i\n", decoder_key);
}
