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
	//初始化
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
	dock1->setWindowTitle("模型主要信息");
	//dock1->setAttribute(Qt::WA_DeleteOnClose);
	QDockWidget::connect(dock1, SIGNAL(destroyed()), this, SLOT(close()));
	// 给浮动窗口添加控件
	QVBoxLayout* VLayout1 = new QVBoxLayout(dock1);
	QHBoxLayout* HLayout1 = new QHBoxLayout(dock1);
	QHBoxLayout* HLayout2 = new QHBoxLayout(dock1);
	QLabel* text1 = new QLabel();	
	QPushButton* PushButton1 = new QPushButton();
	QPushButton* PushButton4 = new QPushButton(mywidget);
	QPushButton* PushButton5 = new QPushButton(mywidget);

	text1->setText("模型全名: ");
	
	MdlName->setText("...");
	MdlName->setStyleSheet(" background-color: white ;border:2px solid red;");
	//LabLists[0]
	//MdlName->setScaledContents(true);
	//MdlName->setText("...");
	//MdlName->setStyleSheet(" background-color: white ;border:2px solid red;");
	//MdlName->setScaledContents(true);
	PushButton1->setText("选择...");
	PushButton1->setMaximumSize(50, 40);
	PushButton4->setText("保存");
	PushButton5->setText("自动计算...");
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
	//创建参数树和变量树
	CreateTable(mywidget);
}

//创建参数表和输出表框架
void CyberInfoDockWidget::CreateTable(MainWindow* mywidget) {

	QLabel* text3 = new QLabel();
	QLabel* text4 = new QLabel();
	QPushButton* PushButton2 = new QPushButton(mywidget);
	QPushButton* PushButton3 = new QPushButton(mywidget);
	QVBoxLayout* VerticalLayout = new QVBoxLayout(dock1);
	QHBoxLayout* HorizontalLayout1 = new QHBoxLayout(dock1);
	QHBoxLayout* HorizontalLayout2 = new QHBoxLayout(dock1);

	//填充视图内容
	updateTable1();
	updateTable2();

	text3->setText("参数信息:");
	text4->setText("状态变量列表:");
	PushButton2->setText("去除所有关注");
	PushButton3->setText("去除所有关注");
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

	//创建接口信息栏
	createConInfo();

	if (((DocumentComponent*)mywid->getDocument()))
	{
		CyberInfo* pCyberInfo = ((DocumentComponent*)mywid->getDocument())->GetCyberInfo();

		if (!pCyberInfo) {
			//QMessageBox msgBox;
			//msgBox.setText("读取失败，请先保存模型数据");
			//msgBox.exec();
			return;
		}
		readCyberInfo(pCyberInfo);
		//delete pCyberInfo;
	}

	MdlName->setText(GetMdlname(MoFilename));
}

//创建接口信息栏
void CyberInfoDockWidget::createConInfo() {
	//获取接口信息
	m_cons = paratreemodel->getm_cons();	
	m_cons_table->setColumnCount(2);
	m_cons_table->setRowCount(m_cons.size());
	//布置接口信息表
	QStringList header;
	header << "接口类型" << "接口名称";
	m_cons_table->setHorizontalHeaderLabels(header);

	updateConInfo();
	m_cons_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	m_cons_table->show();
	
	QVBoxLayout* VerticalLayout = new QVBoxLayout(dock1);
	VerticalLayout->addWidget(WidgLists[1]);
	VerticalLayout->addWidget(m_cons_table);
	m_cons_table->horizontalHeader()->setSectionResizeMode(0,QHeaderView::Stretch);     //x先自适应宽度		
	VerticalLayout->setStretchFactor(WidgLists[1],3);
	VerticalLayout->setStretchFactor(m_cons_table, 1);
	WidgLists[2]->setLayout(VerticalLayout);

	dock1->setWidget(WidgLists[2]);
}

//更新接口信息栏
void CyberInfoDockWidget::updateConInfo() {
	m_cons_table->clear();
	QStringList header;
	header<< "接口类型" << "接口名称";
	m_cons_table->setHorizontalHeaderLabels(header);
	m_cons_table->setRowCount(m_cons.size());
	for (int i = 0; i < m_cons.size(); i++) {
		m_cons_table->setItem(i, 1, new QTableWidgetItem(m_cons[i].first));
		m_cons_table->setItem(i, 0, new QTableWidgetItem(m_cons[i].second));
	}
}

//创建参数表
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

//创建输出表
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

//打开MO模型文件
void CyberInfoDockWidget::OpenMo() {
	//MoFilename = QFileDialog::getOpenFileName(mywid, tr("Select a .Mo file"), QString(), tr("Modelica Files(*.Mo);;All files(*.*)"));
	;
}

//关联MO文件
void CyberInfoDockWidget::ConnectMo() {
	OpenMo();
}

//清空参数表所有关注
void CyberInfoDockWidget::SetTableZero1() {
	paratreemodel->ReSet(CyberInfoDockWidget::paratreemodel,bFocusedcol, sFocusNamecol);
}

//清空输出表所有关注
void CyberInfoDockWidget::SetTableZero2() {
	paratreemodel->ReSet(CyberInfoDockWidget::paratreemodel, 2, 3);
}

//返回悬浮窗
QDockWidget* CyberInfoDockWidget::getdock() {
	//dock1->setFeatures(QDockWidget::NoDockWidgetFeatures);
	return dock1;
}

//判断是否为mo文件
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
//读取mo模型全名
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

