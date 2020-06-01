# dynami_c
a simple &amp; lightweight interpreter

## Example files

```
$1="I said \"Hello\" and val is 0x%x\n"
$2=0x1234
printf
```
Output:
```
I said "Hello" and val is 0x1234
```
A `c` program to run this program:
```c
#include <stdio.h>
#include "dynamic.h"

struct symbol symbols[] = {
    { "printf", &printf },
    { }
};

int main() {
    // Make sure this is mutable! (p.s. it will be destroyed after the run)
    char program[] = "$1=\"I said \\\"Hello\\\" and val is 0x%x\\n\"\n"
                     "$2=0x1234\n"
                     "printf";
    if (dynamic_eval(program, symbols) != 0) {
        printf("dynami_c failed to parse the program\n");
        return -1;
    }
    return 0;
}
```
