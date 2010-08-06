#ifndef UI_LIB_TAB_BAR_H_
#define UI_LIB_TAB_BAR_H_

#include <vector>
#include "tab_button.h"

namespace ui
{

class TabBar : public QWidget
{
    Q_OBJECT
public:
    TabBar(QWidget *parent);
    ~TabBar(void);

public:
    bool AddButton(const int id, const QString & title, const QIcon & icon);
    bool RemoveButton(const int id);
    bool ClickButton(const int id);
    int  GetSelectedButton();
    bool SetButtonText(const int id, const QString & title);

    bool SetOrientation(const Qt::Orientation orientation);
    Qt::Orientation Orientation() const { return orientation_; }

Q_SIGNALS:
    void ButtonClicked(const int id);

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *);
    virtual void keyPressEvent(QKeyEvent *);
    virtual void keyReleaseEvent(QKeyEvent *);

private Q_SLOTS:
    void OnClicked(const int id, bool check);

private:
    void InitLayout();
    void Clear();
    void SetFocusNextPrevChild(bool next);
    void ClickSelectedChild();

private:
    QBoxLayout layout_;
    Qt::Orientation orientation_;
    typedef std::vector<TabButton *> Buttons;
    typedef Buttons::iterator ButtonIter;
    Buttons buttons_;
};

};

#endif
