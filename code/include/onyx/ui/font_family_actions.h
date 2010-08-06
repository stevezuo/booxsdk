#ifndef FONT_FAMILY_ACTIONS_H_
#define FONT_FAMILY_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

class FontFamilyActions : public BaseActions
{
public:
    FontFamilyActions(void);
    ~FontFamilyActions(void);

public:
    void generateActions(const QString &selected_font,
                         bool scan_others);

    void reloadExternalFonts();

    /// Retrieve selected font family.
    const QString & selectedFont();
    static void loadExternalFonts(QVector<int> * ids = 0);

private:
    static void scan(QDir &dir, QVector<int> * ids = 0);

private:
    QString font_family_;
    QVector<int> fonts_;

};  // FontFamilyActions

}  // namespace ui

#endif  // FONT_FAMILY_ACTIONS_H_
