#ifndef BASE_TASKS_H_
#define BASE_TASKS_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"

using namespace ui;

namespace vbf
{

enum TaskStatus
{
    TASK_RUN = 0,
    TASK_PAUSE,
    TASK_STOP,
    TASK_ERROR
};

/// Base class of the task
class BaseTask
{
public:
    BaseTask(): status_(TASK_RUN), type_(0) {}
    BaseTask(int t) : status_(TASK_RUN), type_(t) {}
    BaseTask(const BaseTask & right) : status_(right.status_), type_(right.type_) {}
    virtual ~BaseTask() {}

    // operations
    virtual void exec() = 0;

    void abort() { status_ = TASK_STOP; }
    void pause() { status_ = TASK_PAUSE; }
    void start() { status_ = TASK_RUN; }

    TaskStatus status() const { return status_; }
    int        type() const   { return type_; }

private:
    TaskStatus status_;    // status of the task
    int        type_;      // type of the task
};

/// @brief TasksQueue is a container(queue) of all the tasks
/// it is not thread-safe.
template<class T>
class TasksQueue
{
public:
    TasksQueue();
    ~TasksQueue();

    void append(T t);
    void prepend(T t);
    void clear();
    template<class E>
    void clear(E e);
    T grabFront();
    T grabBack();
    int size();

private:
    typedef std::list<T> Queue;
    typedef typename Queue::iterator QueueIter;

private:
    // tasks queue
    Queue queue_;

    NO_COPY_AND_ASSIGN(TasksQueue);
};

template<class T>
TasksQueue<T>::TasksQueue():queue_()
{
}

template<class T>
TasksQueue<T>::~TasksQueue()
{
    clear();
}

template<class T>
void TasksQueue<T>::append(T t)
{
    queue_.push_back(t);
}

template<class T>
void TasksQueue<T>::prepend(T t)
{
    queue_.push_front(t);
}

template<class T>
void TasksQueue<T>::clear()
{
    if (queue_.empty())
    {
        return;
    }

    QueueIter begin = queue_.begin();
    QueueIter end   = queue_.end();
    QueueIter iter  = begin;
    for(; iter != end; ++iter)
    {
        // delete left tasks
        delete *iter;
    }
    queue_.clear();
}

template<class T>
template<class E>
void TasksQueue<T>::clear(E e)
{
    if (queue_.empty())
    {
        return;
    }

    QueueIter end   = queue_.end();
    QueueIter iter  = queue_.begin();
    while (iter != end)
    {
        if ((*iter)->type() == e)
        {
            delete *iter;
            iter = queue_.erase(iter);
        }
        else
        {
            iter++;
        }
    }
}

template<class T>
T TasksQueue<T>::grabFront()
{
    if (!queue_.empty())
    {
        T t = queue_.front();
        queue_.pop_front();
        return t;
    }
    return 0;
}

template<class T>
T TasksQueue<T>::grabBack()
{
    if (!queue_.empty())
    {
        T t = queue_.back();
        queue_.pop_back();
        return t;
    }
    return 0;
}

template<class T>
int TasksQueue<T>::size()
{
    return static_cast<int>(queue_.size());
}

};  // namespace vbf

#endif
