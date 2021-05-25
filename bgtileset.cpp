#include "bgtileset.h"
#include "fecompress.h"
#include "addresser/addresser.h"
#include <QPainter>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iostream>
#include <algorithm>

BGTileSet::BGTileSet(uchar* base_data, uchar *tileset_data, uchar *bg_tile_data, uchar *pallete_data, uchar *bg_tile_data_2) :
    active_tile(0), active_x(0), active_y(0), tileHighlightColor(QColor(0,0,255,100)), bgHighlightColor(QColor(0,0,0,0)),
    m_iTilesetAddress(0), m_iBGTilesAddress(0), m_iExtraTilesAddress(0), m_iPaletteAddress(0)
{
    uchar *output;
    int BGTileSize = FECompress::DecompressData(output, bg_tile_data);
    for(int i = 0; i < BGTileSize; i++)
    {
        BGTileData.push_back(output[i]);
    }
    delete[] output;
    m_BGTileSize1 = BGTileSize;
    m_BGTileSize2 = 0;
    if(bg_tile_data_2 != nullptr) {
        int BGTileSize2 = FECompress::DecompressData(output, bg_tile_data_2);
        m_BG2TransferSize = *((unsigned short*)output);
        for(int i = 2; i < BGTileSize2; i++) {
            BGTileData2.push_back(output[i]);
        }
        delete[] output;
        m_BGTileSize2 = BGTileSize2;
    }
    else {
        m_iExtraTilesAddress = 0;
    }
    int tilesetSize = FECompress::DecompressData(output, tileset_data);
    for(int i = 0; i < tilesetSize; i++) {
        TileSet.push_back(output[i]);
    }
    m_TileSetSize = tilesetSize;
    delete[] output;
    generateDefaulPalettes();
    for(int pal_num = 3; pal_num < 8; pal_num++) {
        int pal_index = pal_num*16;
        pal_index = (pal_num-3)*32;
        for(int color_num = 0; color_num < 32; color_num+=2) {
            unsigned short color16 = (*(unsigned short*)&(pallete_data[pal_index + color_num]));
            int blue = (color16&0x7C00) >> 10;
            int green =(color16&0x3E0) >> 5;
            int red = (color16&0x1F);
            palettes[pal_num][color_num/2] = QColor(red<<3, green<<3, blue<<3);
        }
    }
    m_iTilesetAddress = PCToLoROM(tileset_data - base_data, false);
    m_iBGTilesAddress = PCToLoROM(bg_tile_data - base_data, false);
    m_iPaletteAddress = PCToLoROM(pallete_data - base_data, false);
    if (bg_tile_data_2 != nullptr) {
        m_iExtraTilesAddress = PCToLoROM(bg_tile_data_2 - base_data, false);
    }

}

BGTileSet::BGTileSet(uchar *tileset_data, int tilesetSize, uchar *bg_tile_data, int tileSize, uchar *pallete_data, uchar *bg_tile_data_2, int extraTileSize) :
    active_tile(0), active_x(0), active_y(0), tileHighlightColor(QColor(0,0,255,100)), bgHighlightColor(QColor(0,0,0,0)),
    m_iTilesetAddress(0), m_iBGTilesAddress(0), m_iExtraTilesAddress(0), m_iPaletteAddress(0)
{
    for(int i = 0; i < tilesetSize; i++) {
        TileSet.push_back(tileset_data[i]);
    }
    m_TileSetSize = tilesetSize;
    for(int i = 0; i < tileSize; i++)
    {
        BGTileData.push_back(bg_tile_data[i]);
    }
    m_BGTileSize1 = tileSize;
    if (bg_tile_data_2 != nullptr) {
        m_BG2TransferSize = *((unsigned short*)bg_tile_data_2);
        for(int i = 2; i < extraTileSize; i++) {
            BGTileData2.push_back(bg_tile_data_2[i]);
        }
        m_BGTileSize2 = extraTileSize;
    }
    generateDefaulPalettes();
    for(int pal_num = 3; pal_num < 8; pal_num++) {
        int pal_index = pal_num*16;
        pal_index = (pal_num-3)*32;
        for(int color_num = 0; color_num < 32; color_num+=2) {
            unsigned short color16 = (*(unsigned short*)&(pallete_data[pal_index + color_num]));
            int blue = (color16&0x7C00) >> 10;
            int green =(color16&0x3E0) >> 5;
            int red = (color16&0x1F);
            palettes[pal_num][color_num/2] = QColor(red<<3, green<<3, blue<<3);
        }
    }
}

