////////////////////////////////////2021.10//////
//Design by YZ Wu
/////////////////////////////////////////////////
#include "M3d.h"
#include "cyberInfo.h"
#include <QString>
#include <QFile>
#include <QFileInfo>
#include "global.h"
#include "DocumentComponent.h"
#include "ModelicaModel.h"

extern bool IsClassDefining(QString sLine, QString& sNameReturn, QString& sTypeReturn);
extern bool IsClassEnd(QString sLine, QString sModelName);
extern string GetSentenceFromPosToChar(string& subStr, int& pos, char to);
/////////////////////////////////////////////////
// class CyberInfo
/////////////////////////////////////////////////
CyberInfo::CyberInfo(DocumentComponent* pDoc)
{
	m_pDoc = pDoc;
}

CyberInfo::CyberInfo(DocumentComponent* pDoc,const char* sFullName)
{
	m_pDoc = pDoc;
	m_sMdlFullName = sFullName;
}

CyberInfo::~CyberInfo()
{
	//����������������
	vector<myParameter*>::iterator itr;
	for (itr = m_Parameters.begin(); itr != m_Parameters.end(); ++itr)
	{
		myParameter* pPar = *itr;
		delete pPar;
	}
	m_Parameters.clear();

	vector<myOutputVar*>::iterator itr2;
	for (itr2 = m_OutputVars.begin(); itr2 != m_OutputVars.end(); ++itr2)
	{
		myOutputVar* pVar = *itr2;
		delete pVar;
	}
	m_OutputVars.clear();
}

//�˺������ܣ�����ģ��ȫ������ȡModelicaģ�͵�ȫ����Ϣ��ͨ��OpenModelica��MWorks���ο����ӿ�
//����������ʵ��
extern bool GetModelicaFileModelInfo(string sMdlFullName, vector<myParameter*>& vPars, vector<myOutputVar*>& vVars, vector<string>& vConnectTypes)
{
	return false;
}

//����ģ��ȫ����ü���
QString GetShortName(const char* sFullName)
{
	QString sShName = sFullName;

	int pos = sShName.lastIndexOf(".");
	if (pos > 0)
	{
		sShName = sShName.right(sShName.length() - pos - 1);
	}

	return sShName;
}

//����ģ�͵Ĳ������֣�����ȫ��
//vDirΪͬһ����ǰ׺����
//vImportsΪ�����ȫ������
string GetModelFullName(string sMdlPartialName, vector<string> vDirs, vector<string> vImports)
{
	string sMdlFullName = sMdlPartialName;

	//�Ƿ�Ϊȫ����
	if (sMdlFullName.find("Modelica.") == 0)
	{
		return sMdlFullName;
	}

	//�����·����
	for (int i = 0; i < vDirs.size(); i++)
	{
		string sDir_i = vDirs[i]; //A.B.C
		string sShortName = sDir_i;
		int pos = sShortName.rfind('.');
		if (pos > 0)
		{
			sShortName = sShortName.substr(pos + 1); //C
			sDir_i = sDir_i.substr(0, pos);
		}

		sShortName += ".";
		if (sMdlPartialName.find(sShortName) == 0) //C.X
		{
			sMdlFullName = sDir_i + "." + sMdlPartialName; //A.B.C.X
			return sMdlFullName;
		}
	}

	//��import�����ң�������һ������
	for (int i = 0; i < vImports.size(); i++)
	{
		string sDir_i = vImports[i]; //A.B.C
		string sShortName = sDir_i;
		int pos = sShortName.rfind('.');
		if (pos > 0)
		{
			sShortName = sShortName.substr(pos + 1); //C
			sDir_i = sDir_i.substr(0, pos);
		}

		sShortName += ".";
		if (sMdlPartialName.find(sShortName) == 0) //C.X
		{
			sMdlFullName = sDir_i + "." + sMdlPartialName; //A.B.C.X
			return sMdlFullName;
		}
	}

	return sMdlFullName;
}

