#include "CyberInfoDockWidget.h"
#include <QFileDialog>
#include <QFile>
#include "DocumentComponent.h"
#include <QLineEdit>
#include "moTreeDlg.h"
#include "ModelicaModel.h"

const int LABEL_LISTS_SIZE = 1;
const int WIDGET_LISTS_SIZE = 3;
static CyberInfo* m_CyberInfo;
bool CyberInfoDockWidget::m_closed = false;
using namespace std;

CyberInfoDockWidget::CyberInfoDockWidget(MainWindow* mywidget,QAction* sender)
	: QDockWidget(mywidget),m_sender(sender),
	MoFilename("name")
{
	//��ʼ��
	m_CyberInfo = new CyberInfo(nullptr);
	//m_closed =false;
	paratreeview = new QTreeView(mywidget);
	outputtreeview = new QTreeView(mywidget);
	m_cons_table = new QTableWidget(this);
	dock1 = new QDockWidget(mywidget);
	MdlName = new QLineEdit();
	for (int i = 0; i < LABEL_LISTS_SIZE; i++)
	{
		LabLists.append(new QLabel());
	}
	MdlName->setMinimumSize(200, 20);

	for (int i = 0; i < WIDGET_LISTS_SIZE; i++)
	{
		WidgLists.append(new QWidget(mywidget));
	}
	
	mywid = mywidget;
	dock1->setWindowTitle("ģ����Ҫ��Ϣ");
	//dock1->setAttribute(Qt::WA_DeleteOnClose);
	QDockWidget::connect(dock1, SIGNAL(destroyed()), this, SLOT(close()));
	// ������������ӿؼ�
	QVBoxLayout* VLayout1 = new QVBoxLayout(dock1);
	QHBoxLayout* HLayout1 = new QHBoxLayout(dock1);
	QHBoxLayout* HLayout2 = new QHBoxLayout(dock1);
	QLabel* text1 = new QLabel();	
	QPushButton* PushButton1 = new QPushButton();
	QPushButton* PushButton4 = new QPushButton(mywidget);
	QPushButton* PushButton5 = new QPushButton(mywidget);

	text1->setText("ģ��ȫ��: ");
	
	MdlName->setText("...");
	MdlName->setStyleSheet(" background-color: white ;border:2px solid red;");
	//LabLists[0]
	//MdlName->setScaledContents(true);
	//MdlName->setText("...");
	//MdlName->setStyleSheet(" background-color: white ;border:2px solid red;");
	//MdlName->setScaledContents(true);
	PushButton1->setText("ѡ��...");
	PushButton1->setMaximumSize(50, 40);
	PushButton4->setText("����");
	PushButton5->setText("�Զ�����...");
	QObject::connect(PushButton1, SIGNAL(clicked(bool)), this, SLOT(updatetable()));
	QObject::connect(PushButton4, SIGNAL(clicked(bool)), this, SLOT(saveCyberInfo()));
	QObject::connect(PushButton5, SIGNAL(clicked(bool)), this, SLOT(readCyberInfo()));

	//if (!((DocumentComponent*)mywid->getDocument())->GetCyberInfo())OpenMo();
	
	HLayout1->addWidget(PushButton4);
	HLayout1->addWidget(PushButton5);
	HLayout2->addWidget(text1);
	//HLayout2->addWidget(MdlName);
	HLayout2->addWidget(MdlName);
	HLayout2->addWidget(PushButton1);
	HLayout2->setStretchFactor(MdlName, 3);
	VLayout1->setMargin(5);
	VLayout1->addLayout(HLayout1);
	VLayout1->addLayout(HLayout2);
	WidgLists[0]->setLayout(VLayout1);
	//�����������ͱ�����
	CreateTable(mywidget);
}

