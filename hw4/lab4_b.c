// Write a subroutine that creates the process tree shown below and returns 0 for parent, 1 for 1st child, 2 for second child, and 3 for third child

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int tree()
{
    int pid_1, pid_2, pid_3;

    pid_1 = fork();
    if (pid_1 == 0) // we are in 1st child
        return 1;
    else // we are still in parent
    {
        pid_2 = fork();
        if (pid_2 == 0) // we are in 2nd child
        {
            pid_3 = fork();
            if (pid_3 == 0) // we are in 3rd child
                return 3;
            return 2;
        }
    }

    return 0;
}

int main()
{
    int result = tree();
    printf("Result: %d\n", result);
    return 0;
}