#include "viewport.h"
#include "ui_viewport.h"
#include <QPainter>
#include <QDebug>

Viewport::Viewport(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Viewport)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_PaintOnScreen);

}

Viewport::~Viewport()
{
    delete ui;
}

QPaintEngine *Viewport::paintEngine() const
{
        return 0;
}

bool Viewport::event(QEvent *event)
{
    return QWidget::event(event);
}

int i = 0;
void Viewport::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(Qt::blue);
    painter.setFont(QFont("Arial", 30));
    painter.drawText(rect(), Qt::AlignCenter, "Qt");
    emit this->redraw();
}
