#include "DigitaltwinsWidget.h"
#include "DDSData.h"
#include "DocumentModel.h"

//#include "DigitaltwinsWidget.h"
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
#include <set>
#include "cyberInfo.h"
#include <QMenu>
#include <QKeyEvent>
#include <QTextCursor>
#include "CSV_Parser.h"


using namespace std;
using namespace CSV_Parser;

static CyberInfo* m_twinCyberInfo;


DigitaltwinsWidget::DigitaltwinsWidget(QWidget* parent) :Parent(parent)
{
	this->m_doc = (DocumentCommon*)qobject_cast<MainWindow*>(parent)->getDocument();

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	setMinimumWidth(300);
	setMaximumWidth(600);
	//setFixedWidth(400);

	dpWidInit();
}

void DigitaltwinsWidget::dpWidInit()
{

	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//设置整体布局为垂直布局
	VerticalLayout->setDirection(QBoxLayout::TopToBottom);

	//1.计算机匹配栏
	MatchComputer = new QLabel("计算机匹配栏");
	font = MatchComputer->font();//加粗字体
	font.setBold(true);
	MatchComputer->setFont(font);
	MatchComputer->setMinimumHeight(25);

	//本机id
	LocalMachineIdShow = new QLabel("本机ID：");
	LocalMachineIdShow->setMinimumHeight(25);
	LocalMachineIdShow->setFixedWidth(150);

	LocalMachineIdEdit = new QLineEdit;
	LocalMachineIdEdit->setReadOnly(false); //可编辑
	LocalMachineIdEdit->setMinimumHeight(25);
	LocalMachineIdEdit->setFixedWidth(150);

	//允许接收消息id（可能有多个）
	ReceivedMessageIdShow = new QLabel("接收消息ID(;分隔)：");
	ReceivedMessageIdShow->setMinimumHeight(25);
	ReceivedMessageIdShow->setFixedWidth(150);

	ReceivedMessageIdEdit = new QLineEdit;
	ReceivedMessageIdEdit->setReadOnly(false); //可编辑
	ReceivedMessageIdEdit->setMinimumHeight(25);
	ReceivedMessageIdEdit->setFixedWidth(150);

	//选择DDS发现方式
	ButtonRTPS = new QRadioButton(this);
	ButtonRTPS->setText("端到端发现");
	ButtonDCPSInfoRepo = new QRadioButton(this);
	ButtonDCPSInfoRepo->setText("集中式发现");
	groupButtonDiscoveryMethod = new QButtonGroup(this);
	groupButtonDiscoveryMethod->addButton(ButtonRTPS, 0);//id为0
	groupButtonDiscoveryMethod->addButton(ButtonDCPSInfoRepo, 1);//id为1
	groupButtonDiscoveryMethod->button(MethodID)->setChecked(true); //默认选中端到端发现

	//初始化匹配按钮
	PushButtonInitMatch = new QPushButton(this);
	PushButtonInitMatch->setText("初始化匹配");
	PushButtonInitMatch->setFixedSize(400, 25);
	connect(PushButtonInitMatch, SIGNAL(clicked()), this, SLOT(ClickButtonInitMatch()));

	//结束匹配按钮
	PushButtonCloseMatch = new QPushButton(this);
	PushButtonCloseMatch->setText("断开匹配");
	PushButtonCloseMatch->setFixedSize(400, 25);
	connect(PushButtonCloseMatch, SIGNAL(clicked()), this, SLOT(ClickButtonCloseMatch()));

	//2.神经网络加载栏
	LoadNeuralNetwork = new QLabel("神经网络加载栏");
	font = LoadNeuralNetwork->font();//加粗字体
	font.setBold(true);
	LoadNeuralNetwork->setFont(font);
	LoadNeuralNetwork->setMinimumHeight(25);

	//不使用神经网络按钮
	PushButtonNoUseNet = new QPushButton(this);
	PushButtonNoUseNet->setText("不使用");
	PushButtonNoUseNet->setFixedSize(75, 25);
	connect(PushButtonNoUseNet, SIGNAL(clicked()), this, SLOT(ClickButtonNoUseNet()));

	//增加行数按钮
	PushButtonAddLine = new QPushButton(this);
	PushButtonAddLine->setText("+");
	PushButtonAddLine->setFixedSize(25, 25);
	connect(PushButtonAddLine, SIGNAL(clicked()), this, SLOT(ClickButtonAddLine()));

	//减去行数按钮
	PushButtonDelLine = new QPushButton(this);
	PushButtonDelLine->setText("-");
	PushButtonDelLine->setFixedSize(25, 25);
	connect(PushButtonDelLine, SIGNAL(clicked()), this, SLOT(ClickButtonDelLine()));

	//检查配置按钮
	PushButtonCheckConfig = new QPushButton(this);
	PushButtonCheckConfig->setText("检查配置文件");
	PushButtonCheckConfig->setFixedSize(125, 25);
	connect(PushButtonCheckConfig, SIGNAL(clicked()), this, SLOT(ClickButtonCheckConfig()));

	m_table = new QTableWidget(this);
	connect(m_table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
		this, SLOT(currentItemChangedSLOT(QTableWidgetItem*, QTableWidgetItem*)));
	fillPathtable();



	//3.变量显示栏
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
	//outputtreeview->setEditTriggers(QTreeView::NoEditTriggers);		   //单元格不能编辑
	outputtreeview->setSelectionBehavior(QTreeView::SelectRows);		   //一次选中整行
	outputtreeview->setSelectionMode(QTreeView::SingleSelection);          //单选，配合上面的整行就是一次选单行
	//outputtreeview->setAlternatingRowColors(true);                       //每间隔一行颜色不一样，当有qss时该属性无效
	//outputtreeview->setFocusPolicy(Qt::NoFocus);                         //去掉鼠标移到单元格上时的虚线框
	//2.列头相关设置
	//outputtreeview->header()->hide();                                  //隐藏列头
	outputtreeview->header()->setStretchLastSection(true);               //最后一列自适应宽度
	outputtreeview->header()->setDefaultAlignment(Qt::AlignCenter);      //列头文字默认居中对齐

	
	outputtreemodel = new outputTreeModel(nullptr);
	outputtreeview->setModel(outputtreemodel);
	outputtreeview->setItemDelegateForColumn(2, new CheckBoxDelegate(outputtreeview));
	//outputtreeview->expandAll();

	//信号槽，右键菜单
	outputtreeview->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(outputtreeview, &QTreeView::customContextMenuRequested, this, &DigitaltwinsWidget::slotTreeMenu);
	
	
	//4.已收到信息栏
	ShowMessage = new QLabel("已接收消息");
	font = ShowMessage->font();//加粗字体
	font.setBold(true);
	ShowMessage->setFont(font);
	ShowMessage->setMinimumHeight(25);

	ReceivedMessage = new QTextEdit(this);
	ReceivedMessage->setText("请先进行初始化匹配，待接收到参数名称后，再进行神经网络加载（若不加载神经网络，须点击“不使用”按钮），加载完成后即可使用神经网络进行位姿计算...\n");


	//布局
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(MatchComputer);
	MatchComputer->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(this);
	HorinztalLayout2->addWidget(LocalMachineIdShow);
	HorinztalLayout2->addWidget(LocalMachineIdEdit);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->addWidget(ReceivedMessageIdShow);
	HorinztalLayout3->addWidget(ReceivedMessageIdEdit);
	QHBoxLayout* HorinztalLayoutAdd1 = new QHBoxLayout(this);
	HorinztalLayoutAdd1->setMargin(0);//一定要有
	HorinztalLayoutAdd1->addWidget(ButtonRTPS);
	HorinztalLayoutAdd1->setAlignment(ButtonRTPS, Qt::AlignCenter);//控件在布局中居中显示
	HorinztalLayoutAdd1->addWidget(ButtonDCPSInfoRepo);
	HorinztalLayoutAdd1->setAlignment(ButtonDCPSInfoRepo, Qt::AlignCenter);//控件在布局中居中显示
	QHBoxLayout* HorinztalLayout4 = new QHBoxLayout(this);
	HorinztalLayout4->addWidget(PushButtonInitMatch);
	QHBoxLayout* HorinztalLayout5 = new QHBoxLayout(this);
	HorinztalLayout5->addWidget(PushButtonCloseMatch);

	QHBoxLayout* HorinztalLayoutNN = new QHBoxLayout(this);
	HorinztalLayoutNN->addWidget(LoadNeuralNetwork);
	HorinztalLayoutNN->addWidget(PushButtonNoUseNet);
	HorinztalLayoutNN->addWidget(PushButtonAddLine);
	HorinztalLayoutNN->addWidget(PushButtonDelLine);
	HorinztalLayoutNN->addWidget(PushButtonCheckConfig);

	QHBoxLayout* HorinztalLayoutTable = new QHBoxLayout(this);
	HorinztalLayoutTable->addWidget(m_table);
	
	QHBoxLayout* HorinztalLayout6 = new QHBoxLayout(this);
	HorinztalLayout6->addWidget(ShowMessage);
	QHBoxLayout* HorinztalLayout7 = new QHBoxLayout(this);
	HorinztalLayout7->addWidget(ReceivedMessage);
	
	QHBoxLayout* HorinztalLayout8 = new QHBoxLayout(this);
	HorinztalLayout8->addWidget(ShowVariable);
	HorinztalLayout8->addWidget(PushButtonUpdateVariable);
	HorinztalLayout8->addWidget(PushButtonSave);
	HorinztalLayout8->addWidget(PushButtonOnlyShow);
	QHBoxLayout* HorinztalLayout9 = new QHBoxLayout(this);
	HorinztalLayout9->addWidget(outputtreeview);

	VerticalLayout->setSpacing(15);
	VerticalLayout->addLayout(HorinztalLayout1);
	VerticalLayout->addLayout(HorinztalLayout2);
	VerticalLayout->addLayout(HorinztalLayout3);
	VerticalLayout->addLayout(HorinztalLayoutAdd1);
	VerticalLayout->addLayout(HorinztalLayout4);
	VerticalLayout->addLayout(HorinztalLayout5);
	VerticalLayout->addLayout(HorinztalLayoutNN);
	VerticalLayout->addLayout(HorinztalLayoutTable);
	VerticalLayout->addLayout(HorinztalLayout8);
	VerticalLayout->addLayout(HorinztalLayout9);
	VerticalLayout->addLayout(HorinztalLayout6);
	VerticalLayout->addLayout(HorinztalLayout7);


	this->setLayout(VerticalLayout);
}

