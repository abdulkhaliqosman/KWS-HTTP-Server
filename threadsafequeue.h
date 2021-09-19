#include <pthread.h>

// Not lockless, will experiment with lockless next iteration

template <typename T, unsigned SIZE>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue()
    
    {
        pthread_mutex_init(&mutex, nullptr);

        for (size_t i = 0; i < SIZE-1; i++)
        {
            queue[i].next = &queue[i+1];
        }
        
        m_UnusedHead = &queue[0];
        m_UnusedTail = &queue[SIZE-1];
    }

    ~ThreadSafeQueue()
    {
        pthread_mutex_destroy(&mutex);
    }

    bool Push(const T &data)
    {
        pthread_mutex_lock(&mutex);
        if (m_UnusedHead == nullptr)
        {
            pthread_mutex_unlock(&mutex);
            return false;
        }

        Node *node = m_UnusedHead;
        m_UnusedHead = node->next;

        node->data = data;
        node->next = nullptr;

        if (m_QueueHead == nullptr)
        {
            m_QueueHead = m_QueueTail = node;
        }
        else
        {
            m_QueueTail->next = node;
            m_QueueTail = node;
        }

        pthread_mutex_unlock(&mutex);
        return true;
    }

    bool Pop(T &result)
    {
        pthread_mutex_lock(&mutex);
        if (m_QueueHead == nullptr)
        {
            pthread_mutex_unlock(&mutex);
            return false;
        }

        Node *node = m_QueueHead;
        m_QueueHead = node->next;

        result = node->data;
        node->next = nullptr;

        if (m_UnusedHead == nullptr)
        {
            m_UnusedHead = m_UnusedTail = node;
        }
        else
        {
            m_UnusedTail->next = node;
            m_UnusedTail = node;
        }

        pthread_mutex_unlock(&mutex);
        return true;
    }

    bool Full()
    {
        pthread_mutex_lock(&mutex);
        bool result = m_UnusedHead == nullptr;
        pthread_mutex_unlock(&mutex);

        return result;
    }

    bool Empty()
    {
        pthread_mutex_lock(&mutex);
        bool result = m_QueueHead == nullptr;
        pthread_mutex_unlock(&mutex);

        return result;
    }

private:
    // mutex
    // the queue
    struct Node
    {
        T data;
        Node *next = nullptr;
    };

    Node queue[SIZE];

    Node *m_UnusedHead = nullptr;
    Node *m_UnusedTail = nullptr;

    Node *m_QueueHead = nullptr;
    Node *m_QueueTail = nullptr;

    pthread_mutex_t mutex;
};