unsigned int BGTileSet::getTilesetAddress() const
{
    return m_iTilesetAddress;
}

unsigned int BGTileSet::getBGTilesAddress() const
{
    return m_iBGTilesAddress;
}

unsigned int BGTileSet::getExtraTilesAddress() const
{
    return m_iExtraTilesAddress;
}

unsigned int BGTileSet::getPaletteAddress() const
{
    return m_iPaletteAddress;
}

QColor BGTileSet::getTileHighlightColor() const
{
    return tileHighlightColor;
}

void BGTileSet::setTileHighlightColor(const QColor &value)
{
    tileHighlightColor = value;
}

QColor BGTileSet::getBgHighlightColor() const
{
    return bgHighlightColor;
}

void BGTileSet::setBgHighlightColor(const QColor &value)
{
    bgHighlightColor = value;
}

int BGTileSet::getBGTileSize1() const
{
    return m_BGTileSize1;
}

int BGTileSet::getBGTileSize2() const
{
    return m_BGTileSize2;
}

int BGTileSet::getTileSetSize() const
{
    return m_TileSetSize;
}

void BGTileSet::generateDefaulPalettes()
{
    const char* defaultPalette = "resource/palettes.bin";
    std::ifstream inFile(defaultPalette, std::ios::in|std::ios::binary);
    for(int j = 0; j<16; j++)
    {
        for(int i = 0; i< 16; i++)
        {
            unsigned short color;
            inFile.read((char *)&color,sizeof(short));
            int blue = (color&0x7C00) >> 10;
            int green =(color&0x3E0) >> 5;
            int red = (color&0x1F);
            palettes[j][i] = QColor(red<<3, green<<3, blue<<3);
            //cout << hex << color[i] << endl;
        }
    }
    inFile.close();
}

QPixmap BGTileSet::getTileSetPixmap(int scale, bool highlight)
{
    QPixmap pix(512*scale,512*scale);
    QPainter p(&pix);
    for(uint i = 0; i < 0x400; i++)
    {
        int x = i%32;
        int y = i/32;
        p.drawPixmap(x*16*scale,y*16*scale,getTilePixmap(i,scale, highlight));
    }
    return pix;
}

QPixmap BGTileSet::getBGTilePixmap(int tilenum, bool vFlip, bool hFlip, int palette_number, int scale)
{
    QPixmap pix(8*scale,8*scale);
    QPainter p(&pix);
    if(tilenum == 0x2FF)
    {
        p.fillRect(0,0,8*scale,8*scale,getPaletteColor(palette_number, 0));
    }
    else if(tilenum == 0x2FA)
    {
        p.fillRect(0,0,8*scale,8*scale,getPaletteColor(palette_number, 4));
    }
    else
    {
        int tile_index = (8 * 4 *tilenum) - 0x1000;
        std::vector<QColor> colors;
        if(!BGTileData2.empty() && tile_index >= 0x3000 && tile_index <  (0x3000 + m_BG2TransferSize))
        {
            tile_index -= 0x3000;
            for(int y = 0; y < 16; y+=2)
            {
                int y_off = tile_index + y;
                for(int x = 0; x < 8; x++)
                {
                    int bitmask = (0x80 >> x);
                    int shift = 7-x;
                    int result  = (BGTileData2[y_off]&bitmask) >> shift;
                    result |= ((BGTileData2[y_off+1]&bitmask) >> shift) << 1;
                    result |= ((BGTileData2[y_off+0x10]&bitmask) >> shift) << 2;
                    result |= ((BGTileData2[y_off+0x11]&bitmask) >> shift) << 3;
                    colors.push_back(getPaletteColor(palette_number, result));
                }
            }
        }
        else
        {
            for(int y = 0; y < 16; y+=2)
            {
                int y_off = tile_index + y;
                for(int x = 0; x < 8; x++)
                {
                    int bitmask = (0x80 >> x);
                    int shift = 7-x;
                    int result  = (BGTileData[y_off]&bitmask) >> shift;
                    result |= ((BGTileData[y_off+1]&bitmask) >> shift) << 1;
                    result |= ((BGTileData[y_off+0x10]&bitmask) >> shift) << 2;
                    result |= ((BGTileData[y_off+0x11]&bitmask) >> shift) << 3;
                    colors.push_back(getPaletteColor(palette_number, result));
                }
            }
        }
        int y_pos = 0;
        int y_off = 1;
        if(vFlip)
        {
            y_pos = 7;
            y_off = -1;
        }
        int x_start = 0;
        int x_off = 1;
        if(hFlip)
        {
            x_start = 7;
            x_off = -1;
        }
        for(int y = 0; y < 8; y ++)
        {
            int x_pos = x_start;
            for(int x = 0; x <8 ; x++)
            {
                //p.setPen(colors[(y_pos*8)+x_pos]);
                p.fillRect(x*scale,y*scale,scale,scale,colors[(y_pos*8)+x_pos]);
                x_pos += x_off;
            }
            y_pos += y_off;
        }
    }
    return pix;
}