DigitaltwinsWidget::~DigitaltwinsWidget()
{
	if (DCPSopen == true)
	{
		myProcess->kill();
		delete myProcess;
	}

	if (m_table)delete m_table;
}

void DigitaltwinsWidget::ClickButtonInitMatch()
{
	if (initMatch == true)
	{
		string errormsg = "已经初始化！请勿重复初始化！";
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(errormsg));
		msgBox.exec();
		return;
	}
	
	int code = -1;
	if (m_doc && m_doc->IsModel())
	{
		MethodID = groupButtonDiscoveryMethod->checkedId();
		if (MethodID == 1 && DCPSopen == false)// 如果采用集中式发现方式，需要开启DCPS服务
		{
			myProcess = new QProcess(this);
			QStringList argument;
			string DCPSserver = "%DDS_ROOT%/bin/DCPSInfoRepo -ORBListenEndpoints iiop://10.15.193.87:65123";
			argument << "/c" << QString::fromStdString(DCPSserver);
			myProcess->start("cmd.exe", argument);
			DCPSopen = true;
		}
		code = ((DocumentModel*)m_doc)->InitSubsAndListner(MethodID);
	}
	if (code == 0)
	{
		// 如果此时的本机id存在，则将其传给数据监听者，便于其对收到的数据进行过滤，否则收到所有数据
		if (LocalMachineIdEdit->text().toStdString() != "")
		{
			SetSubhomeID(LocalMachineIdEdit->text().toStdString());
		}

		// 如果此时的接收消息id存在，则将其传给数据监听者，便于其对收到的数据进行过滤，否则收到所有数据
		if (ReceivedMessageIdEdit->text().toStdString() != "")
		{
			SetRecvMsgId(ReceivedMessageIdEdit->text().toStdString());
		}
		
		MessageDisplay += "匹配成功！连接已建立！";
		ReceivedMessage->setText(MessageDisplay);
		//利用光标使滚动条始终置于底部
		QTextCursor textCursor = ReceivedMessage->textCursor();
		textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		ReceivedMessage->setTextCursor(textCursor);
		ClickButtonUpdateVariable();
		initMatch = true;
		return;
	}
	else
	{
		string errormsg = "初始化失败!错误代码：" + std::to_string(code);
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(errormsg));
		msgBox.exec();
		((DocumentModel*)m_doc)->EndSubsAndListner();
		return;
	}
	return;
}

