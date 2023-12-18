#include "ComAttriDlg.h"
#include "mainwindow.h"
#include <QMessageBox>
#include <QTimer>

ComAttriDlg::ComAttriDlg(QWidget* parent) :QDialog(parent)
{
	myDocument3d = (DocumentModel*)qobject_cast<MainWindow*>(parent)->getDocument();
	Handle(AIS_InteractiveContext) myContext = myDocument3d->getContext();
	myContext->InitSelected();
	Component* m_pCompRoot = myDocument3d->GetComponent();

	//判断选中是否为空
	TopoDS_Shape shpSel = myContext->SelectedShape();
	if (shpSel.IsNull())
	{
		QMessageBox::information(NULL, tr("提示"), tr("请选择组件"), QMessageBox::Ok);
		QTimer::singleShot(0, this, SLOT(close()));
	}
	else
	{
		if (m_pCompRoot)
		{
			list<Component*> comChilds = m_pCompRoot->GetChilds();
			list<Component*>::iterator itr = comChilds.begin();
			bool bFound = false;
			for (; itr != comChilds.end(); ++itr)
			{
				Component* pComChild = *itr;
				//判断是否为组件
				if (shpSel.IsPartner(pComChild->GetCompound()))
				{
					bFound = true;
					compName = pComChild->GetCompName();
					comAttriDlgInit();
					setAttriValue();
					break;
				}
			}
			if (!bFound)
			{
				QMessageBox::information(NULL, tr("提示"), tr("所选实体不为组件"), QMessageBox::Ok);
				QTimer::singleShot(0, this, SLOT(close()));
			}
		}
	}
}

void ComAttriDlg::comAttriDlgInit()
{
	setWindowTitle(tr("组件属性"));
	form = new QVBoxLayout(this);

	BtnStyle = QString(
		"QGroupBox{"
		"font: 18px;"
		"}"
		"QCheckBox{"
		"font: 18px;"
		"}"
		"QRadioButton{"
		"font: 18px;"
		"}");

	//属性
	pro_box = new QGroupBox(this);
	pro_box->setTitle(tr("属性"));
	pro_box->setStyleSheet(BtnStyle);
	Final = new QCheckBox("Final", this);
	Final->setStyleSheet(BtnStyle);
	Final->setMinimumHeight(25);
	Protected = new QCheckBox("Protected", this);
	Protected->setStyleSheet(BtnStyle);
	Protected->setMinimumHeight(25);
	Replaceable = new QCheckBox("Replaceable", this);
	Replaceable->setStyleSheet(BtnStyle);
	Replaceable->setMinimumHeight(25);
	pro_form = new QVBoxLayout(pro_box);
	pro_form->addWidget(Final);
	pro_form->addWidget(Protected);
	pro_form->addWidget(Replaceable);

	//可变性
	var_box = new QGroupBox(this);
	var_box->setTitle(tr("可变性"));
	var_box->setStyleSheet(BtnStyle);
	Constant = new QRadioButton("Constant", this);
	Constant->setStyleSheet(BtnStyle);
	Constant->setMinimumHeight(25);
	Parameter = new QRadioButton("Parameter", this);
	Parameter->setStyleSheet(BtnStyle);
	Parameter->setMinimumHeight(25);
	Discrete = new QRadioButton("Discrete", this);
	Discrete->setStyleSheet(BtnStyle);
	Discrete->setMinimumHeight(25);
	Default = new QRadioButton(tr("缺省"), this);
	Default->setStyleSheet(BtnStyle);
	Default->setMinimumHeight(25);
	var_group = new QButtonGroup(this);
	var_group->addButton(Constant, 1);
	var_group->addButton(Parameter, 2);
	var_group->addButton(Discrete, 3);
	var_group->addButton(Default, 0);
	var_form = new QVBoxLayout(var_box);
	var_form->addWidget(Constant);
	var_form->addWidget(Parameter);
	var_form->addWidget(Discrete);
	var_form->addWidget(Default);

	//动态类型
	dyn_box = new QGroupBox(this);
	dyn_box->setTitle(tr("动态类型"));
	dyn_box->setStyleSheet(BtnStyle);
	Inner = new QCheckBox("Inner", this);
	Inner->setStyleSheet(BtnStyle);
	Inner->setMinimumHeight(25);
	Outer = new QCheckBox("Outer", this);
	Outer->setStyleSheet(BtnStyle);
	Outer->setMinimumHeight(25);
	dyn_form = new QVBoxLayout(dyn_box);
	dyn_form->addWidget(Inner);
	dyn_form->addWidget(Outer);

	//因果性
	cas_box = new QGroupBox(this);
	cas_box->setTitle(tr("因果性"));
	cas_box->setStyleSheet(BtnStyle);
	Input = new QRadioButton("Input", this);
	Input->setStyleSheet(BtnStyle);
	Input->setMinimumHeight(25);
	Output = new QRadioButton("Output", this);
	Output->setStyleSheet(BtnStyle);
	Output->setMinimumHeight(25);
	None = new QRadioButton(tr("无"), this);
	None->setStyleSheet(BtnStyle);
	None->setMinimumHeight(25);
	cas_group = new QButtonGroup(this);
	cas_group->addButton(Input, 1);
	cas_group->addButton(Output, 2);
	cas_group->addButton(None, 0);
	cas_form = new QVBoxLayout(cas_box);
	cas_form->addWidget(Input);
	cas_form->addWidget(Output);
	cas_form->addWidget(None);

	//连接器成员
	con_box = new QGroupBox(this);
	con_box->setTitle(tr("连接器成员"));
	con_box->setStyleSheet(BtnStyle);
	Flow = new QRadioButton("Flow", this);
	Flow->setStyleSheet(BtnStyle);
	Flow->setMinimumHeight(25);
	Stream = new QRadioButton("Stream", this);
	Stream->setStyleSheet(BtnStyle);
	Stream->setMinimumHeight(25);
	Custom = new QRadioButton(tr("常规"), this);
	Custom->setStyleSheet(BtnStyle);
	Custom->setMinimumHeight(25);
	con_group = new QButtonGroup(this);
	con_group->addButton(Flow, 1);
	con_group->addButton(Stream, 2);
	con_group->addButton(Custom, 0);
	con_form = new QVBoxLayout(con_box);
	con_form->addWidget(Flow);
	con_form->addWidget(Stream);
	con_form->addWidget(Custom);
	con_form->addStretch();

	//增加ok，cancel
	okBtn = new QPushButton(this);
	okBtn->setText("确定");
	cancelBtn = new QPushButton(this);
	cancelBtn->setText("取消");
	hlayout = new QHBoxLayout(this);
	hlayout->addStretch();
	hlayout->addWidget(okBtn);
	hlayout->addStretch();
	hlayout->addWidget(cancelBtn);
	hlayout->addStretch();
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);
	connect(okBtn, &QPushButton::clicked, this, &ComAttriDlg::Accept);

	//布局
	pro_cas = new QHBoxLayout(this);
	pro_cas->addWidget(pro_box);
	pro_cas->addWidget(cas_box);
	var_con = new QHBoxLayout(this);
	var_con->addWidget(var_box);
	var_con->addWidget(con_box);


	form->addLayout(pro_cas);
	form->addLayout(var_con);
	form->addWidget(dyn_box);
	form->addLayout(hlayout);

	setFixedWidth(400);
}

