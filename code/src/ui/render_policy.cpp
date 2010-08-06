#include "onyx/ui/render_policy.h"

namespace vbf
{

static const int MIN_PRIORITY = 0xffff;

RenderPolicy::RenderPolicy()
    : need_prerender_(true)
    , requests_()
{
}

RenderPolicy::~RenderPolicy()
{
}

/// Get the priority of a page
/// If the page is not in the request list, return -1.
int RenderPolicy::getPriority(int page_number)
{
    QMutexLocker mtx(&mutex_);
    if (requests_.empty() ||
        requests_.find(page_number) == requests_.end())
    {
        return MIN_PRIORITY;
    }

    return requests_[page_number];
}


void RenderPolicy::updateRequests(const QVector<int> & sources)
{
    QMutexLocker mtx(&mutex_);
    requests_.clear();
    for (int idx = 0; idx < sources.size(); ++idx)
    {
        requests_[sources.at(idx)] = idx;
    }
}

bool RenderPolicy::isRenderingPage(int page_number)
{
    QMutexLocker mtx(&mutex_);
    return requests_.find(page_number) !=
           requests_.end();
}

}
