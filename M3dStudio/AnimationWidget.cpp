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

	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//�������岼��Ϊ��ֱ����
	VerticalLayout->setDirection(QBoxLayout::TopToBottom);

	//1.����������
	SetAnimation = new QLabel("����������");
	font = SetAnimation->font();//�Ӵ�����
	font.setBold(true);
	SetAnimation->setFont(font);
	SetAnimation->setMinimumHeight(25);

	//���Ű�ť
	PushButtonPlay = new QPushButton(this);
	PushButtonPlay->setText("");
	QPixmap pixmap("./images/animationplay.png");
	QPixmap fitpixmap = pixmap.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	PushButtonPlay->setIcon(QIcon(fitpixmap));
	PushButtonPlay->setIconSize(QSize(50, 50));
	PushButtonPlay->setFlat(true);
	PushButtonPlay->setStyleSheet("border: 0px"); //�����߿�
	PushButtonPlay->setToolTip("����");
	//PushButtonPlay->setText("����");
	//PushButtonPlay->setFixedSize(80, 80);
	connect(PushButtonPlay, SIGNAL(clicked()), this, SLOT(ClickButtonPlay()));

	//��ͣ��ť
	PushButtonPause = new QPushButton(this);
	PushButtonPause->setText("");
	QPixmap pixmap2("./images/animationpause.png");
	QPixmap fitpixmap2 = pixmap2.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	PushButtonPause->setIcon(QIcon(fitpixmap2));
	PushButtonPause->setIconSize(QSize(50, 50));
	PushButtonPause->setFlat(true);
	PushButtonPause->setStyleSheet("border: 0px"); //�����߿�
	PushButtonPause->setToolTip("��ͣ");
	//PushButtonPause->setText("��ͣ");
	//PushButtonPause->setFixedSize(80, 80);
	connect(PushButtonPause, SIGNAL(clicked()), this, SLOT(ClickButtonPause()));

	//�ָ���ť
	PushButtonResume = new QPushButton(this);
	PushButtonResume->setText("");
	QPixmap pixmap3("./images/animationcontinues.png");
	QPixmap fitpixmap3 = pixmap3.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	PushButtonResume->setIcon(QIcon(fitpixmap3));
	PushButtonResume->setIconSize(QSize(50, 50));
	PushButtonResume->setFlat(true);
	PushButtonResume->setStyleSheet("border: 0px"); //�����߿�
	PushButtonResume->setToolTip("�ָ�����");
	//PushButtonResume->setText("�ָ�");
	//PushButtonResume->setFixedSize(80, 80);
	connect(PushButtonResume, SIGNAL(clicked()), this, SLOT(ClickButtonResume()));

	//���ð�ť
	PushButtonReset = new QPushButton(this);
	PushButtonReset->setText("");
	QPixmap pixmap4("./images/animationreset.png");
	QPixmap fitpixmap4 = pixmap4.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	PushButtonReset->setIcon(QIcon(fitpixmap4));
	PushButtonReset->setIconSize(QSize(50, 50));
	PushButtonReset->setFlat(true);
	PushButtonReset->setStyleSheet("border: 0px"); //�����߿�
	PushButtonReset->setToolTip("���ö���");
	//PushButtonReset->setText("����");
	//PushButtonReset->setFixedSize(80, 80);
	connect(PushButtonReset, SIGNAL(clicked()), this, SLOT(ClickButtonReset()));

	SetSpeed = new QLabel("�ٶȵ���");
	font = SetSpeed->font();//�Ӵ�����
	font.setBold(true);
	SetSpeed->setFont(font);
	SetSpeed->setMinimumHeight(25);

	SpeedAdjust = new QDoubleSpinBox(this);
	SpeedAdjust->setRange(0.1, 5);  // ��Χ
	SpeedAdjust->setSingleStep(0.1); // ����
	SpeedAdjust->setValue(1);  // ��ǰֵ
	SpeedAdjust->setWrapping(true);  // ����ѭ��
	SpeedAdjust->setDecimals(1);  // ����
	connect(SpeedAdjust, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &AnimationWidget::ClickButtonSpeedAdjust);

	//2.������ʾ��
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
	//outputtreeview->setEditTriggers(QTreeView::NoEditTriggers);			//��Ԫ���ܱ༭
	outputtreeview->setSelectionBehavior(QTreeView::SelectRows);			//һ��ѡ������
	outputtreeview->setSelectionMode(QTreeView::SingleSelection);          //��ѡ�������������о���һ��ѡ����
	//outputtreeview->setAlternatingRowColors(true);                       //ÿ���һ����ɫ��һ��������qssʱ��������Ч
	//outputtreeview->setFocusPolicy(Qt::NoFocus);                         //ȥ������Ƶ���Ԫ����ʱ�����߿�
	//2.��ͷ�������
	//outputtreeview->header()->hide();                                  //������ͷ
	outputtreeview->header()->setStretchLastSection(true);               //���һ������Ӧ���
	outputtreeview->header()->setDefaultAlignment(Qt::AlignCenter);      //��ͷ����Ĭ�Ͼ��ж���

	//CreateOutputModel();//����outputtreemodel
	outputtreemodel = new outputTreeModel(nullptr);
	outputtreeview->setModel(outputtreemodel);
	outputtreeview->setItemDelegateForColumn(2, new CheckBoxDelegate(outputtreeview));
	//outputtreeview->expandAll();

	//�źŲۣ��Ҽ��˵�
	outputtreeview->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(outputtreeview, &QTreeView::customContextMenuRequested, this, &AnimationWidget::slotTreeMenu);

	//3.������ʾ��
	ShowParameter = new QLabel("������ʾ��");
	font = ShowParameter->font();//�Ӵ�����
	font.setBold(true);
	ShowParameter->setFont(font);
	ShowParameter->setMinimumHeight(25);

	PushButtonShowAllPara = new QPushButton(this);
	PushButtonShowAllPara->setText("��ʾ���в���");
	PushButtonShowAllPara->setFixedSize(125, 25);
	connect(PushButtonShowAllPara, SIGNAL(clicked()), this, SLOT(ClickButtonShowAllPara()));

	PushButtonShowAttenPara = new QPushButton(this);
	PushButtonShowAttenPara->setText("��ʾ��ע����");
	PushButtonShowAttenPara->setFixedSize(125, 25);
	connect(PushButtonShowAttenPara, SIGNAL(clicked()), this, SLOT(ClickButtonShowAttenPara()));

	PushButtonSavePara = new QPushButton(this);
	PushButtonSavePara->setText("�����޸Ĳ���");
	PushButtonSavePara->setFixedSize(125, 25);
	connect(PushButtonSavePara, SIGNAL(clicked()), this, SLOT(ClickButtonSavePara()));

	PushButtonResimulate = new QPushButton(this);
	PushButtonResimulate->setText("���·���");
	PushButtonResimulate->setFixedSize(125, 25);
	connect(PushButtonResimulate, SIGNAL(clicked()), this, SLOT(ClickButtonResimulate()));

	//����view
	paratreeview = new QTreeView(this);
	paratreeview->setSelectionBehavior(QTreeView::SelectRows);			//һ��ѡ������
	paratreeview->setSelectionMode(QTreeView::SingleSelection);         //��ѡ�������������о���һ��ѡ����
	paratreeview->header()->setStretchLastSection(true);                //���һ������Ӧ���
	paratreeview->header()->setDefaultAlignment(Qt::AlignCenter);       //��ͷ����Ĭ�Ͼ��ж���
	//����paratreemodel
	paratreemodel = new QStandardItemModel(this);
	paratreeview->setModel(paratreemodel);
	PushButtonSavePara->setFocus();
	PushButtonSavePara->setDefault(true);
	paratreeview->installEventFilter(this);                             //��������Զ�������eventFilter����
	//�źŲۣ��Ҽ��˵�
	paratreeview->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(paratreeview, &QTreeView::customContextMenuRequested, this, &AnimationWidget::slotParaTreeMenu);

	//4.������Ϣ��ʾ��
	ShowMessage = new QLabel("������Ϣ��ʾ");
	font = ShowMessage->font();//�Ӵ�����
	font.setBold(true);
	ShowMessage->setFont(font);
	ShowMessage->setMinimumHeight(25);

	AnimationMessage = new QTextEdit(this);
	AnimationMessage->setText("������Ϣ����...\n");
	//AnimationMessage->setMaximumHeight(100);

	//5.������Ϣͬ��������
	PushButtonSendSimData = new QPushButton(this);
	PushButtonSendSimData->setText("������Ϣͬ������");
	PushButtonSendSimData->setFixedSize(400, 25);
	connect(PushButtonSendSimData, SIGNAL(clicked()), this, SLOT(ClickButtonSendSimData()));

	//����
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
		//display = "������Ϣ��\n" + message[2] + "\n" + "�˴η����ѽ���\n";
		display = "������Ϣ��\n�˴η����ѽ���\n";
		string SolverName = LibPathsSetDlg::GetSolverName();
		if (SolverName == "MWorks")
		{
			m_pCompRoot = ((DocumentModel*)m_doc)->GetComponent();
			string sWorkDir = LibPathsSetDlg::GetWorkDir();
			if (sWorkDir.back() != '/')
			{
				sWorkDir += "/";
			}
			string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//�Բ�ͬģ���в�ͬ�ļ��е����·��
			string RecordFilePath = saveCompareRaw + "RecordOutput.txt";
			char* fn = (char*)RecordFilePath.c_str();//���������VS����Ҫ����ǿ��ת��(char*)"a.txt"
			string str;
			str = readFileIntoString(fn);
			display += QString::fromStdString(str);
		}
	}
	else
	{
		if (message[0] == "")
		{
			display = "�����Ϣ��\n" + message[1];
		}
		else if (message[1] == "")
		{
			display = "������Ϣ��\n" + message[0];
		}
		else
		{
			display = "������Ϣ��\n" + message[0] + "�����Ϣ��\n" + message[1];
		}
	}
	MessageDisplay += display;
	AnimationMessage->setText(MessageDisplay);
	//���ù��ʹ������ʼ�����ڵײ�
	QTextCursor textCursor = AnimationMessage->textCursor();
	textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	AnimationMessage->setTextCursor(textCursor);
}

