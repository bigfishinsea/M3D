//2021.11.09 WU YZ
#pragma once
#include <string>
#include <list>
#include <map>
#include <vector>
#include <TDF_Label.hxx>
#include <TopoDS.hxx>
#include <AIS_InteractiveContext.hxx>
#include "M3dCom_DS.h"
//#include "M3d_DS.h"

using namespace std;

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
	ParameterValue() {};
	ParameterValue(string m_mapname, string m_paval="", string m_paname="") :
		sParamName(m_paname), sMapParamName(m_mapname), sParamValue(m_paval){};
};

struct CompInfo
{
	string sCompName;
	string sCompFile;
	vector<double> vCompDisplacement;
};

//组件属性
struct ComponentAttrib
{
	//variablity
	int iVariablity; //0-缺省；1-Constant; 2-parameter; 3-Discrete
	//Casuality
	int iCasuality; //0-None; 1-Input; 2-Output
	//Properties
	bool bFinal;
	bool bProtected;
	bool bReplaceable;
	//Inner/Outer
	bool bInner;
	bool bOuter;
};
///
/// 组件对象，包含零件和部件
/// 
class Component
{
public:
	Component(Component* parent=nullptr);
	~Component();

private:
	bool m_IsAssembly; //是否装配体 不存储
	ComponentAttrib m_CompAttr; //组件属性
	Component* m_parent;
	list<Component*> m_lstChildren;  //嵌套组件，不存储

	string m_sCompName;               //组件名，根据零件名加，如”cam_1”，从1开始编号
	string m_sFileName;                //组件或零件的文件名，*.m3dcom或*.m3dmdl
	
	//子组件信息，从读第一层获得，由当前的装配文件读取
	map<string, CompInfo> m_mapCompInfo;
									   //point m_position;                  //原点位置，在当前模型中的坐标
	//orientation m_orient;               //姿态，可以与位置合并，transform
	vector<double> m_transform;           //18个实数表示的变换矩阵：From,ToSystem的P,N,Vx的三个矢量
	
	vector<ConnectConstraint> m_lstConnectConstraints; //接口约束，（物理）运动副约束
	vector<ConnectCurve> m_lstConnectCurves; //连接线
	vector<ParameterValue> m_lstParamValues; //参数设置
	//list<myParameter*> lstParameters; //关注的参数列表，不存储
	////几何参数怎么办？可以通过增加 尺寸约束的名字来标注。由于零件的尺寸参数化导致装配体的位姿参数化，求解过程复杂，放在后面考虑吧。。。。。。。
	//list<myOutputVar*> lstVars; //关注的输出变量表，不存储
	//M3dCom_DS* m_pDS ? 还是基类 M3d_DS * ?
	
	//如果是零件，则读出形状
	TopoDS_Shape m_shpCompound;
	//Modelica信息：组件时可能变
	CyberInfo* m_pCyberInfo;
	//材料
	Graphic3d_NameOfMaterial m_material;
	//颜色
	vector<ShapeColor> m_mapShpColor;
	//几何约束
	vector<myConstraint> m_geomConstraints;
	//基准点
	vector<DatumPoint*> m_vDatumPts;
	//基准线
	vector<DatumLine*> m_vDatumLns;
	//接口
	vector<Connector*> m_vConnectors;

private:
	void DeleteCompConnectCurves();
	void DeleteCompConnConstraits();
	void DeleteCompParameterValues();

public:
	void SetMaterial(Graphic3d_NameOfMaterial m0) { m_material = m0; }
	void SetGeomConstraints(vector<myConstraint>& geomConstraints) { m_geomConstraints = geomConstraints; }
	void SetDatumPoints(vector<DatumPoint*>& vDatumPts) { m_vDatumPts = vDatumPts; }
	void SetDatumLines(vector<DatumLine*>& vDatumLns) { m_vDatumLns = vDatumLns; }
	void SetConnectors(vector<Connector*>& vConnectors) { m_vConnectors = vConnectors; }
	void SetShape(TopoDS_Shape theCompound) { m_shpCompound = theCompound; }
	void SetShapeColors(vector<ShapeColor>& mapShpColor) { m_mapShpColor = mapShpColor; }
	void SetComponentsInfo(map<string, CompInfo>& m0) { m_mapCompInfo = m0; }
	map<string, CompInfo> GetComponentsInfo() const { return m_mapCompInfo; }
	Connector* Component::GetConnector(const TopoDS_Shape& shp) const;

public:
	void SetIsAssebly(bool b = true) { m_IsAssembly = b; }
	bool IsAssembly() const { return m_IsAssembly;  }
	//得到组件的实体compoundSolid
	TopoDS_Shape GetCompound() { return m_shpCompound; }
	void SetCompound(const TopoDS_Shape& shp) { m_shpCompound = shp; }
	TopoDS_Shape GenerateCompound();
	TopoDS_Shape GenerateCompound1(vector<ShapeMaterial>& shpMats,
		vector<ShapeTransparency>& shpTransparencys, vector<ShapeColor>& shpColors,
		vector<TopoDS_Shape>& shpDatumPts, vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectos,
		vector<gp_Ax3>& coordSys);
	//增加一个子节点
	void AddChild(Component* pCom);
	//设置组件名称
	void SetCompName(const char* name);
	string GetCompName() const { return m_sCompName; }
	//设置组件文件名
	void SetCompFile(const char* name);
	string GetCompFile() const { return m_sFileName;  }
	//设置组件方位
	void SetDisplacement(vector<double> vDisp) { m_transform = vDisp; }
	vector<double> GetDisplacement() const { return m_transform; }
	//父节点
	void SetParent(Component* pCom) { m_parent = pCom;  }
	Component* GetParent() const { return m_parent;  }
	list<Component*> GetChilds() const { return m_lstChildren; }
	Component* GetChild(const char* sChild);
	void DeleteChild(const char* sChild);
	//CyberInfo
	void SetCyberInfo(CyberInfo* pInfo) { m_pCyberInfo = pInfo; }
	CyberInfo* GetCyberInfo() const { return m_pCyberInfo; }

public:
	void SetCompConnConstraints(vector<ConnectConstraint>& list1) { m_lstConnectConstraints = list1; }
	void SetCompConnections(vector<ConnectCurve>& list2) { m_lstConnectCurves = list2; }
	void SetParamterValues(vector<ParameterValue>& list3) { m_lstParamValues = list3; }

