#include "onyx/base/tasks_handler.h"

using namespace ui;

namespace vbf
{

TasksHandler::TasksHandler()
    : task_timer_()
    , tasks_()
    , cur_task_(0)
{
    connect(&task_timer_, SIGNAL(timeout()), this, SLOT(execute()));
}

TasksHandler::~TasksHandler()
{
    clearTasks();
}

//TasksHandler& TasksHandler::instance()
//{
//    static TasksHandler instance;
//    return instance;
//}

/// Add a new task into the queue
/// if the timer stops when adding, start it
/// NOTE: abort_current and clear_all are valid when append is false
void TasksHandler::addTask(BaseTask *t, bool append, bool abort_current, bool clear_all)
{
    if (append)
    {
        tasks_.append(t);
    }
    else
    {
        if (clear_all)
        {
            clearTasks(t->type());
        }
        tasks_.prepend(t);
        if (abort_current)
        {
            abortCurTask();
        }
    }

    // start the timer if it is not running
    if (!task_timer_.isActive())
    {
        startTimer();
    }
}

/// Abort current task
/// if there is no more task, stop the timer
/// otherwise execute the next task
void TasksHandler::abortCurTask()
{
    if (cur_task_)
    {
        cur_task_->abort();
    }
}

/// Pause current task
/// if there is no more task, stop the timer
/// otherwise execute the next task
void TasksHandler::pauseCurTask()
{
    if (cur_task_)
    {
        cur_task_->pause();
    }
}

/// Start current task
/// if the task has been paused beforehand, restart it
void TasksHandler::startCurTask()
{
    if (cur_task_)
    {
        cur_task_->start();
    }
}

void TasksHandler::execute()
{
    BaseTask *t = currentTask();
    switch (t->status())
    {
    case TASK_RUN:
        // the current task is to be started or still running,
        // execute it.
        t->exec();

        break;
    case TASK_PAUSE:
        // current task is pausing, it should be pushed to the
        // back of the task queue again; at the same time, a new
        // task is retrieved
        t->start();
        addTask(t);
        retrieveNextTask();

        break;
    case TASK_ERROR:
    case TASK_STOP:
        // current task has been finished, it should be removed
        // and then retrieve a new task. However, in Naboo viewer
        // we need wait for document host remove the aborted task
        removeCurrentTask();
        retrieveNextTask();

        break;
    default:
        break;
    }
}

void TasksHandler::startTimer()
{
    task_timer_.start();
}

void TasksHandler::stopTimer()
{
    task_timer_.stop();
}

/// Remove current task
void TasksHandler::removeCurrentTask()
{
    if (cur_task_ != 0 && cur_task_->status() == TASK_RUN)
    {
        // if current task is still runnint, abort it
        cur_task_->abort();
    }

    // delete current task instance
    delete cur_task_;
    cur_task_ = 0;
}

/// Clear the tasks queue and current task
void TasksHandler::clearTasks()
{
    removeCurrentTask();
    tasks_.clear();
}

/// Clear the tasks with given type
void TasksHandler::clearTasks(int t)
{
    tasks_.clear<int>(t);
}

/// Retrieve next task in the queue
void TasksHandler::retrieveNextTask()
{
    // NOTICE: current task MUST be handled before executing this
    // function
    // 1. Remove it
    // 2. Push it back to the tasks queue
    cur_task_ = tasks_.grabFront();
    if (!cur_task_)
    {
        // there is no task in the queue, stop the timer
        stopTimer();
    }
}

/// Get current task
BaseTask* TasksHandler::currentTask()
{
    if (cur_task_ == 0)
    {
        retrieveNextTask();
    }

    return cur_task_;
}

/// Check whether the task queue is empty or not
bool TasksHandler::isEmpty()
{
    return (tasks_.size() == 0);
}

}  // namespace vbf
