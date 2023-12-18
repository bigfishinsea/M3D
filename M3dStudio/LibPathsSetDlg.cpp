#include "M3d.h"
#include "GeomWidget.h"
#include "LibPathsSetDlg.h"
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>
#include <Geom_CartesianPoint.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Geom_Plane.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <qpushbutton.h>
#include <QMessageBox>
#include <QFileDialog>
#include "mainwindow.h"
#include <QLineEdit>
#include <QSettings>


QSettings setting("HKEY_CURRENT_USER\\SOFTWARE\\M3d\\Test", QSettings::NativeFormat);
static QString temp_ModelLibPath = setting.value("ModelLibPaths", "").toString();
static QString temp_WorkList = setting.value("AnimationWorkList", "").toString();
static QString temp_SolverPath = setting.value("SolverPath", "").toString();
//static QString temp_WorkList = "C:/Users/dell/Desktop/SimulationFolder";//默认仿真工作文件夹路径
//static QString temp_SolverPath = "D:/openmodelica/bin";//默认仿真求解器路径
static double temp_GlobalSysSize = setting.value("GlobalSystemSize", "").toString().toDouble();
static double temp_LocalSysSize = setting.value("LocalSystemSize", "").toString().toDouble();
static double temp_ConnSize = setting.value("ConnectionSize", "").toString().toDouble();
static int temp_SolverID = setting.value("SolverID", "").toString().toInt();



LibPathsSetDlg::LibPathsSetDlg(QWidget* parent) :QDialog(parent),
dirpath1(temp_ModelLibPath), WorkList2(temp_WorkList), SolverPath3(temp_SolverPath), 
GlobalSysSize4(temp_GlobalSysSize), LocalSysSize5(temp_LocalSysSize), ConnSize6(temp_ConnSize),SolverID(temp_SolverID)
{
	dpDlgInit();
}

