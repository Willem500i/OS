/*
 * lab9_consumer.c - Consumer program for Lab 9
 * NYU Tandon School of Engineering
 * Introduction to Operating Systems
 *
 * Usage: ./lab9_consumer <n> <d>
 * n: number of elements to consume
 * d: (not used in consumer, but passed for consistency)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
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

    int n = atoi(argv[1]);
    // d is not used in consumer but we accept it for consistency

    if (n <= 1)
    {
        fprintf(stderr, "Error: n must be greater than 1\n");
        exit(EXIT_FAILURE);
    }

    // Open existing shared memory
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1)
    {
        perror("shm_open");
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
    printf("Consumer: Shared buffer address = %p (logical/virtual address)\n", (void *)shm);

    // Print the address of n (from command line)
    printf("Consumer: Address of n = %p\n", (void *)&n);

    // Consumer logic
    for (int k = 0; k < n; k++)
    {
        // Wait for producer to place value (value != 100)
        while (shm[k % BUF_SZ] == 100)
        {
            // Busy wait
        }

        // Retrieve value
        int value = shm[k % BUF_SZ];
        printf("Consumer: Received value %d\n", value);
        fflush(stdout);

        // Mark as read (reset to 100 for the producer to reuse)
        shm[k % BUF_SZ] = 100;
    }

    // Clean up and unlink the shared memory
    munmap(shm, BUF_SZ * sizeof(int));
    close(fd);
    shm_unlink(SHM_NAME); // Delete the shared memory object

    printf("Consumer: Finished consuming %d values\n", n);

    return 0;
}