void DigitaltwinsWidget::ClickButtonCloseMatch()
{
	int code = -1;
	if (m_doc && m_doc->IsModel())
	{
		code = ((DocumentModel*)m_doc)->EndSubsAndListner();
	}	
	if (code == 0)
	{
		MessageDisplay = "";
		MessageDisplay += "连接已断开，实体已清理！如需重连，请重新初始化！";
		ReceivedMessage->setText(MessageDisplay);
		//利用光标使滚动条始终置于底部
		QTextCursor textCursor = ReceivedMessage->textCursor();
		textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		ReceivedMessage->setTextCursor(textCursor);
		QMessageBox msgBox;
		msgBox.setText("连接已断开，实体已清理!");
		msgBox.exec();
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

bool DigitaltwinsWidget::getIsUseNet()
{
	lock_guard<std::mutex> lck(mtxUseNet);
	return isUseNet;
}

bool DigitaltwinsWidget::getCanLoadNet()
{
	lock_guard<std::mutex> lck(mtxLoadNet);
	return canLoadNet;
}

//不使用神经网络
void DigitaltwinsWidget::ClickButtonNoUseNet()
{
	{
		lock_guard<std::mutex> lck(mtxUseNet);
		isUseNet = false;
	}
	{
		lock_guard<std::mutex> lck(mtxLoadNet);
		canLoadNet = true;
	}
}

//增加行数
void DigitaltwinsWidget::ClickButtonAddLine()
{
	m_table->setRowCount(m_table->rowCount() + 1);
	NeuralNetworkInfo info;
	m_data.push_back(make_pair(info, true));
	indexmap[m_table->rowCount() - 1] = m_data.size() - 1;
}


//减少行数
void DigitaltwinsWidget::ClickButtonDelLine()
{
	if (m_table->rowCount() == 0) {
		QMessageBox msgBox;
		msgBox.setText("参数表无多余行可删除");
		msgBox.exec();
		return;
	}
	int idx = m_table->rowCount() - 1;
	if (indexmap.find(idx) != indexmap.end()) {
		m_data[indexmap.at(idx)].second = false;
		indexmap.erase(indexmap.find(idx));
	}
	m_table->removeRow(idx);
}


//检查配置
void DigitaltwinsWidget::ClickButtonCheckConfig()
{
	if (initMatch == false)
	{
		QMessageBox msgBox;
		msgBox.setText("请先进行初始化匹配！");
		msgBox.exec();
		return;
	}
	
	if (m_table->rowCount() == 0) {
		QMessageBox msgBox;
		msgBox.setText("当前还未添加神经网络！如需进行检查，请至少完整添加一行数据！");
		msgBox.exec();
		return;
	}
	
	// 检查刷新表格种所填写的内容
	for (int row = 0; row < m_table->rowCount(); row++) {
		for (int col = 0; col < m_table->columnCount(); col++) {
			QTableWidgetItem* cur = m_table->item(row, col);
			if (cur->text().toStdString() == "") {
				QMessageBox msgBox;
				string msg = "当前第" + std::to_string(row + 1) + "行，第" + std::to_string(col + 1) + "列数据为空，请填写完整或删除该行数据。";
				msgBox.setText(QString::fromStdString(msg));
				msgBox.exec();
				return;
			}
			int idx = indexmap.at(row);
			if (col == 0) {
				m_data[idx].first.sNetworkFilePath = cur->text().toStdString();
			}
			else if (col == 1) {
				m_data[idx].first.sConfigFilePath = cur->text().toStdString();
			}
		}
	}

	set<string> setReceivedParas;
	if (m_doc && m_doc->IsModel()) {
		TwinAnimationThread* twinAniThread = ((DocumentModel*)m_doc)->GetTwinAniThread();
		StoredData* storedData = twinAniThread->m_pStoredData;
		vector<string>receivedParas = storedData->GetParameterNmaes();
		// 如果还没收到任何参数，提示等待
		if (receivedParas.size() == 0) {
			QMessageBox msgBox;
			msgBox.setText("还未接收到发布端传来的参数名称！请稍后再进行尝试！");
			msgBox.exec();
			return;
		}
		for (string receivedpara : receivedParas) {
			setReceivedParas.insert(receivedpara);
		}
	}

	
	// 对神经网络配置文件进行检查
	string allPaths = "";
	int i = 1;
	for (auto itr = indexmap.begin(); itr != indexmap.end(); itr++, i++) {
		string sNetworkFilePath = m_data[(*itr).second].first.sNetworkFilePath;
		string sConfigFilePath = m_data[(*itr).second].first.sConfigFilePath;
		string nowPath = "当前第" + std::to_string(i) + "行:神经网络路径：" + sNetworkFilePath + ",配置文件路径：" + sConfigFilePath + "\n";
		allPaths += nowPath;
		
		// 读取配置文件内容，存储到NeuralNetworkInfo中
		Sheet sheet = FileParse::parseCSV(QString::fromStdString(sConfigFilePath));
		int inputnums = atoi(sheet.data[0][0].toStdString().c_str());
		int outputnums = atoi(sheet.data[0][1].toStdString().c_str());
		m_data[(*itr).second].first.inputNums = inputnums;
		m_data[(*itr).second].first.outputNums = outputnums;
		QStringList paras = sheet.data[1];
		for (int i = 0; i < inputnums; i++) {
			m_data[(*itr).second].first.inputVarNames.push_back(paras[i].toStdString());
		}
		for (int j = inputnums; j < paras.size(); j++) {
			m_data[(*itr).second].first.outputVarNames.push_back(paras[j].toStdString());
		}

		// 检查神经网络是否可用
		vector<string> inputVarNames = m_data[(*itr).second].first.inputVarNames;
		for (string inputvar : inputVarNames) {
			if (!setReceivedParas.count(inputvar)) {
				QMessageBox msgBox;
				string msg = "当前第" + std::to_string(i) + "行神经网络不可用，缺少如下输入参数：" + inputvar;
				msgBox.setText(QString::fromStdString(msg));
				msgBox.exec();
				return;
			}
		}
	}
	allPaths += "神经网络均可用。\n";
	QMessageBox msgBox;
	msgBox.setText(QString::fromStdString(allPaths));
	msgBox.exec();
	{
		lock_guard<std::mutex> lck(mtxUseNet);
		isUseNet = true;
	}
	{
		lock_guard<std::mutex> lck(mtxLoadNet);
		canLoadNet = true;
	}
}


void DigitaltwinsWidget::currentItemChangedSLOT(QTableWidgetItem* last, QTableWidgetItem* cur)
{
	if (!cur) return;
	int r = cur->row();
	if (indexmap.find(r) == indexmap.end())return;
	int idx = indexmap.at(cur->row());
	switch (cur->column())
	{
	case 0:
		m_data[idx].first.sNetworkFilePath = cur->text().toStdString();
		break;
	case 1:
		m_data[idx].first.sConfigFilePath = cur->text().toStdString();
		break;
	}
}

//根据映射填充参数表
void DigitaltwinsWidget::fillPathtable()
{
	m_table->clear();

	QStringList header;
	m_table->setColumnCount(2);
	header << "神经网络文件路径(*.pt)" << "配置文件路径(*.csv)";
	m_table->setMinimumWidth(350);
	m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_table->setHorizontalHeaderLabels(header);
	m_table->setRowCount(indexmap.size());
	int i = 0;
	for (auto itr = indexmap.begin(); itr != indexmap.end(); itr++) {
		m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_data[(*itr).second].first.sNetworkFilePath)));
		m_table->setItem(i++, 1, new QTableWidgetItem(QString::fromStdString(m_data[(*itr).second].first.sConfigFilePath)));
	}
}