//��ʾ������Ϣ
void AnimationWidget::ShowPostMessage(bool isSimulate, bool isSimulateSuccess)
{
	if (isSimulate == false) {
		//���û�н��з��棬��ʾ��Ϣ�����������£��Զ����ع�ע�����͹�ע����
		AnimationMessage->setText("-------------��������������-------------");
		ClickButtonOnlyShow();
		ClickButtonShowAttenPara();
	}else {
		if (isSimulateSuccess == true) {
			MessageDisplay += "\n-------------����ɹ�-------------\n";
			AnimationMessage->setText(MessageDisplay);
			ClickButtonOnlyShow();
			ClickButtonShowAttenPara();
		}
		else {
			MessageDisplay += "\n-------------����ʧ��-------------\n";
			AnimationMessage->setText(MessageDisplay);
			outputtreeview->setModel(nullptr);
			paratreeview->setModel(nullptr);
		}
	}

}

//�����Ϣ��
void AnimationWidget::ClearMessage()
{
	MessageDisplay = "";
	AnimationMessage->setText("-------------��������У����Ժ�-------------");
	outputtreeview->setModel(nullptr);
	paratreeview->setModel(nullptr);
}

//���ļ����뵽string��
string AnimationWidget::readFileIntoString(char* filename)
{
	ifstream ifile(filename);
	//���ļ����뵽ostringstream����buf��
	ostringstream buf;
	char ch;
	while (buf && ifile.get(ch))
		buf.put(ch);
	//������������buf�������ַ���
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
	//����ڶ���ִ�й����У����˳�
	//���������߳�
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
	//string ReminderMessage = "�����ٶȱ���Ϊ" + to_string(speed);
	//QMessageBox::information(NULL, "��ʾ", QString::fromStdString(ReminderMessage), QMessageBox::Ok);
}

