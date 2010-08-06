#ifndef MY_DICT_PLUGIN_H_
#define MY_DICT_PLUGIN_H_

#include <QtCore/QtCore>
#include "dictionary_plugin.h"

/// Implement Dictionary interface.
class MyDictionaryImpl : public Dictionary
{
public:
    MyDictionaryImpl(){}
    virtual ~MyDictionaryImpl(){}

public:
    virtual bool info(DictionaryInfo& info) { return true; }

    virtual bool load(const QString & working_directory) { return  true; }
    virtual bool isLoaded() { return false; }
    virtual bool close() { return true; }

    virtual bool translate(const QString &word, QString& result) { return true;}
    virtual bool similarWords(const QString &word, QStringList & result, const int offset, const int count) { return true; }
};


/// Implement plugin interface.
class DictionaryPluginImpl : public DictionaryPlugin
{
    Q_OBJECT
public:
    DictionaryPluginImpl(/*const QStringList &directories*/) : DictionaryPlugin(QStringList()){}
    virtual ~DictionaryPluginImpl(){}

public:

    /// This method returns all available dictionaries.
    /// The dictionary is not necessary loaded at this moment.
    virtual bool dictionaries(QStringList & list);

    /// Load the specified dictionary.
    virtual bool load(const QString & name);

    /// Load all dictionaries.
    virtual int loadAll();

    /// Get the dictionary specified by the name.
    virtual Dictionary * dictionary(const QString & name);


protected:
    MyDictionaryImpl m_dict;
};


#endif // MY_DICT_PLUGIN_H_

