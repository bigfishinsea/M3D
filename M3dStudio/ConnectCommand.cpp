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
	//状态栏中添加:请选择第一个接口
	this->view->StateChanged(QString("连接:请选择第一个接口"));
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
	//m_step总共有1,2两种情况
	//m_step==1时，捕获第一个接口
	//m_step==2时，捕获第二个接口,画线将第一个接口和第二个接口连接
	m_step++;
	switch (m_step)
	{
	case 1:
	{
		this->view->StateChanged(QString("连接:请选择第一个接口"));
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
		//得到connector1的Ax3
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
				this->view->StateChanged(QString("连接:第一个接口选择失败,请重新选择!"));
				m_step--;
				return;
			}
		}
		myContext->ClearSelected(true);
		//状态栏中添加:请选择第二个接口
		this->view->StateChanged(QString("连接:请选择第二个接口"));
		break;
	}
	case 2:
	{
		this->view->StateChanged(QString("连接:请选择第二个接口"));
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
		//得到connector2的Ax3
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
				this->view->StateChanged(QString("连接:第二个接口选择失败,请重新选择!"));
				m_step--;
				return;
			}
		}
		//调用生成连接线函数
		dynamic_cast<DocumentModel*>(this->GetCmdDocument())->generateConncurvesAIS(comp1, connector1, comp2, connector2);
		this->view->StateChanged(QString("连接完成"));
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
	//状态栏取消装配显示的部分
	this->view->StateChanged(QString());
}
