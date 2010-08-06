#ifndef NABOO_TASKS_HANDLER_H_
#define NABOO_TASKS_HANDLER_H_

#include "onyx/base/base_tasks.h"

using namespace ui;

namespace vbf
{

/// TasksHandler is used for executing the tasks which might
/// consume mess of time:
/// 1. Document loading
/// 2. Page parsing
/// 3. Page rendering
/// 4. Searching(search all for now)
/// NOTICE: this class is not thread-safe
class TasksHandler : public QObject
{
    Q_OBJECT
private:
    TasksHandler(const TasksHandler&);

public:
    TasksHandler();
    ~TasksHandler();

    //static TasksHandler& instance();
    void clearTasks();
    void clearTasks(int t);
    void addTask(BaseTask *t, bool append = true);
    void abortCurTask();
    void pauseCurTask();
    void startCurTask();
    BaseTask* currentTask();
    void removeCurrentTask();
    void retrieveNextTask();
    bool isEmpty();

private Q_SLOTS:
    /// Task executing function
    void execute();

private:
    /// start the timer
    void startTimer();

    /// stop the timer
    void stopTimer();

private:
    /// time triggers the tasks execution
    QTimer task_timer_;

    /// task queue
    TasksQueue<BaseTask*> tasks_;

    /// reference to current executing task
    BaseTask* cur_task_;
};

};  // namespace vbf
#endif
