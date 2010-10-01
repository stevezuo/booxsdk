#include "onyx/data/handwriting_manager.h"
#include "onyx/data/handwriting_functions_model.h"

namespace handwriting
{

HandwritingManager::HandwritingManager()
{
    loadPlugin();
    reload();
}

HandwritingManager::~HandwritingManager()
{
}

bool HandwritingManager::support()
{
    return true;
}

void HandwritingManager::collectPoint(int x, int y)
{
    if (plugin_impl_ != 0)
        plugin_impl_->collectPoint(x, y);
}

void HandwritingManager::clearPoints()
{
    if (plugin_impl_ != 0)
        plugin_impl_->clearPoints();
}

bool HandwritingManager::recognize(QStringList &candidates)
{
    candidates.clear();
#ifdef BUILD_FOR_ARM
    if (plugin_impl_ != 0)
        return plugin_impl_->recognize(candidates);
    return false;
#else
    QString first;
    first.push_back(QChar(0x82A6));
    first.push_back(QChar(0x82A6));
    first.push_back(QChar(0x82A6));
    first.push_back(QChar(0x82A6));
    first.push_back(QChar(0x82A6));
    first.push_back(QChar(0x82A6));
    first.push_back(QChar(0x82A6));
    candidates.push_back(first);

    QString second;
    second.push_back(QChar(0x82A7));
    second.push_back(QChar(0x82A7));
    second.push_back(QChar(0x82A7));
    second.push_back(QChar(0x82A7));
    second.push_back(QChar(0x82A7));
    candidates.push_back(second);

    QString third;
    third.push_back(QChar(0x82A8));
    third.push_back(QChar(0x82A8));
    third.push_back(QChar(0x82A8));
    candidates.push_back(third);

    candidates.push_back(QString(QChar(0x82A9)));
    candidates.push_back(QString(QChar(0x82AA)));
    candidates.push_back(QString(QChar(0x82AB)));
    candidates.push_back(QString(QChar(0x82AC)));
    candidates.push_back(QString(QChar(0x82AD)));
    candidates.push_back(QString(QChar(0x82AE)));
    candidates.push_back(QString(QChar(0x82AF)));
    return true;
#endif
}

bool HandwritingManager::getAssociatedChar(const QString & current_text, QStringList & result)
{
    if (plugin_impl_ != 0)
    {
        return plugin_impl_->getAssociatedChar(current_text, result);
    }
    return false;
}

bool HandwritingManager::adjustAssociatedChar(const QString & dst_text, int index)
{
    if (plugin_impl_ != 0)
    {
        return plugin_impl_->adjustAssociatedChar(dst_text, index);
    }
    return false;
}

bool HandwritingManager::reload()
{
    if (plugin_impl_ != 0)
    {
        return plugin_impl_->initialize(QLocale::system());
    }
    return false;
}

bool HandwritingManager::setLocale(const QLocale & locale)
{
    if (plugin_impl_ != 0)
    {
        return plugin_impl_->initialize(locale);
    }
    return false;
}

bool HandwritingManager::setSpecialRecognizeRange(SpecialRecognizeRange range)
{
    if (plugin_impl_ != 0)
    {
        return plugin_impl_->setSpecialRecognizeRange(range);
    }
    return false;
}

QLocale HandwritingManager::currentLocale()
{
    if (plugin_impl_ != 0)
    {
        return plugin_impl_->currentLocale();
    }
    return QLocale();
}

void HandwritingManager::loadPlugin()
{
    // Load all the other 3rd party plugins.
#ifdef BUILD_FOR_ARM
    QDir dir(SHARE_ROOT);
#else
    QDir dir(QDir::home());
#endif

    if (!dir.cd("handwriting/plugins"))
    {
        qDebug("No handwriting libs found");
        return;
    }

    QDir::Filters filters = QDir::Files|QDir::NoDotAndDotDot;
    QFileInfoList all = dir.entryInfoList(filters);
    foreach(QFileInfo info, all)
    {
        QString path = info.absoluteFilePath();
        QPluginLoader loader(path, 0);
        QObject *obj  = loader.instance();
        if (obj)
        {
            qDebug("Plugin Path:%s", qPrintable(path));
            HandwritingInterface * handwriting_interface = down_cast<HandwritingInterface*>(obj);
            if (handwriting_interface != 0)
            {
                plugin_impl_ = shared_ptr<HandwritingInterface>(handwriting_interface);
                break;
            }
        }
    }
}

void HandwritingManager::finishStroke()
{
    if (plugin_impl_ != 0)
        plugin_impl_->finishStroke();
}

void HandwritingManager::finishCharacter()
{
    if (plugin_impl_ != 0)
        plugin_impl_->finishCharacter();
}

bool HandwritingManager::setWorkArea(const QRect & rect)
{
    if (plugin_impl_ != 0)
        return plugin_impl_->setWorkArea(rect);
    return false;
}

bool HandwritingManager::setCandidateNum(const int number)
{
    if (plugin_impl_ != 0)
        return plugin_impl_->setCandidateNum(number);
    return false;
}

}   // namespace handwriting
