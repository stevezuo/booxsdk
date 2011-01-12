#include "onyx/ui/calendar.h"
#include "onyx/screen/screen_proxy.h"
#include <QPainter>
#include <QKeyEvent>
#include <QApplication>

namespace ui {

enum MIN_SIZE_OF_MONTH
{
    MINW = 200,
    MINH = 200
};

Calendar::Calendar(QWidget *parent)
: QDialog(parent, Qt::FramelessWindowHint)
, page_tag_(0)
, month_count_(0)
{
    setFocusPolicy(Qt::StrongFocus);
    setFocus();
    setModal(true);
}

Calendar::~Calendar(void)
{
}

// TODO, clean code later, should separate into small functions.
void Calendar::drawPage(QPainter* painter,const QDate& date)
{
    int total_width = this->rect().width();
    int total_height = this->rect().height();
    int hor_space = total_width/40;
    int ver_space = total_height/60;
    int col = 0;
    int row = 0;
    int year_height = total_height/20;

    //set col and row
    setColAndRow(col, row, total_width, total_height, hor_space, ver_space);
   
    int month_width = (total_width - (col + 1)*hor_space)/col;
    int month_height = (total_height - (row + 1)*ver_space - year_height)/row;
    int day_width = month_width/7;
    int day_height = month_height/9;
    int ini_month_x = 0 + hor_space;
    int ini_month_y = 0 + ver_space + year_height;
   
    month_count_ = col*row;

    QRect year_rect(hor_space, ver_space, ver_space + total_width - 2*hor_space, year_height);
    drawYear(painter, year_rect, year_height, date.year());
    
    drawArrow(painter, total_width, total_height, hor_space, ver_space, year_height);
   
    //draw each month
    int first_month = firstMonth(date.month());
    int month_x = 0;
    int month_y = 0;
    for(int i = first_month; 
        i < first_month + month_count_;
        i++)
    {
        int x,y;
        month_loc(x,y,col,i);
        month_x = ini_month_x + (x - 1)*(month_width + hor_space);
        month_y = ini_month_y + (y - 1)*(month_height + ver_space);
        
        drawMonth(painter,
                  month_x,
                  month_y,
                  month_width,
                  month_height,
                  day_width,
                  day_height,
                  date.year(),
                  i);
    }
}

int Calendar::firstMonth(int curr_month)
{
    int tmp = 0;
    while(curr_month > month_count_)
    {
        curr_month -= month_count_;
        tmp++;
    }
    return month_count_*tmp + 1;
}

void Calendar::drawYear(QPainter* painter, const QRect& rect, int year_height, int year)
{
    QFont year_font;
    year_font.setBold(true);
    year_font.setPixelSize(year_height - 2);
    painter->setFont(year_font);
    painter->drawText(rect, Qt::AlignCenter, QString::number(year));
}

void Calendar::drawArrow(QPainter* painter, int total_width, int total_height, int hor_space, int ver_space, int year_height)
{
    int left_arrow_x = total_width/2 - 2*year_height;
    int right_arrow_x = total_width/2 + 2*year_height + year_height*3/10;
    const QPointF arrow_left[3] = 
    {
        QPointF(left_arrow_x + year_height/2, ver_space + year_height/10),
        QPointF(left_arrow_x + year_height/2, ver_space + year_height - year_height/10),
        QPointF(left_arrow_x , (2*ver_space + year_height)/2)
    };
    const QPointF arrow_right[3] = 
    {   
        QPointF(right_arrow_x - year_height/2, ver_space + year_height/10),
        QPointF(right_arrow_x - year_height/2, ver_space + year_height - year_height/10),
        QPointF(right_arrow_x , (2*ver_space + year_height)/2)
    };
    painter->setRenderHint(QPainter::Antialiasing,true);
    painter->setPen(Qt::black);
    painter->setBrush(Qt::black);
    painter->drawPolygon(arrow_left,3);
    painter->drawPolygon(arrow_right,3);
    painter->setRenderHint(QPainter::Antialiasing,false);
}

void Calendar::setColAndRow(int& col, int& row, int total_width, int total_height, int hor_space, int ver_space)
{
    if (total_width > total_height)
    {
        if (total_width/(hor_space+MINW) >= 4)
        {
            col = 4;
            row = 3;
        }
        else 
        {
            col = 3;
            row = 2;
        }
    }
    else
    {
        if (total_height/(ver_space+MINH) >= 4)
        {
            row = 4;
            col = 3;
        }
        else 
        {
            row = 3;
            col = 2;
        }
    }
}

void Calendar::drawMonth(QPainter* painter,
                                int inix,
                                int iniy,
                                int month_width,
                                int month_height,
                                int day_width,
                                int day_height,
                                int year,
                                int month)
{
    QDate date(year,month,1);
    QRect month_name_rect(inix, iniy, month_width, 2*day_height);
    QRect week_name_rect(inix, month_name_rect.y() + month_name_rect.height(), day_width, day_height);
    QRect day_rect(inix, week_name_rect.y() + week_name_rect.height(), day_width, day_height);

    // draw month name
    QFont month_font;
    month_font.setBold(true);
    month_font.setPixelSize(day_height);
    painter->setFont(month_font);
    painter->drawText(month_name_rect, Qt::AlignCenter, QDate::longMonthName(date.month()));

    // draw week name
    QFont week_font;
    week_font.setBold(true);
    week_font.setPixelSize(day_width*7/20);
    painter->setFont(week_font);
    painter->drawText(week_name_rect,Qt::AlignRight, QDate::shortDayName(7));
    for (int i = 1; i < 7; i++)
    {
        week_name_rect.moveLeft(week_name_rect.x() + day_width);
        painter->drawText(week_name_rect,Qt::AlignRight, QDate::shortDayName(i));
    }

    // draw day
    QFont day_font;
    day_font.setBold(false);
    day_font.setPixelSize(day_height*6/10);
    painter->setFont(day_font);
    int day_num = 1;
    int day_loc = date.dayOfWeek();
    if (day_loc == 7) day_loc = 0;
    for (int row = 0; row < 6; row++)
    {
        for (int col = 0; col < 7; col++)
        {
            day_rect.moveTo(inix + col*day_width, day_rect.y());
            if(day_loc > 0)
            {
                day_loc--;
            }
            else if(day_num <= date.daysInMonth())
            {
                //highlight currect date
                QDate date(year,month,day_num);
                if(date == QDate::currentDate())
                {
                    painter->setBrush(Qt::black);
                    QPoint top_left = day_rect.topLeft();
                    top_left.rx() = top_left.x() + day_width/4;
                    painter->drawRect(QRect(top_left,QSize(day_width*8/10,day_height)));
                    painter->setPen(Qt::white);
                    painter->setFont(day_font);
                }
                painter->drawText(day_rect, Qt::AlignRight|Qt::AlignVCenter, QString::number(day_num));
                day_num++;
                painter->setPen(Qt::black);
            }
            else
            {
                return;
            }
        }
        day_rect.moveTo(inix, day_rect.y() + day_height);
    }
}

void Calendar::month_loc(int& x,int& y, int col,int month)
{
    x = y = 1;
    while(month > month_count_) month -= month_count_;
    while (month > col)
    {
        month -= col;
        y++;
    }
    x = month;
}

int Calendar::exec()
{
    onyx::screen::instance().enableUpdate(false);
    showFullScreen();
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC, true, onyx::screen::ScreenCommand::WAIT_ALL);

