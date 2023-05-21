//threadpool.h
#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include <condition_variable>
using namespace std;

using callback = void(*)(void*);

class Task
{
public:
    callback function;
    void* arg;
public:
    Task(callback f, void* arg) { function = f; this->arg = arg; }
};

class ThreadPool
{
public:
    ThreadPool(int min, int max);
    void Add(void(*func)(void*), void* arg);
    void Add(Task task);
    int Busynum();
    int Alivenum();
    ~ThreadPool();

private:
    queue<Task> taskQ;
    thread managerID;   //管理者线程ID
    vector<thread> threadIDs;   //
    int minNum;   //最小线程数
    int maxNum;   //最大线程数
    int busyNum;   //忙的线程数
    int liveNum;    //存活的线程数
    int exitNum;    //要销毁的线程数

    mutex mutexPool;    //整个线程池的锁
    condition_variable cond;     //任务队列是否为空
    bool shutdown;    //是否销毁线程池，销毁为1，不销毁为0
    static void manager(void* arg);   //管理者线程
    static void worker(void* arg);   //工作线程
};
#endif
