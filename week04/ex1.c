#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    clock_t main_start = clock();

    pid_t child_1_fpid = fork();
    clock_t child_1_start = clock();

    if (child_1_fpid == 0)
    {
        // child 1 process
        printf("[CHILD 1] PID{%d} PPID{%d} TIME(ms){%.4f}\n",
               getpid(),
               getppid(),
               (double)(clock() - child_1_start) / CLOCKS_PER_SEC * 1000.0);
    }
    else
    {
        // main process
        pid_t child_2_fpid = fork();
        clock_t child_2_start = clock();

        if (child_2_fpid == 0)
        {
            // child 2 process
            printf("[CHILD 2] PID{%d} PPID{%d} TIME(ms){%.4f}\n",
                   getpid(),
                   getppid(),
                   (double)(clock() - child_2_start) / CLOCKS_PER_SEC * 1000.0);
        }
        else
        {
            // main process
            while (wait(NULL) > 0)
            {
            }
            printf("[ MAIN  ] PID{%d} PPID{%d} TIME(ms){%.4f}\n",
                   getpid(),
                   getppid(),
                   (double)(clock() - main_start) / CLOCKS_PER_SEC * 1000.0);
        }
    }

    return 0;
}
