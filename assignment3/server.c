// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#include <sys/wait.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    int PORT = 8080;
    bool isChild = false;
    FILE *fd = NULL;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-p") == 0) {
            // port specified
            PORT = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-c") == 0) {
            // socket handler
            isChild = true;
            server_fd = atoi(argv[i + 1]);
        } else if (strcmp(argv[i], "-f") == 0) {
            // file descriptor
            if ((fd = fopen(argv[i + 1], "r")) == NULL) {
                printf("Invalid file descriptor.");
                return -1;
            }
        } else if (strcmp(argv[i], "--fd") == 0 && atoi(argv[i + 1]) != -1) {
            fd = fdopen(atoi(argv[i + 1]), "r");
        }
    }

    if (isChild) {
        // get the uid of "nobody" user in system
        struct passwd *pwd = getpwnam("nobody");
        uid_t user_id = pwd->pw_uid;

        printf("UID before privilege separation: %d\n", getuid());

        // change root to `empty` folder
        if (chdir("./empty") != 0) {
            perror("chdir failed");
            exit(EXIT_FAILURE);
        }
        if (chroot("./") != 0) {
            perror("chroot failed");
            exit(EXIT_FAILURE);
        }
        printf("chroot to \"./empty/\"\n");

        // drop the privilege to "nobody" user to
        if (setuid(user_id) < 0) {
            perror("set uid failed, please run with sudo");
            exit(EXIT_FAILURE);
        }
        printf("UID after privilege separation: %d\n", getuid());

        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[1024] = {0};
        char *hello = "Hello from server";

        if (fd != NULL) {
            printf("file_descriptor passed in\n");
            char c;
            c = fgetc(fd);
            while (c != EOF) {
                printf("%c", c);
                c = fgetc(fd);
            }
            fclose(fd);
        }

        printf("socket server_fd: %d\n", server_fd);

        // Forcefully attaching socket to the port 8080
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                    &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );

        // Forcefully attaching socket to the port 8080
        if (bind(server_fd, (struct sockaddr *)&address,
                                    sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                        (socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        valread = read( new_socket , buffer, 1024);
        printf("%s\n",buffer );
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");
        return 0;
    }

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // attach port and process data from the client
    if (fork() == 0) { // split
        char socket_buffer [33];
        sprintf(socket_buffer, "%d", server_fd);
        char port_buffer [33];
        sprintf(port_buffer, "%d", PORT);
        char fd_buffer [33];
        sprintf(fd_buffer, "%d", fd == NULL ? -1 : fileno(fd));
        if (execl("./server", "server", "-p", port_buffer, "-c", socket_buffer, "--fd", fd_buffer, NULL) < 0) {
            perror("execl failed");
            exit(EXIT_FAILURE);
        }
    }
    while (wait(NULL) > 0) ; // wait for the child process to complete

    return 0;
}
