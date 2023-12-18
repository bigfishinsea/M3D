#include "ModelicaModel.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStack>

extern string GetSentenceFromPosToChar(string& subStr, int& pos, char to);
extern bool IsClassDefining(QString sLine, QString& sNameReturn, QString& sTypeReturn);
extern bool IsClassEnd(QString sLine, QString sModelName);
extern bool IsComponentDefine(QString& sLine);
extern bool IsParameterDefine(QString sLine, myParameter*& pParam);
extern bool IsVariableDefine(QString sLine, myOutputVar*& pVar);
extern QString GetFirstWord(QString sLine);
extern QString GetShortName(const char* sFullName);

string GetExtendsMdlName(const char* csLine)
{
	QString sLine = csLine;
	string sExtMdlPartialName;
	sLine = sLine.trimmed();
	sLine.replace("\n", " ");
	sLine.replace("  ", " ");

	if (sLine.left(8) == "extends ")
	{
		sLine = sLine.mid(8);
		sExtMdlPartialName = GetFirstWord(sLine).toStdString();

		return sExtMdlPartialName;
	}

	return "";
}

string GetImportMdlName(const char* csLine) //������
{
	string sImportMdlPartialName;
	QString sLine = csLine;
	sLine = sLine.trimmed();
	sLine.replace("\n", " ");
	sLine.replace("  ", " ");

	if (sLine.left(7) == "import ")
	{
		sLine = sLine.mid(7);
		int posEqu = sLine.indexOf("=");
		if (posEqu > 0)
		{
			sLine = sLine.mid(posEqu + 1);
		}
		sImportMdlPartialName = GetFirstWord(sLine).toStdString();

		return sImportMdlPartialName;
	}

	return "";
}

/// <summary>
/// ModelicaModel
/// </summary>
ModelicaModel::ModelicaModel()
{
	m_bParsed = false;
	m_pParent = nullptr;
}

ModelicaModel::~ModelicaModel()
{
	//ɾ������
	for (int i = 0; i < m_vParameters.size(); i++)
	{
		delete m_vParameters[i];
	}

	//ɾ������
	for (int i = 0; i < m_vOutVars.size(); i++)
	{
		delete m_vOutVars[i];
	}

	//ɾ��Connector
	for (int i = 0; i < m_vConnectors.size(); i++)
	{
		delete m_vConnectors[i];
	}

	//ɾ��Ƕ����
	for (int i = 0; i < m_vEnbeddedModels.size(); i++)
	{
		delete m_vEnbeddedModels[i];
	}

	//���಻��Ҫɾ�������Ҳ�ǣ���ģ��ֻ�����ã�����
}

int GetModelSpecificationEnd(const QString & sLine, const char * sMdlName)
{
	int mESPos = sLine.indexOf(sMdlName);
	QString qsMdl = sMdlName;
	mESPos += qsMdl.length();
	
	int pos0 = sLine.indexOf('\"', mESPos);
	if (pos0 < 0)
	{
		return mESPos;
	}

	int pos1;
	while (1) //ȥ�� \"
	{
		pos1 = sLine.indexOf('\"', pos0+1);
		assert(pos1 > 0);
		if (sLine.at(pos1-1) == '\\')
		{
			pos0 = pos1;
		}
		else
			break;
	}

	return pos1 + 1;
}

bool IsShortClassDefine(QString sLine)
{
	QString sName, sType;
	if (!IsClassDefining(sLine,sName, sType))
	{
		return false;
	}

	sLine.replace("  ", " ");
	int pos = sLine.indexOf(sName);
	assert(pos > 0);
	sLine = sLine.right(sLine.length() - pos - sName.length());
	sLine = sLine.trimmed();
	if (sLine.indexOf('=') == 0)
	{
		return true;
	}

	return false;
}

