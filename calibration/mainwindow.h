#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Eigen/Dense>
using namespace Eigen;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Matrix4d T1;
    Matrix4d T2;
    Matrix4d T3;
    Matrix4d T;
    Matrix4d fun(double x1,double y1,double z1,double sx1,double sz1,double x2,double y2,double z2,double sx2,double sz2,double x3,double y3,double z3,double sx3,double sz3);
private slots:


    void on_biaoding_clicked();

    void on_save_clicked();

    void on_save_2_clicked();

    void on_save_3_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
