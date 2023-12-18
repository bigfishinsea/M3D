#include "DocumentModel.h"
#include "SelectCommand.h"
#include "FeatureButton.h"
//#include "DocumentModel.h"

SelectCommand::SelectCommand(View * pView)
	: Command(pView)
{
	init();
}

void SelectCommand::init()
{
	this->view->StateChanged(QString("ѡ��"));
}

SelectCommand::~SelectCommand()
{

}

void SelectCommand::OnMouseMove()
{
	//��״̬����ʾ ѡ��ʵ��
	this->view->StateChanged(QString("ѡ��ʵ��"));
}

void SelectCommand::OnLeftButtonDown()
{

}

void SelectCommand::OnLeftButtonRelease()
{
	//���ѡ���ʵ���������������ڷ����ַ���

/*	m_pCallWidget->setText("");*/ //���Ƿ���ˢ����ʾ��������У�����Ҫ���вۺ�������
	//emit ...

	myContext->InitSelected();
	if (!myContext->HasSelectedShape())
	{
		return;
	}
	TopoDS_Shape shp = myContext->SelectedShape();
	QString sel_name = QString::fromStdString(this->GetCmdDocument()->GetShapeName(shp));
	QObject::connect(this, &SelectCommand::sendSelectContext, view, &View::OnsendSelectContext/*,Qt::UniqueConnection*/);
	//�����ź�
	emit sendSelectContext(sel_name);
	//״̬����ʾ��ѡ�����
	this->view->StateChanged(QString("ѡ�����"));
}

void SelectCommand::OnRightButtonDown()
{

}

//ȡ������
void SelectCommand::OnRightButtonRelease()
{
	//ִ��ȡ��
	CancelCommand();
}
