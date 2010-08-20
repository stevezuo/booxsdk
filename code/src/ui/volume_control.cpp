#include "onyx/ui/volume_control.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys.h"
#include "math.h"

namespace ui
{

static const int MARGIN = 0;
static const int Y_POS  = 18;
static const double PI  = 3.1415926;
static const double angle = 15.0;
static const int WIDTH  = 120;

VolumeControl::VolumeControl(QWidget *parent)
    : QWidget(parent)
    , current_(0)
    , min_(0)
    , max_(1)
    , pressing_value_(-1)
{
    // TODO update min and max value by system status
    SysStatus & sys_status = SysStatus::instance();
    min_ = 0;
    max_ = 100;
    current_ = sys_status.volume() - min_;

    createLayout();
    timer_.setSingleShot(true);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));

    // connect the signals with sys_state_
    connect(&sys_status, SIGNAL(volumeChanged(int, bool)), this, SLOT(onVolumeChanged(int, bool)));

    connect(this, SIGNAL(clicked(const int, const int)), this, SLOT(onClicked(const int, const int)));
    connect(this, SIGNAL(changing(const int, const int)), this, SLOT(onChanged(const int, const int)));
}

VolumeControl::~VolumeControl(void)
{
}

void VolumeControl::onVolumeChanged(int volume, bool is_mutel)
{
    if (volume == current_)
    {
        return;
    }
    current_ = volume - min_;

    updatefgPath(current_);
    onyx::screen::instance().enableUpdate(false);
    repaint();
    onyx::screen::instance().updateWidget(
        this,
        onyx::screen::ScreenProxy::DW,
        false,
        onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
    onyx::screen::instance().enableUpdate(true);
}

void VolumeControl::paintEvent(QPaintEvent *pe)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.fillPath(bk_path_, Qt::white);
    p.fillPath(fg_path_, Qt::black);
}

void VolumeControl::resizeEvent(QResizeEvent * event)
{
    int height = (static_cast<double>(width()) * tan(angle * PI / 180.0));
    QRect rc(0, 0, width(), height);
    updatePath(bk_path_, rc);

    if (pressing_value_ > 0)
    {
        updatefgPath(pressing_value_);
    }
    else
    {
        updatefgPath(current_);
    }
}

void VolumeControl::mousePressEvent(QMouseEvent *me)
{
    // Check position.
    me->accept();
    int x = me->x() < 0 ? 0 : me->x();
    int value = x * max_ / (width() - 2) + 1;
    if (value > max_)
    {
        value = max_;
    }

    pressing_value_ = current_;
    if (value != pressing_value_)
    {
        pressing_value_ = value;
        updatefgPath(pressing_value_);
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().updateWidget(
            this,
            onyx::screen::ScreenProxy::DW,
            false,
            onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
        onyx::screen::instance().enableUpdate(true);
        timer_.stop();
        timer_.start(300);
    }
}

void VolumeControl::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
    int x = me->x() < 0 ? 0 : me->x();
    int value = x * max_ / (width() - 2) + 1;
    if (value > max_)
    {
        value = max_;
    }

    if (value != pressing_value_)
    {
        pressing_value_ = value;
        updatefgPath(pressing_value_);
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().updateWidget(
            this,
            onyx::screen::ScreenProxy::DW,
            false,
            onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
        onyx::screen::instance().enableUpdate(true);
        timer_.stop();
        timer_.start(300);
    }
}

void VolumeControl::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    timer_.stop();
    if (current_ != pressing_value_)
    {
        emit clicked(pressing_value_ * 100 / max_, pressing_value_);
    }
    pressing_value_ = -1;
}

void VolumeControl::onTimeout()
{
    static int count = 0;
    qDebug("timeout %d value %d total %d", ++count, pressing_value_, max_);
    emit changing(pressing_value_, max_);
    timer_.stop();
}

void VolumeControl::createLayout()
{
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
    setFixedSize(WIDTH, WIDTH *  tan(angle * PI / 180.0));
}

void VolumeControl::updatefgPath(int value)
{
    int w = (width() - MARGIN * 2) * value / max_;
    int height = (static_cast<double>(w) * tan(angle * PI / 180.0));
    int bk_height = WIDTH *  tan(angle * PI / 180.0);
    QRect rc(MARGIN, bk_height - height + MARGIN, w, height - MARGIN);
    updatePath(fg_path_, rc);
}

void VolumeControl::updatePath(QPainterPath & result, const QRect & rect)
{
    int x_start = rect.left();
    int x_end   = rect.right();
    int top     = rect.top();
    int bottom  = rect.bottom();

    QPainterPath path;
    path.moveTo(x_end, top);
    path.lineTo(x_start, bottom);
    path.lineTo(x_end, bottom);
    path.lineTo(x_end, top);
    path.closeSubpath();

    result = path;
}

void VolumeControl::onClicked(const int percent, const int value)
{
    SysStatus & sys_status = SysStatus::instance();
    sys_status.setVolume(value + min_);
}

void VolumeControl::onChanged(const int value, const int total)
{
    SysStatus & sys_status = SysStatus::instance();
    sys_status.setVolume(value + min_);
}

// VolumeControlDialog
VolumeControlDialog::VolumeControlDialog(QWidget *parent)
    : QDialog(parent, static_cast<Qt::WindowFlags>(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint))
    , layout_(this)
    , control_(0)
    , update_parent_(false)
    , always_active_(false)
{
    createLayout();
    setModal(false);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setFocusPolicy(Qt::NoFocus);
}

VolumeControlDialog::~VolumeControlDialog()
{
}

void VolumeControlDialog::createLayout()
{
    // hbox to layout line edit and buttons.
    layout_.setContentsMargins(4, 4, 4, 4);
    layout_.setSpacing(2);

    layout_.addWidget(&control_);
}

void VolumeControlDialog::ensureVisible()
{
    SysStatus & sys_status = SysStatus::instance();
    control_.onVolumeChanged(sys_status.volume(), false);

    if (!isVisible())
    {
        show();
    }

    QRect screen_rect = qApp->desktop()->screenGeometry();
    int x = screen_rect.width() - width() - 10;
    int y = screen_rect.bottom() - height() - 40;

    // Check position.
    QPoint new_pos(x, y);
    update_parent_ = true;
    if (pos() != new_pos)
    {
        move(new_pos);
    }

    // Make sure the widget is visible.
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GU);
}

void VolumeControlDialog::moveEvent(QMoveEvent *e)
{
    update_parent_ = true;
}

void VolumeControlDialog::resizeEvent(QResizeEvent *e)
{
    update_parent_ = true;
}

void VolumeControlDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(rect(), QBrush(QColor(100, 100, 100)));
}

void VolumeControlDialog::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void VolumeControlDialog::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void VolumeControlDialog::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    if (!rect().contains(me->pos()))
    {
        reject();
    }
}

bool VolumeControlDialog::event(QEvent *e)
{
    int ret = QDialog::event(e);
    if (e->type() == QEvent::UpdateRequest)
    {
        if (update_parent_)
        {
            onyx::screen::instance().updateWidget(0, onyx::screen::ScreenProxy::GC);
            update_parent_ = false;
        }
        else
        {
            onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
        }
        e->accept();
    }
    return ret;
}

void VolumeControlDialog::keyPressEvent(QKeyEvent *ke)
{
    ke->ignore();
}

void VolumeControlDialog::keyReleaseEvent(QKeyEvent *ke)
{
    int key = ke->key();
    if (key == Qt::Key_Escape)
    {
        done(QDialog::Rejected);
        ke->accept();
        return;
    }
    ke->ignore();
}

}   // namespace ui