QPixmap BGTileSet::getBGTilesPixmap(bool vFlip, bool hFlip, int palette_number, int scale)
{
    QPixmap pix(0x80*scale,0x140*scale);

    QPainter p(&pix);
    for(int i = 0x80; i < 0x300; i++)
    {
//    int i = 0x81;
        int x = (i-0x80)%0x10;
        int y = (i-0x80)/0x10;
        p.drawPixmap(x*scale*8,y*scale*8, getBGTilePixmap(i, vFlip,hFlip,palette_number,scale));
        unsigned short activeTileData = (*(unsigned short*)&TileSet[(active_tile * 8)+(active_y + (active_x << 1))*2]);
        if(tileHighlightColor.alpha() != 0 && i == (activeTileData&0x03FF))
        {
            p.fillRect(x*scale*8,y*scale*8,8*scale,8*scale,tileHighlightColor);
        }
    }
    return pix;
}

QPixmap BGTileSet::getTilePixmap(uint tilenum, int scale, bool highlight)
{
    QPixmap pix(16*scale,16*scale);
    //int test = TileSet.size()/8;
    if(tilenum < 0x400)
    {
        QPainter p(&pix);
        for(int i = 0; i < 8; i +=2)
        {
            int base_location = (tilenum*8)+i;
            unsigned short tileData = *(unsigned short*)&TileSet[base_location];
            int tile_num = tileData&0x03FF;
            int palette_num = (tileData&0x1C00)>>10;
            bool vFlip = (tileData&0x8000)!= 0;
            bool hFlip = (tileData&0x4000)!= 0;
            int x = i/4;
            int y = (i/2)%2;
            p.drawPixmap(x*scale*8,y*scale*8, getBGTilePixmap(tile_num, vFlip,hFlip,palette_num,scale));

            //QColor test = bgHighlightColor;
            unsigned short active_num = (*( (unsigned short*)
                                           &TileSet[(active_tile * 8)+((active_y + (active_x<<1))*2)]
                                         ) )&0x3FF;
            if(bgHighlightColor.alpha() != 0 && tile_num == active_num && highlight)
            {
                p.fillRect(x*scale*8,y*scale*8,8*scale,8*scale,bgHighlightColor);
            }
            else if(tileHighlightColor.alpha() != 0 && tilenum == active_tile && highlight)
            {
                p.fillRect(x*scale*8,y*scale*8,8*scale,8*scale,tileHighlightColor);
            }
        }
    }
    return pix;
}

QColor BGTileSet::getPaletteColor(uint palette, uint color) const
{
    if(palette < 8 && color < 16)
        return palettes[palette][color];
    return QColor();
}

