#include "DocumentModel.h"
#include <map>
#include <string.h> 
#include "DocumentModel.h"
//#include "DocumentComponent.h"
#include "Connector.h"

using namespace std;

//�ָ��ַ�����������ָ���ַ���s�Լ��ָ��sep
vector<string> split(string s, char sep)
{
	istringstream iss(s);
	vector<string> res;
	string buffer;
	while (getline(iss, buffer, sep)) {
		res.push_back(buffer);
	}
	return res;
}

//����
void IdentString(int level, string& sReturnCode)
{
	//����
	for (int indent = 0; indent < level; indent++)
	{
		sReturnCode += "  ";//2���ո�
	}
}

//��������װ���壬���������
myParameter* DocumentModel::GetParameterInfo(const string& sMapName) const
{
	myParameter* pPar;

	//�õ�����������
	vector<string> vStr = split(sMapName, '.');
	assert(vStr.size() == 2);

	string sComName = vStr[0];
	string sParName = vStr[1];
	Component* pComChild = m_pCompRoot->GetChild(sComName.c_str());
	assert(pComChild);

	if (pComChild->IsAssembly())
	{
		//����ǲ��������ҵ����Ĳ���ֵ ���ٵõ���ӳ��������������ݹ�
		DocumentModel* pMdl = (DocumentModel*)pComChild->GetDocument();
		ParameterValue pv;
		if (pMdl->GetParameterValue(sParName.c_str(), pv))
		{
			string sTheMapName = pv.sMapParamName;
			//assert(sTheMapName);
			//�ݹ�
			return GetParameterInfo(sTheMapName);
		}
		else {
			return nullptr;
		}
	}
	else {
		//���
		CyberInfo* pCyber = pComChild->GetCyberInfo();
		pPar = pCyber->GetParameter(sParName.c_str());
		delete pCyber;

		return pPar;
	}

	return nullptr;
}

