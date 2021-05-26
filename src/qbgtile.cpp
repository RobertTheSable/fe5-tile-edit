#include "qbgtile.h"
#include "mainwindow.h"
#include <QPainter>
#include <QGraphicsScene>

QBGTile::QBGTile(bool vertical_flip, bool horizontal_flip, bool priority, int tilenum, QColor palette[16], int colors[64], int scale)
{
      for(int i = 0; i < 64; i++)
      {
          int index_x = i%8;
          int index_y = i/8;
          m_colors[index_y][index_x] = colors[i];
      }
      m_vFlip = vertical_flip;
      m_hFlip = horizontal_flip;
      m_priority = priority;
      m_Scale = scale;
      m_TileNumber = tilenum;
      for(int i = 0; i < 16; i++)
      {
          QColor test(palette[i]);
          m_palette[i] = test;
      }
}

QRectF QBGTile::boundingRect() const
{
    return QRectF(0,0,8*m_Scale,8*m_Scale);
}

void QBGTile::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QImage img(8,8,QImage::Format_RGB888);

    for(int y = 0; y < 8; y++)
    {
        int y_off = y;
        if(m_vFlip)
        {
            y_off = 7-y;
        }
        for(int x = 0; x < 8; x++)
        {
            int x_off = x;
            if(m_hFlip)
            {
                x_off = 7-x;
            }
            img.setPixelColor(x_off,y_off,m_palette[m_colors[y][x]]);
        }
    }
    img.scaled(8*m_Scale, 8*m_Scale);
    painter->drawImage(boundingRect(),img);
}

int QBGTile::setPixel(int x, int y, int color_index)
{
    int ret_val = 0;
    if(x > 7 || x < 0 || y > 7 || y < 0)
    {
        ret_val = 1;
    }
    else
    {
        m_colors[y][x] = color_index;
        update();
    }

    return ret_val;
}

void QBGTile::setPallete(QColor palette[])
{
    for(int i = 0; i < 16; i++)
    {
        QColor test(palette[i]);
        m_palette[i] = test;
    }
}
void QBGTile::setVFlip(bool vFlip)
{
    m_vFlip = vFlip;
}

void QBGTile::setHFlip(bool hFlip)
{
    m_hFlip = hFlip;
}

void QBGTile::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}
