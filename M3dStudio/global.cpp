
#include "global.h"
#include "m3d.h"
#include "gp_Ax3.hxx"
#include "gp_Trsf.hxx"
#include <QRegExp>

string ExtString2string(TCollection_ExtendedString source)
{
	string sRet;
	//QString qsRet;
	//Standard_PCharacter c_string = new Standard_Character[source.LengthOfCString()];
	char* c_string = new char[source.LengthOfCString()+1];
	source.ToUTF8CString(c_string);
	//qsRet = c_string;
	//sRet = qsRet.toStdString();
	sRet = c_string;
		
	delete c_string;

	return sRet;
}

gp_Ax3 MakeAx3(vector<double> vDisp) //9dim
{
	gp_Pnt P1(vDisp[0], vDisp[1], vDisp[2]);
	gp_Dir N1(vDisp[3], vDisp[4], vDisp[5]);
	gp_Dir Vx1(vDisp[6], vDisp[7], vDisp[8]);
	gp_Ax3 coordSys(P1, N1, Vx1);

	return coordSys;
}

//v15
//从Ax3(9), xyz rxryrz，创建变换矩阵
//先平移后旋转
gp_Trsf MakeTransFromV15(vector<double>& v15) //15dim
{
	gp_Trsf trsf;

	if (v15.size() < 15)
	{
		assert(false);
		return trsf;
	}
	if (fabs(v15[3]) < LINEAR_TOL && fabs(v15[4]) < LINEAR_TOL && fabs(v15[5]) < LINEAR_TOL)
	{
		trsf.SetValues(v15[0], v15[1], v15[2], v15[6], v15[7], v15[8], v15[9], v15[10], v15[11], v15[12], v15[13], v15[14]);
		return trsf;
	}
	//先从局部坐标系转到全局坐标系
	gp_Ax3 Ax3_1 = MakeAx3(v15);
	gp_Ax3 Ax3_0;
	Ax3_0.SetLocation(Ax3_1.Location());
	gp_Trsf tr0;
	tr0.SetTransformation(Ax3_1, Ax3_0);

	//再进行平移运动
	gp_Trsf trXYZ;
	gp_Pnt p0(0,0,0);
	gp_Pnt p1(v15[9], v15[10], v15[11]);
	p1.Transform(tr0);
	trXYZ.SetTranslation(p0,p1);

	//再进行旋转，顺序x,y,z轴
	gp_Trsf rotX;
	gp_Ax1 Axis_X(Ax3_1.Location(),Ax3_1.XDirection());//Axis_X(gp_Pnt(0, 0, 0), gp_Vec(1, 0, 0));
	rotX.SetRotation(Axis_X, v15[12]);
	gp_Trsf rotY;
	gp_Dir yDir = Ax3_1.XDirection().Crossed(Ax3_1.Direction());
	gp_Ax1 Axis_Y(Ax3_1.Location(), yDir);// (gp_Pnt(0, 0, 0), gp_Vec(0, 1, 0));
	rotY.SetRotation(Axis_Y, v15[13]);
	gp_Trsf rotZ;
	gp_Ax1 Axis_Z(Ax3_1.Location(), Ax3_1.Direction());// (gp_Pnt(0, 0, 0), gp_Vec(0, 0, 1));
	rotZ.SetRotation(Axis_Z, v15[14]);

	////还原到原位置：开始的逆变换
	//gp_Trsf tr1;
	//tr1.SetTransformation(Ax3_1, Ax3_0);

	trsf = rotZ* rotY* rotX*trXYZ; 

	return trsf;
}

gp_Trsf MakeTransFromV12(vector<double>& v12) //12dim
{
	gp_Trsf trsf;

	trsf.SetValues(v12[0], v12[1], v12[2], v12[3], \
		v12[4], v12[5], v12[6], v12[7], \
		v12[8], v12[9], v12[10], v12[11]);

	return trsf;
}

gp_Trsf MakeTransform(vector<double>& vDisp) //18dim
{
	gp_Trsf trsf;

	gp_Pnt P1(vDisp[0], vDisp[1], vDisp[2]);
	gp_Dir N1(vDisp[3], vDisp[4], vDisp[5]);
	gp_Dir Vx1(vDisp[6], vDisp[7], vDisp[8]);
	gp_Ax3 fromSys(P1,N1,Vx1);

	gp_Pnt P2(vDisp[9], vDisp[10], vDisp[11]);
	gp_Dir N2(vDisp[12], vDisp[13], vDisp[14]);
	gp_Dir Vx2(vDisp[15], vDisp[16], vDisp[17]);
	gp_Ax3 toSys(P2, N2, Vx2);

	trsf.SetDisplacement(fromSys, toSys);

	return trsf;
}

//V12
vector<double> GetTransformValues12(gp_Trsf transf)
{
	vector<double> v12;

	//3 rows 4 cols
	for (int i = 1; i <= 3; i++)
	{
		for (int j = 1; j <= 4; j++)
		{
			v12.push_back(transf.Value(i, j));
		}
	}

	return v12;
}


