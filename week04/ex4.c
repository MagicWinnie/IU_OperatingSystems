#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LINE_SIZE 1024
#define NUM_ARGS 64
#define DELIMITERS " \t\n"

int main(void)
{
    printf("> ");
    while (1)
    {
        fflush(stdin);

        char line[LINE_SIZE];
        fgets(line, LINE_SIZE, stdin);
        if (strlen(line) == 1)
        {
            printf("> ");
            continue;
        }

        // Split string by whitespaces (https://ru.wikipedia.org/wiki/Strtok)
        int argc = 0;
        char *argv[NUM_ARGS] = {NULL};
        char *splitted = strtok(line, DELIMITERS);
        while (splitted != NULL)
        {
            if (argc + 1 == NUM_ARGS)
            {
                printf("[ERROR] Not more than %d args allowed\n", NUM_ARGS - 1);
                break;
            }
            argv[argc] = splitted;
            splitted = strtok(NULL, DELIMITERS);
            argc++;
        }

        // printf("Args (%d): ", argc);
        // for (int i = 0; i < argc; i++)
        //     printf("%s ", argv[i]);
        // printf("\n");

        if (strcmp(argv[0], "exit") == 0)
            break;

        int background = 0;
        if (argc > 0 && strcmp(argv[argc - 1], "&") == 0)
        {
            background = 1;
            argv[argc - 1] = NULL;
            argc--;
        }

        pid_t pid = fork();
        if (pid == 0)
        {
            // child process
            execvp(argv[0], argv);
            printf("[ERROR] Some error occured while running command\n");
            exit(0);
        }
        else
        {
            // main process
            if (!background)
                waitpid(pid, NULL, 0);
            printf("> ");
        }
    }

    return 0;
}