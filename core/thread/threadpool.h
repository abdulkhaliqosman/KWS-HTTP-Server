#ifndef CORE_THREAD_THREADPOOL_H
#define CORE_THREAD_THREADPOOL_H

#include <pthread.h>

struct ThreadInfo
{
public:
    pthread_t m_ThreadId = 0;
    int m_ThreadNum = 0;

    using ThreadFuncType = void *(void *);

    void init(const pthread_attr_t &attr, ThreadFuncType func)
    {
        pthread_create(&m_ThreadId, &attr, func, (void *)this);
    }

    void destroy()
    {
        pthread_join(m_ThreadId, nullptr);
    }

private:
};

class ThreadPool
{
public:
    ThreadPool(int threadCount, ThreadInfo::ThreadFuncType func)
        : m_Threads(new ThreadInfo[threadCount]), m_NumThreads(threadCount)
    {
        pthread_attr_init(&attr);
        // printf("Number of processors: %d\n", num_procs);

        for (int i = 0; i < m_NumThreads; ++i)
        {
            m_Threads[i].m_ThreadNum = i + 1;
            m_Threads[i].init(attr, func);
        }
    }

    virtual ~ThreadPool()
    {
        for (int i = 0; i < m_NumThreads; ++i)
        {
            m_Threads[i].destroy();
        }

        delete[] m_Threads;

        pthread_attr_destroy(&attr);
    }

private:
    ThreadInfo *m_Threads;
    int m_NumThreads;

    pthread_attr_t attr;
};

#endif // #ifndef CORE_THREADPOOL_THREADPOOL_H
