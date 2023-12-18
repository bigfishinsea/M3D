#pragma once
#include "M3d.h"
#include "cyberInfo.h"
#include "Connector.h"

class ModelicaModel
{
public: //����ʱ����䣬�ڽ���ʱ��������� ���ࡢ�����ࡢ�������������������ӿ�
	vector<string> m_vExtendsStringList; //���ඨ����
	vector<string> m_vImportsStringList; //������
	vector<string> m_vParameterStringList; //����
	vector<string> m_vComponentsStringList; //������ɴ˽���var, part, connector

public:
	string m_sName; //ģ�ͼ���
	string m_sType; //class, model, connector, block, ��
	
	bool m_bParsed;
	vector<ModelicaModel*> m_vExtends; //����
	vector<ModelicaModel*> m_vImports; //������
	vector<myParameter*> m_vParameters; //����
	vector<myOutputVar*> m_vOutVars;//����������һ�������Real, int,�������������
	map<string, ModelicaModel*> m_mapParts; //���������Ϊclass, model, block, connector�ı�������
	vector<Connector*> m_vConnectors; //�ӿ������:ֻ�洢���ͺ�����

	ModelicaModel* m_pParent; //��ģ�ͣ����Եݹ����ϣ����ģ�͵�ȫ��
	vector<ModelicaModel*> m_vEnbeddedModels; //Ƕ����,����

public:
	ModelicaModel();
	~ModelicaModel();
	string GetFullName() const;
	void Parse(); //����
	CyberInfo* GenerateCyberInfo();
	//ͨ������������ģ��
	ModelicaModel* LookupModelFromPartialName(const char* sPartialName);
	ModelicaModel* LookupMdlFromImports(const char* sPartialName);

public:
	static ModelicaModel* LoadMoLibrary(const char* sPath);
	//ͨ��ȫ���ڼ��ص�ģ�Ϳ��в���ģ��
	static ModelicaModel* LookupModel(string sMdlFullName);
};
