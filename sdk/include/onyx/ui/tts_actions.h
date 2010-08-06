#ifndef TTS_ACTIONS_H_
#define TTS_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

class TTSSpeakerActions : public BaseActions
{
public:
    TTSSpeakerActions(void);
    ~TTSSpeakerActions(void);

public:
    /// Generate or re-generate the setting actions group.
    void generateActions(QStringList & speakers, const QString & current);

    /// Set the status of speaker
    void setActionStatus(const QString & speaker, bool selected);

    /// Retrieve the selected speaker.
    QString selectedSpeaker();

};  // TTSSpeakerActions

class TTSSpeedActions : public BaseActions
{
public:
    TTSSpeedActions(void);
    ~TTSSpeedActions(void);

public:
    /// Generate or re-generate the setting actions group.
    void generateActions(QVector<int> & speeds, const int current);

    /// Set the status of speed
    void setActionStatus(const int speed, bool selected);

    /// Retrieve the selected speaker.
    int selectedSpeed();

};  // TTSSpeedActions

class TTSStyleActions : public BaseActions
{
public:
    TTSStyleActions(void);
    ~TTSStyleActions(void);

public:
    /// Generate or re-generate the setting actions group.
    void generateActions(QVector<int> & styles, const int current);

    /// Set the status of style
    void setActionStatus(const int style, bool selected);

    /// Retrieve the selected style.
    int selectedStyle();

};  // TTSStyleActions

}  // namespace ui

#endif  // TTS_ACTIONS_H_
