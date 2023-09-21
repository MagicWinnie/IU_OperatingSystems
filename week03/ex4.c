#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <float.h>

void *aggregate(void *base, size_t size, int n, void *initial_value, void *(*opr)(const void *, const void *))
{
    void *x = initial_value;
    if (size / n == sizeof(int))
    {
        for (int i = 0; i < n; i++)
            x = opr(x, (int *)base + i);
    }
    else if (size / n == sizeof(double))
    {
        for (int i = 0; i < n; i++)
            x = opr(x, (double *)base + i);
    }
    return x;
}

void *addition_int(const void *a, const void *b)
{
    int *c = (int *)malloc(sizeof(int *));
    *c = (*(int *)a) + (*(int *)b);
    return c;
}

void *multiplication_int(const void *a, const void *b)
{
    int *c = (int *)malloc(sizeof(int *));
    *c = (*(int *)a) * (*(int *)b);
    return c;
}

void *max_int(const void *a, const void *b)
{
    int *c = (int *)malloc(sizeof(int *));
    if (*(int *)a > *(int *)b)
        *c = *(int *)a;
    else
        *c = *(int *)b;
    return c;
}

void *addition_dbl(const void *a, const void *b)
{
    double *c = (double *)malloc(sizeof(double *));
    *c = (*(double *)a) + (*(double *)b);
    return c;
}

void *multiplication_dbl(const void *a, const void *b)
{
    double *c = (double *)malloc(sizeof(double *));
    *c = (*(double *)a) * (*(double *)b);
    return c;
}

void *max_dbl(const void *a, const void *b)
{
    double *c = (double *)malloc(sizeof(double *));
    if (*(double *)a > *(double *)b)
        *c = *(double *)a;
    else
        *c = *(double *)b;
    return c;
}

int main(void)
{
    int int_arr[5] = {1, 2, 3, 4, 5};

    int int_init = 0;
    int *int_add = aggregate(int_arr, sizeof(int_arr), sizeof(int_arr) / sizeof(int_arr[0]), &int_init, &addition_int);
    printf("Addition of int: %d\n", *int_add);

    int_init = 1;
    int *int_mul = aggregate(int_arr, sizeof(int_arr), sizeof(int_arr) / sizeof(int_arr[0]), &int_init, &multiplication_int);
    printf("Multiplication of int: %d\n", *int_mul);

    int_init = INT_MIN;
    int *int_max = aggregate(int_arr, sizeof(int_arr), sizeof(int_arr) / sizeof(int_arr[0]), &int_init, &max_int);
    printf("Max of int: %d\n", *int_max);

    double dbl_arr[5] = {5.1, 4.2, 3.3, 2.4, 1.5};

    double dbl_init = 0;
    double *dbl_add = aggregate(dbl_arr, sizeof(dbl_arr), sizeof(dbl_arr) / sizeof(dbl_arr[0]), &dbl_init, &addition_dbl);
    printf("Addition of double: %f\n", *dbl_add);

    dbl_init = 1;
    double *dbl_mul = aggregate(dbl_arr, sizeof(dbl_arr), sizeof(dbl_arr) / sizeof(dbl_arr[0]), &dbl_init, &multiplication_dbl);
    printf("Multiplication of double: %f\n", *dbl_mul);

    dbl_init = DBL_MIN;
    double *dbl_max = aggregate(dbl_arr, sizeof(dbl_arr), sizeof(dbl_arr) / sizeof(dbl_arr[0]), &dbl_init, &max_dbl);
    printf("Max of double: %f\n", *dbl_max);

    return 0;
}