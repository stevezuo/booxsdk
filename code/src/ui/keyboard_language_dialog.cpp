#include "onyx/ui/keyboard_language_dialog.h"
#include "onyx/ui/languages.h"
#include "onyx/ui/text_layout.h"
#include "onyx/screen/screen_proxy.h"

namespace ui
{

/// Always use UTF-8.
struct LocaleItem
{
    const char * text;
    QLocale locale;
};

// Define all supported languages.
static const char* SCOPE = "LocalePage";
static const LocaleItem LANGUAGES[] =
{
    {QT_TRANSLATE_NOOP("LocalePage", "English"),  QLocale(QLocale::English, QLocale::UnitedStates)},
    {QT_TRANSLATE_NOOP("LocalePage", "Russian"),  QLocale(QLocale::Russian, QLocale::RussianFederation)},
    {QT_TRANSLATE_NOOP("LocalePage", "Poland"),  QLocale(QLocale::Polish, QLocale::Poland)},
    {QT_TRANSLATE_NOOP("LocalePage", "Swedish"),  QLocale(QLocale::Swedish, QLocale::Sweden)},
    {QT_TRANSLATE_NOOP("LocalePage", "Hungarian"),  QLocale(QLocale::Hungarian, QLocale::Hungary)},
};
static const int LANGUAGE_COUNT = sizeof(LANGUAGES) / sizeof(LANGUAGES[0]);

KeyboardLanguageDialog::KeyboardLanguageDialog(QWidget *parent, const QLocale & current_locale)
    : OnyxDialog(parent)
    , ver_layout_(&content_widget_)
    , current_locale_(current_locale)
{
    setModal(true);
    createLayout();
}

KeyboardLanguageDialog::~KeyboardLanguageDialog(void)
{
}

int KeyboardLanguageDialog::exec()
{
    updateTitle(QApplication::tr("Switch Language"));
    shadows_.show(true);
    show();
    onyx::screen::instance().flush();
    onyx::screen::instance().updateWidgetRegion(0, outbounding(parentWidget()), onyx::screen::ScreenProxy::GC, false, onyx::screen::ScreenCommand::WAIT_ALL);
    return QDialog::exec();
}

void KeyboardLanguageDialog::keyPressEvent(QKeyEvent *ke)
{
    ke->accept();
}

void KeyboardLanguageDialog::keyReleaseEvent(QKeyEvent *ke)
{
    QWidget * wnd = 0;
    // Check the current selected type.
    ke->accept();
    switch (ke->key())
    {
    case Qt::Key_Up:
    case Qt::Key_Left:
    case Qt::Key_PageDown:
    case Qt::Key_PageUp:
    case Qt::Key_Right:
    case Qt::Key_Down:
        wnd = ui::moveFocus(&content_widget_, ke->key());
        if (wnd)
        {
            wnd->setFocus();
        }
        break;
    case Qt::Key_Return:
        onReturn();
        break;
    case Qt::Key_Escape:
        reject();
        break;
    }
}

void KeyboardLanguageDialog::createLayout()
{
    // Title
    updateTitleIcon(QPixmap(":/images/small/locale.png"));
    content_widget_.setBackgroundRole(QPalette::Button);
    ver_layout_.setContentsMargins(SPACING, 0, SPACING, 0);

    OnyxCheckBox * btn = 0;
    int index = 0;
    int row = 0;
    for(; row < LANGUAGE_COUNT; ++row)
    {
        btn = new OnyxCheckBox(qApp->translate(SCOPE, LANGUAGES[index++].text), 0);
        language_buttons_.push_back(btn);
        connect(btn, SIGNAL(clicked(bool)),
                this, SLOT(onLanguageButtonClicked(bool)));
        ver_layout_.addWidget(btn);
    }

    // The current one.
    for(int i = 0; i < LANGUAGE_COUNT; ++i)
    {
        if (current_locale_.name() == LANGUAGES[i].locale.name())
        {
            language_buttons_[i]->setChecked(true);
            language_buttons_[i]->setFocus();
            break;
        }
    }
}

void KeyboardLanguageDialog::onReturn()
{
    size_t count = language_buttons_.size();
    for(size_t i = 0; i < count; ++i)
    {
        if (language_buttons_[i]->hasFocus())
        {
            language_buttons_[i]->setChecked(true);
            QApplication::processEvents();
            onLanguageButtonClicked(true);
            return;
        }
    }
}

void KeyboardLanguageDialog::onLanguageButtonClicked(bool)
{
    size_t count = language_buttons_.size();
    for(size_t i = 0; i < count; ++i)
    {
        if (language_buttons_[i]->isChecked())
        {
            current_locale_ = LANGUAGES[i].locale;
            accept();
            return;
        }
    }
}

void KeyboardLanguageDialog::onCloseClicked()
{
    reject();
}

bool KeyboardLanguageDialog::event(QEvent * event)
{
    bool ret = OnyxDialog::event(event);
    if (event->type() == QEvent::UpdateRequest && onyx::screen::instance().isUpdateEnabled())
    {
        // onyx::screen::instance().sync(&shadows_.hor_shadow());
        // onyx::screen::instance().sync(&shadows_.ver_shadow());
        onyx::screen::instance().updateWidget(this, onyx::screen::ScreenProxy::DW);
    }
    return ret;
}

void KeyboardLanguageDialog::moveEvent(QMoveEvent *e)
{
    OnyxDialog::moveEvent(e);
}

void KeyboardLanguageDialog::resizeEvent(QResizeEvent *e)
{
    OnyxDialog::resizeEvent(e);
}

}   // namespace ui
