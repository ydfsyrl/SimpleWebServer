#ifndef LOCKER_H
#define LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class sem
{
public:
    sem()
    {
        if (sem_init(&m_sem, 0, 0) != 0)
        {
            throw std::exception();
        }
    }

    sem(int num)
    {
        if (sem_init(&m_sem, 0, num) != 0)
        {
            throw std::exception();
        }
    }

    ~sem()
    {
        sem_destroy(&m_sem);
    }

    bool wait()  
    {
        return sem_wait(&m_sem) == 0;  //函数将以原子操作方式将信号量减一,信号量为0时,sem_wait阻塞。成功返回0
    }
    bool post() 
    {
        return sem_post(&m_sem) == 0;  // 函数以原子操作方式将信号量加一,信号量大于0时,唤醒调用sem_post的线程
    }

private:
    sem_t m_sem;
};


class locker
{
public:
    locker()
    {
        if (pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }
    ~locker()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
    pthread_mutex_t *get()
    {
        return &m_mutex;
    }

private:
    pthread_mutex_t m_mutex;
};


class cond  //条件变量提供了一种线程间的通知机制,当某个共享数据达到某个值时,唤醒等待这个共享数据的线程.
{
public:
    cond()
    {
        if (pthread_cond_init(&m_cond, NULL) != 0)
        {
            //pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }

    ~cond()
    {
        pthread_cond_destroy(&m_cond);
    }

    bool wait(pthread_mutex_t *m_mutex)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_wait(&m_cond, m_mutex); //函数将解锁mutex参数指向的互斥锁，并使当前线程阻塞在m_cond上
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    bool timewait(pthread_mutex_t *m_mutex, struct timespec t)
    {
        int ret = 0;
        //pthread_mutex_lock(&m_mutex);
        ret = pthread_cond_timedwait(&m_cond, m_mutex, &t); //函数到了一定的时间，即使条件未发生也会解除阻塞
        //pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;  //函数被用来释放被阻塞在指定条件变量上的一个线程
    }

    bool broadcast()
    {
        return pthread_cond_broadcast(&m_cond) == 0;  //函数以广播的方式唤醒所有等待目标条件变量的线程
    }

private:
    //static pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
};

#endif
