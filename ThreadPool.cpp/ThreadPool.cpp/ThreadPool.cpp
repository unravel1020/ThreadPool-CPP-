//threadpool.cpp
#include "threadpool.h"
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>
using namespace std;
const int NUMBER = 2;

ThreadPool::ThreadPool(int min, int max)
{
    do
    {
        minNum = min;
        maxNum = max;
        busyNum = 0;
        liveNum = min;
        exitNum = 0;

        shutdown = false;
        managerID = thread(manager, this);

        threadIDs.resize(max);
        for (int i = 0; i < min; ++i)
        {
            threadIDs[i] = thread(worker, this);
        }
        return;
    } while (0);
}

ThreadPool::~ThreadPool()
{
    shutdown = true;
    //�������չ������߳�
    if (managerID.joinable()) managerID.join();
    //�����������������߳�
    cond.notify_all();
    for (int i = 0; i < maxNum; ++i)
    {
        if (threadIDs[i].joinable()) threadIDs[i].join();
    }
}

void ThreadPool::Add(Task t)
{
    unique_lock<mutex> lk(mutexPool);
    if (shutdown)
    {
        return;
    }
    //�������
    taskQ.push(t);
    cond.notify_all();
}

void ThreadPool::Add(callback f, void* a)
{
    unique_lock<mutex> lk(mutexPool);
    if (shutdown)
    {
        return;
    }
    //�������
    taskQ.push(Task(f, a));
    cond.notify_all();
}

int ThreadPool::Busynum()
{
    mutexPool.lock();
    int busy = busyNum;
    mutexPool.unlock();
    return busy;
}

int ThreadPool::Alivenum()
{
    mutexPool.lock();
    int alive = liveNum;
    mutexPool.unlock();
    return alive;
}

void ThreadPool::worker(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while (true)
    {
        unique_lock<mutex> lk(pool->mutexPool);
        //��ǰ��������Ƿ�Ϊ��
        while (pool->taskQ.empty() && !pool->shutdown)
        {
            //���������߳�
            pool->cond.wait(lk);
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    cout << "threadid: " << pthread_self() << " exit......" << endl;
                    lk.unlock();
                    return;
                }
            }
        }

        //�ж��̳߳��Ƿ�ر���
        if (pool->shutdown)
        {
            cout << "threadid: " << pthread_self() << "exit......" << endl;
            return;
        }

        //�����������ȥ��һ������
        Task task = pool->taskQ.front();
        pool->taskQ.pop();
        pool->busyNum++;
        //����
        lk.unlock();

        cout << "thread: " << pthread_self() << " start working..." << endl;
        task.function(task.arg);
        //(*task.function)(task.arg);
        free(task.arg);
        task.arg = nullptr;

        cout << "thread: " << pthread_self() << " end working..." << endl;
        lk.lock();
        pool->busyNum--;
        lk.unlock();
    }
}

void ThreadPool::manager(void* arg)
{
    ThreadPool* pool = static_cast<ThreadPool*>(arg);
    while (!pool->shutdown)
    {
        //ÿ��3����һ��
        sleep(3);

        //ȡ���̳߳�������������͵�ǰ�̵߳�����
        unique_lock<mutex> lk(pool->mutexPool);
        int queuesize = pool->taskQ.size();
        int livenum = pool->liveNum;
        int busynum = pool->busyNum;
        lk.unlock();

        //����߳�
        //����ĸ���>�����̸߳��� && �����߳��� < ����߳���
        if (queuesize > livenum && livenum < pool->maxNum)
        {
            lk.lock();
            int count = 0;
            for (int i = 0; i < pool->maxNum && count < NUMBER && pool->liveNum < pool->maxNum; ++i)
            {
                if (pool->threadIDs[i].get_id() == thread::id())
                {
                    cout << "Create a new thread..." << endl;
                    pool->threadIDs[i] = thread(worker, pool);
                    count++;
                    pool->liveNum++;
                }
            }
            lk.unlock();
        }

        //�����߳�
        //æ���߳�*2 < �����߳��� && �����߳��� >  ��С���߳���
        if (busynum * 2 < livenum && livenum > pool->minNum)
        {
            lk.lock();
            pool->exitNum = NUMBER;
            lk.unlock();
            //�ù������߳���ɱ
            for (int i = 0; i < NUMBER; ++i) pool->cond.notify_all();
        }
    }
}
