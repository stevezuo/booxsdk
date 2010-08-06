#ifndef IM_CHAR_SELECTION_H_
#define IM_CHAR_SELECTION_H_

#include "keyboard_utils.h"
#include "onyx/ui/buttons.h"
#include "keyboard_key.h"

namespace ui
{

class InputMethodCharSelection : public QWidget
{
    Q_OBJECT
public:
    InputMethodCharSelection(QWidget* parent = 0);
    ~InputMethodCharSelection();

    void displayCharacters(const QStringList & texts);
    void hideCharacters();

Q_SIGNALS:
    void textSelected(const QString & text, int index);
    void buttonClicked(QAbstractButton *button);

private:
    void init();

private Q_SLOTS:
    void onButtonClicked(QAbstractButton *button);

private:
    QVBoxLayout * ver_layout_;
    QVector< shared_ptr<QHBoxLayout> > hor_layouts_;
    scoped_ptr<QButtonGroup> button_group_;
    QVector< shared_ptr<OnyxPushButton> > buttons_;
    QVector< shared_ptr<KeyBoardKey> > keys_;

    QStringList candidates_;
};

};

#endif
