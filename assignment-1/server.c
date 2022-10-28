// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/wait.h>
#include <pwd.h>

#define PORT 80
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char *hello = "Hello from server";

    // privilege mode
    char *name = "nobody";
    struct passwd *pass;
    int val;
    pid_t cur_pid;

    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
             sizeof(address)) < 0)
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
                             (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    // Assignment - 1 solution. The server should run in sudo mode
    // fork a child process

    cur_pid = fork();

    if (cur_pid == 0)
    {
        // this is child process
        pass = getpwnam(name);

        // no uid found for the name
        if (pass == NULL)
        {
            printf("UID not found for name %s\n", name);
            return 0;
        }

        // set uid to nobody
        val = setuid((long)pass->pw_uid);
        if (val == -1)
        {
            printf("Could not drop privileges");
            return 0;
        }

        valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
        return 0;
    }
    else if (cur_pid > 0)
    {
        // process in parent
        wait(NULL);
        printf("Return back to parent\n");
    }
    else
    {
        printf("Process forking has failed\n");
        _exit(2);
    }
    return 0;
}