//V18
vector<double> GetTransformValues18(gp_Ax3 sysFrom, gp_Ax3 sysTo)
{
	vector<double> v18;

	gp_Pnt p0 = sysFrom.Location();
	v18.push_back(p0.X()); v18.push_back(p0.Y()); v18.push_back(p0.Z());

	gp_Dir d0 = sysFrom.Direction();
	v18.push_back(d0.X()); v18.push_back(d0.Y()); v18.push_back(d0.Z());

	gp_Dir d1 =	sysFrom.XDirection();
	v18.push_back(d1.X()); v18.push_back(d1.Y()); v18.push_back(d1.Z());

	{
		gp_Pnt p0 = sysTo.Location();
		v18.push_back(p0.X()); v18.push_back(p0.Y()); v18.push_back(p0.Z());

		gp_Dir d0 = sysTo.Direction();
		v18.push_back(d0.X()); v18.push_back(d0.Y()); v18.push_back(d0.Z());

		gp_Dir d1 = sysTo.XDirection();
		v18.push_back(d1.X()); v18.push_back(d1.Y()); v18.push_back(d1.Z());
	}

	return v18;
}

//由组件的名字和接口名字合成一个形如 A1.p1-A2.p1的名字
string Make1Name(string& c1, string& c2, string& conn1, string& conn2)
{
	string ccName = c1;
	ccName += ".";
	ccName += conn1;
	ccName += "-";
	ccName += c2;
	ccName += ".";
	ccName += conn2;

	return ccName;
}

//将连接线的全名分割成4个子名字:A1, A2, p1, p2
void Make4Names(string& sFullName, string& c1, string& c2, string& conn1, string& conn2)
{
	size_t pos0 = sFullName.find('.', 0);
	c1 = sFullName.substr(0, pos0);
	size_t pos1 = sFullName.find('-', 0);
	conn1 = sFullName.substr(pos0 + 1, pos1 - pos0-1);
	size_t pos2 = sFullName.find('.', pos1+1);
	c2 = sFullName.substr(pos1 + 1, pos2 - pos1 - 1);
	conn2 = sFullName.substr(pos2 + 1, sFullName.length() - pos2-1);
}

//将连接线的全名分割成2个子名字：A1.p1 和 A2.p1
void Make2Names(string& sFullName, string& s1, string& s2)
{
	string c1, c2, conn1, conn2;
	Make4Names(sFullName, c1, c2, conn1, conn2);

	s1 = c1 + "." + conn1;
	s2 = c2 + "." + conn2;
}

//由字符串获得点
bool GetPoint(const QString& sPnt, gp_Pnt& pnt)
{
	int nPos = sPnt.indexOf("(");
	if (nPos == -1) return false;

	QString sRight = sPnt.right(sPnt.length() - nPos - 1);
	nPos = sRight.indexOf(",");
	if(nPos == -1) return false;
	QString sLeft = sRight.left(nPos);
	double X = sLeft.toDouble();
	sRight = sRight.right(sRight.length() - nPos - 1);

	nPos = sRight.indexOf(",");
	if (nPos == -1) return false;
	sLeft = sRight.left(nPos);
	double Y = sLeft.toDouble();
	sRight = sRight.right(sRight.length() - nPos - 1);

	nPos = sRight.indexOf(")");
	if (nPos == -1) return false;
	sLeft = sRight.left(nPos);
	double Z = sLeft.toDouble();

	pnt = gp_Pnt(X, Y, Z);

	return true;
}

QString GetVectorString(gp_Vec& pnt)
{
	QString rStr;
	rStr = "{";
	rStr += DoubleToQString(pnt.X());
	rStr += ",";
	rStr += DoubleToQString(pnt.Y());
	rStr += ",";
	rStr += DoubleToQString(pnt.Z());
	rStr += "}";

	return rStr;
}

QString GetPointString(gp_Pnt& pnt)
{
	QString rStr;
	rStr = "(";
	rStr += DoubleToQString(pnt.X());
	rStr += ",";
	rStr += DoubleToQString(pnt.Y());
	rStr += ",";
	rStr += DoubleToQString(pnt.Z());
	rStr += ")";

	return rStr;
}

QString GetPointString(gp_Dir& pnt)
{
	QString rStr;
	rStr = "(";
	rStr += DoubleToQString(pnt.X());
	rStr += ",";
	rStr += DoubleToQString(pnt.Y());
	rStr += ",";
	rStr += DoubleToQString(pnt.Z());
	rStr += ")";

	return rStr;
}

QString DoubleToQString(double f1)
{
	QString sret;
	sret = sret.number(f1, 'g',3); //科学计数值

	return sret;
}

//由字符串获得方向
bool GetDirection(QString& sPnt, gp_Dir& dir0)
{
	gp_Pnt pnt;
	bool b0 = GetPoint(sPnt, pnt);
	if (!b0) return false;

	dir0 = gp_Dir(pnt.X(), pnt.Y(), pnt.Z());

	return true;
}

bool IsLegalName(QString text)
{
	if (text.isEmpty())
	{
		return false;
	}

	//不能重命名为world
	if (text == "world")
	{
		return false;
	}

	bool is_legal = true;

	QString pattern("[\\\\/:*?\"<>]");
	QRegExp rx(pattern);
	int match = text.indexOf(rx);
	if (match >= 0)
	{
		is_legal = false;
	}

	return is_legal;
}
