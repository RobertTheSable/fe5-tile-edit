#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "bgtileset.h"
#include "exportbindialog.h"
#include "opendirdialog.h"
#include "addresser/addresser.h"
#include <QMainWindow>
#include <QObject>
#include <vector>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void show();
private slots:
    void on_actionOpen_triggered();
    void setTileFocus(int x, int y);
    void selectTileFromBank(int x, int y);
    void setBGTileFromQuadrant(int x, int y);
    void editActiveTile(int x, int y);
    void selectColor(int x, int y);
    void dragBGTile(int old_x, int old_y, int new_x, int new_y);
    void dragMainTile(int old_x, int old_y, int new_x, int new_y);

    //void setMainCursor();
    void on_chapterSetButton_clicked();
    void on_chapterSpinBox_editingFinished();
    void on_actionExport_triggered();
    void on_zoomBGTileSpinBox_editingFinished();
    void on_actionHighlight_Same_BG_Tile_toggled(bool arg1);
    void on_tileVFlipCheckBox_toggled(bool checked);
    void on_tileHFlipCheckBox_toggled(bool checked);
    void on_tilePalletteSpinBox_valueChanged(int arg1);

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionOpen_From_Files_triggered();

    void on_actionExport_PNG_triggered();

private:
    Ui::MainWindow *ui;
    std::vector<BGTileSet> m_Tilesets;
    RomMap m_RomMap;
    int current_chapter;
    //int active_tile_x, active_tile_y;
    void updateChapterData();
    void updateTileDisplay();
    void updateBGTileDisplay(bool reload = true);
    void updateTilesetDisplay();
    void updateBGTilesetDisplay();
    void updatePaletteDisplay();
    QColor highlightColor;
    int current_palette_color;
    bool switching;
//    QGraphicsScene *tileMapScene;
//    QBGTile *tile;
};

#endif // MAINWINDOW_H
