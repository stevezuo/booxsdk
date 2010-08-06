
#include "onyx/ui/keyboard_navigator.h"

namespace ui
{
typedef QList<QWidget *> QWidgets;
static const int THRES_HOLD = 5;

// Return screen x position of a widget.
static int screen_x_pos(QWidget *widget)
{
    return widget->mapToGlobal(widget->rect().topLeft()).x();
}

static int screen_y_pos(QWidget *widget)
{
    return widget->mapToGlobal(widget->rect().topLeft()).y();
}

static int screen_y_center(QWidget *widget)
{
    int y = widget->mapToGlobal(widget->rect().topLeft()).y();
    return y + widget->size().height() / 2;
}

struct LessByYCenter
{
    bool operator()( QWidget * a, QWidget *b ) const
    {
        return (screen_y_center(a) < screen_y_center(b));
    }
};

struct GreaterByYCenter
{
    bool operator() ( QWidget *a, QWidget * b) const
    {
        return (screen_y_center(a) > screen_y_center(b));
    }
};

struct LessByXPos
{
    bool operator () (QWidget *a, QWidget *b) const
    {
        return (screen_x_pos(a) < screen_x_pos(b));
    }
};

struct GreaterByXPos
{
    bool operator() ( QWidget *a, QWidget *b) const
    {
        return (screen_x_pos(a) > screen_x_pos(b));
    }
};

struct LessByYPos
{
    bool operator() ( QWidget * a, QWidget * b) const
    {
        return (screen_y_pos(a) < screen_y_pos(b));
    }
};

struct GreaterByYPos
{
    bool operator() ( QWidget *a, QWidget * b) const
    {
        return (screen_y_pos(a) > screen_y_pos(b));
    }
};

template<class Comp>
bool collect(QWidget * current, QWidgets & children, QWidgets & candidates, Comp c)
{
    foreach(QWidget *p, children)
    {
        if (c(p, current))
        {
            candidates.push_back(p);
        }
    }
    return (candidates.size() > 0);
}

static QWidget * searchYLess(QWidget *parent,
                             QWidgets & children,
                             QWidget *current)
{

    QWidgets candidates;
    if (!collect(current, children, candidates, LessByYPos()))
    {
        if (!collect(current, children, candidates, GreaterByYPos()))
        {
            return 0;
        }
    }

    qStableSort(candidates.begin(), candidates.end(), GreaterByYCenter());
    int dist = INT_MAX;
    QWidget * result = candidates.front();
    foreach(QWidget *p, candidates)
    {
        int diff = abs(screen_x_pos(p) - screen_x_pos(current));
        if (dist > diff && dist > THRES_HOLD)
        {
            dist = diff;
            result = p;
        }
    }
    return result;
}

static QWidget * searchYGreater(QWidget *parent,
                                QWidgets & children,
                                QWidget *current)
{
    QWidgets candidates;
    if (!collect(current, children, candidates, GreaterByYPos()))
    {
        if (!collect(current, children, candidates, LessByYPos()))
        {
            return 0;
        }
    }
    qStableSort(candidates.begin(), candidates.end(), LessByYCenter());
    int dist = INT_MAX;
    QWidget * result = candidates.front();
    foreach(QWidget *p, candidates)
    {
        if (dist > abs(screen_x_pos(p)- screen_x_pos(current)))
        {
            dist = abs(screen_x_pos(p)- screen_x_pos(current));
            result = p;
        }
    }
    return result;
}

static QWidget * searchXLess(QWidget *parent,
                             QWidgets & children,
                             QWidget *current)
{
    QWidgets candidates;
    if (!collect(current, children, candidates, LessByXPos()))
    {
        if (!collect(current, children, candidates, GreaterByXPos()))
        {
            return 0;
        }
    }

    qStableSort(candidates.begin(), candidates.end(), GreaterByXPos());
    int dist = INT_MAX;
    QWidget * result = candidates.front();
    foreach(QWidget *p, candidates)
    {
        int value = abs(screen_y_center(p) - screen_y_center(current));
        if (dist > value)
        {
            dist = value;
            result = p;
        }
    }
    return result;
}

static QWidget * searchXGreater(QWidget *parent,
                                QWidgets & children,
                                QWidget *current)
{
    QWidgets candidates;
    if (!collect(current, children, candidates, GreaterByXPos()))
    {
        if (!collect(current, children, candidates, LessByXPos()))
        {
            return 0;
        }
    }

    qStableSort(candidates.begin(), candidates.end(), LessByXPos());
    int dist = INT_MAX;
    QWidget * result = candidates.front();
    foreach(QWidget *p, candidates)
    {
        int value = abs(screen_y_center(p) - screen_y_center(current));
        if (dist > value)
        {
            dist = value;
            result = p;
        }
    }
    return result;
}

static void collectWidgets(const QObject *parent,
                           QWidgets &widgets)
{
    const QObjectList & all = parent->children();
    foreach(const QObject *object, all)
    {
        if (object->isWidgetType())
        {
            QWidget * wnd = const_cast<QWidget *>(static_cast<const QWidget *>(object));
            if (wnd && wnd->isVisible())
            {
                if (wnd->focusPolicy() != Qt::NoFocus)
                {
                    widgets.push_back(wnd);
                }
                collectWidgets(wnd, widgets);
            }
        }
    }
}

static QWidget * search(QWidget *parent,
                        QWidget *current,
                        int key)
{
    if (parent == 0)
    {
        return 0;
    }

    // Need to find out which widgets need to be compared.
    QWidgets widgets;
    collectWidgets(parent, widgets);

    // Sort them based on key.
    if (key == Qt::Key_Up)
    {
        return searchYLess(parent, widgets, current);
    }
    else if (key == Qt::Key_Down)
    {
        return searchYGreater(parent, widgets, current);
    }
    else if (key == Qt::Key_Left || key == Qt::Key_PageUp)
    {
       return searchXLess(parent, widgets, current);
    }
    else if (key == Qt::Key_Right || key == Qt::Key_PageDown)
    {
        return searchXGreater(parent, widgets, current);
    }
    return 0;
}

QWidget * moveFocus(QWidget *parent, int key)
{
    QWidget *current = parent->focusWidget();
    if (current)
    {
        return search(parent, current, key);
    }
    return 0;
}

}


