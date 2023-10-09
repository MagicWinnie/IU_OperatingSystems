#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// primality test
bool is_prime(int n)
{
    if (n <= 1)
        return false;
    int i = 2;
    for (; i * i <= n; i++)
    {
        if (n % i == 0)
            return false;
    }
    return true;
}

// Primes counter in [a, b)
int primes_count(int a, int b)
{
    int ret = 0;
    for (int i = a; i < b; i++)
        if (is_prime(i) != 0)
            ret++;
    return ret;
}

// argument to the start_routine
// of the thread
typedef struct prime_request
{
    int a, b;
} prime_request;

// start_routine of the thread
void *prime_counter(void *arg)
{
    // get the request from arg
    prime_request req = *(prime_request *)arg;
    // perform the request
    int *count = malloc(sizeof(int));
    *count = primes_count(req.a, req.b);
    return ((void *)count);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Err! Only two arguments N and M are allowed!\n");
        return 1;
    }
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    if (n < 0)
    {
        fprintf(stderr, "Oi! N should be in range [0; +inf)!\n");
        return 1;
    }
    if (m < 0)
    {
        fprintf(stderr, "Oi! M should be in range [0; +inf)!\n");
        return 1;
    }

    int per_thread = n / m;

    pthread_t *thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * m);
    prime_request *arguments = (prime_request *)malloc(sizeof(prime_request) * m);
    for (int i = 0; i < m; i++)
    {
        arguments[i].a = per_thread * i;
        arguments[i].b = per_thread * (i + 1);
        if (i == m - 1)
            arguments[i].b += n % m;
        if (arguments[i].b > n)
            arguments[i].b = n;
        pthread_create(&thread_ids[i], NULL, prime_counter, &arguments[i]);
    }

    void **counts = (void **)malloc(sizeof(void *) * m);
    for (int i = 0; i < m; i++)
        pthread_join(thread_ids[i], &counts[i]);

    int count = 0;
    for (int i = 0; i < m; i++)
        count += *(int *)counts[i];

    printf("Result: %d", count);

    free(thread_ids);
    free(arguments);
    for (int i = 0; i < m; i++)
        free(counts[i]);
    free(counts);

    return 0;
}
