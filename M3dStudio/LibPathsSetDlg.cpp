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
//static QString temp_WorkList = "C:/Users/dell/Desktop/SimulationFolder";//Ĭ�Ϸ��湤���ļ���·��
//static QString temp_SolverPath = "D:/openmodelica/bin";//Ĭ�Ϸ��������·��
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
	setWindowTitle(tr("����"));
	BtnStyle = QString(
		//����״̬
		"QPushButton{"
		"background-color:white;"//������ɫ��͸����
		"color:black;"//������ɫ
		"border: 2px inset rgb(128,128,128);"//�߿��ȡ���ʽ�Լ���ɫ
		"font: 18px;"//�������ͺ������С
		"text-align: left;"
		"}"
		//����ʱ��״̬
		"QPushButton:pressed{"
		"background-color:rgb(255,255,255);"
		"color:rgb(0,0,0);"
		"}"
		//������
		//"QComboBox{"
		//"font: 18px;"
		//"}"
	);

	//1��modelicaģ�Ϳ�·�����1
	form = new QVBoxLayout(this);//��ֱ����
	defPath1 = new QLabel("ģ�Ϳ�����");
	font = defPath1->font();//�Ӵ�����
	font.setBold(true);
	defPath1->setFont(font);
	defPath1->setMinimumHeight(25);

	showPath1 = new QLineEdit;
	showPath1->setReadOnly(false); //�ɱ༭
	showPath1->setText(dirpath1);
	showPath1->setMinimumHeight(25);
	ShowModelLibPath = temp_ModelLibPath;

	PushButton1 = new QPushButton(this);
	PushButton1->setText("ģ�Ϳ�·�����");
	PushButton1->setFixedSize(175, 25);
	//PushButton1->setMaximumSize(150, 25);
	inputPath1 = new QHBoxLayout(this);//ˮƽ����
	inputPath1->addWidget(showPath1);
	inputPath1->addWidget(PushButton1);
	connect(PushButton1, SIGNAL(clicked()), this, SLOT(ClickButton1()));


	//2�����湤���ļ���·������
	defWorkList2 = new QLabel("���湤��Ŀ¼����");
	font = defWorkList2->font();//�Ӵ�����
	font.setBold(true);
	defWorkList2->setFont(font);
	defWorkList2->setMinimumHeight(25);

	showWorkList2 = new QLineEdit;
	showWorkList2->setReadOnly(false); //�ɱ༭
	showWorkList2->setText(WorkList2);
	showWorkList2->setMinimumHeight(25);
	//worklist = temp_WorkList.toStdString();

	PushButton2 = new QPushButton(this);
	PushButton2->setText("���湤��Ŀ¼ѡ��");
	PushButton2->setFixedSize(175, 25);
	//PushButton2->setMaximumSize(150, 25);
	inputWorkList2 = new QHBoxLayout(this);
	inputWorkList2->addWidget(showWorkList2);
	inputWorkList2->addWidget(PushButton2);
	connect(PushButton2, SIGNAL(clicked()), this, SLOT(ClickButton2()));


	//3����������������ļ���·������
	defSolverPath3 = new QLabel("������ļ���·��ѡ��");
	font = defSolverPath3->font();//�Ӵ�����
	font.setBold(true);
	defSolverPath3->setFont(font);
	defSolverPath3->setMinimumHeight(25);

	showSolverPath3 = new QLineEdit;
	showSolverPath3->setReadOnly(false); //�ɱ༭
	showSolverPath3->setText(SolverPath3);
	showSolverPath3->setMinimumHeight(25);

	PushButton3 = new QPushButton(this);
	PushButton3->setText("������ļ���·��ѡ��");
	PushButton3->setFixedSize(175, 25);
	//PushButton3->setMaximumSize(150, 25);
	inputSolverPath3 = new QHBoxLayout(this);
	inputSolverPath3->addWidget(showSolverPath3);
	inputSolverPath3->addWidget(PushButton3);
	connect(PushButton3, SIGNAL(clicked()), this, SLOT(ClickButton3()));

	//ѡ�������
	ButtonOpenmodelica = new QRadioButton(this);
	ButtonOpenmodelica->setText("openmodelica");
	ButtonMWorks = new QRadioButton(this);
	ButtonMWorks->setText("MWorks");
	groupButtonSolvers = new QButtonGroup(this);
	groupButtonSolvers->addButton(ButtonOpenmodelica, 0);//idΪ0
	groupButtonSolvers->addButton(ButtonMWorks, 1);//idΪ1
	SelectSolver = new QHBoxLayout(this);
	SelectSolver->addWidget(ButtonOpenmodelica);
	SelectSolver->addWidget(ButtonMWorks);
	groupButtonSolvers->button(SolverID)->setChecked(true); //Ĭ��ѡ���ϴ�ѡ��������


	//4.ȫ������ϵ�ߴ�����
	defGlobalSysSize4 = new QLabel("ȫ������ϵ�ߴ磺");
	font = defGlobalSysSize4->font();//�Ӵ�����
	font.setBold(true);
	defGlobalSysSize4->setFont(font);
	defGlobalSysSize4->setMinimumHeight(25);

	showGlobalSysSize4 = new QLineEdit;
	showGlobalSysSize4->setReadOnly(false); //�ɱ༭
	QDoubleValidator* adoubleValidator4 = new QDoubleValidator(0, 500, 2, this); //�ĸ�����������ֱ�����Сֵ�����ֵ��С��λ�����̳���
	showGlobalSysSize4->setValidator(adoubleValidator4);
	showGlobalSysSize4->setText(QString::number(GlobalSysSize4, 'f', 2));//ָ��С��λ��Ϊ2
	showGlobalSysSize4->setMinimumHeight(25);
	showGlobalSysSize4->setFixedWidth(50);
	inputSysSize = new QHBoxLayout(this);
	inputSysSize->addWidget(defGlobalSysSize4);
	inputSysSize->addWidget(showGlobalSysSize4);

	//5.�ֲ�����ϵ�ߴ�����
	defLocalSysSize5 = new QLabel("�ֲ�����ϵ�ߴ磺");
	font = defLocalSysSize5->font();//�Ӵ�����
	font.setBold(true);
	defLocalSysSize5->setFont(font);
	defLocalSysSize5->setMinimumHeight(25);

	showLocalSysSize5 = new QLineEdit;
	showLocalSysSize5->setReadOnly(false); //�ɱ༭
	QDoubleValidator* adoubleValidator5 = new QDoubleValidator(0, 500, 2, this); //�ĸ�����������ֱ�����Сֵ�����ֵ��С��λ�����̳���
	showLocalSysSize5->setValidator(adoubleValidator5);
	showLocalSysSize5->setText(QString::number(LocalSysSize5, 'f', 2));//ָ��С��λ��Ϊ2
	showLocalSysSize5->setMinimumHeight(25);
	showLocalSysSize5->setFixedWidth(50);
	inputSysSize->addWidget(defLocalSysSize5);
	inputSysSize->addWidget(showLocalSysSize5);

	//6.�ӿڳߴ�����
	defConnSize6 = new QLabel("�ӿڳߴ磺");
	font = defConnSize6->font();//�Ӵ�����
	font.setBold(true);
	defConnSize6->setFont(font);
	defConnSize6->setMinimumHeight(25);

	showConnSize6 = new QLineEdit;
	showConnSize6->setReadOnly(false); //�ɱ༭
	QDoubleValidator* adoubleValidator6 = new QDoubleValidator(0, 500, 2, this); //�ĸ�����������ֱ�����Сֵ�����ֵ��С��λ�����̳���
	showConnSize6->setValidator(adoubleValidator6);
	showConnSize6->setText(QString::number(ConnSize6, 'f', 2));//ָ��С��λ��Ϊ2
	showConnSize6->setMinimumHeight(25);
	showConnSize6->setFixedWidth(50);
	inputSysSize->addWidget(defConnSize6);
	inputSysSize->addWidget(showConnSize6);


	//����ok��cancel
	okBtn = new QPushButton(this);
	okBtn->setText("ȷ��");
	cancelBtn = new QPushButton(this);
	cancelBtn->setText("ȡ��");
	hlayout = new QHBoxLayout(this);
	hlayout->addWidget(okBtn);
	hlayout->addWidget(cancelBtn);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);
	connect(okBtn, &QPushButton::clicked, this, &LibPathsSetDlg::Accept);


	//��ӵ�������
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
	setFixedWidth(600);//���öԻ����Ⱥ�Ϊ600
	form->setContentsMargins(20, 20, 20, 20);
}

