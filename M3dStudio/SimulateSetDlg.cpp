#include "M3d.h"
#include "SimulateSetDlg.h"
#include <qpushbutton.h>
#include <QMessageBox>
#include "mainwindow.h"
#include <QLineEdit>
#include <QSettings>
#include <QVBoxLayout>
#include <QHBoxLayout>

QSettings settingSimulate("HKEY_CURRENT_USER\\SOFTWARE\\M3d\\Test\\SimulateSet", QSettings::NativeFormat);
static double temp_starttime = settingSimulate.value("SimulateStartTime", "").toString().toDouble();
static double temp_endtime = settingSimulate.value("SimulateEndTime", "").toString().toDouble();
static int temp_OpenModelica_NumberOfIntervals = settingSimulate.value("OpenModelica_NumberOfIntervals", "").toString().toInt();
static int temp_Openmodelica_Method_ID = settingSimulate.value("Openmodelica_Method_ID", "").toString().toInt();
static int temp_Openmodelica_OutputMethodID = settingSimulate.value("Openmodelica_OutputMethodID", "").toString().toInt();
static int temp_MWorks_NumberOfIntervals = settingSimulate.value("MWorks_NumberOfIntervals", "").toString().toInt();
static int temp_MWorks_Method_ID = settingSimulate.value("MWorks_Method_ID", "").toString().toInt();
static int temp_MWorks_OutputMethodID = settingSimulate.value("MWorks_OutputMethodID", "").toString().toInt();


SimulateSetDlg::SimulateSetDlg(QWidget* parent) :QDialog(parent),
starttime(temp_starttime),endtime(temp_endtime), 
OpenModelica_NumberOfIntervals(temp_OpenModelica_NumberOfIntervals), Openmodelica_Method_ID(temp_Openmodelica_Method_ID), Openmodelica_OutputMethodID(temp_Openmodelica_OutputMethodID),
MWorks_NumberOfIntervals(temp_MWorks_NumberOfIntervals), MWorks_Method_ID(temp_MWorks_Method_ID), MWorks_OutputMethodID(temp_MWorks_OutputMethodID)
{
	dpDlgInit();
}