void ComAttriDlg::setAttriValue()
{
	ComponentAttrib comAtrib = myDocument3d->GetComponentAttrib(compName.c_str());
	if (comAtrib.bFinal)
	{
		Final->setChecked(true);
	}
	if (comAtrib.bProtected)
	{
		Protected->setChecked(true);
	}
	if (comAtrib.bReplaceable)
	{
		Replaceable->setChecked(true);
	}
	if (comAtrib.bInner)
	{
		Inner->setChecked(true);
	}
	if (comAtrib.bOuter)
	{
		Outer->setChecked(true);
	}
	int b = comAtrib.iCasuality;
	switch (comAtrib.iCasuality)
	{
	case 0:
		None->setChecked(true);
		break;
	case 1:
		Input->setChecked(true);
		break;
	case 2:
		Output->setChecked(true);
	}
	int a = comAtrib.iVariablity;
	switch (comAtrib.iVariablity)
	{
	case 0:
		Default->setChecked(true);
		break;
	case 1:
		Constant->setChecked(true);
		break;
	case 2:
		Parameter->setChecked(true);
		break;
	case 3:
		Discrete->setChecked(true);
	}
	int c = comAtrib.iVariablity;
	switch (comAtrib.iConnectorMember)
	{
	case 0:
		Custom->setChecked(true);
		break;
	case 1:
		Flow->setChecked(true);
		break;
	case 2:
		Stream->setChecked(true);
	}
}

ComAttriDlg::~ComAttriDlg()
{

}

void ComAttriDlg::Accept()
{
	ComponentAttrib attrib;
	attrib.bFinal = Final->isChecked();
	attrib.bProtected = Protected->isChecked();
	attrib.bReplaceable = Replaceable->isChecked();
	attrib.bInner = Inner->isChecked();
	attrib.bOuter = Outer->isChecked();

	int casID = cas_group->checkedId();
	switch (casID)
	{
	case 0:
		attrib.iCasuality = 0;
		break;
	case 1:
		attrib.iCasuality = 1;
		break;
	case 2:
		attrib.iCasuality = 2;
		break;
	}

	int varID = var_group->checkedId();
	switch (varID)
	{
	case 0:
		attrib.iVariablity = 0;
		break;
	case 1:
		attrib.iVariablity = 1;
		break;
	case 2:
		attrib.iVariablity = 2;
		break;
	case 3:
		attrib.iVariablity = 3;
		break;
	}

	int conID = con_group->checkedId();
	switch (conID)
	{
	case 0:
		attrib.iConnectorMember = 0;
		break;
	case 1:
		attrib.iConnectorMember = 1;
		break;
	case 2:
		attrib.iConnectorMember = 2;
		break;
	}

	myDocument3d->SetComponentAttrib(compName.c_str(), attrib);//设置组件属性，存储到文档
	QDialog::close();
}