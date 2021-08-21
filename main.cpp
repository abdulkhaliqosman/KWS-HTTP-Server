#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

constexpr int BACKLOG_COUNT = 5;
constexpr int MAXLINE = 2048;

int main()
{
    std::cout << "Hello world!" << std::endl;
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);
    /* First call to socket() function */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    /* Initialize socket structure */
    bzero((char *)&serv_addr, sizeof(serv_addr));
    int portno = 5001;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(socketfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR on binding");
        exit(1);
    }

    if (listen(socketfd, BACKLOG_COUNT) < 0)
    {
        perror("ERROR on listen");
        exit(1);
    }

    fd_set rset;
    int maxfdp1 = socketfd + 1;
    FD_ZERO(&rset); /* initialize the set: all bits off */
    FD_SET(socketfd, &rset);
    char recvline[MAXLINE];
    while (true)
    {
        int readyFD = select(maxfdp1, &rset, nullptr, nullptr, nullptr);
        if (readyFD < 0)
        {
            perror("server terminated prematurely");
            exit(1);
        }
        printf("readyFD = %d\n", readyFD);
        if (FD_ISSET(socketfd, &rset))
        { /* socket is readable */

            int connfd = accept(socketfd, (struct sockaddr *)&cli_addr, &cli_addr_len);
            int len = recv(connfd, recvline, MAXLINE, 0);
            if (len == -1)
            {
                perror("recv error");
                exit(1);
            }
            const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/html\nContent-Length: 21\n\n"
            "<h1>Hello world!</h1>";
            printf("len = %d, msg=%s, strlen: %lu\n", len, recvline,  strlen(hello));


            int sent = send(connfd, hello, strlen(hello), 0);
            printf("sent = %d\n", sent);

            close(connfd);
            // fputs(recvline, stdout);
        }
    }
    close(socketfd);
}