void SimulateSetDlg::dpDlgInit()
{
	setWindowTitle(tr("仿真设置"));
	BtnStyle = QString(
		//正常状态
		"QPushButton{"
		"background-color:white;"//背景颜色和透明度
		"color:black;"//字体颜色
		"border: 2px inset rgb(128,128,128);"//边框宽度、样式以及颜色
		"font: 18px;"//字体类型和字体大小
		"text-align: left;"
		"}"
		//按下时的状态
		"QPushButton:pressed{"
		"background-color:rgb(255,255,255);"
		"color:rgb(0,0,0);"
		"}"
		//下拉框
		//"QComboBox{"
		//"font: 18px;"
		//"}"
	);

	form = new QVBoxLayout(this);//垂直布局
	
	//1.共同设置
	CommonSet = new QLabel("共同设置");
	font = CommonSet->font();//加粗字体
	font.setBold(true);
	CommonSet->setFont(font);
	CommonSet->setMinimumHeight(25);

	SetStartTime = new QLabel("仿真开始时间：");
	SetStartTime->setMinimumHeight(25);
	
	StartTimeEdit = new QLineEdit;
	StartTimeEdit->setReadOnly(false); //可编辑
	StartTimeEdit->setText(QString::number(starttime, 'f', 2));//指定小数位数为2
	StartTimeEdit->setMinimumHeight(25);
	StartTimeEdit->setValidator(new QDoubleValidator(0, 10000, 2, this));//范围

	SetEndTime = new QLabel("仿真结束时间：");
	SetEndTime->setMinimumHeight(25);

	EndTimeEdit = new QLineEdit;
	EndTimeEdit->setReadOnly(false); //可编辑
	EndTimeEdit->setText(QString::number(endtime, 'f', 2));//指定小数位数为2
	EndTimeEdit->setMinimumHeight(25);
	EndTimeEdit->setValidator(new QDoubleValidator(0, 10000, 2, this));//范围

	//2.openmodelica设置
	OpenmodelicaSet = new QLabel("openmodelica设置");
	font = OpenmodelicaSet->font();//加粗字体
	font.setBold(true);
	OpenmodelicaSet->setFont(font);
	OpenmodelicaSet->setMinimumHeight(25);

	SetIntervalsNum = new QLabel("模拟间隔数量：");
	SetIntervalsNum->setMinimumHeight(25);

	IntervalsNumEdit = new QLineEdit;
	IntervalsNumEdit->setReadOnly(false); //可编辑
	IntervalsNumEdit->setText(QString::number(OpenModelica_NumberOfIntervals));
	IntervalsNumEdit->setMinimumHeight(25);
	IntervalsNumEdit->setValidator(new QIntValidator(1, 1000, this));//范围

	SetMethod = new QLabel("选择求解方式：");
	SetMethod->setMinimumHeight(25);

	cbo_Method = new QComboBox();
	cbo_Method->addItem(QString::asprintf("dassl", 0));
	cbo_Method->addItem(QString::asprintf("euler", 1));
	cbo_Method->addItem(QString::asprintf("rungekutta", 2));
	cbo_Method->setCurrentIndex(Openmodelica_Method_ID);

	//选择openmodelica输出方式
	om_ButtonAllVariables = new QRadioButton(this);
	om_ButtonAllVariables->setText("输出全部变量");
	om_ButtonAnimationVariables = new QRadioButton(this);
	om_ButtonAnimationVariables->setText("输出与动画相关变量");
	om_groupButtonOutput = new QButtonGroup(this);
	om_groupButtonOutput->addButton(om_ButtonAllVariables, 0);//id为0
	om_groupButtonOutput->addButton(om_ButtonAnimationVariables, 1);//id为1
	om_groupButtonOutput->button(Openmodelica_OutputMethodID)->setChecked(true); //默认选中上次选择的输出方式

	//3.MWorks设置
	MWorksSet = new QLabel("MWorks设置");
	font = MWorksSet->font();//加粗字体
	font.setBold(true);
	MWorksSet->setFont(font);
	MWorksSet->setMinimumHeight(25);

	SetIntegral_step = new QLabel("输出区间个数：");
	SetIntegral_step->setMinimumHeight(25);

	NumberOfIntervalsEdit = new QLineEdit;
	NumberOfIntervalsEdit->setReadOnly(false); //可编辑
	NumberOfIntervalsEdit->setText(QString::number(MWorks_NumberOfIntervals));
	NumberOfIntervalsEdit->setMinimumHeight(25);
	NumberOfIntervalsEdit->setValidator(new QIntValidator(1, 100000, this));//范围

	SetAlgo = new QLabel("选择求解方式：");
	SetAlgo->setMinimumHeight(25);

	cbo_Algo = new QComboBox();
	cbo_Algo->addItem(QString::asprintf("Dassl", 0));
	cbo_Algo->addItem(QString::asprintf("Euler", 1));
	cbo_Algo->addItem(QString::asprintf("Radau5", 2));
	cbo_Algo->addItem(QString::asprintf("Rkfix2", 3));
	cbo_Algo->addItem(QString::asprintf("Rkfix3", 4));
	cbo_Algo->addItem(QString::asprintf("Rkfix4", 5));
	cbo_Algo->addItem(QString::asprintf("Rkfix6", 6));
	cbo_Algo->addItem(QString::asprintf("Rkfix8", 7));
	cbo_Algo->setCurrentIndex(MWorks_Method_ID);

	//选择MWorks输出方式
	ButtonAllVariables = new QRadioButton(this);
	ButtonAllVariables->setText("输出全部变量");
	ButtonAnimationVariables = new QRadioButton(this);
	ButtonAnimationVariables->setText("输出与动画相关变量");
	groupButtonOutput = new QButtonGroup(this);
	groupButtonOutput->addButton(ButtonAllVariables, 0);//id为0
	groupButtonOutput->addButton(ButtonAnimationVariables, 1);//id为1
	groupButtonOutput->button(MWorks_OutputMethodID)->setChecked(true); //默认选中上次选择的输出方式

	//4.恢复默认设置按钮
	RestoreDefaultSet = new QPushButton(this);
	RestoreDefaultSet->setText("恢复默认设置");
	RestoreDefaultSet->setFixedHeight(25);
	connect(RestoreDefaultSet, SIGNAL(clicked()), this, SLOT(ClickResetButton()));

	
	//增加ok，cancel
	okBtn = new QPushButton(this);
	okBtn->setText("确定");
	cancelBtn = new QPushButton(this);
	cancelBtn->setText("取消");
	hlayout = new QHBoxLayout(this);
	hlayout->addWidget(okBtn);
	hlayout->addWidget(cancelBtn);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);
	connect(okBtn, &QPushButton::clicked, this, &SimulateSetDlg::Accept);

	//布局
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(CommonSet);
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(this);
	HorinztalLayout2->addWidget(SetStartTime);
	HorinztalLayout2->addWidget(StartTimeEdit);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->addWidget(SetEndTime);
	HorinztalLayout3->addWidget(EndTimeEdit);
	QHBoxLayout* HorinztalLayout4 = new QHBoxLayout(this);//opemmodelica设置与MWorks设置
	QVBoxLayout* VerticalLayout1 = new QVBoxLayout(this);//openmodelica设置
	VerticalLayout1->addWidget(OpenmodelicaSet);
	QHBoxLayout* HorinztalLayout5 = new QHBoxLayout(this);
	HorinztalLayout5->addWidget(SetIntervalsNum);
	HorinztalLayout5->addWidget(IntervalsNumEdit);
	VerticalLayout1->addLayout(HorinztalLayout5);
	QHBoxLayout* HorinztalLayout6 = new QHBoxLayout(this);
	HorinztalLayout6->addWidget(SetMethod);
	HorinztalLayout6->addWidget(cbo_Method);
	VerticalLayout1->addLayout(HorinztalLayout6);
	VerticalLayout1->addWidget(om_ButtonAllVariables);
	VerticalLayout1->addWidget(om_ButtonAnimationVariables);
	//VerticalLayout1->addStretch();//增加一个伸缩空间
	HorinztalLayout4->addLayout(VerticalLayout1);
	QVBoxLayout* VerticalLayout2 = new QVBoxLayout(this);//MWorks设置
	VerticalLayout2->addWidget(MWorksSet);
	QHBoxLayout* HorinztalLayout7 = new QHBoxLayout(this);
	HorinztalLayout7->addWidget(SetIntegral_step);
	HorinztalLayout7->addWidget(NumberOfIntervalsEdit);
	VerticalLayout2->addLayout(HorinztalLayout7);
	QHBoxLayout* HorinztalLayout8 = new QHBoxLayout(this);
	HorinztalLayout8->addWidget(SetAlgo);
	HorinztalLayout8->addWidget(cbo_Algo);
	VerticalLayout2->addLayout(HorinztalLayout8);
	VerticalLayout2->addWidget(ButtonAllVariables);
	VerticalLayout2->addWidget(ButtonAnimationVariables);
	HorinztalLayout4->addLayout(VerticalLayout2);


	//添加到布局中
	form->addLayout(HorinztalLayout1);
	form->addLayout(HorinztalLayout2);
	form->addLayout(HorinztalLayout3);
	form->addLayout(HorinztalLayout4);
	form->addWidget(RestoreDefaultSet);
	form->addLayout(hlayout);

	this->setLayout(form);
	//setFixedWidth(600);//设置对话框宽度恒为600
	form->setContentsMargins(20, 20, 20, 20);
}

