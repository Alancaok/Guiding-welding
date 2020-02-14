#ifndef CALCULATEXY_H
#define CALCULATEXY_H
#include<QString>
using namespace std;

class calculatexy
{
public:
    calculatexy();
    void piliangxy();

    QString jiqirenndian ;  //"D:/qtcode/biaoding/jiqiren.txt" //机器人n点地址   n*16
    QString chuanganqindian; //"D:/qtcode/biaoding/chuangan.txt" //传感器n点数据地址  n*2
    QString shouyanjuzhenjieguo; //"D:/qtcode/biaoding/shouyanjuzhen.txt" //手眼矩阵地址
    QString yonghuzuobiao; //"D:/qtcode/biaoding/oxy.txt" //用户坐标系地址
    QString zoudian; //"D:/qtcode/biaoding/zoudian.txt" //最终xy结果
};

#endif // CALCULATEXY_H
