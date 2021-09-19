#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#include <cstdio>

#include "file.cpp"
#include <pthread.h>

constexpr int BACKLOG_COUNT = 5;
constexpr int MAXLINE = 2048;
constexpr int DEFAULT_PORT = 8080;
#include <sys/sysinfo.h>

#include <fcntl.h>

void *SocketThread(void *arg); // forward declaration

struct ThreadInfo
{
    pthread_t m_ThreadId = 0;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int m_ThreadNum = 0;
    int m_ConnFd = 0;
    bool m_Waiting = false;

    void init(const pthread_attr_t &attr)
    {
        pthread_mutex_init(&mutex, nullptr);
        pthread_cond_init(&cond, nullptr);
        pthread_create(&m_ThreadId, &attr, &SocketThread, (void *)this);
    }

    void destroy()
    {
        pthread_join(m_ThreadId, nullptr);
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&cond);
    }
};

void *SocketThread(void *arg)
{
    ThreadInfo *threadInfo = (ThreadInfo *)arg;
    while (true)
    {
        pthread_mutex_lock(&threadInfo->mutex);

        threadInfo->m_Waiting = true;
        // printf("Thread %d Waiting\n", threadInfo->m_ThreadNum);

        pthread_cond_wait(&threadInfo->cond, &threadInfo->mutex);

        pthread_mutex_unlock(&threadInfo->mutex);

        printf("Thread %d running, wait = %s\n", threadInfo->m_ThreadNum, threadInfo->m_Waiting ? "true" : "false");
        char method[8];
        char uri[MAXLINE];
        char httpver[16];

        {
            char recvline[MAXLINE + 1];
            int recvlen = recv(threadInfo->m_ConnFd, recvline, MAXLINE, 0);
            if (recvlen == -1)
            {
                perror("recv error");
                exit(1);
            }

            recvline[recvlen] = 0;
            printf("recvlen = %d\n", recvlen);

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
            printf("Thread %d GET /\n", threadInfo->m_ThreadNum);
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

            int sentlen = send(threadInfo->m_ConnFd, sendline, sendlen, 0);

            // printf("sent = %d\n", sentlen);
        }
        printf("Thread %d close\n", threadInfo->m_ThreadNum);

        close(threadInfo->m_ConnFd);
    }
    return nullptr;
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

    // int flags = fcntl(socketfd, F_GETFL, 0);
    // fcntl(socketfd, F_SETFL, flags | O_NONBLOCK);

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

    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(socketfd, &rset);

    int maxfdp1 = socketfd + 1;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    printf("Number of processors: %d\n", num_procs);

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
            perror("server terminated prematurely");
            exit(1);
        }

        if (FD_ISSET(socketfd, &rset))
        { /* socket is readable */
            // printf("Socket is readable\n");
            // find first sleeping thread

            // bool read = false;
            for (int i = 0; i < num_threads; ++i)
            {
                ThreadInfo &thread = threadPool[i];
                pthread_mutex_lock(&thread.mutex);

                if (thread.m_Waiting)
                {
                    // printf("thread i = %d is ready\n", i + 1);
                    sockaddr_in cli_addr;
                    socklen_t cli_addr_len = sizeof(cli_addr);

                    int connfd = accept(socketfd, (sockaddr *)&cli_addr, &cli_addr_len);
                    if (connfd >= 0)
                    {
                        thread.m_ConnFd = connfd;
                        thread.m_Waiting = false;
                        printf("Accept Count: %d\n", ++acceptCount);

                        pthread_cond_signal(&thread.cond);
                    }

                    pthread_mutex_unlock(&thread.mutex);
                    break;
                }
                else
                {
                    pthread_mutex_unlock(&thread.mutex);
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