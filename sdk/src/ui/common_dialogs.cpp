
#include "onyx/ui/common_dialogs.h"

namespace ui
{

LowBatteryDialog::LowBatteryDialog(QWidget *parent)
: MessageDialog(QMessageBox::Warning,
                tr("Warning"),
                tr("Low battery please charge."),
                QMessageBox::Yes,
                parent)
{
}

LowBatteryDialog::~LowBatteryDialog()
{
}


USBConnectionDialog::USBConnectionDialog(QWidget *parent)
: MessageDialog(QMessageBox::Information,
                tr("USB Connection"),
                tr(""),
                QMessageBox::Yes|QMessageBox::No)
{
    MessageDialog::updateInformation(tr("USB connection detected. Do you want to connect to PC?"));
}

USBConnectionDialog::~USBConnectionDialog(void)
{
}

DeleteFileDialog::DeleteFileDialog(const QString &path, QWidget *parent)
: MessageDialog(QMessageBox::Warning,
                tr("Delete"),
                tr(""),
                QMessageBox::Yes|QMessageBox::No)
{
    QString str(tr("Do you want to remove %1?"));
    QFileInfo info(path);
    str = str.arg(info.fileName());
    MessageDialog::updateInformation(str);
    button(QMessageBox::No)->setFocus();
}

DeleteFileDialog::~DeleteFileDialog(void)
{
}

FormatFlashDialog::FormatFlashDialog(QWidget *parent)
: MessageDialog(QMessageBox::Warning,
                tr("Warning"),
                tr("Do you want to format internal flash, it will erase all data on flash?"),
                QMessageBox::Yes|QMessageBox::No)
{
}

FormatFlashDialog::~FormatFlashDialog (void)
{

}

FormatSDDialog::FormatSDDialog(QWidget *parent)
: MessageDialog(QMessageBox::Warning,
                tr("Warning"),
                tr("Do you want to format SD card, it will erase all data on SD card?"),
                QMessageBox::Yes|QMessageBox::No)
{
}

FormatSDDialog::~FormatSDDialog (void)
{
}

ErrorDialog::ErrorDialog(const QString & err, QWidget *parent)
: MessageDialog(QMessageBox::Warning,
                tr("Error"),
                err,
                QMessageBox::Yes,
                parent)
{
}

ErrorDialog::~ErrorDialog(void)
{
}

}
