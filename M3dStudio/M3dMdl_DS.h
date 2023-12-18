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
	rotate_pair = 1,    //ת����
	translate_pair = 2, //�ƶ���
	sphere_pair = 3      //��
};
struct ConnectConstraint
{
	Kinematic_pair_type kpType; //�˶���Լ������
	string component1; //��һ������
	string component2; //�ڶ�������
	string connector1; //��һ�������ϵĽӿ�����
	string connector2; //�ڶ��������Ͻӿ�����
};

enum ConnectCurveType
{
	single = 1,
	control = 2,
	controlBus = 3
};
struct ConnectCurve
{
	ConnectCurveType ccType; //���������ͣ����ߣ������ߣ��ܿ��ߣ�������
	string component1; //��һ������
	string component2; //�ڶ�������
	string connector1; //��һ�������ϵĽӿ�����
	string connector2; //�ڶ��������Ͻӿ�����
	list<gp_Pnt> lstPoints;// �м���Ƶ㣬��B��������
	TopoDS_Shape shape; //����
};

struct ParameterValue
{
	string sParamName; //��ǰ����Ĳ������������㣻����Ϊ��
	string sMapParamName; //ӳ��������ȫ�������ҽ�ֻ��һ����
	string sParamValue; //���ַ�������Ϊ���������飬������bool�������ַ���
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

//�������
struct ComponentAttrib
{
	//variablity
	int iVariablity; //0-ȱʡ��1-Constant; 2-parameter; 3-Discrete
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
/// ������󣬰�������Ͳ���
/// 
class Component
{
public:
	Component(Component* parent=nullptr);
	~Component();

private:
	bool m_IsAssembly; //�Ƿ�װ���� ���洢
	ComponentAttrib m_CompAttr; //�������
	Component* m_parent;
	list<Component*> m_lstChildren;  //Ƕ����������洢

	string m_sCompName;               //�����������������ӣ��硱cam_1������1��ʼ���
	string m_sFileName;                //�����������ļ�����*.m3dcom��*.m3dmdl
	
	//�������Ϣ���Ӷ���һ���ã��ɵ�ǰ��װ���ļ���ȡ
	map<string, CompInfo> m_mapCompInfo;
									   //point m_position;                  //ԭ��λ�ã��ڵ�ǰģ���е�����
	//orientation m_orient;               //��̬��������λ�úϲ���transform
	vector<double> m_transform;           //18��ʵ����ʾ�ı任����From,ToSystem��P,N,Vx������ʸ��
	
	vector<ConnectConstraint> m_lstConnectConstraints; //�ӿ�Լ�����������˶���Լ��
	vector<ConnectCurve> m_lstConnectCurves; //������
	vector<ParameterValue> m_lstParamValues; //��������
	//list<myParameter*> lstParameters; //��ע�Ĳ����б����洢
	////���β�����ô�죿����ͨ������ �ߴ�Լ������������ע����������ĳߴ����������װ�����λ�˲������������̸��ӣ����ں��濼�ǰɡ�������������
	//list<myOutputVar*> lstVars; //��ע��������������洢
	//M3dCom_DS* m_pDS ? ���ǻ��� M3d_DS * ?
	
	//�����������������״
	TopoDS_Shape m_shpCompound;
	//Modelica��Ϣ�����ʱ���ܱ�
	CyberInfo* m_pCyberInfo;
	//����
	Graphic3d_NameOfMaterial m_material;
	//��ɫ
	vector<ShapeColor> m_mapShpColor;
	//����Լ��
	vector<myConstraint> m_geomConstraints;
	//��׼��
	vector<DatumPoint*> m_vDatumPts;
	//��׼��
	vector<DatumLine*> m_vDatumLns;
	//�ӿ�
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
	//�õ������ʵ��compoundSolid
	TopoDS_Shape GetCompound() { return m_shpCompound; }
	void SetCompound(const TopoDS_Shape& shp) { m_shpCompound = shp; }
	TopoDS_Shape GenerateCompound();
	TopoDS_Shape GenerateCompound1(vector<ShapeMaterial>& shpMats,
		vector<ShapeTransparency>& shpTransparencys, vector<ShapeColor>& shpColors,
		vector<TopoDS_Shape>& shpDatumPts, vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectos,
		vector<gp_Ax3>& coordSys);
	//����һ���ӽڵ�
	void AddChild(Component* pCom);
	//�����������
	void SetCompName(const char* name);
	string GetCompName() const { return m_sCompName; }
	//��������ļ���
	void SetCompFile(const char* name);
	string GetCompFile() const { return m_sFileName;  }
	//���������λ
	void SetDisplacement(vector<double> vDisp) { m_transform = vDisp; }
	vector<double> GetDisplacement() const { return m_transform; }
	//���ڵ�
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

