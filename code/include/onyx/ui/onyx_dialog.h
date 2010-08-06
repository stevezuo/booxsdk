#ifndef ONYX_DIALOG_H_
#define ONYX_DIALOG_H_

#include "buttons.h"
#include "label.h"
#include "soft_shadow.h"

namespace ui
{

/// Onyx dialog with soft shadow and basic title support.
class OnyxDialog : public QDialog
{
    Q_OBJECT

public:
    OnyxDialog(QWidget *parent);
    ~OnyxDialog(void);

    void updateTitle(const QString &message);

protected:
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);
    void hideEvent(QHideEvent * event);
    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

    void createDefaultLayout();
    void updateTitleIcon(const QPixmap& pixmap);

    void showCloseButton(bool show = true);
    QRect outbounding(QWidget *parent);

protected Q_SLOTS:
    virtual void onCloseClicked();

protected:
    Shadows shadows_;
    QVBoxLayout  vbox_;

    QWidget title_widget_;
    QVBoxLayout title_vbox_;
    QHBoxLayout  title_hbox_;
    QLabel top_separator_;
    OnyxLabel title_icon_label_;
    OnyxLabel title_text_label_;
    OnyxPushButton close_button_;

    QWidget content_widget_;

    static const int SPACING = 2;
    static const int WIDGET_HEIGHT = 36;
};

};  // namespace ui

#endif  // ONYX_DIALOG_H_
