#pragma once
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include <vector>
#include <string>
#include <list>
#include <map>
#include <hash_map>
#include <TDF_Label.hxx>
#include <TDF_Attribute.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <Quantity_Color.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <AIS_ColoredShape.hxx>
#include "M3d_DS.h"

using namespace std;
using namespace stdext;
class Component;

enum myShapeType
{
	NM_DEFAULT=0,
	NM_VERTEX=1,
	NM_EDGE=2,
	NM_FACE=3,
	NM_SOLID=4,
	NM_CONNECTOR=5,    //接口，用面类型，以属性Name区分
	NM_DATUMPOINT=6, //参考点
	NM_DATUMLINE=7,   //参考线
	NM_DATUMPLANE=8,   //参考面
};

//约束类型,不全需补充...
enum myConstraintType
{
	PARALLEL=1,
	CONCINE=2,
	TAGENT=3,
	DIM_LENGTH=4,
	DIM_ANGLE=5
};

//约束结构体
struct myConstraint
{
	int nIndex; //约束的序号
	myConstraintType consType;  //约束类型
	//vector<int>      shapeID1;  //第一个元素
	string           shapeName1;
	//vector<int>      shapeID2;  //第二个元素
	string           shapeName2;
	double           consValue; //约束值，尺寸约束
	bool             bActive;   //是否起作用
};

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
	VERTEX_OFFSET=1, //点偏移
	FACE_CENTER=2, //面中心
	INTER_POINT=3, //线与面的交点
	MID_POINT=4, //任选两个点，得到中点
	CENTRIC_POINT=5,//质心：Solid
	PROJECT_POINT=6 //投影点：第一个Vertex，第二个面
};

//参考轴定义类型
enum myDatumLineType
{
	EDGE_OFFSET=1,//轴偏移 X,Y,Z
	THROUGH2POINTS=2,//经过两点
	CYLINDER_AXIS=3,//圆柱面
	FACES_INTER=4, //面交线
	FACE_PERPEND=5//面垂直(选择一个点和一个面，得到经过点的面的垂线)
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

//接口定义类型
enum myConnectDefineType
{
	//通过坐标原点和方位
	THREE_POINTS = 1,//三个点定义原点和坐标系
	TWO_EDGES=2,    //两条线定义原点和坐标系

	//通过原点和x轴定义，YOZ平面缺省方位
	POINT_X_AXIS=3, //第一个是原点，第二个实体是X轴方向

	//通过原点定义，坐标轴随意
	ONE_POINT=4
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

//参数
struct myParameter
{
	string sFullName;      //全名
	double defaultValue;  //缺省值
	string sUnit;          //单位
	bool   bFocused;      //是否关注
	string sFocusName;     //关注名
	string sType;			//参数类型
	string sDimension;		//维度 ：空（标量）、[5]、[2,1]或[2,3,4]
};

//输出变量
struct myOutputVar
{
	string sFullName;
	string sUnit;
	bool   bFocused;      //是否关注
	string sFocusName;     //关注名
	//string sType;			//类型
	//string sDimension;		//维度 ：空（标量）、[5]、[2,1]或[2,3,4]
};

//信息类
class CyberInfo
{
public:
	CyberInfo();
	CyberInfo(string& sFullName);
	~CyberInfo();

private:
	//Modelica类全名
	string m_sMdlFullName;
	//参数信息
	vector<myParameter*> m_Parameters;
	//输出变量信息
	vector<myOutputVar*> m_OutputVars;
	//接口类型，类型全名列表
	vector<string> m_vConnetorTypes;
	//接口名字
	vector<string> m_vConnectorNames;
public:
	//根据模型全名，获得Model的全部信息！！！参数、输出变量、接口类型
	bool BuildCyberInfo();

	//Get...
	string GetMdlName() const;
	vector<myParameter*> GetParameters() const;
	vector<myOutputVar*> GetOutputVars() const;
	vector<string>   GetConnectorTypes() const;
	vector<string>   GetConnectorNames() const;
	
