#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#define B (1)
#define K (1024 * B)
#define M (1024 * K)
#define G (1024 * M)

int main() {
    char * p = (char *) malloc(((long long)4 * G));
    assert (p != NULL);
    *p = 1;
    printf("addr:%p, value:%d\n", p, *p);
    *(p + (M)) = 2;
    printf("addr:%p, value:%d\n", (p + (M)), *(p + (M)));
    *(p + (G) - 1) = 3;
    printf("addr:%p, value:%d\n", (p + (G) - 1), *(p + (G) - 1));
    free(p);
    return 0;
}