//����mo�ļ�������ģ����: ģ���������ļ��ж�ȡ�õ�������·����������
//һ���� .../package.mo
//��һ���ǣ�.../abc.mo
ModelicaModel* CreateSingleFileModel(const char* sMoFile)
{
	ModelicaModel* pModel = nullptr;
	if (!QFileInfo(sMoFile).exists())
	{
		return pModel;
	}

	QFile qFile(sMoFile);
	qFile.open(QIODevice::ReadOnly);
	string sFullText = qFile.readAll();
	qFile.close();

	QStack<ModelicaModel*> mdlStack;
	int pos = 0;
	while (pos < sFullText.length())
	{
		//�ȶ�һ�У������ģ�Ͷ��������������һ��;
		int oldPos = pos;
		QString sLine = GetSentenceFromPosToChar(sFullText, pos, ';').c_str();

		//����
		int posX = sLine.indexOf("model FixedRotation");
		if (posX >= 0)
		{
			int x = 0;// assert(false);
		}

		//ȥ��ע��
		QString sLine0 = sLine.trimmed();
		while (sLine0.left(2) == "//")
		{
			int posd = sLine0.indexOf('\n');
			if (posd >= 0) {
				//oldPos += posd + 1;
				//pos = oldPos;
				sLine0 = sLine0.right(sLine0.length() - posd - 1);
				sLine0 = sLine0.trimmed();
			}
			else {
				break;
			}
		}
		sLine = sLine0;

		QString sName, sType;
		if (IsClassDefining(sLine, sName, sType))
		{
			ModelicaModel* pMdl = new ModelicaModel();
			pMdl->m_sName = sName.toStdString();
			pMdl->m_sType = sType.toStdString();
			//pMdl->m_sMoPath = sMoFile.toStdString();

			if (mdlStack.size() == 0)
			{
				pModel = pMdl; //���Ƕ�ģ��
			}
			else
			{
				ModelicaModel* pParent = mdlStack.top();
				if (pParent)
				{
					pParent->m_vEnbeddedModels.push_back(pMdl);
					pMdl->m_pParent = pParent;
				}
			}

			//����� ���� ��connector RealInput = ...
			if (IsShortClassDefine(sLine))
			{
				continue;
			}
			else
			{
				mdlStack.push(pMdl);
			}

			//��ȥģ�͵�ע��
			int mESPos = GetModelSpecificationEnd(sLine,sName.toStdString().c_str());
			////�����ģ��Ƕ�׶��壬���ж�
			//while (oldPos < pos)
			//{//ȥ������
			//	sLine = GetSentenceFromPosToChar(sFullText, oldPos, '\n').c_str();
			//	sLine = sLine.trimmed();
			//	if (sLine.indexOf('\"') == 0) continue;
			//	if (!sLine.isEmpty()) break;
			//}
			if (mESPos > 0)
			{
				sLine = sLine.right(sLine.length() - mESPos - 1);
				oldPos += mESPos;
			}

			while (!sLine.isEmpty())//oldPos < pos)
			{
				//int pos0 = oldPos;
				//sLine = GetSentenceFromPosToChar(sFullText, oldPos, '\n').c_str();
				QString sName0, sType0;
				if (IsClassDefining(sLine, sName0, sType0))
				{
					ModelicaModel* pMdl0 = new ModelicaModel();
					pMdl0->m_sName = sName0.toStdString();
					pMdl0->m_sType = sType0.toStdString();

					ModelicaModel* pParent0 = mdlStack.top();
					assert(pParent0);
					pParent0->m_vEnbeddedModels.push_back(pMdl0);
					pMdl0->m_pParent = pParent0;
					mdlStack.push(pMdl0);

					//��ȥ����
					mESPos = GetModelSpecificationEnd(sLine, sName0.toStdString().c_str());
					if (mESPos > 0)
					{
						sLine = sLine.right(sLine.length() - mESPos - 1);
						oldPos += mESPos;
					}
				}
				else {
					//pos = oldPos; //���˵����������俪ʼ��
					//pos -= sLine.length(); 
					break;
				}
			}
			//continue;
		}

		sLine = sLine.trimmed();
		if (sLine.indexOf("annotation ") == 0)
		{
			continue;
		}

		//����ǽ���
		if (mdlStack.size() > 0)
		{
			ModelicaModel* pTopMdl = mdlStack.top();
			QString sShName = pTopMdl->m_sName.c_str();
			if (IsClassEnd(sLine, sShName))
			{
				mdlStack.pop();
				continue;
			}
		}

		//����Ƿ��̻��㷨��ʼ����ȫ������
		if (sLine.indexOf("equation") == 0 || sLine.indexOf("initial equation") == 0 || sLine.indexOf("algorithm") == 0 || sLine.indexOf("initial algorithm") == 0)
		{
			while (pos < sFullText.length())
			{
				sLine = GetSentenceFromPosToChar(sFullText, pos, ';').c_str();
				sLine = sLine.trimmed();
				if (sLine.indexOf("end ") == 0)
				{
					ModelicaModel* pTopMdl = mdlStack.top();
					QString sShName = pTopMdl->m_sName.c_str();
					if (IsClassEnd(sLine, sShName))
					{
						mdlStack.pop();
						break;
					}
				}
			}
			continue; //����һ�䣻
		}

		//����ǲ��������¼
		if (sLine.indexOf("parameter ") == 0 || sLine.indexOf("final parameter ") == 0)
		{
			ModelicaModel* pTopMdl = mdlStack.top();
			pTopMdl->m_vParameterStringList.push_back(sLine.toStdString());
			continue;
		}

		//�����extends
		if (sLine.indexOf("extends ") == 0 || sLine.indexOf("extends\n") == 0)
		{
			ModelicaModel* pTopMdl = mdlStack.top();
			pTopMdl->m_vExtendsStringList.push_back(sLine.toStdString());
			continue;
		}

		//�����imports
		if (sLine.indexOf("import ") == 0)
		{
			ModelicaModel* pTopMdl = mdlStack.top();
			pTopMdl->m_vImportsStringList.push_back(sLine.toStdString());
			continue;
		}

		//�����component
		if (IsComponentDefine(sLine))
		{
			ModelicaModel* pTopMdl = mdlStack.top();
			pTopMdl->m_vComponentsStringList.push_back(sLine.toStdString());
			continue;
		}
	}

	return pModel;
}

