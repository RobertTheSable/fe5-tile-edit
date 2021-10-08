#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QErrorMessage>
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <vector>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    highlightColor(QColor(0,0,0,0)),
    current_palette_color(0),
    switching(false)
{
    ui->setupUi(this);

    current_chapter = -1;
    ui->tileEditLabel->setUnitSize(8);
    ui->tileEditLabel->setScale(4);
    ui->rawTileEditLabel->setUnitSize(1);
    ui->rawTileEditLabel->setScale(ui->zoomBGTileSpinBox->value());
    ui->rawTilesLabel->setUnitSize(8);
    ui->rawTilesLabel->setScale(2);
    ui->paletteLabel->setScale(3);
    ui->paletteLabel->setUnitSize(8);
    connect(ui->tileSetLabel, &TileSetLabel::clicked, this, &MainWindow::setTileFocus);
    connect(ui->tileEditLabel, &TileSetLabel::clicked, this, &MainWindow::setBGTileFromQuadrant);
    connect(ui->rawTilesLabel, &TileSetLabel::double_clicked, this, &MainWindow::selectTileFromBank);
    connect(ui->paletteLabel, &TileSetLabel::clicked, this, &MainWindow::selectColor);
    connect(ui->rawTileEditLabel, &TileSetLabel::clicked, this, &MainWindow::editActiveTile);
    connect(ui->rawTilesLabel, &TileSetLabel::released, this, &MainWindow::dragBGTile);
    connect(ui->tileSetLabel, &TileSetLabel::released, this, &MainWindow::dragMainTile);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::show()
{
    QMainWindow::show();
    auto iniPath = QStandardPaths::locate(QStandardPaths::AppDataLocation, INI_DIR, QStandardPaths::LocateDirectory);
    if (iniPath == "") {
        QMessageBox::critical(
                    NULL,
                    tr("Error"),
                    tr("The ini folder was not found - this folder is needed for the application to function.")
                    );
        close();
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open ROM", QDir::currentPath(), tr("SNES Roms (*.sfc *.smc)"));
    if(filename != "")
    {
        while (!m_Tilesets.empty()) {
            m_Tilesets.pop_back();
        }
        std::string text = filename.toStdString();
        auto iniPath = QStandardPaths::locate(QStandardPaths::AppDataLocation, INI_DIR, QStandardPaths::LocateDirectory);
        m_RomMap = RomMap(text, iniPath.toStdString());
        if(m_RomMap.getMyState() == RomMap::rom_ok)
        {
            for(int i = 0; i < 37; i++)
            {
                int read_address = m_RomMap.getAddress("CHAPTER_DATA_TABLE") + (i<<1);
                read_address = m_RomMap.getAddress("CHAPTER_DATA_BANK") + (m_RomMap.readAddress(read_address)&0xFFFF);

                uchar *chapter_tileset, *chapter_BGData, *chapter_BGData2, *chapter_palette;

                if((m_RomMap.readAddress(read_address+0x2E)) == 0)
                {
                    chapter_tileset = &m_RomMap[m_RomMap.readAddress(read_address+9)];
                    chapter_BGData = &m_RomMap[m_RomMap.readAddress(read_address)];
                    chapter_BGData2 = nullptr;
                    chapter_palette = &m_RomMap[m_RomMap.readAddress(read_address+0xC)+0x60];
                }
                else
                {
                    chapter_tileset = &m_RomMap[m_RomMap.readAddress(read_address+9)];
                    chapter_BGData = &m_RomMap[m_RomMap.readAddress(read_address)];
                    chapter_BGData2 = &m_RomMap[m_RomMap.readAddress(read_address+0x2E)];
                    chapter_palette = &m_RomMap[m_RomMap.readAddress(read_address+0xC)+0x60];
                }
                m_Tilesets.push_back(BGTileSet(&m_RomMap[0x808000], chapter_tileset,chapter_BGData,chapter_palette,chapter_BGData2));
            }
            ui->chapterSetButton->setEnabled(true);
            ui->tileSetLabel->setEnabled(true);
            ui->chapterSpinBox->setEnabled(true);
            ui->chapterSpinBox->setMaximum(m_Tilesets.size());
            ui->chapterSpinBox->setValue(m_Tilesets.size());
            ui->chapterSetButton->setEnabled(true);
            ui->zoomBGTileSpinBox->setEnabled(true);
            ui->actionExport->setEnabled(true);
            ui->actionHighlight_Same_BG_Tile->setEnabled(true);
            ui->actionRedo->setEnabled(true);
            ui->actionUndo->setEnabled(true);
            ui->actionOpen_From_Files->setEnabled(false);
            ui->actionExport_PNG->setEnabled(true);
            updateChapterData();
            updatePaletteDisplay();
        }
        else
        {
            switch (m_RomMap.getMyState()) {
            case RomMap::bad_header:
                QMessageBox::warning(this, tr("Error Opening ROM"), tr("ROM header is malformed."));
                break;
            case RomMap::not_supported:
                QMessageBox::warning(this, tr("Error Opening ROM"), tr("Current ROM is not supported."));
                break;
            case RomMap::malformed_name:
                QMessageBox::warning(this, tr("Error Opening ROM"), tr("Internal header appears malformed."));
                break;
            case RomMap::no_file:
                QMessageBox::warning(this, tr("Error Opening ROM"), tr("Could not open ROM file."));
                break;
            default:
                QMessageBox::warning(this, tr("Error Opening ROM"), tr("Unidentified error opening ROM."));
                break;
            }
        }
    }
}

void MainWindow::setTileFocus(int x, int y)
{
    switching = true;
    if(x < 32 && y < 32)
    {
        int index = y*32 +x;
        m_Tilesets[ui->chapterSpinBox->value()-1].setActive_tile(index);
        updateBGTileDisplay();
        updateTileDisplay();
        updateBGTilesetDisplay();
    }
    switching = false;
}

void MainWindow::selectTileFromBank(int x, int y)
{
    int tile_index = x+(y*16);
    m_Tilesets[ui->chapterSpinBox->value()-1].setActiveTileData(tile_index, ui->tileVFlipCheckBox->isChecked(), ui->tileHFlipCheckBox->isChecked(), ui->tilePalletteSpinBox->value());
    updateTileDisplay();
    updateBGTileDisplay();
    updateBGTilesetDisplay();
    updateTilesetDisplay();
}

void MainWindow::setBGTileFromQuadrant(int x, int y)
{
    switching = true;
    if(x < 2 && y < 2)
    {
        m_Tilesets[ui->chapterSpinBox->value()-1].setActiveQuarter(x,y);
        updateBGTileDisplay();
        updateTilesetDisplay();
        updateBGTilesetDisplay();
    }
    switching = false;
}

void MainWindow::editActiveTile(int x, int y)
{
    if(x < 8 && y < 8)
    {
        m_Tilesets[ui->chapterSpinBox->value()-1].setActiveTilePixel(x,y,current_palette_color);
        updateBGTilesetDisplay();
        updateTileDisplay();
        updateBGTileDisplay();
        updateTilesetDisplay();
    }
}

void MainWindow::selectColor(int x, int y)
{
    current_palette_color = x+(y*8);
    updatePaletteDisplay();
}

void MainWindow::dragBGTile(int old_x, int old_y, int new_x, int new_y)
{
    m_Tilesets[current_chapter-1].copyBGTile(old_x + (old_y*16), new_x + (new_y*16));
    updateBGTilesetDisplay();
    m_Tilesets[current_chapter-1].setActive_tile(m_Tilesets[current_chapter-1].getActive_tile());
    updateTilesetDisplay();
    updateTileDisplay();
    updateBGTileDisplay();
}

void MainWindow::dragMainTile(int old_x, int old_y, int new_x, int new_y)
{
    m_Tilesets[current_chapter-1].copyTile(old_x + (old_y*32), new_x + (new_y*32));
    updateBGTilesetDisplay();
    updateTilesetDisplay();
    updateTileDisplay();
    updateBGTileDisplay();
}

void MainWindow::on_chapterSetButton_clicked()
{
    updateChapterData();
}

void MainWindow::on_chapterSpinBox_editingFinished()
{
    updateChapterData();
}

void MainWindow::updateChapterData()
{
    if(ui->chapterSpinBox->value() != current_chapter)
    {
        current_chapter = ui->chapterSpinBox->value();
        m_Tilesets[current_chapter-1].setBgHighlightColor(highlightColor);
        BGTileSet tileset = m_Tilesets[current_chapter-1];
        //ui->tileSetLabel->setPixmap(tileset.getTileSetPixmap(1));
        ui->tilesetAddressDisplay->setEnabled(true);
        ui->tilesetAddressDisplay->setText(QString::number(tileset.getTilesetAddress(),16));
        ui->paletteAddressDisplay->setEnabled(true);
        ui->paletteAddressDisplay->setText(QString::number(tileset.getPaletteAddress(), 16));
        ui->bgTileAddressDisplay->setEnabled(true);
        ui->bgTileAddressDisplay->setText(QString::number(tileset.getBGTilesAddress(), 16));
        if(tileset.getExtraTilesAddress() != 0)
        {
            ui->extraTileAddressDisplay->setEnabled(true);
            ui->extraTileAddressDisplay->setText(QString::number(tileset.getExtraTilesAddress(), 16));
        }
        else
        {
            ui->extraTileAddressDisplay->setEnabled(false);
            ui->extraTileAddressDisplay->setText(tr(""));
        }
        updateTilesetDisplay();
        updateBGTilesetDisplay();
        updateTileDisplay();
        updateBGTileDisplay();
    }
}

void MainWindow::updateTileDisplay()
{
    ui->tileEditLabel->setEnabled(true);
    ui->tilePalletteSpinBox->setEnabled(true);
    ui->tileNumberEdit->setEnabled(true);
    ui->tileHFlipCheckBox->setEnabled(true);
    ui->tileVFlipCheckBox->setEnabled(true);
    ui->tileEditLabel->setPixmap(m_Tilesets[ui->chapterSpinBox->value()-1].getActiveTilePixmap(ui->tileEditLabel->getScale()));
    ui->tileSetLabel->setPixmap(m_Tilesets[ui->chapterSpinBox->value()-1].getTileSetPixmap(1));
}

void MainWindow::updateBGTileDisplay(bool reload)
{
    if(reload) {
        int tile_data = m_Tilesets[ui->chapterSpinBox->value()-1].getActiveTileData();//getTileData(index, x, y);
        int tile_num = tile_data&0x03FF;
        int palette_num = (tile_data&0x1C00)>>10;
        bool vFlip = (tile_data&0x8000)!= 0;
        bool hFlip = (tile_data&0x4000)!= 0;
        ui->tileNumberEdit->setText(QString::number(tile_num));
        ui->tilePalletteSpinBox->setValue(palette_num);
        ui->tileHFlipCheckBox->setChecked(hFlip);
        ui->tileVFlipCheckBox->setChecked(vFlip);
    }
    ui->rawTileEditLabel->setPixmap(m_Tilesets[ui->chapterSpinBox->value()-1].getActiveTilePixmap(ui->rawTileEditLabel->getScale(), false));//getBGTilePixmap(tile_num,vFlip, hFlip,palette_num, ui->rawTileEditLabel->getScale()));
}

void MainWindow::updateTilesetDisplay()
{
    current_chapter = ui->chapterSpinBox->value();
    ui->tileSetLabel->setPixmap(m_Tilesets[current_chapter-1].getTileSetPixmap(1));
}

void MainWindow::updateBGTilesetDisplay()
{
    int current_palette = ui->tilePalletteSpinBox->value();
    current_chapter = ui->chapterSpinBox->value();
    BGTileSet tileset = m_Tilesets[current_chapter-1];
    ui->rawTilesLabel->setPixmap(tileset.getBGTilesPixmap(false, false, current_palette, ui->rawTilesLabel->getScale()));
}

void MainWindow::updatePaletteDisplay()
{
    int current_palette = ui->tilePalletteSpinBox->value();
    ui->paletteLabel->setPixmap(m_Tilesets[current_chapter-1].getPalettePixmap(current_palette,ui->paletteLabel->getScale()));
    QPixmap palette_color = QPixmap(48,48);
    QPainter p(&palette_color);
    p.fillRect(0,0,48,48,m_Tilesets[current_chapter-1].getPaletteColor(current_palette,current_palette_color));
    p.drawRect(0,0,47,47);
    ui->selectedColorLabel->setPixmap(palette_color);

}

void MainWindow::on_actionExport_triggered()
{
    ExportBinDialog export_dialogue;
    export_dialogue.setExportExtraTiles(ui->extraTileAddressDisplay->isEnabled());
    if(export_dialogue.exec() == QDialog::Accepted)
    {
        //QMessageBox::information(this, tr("Test"),export_dialogue.getExtra_tiles_bin());
        if(export_dialogue.getTileset_bin()!= "")
        {
            QString filename = export_dialogue.getTileset_bin();
            QFile outfile(filename);
            if(!outfile.open(QFile::WriteOnly))
            {
                QMessageBox::warning(this, tr("Error opening file"),tr("Could not open ") + filename + tr(" for writing."));
            }
            else
            {
                uchar* output = new uchar[m_Tilesets[current_chapter-1].getTileSetSize()];
                m_Tilesets[current_chapter-1].exportData(BGTileSet::TILESET_MODE, output);
                outfile.write((char*)output, m_Tilesets[current_chapter-1].getTileSetSize());
                outfile.flush();
                outfile.close();
                delete[] output;
            }
        }
        if(export_dialogue.getTiles_bin() != "")
        {
            QString filename = export_dialogue.getTiles_bin();
            QFile outfile(filename);
            if(!outfile.open(QFile::WriteOnly))
            {
                QMessageBox::warning(this, tr("Error opening file"),tr("Could not open ") + filename + tr(" for writing."));
            }
            else
            {
                uchar* output = new uchar[m_Tilesets[current_chapter-1].getBGTileSize1()];
                m_Tilesets[current_chapter-1].exportData(BGTileSet::TILE_MODE, output);
                outfile.write((char*)output, m_Tilesets[current_chapter-1].getBGTileSize1());
                outfile.flush();
                outfile.close();
                delete[] output;
            }

        }
        if(export_dialogue.getPalette_bin() != "")
        {
            QString filename = export_dialogue.getPalette_bin();
            QFile outfile(filename);
            if(!outfile.open(QFile::WriteOnly))
            {
                QMessageBox::warning(this, tr("Error opening file"),tr("Could not open ") + filename + tr(" for writing."));
            }
            else
            {
                uchar* output = new uchar[32*5];
                m_Tilesets[current_chapter-1].exportData(BGTileSet::PALETTE_MODE, output);
                outfile.write((char*)output, 32*5);
                outfile.flush();
                outfile.close();
                delete[] output;
            }
        }
        if(export_dialogue.getExtra_tiles_bin() != "")
        {
            QString filename = export_dialogue.getExtra_tiles_bin();
            QFile outfile(filename);
            if(!outfile.open(QFile::WriteOnly))
            {
                QMessageBox::warning(this, tr("Error opening file"),tr("Could not open ") + filename + tr(" for writing."));
            }
            else
            {
                uchar* output = new uchar[m_Tilesets[current_chapter-1].getBGTileSize2()+2];
                m_Tilesets[current_chapter-1].exportData(BGTileSet::EXTRA_TILE_MODE, output);
                outfile.write((char*)output, m_Tilesets[current_chapter-1].getBGTileSize2()+2);
                outfile.flush();
                outfile.close();
                delete[] output;
            }
        }
        if(export_dialogue.getSaveAddresses())
        {
            QString filename = export_dialogue.getWrite_directory() + QDir::separator() + tr("addresses.txt");
            QFile outfile(filename);
            if(!outfile.open(QFile::WriteOnly|QFile::Text))
            {
                QMessageBox::warning(this, tr("Error opening file"),tr("Could not open ") + filename + tr(" for writing."));
            }
            QTextStream address_out(&outfile);
            address_out.setIntegerBase(16);
            if(export_dialogue.getTileset_bin()!= "")
            {
                address_out << "Tileset Address: $"  << m_Tilesets[current_chapter-1].getTilesetAddress() << endl;
            }
            if(export_dialogue.getTiles_bin() != "")
            {
                address_out << "Raw Tiles Address: $"  << m_Tilesets[current_chapter-1].getBGTilesAddress() << endl;
            }
            if(export_dialogue.getExtra_tiles_bin() != "")
            {
                address_out << "Extra Tiles Address: $"  << m_Tilesets[current_chapter-1].getExtraTilesAddress() << endl;
            }
            if(export_dialogue.getPalette_bin() != "")
            {
                address_out << "Palette Address: $"  << m_Tilesets[current_chapter-1].getPaletteAddress() << endl;
            }
            outfile.close();
        }
    }
}

void MainWindow::on_zoomBGTileSpinBox_editingFinished()
{
    ui->rawTileEditLabel->setScale(ui->zoomBGTileSpinBox->value());
    updateBGTileDisplay();
}

void MainWindow::on_actionHighlight_Same_BG_Tile_toggled(bool arg1)
{
    if(arg1)
    {
        highlightColor = QColor(255,0,255,100);
    }
    else
    {
        highlightColor = QColor(0,0,0,0);
    }
    m_Tilesets[current_chapter-1].setBgHighlightColor(highlightColor);
    updateTilesetDisplay();
}

void MainWindow::on_tileVFlipCheckBox_toggled(bool checked)
{
    if(!switching)
    {
        m_Tilesets[ui->chapterSpinBox->value()-1].setActiveTileData(ui->tileNumberEdit->text().toInt()-0x80,checked, ui->tileHFlipCheckBox->isChecked(), ui->tilePalletteSpinBox->value());
        updateBGTileDisplay();
        updateTileDisplay();
        updateTilesetDisplay();
    }
}

void MainWindow::on_tileHFlipCheckBox_toggled(bool checked)
{
    if(!switching)
    {
        m_Tilesets[ui->chapterSpinBox->value()-1].setActiveTileData(ui->tileNumberEdit->text().toInt()-0x80, ui->tileVFlipCheckBox->isChecked(), checked, ui->tilePalletteSpinBox->value());
        updateBGTileDisplay();
        updateTileDisplay();
        updateTilesetDisplay();
    }
}

void MainWindow::on_tilePalletteSpinBox_valueChanged(int arg1)
{
    if(!switching)
    {
        m_Tilesets[ui->chapterSpinBox->value()-1].setActiveTileData(ui->tileNumberEdit->text().toInt()-0x80, ui->tileVFlipCheckBox->isChecked(), ui->tileHFlipCheckBox->isChecked(), arg1);
        updateBGTileDisplay(false);
        updateTileDisplay();
        updateTilesetDisplay();
    }
    updatePaletteDisplay();
}

void MainWindow::on_actionUndo_triggered()
{
     m_Tilesets[ui->chapterSpinBox->value()-1].undoLastEdit();
     updateBGTilesetDisplay();
     updateTilesetDisplay();
     updateBGTileDisplay();
     updateTileDisplay();
}

void MainWindow::on_actionRedo_triggered()
{
    m_Tilesets[ui->chapterSpinBox->value()-1].redoLastEdit();
    updateBGTilesetDisplay();
    updateTilesetDisplay();
    updateBGTileDisplay();
    updateTileDisplay();
}

std::vector<uchar> getDataFromBinaryFile(QString filename)
{
    std::ifstream input_tileset(filename.toStdString(), std::ios::binary | std::ios::ate);
    input_tileset.unsetf(std::ios::skipws);
    uint tilesetFileSize = input_tileset.tellg();
    std::vector<uchar> tileset_data;
    tileset_data.reserve(tilesetFileSize);
    input_tileset.seekg(0, std::ios::beg);
    std::copy(std::istream_iterator<uchar>(input_tileset), std::istream_iterator<uchar>(), std::back_inserter(tileset_data));
    input_tileset.close();
    return tileset_data;
}

void MainWindow::on_actionOpen_From_Files_triggered()
{
    OpenDirDialog opendir;
    if (opendir.exec() == QDialog::Accepted)
    {
        std::vector<uchar> tilesetData = getDataFromBinaryFile(opendir.getTileSetFile());
        std::vector<uchar> tileData = getDataFromBinaryFile(opendir.getTilesFile());
        std::vector<uchar> paletteData = getDataFromBinaryFile(opendir.getPaletteFile());
        if(opendir.getExtraTilesFile() != "") {
            std::vector<uchar> extraTileData = getDataFromBinaryFile(opendir.getExtraTilesFile());
            m_Tilesets.push_back(BGTileSet(&tilesetData[0], tilesetData.size(),
                    &tileData[0], tileData.size(), &paletteData[0], &extraTileData[0], extraTileData.size()));
        } else {
            m_Tilesets.push_back(BGTileSet(&tilesetData[0], tilesetData.size(),
                &tileData[0], tileData.size(), &paletteData[0]));
        }
        ui->chapterSetButton->setEnabled(true);
        ui->chapterSetButton->setEnabled(true);
        ui->tileSetLabel->setEnabled(true);
        ui->chapterSpinBox->setEnabled(true);
        ui->chapterSpinBox->setMaximum(m_Tilesets.size());
        ui->chapterSpinBox->setValue(m_Tilesets.size());
        ui->chapterSetButton->setEnabled(true);
        ui->zoomBGTileSpinBox->setEnabled(true);
        ui->actionExport->setEnabled(true);
        ui->actionHighlight_Same_BG_Tile->setEnabled(true);
        ui->actionRedo->setEnabled(true);
        ui->actionUndo->setEnabled(true);
        ui->actionExport_PNG->setEnabled(true);
        updateChapterData();
        updatePaletteDisplay();
    }
}

void MainWindow::on_actionExport_PNG_triggered()
{
    QString filename = QFileDialog::getSaveFileName(this, "Output File", QDir::currentPath(), tr("PNG (*.png)"));
    if (filename != "") {
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly)) {
            m_Tilesets[ui->chapterSpinBox->value()-1].getTileSetPixmap(1, false).save(&file, "PNG");
            QMessageBox::information(this, tr("Success"), tr("Image exported to ") + filename);
        } else {
            QMessageBox::warning(this, tr("Error opening file"),tr("Could not open ") + filename + tr(" for writing."));
        }


    }
}
