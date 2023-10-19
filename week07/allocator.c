#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 10000000
#define QUERIES_FILE_NAME "queries.txt"

unsigned CELLS[N] = {0};

void allocate_first_fit(unsigned adrs, size_t size)
{
    size_t block_start = 0; // inclusive
    size_t block_end = 0;   // inclusive
    while (block_start < N && block_end < N)
    {
        if (CELLS[block_start] != 0)
        {
            block_start++;
            block_end++;
        }
        else if (CELLS[block_end] == 0)
        {
            if (block_end - block_start + 1 >= size)
            {
                for (size_t j = block_start; j <= block_end; j++)
                    CELLS[j] = adrs;
                return;
            }
            block_end++;
        }
        else
        {
            block_start = block_end;
        }
    }
    fprintf(stderr, "[ERROR] Not enough free blocks in memory for adrs: %u, size: %zu\n", adrs, size);
}

void allocate_best_fit(unsigned adrs, size_t size)
{
    size_t block_start = 0; // inclusive
    size_t block_end = 0;   // inclusive

    size_t best_fit = N + 1;
    size_t best_start = 0, best_end = 0;
    while (block_start < N && block_end < N)
    {
        if (CELLS[block_start] != 0)
        {
            block_start++;
            block_end++;
        }
        else if (CELLS[block_end] == 0)
        {
            if (block_end - block_start + 1 >= size && block_end - block_start + 1 < best_fit)
            {
                best_fit = block_end - block_start;
                best_start = block_start;
                best_end = block_end;
            }
            block_end++;
        }
        else
        {
            block_start = block_end;
        }
    }
    if (best_fit != N + 1)
    {
        for (size_t j = best_start; j < best_start + size; j++)
            CELLS[j] = adrs;
        return;
    }
    fprintf(stderr, "[ERROR] Not enough free blocks in memory for adrs: %u, size: %zu\n", adrs, size);
}

void allocate_worst_fit(unsigned adrs, size_t size)
{
    size_t block_start = 0; // inclusive
    size_t block_end = 0;   // inclusive

    size_t worst_fit = 0;
    size_t worst_start = 0, worst_end = 0;
    while (block_start < N && block_end < N)
    {
        if (CELLS[block_start] != 0)
        {
            block_start++;
            block_end++;
        }
        else if (CELLS[block_end] == 0)
        {
            if (block_end - block_start + 1 >= size && block_end - block_start + 1 > worst_fit)
            {
                worst_fit = block_end - block_start;
                worst_start = block_start;
                worst_end = block_end;
            }
            block_end++;
        }
        else
        {
            block_start = block_end;
        }
    }
    if (worst_fit != 0)
    {
        for (size_t j = worst_start; j < worst_start + size; j++)
            CELLS[j] = adrs;
        return;
    }
    fprintf(stderr, "[ERROR] Not enough free blocks in memory for adrs: %u, size: %zu\n", adrs, size);
}

void clear(unsigned adrs)
{
    if (adrs == 0)
    {
        fprintf(stderr, "[ERROR] Value of adrs should be greater than zero!\n");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < N; i++)
        if (CELLS[i] == adrs)
            CELLS[i] = 0;
}

double process(FILE *file, void (*allocate_algo)(unsigned, size_t))
{
    char cmd[9] = {0};
    unsigned adrs;
    size_t size, count = 0;

    clock_t begin = clock();
    while (1)
    {
        fscanf(file, "%8s", cmd);
        if (strcmp(cmd, "end") == 0)
        {
            break;
        }
        else if (strcmp(cmd, "allocate") == 0)
        {
            fscanf(file, "%u %zu", &adrs, &size);
            if (size < 1 || size > N)
            {
                fprintf(stderr, "[ERROR] Size is out of bounds: %zu\n", size);
                exit(EXIT_FAILURE);
            }
            allocate_algo(adrs, size);
        }
        else if (strcmp(cmd, "clear") == 0)
        {
            fscanf(file, "%u", &adrs);
            clear(adrs);
        }
        else
        {
            fprintf(stderr, "[ERROR] Unknown command in queries file: %s\n", cmd);
            exit(EXIT_FAILURE);
        }
        // printf("--------------------\n");
        // printf("CELLS:\n");
        // for (int i = 0; i < 20; i++)
        //     printf("%d ", CELLS[i]);
        // printf("\n--------------------\n");
        count++;
    }
    clock_t end = clock();

    double delta = (double)(end - begin) / CLOCKS_PER_SEC;
    return (double)count / delta;
}

int main(void)
{
    FILE *queries = fopen(QUERIES_FILE_NAME, "r");
    if (queries == NULL)
    {
        perror("[ERROR] Could not open queries file");
        return EXIT_FAILURE;
    }

    double throughput;

    throughput = process(queries, allocate_first_fit);
    printf("First fit: %f\n", throughput);

    memset(CELLS, 0, sizeof(CELLS));
    rewind(queries);

    throughput = process(queries, allocate_best_fit);
    printf("Best fit: %f\n", throughput);

    memset(CELLS, 0, sizeof(CELLS));
    rewind(queries);

    throughput = process(queries, allocate_worst_fit);
    printf("Worst fit: %f\n", throughput);

    return EXIT_SUCCESS;
}