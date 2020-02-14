#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QDebug>
#include<QByteArray>
#include<QDataStream>
#include "biaoding.h"
#include <iostream>
#include<fstream>
#include<QTimer>
#include "calculatexy.h"
using namespace std;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //客户端
    socket1 = new QTcpSocket();
    ui->btnSendData->setEnabled(false);
    ui->lineEdit_IP->setText(QNetworkInterface().allAddresses().at(1).toString());
   // qDebug()<<" "<<QNetworkInterface().allAddresses().at(1).toString()<<endl;
   // ui->lineEdit_IP->setText("169.254.116.183");
    ui->lineEdit_Port->setText("3100");
    connectState=0;
    isSendSign=0;

    //记录存储点数
    robotPointCount = 0;
    chuanGanQiPointCount = 0;
    shiJiaoCount=0;

    //---------------------------------服务器--------------------------------//
    tcpServer = new QTcpServer(this);
    ui->fedtIP->setText(QNetworkInterface().allAddresses().at(1).toString());   //获取本地IP
    ui->fbtnConnect->setEnabled(true);
    ui->fbtnSend->setEnabled(false);
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(fNewConnectionSlot()));

    //用于发送数据
    timer = new QTimer();
    timerSendData = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(timerSendData, SIGNAL(timeout()), this, SLOT(sendSign()));

    //标定和xz转换为xy
    mybd = new biaoding();
    mycalxy = new calculatexy();


    ui->lb_bd_name->setText("jiqiren.txt");
    ui->lb_hd_name->setText("chuangan.txt");

}

MainWindow::~MainWindow()
{
    delete ui;
}

//-----下客户端
//连接上软件操作
void MainWindow::connected()
{
    ui->btnSendData->setEnabled(true);
    //ui->btnConnect->setEnabled(false);
    //qDebug()<<"Socketconnectedasclient";
    connect(socket1,SIGNAL(readyRead()),this,SLOT(readData()));
    ui->btnConnect->setText("断开连接");
    ui->lbConnectState->setStyleSheet ("background-color: rgb(0, 255, 0);");
}

//收到软件发来的信息处理
void MainWindow::readData()
{
    QByteArray buffer;  //add by me
    buffer.resize(socket1->bytesAvailable());//add by me
    socket1->read(buffer.data(),buffer.size());//add by me

    QString msg = QString::fromLocal8Bit(buffer.data());//add by me
    //滤除没用的数据
    if(buffer.size()<= 0)  return;
    //cout<<"buffer size"<<buffer.size()<<endl;

//    QByteArray arr=socket1->readAll();
    //对收到的数据进行坐标点的提取
    qDebug()<<msg.mid(7, 7)<<" buffer "<<msg.right(7);

    ui->textEdit_recv->append(msg);
    ui->lb_x->setText(msg.mid(7, 7));
    ui->lb_z->setText(msg.right(7));

}

//是否发送数据给软件获取焊点
void MainWindow::on_btnSendData_clicked()
{
    isSendSign = !isSendSign;
    if(isSendSign)
    {
        timerSendData->start(100);
        ui->btnSendData->setText("Stop");
    }else
    {
        timerSendData->stop();
        ui->btnSendData->setText("Send");
    }


}

//发送文本框内的数据给软件
void MainWindow::sendSign()
{
    QString data = ui->textEdit_send->toPlainText();
    QByteArray text = data.toLocal8Bit();  //将QString以自己正确的编码方式读取并转换成编译器编码方式的字节流,数组
    //cout<<text[0]<<endl;
    if(data == "")  return;    // 文本输入框为空时
    socket1->write(text);
    //ui->textEdit_send->append(text);

}

//连接按钮---是否连接软件
void MainWindow::on_btnConnect_clicked()
{
    connectState = !connectState;
    //点击链接
    QString connectIP;
    connectIP=ui->lineEdit_IP->text();
    QString connectPort;
    connectPort=ui->lineEdit_Port->text();
    connect(socket1,SIGNAL(connected()),this,SLOT(connected()));
    connect(socket1, SIGNAL(disconnected()), this, SLOT(disconnectedSlot())); //断开连接

    if(connectState)
    {

        socket1->connectToHost(connectIP,connectPort.toInt(),QTcpSocket::ReadWrite);
        //qDebug()<<"2";

    }
    else
    {

        socket1->close();

    }



}

//连接断开操作
void MainWindow::disconnectedSlot()
{
    //由于disconnected信号并未提供SocketDescriptor，所以需要遍历寻找
   // qDebug()<<"dis";
    ui->btnConnect->setText("连接");
    ui->lbConnectState->setStyleSheet ("background-color: rgb(189, 189, 189);");

}

