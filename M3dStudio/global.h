#pragma once
#include <string>
#include <vector>
#include <TCollection_ExtendedString.hxx>
#include <gp_trsf.hxx>
#include <QString>

using namespace std;

//��TCollection���ַ���ת���ɱ�׼�ַ���
string ExtString2string(TCollection_ExtendedString source);

//9��ʵ��������һ��Ax3
gp_Ax3 MakeAx3(vector<double> vDisp);

//����ʵ������18ά�ȣ�ת����һ���任����
gp_Trsf MakeTransform(vector<double>& vDisp);

gp_Trsf MakeTransFromV12(vector<double>& v12); //12dim

gp_Trsf MakeTransFromV15(vector<double>& v15);

//����������ϵͳ�õ�V18��ʵ��
vector<double> GetTransformValues18(gp_Ax3 sysFrom, gp_Ax3 sysTo);

vector<double> GetTransformValues12(gp_Trsf transf);

//�ӿ����ֲ�����������
string Make1Name(string& c1, string& c2, string& conn1, string& conn2);
void Make4Names(string& sFullName, string& c1, string& c2, string& conn1, string& conn2);
void Make2Names(string& sFullName, string& s1, string& s2);

//���ַ�����õ�
bool GetPoint(const QString& sPnt, gp_Pnt& pnt);
QString GetPointString(gp_Pnt& pnt);
QString GetPointString(gp_Dir& pnt);
QString GetVectorString(gp_Vec& pnt);
QString DoubleToQString(double f1);

//���ַ�����÷���
bool GetDirection(QString& sPnt, gp_Dir& dir0);

//�ж�һ���ַ����Ƿ�Ϊ�Ϸ��ļ��������������
bool IsLegalName(QString name);

