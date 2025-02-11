#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {

    // get the pid into an int
    int id = getpid();
    // print out the pid
    printf("pid: %d\n", id);

    // sleep from 1 to 5 seconds
    srand(time(NULL));
    int rand_int = rand() % 5 + 1;
    sleep(rand_int);
    
    // open the file in readonly mode
    int fd = open(argv[1], O_RDONLY);

    // Check if the file was opened successfully
    if (fd == -1) {
        printf("Error opening file.\n");
        return 1;
    }

    // Store vars to be used in loop
    char buffer[1024];
    ssize_t bytes_read;
    char *start, *newline;

    // Read buffer (1024) amount of bytes at a time from the file and print them out
    while ((bytes_read = read(fd, buffer, 1024 - 1)) > 0) {
	    buffer[bytes_read] = '\0'; // null terminate the buffer

	    start = buffer;
	    while ((newline = strchr(start, '\n')) != NULL) {
		    *newline = '\0'; // replace '\n' with null terminator
		    printf("%s\n", start); // print out the line
		    start = newline + 1; // go to next line
	    }
    }

    // Close the file and return its success
    return close(fd);
}
