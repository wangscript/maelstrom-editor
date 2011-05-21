#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QWidget>
#include <QtGui\QPaintEngine>

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
    QPaintEngine *paintEngine() const;
signals:
    void redraw(void);

private:
    Ui::Viewport *ui;
};

#endif // VIEWPORT_H
