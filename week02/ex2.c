#include <stdio.h>
#include <string.h>
#define MAX_SIZE 256

int main(void)
{
    char buffer[MAX_SIZE + 1] = {0};
    char ch;

    for (size_t i = 0; i < MAX_SIZE; i++)
    {
        scanf("%c", &ch);
        if (ch == '.' || ch == '\n')
            break;
        buffer[i] = ch;
    }

    size_t size = strlen(buffer);

    printf("\"");
    for (size_t i = 0; i < size; i++)
        printf("%c", buffer[size - i - 1]);
    printf("\"\n");

    return 0;
}