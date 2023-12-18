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
};

//���������Ϣ
struct CompBaseInfo
{
	string sCompName;                    //�����
	string sCompFile;                    //������ļ���
	vector<double> v18Transf;    //�������ڸ�����ı任����
};

//�������
class ComponentAttrib
{
public:
	ComponentAttrib();

	//variablity
	int iVariablity; //0-ȱʡ��1-Constant; 2-parameter; 3-Discrete
	//Casuality
	int iCasuality; //0-None; 1-Input; 2-Output
	//ConnectorMember
	int iConnectorMember;//0-����; 1-flow; 2-stream
	//Properties
	bool bFinal;
	bool bProtected;
	bool bReplaceable;
	//Inner/Outer
	bool bInner;
	bool bOuter;
};
///
/// ������󣬰�������Ͳ���: ��Ϣ����DocumentCommon���߼��ϵ������
/// 
/// �����ܣ���ʾ�����ˡ����ɴ���
class Component
{
public:
	//��ʾ����ʾ�������ʾ��list(AIS_InteractiveObject)
	void Display(Handle(AIS_InteractiveContext) theContext, gp_Trsf delTrans);
	void Display(Handle(AIS_InteractiveContext) theContext, bool bRegen=true);
	
	//���ˣ��������˵ı任�����ĵ���������ͬʱ�������ʾ����б任
	void Transform(gp_Trsf theTransform);

	//��������Ĳ�ι�ϵ������Modelica���룬��Ӧ����Ϣ����Document��ȡ
	bool GenerateMoCode(string & sCode, int nLevel=0);

private:
	DocumentCommon* m_pDoc;

public:
	Component(DocumentCommon* pDoc, Component* parent = nullptr);
	~Component();

private:
	Component* m_parent;
	list<Component*> m_lstChildren;  
	string m_sCompName;               //�������������������ӣ��硱cam1������1��ʼ���

	//�������Ϣ���Ӷ���һ���ã��ɵ�ǰ��װ���ļ���ȡ
	//map<string, CompInfo> m_mapCompInfo;
	//������ڸ�����еı任���󣬼�����������������˱任���������Ϊ�գ���Ϊ��λ��
	//vector<double> m_transform;
	TopoDS_Shape m_shpCompound; //�ڴ�����������ʱ���� generateCompound
	////�����ȫ�����ӻ�AIS_InteractiveObject,������ʵ�壬�ӿڣ��ο��ߣ�����ϵ��
	//NCollection_Vector<Handle(AIS_InteractiveObject)> m_theAISObjects;
	//gp_Trsf m_DeltaTransform;

public:
	//map<string, CompInfo> GetComponentsInfo() const { return m_mapCompInfo; }
	//�õ������ʵ��compoundSolid
	TopoDS_Shape GetCompound() const { return m_shpCompound; }
	TopoDS_Shape GenerateCompound(gp_Trsf theTransf);
	TopoDS_Shape GenerateCompound1(vector<ShapeMaterial>& shpMats,vector<ShapeTransparency>& shpTransparencys, vector<ShapeColor>& shpColors,vector<TopoDS_Shape>& shpDatumPts, vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectos,vector<gp_Ax3>& coordSys);
	TopoDS_Shape GenerateColoredCompound(gp_Trsf transf, vector<ShapeTransparency>& shpTransparencys, vector<ShapeColor>& shpColors, vector<ShapeTexture>& shpTextures, vector<TopoDS_Shape>& shpDatumPts, vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectos, vector<gp_Ax3>& coordSys, vector<TopoDS_Shape>& shpMarkSurf);

	//�����������ʾ���
	void GenerateAISObjects(gp_Trsf theTransform, vector<Component*>& vComponents);
	//�õ�ȫ������
	void GetAllDescendants(vector<Component*>& vComps);

	//����һ���ӽڵ�
	void AddChild(Component* pCom);
	//�����������
	void SetCompName(const char* name);
	string GetCompName() const { return m_sCompName; }
	string GetCompFullName() const; //�õ������ȫ��

	//���ڵ�
	void SetParent(Component* pCom) { m_parent = pCom; }
	Component* GetParent() const { return m_parent; }
	list<Component*> GetChilds() const { return m_lstChildren; }
	Component* GetChild(const char* sChild);
	void DeleteChild(const char* sChild);

	void SetDocument(DocumentCommon* pDoc) { m_pDoc = pDoc; }
	DocumentCommon* GetDocument() const { return m_pDoc; }

	//�������Document��ӻ�û�������
	gp_Trsf GetOriginTransform() const; //ԭʼ�任
	gp_Trsf GetDeltaTransform() const;	//�õ����λ��
	void SetOriginTransform(gp_Trsf tras);
	void SetDeltaTransform(vector<double>& v15 /*gp_Trsf trf*/);

	QString GetCompFile() const;
	//CyberInfo
	CyberInfo* GetCyberInfo() const;
	Connector* GetConnector(const TopoDS_Shape& shp) const;

	//���ʱ
	bool IsConnector() const;
	Connector* GetMainConnector() const;
	bool IsWorld() const;

	//////
	bool IsAssembly() const;

	//������
	bool IsDeformable() const;

	//������ʾ�б�
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
