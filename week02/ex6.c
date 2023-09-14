#include <stdio.h>

int main(void)
{
    int n;

    printf("Enter number of rows: ");
    scanf("%d", &n);

    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= i; j++)
            printf("*");
        printf("\n");
    }

    printf("==========\n");

    for (int i = 1; i <= n / 2; i++)
    {
        for (int j = 1; j <= i; j++)
            printf("*");
        printf("\n");
    }

    if (n % 2)
    {
        for (int j = 1; j <= n / 2 + 1; j++)
            printf("*");
        printf("\n");
    }

    for (int i = n / 2; i >= 1; i--)
    {
        for (int j = 1; j <= i; j++)
            printf("*");
        printf("\n");
    }

    printf("==========\n");

    for (int i = 1; i <= n; i++)
    {
        for (int j = 1; j <= n; j++)
            printf("*");
        printf("\n");
    }

    return 0;
}
