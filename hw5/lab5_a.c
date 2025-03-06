#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <sys/wait.h>

#define BUF_SZ 5
#define FILE_NAME "shared_mem_file"

/*
Usage:
gcc -o lab5_a lab5_a.c
./lab5_a 5 2
*/

void producer(int *shm, int n, int d)
{
    srand(time(NULL));

    for (int k = 0; k < n; k++)
    {
        int value = k * d;
        sleep(rand() % 4); // Random delay (0-3 seconds)

        while (shm[k % BUF_SZ] != -1)
        {
        } // Wait if buffer is full

        shm[k % BUF_SZ] = value; // Store value
    }
}

void consumer(int *shm, int n)
{
    for (int k = 0; k < n; k++)
    {
        while (shm[k % BUF_SZ] == -1)
        {
        } // Wait for producer

        printf("Received: %d\n", shm[k % BUF_SZ]);
        fflush(stdout);

        shm[k % BUF_SZ] = -1; // Mark as read
    }
}

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

    // Create a regular file for shared memory
    int fd = open(FILE_NAME, O_CREAT | O_RDWR, 0666);
    if (fd == -1)
    {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Set file size
    ftruncate(fd, BUF_SZ * sizeof(int));

    // Memory-map the file
    int *shm = mmap(NULL, BUF_SZ * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shm == MAP_FAILED)
    {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Initialize shared memory
    for (int i = 0; i < BUF_SZ; i++)
    {
        shm[i] = -1;
    }

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    { // Child process (Producer)
        producer(shm, n, d);
        exit(EXIT_SUCCESS);
    }
    else
    { // Parent process (Consumer)
        consumer(shm, n);

        // Unlink the file before both processes exit
        unlink(FILE_NAME);

        // Wait for child to finish
        wait(NULL);

        // Cleanup
        munmap(shm, BUF_SZ * sizeof(int));
        close(fd);
    }

    return 0;
}