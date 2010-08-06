
#include "onyx/ui/languages.h"

namespace ui
{

bool loadTranslator(const QString &locale)
{
    // Not sure it will cause memory leak.
    static QTranslator *pre_translator;
    QTranslator *translator = new QTranslator;
    QString file = "onyx_" + locale + ".qm";

    // Double check.
    QDir dir(QDir::home());
    QString path = SHARE_ROOT;
    if (!path.isEmpty())
    {
        dir.cd(path);
    }

    if (!dir.cd("translations"))
    {
        return false;
    }

    qDebug("Locale file %s", qPrintable(dir.absoluteFilePath(file)));
    translator->load(dir.absoluteFilePath(file));

    if (pre_translator)
    {
        QApplication::removeTranslator(pre_translator);
    }
    QApplication::installTranslator(translator);
    pre_translator = translator;
    return true;
}

}

