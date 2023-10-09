#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#define MAX_SIZE_STR 256

struct Thread
{
    pthread_t id;
    int i;
    char message[MAX_SIZE_STR + 1];
};

void *foo(void *arg)
{
    struct Thread *thread = (struct Thread *)arg;
    printf("ID: %ld; MSG: %s\n", thread->id, thread->message);
    return NULL;
}

int main(void)
{
    int n;
    scanf("%d", &n);

    struct Thread *arr = (struct Thread *)malloc(sizeof(struct Thread) * n);
    for (int i = 0; i < n; i++)
    {
        arr[i].i = i;
        snprintf(arr[i].message, MAX_SIZE_STR, "Hello from thread %d", i);
        pthread_create(&arr[i].id, NULL, foo, &arr[i]);
        printf("Thread %d is created\n", i);
        pthread_join(arr[i].id, NULL);
    }

    return 0;
}
