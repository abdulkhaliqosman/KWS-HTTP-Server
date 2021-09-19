#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <cstdio>

#include "file.cpp"
#include <pthread.h>

#include <sys/sysinfo.h>

#include <fcntl.h>

#include "threadsafequeue.h"
#include <signal.h>

constexpr int BACKLOG_COUNT = 5;
constexpr int MAXLINE = 2048;
constexpr int DEFAULT_PORT = 8080;
constexpr int MAX_CONN = 1024;
constexpr int SLEEP_TIME = 20;

struct SocketData
{
    int socketfd = 0;
};

using SocketQueue = ThreadSafeQueue<SocketData, MAX_CONN>;

SocketQueue g_SocketQueue;
int g_SocketFd = 0;

void *SocketThread(void *arg); // forward declaration

struct ThreadInfo
{
    pthread_t m_ThreadId = 0;
    int m_ThreadNum = 0;

    void init(const pthread_attr_t &attr)
    {
        pthread_create(&m_ThreadId, &attr, &SocketThread, (void *)this);
    }

    void destroy()
    {
        pthread_join(m_ThreadId, nullptr);
    }
};

void *SocketThread(void *arg)
{
    ThreadInfo *threadInfo = (ThreadInfo *)arg;
    while (true)
    {
        SocketData data;
        while (!g_SocketQueue.Pop(data))
        {
            if (g_SocketQueue.Empty())
            {
                usleep(SLEEP_TIME);
            }
        }

        char recvline[MAXLINE + 1];
        int recvlen = read(data.socketfd, recvline, MAXLINE);
        if (recvlen == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
            {
                while (!g_SocketQueue.Push(data)) // put at end of queue
                {
                    if (g_SocketQueue.Full())
                    {
                        usleep(SLEEP_TIME);
                    }
                }
                continue;
            }

            perror("recv error");
            exit(1);
        }

        char method[8];
        char uri[MAXLINE];
        char httpver[16];

        {

            recvline[recvlen] = 0;
            // printf("recvlen = %d\n", recvlen);

            int num = sscanf(recvline, "%7s %2047s %15s", method, uri, httpver);

            if (num < 2)
            {
                method[0] = 0;
            }
            else
            {
                // printf("recvmsg=%s\n", recvline);

                // printf("HTTP request method: %s\n", method);
                // printf("uri: %s\n", uri);
            }
        }

        // We do not support anything other than GET /
        if (strcmp(method, "GET") == 0 && strcmp(uri, "/") == 0)
        {
            // printf("Thread %d GET /\n", threadInfo->m_ThreadNum);
            char sendline[MAXLINE];

            char filebuf[MAXLINE];

            const char *response = "HTTP/1.1 200 OK";
            const char *contentType = "Content-Type: text/html";
            const char *contentLength = "Content-Length";

            int filelen = readfile("./index.html", filebuf, MAXLINE);
            //! How you make sure sendline have enought space ?
            int sendlen = snprintf(sendline, MAXLINE, "%s\n%s\n%s: %d\n\n%s", response, contentType, contentLength, filelen, filebuf);

            // printf("sendmsg: %s\n", sendline);
            // printf("sendlen =%d\n", sendlen);

            int sentlen = write(data.socketfd, sendline, sendlen);

            // printf("sent = %d\n", sentlen);
        }
        // printf("Thread %d close\n", threadInfo->m_ThreadNum);

        close(data.socketfd);
    }
    return nullptr;
}

void sigint(int signum)
{
    close(g_SocketFd);
    exit(0);
}

int main(int argc, char *argv[])
{
    int num_procs = get_nprocs();

    printf("Welcome to Khaliq Web Services kwserver!\n");
    fflush(stdout);

    int socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    sockaddr_in serv_addr;
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

    g_SocketFd = socketfd;
    signal(SIGINT, sigint);

    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(socketfd, &rset);

    int maxfdp1 = socketfd + 1;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // printf("Number of processors: %d\n", num_procs);

    int num_threads = num_procs - 1;
    ThreadInfo *threadPool = new ThreadInfo[num_threads];

    for (int i = 0; i < num_threads; ++i)
    {
        threadPool[i].m_ThreadNum = i + 1;
        threadPool[i].init(attr);
    }
    int acceptCount = 0;

    while (true)
    {
        int readyFD = select(maxfdp1, &rset, nullptr, nullptr, nullptr);
        if (readyFD < 0)
        {
            if (errno != EINTR)
            {
                perror("server terminated prematurely");
                exit(1);
            }
        }

        if (FD_ISSET(socketfd, &rset))
        {
            sockaddr_in cli_addr;
            socklen_t cli_addr_len = sizeof(cli_addr);

            int connfd = accept(socketfd, (sockaddr *)&cli_addr, &cli_addr_len);
            int flags = fcntl(socketfd, F_GETFL, 0);
            fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);

            SocketData data;
            data.socketfd = connfd;

            while (!g_SocketQueue.Push(data))
            {
                if (g_SocketQueue.Full())
                {
                    usleep(SLEEP_TIME);
                }
            }
        }
    }

    for (int i = 0; i < num_threads; ++i)
    {
        threadPool[i].destroy();
    }

    delete[] threadPool;

    pthread_attr_destroy(&attr);
    close(socketfd);
}