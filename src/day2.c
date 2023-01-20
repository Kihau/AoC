#include "all_days.h"

/* Rules:
 *    Scores:
 *    lose - 0 points
 *    draw - 3 points
 *    win  - 6 points
 *    
 *    Opponent elf:
 *    A - rock
 *    B - paper
 *    C - scissors
 *    
 *    You, The player:
 *    X - rock, 1 point
 *    Y - paper, 2 points
 *    Z - scissors, 3 points
 *    
 *    Part 2:
 *    X - Lose
 *    Y - Draw
 *    Z - Win
 */

typedef enum {
    WIN  = 6,
    DRAW = 3,
    LOSE = 0,
} RESULT;

typedef enum {
    ROCK     = 1,
    PAPER    = 2,
    SCISSORS = 3,
} MOVE;

static RESULT find_result(MOVE opponent, MOVE player) {
    switch (opponent) {
        case ROCK: {
            switch (player) {
                case ROCK:     return DRAW;
                case PAPER:    return WIN;
                case SCISSORS: return LOSE;
            }
        } break;
        case PAPER: {
            switch (player) {
                case ROCK:     return LOSE;
                case PAPER:    return DRAW;
                case SCISSORS: return WIN;
            }
        } break;
        case SCISSORS: {
            switch (player) {
                case ROCK:     return WIN;
                case PAPER:    return LOSE;
                case SCISSORS: return DRAW;
            }
        } break;
    }
    assert(false && "Invalid move. Was the input correct?");
} 

static MOVE make_move(RESULT result, MOVE opponent) {
    switch (result) {
        case WIN: {
            switch (opponent) {
                case ROCK:     return PAPER;
                case PAPER:    return SCISSORS;
                case SCISSORS: return ROCK;
            }
        } break;
        case DRAW: return opponent; break;
        case LOSE: {
            switch (opponent) {
                case ROCK:     return SCISSORS;
                case PAPER:    return ROCK;
                case SCISSORS: return PAPER;
            }
        } break;
    }
    assert(false && "Invalid move (or result). Was the input correct?");
} 

void day2(FILE *input) {
    char buffer[8];

    int player_score1 = 0, player_score2 = 0;

    while (fgets(buffer, sizeof(buffer), input)) {
        MOVE opponent_move;
        switch (buffer[0]) {
            case 'A': opponent_move = ROCK; break;
            case 'B': opponent_move = PAPER; break;
            case 'C': opponent_move = SCISSORS; break;
        }

        MOVE player_move1;
        switch (buffer[2]) {
            case 'X': player_move1 = ROCK; break;
            case 'Y': player_move1 = PAPER; break;
            case 'Z': player_move1 = SCISSORS; break;
        }

        // Part 1
        RESULT result1 = find_result(opponent_move, player_move1);
        player_score1 += result1;
        player_score1 += player_move1;

        // Part 2
        RESULT result2;
        switch (buffer[2]) {
            case 'X': result2 = LOSE; break;
            case 'Y': result2 = DRAW; break;
            case 'Z': result2 = WIN; break;
        }

        MOVE player_move2 = make_move(result2, opponent_move);
        player_score2 += result2;
        player_score2 += player_move2;
    }

    printf("PART1: Player score = %i\n", player_score1);
    printf("PART2: Player score = %i\n", player_score2);
}