double AnimationWidget::GetAnimationSpeed()
{
	return speed;
}

bool AnimationWidget::readCyberInfo(CyberInfo* pCyberInfo)
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

void AnimationWidget::CreateOutputModel()
{
	////����model
	//outputtreemodel = new QStandardItemModel(this);
	//outputtreemodel->setHorizontalHeaderLabels(QStringList() << QString("��������") << QString("�Ƿ��ע"));     //������ͷ
	//QString m_sCSVFilePath = "D:/openmodelica/bin/Modelica.Mechanics.MultiBody.Examples.Elementary.Pendulum_res.csv";
	//QStringList AllVariablesNames = FileParse::GetAllVariablesNames(m_sCSVFilePath);
	////����ȡ���ı�����
	////���Ȼ������FirstName
	//vector<QString> AllFirstName;//����
	//set<QString> SetAllFirstName;//����
	//for (int i = 0; i < AllVariablesNames.size(); i++)
	//{
	//	QString FullName = AllVariablesNames.at(i);
	//	//����������ַ��������Ǽ����飬��Щ���� sep �ָ���start �� end ָ�����
	//	//end Ĭ��Ϊ �C1 �����ص���[ start, end ]�ڵĿ���ɵ��ַ���
	//	QString FirstName = FullName.section(QRegExp("[.(]"), 0, 0);
	//	//QString LastName = FullName.section('.', 1, -1);
	//	SetAllFirstName.insert(FirstName);
	//}
	//AllFirstName.assign(SetAllFirstName.begin(), SetAllFirstName.end());//������ת��Ϊ����
	////Ȼ���LastName�����ά����
	//vector<vector<QString>> AllLastName;  //������һ����ά����
	//AllLastName.resize(AllFirstName.size());  //�ȸ�����Ҫָ������С
	//for (int i = 0; i < AllVariablesNames.size(); i++)
	//{
	//	QString FullName = AllVariablesNames.at(i);
	//	//����������ַ��������Ǽ����飬��Щ���� sep �ָ���start �� end ָ�����
	//	//end Ĭ��Ϊ �C1 �����ص���[ start, end ]�ڵĿ���ɵ��ַ���
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
	////�����modeltree
	//for (int i = 0; i < AllFirstName.size(); i++)
	//{
	//	//һ���ڵ㣬����mModel
	//	QList<QStandardItem*> items1;
	//	QStandardItem* item1 = new QStandardItem(AllFirstName[i]);
	//	//QStandardItem* item2 = new QStandardItem(QString("��ע"));
	//	items1.append(item1);
	//	//items1.append(item2);
	//	outputtreemodel->appendRow(items1);
	//	//�����ڵ㣬����һ���ڵ�
	//	for (int j = 0; j < AllLastName[i].size(); j++)
	//	{
	//		QList<QStandardItem*> items2;
	//		QStandardItem* item3 = new QStandardItem(AllLastName[i][j]);
	//		QStandardItem* item4 = new QStandardItem(QString("��ע"));
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
	outputtreeview->setEditTriggers(QAbstractItemView::DoubleClicked);  //˫���ɱ༭
}

void AnimationWidget::ClickButtonSave()
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
	msgBox.setText("����ɹ������ȵ��\"�������б���\"���ٵ��\"ֻ��ʾ��ע\"��");
	msgBox.exec();
}


