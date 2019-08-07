#ifndef BANSHEE1280_H
#define BANSHEE1280_H

#include <QMainWindow>

namespace Ui {
class Banshee1280;
}

class Banshee1280 : public QMainWindow
{
    Q_OBJECT

public:
    explicit Banshee1280(QWidget *parent = 0);
    ~Banshee1280();

private slots:
    void on_write_pushButton_clicked();

private:
    Ui::Banshee1280 *ui;
};

#endif // BANSHEE1280_H