//�ݹ鴴��model������pMdl�£�thePackage·���� D:/w1/w2/package.mo
ModelicaModel*  RecurseCreateModel(const char* thePackagePath)
{
	ModelicaModel* pMdl = nullptr;
	if (!QFileInfo(thePackagePath).exists())
	{
		return pMdl;
	}

	//��������package.mo��ģ��
	pMdl = CreateSingleFileModel(thePackagePath);

	//�õ�·��
	QString path = thePackagePath;
	path.replace("/package.mo", "");

	QDir dir(path);
	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

	//�ҵ���ǰ·���µ���·�����ݹ�
	for (int i = 0; i != folder_list.size(); i++)
	{
		QString subPath = folder_list.at(i).absoluteFilePath();
		QString sBaseName = folder_list.at(i).baseName();
		QString subPackage = subPath;
		subPackage += "/package.mo";

		if (!QFileInfo(subPackage).exists())
		{
			continue;
		}
		ModelicaModel* pSubMdl = RecurseCreateModel(subPackage.toStdString().c_str());
		pMdl->m_vEnbeddedModels.push_back(pSubMdl);
		pSubMdl->m_pParent = pMdl;
	}

	//�Ե���mo�ļ�����
	for (int i = 0; i != file_list.size(); i++)
	{
		QString subPath = file_list.at(i).absoluteFilePath();

		if (subPath.indexOf(".mo") > 0 && subPath != thePackagePath)
		{
			ModelicaModel* pChldMdl = CreateSingleFileModel(subPath.toStdString().c_str());
			pMdl->m_vEnbeddedModels.push_back(pChldMdl);
			pChldMdl->m_pParent = pMdl;
		}
	}

	return pMdl;
}

//��Ҫ����������
//װ��ģ��ʱ��Ϊ������ٶȣ��Ƚ��ַ�����Ϣ����洢���ڽ���ʱ�ٹ�������
ModelicaModel* ModelicaModel::LoadMoLibrary(const char* sMoPath)
{
	ModelicaModel* theTopModel = RecurseCreateModel(sMoPath);

	return theTopModel;
}

