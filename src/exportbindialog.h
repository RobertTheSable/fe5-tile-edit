#ifndef EXPORTBINDIALOG_H
#define EXPORTBINDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "bgtileset.h"

namespace Ui {
class ExportBinDialog;
}

class ExportBinDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportBinDialog(QWidget *parent = 0);
    ~ExportBinDialog();

    QString getWrite_directory() const;
    QString getTileset_bin() const;
    QString getTiles_bin() const;
    QString getPalette_bin() const;
    QString getExtra_tiles_bin() const;
    bool getSaveAddresses() const;
    void setExportExtraTiles(bool value);

private slots:
    void on_pushButton_clicked();

    void on_tileSetLineEdit_editingFinished();

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::ExportBinDialog *ui;
    QString write_directory;
    QString tileset_bin;
    QString tiles_bin;
    QString palette_bin;
    QString extra_tiles_bin;
    bool saveAddresses;
    bool exportExtraTiles;
};

#endif // EXPORTBINDIALOG_H
