#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define BIG_FILE_NAME "text.txt"
#define BIG_FILE_SIZE (500 * 1024 * 1024) // bytes
#define DEV_RANDOM "/dev/random"
#define MAX_LINE_LENGTH 1024
#define MULTIPLE_OF_PAGE_SZ 1024

void create_random_file()
{
    FILE *big_file = fopen(BIG_FILE_NAME, "w");
    if (big_file == NULL)
    {
        perror("[ERROR] [CREATE_RANDOM_FILE] Could not open output file to write");
        exit(EXIT_FAILURE);
    }
    FILE *dev_random = fopen(DEV_RANDOM, "r");
    if (dev_random == NULL)
    {
        perror("[ERROR] [CREATE_RANDOM_FILE] Could not open input file to write");
        exit(EXIT_FAILURE);
    }

    printf("[INFO] [CREATE_RANDOM_FILE] Generating the big file...\n");
    int count = 0; // one char is one byte
    int line_length = 0;
    int upper_count = 0;
    while (count < BIG_FILE_SIZE)
    {
        char ch = getc(dev_random);
        if (isprint(ch))
        {
            fprintf(big_file, "%c", ch);
            count++;
            line_length++;
            if (isupper(ch))
                upper_count++;
        }
        if (line_length == MAX_LINE_LENGTH - 1)
        {
            fprintf(big_file, "\n");
            count++;
            line_length = 0;
        }
    }
    printf("[INFO] [CREATE_RANDOM_FILE] Finished generating the big file\n");
    printf("[INFO] [CREATE_RANDOM_FILE] For selfcheck, # of capital letters: %d\n", upper_count);

    fclose(big_file);
    fclose(dev_random);
}

int main(void)
{
    create_random_file();

    long page_sz = sysconf(_SC_PAGESIZE);
    long chunk_sz = (long)MULTIPLE_OF_PAGE_SZ * page_sz;

    int fd = open(BIG_FILE_NAME, O_RDWR);
    if (fd < 0)
    {
        perror("[ERROR] [MAIN] Could not open input file");
        exit(EXIT_FAILURE);
    }

    long count_capital = 0;
    for (int i = 0; i < BIG_FILE_SIZE / chunk_sz; i++)
    {
        char *ptr = mmap(NULL, chunk_sz, PROT_READ | PROT_WRITE, MAP_SHARED, fd, i * chunk_sz);
        if (ptr == MAP_FAILED)
        {
            perror("[ERROR] [MAIN] Could not mmap");
            return EXIT_FAILURE;
        }

        for (int j = 0; j < chunk_sz; j++)
        {
            if (isupper(ptr[j]))
            {
                ptr[j] = tolower(ptr[j]);
                count_capital++;
            }
        }

        if (munmap(ptr, chunk_sz))
        {
            perror("[ERROR] [MAIN] Could not munmap");
            return EXIT_FAILURE;
        }
        msync(ptr, chunk_sz, MS_ASYNC);
    }

    printf("[INFO] [MAIN] Total number of capital letters: %ld\n", count_capital);

    close(fd);

    return EXIT_SUCCESS;
}