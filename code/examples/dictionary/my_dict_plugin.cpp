

#include <QtCore/QtCore>
#include "my_dict_plugin.h"


bool DictionaryPluginImpl::dictionaries(QStringList & list)
{
    list.insert(list.end(), QString::fromUtf8("MyDictionary"));
    return true;
}

bool DictionaryPluginImpl::load(const QString & name)
{
    return true;
}

int DictionaryPluginImpl::loadAll()
{
    return 0;
}

Dictionary * DictionaryPluginImpl::dictionary(const QString & name)
{
    return &m_dict;
}


Q_EXPORT_PLUGIN2(MyPlugin, DictionaryPluginImpl)

