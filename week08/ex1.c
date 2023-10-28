#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

int main(void)
{
    FILE *pid_file = fopen("/tmp/ex1.pid", "w");
    if (pid_file == NULL)
    {
        perror("[ERROR] Could not open /tmp/ex1.pid");
        exit(EXIT_FAILURE);
    }
    fprintf(pid_file, "%d", getpid());
    fclose(pid_file);

    char *password = (char *)calloc(5 + 8 + 1, sizeof(char)); // pass: + PASSWORD + \0

    password[0] = 'p';
    password[1] = 'a';
    password[2] = 's';
    password[3] = 's';
    password[4] = ':';

    FILE *dev_random = fopen("/dev/random", "r");
    if (dev_random == NULL)
    {
        perror("[ERROR] Could not open /dev/random");
        exit(EXIT_FAILURE);
    }

    int symbol_i = 0;
    while (symbol_i < 8)
    {
        char random_symbol = getc(dev_random);
        if (isprint(random_symbol))
        {
            password[5 + symbol_i] = random_symbol;
            symbol_i++;
        }
    }

    fclose(dev_random);

    while (1)
        ;

    return EXIT_SUCCESS;
}