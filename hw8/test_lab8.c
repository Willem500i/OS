#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define BUFFER_SIZE 100

int main()
{
    int fd;
    char buffer[BUFFER_SIZE];
    struct timeval start_time, end_time;
    long elapsed_time;
    ssize_t bytes_read;

    // record start time
    gettimeofday(&start_time, NULL);

    // open the device
    printf("Opening device...\n");
    fd = open("/dev/lab8", O_RDONLY);
    if (fd < 0)
    {
        perror("Failed to open device");
        return 1;
    }

    // read from the device
    printf("Reading from device...\n");
    bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0)
    {
        perror("Failed to read from device");
        close(fd);
        return 1;
    }

    // calculate our own elapsed time
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) * 1000 +
                   (end_time.tv_usec - start_time.tv_usec) / 1000;

    // make sure the buffer is null-terminated
    buffer[bytes_read] = '\0';

    // print what we got from the device
    printf("Output from device (%zd bytes):\n%s\n", bytes_read, buffer);
    printf("Our measured time elapsed: %ld ms\n", elapsed_time);

    // close the device
    close(fd);
    return 0;
}