void LibPathsSetDlg::dpDlgInit()
{
	setWindowTitle(tr("设置"));
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

	//1、modelica模型库路径添加1
	form = new QVBoxLayout(this);//垂直布局
	defPath1 = new QLabel("模型库设置");
	font = defPath1->font();//加粗字体
	font.setBold(true);
	defPath1->setFont(font);
	defPath1->setMinimumHeight(25);

	showPath1 = new QLineEdit;
	showPath1->setReadOnly(false); //可编辑
	showPath1->setText(dirpath1);
	showPath1->setMinimumHeight(25);
	ShowModelLibPath = temp_ModelLibPath;

	PushButton1 = new QPushButton(this);
	PushButton1->setText("模型库路径添加");
	PushButton1->setFixedSize(175, 25);
	//PushButton1->setMaximumSize(150, 25);
	inputPath1 = new QHBoxLayout(this);//水平布局
	inputPath1->addWidget(showPath1);
	inputPath1->addWidget(PushButton1);
	connect(PushButton1, SIGNAL(clicked()), this, SLOT(ClickButton1()));


	//2、仿真工作文件夹路径设置
	defWorkList2 = new QLabel("仿真工作目录设置");
	font = defWorkList2->font();//加粗字体
	font.setBold(true);
	defWorkList2->setFont(font);
	defWorkList2->setMinimumHeight(25);

	showWorkList2 = new QLineEdit;
	showWorkList2->setReadOnly(false); //可编辑
	showWorkList2->setText(WorkList2);
	showWorkList2->setMinimumHeight(25);
	//worklist = temp_WorkList.toStdString();

	PushButton2 = new QPushButton(this);
	PushButton2->setText("仿真工作目录选择");
	PushButton2->setFixedSize(175, 25);
	//PushButton2->setMaximumSize(150, 25);
	inputWorkList2 = new QHBoxLayout(this);
	inputWorkList2->addWidget(showWorkList2);
	inputWorkList2->addWidget(PushButton2);
	connect(PushButton2, SIGNAL(clicked()), this, SLOT(ClickButton2()));


	//3、仿真求解器所在文件夹路径设置
	defSolverPath3 = new QLabel("求解器文件夹路径选择");
	font = defSolverPath3->font();//加粗字体
	font.setBold(true);
	defSolverPath3->setFont(font);
	defSolverPath3->setMinimumHeight(25);

	showSolverPath3 = new QLineEdit;
	showSolverPath3->setReadOnly(false); //可编辑
	showSolverPath3->setText(SolverPath3);
	showSolverPath3->setMinimumHeight(25);

	PushButton3 = new QPushButton(this);
	PushButton3->setText("求解器文件夹路径选择");
	PushButton3->setFixedSize(175, 25);
	//PushButton3->setMaximumSize(150, 25);
	inputSolverPath3 = new QHBoxLayout(this);
	inputSolverPath3->addWidget(showSolverPath3);
	inputSolverPath3->addWidget(PushButton3);
	connect(PushButton3, SIGNAL(clicked()), this, SLOT(ClickButton3()));

	//选择求解器
	ButtonOpenmodelica = new QRadioButton(this);
	ButtonOpenmodelica->setText("openmodelica");
	ButtonMWorks = new QRadioButton(this);
	ButtonMWorks->setText("MWorks");
	groupButtonSolvers = new QButtonGroup(this);
	groupButtonSolvers->addButton(ButtonOpenmodelica, 0);//id为0
	groupButtonSolvers->addButton(ButtonMWorks, 1);//id为1
	SelectSolver = new QHBoxLayout(this);
	SelectSolver->addWidget(ButtonOpenmodelica);
	SelectSolver->addWidget(ButtonMWorks);
	groupButtonSolvers->button(SolverID)->setChecked(true); //默认选中上次选择的求解器


	//4.全局坐标系尺寸设置
	defGlobalSysSize4 = new QLabel("全局坐标系尺寸：");
	font = defGlobalSysSize4->font();//加粗字体
	font.setBold(true);
	defGlobalSysSize4->setFont(font);
	defGlobalSysSize4->setMinimumHeight(25);

	showGlobalSysSize4 = new QLineEdit;
	showGlobalSysSize4->setReadOnly(false); //可编辑
	QDoubleValidator* adoubleValidator4 = new QDoubleValidator(0, 500, 2, this); //四个参数的意义分别是最小值，最大值，小数位数，继承于
	showGlobalSysSize4->setValidator(adoubleValidator4);
	showGlobalSysSize4->setText(QString::number(GlobalSysSize4, 'f', 2));//指定小数位数为2
	showGlobalSysSize4->setMinimumHeight(25);
	showGlobalSysSize4->setFixedWidth(50);
	inputSysSize = new QHBoxLayout(this);
	inputSysSize->addWidget(defGlobalSysSize4);
	inputSysSize->addWidget(showGlobalSysSize4);

	//5.局部坐标系尺寸设置
	defLocalSysSize5 = new QLabel("局部坐标系尺寸：");
	font = defLocalSysSize5->font();//加粗字体
	font.setBold(true);
	defLocalSysSize5->setFont(font);
	defLocalSysSize5->setMinimumHeight(25);

	showLocalSysSize5 = new QLineEdit;
	showLocalSysSize5->setReadOnly(false); //可编辑
	QDoubleValidator* adoubleValidator5 = new QDoubleValidator(0, 500, 2, this); //四个参数的意义分别是最小值，最大值，小数位数，继承于
	showLocalSysSize5->setValidator(adoubleValidator5);
	showLocalSysSize5->setText(QString::number(LocalSysSize5, 'f', 2));//指定小数位数为2
	showLocalSysSize5->setMinimumHeight(25);
	showLocalSysSize5->setFixedWidth(50);
	inputSysSize->addWidget(defLocalSysSize5);
	inputSysSize->addWidget(showLocalSysSize5);

	//6.接口尺寸设置
	defConnSize6 = new QLabel("接口尺寸：");
	font = defConnSize6->font();//加粗字体
	font.setBold(true);
	defConnSize6->setFont(font);
	defConnSize6->setMinimumHeight(25);

	showConnSize6 = new QLineEdit;
	showConnSize6->setReadOnly(false); //可编辑
	QDoubleValidator* adoubleValidator6 = new QDoubleValidator(0, 500, 2, this); //四个参数的意义分别是最小值，最大值，小数位数，继承于
	showConnSize6->setValidator(adoubleValidator6);
	showConnSize6->setText(QString::number(ConnSize6, 'f', 2));//指定小数位数为2
	showConnSize6->setMinimumHeight(25);
	showConnSize6->setFixedWidth(50);
	inputSysSize->addWidget(defConnSize6);
	inputSysSize->addWidget(showConnSize6);


	//增加ok，cancel
	okBtn = new QPushButton(this);
	okBtn->setText("确定");
	cancelBtn = new QPushButton(this);
	cancelBtn->setText("取消");
	hlayout = new QHBoxLayout(this);
	hlayout->addWidget(okBtn);
	hlayout->addWidget(cancelBtn);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);
	connect(okBtn, &QPushButton::clicked, this, &LibPathsSetDlg::Accept);


	//添加到布局中
	form->addWidget(defPath1);
	form->addLayout(inputPath1);
	form->addWidget(defWorkList2);
	form->addLayout(inputWorkList2);
	form->addWidget(defSolverPath3);
	form->addLayout(inputSolverPath3);
	form->addLayout(SelectSolver);
	form->addLayout(inputSysSize);

	form->addLayout(hlayout);

	setLayout(form);
	setFixedWidth(600);//设置对话框宽度恒为600
	form->setContentsMargins(20, 20, 20, 20);
}