//选择模型，获得并显示信息
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
//保存CyberInfo到文档
void CyberInfoDockWidget::saveCyberInfo()
{
	if (MdlName->text().isEmpty()/*paratreemodel->empty()*/) {
		QMessageBox msgBox;
		msgBox.setText("保存失败，请先关联MO模型");
		msgBox.exec();
		return;
	}
	
	//1.模型名
	//m_CyberInfo->SetMdlName(GetMdlname(MoFilename).toStdString());
	m_CyberInfo->SetMdlName(MdlName->text().toStdString());
	ModelicaModel* pModel = ModelicaModel::LookupModel(m_CyberInfo->GetMdlName());
	if (pModel)
	{
		m_CyberInfo->SetMdlType(pModel->m_sType.c_str());
	}
	//2.模型参数
	vector<myParameter*> m_para;
	TreeItem* root = paratreemodel->getrootItem();
	for (int itr = 0; itr < root->childCount(); itr++)
		ParaDFS(m_para, root->child(itr));
	m_CyberInfo->SetParameters(m_para);

	//3.模型输出变量
	vector<myOutputVar*> m_output;
	 root = outputtreemodel->getrootItem();
	for (int itr = 0; itr < root->childCount(); itr++)
		OutputDFS(m_output, root->child(itr));
	m_CyberInfo->SetOutputVars(m_output);

	//4.模型接口信息
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
	//msgBox.setText("保存成功");
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
	//维度？？？
	//temp->sDimension = root->data(sDimCol).toString().toStdString();
	if (root->data(bFocusedcol) == "是") {
		temp->bFocused = 1;
		/*DocumentComponent* m_doc = (DocumentComponent*)mywid->getDocument();
		M3dMdl_DS* m_DS = dynamic_cast<M3dMdl_DS*>(m_doc->GetDS());
		QMessageBox msgBox;
		if (!m_DS) {
			msgBox.setText("M3dMdl_DS不存在");
			msgBox.exec();
			//return false;
		}
		ParameterValue m_para;
		if (!m_DS->GetComponent()) {
			msgBox.setText("Component不存在");
			msgBox.exec();
		}
		//关注名？
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
	if (root->data(2) == "是") temp->bFocused = 1;
	else temp->bFocused = 0;
	temp->sFocusName = root->data(3).toString().toStdString();
	m_output.push_back(temp);
	for (int itr = 0; itr < root->childCount(); itr++)
		OutputDFS(m_output, root->child(itr), s);
}

//读取CyberInfo构建参数和输出表
//读完删除 CyberInfo数据
bool CyberInfoDockWidget::readCyberInfo(CyberInfo* pCyberInfo)
{
	if (!pCyberInfo) //自动计算
	{
		QMessageBox msgBox;
		auto doc = (DocumentComponent*)mywid->getDocument();
		if (!doc) {
			msgBox.setText("文档不存在");
			msgBox.exec();
			return false;
		}

		//先存储一下
		saveCyberInfo();

		pCyberInfo = ((DocumentComponent*)mywid->getDocument())->GetCyberInfo();
		if (!pCyberInfo) {
			//QMessageBox msgBox;
			//msgBox.setText("读取失败，请先保存模型数据");
			//msgBox.exec();
			return false;
		}

		pCyberInfo->SetDocument((DocumentComponent*)mywid->getDocument());
		pCyberInfo->AutomaticComputing();
	}

	MoFilename = QString::fromStdString(pCyberInfo->GetMdlName());
	////构建参数表
	//QStringList m_paraList;
	//int size = pCyberInfo->GetParameters().size();
	//for (int itr = 0; itr < size; ++itr) {
	//	string s="";
	//	
	//	//1.全名处理
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
	//	//加入参数类型
	//	s += "\t";
	//	string sType= pCyberInfo->GetParameters().at(itr)->sType;
	//	if (sType == "")sType = " ";
	//	s += sType;

	//	//2.缺省值
	//	s += "\t";
	//	//double value = m_newCyberInfo->GetParameters().at(itr)->defaultValue;
	//	string value = pCyberInfo->GetParameters().at(itr)->defaultValue;
	//	s += value;
	//	//ostringstream strStream;
	//	//strStream << value;
	//	//if(value!=0)s += strStream.str(); 

	//	//3.单位
	//	s += "\t";
	//	s += pCyberInfo->GetParameters().at(itr)->sUnit;

	//	//加入维度
	//	string sDim = pCyberInfo->GetParameters().at(itr)->sDimension;
	//	if (sDim!="") {
	//	s += "\t";
	//	//string sDim = m_newCyberInfo->GetParameters().at(itr)->sDimension;
	//	//if (sDim == "")sDim = " ";
	//	s += sDim;
	//	}

	//	//4.是否关注与关注名
	//	s += "\t";
	//	if (pCyberInfo->GetParameters().at(itr)->bFocused)
	//	{
	//		s += "关注";
	//		s += "\t";
	//		s += pCyberInfo->GetParameters().at(itr)->sFocusName;
	//	}
	//	m_paraList .append(QString::fromStdString(s));
	//}

	////构建输出变量表
	//QStringList m_outputList;
	//size = pCyberInfo->GetOutputVars().size();
	//for (int itr = 0; itr < size; ++itr) {
	//	string s = "";
	//	string cur = "";
	//	//1.全名处理
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

	//	//3.是否关注与关注名
	//	s += "\t";
	//	s += pCyberInfo->GetOutputVars().at(itr)->bFocused ? "关注" : "NULL";

	//	//2.类型
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


	//构建接口参数
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
	//bug:Cyber窗口不关的情况下，连续打开两个文档，关闭时报错
	if (dock1) {
		//delete dock1;
		dock1 = nullptr;
	}
}
