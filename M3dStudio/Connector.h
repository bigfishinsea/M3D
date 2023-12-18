#pragma once
#include "M3d.h"
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

//物性信息结构体,10个元素
struct myPhysicalProperty
{
	double mass;                           //质量
	vector<double> centric;                //质心 [3]
	vector<double> inertia;       //惯量 [1,1],[2,1],[2,2],[3,1],[3,2],[3,3]
};

//参考点定义类型
enum myDatumPointType
{
	VERTEX_OFFSET = 1, //点偏移
	FACE_CENTER = 2, //面中心
	INTER_POINT = 3, //线与面的交点
	MID_POINT = 4, //任选两个点，得到中点
	CENTRIC_POINT = 5,//质心：Solid
	PROJECT_POINT = 6 //投影点：第一个Vertex，第二个面
};

//参考轴定义类型
enum myDatumLineType
{
	EDGE_OFFSET = 1,//轴偏移 X,Y,Z
	THROUGH2POINTS = 2,//经过两点
	CYLINDER_AXIS = 3,//圆柱面
	FACES_INTER = 4, //面交线
	FACE_PERPEND = 5//面垂直(选择一个点和一个面，得到经过点的面的垂线)
};

//标记面定义类型
enum myMarkingSurfaceType
{
	SPHERE = 1, // 球面
};

//参考点
class DatumPoint
{
private:
	myDatumPointType m_dptType; //定义方式
	string m_shape1_Name;
	string m_shape2_Name;
	vector<double> m_offVal;  //可能不用
	TopoDS_Shape m_shape;

public:
	DatumPoint() :m_dptType(VERTEX_OFFSET), m_shape1_Name(""), m_shape2_Name(""), m_offVal({ 0.0,0.0,0.0 }) {};
	myDatumPointType GetDptType() const { return m_dptType; }
	void SetDptType(myDatumPointType typ0) { m_dptType = typ0; }
	string GetShape1Name() const { return m_shape1_Name; }
	void SetShape1Name(const char* s) { m_shape1_Name = s; }
	string GetShape2Name() const { return m_shape2_Name; }
	void SetShape2Name(const char* s) { m_shape2_Name = s; }
	vector<double> GetOffsetValue() const { return m_offVal; }
	void SetOffsetValue(vector<double>& val) { m_offVal = val; }
	//创建形状
	//TopoDS_Shape Generate();
	void SetShape(TopoDS_Shape shp) { m_shape = shp; }
	TopoDS_Shape GetShape() const { return m_shape; }
};

//参考轴,参考线
class DatumLine
{
private:
	myDatumLineType m_dlnType;  //定义类型
	string m_shape1_Name;
	string m_shape2_Name;
	vector<double> m_offVal;   //偏移值可能不用
	TopoDS_Shape m_shape;

public:
	DatumLine() :m_dlnType(EDGE_OFFSET), m_shape1_Name(""), m_shape2_Name(""), m_offVal({ 0.0,0.0,0.0 }) {};
	myDatumLineType GetDlnType() const { return m_dlnType; }
	void SetDlnType(myDatumLineType typ0) { m_dlnType = typ0; }
	string GetShape1Name() const { return m_shape1_Name; }
	void SetShape1Name(const char* s) { m_shape1_Name = s; }
	string GetShape2Name() const { return m_shape2_Name; }
	void SetShape2Name(const char* s) { m_shape2_Name = s; }
	vector<double> GetOffsetValue() const { return m_offVal; }
	void SetOffsetValue(vector<double>& val) { m_offVal = val; }
	//创建形状
	//TopoDS_Shape Generate();
	void SetShape(TopoDS_Shape shp) { m_shape = shp; }
	TopoDS_Shape GetShape() const { return m_shape; }
};

//标记面
class MarkingSurface
{
private:
	myMarkingSurfaceType m_msurfType; //定义类型
	TopoDS_Shape m_shape;
	vector<double> m_offVal;  //可能不用
	double m_radius; // 半径
public:
	MarkingSurface() : m_msurfType(SPHERE), m_offVal({ 0.0,0.0,0.0 }), m_radius(0.0) {};
	myMarkingSurfaceType GetMsurfType() const { return m_msurfType; }
	void SetMsurfType(myMarkingSurfaceType typ0) { m_msurfType = typ0; }
	vector<double> GetOffsetValue() const { return m_offVal; }
	void SetOffsetValue(vector<double>& val) { m_offVal = val; }
	double GetRadius() const { return m_radius; }
	void SetRadius(double val) { m_radius = val; }
	void SetShape(TopoDS_Shape shp) { m_shape = shp; }
	TopoDS_Shape GetShape() const { return m_shape; }
};

//接口定义类型
enum myConnectDefineType
{
	//通过坐标原点和方位
	THREE_POINTS = 1,//三个点定义原点和坐标系
	TWO_EDGES = 2,    //两条线定义原点和坐标系

	//通过原点和x轴定义，YOZ平面缺省方位
	POINT_X_AXIS = 3, //第一个是原点，第二个实体是X轴方向

	//通过原点定义，坐标轴随意
	ONE_POINT = 4
};

//接口结构体
class Connector
{
private:
	//位姿（相对于用户坐标系）,9个实数
	vector<double> m_displacement;
	TopoDS_Shape m_shape;

private:
	string m_sConnName;          //接口名字；
	string m_sConnType;          //接口类型,class全名，由前缀判断其所属类别：机械、电子电气、液压、控制、热等
	myConnectDefineType    m_defType;   //定义类型
	vector<string>         m_shapeRefs;  //引用的实体ID //最少一个，最多3个
	bool m_bZ_Reversed;                  //Z轴反向

public:
	Connector();
	Connector(string& sType);
	~Connector();

	void SetShape(TopoDS_Shape shp) { m_shape = shp; }
	TopoDS_Shape GetShape() const { return m_shape; }
	//TopoDS_Shape Generate(gp_Ax3& A3, double& lenth, double& arrowLen);
	void SetZ_Reverse(bool bRev = true) { m_bZ_Reversed = bRev; }
	bool IsZ_Reverse() const { return m_bZ_Reversed; }

	void SetDisplacement(vector<double>& v9) { m_displacement = v9; }
	vector<double> GetDisplacement() const { return m_displacement; }
	//获得中心
	gp_Pnt GetCenter() const;

	void SetConnName(const char* name) { m_sConnName = name; }
	string GetConnName() const { return m_sConnName; }
	//接口类型type：Modelica全名
	void SetConnType(string sType);
	string GetConnType() const;
	//定义类型
	void SetDefineType(myConnectDefineType vType);
	myConnectDefineType GetDefineType() const;
	//引用实体
	void SetRefEnts(vector<string>& ents);
	vector<string> GetRefEnts() const;
	////得到位姿
	//void SetPosOrient(vector<double>& v6);
	//vector<double> GetPosOrent() const;
};