//������������������
void CyberInfoDockWidget::CreateTable(MainWindow* mywidget) {

	QLabel* text3 = new QLabel();
	QLabel* text4 = new QLabel();
	QPushButton* PushButton2 = new QPushButton(mywidget);
	QPushButton* PushButton3 = new QPushButton(mywidget);
	QVBoxLayout* VerticalLayout = new QVBoxLayout(dock1);
	QHBoxLayout* HorizontalLayout1 = new QHBoxLayout(dock1);
	QHBoxLayout* HorizontalLayout2 = new QHBoxLayout(dock1);

	//�����ͼ����
	updateTable1();
	updateTable2();

	text3->setText("������Ϣ:");
	text4->setText("״̬�����б�:");
	PushButton2->setText("ȥ�����й�ע");
	PushButton3->setText("ȥ�����й�ע");
	QObject::connect(PushButton2, SIGNAL(clicked(bool)), this, SLOT(SetTableZero1()));
	QObject::connect(PushButton3, SIGNAL(clicked(bool)), this, SLOT(SetTableZero2()));
	HorizontalLayout1->addWidget(text3);
	HorizontalLayout1->addWidget(PushButton2);
	VerticalLayout->addWidget(WidgLists[0]);
	VerticalLayout->addLayout(HorizontalLayout1);
	VerticalLayout->addWidget(paratreeview);
	HorizontalLayout2->addWidget(text4);
	HorizontalLayout2->addWidget(PushButton3);
	VerticalLayout->addLayout(HorizontalLayout2);
	VerticalLayout->addWidget(outputtreeview);
	WidgLists[1]->setLayout(VerticalLayout);

	//�����ӿ���Ϣ��
	createConInfo();

	if (((DocumentComponent*)mywid->getDocument()))
	{
		CyberInfo* pCyberInfo = ((DocumentComponent*)mywid->getDocument())->GetCyberInfo();

		if (!pCyberInfo) {
			//QMessageBox msgBox;
			//msgBox.setText("��ȡʧ�ܣ����ȱ���ģ������");
			//msgBox.exec();
			return;
		}
		readCyberInfo(pCyberInfo);
		//delete pCyberInfo;
	}

	MdlName->setText(GetMdlname(MoFilename));
}

//�����ӿ���Ϣ��
void CyberInfoDockWidget::createConInfo() {
	//��ȡ�ӿ���Ϣ
	m_cons = paratreemodel->getm_cons();	
	m_cons_table->setColumnCount(2);
	m_cons_table->setRowCount(m_cons.size());
	//���ýӿ���Ϣ��
	QStringList header;
	header << "�ӿ�����" << "�ӿ�����";
	m_cons_table->setHorizontalHeaderLabels(header);

	updateConInfo();
	m_cons_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_cons_table->show();
	
	QVBoxLayout* VerticalLayout = new QVBoxLayout(dock1);
	VerticalLayout->addWidget(WidgLists[1]);
	VerticalLayout->addWidget(m_cons_table);
	m_cons_table->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);     //x������Ӧ���		
	VerticalLayout->setStretchFactor(WidgLists[1],3);
	VerticalLayout->setStretchFactor(m_cons_table, 1);
	WidgLists[2]->setLayout(VerticalLayout);

	dock1->setWidget(WidgLists[2]);
}

//���½ӿ���Ϣ��
void CyberInfoDockWidget::updateConInfo() {
	m_cons_table->clear();
	QStringList header;
	header<< "�ӿ�����" << "�ӿ�����";
	m_cons_table->setHorizontalHeaderLabels(header);
	m_cons_table->setRowCount(m_cons.size());
	for (int i = 0; i < m_cons.size(); i++) {
		m_cons_table->setItem(i, 1, new QTableWidgetItem(m_cons[i].first));
		m_cons_table->setItem(i, 0, new QTableWidgetItem(m_cons[i].second));
	}
}

//����������
void CyberInfoDockWidget::updateTable1() {
	//QString filename = MoFilename;
	//if(!((DocumentComponent*)mywid->getDocument())->GetCyberInfo())
	//filename.replace(MoFilename.lastIndexOf('/')+1,MoFilename.size()- MoFilename.lastIndexOf('/')-1,"ParaInfo.txt");
	//QFile file(filename);
	//file.open(QIODevice::ReadOnly);
	
	paratreemodel=new ListTreeModel(""/*file.readAll()*/);
	//file.close();

	paratreeview->setModel(paratreemodel);
	paratreeview->setEditTriggers(QAbstractItemView::DoubleClicked);
	paratreeview->setItemDelegateForColumn(bFocusedcol,new CheckBoxDelegate(paratreeview));
	paratreeview->expandAll();
}

//���������
void CyberInfoDockWidget::updateTable2() {
	//QString filename = MoFilename;
	//if (!((DocumentComponent*)mywid->getDocument())->GetCyberInfo())
	//filename.replace(MoFilename.lastIndexOf('/') + 1, MoFilename.size() - MoFilename.lastIndexOf('/') - 1, "OutputInfo.txt");
	//QFile file(filename);
	//file.open(QIODevice::ReadOnly);
	outputtreemodel = new outputTreeModel(""/*file.readAll()*/);
	//file.close();

	outputtreeview->setModel(outputtreemodel);
	outputtreeview->setItemDelegateForColumn(2, new CheckBoxDelegate(outputtreeview));
	outputtreeview->expandAll();
}
CyberInfoDockWidget::~CyberInfoDockWidget() 
{
	close();
	delete m_CyberInfo;
}

