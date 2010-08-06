#ifndef ONYX_HANDWRITING_INTERFACE_H_
#define ONYX_HANDWRITING_INTERFACE_H_

#include <QtGui/QtGui>

namespace handwriting
{

enum SpecialRecognizeRange
{
    NUMBER = -1,
    INTERPUNCTION = -2
};

class HandwritingInterface : public QObject
{
    Q_OBJECT
public:
    HandwritingInterface(){}
    virtual ~HandwritingInterface(){}

public:
    virtual bool initialize(const QLocale & locale) = 0;
    virtual bool setWorkArea(const QRect & rect) = 0;
    virtual bool setCandidateNum(const int number) = 0;
    virtual bool setSpecialRecognizeRange(SpecialRecognizeRange range) = 0;
    virtual QLocale currentLocale() = 0;

    virtual void clearPoints() = 0;
    virtual void collectPoint(int x, int y) = 0;
    virtual void finishStroke() = 0;
    virtual void finishCharacter() = 0;
    virtual bool recognize(QStringList & results) = 0;

    virtual bool getAssociatedChar(const QString & current_text, QStringList & result) = 0;
    virtual bool adjustAssociatedChar(const QString & dst_text, int index) = 0;
};

}   // namespace handwriting

#endif  // ONYX_HANDWRITING_INTERFACE_H_
