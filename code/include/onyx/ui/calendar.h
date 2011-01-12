
#ifndef ONYX_FULL_SCREEN_CALENDAR_H_
#define ONYX_FULL_SCREEN_CALENDAR_H_

#include <QDialog>
#include <QDate>

namespace ui
{

/// Fullscreen calendar
class Calendar : public QDialog
{
    Q_OBJECT
public:
    Calendar(QWidget *parent);
    ~Calendar(void);

public:
    int exec();

private:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    bool event(QEvent *e);
    void createLayout();
    void paintEvent(QPaintEvent *);  
    void drawMonth(QPainter* painter,
                   int inix,
                   int iniy,
                   int monthWth,
                   int monthHth,
                   int dayWth,
                   int dayHth,
                   int year,
                   int month);
    void drawPage(QPainter*, const QDate& date);
    void drawArrow(QPainter* painter, int total_width, int total_height, int hor_space, int ver_space, int year_height);
    void drawYear(QPainter* painter, const QRect& rect, int year_height, int year);
    void setColAndRow(int& col, int& row, int total_width, int total_height, int hor_space, int ver_space);
    void month_loc(int& x,int& y, int col,int month);
    int firstMonth(int curr_month);

private Q_SLOTS:
    void onReturn();
    void onOkClicked(bool);
    void onCloseClicked();

private:
    int page_tag_;
    int month_count_;
};

}   // namespace ui

#endif
