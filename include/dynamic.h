#pragma once

struct symbol {
    char* name;
    void* value;
};

int dynamic_eval(char* code, const struct symbol* symbols);