//��g_vModelicaModels�в���
ModelicaModel* ModelicaModel::LookupModel(string sMdlFullName)
{
	ModelicaModel* retModel = nullptr;

	vector<ModelicaModel*> vTmp = g_vModelicaModels;
	string sRightName = sMdlFullName;
	while (!sRightName.empty())
	{
		string sLeft = sRightName;
		int pos = sRightName.find('.');
		if (pos > 0)
		{
			sLeft = sRightName.substr(0, pos);
			sRightName = sRightName.substr(pos + 1);
		}
		else {
			sRightName = "";
		}

		bool bFound = false;
		for (int i = 0; i < vTmp.size(); i++)
		{
			ModelicaModel* pMdl_i = vTmp[i];
			if (pMdl_i->m_sName == sLeft)
			{
				vTmp = pMdl_i->m_vEnbeddedModels; //Ƕ����
				retModel = pMdl_i;
				bFound = true;
				break;
			}
		}
		if (!bFound) {
			retModel = nullptr;
			break;
		}
	}

	////����
	//assert(retModel->GetFullName() == sMdlFullName);

	return retModel;
}

string ModelicaModel::GetFullName() const
{
	string sFullName = m_sName;

	ModelicaModel* pParent = m_pParent;
	while (pParent)
	{
		sFullName = pParent->m_sName + "." + sFullName;
		pParent = pParent->m_pParent;
	}

	return sFullName;
}

//���ַ��������õ����ݽṹ
void ModelicaModel::Parse()
{
	if (m_bParsed) return;

	////����
	//if (m_sName == "World")
	//{
	//	assert(false);
	//}

	//����extends
	for (int i = 0; i < m_vExtendsStringList.size(); i++)
	{
		string sLine = m_vExtendsStringList[i];
		string sExtMdlPartialName = GetExtendsMdlName(sLine.c_str()); //������
		if (!sExtMdlPartialName.empty()) //�˴�����assert
		{
			//���һ���ģ��
			ModelicaModel* pExtMdl = LookupModelFromPartialName(sExtMdlPartialName.c_str());
			if (pExtMdl)
			{
				m_vExtends.push_back(pExtMdl);
				//������ࣺ������Ĳ������������Ϣȫ���ӵ�this
				pExtMdl->Parse();
			}
			continue;
		}
	}

	//����import
	for (int i = 0; i < m_vImportsStringList.size(); i++)
	{
		string sLine = m_vImportsStringList[i];
		string sImportMdlPartialName = GetImportMdlName(sLine.c_str()); //������
		if (!sImportMdlPartialName.empty()) //�˴�����assert
		{
			//���һ���ģ��
			ModelicaModel* pImportMdl = LookupModelFromPartialName(sImportMdlPartialName.c_str());
			if(pImportMdl)
				m_vImports.push_back(pImportMdl);
			
			continue;
		}
	}

	//��������
	for (int i = 0; i < m_vParameterStringList.size(); i++)
	{
		string sLine = m_vParameterStringList[i];
		myParameter* pParam;
		if (IsParameterDefine(sLine.c_str(), pParam))
		{
			m_vParameters.push_back(pParam);
		}
	}

	//����Component: outVar, Part, Connector
	for (int i = 0; i < m_vComponentsStringList.size(); i++)
	{
		string sLine = m_vComponentsStringList[i];
		//�ж��Ƿ��Ǳ����������ǵĻ�����ñ�����Ϣ��
		myOutputVar* pVar;
		if (IsVariableDefine(sLine.c_str(), pVar))
		{
			//���ж��Ƿ��ǽӿ��������ǣ����ýӿ���Ϣ
			vector<string> vTypes = { "Integer","Real","String","Boolean" }; //����������
			if (find(vTypes.begin(), vTypes.end(), pVar->sType) != vTypes.end())
			{
				m_vOutVars.push_back(pVar);
				continue;
			}

			//�õ�����ȫ��
			ModelicaModel* pPartMdl = LookupModelFromPartialName(pVar->sType.c_str());
			string sType;
			if (pPartMdl) //����Ǽ���䣬pPartMdlΪnull
			{
				sType = pPartMdl->m_sType;
			}
			if (sType == "connector" || sType == "class" || sType == "model" || sType == "block" || sType == "record")
			{
				//�ж��Ƿ�Ϊ�������������ǣ���������ģ��
				pPartMdl->Parse();
				m_mapParts.insert(make_pair(pVar->sFullName,pPartMdl)); //��ʱ������Ϊ����

				if (sType == "connector")
				{//���������
					Connector* pCon = new Connector();
					pCon->SetConnName(pVar->sFullName.c_str());
					//pCon->SetConnType(pVar->sType);
					pCon->SetConnType(pPartMdl->GetFullName());
					m_vConnectors.push_back(pCon);
				}
				delete pVar;
			}
			else {
				//һ���������
				m_vOutVars.push_back(pVar);
			}
		}
	}

	m_bParsed = true;
}

