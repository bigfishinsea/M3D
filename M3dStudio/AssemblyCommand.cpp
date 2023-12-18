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
	//״̬�������:��ѡ���һ���ӿ�
	this->view->StateChanged(QString("װ��:��ѡ���һ���ӿ�"));
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
	//m_step�ܹ���1,2�������
	//m_step==1ʱ�������һ���ӿ�
	//m_step==2ʱ������ڶ����ӿ�,���ҽ��ڶ����ӿ����һ���ӿ��غ�
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
		this->view->StateChanged(QString("װ��:��ѡ���һ���ӿ�"));
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
					// ��������װ����
					// ͨ���ݹ麯���ҵ����еķ���װ��������
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
				this->view->StateChanged(QString("װ��:��һ���ӿ�ѡ��ʧ��,������ѡ��!"));
				m_step--;
				return;
			}
			string comp_conn_name1 = pCom1->GetCompName() + '+' + connector1->GetConnName();
			//if (assemblegraph.count(comp_conn_name1))
			//{
			//	this->view->StateChanged(QString("װ��:��һ���ӿ��Ѿ���װ��,������ѡ��!"));
			//	m_step--;
			//	return;
			//}
		}
		//myContext->ClearSelected(true);
		//״̬�������:��ѡ��ڶ����ӿ�
		this->view->StateChanged(QString("װ��:��ѡ��ڶ����ӿ�"));
		break;
	}
	case 2:
	{
		this->view->StateChanged(QString("װ��:��ѡ��ڶ����ӿ�"));
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
					// ��������װ����
					// ͨ���ݹ麯���ҵ����еķ���װ��������
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
				this->view->StateChanged(QString("װ��:�ڶ����ӿ�ѡ��ʧ��,������ѡ��!"));
				m_step--;
				return;
			}
			string comp_conn_name2 = pCom2->GetCompName() + '+' + connector2->GetConnName();
			//if (assemblegraph.count(comp_conn_name2))
			//{
			//	this->view->StateChanged(QString("װ��:�ڶ����ӿ��Ѿ���װ��,������ѡ��!"));
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
					//�ҵ������1����ͬһ��װ������
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
				//�ҵ�������������ӿ����������
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
				this->view->StateChanged(QString("װ�䣺����װ������ͬһ��װ�����ϣ��޷�װ�䣬������ѡ��"));
				m_step--;
				return;
			}
			//��¼װ��Լ��
			ConnectConstraint cc;
			//��Ҫ����װ�������DeltaTransform��Ϊ��λ��
			//���pCom1����World����pCom1��������,�����deltaTransform
			if (pCom1->GetCompName() != "world" && name_map.find(pCom1->GetCompName()) == name_map.end())
			{
				gp_Trsf oTrans = pCom1->GetOriginTransform();
				gp_Trsf dTrans = pCom1->GetDeltaTransform();
				oTrans = dTrans * oTrans;
				pCom1->SetOriginTransform(oTrans);
				vector<double> v15{ 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
				pCom1->SetDeltaTransform(v15);
			}
			////���pCom2����World����pCom2��������,�����deltaTransform
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
				//���2��World��Ҫ����˳��
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
			this->view->StateChanged(QString("װ�����"));
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
	//״̬��ȡ��װ����ʾ�Ĳ���
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