//将收到的信息显示
void DigitaltwinsWidget::ShowReceivedMsg() {
	if (m_doc && m_doc->IsModel())
	{
		string receivedmsg = ((DocumentModel*)m_doc)->GetReceivedMsg();
		MessageDisplay += QString::fromStdString(receivedmsg);
		// FIXME
		//这里暂时不能显示，内存不足，会导致程序卡死
		//减少发送数据的数据量后，应该不会卡死了
		ReceivedMessage->setText(MessageDisplay);
		//ReceivedMessage->setText("已收到参数名称！！！");
		//利用光标使滚动条始终置于底部
		QTextCursor textCursor = ReceivedMessage->textCursor();
		textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		ReceivedMessage->setTextCursor(textCursor);
	}
}

void DigitaltwinsWidget::slotTreeMenu(const QPoint& pos)
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

void DigitaltwinsWidget::CreateOutputModel()
{
	//0304
	m_outputList.clear();
	outputList_Attention.clear();
	AttenOutPutList_FullName.clear();
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

bool DigitaltwinsWidget::readCyberInfo(CyberInfo* pCyberInfo)
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

void DigitaltwinsWidget::OutputDFS(vector<myOutputVar*>& m_output, TreeItem* root, string s)
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

void DigitaltwinsWidget::ClickButtonUpdateVariable()
{
	CreateOutputModel();
	outputtreeview->setModel(outputtreemodel);
	//outputtreeview->expandAll();
	outputtreeview->setEditTriggers(QAbstractItemView::DoubleClicked);  //双击可编辑
}

void DigitaltwinsWidget::ClickButtonOnlyShow()
{
	//构造model
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

void DigitaltwinsWidget::ClickButtonSave()
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
		m_twinCyberInfo = new CyberInfo(nullptr);
		TreeItem* root2 = root->child(i);

		for (int itr = 0; itr < root2->childCount(); itr++)
			OutputDFS(m_output, root2->child(itr));
		m_twinCyberInfo->SetOutputVars(m_output);

		string name = childCompCyIn->GetMdlName();
		string type = childCompCyIn->GetMdlType();
		vector<myParameter*> m_para = childCompCyIn->GetParameters();
		vector<string> m_connectortypes = childCompCyIn->GetConnectorTypes();
		vector<string> m_connectornames = childCompCyIn->GetConnectorNames();
		m_twinCyberInfo->SetMdlName(name);
		m_twinCyberInfo->SetMdlType(type.c_str());
		m_twinCyberInfo->SetParameters(m_para);
		m_twinCyberInfo->SetConncTypes(m_connectortypes);
		m_twinCyberInfo->SetConncNames(m_connectornames);

		((DocumentComponent*)pComChild->GetDocument())->SetCyberInfo(m_twinCyberInfo);
		++itr;
	}
	QMessageBox msgBox;
	msgBox.setText("保存成功！请先点击\"加载所有变量\"，再点击\"只显示关注\"。");
	msgBox.exec();
}

