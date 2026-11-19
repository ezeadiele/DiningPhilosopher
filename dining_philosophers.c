/*
 * Dining Philosophers problem.
 *
 * Each philosopher is a thread and each chopstick is a mutex. Deadlock is
 * avoided by resource ordering: every philosopher picks up the lower-numbered
 * chopstick first, so a circular wait can never form.
 *
 * Build: make
 * Run:   ./dining_philosophers <number of philosophers> <number of times to eat>
 */
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MIN_PHILOSOPHERS 2
#define MAX_PHILOSOPHERS 1000
#define EAT_TIME_MS 200
#define THINK_TIME_MS 100

typedef struct {
    int id;
    int meals;
    int num_philosophers;
    pthread_mutex_t *chopsticks;
} philosopher_t;

static void sleep_ms(long ms)
{
    struct timespec ts = {.tv_sec = ms / 1000, .tv_nsec = (ms % 1000) * 1000000L};
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
        /* resume the remaining sleep after a signal */
    }
}

/* Parses a base-10 integer in [min, max]; returns 0 on success, -1 on error. */
static int parse_int(const char *text, int min, int max, int *out)
{
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, 10);

    if (errno != 0 || end == text || *end != '\0' || value < min || value > max) {
        return -1;
    }
    *out = (int)value;
    return 0;
}

static void *philosopher(void *arg)
{
    const philosopher_t *p = arg;
    const int left = p->id;
    const int right = (p->id + 1) % p->num_philosophers;
    /* Always lock the lower-numbered chopstick first to break circular wait. */
    const int first = left < right ? left : right;
    const int second = left < right ? right : left;

    for (int meal = 1; meal <= p->meals; meal++) {
        printf("Philosopher %d is thinking\n", p->id);
        sleep_ms(THINK_TIME_MS);

        printf("Philosopher %d wants to eat\n", p->id);
        pthread_mutex_lock(&p->chopsticks[first]);
        printf("Philosopher %d picks up chopstick %d\n", p->id, first);
        pthread_mutex_lock(&p->chopsticks[second]);
        printf("Philosopher %d picks up chopstick %d\n", p->id, second);

        printf("Philosopher %d begins to eat (meal %d/%d)\n", p->id, meal, p->meals);
        sleep_ms(EAT_TIME_MS);
        printf("Philosopher %d has finished eating\n", p->id);

        pthread_mutex_unlock(&p->chopsticks[second]);
        printf("Philosopher %d puts down chopstick %d\n", p->id, second);
        pthread_mutex_unlock(&p->chopsticks[first]);
        printf("Philosopher %d puts down chopstick %d\n", p->id, first);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    int num_philosophers = 0;
    int meals = 0;

    if (argc != 3 ||
        parse_int(argv[1], MIN_PHILOSOPHERS, MAX_PHILOSOPHERS, &num_philosophers) != 0 ||
        parse_int(argv[2], 1, INT_MAX, &meals) != 0) {
        fprintf(stderr,
                "Usage: %s <number of philosophers (%d-%d)> <number of times to eat (>= 1)>\n",
                argv[0], MIN_PHILOSOPHERS, MAX_PHILOSOPHERS);
        return EXIT_FAILURE;
    }

    pthread_mutex_t *chopsticks = calloc((size_t)num_philosophers, sizeof *chopsticks);
    pthread_t *threads = calloc((size_t)num_philosophers, sizeof *threads);
    philosopher_t *philosophers = calloc((size_t)num_philosophers, sizeof *philosophers);
    int status = EXIT_SUCCESS;
    int initialised = 0;
    int started = 0;

    if (chopsticks == NULL || threads == NULL || philosophers == NULL) {
        fprintf(stderr, "Out of memory\n");
        status = EXIT_FAILURE;
        goto cleanup;
    }

    for (; initialised < num_philosophers; initialised++) {
        int rc = pthread_mutex_init(&chopsticks[initialised], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_mutex_init: %s\n", strerror(rc));
            status = EXIT_FAILURE;
            goto cleanup;
        }
    }

    for (; started < num_philosophers; started++) {
        philosophers[started] = (philosopher_t){
            .id = started,
            .meals = meals,
            .num_philosophers = num_philosophers,
            .chopsticks = chopsticks,
        };
        int rc = pthread_create(&threads[started], NULL, philosopher, &philosophers[started]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create: %s\n", strerror(rc));
            status = EXIT_FAILURE;
            break;
        }
    }

    for (int i = 0; i < started; i++) {
        pthread_join(threads[i], NULL);
    }

    if (status == EXIT_SUCCESS) {
        printf("All %d philosophers ate %d time(s). No deadlock.\n", num_philosophers, meals);
    }

cleanup:
    for (int i = 0; i < initialised; i++) {
        pthread_mutex_destroy(&chopsticks[i]);
    }
    free(philosophers);
    free(threads);
    free(chopsticks);
    return status;
}
