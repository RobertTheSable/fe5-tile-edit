#include "tilesetlabel.h"
#include <QMouseEvent>

TileSetLabel::TileSetLabel(QWidget *parent, Qt::WindowFlags f)
: QLabel(parent,f), clicked_x(-1), clicked_y(-1), mouse_held(false) {
    unit_size = 16;
    scale = 1;
}

TileSetLabel::~TileSetLabel()
{

}

void TileSetLabel::setUnitSize(int usize)
{
    unit_size = usize;
}

int TileSetLabel::getScale() const
{
    return scale;
}

void TileSetLabel::setScale(int value)
{
    scale = value;
}

bool TileSetLabel::getMouse_held() const
{
    return mouse_held;
}

void TileSetLabel::mousePressEvent(QMouseEvent *event)
{
    if(this->isEnabled())
    {
        int x = event->pos().x()/(unit_size*scale);
        int y = event->pos().y()/(unit_size*scale);
        clicked_x = x;
        clicked_y = y;
        emit clicked(x, y);
    }
    mouse_held = true;
    //mousePressEvent(event);
}

void TileSetLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(this->isEnabled())
    {
        int x = event->pos().x()/(unit_size*scale);
        int y = event->pos().y()/(unit_size*scale);
        emit double_clicked(x, y);
    }
    mouse_held = true;
    //mouseDoubleClickEvent(event);
}

void TileSetLabel::mouseReleaseEvent(QMouseEvent *event)
{

    if(mouse_held && clicked_x != -1 && clicked_y != -1)
    {
        int x = event->pos().x()/(unit_size*scale);
        int y = event->pos().y()/(unit_size*scale);
        if(x != clicked_x || y != clicked_y)
        {
            emit released(clicked_x, clicked_y, x,y);
        }
    }
    clicked_x = -1;
    clicked_y = -1;
    mouse_held = false;
    setCursor(Qt::ArrowCursor);
    //mouseReleaseEvent(event);
}

void TileSetLabel::mouseMoveEvent(QMouseEvent *ev)
{
    if(mouse_held)
    {
        setCursor(Qt::DragMoveCursor);
    }
}

void TileSetLabel::leaveEvent(QEvent *event)
{
    mouse_held = false;
    clicked_x = -1;
    clicked_y = -1;
    //leaveEvent(event);
}
