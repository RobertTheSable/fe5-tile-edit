#ifndef TILESETLABEL_H
#define TILESETLABEL_H
#include <QLabel>
#include <QWidget>
#include <Qt>

class TileSetLabel : public QLabel {
    Q_OBJECT
public:
    explicit TileSetLabel(QWidget* parent = Q_NULLPTR, Qt::WindowFlags f = Qt::WindowFlags());
    ~TileSetLabel();
    void setUnitSize(int usize);
    int getScale() const;
    void setScale(int value);

    bool getMouse_held() const;

private:
    int unit_size;
    int scale;
    int clicked_x, clicked_y;
    bool mouse_held;
signals:
    void clicked(int x, int y);
    void double_clicked(int x, int y);
    void released(int old_x, int old_y, int new_x, int new_y);

protected:
    void mousePressEvent(QMouseEvent* event);
    void mouseDoubleClickEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent *ev);
    void leaveEvent(QEvent *event);

};
#endif // TILESETLABEL_H
