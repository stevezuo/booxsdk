#include "onyx/ui/onyx_dialog.h"

namespace ui
{

static const QString BUTTON_STYLE =    "\
QPushButton                             \
{                                       \
    background: transparent;            \
    font-size: 14px;                    \
    border-width: 1px;                  \
    border-color: transparent;          \
    border-style: solid;                \
    color: black;                       \
    padding: 0px;                       \
}                                       \
QPushButton:pressed                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: black;                \
    background-color: black;            \
}                                       \
QPushButton:checked                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: black;                \
    background-color: black;            \
}                                       \
QPushButton:disabled                    \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    border-color: dark;                 \
    color: dark;                        \
    background-color: white;            \
}";

OnyxDialog::OnyxDialog(QWidget *parent)
    : QDialog(parent, Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint)
    , shadows_(parent)
    , vbox_(this)
    , title_widget_(this)
    , title_vbox_(&title_widget_)
    , title_hbox_(0)
    , top_separator_(this)
    , title_icon_label_(this)
    , title_text_label_(this)
    , close_button_("", this)
{
    setAutoFillBackground(true);
    setBackgroundRole(QPalette::Button);

    createDefaultLayout();
}

OnyxDialog::~OnyxDialog(void)
{
}

void OnyxDialog::createDefaultLayout()
{
    vbox_.setSizeConstraint(QLayout::SetMinAndMaxSize);
    vbox_.setSpacing(SPACING);
    vbox_.setContentsMargins(0, 0, 0, 0);

    title_vbox_.setSpacing(0);
    title_vbox_.setContentsMargins(SPACING, 0, SPACING, 0);

    // title hbox.
    title_icon_label_.setPixmap(QPixmap(":/images/dictionary_search.png"));
    title_text_label_.setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    title_icon_label_.setFixedHeight(WIDGET_HEIGHT);
    title_text_label_.setFixedHeight(WIDGET_HEIGHT);
    title_text_label_.useTitleBarStyle();

    close_button_.setStyleSheet(BUTTON_STYLE);
    QPixmap close_pixmap(":/images/close.png");
    close_button_.setIconSize(close_pixmap.size());
    close_button_.setIcon(QIcon(close_pixmap));
    close_button_.setFocusPolicy(Qt::NoFocus);
    connect(&close_button_, SIGNAL(clicked()), this, SLOT(onCloseClicked()), Qt::QueuedConnection);

    title_hbox_.addWidget(&title_icon_label_);
    title_hbox_.addSpacing(SPACING * 4);
    title_hbox_.addWidget(&title_text_label_, 500);
    title_hbox_.addWidget(&close_button_);
    title_hbox_.setContentsMargins(SPACING << 1, SPACING, SPACING << 1, SPACING);
    title_vbox_.addLayout(&title_hbox_);

    // title seperator
    top_separator_.setFocusPolicy(Qt::NoFocus);
    top_separator_.setFixedHeight(1);
    top_separator_.setFrameShape(QFrame::HLine);
    top_separator_.setAutoFillBackground(true);
    top_separator_.setBackgroundRole(QPalette::Light);
    title_vbox_.addWidget(&top_separator_);

    // title widget.
    title_widget_.setAutoFillBackground(true);
    title_widget_.setBackgroundRole(QPalette::Dark);
    title_widget_.setContentsMargins(0, 0, 0, 0);
    title_widget_.setFixedHeight(WIDGET_HEIGHT + SPACING * 2);
    vbox_.addWidget(&title_widget_);

    // content widget.
    content_widget_.setAutoFillBackground(true);
    content_widget_.setBackgroundRole(QPalette::Mid);
    content_widget_.setContentsMargins(2 * SPACING, SPACING, 2 * SPACING, SPACING);
    vbox_.addWidget(&content_widget_);
}

void OnyxDialog::updateTitle(const QString &message)
{
    title_text_label_.setText(message);
}

void OnyxDialog::updateTitleIcon(const QPixmap& pixmap)
{
    title_icon_label_.setPixmap(pixmap);
}

void OnyxDialog::showCloseButton(bool show)
{
    close_button_.setVisible(show);
}

QRect OnyxDialog::outbounding(QWidget *parent)
{
    QRect rc(rect());
    rc.adjust(0, 0, Shadows::PIXELS << 1, Shadows::PIXELS << 1);
    rc.moveTo(mapToGlobal(rc.topLeft()));
    return rc;
}

void OnyxDialog::onCloseClicked()
{
    shadows_.show(false);
    done(QDialog::Rejected);
}

void OnyxDialog::moveEvent(QMoveEvent *e)
{
    shadows_.onWidgetMoved(this);
}

void OnyxDialog::resizeEvent(QResizeEvent *e)
{
    QPainterPath p;
    p.addRoundedRect(rect(), 10, 10);
    QRegion maskedRegion(p.toFillPolygon().toPolygon());
    setMask(maskedRegion);

    shadows_.onWidgetResized(this);
}

void OnyxDialog::hideEvent(QHideEvent * event)
{
    QDialog::hideEvent(event);
    shadows_.show(false);
}

void OnyxDialog::keyPressEvent(QKeyEvent * event)
{
    event->accept();
}

void OnyxDialog::keyReleaseEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Escape)
    {
        reject();
    }
}

}   // namespace ui

