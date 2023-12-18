#include "DDSData.h"
#include "DocumentModel.h"

#include "AnimationWidget.h"
#include "DocumentComponent.h"
#include <QAction>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
//#include "DocumentModel.h"
#include <QMessageBox>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <string>
#include "CSV_Parser.h"
#include <set>
#include "ParaTreeModel.h"
#include "cyberInfo.h"
#include <QMenu>
#include "LibPathsSetDlg.h"
#include <QKeyEvent>
#include <QTextCursor>



using namespace std;
using namespace CSV_Parser;

static CyberInfo* m_CyberInfo;

//AnimationWidget::AnimationWidget(MainWindow* mywidget):mywid(mywidget)
AnimationWidget::AnimationWidget(QWidget* parent) :Parent(parent)
{
	//this->myDocument3d = (DocumentModel*)qobject_cast<MainWindow*>(parent)->getDocument();
	this->m_doc = (DocumentCommon*)qobject_cast<MainWindow*>(parent)->getDocument();
	//this->m_pCompRoot = ((DocumentModel*)m_doc)->GetComponent();
	//this->m_pCompRoot = ((DocumentModel*)qobject_cast<MainWindow*>(parent)->getDocument())->GetComponent();

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	setMinimumWidth(300);
	setMaximumWidth(600);
	//setFixedWidth(400);

	dpWidInit();
}

void AnimationWidget::dpWidInit()
{

	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//设置整体布局为垂直布局
	VerticalLayout->setDirection(QBoxLayout::TopToBottom);

	//1.动画调节栏
	SetAnimation = new QLabel("动画调节栏");
	font = SetAnimation->font();//加粗字体
	font.setBold(true);
	SetAnimation->setFont(font);
	SetAnimation->setMinimumHeight(25);

	//播放按钮
	PushButtonPlay = new QPushButton(this);
	PushButtonPlay->setText("");
	QPixmap pixmap("./images/animationplay.png");
	QPixmap fitpixmap = pixmap.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	PushButtonPlay->setIcon(QIcon(fitpixmap));
	PushButtonPlay->setIconSize(QSize(50, 50));
	PushButtonPlay->setFlat(true);
	PushButtonPlay->setStyleSheet("border: 0px"); //消除边框
	PushButtonPlay->setToolTip("播放");
	//PushButtonPlay->setText("播放");
	//PushButtonPlay->setFixedSize(80, 80);
	connect(PushButtonPlay, SIGNAL(clicked()), this, SLOT(ClickButtonPlay()));

	//暂停按钮
	PushButtonPause = new QPushButton(this);
	PushButtonPause->setText("");
	QPixmap pixmap2("./images/animationpause.png");
	QPixmap fitpixmap2 = pixmap2.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	PushButtonPause->setIcon(QIcon(fitpixmap2));
	PushButtonPause->setIconSize(QSize(50, 50));
	PushButtonPause->setFlat(true);
	PushButtonPause->setStyleSheet("border: 0px"); //消除边框
	PushButtonPause->setToolTip("暂停");
	//PushButtonPause->setText("暂停");
	//PushButtonPause->setFixedSize(80, 80);
	connect(PushButtonPause, SIGNAL(clicked()), this, SLOT(ClickButtonPause()));

	//恢复按钮
	PushButtonResume = new QPushButton(this);
	PushButtonResume->setText("");
	QPixmap pixmap3("./images/animationcontinues.png");
	QPixmap fitpixmap3 = pixmap3.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	PushButtonResume->setIcon(QIcon(fitpixmap3));
	PushButtonResume->setIconSize(QSize(50, 50));
	PushButtonResume->setFlat(true);
	PushButtonResume->setStyleSheet("border: 0px"); //消除边框
	PushButtonResume->setToolTip("恢复播放");
	//PushButtonResume->setText("恢复");
	//PushButtonResume->setFixedSize(80, 80);
	connect(PushButtonResume, SIGNAL(clicked()), this, SLOT(ClickButtonResume()));

	//重置按钮
	PushButtonReset = new QPushButton(this);
	PushButtonReset->setText("");
	QPixmap pixmap4("./images/animationreset.png");
	QPixmap fitpixmap4 = pixmap4.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	PushButtonReset->setIcon(QIcon(fitpixmap4));
	PushButtonReset->setIconSize(QSize(50, 50));
	PushButtonReset->setFlat(true);
	PushButtonReset->setStyleSheet("border: 0px"); //消除边框
	PushButtonReset->setToolTip("重置动画");
	//PushButtonReset->setText("重置");
	//PushButtonReset->setFixedSize(80, 80);
	connect(PushButtonReset, SIGNAL(clicked()), this, SLOT(ClickButtonReset()));

	SetSpeed = new QLabel("速度调节");
	font = SetSpeed->font();//加粗字体
	font.setBold(true);
	SetSpeed->setFont(font);
	SetSpeed->setMinimumHeight(25);

	SpeedAdjust = new QDoubleSpinBox(this);
	SpeedAdjust->setRange(0.1, 5);  // 范围
	SpeedAdjust->setSingleStep(0.1); // 步长
	SpeedAdjust->setValue(1);  // 当前值
	SpeedAdjust->setWrapping(true);  // 开启循环
	SpeedAdjust->setDecimals(1);  // 精度
	connect(SpeedAdjust, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &AnimationWidget::ClickButtonSpeedAdjust);

	//2.变量显示栏
	ShowVariable = new QLabel("变量显示栏");
	font = ShowVariable->font();//加粗字体
	font.setBold(true);
	ShowVariable->setFont(font);
	ShowVariable->setMinimumHeight(25);

	PushButtonUpdateVariable = new QPushButton(this);
	PushButtonUpdateVariable->setText("加载所有变量");
	PushButtonUpdateVariable->setFixedSize(125, 25);
	connect(PushButtonUpdateVariable, SIGNAL(clicked()), this, SLOT(ClickButtonUpdateVariable()));

	PushButtonSave = new QPushButton(this);
	PushButtonSave->setText("保存修改");
	PushButtonSave->setFixedSize(125, 25);
	connect(PushButtonSave, SIGNAL(clicked()), this, SLOT(ClickButtonSave()));

	PushButtonOnlyShow = new QPushButton(this);
	PushButtonOnlyShow->setText("只显示关注");
	PushButtonOnlyShow->setFixedSize(125, 25);
	connect(PushButtonOnlyShow, SIGNAL(clicked()), this, SLOT(ClickButtonOnlyShow()));

	//构造view
	outputtreeview = new QTreeView(this);
	//1.QTreeView常用设置项
	//outputtreeview->setEditTriggers(QTreeView::NoEditTriggers);			//单元格不能编辑
	outputtreeview->setSelectionBehavior(QTreeView::SelectRows);			//一次选中整行
	outputtreeview->setSelectionMode(QTreeView::SingleSelection);          //单选，配合上面的整行就是一次选单行
	//outputtreeview->setAlternatingRowColors(true);                       //每间隔一行颜色不一样，当有qss时该属性无效
	//outputtreeview->setFocusPolicy(Qt::NoFocus);                         //去掉鼠标移到单元格上时的虚线框
	//2.列头相关设置
	//outputtreeview->header()->hide();                                  //隐藏列头
	outputtreeview->header()->setStretchLastSection(true);               //最后一列自适应宽度
	outputtreeview->header()->setDefaultAlignment(Qt::AlignCenter);      //列头文字默认居中对齐

	//CreateOutputModel();//构造outputtreemodel
	outputtreemodel = new outputTreeModel(nullptr);
	outputtreeview->setModel(outputtreemodel);
	outputtreeview->setItemDelegateForColumn(2, new CheckBoxDelegate(outputtreeview));
	//outputtreeview->expandAll();

	//信号槽，右键菜单
	outputtreeview->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(outputtreeview, &QTreeView::customContextMenuRequested, this, &AnimationWidget::slotTreeMenu);

	//3.参数显示栏
	ShowParameter = new QLabel("参数显示栏");
	font = ShowParameter->font();//加粗字体
	font.setBold(true);
	ShowParameter->setFont(font);
	ShowParameter->setMinimumHeight(25);

	PushButtonShowAllPara = new QPushButton(this);
	PushButtonShowAllPara->setText("显示所有参数");
	PushButtonShowAllPara->setFixedSize(125, 25);
	connect(PushButtonShowAllPara, SIGNAL(clicked()), this, SLOT(ClickButtonShowAllPara()));

	PushButtonShowAttenPara = new QPushButton(this);
	PushButtonShowAttenPara->setText("显示关注参数");
	PushButtonShowAttenPara->setFixedSize(125, 25);
	connect(PushButtonShowAttenPara, SIGNAL(clicked()), this, SLOT(ClickButtonShowAttenPara()));

	PushButtonSavePara = new QPushButton(this);
	PushButtonSavePara->setText("保存修改参数");
	PushButtonSavePara->setFixedSize(125, 25);
	connect(PushButtonSavePara, SIGNAL(clicked()), this, SLOT(ClickButtonSavePara()));

	PushButtonResimulate = new QPushButton(this);
	PushButtonResimulate->setText("重新仿真");
	PushButtonResimulate->setFixedSize(125, 25);
	connect(PushButtonResimulate, SIGNAL(clicked()), this, SLOT(ClickButtonResimulate()));

	//构造view
	paratreeview = new QTreeView(this);
	paratreeview->setSelectionBehavior(QTreeView::SelectRows);			//一次选中整行
	paratreeview->setSelectionMode(QTreeView::SingleSelection);         //单选，配合上面的整行就是一次选单行
	paratreeview->header()->setStretchLastSection(true);                //最后一列自适应宽度
	paratreeview->header()->setDefaultAlignment(Qt::AlignCenter);       //列头文字默认居中对齐
	//构造paratreemodel
	paratreemodel = new QStandardItemModel(this);
	paratreeview->setModel(paratreemodel);
	PushButtonSavePara->setFocus();
	PushButtonSavePara->setDefault(true);
	paratreeview->installEventFilter(this);                             //设置完后自动调用其eventFilter函数
	//信号槽，右键菜单
	paratreeview->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(paratreeview, &QTreeView::customContextMenuRequested, this, &AnimationWidget::slotParaTreeMenu);

	//4.仿真信息提示栏
	ShowMessage = new QLabel("仿真信息提示");
	font = ShowMessage->font();//加粗字体
	font.setBold(true);
	ShowMessage->setFont(font);
	ShowMessage->setMinimumHeight(25);

	AnimationMessage = new QTextEdit(this);
	AnimationMessage->setText("仿真信息如下...\n");
	//AnimationMessage->setMaximumHeight(100);

	//5.仿真信息同步发送栏
	PushButtonSendSimData = new QPushButton(this);
	PushButtonSendSimData->setText("仿真信息同步发送");
	PushButtonSendSimData->setFixedSize(400, 25);
	connect(PushButtonSendSimData, SIGNAL(clicked()), this, SLOT(ClickButtonSendSimData()));

	//布局
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(SetAnimation);
	SetAnimation->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(this);
	HorinztalLayout2->addWidget(PushButtonPlay);
	HorinztalLayout2->addWidget(PushButtonPause);
	HorinztalLayout2->addWidget(PushButtonResume);
	HorinztalLayout2->addWidget(PushButtonReset);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->addWidget(SetSpeed);
	HorinztalLayout3->addWidget(SpeedAdjust);
	QHBoxLayout* HorinztalLayout4 = new QHBoxLayout(this);
	HorinztalLayout4->addWidget(ShowVariable);
	HorinztalLayout4->addWidget(PushButtonUpdateVariable);
	HorinztalLayout4->addWidget(PushButtonSave);
	HorinztalLayout4->addWidget(PushButtonOnlyShow);
	QHBoxLayout* HorinztalLayout5 = new QHBoxLayout(this);
	HorinztalLayout5->addWidget(ShowParameter);
	HorinztalLayout5->addWidget(PushButtonShowAllPara);
	HorinztalLayout5->addWidget(PushButtonShowAttenPara);
	HorinztalLayout5->addWidget(PushButtonSavePara);
	HorinztalLayout5->addWidget(PushButtonResimulate);
	QHBoxLayout* HorinztalLayout6 = new QHBoxLayout(this);
	HorinztalLayout6->addWidget(ShowMessage);
	QHBoxLayout* HorinztalLayout7 = new QHBoxLayout(this);
	HorinztalLayout7->addWidget(AnimationMessage);
	QHBoxLayout* HorinztalLayout8 = new QHBoxLayout(this);
	HorinztalLayout8->addWidget(PushButtonSendSimData);

	VerticalLayout->setSpacing(15);
	VerticalLayout->addLayout(HorinztalLayout1);
	VerticalLayout->addLayout(HorinztalLayout2);
	VerticalLayout->addLayout(HorinztalLayout3);
	VerticalLayout->addLayout(HorinztalLayout4);
	VerticalLayout->addWidget(outputtreeview);
	VerticalLayout->addLayout(HorinztalLayout5);
	VerticalLayout->addWidget(paratreeview);
	VerticalLayout->addLayout(HorinztalLayout6);
	VerticalLayout->addLayout(HorinztalLayout7);
	VerticalLayout->addLayout(HorinztalLayout8);


	//QWidget* temp = new QWidget();
	//temp->setLayout(VerticalLayout);
	//m_dock->setWidget(temp);
	this->setLayout(VerticalLayout);

}