void DigitaltwinsWidget::MenuGeneCurves()
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
		TwinAnimationThread* m_pTwinAniThread = ((DocumentModel*)m_doc)->GetTwinAniThread();
		m_pStoredData = m_pTwinAniThread->m_pStoredData;
		if (m_pStoredData != NULL)
		{
			if (m_pStoredData->isreceiving == false)
			{
				QMessageBox msgBox;
				msgBox.setText("还未接收到任何数据！");
				msgBox.exec();
				return;
			}
			else
			{
				vector<string> ParaNames = m_pStoredData->GetParameterNmaes();
				ColumnCount.clear();
				for (int i = 0; i < ParaNames.size(); i++)
				{
					string FullName = ParaNames[i];
					string::size_type idx;
					idx = FullName.find(name);
					if (idx != string::npos)
					{
						ColumnCount.push_back(i);
					}
				}
				if (!ColumnCount.empty())
				{
					DynamicCurveDisplayDlg* CurDisDlg = new DynamicCurveDisplayDlg(name, ColumnCount, this);
					CurDisDlg->show();
				}
				else
				{
					QMessageBox msgBox;
					QString message = text + "变量未在接收数据中，不能生成曲线！";
					msgBox.setText(message);
					msgBox.exec();
					return;
				}
			}

		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText("请先初始化匹配，确保已收到数据！");
			msgBox.exec();
			return;
		}		
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("未找到变量名称");
		msgBox.exec();
		return;
	}
}

