#pragma once
#include <string>
#include <vector>
#include <TCollection_ExtendedString.hxx>
#include <gp_trsf.hxx>
#include <QString>

using namespace std;

//由TCollection的字符串转换成标准字符串
string ExtString2string(TCollection_ExtendedString source);

//9个实型数构造一个Ax3
gp_Ax3 MakeAx3(vector<double> vDisp);

//根据实型数组18维度，转换成一个变换矩阵
gp_Trsf MakeTransform(vector<double>& vDisp);

gp_Trsf MakeTransFromV12(vector<double>& v12); //12dim

gp_Trsf MakeTransFromV15(vector<double>& v15);

//从两个坐标系统得到V18的实数
vector<double> GetTransformValues18(gp_Ax3 sysFrom, gp_Ax3 sysTo);

vector<double> GetTransformValues12(gp_Trsf transf);

//接口名字操作几个函数
string Make1Name(string& c1, string& c2, string& conn1, string& conn2);
void Make4Names(string& sFullName, string& c1, string& c2, string& conn1, string& conn2);
void Make2Names(string& sFullName, string& s1, string& s2);

//由字符串获得点
bool GetPoint(const QString& sPnt, gp_Pnt& pnt);
QString GetPointString(gp_Pnt& pnt);
QString GetPointString(gp_Dir& pnt);
QString GetVectorString(gp_Vec& pnt);
QString DoubleToQString(double f1);

//由字符串获得方向
bool GetDirection(QString& sPnt, gp_Dir& dir0);

//判断一个字符串是否为合法文件名（或组件名）
bool IsLegalName(QString name);

