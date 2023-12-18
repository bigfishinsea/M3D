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
	NM_CONNECTOR=5,    //�ӿڣ��������ͣ�������Name����
	NM_DATUMPOINT=6, //�ο���
	NM_DATUMLINE=7,   //�ο���
	NM_DATUMPLANE=8,   //�ο���
};

//Լ������,��ȫ�貹��...
enum myConstraintType
{
	PARALLEL=1,
	CONCINE=2,
	TAGENT=3,
	DIM_LENGTH=4,
	DIM_ANGLE=5
};

//Լ���ṹ��
struct myConstraint
{
	int nIndex; //Լ�������
	myConstraintType consType;  //Լ������
	//vector<int>      shapeID1;  //��һ��Ԫ��
	string           shapeName1;
	//vector<int>      shapeID2;  //�ڶ���Ԫ��
	string           shapeName2;
	double           consValue; //Լ��ֵ���ߴ�Լ��
	bool             bActive;   //�Ƿ�������
};

//������Ϣ�ṹ��,10��Ԫ��
struct myPhysicalProperty
{
	double mass;                           //����
	vector<double> centric;                //���� [3]
	vector<double> inertia;       //���� [1,1],[2,1],[2,2],[3,1],[3,2],[3,3]
};

//�ο��㶨������
enum myDatumPointType
{
	VERTEX_OFFSET=1, //��ƫ��
	FACE_CENTER=2, //������
	INTER_POINT=3, //������Ľ���
	MID_POINT=4, //��ѡ�����㣬�õ��е�
	CENTRIC_POINT=5,//���ģ�Solid
	PROJECT_POINT=6 //ͶӰ�㣺��һ��Vertex���ڶ�����
};

//�ο��ᶨ������
enum myDatumLineType
{
	EDGE_OFFSET=1,//��ƫ�� X,Y,Z
	THROUGH2POINTS=2,//��������
	CYLINDER_AXIS=3,//Բ����
	FACES_INTER=4, //�潻��
	FACE_PERPEND=5//�洹ֱ(ѡ��һ�����һ���棬�õ����������Ĵ���)
};

//�ο���
class DatumPoint
{
private:
	myDatumPointType m_dptType; //���巽ʽ
	string m_shape1_Name;
	string m_shape2_Name;
	vector<double> m_offVal;  //���ܲ���
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
	//������״
	//TopoDS_Shape Generate();
	void SetShape(TopoDS_Shape shp) { m_shape = shp; }
	TopoDS_Shape GetShape() const { return m_shape; }
};

//�ο���,�ο���
class DatumLine
{
private:
	myDatumLineType m_dlnType;  //��������
	string m_shape1_Name;
	string m_shape2_Name;
	vector<double> m_offVal;   //ƫ��ֵ���ܲ���
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
	//������״
	//TopoDS_Shape Generate();
	void SetShape(TopoDS_Shape shp) { m_shape = shp; }
	TopoDS_Shape GetShape() const { return m_shape; }
};

//�ӿڶ�������
enum myConnectDefineType
{
	//ͨ������ԭ��ͷ�λ
	THREE_POINTS = 1,//�����㶨��ԭ�������ϵ
	TWO_EDGES=2,    //�����߶���ԭ�������ϵ

	//ͨ��ԭ���x�ᶨ�壬YOZƽ��ȱʡ��λ
	POINT_X_AXIS=3, //��һ����ԭ�㣬�ڶ���ʵ����X�᷽��

	//ͨ��ԭ�㶨�壬����������
	ONE_POINT=4
};

//�ӿڽṹ��
class Connector
{
private:
	//λ�ˣ�������û�����ϵ��,9��ʵ��
	vector<double> m_displacement;
	TopoDS_Shape m_shape;

private:
	string m_sConnName;          //�ӿ����֣�
	string m_sConnType;          //�ӿ�����,classȫ������ǰ׺�ж���������𣺻�е�����ӵ�����Һѹ�����ơ��ȵ�
	myConnectDefineType    m_defType;   //��������
	vector<string>         m_shapeRefs;  //���õ�ʵ��ID //����һ�������3��
	bool m_bZ_Reversed;                  //Z�ᷴ��
	
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
	//�������
	gp_Pnt GetCenter() const;

	void SetConnName(const char* name) { m_sConnName = name; }
	string GetConnName() const { return m_sConnName; }
	//�ӿ�����type��Modelicaȫ��
	void SetConnType(string sType);
	string GetConnType() const;
	//��������
	void SetDefineType(myConnectDefineType vType);
	myConnectDefineType GetDefineType() const;
	//����ʵ��
	void SetRefEnts(vector<string>& ents);
	vector<string> GetRefEnts() const;
	////�õ�λ��
	//void SetPosOrient(vector<double>& v6);
	//vector<double> GetPosOrent() const;
};

