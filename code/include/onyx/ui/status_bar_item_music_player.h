#ifndef ONYX_STATUS_BAR_ITEM_MUSIC_PLAYER_H_
#define ONYX_STATUS_BAR_ITEM_MUSIC_PLAYER_H_

#include "status_bar_item.h"

namespace ui
{

class StatusBarItemMusicPlayer : public StatusBarItem
{
    Q_OBJECT

public:
    StatusBarItemMusicPlayer(QWidget *parent);
    virtual ~StatusBarItemMusicPlayer(void);

public Q_SLOTS:
    void setImage(int);

Q_SIGNALS:
    void clicked();

private:
    virtual void paintEvent(QPaintEvent *);
    virtual void mousePressEvent(QMouseEvent *);
    virtual void mouseReleaseEvent(QMouseEvent *);

    void createLayout();

private:
    QImage img_;
    int state_;
};

};  // namespace ui

#endif  // ONYX_STATUS_BAR_ITEM_MUSIC_PLAYER_H_
