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
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // privilege mode
    char *name = "nobody";
    struct passwd *pass;
    int val;
    pid_t cur_pid;

    printf("execve=0x%p\n", execve);

    if (argc == 1)
    {
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

        if (new_socket == -2)
        {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                     (socklen_t *)&addrlen)) < 0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
        }

        // Creating child process using fork
        cur_pid = fork();

        if (cur_pid == 0)
        {
            // Assignment - 2 solution. The server should run in sudo mode for this to work as expected
            printf("Executing the child process of server\n");
            char *fname = "./server";
            char *args[3];
            char socket_str[10];
            sprintf(socket_str, "%d", new_socket);
            args[0] = fname;
            args[1] = socket_str;
            args[2] = NULL;

            // executing the child process with socket id which will be passed as parameters to it
            if (execvp(args[0], args) < 0)
            {
                perror("Execution failed");
                exit(EXIT_FAILURE);
            }
        }
        else if (cur_pid > 0)
        {
            // parent process
            wait(NULL);
            printf("Returning back to parent process\n");
        }
        else
        {
            printf("Forking failed\n");
            _exit(2);
        }
    }
    else
    {
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

        if (argc > 1)
        {
            new_socket(argv[1]);
        }

        valread = read(new_socket, buffer, 1024);

        if (valread < 0)
        {
            perror("Reading from client failed");
            exit(EXIT_FAILURE);
        }

        printf("Read %d bytes: %s\n", valread, buffer);
        send(new_socket, hello, strlen(hello), 0);
        printf("Hello message sent\n");
    }

    return 0;
}
