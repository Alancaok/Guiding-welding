#include "biaoding.h"
#include<iostream>
#include<fstream>
#include<cmath>
#include<iomanip>
#include <vector>
#include<QString>
#include <QCoreApplication>
#include <QDebug>
using namespace std;
//迭代次数为100次 超过100次建议重新测量
#define pi 3.1415926


biaoding::biaoding()
{



}

static void LUni(double A[4][4], double B[4][4])
{
    double u[4][4] = { 0 }; double l[4][4] = { 0 }; double uni[4][4] = { 0 }; double lni[4][4] = { 0 }; double s = 0; double t = 0;
    for (int i = 0; i < 4; i++)       //计算l矩阵对角线
    {
        l[i][i] = 1;
    }

    for (int i = 0; i < 4; i++)
    {
        for (int j = i; j < 4; j++)
        {
            s = 0;
            for (int k = 0; k < i; k++)
            {
                s += l[i][k] * u[k][j];
            }
            u[i][j] = A[i][j] - s;      //按行计算u值
        }

        for (int j = i + 1; j < 4; j++)
        {
            s = 0;
            for (int k = 0; k < i; k++)
            {
                s += l[j][k] * u[k][i];
            }
            l[j][i] = (A[j][i] - s) / u[i][i];      //按列计算l值
        }
    }

    for (int i = 0; i < 4; i++)        //按行序，行内从高到低，计算l的逆矩阵
    {
        lni[i][i] = 1;
    }
    for (int i = 1; i < 4; i++)
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
    for (int i = 0; i < 4; i++)                    //按列序，列内按照从下到上，计算u的逆矩阵
    {
        uni[i][i] = 1 / u[i][i];
    }
    for (int i = 1; i < 4; i++)
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
    for (int i = 0; i < 4; i++)            //计算矩阵a的逆矩阵
        for (int j = 0; j < 4; j++)
            for (int k = 0; k < 4; k++)
                B[i][j] += uni[i][k] * lni[k][j];
}
static void JacbiCor3(double A[3][3], double B[3][3], double dbEps, int nJt) //3*3矩阵求特征值与特征向量
{
    int nCount = 0;		//迭代次数
    while (1)
    {
        //在A的非对角线上找到最大元素
        double dbMax = fabs(A[0][1]);
        int nRow = 0;
        int nCol = 1;
        for (int i = 0; i < 3; i++)			//行
        {
            for (int j = 0; j < 3; j++)		//列
            {
                double d = fabs(A[i][j]);

                if ((i != j) && (d > dbMax))
                {
                    dbMax = d;
                    nRow = i;
                    nCol = j;
                }
            }
        }

        if (dbMax < dbEps)     //精度符合要求
            break;

        if (nCount > nJt)       //迭代次数超过限制
            break;

        nCount++;

        double dbApp = A[nRow][nRow];
        double dbApq = A[nRow][nCol];
        double dbAqq = A[nCol][nCol];

        //计算旋转角度
        double dbAngle = 0.5 * atan2(-2 * dbApq, dbAqq - dbApp);
        double dbSinTheta = sin(dbAngle);
        double dbCosTheta = cos(dbAngle);
        double dbSin2Theta = sin(2 * dbAngle);
        double dbCos2Theta = cos(2 * dbAngle);

        A[nRow][nRow] = dbApp * dbCosTheta * dbCosTheta +
            dbAqq * dbSinTheta * dbSinTheta + 2 * dbApq * dbCosTheta * dbSinTheta;
        A[nCol][nCol] = dbApp * dbSinTheta * dbSinTheta +
            dbAqq * dbCosTheta * dbCosTheta - 2 * dbApq * dbCosTheta * dbSinTheta;
        A[nRow][nCol] = 0.5 * (dbAqq - dbApp) * dbSin2Theta + dbApq * dbCos2Theta;
        A[nCol][nRow] = A[nRow][nCol];

        for (int i = 0; i < 3; i++)
        {
            if ((i != nCol) && (i != nRow))
            {

                double iq = A[i][nCol];
                double ip = A[i][nRow];
                A[i][nRow] = iq * dbSinTheta + ip * dbCosTheta;
                A[i][nCol] = iq * dbCosTheta - ip * dbSinTheta;
                A[nRow][i] = A[i][nRow];
                A[nCol][i] = A[i][nCol];
            }
        }

        //计算特征向量
        if (nCount == 1)
        {
            B[nRow][nRow] = dbCosTheta;
            B[nCol][nCol] = dbCosTheta;
            B[nRow][nCol] = -dbSinTheta;
            B[nCol][nRow] = dbSinTheta;
        }
        else
        {
            for (int i = 0; i < 3; i++)
            {

                double uip = B[i][nRow];
                double uiq = B[i][nCol];
                B[i][nRow] = uiq * dbSinTheta + uip * dbCosTheta;
                B[i][nCol] = uiq * dbCosTheta - uip * dbSinTheta;
            }
        }
    }
    //对特征值进行排序以及重新排列特征向量,特征值即A主对角线上的元素
    double A1 = 0;
    double B1[3] = { 0 };
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3 - i - 1; j++)
        {
            if (A[j][j] < A[j + 1][j + 1])
            {
                A1 = A[j][j];
                A[j][j] = A[j + 1][j + 1];
                A[j + 1][j + 1] = A1;
                for (int k = 0; k < 3; k++)
                {
                    B1[k] = B[k][j];
                    B[k][j] = B[k][j + 1];
                    B[k][j + 1] = B1[k];

                }
            }
        }
    }
    /*
    //设定正负号
    for (int i = 0; i < 9; i++)
    {
        double dSumVec = 0;
        for (int j = 0; j < 9; j++)
            dSumVec += B[j][i];
        if (dSumVec < 0)
        {
            for (int j = 0; j < 9; j++)
                B[j][i] *= -1;
        }
    }
    */
}
static void JacbiCor6(double A[6][6], double B[6][6], double dbEps, int nJt) //3*3矩阵求特征值与特征向量
{
    int nCount = 0;		//迭代次数
    while (1)
    {
        //在A的非对角线上找到最大元素
        double dbMax = fabs(A[0][1]);
        int nRow = 0;
        int nCol = 1;
        for (int i = 0; i < 6; i++)			//行
        {
            for (int j = 0; j < 6; j++)		//列
            {
                double d = fabs(A[i][j]);

                if ((i != j) && (d > dbMax))
                {
                    dbMax = d;
                    nRow = i;
                    nCol = j;
                }
            }
        }

        if (dbMax < dbEps)     //精度符合要求
            break;

        if (nCount > nJt)       //迭代次数超过限制
            break;

        nCount++;

        double dbApp = A[nRow][nRow];
        double dbApq = A[nRow][nCol];
        double dbAqq = A[nCol][nCol];

        //计算旋转角度
        double dbAngle = 0.5 * atan2(-2 * dbApq, dbAqq - dbApp);
        double dbSinTheta = sin(dbAngle);
        double dbCosTheta = cos(dbAngle);
        double dbSin2Theta = sin(2 * dbAngle);
        double dbCos2Theta = cos(2 * dbAngle);

        A[nRow][nRow] = dbApp * dbCosTheta * dbCosTheta +
            dbAqq * dbSinTheta * dbSinTheta + 2 * dbApq * dbCosTheta * dbSinTheta;
        A[nCol][nCol] = dbApp * dbSinTheta * dbSinTheta +
            dbAqq * dbCosTheta * dbCosTheta - 2 * dbApq * dbCosTheta * dbSinTheta;
        A[nRow][nCol] = 0.5 * (dbAqq - dbApp) * dbSin2Theta + dbApq * dbCos2Theta;
        A[nCol][nRow] = A[nRow][nCol];

        for (int i = 0; i < 6; i++)
        {
            if ((i != nCol) && (i != nRow))
            {

                double iq = A[i][nCol];
                double ip = A[i][nRow];
                A[i][nRow] = iq * dbSinTheta + ip * dbCosTheta;
                A[i][nCol] = iq * dbCosTheta - ip * dbSinTheta;
                A[nRow][i] = A[i][nRow];
                A[nCol][i] = A[i][nCol];
            }
        }

        //计算特征向量
        if (nCount == 1)
        {
            B[nRow][nRow] = dbCosTheta;
            B[nCol][nCol] = dbCosTheta;
            B[nRow][nCol] = -dbSinTheta;
            B[nCol][nRow] = dbSinTheta;
        }
        else
        {
            for (int i = 0; i < 6; i++)
            {

                double uip = B[i][nRow];
                double uiq = B[i][nCol];
                B[i][nRow] = uiq * dbSinTheta + uip * dbCosTheta;
                B[i][nCol] = uiq * dbCosTheta - uip * dbSinTheta;
            }
        }
    }
    //对特征值进行排序以及重新排列特征向量,特征值即A主对角线上的元素
    double A1 = 0;
    double B1[6] = { 0 };
    for (int i = 0; i < 6; i++)
    {
        for (int j = 0; j < 6 - i - 1; j++)
        {
            if (A[j][j] < A[j + 1][j + 1])
            {
                A1 = A[j][j];
                A[j][j] = A[j + 1][j + 1];
                A[j + 1][j + 1] = A1;
                for (int k = 0; k < 6; k++)
                {
                    B1[k] = B[k][j];
                    B[k][j] = B[k][j + 1];
                    B[k][j + 1] = B1[k];

                }
            }
        }
    }
    /*
    //设定正负号
    for (int i = 0; i < 9; i++)
    {
        double dSumVec = 0;
        for (int j = 0; j < 9; j++)
            dSumVec += B[j][i];
        if (dSumVec < 0)
        {
            for (int j = 0; j < 9; j++)
                B[j][i] *= -1;
        }
    }
    */
}
static void MatrixMul(double MatrixA[4][4], double MatrixB[4][4], double Result[4][4]) //4*4矩阵与4*4矩阵相乘
{
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                Result[i][j] = Result[i][j] + MatrixA[i][k] * MatrixB[k][j];
            }
        }
}
static void MatrixMul1(double MatrixA[4][4], double MatrixB[4][1], double Result[4][1]) //4*4矩阵与4*1矩阵相乘
{
    for (int i = 0; i < 4; i++)
    {
        for (int k = 0; k < 4; k++)
        {
            Result[i][0] = Result[i][0] + MatrixA[i][k] * MatrixB[k][0];
        }
    }
}
static void MatrixMulbuchang(double MatrixA[3][8], double MatrixB[8][1], double Result[3][1]) //3*8矩阵与8*1矩阵相乘
{
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 1; j++)
        {
            for (int k = 0; k < 8; k++)
            {
                Result[i][j] = Result[i][j] + MatrixA[i][k] * MatrixB[k][j];
            }
        }
}