LibPathsSetDlg::~LibPathsSetDlg()
{

}

void LibPathsSetDlg::Accept()
{
	//1.modelica模型库添加判断
	if (showPath1->text() != temp_ModelLibPath) { change = 1; }
	else { change = 0; }
	if (change == 1)
	{
		temp_ModelLibPath = showPath1->text();//目前文件库路径
		setting.setValue("ModelLibPaths", temp_ModelLibPath);
	}
	//2.仿真工作目录更改判断
	if (showWorkList2->text() == nullptr)
	{
		QMessageBox::information(NULL, "提示", "仿真工作目录不能为空，请重新选择！", QMessageBox::Ok);
		return;
	}
	else if (showWorkList2->text() == temp_WorkList) { change_worklist = 0; }
	else { change_worklist = 1; }
	if (change_worklist == 1)
	{
		temp_WorkList = showWorkList2->text();
		setting.setValue("AnimationWorkList", temp_WorkList);
		//worklist = temp_WorkList.toStdString();
	}
	//3.求解器路径更改判断
	if (showSolverPath3->text() == nullptr)
	{
		QMessageBox::information(NULL, "提示", "求解器路径不能为空，请重新选择！", QMessageBox::Ok);
		return;
	}
	else if (showSolverPath3->text() == temp_SolverPath) { change_solverpath = 0; }
	else { change_solverpath = 1; }
	if (change_solverpath == 1)
	{
		temp_SolverPath = showSolverPath3->text();
		setting.setValue("SolverPath", temp_SolverPath);
	}
	
	//4.全局坐标系尺寸设置
	if (showGlobalSysSize4->text().toDouble() != temp_GlobalSysSize)
	{
		temp_GlobalSysSize = showGlobalSysSize4->text().toDouble();
		setting.setValue("GlobalSystemSize", temp_GlobalSysSize);
	}
	
	//5.局部坐标系尺寸设置
	if (showLocalSysSize5->text().toDouble() != temp_LocalSysSize)
	{
		temp_LocalSysSize = showLocalSysSize5->text().toDouble();
		setting.setValue("LocalSystemSize", temp_LocalSysSize);
	}

	//6.接口尺寸设置
	if (showConnSize6->text().toDouble() != temp_ConnSize)
	{
		temp_ConnSize = showConnSize6->text().toDouble();
		setting.setValue("ConnectionSize", temp_ConnSize);
	}

	//7.选择求解器
	temp_SolverID = groupButtonSolvers->checkedId();
	setting.setValue("SolverID", temp_SolverID);
	
	QDialog::close();
	//QString ReminderMessage = "全局坐标系尺寸为：" + QString::number(temp_GlobalSysSize, 'f', 2)+ "局部坐标系尺寸为：" + QString::number(temp_LocalSysSize, 'f', 2)+ "接口尺寸为：" + QString::number(temp_ConnSize, 'f', 2);
	//QMessageBox::information(NULL, "提示", ReminderMessage, QMessageBox::Ok);
}


