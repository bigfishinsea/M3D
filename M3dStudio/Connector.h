#pragma once
#include "M3d.h"
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

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
	VERTEX_OFFSET = 1, //��ƫ��
	FACE_CENTER = 2, //������
	INTER_POINT = 3, //������Ľ���
	MID_POINT = 4, //��ѡ�����㣬�õ��е�
	CENTRIC_POINT = 5,//���ģ�Solid
	PROJECT_POINT = 6 //ͶӰ�㣺��һ��Vertex���ڶ�����
};

//�ο��ᶨ������
enum myDatumLineType
{
	EDGE_OFFSET = 1,//��ƫ�� X,Y,Z
	THROUGH2POINTS = 2,//��������
	CYLINDER_AXIS = 3,//Բ����
	FACES_INTER = 4, //�潻��
	FACE_PERPEND = 5//�洹ֱ(ѡ��һ�����һ���棬�õ����������Ĵ���)
};

//����涨������
enum myMarkingSurfaceType
{
	SPHERE = 1, // ����
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

//�����
class MarkingSurface
{
private:
	myMarkingSurfaceType m_msurfType; //��������
	TopoDS_Shape m_shape;
	vector<double> m_offVal;  //���ܲ���
	double m_radius; // �뾶
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

//�ӿڶ�������
enum myConnectDefineType
{
	//ͨ������ԭ��ͷ�λ
	THREE_POINTS = 1,//�����㶨��ԭ�������ϵ
	TWO_EDGES = 2,    //�����߶���ԭ�������ϵ

	//ͨ��ԭ���x�ᶨ�壬YOZƽ��ȱʡ��λ
	POINT_X_AXIS = 3, //��һ����ԭ�㣬�ڶ���ʵ����X�᷽��

	//ͨ��ԭ�㶨�壬����������
	ONE_POINT = 4
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