LibPathsSetDlg::~LibPathsSetDlg()
{

}

void LibPathsSetDlg::Accept()
{
	//1.modelicaģ�Ϳ�����ж�
	if (showPath1->text() != temp_ModelLibPath) { change = 1; }
	else { change = 0; }
	if (change == 1)
	{
		temp_ModelLibPath = showPath1->text();//Ŀǰ�ļ���·��
		setting.setValue("ModelLibPaths", temp_ModelLibPath);
	}
	//2.���湤��Ŀ¼�����ж�
	if (showWorkList2->text() == nullptr)
	{
		QMessageBox::information(NULL, "��ʾ", "���湤��Ŀ¼����Ϊ�գ�������ѡ��", QMessageBox::Ok);
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
	//3.�����·�������ж�
	if (showSolverPath3->text() == nullptr)
	{
		QMessageBox::information(NULL, "��ʾ", "�����·������Ϊ�գ�������ѡ��", QMessageBox::Ok);
		return;
	}
	else if (showSolverPath3->text() == temp_SolverPath) { change_solverpath = 0; }
	else { change_solverpath = 1; }
	if (change_solverpath == 1)
	{
		temp_SolverPath = showSolverPath3->text();
		setting.setValue("SolverPath", temp_SolverPath);
	}
	
	//4.ȫ������ϵ�ߴ�����
	if (showGlobalSysSize4->text().toDouble() != temp_GlobalSysSize)
	{
		temp_GlobalSysSize = showGlobalSysSize4->text().toDouble();
		setting.setValue("GlobalSystemSize", temp_GlobalSysSize);
	}
	
	//5.�ֲ�����ϵ�ߴ�����
	if (showLocalSysSize5->text().toDouble() != temp_LocalSysSize)
	{
		temp_LocalSysSize = showLocalSysSize5->text().toDouble();
		setting.setValue("LocalSystemSize", temp_LocalSysSize);
	}

	//6.�ӿڳߴ�����
	if (showConnSize6->text().toDouble() != temp_ConnSize)
	{
		temp_ConnSize = showConnSize6->text().toDouble();
		setting.setValue("ConnectionSize", temp_ConnSize);
	}

	//7.ѡ�������
	temp_SolverID = groupButtonSolvers->checkedId();
	setting.setValue("SolverID", temp_SolverID);
	
	QDialog::close();
	//QString ReminderMessage = "ȫ������ϵ�ߴ�Ϊ��" + QString::number(temp_GlobalSysSize, 'f', 2)+ "�ֲ�����ϵ�ߴ�Ϊ��" + QString::number(temp_LocalSysSize, 'f', 2)+ "�ӿڳߴ�Ϊ��" + QString::number(temp_ConnSize, 'f', 2);
	//QMessageBox::information(NULL, "��ʾ", ReminderMessage, QMessageBox::Ok);
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

//�����ַ��з�string��������ǰ�������ַ�
void LibPathsSetDlg::CutString(string line, vector<string>& subline, char a)
{
	//����ĸΪa���޳�����ĸ
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

//���*.moģ�Ϳ�·��
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

//��÷��湤��Ŀ¼·��
string LibPathsSetDlg::GetWorkDir()
{
	static QString WorkList;
	WorkList = temp_WorkList;
	return WorkList.toStdString();
}

//���������ļ���·��
string LibPathsSetDlg::GetSolverPath()
{
	static QString SolverPath;
	SolverPath = temp_SolverPath;
	return SolverPath.toStdString();
}

//�����������ڴ���
string LibPathsSetDlg::GetSolverDisk()
{
	static QString SolverPath;
	SolverPath = temp_SolverPath;
	return SolverPath.toStdString().substr(0, 2);//���ַ����±�Ϊ0�ĵط���ʼ��ȡ����ȡ����Ϊ2�����D:
}

//���ȫ������ϵ�ߴ�
double LibPathsSetDlg::GetGlobalSysSize()
{
	static double GlobalSysSize;
	GlobalSysSize = temp_GlobalSysSize;
	return GlobalSysSize;
}

//��þֲ�����ϵ�ߴ�
double LibPathsSetDlg::GetLocalSysSize()
{
	static double LocalSysSize;
	LocalSysSize = temp_LocalSysSize;
	return LocalSysSize;
}

//��ýӿڳߴ�
double LibPathsSetDlg::GetConnSize()
{
	static double ConnSize;
	ConnSize = temp_ConnSize;
	return ConnSize;
}

//������������
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