//动态曲线展示对话框
DynamicCurveDisplayDlg::DynamicCurveDisplayDlg(string VariableName, vector<int> ColumnCount, DigitaltwinsWidget* parent) :m_parent(parent), QDialog(parent)
{
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

void DynamicCurveDisplayDlg::CurveDisplayDlgInit(string VariableName, vector<int> ColumnCount)
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
	StoredData* m_pStoredData = m_parent->GetStoredData();
	vector<string> ParaNames = m_pStoredData->GetParameterNmaes();
	cbo_Para = new QComboBox();
	for (int i = 0; i < ColumnCount.size(); i++)
	{
		QString name = QString::fromStdString(ParaNames[ColumnCount[i]]);
		//cbo_Para->addItem(QWidget::tr("male"));
		cbo_Para->addItem(name);
	}
	cbo_Para->setStyleSheet(BtnStyle);
	cbo_Para->setMinimumHeight(25);
	disconnect(cbo_Para, SIGNAL(activated(QString)), this, SLOT(ParaChanged()));
	connect(cbo_Para, SIGNAL(activated(QString)), this, SLOT(ParaChanged()));

	m_timer = new QTimer(this);
	m_timer->setSingleShot(false);
	QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));

	//曲线显示
	chartview = new QChartView();
	//根据列名获取所有点的信息
	vector<double> timeSeries;
	vector<double> paraSeries;
	vector<vector<string>> AllDatas = m_pStoredData->GetAllDatas();
	lasttime = -1;
	for (int i = 0; i < AllDatas.size(); i++)
	{
		double d_i = stod(AllDatas[i][0]);
		double d_para = stod(AllDatas[i][ColumnCount[0]]);
		if (d_i > lasttime)
		{
			timeSeries.push_back(d_i);
			paraSeries.push_back(d_para);
			lasttime = d_i;
		}
		else
		{
			timeSeries.clear();
			paraSeries.clear();
			timeSeries.push_back(d_i);
			paraSeries.push_back(d_para);
			lasttime = d_i;
		}
	}
	pretoreadRow = AllDatas.size();

	//设置表头
	chart = new DynamicChart(this, QString::fromStdString(ParaNames[ColumnCount[0]]));
	//设置坐标系
	max_value = *max_element(paraSeries.begin(), paraSeries.end());
	min_value = *min_element(paraSeries.begin(), paraSeries.end());
	double range = (max_value - min_value) / 10;
	if (range == 0) { range = 1; }                          //如果变量值为常数则相应增加range的范围
	max_time=*max_element(timeSeries.begin(), timeSeries.end());
	nowcol = ColumnCount[0];
	chart->setAxis("time", 0, max_time, 11, QString::fromStdString(ParaNames[ColumnCount[0]]), min_value - range, max_value + range, 11);
	//设置离散点数据
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
	
	m_timer->start(500);//启动定时器
}


