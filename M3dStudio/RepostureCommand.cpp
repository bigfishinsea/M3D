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
	/*QMessageBox::about(nullptr, QString("��ʾ"), QString("��ѡ��һ��������е���"));*/
	connector = nullptr;
	component = nullptr;
	isFreeAdjust = false;
	consconnector = nullptr;
	conscomponent = nullptr;
	this->view->StateChanged(QString("���ˣ���ѡ��һ�����"));

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
		//��ʼ�任
		if (aManipulator->HasActiveMode())
		{
			aManipulator->StartTransform(curr_x, curr_y, view->getView());
		}
	}
}

void RepostureCommand::OnRightButtonDown()
{
	//����ǲ������Ļ�,ȡ�����α任���Ƴ�������
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
			//�ж�������α任�ǵ�λ�任,����Ϊȷ��
			vector<double> v12 = GetTransformValues12(trans);
			if (fabs(v12[0] - 1) < LINEAR_TOL && fabs(v12[1] - 0) < LINEAR_TOL && fabs(v12[2] - 0) < LINEAR_TOL && fabs(v12[3] - 0) < LINEAR_TOL && \
				fabs(v12[4] - 0) < LINEAR_TOL && fabs(v12[5] - 1) < LINEAR_TOL && fabs(v12[6] - 0) < LINEAR_TOL && fabs(v12[7] - 0) < LINEAR_TOL && \
				fabs(v12[8] - 0) < LINEAR_TOL && fabs(v12[9] - 0) < LINEAR_TOL && fabs(v12[10] - 1) < LINEAR_TOL && fabs(v12[11] - 0) < LINEAR_TOL)
			{
				//����Ҫ����deltaTransform
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
				this->view->StateChanged(QString("�������"));
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
		QMessageBox::about(nullptr, QString("����"), QString("δ�ܳɹ�ѡ�������������"));
		m_step--;
		return;
	}
	if (myContext->NbSelected() > 1)
	{
		QMessageBox::about(nullptr, QString("����"), QString("ѡ�������������һ��,�޷����е��˲���"));
		m_step--;
		return;
	}
	TopoDS_Shape shape;
	Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
	if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
	{
		shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
	}

	this->view->StateChanged(QString("��������"));
	if (m_Comproot)
	{
		list<Component*> comChildren = m_Comproot->GetChilds();
		//�ӿ��ܵ�Լ��ֵ
		vector<ConnectConstraint> consvec = (dynamic_cast<DocumentModel*> (m_Comproot->GetDocument()))->GetAllConnConstraints();
		//�ж�װ�����ǲ�����Լ�������棬���жϽӿ��ǲ���Լ��ס
		for (auto iter = comChildren.begin(); iter != comChildren.end(); ++iter)
		{
			Component* pComChild = *iter;
			if (pComChild->GetCompound().IsPartner(shape))
			{
				//world��������е���
				if (pComChild->IsWorld())
				{
					QMessageBox::about(nullptr, "��ʾ", QString("world���������!"));
					return;
				}
				string curr_name = pComChild->GetCompName();
				//�����Լ������������˼���
				int cnt = 0;
				for (auto cons_iter = consvec.begin(); cons_iter != consvec.end(); ++cons_iter)
				{
					//Ӧ����ͳ��װ�䵽��Լ������,��ֻ��component2
					if (cons_iter->component1 == curr_name || cons_iter->component2 == curr_name)
					{
						++cnt;
					}
				}
				//����cnt���
				switch (cnt)
				{
				case 0:
				{
					//���������
					component = pComChild;
					CreateManipulator();
					isFreeAdjust = true;
					break;
				}
				default:
				{
					component = pComChild;
					//���̷ŵ����˴������ж�
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
					//	QMessageBox::about(nullptr, "����", QString("�ӿڳ���"));
					//	return;
					//}
					//�ӿ������ж���Ҫ��ѡ��ӿ�ʱ�ſ���

					_dialog = new reposturedialog(view, this, "");
					_dialog->show();
					break;
				}
				//default:
				//	//���ܵ���
				//	QMessageBox::about(nullptr, QString("����"), QString("����ѱ�Լ��,���ܽ��е��˲���"));
				//	break;
				}
			}
		}
	}
}

void RepostureCommand::OnRightButtonRelease()
{
	//ȡ�����β���
	this->view->StateChanged(QString("���˽���"));
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

			//һ������ƶ��ӿ���ֲ�����ϵ
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
	//�����������ŵ�Ҫ�����������
	//֮ǰ�Ѿ��жϹ�ѡ������Ż�����ú���,�᲻������쳣�����
	Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
	//��Ҫ����������ӵ�Context��,����Detach����
	aManipulator->Attach(obj);

	//����Scaling
	aManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);
	aManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);
	aManipulator->SetPart(2, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);

	aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Rotation);//������ת
	aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Translation);//����ƽ��
	//���������
	aManipulator->SetModeActivationOnDetection(Standard_True);
	//myContext->UpdateCurrentViewer();
	//allTrans��ʼ��
	allTrans = this->GetComponent()->GetDeltaTransform();
}
