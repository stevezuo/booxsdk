#ifndef ONYX_COMMON_DIALOG_H_
#define ONYX_COMMON_DIALOG_H_

#include "message_dialog.h"

namespace ui
{

/// Low battery warning dialog.
class LowBatteryDialog : public MessageDialog
{
    Q_OBJECT

public:
    explicit LowBatteryDialog(QWidget *parent = 0);
    ~LowBatteryDialog(void);
};


/// USB connection dialog.
class USBConnectionDialog : public MessageDialog
{
    Q_OBJECT

public:
    USBConnectionDialog(QWidget *parent);
    ~USBConnectionDialog(void);

};

/// Delete file dialog.
class DeleteFileDialog : public MessageDialog
{
    Q_OBJECT

public:
    DeleteFileDialog(const QString &path, QWidget *parent = 0);
    ~DeleteFileDialog (void);
};

/// Format flash dialog.
class FormatFlashDialog : public MessageDialog
{
    Q_OBJECT

public:
    FormatFlashDialog(QWidget *parent = 0);
    ~FormatFlashDialog (void);
};

class FormatSDDialog : public MessageDialog
{
    Q_OBJECT

public:
    FormatSDDialog(QWidget *parent = 0);
    ~FormatSDDialog(void);
};

/// Error dialog
class ErrorDialog : public MessageDialog
{
    Q_OBJECT

public:
    ErrorDialog(const QString & err, QWidget *parent = 0);
    ~ErrorDialog(void);
};

};  // namespace ui


#endif  // ONYX_COMMON_DIALOG_H_
