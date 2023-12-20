#include <stdio.h>

typedef struct s {
    int x, y, z;
} s_t;

int main() {
    s_t s;
    s_t *p = &s;
    printf("%ld\n", p);
    printf("%ld\n", p);
}