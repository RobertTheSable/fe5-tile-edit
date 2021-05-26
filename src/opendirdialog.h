#ifndef OPENDIRDIALOG_H
#define OPENDIRDIALOG_H

#include <QDialog>

namespace Ui {
class OpenDirDialog;
}

class OpenDirDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OpenDirDialog(QWidget *parent = 0);
    ~OpenDirDialog();
    QString getTileSetFile();
    QString getTilesFile();
    QString getExtraTilesFile();
    QString getPaletteFile();


private slots:
    void on_chooseTilesetButton_clicked();

    void on_chooseTilesButton_clicked();

    void on_chooseExtraTilesButton_clicked();

    void on_choosePaletteButton_clicked();

private:
    Ui::OpenDirDialog *ui;
    void UpdateAcceptButton();
};

#endif // OPENDIRDIALOG_H
