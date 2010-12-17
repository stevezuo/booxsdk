#include "onyx/ui/volume_control.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys.h"
#include "math.h"

namespace ui
{

static const int TIMEOUT = 2000;

// VolumeControlDialog
VolumeControlDialog::VolumeControlDialog(QWidget *parent)
    : QDialog(parent, static_cast<Qt::WindowFlags>(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint))
    , layout_(this)
    , image_(":/images/volume_strength.png")
    , current_(0)
    , min_(0)
    , max_(1)
    , pressing_value_(-1)
    , label_(0)
{
    SysStatus & sys_status = SysStatus::instance();
    SystemConfig sys_conf;
    min_ = sys_conf.minVolume();
    max_ = sys_conf.maxVolume();
    current_ = sys_status.volume() - min_;

    // connect the signals with sys_state_
    connect(&sys_status, SIGNAL(volumeChanged(int, bool)), this, SLOT(setVolume(int, bool)));

    createLayout();
    setModal(true);
    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    setFocusPolicy(Qt::NoFocus);

    timer_.setSingleShot(true);
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));
}

VolumeControlDialog::~VolumeControlDialog()
{
}

void VolumeControlDialog::createLayout()
{
    // hbox to layout line edit and buttons.
    layout_.setContentsMargins(4, 4, 4, 4);
    layout_.setSpacing(2);
    layout_.addWidget(&label_);
    label_.setAlignment(Qt::AlignLeft|Qt::AlignTop);

    QPixmap pixmap=QPixmap::fromImage(image_);
    label_.setPixmap(pixmap);
}

void VolumeControlDialog::done(int r)
{
    stopTimer();
    QDialog::done(r);
}

void VolumeControlDialog::resetTimer()
{
    timer_.stop();
    timer_.start(TIMEOUT);
}

int VolumeControlDialog::ensureVisible()
{
    show();
    const QRect screen = QApplication::desktop()->screenGeometry();
    move( screen.center() - this->rect().center() );
    
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GC, false, onyx::screen::ScreenCommand::WAIT_ALL);

    resetTimer();
    return exec();
}

void VolumeControlDialog::moveEvent(QMoveEvent *e)
{
}

void VolumeControlDialog::resizeEvent(QResizeEvent *e)
{
    QPainterPath p;
    p.addRoundedRect(rect(), 10, 10);
    QRegion maskedRegion(p.toFillPolygon().toPolygon());
    setMask(maskedRegion);
}

void VolumeControlDialog::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QBrush(QColor(190, 190, 190)));

    map_.clear();
    SystemConfig sys_conf;
    QVector<int>  volumes = sys_conf.volumes();
    int x = 10;
    for (int i = 1 ;i < volumes.size(); ++i)
    {
        painter.fillRect(x,height()-30,15,15,current_ >= volumes[i] ? Qt::white : Qt::black);
        map_.insert(x,volumes[i]);
        x += 20;
    }
}

void VolumeControlDialog::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void VolumeControlDialog::mousePressEvent(QMouseEvent *me)
{
    // Check position.
    resetTimer();
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

    if (value != current_)
    {
        sys::SysStatus::instance().setVolume(value);
#ifndef BUILD_FOR_ARM
        setVolume(value, false);
#endif
    }
}

void VolumeControlDialog::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    if (!rect().contains(me->pos()))
    {
        reject();
    }
}

void VolumeControlDialog::onScreenUpdateRequest()
{
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU, false, onyx::screen::ScreenCommand::WAIT_COMMAND_FINISH);
}


bool VolumeControlDialog::event(QEvent *e)
{
    qDebug("process event %d", e->type());
    int ret = QDialog::event(e);
    if (e->type() == QEvent::UpdateRequest && onyx::screen::instance().isUpdateEnabled())
    {
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW);
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

void VolumeControlDialog::stopTimer()
{
    timer_.stop();
}

void VolumeControlDialog::setVolume(int volume, bool is_mute)
{
    static int count = 0;
    if (current_ == volume)
    {
        return;
    }
    current_ = volume;
    repaint();

    qDebug("timeout %d value %d total %d", ++count, current_, max_);
}

void VolumeControlDialog::onTimeout()
{
    stopTimer();
    accept();
}

}   // namespace ui