//����
struct myParameter
{
	string sFullName;      //ȫ��
	double defaultValue;  //ȱʡֵ
	string sUnit;          //��λ
	bool   bFocused;      //�Ƿ��ע
	string sFocusName;     //��ע��
	string sType;			//��������
	string sDimension;		//ά�� ���գ���������[5]��[2,1]��[2,3,4]
};

//�������
struct myOutputVar
{
	string sFullName;
	string sUnit;
	bool   bFocused;      //�Ƿ��ע
	string sFocusName;     //��ע��
	//string sType;			//����
	//string sDimension;		//ά�� ���գ���������[5]��[2,1]��[2,3,4]
};

//��Ϣ��
class CyberInfo
{
public:
	CyberInfo();
	CyberInfo(string& sFullName);
	~CyberInfo();

private:
	//Modelica��ȫ��
	string m_sMdlFullName;
	//������Ϣ
	vector<myParameter*> m_Parameters;
	//���������Ϣ
	vector<myOutputVar*> m_OutputVars;
	//�ӿ����ͣ�����ȫ���б�
	vector<string> m_vConnetorTypes;
	//�ӿ�����
	vector<string> m_vConnectorNames;
public:
	//����ģ��ȫ�������Model��ȫ����Ϣ����������������������ӿ�����
	bool BuildCyberInfo();

	//Get...
	string GetMdlName() const;
	vector<myParameter*> GetParameters() const;
	vector<myOutputVar*> GetOutputVars() const;
	vector<string>   GetConnectorTypes() const;
	vector<string>   GetConnectorNames() const;
	
	//Set...
	void SetMdlName(string & sFullName);

	//���漸��Set����������...
	void SetParameters(vector<myParameter*> & vPars);
	void SetOutputVars(vector<myOutputVar*> & vVars);
	void SetConncTypes(vector<string> & vTypes);
	void CyberInfo::SetConncNames(vector<string>& vNames);
};

//��״+��ɫ
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
/// ����ĵ�
/// </summary>
class M3dCom_DS : public M3d_DS
{
public:
	M3dCom_DS();
	//��������죬pRoot���ĵ���TDF���ڵ㴫��
	M3dCom_DS(TDF_Label lblRoot);
	~M3dCom_DS();

public:
	void SetRoot(TDF_Label pRoot);

	//1.��״�������
private:
	//��һ��hash��洢��״shape�����ڵ��Label��Ӧ��ϵ�����ڿ��ٲ���
	//��ֱ������������
	//hash_map <int, Handle(TDF_Label)> m_hmShapes;
	//map<Handle(TopoDS_Shape), TDF_Label> m_mapShapes;
private:
	//������״���ͣ����ӵ���Ӧ�����ڵ��ϣ��洢��ϵ��hm
	void AddShape(myShapeType t0, const TopoDS_Shape& aShape);
	//��hm�õ�Shape��Label
	TDF_Label GetShapeLabelandID(const TopoDS_Shape& theShape, vector<int> & ID) const;
	TDF_Label GetShapeLabel(const TopoDS_Shape& theShape) const;
	//�õ�TopoDS_Shape��ID ,��[3,3]���ȴ�hashmap�õ�label,�ٴ����ϱ����ҵ�label�������ţ���ֱ����Label��tag�õ�
	vector<int> GetShapeID(const TopoDS_Shape& aShape) const;
	//��ID�õ���״
	TopoDS_Shape GetShapeFromID(vector<int>& vID) const;
	TopoDS_Shape GetShapeFromID0(vector<int>& vID) const;
	//����hashmap����״��:���ĵ������ݸ���hmShapes�����ĵ���ȡʱ��;Undo/Redo
	void UpdateShapes();
	//���ȫ��Shapes���ڶ��ļ��󣬻��ȫ����TopoDS_Shape������ʾ
	vector<TopoDS_Shape> GetAllShapes() const;
public:
	//������ʵ�壬���ڴ�ȡ���Բ���: �������ʵ��ֻ��һ��!!!
	void SetMainSolid(const TopoDS_Shape& theShape, Graphic3d_NameOfMaterial mat= Graphic3d_NameOfMaterial::Graphic3d_NameOfMaterial_Steel);
	//����ĵ�
	void Clear();
	//�����ʵ��
	TopoDS_Shape GetMainSolid(Graphic3d_NameOfMaterial& mat) const;
	//�õ�TopoDS_Shape���������硰��3��
	virtual string GetShapeName(const TopoDS_Shape& aShape) const;
	//�����ֵõ���״
	TopoDS_Shape GetShapeFromName(string& theName) const;