//�ݹ���Cyber��Ϣ���������������ӿ�
void RecurseGetCyberInfo(ModelicaModel* pMdl, string sPre, CyberInfo* pCyberInfo)
{
	//�Ƚ�pMdl����Ϣ��:�������������ӿ�
	pCyberInfo->AddParameters(pMdl,pMdl->m_vParameters, sPre);
	pCyberInfo->AddOutputVars(pMdl->m_vOutVars, sPre);
	//�ӿڣ�ֻ�ӵ�һ��
	if (sPre.empty())
	{
		vector<string> vConnNames, vConnTypes;
		for (int i = 0; i < pMdl->m_vConnectors.size(); i++)
		{
			Connector* pConn = pMdl->m_vConnectors[i];
			vConnNames.push_back(pConn->GetConnName());
			vConnTypes.push_back(pConn->GetConnType());
		}
		pCyberInfo->AddConnNames(vConnNames);
		pCyberInfo->AddConnTypes(vConnTypes);
	}

	//�ݹ� ���������Ϣ��
	for (int i = 0; i < pMdl->m_vExtends.size(); i++)
	{
		ModelicaModel* pExtMdl = pMdl->m_vExtends[i];
		RecurseGetCyberInfo(pExtMdl, sPre, pCyberInfo);
	}

	//�ݹ� ���������Ϣ
	for (auto itr = pMdl->m_mapParts.begin(); itr != pMdl->m_mapParts.end(); ++itr)
	{
		string sPre0 = sPre;
		string sPartName = itr->first;
		ModelicaModel* pPartMdl = itr->second;

		if (sPre0.empty())
		{
			sPre0 = sPartName;
		}
		else {
			sPre0 += "." + sPartName;
		}

		//����Ӹ�Ŀ¼
		//vector<myParameter*> vParams;
		//myParameter* pPar = new myParameter;
		//pPar->sFullName = sPartName;
		//pPar->sType = pPartMdl->m_sName;
		//pPar->bFocused = false;
		//vParams.push_back(pPar);
		//pCyberInfo->AddParameters(pMdl,vParams, sPre);
		//vector<myOutputVar*> vVars;
		//myOutputVar* pVar = new myOutputVar();
		//pVar->sFullName = sPartName;
		//pVar->sType = pPartMdl->m_sName;
		//pVar->bFocused = false;
		//vVars.push_back(pVar);
		//pCyberInfo->AddOutputVars(vVars, sPre);

		RecurseGetCyberInfo(pPartMdl, sPre0, pCyberInfo);
	}
}

//��Modelicaģ�͹���CyberInfo����
CyberInfo* ModelicaModel::GenerateCyberInfo()
{
	CyberInfo* pCyberInfo = new CyberInfo(nullptr);
	pCyberInfo->SetMdlName(m_sName);

	RecurseGetCyberInfo(this, "", pCyberInfo);

	return pCyberInfo;
}

