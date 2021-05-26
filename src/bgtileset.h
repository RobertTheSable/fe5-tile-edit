#ifndef QBGTILESET_H
#define QBGTILESET_H
#include <QDir>
#include <QString>
#include <QDataStream>
#include <QPixmap>
#include <QColor>
#include <vector>
#include "undostack.h"

class BGTileSet
{
private:
    std::stack<UndoStack::UndoEntry> undo_stack, redo_stack;
    struct ActiveBGTile{
        int num;
        uchar data [0x20];
    };
    int m_BGTileSize1, m_BGTileSize2, m_TileSetSize;
    unsigned short m_BG2TransferSize;
    //unsigned short activeTileData[4];
    //unsigned char activeBGTileData[4][0x20];
    //std::vector<ActiveBGTile> active_data;
    //uint active_tile_bg_numbers[4];
    uint active_tile, active_x, active_y;

    QColor tileHighlightColor, bgHighlightColor;

    std::vector<uchar> TileSet;
    std::vector<uchar> BGTileData;
    std::vector<uchar> BGTileData2;
    QColor palettes[16][16];
    unsigned int m_iTilesetAddress, m_iBGTilesAddress, m_iExtraTilesAddress, m_iPaletteAddress;

    //QFile m_RomFile;
    //int readAddressFromTable(int index, int data, int entrysize = 2);
    //int getChapterData()
    void generateDefaulPalettes();
    //int getActiveTileIndex(int tilenum);
public:
    enum TileQuadrant{UPPER_LEFT = 0, LOWER_LEFT = 1, UPPER_RIGHT = 2, LOWER_RIGHT = 3};

    BGTileSet(uchar* base_data, uchar* tileset_data, uchar* bg_tile_data, uchar* pallete_data, uchar* bg_tile_data_2 = nullptr);
    BGTileSet(uchar* tileset_data, int tilesesSize, uchar* bg_tile_data, int tileSize, uchar* pallete_data, uchar* bg_tile_data_2 = nullptr, int extraTileSize = 0);
//    BGTileSet(QString tileset, QString bgtiles1, QString palettes, QString bgtiles2 = "");

    QPixmap getTileSetPixmap(int scale, bool highlight = true);
    QPixmap getBGTilePixmap(int tilenum, bool vFlip, bool hFlip, int palette_number, int scale);
    QPixmap getBGTilesPixmap(bool vFlip, bool hFlip, int palette_number, int scale);
    QPixmap getTilePixmap(uint tilenum, int scale, bool highlight = true);
    QPixmap getActiveTilePixmap(int scale, bool whole = true);
    QPixmap getPalettePixmap(uint palette_num, int scale);

    QColor getPaletteColor(uint palette, uint color) const;
    QColor getTileHighlightColor() const;
    void setTileHighlightColor(const QColor &value);
    QColor getBgHighlightColor() const;
    void setBgHighlightColor(const QColor &value);


    unsigned int getTilesetAddress() const;
    unsigned int getBGTilesAddress() const;
    unsigned int getExtraTilesAddress() const;
    unsigned int getPaletteAddress() const;

    int getTileData(int index, int x, int y) const;
    int getActiveTileData() const;
    int getActive_x() const;
    void setActiveQuarter(int x, int y);
    int getActive_y() const;
    int getActive_tile() const;
    void setActive_tile(uint value);

    void setActiveTileData(uint value, bool vFlip, bool hFlip, uint palette);
    void setActiveTilePixel(int x, int y, int value);
    void copyBGTile(uint old_index, uint new_index);
    void copyTile(uint old_index, uint new_index);

    void undoLastEdit();
    void redoLastEdit();

    void exportData(int mode, uchar*& dest);
    static const int TILESET_MODE = 0;
    static const int TILE_MODE = 1;
    static const int EXTRA_TILE_MODE = 2;
    static const int PALETTE_MODE = 4;
    int getBGTileSize1() const;
    int getBGTileSize2() const;
    int getTileSetSize() const;
};

#endif // QBGTILESET_H