	//2.�ӿڴ洢���
public:
	//���ӽӿڣ���Root�������ӽڵ�洢���ӿ�
	void AddConnector(Connector * pConn);//��Generate()������״
	//����״�õ��ӿڵ���Ϣ����new Connector���û�������Ҫdelete,�������״����Connector���򷵻�null
	Connector* GetConnector(const TopoDS_Shape& connShape);
	//�������нӿ�
	bool UpdateConnector(const TopoDS_Shape& connShape, Connector* pCon);
	//ɾ���ӿ�
	bool DeleteConnector(const TopoDS_Shape& connShape);

	//����\���\���¡�ɾ���ο��㡢��
	void AddDatumPoint(const TopoDS_Shape& connShape, DatumPoint* pDPt);
	DatumPoint* GetDatumPoint(const TopoDS_Shape& connShape);
	bool UpdateDatumPoint(const TopoDS_Shape& connShape, DatumPoint* pDPt);
	bool DeleteDatumPoint(const TopoDS_Shape& connShape);
	void AddDatumLine(const TopoDS_Shape& connShape, DatumLine* pDPt);
	DatumLine* GetDatumLine(const TopoDS_Shape& connShape);
	bool UpdateDatumLine(const TopoDS_Shape& connShape, DatumLine* pDPt);
	bool DeleteDatumLine(const TopoDS_Shape& connShape);

	//3.��ɫ��ȡ���
public:
	//���ĵ�����ĳ��״����ɫ���ҵ����Ӧ��Label����������ɫ����
	void SetShapeColor(const TopoDS_Shape& theShape, Quantity_Color theColor);
	//���ĵ����ĳ��״����ɫ
	Quantity_Color GetShapeColor(const TopoDS_Shape& theShape) const;
	map<string, vector<double> > GetColorMap();
	
	//4.Լ����ȡ���
private:
	//���ĵ�����Լ����������Ӧ��TDF_Label�ӽڵ�
	void AddConstraint(TDF_Label &parent,int nth, myConstraint & pConstraint);
	//���ĵ�ɾ��ĳ��Լ��,�ڼ���
	bool DeleteConstraint(int theConsID);
	//���µ�theID��Լ��
	bool UpdateConstraint(int theID, myConstraint& pConstraint);
	//�õ��ڼ���Լ��, ����null������ֵ���û�����ɾ��
	myConstraint* GetConstraint(TDF_Label& parent, int nTh) const;
	//�õ��ڼ���Լ��������ʵ��Ԫ��
	vector<TopoDS_Shape> GetConstraintShapes(int nTh) const;
	//Լ������״
	//......
public:
	//�����ǰ��Լ���������µ�Լ����
	void SetConstraints(vector<myConstraint> & vConstraints);
	//�õ�ȫ��Լ��
	vector<myConstraint> GetAllConstraints() const;

	//5.���Ϻ�͸����
public:
	//�����������,���������Դ洢��TDF_Label(solid)��
	void SetMaterial(Graphic3d_NameOfMaterial nMaterial);
	//���ĵ���ò���
	Graphic3d_NameOfMaterial GetMaterial() const;
	//����͸����,ͬ ����
	void SetTransparency(double nTransp);
	//������͸����
	double  GetTransparency() const;

	//6.������Ϣ��ȡ:����[3]��������ת������[3X3]/2-----------��ʱ���洢
private:
	//�������Բ���,�洢�ڸ��ڵ�,��ΪAttribute
	void SetPhysicalProperty(myPhysicalProperty& par);
	//���ĵ�������Բ���
	myPhysicalProperty GetPhysicalProperty() const;
	
	//7.Cyber��Ϣ���ݴ�ȡ
private:
	void AddParameter(TDF_Label &parLabel, int nIndex, myParameter *aPar);
	void AddOutputVar(TDF_Label &parLabel, int nIndex, myOutputVar* aVar);
	myParameter * GetParameter(TDF_Label&  parLabel, int nth) const;
	myOutputVar * GetOutputVar(TDF_Label& parLabel, int nth) const;
public:
	//����Updateʱ������Cyber��Ϣ���ĵ�
	bool SetCyberInfo(CyberInfo* pCyber);
	//���ĵ���ȡCyber��Ϣ,���ʧ�ܷ���null,�˺���new CyberInfo���û��Լ�delete
	CyberInfo* GetCyberInfo() const;
	//���ļ�
	static M3dCom_DS* ReadFile(const char* file);
	//����Component����
	Component* GenerateComponent();
	vector<DatumPoint*> GetAllDatumPoints() const;
	vector<DatumLine*>  GetAllDatumLines() const;
	vector<Connector*>  GetAllConnectors() const;
	vector<ShapeColor> GetAllShapeColors() const;

	//ɾ��������Ԫ��sName�Ķ���
	void DeleteRefObjects(const char* sName);

private:
	bool LookForColorLabel(const char * shpName, TDF_Label &theLabel);
};
