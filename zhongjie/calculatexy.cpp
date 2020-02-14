#include "calculatexy.h"
#include<iostream>
#include<fstream>
#include<cmath>
#include<iomanip>
#include <QCoreApplication>
//#define jiqirenndian "D:/qtcode/biaoding/jiqiren.txt" //机器人n点地址   n*16
//#define chuanganqindian "D:/qtcode/biaoding/chuangan.txt" //传感器n点数据地址  n*2
//#define shouyanjuzhenjieguo "D:/qtcode/biaoding/shouyanjuzhen.txt" //手眼矩阵地址
//#define yonghuzuobiao "D:/qtcode/biaoding/oxy.txt" //用户坐标系地址
//#define zoudian "D:/qtcode/biaoding/zoudian.txt" //最终xy结果
#define n 8//取点个数
using namespace std;
calculatexy::calculatexy()
{
    jiqirenndian = QCoreApplication::applicationDirPath() + "/jiqiren.txt" ;  //"D:/qtcode/biaoding/jiqiren.txt" //机器人n点地址   n*16
    chuanganqindian = QCoreApplication::applicationDirPath() + "/chuangan.txt"; //"D:/qtcode/biaoding/chuangan.txt" //传感器n点数据地址  n*2
    shouyanjuzhenjieguo = QCoreApplication::applicationDirPath() + "/shouyanjuzhen.txt"; //"D:/qtcode/biaoding/shouyanjuzhen.txt" //手眼矩阵地址
    yonghuzuobiao = QCoreApplication::applicationDirPath() + "/oxy.txt"; //"D:/qtcode/biaoding/oxy.txt" //用户坐标系地址
    zoudian = QCoreApplication::applicationDirPath() + "/zoudian.txt"; //"D:/qtcode/biaoding/zoudian.txt" //最终xy结果


}


static void LUni(double A[2][2], double B[2][2])
{
    double u[2][2] = { 0 }; double l[2][2] = { 0 }; double uni[2][2] = { 0 }; double lni[2][2] = { 0 }; double s = 0; double t = 0;
    for (int i = 0; i < 2; i++)       //计算l矩阵对角线
        l[i][i] = 1;

    for (int i = 0; i < 2; i++)
    {
        for (int j = i; j < 2; j++)
        {
            s = 0;
            for (int k = 0; k < i; k++)
            {
                s += l[i][k] * u[k][j];
            }
            u[i][j] = A[i][j] - s;      //按行计算u值
        }

        for (int j = i + 1; j < 2; j++)
        {
            s = 0;
            for (int k = 0; k < i; k++)
            {
                s += l[j][k] * u[k][i];
            }
            l[j][i] = (A[j][i] - s) / u[i][i];      //按列计算l值
        }
    }

    for (int i = 0; i < 2; i++)        //按行序，行内从高到低，计算l的逆矩阵
    {
        lni[i][i] = 1;
    }
    for (int i = 1; i < 2; i++)
    {
        for (int j = 0; j < i; j++)
        {
            s = 0;
            for (int k = 0; k < i; k++)
            {
                s += l[i][k] * lni[k][j];
            }
            lni[i][j] = -s;
        }
    }
    for (int i = 0; i < 2; i++)                    //按列序，列内按照从下到上，计算u的逆矩阵
    {
        uni[i][i] = 1 / u[i][i];
    }
    for (int i = 1; i < 2; i++)
    {
        for (int j = i - 1; j >= 0; j--)
        {
            s = 0;
            for (int k = j + 1; k <= i; k++)
            {
                s += u[j][k] * uni[k][i];
            }
            uni[j][i] = -s / u[j][j];
        }
    }
    for (int i = 0; i < 2; i++)            //计算矩阵a的逆矩阵
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++)
                B[i][j] += uni[i][k] * lni[k][j];
}
static void jizuobiaoxyz(double B[4][4], double A[4][4], double C[2], double D[3])
{
    double E[4][4] = { 0 };
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                E[i][j] += A[i][k] * B[k][j];
    D[0] = E[0][0] * C[0] + E[0][2] * C[1] + E[0][3];
    D[1] = E[1][0] * C[0] + E[1][2] * C[1] + E[1][3];
    D[2] = E[2][0] * C[0] + E[2][2] * C[1] + E[2][3];
}
static void yonghuzuobiaoxy(double A[2], double B[2], double C[2], double D[3], double XY[2])
{
    double AA[2][2] = { 0 };
    AA[0][0] = (B[0] - A[0]) / sqrt(pow(B[0] - A[0], 2) + pow(B[1] - A[1], 2));
    AA[0][1] = (C[0] - A[0]) / sqrt(pow(C[0] - A[0], 2) + pow(C[1] - A[1], 2));
    AA[1][0] = (B[1] - A[1]) / sqrt(pow(B[0] - A[0], 2) + pow(B[1] - A[1], 2));
    AA[1][1] = (C[1] - A[1]) / sqrt(pow(C[0] - A[0], 2) + pow(C[1] - A[1], 2));
    double AAni[2][2] = { 0 };
    LUni(AA, AAni);
    XY[0] = AAni[0][0] * (D[0] - A[0]) + AAni[0][1] * (D[1] - A[1]);
    XY[1] = AAni[1][0] * (D[0] - A[0]) + AAni[1][1] * (D[1] - A[1]);
}
static void xy(double moduan[4][4], double shouyanbianhuan[4][4], double dushu[2], double oo[2], double xx[2], double yy[2], double xyjieguo[2])
{
    double jieguo[3] = { 0 };//基坐标系下焊点位置xyz 中间值


    jizuobiaoxyz(shouyanbianhuan, moduan, dushu, jieguo);//计算基坐标系下焊点位置
    //cout << jieguo[0] <<" "<< jieguo[1]<<" " << jieguo[2]<<endl;
    yonghuzuobiaoxy(oo, xx, yy, jieguo, xyjieguo);//计算XY的值
}
void calculatexy::piliangxy()
{
    //存储结果矩阵
    double xyjieguo[n][2] = { 0 };

    ifstream infile;
    /*
    infile.open(jiqirenndian);
    int n = 0;
    char ch;
    while (infile.get(ch))
    {
        if (ch == '\n')
            n += 1;
    }
    //cout << n;
    infile.close();
    */
    double point[n][4][4] = { 0 };
    double camera[n][2] = { 0 };
    double shouyanbianhuan[4][4] = { 0 };
    double ooxxyy[3][2] = { 0 };


    infile.open(jiqirenndian.toLatin1().data());
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            infile >> point[i][j / 4][j % 4];
        }
    }
    infile.close();
    infile.open(chuanganqindian.toLatin1().data());
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            infile >> camera[i][j];
        }
    }
    infile.close();
    infile.open(shouyanjuzhenjieguo.toLatin1().data());
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            infile >> shouyanbianhuan[i][j];
        }
    }
    infile.close();
    infile.open(yonghuzuobiao.toLatin1().data());

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            infile >> ooxxyy[i][j];
            cout << ooxxyy[i][j] << endl;
        }
    }
    infile.close();
    for (int i = 0; i < n; i++)
    {
        xy(point[i], shouyanbianhuan, camera[i], ooxxyy[0], ooxxyy[1], ooxxyy[2], xyjieguo[i]);
        cout << xyjieguo[i][0] << " " << xyjieguo[i][1] << endl;
    }
    ofstream outfile(zoudian.toLatin1().data(), ios::out);
    outfile <<n<< " ";
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            outfile << QString::number(xyjieguo[i][j],'f',3).toFloat() << " ";
        }
        outfile <<0<< " ";
    }
    outfile.close();
}
