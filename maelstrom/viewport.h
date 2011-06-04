#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QWidget>
//#include <QtGui\QPaintEngine>
#include <QPaintEngine>

namespace Ui {
    class Viewport;
}

class Viewport : public QWidget
{
    Q_OBJECT

public:
    explicit Viewport(QWidget *parent = 0);
    ~Viewport();

    void paintEvent(QPaintEvent *);
    bool event(QEvent *event);
    QPaintEngine *paintEngine() const;
signals:
    void redraw(void);

private:
    Ui::Viewport *ui;
};

#endif // VIEWPORT_H