AnimationWidget::~AnimationWidget()
{
	if (InfoSendDlg != nullptr) {
		delete InfoSendDlg;
	}
	delete outputtreemodel;
	delete outputtreeview;
	delete paratreemodel;
	delete paratreeview;
	delete AttenOutputTreeModel;
}

void AnimationWidget::GetAnimationMessage(bool isEnd)
{
	vector<QString> message;
	QString display;
	if (m_doc && m_doc->IsModel())
	{
		message = ((DocumentModel*)m_doc)->AniWidGetMessage();
	}
	if (isEnd == true)
	{
		//display = "结束信息：\n" + message[2] + "\n" + "此次仿真已结束\n";
		display = "结束信息：\n此次仿真已结束\n";
		string SolverName = LibPathsSetDlg::GetSolverName();
		if (SolverName == "MWorks")
		{
			m_pCompRoot = ((DocumentModel*)m_doc)->GetComponent();
			string sWorkDir = LibPathsSetDlg::GetWorkDir();
			if (sWorkDir.back() != '/')
			{
				sWorkDir += "/";
			}
			string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//对不同模型有不同文件夹的相对路径
			string RecordFilePath = saveCompareRaw + "RecordOutput.txt";
			char* fn = (char*)RecordFilePath.c_str();//这里如果用VS可能要来个强制转换(char*)"a.txt"
			string str;
			str = readFileIntoString(fn);
			display += QString::fromStdString(str);
		}
	}
	else
	{
		if (message[0] == "")
		{
			display = "输出信息：\n" + message[1];
		}
		else if (message[1] == "")
		{
			display = "错误信息：\n" + message[0];
		}
		else
		{
			display = "错误信息：\n" + message[0] + "输出信息：\n" + message[1];
		}
	}
	MessageDisplay += display;
	AnimationMessage->setText(MessageDisplay);
	//利用光标使滚动条始终置于底部
	QTextCursor textCursor = AnimationMessage->textCursor();
	textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	AnimationMessage->setTextCursor(textCursor);
}

//显示后处理信息
void AnimationWidget::ShowPostMessage(bool isSimulate, bool isSimulateSuccess)
{
	if (isSimulate == false) {
		//如果没有进行仿真，显示信息：仿真结果最新，自动加载关注变量和关注参数
		AnimationMessage->setText("-------------仿真结果已是最新-------------");
		ClickButtonOnlyShow();
		ClickButtonShowAttenPara();
	}else {
		if (isSimulateSuccess == true) {
			MessageDisplay += "\n-------------仿真成功-------------\n";
			AnimationMessage->setText(MessageDisplay);
			ClickButtonOnlyShow();
			ClickButtonShowAttenPara();
		}
		else {
			MessageDisplay += "\n-------------仿真失败-------------\n";
			AnimationMessage->setText(MessageDisplay);
			outputtreeview->setModel(nullptr);
			paratreeview->setModel(nullptr);
		}
	}

}

//清除消息框
void AnimationWidget::ClearMessage()
{
	MessageDisplay = "";
	AnimationMessage->setText("-------------仿真进行中，请稍候-------------");
	outputtreeview->setModel(nullptr);
	paratreeview->setModel(nullptr);
}

//从文件读入到string里
string AnimationWidget::readFileIntoString(char* filename)
{
	ifstream ifile(filename);
	//将文件读入到ostringstream对象buf中
	ostringstream buf;
	char ch;
	while (buf && ifile.get(ch))
		buf.put(ch);
	//返回与流对象buf关联的字符串
	return buf.str();
}

void AnimationWidget::exctReadStandardOutput()
{
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->exctReadStandardOutput();
	}
	GetAnimationMessage(false);
}

void AnimationWidget::exctReadStandardError()
{
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->exctReadStandardError();
	}
	GetAnimationMessage(false);
}


void AnimationWidget::exctEnd(int exitCode, QProcess::ExitStatus exitStatus)
{
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->exctEnd(exitCode, exitStatus);
	}
	GetAnimationMessage(true);
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->SetPostMessage();
	}
}

void AnimationWidget::ClickButtonPlay()
{
	//DocumentCommon* m_doc = mywid->getDocument();
	//如果在动画执行过程中，则退出
	//启动动画线程
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->Animating();
	}
}

void AnimationWidget::ClickButtonPause()
{
	//DocumentCommon* m_doc = mywid->getDocument();
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->AnimatingPause();
	}
}

void AnimationWidget::ClickButtonResume()
{
	//DocumentCommon* m_doc = mywid->getDocument();
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->AnimatingResume();
	}
}

void AnimationWidget::ClickButtonReset()
{
	//DocumentCommon* m_doc = mywid->getDocument();
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->AnimatingReset();
	}
}

void AnimationWidget::ClickButtonSpeedAdjust()
{
	speed = SpeedAdjust->value();
	//string ReminderMessage = "现在速度倍速为" + to_string(speed);
	//QMessageBox::information(NULL, "提示", QString::fromStdString(ReminderMessage), QMessageBox::Ok);
}

double AnimationWidget::GetAnimationSpeed()
{
	return speed;
}

bool AnimationWidget::readCyberInfo(CyberInfo* pCyberInfo)
{
	if (!pCyberInfo) {
		QMessageBox msgBox;
		msgBox.setText("读取失败");
		msgBox.exec();
		return false;
	}
	//构建输出变量表
	//QStringList m_outputList;
	int size = pCyberInfo->GetOutputVars().size();
	for (int itr = 0; itr < size; ++itr) {
		string s = "";
		string s_FullName = "";
		string cur = "";
		//1.全名处理
		string sName = pCyberInfo->GetOutputVars().at(itr)->sFullName;
		string sName_Full = sCompName + "." + pCyberInfo->GetOutputVars().at(itr)->sFullName;
		int n_times = count(sName.begin(), sName.end(), '.') + 1;//由于加入了组件名，所以+1
		//int n_times = count(sName.begin(), sName.end(), '.');
		if (n_times - 1) sName.replace(0, sName.find_last_of('.') + 1, "");//判断条件相应-1
		string space = "";
		while (n_times)
		{
			space += " ";
			n_times--;
		}
		s += space;
		s += "NULL";
		s += "\t";
		s += sName;

		s_FullName += "NULL";
		s_FullName += "\t";
		s_FullName += sName_Full;

		//3.是否关注与关注名
		s += "\t";
		s += pCyberInfo->GetOutputVars().at(itr)->bFocused ? "是" : "NULL";

		s_FullName += "\t";
		s_FullName += pCyberInfo->GetOutputVars().at(itr)->bFocused ? "是" : "NULL";

		//2.类型
		cur = pCyberInfo->GetOutputVars().at(itr)->sType;
		if (cur != "")
		{
			s += "\t";
			s += cur;

			s_FullName += "\t";
			s_FullName += cur;
		}

		s += "\t";
		s_FullName += "\t";
		cur = "";
		cur = pCyberInfo->GetOutputVars().at(itr)->sFocusName;
		s += cur == "" ? "NULL" : cur;
		s_FullName += cur == "" ? "NULL" : cur;
		m_outputList.append(QString::fromStdString(s));
		string::size_type idx;
		idx = s.find("是");
		if (idx != string::npos)
		{
			outputList_Attention.append(QString::fromStdString(s));
			AttenOutPutList_FullName.append(QString::fromStdString(s_FullName));
		}
	}
	return true;
}

