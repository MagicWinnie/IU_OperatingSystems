#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// primality test (from ex3)
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

// The mutex
pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;

// Do not modify these variables directly, use the functions on the right side.
int k = 0;
int c = 0;

// input from command line
int n = 0;

// get next prime candidate
int get_number_to_check()
{
    int ret = k;
    if (k != n)
        k++;
    return ret;
}

// increase prime counter
void increment_primes()
{
    c++;
}

// start_routine
void *check_primes(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&global_lock);
        int num = get_number_to_check();
        pthread_mutex_unlock(&global_lock);
        if (num >= n)
            break;
        if (is_prime(num))
        {
            pthread_mutex_lock(&global_lock);
            increment_primes();
            pthread_mutex_unlock(&global_lock);
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Err! Only two arguments N and M are allowed!\n");
        return 1;
    }
    n = atoi(argv[1]);
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

    pthread_t *thread_ids = (pthread_t *)malloc(sizeof(pthread_t) * m);
    for (int i = 0; i < m; i++)
        pthread_create(&thread_ids[i], NULL, check_primes, NULL);
    for (int i = 0; i < m; i++)
        pthread_join(thread_ids[i], NULL);

    printf("Result: %d", c);

    free(thread_ids);
    pthread_mutex_destroy(&global_lock);

    return 0;
}
