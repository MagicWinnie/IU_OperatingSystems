#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define VECTOR_SIZE 120
#define VECTOR_MIN 0
#define VECTOR_MAX 99
#define TEMP_FILENAME "temp.txt"

long long multiprocess_dot_product(size_t n, int u[VECTOR_SIZE], int v[VECTOR_SIZE])
{
    if (n > VECTOR_SIZE)
    {
        printf("[ERROR] Value of n is out of range: n <= VECTOR_SIZE\n");
        exit(1);
    }

    FILE *file = fopen(TEMP_FILENAME, "w");
    if (file == NULL)
    {
        printf("[ERROR] Could not open temporary file to write\n");
        exit(1);
    }

    // create processes
    size_t offset = 0;
    size_t per_process = VECTOR_SIZE / n;
    pid_t *processes = (pid_t *)malloc(sizeof(pid_t) * n);
    for (size_t i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            // child process
            long long sum = 0;
            for (size_t j = offset; j < offset + per_process; j++)
            {
                if (j >= VECTOR_SIZE)
                    break;
                sum += (long long)u[j] * (long long)v[j];
            }
            fprintf(file, "%lld\n", sum);

            exit(0);
        }
        else
        {
            processes[i] = pid;
            offset += per_process;
        }
    }

    // wait for child processes
    for (size_t i = 0; i < n; i++)
        waitpid(processes[i], NULL, 0);

    free(processes);

    // read values back from the file and sum them
    freopen(TEMP_FILENAME, "r", file);
    if (file == NULL)
    {
        printf("[ERROR] Could not open temporary file to write\n");
        exit(1);
    }

    long long sum = 0;
    for (size_t i = 0; i < n; i++)
    {
        long long temp;
        fscanf(file, "%lld", &temp);
        sum += temp;
    }

    fclose(file);

    return sum;
}

int main(void)
{
    srand(time(NULL));

    int u[VECTOR_SIZE], v[VECTOR_SIZE];
    for (size_t i = 0; i < VECTOR_SIZE; i++)
    {
        u[i] = rand() % (VECTOR_MAX - VECTOR_MIN + 1) + VECTOR_MIN;
        v[i] = rand() % (VECTOR_MAX - VECTOR_MIN + 1) + VECTOR_MIN;
    }

    printf("Content of vector u: ");
    for (size_t i = 0; i < VECTOR_SIZE; i++)
        printf("%d ", u[i]);
    printf("\nContent of vector v: ");
    for (size_t i = 0; i < VECTOR_SIZE; i++)
        printf("%d ", v[i]);
    printf("\n");

    size_t n;
    printf("Enter number of processes: ");
    scanf("%ld", &n);
    if (n % 2 != 0 || n / 2 < 1 || n / 2 > 5)
    {
        printf("[ERROR] Value of n is out of range: n=2k, 1<=k<=5\n");
        return 0;
    }

    printf("Dot product: %lld\n", multiprocess_dot_product(n, u, v));

    printf("Check answer by solving it using \"standard\" way: ");
    long long sum = 0;
    for (size_t i = 0; i < VECTOR_SIZE; i++)
        sum += (long long)u[i] * (long long)v[i];
    printf("%lld\n", sum);

    return 0;
}