void AnimationWidget::CreateOutputModel()
{
	////构造model
	//outputtreemodel = new QStandardItemModel(this);
	//outputtreemodel->setHorizontalHeaderLabels(QStringList() << QString("变量名称") << QString("是否关注"));     //设置列头
	//QString m_sCSVFilePath = "D:/openmodelica/bin/Modelica.Mechanics.MultiBody.Examples.Elementary.Pendulum_res.csv";
	//QStringList AllVariablesNames = FileParse::GetAllVariablesNames(m_sCSVFilePath);
	////整理取出的变量名
	////首先获得所有FirstName
	//vector<QString> AllFirstName;//向量
	//set<QString> SetAllFirstName;//集合
	//for (int i = 0; i < AllVariablesNames.size(); i++)
	//{
	//	QString FullName = AllVariablesNames.at(i);
	//	//这个函数把字符串看成是几个块，这些块由 sep 分隔，start 和 end 指定块号
	//	//end 默认为 –1 ，返回的是[ start, end ]内的块组成的字符串
	//	QString FirstName = FullName.section(QRegExp("[.(]"), 0, 0);
	//	//QString LastName = FullName.section('.', 1, -1);
	//	SetAllFirstName.insert(FirstName);
	//}
	//AllFirstName.assign(SetAllFirstName.begin(), SetAllFirstName.end());//将集合转化为向量
	////然后把LastName填入二维向量
	//vector<vector<QString>> AllLastName;  //先声明一个二维向量
	//AllLastName.resize(AllFirstName.size());  //先根据需要指定外层大小
	//for (int i = 0; i < AllVariablesNames.size(); i++)
	//{
	//	QString FullName = AllVariablesNames.at(i);
	//	//这个函数把字符串看成是几个块，这些块由 sep 分隔，start 和 end 指定块号
	//	//end 默认为 –1 ，返回的是[ start, end ]内的块组成的字符串
	//	QString FirstName = FullName.section(QRegExp("[.(]"), 0, 0);
	//	QString LastName = FullName.section(QRegExp("[.(]"), 1, -1);
	//	LastName = LastName.remove(QRegExp("[()]"));
	//	for (int j = 0; j < AllFirstName.size(); j++)
	//	{
	//		if (FirstName == AllFirstName[j])
	//		{
	//			AllLastName[j].push_back(LastName);
	//		}
	//		else {}
	//	}
	//}
	////最后构造modeltree
	//for (int i = 0; i < AllFirstName.size(); i++)
	//{
	//	//一级节点，加入mModel
	//	QList<QStandardItem*> items1;
	//	QStandardItem* item1 = new QStandardItem(AllFirstName[i]);
	//	//QStandardItem* item2 = new QStandardItem(QString("关注"));
	//	items1.append(item1);
	//	//items1.append(item2);
	//	outputtreemodel->appendRow(items1);
	//	//二级节点，加入一级节点
	//	for (int j = 0; j < AllLastName[i].size(); j++)
	//	{
	//		QList<QStandardItem*> items2;
	//		QStandardItem* item3 = new QStandardItem(AllLastName[i][j]);
	//		QStandardItem* item4 = new QStandardItem(QString("关注"));
	//		items2.append(item3);
	//		items2.append(item4);
	//		item1->appendRow(items2);
	//	}

	//}



	//0304
	m_outputList.clear();
	outputList_Attention.clear();
	AttenOutPutList_FullName.clear();
	delete outputtreemodel;
	outputtreemodel = new outputTreeModel(nullptr);
	m_pCompRoot = ((DocumentModel*)m_doc)->GetComponent();
	list<Component*> comChilds = m_pCompRoot->GetChilds();
	list<Component*>::iterator itr = comChilds.begin();
	for (; itr != comChilds.end(); ++itr)
	{
		Component* pComChild = *itr;
		sCompName = pComChild->GetCompName();
		string sComp;
		sComp = "NULL\t" + sCompName + "\t" + "NULL\t" + "NULL\t" + "NULL";
		m_outputList.append(QString::fromStdString(sComp));
		outputList_Attention.append(QString::fromStdString(sComp));
		CyberInfo* childCompCyIn = pComChild->GetCyberInfo();
		readCyberInfo(childCompCyIn);
		delete childCompCyIn;
	}
	outputtreemodel->setupModelData(m_outputList, outputtreemodel->getrootItem());
}

void AnimationWidget::ClickButtonUpdateVariable()
{
	CreateOutputModel();
	outputtreeview->setModel(outputtreemodel);
	//outputtreeview->expandAll();
	outputtreeview->setEditTriggers(QAbstractItemView::DoubleClicked);  //双击可编辑
}

void AnimationWidget::ClickButtonSave()
{
	//模型输出变量
	list<Component*> comChilds = m_pCompRoot->GetChilds();
	list<Component*>::iterator itr = comChilds.begin();
	TreeItem* root = outputtreemodel->getrootItem();
	for (int i = 0; i < root->childCount(); i++)
	{
		Component* pComChild = *itr;
		CyberInfo* childCompCyIn = pComChild->GetCyberInfo();
		vector<myOutputVar*> m_output;
		m_CyberInfo = new CyberInfo(nullptr);
		TreeItem* root2 = root->child(i);

		for (int itr = 0; itr < root2->childCount(); itr++)
			OutputDFS(m_output, root2->child(itr));
		m_CyberInfo->SetOutputVars(m_output);

		string name = childCompCyIn->GetMdlName();
		string type = childCompCyIn->GetMdlType();
		vector<myParameter*> m_para = childCompCyIn->GetParameters();
		vector<string> m_connectortypes = childCompCyIn->GetConnectorTypes();
		vector<string> m_connectornames = childCompCyIn->GetConnectorNames();
		m_CyberInfo->SetMdlName(name);
		m_CyberInfo->SetMdlType(type.c_str());
		m_CyberInfo->SetParameters(m_para);
		m_CyberInfo->SetConncTypes(m_connectortypes);
		m_CyberInfo->SetConncNames(m_connectornames);

		((DocumentComponent*)pComChild->GetDocument())->SetCyberInfo(m_CyberInfo);
		++itr;
	}
	QMessageBox msgBox;
	msgBox.setText("保存成功！请先点击\"加载所有变量\"，再点击\"只显示关注\"。");
	msgBox.exec();
}


void AnimationWidget::ClickButtonOnlyShow()
{
	//构造model
	if (AttenOutputTreeModel != nullptr) {
		delete AttenOutputTreeModel;
	}
	AttenOutputTreeModel = new QStandardItemModel(this);
	AttenOutputTreeModel->setHorizontalHeaderLabels(QStringList() << QString("全名") << QString("类型") << QString("是否关注") << QString("变型"));     //设置列头
	//整理取出的AttentionOutPutList
	//获得变量全名、类型名
	vector<QString> vec_FullName;
	vector<QString> vec_Types;
	//vector<QString> AllFirstName;//向量
	//set<QString> SetAllFirstName;//集合
	for (int i = 0; i < AttenOutPutList_FullName.size(); i++)
	{
		//这个函数把字符串看成是几个块，这些块由 sep 分隔，start 和 end 指定块号
		//end 默认为 –1 ，返回的是[ start, end ]内的块组成的字符串
		QString FullString = AttenOutPutList_FullName.at(i);
		QString FullName = FullString.section("\t", 1, 1);
		vec_FullName.push_back(FullName);
		QString Types = FullString.section("\t", 3, 3);
		vec_Types.push_back(Types);
	}


	//最后构造modeltree
	for (int i = 0; i < AttenOutPutList_FullName.size(); i++)
	{
		//一级节点，加入mModel
		QList<QStandardItem*> items1;
		QStandardItem* item1 = new QStandardItem(vec_FullName[i]);
		QStandardItem* item2 = new QStandardItem(vec_Types[i]);
		QStandardItem* item3 = new QStandardItem(QString("是"));
		items1.append(item1);
		items1.append(item2);
		items1.append(item3);
		AttenOutputTreeModel->appendRow(items1);
	}
	outputtreeview->setModel(AttenOutputTreeModel);
	outputtreeview->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置其为不可编辑
	//outputtreeview->expandAll();

	//直接调用setupModelData函数树状结构显示有误
	//outputtreemodel->setupModelData(outputList_Attention, outputtreemodel->getrootItem());
	//outputtreeview->setModel(outputtreemodel);
	//outputtreeview->expandAll();

}

void AnimationWidget::OutputDFS(vector<myOutputVar*>& m_output, TreeItem* root, string s)
{
	if (s != "") s += ".";
	s += (root->data(0)).toString().toStdString();
	myOutputVar* temp = new myOutputVar();
	temp->sFullName = s;
	temp->sType = root->data(1).toString().toStdString();
	if (root->data(2) == "是") temp->bFocused = 1;
	else temp->bFocused = 0;
	temp->sFocusName = root->data(3).toString().toStdString();
	m_output.push_back(temp);
	for (int itr = 0; itr < root->childCount(); itr++)
		OutputDFS(m_output, root->child(itr), s);
}

