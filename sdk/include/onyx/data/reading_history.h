#ifndef READING_HISTORY_H
#define READING_HISTORY_H

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"

namespace vbf
{

typedef QVector<QVariant> BackForwardVector;
typedef BackForwardVector::iterator BackForwardIter;

class ReadingHistory
{
public:
    ReadingHistory();
    ~ReadingHistory();

    void back();
    void forward();

    QVariant backItem() const;
    QVariant forwardItem() const;
    QVariant currentItem() const;
    QVariant itemAt(int i) const;

    QList<QVariant> backItems(int max_items) const;
    QList<QVariant> forwardItems(int max_items) const;
    QList<QVariant> items() const;

    bool canGoBack() const;
    bool canGoForward() const;

    void clear();
    int count() const;

    void goToItem(const QVariant & item);

    void setCapacity(unsigned size);
    void addItem(const QVariant & item);
private:
    int backListCount() const;
    int forwardListCount() const;

private:
    BackForwardVector history_items_;
    unsigned current_item_;
    BackForwardVector::size_type capacity_;
};

};
#endif
