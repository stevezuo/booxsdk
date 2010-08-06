#include "onyx/ui/tts_actions.h"

namespace ui
{

TTSSpeakerActions::TTSSpeakerActions(void)
    : BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/speaker.png")));
}

TTSSpeakerActions::~TTSSpeakerActions(void)
{
}

void TTSSpeakerActions::generateActions(QStringList & speakers, const QString & current)
{
    category()->setText(QCoreApplication::tr("Speaker"));
    actions_.clear();

    QStringList::const_iterator begin = speakers.begin();
    QStringList::const_iterator end   = speakers.end();
    for(QStringList::const_iterator iter = begin; iter != end; ++iter)
    {
        // The text
        QString text = *iter;

        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(text, exclusiveGroup()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);

        QString icon_name = QString(":/images/%1.png").arg(text);
        act->setIcon(QIcon(QPixmap(icon_name)));
        if ( *iter == current )
        {
            act->setChecked(true);
        }

        actions_.push_back(act);
    }
}

void TTSSpeakerActions::setActionStatus(const QString & speaker, bool selected)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin; iter != end; ++iter)
    {
        if ( (*iter)->data().toString() == speaker )
        {
            (*iter)->setChecked( true );
            return;
        }
    }
}

QString TTSSpeakerActions::selectedSpeaker()
{
    // Search for the changed actions.
    QString selected;
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        selected = act->data().toString();
    }
    return selected;
}


TTSSpeedActions::TTSSpeedActions(void)
    : BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/speed.png")));
}

TTSSpeedActions::~TTSSpeedActions(void)
{
}

void TTSSpeedActions::generateActions(QVector<int> & speeds, const int current)
{
    category()->setText(QCoreApplication::tr("Speed"));
    actions_.clear();

    QVector<int>::const_iterator begin = speeds.begin();
    QVector<int>::const_iterator end   = speeds.end();
    for(QVector<int>::const_iterator iter = begin; iter != end; ++iter)
    {
        // The text
        QString text;
        switch ( *iter )
        {
        case 0:
            text = QCoreApplication::tr("Very Slow");
            break;
        case 1:
            text = QCoreApplication::tr("Slow");
            break;
        case 2:
            text = QCoreApplication::tr("Medium");
            break;
        case 3:
            text = QCoreApplication::tr("Fast");
            break;
        case 4:
            text = QCoreApplication::tr("Very Fast");
            break;
        default:
            text = QCoreApplication::tr("Invalid Speed");
            break;
        }

        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(text, exclusiveGroup()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);
        act->setIcon(QIcon(QPixmap(":/images/speed_item.png")));

        if ( *iter == current )
        {
            act->setChecked(true);
        }

        actions_.push_back(act);
    }
}

void TTSSpeedActions::setActionStatus(const int speed, bool selected)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ( (*iter)->data().toInt() == speed )
        {
            (*iter)->setChecked( selected );
            return;
        }
    }
}

int TTSSpeedActions::selectedSpeed()
{
    // Search for the changed actions.
    int speed = 2;
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        bool ok = false;
        speed = act->data().toInt(&ok);
    }
    return speed;
}

TTSStyleActions::TTSStyleActions(void)
    : BaseActions()
{
    category()->setIcon(QIcon(QPixmap(":/images/style.png")));
}

TTSStyleActions::~TTSStyleActions(void)
{
}

void TTSStyleActions::generateActions(QVector<int> & styles, const int current)
{
    category()->setText(QCoreApplication::tr("Style"));
    actions_.clear();

    QVector<int>::const_iterator begin = styles.begin();
    QVector<int>::const_iterator end   = styles.end();
    for(QVector<int>::const_iterator iter = begin; iter != end; ++iter)
    {
        // The text
        QString text;
        QString icon_name(":/images/style_item.png");
        switch ( *iter )
        {
        case SPEAK_STYLE_CLEAR:
            text = QCoreApplication::tr("Clear");
            icon_name = ":/images/tts_clear.png";
            break;
        case SPEAK_STYLE_NORMAL:
            text = QCoreApplication::tr("Normal");
            icon_name = ":/images/tts_normal.png";
            break;
        case SPEAK_STYLE_PLAIN:
            text = QCoreApplication::tr("Plain");
            icon_name = ":/images/tts_plain.png";
            break;
        case SPEAK_STYLE_VIVID:
            text = QCoreApplication::tr("Vivid");
            icon_name = ":/images/tts_vivid.png";
            break;
        default:
            text = QCoreApplication::tr("Invalid Speed");
            break;
        }

        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(text, exclusiveGroup()));

        // Change font and make it as checkable.
        act->setCheckable(true);
        act->setData(*iter);
        act->setIcon(QIcon(QPixmap(icon_name)));

        if ( *iter == current )
        {
            act->setChecked(true);
        }

        actions_.push_back(act);
    }
}

void TTSStyleActions::setActionStatus(const int style, bool selected)
{
    vector< shared_ptr<QAction> >::const_iterator begin = actions_.begin();
    vector< shared_ptr<QAction> >::const_iterator end   = actions_.end();
    for(vector< shared_ptr<QAction> >::const_iterator iter = begin;
        iter != end;
        ++iter)
    {
        if ( (*iter)->data().toInt() == style )
        {
            (*iter)->setChecked( selected );
            return;
        }
    }
}

int TTSStyleActions::selectedStyle()
{
    // Search for the changed actions.
    int style = SPEAK_STYLE_NORMAL;
    QAction * act = exclusiveGroup()->checkedAction();
    if (act)
    {
        bool ok = false;
        style = act->data().toInt(&ok);
    }
    return style;
}

}