void AnimationWidget::slotTreeMenu(const QPoint& pos)
{
	QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                QMenu::item{padding:3px 20px 3px 20px;}\
                QMenu::indicator{width:13px;height:13px;}\
                QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                QMenu::separator{height:1px;background:#757575;}";

	QMenu menu;
	//menu.setStyleSheet(qss);    //可以给菜单设置样式

	QModelIndex curIndex = outputtreeview->indexAt(pos);      //当前点击的元素的index
	QModelIndex index = curIndex.sibling(curIndex.row(), 0); //该行的第1列元素的index
	if (index.isValid())
	{
		//可获取元素的文本、data,进行其他判断处理
		//QStandardItem* item = AttenOutputTreeModel->itemFromIndex(index);
		//QString text = item->text();
		//QVariant data = item->data(Qt::UserRole + 1);
		//...

		//添加一行菜单
		menu.addAction(QString("输出曲线"), this, SLOT(MenuGeneCurves()));
		//menu.addSeparator();    //添加一个分隔线
		//menu.addAction(QString("折叠"), this, SLOT(slotTreeMenuCollapse(bool)));
	}
	menu.exec(QCursor::pos());  //显示菜单
}

void AnimationWidget::MenuGeneCurves()
{
	QModelIndex curIndex = outputtreeview->currentIndex();
	QModelIndex index = curIndex.sibling(curIndex.row(), 0); //同一行第一列元素的index
	string name;
	QString text;
	if (outputtreeview->model() != AttenOutputTreeModel)
	{
		if (index.isValid())
		{
			//virtual QMap<int, QVariant> itemData(const QModelIndex & index) const;
			QMap<int, QVariant> item = outputtreemodel->itemData(index);
			QString LastName = item[0].toString();
			//Q_INVOKABLE virtual QModelIndex parent(const QModelIndex & child) const = 0;
			//逐步寻找父节点，获得变量全名
			vector<QString> FullName;
			FullName.push_back(LastName);
			while (outputtreemodel->itemData(outputtreemodel->parent(index)).size() != 0)
			{
				QModelIndex parent = outputtreemodel->parent(index);
				QMap<int, QVariant> parent_item = outputtreemodel->itemData(parent);
				QString ParentName = parent_item[0].toString();
				FullName.push_back(ParentName);
				index = parent;
			}

			QString item_FullName;
			for (int i = FullName.size() - 1; i >= 1; i--)
			{
				item_FullName += FullName[i] + ".";
			}
			item_FullName += FullName[0];
			text = item_FullName;
			name = item_FullName.toStdString();
		}
	}
	else if (outputtreeview->model() == AttenOutputTreeModel)
	{
		if (index.isValid())
		{
			QStandardItem* item = AttenOutputTreeModel->itemFromIndex(index);
			text = item->text();
			name = text.toStdString();
		}
	}
	if (index.isValid())
	{
		string SolverName = LibPathsSetDlg::GetSolverName();
		if (SolverName == "openmodelica")
		{
			sCSVFile = LibPathsSetDlg::GetSolverPath().c_str();
			if (sCSVFile.back() != '/')
			{
				sCSVFile += "/";
			}
			sCSVFile += m_pCompRoot->GetCompName().c_str();
			sCSVFile += "_res.csv";
		}
		else if (SolverName == "MWorks")
		{
			sCSVFile = LibPathsSetDlg::GetWorkDir().c_str();
			if (sCSVFile.back() != '/')
			{
				sCSVFile += "/";
			}
			sCSVFile += m_pCompRoot->GetCompName().c_str();
			sCSVFile += "/";
			sCSVFile += "Result.csv";
		}
		QString m_sCSVFilePath = sCSVFile;
		QStringList AllVariablesNames = FileParse::GetAllVariablesNames(m_sCSVFilePath);
		ColumnCount.clear();
		for (int i = 0; i < AllVariablesNames.size(); i++)
		{
			QString FullName = AllVariablesNames.at(i);
			string::size_type idx;
			idx = FullName.toStdString().find(name);
			if (idx != string::npos)
			{
				ColumnCount.push_back(i);
			}
		}
		if (!ColumnCount.empty())
		{
			CurveDisplayDlg* CurDisDlg = new CurveDisplayDlg(name, ColumnCount, this);
			CurDisDlg->show();
		}
		else
		{
			QMessageBox msgBox;
			QString message = text + "变量未在CSV文件中，不能生成曲线！";
			msgBox.setText(message);
			msgBox.exec();
		}
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("未找到变量名称");
		msgBox.exec();
	}
}

void AnimationWidget::ClickButtonShowAllPara()
{
	CreateAllParaModel();
	paratreeview->setModel(Allparatreemodel);
	//paratreeview->expandAll();
	paratreeview->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置其为不可编辑
}

void AnimationWidget::CreateAllParaModel()
{
	m_paraList.clear();
	Allparatreemodel = new ListTreeModel(nullptr);
	m_pCompRoot = ((DocumentModel*)m_doc)->GetComponent();
	list<Component*> comChilds = m_pCompRoot->GetChilds();
	list<Component*>::iterator itr = comChilds.begin();
	for (; itr != comChilds.end(); ++itr)
	{
		Component* pComChild = *itr;
		sCompName = pComChild->GetCompName();
		string sComp;
		sComp = sCompName + "\t" + "M3dCom\t" + "NULL\t" + "NULL";
		m_paraList.append(QString::fromStdString(sComp));
		CyberInfo* childCompCyIn = pComChild->GetCyberInfo();
		readCyberInfo_para(childCompCyIn);
		delete childCompCyIn;
	}
	Allparatreemodel->setupModelData(m_paraList, Allparatreemodel->getrootItem());
}

bool AnimationWidget::readCyberInfo_para(CyberInfo* pCyberInfo)
{
	if (!pCyberInfo) {
		QMessageBox msgBox;
		msgBox.setText("读取失败");
		msgBox.exec();
		return false;
	}
	//构建参数表
	int size = pCyberInfo->GetParameters().size();
	for (int itr = 0; itr < size; ++itr) {
		string s="";	
		//1.全名处理
		string sName = pCyberInfo->GetParameters().at(itr)->sFullName;
		int n_times = count(sName.begin(), sName.end(), '.') + 1;
		if (n_times - 1) sName.replace(0, sName.find_last_of('.') + 1, "");
		string space = "";
		while (n_times)
		{
			space += " ";
			n_times--;
		}
		s += space;
		s += sName;
		
		//加入参数类型
		s += "\t";
		string sType= pCyberInfo->GetParameters().at(itr)->sType;
		if (sType == "")sType = " ";
		s += sType;

		//2.缺省值
		s += "\t";
		//double value = m_newCyberInfo->GetParameters().at(itr)->defaultValue;
		string value = pCyberInfo->GetParameters().at(itr)->defaultValue;
		s += value;
		//ostringstream strStream;
		//strStream << value;
		//if(value!=0)s += strStream.str(); 

		//3.单位
		s += "\t";
		s += pCyberInfo->GetParameters().at(itr)->sUnit;

		//加入维度
		string sDim = pCyberInfo->GetParameters().at(itr)->sDimension;
		if (sDim!="") {
		s += "\t";
		//string sDim = m_newCyberInfo->GetParameters().at(itr)->sDimension;
		//if (sDim == "")sDim = " ";
		s += sDim;
		}

		//4.是否关注与关注名
		s += "\t";
		if (pCyberInfo->GetParameters().at(itr)->bFocused)
		{
			s += "关注";
			s += "\t";
			s += pCyberInfo->GetParameters().at(itr)->sFocusName;
		}
		m_paraList .append(QString::fromStdString(s));
	}
	return true;
}

void AnimationWidget::slotParaTreeMenu(const QPoint& pos)
{
	if (paratreeview->model() == Allparatreemodel)
	{
		QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                QMenu::item{padding:3px 20px 3px 20px;}\
                QMenu::indicator{width:13px;height:13px;}\
                QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                QMenu::separator{height:1px;background:#757575;}";

		QMenu menu;
		//menu.setStyleSheet(qss);    //可以给菜单设置样式

		QModelIndex curIndex = paratreeview->indexAt(pos);       //当前点击的元素的index
		QModelIndex index = curIndex.sibling(curIndex.row(), 0); //该行的第1列元素的index
		if (index.isValid())
		{
			//可获取元素的文本、data,进行其他判断处理
			//QStandardItem* item = AttenOutputTreeModel->itemFromIndex(index);
			//QString text = item->text();
			//QVariant data = item->data(Qt::UserRole + 1);
			//...

			//添加一行菜单
			menu.addAction(QString("将参数添加到关注"), this, SLOT(AddParaAtten()));
			//menu.addSeparator();    //添加一个分隔线
			//menu.addAction(QString("折叠"), this, SLOT(slotTreeMenuCollapse(bool)));
		}
		menu.exec(QCursor::pos());  //显示菜单
	}
	else if (paratreeview->model() == paratreemodel)
	{
		QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                QMenu::item{padding:3px 20px 3px 20px;}\
                QMenu::indicator{width:13px;height:13px;}\
                QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                QMenu::separator{height:1px;background:#757575;}";

		QMenu menu;
		//menu.setStyleSheet(qss);    //可以给菜单设置样式

		QModelIndex curIndex = paratreeview->indexAt(pos);       //当前点击的元素的index
		QModelIndex index = curIndex.sibling(curIndex.row(), 0); //该行的第1列元素的index
		if (index.isValid())
		{
			//可获取元素的文本、data,进行其他判断处理
			//QStandardItem* item = AttenOutputTreeModel->itemFromIndex(index);
			//QString text = item->text();
			//QVariant data = item->data(Qt::UserRole + 1);
			//...

			//添加一行菜单
			menu.addAction(QString("取消关注"), this, SLOT(CancelParaAtten()));
			//menu.addSeparator();    //添加一个分隔线
			//menu.addAction(QString("折叠"), this, SLOT(slotTreeMenuCollapse(bool)));
		}
		menu.exec(QCursor::pos());  //显示菜单
	}
}

void AnimationWidget::AddParaAtten()
{
	//通过索引获得参数全名、参数类型、缺省值
	QModelIndex curIndex = paratreeview->currentIndex();
	QModelIndex index = curIndex.sibling(curIndex.row(), 0); //同一行第一列元素的index
	string name;           //参数全名
	string ParaType;       //参数类型
	QString qs_ParaType;
	string ParaValue;      //参数缺省值
	QString qs_ParaValue;
	QString text;
	if (index.isValid())
	{
		//virtual QMap<int, QVariant> itemData(const QModelIndex & index) const;
		QMap<int, QVariant> item = Allparatreemodel->itemData(index);
		QString LastName = item[0].toString();
		QModelIndex index2 = curIndex.sibling(curIndex.row(), 1); //同一行第二列元素的index
		QMap<int, QVariant> item2 = Allparatreemodel->itemData(index2);
		qs_ParaType = item2[0].toString();
		QModelIndex index3 = curIndex.sibling(curIndex.row(), 2); //同一行第三列元素的index
		QMap<int, QVariant> item3 = Allparatreemodel->itemData(index3);
		qs_ParaValue = item3[0].toString();
		//Q_INVOKABLE virtual QModelIndex parent(const QModelIndex & child) const = 0;
		//逐步寻找父节点，获得变量全名
		vector<QString> FullName;
		FullName.push_back(LastName);
		while (Allparatreemodel->itemData(Allparatreemodel->parent(index)).size() != 0)
		{
			QModelIndex parent = Allparatreemodel->parent(index);
			QMap<int, QVariant> parent_item = Allparatreemodel->itemData(parent);
			QString ParentName = parent_item[0].toString();
			FullName.push_back(ParentName);
			index = parent;
		}

		QString item_FullName;
		for (int i = FullName.size() - 1; i >= 1; i--)
		{
			item_FullName += FullName[i] + ".";
		}
		item_FullName += FullName[0];
		text = item_FullName;
		name = item_FullName.toStdString();
		ParaType = qs_ParaType.toStdString();
		ParaValue = qs_ParaValue.toStdString();
	}

	//将参数加入DocumentModel的ParameterValues中，已经存在的不加，类型是M3dCom的不加
	//vec获取已经更改的关注参数
	auto vec = ((DocumentModel*)m_doc)->GetAllParameterValues();
	set<string> sPara_sMapParamName;//DocumentModel中参数的sMapParamName
	for (vector<ParameterValue>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		sPara_sMapParamName.insert(it->sMapParamName);
	}

	if (sPara_sMapParamName.find(name) != sPara_sMapParamName.end())
	{
		return;
	}

	if (ParaType == "M3dCom")
	{
		return;
	}

	if (sPara_sMapParamName.find(name) == sPara_sMapParamName.end())
	{
		ParameterValue NewParaValue;
		NewParaValue.sMapParamName = name;
		NewParaValue.sParamValue = ParaValue;
		vec.push_back(NewParaValue);
		((DocumentModel*)m_doc)->SetParameterValues(vec);

		//同时也要将对应零件的cyberinfo置为关注，否则装配体显示参数时可能会出现问题
		QString QFullName = QString::fromStdString(name);
		QString QCompName = QFullName.section('.', 0, 0);
		CyberInfo* childCompCyIn;
		list<Component*> comChilds = m_pCompRoot->GetChilds();
		list<Component*>::iterator itr = comChilds.begin();
		Component* pComChild;
		for (; itr != comChilds.end(); ++itr)
		{
			pComChild = *itr;
			string sCompName = pComChild->GetCompName();
			if (sCompName == QCompName.toStdString())
			{
				childCompCyIn = pComChild->GetCyberInfo();
				break;
			}
		}
		if (childCompCyIn == nullptr)
		{
			QMessageBox msgBox;
			msgBox.setText("未找到对应的CyberInfo!");
			msgBox.exec();
			return;
		}
		myParameter* mypara = childCompCyIn->GetParameter(QFullName.section('.', 1).toStdString().c_str());
		if (mypara == nullptr)
		{
			QMessageBox msgBox;
			msgBox.setText("未找到对应的参数名!");
			msgBox.exec();
			return;
		}
		mypara->bFocused = true;
		
		vector<myParameter*> BeforeAllParas = childCompCyIn->GetParameters();
		vector<myParameter*>::iterator itr2 = BeforeAllParas.begin();
		for (int i = 0; itr2 != BeforeAllParas.end(); ++itr2, ++i)
		{
			myParameter* now_para = *itr2;
			if (now_para->sFullName == QFullName.section('.', 1).toStdString())
			{
				*itr2 = mypara;
				//BeforeAllParas[i] = mypara;
				break;
			}
		}
		childCompCyIn->SetParameters(BeforeAllParas);
		((DocumentComponent*)pComChild->GetDocument())->SetCyberInfo(childCompCyIn);
	}
}

void AnimationWidget::CancelParaAtten()
{
	QModelIndex curIndex = paratreeview->currentIndex();
	QModelIndex index = curIndex.sibling(curIndex.row(), 0); //同一行第一列元素的index
	string name;
	QString Q_name;
	if (index.isValid())
	{
		QStandardItem* item = paratreemodel->itemFromIndex(index);
		Q_name = item->text();
		name = Q_name.toStdString();
	}

	//删除DocumentModel->ParameterValues中的参数
	auto vec = ((DocumentModel*)m_doc)->GetAllParameterValues();
	set<string> sPara_sMapParamName;//DocumentModel中参数的sMapParamName
	for (vector<ParameterValue>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
	{
		if (iter->sMapParamName == name)
		{
			vec.erase(iter);
			break;
		}
	}
	((DocumentModel*)m_doc)->SetParameterValues(vec);

	//将零件的CyberInfo中的参数置为不关注
	QString QCompName = Q_name.section('.', 0, 0);
	CyberInfo* childCompCyIn;
	list<Component*> comChilds = m_pCompRoot->GetChilds();
	list<Component*>::iterator itr = comChilds.begin();
	Component* pComChild;
	for (; itr != comChilds.end(); ++itr)
	{
		pComChild = *itr;
		string sCompName = pComChild->GetCompName();
		if (sCompName == QCompName.toStdString())
		{
			childCompCyIn = pComChild->GetCyberInfo();
			break;
		}
	}
	if (childCompCyIn == nullptr)
	{
		QMessageBox msgBox;
		msgBox.setText("未找到对应的CyberInfo!");
		msgBox.exec();
		return;
	}
	myParameter* mypara = childCompCyIn->GetParameter(Q_name.section('.', 1).toStdString().c_str());
	if (mypara == nullptr)
	{
		QMessageBox msgBox;
		msgBox.setText("未找到对应的参数名!");
		msgBox.exec();
		return;
	}
	mypara->bFocused = false;

	vector<myParameter*> BeforeAllParas = childCompCyIn->GetParameters();
	vector<myParameter*>::iterator itr2 = BeforeAllParas.begin();
	for (int i = 0; itr2 != BeforeAllParas.end(); ++itr2, ++i)
	{
		myParameter* now_para = *itr2;
		if (now_para->sFullName == Q_name.section('.', 1).toStdString())
		{
			*itr2 = mypara;
			//BeforeAllParas[i] = mypara;
			break;
		}
	}
	childCompCyIn->SetParameters(BeforeAllParas);
	((DocumentComponent*)pComChild->GetDocument())->SetCyberInfo(childCompCyIn);
}


void AnimationWidget::ClickButtonShowAttenPara()
{
	m_data.clear();
	Component* cur;
	if (!m_doc || !(cur = m_doc->GetComponent())) {
		QMessageBox msgBox;
		msgBox.setText("文档读取失败");
		msgBox.exec();
		return;
	}

	//vec获取已经更改的关注参数
	auto vec = ((DocumentModel*)m_doc)->GetAllParameterValues();
	set<string> sPara_sMapParamName;//DocumentModel中参数的sMapParamName
	for (vector<ParameterValue>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		sPara_sMapParamName.insert(it->sMapParamName);
	}

	//遍历当前装配体下的一级子节点，获取关注参数的缺省值
	vector<ParameterValue> vPVs;
	//遍历当前装配体下的一级子节点，获取关注参数存入m_data
	for (Component* child : cur->GetChilds())
	{
		string scompname = child->GetCompName();
		if (child->IsAssembly())
		{
			//装配体，增加ParameterValue的参数
			DocumentModel* pDocMdl = (DocumentModel*)child->GetDocument();
			auto vParVals = pDocMdl->GetAllParameterValues();
			for (ParameterValue pv : vParVals)
			{
				pv.sMapParamName = scompname + "." + pv.sParamName;
				m_data.push_back(make_pair(pv, false));
				vPVs.push_back(pv);
			}
		}
		else
		{
			vector<myParameter*> paras;
			DocumentComponent* m_doc = (DocumentComponent*)child->GetDocument();
			CyberInfo* m_c = m_doc->GetCyberInfo();
			if (!m_c) {
				QMessageBox msgBox;
				msgBox.setText("CyberInfo不存在");
				msgBox.exec();
				return;
			}
			m_c->SetDocument(m_doc);
			paras = m_c->GetParameters();
			for (myParameter* para : paras) {
				if (para->bFocused)
				{
					ParameterValue temp;
					temp.sMapParamName = scompname + "." + para->sFullName;
					m_data.push_back(make_pair(temp, 0));
					temp.sParamName = "";
					temp.sParamValue = para->defaultValue;
					vPVs.push_back(temp);
				}
			}
		}
	}

	//将已经添加关注，但是不在DocumentModel里的关注参数加入vec中
	for (vector<ParameterValue>::iterator it = vPVs.begin(); it != vPVs.end(); ++it)
	{
		if (sPara_sMapParamName.find(it->sMapParamName) == sPara_sMapParamName.end())
		{
			vec.push_back(*it);
		}
	}
	
	indexmap.clear();
	if (!vec.empty()) {
		unordered_map<string, int> map;
		int size = m_data.size();
		for (int i = 0; i < size; i++)
			map.emplace(m_data[i].first.sMapParamName, i);
		indexmap.clear();
		int idx = 0;
		for (auto para : vec)
		{
			int pos = map[para.sMapParamName];
			indexmap.emplace(idx++, to_string(pos));
			m_data[pos].first.sParamName = para.sParamName;
			m_data[pos].first.sParamValue = para.sParamValue;
			m_data[pos].second = 1;
		}
	}

	//获得组件参数、新建参数、参数值
	vector<QString> vec_MapParamName;
	vector<QString> vec_ParamName;
	vector<QString> vec_ParamValue;
	for (auto itr = indexmap.begin(); itr != indexmap.end(); itr++)
	{
		QString MapParamName = QString::fromStdString(m_data[stoi((*itr).second)].first.sMapParamName);
		vec_MapParamName.push_back(MapParamName);
		QString ParamName = QString::fromStdString(m_data[stoi((*itr).second)].first.sParamName);
		vec_ParamName.push_back(ParamName);
		QString ParamValue = QString::fromStdString(m_data[stoi((*itr).second)].first.sParamValue);
		vec_ParamValue.push_back(ParamValue);
	}

	//构造model
	paratreemodel = new QStandardItemModel(this);
	paratreemodel->setHorizontalHeaderLabels(QStringList() << QString("组件参数") << QString("新建参数") << QString("参数值"));     //设置列头

	//最后构造modeltree																													 
	for (int i = 0; i < vec_MapParamName.size(); i++)
	{
		//一级节点，加入mModel
		QList<QStandardItem*> items1;
		QStandardItem* item1 = new QStandardItem(vec_MapParamName[i]);
		QStandardItem* item2 = new QStandardItem(vec_ParamName[i]);
		QStandardItem* item3 = new QStandardItem(vec_ParamValue[i]);
		items1.append(item1);
		items1.append(item2);
		items1.append(item3);
		paratreemodel->appendRow(items1);
	}
	paratreeview->setModel(paratreemodel);
	paratreeview->setEditTriggers(QTreeView::DoubleClicked);
	class DisableEditor : public QStyledItemDelegate
	{
	public:
		DisableEditor(QObject* parent)
			: QStyledItemDelegate(parent) {}

		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
			const QModelIndex& index) const override
		{

			return 0;
		}
	};
	//将除参数值外的其他列设置为不可编辑
	paratreeview->setItemDelegateForColumn(0, new DisableEditor(paratreeview));
	paratreeview->setItemDelegateForColumn(1, new DisableEditor(paratreeview));
	//paratreeview->expandAll();
}


void AnimationWidget::ClickButtonSavePara()
{
	if (paratreeview->model() == paratreemodel)
	{
		if (!m_doc->IsModel())
		{
			QMessageBox msgBox;
			msgBox.setText("文档读取失败");
			msgBox.exec();
			return;
		}

		vector<ParameterValue> vec;

		//保存前先刷新显示
		paratreeview->setFocus();
		paratreeview->QAbstractItemView::viewport()->update();

		paratreemodel = static_cast<QStandardItemModel*>(paratreeview->model());
		for (int i = 0; i < paratreemodel->rowCount(); i++)
		{
			QStandardItem* item1 = paratreemodel->item(i, 0);
			string MapParamName = item1->text().toStdString();
			QStandardItem* item2 = paratreemodel->item(i, 1);
			string ParamName = item2->text().toStdString();
			QStandardItem* item3 = paratreemodel->item(i, 2);
			string ParamValue = item3->text().toStdString();
			ParameterValue pv;
			pv.sMapParamName = MapParamName;
			pv.sParamName = ParamName;
			pv.sParamValue = ParamValue;
			vec.push_back(pv);
		}
		((DocumentModel*)m_doc)->SetParameterValues(vec);
		QMessageBox msgBox;
		msgBox.setText("保存成功！");
		msgBox.exec();
	}
}

//实现事件过滤器
bool AnimationWidget::eventFilter(QObject* target, QEvent* event)
{
	if (target == paratreeview)
	{
		//获取按键信号
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* k = static_cast<QKeyEvent*>(event);
			if (k->key() == Qt::Key_Return)          //选定回车键(可自行定义其他按键)
			{
				ClickButtonSavePara();               //链接槽信号
				return true;
			}
		}
	}
	return QWidget::eventFilter(target, event);
}


void AnimationWidget::ClickButtonResimulate()
{
	if (m_doc && m_doc->IsModel())
	{
		((DocumentModel*)m_doc)->ReSimulateModel();
	}
}

void AnimationWidget::ClickButtonSendSimData()
{
	//如果没有创建过就新建一个，创建过就不创建新的了
	if (haveCreatedInfoSendDlg == false)
	{
		QString m_sCSVFilePath = GetCSVFilePath();
		InfoSendDlg = new SimulInfoSendDlg(m_sCSVFilePath);
		InfoSendDlg->m_doc = this->m_doc;
		haveCreatedInfoSendDlg = true;
	}
	InfoSendDlg->show();
	//InfoSendDlg->raise();
	QString m_sCSVFilePath = GetCSVFilePath();
	InfoSendDlg->SetCSVPath(m_sCSVFilePath);
}

void AnimationWidget::MsgCSVReadStart()
{
	MessageDisplay += "仿真结果文件（*.csv）预读中，请稍等...";
	AnimationMessage->setText(MessageDisplay);
	//利用光标使滚动条始终置于底部
	QTextCursor textCursor = AnimationMessage->textCursor();
	textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	AnimationMessage->setTextCursor(textCursor);
}

void AnimationWidget::MsgCSVReadFinish()
{
	MessageDisplay += "\n-------------仿真结果文件（*.csv）读取完成-------------\n";
	AnimationMessage->setText(MessageDisplay);
	//利用光标使滚动条始终置于底部
	QTextCursor textCursor = AnimationMessage->textCursor();
	textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	AnimationMessage->setTextCursor(textCursor);
}


//SimulInfoSendDlg::SimulInfoSendDlg(AnimationWidget* parent) :m_parent(parent), QDialog(parent)
SimulInfoSendDlg::SimulInfoSendDlg(QString CSVFilePath, QDialog* parent) : Parent(parent)
{
	// connect(this, SIGNAL(rep()), this, SLOT(SendParas()), Qt::BlockingQueuedConnection);
	SetCSVPath(CSVFilePath);
	SimulInfoSendDlgInit();
	setMinimumWidth(500);
	setMinimumHeight(600);

	// 设置窗体最大化和最小化
	Qt::WindowFlags windowFlag = Qt::Dialog;
	windowFlag |= Qt::WindowMinimizeButtonHint;
	windowFlag |= Qt::WindowMaximizeButtonHint;
	windowFlag |= Qt::WindowCloseButtonHint;
	setWindowFlags(windowFlag);
}

void SimulInfoSendDlg::SetCSVPath(QString csvpath)
{
	m_sCSVFilePath = csvpath;
}

void SimulInfoSendDlg::SimulInfoSendDlgInit()
{
	setWindowTitle(tr("仿真信息发送"));
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
		"QComboBox{"
		"font: 18px;"
		"}"
	);

	//1.全局唯一消息ID
	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//设置整体布局为垂直布局
	LocalMessageIdShow = new QLabel("全局唯一消息ID：");
	LocalMessageIdShow->setMinimumHeight(25);
	LocalMessageIdShow->setFixedWidth(150);

	LocalMessageIdEdit = new QLineEdit;
	LocalMessageIdEdit->setReadOnly(false); //可编辑
	//LocalMachineIPEdit->setText();
	LocalMessageIdEdit->setMinimumHeight(25);
	LocalMessageIdEdit->setFixedWidth(150);

	//2.目的机ID
	DestinationMachineIdShow = new QLabel("目的机器ID(;分隔)：");
	DestinationMachineIdShow->setMinimumHeight(25);
	DestinationMachineIdShow->setFixedWidth(150);

	DestinationMachineIdEdit = new QLineEdit;
	DestinationMachineIdEdit->setReadOnly(false); //可编辑
	//LocalMachineIPEdit->setText();
	DestinationMachineIdEdit->setMinimumHeight(25);
	DestinationMachineIdEdit->setFixedWidth(150);

	//选择DDS发现方式
	ButtonRTPS = new QRadioButton(this);
	ButtonRTPS->setText("端到端发现");
	ButtonDCPSInfoRepo = new QRadioButton(this);
	ButtonDCPSInfoRepo->setText("集中式发现");
	groupButtonDiscoveryMethod = new QButtonGroup(this);
	groupButtonDiscoveryMethod->addButton(ButtonRTPS, 0);//id为0
	groupButtonDiscoveryMethod->addButton(ButtonDCPSInfoRepo, 1);//id为1
	groupButtonDiscoveryMethod->button(MethodID)->setChecked(true); //默认选中端到端发现

	//选择发送字段
	SelectSendVars = new QLabel("选择发送字段");
	font = SelectSendVars->font();//加粗字体
	font.setBold(true);
	SelectSendVars->setFont(font);
	SelectSendVars->setMinimumHeight(25);

	PushButtonSelectAll = new QPushButton(this);
	PushButtonSelectAll->setText("全选");
	PushButtonSelectAll->setFixedSize(100, 25);
	connect(PushButtonSelectAll, SIGNAL(clicked()), this, SLOT(ClickButtonSelectAll()));

	PushButtonSelectNone = new QPushButton(this);
	PushButtonSelectNone->setText("取消全选");
	PushButtonSelectNone->setFixedSize(100, 25);
	connect(PushButtonSelectNone, SIGNAL(clicked()), this, SLOT(ClickButtonSelectNone()));

	PushButtonShowSelected = new QPushButton(this);
	PushButtonShowSelected->setText("展示已选");
	PushButtonShowSelected->setFixedSize(100, 25);
	connect(PushButtonShowSelected, SIGNAL(clicked()), this, SLOT(ClickButtonShowSelected()));


	//构造view
	sendvarstreeview = new QTreeView(this);
	//1.QTreeView常用设置项
	sendvarstreeview->setSelectionBehavior(QTreeView::SelectRows);		   //一次选中整行
	sendvarstreeview->setSelectionMode(QTreeView::SingleSelection);        //单选，配合上面的整行就是一次选单行
	sendvarstreeview->header()->setStretchLastSection(true);               //最后一列自适应宽度
	sendvarstreeview->header()->setDefaultAlignment(Qt::AlignCenter);      //列头文字默认居中对齐
	//构造sendvarstreemodel
	sendvarstreemodel = new QStandardItemModel(this);
	//设置表头
	sendvarstreemodel->setHorizontalHeaderLabels(QStringList() << "发送字段名称" << "是否发送");
	CreateSendVarsModel();
	//sendvarstreeview->setModel(sendvarstreemodel);
	//sendvarstreeview->setItemDelegateForColumn(1, new CheckBoxDelegateIsSend(sendvarstreeview));

	//3.初始化匹配按钮
	PushButtonInitMatch = new QPushButton(this);
	PushButtonInitMatch->setText("初始化匹配");
	PushButtonInitMatch->setFixedSize(400, 25);
	connect(PushButtonInitMatch, SIGNAL(clicked()), this, SLOT(ClickButtonInitMatch()));

	//4.结束匹配按钮
	PushButtonCloseMatch = new QPushButton(this);
	PushButtonCloseMatch->setText("断开匹配");
	PushButtonCloseMatch->setFixedSize(400, 25);
	connect(PushButtonCloseMatch, SIGNAL(clicked()), this, SLOT(ClickButtonCloseMatch()));

	//5.发送消息的提示栏
	ShowSendMessage = new QLabel("已发送消息");
	font = ShowSendMessage->font();//加粗字体
	font.setBold(true);
	ShowSendMessage->setFont(font);
	ShowSendMessage->setMinimumHeight(25);

	SendNowMessages = new QTextEdit(this);
	SendNowMessages->setText("已发送信息如下...\n");




	//布局
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(LocalMessageIdShow);
	HorinztalLayout1->addWidget(LocalMessageIdEdit);
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(this);
	HorinztalLayout2->addWidget(DestinationMachineIdShow);
	HorinztalLayout2->addWidget(DestinationMachineIdEdit);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->setMargin(0);//一定要有
	HorinztalLayout3->addWidget(ButtonRTPS);
	HorinztalLayout3->setAlignment(ButtonRTPS, Qt::AlignCenter);//控件在布局中居中显示
	HorinztalLayout3->addWidget(ButtonDCPSInfoRepo);
	HorinztalLayout3->setAlignment(ButtonDCPSInfoRepo, Qt::AlignCenter);//控件在布局中居中显示
	QHBoxLayout* HorinztalLayoutSendVars = new QHBoxLayout(this);
	HorinztalLayoutSendVars->addWidget(SelectSendVars);
	HorinztalLayoutSendVars->addWidget(PushButtonSelectAll);
	HorinztalLayoutSendVars->addWidget(PushButtonSelectNone);
	HorinztalLayoutSendVars->addWidget(PushButtonShowSelected);
	QHBoxLayout* HorinztalLayoutTreeView = new QHBoxLayout(this);
	HorinztalLayoutTreeView->addWidget(sendvarstreeview);
	QHBoxLayout* HorinztalLayout4 = new QHBoxLayout(this);
	HorinztalLayout4->addWidget(PushButtonInitMatch);
	QHBoxLayout* HorinztalLayout5 = new QHBoxLayout(this);
	HorinztalLayout5->addWidget(PushButtonCloseMatch);
	QHBoxLayout* HorinztalLayout6 = new QHBoxLayout(this);
	HorinztalLayout6->addWidget(ShowSendMessage);
	QHBoxLayout* HorinztalLayout7 = new QHBoxLayout(this);
	HorinztalLayout7->addWidget(SendNowMessages);


	VerticalLayout->setSpacing(15);
	VerticalLayout->addLayout(HorinztalLayout1);
	VerticalLayout->addLayout(HorinztalLayout2);
	VerticalLayout->addLayout(HorinztalLayout3);
	VerticalLayout->addLayout(HorinztalLayoutSendVars);
	VerticalLayout->addLayout(HorinztalLayoutTreeView);
	VerticalLayout->addLayout(HorinztalLayout4);
	VerticalLayout->addLayout(HorinztalLayout5);
	VerticalLayout->addLayout(HorinztalLayout6);
	VerticalLayout->addLayout(HorinztalLayout7);

	this->setLayout(VerticalLayout);
}


SimulInfoSendDlg::~SimulInfoSendDlg()
{
	ClearXDataMembers();
	delete sendvarstreemodel;
	delete selectedtreemodel;
	delete sendvarstreeview;
}

void SimulInfoSendDlg::CreateSendVarsModel()
{
	//最后构造modeltree	
	QStringList VariablesNames = FileParse::GetAllVariablesNames(m_sCSVFilePath);
	for (int i = 0; i < VariablesNames.size(); i++)
	{
		QString variablesname = VariablesNames.at(i);
		//一级节点，加入mModel
		QList<QStandardItem*> items1;
		QStandardItem* item1 = new QStandardItem(variablesname);
		QStandardItem* item2 = new QStandardItem();
		items1.append(item1);
		items1.append(item2);
		sendvarstreemodel->appendRow(items1);
	}
	sendvarstreeview->setModel(sendvarstreemodel);
	sendvarstreeview->setEditTriggers(QTreeView::DoubleClicked);
	class DisableEditor : public QStyledItemDelegate
	{
	public:
		DisableEditor(QObject* parent)
			: QStyledItemDelegate(parent) {}

		virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
			const QModelIndex& index) const override
		{

			return 0;
		}
	};
	//输出项名称列设置为不可编辑
	sendvarstreeview->setItemDelegateForColumn(0, new DisableEditor(sendvarstreeview));
	sendvarstreeview->setItemDelegateForColumn(1, new CheckBoxDelegateIsSend(sendvarstreeview));
	sendvarstreeview->expandAll();
}