//����Modelica����
bool DocumentModel::GenerateModelicaCode(string& sReturnCode,int level)
{
	//model �����
	string compName = m_pCompRoot->GetCompName();

	IdentString(level, sReturnCode);
	sReturnCode += "model ";
	sReturnCode += compName;
	sReturnCode += "\n";

	//1.��������
	//eg��parameter SI.Position rLoad[3] = {0.1, 0.25, 0.1}
	vector<ParameterValue> allPars = this->GetAllParameterValues();
	for (vector<ParameterValue>::iterator iter = allPars.begin(); iter != allPars.end(); iter++)
	{
		ParameterValue pv = *iter;
		//sParamName��Ϊ��ʱ
		if (! pv.sParamName.empty())
		{
			IdentString(level, sReturnCode);

			//��ò������͡�����ά��
			myParameter* par = GetParameterInfo(pv.sMapParamName);

			sReturnCode += "  ";
			sReturnCode += "parameter ";
			//�������ͣ���
			sReturnCode += par->sType;
			sReturnCode += " ";
			sReturnCode += pv.sParamName;
			//����ά�ȣ���
			sReturnCode += par->sDimension;
			sReturnCode += "=";
			sReturnCode += pv.sParamValue;
			sReturnCode += ";";
			sReturnCode += "\n";

			delete par;
		}
	}

	//2.������������---------������� �����(�������)
	//eg��Components.MechanicalStructure mechanics(mLoad = mLoad,rLoad = rLoad,g = g)
	//�Ӹ��ڵ����component��
	if (m_pCompRoot)
	{
		list<Component*> comChilds = m_pCompRoot->GetChilds();
		list<Component*>::iterator itr = comChilds.begin();
		vector<string> splitCompMdl; //��'.'�ָ���������
		for (; itr != comChilds.end(); ++itr)
		{
			Component* pComChild = *itr;
			if (pComChild->IsAssembly())
			{
				string scode;
				DocumentModel* pChildMdl = (DocumentModel * )pComChild->GetDocument();
				if (pChildMdl->GenerateModelicaCode(scode, level++))
				{
					sReturnCode += scode;
					sReturnCode += "\n";
				}
				else
					return false;
			}

			IdentString(level, sReturnCode);
			sReturnCode += "  ";

			//2.1 ������ԣ�protected��inner��
			//ComponentAttrib   
			string childCompName = pComChild->GetCompName();//�����
			ComponentAttrib childCompAttr = this->GetComponentAttrib(childCompName.c_str());

			if (childCompAttr.bProtected)
			{
				sReturnCode += "protected ";
			}
			if (childCompAttr.bFinal)
			{
				sReturnCode += "final ";
			}
			if (childCompAttr.bInner)
			{
				sReturnCode += "inner ";
			}
			if (childCompAttr.bOuter)
			{
				sReturnCode += "outer ";
			}
			if (childCompAttr.bReplaceable)
			{
				sReturnCode += "replaceable ";
			}
			switch (childCompAttr.iConnectorMember)
			{
			case 0:
				break;
			case 1:
				sReturnCode += "flow ";
				break;
			case 2:
				sReturnCode += "stream ";
				break;
			}
			switch (childCompAttr.iVariablity)
			{
			case 0:
				break;
			case 1:
				sReturnCode += "constant ";
				break;
			case 2:
				sReturnCode += "parameter ";
				break;
			case 3:
				sReturnCode += "discrete ";
				break;
			}
			switch (childCompAttr.iCasuality)
			{
			case 0:
				break;
			case 1:
				sReturnCode += "input ";
				break;
			case 2:
				sReturnCode += "output ";
				break;
			}

			//2.2 �������
			CyberInfo* childCompCyIn = pComChild->GetCyberInfo();
			if (!childCompCyIn)
			{
				assert(false);
				continue;
			}
			string childCompMdl = childCompCyIn->GetMdlName();
			sReturnCode += childCompMdl;

			//2.3 �����        
			sReturnCode += " ";
			sReturnCode += childCompName;

			//2.4 ���������������Ĺ�ע������fixed������װ����Ĳ�����
			bool bHave = false; //����Ѿ��ж��壬��"(...,"��
			//��ǰ������Ĳ���ֵ����
			map<string, string> setParameterValues;
			for (vector<ParameterValue>::iterator iter1 = allPars.begin(); iter1 != allPars.end(); iter1++)
			{
				vector<string> mParaName;//��'.'�ָ��sMapParamName
				mParaName = split((*iter1).sMapParamName, '.');
				assert(mParaName.size() == 2);
				if (mParaName[0] == childCompName)
				{
					string sComParName = mParaName[1];
					string sComParValue;
					if ((*iter1).sParamName.empty())
					{
						sComParValue = (*iter1).sParamValue;
					}
					else
					{
						sComParValue = (*iter1).sParamName;
					}
					setParameterValues.insert(make_pair(sComParName,sComParValue));
				}
			}
			//����Ĺ�ע����
			if (!pComChild->IsAssembly())
			{
				CyberInfo* pCbInfo = pComChild->GetCyberInfo();
				vector<myParameter*> vParams = pCbInfo->GetParameters();
				for (int k = 0; k < vParams.size(); k++)
				{
					myParameter* pPar = vParams[k];
					if (pPar->bFocused && !pPar->defaultValue.empty())
					{
						setParameterValues.insert(make_pair(pPar->sFullName, pPar->defaultValue));
					}
					//delete pPar; //?
				}
				
				//��ע������fixed
				vector<myOutputVar*> vVars = pCbInfo->GetOutputVars();
				for (int k = 0; k < vVars.size(); k++)
				{
					myOutputVar* pVar = vVars[k];
					if (pVar->bFocused && !pVar->sFocusName.empty())// == "fixed")
					{//���ĳ������var�������˹�ע��Ϊ ���ͣ�������var(fixed=true)
						string sVarName_ = pVar->sFullName;
						sVarName_ += "(";
						sVarName_ += pVar->sFocusName;
						sVarName_ += ")";
						string sVarValue_ = "";
						setParameterValues.insert(make_pair(sVarName_, sVarValue_));
					}
					//delete pVar; //?
				}
				delete pCbInfo;
			}

			string sParameterSet;
			auto itr0 = setParameterValues.begin();
			for (; itr0 != setParameterValues.end(); ++itr0)
			{
				sParameterSet += itr0->first;
				if (!itr0->second.empty())
				{
					sParameterSet += "=";
					sParameterSet += itr0->second;
				}
				sParameterSet += ",";
			}
			if (!sParameterSet.empty())
			{
				//ȥ�����һ�����š�,"
				sParameterSet = sParameterSet.substr(0, sParameterSet.length() - 1);
				sReturnCode += "(";
				sReturnCode += sParameterSet;
				sReturnCode += ")";
			}
			sReturnCode += ";\n";
		}
	}
	
	//3.connect����---------connect(�����1.�ӿ���,  �����2.�ӿ���)
	//eg��connect(axis2.flange, mechanics.axis2)
	IdentString(level, sReturnCode);
	sReturnCode += "equation";
	sReturnCode += "\n";
	//3.1 �����ߵķ���
	vector<ConnectCurve> allConnCuv = this->GetAllConnectCurves();
	if (!allConnCuv.empty())
	{
		for (vector<ConnectCurve>::iterator k1 = allConnCuv.begin(); k1 != allConnCuv.end(); k1++)
		{
			IdentString(level, sReturnCode);
			
			sReturnCode += "  ";
			sReturnCode += "connect(";
			sReturnCode += (*k1).component1;
			if ((*k1).connector1 != "MainConnector")
			{
				sReturnCode += ".";
				sReturnCode += (*k1).connector1;
			}
			sReturnCode += ",";
			sReturnCode += (*k1).component2;
			if ((*k1).connector2 != "MainConnector")
			{
				sReturnCode += ".";
				sReturnCode += (*k1).connector2;
			}
			sReturnCode += ")";
			sReturnCode += ";";
			sReturnCode += "\n";
		}
	}

	//3.2 ����ķ���
	vector<ConnectConstraint> allConnCons = this->GetAllConnConstraints();
	if (!allConnCons.empty())
	{
		for (vector<ConnectConstraint>::iterator k2 = allConnCons.begin(); k2 != allConnCons.end(); k2++)
		{
			IdentString(level, sReturnCode);

			sReturnCode += "  ";
			sReturnCode += "connect(";
			sReturnCode += (*k2).component1;
			if ((*k2).connector1 != "MainConnector")
			{
				sReturnCode += ".";
				sReturnCode += (*k2).connector1;
			}
			sReturnCode += ",";
			sReturnCode += (*k2).component2;
			if ((*k2).connector2 != "MainConnector")
			{
				sReturnCode += ".";
				sReturnCode += (*k2).connector2;
			}
			sReturnCode += ")";
			sReturnCode += ";";
			sReturnCode += "\n";
		}
	}
	
	IdentString(level, sReturnCode);
	sReturnCode += "end ";
	sReturnCode += compName;
	sReturnCode += ";\n";
	
	return true;
}
