#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: %s <n> <d>\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]), d = atoi(argv[2]);

    int pid = fork();

    if (pid == 0)
    { // in the child

        for (int i = 0; i < n; i++)
        {
            printf("%ld\n", (long)i * d);
        }
    }
    else
    { // in the parent
        wait(NULL);

        for (int i = n; i < n + 3; i++)
        {
            printf("%ld\n", (long)i * d);
        }
    }
}