SimulateSetDlg::~SimulateSetDlg()
{

}

void SimulateSetDlg::Accept()
{
	temp_starttime = StartTimeEdit->text().toDouble();
	settingSimulate.setValue("SimulateStartTime", temp_starttime);

	temp_endtime = EndTimeEdit->text().toDouble();
	settingSimulate.setValue("SimulateEndTime", temp_endtime);

	temp_OpenModelica_NumberOfIntervals = IntervalsNumEdit->text().toInt();
	settingSimulate.setValue("OpenModelica_NumberOfIntervals", temp_OpenModelica_NumberOfIntervals);

	temp_Openmodelica_Method_ID = cbo_Method->currentIndex();
	settingSimulate.setValue("Openmodelica_Method_ID", temp_Openmodelica_Method_ID);

	temp_Openmodelica_OutputMethodID = om_groupButtonOutput->checkedId();
	settingSimulate.setValue("Openmodelica_OutputMethodID", temp_Openmodelica_OutputMethodID);

	temp_MWorks_NumberOfIntervals = NumberOfIntervalsEdit->text().toInt();
	settingSimulate.setValue("MWorks_NumberOfIntervals", temp_MWorks_NumberOfIntervals);

	temp_MWorks_Method_ID = cbo_Algo->currentIndex();
	settingSimulate.setValue("MWorks_Method_ID", temp_MWorks_Method_ID);

	temp_MWorks_OutputMethodID = groupButtonOutput->checkedId();
	settingSimulate.setValue("MWorks_OutputMethodID", temp_MWorks_OutputMethodID);
	
	QDialog::close();
}

