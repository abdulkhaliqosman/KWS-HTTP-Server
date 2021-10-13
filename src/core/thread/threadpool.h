#ifndef CORE_THREAD_THREADPOOL_H
#define CORE_THREAD_THREADPOOL_H

#include <pthread.h> // pthread_attr_t, pthread_t

namespace ak::core
{
    class ThreadPool
    {
        using ThreadFuncType = void *(void *);

    public:
        ThreadPool(int threadCount, ThreadFuncType func);
        virtual ~ThreadPool();

    private:
        struct ThreadInfo
        {
            pthread_t m_ThreadId = 0;
            int m_ThreadNum = 0;

            void init(const pthread_attr_t &attr, ThreadFuncType func);
            void destroy();
        };

        ThreadInfo *m_Threads;
        int m_NumThreads;
        pthread_attr_t attr;
    };
} // namespace ak::core

#endif // #ifndef CORE_THREADPOOL_THREADPOOL_H
