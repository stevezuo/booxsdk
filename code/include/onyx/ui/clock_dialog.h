#ifndef ONYX_CLOCK_DIALOG_H_
#define ONYX_CLOCK_DIALOG_H_

#include "onyx_dialog.h"

namespace ui
{

/// Clock dialog.
class ClockDialog : public OnyxDialog
{
    Q_OBJECT

public:
    explicit ClockDialog(const QDateTime & start, QWidget *parent = 0);
    ~ClockDialog(void);

public:
    int exec();

private:
    void createLayout();

private:
    QDateTime start_;
    QVBoxLayout layout_;
    QHBoxLayout time_layout_;
    QHBoxLayout reading_layout_;
    OnyxLabel time_number_;
    OnyxLabel year_label_;
    QLabel separator_;
    OnyxLabel reading_label_;
};

};  // namespace ui


#endif  // ONYX_CLOCK_DIALOG_H_
