
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<iostream>
#include<qdebug>

#include<QString>
#include <Eigen/Dense>
#include<vector>
using namespace std;
using namespace Eigen;


Matrix4d fun(double x1,double y1,double z1,double sx1,double sz1,double x2,double y2,double z2,double sx2,double sz2,double x3,double y3,double z3,double sx3,double sz3);
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->biaoding->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


Matrix4d MainWindow::fun(double x1,double y1,double z1,double sx1,double sz1,double x2,double y2,double z2,double sx2,double sz2,double x3,double y3,double z3,double sx3,double sz3)
{
    double a1, b1, c1, a2, b2, c2, a3, b3, c3, p1, q1, p2, q2, p3, q3;
    a1=x1;
    b1=y1;
    c1=z1;
    a2=x2;
    b2=y2;
    c2=z2;
    a3=x3;
    b3=y3;
    c3=z3;
    p1=sx1;
    q1=sz1;
    p2=sx2;
    q2=sz2;
    p3=sx3;
    q3=sz3;
    //计算3个列矩阵


    Matrix3d m1;
    m1 << p1, q1, 1,
        p2, q2, 1,
        p3, q3, 1;
    Vector3d a_1;
    a_1 << a1, a2, a3;
    Vector3d r1;
    r1 = m1.inverse()*a_1;

    Vector3d a_2;
    a_2 << b1, b2, b3;
    Vector3d r2;
    r2 = m1.inverse()*a_2;

    Vector3d a_3;
    a_3 << c1, c2, c3;
    Vector3d r3;
    r3 = m1.inverse()*a_3;

    //cout << "r1:" << r1 << endl << endl;
    //cout << "r2:" << r2 << endl << endl;
    //cout << "r3:" << r3 << endl << endl;
    //叉乘求最后一个向量
    Vector3d r_1;
    Vector3d r_2;
    Vector3d r_3;
    r_1 << r1(0, 0), r2(0, 0), r3(0, 0);
    r_3 << r1(1, 0), r2(1, 0), r3(1, 0);
    r_2 = r_1.cross(r_3);
    //cout << "r_2" << r_2 << endl << endl;
    //归一化
    Vector3d t1;
    t1 = r_1 / sqrt(pow(r_1(0, 0), 2) + pow(r_1(1, 0), 2) + pow(r_1(2, 0), 2));
    Vector3d t2;
    t2 = r_2 / sqrt(pow(r_2(0, 0), 2) + pow(r_2(1, 0), 2) + pow(r_2(2, 0), 2));
    Vector3d t3;
    t3 = r_3 / sqrt(pow(r_3(0, 0), 2) + pow(r_3(1, 0), 2) + pow(r_3(2, 0), 2));
    Matrix4d T;
    T << t1(0, 0), t2(0, 0), t3(0, 0), r1(2, 0),
        t1(1, 0), t2(1, 0), t3(1, 0), r2(2, 0),
        t1(2, 0), t2(2, 0), t3(2, 0), r3(2, 0),
        0, 0, 0, 1;
    cout << "T:" << T << endl;
    return T;
}

void MainWindow::on_biaoding_clicked()
{

   T=(T1+T2+T3)/3;
    cout<<T<<endl;
}

void MainWindow::on_save_clicked()
{
    QString base_coordx1 = ui->basex1->text();
    QString sensor_coordx1 = ui->sensorx1->text();
    QString base_coordy1 = ui->basey1->text();
    QString base_coordz1 = ui->basez1->text();
    QString sensor_coordz1 = ui->sensorz1->text();

    QString base_coordx2 = ui->basex2->text();
    QString sensor_coordx2 = ui->sensorx2->text();
    QString base_coordy2 = ui->basey2->text();
    QString base_coordz2 = ui->basez2->text();
    QString sensor_coordz2 = ui->sensorz2->text();

    QString base_coordx3 = ui->basex3->text();
    QString sensor_coordx3 = ui->sensorx3->text();
    QString base_coordy3 = ui->basey3->text();
    QString base_coordz3 = ui->basez3->text();
    QString sensor_coordz3 = ui->sensorz3->text();

    double a1=base_coordx1.toDouble();
    double b1=base_coordy1.toDouble();
    double c1=base_coordz1.toDouble();

    double a2=base_coordx2.toDouble();
    double b2=base_coordy2.toDouble();
    double c2=base_coordz2.toDouble();

    double a3=base_coordx3.toDouble();
    double b3=base_coordy3.toDouble();
    double c3=base_coordz3.toDouble();

    double p1=sensor_coordx1.toDouble();
    double q1=sensor_coordz1.toDouble();

    double p2=sensor_coordx2.toDouble();
    double q2=sensor_coordz2.toDouble();

    double p3=sensor_coordx3.toDouble();
    double q3=sensor_coordz3.toDouble();
    T1 = fun(a1,b1,c1,p1,q1,a2,b2,c2,p2,q2,a3,b3,c3,p3,q3);

    ui->biaoding->setEnabled(false);
    ui->basex1->clear();
    ui->sensorx1->clear();
    ui->basey1->clear();
    ui->basez1->clear();
    ui->sensorz1->clear();
    ui->basex1->clear();
    ui->basex2->clear();
    ui->sensorx2->clear();
    ui->basey2->clear();
    ui->basez2->clear();
    ui->sensorz2->clear();
    ui->basex3->clear();
    ui->sensorx3->clear();
    ui->basey3->clear();
    ui->basez3->clear();
    ui->sensorz3->clear();
}

