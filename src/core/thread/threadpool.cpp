#include "threadpool.h"
#include "logger/logger.h"

namespace ak::core
{
    void ThreadPool::ThreadInfo::init(const pthread_attr_t &attr, ThreadFuncType func)
    {
        pthread_create(&m_ThreadId, &attr, func, (void *)this);
    }
    void ThreadPool::ThreadInfo::destroy()
    {
        pthread_join(m_ThreadId, nullptr);
    }

    ThreadPool::ThreadPool(int threadCount, ThreadPool::ThreadFuncType func)
        : m_Threads(new ThreadInfo[threadCount]), m_NumThreads(threadCount)
    {
        pthread_attr_init(&attr);
        
        akLogDebug("Threads Created: %d\n", threadCount);

        for (int i = 0; i < m_NumThreads; ++i)
        {
            m_Threads[i].m_ThreadNum = i + 1;
            m_Threads[i].init(attr, func);
        }
    }

    ThreadPool::~ThreadPool()
    {
        for (int i = 0; i < m_NumThreads; ++i)
        {
            m_Threads[i].destroy();
        }

        delete[] m_Threads;

        pthread_attr_destroy(&attr);
    }
} // namespace ak::core
