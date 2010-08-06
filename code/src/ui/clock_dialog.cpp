
#include "onyx/ui/clock_dialog.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{
const QString LABEL_STYLE = "           \
QLabel                                  \
{                                       \
     padding: 0px;                      \
     background: transparent;           \
     font: 48px ;                       \
     color: black;                      \
 }";


const QString DATE_FORMAT  = "hh:mm";


ClockDialog::ClockDialog(const QDateTime & start, QWidget *parent)
    : OnyxDialog(parent)
    , start_(start)
    , layout_(&content_widget_)
    , time_layout_(0)
    , reading_layout_(0)
    , time_number_(this)
    , year_label_(this)
    , separator_(0)
    , reading_label_(this)
{
    createLayout();
}

ClockDialog::~ClockDialog(void)
{
}

void ClockDialog::createLayout()
{
    content_widget_.setBackgroundRole(QPalette::Button);
    updateTitle(QApplication::tr("Clock"));
    updateTitleIcon(QPixmap(":/images/clock.png"));
    layout_.setSizeConstraint(QLayout::SetFixedSize);
    layout_.setContentsMargins(20, 20, 20, 20);
    time_layout_.setContentsMargins(0, 0, 0, 0);
    reading_layout_.setContentsMargins(0, 0, 0, 0);

    // time
    QDateTime current(QDateTime::currentDateTime());
    time_layout_.addWidget(&time_number_);
    QString string = current.toString(DATE_FORMAT);
    time_number_.setStyleSheet(LABEL_STYLE);
    time_number_.setText(string);

    // spacing.
    time_layout_.addSpacing(100);

    // year
    year_label_.setWordWrap(true);
    year_label_.setTextFormat(Qt::RichText);
    QString y("%1<br>%2-%3");
    y = y.arg(current.date().year()).arg(current.date().month()).arg(current.date().day());
    year_label_.setText(y);
    time_layout_.addWidget(&year_label_, 100);
    layout_.addLayout(&time_layout_);

    // separator
    separator_.setFocusPolicy(Qt::NoFocus);
    separator_.setFixedHeight(1);
    separator_.setFrameShape(QFrame::HLine);
    separator_.setAutoFillBackground(true);
    separator_.setBackgroundRole(QPalette::Light);
    layout_.addWidget(&separator_);

    // label
    reading_label_.setWordWrap(true);
    reading_layout_.addWidget(&reading_label_);
    int seconds = start_.secsTo(QDateTime::currentDateTime());

    if (seconds < 60)
    {
        QString str(tr("Reading time:<br> Less than one minute."));
        reading_label_.setText(str);
    }
    else if (seconds < 3600)
    {
        QString str(tr("Reading time:<br> %1 minutes."));
        str = str.arg(seconds / 60);
        reading_label_.setText(str);
    }
    else
    {
        QString str(tr("Reading time:<br> %1 hours %2 minutes."));
        str = str.arg(seconds / 3600).arg((seconds % 3600) / 60);
        reading_label_.setText(str);
    }

    layout_.addLayout(&reading_layout_);
}

int ClockDialog::exec()
{
    shadows_.show(true);
    show();
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidgetRegion(
        0,
        outbounding(parentWidget()),
        onyx::screen::ScreenProxy::GC,
        false,
        onyx::screen::ScreenCommand::WAIT_ALL);
    return QDialog::exec();
}

}
