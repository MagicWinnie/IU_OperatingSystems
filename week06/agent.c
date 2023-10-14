#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#define PID_FILE_PATH "/tmp/week06-ex1-agent.pid"
#define TXT_FILE_PATH "text.txt"
#define TXT_FILE_LINE_LENGTH 1024

void print_txt_file()
{
    FILE *txt_file = fopen(TXT_FILE_PATH, "r");
    if (txt_file == NULL)
    {
        perror("[ERROR] Opening txt file to write");
        exit(1);
    }
    char buffer[TXT_FILE_LINE_LENGTH] = {0};
    fgets(buffer, TXT_FILE_LINE_LENGTH, txt_file);
    fclose(txt_file);
    printf("%s\n", buffer);
}

void sigusr1_handler(int code)
{
    print_txt_file();
}

void sigusr2_handler(int code)
{
    printf("Process terminating...\n");
    exit(0);
}

int main(void)
{
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGUSR2, sigusr2_handler);

    FILE *pid_file = fopen(PID_FILE_PATH, "w");
    if (pid_file == NULL)
    {
        perror("[ERROR] Opening pid file to write");
        return 1;
    }
    fprintf(pid_file, "%d\n", getpid());
    fclose(pid_file);

    printf("[STARTUP] ");
    print_txt_file();

    while (1)
    {
        pause();
    }

    return 0;
}