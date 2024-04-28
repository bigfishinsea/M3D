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

	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//�������岼��Ϊ��ֱ����
	VerticalLayout->setDirection(QBoxLayout::TopToBottom);

	//1.�����ƥ����
	MatchComputer = new QLabel("�����ƥ����");
	font = MatchComputer->font();//�Ӵ�����
	font.setBold(true);
	MatchComputer->setFont(font);
	MatchComputer->setMinimumHeight(25);

	//����id
	LocalMachineIdShow = new QLabel("����ID��");
	LocalMachineIdShow->setMinimumHeight(25);
	LocalMachineIdShow->setFixedWidth(150);

	LocalMachineIdEdit = new QLineEdit;
	LocalMachineIdEdit->setReadOnly(false); //�ɱ༭
	LocalMachineIdEdit->setMinimumHeight(25);
	LocalMachineIdEdit->setFixedWidth(150);

	//���������Ϣid�������ж����
	ReceivedMessageIdShow = new QLabel("������ϢID(;�ָ�)��");
	ReceivedMessageIdShow->setMinimumHeight(25);
	ReceivedMessageIdShow->setFixedWidth(150);

	ReceivedMessageIdEdit = new QLineEdit;
	ReceivedMessageIdEdit->setReadOnly(false); //�ɱ༭
	ReceivedMessageIdEdit->setMinimumHeight(25);
	ReceivedMessageIdEdit->setFixedWidth(150);

	//ѡ��DDS���ַ�ʽ
	ButtonRTPS = new QRadioButton(this);
	ButtonRTPS->setText("�˵��˷���");
	ButtonDCPSInfoRepo = new QRadioButton(this);
	ButtonDCPSInfoRepo->setText("����ʽ����");
	groupButtonDiscoveryMethod = new QButtonGroup(this);
	groupButtonDiscoveryMethod->addButton(ButtonRTPS, 0);//idΪ0
	groupButtonDiscoveryMethod->addButton(ButtonDCPSInfoRepo, 1);//idΪ1
	groupButtonDiscoveryMethod->button(MethodID)->setChecked(true); //Ĭ��ѡ�ж˵��˷���

	//��ʼ��ƥ�䰴ť
	PushButtonInitMatch = new QPushButton(this);
	PushButtonInitMatch->setText("��ʼ��ƥ��");
	PushButtonInitMatch->setFixedSize(400, 25);
	connect(PushButtonInitMatch, SIGNAL(clicked()), this, SLOT(ClickButtonInitMatch()));

	//����ƥ�䰴ť
	PushButtonCloseMatch = new QPushButton(this);
	PushButtonCloseMatch->setText("�Ͽ�ƥ��");
	PushButtonCloseMatch->setFixedSize(400, 25);
	connect(PushButtonCloseMatch, SIGNAL(clicked()), this, SLOT(ClickButtonCloseMatch()));

	//2.�����������
	LoadNeuralNetwork = new QLabel("�����������");
	font = LoadNeuralNetwork->font();//�Ӵ�����
	font.setBold(true);
	LoadNeuralNetwork->setFont(font);
	LoadNeuralNetwork->setMinimumHeight(25);

	//��ʹ�������簴ť
	PushButtonNoUseNet = new QPushButton(this);
	PushButtonNoUseNet->setText("��ʹ��");
	PushButtonNoUseNet->setFixedSize(75, 25);
	connect(PushButtonNoUseNet, SIGNAL(clicked()), this, SLOT(ClickButtonNoUseNet()));

	//����������ť
	PushButtonAddLine = new QPushButton(this);
	PushButtonAddLine->setText("+");
	PushButtonAddLine->setFixedSize(25, 25);
	connect(PushButtonAddLine, SIGNAL(clicked()), this, SLOT(ClickButtonAddLine()));

	//��ȥ������ť
	PushButtonDelLine = new QPushButton(this);
	PushButtonDelLine->setText("-");
	PushButtonDelLine->setFixedSize(25, 25);
	connect(PushButtonDelLine, SIGNAL(clicked()), this, SLOT(ClickButtonDelLine()));

	//������ð�ť
	PushButtonCheckConfig = new QPushButton(this);
	PushButtonCheckConfig->setText("��������ļ�");
	PushButtonCheckConfig->setFixedSize(125, 25);
	connect(PushButtonCheckConfig, SIGNAL(clicked()), this, SLOT(ClickButtonCheckConfig()));

	m_table = new QTableWidget(this);
	connect(m_table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
		this, SLOT(currentItemChangedSLOT(QTableWidgetItem*, QTableWidgetItem*)));
	fillPathtable();



	//3.������ʾ��
	ShowVariable = new QLabel("������ʾ��");
	font = ShowVariable->font();//�Ӵ�����
	font.setBold(true);
	ShowVariable->setFont(font);
	ShowVariable->setMinimumHeight(25);

	PushButtonUpdateVariable = new QPushButton(this);
	PushButtonUpdateVariable->setText("�������б���");
	PushButtonUpdateVariable->setFixedSize(125, 25);
	connect(PushButtonUpdateVariable, SIGNAL(clicked()), this, SLOT(ClickButtonUpdateVariable()));

	PushButtonSave = new QPushButton(this);
	PushButtonSave->setText("�����޸�");
	PushButtonSave->setFixedSize(125, 25);
	connect(PushButtonSave, SIGNAL(clicked()), this, SLOT(ClickButtonSave()));

	PushButtonOnlyShow = new QPushButton(this);
	PushButtonOnlyShow->setText("ֻ��ʾ��ע");
	PushButtonOnlyShow->setFixedSize(125, 25);
	connect(PushButtonOnlyShow, SIGNAL(clicked()), this, SLOT(ClickButtonOnlyShow()));

	//����view
	outputtreeview = new QTreeView(this);
	//1.QTreeView����������
	//outputtreeview->setEditTriggers(QTreeView::NoEditTriggers);		   //��Ԫ���ܱ༭
	outputtreeview->setSelectionBehavior(QTreeView::SelectRows);		   //һ��ѡ������
	outputtreeview->setSelectionMode(QTreeView::SingleSelection);          //��ѡ�������������о���һ��ѡ����
	//outputtreeview->setAlternatingRowColors(true);                       //ÿ���һ����ɫ��һ��������qssʱ��������Ч
	//outputtreeview->setFocusPolicy(Qt::NoFocus);                         //ȥ������Ƶ���Ԫ����ʱ�����߿�
	//2.��ͷ�������
	//outputtreeview->header()->hide();                                  //������ͷ
	outputtreeview->header()->setStretchLastSection(true);               //���һ������Ӧ���
	outputtreeview->header()->setDefaultAlignment(Qt::AlignCenter);      //��ͷ����Ĭ�Ͼ��ж���

	
	outputtreemodel = new outputTreeModel(nullptr);
	outputtreeview->setModel(outputtreemodel);
	outputtreeview->setItemDelegateForColumn(2, new CheckBoxDelegate(outputtreeview));
	//outputtreeview->expandAll();

	//�źŲۣ��Ҽ��˵�
	outputtreeview->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(outputtreeview, &QTreeView::customContextMenuRequested, this, &DigitaltwinsWidget::slotTreeMenu);
	
	
	//4.���յ���Ϣ��
	ShowMessage = new QLabel("�ѽ�����Ϣ");
	font = ShowMessage->font();//�Ӵ�����
	font.setBold(true);
	ShowMessage->setFont(font);
	ShowMessage->setMinimumHeight(25);

	ReceivedMessage = new QTextEdit(this);
	ReceivedMessage->setText("���Ƚ��г�ʼ��ƥ�䣬�����յ��������ƺ��ٽ�����������أ��������������磬��������ʹ�á���ť����������ɺ󼴿�ʹ�����������λ�˼���...\n");


	//����
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
	HorinztalLayoutAdd1->setMargin(0);//һ��Ҫ��
	HorinztalLayoutAdd1->addWidget(ButtonRTPS);
	HorinztalLayoutAdd1->setAlignment(ButtonRTPS, Qt::AlignCenter);//�ؼ��ڲ����о�����ʾ
	HorinztalLayoutAdd1->addWidget(ButtonDCPSInfoRepo);
	HorinztalLayoutAdd1->setAlignment(ButtonDCPSInfoRepo, Qt::AlignCenter);//�ؼ��ڲ����о�����ʾ
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
		string errormsg = "�Ѿ���ʼ���������ظ���ʼ����";
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(errormsg));
		msgBox.exec();
		return;
	}
	
	int code = -1;
	if (m_doc && m_doc->IsModel())
	{
		MethodID = groupButtonDiscoveryMethod->checkedId();
		if (MethodID == 1 && DCPSopen == false)// ������ü���ʽ���ַ�ʽ����Ҫ����DCPS����
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
		// �����ʱ�ı���id���ڣ����䴫�����ݼ����ߣ���������յ������ݽ��й��ˣ������յ���������
		if (LocalMachineIdEdit->text().toStdString() != "")
		{
			SetSubhomeID(LocalMachineIdEdit->text().toStdString());
		}

		// �����ʱ�Ľ�����Ϣid���ڣ����䴫�����ݼ����ߣ���������յ������ݽ��й��ˣ������յ���������
		if (ReceivedMessageIdEdit->text().toStdString() != "")
		{
			SetRecvMsgId(ReceivedMessageIdEdit->text().toStdString());
		}
		
		MessageDisplay += "ƥ��ɹ��������ѽ�����";
		ReceivedMessage->setText(MessageDisplay);
		//���ù��ʹ������ʼ�����ڵײ�
		QTextCursor textCursor = ReceivedMessage->textCursor();
		textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		ReceivedMessage->setTextCursor(textCursor);
		ClickButtonUpdateVariable();
		initMatch = true;
		return;
	}
	else
	{
		string errormsg = "��ʼ��ʧ��!������룺" + std::to_string(code);
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
		MessageDisplay += "�����ѶϿ���ʵ�����������������������³�ʼ����";
		ReceivedMessage->setText(MessageDisplay);
		//���ù��ʹ������ʼ�����ڵײ�
		QTextCursor textCursor = ReceivedMessage->textCursor();
		textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		ReceivedMessage->setTextCursor(textCursor);
		QMessageBox msgBox;
		msgBox.setText("�����ѶϿ���ʵ��������!");
		msgBox.exec();
		initMatch = false;
		return;
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("����δ֪����!");
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

//��ʹ��������
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

//��������
void DigitaltwinsWidget::ClickButtonAddLine()
{
	m_table->setRowCount(m_table->rowCount() + 1);
	NeuralNetworkInfo info;
	m_data.push_back(make_pair(info, true));
	indexmap[m_table->rowCount() - 1] = m_data.size() - 1;
}


//��������
void DigitaltwinsWidget::ClickButtonDelLine()
{
	if (m_table->rowCount() == 0) {
		QMessageBox msgBox;
		msgBox.setText("�������޶����п�ɾ��");
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


//�������
void DigitaltwinsWidget::ClickButtonCheckConfig()
{
	if (initMatch == false)
	{
		QMessageBox msgBox;
		msgBox.setText("���Ƚ��г�ʼ��ƥ�䣡");
		msgBox.exec();
		return;
	}
	
	if (m_table->rowCount() == 0) {
		QMessageBox msgBox;
		msgBox.setText("��ǰ��δ��������磡������м�飬�������������һ�����ݣ�");
		msgBox.exec();
		return;
	}
	
	// ���ˢ�±��������д������
	for (int row = 0; row < m_table->rowCount(); row++) {
		for (int col = 0; col < m_table->columnCount(); col++) {
			QTableWidgetItem* cur = m_table->item(row, col);
			if (cur->text().toStdString() == "") {
				QMessageBox msgBox;
				string msg = "��ǰ��" + std::to_string(row + 1) + "�У���" + std::to_string(col + 1) + "������Ϊ�գ�����д������ɾ���������ݡ�";
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
		// �����û�յ��κβ�������ʾ�ȴ�
		if (receivedParas.size() == 0) {
			QMessageBox msgBox;
			msgBox.setText("��δ���յ������˴����Ĳ������ƣ����Ժ��ٽ��г��ԣ�");
			msgBox.exec();
			return;
		}
		for (string receivedpara : receivedParas) {
			setReceivedParas.insert(receivedpara);
		}
	}

	
	// �������������ļ����м��
	string allPaths = "";
	int i = 1;
	for (auto itr = indexmap.begin(); itr != indexmap.end(); itr++, i++) {
		string sNetworkFilePath = m_data[(*itr).second].first.sNetworkFilePath;
		string sConfigFilePath = m_data[(*itr).second].first.sConfigFilePath;
		string nowPath = "��ǰ��" + std::to_string(i) + "��:������·����" + sNetworkFilePath + ",�����ļ�·����" + sConfigFilePath + "\n";
		allPaths += nowPath;
		
		// ��ȡ�����ļ����ݣ��洢��NeuralNetworkInfo��
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

		// ����������Ƿ����
		vector<string> inputVarNames = m_data[(*itr).second].first.inputVarNames;
		for (string inputvar : inputVarNames) {
			if (!setReceivedParas.count(inputvar)) {
				QMessageBox msgBox;
				string msg = "��ǰ��" + std::to_string(i) + "�������粻���ã�ȱ���������������" + inputvar;
				msgBox.setText(QString::fromStdString(msg));
				msgBox.exec();
				return;
			}
		}
	}
	allPaths += "����������á�\n";
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

//����ӳ����������
void DigitaltwinsWidget::fillPathtable()
{
	m_table->clear();

	QStringList header;
	m_table->setColumnCount(2);
	header << "�������ļ�·��(*.pt)" << "�����ļ�·��(*.csv)";
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

//���յ�����Ϣ��ʾ
void DigitaltwinsWidget::ShowReceivedMsg() {
	if (m_doc && m_doc->IsModel())
	{
		string receivedmsg = ((DocumentModel*)m_doc)->GetReceivedMsg();
		MessageDisplay += QString::fromStdString(receivedmsg);
		// FIXME
		//������ʱ������ʾ���ڴ治�㣬�ᵼ�³�����
		//���ٷ������ݵ���������Ӧ�ò��Ῠ����
		ReceivedMessage->setText(MessageDisplay);
		//ReceivedMessage->setText("���յ��������ƣ�����");
		//���ù��ʹ������ʼ�����ڵײ�
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
	//menu.setStyleSheet(qss);    //���Ը��˵�������ʽ

	QModelIndex curIndex = outputtreeview->indexAt(pos);      //��ǰ�����Ԫ�ص�index
	QModelIndex index = curIndex.sibling(curIndex.row(), 0); //���еĵ�1��Ԫ�ص�index
	if (index.isValid())
	{
		//�ɻ�ȡԪ�ص��ı���data,���������жϴ���
		//QStandardItem* item = AttenOutputTreeModel->itemFromIndex(index);
		//QString text = item->text();
		//QVariant data = item->data(Qt::UserRole + 1);
		//...

		//���һ�в˵�
		menu.addAction(QString("�������"), this, SLOT(MenuGeneCurves()));
		//menu.addSeparator();    //���һ���ָ���
		//menu.addAction(QString("�۵�"), this, SLOT(slotTreeMenuCollapse(bool)));
	}
	menu.exec(QCursor::pos());  //��ʾ�˵�
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
		msgBox.setText("��ȡʧ��");
		msgBox.exec();
		return false;
	}
	//�������������
	//QStringList m_outputList;
	int size = pCyberInfo->GetOutputVars().size();
	for (int itr = 0; itr < size; ++itr) {
		string s = "";
		string s_FullName = "";
		string cur = "";
		//1.ȫ������
		string sName = pCyberInfo->GetOutputVars().at(itr)->sFullName;
		string sName_Full = sCompName + "." + pCyberInfo->GetOutputVars().at(itr)->sFullName;
		int n_times = count(sName.begin(), sName.end(), '.') + 1;//���ڼ����������������+1
		//int n_times = count(sName.begin(), sName.end(), '.');
		if (n_times - 1) sName.replace(0, sName.find_last_of('.') + 1, "");//�ж�������Ӧ-1
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

		//3.�Ƿ��ע���ע��
		s += "\t";
		s += pCyberInfo->GetOutputVars().at(itr)->bFocused ? "��" : "NULL";

		s_FullName += "\t";
		s_FullName += pCyberInfo->GetOutputVars().at(itr)->bFocused ? "��" : "NULL";

		//2.����
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
		idx = s.find("��");
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
	if (root->data(2) == "��") temp->bFocused = 1;
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
	outputtreeview->setEditTriggers(QAbstractItemView::DoubleClicked);  //˫���ɱ༭
}

void DigitaltwinsWidget::ClickButtonOnlyShow()
{
	//����model
	AttenOutputTreeModel = new QStandardItemModel(this);
	AttenOutputTreeModel->setHorizontalHeaderLabels(QStringList() << QString("ȫ��") << QString("����") << QString("�Ƿ��ע") << QString("����"));     //������ͷ
	//����ȡ����AttentionOutPutList
	//��ñ���ȫ����������
	vector<QString> vec_FullName;
	vector<QString> vec_Types;
	//vector<QString> AllFirstName;//����
	//set<QString> SetAllFirstName;//����
	for (int i = 0; i < AttenOutPutList_FullName.size(); i++)
	{
		//����������ַ��������Ǽ����飬��Щ���� sep �ָ���start �� end ָ�����
		//end Ĭ��Ϊ �C1 �����ص���[ start, end ]�ڵĿ���ɵ��ַ���
		QString FullString = AttenOutPutList_FullName.at(i);
		QString FullName = FullString.section("\t", 1, 1);
		vec_FullName.push_back(FullName);
		QString Types = FullString.section("\t", 3, 3);
		vec_Types.push_back(Types);
	}


	//�����modeltree
	for (int i = 0; i < AttenOutPutList_FullName.size(); i++)
	{
		//һ���ڵ㣬����mModel
		QList<QStandardItem*> items1;
		QStandardItem* item1 = new QStandardItem(vec_FullName[i]);
		QStandardItem* item2 = new QStandardItem(vec_Types[i]);
		QStandardItem* item3 = new QStandardItem(QString("��"));
		items1.append(item1);
		items1.append(item2);
		items1.append(item3);
		AttenOutputTreeModel->appendRow(items1);
	}
	outputtreeview->setModel(AttenOutputTreeModel);
	outputtreeview->setEditTriggers(QAbstractItemView::NoEditTriggers);   //������Ϊ���ɱ༭
	//outputtreeview->expandAll();

	//ֱ�ӵ���setupModelData������״�ṹ��ʾ����
	//outputtreemodel->setupModelData(outputList_Attention, outputtreemodel->getrootItem());
	//outputtreeview->setModel(outputtreemodel);
	//outputtreeview->expandAll();

}

void DigitaltwinsWidget::ClickButtonSave()
{
	//ģ���������
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
	msgBox.setText("����ɹ������ȵ��\"�������б���\"���ٵ��\"ֻ��ʾ��ע\"��");
	msgBox.exec();
}

void DigitaltwinsWidget::MenuGeneCurves()
{
	QModelIndex curIndex = outputtreeview->currentIndex();
	QModelIndex index = curIndex.sibling(curIndex.row(), 0); //ͬһ�е�һ��Ԫ�ص�index
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
			//��Ѱ�Ҹ��ڵ㣬��ñ���ȫ��
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
				msgBox.setText("��δ���յ��κ����ݣ�");
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
					QString message = text + "����δ�ڽ��������У������������ߣ�";
					msgBox.setText(message);
					msgBox.exec();
					return;
				}
			}

		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText("���ȳ�ʼ��ƥ�䣬ȷ�����յ����ݣ�");
			msgBox.exec();
			return;
		}		
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("δ�ҵ���������");
		msgBox.exec();
		return;
	}
}

//��̬����չʾ�Ի���
DynamicCurveDisplayDlg::DynamicCurveDisplayDlg(string VariableName, vector<int> ColumnCount, DigitaltwinsWidget* parent) :m_parent(parent), QDialog(parent)
{
	CurveDisplayDlgInit(VariableName, ColumnCount);
	setMinimumWidth(800);
	setMinimumHeight(600);

	// ���ô�����󻯺���С��
	Qt::WindowFlags windowFlag = Qt::Dialog;
	windowFlag |= Qt::WindowMinimizeButtonHint;
	windowFlag |= Qt::WindowMaximizeButtonHint;
	windowFlag |= Qt::WindowCloseButtonHint;
	setWindowFlags(windowFlag);
}

void DynamicCurveDisplayDlg::CurveDisplayDlgInit(string VariableName, vector<int> ColumnCount)
{
	setWindowTitle(tr("����������ʾ"));
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
		"QComboBox{"
		"font: 18px;"
		"}"
	);

	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//�������岼��Ϊ��ֱ����
	VariaNameShow = new QLabel("�������ƣ�");
	VariaNameShow->setMinimumHeight(25);

	VariableNameEdit = new QLineEdit;
	VariableNameEdit->setReadOnly(true); //���ɱ༭
	VariableNameEdit->setText(QString::fromStdString(VariableName));
	VariableNameEdit->setMinimumHeight(25);
	VariableNameEdit->setStyleSheet("background:transparent;border-width:0;border-style:outset");//�������ޱ߿���͸��

	ParaSelectTips = new QLabel("ѡ��Ҫ�������ߵĲ�����");
	ParaSelectTips->setMinimumHeight(25);

	//�����������ɹ�ѡ��Ĳ���
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

	//������ʾ
	chartview = new QChartView();
	//����������ȡ���е����Ϣ
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

	//���ñ�ͷ
	chart = new DynamicChart(this, QString::fromStdString(ParaNames[ColumnCount[0]]));
	//��������ϵ
	max_value = *max_element(paraSeries.begin(), paraSeries.end());
	min_value = *min_element(paraSeries.begin(), paraSeries.end());
	double range = (max_value - min_value) / 10;
	if (range == 0) { range = 1; }                          //�������ֵΪ��������Ӧ����range�ķ�Χ
	max_time=*max_element(timeSeries.begin(), timeSeries.end());
	nowcol = ColumnCount[0];
	chart->setAxis("time", 0, max_time, 11, QString::fromStdString(ParaNames[ColumnCount[0]]), min_value - range, max_value + range, 11);
	//������ɢ������
	for (int i = 0; i < timeSeries.size(); i++)
	{
		pointlist.append(QPointF(timeSeries[i], paraSeries[i]));
	}
	//����
	chart->buildChart(pointlist);
	chartview->setChart(chart->qchart);


	//����
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(VariaNameShow);
	HorinztalLayout1->addWidget(VariableNameEdit);
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(this);
	HorinztalLayout2->addWidget(ParaSelectTips);
	HorinztalLayout2->addWidget(cbo_Para);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->addWidget(chartview);
	chartview->setRenderHint(QPainter::Antialiasing);//��ֹͼ������


	VerticalLayout->setSpacing(15);
	VerticalLayout->addLayout(HorinztalLayout1);
	VerticalLayout->addLayout(HorinztalLayout2);
	VerticalLayout->addLayout(HorinztalLayout3);

	this->setLayout(VerticalLayout);
	
	m_timer->start(500);//������ʱ��
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
	//QString message = "��ǰ��ѡ����Ϊ��" + ParaName;
	//msgBox.setText(message);
	//msgBox.exec();
	//chart = new Chart(this, ParaName);
	//chart->setAxis("X��", 0, 100, 11, "Y��", 0, 20, 11);
	//QList<QPointF> pointlist = { QPointF(0,1), QPointF(10,10), QPointF(20,4), QPointF(30,8), QPointF(40,16), \
	//							QPointF(50,16), QPointF(60,8), QPointF(70,4), QPointF(80,2), QPointF(90,1), };

	//����������ȡ���е����Ϣ
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
		msgBox.setText("�Ҳ����˲�����");
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
	//���ñ�ͷ
	delete chart;
	chart = new DynamicChart(this, ParaName);
	//��������ϵ
	max_value = *max_element(paraSeries.begin(), paraSeries.end());
	min_value = *min_element(paraSeries.begin(), paraSeries.end());
	double range = (max_value - min_value) / 10;
	if (range <= 0.000001) { range = 1; }
	max_time = *max_element(timeSeries.begin(), timeSeries.end());
	chart->setAxis("time", 0, max_time, 11, ParaName, min_value - range, max_value + range, 11);
	//������ɢ������
	for (int i = 0; i < timeSeries.size(); i++)
	{
		pointlist.append(QPointF(timeSeries[i], paraSeries[i]));
	}
	//����
	chart->buildChart(pointlist);
	chartview->setChart(chart->qchart);
	m_timer->start(500);//������ʱ��
}

void DynamicCurveDisplayDlg::slotTimeout()
{
	m_timer->stop();
	StoredData* m_pStoredData = m_parent->GetStoredData();
	vector<vector<string>> AllDatas = m_pStoredData->GetAllDatas();
	vector<string> ParaNames = m_pStoredData->GetParameterNmaes();
	bool havecleared = false;//�Ƿ��б���չ�
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
			//���һֱʹ��ChangeData(),ͼ���һֱ����������ֻ�������ԭ�����ݵ�ʱ�����ChangeData()
			chart->AddData(addpointlist);
		}
		//chartview->setChart(chart->qchart);
	}
	m_timer->start(500);//������ʱ��
}


//Chart��ʵ��
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

	axisX->setRange(xmin, xmax);    //���÷�Χ
	axisX->setLabelFormat("%f");   //���ÿ̶ȵĸ�ʽ
	/************************************
		%u �޷���ʮ��������
		%s �ַ���
		%c һ���ַ�
		%d �з���ʮ��������
		%e ��������e-������
		%f ��������ʮ���Ƽ�����
		%s �ַ���
		%E ��������E-������
	****************************************/
	axisX->setGridLineVisible(true);   //�����߿ɼ�
	axisX->setTickCount(xtickc);       //���ö��ٸ����
	axisX->setMinorTickCount(1);   //����ÿ���������С�̶��ߵ���Ŀ
	axisX->setTitleText(xname);  //��������
	axisY->setRange(ymin, ymax);
	axisY->setLabelFormat("%f");
	axisY->setGridLineVisible(true);
	axisY->setTickCount(ytickc);
	axisY->setMinorTickCount(1);
	axisY->setTitleText(yname);
	qchart->addAxis(axisX, Qt::AlignBottom); //�£�Qt::AlignBottom  �ϣ�Qt::AlignTop
	qchart->addAxis(axisY, Qt::AlignLeft);   //��Qt::AlignLeft    �ң�Qt::AlignRight
}

void DynamicChart::buildChart(QList<QPointF> pointlist)
{
	//��������Դ
	series->setPen(QPen(Qt::blue, 3, Qt::SolidLine));
	series->clear();
	for (int i = 0; i < pointlist.size(); i++)
		series->append(pointlist.at(i).x(), pointlist.at(i).y());

	qchart->setTitle(chartname);
	qchart->setAnimationOptions(QChart::SeriesAnimations);//�������߶���ģʽ
	qchart->legend()->hide(); //����ͼ��
	qchart->addSeries(series);//��������
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