void SimulInfoSendDlg::ClickButtonSelectAll()
{
	for (int i = 0; i < sendvarstreemodel->rowCount(); i++)
	{
		QStandardItem* itemIsSend = new QStandardItem("是");
		sendvarstreemodel->setItem(i, 1, itemIsSend);
	}
	sendvarstreeview->setModel(sendvarstreemodel);
}


void SimulInfoSendDlg::ClickButtonSelectNone()
{
	for (int i = 0; i < sendvarstreemodel->rowCount(); i++)
	{
		QStandardItem* itemIsSend = new QStandardItem("否");
		sendvarstreemodel->setItem(i, 1, itemIsSend);
	}
	sendvarstreeview->setModel(sendvarstreemodel);
}


void SimulInfoSendDlg::ClickButtonShowSelected()
{
	if (PushButtonShowSelected->text() == "展示已选")
	{
		if (selectedtreemodel != nullptr) {
			delete selectedtreemodel;
		}
		selectedtreemodel = new QStandardItemModel(this);
		//设置表头
		selectedtreemodel->setHorizontalHeaderLabels(QStringList() << "已选发送字段");
		for (int i = 0; i < sendvarstreemodel->rowCount(); i++) {
			QStandardItem* item1 = sendvarstreemodel->item(i, 1);
			QString vartype = item1->text();
			if (vartype == "是")
			{
				QStandardItem* item0 = sendvarstreemodel->item(i, 0);
				QString varname = item0->text();
				//一级节点，加入mModel
				QList<QStandardItem*> items1;
				QStandardItem* item1 = new QStandardItem(varname);
				items1.append(item1);
				selectedtreemodel->appendRow(items1);
			}
		}
		sendvarstreeview->setModel(selectedtreemodel);
		PushButtonShowSelected->setText("展示所有");
	}
	else {
		sendvarstreeview->setModel(sendvarstreemodel);
		PushButtonShowSelected->setText("展示已选");
	}
}


