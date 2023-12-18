#pragma once
#include "M3d.h"
#include "cyberInfo.h"
#include "Connector.h"

class ModelicaModel
{
public: //读入时候填充，在解析时产生下面的 基类、导入类、参数、变量、部件、接口
	vector<string> m_vExtendsStringList; //基类定义行
	vector<string> m_vImportsStringList; //导入类
	vector<string> m_vParameterStringList; //参数
	vector<string> m_vComponentsStringList; //组件，由此解析var, part, connector

public:
	string m_sName; //模型简名
	string m_sType; //class, model, connector, block, …
	
	bool m_bParsed;
	vector<ModelicaModel*> m_vExtends; //基类
	vector<ModelicaModel*> m_vImports; //导入类
	vector<myParameter*> m_vParameters; //参数
	vector<myOutputVar*> m_vOutVars;//简短类变量，一般变量（Real, int,…），除了组件
	map<string, ModelicaModel*> m_mapParts; //组件（类型为class, model, block, connector的变量）；
	vector<Connector*> m_vConnectors; //接口类组件:只存储类型和名字

	ModelicaModel* m_pParent; //父模型：可以递归向上，获得模型的全名
	vector<ModelicaModel*> m_vEnbeddedModels; //嵌套类,子类

public:
	ModelicaModel();
	~ModelicaModel();
	string GetFullName() const;
	void Parse(); //解析
	CyberInfo* GenerateCyberInfo();
	//通过部分名字找模型
	ModelicaModel* LookupModelFromPartialName(const char* sPartialName);
	ModelicaModel* LookupMdlFromImports(const char* sPartialName);

public:
	static ModelicaModel* LoadMoLibrary(const char* sPath);
	//通过全名在加载的模型库中查找模型
	static ModelicaModel* LookupModel(string sMdlFullName);
};