//启动标定程序，计算结果并且输出
void MainWindow::on_btn_biaoding_start_clicked()
{
    mybd->shouyan();
    vector<double> temp;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp.push_back(mybd->shouyanjuzhen[i][j]);
            cout <<mybd->shouyanjuzhen[i][j] << "   ";
        }
        mappingMatrix.push_back(temp);
        temp.clear();
        cout << endl;
    }
    cout<<" "<<mappingMatrix.size()<<" "<<mappingMatrix[0].size()<<endl;

}

//保存焊点数据
void MainWindow::on_btn_save_hd_clicked()
{
    QString hd_name = ui->lb_hd_name->text();
    QString chuanGanQiZuoBiaoFile = QCoreApplication::applicationDirPath() + "/" + hd_name;
    ofstream outfile(chuanGanQiZuoBiaoFile.toLatin1().data(), ios::app);
    QString pointX = ui->lb_x->text();
    QString pointZ = ui->lb_z->text();
    outfile <<pointX.toFloat() << " "<<pointZ.toFloat()<<endl;
    outfile.close();

    chuanGanQiPointCount++;
     ui->lb_hd_count->setText(QString::number(chuanGanQiPointCount));


}

//保存输入的机器人坐标数据
void MainWindow::on_btn_save_zb_clicked()
{
    QString bd_name = ui->lb_bd_name->text();
    QString chuanGanQiZuoBiaoFile = QCoreApplication::applicationDirPath() + "/" + bd_name;
    qDebug()<< chuanGanQiZuoBiaoFile;
    ofstream outfile(chuanGanQiZuoBiaoFile.toLatin1().data(), ios::app);
    QString pointX = ui->le_x->text();
    QString pointX2 = ui->le_x_2->text();
    QString pointX3 = ui->le_x_3->text();
    QString pointX4 = ui->le_x_4->text();
    QString pointY = ui->le_y->text();
    QString pointY2 = ui->le_y_2->text();
    QString pointY3 = ui->le_y_3->text();
    QString pointY4 = ui->le_y_4->text();
    QString pointZ = ui->le_z->text();
    QString pointZ2 = ui->le_z_2->text();
    QString pointZ3 = ui->le_z_3->text();
    QString pointZ4 = ui->le_z_4->text();
    outfile <<pointX.toFloat() << " "<<pointX2.toFloat()<< " "<<pointX3.toFloat()<< " "<<pointX4.toFloat();
    outfile <<" "<<pointY.toFloat() << " "<<pointY2.toFloat()<< " "<<pointY3.toFloat()<< " "<<pointY4.toFloat();
    outfile <<" "<<pointZ.toFloat() << " "<<pointZ2.toFloat()<< " "<<pointZ3.toFloat()<< " "<<pointZ4.toFloat();
    outfile <<" "<<0<< " "<<0<< " "<<0<<" " << 1<<endl;
    outfile.close();
    robotPointCount++;
    ui->lb_count->setText(QString::number(robotPointCount));
}

//清除保存在文件中的机器人坐标数据
void MainWindow::on_btn_clear_clicked()
{
    QString bd_name = ui->lb_bd_name->text();
    QString chuanGanQiZuoBiaoFile = QCoreApplication::applicationDirPath() + "/" + bd_name;
    ofstream outfile(chuanGanQiZuoBiaoFile.toLatin1().data(), ios::out);
    outfile.close();
    robotPointCount = 0;
    ui->lb_count->setText(QString::number(robotPointCount));
}

//清除保存在文件中的焊点数据
void MainWindow::on_btn_hd_clear_clicked()
{
    QString hd_name = ui->lb_hd_name->text();
    QString chuanGanQiZuoBiaoFile = QCoreApplication::applicationDirPath() + "/" + hd_name;
    ofstream outfile(chuanGanQiZuoBiaoFile.toLatin1().data(), ios::out);
    outfile.close();
    chuanGanQiPointCount = 0;
    ui->lb_hd_count->setText(QString::number(chuanGanQiPointCount));
}


//————————————————————————————服务器—————————————————————————————————————————//

