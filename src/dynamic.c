#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include "dynamic.h"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(*arr))

struct dc_variable {
    char name[24];
    size_t value;
};

struct dc_state {
    struct dc_variable vars[16];
    size_t last_call_retval;
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

static const char* find_setter(const char* word) {
    while(*word != '\n' && *word != '\0' && *word != ' ' && *word != '=') {
        word++;
    }
    return *word == '=' ? word + 1 : NULL;
}

static bool parse_literal(struct dc_state* state, const char* lit, size_t lit_len, size_t* target) {
    if (lit_len == 1 && lit[0] == '?') {
        *target = state->last_call_retval;
        return true;
    } else if (lit_len > 2) {
        if (lit[0] == '0' && lit[1] == 'x') {
            size_t val = 0;
            for (size_t i = 2; i < lit_len; i++) {
                val <<= 4u;
                if (lit[i] >= '0' && lit[i] <= '9') {
                    val |= (unsigned int) lit[i] - '0';
                } else if (lit[i] >= 'a' && lit[i] <= 'f') {
                    val |= (unsigned int) lit[i] - 'a' + 10;
                } else if (lit[i] >= 'A' && lit[i] <= 'F') {
                    val |= (unsigned int) lit[i] - 'A' + 10;
                } else {
                    return false;
                }
            }
            *target = val;
            return true;
        }
    }
    return false;
}

int dynamic_eval(const char* code, const struct symbol* symbols) {
    struct dc_state state = {};

    const char* curr = code;
    while (1) {
        const char* eol = find_eol(curr);
        const char* setter = find_setter(curr);
        if (setter) {
            size_t setter_var_len = setter - curr - 1;
            size_t setter_expression_len = eol - setter;

#ifdef DC_DEBUG
            printf("dc: set ");
            for (size_t i = 0; i < setter_var_len; i++) {
                printf("%c", curr[i]);
            }
            printf(" to ");
            for (size_t i = 0; i < setter_expression_len; i++) {
                printf("%c", setter[i]);
            }
            printf("\n");
#endif

            if (setter_var_len > sizeof(state.vars[0].name)) {
                return -1;
            }

            bool found = false;
            for (size_t i = 0; i < ARRAY_SIZE(state.vars); i++) {
                if (__builtin_memcmp(&state.vars[i].name, curr, setter_var_len) == 0) {
                    if (!parse_literal(&state, setter, setter_expression_len, &state.vars[i].value)) {
                        return -1;
                    }
                    found = true;
                    break;
                }
            }
            if (!found) {
                for (size_t i = 0; i < ARRAY_SIZE(state.vars); i++) {
                    if (state.vars[i].name[0] == '\0') {
                        memcpy(state.vars[i].name, curr, setter_var_len);
                        if (!parse_literal(&state, setter, setter_expression_len, &state.vars[i].value)) {
                            return -1;
                        }
                        break;
                    }
                }
            }
        } else {
            const char* eow = find_eow(curr);

#ifdef DC_DEBUG
            printf("dc: call ");
            for (size_t i = 0; i < eow - curr; i++) {
                printf("%c", curr[i]);
            }
            printf("\n");
#endif

            void* symbol_addr = NULL;
            for (const struct symbol* iter = symbols; iter->name; iter++) {
                if (strncmp(iter->name, curr, eow - curr) == 0) {
                    symbol_addr = iter->value;
                    break;
                }
            }

            if (!symbol_addr) {
                return -1;
            }

            size_t args[8] = {};
            for (size_t i = 0; i < 8; i++) {
                const char* last_eow = eow;
                if (*last_eow == '\n' || *last_eow == '\0') {
                    break;
                }
                last_eow++;
                eow = find_eow(last_eow);

                bool found = false;
                for (size_t j = 0; j < ARRAY_SIZE(state.vars); j++) {
                    if (__builtin_memcmp(&state.vars[j].name, last_eow, eow - last_eow) == 0) {
                        args[i] = state.vars[j].value;
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    return -1;
                }
            }

            typedef size_t (*func_t)(size_t arg0, size_t arg1, size_t arg2, size_t arg3, size_t arg4, size_t arg5, size_t arg6, size_t arg7);
            state.last_call_retval = ((func_t)symbol_addr)(args[0], args[1], args[2], args[3], args[4], args[5], args[6], args[7]);
        }

        curr = eol + 1;
        if (*eol == '\0') {
            break;
        }
    }

#ifdef DC_DEBUG
    printf("=== DC VARS ===\n");
    for (size_t j = 0; j < ARRAY_SIZE(state.vars); j++) {
        if (*state.vars[j].name == '\0') {
            continue;
        }
        printf("%s: 0x%lx\n", state.vars[j].name, state.vars[j].value);
    }
    printf("===============\n");
#endif
    
    return 0;
}