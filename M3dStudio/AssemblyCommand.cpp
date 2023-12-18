#include "DocumentModel.h"
#include "AssemblyCommand.h"
//#include "DocumentModel.h"
#include "global.h"
#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <gp_Trsf.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepTools_ReShape.hxx>
#include <BRep_Tool.hxx>
#include <QMessageBox>
#include <QString>
#include <queue>
#include <string>
#include <unordered_set>
#include "Connector.h"
#include "DocumentComponent.h"

AssemblyCommand::AssemblyCommand(View* view):pCom1(nullptr),connector1(nullptr),connector2(nullptr),Command(view)
{
	//状态栏中添加:请选择第一个接口
	this->view->StateChanged(QString("装配:请选择第一个接口"));
	myDocumentModel = dynamic_cast<DocumentModel*>(myDocumentCommon);
}

AssemblyCommand::~AssemblyCommand()
{
	//release();
}

void AssemblyCommand::OnLeftButtonDown()
{

}

void AssemblyCommand::OnRightButtonDown()
{

}

void AssemblyCommand::OnLeftButtonRelease()
{
	//m_step总共有1,2两种情况
	//m_step==1时，捕获第一个接口
	//m_step==2时，捕获第二个接口,并且将第二个接口与第一个接口重合
	m_step++;
	unordered_map<string, unordered_set<string>> name_map;
	unordered_map<string, string> assemblegraph;
	for (auto constraint : dynamic_cast<DocumentModel*>(this->GetCmdDocument())->GetAllConnConstraints())
	{
		name_map[constraint.component1].emplace(constraint.component1 + '+' + constraint.connector1);
		name_map[constraint.component2].emplace(constraint.component2 + '+' + constraint.connector2);
		assemblegraph[constraint.component1 + '+' + constraint.connector1] = constraint.component2 + '+' + constraint.connector2;
		assemblegraph[constraint.component2 + '+' + constraint.connector2] = constraint.component1 + '+' + constraint.connector1;
	}
	switch (m_step)
	{
	case 1:
	{
		this->view->StateChanged(QString("装配:请选择第一个接口"));
		myContext->InitSelected();
		if (!myContext->HasSelectedShape())
		{
			m_step--;
			return;
		}
		Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
		TopoDS_Shape shape;
		if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
		}
		if (m_Comproot)
		{
			list<Component*> comChildren = m_Comproot->GetChilds();
			list<Component*>::iterator iter;
			bool finded = false;
			for (iter = comChildren.begin(); iter != comChildren.end(); ++iter)
			{
				Component* pComChild = *iter;
				if (pComChild->IsAssembly())
				{
					// 假如是子装配体
					// 通过递归函数找到所有的非子装配体的组件
					list<Component*> iter_compChildren = GetAllNotAssembly(pComChild);
					for (auto iter_compChild : iter_compChildren)
					{
						connector1 = iter_compChild->GetConnector(shape);
						if (connector1)
						{
							pCom1 = iter_compChild;
							finded = true;
							break;
						}
					}
				}
				else
				{
					connector1 = pComChild->GetConnector(shape);
					if (connector1)
					{
						pCom1 = pComChild;
						finded = true;
						break;
					}
				}
				if (finded == true)
				{
					break;
				}
			}
			if (connector1==nullptr)
			{
				this->view->StateChanged(QString("装配:第一个接口选择失败,请重新选择!"));
				m_step--;
				return;
			}
			string comp_conn_name1 = pCom1->GetCompName() + '+' + connector1->GetConnName();
			//if (assemblegraph.count(comp_conn_name1))
			//{
			//	this->view->StateChanged(QString("装配:第一个接口已经被装配,请重新选择!"));
			//	m_step--;
			//	return;
			//}
		}
		//myContext->ClearSelected(true);
		//状态栏中添加:请选择第二个接口
		this->view->StateChanged(QString("装配:请选择第二个接口"));
		break;
	}
	case 2:
	{
		this->view->StateChanged(QString("装配:请选择第二个接口"));
		myContext->InitSelected();
		if (!myContext->HasSelectedShape())
		{
			m_step--;
			return;
		}
		Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
		TopoDS_Shape shape;
		if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
		}
		Component* pCom2 = nullptr;
		bool hasWorld = false;
		bool hasCycle = false;
		if (m_Comproot)
		{
			list<Component*> comChildren = m_Comproot->GetChilds();
			list<Component*>::iterator iter;
			for (iter = comChildren.begin(); iter != comChildren.end(); ++iter)
			{
				Component* pComChild = *iter;
				if (pComChild->IsAssembly())
				{
					// 假如是子装配体
					// 通过递归函数找到所有的非子装配体的组件
					list<Component*> iter_compChildren = GetAllNotAssembly(pComChild);
					for (auto iter_compChild : iter_compChildren)
					{
						connector2 = iter_compChild->GetConnector(shape);
						if (connector2)
						{
							pCom2 = iter_compChild;
							break;
						}
					}
				}
				else
				{
					connector2 = pComChild->GetConnector(shape);
					if (connector2)
					{
						pCom2 = pComChild;
						break;
					}
				}
			}
			if (connector2 == nullptr)
			{
				this->view->StateChanged(QString("装配:第二个接口选择失败,请重新选择!"));
				m_step--;
				return;
			}
			string comp_conn_name2 = pCom2->GetCompName() + '+' + connector2->GetConnName();
			//if (assemblegraph.count(comp_conn_name2))
			//{
			//	this->view->StateChanged(QString("装配:第二个接口已经被装配,请重新选择!"));
			//	m_step--;
			//	return;
			//}
			string comp_conn_name1 = pCom1->GetCompName() + '+' + connector1->GetConnName();
			int pos1 = comp_conn_name1.find('+');
			string comp_name1 = comp_conn_name1.substr(0, pos1);
			queue<string> Q;
			unordered_set<string> visited;
			visited.insert(comp_conn_name2);
			Q.emplace(comp_conn_name2);
			while (!Q.empty())
			{
				auto curr_comp_conn_name = Q.front();
				int curr_pos = curr_comp_conn_name.find('+');
				string curr_comp_name = curr_comp_conn_name.substr(0, curr_pos);
				Q.pop();
				if (curr_comp_name == comp_name1)
				{
					//找到了组件1，在同一条装配链上
					hasCycle = true;
					break;
				}
				if (curr_comp_name == "world")
				{
					hasWorld = true;
				}
				string curr_conn_name = curr_comp_conn_name.substr(curr_pos + 1, curr_comp_conn_name.length() - curr_pos - 1);
				Component* curr_comp = m_Comproot->GetChild(curr_comp_name.c_str());
				Connector* curr_conn = dynamic_cast<DocumentComponent*>(curr_comp->GetDocument())->GetConnector(curr_conn_name.c_str());
				//找到该组件的其它接口相连的组件
				if (name_map.find(curr_comp_name) == name_map.end())
				{
					continue;
				}
				for (auto& other_comp_conn_name : name_map[curr_comp_name])
				{
					int other_pos = other_comp_conn_name.find('+');
					string other_comp_name = other_comp_conn_name.substr(0, other_pos);
					string other_conn_name = other_comp_conn_name.substr(other_pos + 1, other_comp_conn_name.length() - other_pos - 1);
					if (other_conn_name == curr_conn_name)
					{
						continue;
					}
					string next_comp_conn_name = assemblegraph[other_comp_conn_name];
					if (!visited.count(next_comp_conn_name))
					{
						visited.insert(next_comp_conn_name);
						Q.push(next_comp_conn_name);
					}
				}
			}
			myContext->ClearSelected(true);
			if (hasCycle)
			{
				this->view->StateChanged(QString("装配：两个装配体在同一条装配链上，无法装配，请重新选择"));
				m_step--;
				return;
			}
			//记录装配约束
			ConnectConstraint cc;
			//需要将被装配组件的DeltaTransform置为单位阵
			//如果pCom1不是World并且pCom1是自由体,则清空deltaTransform
			if (pCom1->GetCompName() != "world" && name_map.find(pCom1->GetCompName()) == name_map.end())
			{
				gp_Trsf oTrans = pCom1->GetOriginTransform();
				gp_Trsf dTrans = pCom1->GetDeltaTransform();
				oTrans = dTrans * oTrans;
				pCom1->SetOriginTransform(oTrans);
				vector<double> v15{ 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
				pCom1->SetDeltaTransform(v15);
			}
			////如果pCom2不是World并且pCom2是自由体,则清空deltaTransform
			if (pCom2->GetCompName() != "world" && name_map.find(pCom2->GetCompName()) == name_map.end())
			{
				gp_Trsf oTrans = pCom2->GetOriginTransform();
				gp_Trsf dTrans = pCom2->GetDeltaTransform();
				oTrans = dTrans * oTrans;
				pCom2->SetOriginTransform(oTrans);
				vector<double> v15{ 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
				pCom2->SetDeltaTransform(v15);
			}
			if (hasWorld)
			{
				//组件2有World需要交换顺序
				cc.component1 = pCom2->GetCompName();
				cc.component2 = pCom1->GetCompName();
				cc.connector1 = connector2->GetConnName();
				cc.connector2 = connector1->GetConnName();
				myDocumentModel->AddConnConstraint(cc);
				//1 -> 2
				myDocumentModel->Assemble(pCom2, connector2, pCom1, connector1);
			}
			else
			{
				cc.component1 = pCom1->GetCompName();
				cc.component2 = pCom2->GetCompName();
				cc.connector1 = connector1->GetConnName();
				cc.connector2 = connector2->GetConnName();
				myDocumentModel->AddConnConstraint(cc);
				//2 -> 1
				myDocumentModel->Assemble(pCom1, connector1, pCom2, connector2);
			}
			this->view->StateChanged(QString("装配完成"));
		}
		break;
	}
	default:
		break;
	}
}

void AssemblyCommand::OnRightButtonRelease()
{

}

void AssemblyCommand::OnMouseMove()
{

}

list<Component*> AssemblyCommand::GetAllNotAssembly(Component* comp)
{
	list<Component*> l;
	DFS(comp, l);
	return l;
}

void AssemblyCommand::release()
{
	//状态栏取消装配显示的部分
	this->view->StateChanged(QString());
}

void AssemblyCommand::DFS(Component* comp, list<Component*>& l)
{
	if (!comp->IsAssembly())
	{
		l.push_back(comp);
		return;
	}
	list<Component*> comChildren = comp->GetChilds();
	for (auto pCom : comChildren)
	{
		DFS(pCom, l);
	}
}