//ͨ������ �� ' ', '[', '(', ';', '=', '\"'
QString GetFirstWord(QString sLine)
{
	QString sRetWord = sLine;
	//ȥ���س�deng
	sLine.replace("\n", " ");
	sLine.replace("\t", " ");
	sLine = sLine.trimmed();

	int pos = 10000;
	int pos1 = sLine.indexOf(' ');
	if (pos1 > 0) pos = pos1;

	int pos2 = sLine.indexOf('[');
	if (pos2 > 0 && pos2 < pos) pos = pos2;

	int pos3 = sLine.indexOf('(');
	if (pos3 > 0 && pos3 < pos) pos = pos3;

	int pos6 = sLine.indexOf('\"');
	if (pos6 > 0 && pos6 < pos) pos = pos6;

	int pos4 = sLine.indexOf('=');
	if (pos4 > 0 && pos4 < pos) pos = pos4;

	int pos5 = sLine.indexOf(';');
	if (pos5 > 0 && pos5 < pos) pos = pos5;

	assert(pos > 0 && pos < 10000);

	sRetWord = sLine.left(pos);
	return sRetWord;
}

//�Ƿ�Ϊextends�Ķ��壬����ǣ����û����ģ��
bool IsExtends(QString sLine, string& sExtMdlPartialName)
{
	sLine = sLine.trimmed();
	sLine.replace("  ", " ");

	if (sLine.left(8) == "extends ")
	{
		sLine = sLine.mid(8);
		sExtMdlPartialName = GetFirstWord(sLine).toStdString();

		return true;
	}
	
	return false;
}

//��ȡ����������Ϣ
bool IsParameterDefine(QString sLine, myParameter*& pParam)
{
	sLine.replace("final ", " ");
	sLine = sLine.trimmed();
	sLine.replace("\n", " ");
	sLine.replace("  ", " ");

	if (sLine.indexOf("parameter ") == 0)
	{
		pParam = new myParameter();
		pParam->bFocused = false;
		pParam->sFocusName = "";
		//1.��������
		sLine = sLine.right(sLine.length() - 10);
		int pos = sLine.indexOf(' ');
		assert(pos > 0);
		pParam->sType = sLine.left(pos).toStdString() ;//�������ͣ����ﲻ�����ˣ�
		//2.�ұ�����,������������ո�=, (, [ ��;
		sLine = sLine.mid(pos + 1);
		pParam->sFullName = GetFirstWord(sLine).toStdString(); //�˴�ֻ���Ƕ���
		sLine = sLine.mid(pParam->sFullName.length());
		sLine = sLine.trimmed();
		//3.��ά��
		pParam->sDimension = " ";
		int pos1 = sLine.indexOf('[');
		int pos2 = sLine.indexOf(']');
		if (pos1 >= 0 && pos2 > 0)
		{
			pParam->sDimension = sLine.mid(pos1, pos2 - pos1 + 1).toStdString();
		}
		//4.�ҵ�λ
		pParam->sUnit = " ";
		QString sTmp = sLine;
		sTmp.replace(" ",""); //ȥ���ո�
		int pos3 = sTmp.indexOf("(unit=");
		if (pos3 >= 0)
		{
			int pos41 = sTmp.indexOf("\"", pos3);
			if (pos41 > 0)
			{
				int pos42 = sTmp.indexOf("\"", pos41 + 1);
				if (pos42 > 0)
				{
					pParam->sUnit = sTmp.mid(pos41 + 1, pos42 - pos41 - 1).toStdString();
				}
			}
		}
		//5.��ȱʡֵ
		pParam->defaultValue = " ";
		sLine.replace("\n", " ");
		sLine.replace("  ", " ");
		sLine.replace("unit=", ""); sLine.replace("unit =", "");
		sLine.replace("start=", ""); sLine.replace("start =", "");
		sLine.replace("min=", ""); sLine.replace("min =", "");
		sLine.replace("max=", ""); sLine.replace("max =", "");
		int pos6 = sLine.indexOf('=');
		if (pos6 >= 0)
		{
			sLine = sLine.mid(pos6+1);
			pos6 = sLine.indexOf('\"');//�˴���bug
			if (pos6 >= 0)
			{
				sLine = sLine.left(pos6);
			}
			else {
				pos6 = sLine.indexOf(';');
				sLine = sLine.left(pos6);
			}
			pParam->defaultValue = sLine.toStdString();// GetFirstWord(sLine).toStdString();
		}

		return true;
	}

	return false;
}

