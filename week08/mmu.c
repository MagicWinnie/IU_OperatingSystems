#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#define PAGETABLE_FILENAME "/tmp/ex2/pagetable"
#define ARGUMENT_STRING_SIZE 1024
#define DASH_SIZE 25

struct PTE
{
    // The page is in the physical memory (RAM)
    bool valid;
    // The frame number of the page in the RAM
    int frame;
    // The page should be written to disk
    bool dirty;
    // The page is referenced/requested
    int referenced;
};

int P;
struct PTE *pagetable;

void print_pagetable()
{
    for (size_t i = 0; i < DASH_SIZE; i++)
        printf("-");
    printf("\nPage table\n");

    for (size_t i = 0; i < P; i++)
        printf("Page %ld ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n", i, pagetable[i].valid, pagetable[i].frame, pagetable[i].dirty, pagetable[i].referenced);

    for (size_t i = 0; i < DASH_SIZE; i++)
        printf("-");
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, "[ERROR] Usage: ./mmu.out <number of pages> <strings of memory accesses> <PID of pager process>\n");
        fprintf(stderr, "[ERROR] Example: ./mmu.out 4 R0 R1 W1 R0 R2 W2 R0 R3 W2 12345\n");
        exit(EXIT_FAILURE);
    }

    int pagetable_fd = open(PAGETABLE_FILENAME, O_RDWR);
    if (pagetable_fd == -1)
    {
        perror("[ERROR] Could not open the page table file");
        exit(EXIT_FAILURE);
    }

    P = atoi(argv[1]);
    int pager_pid = atoi(argv[argc - 1]);

    size_t sizeof_pagetable_file = sizeof(struct PTE) * P;
    pagetable = (struct PTE *)mmap(NULL, sizeof_pagetable_file, PROT_READ | PROT_WRITE, MAP_SHARED, pagetable_fd, 0);

    print_pagetable();

    for (int i = 2; i < argc - 1; i++)
    {
        if (argv[i][0] != 'W' && argv[i][0] != 'R')
        {
            fprintf(stderr, "[ERROR] Memory accesses have invalid commands\n");
            exit(EXIT_FAILURE);
        }
        for (int j = 1; j < strlen(argv[i]); j++)
        {
            if (!isdigit(argv[i][j]))
            {
                fprintf(stderr, "[ERROR] There should be a page after a mode\n");
                exit(EXIT_FAILURE);
            }
        }
        char cmd = argv[i][0];
        argv[i][0] = '0';
        int place = atoi(argv[i]);

        printf("Read request for page %d\n", place);

        if (place >= P)
        {
            fprintf(stderr, "[ERROR] Page is greater than P=%d\n", P);
            exit(EXIT_FAILURE);
        }

        if (pagetable[place].valid == 0)
        {
            printf("It is not a valid page --> page fault\n");
            pagetable[place].referenced = getpid();
            printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");
            kill(pager_pid, SIGUSR1);
            kill(getpid(), SIGSTOP);
            printf("MMU resumed by SIGCONT signal from pager\n");
        }
        else
        {
            printf("It is a valid page\n");
        }

        if (cmd == 'W')
        {
            printf("It is a write request then set the dirty field\n");
            pagetable[place].dirty = true;
        }

        print_pagetable();

        for (size_t j = 0; j < 2 * DASH_SIZE; j++)
            printf("-");
        printf("\n");
    }

    printf("Done all requests.\n");

    munmap(pagetable, sizeof_pagetable_file);
    close(pagetable_fd);

    printf("MMU sends SIGUSR1 to the pager.\n");
    kill(pager_pid, SIGUSR1);

    printf("MMU terminates.\n");

    return EXIT_SUCCESS;
}