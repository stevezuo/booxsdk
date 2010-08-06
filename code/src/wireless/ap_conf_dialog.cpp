#include "onyx/screen/screen_proxy.h"
#include "onyx/wireless/ap_conf_dialog.h"


namespace ui
{

ApConfigDialog::ApConfigDialog(QWidget *parent, WifiProfile & profile)
    : OnyxDialog(parent)
    , form_layout_(0)
    , auth_hbox_(0)
    , enc_hbox_(0)
    , ssid_label_(tr("SSID"), 0)
    , auth_label_(tr("Authentication"), 0)
    , enc_label_(tr("Encryption"), 0)
    , psk_label_(tr("PSK"), 0)
    , ssid_edit_("", 0)
    , auth_group_(0)
    , plain_button_(tr("None"), 0)
    , wep_button_(tr("WEP"), 0)
    , wpa_psk_button_(tr("WPA"), 0)
    , wpa2_psk_button_(tr("WPA2"), 0)
    , enc_tkip_button_(tr("TKIP"), 0)
    , enc_ccmp_button_(tr("AES"), 0)
    , psk_edit_("", 0)
    , ok_button_(tr("OK"), this)
    , clear_button_(tr("Clear"), this)
    , show_plain_text_(tr("Show Plain Text"), this)
    , keyboard_(this)
    , keyboard_receiver_(0)
    , profile_(profile)
{
    createLayout();

    // Widget attributes.
    setModal(true);
    setFocusPolicy(Qt::NoFocus);

    updateTitle(tr("Wifi Configuration"));
}

ApConfigDialog::~ApConfigDialog(void)
{
}

void ApConfigDialog::createLayout()
{
    // Title
    title_icon_label_.setPixmap(QPixmap(":/images/dictionary_search.png"));
    content_widget_.hide();
    vbox_.setContentsMargins(SPACING * 4, SPACING * 4, SPACING * 4, SPACING * 4);
    vbox_.setSpacing(SPACING * 4);
    vbox_.addLayout(&form_layout_);

    Qt::Alignment align = static_cast<Qt::Alignment>(Qt::AlignHCenter | Qt::AlignVCenter);
    form_layout_.setFormAlignment(align);
    form_layout_.addRow(&ssid_label_, &ssid_edit_);
    form_layout_.addRow(&auth_label_, &auth_hbox_);
    form_layout_.addRow(&enc_label_, &enc_hbox_);
    form_layout_.addRow(&psk_label_, &psk_hbox_);

    auth_hbox_.addWidget(&plain_button_, 0, 0);
    auth_hbox_.addWidget(&wep_button_, 0, 1);
    auth_hbox_.addWidget(&wpa_psk_button_, 0, 2);
    auth_hbox_.addWidget(&wpa2_psk_button_, 0, 3);
    auth_group_.addButton(&plain_button_);
    auth_group_.addButton(&wep_button_);
    auth_group_.addButton(&wpa_psk_button_);
    auth_group_.addButton(&wpa2_psk_button_);
    plain_button_.setCheckable(true);
    wep_button_.setCheckable(true);
    wpa_psk_button_.setCheckable(true);
    wpa2_psk_button_.setCheckable(true);

    enc_hbox_.addWidget(&enc_tkip_button_, 0, 0);
    enc_hbox_.addWidget(&enc_ccmp_button_, 0, 1);
    enc_tkip_button_.setCheckable(true);
    enc_ccmp_button_.setCheckable(true);

    // Line edit.
    psk_hbox_.addWidget(&psk_edit_, 0, 0);
    psk_hbox_.addWidget(&clear_button_, 0, 1);
    psk_hbox_.addWidget(&ok_button_, 0, 2);
    psk_edit_.setFixedHeight(WIDGET_HEIGHT);

    // Buttons.
    ok_button_.useDefaultHeight();
    show_plain_text_.setFixedHeight(WIDGET_HEIGHT);
    show_plain_text_.selectOnClicked(false);
    clear_button_.useDefaultHeight();

    show_plain_text_.setFocusPolicy(Qt::NoFocus);
    ok_button_.setFocusPolicy(Qt::NoFocus);
    show_plain_text_.selectOnClicked(false);
    clear_button_.setFocusPolicy(Qt::NoFocus);

    vbox_.addWidget(&show_plain_text_);

    // keyboard.
    connect(&ssid_edit_, SIGNAL(getFocus(OnyxLineEdit *)), this, SLOT(onSsidGetFocus(OnyxLineEdit *)));
    connect(&psk_edit_, SIGNAL(getFocus(OnyxLineEdit *)), this, SLOT(onPskGetFocus(OnyxLineEdit *)));

    keyboard_.attachReceiver(this);
    vbox_.addWidget(&keyboard_);

    // Setup connections.
    connect(&show_plain_text_, SIGNAL(clicked()), this, SLOT(onShowPlainTextClicked()), Qt::QueuedConnection);
    connect(&ok_button_, SIGNAL(clicked()), this, SLOT(onOKClicked()));
    connect(&clear_button_, SIGNAL(clicked()), &psk_edit_, SLOT(clear()));
    connect(&timer_, SIGNAL(timeout()), this, SLOT(onTimeout()));

    connect(&plain_button_, SIGNAL(clicked()), this, SLOT(onPlainButtonClicked()));
    connect(&wep_button_, SIGNAL(clicked()), this, SLOT(onWepButtonClicked()));
    connect(&wpa_psk_button_, SIGNAL(clicked()), this, SLOT(onWpaButtonClicked()));
    connect(&wpa2_psk_button_, SIGNAL(clicked()), this, SLOT(onWpa2ButtonClicked()));

    onShowPlainTextClicked();
}

void ApConfigDialog::updateWidgets(WifiProfile & profile)
{
    ssid_edit_.setEnabled(profile.ssid().isEmpty());
    ssid_edit_.setText(profile.ssid());

    // Authentication
    if (profile.isWep())
    {
        wep_button_.setChecked(true);
        psk_edit_.setText(profile.wepKey1());
        onWepButtonClicked();
    }
    else if (profile.isWpa())
    {
        wpa_psk_button_.setChecked(true);
        psk_edit_.setText(profile.psk());
        onWpaButtonClicked();
    }
    else if (profile.isWpa2())
    {
        wpa2_psk_button_.setChecked(true);
        psk_edit_.setText(profile.psk());
        onWpa2ButtonClicked();
    }
    else
    {
        plain_button_.setChecked(true);
        onPlainButtonClicked();
    }
}

void ApConfigDialog::onShowPlainTextClicked()
{
    if (!show_plain_text_.isChecked())
    {
        psk_edit_.setEchoMode(QLineEdit::Password);
    }
    else
    {
        psk_edit_.setEchoMode(QLineEdit::Normal);
    }
}

void ApConfigDialog::setPlainProfile(WifiProfile & profile,
                                     const QString & id)
{
    QByteArray d = profile.bssid();
    profile.clear();
    profile.setBssid(d);
    profile.defineByUser(true);
    profile.setSsid(id);
    profile.setKeyMgmt("NONE");
}

void ApConfigDialog::setWepProfile(WifiProfile & profile,
                                   const QString &id,
                                   const QString & key)
{
    QByteArray d = profile.bssid();
    profile.clear();
    profile.setBssid(d);
    profile.defineByUser(true);
    profile.setSsid(id);
    profile.setKeyMgmt("NONE");
    profile.setWep(true);
    profile.setWepKey1(key);
}

void ApConfigDialog::setWpaProfile(WifiProfile & profile,
                                   const QString &id,
                                   const QString & key)
{
    QByteArray d = profile.bssid();
    profile.clear();
    profile.setBssid(d);
    profile.defineByUser(true);
    profile.setSsid(id);
    profile.setKeyMgmt("WPA-PSK");
    profile.setProto("WPA");
    profile.setWpa(true);
    QString t;
    if (enc_ccmp_button_.isChecked())
    {
        t.append("CCMP");
    }
    if (enc_tkip_button_.isChecked())
    {
        if (!t.isEmpty())
        {
            t.append(" ");
        }
        t.append("TKIP");
    }
    profile.setPairwise(t);
    profile.setPsk(key);
}

void ApConfigDialog::setWpa2Profile(WifiProfile & profile,
                                    const QString &id,
                                    const QString & key)
{
    QByteArray d = profile.bssid();
    profile.clear();
    profile.setBssid(d);
    profile.defineByUser(true);
    profile.setSsid(id);
    profile.setKeyMgmt("WPA-PSK");
    profile.setProto("WPA2");
    profile.setWpa2(true);
    QString t;
    if (enc_ccmp_button_.isChecked())
    {
        t.append("CCMP");
    }
    if (enc_tkip_button_.isChecked())
    {
        if (!t.isEmpty())
        {
            t.append(" ");
        }
        t.append("TKIP");
    }
    profile.setPairwise(t);
    profile.setPsk(key);
}


void ApConfigDialog::onOKClicked()
{
    if (plain_button_.isChecked())
    {
        setPlainProfile(profile_, ssid_edit_.text());
    }
    else if (wep_button_.isChecked())
    {
        setWepProfile(profile_, ssid_edit_.text(), psk_edit_.text());
    }
    else if (wpa_psk_button_.isChecked())
    {
        setWpaProfile(profile_, ssid_edit_.text(), psk_edit_.text());
    }
    else if (wpa2_psk_button_.isChecked())
    {
        setWpa2Profile(profile_, ssid_edit_.text(), psk_edit_.text());
    }

    // Update profile.
    shadows_.show(false);
    done(QDialog::Accepted);
}

void ApConfigDialog::onCloseClicked()
{
    shadows_.show(false);
    done(QDialog::Rejected);
}

void ApConfigDialog::onTextChanged(const QString& text)
{
    ok_button_.setEnabled(!text.isEmpty());
    clear_button_.setEnabled(!text.isEmpty());
}

void ApConfigDialog::onPlainButtonClicked()
{
    enc_tkip_button_.setEnabled(false);
    enc_ccmp_button_.setEnabled(false);
    psk_edit_.setText("");
    psk_edit_.setEnabled(false);
}

void ApConfigDialog::onWepButtonClicked()
{
    enc_tkip_button_.setEnabled(false);
    enc_ccmp_button_.setEnabled(false);
    psk_edit_.setEnabled(true);
}

void ApConfigDialog::onWpaButtonClicked()
{
    enc_tkip_button_.setEnabled(true);
    enc_ccmp_button_.setEnabled(true);
    psk_edit_.setEnabled(true);
    if (profile_.pairwise().contains("TKIP", Qt::CaseInsensitive))
    {
        enc_tkip_button_.setChecked(true);
    }
    if (profile_.pairwise().contains("CCMP", Qt::CaseInsensitive))
    {
        enc_ccmp_button_.setChecked(true);
    }
}

void ApConfigDialog::onWpa2ButtonClicked()
{
    enc_tkip_button_.setEnabled(true);
    enc_ccmp_button_.setEnabled(true);
    psk_edit_.setEnabled(true);
    if (profile_.pairwise().contains("TKIP", Qt::CaseInsensitive))
    {
        enc_tkip_button_.setChecked(true);
    }
    if (profile_.pairwise().contains("CCMP", Qt::CaseInsensitive))
    {
        enc_ccmp_button_.setChecked(true);
    }
}

void ApConfigDialog::onSsidGetFocus(OnyxLineEdit *edit)
{
    keyboard_.attachReceiver(edit);
}

void ApConfigDialog::onPskGetFocus(OnyxLineEdit *edit)
{
    keyboard_.attachReceiver(edit);
}

/// This function is called by parent widget to display
/// Ap configuration dialog.
int ApConfigDialog::popup()
{
    updateWidgets(profile_);

    // By default, set the password edit as receiver.
    setReceiver(&psk_edit_);

    shadows_.show(true);
    if (isHidden())
    {
        show();
    }
    if (profile_.isWpa() || profile_.isWpa2())
    {
        psk_edit_.setText(profile_.psk());
    }
    else if (profile_.isWep())
    {
        psk_edit_.setText(profile_.wepKey1());
    }

    QWidget *p = parentWidget();
    if (p == 0)
    {
        p = QApplication::desktop();
    }
    QRect parent_rect = p->rect();
    int border = Shadows::PIXELS;
    int width = parent_rect.width() - border * 2;
    if (size().width() != width)
    {
        resize(width, height());
    }

    int x = Shadows::PIXELS;
    int y = p->height() - height() - 2 * Shadows::PIXELS;
    move(x, y);

    onyx::screen::instance().flush(0, onyx::screen::ScreenProxy::GC, false);
    return exec();
}

void ApConfigDialog::mouseMoveEvent(QMouseEvent *me)
{
    me->accept();
}

void ApConfigDialog::mousePressEvent(QMouseEvent *me)
{
    me->accept();
}

void ApConfigDialog::mouseReleaseEvent(QMouseEvent *me)
{
    me->accept();
}

void ApConfigDialog::keyReleaseEvent(QKeyEvent *ke)
{
    int key = ke->key();
    if (key == Qt::Key_Escape || key == ui::Device_Menu_Key)
    {
        onCloseClicked();
        return;
    }
}

/// The keyPressEvent could be sent from virtual keyboard.
void ApConfigDialog::keyPressEvent(QKeyEvent * ke)
{
    ke->accept();
    if (ke->key() == Qt::Key_Enter)
    {
        return;
    }
    else if (ke->key() == Qt::Key_Shift || ke->key() == Qt::Key_CapsLock)
    {
        return;
    }

    // Disable the parent widget to update screen.
    QKeyEvent * key_event = new QKeyEvent(ke->type(), ke->key(), ke->modifiers(), ke->text());
    QApplication::postEvent(receiver(), key_event);

    // Can not use flush here, could be caused by the keyboard event.
    onyx::screen::instance().enableUpdate(false);
    QApplication::processEvents();
    onyx::screen::instance().enableUpdate(true);
    onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW);
}

bool ApConfigDialog::event(QEvent * event)
{
    bool ret = OnyxDialog::event(event);
    if (event->type() == QEvent::UpdateRequest && onyx::screen::instance().isUpdateEnabled())
    {
        onyx::screen::instance().sync(&shadows_.hor_shadow());
        onyx::screen::instance().sync(&shadows_.ver_shadow());
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::GU);
    }
    return ret;
}

void ApConfigDialog::moveEvent(QMoveEvent *e)
{
    OnyxDialog::moveEvent(e);
}

void ApConfigDialog::resizeEvent(QResizeEvent *e)
{
    OnyxDialog::resizeEvent(e);
}

void ApConfigDialog::onTimeout()
{
    onyx::screen::instance().updateScreen(onyx::screen::ScreenProxy::GU);
}

}   // namespace ui

///
/// \example wifi/ap_conf_main.cpp
/// This is an example of how to use the ApConfigDialog class.
///