DynamicCurveDisplayDlg::~DynamicCurveDisplayDlg()
{
	m_timer->stop();
}

void DynamicCurveDisplayDlg::ParaChanged()
{
	m_timer->stop();
	pointlist.clear();
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
	vector<double> paraSeries;
	StoredData* m_pStoredData = m_parent->GetStoredData();
	vector<string> ParaNames = m_pStoredData->GetParameterNmaes();
	vector<vector<string>> AllDatas = m_pStoredData->GetAllDatas();
	int col = -1;
	for (int i = 0; i < ParaNames.size(); i++)
	{
		if (ParaNames[i] == ParaName.toStdString())
		{
			col = i;
			nowcol = col;
			break;
		}
	}
	if (col == -1)
	{
		QMessageBox msgBox;
		msgBox.setText("找不到此参数！");
		msgBox.exec();
		return;
	}	
	lasttime = -1;
	for (int i = 0; i < AllDatas.size(); i++)
	{
		double d_i = stod(AllDatas[i][0]);
		double d_para = stod(AllDatas[i][nowcol]);
		if (d_i > lasttime)
		{
			timeSeries.push_back(d_i);
			paraSeries.push_back(d_para);
			lasttime = d_i;
		}
		else
		{
			timeSeries.clear();
			paraSeries.clear();
			timeSeries.push_back(d_i);
			paraSeries.push_back(d_para);
			lasttime = d_i;
		}
	}
	pretoreadRow = AllDatas.size();
	//设置表头
	delete chart;
	chart = new DynamicChart(this, ParaName);
	//设置坐标系
	max_value = *max_element(paraSeries.begin(), paraSeries.end());
	min_value = *min_element(paraSeries.begin(), paraSeries.end());
	double range = (max_value - min_value) / 10;
	if (range <= 0.000001) { range = 1; }
	max_time = *max_element(timeSeries.begin(), timeSeries.end());
	chart->setAxis("time", 0, max_time, 11, ParaName, min_value - range, max_value + range, 11);
	//设置离散点数据
	for (int i = 0; i < timeSeries.size(); i++)
	{
		pointlist.append(QPointF(timeSeries[i], paraSeries[i]));
	}
	//绘制
	chart->buildChart(pointlist);
	chartview->setChart(chart->qchart);
	m_timer->start(500);//启动定时器
}

