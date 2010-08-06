#ifndef BASE_MODEL_H_
#define BASE_MODEL_H_

#include "onyx/base/base.h"
#include <QtCore/QtCore>
#include "onyx/data/configuration.h"

namespace vbf
{

enum MetadataTag
{
    TAG_AUTHOR = 0,
    TAG_TITLE,
    TAG_PUBLISHER,
    TAG_DESCRIPTION
};


/// Base Model. The model may contain all stuff that used by the BaseView.
/// It also provides some pre-defined signals for view and other components.
/// Here, the BaseModel only provides limited methods. The associated view
/// is able to know the concrete model object, so it can cast to correct model
/// object during runtime. The limited methods are used by Model Manager.
class BaseModel : public QObject
{
    Q_OBJECT

public:
    BaseModel();
    virtual ~BaseModel();

public:
    /// Estimate whether the queried document is this one.
    virtual bool isTheDocument(const QString &path) = 0;

    /// Open a document by specified path. The path is in local 8bit.
    /// The path is constructed by caller and can be passed by
    /// command line or dbus message.
    virtual bool open(const QString &path) = 0;

    /// Retrieve the metadata.
    virtual bool metadata(const MetadataTag tag, QVariant &value) = 0;

    /// Close this document
    virtual bool close() = 0;

    /// Retrieve the configuration.
    virtual Configuration & getConf() = 0;


Q_SIGNALS:
    /// Document has been successfully opened.
    void docOpenedSignal(const QString & path);

    /// The signal is emitted when it can not open the document.
    void docOpenFailedSignal(const QString &path,
                             const QString &message);

    /// Document has been closed signal.
    void docClosedSignal(const QString &path);

private:
    NO_COPY_AND_ASSIGN(BaseModel);

};  // BaseModel

} // namespace vbf

#endif