static void SVDC(double C[8][3], double result[3][8])            //矩阵C求伪逆
{
    double CTC[3][3] = { 0 }; double CCT[8][8] = { 0 };
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            for (int k = 0; k < 8; k++)
            {
                CTC[i][j] = CTC[i][j] + C[k][i] * C[k][j];
            }
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            for (int k = 0; k < 3; k++)
            {
                CCT[i][j] = CCT[i][j] + C[i][k] * C[j][k];
            }
    double U[8][8] = { 0 };
    double V[3][3] = { 0 };
    for (int i = 0; i < 3; i++)
        V[i][i] = 1;
    JacbiCor3(CTC, V, 1e-15, 1000);
    double S[3][8] = { 0 };
    double VS[3][8] = { 0 };

    for (int i = 0; i < 3; i++)
    {
        //cout << setprecision(15) << sqrt(JTJ[i][i]) << "  ";
        if (CTC[i][i] > 1e-5)
        {
            S[i][i] = 1 / sqrt(CTC[i][i]);
            //cout << setprecision(15) << S[i][i]<<"    ";
        }
    }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                VS[i][j] = VS[i][j] + V[i][k] * S[k][j];
            }
        }
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 3; k++)
            {
                U[i][j] = U[i][j] + C[i][k] * VS[k][j];
            }
        }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 8; k++)
            {
                result[i][j] = result[i][j] + VS[i][k] * U[j][k];
            }
        }
}
static void SVDjaco(double C[8][6], double result[6][8])            //矩阵C求伪逆
{
    double CTC[6][6] = { 0 }; double CCT[8][8] = { 0 };
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 6; j++)
            for (int k = 0; k < 8; k++)
            {
                CTC[i][j] = CTC[i][j] + C[k][i] * C[k][j];
            }
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            for (int k = 0; k < 6; k++)
            {
                CCT[i][j] = CCT[i][j] + C[i][k] * C[j][k];
            }
    double U[8][8] = { 0 };
    double V[6][6] = { 0 };
    for (int i = 0; i < 6; i++)
        V[i][i] = 1;
    JacbiCor6(CTC, V, 1e-15, 1000);
    double S[6][8] = { 0 };
    double VS[6][8] = { 0 };

    for (int i = 0; i < 6; i++)
    {
        //cout << setprecision(15) << sqrt(JTJ[i][i]) << "  ";
        if (CTC[i][i] > 1e-5)
        {
            S[i][i] = 1 / sqrt(CTC[i][i]);
            //cout << setprecision(15) << S[i][i]<<"    ";
        }
    }
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 6; k++)
            {
                VS[i][j] = VS[i][j] + V[i][k] * S[k][j];
            }
        }
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 6; k++)
            {
                U[i][j] = U[i][j] + C[i][k] * VS[k][j];
            }
        }
    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 8; j++)
        {
            for (int k = 0; k < 8; k++)
            {
                result[i][j] = result[i][j] + VS[i][k] * U[j][k];
            }
        }
}
static void dxdydz(double PX[8][1], double PY[8][1], double PZ[8][1], double a, double b, double c, double C[8][3], double px, double py, double pz, double dx[8], double dy[8], double dz[8])
{
    for (int i = 0; i < 8; i++)
    {

        dx[i] = PX[i][0] - cos(a) * cos(b) * C[i][0] - cos(a) * sin(b) * cos(c) * C[i][1] - sin(a) * sin(c) * C[i][1] - px;
        dy[i] = PY[i][0] - sin(a) * cos(b) * C[i][0] - sin(a) * sin(b) * cos(c) * C[i][1] + cos(a) * sin(c) * C[i][1] - py;
        dz[i] = PZ[i][0] + sin(b) * C[i][0] - cos(b) * cos(c) * C[i][1] - pz;
    }
}
static void j1j2j3j4j5j6(double PX[8][1], double PY[8][1], double PZ[8][1], double a, double b, double c, double C[8][3], double px, double py, double pz, double dx[8], double dy[8], double dz[8], double jaco[8][6])
{
    for (int i = 0; i < 8; i++)
    {
        jaco[i][0] = (sin(a) * cos(b) * C[i][0] + sin(a) * sin(b) * cos(c) * C[i][1] - cos(a) * sin(c) * C[i][1]) * dx[i] + (-cos(a) * cos(b) * C[i][0] - cos(a) * sin(b) * cos(c) * C[i][1] - sin(a) * sin(c) * C[i][1]) * dy[i];
        jaco[i][1] = (cos(a) * sin(b) * C[i][0] - cos(a) * cos(b) * cos(c) * C[i][1]) * dx[i] + (sin(a) * sin(b) * C[i][0] - sin(a) * cos(b) * cos(c) * C[i][1]) * dy[i] + (cos(b) * C[i][0] + sin(b) * cos(c) * C[i][1]) * dz[i];
        jaco[i][2] = (cos(a) * sin(b) * sin(c) * C[i][1] - sin(a) * cos(c) * C[i][1]) * dx[i] + (sin(a) * sin(b) * sin(c) * C[i][1] + cos(a) * cos(c) * C[i][1]) * dy[i] + (cos(b) * sin(c) * C[i][1]) * dz[i];
        jaco[i][3] = -dx[i];
        jaco[i][4] = -dy[i];
        jaco[i][5] = -dz[i];
        //cout << jaco[i][0] << " "<<jaco[i][1]<< " "<< jaco[i][2]<< " "<<jaco[i][3]<< " " << jaco[i][4]<<" " << jaco[i][5]<<endl;

    }
}

