#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#define SIZE (10 * 1024 * 1024) // in MiB

int main(void)
{
    for (int i = 0; i < 10; i++)
    {
        void *ptr = malloc(SIZE);
        memset(ptr, 0, SIZE);

        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        printf("----------------------------------\n");
        printf("Memory usage (in kilobytes): %ld\n", usage.ru_maxrss);
        printf("----------------------------------\n");

        sleep(1);

        // free(ptr);
    }

    return EXIT_SUCCESS;
}