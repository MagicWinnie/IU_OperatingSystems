#include <stdio.h>
#include <string.h>
#include <ctype.h>
#define MAX_SIZE 256

int count(char *str, char ch)
{
    int occurances = 0;
    for (size_t i = 0; i < strlen(str); i++)
        if (str[i] == ch)
            occurances++;

    return occurances;
}

void countAll(char *str)
{
    size_t size = strlen(str);
    for (size_t i = 0; i < size; i++)
    {
        printf("%c:%d", str[i], count(str, str[i]));
        if (i != size - 1)
            printf(", ");
    }
    printf("\n");
}

int main(void)
{
    char buffer[MAX_SIZE + 1] = {0};

    scanf("%256s", buffer);

    for (size_t i = 0; i < strlen(buffer); i++)
        if (isupper(buffer[i]))
            buffer[i] += 32;

    countAll(buffer);

    return 0;
}