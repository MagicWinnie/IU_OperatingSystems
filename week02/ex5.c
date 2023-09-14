#include <stdio.h>

long long tribonacci(int n)
{
    if (n == 0)
        return 0;
    if (n == 1 || n == 2)
        return 1;
    long long a = 0, b = 1, c = 1, d;
    for (int i = 0; i < n; i++)
    {
        d = a + b + c;
        a = b;
        b = c;
        c = d;
    }
    return a;
}

int main(void)
{
    printf("T(4): %lld\n", tribonacci(4));
    printf("T(36): %lld\n", tribonacci(36));

    return 0;
}