    return QDialog::exec();
}

void Calendar::keyPressEvent(QKeyEvent *ke)
{
    ke->accept();
}


void Calendar::keyReleaseEvent(QKeyEvent *ke)
{
    // Check the current selected type.
    ke->accept();
    switch (ke->key())
    {
        case Qt::Key_Left:
        case Qt::Key_PageUp:
        {
            --page_tag_;
            repaint();
            break;
        }
        case Qt::Key_Right:
        case Qt::Key_PageDown:
        {
            ++page_tag_;
            repaint();
            break;
        }
        case Qt::Key_Down:
        case Qt::Key_Up:
            break;
        case Qt::Key_Return:
            onReturn();
            break;
        case Qt::Key_Escape:
            reject();
            break;
    }
}

bool Calendar::event(QEvent *e)
{
    int ret = QWidget::event(e);
    if (e->type() == QEvent::UpdateRequest)
    {
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC, true, onyx::screen::ScreenCommand::WAIT_ALL);
        e->accept();
        return true;
    }
    return ret;
}

void Calendar::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QDate date(QDate::currentDate().addMonths(page_tag_*month_count_));
    drawPage(&painter,date);
}

void Calendar::onReturn()
{
    onOkClicked(true);
}

void Calendar::onOkClicked(bool)
{
    accept();
}

void Calendar::onCloseClicked()
{
    reject();
}

}   // namespace ui