QPixmap BGTileSet::getActiveTilePixmap(int scale, bool whole)
{
    QPixmap pix(8*scale,8*scale);
    unsigned int tile_location = active_tile * 8;
    if(whole)
    {
        pix = QPixmap(16*scale,16*scale);
        QPainter p(&pix);
        uint old_x = active_x;
        uint old_y = active_y;

        for(int i = 0; i < 8; i +=2)
        {
            //int base_location = (tilenum*8)+i;
            unsigned short tileData = *((unsigned short*)&TileSet[tile_location+i]);//activeTileData[i/2];
            int tile_num = tileData&0x03FF;
            int palette_num = (tileData&0x1C00)>>10;
            bool vFlip = (tileData&0x8000)!= 0;
            bool hFlip = (tileData&0x4000)!= 0;
            active_x = i/4;
            active_y = (i/2)%2;

            p.drawPixmap(active_x*scale*8,active_y*scale*8, getBGTilePixmap(tile_num, vFlip,hFlip,palette_num,scale));
        }
        active_x = old_x;
        active_y = old_y;
    }
    else
    {
        unsigned short active_data = *((unsigned short*)&TileSet[tile_location+((active_y + (active_x<<1))*2)]);//activeTileData[active_y + (active_x<<1)];
        int tile_num = active_data&0x03FF;
        int palette_num = (active_data&0x1C00)>>10;
        bool vFlip = (active_data&0x8000)!= 0;
        bool hFlip = (active_data&0x4000)!= 0;
        pix = getBGTilePixmap(tile_num,vFlip,hFlip,palette_num,scale);
    }
    return pix;
}

QPixmap BGTileSet::getPalettePixmap(uint palette_num, int scale)
{
    QPixmap pix(64*scale,16*scale);
    if(palette_num < 16)
    {
        QPainter p(&pix);
        //unsigned short* current_palette = palettes[palette_num];
        for(int i = 0; i < 16; i++)
        {
            int x = (i%8)*8*scale;
            int y = (i/8)*8*scale;
            QColor p_color  = getPaletteColor(palette_num, i);
            p.fillRect(x,y,8*scale,8*scale,p_color);
            p.setPen(QColor(0,0,0));
            p.drawRect(x,y,(8*scale)-1,(8*scale)-1);

        }
    }
    return pix;
}

int BGTileSet::getTileData(int index, int x, int y) const
{
    int tile_quadrant  = y | (x <<1);
    int location = (index*8) + (tile_quadrant*2);
    return (*(unsigned short*)&TileSet[location]);
}

int BGTileSet::getActiveTileData() const
{
    int return_val = -1;
    if((active_x == 0 || active_x == 1) && (active_y == 0 || active_y == 1))
    {
        int tile_quadrant  = (active_y | (active_x <<1))*2;
        unsigned int tile_location = active_tile * 8;
        return_val = *((unsigned short*)&TileSet[tile_location+tile_quadrant]);//activeTileData[tile_quadrant];
    }
    return return_val;
}
int BGTileSet::getActive_x() const
{
    return active_x;
}

void BGTileSet::setActiveQuarter(int x, int y)
{
    active_x = x;
    active_y = y;
}

int BGTileSet::getActive_y() const
{
    return active_y;
}

int BGTileSet::getActive_tile() const
{
    return active_tile;
}

void BGTileSet::setActive_tile(uint value)
{
    if(active_tile != value)
    {
        setActiveQuarter(0,0);
    }
    active_tile = value;
}

void BGTileSet::setActiveTileData(uint new_value, bool vFlip, bool hFlip, uint palette)
{
    unsigned int value = new_value + 0x80;
    std::vector<uchar>::iterator activeTileData = TileSet.begin() + (active_tile*8);
    UndoStack::UndoEntry u = {UndoStack::TILESET_EDIT, active_tile*8, std::vector<uchar>(activeTileData, activeTileData+8)};
    undo_stack.push(u);
    while(!redo_stack.empty())
    {
        redo_stack.pop();
    }
    if(value <= 0x3FF)
    {
        unsigned short tile_data = value;
        //int old_tile = activeTileData[active_y + (active_x<<1)]&0x3FF;
        tile_data |= (palette << 10);
        if(vFlip)
        {
            tile_data |= 0x8000;
        }
        if(hFlip)
        {
            tile_data |= 0x4000;
        }
        activeTileData[(active_y + (active_x<<1))*2] = tile_data&0xFF;
        activeTileData[((active_y + (active_x<<1))*2) + 1] = (tile_data>>8)&0xFF;
    }
}