void biaoding::shouyan( )
{
    double xyjieguo[8][2] = { 0 };
    //qDebug()<<QCoreApplication::applicationDirPath().replace("release","");
    jiqiren10dian = QCoreApplication::applicationDirPath() + "/example.txt" ;//机器人十点地址   10*16
    chuanganqi = QCoreApplication::applicationDirPath() + "/example1.txt"; //传感器数据地址  8*2
    jieguoshuju =QCoreApplication::applicationDirPath() + "/example2.txt"; //结果保存地址

    double point[10][4][4]; double camera[8][2];
    ifstream infile;
    infile.open(jiqiren10dian.toLatin1().data());
    for (int i = 0; i < 10; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            infile >> point[i][j/4][j%4];
        }
    }
    infile.close();
    infile.open(chuanganqi.toLatin1().data());
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            infile >> camera[i][j];
        }
    }
    infile.close();
    double endni11[4][4] = { 0 }; double endni12[4][4] = { 0 }; double endni13[4][4] = { 0 }; double endni14[4][4] = { 0 }; double endni21[4][4] = { 0 }; double endni22[4][4] = { 0 }; double endni23[4][4] = { 0 }; double endni24[4][4] = { 0 };
    double B11[4][1] = { 0 }; double B12[4][1] = { 0 }; double B13[4][1] = { 0 }; double B14[4][1] = { 0 }; double B21[4][1] = { 0 }; double B22[4][1] = { 0 }; double B23[4][1] = { 0 }; double B24[4][1] = { 0 };
    LUni(point[2], endni11); LUni(point[3], endni21); LUni(point[4], endni12); LUni(point[5], endni22); LUni(point[6], endni13); LUni(point[7], endni23); LUni(point[8], endni14); LUni(point[9], endni24);
    double point11[4][1] = { point[0][0][3],point[0][1][3],point[0][2][3],1 }; double point12[4][1] = { point[1][0][3],point[1][1][3],point[1][2][3],1 };
    MatrixMul1(endni11, point11, B11); MatrixMul1(endni21, point12, B21); MatrixMul1(endni12, point11, B12); MatrixMul1(endni22, point12, B22); MatrixMul1(endni13, point11, B13); MatrixMul1(endni23, point12, B23); MatrixMul1(endni14, point11, B14); MatrixMul1(endni24, point12, B24);
    double D1[8][1] = { B11[0][0],B12[0][0],B13[0][0],B14[0][0],B21[0][0],B22[0][0],B23[0][0],B24[0][0] }; double D2[8][1] = { B11[1][0],B12[1][0],B13[1][0],B14[1][0],B21[1][0],B22[1][0],B23[1][0],B24[1][0] }; double D3[8][1] = { B11[2][0],B12[2][0],B13[2][0],B14[2][0],B21[2][0],B22[2][0],B23[2][0],B24[2][0] };
    double C[8][3] = { camera[0][0],camera[0][1],1,camera[1][0],camera[1][1],1,camera[2][0],camera[2][1],1,camera[3][0],camera[3][1],1 ,camera[4][0],camera[4][1],1,camera[5][0],camera[5][1],1,camera[6][0],camera[6][1],1,camera[7][0],camera[7][1],1 };
    double Cni[3][8] = {};
    double jieguo1[3][1] = { 0 }; double jieguo2[3][1] = { 0 }; double jieguo3[3][1] = { 0 };
    SVDC(C, Cni);

    MatrixMulbuchang(Cni, D1, jieguo1); MatrixMulbuchang(Cni, D2, jieguo2); MatrixMulbuchang(Cni, D3, jieguo3);
    double a = pi / 2; double b = 0; double c = pi / 4;double px = jieguo1[2][0];double py = jieguo2[2][0];double pz = jieguo3[2][0];
    double pandingzhi = 1000;
    int count = 0;
    while (1)
    {
        double dx[8] = { 0 }; double dy[8] = { 0 }; double dz[8] = { 0 };
        double jaco[8][6] = { 0 }; double wucha[8][1] = { 0 }; double jaconi[6][8] = { 0 }; double diedaizhi[6][1] = { 0 };
        dxdydz(D1, D2, D3, a, b, c, C, px, py, pz, dx, dy, dz);
        j1j2j3j4j5j6(D1, D2, D3, a, b, c, C, px, py, pz, dx, dy, dz, jaco);
        for (int i = 0; i < 8; i++)
        {
            wucha[i][0] = (dx[i] * dx[i] + dy[i] * dy[i] + dz[i] * dz[i]) / 2;
        }

        SVDjaco(jaco, jaconi);

        for (int i = 0; i < 6; i++)
            for (int j = 0; j < 1; j++)
            {
                for (int k = 0; k < 8; k++)
                {
                    diedaizhi[i][j] = diedaizhi[i][j] + jaconi[i][k] * wucha[k][j];
                }
            }
        a = a - diedaizhi[0][0]; b = b - diedaizhi[1][0]; c = c - diedaizhi[2][0]; px = px - diedaizhi[3][0]; py = py - diedaizhi[4][0]; pz = pz - diedaizhi[5][0];
        pandingzhi = (wucha[0][0] + wucha[1][0] + wucha[2][0] + wucha[3][0] + wucha[4][0] + wucha[5][0] + wucha[6][0] + wucha[7][0] ) / 8;
        count = count + 1;
        if (pandingzhi - 0.75 < 0)
        {

            shouyanjuzhen[0][0] = cos(a) * cos(b);
            shouyanjuzhen[0][1] = cos(a) * sin(b) * sin(c) - sin(a) * cos(c);
            shouyanjuzhen[0][2] = cos(a) * sin(b) * cos(c) + sin(a) * sin(c);
            shouyanjuzhen[0][3] = px;
            shouyanjuzhen[1][0] = sin(a) * cos(b);
            shouyanjuzhen[1][1] = sin(a) * sin(b) * sin(c) + cos(a) * cos(c);
            shouyanjuzhen[1][2] = sin(a) * sin(b) * cos(c) - cos(a) * sin(c);
            shouyanjuzhen[1][3] = py;
            shouyanjuzhen[2][0] = -sin(b);
            shouyanjuzhen[2][1] = cos(b) * sin(c);
            shouyanjuzhen[2][2] = cos(b) * cos(c);
            shouyanjuzhen[2][3] = pz;
            shouyanjuzhen[3][3] = 1;
            ofstream outfile(jieguoshuju.toLatin1().data(), ios::out);
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    outfile <<shouyanjuzhen[i][j] << " ";
                }
                //outfile << endl;
            }
            outfile.close();
            //cout << "数据写入成功";
            break;
        }
        else if (count > 100)
        {
            //cout << "请重新测量";
            break;
        }
    }

//    shouyanjuzhen[0][0] = cos(a) * cos(b);
//    shouyanjuzhen[0][1] = cos(a) * sin(b) * sin(c) - sin(a) * cos(c);
//    shouyanjuzhen[0][2] = cos(a) * sin(b) * cos(c) + sin(a) * sin(c);
//    shouyanjuzhen[0][3] = px;
//    shouyanjuzhen[1][0] = sin(a) * cos(b);
//    shouyanjuzhen[1][1] = sin(a) * sin(b) * sin(c) + cos(a) * cos(c);
//    shouyanjuzhen[1][2] = sin(a) * sin(b) * cos(c) - cos(a) * sin(c);
//    shouyanjuzhen[1][3] = py;
//    shouyanjuzhen[2][0] = -sin(b);
//    shouyanjuzhen[2][1] = cos(b) * sin(c);
//    shouyanjuzhen[2][2] = cos(b) * cos(c);
//    shouyanjuzhen[2][3] = pz;
//    shouyanjuzhen[3][3] = 1;

//    for (int i = 0; i < 4; i++)
//    {
//        for (int j = 0; j < 4; j++)
//        {
//            cout <<shouyanjuzhen[i][j] << "   ";
//        }
//        cout << endl;
//    }

}

