#ifndef ONYX_KEYBOARD_LANGUAGE_DIALOG_H_
#define ONYX_KEYBOARD_LANGUAGE_DIALOG_H_

#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx/ui/ui.h"

namespace ui
{

class KeyboardLanguageDialog : public OnyxDialog
{
    Q_OBJECT
public:
    KeyboardLanguageDialog(QWidget *parent, const QLocale & current_locale);
    ~KeyboardLanguageDialog(void);

public:
    int exec();
    QLocale currentLocale() { return current_locale_; }

private:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    bool event(QEvent * event);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);
    void createLayout();

private Q_SLOTS:
    void onReturn();
    void onLanguageButtonClicked(bool);
    void onCloseClicked();

private:
    typedef OnyxCheckBox * CheckBoxPtr;
    typedef QVector<CheckBoxPtr> Buttons;
    typedef QVector<CheckBoxPtr>::iterator ButtonsIter;

private:
    QVBoxLayout ver_layout_;
    Buttons language_buttons_;
    QLocale current_locale_;

};

}; // namespace ui

#endif
