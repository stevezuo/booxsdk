#include "onyx/data/reading_history.h"

namespace vbf
{

static const unsigned NoCurrentItemIndex = UINT_MAX;
static const unsigned DefaultCapacity = 10;

ReadingHistory::ReadingHistory()
    : history_items_()
    , current_item_(NoCurrentItemIndex)
    , capacity_(DefaultCapacity)
{
}

ReadingHistory::~ReadingHistory()
{
}

/// Set the capacity of the reading histories
void ReadingHistory::setCapacity(unsigned size)
{
    while (static_cast<int>(size) < history_items_.size())
    {
        history_items_.pop_back();
    }

    if (size == 0)
    {
        current_item_ = NoCurrentItemIndex;
    }
    else if (static_cast<int>(current_item_) > history_items_.size())
    {
        current_item_ = history_items_.size() - 1;
    }
    capacity_ = size;
}

/// Add item into the history list
void ReadingHistory::addItem(const QVariant & item)
{
    if (capacity_ == 0)
    {
        return;
    }

    // toss anything in the forward list
    if (current_item_ != NoCurrentItemIndex)
    {
        unsigned target = current_item_ + 1;
        while (history_items_.size() > static_cast<int>(target))
        {
            history_items_.pop_back();
        }
    }

    // toss the first item if the list is getting too big
    if (history_items_.size() == capacity_ &&
        (current_item_ != 0 || capacity_ == 1))
    {
        history_items_.pop_front();
        current_item_--;
    }

    history_items_.append(item);
    current_item_++;
}

/// Goto the back item
void ReadingHistory::back()
{
    assert(current_item_ > 0);
    if (current_item_ > 0)
    {
        current_item_--;
    }
}

/// Goto the forward item
void ReadingHistory::forward()
{
    assert(static_cast<int>(current_item_) < history_items_.size());
    if (static_cast<int>(current_item_) < history_items_.size() - 1)
    {
        current_item_++;
    }
}

QVariant ReadingHistory::backItem() const
{
    if (current_item_ > 0 && current_item_ < NoCurrentItemIndex)
    {
        return history_items_[current_item_ - 1];
    }
    return QVariant();
}

QVariant ReadingHistory::forwardItem() const
{
    if (static_cast<int>(current_item_) < history_items_.size() - 1 && !history_items_.empty())
    {
        return history_items_[current_item_ + 1];
    }
    return QVariant();
}

QVariant ReadingHistory::currentItem() const
{
    if (current_item_ != NoCurrentItemIndex)
    {
        return history_items_[current_item_];
    }
    return QVariant();
}

QVariant ReadingHistory::itemAt(int i) const
{
    if (i >= 0 && i < history_items_.size())
    {
        return history_items_[i];
    }
    return QVariant();
}

QList<QVariant> ReadingHistory::backItems(int max_items) const
{
    QList<QVariant> list;
    if (current_item_ != NoCurrentItemIndex)
    {
        unsigned index = qMax<int>(current_item_ - max_items, 0);
        for (; index < current_item_; ++index)
        {
            list.append(history_items_[index]);
        }
    }
    return list;
}

QList<QVariant> ReadingHistory::forwardItems(int max_items) const
{
    QList<QVariant> list;
    if (history_items_.empty())
    {
        return list;
    }

    unsigned last_max = history_items_.size() - 1;
    if (current_item_ < last_max)
    {
        unsigned last = qMin<int>(current_item_ + max_items, last_max);
        for (unsigned index = current_item_ + 1; index < last; ++index)
        {
            list.append(history_items_[index]);
        }
    }
    return list;
}

QList<QVariant> ReadingHistory::items() const
{
    QList<QVariant> list = history_items_.toList();
    return list;
}

bool ReadingHistory::canGoBack() const
{
    return backListCount() > 0;
}

bool ReadingHistory::canGoForward() const
{
    return forwardListCount() > 0;
}

void ReadingHistory::clear()
{
    history_items_.clear();
}

int ReadingHistory::count() const
{
    return history_items_.size();
}

void ReadingHistory::goToItem(const QVariant & item)
{
    if (history_items_.empty() || item.isNull())
    {
        return;
    }

    int index = 0;
    for (; index < history_items_.size(); ++index)
    {
        if (history_items_[index] == item)
        {
            break;
        }
    }
    if (index < history_items_.size())
    {
        current_item_ = index;
    }
}

int ReadingHistory::backListCount() const
{
    return current_item_ == NoCurrentItemIndex ? 0 : current_item_;
}

int ReadingHistory::forwardListCount() const
{
    return current_item_ == NoCurrentItemIndex ? 0 :
                            static_cast<int>(history_items_.size() - (current_item_ + 1));
}

}
