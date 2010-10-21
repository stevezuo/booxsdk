#include "onyx/sys/sys.h"
#include "onyx/ui/keyboard_key.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

extern ShiftMap shift_map;
extern SpecialMaps special_maps;

const QString KEY_BUTTON_STYLE = "      \
QPushButton                             \
{                                       \
    background: white;                  \
    font-size: 18px;                    \
    border-width: 1px;                  \
    border-color: black;                \
    border-style: solid;                \
    border-radius: 3;                   \
    color: black;                       \
    padding: 0px;                       \
}                                       \
QPushButton:pressed                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: white;                \
    background-color: black;            \
}                                       \
QPushButton:focus                       \
{                                       \
    border-width: 3px;                  \
    border-color: black;                \
    border-style: solid;                \
    border-radius: 3;                   \
}                                       \
QPushButton:checked                     \
{                                       \
    padding-left: 0px;                  \
    padding-top: 0px;                   \
    color: white;                       \
    border-color: white;                \
    background-color: black;            \
}                                       \
QPushButton:disabled                    \
{                                       \
    color:transparent;                  \
    border-color:transparent;           \
    background-color:transparent;       \
}";

static QSize getKeySize(int code)
{
    QChar ch(code);
    int standard_key_size = standardKeySize();

    if (ch.isNumber())
    {
        return QSize(52, 52);
    }

    if (code < BSCode)
    {
        return QSize(standard_key_size, standard_key_size);
    }

    if (code > UnknownCode)
    {
        return QSize(standard_key_size, standard_key_size);
    }

    if (code == Blank)
    {
        bool has_touch = SysStatus::instance().hasTouchScreen();
        int len = (standard_key_size * (has_touch ? 4 : 6)) + (has_touch ? 12 : 20);
        if (isRussian() && isEnglishLayout())
        {
            len = standard_key_size * (has_touch ? 2 : 4) + (has_touch ? 4 : 12);
        }
        return QSize(len, standard_key_size);
    }

    if (code == EnterCode)
    {
        return QSize(((standard_key_size << 1) + 4) , standard_key_size);
    }

    if (code == BackSpace)
    {
        return QSize(((standard_key_size << 1) + 4), standard_key_size);
    }

    if (code == BackSlash)
    {
        return QSize(standard_key_size, standard_key_size);
    }

    if (code == DeleteCode)
    {
        return QSize(standard_key_size, standard_key_size);
    }

    if (code == ShiftCode)
    {
        return QSize(((standard_key_size << 1) + 4), standard_key_size);
    }

    if (code == HandWriting)
    {
        return QSize(((standard_key_size << 1) + 4), standard_key_size);
    }

    if (code == SwitchLanguage)
    {
        return QSize(((standard_key_size << 1) + 4), standard_key_size);
    }

    if (code == CapLock)
    {
       return QSize(((standard_key_size << 1) + 4), standard_key_size);
    }

    return QSize(standard_key_size, standard_key_size);
}

KeyBoardKey::KeyBoardKey(QWidget *parent)
    : QPushButton(parent)
    , code_(BSCode)
    , shift_code_(BSCode)
    , locked_(false)
    , shifted_(false)
{
    setStyleSheet(KEY_BUTTON_STYLE);
}

KeyBoardKey::~KeyBoardKey()
{
}

void KeyBoardKey::updateSizeByCode(const int code)
{
    setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));
    QSize size = getKeySize(code);
    setFixedSize(size);
}

void KeyBoardKey::setCode(const int code)
{
    code_ = code;
    shift_code_ = code;

    // TODO. set the size of the button
    if (code > BSCode && code < UnknownCode)
    {
        // set the size and image of the special key
        setText(special_maps[code - BSCode - 1].label);

        if (code == ShiftCode || code == CapLock)
        {
            setCheckable(true);
        }
    }
    else
    {
        // set the size and text of the regular key
        QChar text(code);
        setText(QString(text));
    }

    if (code == BackSpace)
    {
        QPixmap back_map(":/images/back_spac.png");
        setIcon(QIcon(back_map));
        setIconSize(back_map.size());
    }

    updateSizeByCode(code);
}

bool KeyBoardKey::isShiftKey()
{
    QChar c(code_);
    if (shift_map.contains(c))
    {
        return true;
    }
    return false;
}

void KeyBoardKey::handleShifted(bool shifted)
{
    shifted_ = shifted;
    updateText();
}

void KeyBoardKey::handleCapLocked(bool cap_locked)
{
    locked_ = cap_locked;
    updateText();
}

void KeyBoardKey::updateText()
{
    QChar c(code_);
    if (code_ == QChar('7') && shifted_)
    {
        setText("&&");
    return;
    }
    if (shifted_ && isShiftKey())
    {
        shift_code_ = shift_map[code_].unicode();
    }
    else if (c.isLetter() && locked_)
    {
        shift_code_ = c.toUpper().unicode();
    }
    else
    {
        shift_code_ = code_;
    }

    if (isShiftKey() || c.isLetter())
    {
        setText(QChar(shift_code_));
    }
}

void KeyBoardKey::keyPressEvent(QKeyEvent * event)
{
    event->accept();
}

void KeyBoardKey::keyReleaseEvent(QKeyEvent * event)
{
    switch (event->key())
    {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        event->accept();
        click();
        break;
    default:
        break;
    }
    event->ignore();
}

void KeyBoardKey::mousePressEvent(QMouseEvent *me)
{
    onyx::screen::instance().enableUpdate(false);
    QPushButton::mousePressEvent(me);
    onyx::screen::instance().enableUpdate(true);

    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW);
}

void KeyBoardKey::paintEvent(QPaintEvent *pe)
{
    QPushButton::paintEvent(pe);

    if (this->isEnabled())
    {
        QPainter p(this);
        QRect rc = rect().adjusted(1, 1, 0, 0);
        QPainterPath path;
        path.addRoundedRect(rc, 3, 3);
        QPen pen(Qt::SolidLine);
        pen.setColor(Qt::black);
        pen.setWidth(1);
        p.setPen(pen);
        p.drawPath(path);
    }
}

void KeyBoardKey::resizeEvent(QResizeEvent *re)
{
    if (isEnabled())
    {
        QPainterPath p;
        p.addRoundedRect(rect(), 3, 3);
        QRegion maskedRegion(p.toFillPolygon().toPolygon());
        setMask(maskedRegion);
    }
    else
    {
        QPushButton::resizeEvent(re);
    }
}

void KeyBoardKey::mouseReleaseEvent(QMouseEvent* me)
{
    onyx::screen::instance().enableUpdate(false);
    QPushButton::mouseReleaseEvent(me);
    onyx::screen::instance().enableUpdate(true);

    // screen::ScreenManager::instance().fastUpdateWidget(this, true, false);
}

bool KeyBoardKey::event(QEvent * e)
{
    switch (e->type())
    {
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        e->accept();
        return true;
    default:
        break;
    }
    return QPushButton::event(e);
}

}   // namespace ui
