#include <stdio.h>
#include "dynamic.h"

char* get_fmt() {
    return "Hello world! %d+%d=%d\n";
}

int add(int a, int b) {
    return a + b;
}

int example_print(size_t val) {
    printf("example_print: 0x%lx\n", (unsigned long) val);
    return 0xbeef;
}

struct symbol symbols[] = {
    { "example_print", &example_print },
    { "add", &add },
    { "get_fmt", &get_fmt },
    { "printf", &printf },
    { }
};

int main() {
    char* program = "get_fmt\n"
                    "fmt=?\n"
                    "a=0x2\n"
                    "b=0x3\n"
                    "add a b\n"
                    "res=?\n"
                    "printf fmt a b res";
    if (dynamic_eval(program, symbols) != 0) {
        printf("dynamiC failed to parse program\n");
        return -1;
    }
    return 0;
}