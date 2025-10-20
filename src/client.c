#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void set_input_mode(struct termios *old) {
    struct termios new;
    tcgetattr(STDIN_FILENO, old); // Save old terminal settings
    new = *old;
    new.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode & echo
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
}

void reset_input_mode(struct termios *old) {
    tcsetattr(STDIN_FILENO, TCSANOW, old);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary
    char* ip_addr_remote = getenv("IP_ADDR_REMOTE");
    if (!ip_addr_remote) {
        ip_addr_remote = "127.0.0.1";
    }
    if (inet_pton(AF_INET, ip_addr_remote, &serv_addr.sin_addr) <= 0) {
        perror("invalid address / address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send message
    const char *message = "Hello from client!";
    struct cat { int dog; int meow; };
    struct cat s1;
    s1.dog = 1;
    s1.meow = 1231231231;

    send(sock, &s1, sizeof(struct cat), 0);
    printf("Message sent to server.\n");
    struct termios old;
    set_input_mode(&old);

    printf("Press keys (press 'q' to quit):\n");
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q') {
        switch (c) {
        case 97:
            s1.dog++;
            break;
        case 100:
            s1.dog--;
            break;
        default:
            break;
        }
        send(sock, &s1, sizeof(struct cat), 0);
    }

    reset_input_mode(&old);

    // Receive response
    read(sock, buffer, BUFFER_SIZE);
    printf("Server says: %s\n", buffer);

    close(sock);
    return 0;
}
