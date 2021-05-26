#include "opendirdialog.h"
#include "ui_opendirdialog.h"
#include <QFileDialog>

OpenDirDialog::OpenDirDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OpenDirDialog)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Drawer |Qt::MSWindowsFixedSizeDialogHint);
    this->setFixedSize(this->size());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

OpenDirDialog::~OpenDirDialog()
{
    delete ui;
}

QString OpenDirDialog::getTileSetFile()
{
    return ui->tilesetFileLineEdit->text();
}

QString OpenDirDialog::getTilesFile()
{
    return ui->tileFilelineEdit->text();
}

QString OpenDirDialog::getExtraTilesFile()
{
    return ui->extraTilesLineEdit->text();
}

QString OpenDirDialog::getPaletteFile()
{
    return ui->paletteFileLineEdit->text();
}

void OpenDirDialog::on_chooseTilesetButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Tileset", QDir::currentPath());
    if (filename != "") {
        ui->tilesetFileLineEdit->setText(filename);
    }
    UpdateAcceptButton();
}

void OpenDirDialog::on_chooseTilesButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Tiles", QDir::currentPath());
    if (filename != "") {
        ui->tileFilelineEdit->setText(filename);
    }
    UpdateAcceptButton();
}

void OpenDirDialog::on_chooseExtraTilesButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Extra Tiles", QDir::currentPath());
    if (filename != "") {
        ui->extraTilesLineEdit->setText(filename);
    }
    UpdateAcceptButton();
}

void OpenDirDialog::on_choosePaletteButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open Palette", QDir::currentPath());
    if (filename != "") {
        ui->paletteFileLineEdit->setText(filename);
    }
    UpdateAcceptButton();
}

void OpenDirDialog::UpdateAcceptButton()
{
    if (ui->tilesetFileLineEdit->text()  == "" ||
            ui->paletteFileLineEdit->text() == "" ||
            ui->tileFilelineEdit->text() == "") {

    }
    else {
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}
