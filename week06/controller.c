#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#define PID_FILE_PATH "/tmp/week06-ex1-agent.pid"

int pid;

void sigint_handler(int code)
{
    kill(pid, SIGTERM);
    unlink(PID_FILE_PATH);
    exit(0);
}

int main(void)
{
    signal(SIGINT, sigint_handler);

    FILE *pid_file = fopen(PID_FILE_PATH, "r");
    if (pid_file == NULL)
    {
        perror("Error: No agent found");
        return 1;
    }
    if (fscanf(pid_file, "%d", &pid) != 1)
    {
        printf("Error: No agent found\n");
        return 1;
    }
    fclose(pid_file);
    printf("Agent found.\n");

    printf("[STARTUP] %d\n", pid);
    char command[10] = {0};
    while (1)
    {
        printf("Choose a command {'read', 'exit', 'stop', 'continue'} to send to the agent: ");
        fgets(command, 10, stdin);
        if (strcmp(command, "read\n") == 0)
        {
            kill(pid, SIGUSR1);
        }
        else if (strcmp(command, "exit\n") == 0)
        {
            kill(pid, SIGUSR2);
            unlink(PID_FILE_PATH);
            return 0;
        }
        else if (strcmp(command, "stop\n") == 0)
        {
            kill(pid, SIGSTOP);
        }
        else if (strcmp(command, "continue\n") == 0)
        {
            kill(pid, SIGCONT);
        }
        else
        {
            printf("Unknown command, try again!\n");
        }
    }

    return 0;
}