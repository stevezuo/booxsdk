#ifndef ONYX_HANDWRITING_MANAGER_H_
#define ONYX_HANDWRITING_MANAGER_H_

#include "onyx/base/base.h"
#include "onyx/ui/ui.h"
#include "handwriting_interface.h"

namespace handwriting
{

/// Handwriting manager.
class HandwritingManager : public QObject
{
    Q_OBJECT

    HandwritingManager();
    HandwritingManager(const HandwritingManager & right);
public:
    static HandwritingManager & instance()
    {
        static HandwritingManager instance_;
        return instance_;
    }
    ~HandwritingManager();

public:
    bool support();
    bool reload();
    bool setLocale(const QLocale & locale);
    bool setSpecialRecognizeRange(SpecialRecognizeRange range);
    QLocale currentLocale();

public Q_SLOTS:
    bool setWorkArea(const QRect & rect);
    bool setCandidateNum(const int number);

    void clearPoints();
    void collectPoint(int x, int y);
    void finishStroke();
    void finishCharacter();
    bool recognize(QStringList & text);

    bool getAssociatedChar(const QString & current_text, QStringList & result);
    bool adjustAssociatedChar(const QString & dst_text, int index);

private:
    void loadPlugin();

private:
    shared_ptr<HandwritingInterface> plugin_impl_; ///< Backend instance.
};

}   // namespace handwriting

#endif  // ONYX_HANDWRITING_MANAGER_H_
