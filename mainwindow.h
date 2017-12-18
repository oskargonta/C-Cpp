#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "library.h"
#include "flexflowshop.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int l_iteracji=0;

private:
    Ui::MainWindow *ui;
    FlexFlowShop FFS;
    string sciezka;

    void fillTable();
    void fillTable2();
private slots:
    void button_pressed();
    void on_spinBox_valueChanged(int arg1);

    void on_Licz_clicked();

    void on_wczytaj_plik_clicked();

public slots:
    void change_LCD_number(int number);
    void table_init(vector<int> &number);
};

#endif // MAINWINDOW_H
