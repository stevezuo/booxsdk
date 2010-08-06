#ifndef AP_CONFIG_DIALOG_H_
#define AP_CONFIG_DIALOG_H_

#include "onyx/ui/ui.h"
#include "onyx/sys/sys.h"

namespace ui
{

/// Access point configuration dialog.
class ApConfigDialog : public OnyxDialog
{
    Q_OBJECT

public:
    ApConfigDialog(QWidget *parent, WifiProfile & profile);
    ~ApConfigDialog(void);

public:
    int popup();

protected:
    void mouseMoveEvent(QMouseEvent *me);
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyReleaseEvent(QKeyEvent *);
    void keyPressEvent(QKeyEvent * ke);
    bool event(QEvent * event);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);

private Q_SLOTS:
    void onShowPlainTextClicked();
    void onOKClicked();
    void onCloseClicked();
    void onTimeout();
    void onTextChanged(const QString& text);
    void onPlainButtonClicked();
    void onWepButtonClicked();
    void onWpaButtonClicked();
    void onWpa2ButtonClicked();

    void onSsidGetFocus(OnyxLineEdit *);
    void onPskGetFocus(OnyxLineEdit *);

private:
    void createLayout();
    void updateWidgets(WifiProfile & profile);
    void setReceiver(QWidget *receiver) { keyboard_receiver_ = receiver; }
    QWidget * receiver() { return keyboard_receiver_; }

    void setPlainProfile(WifiProfile & profile, const QString & id);
    void setWepProfile(WifiProfile & profile, const QString &id, const QString & key);
    void setWpaProfile(WifiProfile & profile, const QString &id, const QString & key);
    void setWpa2Profile(WifiProfile & profile, const QString &id, const QString & key);

private:
    QFormLayout form_layout_;
    QGridLayout  auth_hbox_;    ///< Authentication box.
    QGridLayout  enc_hbox_;     ///< Encryption box.
    QGridLayout  psk_hbox_;

    OnyxLabel ssid_label_;
    OnyxLabel auth_label_;
    OnyxLabel enc_label_;
    OnyxLabel psk_label_;

    OnyxLineEdit ssid_edit_;

    QButtonGroup auth_group_;
    OnyxPushButton plain_button_;
    OnyxPushButton wep_button_;
    OnyxPushButton wpa_psk_button_;
    OnyxPushButton wpa2_psk_button_;

    OnyxPushButton enc_tkip_button_;
    OnyxPushButton enc_ccmp_button_;

    OnyxLineEdit psk_edit_;

    OnyxPushButton ok_button_;
    OnyxPushButton clear_button_;
    OnyxCheckBox   show_plain_text_;   ///< Show plain text or star.

    KeyBoard      keyboard_;     ///< Keyboard.
    QTimer        timer_;        ///< Timer to update the screen.

    QWidget *keyboard_receiver_;
    WifiProfile & profile_;
};


};  // namespace ui

#endif  // AP_CONFIG_DIALOG_H_
