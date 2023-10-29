#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#define PAGETABLE_FILENAME "/tmp/ex2/pagetable"
#define PID_FILENAME "/tmp/ex2/pager_pid"
#define ARRAY_ELEMENT_SIZE (8 + 1)
#define DASH_SIZE 25

char *generate_random_string(char *str, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        int key;
        if (rand() % 2)
            key = 97 + rand() % 26;
        else
            key = 65 + rand() % 26;
        str[i] = (char)key;
    }
    str[size] = '\0';
    return str;
}

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

size_t P, F;
char **disk, **RAM;
int pagetable_fd, disk_accesses, *frames;
size_t sizeof_pagetable_file;
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

void print_RAM()
{
    for (size_t i = 0; i < DASH_SIZE; i++)
        printf("-");
    printf("\nRAM array\n");

    for (size_t i = 0; i < F; i++)
        printf("Frame %ld ---> %s\n", i, RAM[i]);

    for (size_t i = 0; i < DASH_SIZE; i++)
        printf("-");
    printf("\n");
}

void print_disk()
{
    for (size_t i = 0; i < DASH_SIZE; i++)
        printf("-");
    printf("\nDisk array\n");

    for (size_t i = 0; i < P; i++)
        printf("Page %ld ---> %s\n", i, disk[i]);

    for (size_t i = 0; i < DASH_SIZE; i++)
        printf("-");
    printf("\n");
}

void handle_sigusr1(int code)
{
    int mmu_pid = 0;
    for (int page = 0; page < P; page++)
    {
        if (pagetable[page].referenced != 0)
        {
            mmu_pid = pagetable[page].referenced;
            printf("A disk access request from MMU Process (pid=%d)\n", mmu_pid);
            printf("Page %d is referenced\n", page);

            bool found = false;
            int frame;
            for (frame = 0; frame < F; frame++)
            {
                if (frames[frame] == -1)
                {
                    printf("We can allocate it to free frame %d\n", frame);
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                printf("We do not have free frames in RAM\n");
                frame = rand() % F;
                int victim = frames[frame];
                printf("Chose a random victim page %d\n", victim);
                printf("Replace/Evict it with page %d to be allocated to frame %d\n", page, frame);
                if (pagetable[victim].dirty)
                {
                    strcpy(disk[victim], RAM[frame]);
                    disk_accesses++;
                    pagetable[victim].dirty = false;
                }
                pagetable[victim].frame = -1;
                pagetable[victim].valid = false;
            }
            printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", page, frame);
            frames[frame] = page;
            pagetable[page].frame = frame;
            pagetable[page].valid = true;
            pagetable[page].dirty = false;
            pagetable[page].referenced = 0;
            disk_accesses++;
            strcpy(RAM[frame], disk[page]);
            break;
        }
    }

    print_pagetable();
    print_RAM();
    print_disk();

    printf("disk accesses is %d so far\n", disk_accesses);

    printf("Resume MMU process\n");
    kill(mmu_pid, SIGCONT);
    for (size_t i = 0; i < 2 * DASH_SIZE; i++)
        printf("-");
    printf("\n");

    if (mmu_pid == 0)
    {
        printf("%d disk accesses in total\n", disk_accesses);
        printf("Pager is terminated\n");

        for (size_t i = 0; i < P; i++)
            free(disk[i]);
        free(disk);

        for (size_t i = 0; i < F; i++)
            free(RAM[i]);
        free(RAM);

        free(frames);

        munmap(pagetable, sizeof_pagetable_file);
        close(pagetable_fd);
        unlink(PAGETABLE_FILENAME);

        exit(EXIT_SUCCESS);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "[ERROR] Usage: ./pager.out <number of pages> <number of frames>\n");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR1, handle_sigusr1);

    FILE *pid_file = fopen(PID_FILENAME, "w");
    if (pid_file == NULL)
    {
        perror("[ERROR] Could not open a file to write the PID");
        exit(EXIT_FAILURE);
    }
    fprintf(pid_file, "%d", getpid());
    fclose(pid_file);

    P = atoi(argv[1]);
    F = atoi(argv[2]);

    disk = (char **)malloc(P * sizeof(char *));
    for (size_t i = 0; i < P; i++)
    {
        disk[i] = (char *)calloc(ARRAY_ELEMENT_SIZE, sizeof(char));
        generate_random_string(disk[i], ARRAY_ELEMENT_SIZE - 1);
    }

    RAM = (char **)malloc(F * sizeof(char *));
    for (size_t i = 0; i < F; i++)
        RAM[i] = (char *)calloc(ARRAY_ELEMENT_SIZE, sizeof(char));

    frames = (int *)malloc(F * sizeof(int));
    for (size_t i = 0; i < F; i++)
        frames[i] = -1;

    pagetable_fd = open(PAGETABLE_FILENAME, O_CREAT | O_RDWR, S_IRWXU | S_IRWXO);
    if (pagetable_fd == -1)
    {
        perror("[ERROR] Could not open the page table file");
        exit(EXIT_FAILURE);
    }
    sizeof_pagetable_file = sizeof(struct PTE) * P;
    ftruncate(pagetable_fd, sizeof_pagetable_file);

    pagetable = (struct PTE *)mmap(NULL, sizeof_pagetable_file, PROT_READ | PROT_WRITE, MAP_SHARED, pagetable_fd, 0);
    for (size_t i = 0; i < P; i++)
    {
        struct PTE tmp = {false, -1, false, 0};
        pagetable[i] = tmp;
    }

    print_pagetable();
    print_RAM();
    print_disk();

    while (true)
        ;

    return EXIT_SUCCESS;
}