bool IsComponentDefine(QString& sLine)
{
	//ȥǰ׺
	sLine.replace('\n', ' ');
	sLine.replace("flow ", "");
	sLine.replace("input ", "");
	sLine.replace("output ", "");
	sLine.replace("inner ", "");
	sLine.replace("outer ", "");
	sLine.replace("discrete ", "");
	sLine.replace("protected ", "");
	sLine.replace("public ", "");

	sLine = sLine.trimmed();

	if (sLine.isEmpty() || sLine == "//" )//ע������
	{
		return false;
	}

	//ȥ�����ſ�ͷ�Ľ�����
	if (sLine.indexOf("\"") == 0) {
		return false;
	}

	//ȥ��within
	if (sLine.indexOf("within ") == 0) {
		return false;
	}

	//ȥ��annotation
	if (sLine.indexOf("annotation") == 0) {
		return false;
	}

	//ȥ��parameter
	if (sLine.indexOf("parameter") == 0) {
		return false;
	}

	//ȥ��constant
	if (sLine.indexOf("constant") == 0) {
		return false;
	}

	return true;
}

//����������壺
bool IsVariableDefine(QString sLine, myOutputVar*& pVar)
{
	if (!IsComponentDefine(sLine))
	{
		return false;
	}

	//�������ǰ���Ѿ��ų�������

	pVar = new myOutputVar();
	pVar->bFocused = false;
	pVar->sFocusName = "";
	pVar->sType = GetFirstWord(sLine).toStdString();
	//����
	if (pVar->sType == "extends")
	{
		assert(false);
	}
	sLine = sLine.mid(pVar->sType.length() + 1);
	sLine = sLine.trimmed();
	pVar->sFullName = GetFirstWord(sLine).toStdString();

	return true;
}


//����ģ��ȫ������mo�ļ�·��������
//��һ�㣬ֻ�����һ����mo������һ����mo����ģ��
//�˺������ڲ��㣬��ʱ������...
string LookupModel(string sMdlFullName)
{
	string sMoPath;

	assert(false);

	return sMoPath;
}

string  GetModelType(const char* sFullMdlName, const char* sMoPath)
{
	string sRetType = "package";

	QString sShortName = GetShortName(sFullMdlName);

	QFile qFile(sMoPath);
	qFile.open(QIODevice::ReadOnly);
	string sFullText = qFile.readAll();
	qFile.close();

	int pos = 0;
	while (pos < sFullText.length())
	{
		QString sLine = GetSentenceFromPosToChar(sFullText, pos, '\n').c_str();
		QString sName, sType;
		if (IsClassDefining(sLine, sName, sType) && sName == sShortName)
		{
			sRetType = sType.toStdString();
			break;
		}
	}

	return sRetType;
}

