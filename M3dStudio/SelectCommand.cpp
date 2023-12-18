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
	this->view->StateChanged(QString("选择"));
}

SelectCommand::~SelectCommand()
{

}

void SelectCommand::OnMouseMove()
{
	//在状态栏提示 选择实体
	this->view->StateChanged(QString("选择实体"));
}

void SelectCommand::OnLeftButtonDown()
{

}

void SelectCommand::OnLeftButtonRelease()
{
	//获得选择的实体名，向主调窗口发送字符串

/*	m_pCallWidget->setText("");*/ //看是否能刷新显示，如果不行，则需要进行槽函数调用
	//emit ...

	myContext->InitSelected();
	if (!myContext->HasSelectedShape())
	{
		return;
	}
	TopoDS_Shape shp = myContext->SelectedShape();
	QString sel_name = QString::fromStdString(this->GetCmdDocument()->GetShapeName(shp));
	QObject::connect(this, &SelectCommand::sendSelectContext, view, &View::OnsendSelectContext/*,Qt::UniqueConnection*/);
	//发送信号
	emit sendSelectContext(sel_name);
	//状态栏显示，选择完成
	this->view->StateChanged(QString("选择完成"));
}

void SelectCommand::OnRightButtonDown()
{

}

//取消命令
void SelectCommand::OnRightButtonRelease()
{
	//执行取消
	CancelCommand();
}