//�ӵ�ǰģ��λ�á�importģ�ͣ����ݲ������ֲ���ģ��
ModelicaModel* ModelicaModel::LookupModelFromPartialName(const char* sPartialName)
{
	ModelicaModel* theModel;

	//�Ȱ�������ȫ������
	theModel = LookupModel(sPartialName);
	if (theModel)
	{
		return theModel;
	}
	//��ͷ��
	string sHeadName = sPartialName;
	int pos = sHeadName.find('.');
	if (pos > 0)
	{
		sHeadName = sHeadName.substr(0, pos);
	}
	//�ȴӵ�ǰģ��λ�ã���Բ���
	string sFullMdlName = GetFullName();
	pos = sFullMdlName.rfind('.');
	//assert(pos > 0);
	while (pos != string::npos)
	{
		string sShortName = sFullMdlName.substr(pos + 1);
		sFullMdlName = sFullMdlName.substr(0, pos);
		pos = sFullMdlName.rfind('.');
		if (1) //sShortName == sHeadName)
		{
			sFullMdlName += ".";
			sFullMdlName += sPartialName;

			theModel = LookupModel(sFullMdlName);
			if (theModel)
			{
				return theModel;
			}
		}
	}

	//assert(false);
	
	//����Import�������Ͳ���
	theModel = LookupMdlFromImports(sPartialName);
	//for (int i = 0; i < m_vImports.size(); i++)
	//{
	//	ModelicaModel* pImportMdl = m_vImports[i];
	//	string sFullMdlNameI = pImportMdl->GetFullName();
	//	int posI = sFullMdlNameI.find('.');
	//	if (posI != string::npos)
	//	{
	//		sFullMdlNameI = sFullMdlNameI.substr(pos + 1); //��ģ��
	//		sFullMdlNameI += sPartialName;
	//	}
	//	else {
	//		sFullMdlNameI = sPartialName;
	//	}

	//	theModel = LookupModel(sFullMdlNameI);
	//	if (theModel)
	//	{
	//		return theModel;
	//	}
	//}

	//û���ҵ�
	return theModel;
}

//�ݹ�Ӹ���Imports�в��Ҳ�����������ģ��
ModelicaModel* ModelicaModel::LookupMdlFromImports(const char* sPartialName)
{
	ModelicaModel* theModel;
	theModel = LookupModel(sPartialName);
	if (theModel)
	{
		return theModel;
	}

	//�õ�ǰ������
	QString sTypeName = sPartialName;
	sTypeName.trimmed();
	int pos = sTypeName.indexOf(".");
	QString theFormer = sTypeName;
	QString theLatter = sTypeName;
	if(pos >= 1);
	{
		theFormer = sTypeName.left(pos);
		theLatter = sTypeName.right(sTypeName.length() - pos - 1);
	}

	ModelicaModel* pMdl = this;
	bool bFound = false;
	while (pMdl)
	{
		auto vImports = pMdl->m_vImportsStringList;
		for (auto itr = vImports.begin(); itr != vImports.end(); ++itr)
		{
			QString sImport = (*itr).c_str();
			sImport.replace("import", "");
			sImport = sImport.trimmed();
			//�ȿ� = �ָ�ļ�����ȫ��
			QStringList qsl = sImport.split("=");
			if (qsl.length() == 2)
			{
				QString theShortName = qsl[0];
				theShortName = theShortName.trimmed();
				if (theShortName == theFormer)
				{
					QString theFullName = qsl[1];
					theFullName.replace(";", "");
					theFullName = theFullName.trimmed();
					QString theModelTypeName = theFullName + "." + theLatter;
					theModel = LookupModel(theModelTypeName.toStdString());
					if (theModel)
					{
						bFound = true;
						break;
					}
				}
			}
			else //�� = ��import
			{
				int pos1 = sImport.lastIndexOf(".");
				assert(pos1 > 0);
				QString sTheFirst = sImport.left(pos1);
				QString sTheLast = sImport.right(sImport.length() - pos1 - 1);
				if (sTheLast == theFormer)
				{
					QString theModelTypeName = sImport + "." + theLatter;
					theModel = LookupModel(theModelTypeName.toStdString());
					if (theModel)
					{
						bFound = true;
						break;
					}
				}
			}
		}

		if (bFound)
		{
			break;
		}
		pMdl = pMdl->m_pParent;
	}

	return theModel;
}