	//Set...
	void SetMdlName(string & sFullName);

	//下面几个Set函数测试用...
	void SetParameters(vector<myParameter*> & vPars);
	void SetOutputVars(vector<myOutputVar*> & vVars);
	void SetConncTypes(vector<string> & vTypes);
	void CyberInfo::SetConncNames(vector<string>& vNames);
};

//形状+颜色
struct ShapeColor
{
	TopoDS_Shape theShape;
	Quantity_Color theColor;
};

struct ShapeMaterial
{
	TopoDS_Shape theShape;
	Graphic3d_NameOfMaterial theMaterial;
};

struct ShapeTransparency
{
	TopoDS_Shape theShape;
	double transparent;
};

/// <summary>
/// 零件文档
/// </summary>
class M3dCom_DS : public M3d_DS
{
public:
	M3dCom_DS();
	//用这个构造，pRoot由文档的TDF根节点传来
	M3dCom_DS(TDF_Label lblRoot);
	~M3dCom_DS();

public:
	void SetRoot(TDF_Label pRoot);

	//1.形状操作相关
private:
	//用一个hash表存储形状shape与树节点的Label对应关系，便于快速查找
	//先直接在树上搜索
	//hash_map <int, Handle(TDF_Label)> m_hmShapes;
	//map<Handle(TopoDS_Shape), TDF_Label> m_mapShapes;
private:
	//根据形状类型，增加到相应的树节点上；存储关系表hm
	void AddShape(myShapeType t0, const TopoDS_Shape& aShape);
	//从hm得到Shape的Label
	TDF_Label GetShapeLabelandID(const TopoDS_Shape& theShape, vector<int> & ID) const;
	TDF_Label GetShapeLabel(const TopoDS_Shape& theShape) const;
	//得到TopoDS_Shape的ID ,如[3,3]，先从hashmap得到label,再从树上遍历找到label，获得序号；或直接由Label的tag得到
	vector<int> GetShapeID(const TopoDS_Shape& aShape) const;
	//由ID得到形状
	TopoDS_Shape GetShapeFromID(vector<int>& vID) const;
	TopoDS_Shape GetShapeFromID0(vector<int>& vID) const;
	//更新hashmap的形状表:由文档的内容更新hmShapes，打开文档读取时候;Undo/Redo
	void UpdateShapes();
	//获得全部Shapes，在读文件后，获得全部的TopoDS_Shape用于显示
	vector<TopoDS_Shape> GetAllShapes() const;
public:
	//加入主实体，用于存取物性参数: 零件的主实体只有一个!!!
	void SetMainSolid(const TopoDS_Shape& theShape, Graphic3d_NameOfMaterial mat= Graphic3d_NameOfMaterial::Graphic3d_NameOfMaterial_Steel);
	//清空文档
	void Clear();
	//获得主实体
	TopoDS_Shape GetMainSolid(Graphic3d_NameOfMaterial& mat) const;
	//得到TopoDS_Shape的命名，如“面3”
	virtual string GetShapeName(const TopoDS_Shape& aShape) const;
	//由名字得到形状
	TopoDS_Shape GetShapeFromName(string& theName) const;

	//2.接口存储相关
public:
	//增加接口，在Root下增加子节点存储各接口
	void AddConnector(Connector * pConn);//先Generate()创建形状
	//由形状得到接口的信息；此new Connector，用户根据需要delete,如果该形状不是Connector，则返回null
	Connector* GetConnector(const TopoDS_Shape& connShape);
	//更新已有接口
	bool UpdateConnector(const TopoDS_Shape& connShape, Connector* pCon);
	//删除接口
	bool DeleteConnector(const TopoDS_Shape& connShape);

