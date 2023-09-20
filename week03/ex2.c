#include <stdio.h>
#include <math.h>

struct Point
{
    double x;
    double y;
};

double distance(struct Point a, struct Point b)
{
    return pow(pow(a.x - b.x, 2.0) + pow(a.y - b.y, 2.0), 0.5);
}

double area(struct Point a, struct Point b, struct Point c)
{
    return 0.5 * fabs(a.x * b.y - b.x * a.y + b.x * c.y - c.x * b.y + c.x * a.y - a.x * c.y);
}

int main(void)
{
    struct Point A = {2.5, 6};
    struct Point B = {1, 2.2};
    struct Point C = {10, 6};

    printf("Distance (AB): %f\n", distance(A, B));
    printf("Area (ABC): %f\n", area(A, B, C));

    return 0;
}
