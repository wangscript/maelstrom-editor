#include "viewport.h"
#include "ui_viewport.h"
#include <QPainter>


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

int i = 0;
void Viewport::paintEvent(QPaintEvent *event)
{
    emit this->redraw();
}
