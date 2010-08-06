#ifndef ONYX_SEARCH_WIDGET_H_
#define ONYX_SEARCH_WIDGET_H_

#include "buttons.h"
#include "line_edit.h"
#include "onyx_dialog.h"
#include "keyboard.h"

/// Define search context. Application may inherit this class
/// to implement its own search context.
class BaseSearchContext
{
public:
    BaseSearchContext();
    virtual ~BaseSearchContext();

public:
    void reset();

    const QString & pattern() const { return pattern_; }
    void setPattern(const QString &pattern);

    bool forward() const { return forward_; }
    void setForward(bool forward = true);

    bool case_sensitive() const { return case_sensitive_; }
    void setCaseSensitive(bool sensitive = true);

    bool match_whole_word() const { return match_whole_word_; }
    void setMatchWholeWord(bool match = true);

    bool isStopped() { return stop_; }
    void stop(bool s = true) { stop_ = s; }

    int userData() const { return user_data_; }
    int & userData() { return user_data_; }

private:
    QString pattern_;
    bool forward_;
    bool case_sensitive_;
    bool match_whole_word_;
    bool stop_;
    int user_data_;
};

namespace ui
{

/// Search widget to enable user to change search criteria.
class SearchWidget : public OnyxDialog
{
    Q_OBJECT

public:
    SearchWidget(QWidget *parent, BaseSearchContext & ctx);
    ~SearchWidget(void);

public:
    inline bool isHighlightAll();

    void ensureVisible();
    void adjustPosition();
    QSize sizeHint() const;
    QSize minimumSize () const;
    QSize maximumSize () const;

public Q_SLOTS:
    void noMoreMatches();

Q_SIGNALS:
    /// The search signal is emitted when user clicked search button,
    /// search next button and search previous button.
    void search(BaseSearchContext & ctx);

    /// The closeClicked signal is emitted when user close the search
    /// widget. The caller decides to destroy the widget or just hide
    /// the widget.
    void closeClicked();

    /// Highlight all of the results or not
    /// void isHighlightAllChanged();

protected:
    void mouseMoveEvent(QMouseEvent *me);
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyReleaseEvent(QKeyEvent *);
    void keyPressEvent(QKeyEvent * ke);
    bool event(QEvent * event);
    void moveEvent(QMoveEvent *e);
    void resizeEvent(QResizeEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);

private Q_SLOTS:
    void onSearchClicked();
    void onSearchNextClicked();
    void onSearchPrevClicked();
    void onHighlightAllClicked();
    void onCloseClicked();
    void onTextChanged(const QString&);
    void onTimeout();

private:
    void createLayout();
    void updateChildrenWidgets(bool searching);
    void readyToSearch(bool forward);
    void updateTitle(const QString &message = QString());

private:
    QHBoxLayout  hbox_;

    OnyxLineEdit text_edit_;    ///< Input edit.
    OnyxPushButton   search_button_;   ///< Start to search.
    OnyxPushButton   clear_button_;   ///< Clear the text.
    OnyxPushButton   search_next_;  ///< Search next
    OnyxPushButton   search_prev_;  ///< Search previous
    OnyxPushButton   highlight_all_; ///< Highlight all of the search results

    KeyBoard      keyboard_;     ///< Keyboard.
    QTimer        timer_;        ///< Timer to update the screen.

    BaseSearchContext & ctx_;
    bool update_parent_;
    bool full_mode_;
    bool is_highlight_all_;
};

// bool SearchWidget::isHighlightAll()
// {
//     return is_highlight_all_;
// }

};  // namespace ui

#endif  // ONYX_SEARCH_WIDGET_H_