// newConnection -> fnewConnectionSlot 新连接建立的槽函数
void MainWindow::fNewConnectionSlot()
{
    currentClient = tcpServer->nextPendingConnection();
    tcpClient1.append(currentClient);
    ui->fcbxConnection->addItem(tr("%1:%2").arg(currentClient->peerAddress().toString().split("::ffff:")[1])\
                                          .arg(currentClient->peerPort()));
    connect(currentClient, SIGNAL(readyRead()), this, SLOT(ReadData_2()));
    connect(currentClient, SIGNAL(disconnected()), this, SLOT(fdisconnectedSlot()));
    ui->fstate->setStyleSheet ("background-color: rgb(0, 255, 0);");
}
//定时器 -- 使得一次性能够发送 B + 数据点
void MainWindow::update()
{


    //QString data1 ="4 10 10 0 20 20 0 30 30 0 40 40 0";
    QString data1 =ui->fedtSend->toPlainText();
//data+=QString::number(x);
//data+="V00A>+";
//data+=QString::number(y);
//QString data = ui->fedtSend->toPlainText();

    QByteArray text1 = data1.toLocal8Bit();

    if(data1 == "")  return;    // 文本输入框为空时

    if(ui->fcbxConnection->currentIndex() == 0)
    {
        for(int i=0; i<tcpClient1.length(); i++)
        {
        //tcpClient1[i]->write(data.toLatin1()); //qt5除去了.toAscii()    //by me

        //add by me

        tcpClient1[i]->write(text1,text1.length());
        //return;

        }
    }
    timer->stop();

}
// 客户端数据可读信号，对应的读数据槽函数
void MainWindow::ReadData_2()
{
    qDebug()<<111;
    // 由于readyRead信号并未提供SocketDecriptor，所以需要遍历所有客户端
    for(int i=0; i<tcpClient1.length(); i++)
    {
        QByteArray buffer;  //add by me
        buffer.resize(tcpClient1[i]->bytesAvailable());//add by me
        tcpClient1[i]->read(buffer.data(),buffer.size());//add by me
        //QByteArray buffer = tcpClient1[i]->readAll();  //by me
        if(buffer.isEmpty())    continue;

        static QString IP_Port, IP_Port_Pre;
        IP_Port = tr("[%1:%2]:").arg(tcpClient1[i]->peerAddress().toString().split("::ffff:")[1])\
                                     .arg(tcpClient1[i]->peerPort());

        // 若此次消息的地址与上次不同，则需显示此次消息的客户端地址
        if(IP_Port != IP_Port_Pre)
            ui->fedtRecv->append(IP_Port);

        QString msg = QString::fromLocal8Bit(buffer.data());//add by me

        ui->fedtRecv->append(msg);  //buffer ->  msg ; by me
       // bool equal;
        QString gvc;
        gvc=msg.toStdString().data();
        //equal=gvc.compare("A/r/n");
        qDebug()<<gvc;
        if(gvc ==QString::fromLocal8Bit("A\r\n"))
        {
            cout<<111;
            QString data ="B " ;
            QByteArray text = data.toLocal8Bit();
            if(ui->fcbxConnection->currentIndex() == 0)
             {
                for(int i=0; i<tcpClient1.length(); i++)



                    tcpClient1[i]->write(text,text.length());         //add by me



              }


             timer->start(200);

//            QString data ="B " ;

//            QString data1 ="2 10 10 0 20 20 0";
//            //data+=QString::number(x);
//            //data+="V00A>+";
//            //data+=QString::number(y);
//            //QString data = ui->fedtSend->toPlainText();

//            QByteArray text = data.toLocal8Bit();    //add by me
//            QByteArray text1 = data1.toLocal8Bit();
//            ui->fedtSend->setPlainText(data);
//            if(data == "")  return;    // 文本输入框为空时

//            if(ui->fcbxConnection->currentIndex() == 0)
//            {
//                for(int i=0; i<tcpClient1.length(); i++)
//                    {
//                    //tcpClient1[i]->write(data.toLatin1()); //qt5除去了.toAscii()    //by me

//                    tcpClient1[i]->write(text,text.length());         //add by me

//                    tcpClient1[i]->write(text1,text1.length());

//                    }
//            }
        }

        //更新ip_port
        IP_Port_Pre = IP_Port;
    }
}
// disconnected -> fdisconnectedSlot 客户端断开连接的槽函数
void MainWindow::fdisconnectedSlot()
{
    //由于disconnected信号并未提供SocketDescriptor，所以需要遍历寻找
    for(int i=0; i<tcpClient1.length(); i++)
    {
        if(tcpClient1[i]->state() == QAbstractSocket::UnconnectedState)
        {
            // 删除存储在combox中的客户端信息
            ui->fcbxConnection->removeItem(ui->fcbxConnection->findText(tr("%1:%2")\
                                  .arg(tcpClient1[i]->peerAddress().toString().split("::ffff:")[1])\
                                  .arg(tcpClient1[i]->peerPort())));
            // 删除存储在tcpClient列表中的客户端信息
             tcpClient1[i]->destroyed();
             tcpClient1.removeAt(i);
        }
    }
    ui->fstate->setStyleSheet ("background-color: rgb(189, 189, 189);");
}