//��MOģ���ļ�
void CyberInfoDockWidget::OpenMo() {
	//MoFilename = QFileDialog::getOpenFileName(mywid, tr("Select a .Mo file"), QString(), tr("Modelica Files(*.Mo);;All files(*.*)"));
	;
}

//����MO�ļ�
void CyberInfoDockWidget::ConnectMo() {
	OpenMo();
}

//��ղ��������й�ע
void CyberInfoDockWidget::SetTableZero1() {
	paratreemodel->ReSet(CyberInfoDockWidget::paratreemodel,bFocusedcol, sFocusNamecol);
}

//�����������й�ע
void CyberInfoDockWidget::SetTableZero2() {
	paratreemodel->ReSet(CyberInfoDockWidget::paratreemodel, 2, 3);
}

//����������
QDockWidget* CyberInfoDockWidget::getdock() {
	//dock1->setFeatures(QDockWidget::NoDockWidgetFeatures);
	return dock1;
}

//�ж��Ƿ�Ϊmo�ļ�
bool CyberInfoDockWidget::IsMOFile(QString sMoFilePath)
{
	QString MoFile = sMoFilePath;
	MoFile.append("/package.mo");    
	if (QFile::exists(MoFile)) {

		return true;
	}
	else {

		return false;
	}

}
//��ȡmoģ��ȫ��
QString CyberInfoDockWidget::GetMdlname(QString sMoFilePath)
{
	stack<QString> sPack;
	QString ModelName;
	QString sPackName = sMoFilePath;
	int pos = sPackName.lastIndexOf('/');
	//ASSERT(pos > 0);
	QString sMdlName = sPackName.right(sPackName.length() - pos - 1);
	sPackName = sPackName.left(pos);
	sPack.push(sMdlName);
	while (IsMOFile(sPackName)) {
		pos = sPackName.lastIndexOf('/');
		sMdlName = sPackName.right(sPackName.length() - pos - 1);
		sPackName = sPackName.left(pos);
		sPack.push(sMdlName);
	}
	while (!sPack.empty()) {
		ModelName += sPack.top() + '.';
		sPack.pop();
	}
	int fix = ModelName.indexOf('.', -1);
	ModelName = ModelName.left(fix);
	return ModelName;
}

//ѡ��ģ�ͣ���ò���ʾ��Ϣ
void CyberInfoDockWidget::updatetable()
{
	MoTreeDlg moTreedlg(MoFilename, this);
	moTreedlg.exec();
	if (!moTreedlg.bAccepted)
	{
		return;
	}

	CyberInfo* theCyberInfo = moTreedlg.GetCyberInfo();
	readCyberInfo(theCyberInfo);
	//delete theCyberInfo;
	
	return;

	//updateTable1();
	//updateTable2();

	////MoFilename = filename;
	//MdlName->setText(theCyberInfo->GetMdlName().c_str()/*GetMdlname(MoFilename)*/);

	//m_cons = paratreemodel->getm_cons();
	//updateConInfo();
}
//����CyberInfo���ĵ�
void CyberInfoDockWidget::saveCyberInfo()
{
	if (MdlName->text().isEmpty()/*paratreemodel->empty()*/) {
		QMessageBox msgBox;
		msgBox.setText("����ʧ�ܣ����ȹ���MOģ��");
		msgBox.exec();
		return;
	}
	
	//1.ģ����
	//m_CyberInfo->SetMdlName(GetMdlname(MoFilename).toStdString());
	m_CyberInfo->SetMdlName(MdlName->text().toStdString());
	ModelicaModel* pModel = ModelicaModel::LookupModel(m_CyberInfo->GetMdlName());
	if (pModel)
	{
		m_CyberInfo->SetMdlType(pModel->m_sType.c_str());
	}
	//2.ģ�Ͳ���
	vector<myParameter*> m_para;
	TreeItem* root = paratreemodel->getrootItem();
	for (int itr = 0; itr < root->childCount(); itr++)
		ParaDFS(m_para, root->child(itr));
	m_CyberInfo->SetParameters(m_para);

	//3.ģ���������
	vector<myOutputVar*> m_output;
	 root = outputtreemodel->getrootItem();
	for (int itr = 0; itr < root->childCount(); itr++)
		OutputDFS(m_output, root->child(itr));
	m_CyberInfo->SetOutputVars(m_output);

	//4.ģ�ͽӿ���Ϣ
	vector<string>m_connectortypes;
	vector<string>m_connectornames;

	for (int itr = 0; itr < m_cons.size(); ++itr) 
	{
		m_connectortypes.push_back(m_cons[itr].second.toStdString());
		m_connectornames.push_back(m_cons[itr].first.toStdString());
	}
		
	m_CyberInfo->SetConncTypes(m_connectortypes);
	m_CyberInfo->SetConncNames(m_connectornames);

	((DocumentComponent*)mywid->getDocument())->SetCyberInfo(m_CyberInfo);
	
	//QMessageBox msgBox;
	//msgBox.setText("����ɹ�");
	//msgBox.exec();
}