void MainWindow::on_save_2_clicked()
{
    QString base_coordx1 = ui->basex1->text();
    QString sensor_coordx1 = ui->sensorx1->text();
    QString base_coordy1 = ui->basey1->text();
    QString base_coordz1 = ui->basez1->text();
    QString sensor_coordz1 = ui->sensorz1->text();

    QString base_coordx2 = ui->basex2->text();
    QString sensor_coordx2 = ui->sensorx2->text();
    QString base_coordy2 = ui->basey2->text();
    QString base_coordz2 = ui->basez2->text();
    QString sensor_coordz2 = ui->sensorz2->text();

    QString base_coordx3 = ui->basex3->text();
    QString sensor_coordx3 = ui->sensorx3->text();
    QString base_coordy3 = ui->basey3->text();
    QString base_coordz3 = ui->basez3->text();
    QString sensor_coordz3 = ui->sensorz3->text();

    double a1=base_coordx1.toDouble();
    double b1=base_coordy1.toDouble();
    double c1=base_coordz1.toDouble();

    double a2=base_coordx2.toDouble();
    double b2=base_coordy2.toDouble();
    double c2=base_coordz2.toDouble();

    double a3=base_coordx3.toDouble();
    double b3=base_coordy3.toDouble();
    double c3=base_coordz3.toDouble();

    double p1=sensor_coordx1.toDouble();
    double q1=sensor_coordz1.toDouble();

    double p2=sensor_coordx2.toDouble();
    double q2=sensor_coordz2.toDouble();

    double p3=sensor_coordx3.toDouble();
    double q3=sensor_coordz3.toDouble();
    T2 = fun(a1,b1,c1,p1,q1,a2,b2,c2,p2,q2,a3,b3,c3,p3,q3);
    ui->biaoding->setEnabled(false);
    ui->basex1->clear();
    ui->sensorx1->clear();
    ui->basey1->clear();
    ui->basez1->clear();
    ui->sensorz1->clear();
    ui->basex2->clear();
    ui->sensorx2->clear();
    ui->basey2->clear();
    ui->basez2->clear();
    ui->sensorz2->clear();
    ui->basex3->clear();
    ui->sensorx3->clear();
    ui->basey3->clear();
    ui->basez3->clear();
    ui->sensorz3->clear();
}

void MainWindow::on_save_3_clicked()
{
    QString base_coordx1 = ui->basex1->text();
    QString sensor_coordx1 = ui->sensorx1->text();
    QString base_coordy1 = ui->basey1->text();
    QString base_coordz1 = ui->basez1->text();
    QString sensor_coordz1 = ui->sensorz1->text();

    QString base_coordx2 = ui->basex2->text();
    QString sensor_coordx2 = ui->sensorx2->text();
    QString base_coordy2 = ui->basey2->text();
    QString base_coordz2 = ui->basez2->text();
    QString sensor_coordz2 = ui->sensorz2->text();

    QString base_coordx3 = ui->basex3->text();
    QString sensor_coordx3 = ui->sensorx3->text();
    QString base_coordy3 = ui->basey3->text();
    QString base_coordz3 = ui->basez3->text();
    QString sensor_coordz3 = ui->sensorz3->text();

    double a1=base_coordx1.toDouble();
    double b1=base_coordy1.toDouble();
    double c1=base_coordz1.toDouble();

    double a2=base_coordx2.toDouble();
    double b2=base_coordy2.toDouble();
    double c2=base_coordz2.toDouble();

    double a3=base_coordx3.toDouble();
    double b3=base_coordy3.toDouble();
    double c3=base_coordz3.toDouble();

    double p1=sensor_coordx1.toDouble();
    double q1=sensor_coordz1.toDouble();

    double p2=sensor_coordx2.toDouble();
    double q2=sensor_coordz2.toDouble();

    double p3=sensor_coordx3.toDouble();
    double q3=sensor_coordz3.toDouble();
    T3 = fun(a1,b1,c1,p1,q1,a2,b2,c2,p2,q2,a3,b3,c3,p3,q3);
    ui->biaoding->setEnabled(true);
}

void MainWindow::on_pushButton_clicked()
{
    ui->biaoding->setEnabled(false);
    ui->basex1->clear();
    ui->sensorx1->clear();
    ui->basey1->clear();
    ui->basez1->clear();
    ui->sensorz1->clear();
    ui->basex2->clear();
    ui->sensorx2->clear();
    ui->basey2->clear();
    ui->basez2->clear();
    ui->sensorz2->clear();
    ui->basex3->clear();
    ui->sensorx3->clear();
    ui->basey3->clear();
    ui->basez3->clear();
    ui->sensorz3->clear();

}
