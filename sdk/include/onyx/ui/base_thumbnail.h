// -*- mode: c++; c-basic-offset: 4; -*-

#ifndef ONYX_UI_BASE_THUMBNAIL_H_
#define ONYX_UI_BASE_THUMBNAIL_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui_global.h"

using namespace ui;
using namespace base;

namespace onyx {

namespace ui {

///
/// Base class to represent a thumbnail.
///
class BaseThumbnail
{
public:
    BaseThumbnail();
    virtual ~BaseThumbnail();

    virtual const QRect& displayArea() const = 0;
    virtual const QImage* image() const = 0;
    virtual int key() const = 0;
    virtual const QString& path() const = 0;
    virtual const QString& name() const = 0;

    virtual void clearPage() = 0;
    virtual int length() = 0;
    virtual void setOriginSize(const QSize & size) = 0;
    virtual ZoomFactor zoom() = 0;

    void unlock() { locked_ = false; }
    void lock() { locked_ = true; }
    bool locked() const { return locked_; }

private:
    bool locked_;
    NO_COPY_AND_ASSIGN(BaseThumbnail);
};

typedef shared_ptr<BaseThumbnail> ThumbPtr;

}  // namespace ui
}  // namespace onyx

#endif  // ONYX_UI_BASE_THUMBNAIL_H_
