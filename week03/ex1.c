#include <stdio.h>
#include <stdlib.h>

int const_tri(int *const p, int n)
{
    while (n > 1)
    {
        p[2] = p[0] + p[1] + p[2];
        p[1] = p[2] - p[1] - p[0];
        p[0] = p[2] - p[1] - p[0];
        n--;
    }
    return p[0];
}

int main(void)
{
    const int x = 1;
    const int *q = &x;
    int *const p = (int *const)malloc(sizeof(int));

    *(p + 0) = x;
    *(p + 1) = x;
    *(p + 2) = 2 * x;

    printf("Cell #0: %p\n", p);
    printf("Cell #1: %p\n", p + 1);
    printf("Cell #2: %p\n", p + 2);

    printf("T(4): %d\n", const_tri(p, 36));

    free(p);

    return 0;
}