void AnimationWidget::ClickButtonOnlyShow()
{
	//����model
	if (AttenOutputTreeModel != nullptr) {
		delete AttenOutputTreeModel;
	}
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

void AnimationWidget::OutputDFS(vector<myOutputVar*>& m_output, TreeItem* root, string s)
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

void AnimationWidget::slotTreeMenu(const QPoint& pos)
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

void AnimationWidget::MenuGeneCurves()
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
			QString message = text + "����δ��CSV�ļ��У������������ߣ�";
			msgBox.setText(message);
			msgBox.exec();
		}
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("δ�ҵ���������");
		msgBox.exec();
	}
}

void AnimationWidget::ClickButtonShowAllPara()
{
	CreateAllParaModel();
	paratreeview->setModel(Allparatreemodel);
	//paratreeview->expandAll();
	paratreeview->setEditTriggers(QAbstractItemView::NoEditTriggers);   //������Ϊ���ɱ༭
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
		msgBox.setText("��ȡʧ��");
		msgBox.exec();
		return false;
	}
	//����������
	int size = pCyberInfo->GetParameters().size();
	for (int itr = 0; itr < size; ++itr) {
		string s="";	
		//1.ȫ������
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
		
		//�����������
		s += "\t";
		string sType= pCyberInfo->GetParameters().at(itr)->sType;
		if (sType == "")sType = " ";
		s += sType;

		//2.ȱʡֵ
		s += "\t";
		//double value = m_newCyberInfo->GetParameters().at(itr)->defaultValue;
		string value = pCyberInfo->GetParameters().at(itr)->defaultValue;
		s += value;
		//ostringstream strStream;
		//strStream << value;
		//if(value!=0)s += strStream.str(); 

		//3.��λ
		s += "\t";
		s += pCyberInfo->GetParameters().at(itr)->sUnit;

		//����ά��
		string sDim = pCyberInfo->GetParameters().at(itr)->sDimension;
		if (sDim!="") {
		s += "\t";
		//string sDim = m_newCyberInfo->GetParameters().at(itr)->sDimension;
		//if (sDim == "")sDim = " ";
		s += sDim;
		}

		//4.�Ƿ��ע���ע��
		s += "\t";
		if (pCyberInfo->GetParameters().at(itr)->bFocused)
		{
			s += "��ע";
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
		//menu.setStyleSheet(qss);    //���Ը��˵�������ʽ

		QModelIndex curIndex = paratreeview->indexAt(pos);       //��ǰ�����Ԫ�ص�index
		QModelIndex index = curIndex.sibling(curIndex.row(), 0); //���еĵ�1��Ԫ�ص�index
		if (index.isValid())
		{
			//�ɻ�ȡԪ�ص��ı���data,���������жϴ���
			//QStandardItem* item = AttenOutputTreeModel->itemFromIndex(index);
			//QString text = item->text();
			//QVariant data = item->data(Qt::UserRole + 1);
			//...

			//���һ�в˵�
			menu.addAction(QString("��������ӵ���ע"), this, SLOT(AddParaAtten()));
			//menu.addSeparator();    //���һ���ָ���
			//menu.addAction(QString("�۵�"), this, SLOT(slotTreeMenuCollapse(bool)));
		}
		menu.exec(QCursor::pos());  //��ʾ�˵�
	}
	else if (paratreeview->model() == paratreemodel)
	{
		QString qss = "QMenu{color:#E8E8E8;background:#4D4D4D;margin:2px;}\
                QMenu::item{padding:3px 20px 3px 20px;}\
                QMenu::indicator{width:13px;height:13px;}\
                QMenu::item:selected{color:#E8E8E8;border:0px solid #575757;background:#1E90FF;}\
                QMenu::separator{height:1px;background:#757575;}";

		QMenu menu;
		//menu.setStyleSheet(qss);    //���Ը��˵�������ʽ

		QModelIndex curIndex = paratreeview->indexAt(pos);       //��ǰ�����Ԫ�ص�index
		QModelIndex index = curIndex.sibling(curIndex.row(), 0); //���еĵ�1��Ԫ�ص�index
		if (index.isValid())
		{
			//�ɻ�ȡԪ�ص��ı���data,���������жϴ���
			//QStandardItem* item = AttenOutputTreeModel->itemFromIndex(index);
			//QString text = item->text();
			//QVariant data = item->data(Qt::UserRole + 1);
			//...

			//���һ�в˵�
			menu.addAction(QString("ȡ����ע"), this, SLOT(CancelParaAtten()));
			//menu.addSeparator();    //���һ���ָ���
			//menu.addAction(QString("�۵�"), this, SLOT(slotTreeMenuCollapse(bool)));
		}
		menu.exec(QCursor::pos());  //��ʾ�˵�
	}
}