void BGTileSet::setActiveTilePixel(int x, int y, int value)
{
    unsigned short tileData = *((unsigned short*)&TileSet[(active_tile*8)+((active_y + (active_x<<1))*2)]);
    ushort tile_num = tileData & 0x3FF;
    std::vector<uchar>::iterator activeBGData;
    if(!BGTileData2.empty() &&
            (tile_num >= 0x200 &&
            (tile_num-0x200) < (m_BG2TransferSize)))
    {
        activeBGData = BGTileData2.begin()+(tile_num*0x20)-0x3000;
    }
    else
    {
        activeBGData = BGTileData.begin()+(tile_num*0x20)-0x1000;
    }
    bool vFlip = (tileData&0x8000)!= 0;
    bool hFlip = (tileData&0x4000)!= 0;
    int y_off = y;
    if(vFlip)
    {
        y_off = 7-y;
    }
    int x_off = x;
    if(hFlip)
    {
        x_off = 7-x;
    }
    int bitmask = (0x80 >> x_off);
    uchar push_value = 0;
    int shift = 7-x_off;
    push_value |= (activeBGData[y_off*2]&bitmask) >> shift;
    push_value |= ((activeBGData[(y_off*2)+1]&bitmask) >> shift) << 1;
    push_value |= ((activeBGData[(y_off*2)+0x10]&bitmask) >> shift) << 2;
    push_value |= ((activeBGData[(y_off*2)+0x11]&bitmask) >> shift) << 3;
    uchar pixel_num = x_off+(y_off*8);
    UndoStack::UndoEntry u = {UndoStack::PIXEL_EDIT, tile_num, std::vector<uchar>({pixel_num, push_value})};
    undo_stack.push(u);
    while(!redo_stack.empty())
    {
        redo_stack.pop();
    }
    if((value&0b0001)!= 0)
    {
        activeBGData[y_off*2] |= (bitmask);
    }
    else
    {
        activeBGData[y_off*2] &= ~(bitmask);
    }
    if((value&0b0010)!= 0)
    {
        activeBGData[(y_off*2)+1] |= (bitmask);
    }
    else
    {
        activeBGData[(y_off*2)+1] &= ~(bitmask);
    }
    if((value&0b0100)!= 0)
    {
        activeBGData[(y_off*2)+0x10] |= (bitmask);
    }
    else
    {
        activeBGData[(y_off*2)+0x10] &= ~(bitmask);
    }
    if((value&0b1000)!= 0)
    {
        activeBGData[(y_off*2)+0x11] |= (bitmask);
    }
    else
    {
        activeBGData[(y_off*2)+0x11] &= ~(bitmask);
    }
}

void BGTileSet::copyBGTile(uint old_tile_number, uint new_tile_number)
{
    unsigned int old_tile_index = (old_tile_number * 0x20);
    unsigned int new_tile_index = (new_tile_number * 0x20);
    std::vector<uchar>::iterator old_it;
    std::vector<uchar>::iterator new_it;
    if(!BGTileData2.empty() &&
            (old_tile_index >= 0x3000 &&
            (old_tile_index-0x3000) < (m_BG2TransferSize)))
    {
        old_it = BGTileData2.begin()+old_tile_index;
    }
    else
    {
        old_it = BGTileData.begin()+old_tile_index;
    }
    if(!BGTileData2.empty() &&
            (new_tile_index >= 0x3000 &&
            (new_tile_index - 0x3000) < (m_BG2TransferSize)))
    {
        new_it = BGTileData2.begin()+new_tile_index;
    }
    else
    {
        new_it = BGTileData.begin()+new_tile_index;
    }
    UndoStack::UndoEntry u = {UndoStack::TILE_EDIT, new_tile_number, std::vector<uchar>(new_it, new_it+0x20) };
    undo_stack.push(u);
    while(!redo_stack.empty())
    {
        redo_stack.pop();
    }
    std::copy(old_it, old_it+0x20, new_it);
}

void BGTileSet::copyTile(uint old_index, uint new_index)
{
    unsigned int old_tile_index = (old_index * 8);
    unsigned int new_tile_index = (new_index * 8);
    std::vector<uchar>::iterator old_it = (TileSet.begin() + old_tile_index);
    std::vector<uchar>::iterator new_it = (TileSet.begin() + new_tile_index);
    UndoStack::UndoEntry u = {UndoStack::TILESET_EDIT, new_index*8, std::vector<uchar>(new_it, new_it+8) };
    undo_stack.push(u);
    while(!redo_stack.empty())
    {
        redo_stack.pop();
    }
    std::copy(old_it, old_it+8, new_it);
}

