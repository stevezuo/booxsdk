#ifndef TAB_BUTTON_H_
#define TAB_BUTTON_H_

#include "onyx/ui/ui_global.h"

class TabButton : public QWidget
{
    Q_OBJECT

public:
    TabButton(QWidget *parent, const int id);
    ~TabButton(void);

public:
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    const int id() const { return button_id_; }

    void setText(const QString &title);
    void setIcon(const QIcon & icon);

    void setChecked(bool check = true);
    bool isChecked() const;

    const QIcon & icon() const { return icon_; }

public Q_SLOTS:
    void click();

Q_SIGNALS:
    void clicked(const int id, bool checked = false);

protected:
    virtual bool event(QEvent *e);
    virtual void paintEvent(QPaintEvent *e);
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);

private:
    void updateLayout() const;

    QSize iconActualSize() const;

private:
    int button_id_;
    mutable QPoint icon_pos_;
    mutable QTextLayout title_layout_;
    mutable QIcon icon_;
    bool checked_;
    mutable int layout_width_;
    mutable int layout_height_;
    mutable bool is_dirty_;
};

inline bool TabButton::isChecked() const
{
    return checked_;
}

#endif
