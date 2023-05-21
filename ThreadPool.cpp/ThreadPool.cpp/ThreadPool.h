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
    thread managerID;   //�������߳�ID
    vector<thread> threadIDs;   //
    int minNum;   //��С�߳���
    int maxNum;   //����߳���
    int busyNum;   //æ���߳���
    int liveNum;    //�����߳���
    int exitNum;    //Ҫ���ٵ��߳���

    mutex mutexPool;    //�����̳߳ص���
    condition_variable cond;     //��������Ƿ�Ϊ��
    bool shutdown;    //�Ƿ������̳߳أ�����Ϊ1��������Ϊ0
    static void manager(void* arg);   //�������߳�
    static void worker(void* arg);   //�����߳�
};
#endif