void SimulateSetDlg::ClickResetButton()
{
	StartTimeEdit->setText(QString::number(0));
	EndTimeEdit->setText(QString::number(1));
	IntervalsNumEdit->setText(QString::number(500));
	cbo_Method->setCurrentIndex(0);
	om_groupButtonOutput->button(0)->setChecked(true);//默认输出所有变量
	NumberOfIntervalsEdit->setText(QString::number(500));
	cbo_Algo->setCurrentIndex(0);
	groupButtonOutput->button(0)->setChecked(true);//默认输出所有变量
}




//获得仿真开始时间
double SimulateSetDlg::GetStarttime()
{
	static double get_starttime;
	get_starttime = temp_starttime;
	return get_starttime;
}

//获得仿真结束时间
double SimulateSetDlg::GetEndtime()
{
	static double get_endtime;
	get_endtime = temp_endtime;
	return get_endtime;
}

//获得openmodelica的模拟间隔数量
int SimulateSetDlg::GetOpenModelica_NumberOfIntervals()
{
	static int get_om_numofint;
	get_om_numofint = temp_OpenModelica_NumberOfIntervals;
	return get_om_numofint;
}

//获得openmodelica的求解方法
string SimulateSetDlg::GetOpenModelica_Method()
{
	static int get_om_methodID;
	get_om_methodID = temp_Openmodelica_Method_ID;
	if (get_om_methodID == 0)
	{
		return "dassl";
	}
	else if (get_om_methodID == 1)
	{
		return "euler";
	}
	else if (get_om_methodID == 2)
	{
		return "rungekutta";
	}
	return "dassl";
}

//获得OpenModelica的输出方式，0为输出所有变量，1为输出与驱动动画相关的变量
int SimulateSetDlg::GetOpenModelica_OutputMethodID()
{
	static int get_om_OutputMethodID;
	get_om_OutputMethodID = temp_Openmodelica_OutputMethodID;
	return get_om_OutputMethodID;
}

//获得MWorks的输出区间个数
int SimulateSetDlg::GetMWorks_NumberOfIntervals()
{
	static double get_MW_NumberOfIntervals;
	get_MW_NumberOfIntervals = temp_MWorks_NumberOfIntervals;
	return get_MW_NumberOfIntervals;
}

//获得MWorks的求解方法
string SimulateSetDlg::GetMWorks_Method()
{
	static int get_MW_methodID;
	get_MW_methodID = temp_MWorks_Method_ID;
	if (get_MW_methodID == 0)
	{
		return "\"Dassl\"";
	}
	else if (get_MW_methodID == 1)
	{
		return "\"Euler\"";
	}
	else if (get_MW_methodID == 2)
	{
		return "\"Radau5\"";
	}
	else if (get_MW_methodID == 3)
	{
		return "\"Rkfix2\"";
	}
	else if (get_MW_methodID == 4)
	{
		return "\"Rkfix3\"";
	}
	else if (get_MW_methodID == 5)
	{
		return "\"Rkfix4\"";
	}
	else if (get_MW_methodID == 6)
	{
		return "\"Rkfix6\"";
	}
	else if (get_MW_methodID == 7)
	{
		return "\"Rkfix8\"";
	}
	return "\"Dassl\"";
}

//获得MWorks的输出方式，0为输出所有变量，1为输出与驱动动画相关的变量
int SimulateSetDlg::GetMWorks_OutputMethodID()
{
	static int get_MWorks_OutputMethodID;
	get_MWorks_OutputMethodID = temp_MWorks_OutputMethodID;
	return get_MWorks_OutputMethodID;
}