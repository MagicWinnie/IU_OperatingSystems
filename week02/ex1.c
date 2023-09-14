#include <stdio.h>
#include <limits.h> // max values except for float and double
#include <float.h> // max values for float and double

int main(void)
{
    int var1;
    unsigned short var2;
    long var3;
    float var4;
    double var5;

    var1 = INT_MAX;
    var2 = USHRT_MAX;
    var3 = LONG_MAX;
    var4 = FLT_MAX;
    var5 = DBL_MAX;

    printf("TYPE\tSIZE\tVALUE\n");
    printf("int\t%zu\t%d\n", sizeof(var1), var1);
    printf("ushrt\t%zu\t%hu\n", sizeof(var2), var2);
    printf("long\t%zu\t%ld\n", sizeof(var3), var3);
    printf("float\t%zu\t%f\n", sizeof(var4), var4);
    printf("double\t%zu\t%lf\n", sizeof(var5), var5);

    return 0;
}