void RecurseCreateCyberInfo(CyberInfo* pCyber, const char* sMdlName, const char* sMoPath, vector<myParameter*>& vParams, vector<myOutputVar*>& vVars, vector<string>& vConnectNames, vector<string>& vConnectTypes)
{
	QFile qFile(sMoPath);
	qFile.open(QIODevice::ReadOnly);
	string sFullText = qFile.readAll();
	qFile.close();

	QString sName;
	int pos = 0;
	//�ȶ�λ����ǰģ��λ��;
	while (pos < sFullText.length())
	{
		QString sLine = GetSentenceFromPosToChar(sFullText, pos, '\n').c_str();
		sLine = sLine.trimmed();

		QString sType;
		QString sShortName = GetShortName(sMdlName);
		if (IsClassDefining(sLine, sName, sType) && (sName == sShortName))
		{
			break;
		}
		else {
			continue;
		}
	}
	//�Ӹ�ģ�Ͷ������һ�п�ʼ��
	while (pos < sFullText.length())
	{
		int oldPos = pos;
		QString sLine = GetSentenceFromPosToChar(sFullText, oldPos, '\n').c_str();
		sLine = sLine.trimmed();

		//����end����
		if (IsClassEnd(sLine, sName)) break;
		
		//�����Ƕ��ģ�Ͷ��壬��һֱ��������
		QString sEnbedName, sEnbedType;
		if ( IsClassDefining(sLine, sEnbedName, sEnbedType)) 
		{
			while (1) {
				sLine = GetSentenceFromPosToChar(sFullText, oldPos, '\n').c_str();
				sLine = sLine.trimmed();
				if (sLine.isEmpty()) continue; //����

				if (IsClassEnd(sLine, sEnbedName))
				{//���������Ƕ��ģ�;��˳�
					break;
				}
			}
			pos = oldPos;
			continue; //������
		}

		//����Ƿ��̻��㷨��ʼ����ȫ�����굽end 
		if (sLine.indexOf("equation") == 0 || sLine.indexOf("initial equation") == 0 || sLine.indexOf("algorithm") == 0 || sLine.indexOf("initial algorithm") == 0)
		{
			pos = oldPos;
			while (pos < sFullText.length())
			{
				sLine = GetSentenceFromPosToChar(sFullText, pos, ';').c_str();
				sLine = sLine.trimmed();
				if (sLine.indexOf("end ") == 0)
				{
					break;
				}
			}
		}

		//������ǣ����ԭ����λ�ö���һ�䣻
		sLine = GetSentenceFromPosToChar(sFullText, pos, ';').c_str();

		//�ж��Ƿ��ǻ������������ǣ���õ������CyberInfo
		string sExtMdlPartialName;
		if (IsExtends(sLine, sExtMdlPartialName))
		{
			//���һ���ģ�͵�mo·��
			vector<string> vDirs, vImports; //���ڻ�û�д�����
			string sExtMdlFullName = GetModelFullName(sExtMdlPartialName, vDirs, vImports);
			string sExtMoPath = LookupModel(sExtMdlFullName);
		
			//CyberInfo* pExtendCyber = new CyberInfo(sExtMdlFullName.c_str());
			RecurseCreateCyberInfo(pCyber, sExtMdlFullName.c_str(), sExtMoPath.c_str(), vParams, vVars, vConnectNames, vConnectTypes);
			continue;
		}

		//�ж��ǲ����������ǵĻ�����ò�������Ϣ
		myParameter* pParam;
		if (IsParameterDefine(sLine, pParam))
		{
			vParams.push_back(pParam);
			continue;
		}

		//�ж��Ƿ��Ǳ����������ǵĻ�����ñ�����Ϣ��
		myOutputVar* pVar;
		if (IsVariableDefine(sLine, pVar))
		{
			//���ж��Ƿ��ǽӿ��������ǣ����ýӿ���Ϣ
			//�õ�����ȫ��
			vector<string> vDirs, vImports; //���ڻ�û�д�����
			string sFullMdlName = GetModelFullName(pVar->sType, vDirs, vImports);
			string sMoPath = LookupModel(sFullMdlName); //�ҵ�Mo�ļ�
			string sType = GetModelType(sFullMdlName.c_str(), sMoPath.c_str());

			if (sType == "connector" || sType == "class" || sType == "model" || sType == "block" || sType == "record")
			{
				//�ж��Ƿ�Ϊ�������������ǣ��򴴽����ģ�͵�CyberInfo
				CyberInfo* pPartCyber = new CyberInfo(nullptr,sFullMdlName.c_str());
				vector<myParameter*> v_Pms;
				vector<myOutputVar*> v_Vars;
				vector<string> v_conNames, vconnTypes;
				RecurseCreateCyberInfo(pPartCyber, sFullMdlName.c_str(), sMoPath.c_str(), v_Pms, v_Vars, v_conNames, vconnTypes);
				pPartCyber->SetParameters(v_Pms);
				pPartCyber->SetOutputVars(v_Vars);
				pPartCyber->SetConncNames(v_conNames); pPartCyber->SetConncTypes(vconnTypes);
				pCyber->mapParts.insert(make_pair(pVar->sFullName, pPartCyber));

				if (sType == "connector")
				{
					vConnectNames.push_back(pVar->sFullName); //��ʱΪ����
					vConnectTypes.push_back(pVar->sType/*sFullMdlName*/); //
				}
			}
			else {
				//һ���������
				vVars.push_back(pVar);
			}
		}
	}
}

