#pragma once

struct symbol {
    char* name;
    void* value;
};

int dynamic_eval(const char* code, const struct symbol* symbols);