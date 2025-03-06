#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>

/*
Usage:
gcc -o lab5_b lab5_b.c
./lab5_b 5 2
*/

void producer(int pipe_fd[2], int n, int d)
{
    srand(time(NULL));

    close(pipe_fd[0]); // Close read end

    for (int k = 0; k < n; k++)
    {
        int value = k * d;
        sleep(rand() % 4); // Random delay (0-3 seconds)

        write(pipe_fd[1], &value, sizeof(int)); // Store value
    }

    close(pipe_fd[1]); // Close write end
}

void consumer(int pipe_fd[2], int n)
{
    int value;
    close(pipe_fd[1]); // Close write end
    for (int k = 0; k < n; k++)
    {

        read(pipe_fd[0], &value, sizeof(int));
        printf("Parent received: %d\n", value);
        fflush(stdout); // print immediately
    }
    close(pipe_fd[0]); // Close read end
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <n> <d>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    int d = atoi(argv[2]);

    if (n <= 1)
    {
        fprintf(stderr, "Error: n must be greater than 1\n");
        exit(EXIT_FAILURE);
    }

    // initialize pipe
    int pipe_fd[2];
    pipe(pipe_fd);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    { // Child process (Producer)
        producer(pipe_fd, n, d);
        exit(EXIT_SUCCESS);
    }
    else
    { // Parent process (Consumer)
        consumer(pipe_fd, n);

        // Wait for child to finish
        wait(NULL);
    }

    return 0;
}