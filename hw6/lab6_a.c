#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h>  // For inet_addr
#include <stdbool.h>
#include <string.h> // For memset
#include <errno.h>  // For error handling

#define PORT 8080
#define SERVER_ADDR "127.0.0.1" // localhost

// Client (child/producer)
void producer(int n, int d)
{
    srand(time(NULL) ^ getpid()); // Use different seed from parent

    // Create socket
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        perror("Client socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure for client
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_addr.sin_port = htons(PORT);

    // Loop that repeatedly attempts to connect to server
    // every 100ms until connected
    bool connected = false;

    while (!connected)
    {
        if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == 0)
            connected = true;
        else
        {
            if (errno != ECONNREFUSED)
            {
                perror("Client connect failed with unexpected error");
                exit(EXIT_FAILURE);
            }
            usleep(100000); // wait 100ms
        }
    }

    // Generate and send values to server
    for (int k = 0; k < n; k++)
    {
        int value = k * d;
        sleep(rand() % 4); // Random delay (0-3 seconds)

        if (write(client_fd, &value, sizeof(int)) < 0)
        {
            perror("Client write failed");
            break;
        }
    }

    // Close socket
    close(client_fd);
}

// Server (parent/consumer)
void consumer(int n)
{
    srand(time(NULL));
    int random_sleep = rand() % 5 + 1; // 1-5 seconds
    sleep(random_sleep);

    // Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("Server socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options to reuse address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("Server setsockopt failed");
        exit(EXIT_FAILURE);
    }

    // Prepare the sockaddr_in structure for server
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Server bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(server_fd, 3) < 0)
    {
        perror("Server listen failed");
        exit(EXIT_FAILURE);
    }

    // Accept a connection
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        perror("Server accept failed");
        exit(EXIT_FAILURE);
    }

    // Receive data from client
    int value;
    for (int k = 0; k < n; k++)
    {
        if (read(client_fd, &value, sizeof(int)) < 0)
        {
            perror("Server read failed");
            break;
        }
        printf("Server received: %d\n", value);
    }

    // Close both sockets
    close(client_fd);
    close(server_fd);
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

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0)
    { // Child process (Producer) - Client
        producer(n, d);
        exit(EXIT_SUCCESS);
    }
    else
    { // Parent process (Consumer) - Server
        consumer(n);

        // Wait for child to finish
        wait(NULL);
        printf("Parent: Child has terminated\n");
    }

    return 0;
}