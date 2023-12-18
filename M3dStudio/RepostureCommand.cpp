#include "DocumentModel.h"
#include "RepostureCommand.h"
//#include "DocumentModel.h"
#include "DocumentComponent.h"
#include "Component.h"
#include "RepostureDlg.h"
#include "Connector.h"
#include <AIS_Manipulator.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <Geom_BezierCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <QMessageBox>
#include "cyberInfo.h"
#include "global.h"

RepostureCommand::RepostureCommand(View* view):Command(view),_dialog(nullptr)
{
	/*QMessageBox::about(nullptr, QString("提示"), QString("请选择一个组件进行调姿"));*/
	connector = nullptr;
	component = nullptr;
	isFreeAdjust = false;
	consconnector = nullptr;
	conscomponent = nullptr;
	this->view->StateChanged(QString("调姿：请选择一个组件"));

	myContext->InitSelected();
	if (myContext->HasSelectedShape() && myContext->NbSelected() == 1)
	{
		++m_step;
		InitReposture();
	}
}

RepostureCommand::~RepostureCommand()
{

}

void RepostureCommand::OnLeftButtonDown()
{
	if (isFreeAdjust)
	{
		//开始变换
		if (aManipulator->HasActiveMode())
		{
			aManipulator->StartTransform(curr_x, curr_y, view->getView());
		}
	}
}

void RepostureCommand::OnRightButtonDown()
{
	//如果是操作器的话,取消本次变换并移除操作器
	if (isFreeAdjust)
	{
		if (aManipulator->HasActiveMode())
		{
			aManipulator->StopTransform(false);
			aManipulator->DeactivateCurrentMode();
			aManipulator->Detach();
			myContext->Remove(aManipulator, false);
			this->GetComponent()->Display(myContext);
		}
	}
}

