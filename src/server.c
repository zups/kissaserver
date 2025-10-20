#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

static int client_fd[8];
static int server_fd;
//static pthread_t reader_thread[8];
static struct sockaddr_in address;
void *reader(void *arg);
void *acceptor(void *arg);

void *acceptor(void *arg) {
    int client_id = 0;
    pthread_t reader_thread;
    while (1) {
        socklen_t addrlen = sizeof(address);
        int client_fd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
        if (client_fd < 0) {
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }
        printf("Client joined: %d\n", client_fd);
        if (pthread_create(&reader_thread, NULL, reader, &client_fd)) {
            perror("pthread_create failed");
            exit(1);
        }
        client_id++;
    }
}

void* reader(void* arg) {
    int client_fd = *(int *)arg;
    struct cat { int dog; int meow; };
    struct cat s1 = {0};
    printf("Meow: %d\n", client_fd);
    // Read data from client
    read(client_fd, &s1, sizeof(struct cat));
    while (s1.dog != 0) {
        printf("Client says: %d, %d\n", s1.dog, s1.meow);
        int bytes_read = read(client_fd, &s1, sizeof(struct cat));
        if (bytes_read <= 0) {
            break;
        }
    }

    printf("Quitting reader thread\n");
    close(client_fd);
    return NULL;
}

int main() {
    pthread_t acceptor_thread;

    // Create socket file descriptor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Bind socket to localhost and port
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    

    // Listen for client connections
    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    if (pthread_create(&acceptor_thread, NULL, acceptor, 0)) {
        perror("pthread_create failed");
        exit(1);
    }

    while (1) {
    }


    for (int i = 0; i < 8; i++) {
        close(client_fd[i]);
    }
    close(server_fd);
    return 0;
}