void LibPathsSetDlg::ClickButton1()
{
	dirpath1 = QFileDialog::getOpenFileName(this, tr("Select a .Mo file"), QString(), tr("Modelica Files(*.Mo);;All files(*.*)"));
	ShowModelLibPath = ShowModelLibPath + dirpath1 + ";";
	showPath1->setText(ShowModelLibPath);
}

void LibPathsSetDlg::ClickButton2()
{
	WorkList2 = QFileDialog::getExistingDirectory(this, "choose src Directory", "/");
	showWorkList2->setText(WorkList2);
}

void LibPathsSetDlg::ClickButton3()
{
	SolverPath3 = QFileDialog::getExistingDirectory(this, "choose src Directory", "/");
	showSolverPath3->setText(SolverPath3);
}

//根据字符切分string，兼容最前最后存在字符
void LibPathsSetDlg::CutString(string line, vector<string>& subline, char a)
{
	//首字母为a，剔除首字母
	if (line.size() < 1)
	{
		return;
	}
	if (line[0] == a)
	{
		line.erase(0, 1);
	}

	size_t pos = 0;
	while (pos < line.length())
	{
		size_t curpos = pos;
		pos = line.find(a, curpos);
		if (pos == string::npos)
		{
			pos = line.length();
		}
		subline.push_back(line.substr(curpos, pos - curpos));
		pos++;
	}

	return;
}

//获得*.mo模型库路径
vector<string> LibPathsSetDlg::GetModelLibPaths()
{
	static vector<string> vec_ModelLibPath;
	vec_ModelLibPath.clear();
	static QString ModelLibPath;
	ModelLibPath = setting.value("ModelLibPaths", "").toString();
	static string str_ModelLibPath = ModelLibPath.toStdString();
	static char a = ';';
	CutString(str_ModelLibPath, vec_ModelLibPath, a);
	return vec_ModelLibPath;
}

//获得仿真工作目录路径
string LibPathsSetDlg::GetWorkDir()
{
	static QString WorkList;
	WorkList = temp_WorkList;
	return WorkList.toStdString();
}

//获得求解器文件夹路径
string LibPathsSetDlg::GetSolverPath()
{
	static QString SolverPath;
	SolverPath = temp_SolverPath;
	return SolverPath.toStdString();
}

//获得求解器所在磁盘
string LibPathsSetDlg::GetSolverDisk()
{
	static QString SolverPath;
	SolverPath = temp_SolverPath;
	return SolverPath.toStdString().substr(0, 2);//从字符串下标为0的地方开始截取，截取长度为2，输出D:
}

//获得全局坐标系尺寸
double LibPathsSetDlg::GetGlobalSysSize()
{
	static double GlobalSysSize;
	GlobalSysSize = temp_GlobalSysSize;
	return GlobalSysSize;
}

//获得局部坐标系尺寸
double LibPathsSetDlg::GetLocalSysSize()
{
	static double LocalSysSize;
	LocalSysSize = temp_LocalSysSize;
	return LocalSysSize;
}

//获得接口尺寸
double LibPathsSetDlg::GetConnSize()
{
	static double ConnSize;
	ConnSize = temp_ConnSize;
	return ConnSize;
}

//获得求解器名称
string LibPathsSetDlg::GetSolverName()
{
	static int SolverID;
	SolverID = temp_SolverID;
	if (SolverID == 0)
	{
		return "openmodelica";
	}
	else if (SolverID == 1)
	{
		return "MWorks";
	}
	else
	{
		return "NotFound";
	}
}