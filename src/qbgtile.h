#ifndef QBGTILE_H
#define QBGTILE_H

#include <QGraphicsItem>

class QBGTile : public QGraphicsItem
{

public:
    QBGTile(bool vertical_flip, bool horizontal_flip, bool priority, int tilenum, QColor palette[16], int colors[64], int scale = 1);

    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,QWidget *widget);
    int setPixel(int x, int y, int color_index);
    void setPallete(QColor palette[16]);
    void setPixelIndex(int x, int y, int color_index);
    void setVFlip(bool vFlip);
    void setHFlip(bool hFlip);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
private:
    bool m_vFlip, m_hFlip, m_priority;
    QColor m_palette[16];
    int m_colors[8][8];
    int m_TileNumber;
    int m_Scale;
};

#endif // QBGTILE_H
