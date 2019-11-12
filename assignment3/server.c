// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>
#include <sys/wait.h>
#define PORT 8080
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    if (argc > 1 && strcmp(argv[1], "-c") == 0) {
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);
        char buffer[1024] = {0};
        char *hello = "Hello from server";

        server_fd = atoi(argv[2]);
        printf("new exec'ed server process. file descriptor passed in using argv\nserver_fd: %d\n", server_fd);

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

    // get the uid of "nobody" user in system
    struct passwd *pwd = getpwnam("nobody");
    uid_t user_id = pwd->pw_uid;

    printf("UID before privilege separation: %d\n", getuid());

    // drop the privilege to "nobody" user to
    // attach port and process data from the client
    if (fork() == 0) { // split
        if (setuid(user_id) < 0) {
            perror("set uid failed, please run with sudo");
            exit(EXIT_FAILURE);
        }
        printf("UID after privilege separation: %d\n", getuid());

        char fd_buffer [33];
        sprintf(fd_buffer, "%d", server_fd);
        if (execl("./server", "server", "-c", fd_buffer, NULL) < 0) {
            perror("execl failed");
            exit(EXIT_FAILURE);
        }
    }
    while (wait(NULL) > 0) ; // wait for the child process to complete

    return 0;
}
