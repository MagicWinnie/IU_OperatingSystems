#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#define MAX_SIZE_MSG 1024

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Err! Only one argument ID is allowed!\n");
        return 1;
    }
    int id = atoi(argv[1]);
    if (id < 1 || id > 4)
    {
        fprintf(stderr, "Oi! ID should be in range [1; 4]!\n");
        return 1;
    }

    char path[16] = {0};
    snprintf(path, 16, "/tmp/ex1/s%d", id);

    char buffer[MAX_SIZE_MSG + 1] = {0};
    while (1)
    {
        int fd = open(path, O_RDONLY);
        int n = read(fd, &buffer, MAX_SIZE_MSG);
        if (n == -1)
        {
            fprintf(stderr, "Oh dear! Could not read from named pipe %s!\n", path);
            perror("Error text");
            return 1;
        }
        buffer[n] = '\0';
        printf("From %d: %s", id, buffer);
        close(fd);
    }

    return 0;
}