void BGTileSet::undoLastEdit()
{
    if(!undo_stack.empty())
    {
        if((undo_stack.top().data_type & UndoStack::TILESET_EDIT) != 0)
        {
            uint location = undo_stack.top().change_index;
            UndoStack::UndoEntry u = {undo_stack.top().data_type,
                                      undo_stack.top().change_index,
                                      std::vector<uchar>(TileSet.begin()+location, TileSet.begin()+location+8) };
            redo_stack.push(u);
            std::copy(undo_stack.top().data.begin(), undo_stack.top().data.end(), TileSet.begin()+location);
        }
        else if((undo_stack.top().data_type & UndoStack::TILE_EDIT) != 0)
        {
             uint location = (undo_stack.top().change_index * 0x20);
             std::vector<uchar>::iterator data_begin;
             if(!BGTileData2.empty() && (location >= (uint)0x3000 && location < (uint)(0x3000 + m_BG2TransferSize)))
             {
                 data_begin = BGTileData2.begin() + location - 0x3000;
             }
             else
             {
                 data_begin = BGTileData.begin() + location;
             }
             UndoStack::UndoEntry u = {undo_stack.top().data_type, undo_stack.top().change_index, std::vector<uchar>(data_begin, data_begin+0x20) };
             redo_stack.push(u);
             std::copy(undo_stack.top().data.begin(), undo_stack.top().data.end(), data_begin);
        }
        else if((undo_stack.top().data_type & UndoStack::PIXEL_EDIT) != 0)
        {
            uint location = (undo_stack.top().change_index * 0x20)-0x1000;
            std::vector<uchar>::iterator data_begin;
            if(!BGTileData2.empty() && (location >= (uint)0x3000 && location < (uint)(0x3000 + m_BG2TransferSize)))
            {
                data_begin = BGTileData2.begin() + location - 0x3000;
            }
            else
            {
                data_begin = BGTileData.begin() + location;
            }
            uchar pixel_num = undo_stack.top().data[0];
            uchar current_data = 0;
            int bitmask = 0x80 >> (pixel_num%8);
            int shift = 7-(pixel_num%8);
            int y_off = pixel_num/8;
            current_data |= ((data_begin[0x0 + (y_off*2)]&bitmask) >> shift);
            current_data |= ((data_begin[0x1 + (y_off*2)]&bitmask) >> shift) << 1;
            current_data |= ((data_begin[0x10+ (y_off*2)]&bitmask) >> shift) << 2;
            current_data |= ((data_begin[0x11+ (y_off*2)]&bitmask) >> shift) << 3;
            UndoStack::UndoEntry u = {undo_stack.top().data_type, undo_stack.top().change_index, std::vector<uchar>({pixel_num, current_data}) };
            redo_stack.push(u);

            uchar value = undo_stack.top().data[1];
            if((value&0b0001)!= 0)
            {
                data_begin[0x0 + (y_off*2)] |= (bitmask);
            }
            else
            {
                data_begin[0x0 + (y_off*2)] &= ~(bitmask);
            }
            if((value&0b0010)!= 0)
            {
                data_begin[0x1 + (y_off*2)] |= (bitmask);
            }
            else
            {
                data_begin[0x1 + (y_off*2)] &= ~(bitmask);
            }
            if((value&0b0100)!= 0)
            {
                data_begin[0x10+ (y_off*2)] |= (bitmask);
            }
            else
            {
                data_begin[0x10+ (y_off*2)] &= ~(bitmask);
            }
            if((value&0b1000)!= 0)
            {
                data_begin[0x11+ (y_off*2)] |= (bitmask);
            }
            else
            {
                data_begin[0x11+ (y_off*2)] &= ~(bitmask);
            }
        }
        undo_stack.pop();
    }
}