//两台机器互相匹配的函数
void SimulInfoSendDlg::ClickButtonInitMatch()
{
	if (initMatch == true)
	{
		string errormsg = "已经初始化！请勿重复初始化！";
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(errormsg));
		msgBox.exec();
		return;
	}
	
	MethodID = groupButtonDiscoveryMethod->checkedId();
	int code = GenXDataMembers(MethodID);
	if (code == 0)
	{
		sendVars.clear();
		for (int i = 0; i < sendvarstreemodel->rowCount(); i++)
		{
			QStandardItem* item1 = sendvarstreemodel->item(i, 1);
			string vartype = item1->text().toStdString();
			if (vartype == "是")
			{
				QStandardItem* item0 = sendvarstreemodel->item(i, 0);
				string varname = item0->text().toStdString();
				sendVars.push_back(varname);
			}
		}

		if (sendVars.size() == 0) {
			QString errormsg = "未选择发送字段！请至少选择一个发送字段后，重新初始化。";
			QMessageBox msgBox;
			msgBox.setText(errormsg);
			msgBox.exec();
			ClearXDataMembers();
			return;
		}

		XData data;
		data.time = -1;
		data.vec_Data = sendVars;

		//如果不加这一句，参数名称发不过去，不清楚原因，先加了这句话再说
		Sleep(1000);
		
		if (SendXData(LocalMessageIdEdit->text().toStdString(),DestinationMachineIdEdit->text().toStdString(), data) == 0)
		{
			string sendedmsg = GetSendedMsg();
			SendedMessages += "匹配成功!变量名称已发送!\n";
			SendedMessages += QString::fromStdString(sendedmsg);
			SendNowMessages->setText(SendedMessages);
			//利用光标使滚动条始终置于底部
			QTextCursor textCursor = SendNowMessages->textCursor();
			textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
			SendNowMessages->setTextCursor(textCursor);
			issending = true;//将发送状态置为true
			initMatch = true;
			
			//QMessageBox msgBox;
			//msgBox.setText("匹配成功!变量名称已发送!");
			//msgBox.exec();
			return;
		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText("匹配失败!请检查CSV文件是否成功生成!");
			msgBox.exec();
			ClearXDataMembers();
			return;
		}
	}
	else
	{
		string errormsg = "初始化失败!错误代码：" + std::to_string(code);
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(errormsg));
		msgBox.exec();
		ClearXDataMembers();
		return;
	}
	return;
}


