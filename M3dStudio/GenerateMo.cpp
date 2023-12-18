#include "DocumentModel.h"
#include <map>
#include <string.h> 
#include "DocumentModel.h"
//#include "DocumentComponent.h"
#include "Connector.h"

using namespace std;

//分割字符串，输入待分割的字符串s以及分割符sep
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

//缩进
void IdentString(int level, string& sReturnCode)
{
	//缩进
	for (int indent = 0; indent < level; indent++)
	{
		sReturnCode += "  ";//2个空格
	}
}

//如果组件是装配体，则继续查找
myParameter* DocumentModel::GetParameterInfo(const string& sMapName) const
{
	myParameter* pPar;

	//得到组件名和组件
	vector<string> vStr = split(sMapName, '.');
	assert(vStr.size() == 2);

	string sComName = vStr[0];
	string sParName = vStr[1];
	Component* pComChild = m_pCompRoot->GetChild(sComName.c_str());
	assert(pComChild);

	if (pComChild->IsAssembly())
	{
		//如果是部件，则找到它的参数值 表，再得到其映射组件参数名，递归
		DocumentModel* pMdl = (DocumentModel*)pComChild->GetDocument();
		ParameterValue pv;
		if (pMdl->GetParameterValue(sParName.c_str(), pv))
		{
			string sTheMapName = pv.sMapParamName;
			//assert(sTheMapName);
			//递归
			return GetParameterInfo(sTheMapName);
		}
		else {
			return nullptr;
		}
	}
	else {
		//零件
		CyberInfo* pCyber = pComChild->GetCyberInfo();
		pPar = pCyber->GetParameter(sParName.c_str());
		delete pCyber;

		return pPar;
	}

	return nullptr;
}

//生成Modelica代码
bool DocumentModel::GenerateModelicaCode(string& sReturnCode,int level)
{
	//model 组件名
	string compName = m_pCompRoot->GetCompName();

	IdentString(level, sReturnCode);
	sReturnCode += "model ";
	sReturnCode += compName;
	sReturnCode += "\n";

	//1.参数定义
	//eg：parameter SI.Position rLoad[3] = {0.1, 0.25, 0.1}
	vector<ParameterValue> allPars = this->GetAllParameterValues();
	for (vector<ParameterValue>::iterator iter = allPars.begin(); iter != allPars.end(); iter++)
	{
		ParameterValue pv = *iter;
		//sParamName不为空时
		if (! pv.sParamName.empty())
		{
			IdentString(level, sReturnCode);

			//获得参数类型、参数维度
			myParameter* par = GetParameterInfo(pv.sMapParamName);

			sReturnCode += "  ";
			sReturnCode += "parameter ";
			//参数类型？？
			sReturnCode += par->sType;
			sReturnCode += " ";
			sReturnCode += pv.sParamName;
			//参数维度？？
			sReturnCode += par->sDimension;
			sReturnCode += "=";
			sReturnCode += pv.sParamValue;
			sReturnCode += ";";
			sReturnCode += "\n";

			delete par;
		}
	}

	//2.组件声明与变型---------组件类型 组件名(组件参数)
	//eg：Components.MechanicalStructure mechanics(mLoad = mLoad,rLoad = rLoad,g = g)
	//从根节点遍历component树
	if (m_pCompRoot)
	{
		list<Component*> comChilds = m_pCompRoot->GetChilds();
		list<Component*>::iterator itr = comChilds.begin();
		vector<string> splitCompMdl; //被'.'分割的组件类型
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

			//2.1 组件属性，protected，inner等
			//ComponentAttrib   
			string childCompName = pComChild->GetCompName();//组件名
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

			//2.2 组件类型
			CyberInfo* childCompCyIn = pComChild->GetCyberInfo();
			if (!childCompCyIn)
			{
				assert(false);
				continue;
			}
			string childCompMdl = childCompCyIn->GetMdlName();
			sReturnCode += childCompMdl;

			//2.3 组件名        
			sReturnCode += " ";
			sReturnCode += childCompName;

			//2.4 组件参数：从零件的关注参数和fixed变量，装配体的参数表
			bool bHave = false; //如果已经有定义，有"(...,"了
			//当前部件里的参数值设置
			map<string, string> setParameterValues;
			for (vector<ParameterValue>::iterator iter1 = allPars.begin(); iter1 != allPars.end(); iter1++)
			{
				vector<string> mParaName;//被'.'分割的sMapParamName
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
			//零件的关注参数
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
				
				//关注变量的fixed
				vector<myOutputVar*> vVars = pCbInfo->GetOutputVars();
				for (int k = 0; k < vVars.size(); k++)
				{
					myOutputVar* pVar = vVars[k];
					if (pVar->bFocused && !pVar->sFocusName.empty())// == "fixed")
					{//如果某个变量var是设置了关注名为 变型，则增加var(fixed=true)
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
				//去掉最后一个逗号“,"
				sParameterSet = sParameterSet.substr(0, sParameterSet.length() - 1);
				sReturnCode += "(";
				sReturnCode += sParameterSet;
				sReturnCode += ")";
			}
			sReturnCode += ";\n";
		}
	}
	
	//3.connect方程---------connect(子组件1.接口名,  子组件2.接口名)
	//eg：connect(axis2.flange, mechanics.axis2)
	IdentString(level, sReturnCode);
	sReturnCode += "equation";
	sReturnCode += "\n";
	//3.1 连接线的方程
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

	//3.2 多体的方程
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
