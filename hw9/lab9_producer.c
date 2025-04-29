/*
 * lab9_producer.c - Producer program for Lab 9
 * NYU Tandon School of Engineering
 * Introduction to Operating Systems
 *
 * Usage: ./lab9_producer <n> <d>
 * n: number of elements to produce
 * d: value to multiply by element position
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define BUF_SZ 5
#define SHM_NAME "/lab9_shm"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <n> <d>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]), d = atoi(argv[2]);

    if (n <= 1)
    {
        fprintf(stderr, "Error: n must be greater than 1\n");
        exit(EXIT_FAILURE);
    }

    // Create shared memory
    int fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // Set file size
    if (ftruncate(fd, BUF_SZ * sizeof(int)) == -1)
    {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Memory-map the file
    int *shm = mmap(NULL, BUF_SZ * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Print the start address of the shared buffer
    printf("Producer: Shared buffer address = %p (logical/virtual address)\n", (void *)shm);

    // Print the address of n (from command line)
    printf("Producer: Address of n = %p\n", (void *)&n);

    // Initialize shared memory with non-zero values (all set to 100)
    for (int i = 0; i < BUF_SZ; i++)
    {
        shm[i] = 100; // Non-zero value as required
    }

    // Seed the random number generator
    srand(time(NULL));

    // Producer logic
    for (int k = 0; k < n; k++)
    {
        int value = k * d;
        sleep(rand() % 4); // Random delay (0-3 seconds)

        // Wait if buffer slot is not available (value != 100)
        while (shm[k % BUF_SZ] != 100)
        {
            // Busy wait
        }

        // Store value
        shm[k % BUF_SZ] = value;
        printf("Producer: Generated value %d\n", value);
        fflush(stdout);
    }

    // Clean up
    munmap(shm, BUF_SZ * sizeof(int));
    close(fd);

    printf("Producer: Finished producing %d values\n", n);

    return 0;
}