#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_SIZE_MSG 1024

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Err! Only one argument N is allowed!\n");
        return 1;
    }
    int n = atoi(argv[1]);
    if (n < 1 || n > 4)
    {
        fprintf(stderr, "Oi! N should be in range [1; 4]!\n");
        return 1;
    }

    int **pipes = (int **)malloc(sizeof(int *) * n);
    for (int i = 0; i < n; i++)
    {
        pipes[i] = (int *)malloc(sizeof(int) * 2);
        if (pipe(pipes[i]))
        {
            fprintf(stderr, "Aw man! Could not create pipe #%d!\n", i);
            perror("Error text");
            return 1;
        }
    }

    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid == 0)
        {
            close(pipes[i][1]);

            char path[16] = {0};
            snprintf(path, 16, "/tmp/ex1/s%d", i + 1);

            if (mkfifo(path, 0666) == -1)
            {
                fprintf(stderr, "Geez! Could not create named pipe %s!\n", path);
                perror("Error text");
                return 1;
            }

            char buffer[MAX_SIZE_MSG + 1] = {0};
            while (1)
            {
                int n = read(pipes[i][0], &buffer, MAX_SIZE_MSG);
                if (n == -1)
                {
                    fprintf(stderr, "Oh dear! Could not read from pipe #%d!\n", i);
                    return 1;
                }
                else if (n == 0)
                {
                    break;
                }
                buffer[n] = '\0';
                printf("From fork %d: %s", getpid(), buffer);

                int fd = open(path, O_WRONLY);
                if (fd == -1)
                {
                    fprintf(stderr, "Oh no! Could not open named pipe %s!\n", path);
                    perror("Error text");
                    return 1;
                }
                if (write(fd, buffer, strlen(buffer) + 1) == -1)
                {
                    fprintf(stderr, "Oops-a-daisy! Could not write to named pipe %s!\n", path);
                    perror("Error text");
                    return 1;
                }
                close(fd);
            }

            close(pipes[i][0]);

            return 0;
        }
    }

    for (int i = 0; i < n; i++)
        close(pipes[i][0]);

    char buffer[MAX_SIZE_MSG + 1] = {0};
    while (fgets(buffer, MAX_SIZE_MSG + 1, stdin) != NULL)
    {
        for (int i = 0; i < n; i++)
        {
            if (write(pipes[i][1], &buffer, strlen(buffer)) == -1)
            {
                fprintf(stderr, "Oops-a-daisy! Could not write to pipe #%d!\n", i);
                perror("Error text");
                return 1;
            }
        }
    }

    for (int i = 0; i < n; i++)
        close(pipes[i][1]);

        for (int i = 0; i < n; i++)
    {
        char path[16] = {0};
        snprintf(path, 16, "/tmp/ex1/s%d", i + 1);
        unlink(path);
    }

    return 0;
}