void CyberInfoDockWidget::ParaDFS(vector<myParameter*>& m_para,TreeItem* root,string s ) {
	//	
	if (s != "") s += ".";
	string Paraname = (root->data(FullNamecol)).toString().toStdString();
	s += Paraname;
	myParameter* temp=new myParameter();
	temp->sFullName = s;
	temp->sType = root->data(sTypecol).toString().toStdString();
	temp->defaultValue = root->data(defaultValcol).toString().toStdString();
	temp->sUnit = root->data(sUnitcol).toString().toStdString();
	temp->sType = root->data(sTypecol).toString().toStdString();
	//ά�ȣ�����
	//temp->sDimension = root->data(sDimCol).toString().toStdString();
	if (root->data(bFocusedcol) == "��") {
		temp->bFocused = 1;
		/*DocumentComponent* m_doc = (DocumentComponent*)mywid->getDocument();
		M3dMdl_DS* m_DS = dynamic_cast<M3dMdl_DS*>(m_doc->GetDS());
		QMessageBox msgBox;
		if (!m_DS) {
			msgBox.setText("M3dMdl_DS������");
			msgBox.exec();
			//return false;
		}
		ParameterValue m_para;
		if (!m_DS->GetComponent()) {
			msgBox.setText("Component������");
			msgBox.exec();
		}
		//��ע����
		m_para.sMapParamName = m_DS->GetComponent()->GetCompName()+"." + Paraname;
	
		//m_para.sParamName = Paraname;
		m_para.sParamValue = temp->defaultValue;
		m_DS->AddParameterValue(m_para);*/
	}
	else temp->bFocused = 0;
	//temp->bFocused = root->data(3).toBool();
	temp->sFocusName = root->data(sFocusNamecol).toString().toStdString();
	temp->sDimension=root->data(sDimensioncol).toString().toStdString();
	m_para.push_back(temp);
	for (int itr = 0; itr < root->childCount(); itr++)
		ParaDFS(m_para,root->child(itr),s);
}