bool CyberInfo::BuildCyberInfo(const char* sMoPath)
{
	bool bRet = true;

	RecurseCreateCyberInfo(this, m_sMdlFullName.c_str(),sMoPath, m_Parameters, m_OutputVars, m_vConnectorNames, m_vConnetorTypes);
	//bool bRet = GetModelicaFileModelInfo(m_sMdlFullName, m_Parameters, m_OutputVars, m_vConnetorTypes);

	return bRet;
}

void RecurseGetCyberInfo(CyberInfo* pInfo, const char* csPrex, vector<myParameter*>& vParams, vector<myOutputVar*>& vVars)
{
	//vParams.insert(vParams.end(), pInfo->GetParameters().begin(), pInfo->GetParameters().end());
	//vVars.insert(vVars.end(), pInfo->GetOutputVars().begin(), pInfo->GetOutputVars().end());
	vector<myParameter*> vOldParams = pInfo->GetParameters();
	vector<myOutputVar*> vOldVars = pInfo->GetOutputVars();
	for (int i = 0; i < vOldParams.size(); i++)
	{
		myParameter* pPar = vOldParams[i];

		string sPrefix = csPrex;
		if (!sPrefix.empty())
		{//����ǰ׺
			pPar->sFullName = sPrefix + "." + pPar->sFullName;
		}

		vParams.push_back(pPar);
	}
	for (int i = 0; i < vOldVars.size(); i++)
	{
		myOutputVar* pPar = vOldVars[i];

		string sPrefix = csPrex;
		if (!sPrefix.empty())
		{//����ǰ׺
			pPar->sFullName = sPrefix + "." + pPar->sFullName;
		}

		vVars.push_back(pPar);
	}

	//�����Ĳ����ͱ���
	auto itr = pInfo->mapParts.begin();
	for (; itr != pInfo->mapParts.end(); ++itr)
	{
		string sPartName = itr->first;
		CyberInfo* pPartInfo = itr->second;

		string sNewPrix = csPrex;
		if (sNewPrix.empty())
		{
			sNewPrix = sPartName;
		}
		else {
			sNewPrix += "." + sPartName;
		}
		RecurseGetCyberInfo(pPartInfo, sNewPrix.c_str(),vParams, vVars);

		//ɾ��
		delete pPartInfo;
	}
}

