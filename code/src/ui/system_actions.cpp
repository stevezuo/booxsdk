#ifdef BUILD_FOR_ARM
#include <QtGui/qscreen_qws.h>
#endif

#include "onyx/ui/system_actions.h"
#include "onyx/screen/screen_proxy.h"
#include "onyx/sys/sys_conf.h"

namespace ui
{

SystemActions::SystemActions(void)
    : BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/system.png")));
}

SystemActions::~SystemActions(void)
{
}

void SystemActions::generateActions(const std::vector<int> & actions)
{
    category()->setText(QCoreApplication::tr("System"));
    actions_.clear();

    std::vector<int> all = actions;
    if (all.size() <= 0)
    {
        all.push_back(ROTATE_SCREEN);
        all.push_back(SCREEN_UPDATE_TYPE);
        all.push_back(MUSIC);
        all.push_back(RETURN_TO_LIBRARY);
    }

    for(int i = 0; i < static_cast<int>(all.size()); ++i)
    {
        switch (all[i])
        {
        case ROTATE_SCREEN:
            {
                shared_ptr<QAction> rotate(new QAction(exclusiveGroup()));
                rotate->setCheckable(true);
                rotate->setText(QCoreApplication::tr("Rotate Screen"));
                rotate->setIcon(QIcon(QPixmap(":/images/screen_rotation.png")));
                rotate->setData(ROTATE_SCREEN);
                actions_.push_back(rotate);
                break;
            }
        case SCREEN_UPDATE_TYPE:
            {
                // Screen update type.
                shared_ptr<QAction> screen(new QAction(exclusiveGroup()));
                screen->setCheckable(true);
                if (onyx::screen::instance().defaultWaveform() == onyx::screen::ScreenProxy::GC)
                {
                    screen->setText(QCoreApplication::tr("Full Refresh Off"));
                    screen->setIcon(QIcon(QPixmap(":/images/fast_update.png")));
                }
                else
                {
                    screen->setText(QCoreApplication::tr("Full Refresh On"));
                    screen->setIcon(QIcon(QPixmap(":/images/full_update.png")));
                }
                screen->setData(SCREEN_UPDATE_TYPE);
                actions_.push_back(screen);
                break;
            }
        case FULL_SCREEN:
            {
                shared_ptr<QAction> fullScreen(new QAction(exclusiveGroup()));
                fullScreen->setCheckable(true);
                fullScreen->setText(QCoreApplication::tr("Full Screen"));
                fullScreen->setIcon(QIcon(QPixmap(":/images/full_screen.png")));
                fullScreen->setData(FULL_SCREEN);
                actions_.push_back(fullScreen);
                break;
            }
        case EXIT_FULL_SCREEN:
            {
                shared_ptr<QAction> exitFullScreen(new QAction(exclusiveGroup()));
                exitFullScreen->setCheckable(true);
                exitFullScreen->setText(QCoreApplication::tr("Exit Full Screen"));
                exitFullScreen->setIcon(QIcon(QPixmap(
                        ":/images/exit_full_screen.png")));
                exitFullScreen->setData(EXIT_FULL_SCREEN);
                actions_.push_back(exitFullScreen);
                break;
            }
        case MUSIC:
            {
                if (sys::SystemConfig::isMusicPlayerAvailable())
                {
                    // Music.
                    shared_ptr<QAction> music(new QAction(exclusiveGroup()));
                    music->setCheckable(true);
                    music->setText(QCoreApplication::tr("Music"));
                    music->setIcon(QIcon(QPixmap(":/images/music.png")));
                    music->setData(MUSIC);
                    actions_.push_back(music);
                }
                break;
            }
        case RETURN_TO_LIBRARY:
            {
                // Close document.
                shared_ptr<QAction> close(new QAction(exclusiveGroup()));
                close->setCheckable(true);
                close->setText(QCoreApplication::tr("Close"));
                close->setIcon(QIcon(QPixmap(":/images/return_to_library.png")));
                close->setData(RETURN_TO_LIBRARY);
                actions_.push_back(close);
                break;
            }
        }
    }
}

void SystemActions::addAboutAction()
{
    shared_ptr<QAction> about(new QAction(exclusiveGroup()));
    about->setCheckable(true);
    about->setText(QCoreApplication::tr("About"));
    about->setIcon(QIcon(QPixmap(":/images/about.png")));
    about->setData(ABOUT_INFO);
    actions_.push_back(about);
}

SystemAction SystemActions::selected()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        return static_cast<SystemAction>(act->data().toInt());
    }
    return INVALID_SYSTEM_ACTION;
}

}
