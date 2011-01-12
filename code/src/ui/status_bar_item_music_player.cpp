#include "onyx/base/device.h"
#include "onyx/ui/status_bar_item_music_player.h"
#include "onyx/sys/sys.h"

namespace ui
{

enum MusicPlayerState
{
    START_PLAYER = 0,   ///< To launch player or show the player.
    STOP_PLAYER,        ///< To stop player.
    SHOW_PLAYER,        ///< Show player. TODO, could be removed.
    HIDE_PLAYER,        ///< Play background.
    MUSIC_PLAYING,
    MUSIC_STOPPED,
    MUSIC_PAUSED,
    MUSIC_QUIT
};

StatusBarItemMusicPlayer::StatusBarItemMusicPlayer(QWidget *parent)
    : StatusBarItem(MUSIC_PLAYER, parent)
    , img_ (":/images/music_player_off.png")
    , state_(0)
{
    createLayout();
}

StatusBarItemMusicPlayer::~StatusBarItemMusicPlayer(void)
{
}

void StatusBarItemMusicPlayer::createLayout()
{
qDebug()<<"musicplayer createLayout";
    connect(&sys::SysStatus::instance(), SIGNAL(musicPlayerStateChanged(int)), this, SLOT(setImage(int)));
}

void StatusBarItemMusicPlayer::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
qDebug()<<"musicplayer paint";
    
    QPoint point;
    point.rx() = ((rect().width() - img_.width()) >> 1);
    point.ry() = ((rect().height() - img_.height()) >> 1);
    painter.drawImage(point, img_);
}

void StatusBarItemMusicPlayer::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void StatusBarItemMusicPlayer::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
    emit clicked();
}

/// Retrieve image item according to battery value and status.
void StatusBarItemMusicPlayer::setImage(int state)
{
    if (state == HIDE_PLAYER)
    {
        if(state_ == 0)
        {
            img_ = QImage(":/images/music_player_playing.png");
        }
    }
    else if(state == MUSIC_PLAYING)
    {
        state_ = state;
        img_ = QImage(":/images/music_player_playing.png");
    }
    else if(state == MUSIC_PAUSED)
    {
        state_ = state;
        img_ = QImage(":/images/music_player_paused.png");
    }
    else if(state == MUSIC_STOPPED)
    {
        state_ = state;
        img_ = QImage(":/images/music_player_stopped.png");
    }
    else if(state == STOP_PLAYER || 
        !sys::SysStatus::instance().isMusicPlayerRunning())
    {
        state_ = 0;
        img_ = QImage("");
    }
    setFixedWidth(img_.width());
}

}
