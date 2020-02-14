#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTcpSocket>
#include<QTcpServer>
#include "biaoding.h"
#include <QHostAddress>
#include <QMessageBox>
#include <QNetworkInterface>
#include <vector>
#include <QTimer>
#include "calculatexy.h"
using namespace std;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool connectState;

private:
    Ui::MainWindow *ui;
    QTcpSocket *socket1;
    biaoding *mybd;

    int robotPointCount;
    int chuanGanQiPointCount;
    int shiJiaoCount;
    bool isSendSign;
    vector<vector<float>> SJData;

    //---------------//
    QTcpServer *tcpServer;
    QList<QTcpSocket*> tcpClient1;
    QTcpSocket *currentClient;
    vector<vector<double>> mappingMatrix;

    QTimer *timer;
    QTimer *timerSendData;
    calculatexy *mycalxy;

    QString shiJiaoFile;
    QString zhoudianfile;

private slots:
    void connected();
    void readData();
    void on_btnSendData_clicked();
    void on_btnConnect_clicked();
    void disconnectedSlot();
    void on_btnBiaoDing_clicked();
    void on_btn_return_clicked();
    void on_btn_biaoding_start_clicked();
    void on_btn_save_hd_clicked();
    void on_btn_save_zb_clicked();
    void on_btn_clear_clicked();
    void on_btn_hd_clear_clicked();

    //服务器槽函数
    void fNewConnectionSlot();
    void fdisconnectedSlot();
    void ReadData_2();


    void update();
    void sendSign();
    void on_fbtnConnect_clicked();
    void on_fbtnClear_clicked();
    void on_fbtnSend_clicked();
    void on_btn_send_data_clicked();
    void on_btn_connectCGQ_clicked();
    void on_btn_biaoding_clicked();
    void on_btnconnect_robot_clicked();
    void on_btn_shijiao_clicked();
    void on_btn_return_biaoding_clicked();
    void on_btn_sj_save_zb_clicked();
    void on_btn_sj_clear_clicked();
    void on_btn_move_data_clicked();
};

#endif // MAINWINDOW_H
