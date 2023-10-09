#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX_SIZE_MSG 1024

int main(void)
{
    int pipe_fd[2];
    if (pipe(pipe_fd))
    {
        fprintf(stderr, "Uh-oh! Could not open pipe!\n");
        perror("Error text");
        return 1;
    }

    pid_t publisher_pid = fork();
    if (publisher_pid == 0)
    {
        // publisher process
        close(pipe_fd[0]);

        char buffer[MAX_SIZE_MSG + 1] = {0};
        while (fgets(buffer, MAX_SIZE_MSG + 1, stdin) != NULL)
        {
            if (write(pipe_fd[1], &buffer, strlen(buffer) + 1) == -1)
            {
                fprintf(stderr, "Oops-a-daisy! Could not write to pipe!\n");
                perror("Error text");
                return 1;
            }
        }

        close(pipe_fd[1]);
    }
    else if (publisher_pid < 0)
    {
        fprintf(stderr, "Help me! Could not create publisher process!\n");
        perror("Error text");
        return 1;
    }
    else
    {
        // main process
        pid_t subscriber_pid = fork();
        if (subscriber_pid == 0)
        {
            // subscriber process
            close(pipe_fd[1]);

            char buffer[MAX_SIZE_MSG + 1] = {0};
            while (1)
            {
                int n = read(pipe_fd[0], &buffer, MAX_SIZE_MSG);
                if (n == -1)
                {
                    fprintf(stderr, "Oh dear! Could not read from pipe!\n");
                    perror("Error text");
                    return 1;
                }
                else if (n == 0)
                {
                    break;
                }
                buffer[n] = '\0';
                printf("%s", buffer);
            }

            close(pipe_fd[0]);
        }
        else if (subscriber_pid < 0)
        {
            fprintf(stderr, "I'm having a tough time! Could not create subscriber process!\n");
            perror("Error text");
            return 1;
        }
        else
        {
            // main process
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            while (wait(NULL) > 0)
            {
            }
        }
    }

    return 0;
}