	//增加\获得\更新、删除参考点、线
	void AddDatumPoint(const TopoDS_Shape& connShape, DatumPoint* pDPt);
	DatumPoint* GetDatumPoint(const TopoDS_Shape& connShape);
	bool UpdateDatumPoint(const TopoDS_Shape& connShape, DatumPoint* pDPt);
	bool DeleteDatumPoint(const TopoDS_Shape& connShape);
	void AddDatumLine(const TopoDS_Shape& connShape, DatumLine* pDPt);
	DatumLine* GetDatumLine(const TopoDS_Shape& connShape);
	bool UpdateDatumLine(const TopoDS_Shape& connShape, DatumLine* pDPt);
	bool DeleteDatumLine(const TopoDS_Shape& connShape);

	//3.颜色存取相关
public:
	//向文档设置某形状的颜色，找到其对应的Label，增加其颜色属性
	void SetShapeColor(const TopoDS_Shape& theShape, Quantity_Color theColor);
	//从文档获得某形状的颜色
	Quantity_Color GetShapeColor(const TopoDS_Shape& theShape) const;
	map<string, vector<double> > GetColorMap();
	
	//4.约束存取相关
private:
	//向文档增加约束，增加相应的TDF_Label子节点
	void AddConstraint(TDF_Label &parent,int nth, myConstraint & pConstraint);
	//从文档删除某个约束,第几个
	bool DeleteConstraint(int theConsID);
	//更新第theID个约束
	bool UpdateConstraint(int theID, myConstraint& pConstraint);
	//得到第几个约束, 返回null，或有值，用户负责删除
	myConstraint* GetConstraint(TDF_Label& parent, int nTh) const;
	//得到第几个约束的两个实体元素
	vector<TopoDS_Shape> GetConstraintShapes(int nTh) const;
	//约束的形状
	//......
public:
	//清除当前的约束，设置新的约束们
	void SetConstraints(vector<myConstraint> & vConstraints);
	//得到全部约束
	vector<myConstraint> GetAllConstraints() const;

	//5.材料和透明度
public:
	//设置零件材料,以整型属性存储到TDF_Label(solid)根
	void SetMaterial(Graphic3d_NameOfMaterial nMaterial);
	//从文档获得材料
	Graphic3d_NameOfMaterial GetMaterial() const;
	//设置透明度,同 材料
	void SetTransparency(double nTransp);
	//获得零件透明度
	double  GetTransparency() const;

	//6.物性信息存取:质心[3]，质量，转动惯量[3X3]/2-----------暂时不存储
private:
	//设置物性参数,存储在根节点,作为Attribute
	void SetPhysicalProperty(myPhysicalProperty& par);
	//从文档获得物性参数
	myPhysicalProperty GetPhysicalProperty() const;
	
	//7.Cyber信息数据存取
private:
	void AddParameter(TDF_Label &parLabel, int nIndex, myParameter *aPar);
	void AddOutputVar(TDF_Label &parLabel, int nIndex, myOutputVar* aVar);
	myParameter * GetParameter(TDF_Label&  parLabel, int nth) const;
	myOutputVar * GetOutputVar(TDF_Label& parLabel, int nth) const;
public:
	//界面Update时，设置Cyber信息到文档
	bool SetCyberInfo(CyberInfo* pCyber);
	//从文档获取Cyber信息,如果失败返回null,此函数new CyberInfo，用户自己delete
	CyberInfo* GetCyberInfo() const;
	//读文件
	static M3dCom_DS* ReadFile(const char* file);
	//创建Component对象
	Component* GenerateComponent();
	vector<DatumPoint*> GetAllDatumPoints() const;
	vector<DatumLine*>  GetAllDatumLines() const;
	vector<Connector*>  GetAllConnectors() const;
	vector<ShapeColor> GetAllShapeColors() const;

	//删除引用了元素sName的对象
	void DeleteRefObjects(const char* sName);

private:
	bool LookForColorLabel(const char * shpName, TDF_Label &theLabel);
};