/*
* 1.���ö���Ϊfalse 
	animateWorld=false,
    animateGravity=false,
    enableAnimation=false
	useAxisFlange=true��???
	animation=false
    useQuaternions = false,
	2.�Թ�ע�������ֽ����Զ����� m,r_CM,nX,n_X,nY,n_Y,nZ,n_Z,\
	r_ab, r_ba, I11~I33
*/
void CyberInfo::AutomaticComputing()
{
	for (myParameter* pPar : m_Parameters)
	{
		//1.
		vector<string> vKeyWords = { "animation","useQuaternions","enableAnimation",\
     		 "animateWorld","animateGravity", "animateGround" };
		if ( find(vKeyWords.begin(), vKeyWords.end(), pPar->sFullName)!= vKeyWords.end())
		{
			pPar->defaultValue = "false";
		}
		else if (pPar->sFullName == "useAxisFlange")
		{
			pPar->defaultValue = "true";
		}

		//2.
		if (pPar->bFocused && pPar->sFocusName == "m") //����
		{
			double m_Kg = m_pDoc->ComputeM();
			pPar->defaultValue = DoubleToQString(m_Kg).toStdString();
		}
		else if (pPar->bFocused && pPar->sFocusName == "r_CM") //����
		{
			gp_Vec r_CM = m_pDoc->ComputeR_CM();//��frame_a��CM��ʸ��
			pPar->defaultValue = GetVectorString(r_CM).toStdString();
		}
		else if (pPar->bFocused && (pPar->sFocusName == "r_ab")||(pPar->sFocusName == "r") )
		{
			gp_Vec r_ab = m_pDoc->ComputeR_ab();
			pPar->defaultValue = GetVectorString(r_ab).toStdString();
		}
		else if (pPar->bFocused && pPar->sFocusName == "r_ba")
		{
			gp_Vec r_ba = -m_pDoc->ComputeR_ab();
			pPar->defaultValue = GetVectorString(r_ba).toStdString();
		}
		else if (pPar->bFocused && pPar->sFocusName[0] == 'n')
		{
			if (pPar->sFocusName == "nX")
			{
				pPar->defaultValue = "{1,0,0}";
			}
			else if (pPar->sFocusName == "n_X")
			{
				pPar->defaultValue = "{-1,0,0}";
			}
			else if (pPar->sFocusName == "nY")
			{
				pPar->defaultValue = "{0,1,0}";
			}
			else if (pPar->sFocusName == "n_Y")
			{
				pPar->defaultValue = "{0,-1,0}";
			}
			else if (pPar->sFocusName == "nZ")
			{
				pPar->defaultValue = "{0,0,1}";
			}
			else if (pPar->sFocusName == "n_Z")
			{
				pPar->defaultValue = "{0,0,-1}";
			}
		}
		else if (pPar->bFocused && pPar->sFocusName[0] == 'I')
		{
			gp_Mat aMOI = m_pDoc->ComputeMoment();
			int nRow, nCol;
			nRow = atoi(pPar->sFocusName.substr(2, 1).c_str());
			nCol = atoi(pPar->sFocusName.substr(3, 1).c_str());
			
			assert(nRow >= 1 && nRow <= 3); 
			assert(nCol >= 1 && nCol <= 3);
			pPar->defaultValue = DoubleToQString(aMOI.Value(nRow, nCol)).toStdString();
		}
	}
}


//ƽ̹���������е�������Ĳ����ͱ����ӵ�һ��
void CyberInfo::Flattening()
{
	m_Parameters.clear();
	m_OutputVars.clear();
	RecurseGetCyberInfo(this, "", m_Parameters, m_OutputVars);
}

//Get...
string CyberInfo::GetMdlName() const
{
	return m_sMdlFullName;
}

string CyberInfo::GetMdlType() const
{
	return m_sModelType;
}

//����ȫ�����Ҳ���
myParameter* CyberInfo::GetParameter(const char* sPar) const
{
	auto itr = m_Parameters.begin();

	for (; itr != m_Parameters.end(); ++itr)
	{
		myParameter* pPar = *itr;

		if (pPar->sFullName == sPar)
		{
			myParameter* parRet = new myParameter();
			parRet->bFocused = pPar->bFocused;
			parRet->defaultValue = pPar->defaultValue;
			parRet->sDimension = pPar->sDimension;
			parRet->sFocusName = pPar->sFocusName;
			parRet->sFullName = pPar->sFullName;
			parRet->sType = pPar->sType;
			parRet->sUnit = pPar->sUnit;
			
			return parRet;
		}
	}

	//û�ҵ�
	return nullptr;
}