void RepostureCommand::OnLeftButtonRelease()
{
	if (isFreeAdjust)
	{
		if (aManipulator->HasActiveMode())
		{
			gp_Trsf trans = aManipulator->Transform(curr_x, curr_y, view->getView());
			//判断如果本次变换是单位变换,则视为确定
			vector<double> v12 = GetTransformValues12(trans);
			if (fabs(v12[0] - 1) < LINEAR_TOL && fabs(v12[1] - 0) < LINEAR_TOL && fabs(v12[2] - 0) < LINEAR_TOL && fabs(v12[3] - 0) < LINEAR_TOL && \
				fabs(v12[4] - 0) < LINEAR_TOL && fabs(v12[5] - 1) < LINEAR_TOL && fabs(v12[6] - 0) < LINEAR_TOL && fabs(v12[7] - 0) < LINEAR_TOL && \
				fabs(v12[8] - 0) < LINEAR_TOL && fabs(v12[9] - 0) < LINEAR_TOL && fabs(v12[10] - 1) < LINEAR_TOL && fabs(v12[11] - 0) < LINEAR_TOL)
			{
				//不需要保存deltaTransform
				gp_Trsf oTrans = this->component->GetOriginTransform();
				gp_Trsf resTrans = allTrans * oTrans;
				this->component->SetOriginTransform(resTrans);
				vector<double> v15_reset{ 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
				this->component->SetDeltaTransform(v15_reset);
				aManipulator->DeactivateCurrentMode();
				aManipulator->Detach();
				myContext->Remove(aManipulator, false);
				//this->GetComponent()->Display(myContext);
				dynamic_cast<DocumentModel*>(this->GetCmdDocument())->regenerateConn(this->GetComponent());
				this->view->StateChanged(QString("调姿完成"));
				/*m_step = -1;*/
				m_step = 0;
				return;
			}
			allTrans = trans * allTrans;
			aManipulator->StopTransform(true);
			vector<double> v12_all = GetTransformValues12(allTrans);
			vector<double> v15;
			for (int i = 0; i <= 2; ++i)
			{
				v15.push_back(v12_all[i]);
			}
			for (int i = 0; i <= 2; ++i)
			{
				v15.push_back(0);
			}
			for (int i = 3; i < 12; ++i)
			{
				v15.push_back(v12_all[i]);
			}
			this->GetComponent()->SetDeltaTransform(v15);
			this->GetComponent()->Display(myContext);
			
			aManipulator->Attach(this->GetComponent()->GetAIS()[0]);

			//Component* pCom = GetComponent();
			//if (pCom)
			//{
			//	gp_Trsf oTrsf = pCom->GetOriginTransform();
			//	gp_Trsf delTrsf = pCom->GetDeltaTransform();
			//	vector<Handle(AIS_InteractiveObject)> vAIS = pCom->GetAIS();
			//	for (int i = 0; i < vAIS.size(); i++)
			//	{
			//		myContext->SetLocation(vAIS[i], allTrans);
			//	}
			//	view->getView()->Redraw();
			//}
			return;
		}
	}

	//if (m_step < 0)
	//{
	//	return;
	//}
	if (m_step == 0)
	{
		m_step++;
		InitReposture();
	}
	//m_step++;
	//switch (m_step)
	//{
	//case 1:
	//{
	//	InitReposture();
	//}
	//default:
	//	break;
	//}
}

void RepostureCommand::InitReposture()
{
	myContext->InitSelected();
	if (!myContext->HasSelectedShape())
	{
		QMessageBox::about(nullptr, QString("调姿"), QString("未能成功选择组件，请重试"));
		m_step--;
		return;
	}
	if (myContext->NbSelected() > 1)
	{
		QMessageBox::about(nullptr, QString("调姿"), QString("选中组件个数大于一个,无法进行调姿操作"));
		m_step--;
		return;
	}
	TopoDS_Shape shape;
	Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
	if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
	{
		shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
	}

	this->view->StateChanged(QString("调姿启动"));
	if (m_Comproot)
	{
		list<Component*> comChildren = m_Comproot->GetChilds();
		//接口总的约束值
		vector<ConnectConstraint> consvec = (dynamic_cast<DocumentModel*> (m_Comproot->GetDocument()))->GetAllConnConstraints();
		//判断装配体是不是在约束表里面，再判断接口是不是约束住
		for (auto iter = comChildren.begin(); iter != comChildren.end(); ++iter)
		{
			Component* pComChild = *iter;
			if (pComChild->GetCompound().IsPartner(shape))
			{
				//world不允许进行调姿
				if (pComChild->IsWorld())
				{
					QMessageBox::about(nullptr, "提示", QString("world不允许调姿!"));
					return;
				}
				string curr_name = pComChild->GetCompName();
				//零件在约束表里面出现了几次
				int cnt = 0;
				for (auto cons_iter = consvec.begin(); cons_iter != consvec.end(); ++cons_iter)
				{
					//应该是统计装配到的约束数量,即只有component2
					if (cons_iter->component1 == curr_name || cons_iter->component2 == curr_name)
					{
						++cnt;
					}
				}
				//根据cnt情况
				switch (cnt)
				{
				case 0:
				{
					//自由体调姿
					component = pComChild;
					CreateManipulator();
					isFreeAdjust = true;
					break;
				}
				default:
				{
					component = pComChild;
					//过程放到调姿窗口中判断
					//for (auto cons_iter = consvec.begin(); cons_iter != consvec.end(); ++cons_iter)
					//{
					//	if (cons_iter->component2 == curr_name)
					//	{
					//		conscomponent = m_Comproot->GetChild(cons_iter->component1.c_str());
					//		connector = (dynamic_cast<DocumentComponent*> (pComChild->GetDocument()))->GetConnector(cons_iter->connector2.c_str());
					//		consconnector = (dynamic_cast<DocumentComponent*> (conscomponent->GetDocument()))->GetConnector(cons_iter->connector1.c_str());
					//	}
					//}
					//if (connector == nullptr)
					//{
					//	QMessageBox::about(nullptr, "错误", QString("接口出错"));
					//	return;
					//}
					//接口类型判断需要到选择接口时才可以

					_dialog = new reposturedialog(view, this, "");
					_dialog->show();
					break;
				}
				//default:
				//	//不能调姿
				//	QMessageBox::about(nullptr, QString("调姿"), QString("组件已被约束,不能进行调姿操作"));
				//	break;
				}
			}
		}
	}
}

void RepostureCommand::OnRightButtonRelease()
{
	//取消本次操作
	this->view->StateChanged(QString("调姿结束"));
}

void RepostureCommand::OnMouseMove()
{
	if (isFreeAdjust)
	{
		if (aManipulator->HasActiveMode())
		{
			return; 

			gp_Trsf trans = aManipulator->Transform(curr_x, curr_y, view->getView());
			//allTrans = trans * allTrans;
			//view->getView()->Redraw();

			//一起跟随移动接口与局部坐标系
			trans = trans * allTrans;
			Component* pCom = GetComponent();
			if (pCom)
			{
				vector<Handle(AIS_InteractiveObject)> vAIS = pCom->GetAIS();
				for (int i = 0; i < vAIS.size(); i++)
				{
					myContext->SetLocation(vAIS[i], trans);
				}
			}
		}
	}
}

void RepostureCommand::CreateManipulator()
{
	aManipulator = new AIS_Manipulator();
	//将操作器附着到要操作的组件上
	//之前已经判断过选中组件才会进到该函数,会不会出现异常情况？
	Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
	//需要将操纵器添加到Context中,否则Detach报错
	aManipulator->Attach(obj);

	//禁用Scaling
	aManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);
	aManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);
	aManipulator->SetPart(2, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);

	aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Rotation);//启用旋转
	aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Translation);//启用平移
	//激活操作器
	aManipulator->SetModeActivationOnDetection(Standard_True);
	//myContext->UpdateCurrentViewer();
	//allTrans初始化
	allTrans = this->GetComponent()->GetDeltaTransform();
}