void BGTileSet::redoLastEdit()
{
    if(!redo_stack.empty())
    {
        if((redo_stack.top().data_type & UndoStack::TILESET_EDIT) != 0)
        {
            uint location = redo_stack.top().change_index;
            UndoStack::UndoEntry u = {redo_stack.top().data_type,
                                      redo_stack.top().change_index,
                                      std::vector<uchar>(TileSet.begin()+location, TileSet.begin()+location+8) };
            undo_stack.push(u);
            std::copy(redo_stack.top().data.begin(), redo_stack.top().data.end(), TileSet.begin()+location);
        }
        else if((redo_stack.top().data_type & UndoStack::TILE_EDIT) != 0)
        {
             uint location = (redo_stack.top().change_index * 0x20);
             std::vector<uchar>::iterator data_begin;
             if(!BGTileData2.empty() && (location >= (uint)0x3000 && location < (uint)(0x3000 + m_BG2TransferSize)))
             {
                 data_begin = BGTileData2.begin() + location - 0x3000;
             }
             else
             {
                 data_begin = BGTileData.begin() + location;
             }
             UndoStack::UndoEntry u = {redo_stack.top().data_type, redo_stack.top().change_index, std::vector<uchar>(data_begin, data_begin+0x20) };
             undo_stack.push(u);
             std::copy(redo_stack.top().data.begin(), redo_stack.top().data.end(), data_begin);
        }
        else if((redo_stack.top().data_type & UndoStack::PIXEL_EDIT) != 0)
        {
            uint location = (redo_stack.top().change_index * 0x20)-0x1000;
            std::vector<uchar>::iterator data_begin;
            if(!BGTileData2.empty() && (location >= (uint)0x3000 && location < (uint)(0x3000 + m_BG2TransferSize)))
            {
                data_begin = BGTileData2.begin() + location - 0x3000;
            }
            else
            {
                data_begin = BGTileData.begin() + location;
            }
            uchar pixel_num = redo_stack.top().data[0];
            uchar current_data = 0;
            int bitmask = 0x80 >> (pixel_num%8);
            int shift = 7-(pixel_num%8);
            int y_off = pixel_num/8;
            current_data |= ((data_begin[0x0 + (y_off*2)]&bitmask) >> shift);
            current_data |= ((data_begin[0x1 + (y_off*2)]&bitmask) >> shift) << 1;
            current_data |= ((data_begin[0x10+ (y_off*2)]&bitmask) >> shift) << 2;
            current_data |= ((data_begin[0x11+ (y_off*2)]&bitmask) >> shift) << 3;
            UndoStack::UndoEntry u = {redo_stack.top().data_type, redo_stack.top().change_index, std::vector<uchar>({pixel_num, current_data}) };
            undo_stack.push(u);

            uchar value = redo_stack.top().data[1];
            if((value&0b0001)!= 0)
            {
                data_begin[0x0 + (y_off*2)] |= (bitmask);
            }
            else
            {
                data_begin[0x0 + (y_off*2)] &= ~(bitmask);
            }
            if((value&0b0010)!= 0)
            {
                data_begin[0x1 + (y_off*2)] |= (bitmask);
            }
            else
            {
                data_begin[0x1 + (y_off*2)] &= ~(bitmask);
            }
            if((value&0b0100)!= 0)
            {
                data_begin[0x10+ (y_off*2)] |= (bitmask);
            }
            else
            {
                data_begin[0x10+ (y_off*2)] &= ~(bitmask);
            }
            if((value&0b1000)!= 0)
            {
                data_begin[0x11+ (y_off*2)] |= (bitmask);
            }
            else
            {
                data_begin[0x11+ (y_off*2)] &= ~(bitmask);
            }
        }
        redo_stack.pop();
    }
}

void BGTileSet::exportData(int mode, uchar *&dest)
{
    switch(mode){
    case TILESET_MODE:
        std::copy(TileSet.begin(), TileSet.end(), dest);
        break;
    case TILE_MODE:
        std::copy(BGTileData.begin(), BGTileData.end(), dest);
        break;
    case EXTRA_TILE_MODE:
        if(!BGTileData2.empty())
        {
            dest[0] = m_BG2TransferSize&0xFF;
            dest[1] = (m_BG2TransferSize>>8)&0xFF;
            std::copy(BGTileData2.begin(), BGTileData2.end(), dest+2);
        }
        break;
    case PALETTE_MODE:
        for(int pal_num = 3; pal_num < 8; pal_num++)
        {
            int pal_index = (pal_num-3)*32;
            for(int color_num = 0; color_num < 32; color_num+=2)
            {
                int red, blue, green;
                palettes[pal_num][color_num/2].getRgb(&red, &green, &blue);
                ushort color = ((blue<<7)&0x7C00) | ((green << 2)&0x3E0) | ((red>>3)&0x1F);
                dest[pal_index + color_num] = color&0xFF;
                dest[pal_index + color_num + 1] = color>>8;
            }
        }
        break;
    default:
        break;
    };
}
