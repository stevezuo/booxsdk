#include "onyx/ui/volume_control.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys.h"
#include "math.h"

namespace ui
{
VolumeControl::VolumeControl(QWidget *parent)
    : QWidget(parent)
    , current_(0)
    , min_(0)
    , max_(1)
    , pressing_value_(-1)
{
    SysStatus & sys_status = SysStatus::instance();
    SystemConfig sys_conf;
    min_ = sys_conf.minVolume();
    max_ = sys_conf.maxVolume();
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

    map_.clear();
    SystemConfig sys_conf;
    QVector<int>  volumes = sys_conf.volumes();
    int x = 2;
    for (int i = 1 ;i < volumes.size(); ++i)
    {
        p.fillRect(x,height()-30,15,15,current_ >= volumes[i] ? Qt::white : Qt::black);
        map_.insert(x,volumes[i]);
        x += 20;
    }
}

void VolumeControl::resizeEvent(QResizeEvent * event)
{

}

void VolumeControl::mousePressEvent(QMouseEvent *me)
{
    // Check position.
    me->accept();
    if ( me->y() < height() - 30 || me->y() > height() - 15 )
    {
        return;
    }

    int x = me->x() < 0 ? 0 : me->x();
    int value = 0;
    QMap<int, int>::const_iterator i = map_.begin();
    while (i != map_.end()) 
    {
        if ( i.key() > x)
        {
            break;
        }
        value = i.value();
        ++i;
    }

    if (value > max_)
    {
        value = max_;
    }

    pressing_value_ = current_;
    if (value != pressing_value_)
    {
        pressing_value_ = value;
#ifndef BUILD_FOR_ARM
        current_ = value;
#endif
        onyx::screen::instance().enableUpdate(false);
        repaint();
        onyx::screen::instance().updateWidget(
            this,
            onyx::screen::ScreenProxy::DW,
            false,
            onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
        onyx::screen::instance().enableUpdate(true);

        emit clicked(pressing_value_, pressing_value_);
    }
}

void VolumeControl::mouseMoveEvent(QMouseEvent *me)
{

}

void VolumeControl::mouseReleaseEvent(QMouseEvent *me)
{

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
    setFixedSize(200,200);
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
    , update_parent_(true)
    , always_active_(false)
    , image_(":/images/volume_strength.png")
{
    createLayout();
    setModal(false);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setFocusPolicy(Qt::NoFocus);

    // setWindowOpacity(0.4);
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
    int x = screen_rect.width()/2 - width()/2 ;
    int y = screen_rect.bottom()/2 - height()/2 ;

    // Check position.
    QPoint new_pos(x, y);
    update_parent_ = true;
    if (pos() != new_pos)
    {
        move(new_pos);
    }

    onyx::screen::instance().enableUpdate(false);
    repaint();
    /*
    onyx::screen::instance().updateWidget(
            this,
            onyx::screen::ScreenProxy::DW,
            true,
            onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
            */
    onyx::screen::instance().enableUpdate(true);

    // Make sure the widget is visible.
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC, false, onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
}

void VolumeControlDialog::moveEvent(QMoveEvent *e)
{
    update_parent_ = true;
}

void VolumeControlDialog::resizeEvent(QResizeEvent *e)
{
    update_parent_ = true;

    QPixmap pixmap=QPixmap::fromImage(image_);
    setMask(pixmap.mask());
}

void VolumeControlDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QBrush(QColor(190, 190, 190)));
    painter.drawImage(QPoint(0,0), image_);
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
