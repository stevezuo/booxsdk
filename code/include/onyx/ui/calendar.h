
#ifndef ONYX_CALENDAR_H_
#define ONYX_CALENDAR_H_

#include <QCalendarWidget>
#include "buttons.h"
#include "label.h"

namespace ui
{

/// Calendar widget for eink device. Remove unnecessary screen updates.
class OnyxCalendar : public QWidget
{
    Q_OBJECT
public:
    OnyxCalendar(QWidget *parent);
    ~OnyxCalendar();

public:
    QDate selectedDate() const;

protected:
    bool event(QEvent *e);
    void keyPressEvent(QKeyEvent * ke);
    void keyReleaseEvent(QKeyEvent * ke);
    void paintEvent(QPaintEvent *pe);

private:
    void createLayout();

private Q_SLOTS:
    void onSelectionChanged();
    void onCurrentPageChanged(int, int);

    void onYearNextClicked(bool);
    void onYearPrevClicked(bool);

    void onMonthNextClicked(bool);
    void onMonthPrevClicked(bool);

private:
    QVBoxLayout layout_;
    QHBoxLayout header_layout_;

    OnyxLabel year_title_;
    OnyxLabel year_label_;
    OnyxPushButton year_prev_;
    OnyxPushButton year_next_;

    OnyxLabel month_title_;
    OnyxLabel month_label_;
    OnyxPushButton month_prev_;
    OnyxPushButton month_next_;

    QCalendarWidget calendar_;
};

};      // ONYX_CALENDAR_H_

#endif