void DynamicCurveDisplayDlg::slotTimeout()
{
	m_timer->stop();
	StoredData* m_pStoredData = m_parent->GetStoredData();
	vector<vector<string>> AllDatas = m_pStoredData->GetAllDatas();
	vector<string> ParaNames = m_pStoredData->GetParameterNmaes();
	bool havecleared = false;//是否有被清空过
	if (pretoreadRow < AllDatas.size())
	{
		QList<QPointF> addpointlist;
		for (int i = pretoreadRow; i < AllDatas.size(); i++)
		{
			if (stod(AllDatas[i][0]) > lasttime)
			{
				pointlist.append(QPointF(stod(AllDatas[i][0]), stod(AllDatas[i][nowcol])));
				addpointlist.append(QPointF(stod(AllDatas[i][0]), stod(AllDatas[i][nowcol])));
				max_value = max(max_value, stod(AllDatas[i][nowcol]));
				min_value = min(min_value, stod(AllDatas[i][nowcol]));
				max_time = max(max_time, stod(AllDatas[i][0]));
				lasttime = stod(AllDatas[i][0]);
			}
			else
			{
				havecleared = true;
				pointlist.clear();
				pointlist.append(QPointF(stod(AllDatas[i][0]), stod(AllDatas[i][nowcol])));
				max_value = max(max_value, stod(AllDatas[i][nowcol]));
				min_value = min(min_value, stod(AllDatas[i][nowcol]));
				max_time = stod(AllDatas[i][0]);
				lasttime = stod(AllDatas[i][0]);
			}
		}
		pretoreadRow = AllDatas.size();
		double range = (max_value - min_value) / 10;
		chart->setAxis("time", 0, max_time, 11, QString::fromStdString(ParaNames[nowcol]), min_value - range, max_value + range, 11);
		if (havecleared == true)
		{
			chart->ChangeData(pointlist);
		}
		else
		{
			//如果一直使用ChangeData(),图像会一直闪动，所以只在清空了原来数据的时候调用ChangeData()
			chart->AddData(addpointlist);
		}
		//chartview->setChart(chart->qchart);
	}
	m_timer->start(500);//启动定时器
}


//Chart类实现
DynamicChart::DynamicChart(QWidget* parent, QString _chartname)
{
	setParent(parent);
	chartname = _chartname;
	series = new QSplineSeries(this);
	qchart = new QChart;
	axisX = new QValueAxis(this);
	axisY = new QValueAxis(this);
}

void DynamicChart::setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc, \
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

void DynamicChart::buildChart(QList<QPointF> pointlist)
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

void DynamicChart::ChangeData(QList<QPointF> pointlist) 
{
	series->clear();
	for (int i = 0; i < pointlist.size(); i++)
		series->append(pointlist.at(i).x(), pointlist.at(i).y());
}

void DynamicChart::AddData(QList<QPointF> pointlist)
{
	for (int i = 0; i < pointlist.size(); i++)
		series->append(pointlist.at(i).x(), pointlist.at(i).y());
}