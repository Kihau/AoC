#include "all_days.h"

// TODO: Rename Expression to Equation and vice versa

enum OpToken {
    ADD, SUB, MUL, DIV
};

typedef struct {
    u32 base;
    u32 left;
    u32 right;
    enum OpToken op;
} Equation;

typedef struct {
    u32 base;
    u32 value;
} Value;

typedef struct {
    Equation *eq_data;
    int eq_len;

    Value *val_data;
    int val_len;
} Data;

// HASH THIS MORE: This thing is only 4 bytes and is only a-z
static u32 compress_string(char *string) {
    u32 hash = 0;
    hash |= (u32)string[3] << 3 * 8;
    hash |= (u32)string[2] << 2 * 8;
    hash |= (u32)string[1] << 1 * 8;
    hash |= (u32)string[0] << 0 * 8;
    return hash;
}

static Equation parse_equation(char *string) {
    Equation eq = {};
    eq.base = compress_string(string);

    string += 6;
    eq.left = compress_string(string);

    string += 5;
    switch (*string) {
        case '+': eq.op = ADD; break;
        case '-': eq.op = SUB; break;
        case '*': eq.op = MUL; break;
        case '/': eq.op = DIV; break;
        default: assert(false);
    }

    string += 2;
    eq.right = compress_string(string);

    return eq;
}

static Value parse_value(char *string) {
    Value val = {};

    val.base = compress_string(string);

    string += 6;
    val.value = atoi(string);

    return val;
}

static bool is_equation(char *buffer) {
    if (buffer[6] >= '0' && buffer[6] <= '9')
        return false;
    else return true;
}


int find_equation(Equation *eq_tokens, int eq_len, u32 token) {
    for (int i = 0; i < eq_len; i++) {
        if (eq_tokens[i].base == token)
            return i;
    }
    return -1;
}

int find_value(Value *val_tokens, int val_len, u32 token) {
    for (int i = 0; i < val_len; i++) {
        if (val_tokens[i].base == token)
            return i;
    }
    return -1;
}

Equation find_parent(Equation *eq_tokens, int eq_len, u32 token) {
    for (int i = 0; i < eq_len; i++) {
        Equation eq = eq_tokens[i];
        if (eq.right == token || eq.left == token)
            return eq;
    }
    assert(false);
}

long calculate_value(Data *data, u32 token) {
    int eq_idx = find_equation(data->eq_data, data->eq_len, token);
    if (eq_idx == -1) {
        int val_idx = find_value(data->val_data, data->val_len, token);
        return data->val_data[val_idx].value;
    }
    Equation eq = data->eq_data[eq_idx];
    switch (eq.op) {
        case ADD: return calculate_value(data, eq.left) + calculate_value(data, eq.right);
        case SUB: return calculate_value(data, eq.left) - calculate_value(data, eq.right);
        case MUL: return calculate_value(data, eq.left) * calculate_value(data, eq.right);
        case DIV: return calculate_value(data, eq.left) / calculate_value(data, eq.right);
        default: assert(false);
    }
}

bool check_if_human_subnode(Data* data, u32 current, const u32 humn) {
    int eq_idx = find_equation(data->eq_data, data->eq_len, current);
    if (eq_idx == -1) {
        int val_idx = find_value(data->val_data, data->val_len, current);
        u32 found_token = data->val_data[val_idx].base;
        return found_token == humn;
    }

    Equation eq = data->eq_data[eq_idx];
    return check_if_human_subnode(data, eq.right, humn) || check_if_human_subnode(data, eq.left, humn);
}

typedef struct {
    enum OpToken op;
    bool left;
    long value;
} Expression;

long calculate_human(Data *data, u32 root_token) {
    char human_string[] = { 'h', 'u', 'm', 'n' };
    u32 human_token = compress_string(human_string);

    int eq_idx = find_equation(data->eq_data, data->eq_len, root_token);
    Equation eq = data->eq_data[eq_idx];

    bool human_left_side = check_if_human_subnode(data, eq.left, human_token);
    long root_value;
    if (human_left_side) {
        root_value = calculate_value(data, eq.right);
    } else {
        root_value = calculate_value(data, eq.left);
    }

    Expression *math_stack = malloc(sizeof(Expression) * 1024);
    int stack_len = 0;

    // Constructing a math stack
    u32 current = human_token;
    while (current != root_token) {
        Equation parent = find_parent(data->eq_data, data->eq_len, current);
        if (parent.right == current) {
            long child_value = calculate_value(data, parent.left);
            Expression expr = {
                .left = false,
                .value = child_value,
                .op = parent.op
            };
            math_stack[stack_len++] = expr;
        } else {
            long child_value = calculate_value(data, parent.right);
            Expression expr = {
                .left = true,
                .value = child_value,
                .op = parent.op
            };
            math_stack[stack_len++] = expr;
        }

        current = parent.base;
    }

    stack_len -= 1;

    // Poping elements from the stack and calculating the unknown value
    long human_value = root_value;
    while (stack_len > 0) {
        stack_len -= 1;
        Expression expr = math_stack[stack_len];
        printf("Stack - human: %li op: %i value: %li\n", human_value, expr.op, expr.value);

        if (expr.left) {
            switch (expr.op) {
                case ADD: {
                    human_value -= expr.value;
                } break;
                case SUB: {
                    human_value += expr.value;
                } break;
                case MUL: {
                    human_value /= expr.value;
                } break;
                case DIV: {
                    human_value *= expr.value;
                } break;
                default: assert(false);
            }
        } else {
            switch (expr.op) {
                case ADD: {
                    human_value -= expr.value;
                } break;
                case SUB: {
                    human_value = expr.value - human_value;
                } break;
                case MUL: {
                    human_value /= expr.value;
                } break;
                case DIV: {
                    human_value = expr.value / human_value;
                } break;
                default: assert(false);
            }
        }
    }

    free(math_stack);
    return human_value;
}

// TODO: Visual representation of an entire syntax tree
void day21(FILE *input) { 
    Value *value_tokens = malloc(sizeof(Value) * 2048);
    u32 value_len = 0;

    Equation *eq_tokens = malloc(sizeof(Equation) * 2048);
    u32 eq_len = 0;

    char root_string[] = { 'r', 'o', 'o', 't' };
    u32 root_token = compress_string(root_string);

    char buffer[128];
    while (fgets(buffer, sizeof(buffer), input)) {
        if (is_equation(buffer)) {
            Equation eq_token = parse_equation(buffer);

            eq_tokens[eq_len++] = eq_token;
        } else {
            Value val_token = parse_value(buffer);
            value_tokens[value_len++] = val_token;
        }
    }

    Data data = {
        .eq_data = eq_tokens,
        .eq_len = eq_len,
        .val_data = value_tokens,
        .val_len = value_len,
    };
    long output = calculate_value(&data, root_token);
    printf("%li\n", output);
    long human_value = calculate_human(&data, root_token);
    printf("%li\n", human_value);

    free(value_tokens);
    free(eq_tokens);
}
