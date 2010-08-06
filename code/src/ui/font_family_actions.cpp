#include "onyx/ui/font_family_actions.h"
#include "onyx/base/device.h"

namespace ui
{

FontFamilyActions::FontFamilyActions(void)
    : BaseActions()
    , font_family_()
{
    category()->setIcon(QIcon(QPixmap(":/images/font_family.png")));
}

FontFamilyActions::~FontFamilyActions(void)
{
}

/// Generate all supported font family here.
void FontFamilyActions::generateActions(const QString &font_family,
                                        bool scan_others)
{
    category()->setText(QCoreApplication::tr("Font Family"));
    actions_.clear();

    QFontDatabase fdb;
    if (scan_others)
    {
        // Scan both internal flash and sd card.
        loadExternalFonts(&fonts_);
    }

    QStringList list;
    list =  fdb.families();

    foreach (QString family, list)
    {
        // Add to category automatically.
        shared_ptr<QAction> act(new QAction(family, exclusiveGroup()));

        // Change font and set it as checkable.
        act->setFont(QFont(family));
        act->setCheckable(true);
        act->setData(family);
        act->setIcon(QIcon(QPixmap(":/images/font_family_item.png")));

        if (family == font_family)
        {
            act->setChecked(true);
            font_family_ = font_family;
        }

        actions_.push_back(act);
    }
}

/// List all fonts file and add them into application font database.
void FontFamilyActions::scan(QDir &dir, QVector<int> * ids)
{
    QFileInfoList list = dir.entryInfoList(QDir::Files);
    foreach(QFileInfo info, list)
    {
        // Make more strict check.
        QString name = info.fileName();
        if (name.endsWith(".ttf", Qt::CaseInsensitive) ||
            name.endsWith(".ttc", Qt::CaseInsensitive) ||
            name.endsWith(".otf", Qt::CaseInsensitive))
        {
            if (ids)
            {
                ids->push_back(QFontDatabase::addApplicationFont(info.absoluteFilePath()));
            }
        }
    }
}

void FontFamilyActions::reloadExternalFonts()
{
    QFontDatabase::removeAllApplicationFonts();
    loadExternalFonts(&fonts_);
}

void FontFamilyActions::loadExternalFonts(QVector<int> * ids)
{
    // Scan both internal flash and sd card.
    const QString FONT_DIR_NAME = "fonts";
    QDir dir(LIBRARY_ROOT);
    if (dir.cd(FONT_DIR_NAME))
    {
        scan(dir, ids);
    }

    if (dir.cd(SDMMC_ROOT) && dir.cd(FONT_DIR_NAME))
    {
        scan(dir, ids);
    }
}

const QString & FontFamilyActions::selectedFont()
{
    // Search for the changed actions.
    QAction * act = exclusiveGroup()->checkedAction();
    font_family_.clear();
    if (act)
    {
        font_family_ = act->data().toString();
        return font_family_;
    }
    return font_family_;
}

}   // namespace ui
