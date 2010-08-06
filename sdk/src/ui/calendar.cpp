
#include "onyx/ui/calendar.h"
#include <QEvent>

static const QString STYLE =   "\
QCalendarWidget                         \
{                                       \
    background: white, bold;            \
    font-size: 20px;                    \
    border-width: 1px;                  \
    border-color: black;                \
    border-style: solid;                \
    color: black;                       \
    padding: 0px;                       \
}";

namespace ui
{

OnyxCalendar::OnyxCalendar(QWidget *parent)
: QWidget(parent)
, layout_(this)
, header_layout_(0)
, year_title_(tr("Year"))
, year_label_(tr(""))
, year_prev_(QPixmap(":/images/left_arrow.png"), "", 0)
, year_next_(QPixmap(":/images/right_arrow.png"), "", 0)
, month_title_(tr("Month"))
, month_label_(tr(""))
, month_prev_(QPixmap(":/images/left_arrow.png"), "", 0)
, month_next_(QPixmap(":/images/right_arrow.png"), "", 0)
, calendar_(0)
{
    setAutoFillBackground(false);
    createLayout();

    connect(&calendar_, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
    connect(&calendar_, SIGNAL(currentPageChanged(int,int)), this, SLOT(onCurrentPageChanged(int,int)));

    connect(&year_prev_, SIGNAL(clicked(bool)), this, SLOT(onYearPrevClicked(bool)));
    connect(&year_next_, SIGNAL(clicked(bool)), this, SLOT(onYearNextClicked(bool)));

    connect(&month_prev_, SIGNAL(clicked(bool)), this, SLOT(onMonthPrevClicked(bool)));
    connect(&month_next_, SIGNAL(clicked(bool)), this, SLOT(onMonthNextClicked(bool)));
}

OnyxCalendar::~OnyxCalendar()
{
}


void OnyxCalendar::createLayout()
{
    layout_.setContentsMargins(3, 3, 3, 3);
    layout_.addLayout(&header_layout_);
    header_layout_.setContentsMargins(3, 3, 3, 3);
    header_layout_.setSpacing(0);
    header_layout_.addWidget(&year_title_);
    header_layout_.addWidget(&year_prev_);
    header_layout_.addWidget(&year_label_);
    header_layout_.addWidget(&year_next_);

    header_layout_.addWidget(&month_title_);
    header_layout_.addWidget(&month_prev_);
    header_layout_.addWidget(&month_label_);
    header_layout_.addWidget(&month_next_);

    year_title_.setAlignment(Qt::AlignCenter);
    year_label_.setAlignment(Qt::AlignCenter);
    month_title_.setAlignment(Qt::AlignCenter);
    month_label_.setAlignment(Qt::AlignCenter);

    layout_.addSpacing(6);
    layout_.addWidget(&calendar_);

    calendar_.setStyleSheet(STYLE);
    calendar_.setGridVisible(true);
    calendar_.setNavigationBarVisible(false);
    calendar_.setMinimumDate(QDate(2009, 8, 7));

    onCurrentPageChanged(0, 0);
}

QDate OnyxCalendar::selectedDate() const
{
    return calendar_.selectedDate();
}

bool OnyxCalendar::event(QEvent *e)
{
    qDebug("event type %d", e->type());
    switch (e->type())
    {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
    case QEvent::ToolTip:
    case QEvent::Leave:
    case QEvent::Enter:
        e->accept();
        return true;
    default:
        break;
    }
    return QWidget::event(e);
}

void OnyxCalendar::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(-2, -2, 2, 2), 8, 8, Qt::AbsoluteSize);
    painter.fillPath(path, QBrush(QColor(255, 255, 255)));
    painter.setPen(QColor(Qt::gray));
    painter.drawPath(path);
}

void OnyxCalendar::keyPressEvent(QKeyEvent * ke)
{
    ke->ignore();
}

void OnyxCalendar::keyReleaseEvent(QKeyEvent * ke)
{
    QWidget *focus_widget = this->focusWidget();
    if (focus_widget != &year_prev_ &&
        focus_widget != &year_next_ &&
        focus_widget != &month_prev_ &&
        focus_widget != &month_next_)
    {
        switch (ke->key())
        {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            ke->accept();
            return;
        case Qt::Key_Up:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_Down:
            ke->accept();
            return;
        default:
            break;
        }
    }
    ke->ignore();
}

void OnyxCalendar::onSelectionChanged()
{
}

void OnyxCalendar::onCurrentPageChanged(int y, int m)
{
    QString text;
    text = QString("%1").arg(calendar_.yearShown());
    year_label_.setText(text);

    text = QString("%1").arg(calendar_.monthShown());
    month_label_.setText(text);
}

void OnyxCalendar::onMonthNextClicked(bool)
{
    calendar_.showNextMonth();
    onCurrentPageChanged(0, 0);
}

void OnyxCalendar::onMonthPrevClicked(bool)
{
    calendar_.showPreviousMonth();
    onCurrentPageChanged(0, 0);
}

void OnyxCalendar::onYearNextClicked(bool)
{
    calendar_.showNextYear();
    onCurrentPageChanged(0, 0);
}

void OnyxCalendar::onYearPrevClicked(bool)
{
    calendar_.showPreviousYear();
    onCurrentPageChanged(0, 0);
}

};