void CyberInfoDockWidget::OutputDFS(vector<myOutputVar*>& m_output, TreeItem* root, string s) {
	//	
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

//��ȡCyberInfo���������������
//����ɾ�� CyberInfo����
bool CyberInfoDockWidget::readCyberInfo(CyberInfo* pCyberInfo)
{
	if (!pCyberInfo) //�Զ�����
	{
		QMessageBox msgBox;
		auto doc = (DocumentComponent*)mywid->getDocument();
		if (!doc) {
			msgBox.setText("�ĵ�������");
			msgBox.exec();
			return false;
		}

		//�ȴ洢һ��
		saveCyberInfo();

		pCyberInfo = ((DocumentComponent*)mywid->getDocument())->GetCyberInfo();
		if (!pCyberInfo) {
			//QMessageBox msgBox;
			//msgBox.setText("��ȡʧ�ܣ����ȱ���ģ������");
			//msgBox.exec();
			return false;
		}

		pCyberInfo->SetDocument((DocumentComponent*)mywid->getDocument());
		pCyberInfo->AutomaticComputing();
	}

	MoFilename = QString::fromStdString(pCyberInfo->GetMdlName());
	////����������
	//QStringList m_paraList;
	//int size = pCyberInfo->GetParameters().size();
	//for (int itr = 0; itr < size; ++itr) {
	//	string s="";
	//	
	//	//1.ȫ������
	//	string sName = pCyberInfo->GetParameters().at(itr)->sFullName;
	//	int n_times = count(sName.begin(), sName.end(), '.');
	//	if (n_times) sName.replace(0, sName.find_last_of('.') + 1, "");
	//	string space = "";
	//	while (n_times)
	//	{
	//		space += " ";
	//		n_times--;
	//	}
	//	s += space;
	//	s += sName;
	//	
	//	//�����������
	//	s += "\t";
	//	string sType= pCyberInfo->GetParameters().at(itr)->sType;
	//	if (sType == "")sType = " ";
	//	s += sType;

	//	//2.ȱʡֵ
	//	s += "\t";
	//	//double value = m_newCyberInfo->GetParameters().at(itr)->defaultValue;
	//	string value = pCyberInfo->GetParameters().at(itr)->defaultValue;
	//	s += value;
	//	//ostringstream strStream;
	//	//strStream << value;
	//	//if(value!=0)s += strStream.str(); 

	//	//3.��λ
	//	s += "\t";
	//	s += pCyberInfo->GetParameters().at(itr)->sUnit;

	//	//����ά��
	//	string sDim = pCyberInfo->GetParameters().at(itr)->sDimension;
	//	if (sDim!="") {
	//	s += "\t";
	//	//string sDim = m_newCyberInfo->GetParameters().at(itr)->sDimension;
	//	//if (sDim == "")sDim = " ";
	//	s += sDim;
	//	}

	//	//4.�Ƿ��ע���ע��
	//	s += "\t";
	//	if (pCyberInfo->GetParameters().at(itr)->bFocused)
	//	{
	//		s += "��ע";
	//		s += "\t";
	//		s += pCyberInfo->GetParameters().at(itr)->sFocusName;
	//	}
	//	m_paraList .append(QString::fromStdString(s));
	//}

	////�������������
	//QStringList m_outputList;
	//size = pCyberInfo->GetOutputVars().size();
	//for (int itr = 0; itr < size; ++itr) {
	//	string s = "";
	//	string cur = "";
	//	//1.ȫ������
	//	string sName = pCyberInfo->GetOutputVars().at(itr)->sFullName;
	//	int n_times = count(sName.begin(), sName.end(), '.');
	//	if (n_times) sName.replace(0, sName.find_last_of('.') + 1, "");
	//	string space = "";
	//	while (n_times)
	//	{
	//		space += " ";
	//		n_times--;
	//	}
	//	s += space;
	//	s += "NULL";
	//	s += "\t";
	//	s += sName;

	//	//3.�Ƿ��ע���ע��
	//	s += "\t";
	//	s += pCyberInfo->GetOutputVars().at(itr)->bFocused ? "��ע" : "NULL";

	//	//2.����
	//	cur= pCyberInfo->GetOutputVars().at(itr)->sType;
	//	if (cur != "")
	//	{
	//		s += "\t";
	//		s += cur ;
	//	}

	//	s += "\t";
	//	cur = "";
	//	cur = pCyberInfo->GetOutputVars().at(itr)->sFocusName;
	//	s += cur==""?"NULL":cur;
	//	m_outputList.append(QString::fromStdString(s));
	//}

	//paratreemodel->setupModelData(m_paraList,paratreemodel->getrootItem());
	//outputtreemodel->setupModelData(m_outputList, outputtreemodel->getrootItem());
	paratreemodel->buildModel(pCyberInfo);
	outputtreemodel->buildModel(pCyberInfo);


	//�����ӿڲ���
	int conssize = pCyberInfo->GetConnectorTypes().size();
	m_cons.clear();
	for (int i = 0; i < conssize; i++) {
		m_cons.push_back(make_pair(QString::fromStdString(pCyberInfo->GetConnectorNames().at(i)),QString::fromStdString(pCyberInfo->GetConnectorTypes().at(i))
			 ));
	}
	updateConInfo();

	MdlName->setText(QString::fromStdString(pCyberInfo->GetMdlName()));
	m_cons_table->update();
	paratreeview->expandAll();
	outputtreeview->expandAll();

	delete pCyberInfo;

	return true;
}

void CyberInfoDockWidget::close() {

	m_sender->setChecked(false);
	
	//shutdown();
}

void CyberInfoDockWidget::shutdown()
{
	if (m_closed)return;
	m_closed = true;
	delete paratreemodel;
	//paratreemodel = nullptr;
	delete outputtreemodel;
	//outputtreemodel
	if (m_cons_table) {
		delete m_cons_table;
		m_cons_table = nullptr;
	}
	//bug:Cyber���ڲ��ص�����£������������ĵ����ر�ʱ����
	if (dock1) {
		//delete dock1;
		dock1 = nullptr;
	}
}
