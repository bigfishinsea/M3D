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

using namespace std;
using namespace stdext;

class DocumentComponent;
class ModelicaModel;

//��״+��ɫ
struct ShapeColor
{
	TopoDS_Shape theShape;
	Quantity_Color theColor;
};

//��״+����
struct ShapeTexture
{
	TopoDS_Shape theShape;
	string sTextFile;
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

//����
struct myParameter
{
	string sFullName;      //ȫ��
	string defaultValue;  //ȱʡֵ
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
	string sType;  //����
 	bool   bFocused;      //�Ƿ��ע
	string sFocusName;     //��ע������Ϊ����
	//string sUnit;			//����
	//string sDimension;		//ά�� ���գ���������[5]��[2,1]��[2,3,4]
};

//��Ϣ��
class CyberInfo
{
public:
	//CyberInfo();
	CyberInfo(DocumentComponent* pDoc);
	CyberInfo(DocumentComponent* pDoc,const char* sFullName);
	~CyberInfo();

private:
	DocumentComponent* m_pDoc;
	//Modelica��ȫ��
	string m_sMdlFullName;
	//modelicaģ�͵����ͣ�package,model,class,block,connector,function,record,type
	string m_sModelType;
	//������Ϣ
	vector<myParameter*> m_Parameters;
	//���������Ϣ
	vector<myOutputVar*> m_OutputVars;
	//�ӿ����ͣ�����ȫ���б�
	vector<string> m_vConnetorTypes;
	//�ӿ�����
	vector<string> m_vConnectorNames;

public:
	//�����棺���������Ϣ,<�������������������͵�CyberInfo>
	map<string, CyberInfo*> mapParts; //��BuildInfoʱ�򴴽���ƽ̹��ʱ���

public:
	//����ģ��ȫ������Mo�ļ��л��Model��ȫ����Ϣ����������������������ӿ�����
	bool BuildCyberInfo(const char* sMoPath);
	//ƽ̹�������������ȫ�������������ͽӿ���Ϣȫ��ƽ̹����this
	void Flattening();
	//�Զ��������ֵ
	void AutomaticComputing();

	//Get...
	string GetMdlName() const;
	string GetMdlType() const;
	vector<myParameter*> GetParameters() const;
	vector<myOutputVar*> GetOutputVars() const;
	vector<string>   GetConnectorTypes() const;
	vector<string>   GetConnectorNames() const;
	myParameter* GetParameter(const char* sPar) const;

	//Add..
	void AddParameters(ModelicaModel* pMdl, vector<myParameter*> vParams, string sPre="");
	void AddOutputVars(vector<myOutputVar*> vVars, string sPre = "");
	void AddConnNames(vector<string> vNames); //�ӿڲ���ǰ׺
	void AddConnTypes(vector<string> vTypes);

	//Set...
	void SetMdlName(string sFullName);
	void SetMdlType(const char* csType);

	//���漸��Set����..
	void SetDocument(DocumentComponent* pDoc) { m_pDoc = pDoc; }
	void SetParameters(vector<myParameter*>& vPars);
	void SetOutputVars(vector<myOutputVar*>& vVars);
	void SetConncTypes(vector<string>& vTypes);
	void SetConncNames(vector<string>& vNames);
};