	//显示组件，层次树
	void Display(Handle(AIS_InteractiveContext) context, gp_Trsf Trsf);
	void DisplayAllShapeColors(Handle(AIS_InteractiveContext) context);
};


/// <summary>
/// M3dMdl_DS //模型文档的数据结构
/// </summary>
class M3dMdl_DS : public M3d_DS
{
private:
	void DeleteCompConnectCurves(const char* sCompName);
	void DeleteCompConnConstraits(const char* sCompName);
	void DeleteCompParameterValues(const char* sCompName);

public:
	M3dMdl_DS();
	M3dMdl_DS(TDF_Label & root);
	~M3dMdl_DS();

public:
	//读文件，获得文件的M3dMdl_DS对象
	static M3dMdl_DS* ReadFile(const char* sFileName);
	Component* GenerateTopComponent();
	map<string, CompInfo> GetComponentsInfo()const { return m_mapCompInfo; }

private:
	//Component* m_ComRoot;  //根组件
	
	//组件信息：
	map<string, CompInfo> m_mapCompInfo;
	//list<string> m_componentsNames; //组件名字
	//list<vector<double> >  m_componentsDisplayments; //组件位姿
	//list<string> m_componentsFiles; //组件文件名

	//组件之间的接口约束和连接信息：
	vector<ConnectConstraint> m_lstConnectConstraints; //接口约束
	vector<ConnectCurve> m_lstConnectCurves; //连接线

	//参数设置信息：
	vector<ParameterValue> m_lstParamValues; //参数设置

public:
	virtual string GetShapeName(const TopoDS_Shape& shpSel) const;

public:
	//主要接口函数
	bool InsertComponent(Component* pCom); //插入组件到文档
	//Component* GetComponent(TopoDS_Shape& shape); //得到组件
	//Connector* GetComponentConnector(TopoDS_Shape& shape); //得到接口，只搜索组件的第一层接口，其子孙的接口不搜索；
	void DeleteComponent(Component* pCom); //删除组件

	void AddConnConstraint(ConnectConstraint& cc); //增加接口约束
	void DeleteConnConstraint(ConnectConstraint& cc); //删除接口约束
	void SetConnConstraints(vector<ConnectConstraint> & vConnConstrs); //设置全部接口约束，在接口约束界面“更新”时调用
	vector<ConnectConstraint> GetAllConnConstraints() const;

	void AddConnectCurve(ConnectCurve& pCurve); //增加一条连接线
	ConnectCurve* GetConnectCurve(const TopoDS_Shape& shape) const;//拾取一条连接线
	void DeleteConnectCurve(ConnectCurve& cc); //删除一条连接线
	vector<ConnectCurve> GetAllConnectCurves() const;

	void AddParameterValue(ParameterValue& pv);
	void DeleteParameterValue(string sParamName);
	void SetParameterValues(vector<ParameterValue>& vPVs);
	vector<ParameterValue> GetAllParameterValues() const;

public:
	//查找全部组件的简单信息
	map<string, CompInfo> GetAllCompInfos() const;
	//对组件进行变换
	void SetComponentDisplacement(Component* pCom, vector<double>& v18);

public:
	bool InitCheckModel(); //检查模型，接口的连接与匹配；
	bool GenerateMoFile(string& sFileName); //生成Modelica代码
	bool CompileModel(string& sFileName, string & sModelFullName); //编译模型，生成仿真器
	bool SimulateModel(string& sModelFullName, string& sResFileName); //仿真运行,得到结果文件
	//由结果文件获得变量的时间序列值
	vector<vector<double> > GetVariableTimeSeries(const char* sResFileName, const char* sVarName); 
	//从结果文件获得组件的位姿信息，时间序列：每个时刻对应的 ToSystem gn_Ax3
	vector<vector<double> > GetComponentDisplacement(const char* sResFileName, const char* sCompName);
};
