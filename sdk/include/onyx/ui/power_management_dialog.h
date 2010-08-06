#ifndef UI_POWER_MANAGEMENT_DIALOG_H_
#define UI_POWER_MANAGEMENT_DIALOG_H_

#include "onyx/base/base.h"
#include "onyx/sys/sys.h"
#include "onyx_dialog.h"

namespace ui
{

/// Locale widget. Not sure we should place it in the ui library.
/// So far, only explorer use it.
class PowerManagementDialog : public OnyxDialog
{
    Q_OBJECT

public:
    PowerManagementDialog(QWidget *parent, SysStatus & ref);
    ~PowerManagementDialog(void);

public:
    int exec();

private:
    void createLayout();
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    bool event(QEvent*);

private Q_SLOTS:
    void onStandbyButtonChanged(bool);
    void onShutdownButtonChanged(bool);
    void onOkClicked(bool);

private:
    typedef OnyxCheckBox * CheckBoxPtr;
    typedef std::vector<CheckBoxPtr> Buttons;
    typedef std::vector<CheckBoxPtr>::iterator ButtonsIter;

    SysStatus & status_;
    QVBoxLayout ver_layout_;

    QVBoxLayout standby_layout_;
    QHBoxLayout standby_hor_layout_;
    OnyxLabel standby_image_label_;
    OnyxLabel standby_text_label_;
    QButtonGroup standby_group_;
    Buttons standby_buttons_;

    QVBoxLayout shutdown_layout_;
    QHBoxLayout shutdown_hor_layout_;
    OnyxLabel shutdown_image_label_;
    OnyxLabel shutdown_text_label_;
    QButtonGroup shutdown_group_;
    Buttons shutdown_buttons_;

    QHBoxLayout hor_layout_;
    OnyxPushButton ok_;

    int sys_standby_interval_;
    int standby_interval_;
    int sys_shutdown_interval_;     ///< From system manager.
    int shutdown_interval_;         ///< User specified.
};

}   // namespace ui

#endif      // UI_POWER_MANAGEMENT_DIALOG_H_
