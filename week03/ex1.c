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
    int *const p = (int *const)malloc(3 * sizeof(int));

    p[0] = x;
    p[1] = x;
    p[2] = 2 * x;

    if ((long long)&p[1] - (long long)&p[0] == sizeof(int))
        printf("Cells #0 and #1 are contiguous\n");
    else
        printf("Cells #0 and #1 are not contiguous\n");
    if ((long long)&p[2] - (long long)&p[1] == sizeof(int))
        printf("Cells #1 and #2 are contiguous\n");
    else
        printf("Cells #1 and #2 are not contiguous\n");

    printf("T(4): %d\n", const_tri(p, 36));

    free(p);

    return 0;
}