	//��ʾ����������
	void Display(Handle(AIS_InteractiveContext) context, gp_Trsf Trsf);
	void DisplayAllShapeColors(Handle(AIS_InteractiveContext) context);
};


/// <summary>
/// M3dMdl_DS //ģ���ĵ������ݽṹ
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
	//���ļ�������ļ���M3dMdl_DS����
	static M3dMdl_DS* ReadFile(const char* sFileName);
	Component* GenerateTopComponent();
	map<string, CompInfo> GetComponentsInfo()const { return m_mapCompInfo; }

private:
	//Component* m_ComRoot;  //�����
	
	//�����Ϣ��
	map<string, CompInfo> m_mapCompInfo;
	//list<string> m_componentsNames; //�������
	//list<vector<double> >  m_componentsDisplayments; //���λ��
	//list<string> m_componentsFiles; //����ļ���

	//���֮��Ľӿ�Լ����������Ϣ��
	vector<ConnectConstraint> m_lstConnectConstraints; //�ӿ�Լ��
	vector<ConnectCurve> m_lstConnectCurves; //������

	//����������Ϣ��
	vector<ParameterValue> m_lstParamValues; //��������

public:
	virtual string GetShapeName(const TopoDS_Shape& shpSel) const;

public:
	//��Ҫ�ӿں���
	bool InsertComponent(Component* pCom); //����������ĵ�
	//Component* GetComponent(TopoDS_Shape& shape); //�õ����
	//Connector* GetComponentConnector(TopoDS_Shape& shape); //�õ��ӿڣ�ֻ��������ĵ�һ��ӿڣ�������Ľӿڲ�������
	void DeleteComponent(Component* pCom); //ɾ�����

	void AddConnConstraint(ConnectConstraint& cc); //���ӽӿ�Լ��
	void DeleteConnConstraint(ConnectConstraint& cc); //ɾ���ӿ�Լ��
	void SetConnConstraints(vector<ConnectConstraint> & vConnConstrs); //����ȫ���ӿ�Լ�����ڽӿ�Լ�����桰���¡�ʱ����
	vector<ConnectConstraint> GetAllConnConstraints() const;

	void AddConnectCurve(ConnectCurve& pCurve); //����һ��������
	ConnectCurve* GetConnectCurve(const TopoDS_Shape& shape) const;//ʰȡһ��������
	void DeleteConnectCurve(ConnectCurve& cc); //ɾ��һ��������
	vector<ConnectCurve> GetAllConnectCurves() const;

	void AddParameterValue(ParameterValue& pv);
	void DeleteParameterValue(string sParamName);
	void SetParameterValues(vector<ParameterValue>& vPVs);
	vector<ParameterValue> GetAllParameterValues() const;

public:
	//����ȫ������ļ���Ϣ
	map<string, CompInfo> GetAllCompInfos() const;
	//��������б任
	void SetComponentDisplacement(Component* pCom, vector<double>& v18);

public:
	bool InitCheckModel(); //���ģ�ͣ��ӿڵ�������ƥ�䣻
	bool GenerateMoFile(string& sFileName); //����Modelica����
	bool CompileModel(string& sFileName, string & sModelFullName); //����ģ�ͣ����ɷ�����
	bool SimulateModel(string& sModelFullName, string& sResFileName); //��������,�õ�����ļ�
	//�ɽ���ļ���ñ�����ʱ������ֵ
	vector<vector<double> > GetVariableTimeSeries(const char* sResFileName, const char* sVarName); 
	//�ӽ���ļ���������λ����Ϣ��ʱ�����У�ÿ��ʱ�̶�Ӧ�� ToSystem gn_Ax3
	vector<vector<double> > GetComponentDisplacement(const char* sResFileName, const char* sCompName);
};
