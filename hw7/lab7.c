#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_THREADS 4
#define NUM_POINTS 1000000

// Shared variable to count points inside the circle
int count = 0;

// Mutex for synchronizing access to the shared variable
pthread_mutex_t mutex;

// Worker thread function
void *workerThread(void *arg)
{
    // Each thread needs its own random state
    unsigned int rand_state = (unsigned int)time(NULL) + (unsigned int)pthread_self();

    // Number of points for this thread to process
    int points_per_thread = NUM_POINTS / NUM_THREADS;

    for (int i = 0; i < points_per_thread; i++)
    {
        // Generate random point in square [-1,1] x [-1,1]
        double x = (double)rand_r(&rand_state) / RAND_MAX * 2 - 1;
        double y = (double)rand_r(&rand_state) / RAND_MAX * 2 - 1;

        // Calculate distance from origin (0,0)
        double distance = x * x + y * y;

        // Check if point is inside unit circle
        if (distance <= 1.0)
        {
            // Lock mutex before updating shared variable
            pthread_mutex_lock(&mutex);
            count++;
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

int main()
{
    pthread_t threads[NUM_THREADS];

    // Initialize mutex
    pthread_mutex_init(&mutex, NULL);

    printf("Starting Monte Carlo simulation with %d threads, %d points per thread...\n",
           NUM_THREADS, NUM_POINTS / NUM_THREADS);

    // Create worker threads
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&threads[i], NULL, workerThread, NULL) != 0)
        {
            fprintf(stderr, "Error creating thread %d\n", i);
            return 1;
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // Calculate estimated pi value
    double pi_estimate = 4.0 * (double)count / (NUM_POINTS);

    printf("Total points: %d\n", NUM_POINTS);
    printf("Points inside circle: %d\n", count);
    printf("Estimated value of pi: %f\n", pi_estimate);

    // Clean up mutex
    pthread_mutex_destroy(&mutex);

    return 0;
}