vector<myParameter*> CyberInfo::GetParameters() const
{
	return m_Parameters;
}

vector<myOutputVar*> CyberInfo::GetOutputVars() const
{
	return m_OutputVars;
}

vector<string>   CyberInfo::GetConnectorTypes() const
{
	return m_vConnetorTypes;
}

vector<string>   CyberInfo::GetConnectorNames() const
{
	return m_vConnectorNames;
}

//Add...
void CyberInfo::AddParameters(ModelicaModel* pMdl, vector<myParameter*> vParams, string sPre)
{
	for (int i = 0; i < vParams.size(); i++)
	{
		myParameter* pParm = vParams[i];
		myParameter* newParam = new myParameter();
		newParam->bFocused = pParm->bFocused;
		newParam->defaultValue = pParm->defaultValue;
		newParam->sDimension = pParm->sDimension;
		newParam->sFocusName = pParm->sFocusName;
		newParam->sFullName = pParm->sFullName;
		newParam->sType = pParm->sType;
		vector<string> vTypes = { "Integer","Real","String","Boolean" }; //����������
		if (find(vTypes.begin(), vTypes.end(), pParm->sType) == vTypes.end())
		{
			//����ȫ������Import������Import; ���༰������Import��
			//if (pParm->sType == "SI.Position")
			//{
			//	assert(false);
			//}
			ModelicaModel* pParamModel = pMdl->LookupModelFromPartialName(pParm->sType.c_str());
			//assert(pParamModel);
			if(pParamModel)
				newParam->sType = pParamModel->GetFullName();
		}
		newParam->sUnit = pParm->sUnit;
		if (!sPre.empty())
		{
			newParam->sFullName = sPre + "." + pParm->sFullName;
		}
		m_Parameters.push_back(newParam);
	}
}

void CyberInfo::AddOutputVars(vector<myOutputVar*> vVars, string sPre)
{
	for (int i = 0; i < vVars.size(); i++)
	{
		myOutputVar* pVar = vVars[i];
		myOutputVar* newVar = new myOutputVar();
		newVar->bFocused = pVar->bFocused;
		newVar->sFocusName = pVar->sFocusName;
		newVar->sFullName = pVar->sFullName;
		newVar->sType = pVar->sType;
		if (!sPre.empty())
		{
			newVar->sFullName = sPre + "." + pVar->sFullName;
		}
		m_OutputVars.push_back(newVar);
	}
}

void CyberInfo::AddConnNames(vector<string> vNames) //�ӿڲ���ǰ׺
{
	m_vConnectorNames.insert(m_vConnectorNames.end(), vNames.begin(), vNames.end());
}

void CyberInfo::AddConnTypes(vector<string> vTypes)
{
	m_vConnetorTypes.insert(m_vConnetorTypes.end(), vTypes.begin(), vTypes.end());
}


//Set...
void CyberInfo::SetMdlName(string sFullName)
{
	m_sMdlFullName = sFullName;
}

void CyberInfo::SetMdlType(const char* sTypeName)
{
	m_sModelType = sTypeName;
}

//���漸��Set����������...
void CyberInfo::SetParameters(vector<myParameter*>& vPars)
{
	m_Parameters = vPars;
}

void CyberInfo::SetOutputVars(vector<myOutputVar*>& vVars)
{
	m_OutputVars = vVars;
}

void CyberInfo::SetConncTypes(vector<string>& vTypes)
{
	m_vConnetorTypes = vTypes;
}

void CyberInfo::SetConncNames(vector<string>& vNames)
{
	m_vConnectorNames = vNames;
}
