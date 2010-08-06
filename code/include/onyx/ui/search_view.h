#ifndef SEARCH_VIEW_H
#define SEARCH_VIEW_H

#include "onyx/ui/ui.h"

using namespace ui;
namespace vbf
{

struct SearchContext
{
    QString pattern;
    bool    forward;
    bool    match_case;
    bool    match_whole_word;

    SearchContext() : forward(false),
                      match_case(false),
                      match_whole_word(false) {}
};

class SearchView : public QWidget
{
    Q_OBJECT
public:
    explicit SearchView(QWidget *parent = 0);
    ~SearchView(void);

    void displayNormal();
    QSize sizeHint () const;

Q_SIGNALS:
    void searchNext(const SearchContext & ctx);
    void closeClicked();

protected:
    void mouseMoveEvent(QMouseEvent *me);
    void mousePressEvent(QMouseEvent *me);
    void mouseReleaseEvent(QMouseEvent *me);
    void keyReleaseEvent(QKeyEvent *);
    void keyPressEvent(QKeyEvent * ke);

private Q_SLOTS:
    void onSearchNextClicked();
    void onSearchPrevClicked();
    void onCloseClicked();
    void onShowToolsClicked();
    void onTimeout();


private:
    // message handlers
    bool event(QEvent * event);

    void createLayout();
    void sendRequest(bool forward);
    void adjustPosition();

private:
    QVBoxLayout  vbox_;
    QHBoxLayout  hbox1_;
    //QHBoxLayout  hbox2_;
    QHBoxLayout  hbox3_;

    // TODO, change to use OnyxCheckBox and OnyxPushButton.
    KeyBoard          keyboard_;     ///< Keyboard.
    OnyxLineEdit     text_edit_;    ///< Input edit.
    OnyxPushButton   search_next_;  ///< Search next
    OnyxPushButton   search_prev_;  ///< Search previous

    OnyxPushButton show_tools_;   ///< Display the keyboard and options
    OnyxPushButton close_;        ///< Hide the search view

    //OnyxPushButton match_case_;        ///< Match case
    //OnyxPushButton match_whole_word_;  ///< Match whole word

    QTimer        timer_;        ///< Timer to update the screen.

};

};
#endif