void MainWindow::on_fbtnConnect_clicked()
{
    if(ui->fbtnConnect->text()=="监听")
    {
        bool ok = tcpServer->listen(QHostAddress::Any, ui->fedtPort->text().toInt());
        if(ok)
        {
            ui->fbtnConnect->setEnabled(false);
            ui->fbtnSend->setEnabled(true);
        }
        cout<<ok<<endl;
    }
   // else
   // {

     //   for(int i=0; i<tcpClient1.length(); i++)//断开所有连接
       // {
       //     tcpClient1[i]->disconnectFromHost();
        //    bool ok = tcpClient1[i]->waitForDisconnected(1000);
       //     if(!ok)
        //    {
       //         // 处理异常
         //   }
         //   tcpClient1.removeAt(i);  //从保存的客户端列表中取去除
       // }
       // tcpServer->close();     //不再监听端口
       // ui->fbtnConnect->setText("监听");
       // ui->btnSend_2->setEnabled(false);
}

void MainWindow::on_fbtnClear_clicked()
{
    ui->fedtRecv->clear();
}

//手动按钮-->发送数据
void MainWindow::on_fbtnSend_clicked()
{
    QString data = ui->fedtSend->toPlainText();
    QByteArray text = data.toLocal8Bit();    //add by me
    if(data == "")  return;    // 文本输入框为空时
    //全部连接
    if(ui->fcbxConnection->currentIndex() == 0)
    {
        for(int i=0; i<tcpClient1.length(); i++)
            //tcpClient1[i]->write(data.toLatin1()); //qt5除去了.toAscii()    //by me
            tcpClient1[i]->write(text,text.length());         //add by me

    }
    //指定连接
    else
    {
        QString clientIP = ui->fcbxConnection->currentText().split(":")[0];
        int clientPort = ui->fcbxConnection->currentText().split(":")[1].toInt();
//        qDebug() << clientIP;
//        qDebug() << clientPort;
        for(int i=0; i<tcpClient1.length(); i++)
        {
            if(tcpClient1[i]->peerAddress().toString().split("::ffff:")[1]==clientIP\
                            && tcpClient1[i]->peerPort()==clientPort)
            {
                //tcpClient1[i]->write(data.toLatin1());    //by me
                tcpClient1[i]->write(text,text.length());   //add by me
                return; //ip:port唯一，无需继续检索
            }
        }
    }
}
//————————————————————————————服务器—————————————————————————————————————————//



//启动走点数据计算 -- 并且将结果显示在发送区域中
void MainWindow::on_btn_move_data_clicked()
{
    ui->fedtSend->clear();
    //启动走点数据计算
    mycalxy->piliangxy();

    QString zhoudianfile = QCoreApplication::applicationDirPath() + "/zoudian.txt" ;;
    ifstream inFile(zhoudianfile.toLatin1().data(), ios::in);
    string lineStr;

    QString data;
    while (getline(inFile, lineStr))
    {
        data = QString::fromStdString(lineStr);

   }
    inFile.close();

    ui->fedtSend->append(data);
}



//试教记录点 -- 未用到
void MainWindow::on_btn_sj_save_zb_clicked()
{
    QString shiJiaoFile = QCoreApplication::applicationDirPath() + "/shijiao.txt";
    ofstream outfile(shiJiaoFile.toLatin1().data(), ios::app);
    vector<float> tempPoint;
    QString pointX = ui->le_sj_x->text();
    QString pointY = ui->le_sj_y->text();
    QString pointZ = ui->le_sj_z->text();
    outfile <<pointX.toFloat() << " "<<pointY.toFloat()<< " "<<pointZ.toFloat()<<endl;
    outfile.close();

    tempPoint.push_back(pointX.toFloat());
    tempPoint.push_back(pointY.toFloat());
    tempPoint.push_back(pointZ.toFloat());
    SJData.push_back(tempPoint);
    tempPoint.clear();
    cout<<"SJData "<<SJData.size()<<endl;

    shiJiaoCount++;
    ui->lb_sj_count->setText(QString::number(shiJiaoCount));
}
//试教记录点清除 -- 未用到
void MainWindow::on_btn_sj_clear_clicked()
{
    shiJiaoFile = QCoreApplication::applicationDirPath() + "/shijiao.txt";
    ofstream outfile(shiJiaoFile.toLatin1().data(), ios::out);
    outfile.close();
    shiJiaoCount = 0;
    SJData.clear();
    ui->lb_sj_count->setText(QString::number(shiJiaoCount));
}



//下面切换 ---- 界面
void MainWindow::on_btnBiaoDing_clicked()
{
    //mybd->shouyan();
    ui->stackedWidget->setCurrentIndex(2);

}

void MainWindow::on_btn_return_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btn_send_data_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_btn_connectCGQ_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btn_biaoding_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btnconnect_robot_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_btn_shijiao_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void MainWindow::on_btn_return_biaoding_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}
