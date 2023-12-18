#pragma once
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

//2021.12.27 WU YZ
#include <string>
#include <list>
#include <vector>
#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveContext.hxx>
#include "DocumentCommon.h"
#include "cyberInfo.h"

using namespace std;
class Connector;

enum Kinematic_pair_type
{
	rotate_pair = 1,    //转动副
	translate_pair = 2, //移动副
	sphere_pair = 3      //球副
};
struct ConnectConstraint
{
	Kinematic_pair_type kpType; //运动副约束类型
	string component1; //第一个物体
	string component2; //第二个物体
	string connector1; //第一个物体上的接口名字
	string connector2; //第二个物体上接口名字
};

enum ConnectCurveType
{
	single = 1,
	control = 2,
	controlBus = 3
};
struct ConnectCurve
{
	ConnectCurveType ccType; //连接线类型：单线，控制线，总控线，。。。
	string component1; //第一个物体
	string component2; //第二个物体
	string connector1; //第一个物体上的接口名字
	string connector2; //第二个物体上接口名字
	list<gp_Pnt> lstPoints;// 中间控制点，用B样条曲线
	TopoDS_Shape shape; //几何
};

struct ParameterValue
{
	string sParamName; //当前组件的参数名，不带点；可以为空
	string sMapParamName; //映射的子组件全名，有且仅只带一个点
	string sParamValue; //用字符串，因为可能是数组，可能是bool，甚至字符串
};

//组件基本信息
struct CompBaseInfo
{
	string sCompName;                    //组件名
	string sCompFile;                    //组件的文件名
	vector<double> v18Transf;    //组件相对于父组件的变换矩阵
};

//组件属性
class ComponentAttrib
{
public:
	ComponentAttrib();

	//variablity
	int iVariablity; //0-缺省；1-Constant; 2-parameter; 3-Discrete
	//Casuality
	int iCasuality; //0-None; 1-Input; 2-Output
	//ConnectorMember
	int iConnectorMember;//0-常规; 1-flow; 2-stream
	//Properties
	bool bFinal;
	bool bProtected;
	bool bReplaceable;
	//Inner/Outer
	bool bInner;
	bool bOuter;
};
///
/// 组件对象，包含零件和部件: 信息来自DocumentCommon，逻辑上的组合体
/// 
/// 负责功能：显示、调姿、生成代码
class Component
{
public:
	//显示，显示组件的显示体list(AIS_InteractiveObject)
	void Display(Handle(AIS_InteractiveContext) theContext, gp_Trsf delTrans);
	void Display(Handle(AIS_InteractiveContext) theContext, bool bRegen=true);
	
	//调姿，设置拓扑的变换矩阵（文档操作），同时对组件显示体进行变换
	void Transform(gp_Trsf theTransform);

	//根据组件的层次关系，生成Modelica代码，相应的信息，从Document获取
	bool GenerateMoCode(string & sCode, int nLevel=0);

private:
	DocumentCommon* m_pDoc;

public:
	Component(DocumentCommon* pDoc, Component* parent = nullptr);
	~Component();

private:
	Component* m_parent;
	list<Component*> m_lstChildren;  
	string m_sCompName;               //组件简名，根据零件名加，如”cam1”，从1开始编号

	//子组件信息，从读第一层获得，由当前的装配文件读取
	//map<string, CompInfo> m_mapCompInfo;
	//该组件在父组件中的变换矩阵，即对所有子组件都做此变换；当父组件为空，此为单位阵
	//vector<double> m_transform;
	TopoDS_Shape m_shpCompound; //在创建组件组合体时才有 generateCompound
	////组件的全部可视化AIS_InteractiveObject,包括主实体，接口，参考线，坐标系等
	//NCollection_Vector<Handle(AIS_InteractiveObject)> m_theAISObjects;
	//gp_Trsf m_DeltaTransform;

public:
	//map<string, CompInfo> GetComponentsInfo() const { return m_mapCompInfo; }
	//得到组件的实体compoundSolid
	TopoDS_Shape GetCompound() const { return m_shpCompound; }
	TopoDS_Shape GenerateCompound(gp_Trsf theTransf);
	TopoDS_Shape GenerateCompound1(vector<ShapeMaterial>& shpMats,vector<ShapeTransparency>& shpTransparencys, vector<ShapeColor>& shpColors,vector<TopoDS_Shape>& shpDatumPts, vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectos,vector<gp_Ax3>& coordSys);
	TopoDS_Shape GenerateColoredCompound(gp_Trsf transf, vector<ShapeTransparency>& shpTransparencys, vector<ShapeColor>& shpColors, vector<ShapeTexture>& shpTextures, vector<TopoDS_Shape>& shpDatumPts, vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectos, vector<gp_Ax3>& coordSys, vector<TopoDS_Shape>& shpMarkSurf);

	//创建组件的显示组合
	void GenerateAISObjects(gp_Trsf theTransform, vector<Component*>& vComponents);
	//得到全部子孙
	void GetAllDescendants(vector<Component*>& vComps);

	//增加一个子节点
	void AddChild(Component* pCom);
	//设置组件名称
	void SetCompName(const char* name);
	string GetCompName() const { return m_sCompName; }
	string GetCompFullName() const; //得到组件的全名

	//父节点
	void SetParent(Component* pCom) { m_parent = pCom; }
	Component* GetParent() const { return m_parent; }
	list<Component*> GetChilds() const { return m_lstChildren; }
	Component* GetChild(const char* sChild);
	void DeleteChild(const char* sChild);

	void SetDocument(DocumentCommon* pDoc) { m_pDoc = pDoc; }
	DocumentCommon* GetDocument() const { return m_pDoc; }

	//下面均从Document间接获得或者设置
	gp_Trsf GetOriginTransform() const; //原始变换
	gp_Trsf GetDeltaTransform() const;	//得到相对位姿
	void SetOriginTransform(gp_Trsf tras);
	void SetDeltaTransform(vector<double>& v15 /*gp_Trsf trf*/);

	QString GetCompFile() const;
	//CyberInfo
	CyberInfo* GetCyberInfo() const;
	Connector* GetConnector(const TopoDS_Shape& shp) const;

	//零件时
	bool IsConnector() const;
	Connector* GetMainConnector() const;
	bool IsWorld() const;

	//////
	bool IsAssembly() const;

	//变形体
	bool IsDeformable() const;

	//增加显示列表
private:
	vector<Handle(AIS_InteractiveObject)> m_vAIS;
	vector<Connector*> m_vConnectors;
public:
	vector<Handle(AIS_InteractiveObject)> GetAIS() const { return m_vAIS; }
	void SetAIS(vector<Handle(AIS_InteractiveObject)> v0) { m_vAIS = v0; }
	vector<Connector*> GetConnectors() const { return m_vConnectors; }
	void AddConnector(Connector* pConn) { m_vConnectors.push_back(pConn);}
	void DeleteAllConnectors();
};
