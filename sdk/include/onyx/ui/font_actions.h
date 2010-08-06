#ifndef FONT_ACTIONS_H_
#define FONT_ACTIONS_H_

#include "onyx/base/base.h"
#include "context_dialog_base.h"

namespace ui
{

/// The FontActions is used to specified the font size directly.
class FontActions : public BaseActions
{
public:
    FontActions(void);
    ~FontActions(void);

public:
    /// Generate or re-generate the size actions group.
    /// The size should be in point size.
    void generateActions(QFont font,
                         vector<int> & size,
                         const int selected_size);

    /// Retrieve the selected font style.
    QFont selectedFont();

private:
    void generateDefaultFontSize(vector<int> & size);

private:
    QFont font_;
    scoped_ptr<QActionGroup> size_group_;
    scoped_ptr<QActionGroup> style_group_;
};  // FontActions


/// TextSizeActions is used to change font size according to current size.
class TextSizeActions : public BaseActions
{
public:
    TextSizeActions(void);
    ~TextSizeActions(void);

public:
    /// Generate or re-generate the size actions group.
    /// The size should be in point size.
    void generateActions(vector<qreal> & size,
                         const qreal selected_multiplier);

    /// Retrieve the selected multiplier.
    qreal selectedMultiplier();

private:
    void generateDefaultActions(vector<qreal> & size);

private:
    qreal multiplier_;      ///< The selected multiplier.
};

/// FontIndexActions is used to update the font size by indexes
class FontIndexActions : public BaseActions
{
public:
    FontIndexActions(void);
    ~FontIndexActions(void);

public:
    void generateActions( vector<int> & indexes,
                          const int current );
    void setCurrentIndex( const int current );
    int selectedIndex();

private:
    void generateDefaultActions( vector<int> & indexes );

private:
    int index_;      ///< The selected index.
};

}  // namespace ui

#endif  // FONT_ACTIONS_H_