//发送仿真信息的函数
//void SimulInfoSendDlg::SendSimulateData(double time, vector<string> vdata)
void SimulInfoSendDlg::SendSimulateData()
{
	XData data;
	if (m_doc && m_doc->IsModel())
	{
		data.time = ((DocumentModel*)m_doc)->GetAnimatingTime();
		data.vec_Data = ((DocumentModel*)m_doc)->GetAnimatingDatas();
	}
	if (SendXData(LocalMessageIdEdit->text().toStdString(), DestinationMachineIdEdit->text().toStdString(), data) == 0)
	{
		//这一段先注释掉，因为显示已发送的信息比较耗时，会有卡顿的现象
		string sendedmsg = GetSendedMsg();
		SendedMessages += QString::fromStdString(sendedmsg);
		SendNowMessages->setText(SendedMessages);
		//利用光标使滚动条始终置于底部
		QTextCursor textCursor = SendNowMessages->textCursor();
		textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		SendNowMessages->setTextCursor(textCursor);

		//QMessageBox msgBox;
		//msgBox.setText("数据已发送!");
		//msgBox.exec();

		return;
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("数据发送失败！");
		msgBox.exec();
		return;
	}

}

//两台机器断开匹配的函数
void SimulInfoSendDlg::ClickButtonCloseMatch()
{
	if (ClearXDataMembers() == 0)
	{
		QMessageBox msgBox;
		msgBox.setText("连接已断开，实体已清理!");
		msgBox.exec();
		issending = false;
		initMatch = false;
		return;
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("发生未知错误!");
		msgBox.exec();
		return;
	}
	return;
}

//创建发送字段下拉列表
QWidget* CheckBoxDelegateIsSend::createEditor(QWidget* parent,
	const QStyleOptionViewItem& /* option */,
	const QModelIndex& index) const
{
	QComboBox* editor = new QComboBox(parent);
	editor->addItem("是");
	editor->addItem("否");

	return editor;
}

void CheckBoxDelegateIsSend::setEditorData(QWidget* editor,
	const QModelIndex& index) const
{
	QString text = index.model()->data(index, Qt::EditRole).toString();
	int value = index.model()->data(index, Qt::EditRole).toInt();
	QComboBox* combobox = static_cast<QComboBox*>(editor);
	combobox->setCurrentIndex(combobox->findText(text));
}

void CheckBoxDelegateIsSend::setModelData(QWidget* editor,
	QAbstractItemModel* model,
	const QModelIndex& index) const
{
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	QString text = comboBox->currentText();

	model->setData(index, text, Qt::EditRole);
}
void CheckBoxDelegateIsSend::updateEditorGeometry(QWidget* editor,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}


