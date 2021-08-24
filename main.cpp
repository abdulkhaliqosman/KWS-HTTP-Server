#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <cstdio>

#include "file.cpp"

constexpr int BACKLOG_COUNT = 5;
constexpr int MAXLINE = 2048;
constexpr int DEFAULT_PORT = 80;

int main(int argc, char *argv[])
{
    printf("Welcome to Khaliq Web Services kwserver!\n");
    fflush(stdout);

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr, cli_addr;
    socklen_t cli_addr_len = sizeof(cli_addr);

    /* First call to socket() function */
    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));

    int portno = DEFAULT_PORT;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(socketfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
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
    FD_ZERO(&rset);
    FD_SET(socketfd, &rset);

    int maxfdp1 = socketfd + 1;

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

            int connfd = accept(socketfd, (sockaddr *)&cli_addr, &cli_addr_len);
            char method[8];
            char uri[MAXLINE];
            char httpver[16];

            {
                char recvline[MAXLINE];
                int recvlen = recv(connfd, recvline, MAXLINE, 0);
                if (recvlen == -1)
                {
                    perror("recv error");
                    exit(1);
                }

                sscanf(recvline, "%7s %2047s %15s", method, uri, httpver);

                printf("recvmsg=%s\n", recvline);
                printf("recvlen = %d\n", recvlen);

                printf("HTTP request method: %s\n", method);
                printf("uri: %s\n", uri);
            }

            // We do not support anything other than GET /
            if (strcmp(method, "GET") == 0 && strcmp(uri, "/") == 0)
            {
                char sendline[MAXLINE];

                char filebuf[MAXLINE];

                const char *response = "HTTP/1.1 200 OK";
                const char *contentType = "Content-Type: text/html";
                const char *contentLength = "Content-Length";
                const int filelen = readfile("./index.html", filebuf, MAXLINE);

                const int sendlen = snprintf(sendline, MAXLINE, "%s\n%s\n%s: %d\n\n%s", response, contentType, contentLength, filelen, filebuf);

                printf("sendmsg: %s\n", sendline);
                printf("sendlen =%d\n", sendlen);

                int sentlen = send(connfd, sendline, sendlen, 0);
                printf("sent = %d\n", sentlen);
            }

            close(connfd);
        }
    }
    close(socketfd);
}