void AnimationWidget::AddParaAtten()
{
	//ͨ��������ò���ȫ�����������͡�ȱʡֵ
	QModelIndex curIndex = paratreeview->currentIndex();
	QModelIndex index = curIndex.sibling(curIndex.row(), 0); //ͬһ�е�һ��Ԫ�ص�index
	string name;           //����ȫ��
	string ParaType;       //��������
	QString qs_ParaType;
	string ParaValue;      //����ȱʡֵ
	QString qs_ParaValue;
	QString text;
	if (index.isValid())
	{
		//virtual QMap<int, QVariant> itemData(const QModelIndex & index) const;
		QMap<int, QVariant> item = Allparatreemodel->itemData(index);
		QString LastName = item[0].toString();
		QModelIndex index2 = curIndex.sibling(curIndex.row(), 1); //ͬһ�еڶ���Ԫ�ص�index
		QMap<int, QVariant> item2 = Allparatreemodel->itemData(index2);
		qs_ParaType = item2[0].toString();
		QModelIndex index3 = curIndex.sibling(curIndex.row(), 2); //ͬһ�е�����Ԫ�ص�index
		QMap<int, QVariant> item3 = Allparatreemodel->itemData(index3);
		qs_ParaValue = item3[0].toString();
		//Q_INVOKABLE virtual QModelIndex parent(const QModelIndex & child) const = 0;
		//��Ѱ�Ҹ��ڵ㣬��ñ���ȫ��
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

	//����������DocumentModel��ParameterValues�У��Ѿ����ڵĲ��ӣ�������M3dCom�Ĳ���
	//vec��ȡ�Ѿ����ĵĹ�ע����
	auto vec = ((DocumentModel*)m_doc)->GetAllParameterValues();
	set<string> sPara_sMapParamName;//DocumentModel�в�����sMapParamName
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

		//ͬʱҲҪ����Ӧ�����cyberinfo��Ϊ��ע������װ������ʾ����ʱ���ܻ��������
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
			msgBox.setText("δ�ҵ���Ӧ��CyberInfo!");
			msgBox.exec();
			return;
		}
		myParameter* mypara = childCompCyIn->GetParameter(QFullName.section('.', 1).toStdString().c_str());
		if (mypara == nullptr)
		{
			QMessageBox msgBox;
			msgBox.setText("δ�ҵ���Ӧ�Ĳ�����!");
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
	QModelIndex index = curIndex.sibling(curIndex.row(), 0); //ͬһ�е�һ��Ԫ�ص�index
	string name;
	QString Q_name;
	if (index.isValid())
	{
		QStandardItem* item = paratreemodel->itemFromIndex(index);
		Q_name = item->text();
		name = Q_name.toStdString();
	}

	//ɾ��DocumentModel->ParameterValues�еĲ���
	auto vec = ((DocumentModel*)m_doc)->GetAllParameterValues();
	set<string> sPara_sMapParamName;//DocumentModel�в�����sMapParamName
	for (vector<ParameterValue>::iterator iter = vec.begin(); iter != vec.end(); ++iter)
	{
		if (iter->sMapParamName == name)
		{
			vec.erase(iter);
			break;
		}
	}
	((DocumentModel*)m_doc)->SetParameterValues(vec);

	//�������CyberInfo�еĲ�����Ϊ����ע
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
		msgBox.setText("δ�ҵ���Ӧ��CyberInfo!");
		msgBox.exec();
		return;
	}
	myParameter* mypara = childCompCyIn->GetParameter(Q_name.section('.', 1).toStdString().c_str());
	if (mypara == nullptr)
	{
		QMessageBox msgBox;
		msgBox.setText("δ�ҵ���Ӧ�Ĳ�����!");
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
		msgBox.setText("�ĵ���ȡʧ��");
		msgBox.exec();
		return;
	}

	//vec��ȡ�Ѿ����ĵĹ�ע����
	auto vec = ((DocumentModel*)m_doc)->GetAllParameterValues();
	set<string> sPara_sMapParamName;//DocumentModel�в�����sMapParamName
	for (vector<ParameterValue>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		sPara_sMapParamName.insert(it->sMapParamName);
	}

	//������ǰװ�����µ�һ���ӽڵ㣬��ȡ��ע������ȱʡֵ
	vector<ParameterValue> vPVs;
	//������ǰװ�����µ�һ���ӽڵ㣬��ȡ��ע��������m_data
	for (Component* child : cur->GetChilds())
	{
		string scompname = child->GetCompName();
		if (child->IsAssembly())
		{
			//װ���壬����ParameterValue�Ĳ���
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
				msgBox.setText("CyberInfo������");
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

	//���Ѿ���ӹ�ע�����ǲ���DocumentModel��Ĺ�ע��������vec��
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

	//�������������½�����������ֵ
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

	//����model
	paratreemodel = new QStandardItemModel(this);
	paratreemodel->setHorizontalHeaderLabels(QStringList() << QString("�������") << QString("�½�����") << QString("����ֵ"));     //������ͷ

	//�����modeltree																													 
	for (int i = 0; i < vec_MapParamName.size(); i++)
	{
		//һ���ڵ㣬����mModel
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
	//��������ֵ�������������Ϊ���ɱ༭
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
			msgBox.setText("�ĵ���ȡʧ��");
			msgBox.exec();
			return;
		}

		vector<ParameterValue> vec;

		//����ǰ��ˢ����ʾ
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
		msgBox.setText("����ɹ���");
		msgBox.exec();
	}
}

//ʵ���¼�������
bool AnimationWidget::eventFilter(QObject* target, QEvent* event)
{
	if (target == paratreeview)
	{
		//��ȡ�����ź�
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* k = static_cast<QKeyEvent*>(event);
			if (k->key() == Qt::Key_Return)          //ѡ���س���(�����ж�����������)
			{
				ClickButtonSavePara();               //���Ӳ��ź�
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
	//���û�д��������½�һ�����������Ͳ������µ���
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
	MessageDisplay += "�������ļ���*.csv��Ԥ���У����Ե�...";
	AnimationMessage->setText(MessageDisplay);
	//���ù��ʹ������ʼ�����ڵײ�
	QTextCursor textCursor = AnimationMessage->textCursor();
	textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
	AnimationMessage->setTextCursor(textCursor);
}

void AnimationWidget::MsgCSVReadFinish()
{
	MessageDisplay += "\n-------------�������ļ���*.csv����ȡ���-------------\n";
	AnimationMessage->setText(MessageDisplay);
	//���ù��ʹ������ʼ�����ڵײ�
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

	// ���ô�����󻯺���С��
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
	setWindowTitle(tr("������Ϣ����"));
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

	//1.ȫ��Ψһ��ϢID
	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//�������岼��Ϊ��ֱ����
	LocalMessageIdShow = new QLabel("ȫ��Ψһ��ϢID��");
	LocalMessageIdShow->setMinimumHeight(25);
	LocalMessageIdShow->setFixedWidth(150);

	LocalMessageIdEdit = new QLineEdit;
	LocalMessageIdEdit->setReadOnly(false); //�ɱ༭
	//LocalMachineIPEdit->setText();
	LocalMessageIdEdit->setMinimumHeight(25);
	LocalMessageIdEdit->setFixedWidth(150);

	//2.Ŀ�Ļ�ID
	DestinationMachineIdShow = new QLabel("Ŀ�Ļ���ID(;�ָ�)��");
	DestinationMachineIdShow->setMinimumHeight(25);
	DestinationMachineIdShow->setFixedWidth(150);

	DestinationMachineIdEdit = new QLineEdit;
	DestinationMachineIdEdit->setReadOnly(false); //�ɱ༭
	//LocalMachineIPEdit->setText();
	DestinationMachineIdEdit->setMinimumHeight(25);
	DestinationMachineIdEdit->setFixedWidth(150);

	//ѡ��DDS���ַ�ʽ
	ButtonRTPS = new QRadioButton(this);
	ButtonRTPS->setText("�˵��˷���");
	ButtonDCPSInfoRepo = new QRadioButton(this);
	ButtonDCPSInfoRepo->setText("����ʽ����");
	groupButtonDiscoveryMethod = new QButtonGroup(this);
	groupButtonDiscoveryMethod->addButton(ButtonRTPS, 0);//idΪ0
	groupButtonDiscoveryMethod->addButton(ButtonDCPSInfoRepo, 1);//idΪ1
	groupButtonDiscoveryMethod->button(MethodID)->setChecked(true); //Ĭ��ѡ�ж˵��˷���

	//ѡ�����ֶ�
	SelectSendVars = new QLabel("ѡ�����ֶ�");
	font = SelectSendVars->font();//�Ӵ�����
	font.setBold(true);
	SelectSendVars->setFont(font);
	SelectSendVars->setMinimumHeight(25);

	PushButtonSelectAll = new QPushButton(this);
	PushButtonSelectAll->setText("ȫѡ");
	PushButtonSelectAll->setFixedSize(100, 25);
	connect(PushButtonSelectAll, SIGNAL(clicked()), this, SLOT(ClickButtonSelectAll()));

	PushButtonSelectNone = new QPushButton(this);
	PushButtonSelectNone->setText("ȡ��ȫѡ");
	PushButtonSelectNone->setFixedSize(100, 25);
	connect(PushButtonSelectNone, SIGNAL(clicked()), this, SLOT(ClickButtonSelectNone()));

	PushButtonShowSelected = new QPushButton(this);
	PushButtonShowSelected->setText("չʾ��ѡ");
	PushButtonShowSelected->setFixedSize(100, 25);
	connect(PushButtonShowSelected, SIGNAL(clicked()), this, SLOT(ClickButtonShowSelected()));


	//����view
	sendvarstreeview = new QTreeView(this);
	//1.QTreeView����������
	sendvarstreeview->setSelectionBehavior(QTreeView::SelectRows);		   //һ��ѡ������
	sendvarstreeview->setSelectionMode(QTreeView::SingleSelection);        //��ѡ�������������о���һ��ѡ����
	sendvarstreeview->header()->setStretchLastSection(true);               //���һ������Ӧ���
	sendvarstreeview->header()->setDefaultAlignment(Qt::AlignCenter);      //��ͷ����Ĭ�Ͼ��ж���
	//����sendvarstreemodel
	sendvarstreemodel = new QStandardItemModel(this);
	//���ñ�ͷ
	sendvarstreemodel->setHorizontalHeaderLabels(QStringList() << "�����ֶ�����" << "�Ƿ���");
	CreateSendVarsModel();
	//sendvarstreeview->setModel(sendvarstreemodel);
	//sendvarstreeview->setItemDelegateForColumn(1, new CheckBoxDelegateIsSend(sendvarstreeview));

	//3.��ʼ��ƥ�䰴ť
	PushButtonInitMatch = new QPushButton(this);
	PushButtonInitMatch->setText("��ʼ��ƥ��");
	PushButtonInitMatch->setFixedSize(400, 25);
	connect(PushButtonInitMatch, SIGNAL(clicked()), this, SLOT(ClickButtonInitMatch()));

	//4.����ƥ�䰴ť
	PushButtonCloseMatch = new QPushButton(this);
	PushButtonCloseMatch->setText("�Ͽ�ƥ��");
	PushButtonCloseMatch->setFixedSize(400, 25);
	connect(PushButtonCloseMatch, SIGNAL(clicked()), this, SLOT(ClickButtonCloseMatch()));

	//5.������Ϣ����ʾ��
	ShowSendMessage = new QLabel("�ѷ�����Ϣ");
	font = ShowSendMessage->font();//�Ӵ�����
	font.setBold(true);
	ShowSendMessage->setFont(font);
	ShowSendMessage->setMinimumHeight(25);

	SendNowMessages = new QTextEdit(this);
	SendNowMessages->setText("�ѷ�����Ϣ����...\n");




	//����
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(LocalMessageIdShow);
	HorinztalLayout1->addWidget(LocalMessageIdEdit);
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(this);
	HorinztalLayout2->addWidget(DestinationMachineIdShow);
	HorinztalLayout2->addWidget(DestinationMachineIdEdit);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->setMargin(0);//һ��Ҫ��
	HorinztalLayout3->addWidget(ButtonRTPS);
	HorinztalLayout3->setAlignment(ButtonRTPS, Qt::AlignCenter);//�ؼ��ڲ����о�����ʾ
	HorinztalLayout3->addWidget(ButtonDCPSInfoRepo);
	HorinztalLayout3->setAlignment(ButtonDCPSInfoRepo, Qt::AlignCenter);//�ؼ��ڲ����о�����ʾ
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
	//�����modeltree	
	QStringList VariablesNames = FileParse::GetAllVariablesNames(m_sCSVFilePath);
	for (int i = 0; i < VariablesNames.size(); i++)
	{
		QString variablesname = VariablesNames.at(i);
		//һ���ڵ㣬����mModel
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
	//���������������Ϊ���ɱ༭
	sendvarstreeview->setItemDelegateForColumn(0, new DisableEditor(sendvarstreeview));
	sendvarstreeview->setItemDelegateForColumn(1, new CheckBoxDelegateIsSend(sendvarstreeview));
	sendvarstreeview->expandAll();
}


void SimulInfoSendDlg::ClickButtonSelectAll()
{
	for (int i = 0; i < sendvarstreemodel->rowCount(); i++)
	{
		QStandardItem* itemIsSend = new QStandardItem("��");
		sendvarstreemodel->setItem(i, 1, itemIsSend);
	}
	sendvarstreeview->setModel(sendvarstreemodel);
}


void SimulInfoSendDlg::ClickButtonSelectNone()
{
	for (int i = 0; i < sendvarstreemodel->rowCount(); i++)
	{
		QStandardItem* itemIsSend = new QStandardItem("��");
		sendvarstreemodel->setItem(i, 1, itemIsSend);
	}
	sendvarstreeview->setModel(sendvarstreemodel);
}


void SimulInfoSendDlg::ClickButtonShowSelected()
{
	if (PushButtonShowSelected->text() == "չʾ��ѡ")
	{
		if (selectedtreemodel != nullptr) {
			delete selectedtreemodel;
		}
		selectedtreemodel = new QStandardItemModel(this);
		//���ñ�ͷ
		selectedtreemodel->setHorizontalHeaderLabels(QStringList() << "��ѡ�����ֶ�");
		for (int i = 0; i < sendvarstreemodel->rowCount(); i++) {
			QStandardItem* item1 = sendvarstreemodel->item(i, 1);
			QString vartype = item1->text();
			if (vartype == "��")
			{
				QStandardItem* item0 = sendvarstreemodel->item(i, 0);
				QString varname = item0->text();
				//һ���ڵ㣬����mModel
				QList<QStandardItem*> items1;
				QStandardItem* item1 = new QStandardItem(varname);
				items1.append(item1);
				selectedtreemodel->appendRow(items1);
			}
		}
		sendvarstreeview->setModel(selectedtreemodel);
		PushButtonShowSelected->setText("չʾ����");
	}
	else {
		sendvarstreeview->setModel(sendvarstreemodel);
		PushButtonShowSelected->setText("չʾ��ѡ");
	}
}


//��̨��������ƥ��ĺ���
void SimulInfoSendDlg::ClickButtonInitMatch()
{
	if (initMatch == true)
	{
		string errormsg = "�Ѿ���ʼ���������ظ���ʼ����";
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
			if (vartype == "��")
			{
				QStandardItem* item0 = sendvarstreemodel->item(i, 0);
				string varname = item0->text().toStdString();
				sendVars.push_back(varname);
			}
		}

		if (sendVars.size() == 0) {
			QString errormsg = "δѡ�����ֶΣ�������ѡ��һ�������ֶκ����³�ʼ����";
			QMessageBox msgBox;
			msgBox.setText(errormsg);
			msgBox.exec();
			ClearXDataMembers();
			return;
		}

		XData data;
		data.time = -1;
		data.vec_Data = sendVars;

		//���������һ�䣬�������Ʒ�����ȥ�������ԭ���ȼ�����仰��˵
		Sleep(1000);
		
		if (SendXData(LocalMessageIdEdit->text().toStdString(),DestinationMachineIdEdit->text().toStdString(), data) == 0)
		{
			string sendedmsg = GetSendedMsg();
			SendedMessages += "ƥ��ɹ�!���������ѷ���!\n";
			SendedMessages += QString::fromStdString(sendedmsg);
			SendNowMessages->setText(SendedMessages);
			//���ù��ʹ������ʼ�����ڵײ�
			QTextCursor textCursor = SendNowMessages->textCursor();
			textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
			SendNowMessages->setTextCursor(textCursor);
			issending = true;//������״̬��Ϊtrue
			initMatch = true;
			
			//QMessageBox msgBox;
			//msgBox.setText("ƥ��ɹ�!���������ѷ���!");
			//msgBox.exec();
			return;
		}
		else
		{
			QMessageBox msgBox;
			msgBox.setText("ƥ��ʧ��!����CSV�ļ��Ƿ�ɹ�����!");
			msgBox.exec();
			ClearXDataMembers();
			return;
		}
	}
	else
	{
		string errormsg = "��ʼ��ʧ��!������룺" + std::to_string(code);
		QMessageBox msgBox;
		msgBox.setText(QString::fromStdString(errormsg));
		msgBox.exec();
		ClearXDataMembers();
		return;
	}
	return;
}


//���ͷ�����Ϣ�ĺ���
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
		//��һ����ע�͵�����Ϊ��ʾ�ѷ��͵���Ϣ�ȽϺ�ʱ�����п��ٵ�����
		string sendedmsg = GetSendedMsg();
		SendedMessages += QString::fromStdString(sendedmsg);
		SendNowMessages->setText(SendedMessages);
		//���ù��ʹ������ʼ�����ڵײ�
		QTextCursor textCursor = SendNowMessages->textCursor();
		textCursor.movePosition(QTextCursor::End, QTextCursor::MoveAnchor);
		SendNowMessages->setTextCursor(textCursor);

		//QMessageBox msgBox;
		//msgBox.setText("�����ѷ���!");
		//msgBox.exec();

		return;
	}
	else
	{
		QMessageBox msgBox;
		msgBox.setText("���ݷ���ʧ�ܣ�");
		msgBox.exec();
		return;
	}

}

//��̨�����Ͽ�ƥ��ĺ���
void SimulInfoSendDlg::ClickButtonCloseMatch()
{
	if (ClearXDataMembers() == 0)
	{
		QMessageBox msgBox;
		msgBox.setText("�����ѶϿ���ʵ��������!");
		msgBox.exec();
		issending = false;
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

//���������ֶ������б�
QWidget* CheckBoxDelegateIsSend::createEditor(QWidget* parent,
	const QStyleOptionViewItem& /* option */,
	const QModelIndex& index) const
{
	QComboBox* editor = new QComboBox(parent);
	editor->addItem("��");
	editor->addItem("��");

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


//����չʾ�Ի���
CurveDisplayDlg::CurveDisplayDlg(string VariableName, vector<int> ColumnCount, AnimationWidget* parent) :m_parent(parent), QDialog(parent)
{
	m_sCSVFilePath = m_parent->GetCSVFilePath();
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

void CurveDisplayDlg::CurveDisplayDlgInit(string VariableName, vector<int> ColumnCount)
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

	//������ʾ
	chartview = new QChartView();
	//����������ȡ���е����Ϣ
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
	//���ñ�ͷ
	chart = new Chart(this, AllVariablesNames[ColumnCount[0]]);
	//��������ϵ
	double max_value = *max_element(paraSeries.begin(), paraSeries.end());
	double min_value = *min_element(paraSeries.begin(), paraSeries.end());
	double range = (max_value - min_value) / 10;
	if (range == 0) { range = 1; }                          //�������ֵΪ��������Ӧ����range�ķ�Χ
	chart->setAxis("time", 0, timeSeries[timeSeries.size() - 1], 11, AllVariablesNames[ColumnCount[0]], min_value - range, max_value + range, 11);
	//������ɢ������
	QList<QPointF> pointlist;
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
}


CurveDisplayDlg::~CurveDisplayDlg()
{

}

void CurveDisplayDlg::ParaChanged()
{
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
	//���ñ�ͷ
	chart = new Chart(this, ParaName);
	//��������ϵ
	double max_value = *max_element(paraSeries.begin(), paraSeries.end());
	double min_value = *min_element(paraSeries.begin(), paraSeries.end());
	double range = (max_value - min_value) / 10;
	if (range <= 0.000001) { range = 1; }
	chart->setAxis("time", 0, timeSeries[timeSeries.size() - 1], 11, ParaName, min_value - range, max_value + range, 11);
	//������ɢ������
	QList<QPointF> pointlist;
	for (int i = 0; i < timeSeries.size(); i++)
	{
		pointlist.append(QPointF(timeSeries[i], paraSeries[i]));
	}
	//����
	chart->buildChart(pointlist);
	chartview->setChart(chart->qchart);
}




//Chart��ʵ��
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

void Chart::buildChart(QList<QPointF> pointlist)
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