//曲线展示对话框
CurveDisplayDlg::CurveDisplayDlg(string VariableName, vector<int> ColumnCount, AnimationWidget* parent) :m_parent(parent), QDialog(parent)
{
	m_sCSVFilePath = m_parent->GetCSVFilePath();
	CurveDisplayDlgInit(VariableName, ColumnCount);
	setMinimumWidth(800);
	setMinimumHeight(600);

	// 设置窗体最大化和最小化
	Qt::WindowFlags windowFlag = Qt::Dialog;
	windowFlag |= Qt::WindowMinimizeButtonHint;
	windowFlag |= Qt::WindowMaximizeButtonHint;
	windowFlag |= Qt::WindowCloseButtonHint;
	setWindowFlags(windowFlag);
}

void CurveDisplayDlg::CurveDisplayDlgInit(string VariableName, vector<int> ColumnCount)
{
	setWindowTitle(tr("变量曲线显示"));
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
		"QComboBox{"
		"font: 18px;"
		"}"
	);

	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//设置整体布局为垂直布局
	VariaNameShow = new QLabel("变量名称：");
	VariaNameShow->setMinimumHeight(25);

	VariableNameEdit = new QLineEdit;
	VariableNameEdit->setReadOnly(true); //不可编辑
	VariableNameEdit->setText(QString::fromStdString(VariableName));
	VariableNameEdit->setMinimumHeight(25);
	VariableNameEdit->setStyleSheet("background:transparent;border-width:0;border-style:outset");//设置其无边框且透明

	ParaSelectTips = new QLabel("选择要生成曲线的参数：");
	ParaSelectTips->setMinimumHeight(25);

	//在下拉框加入可供选择的参数
	QStringList AllVariablesNames = FileParse::GetAllVariablesNames(m_sCSVFilePath);
	cbo_Para = new QComboBox();
	for (int i = 0; i < ColumnCount.size(); i++)
	{
		QString name = AllVariablesNames[ColumnCount[i]];
		//cbo_Para->addItem(QWidget::tr("male"));
		cbo_Para->addItem(name);
	}
	cbo_Para->setStyleSheet(BtnStyle);
	cbo_Para->setMinimumHeight(25);
	disconnect(cbo_Para, SIGNAL(activated(QString)), this, SLOT(ParaChanged()));
	connect(cbo_Para, SIGNAL(activated(QString)), this, SLOT(ParaChanged()));

	//曲线显示
	chartview = new QChartView();
	//根据列名获取所有点的信息
	vector<double> timeSeries;
	QStringList timeStrLst = FileParse::GetDataSeries(m_sCSVFilePath, "time");
	for (int i = 0; i < timeStrLst.size(); i++)
	{
		double d_i = timeStrLst.at(i).toDouble();
		timeSeries.push_back(d_i);
	}
	vector<double> paraSeries;
	QStringList paraStrLst = FileParse::GetDataSeries(m_sCSVFilePath, AllVariablesNames[ColumnCount[0]].toStdString().c_str());
	for (int i = 0; i < paraStrLst.size(); i++)
	{
		double d_i = paraStrLst.at(i).toDouble();
		paraSeries.push_back(d_i);
	}
	//设置表头
	chart = new Chart(this, AllVariablesNames[ColumnCount[0]]);
	//设置坐标系
	double max_value = *max_element(paraSeries.begin(), paraSeries.end());
	double min_value = *min_element(paraSeries.begin(), paraSeries.end());
	double range = (max_value - min_value) / 10;
	if (range == 0) { range = 1; }                          //如果变量值为常数则相应增加range的范围
	chart->setAxis("time", 0, timeSeries[timeSeries.size() - 1], 11, AllVariablesNames[ColumnCount[0]], min_value - range, max_value + range, 11);
	//设置离散点数据
	QList<QPointF> pointlist;
	for (int i = 0; i < timeSeries.size(); i++)
	{
		pointlist.append(QPointF(timeSeries[i], paraSeries[i]));
	}
	//绘制
	chart->buildChart(pointlist);
	chartview->setChart(chart->qchart);


	//布局
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(VariaNameShow);
	HorinztalLayout1->addWidget(VariableNameEdit);
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(this);
	HorinztalLayout2->addWidget(ParaSelectTips);
	HorinztalLayout2->addWidget(cbo_Para);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->addWidget(chartview);
	chartview->setRenderHint(QPainter::Antialiasing);//防止图形走样


	VerticalLayout->setSpacing(15);
	VerticalLayout->addLayout(HorinztalLayout1);
	VerticalLayout->addLayout(HorinztalLayout2);
	VerticalLayout->addLayout(HorinztalLayout3);

	this->setLayout(VerticalLayout);
}


CurveDisplayDlg::~CurveDisplayDlg()
{

}

void CurveDisplayDlg::ParaChanged()
{
	QString ParaName = cbo_Para->currentText();
	//QMessageBox msgBox;
	//QString message = "当前所选变量为：" + ParaName;
	//msgBox.setText(message);
	//msgBox.exec();
	//chart = new Chart(this, ParaName);
	//chart->setAxis("X轴", 0, 100, 11, "Y轴", 0, 20, 11);
	//QList<QPointF> pointlist = { QPointF(0,1), QPointF(10,10), QPointF(20,4), QPointF(30,8), QPointF(40,16), \
	//							QPointF(50,16), QPointF(60,8), QPointF(70,4), QPointF(80,2), QPointF(90,1), };

	//根据列名获取所有点的信息
	vector<double> timeSeries;
	QStringList timeStrLst = FileParse::GetDataSeries(m_sCSVFilePath, "time");
	for (int i = 0; i < timeStrLst.size(); i++)
	{
		double d_i = timeStrLst.at(i).toDouble();
		timeSeries.push_back(d_i);
	}
	vector<double> paraSeries;
	QStringList paraStrLst = FileParse::GetDataSeries(m_sCSVFilePath, ParaName.toStdString().c_str());
	for (int i = 0; i < paraStrLst.size(); i++)
	{
		double d_i = paraStrLst.at(i).toDouble();
		paraSeries.push_back(d_i);
	}
	//设置表头
	chart = new Chart(this, ParaName);
	//设置坐标系
	double max_value = *max_element(paraSeries.begin(), paraSeries.end());
	double min_value = *min_element(paraSeries.begin(), paraSeries.end());
	double range = (max_value - min_value) / 10;
	if (range <= 0.000001) { range = 1; }
	chart->setAxis("time", 0, timeSeries[timeSeries.size() - 1], 11, ParaName, min_value - range, max_value + range, 11);
	//设置离散点数据
	QList<QPointF> pointlist;
	for (int i = 0; i < timeSeries.size(); i++)
	{
		pointlist.append(QPointF(timeSeries[i], paraSeries[i]));
	}
	//绘制
	chart->buildChart(pointlist);
	chartview->setChart(chart->qchart);
}




//Chart类实现
Chart::Chart(QWidget* parent, QString _chartname)
{
	setParent(parent);
	chartname = _chartname;
	series = new QSplineSeries(this);
	qchart = new QChart;
	axisX = new QValueAxis(this);
	axisY = new QValueAxis(this);
}

void Chart::setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc, \
	QString _yname, qreal _ymin, qreal _ymax, int _ytickc) {
	xname = _xname; xmin = _xmin; xmax = _xmax; xtickc = _xtickc;
	yname = _yname; ymin = _ymin; ymax = _ymax; ytickc = _ytickc;

	axisX->setRange(xmin, xmax);    //设置范围
	axisX->setLabelFormat("%f");   //设置刻度的格式
	/************************************
		%u 无符号十进制整数
		%s 字符串
		%c 一个字符
		%d 有符号十进制整数
		%e 浮点数、e-记数法
		%f 浮点数、十进制记数法
		%s 字符串
		%E 浮点数、E-记数法
	****************************************/
	axisX->setGridLineVisible(true);   //网格线可见
	axisX->setTickCount(xtickc);       //设置多少个大格
	axisX->setMinorTickCount(1);   //设置每个大格里面小刻度线的数目
	axisX->setTitleText(xname);  //设置描述
	axisY->setRange(ymin, ymax);
	axisY->setLabelFormat("%f");
	axisY->setGridLineVisible(true);
	axisY->setTickCount(ytickc);
	axisY->setMinorTickCount(1);
	axisY->setTitleText(yname);
	qchart->addAxis(axisX, Qt::AlignBottom); //下：Qt::AlignBottom  上：Qt::AlignTop
	qchart->addAxis(axisY, Qt::AlignLeft);   //左：Qt::AlignLeft    右：Qt::AlignRight
}

void Chart::buildChart(QList<QPointF> pointlist)
{
	//创建数据源
	series->setPen(QPen(Qt::blue, 3, Qt::SolidLine));
	series->clear();
	for (int i = 0; i < pointlist.size(); i++)
		series->append(pointlist.at(i).x(), pointlist.at(i).y());

	qchart->setTitle(chartname);
	qchart->setAnimationOptions(QChart::SeriesAnimations);//设置曲线动画模式
	qchart->legend()->hide(); //隐藏图例
	qchart->addSeries(series);//输入数据
	qchart->setAxisX(axisX, series);
	qchart->setAxisY(axisY, series);
}

QString AnimationWidget::GetCSVFilePath()
{
	if (sCSVFile != "")
	{
		return sCSVFile;
	}
	else
	{
		string SolverName = LibPathsSetDlg::GetSolverName();
		m_pCompRoot = ((DocumentModel*)m_doc)->GetComponent();
		if (SolverName == "openmodelica")
		{
			sCSVFile = LibPathsSetDlg::GetSolverPath().c_str();
			if (sCSVFile.back() != '/')
			{
				sCSVFile += "/";
			}
			sCSVFile += m_pCompRoot->GetCompName().c_str();
			sCSVFile += "_res.csv";
		}
		else if (SolverName == "MWorks")
		{
			sCSVFile = LibPathsSetDlg::GetWorkDir().c_str();
			if (sCSVFile.back() != '/')
			{
				sCSVFile += "/";
			}
			sCSVFile += m_pCompRoot->GetCompName().c_str();
			sCSVFile += "/";
			sCSVFile += "Result.csv";
		}
	}
	return sCSVFile;
}