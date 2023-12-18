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

//形状+颜色
struct ShapeColor
{
	TopoDS_Shape theShape;
	Quantity_Color theColor;
};

//形状+纹理
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

//参数
struct myParameter
{
	string sFullName;      //全名
	string defaultValue;  //缺省值
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
	string sType;  //类型
 	bool   bFocused;      //是否关注
	string sFocusName;     //关注名；改为变型
	//string sUnit;			//类型
	//string sDimension;		//维度 ：空（标量）、[5]、[2,1]或[2,3,4]
};

//信息类
class CyberInfo
{
public:
	//CyberInfo();
	CyberInfo(DocumentComponent* pDoc);
	CyberInfo(DocumentComponent* pDoc,const char* sFullName);
	~CyberInfo();

private:
	DocumentComponent* m_pDoc;
	//Modelica类全名
	string m_sMdlFullName;
	//modelica模型的类型：package,model,class,block,connector,function,record,type
	string m_sModelType;
	//参数信息
	vector<myParameter*> m_Parameters;
	//输出变量信息
	vector<myOutputVar*> m_OutputVars;
	//接口类型，类型全名列表
	vector<string> m_vConnetorTypes;
	//接口名字
	vector<string> m_vConnectorNames;

public:
	//不保存：子组件的信息,<子组件简名，子组件类型的CyberInfo>
	map<string, CyberInfo*> mapParts; //在BuildInfo时候创建，平坦化时清除

public:
	//根据模型全名，从Mo文件中获得Model的全部信息！！！参数、输出变量、接口类型
	bool BuildCyberInfo(const char* sMoPath);
	//平坦化，将子组件的全部参数、变量和接口信息全部平坦化到this
	void Flattening();
	//自动计算参数值
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
	void AddConnNames(vector<string> vNames); //接口不带前缀
	void AddConnTypes(vector<string> vTypes);

	//Set...
	void SetMdlName(string sFullName);
	void SetMdlType(const char* csType);

	//下面几个Set函数..
	void SetDocument(DocumentComponent* pDoc) { m_pDoc = pDoc; }
	void SetParameters(vector<myParameter*>& vPars);
	void SetOutputVars(vector<myOutputVar*>& vVars);
	void SetConncTypes(vector<string>& vTypes);
	void SetConncNames(vector<string>& vNames);
};
