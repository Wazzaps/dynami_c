#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "dynamic.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

struct dc_variable {
    size_t value;
};

struct dc_state {
    struct dc_variable vars[16];
};

static const char* find_eol(const char* code) {
    while(*code != '\n' && *code != '\0') {
        code++;
    }
    return code;
}

static const char* find_eow(const char* word) {
    while(*word != '\n' && *word != '\0' && *word != ' ') {
        word++;
    }
    return word;
}

//static const char* find_setter(const char* word) {
//    while(*word != '\n' && *word != '\0' && *word != ' ' && *word != '=') {
//        word++;
//    }
//    return *word == '=' ? word + 1 : NULL;
//}
//
//static bool parse_literal(struct dc_state* state, const char* lit, size_t lit_len, size_t* target) {
//    if (lit_len == 1 && lit[0] == '?') {
//        *target = state->last_call_retval;
//        return true;
//    } else if (lit_len > 2) {
//        if (lit[0] == '0' && lit[1] == 'x') {
//            size_t val = 0;
//            for (size_t i = 2; i < lit_len; i++) {
//                val <<= 4u;
//                if (lit[i] >= '0' && lit[i] <= '9') {
//                    val |= (unsigned int) lit[i] - '0';
//                } else if (lit[i] >= 'a' && lit[i] <= 'f') {
//                    val |= (unsigned int) lit[i] - 'a' + 10;
//                } else if (lit[i] >= 'A' && lit[i] <= 'F') {
//                    val |= (unsigned int) lit[i] - 'A' + 10;
//                } else {
//                    return false;
//                }
//            }
//            *target = val;
//            return true;
//        }
//    }
//    return false;
//}

char* strsplit(char* str, char chr) {
    while (*str != chr && *str != '\0') {
        str++;
    }
    if (*str == '\0') {
        return NULL;
    } else {
        *str = '\0';
        return str + 1;
    }
}

#define HEXDIGIT_TO_INT(i) (((i) >= '0' && (i) <= '9') ? (i) - '0' : (i) - 'a' + 10u)

void parse_literal(struct dc_variable* dst_var, char* expr, struct dc_state* state) {
    if (expr[0] == '$') {
        dst_var->value = state->vars[HEXDIGIT_TO_INT(expr[1])].value;
    } else if (expr[0] == '0' && expr[1] == 'x') {
        expr += 2;
        dst_var->value = 0;
        while (*expr) {
            dst_var->value <<= 4u;
            dst_var->value |= HEXDIGIT_TO_INT(*expr);
            expr++;
        }
    } else if (expr[0] == '"') {
        char* read = expr + 1;
        char* write = expr + 1;
        while(*read != '"' && *read) {
            if (*read == '\\') {
                read++;
                if (*read == 'n') {
                    *write = '\n';
                } else if (*read == 't') {
                    *write = '\t';
                } else if (*read == '"') {
                    *write = '"';
                }
            } else {
                *write = *read;
            }
            read++;
            write++;
        }
        *write = '\0';
        dst_var->value = (size_t) expr + 1;
    }
}

int dynamic_eval(char* code, const struct symbol* symbols) {
    struct dc_state state = {};

    char* line = code;
    while (line) {
        char* next_line = strsplit(line, '\n');
        char* setter_expr = strsplit(line, '=');
        if (setter_expr) {
            parse_literal(&state.vars[HEXDIGIT_TO_INT(line[1])], setter_expr, &state);
        } else {
            struct symbol* iter = symbols;
            void* symbol_addr = NULL;
            while (iter->name) {
                if (strcmp(iter->name, line) == 0) {
                    symbol_addr = iter->value;
                    break;
                }
                iter++;
            }

            typedef size_t (*func_t)(size_t arg0, size_t arg1, size_t arg2, size_t arg3, size_t arg4, size_t arg5, size_t arg6, size_t arg7);
            state.vars[0].value = ((func_t)symbol_addr)(
                    state.vars[1].value,
                    state.vars[2].value,
                    state.vars[3].value,
                    state.vars[4].value,
                    state.vars[5].value,
                    state.vars[6].value,
                    state.vars[7].value,
                    state.vars[8].value
            );
        }
        line = next_line;
    }

#ifdef DC_DEBUG
    printf("=== DC VARS ===\n");
    for (size_t j = 0; j < ARRAY_SIZE(state.vars); j++) {
        if ((state.vars[j].value & 0x7ff000000000) == 0x7ff000000000) {
            printf("%lx: 0x%lx <%s>\n", j, state.vars[j].value, (char*) state.vars[j].value);
        } else {
            printf("%lx: 0x%lx\n", j, state.vars[j].value);
        }
    }
    printf("===============\n");
#endif
    
    return 0;
}