#include <stdio.h>
#include <math.h>

long long convert_to_10(long long x, int s)
{
    long long out = 0;
    int i = 0;
    while (x != 0)
    {
        out += (x % 10) * pow(s, i++);
        x /= 10;
    }
    return out;
}

long long convert_from_10(long long x, int t)
{
    long long out = 0;
    int i = 0;
    while (x != 0)
    {
        out += (x % t) * pow(10, i++);
        x /= t;
    }
    return out;
}

long long convert(long long x, int s, int t)
{
    if (x < 0)
        return -1;
    long long x_tmp = x;
    while (x_tmp != 0)
    {
        if (x_tmp % 10 >= s)
            return -1;
        x_tmp /= 10;
    }
    if (s < 2 || s > 10)
        return -1;
    if (t < 2 || t > 10)
        return -1;

    long long base_10 = convert_to_10(x, s);
    long long base_t = convert_from_10(base_10, t);

    return base_t;
}

int main(void)
{
    long long x, result;
    int s, t;

    scanf("%lld %d %d", &x, &s, &t);
    result = convert(x, s, t);
    if (result == -1)
        printf("cannot convert!\n");
    else
        printf("%lld\n", result);

    return 0;
}