#ifndef ONYX_NUMBER_WIDGET_H_
#define ONYX_NUMBER_WIDGET_H_

#include "context_dialog_base.h"

namespace ui
{

class NumberWidget : public QWidget
{
    Q_OBJECT
public:
    explicit NumberWidget(QWidget *parent);
    ~NumberWidget(void);

Q_SIGNALS:
    void numberClicked(const int num);
    void backspaceClicked();
    void okClicked();

protected:
    void keyReleaseEvent(QKeyEvent *);

private:
    void createLayout();
    void clear();
    void clickFocusButton();
    bool focusUpDownChild(bool up);

private Q_SLOTS:
    void onNumberButtonClicked(bool checked);
    void onBackspaceClicked(bool checked);
    void onOKClicked(bool checked);

private:
    AbstractButtons         buttons_;        ///< All number buttons.
    scoped_ptr<QGridLayout> number_layout_;  ///< Layout.

private:
    NO_COPY_AND_ASSIGN(NumberWidget);
};

};      // namespace ui

#endif  // ONYX_NUMBER_WIDGET_H_
