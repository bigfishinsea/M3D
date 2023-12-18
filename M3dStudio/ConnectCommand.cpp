#include "DocumentModel.h"
#include "ConnectCommand.h"
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
#include <QVector>
#include "Connector.h"
#include <Geom_BezierCurve.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>

ConnectCommand::ConnectCommand(View* view):Command(view),connector1(nullptr),connector2(nullptr),comp1(nullptr),comp2(nullptr)
{
	//״̬�������:��ѡ���һ���ӿ�
	this->view->StateChanged(QString("����:��ѡ���һ���ӿ�"));
}

ConnectCommand::~ConnectCommand()
{
	release();
}

void ConnectCommand::OnLeftButtonDown()
{

}

void ConnectCommand::OnRightButtonDown()
{

}

void ConnectCommand::OnLeftButtonRelease()
{
	//m_step�ܹ���1,2�������
	//m_step==1ʱ�������һ���ӿ�
	//m_step==2ʱ������ڶ����ӿ�,���߽���һ���ӿں͵ڶ����ӿ�����
	m_step++;
	switch (m_step)
	{
	case 1:
	{
		this->view->StateChanged(QString("����:��ѡ���һ���ӿ�"));
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
		//�õ�connector1��Ax3
		if (m_Comproot)
		{
			list<Component*> comChildren = m_Comproot->GetChilds();
			list<Component*>::iterator iter;
			for (iter = comChildren.begin(); iter != comChildren.end(); ++iter)
			{
				Component* pComChild = *iter;
				connector1 = pComChild->GetConnector(shape);
				if (connector1)
				{
					comp1 = pComChild;
					break;
				}
			}
			if (connector1 == nullptr)
			{
				this->view->StateChanged(QString("����:��һ���ӿ�ѡ��ʧ��,������ѡ��!"));
				m_step--;
				return;
			}
		}
		myContext->ClearSelected(true);
		//״̬�������:��ѡ��ڶ����ӿ�
		this->view->StateChanged(QString("����:��ѡ��ڶ����ӿ�"));
		break;
	}
	case 2:
	{
		this->view->StateChanged(QString("����:��ѡ��ڶ����ӿ�"));
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
		//�õ�connector2��Ax3
		if (m_Comproot)
		{
			list<Component*> comChildren = m_Comproot->GetChilds();
			list<Component*>::iterator iter;
			for (iter = comChildren.begin(); iter != comChildren.end(); ++iter)
			{
				Component* pComChild = *iter;
				connector2 = pComChild->GetConnector(shape);
				if (connector2)
				{
					comp2 = pComChild;
					break;
				}
			}
			if (connector2 == nullptr)
			{
				this->view->StateChanged(QString("����:�ڶ����ӿ�ѡ��ʧ��,������ѡ��!"));
				m_step--;
				return;
			}
		}
		//�������������ߺ���
		dynamic_cast<DocumentModel*>(this->GetCmdDocument())->generateConncurvesAIS(comp1, connector1, comp2, connector2);
		this->view->StateChanged(QString("�������"));
		break;
	}
	default:
		break;
	}
}

void ConnectCommand::OnRightButtonRelease()
{

}

void ConnectCommand::OnMouseMove()
{

}

void ConnectCommand::release()
{
	return;

	if (connector2)
	{
		delete connector2;
		connector2 = nullptr;
	}
	if (connector1)
	{
		delete connector1;
		connector1 = nullptr;
	}
	//״̬��ȡ��װ����ʾ�Ĳ���
	this->view->StateChanged(QString());
}
