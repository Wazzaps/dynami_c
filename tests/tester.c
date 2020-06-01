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
    char program[] = "$1=\"I said \\\"Hello\\\" and val is 0x%x\\n\"\n"
                     "$2=0x1234\n"
                     "printf";
    if (dynamic_eval(program, symbols) != 0) {
        printf("dynamiC failed to parse program\n");
        return -1;
    }
    return 0;
}