#include "DocumentModel.h"
#include"ParaDockWidget.h"
#include"DocumentComponent.h"
#include <QAction>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
//#include "DocumentModel.h"
#include <QMessageBox>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include<QLabel>
using namespace std;

ParaDockWidget::ParaDockWidget(MainWindow* mywidget, QAction* sender) :
	mywid(mywidget),m_sender(sender) {
	//indexmap = new unordered_map<int, string>();
	m_dock = new QDockWidget(mywid);
	//m_dock->setWindowTitle("ģ�Ͳ���");
	m_dock->setAttribute(Qt::WA_DeleteOnClose);
	QDockWidget::connect(m_dock, SIGNAL(destroyed()), this, SLOT(close()));
	//m_dock->setW(230);

	//QVBoxLayout* VerticalLayout = new QVBoxLayout(m_dock);

	m_table= new QTableWidget(this);
	
	connect(m_table, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)),
		this, SLOT(currentItemChangedSLOT(QTableWidgetItem*,QTableWidgetItem*)));
	QVBoxLayout* VerticalLayout=new QVBoxLayout(m_dock);
	
	//m_dock->setWidget(m_table);
	m_dock->setAllowedAreas(Qt::RightDockWidgetArea);
	QLabel* label = new QLabel();
	QPushButton* addbutton = new QPushButton();
	QPushButton* delbutton = new QPushButton();
	QPushButton* savebutton = new QPushButton();
	QPushButton* autoComputerbutton = new QPushButton();
	QPushButton* Loadbutton = new QPushButton();
	addbutton->setFixedSize(25,25);
	delbutton->setFixedSize(25, 25);
	savebutton->setFixedSize(60,30);
	autoComputerbutton->setFixedSize(60, 30);
	Loadbutton->setFixedSize(60, 30);
	label->setText("ģ�Ͳ�����");
	addbutton->setText("+");
	delbutton->setText("-");
	savebutton->setText("����");
	Loadbutton->setText("���ع�ע");
	autoComputerbutton->setText("�Զ�����"); //���ˡ�����
	QObject::connect(addbutton, SIGNAL(clicked(bool)), this, SLOT(addline()));
	QObject::connect(delbutton, SIGNAL(clicked(bool)), this, SLOT(delline()));
	QObject::connect(savebutton, SIGNAL(clicked(bool)), this, SLOT(saveParatable()));
	QObject::connect(autoComputerbutton, SIGNAL(clicked(bool)), this, SLOT(autoComputeParatable()));
	QObject::connect(Loadbutton, SIGNAL(clicked(bool)), this, SLOT(LoadParatable()));

	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(m_dock);
	HorinztalLayout1->addWidget(label);
	HorinztalLayout1->addWidget(savebutton);
	HorinztalLayout1->addWidget(Loadbutton);
	HorinztalLayout1->addWidget(autoComputerbutton);
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(m_dock);	
	HorinztalLayout2->addWidget(m_table);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(m_dock);
	HorinztalLayout3->addWidget(addbutton);
	HorinztalLayout3->addWidget(delbutton);
	
	VerticalLayout->addLayout(HorinztalLayout1);
	VerticalLayout->addLayout(HorinztalLayout2);
	VerticalLayout->addLayout(HorinztalLayout3);
	VerticalLayout->setStretch(0, 2);
	VerticalLayout->setStretch(1, 12);
	VerticalLayout->setStretch(2, 1);
	QWidget* temp=new QWidget();
	temp->setLayout(VerticalLayout);
	m_dock->setWidget(temp);
	readParatable(false);
}

ParaDockWidget::~ParaDockWidget()
{
	if (m_table)delete m_table;
	if (m_dock)
		delete m_dock;
}

//��ȡ�ĵ�����ʼ����ע����
void ParaDockWidget::initParatable()
{
	DocumentCommon* m_doc = (DocumentCommon*)mywid->getDocument();
	//auto m_DS = dynamic_cast<M3d_DS*>(m_doc->GetDS());
	Component* cur;
	if (!m_doc || !(cur = m_doc->GetComponent())) {
		QMessageBox msgBox;
		msgBox.setText("�ĵ���ȡʧ��");
		msgBox.exec();
		return;
	}

	//������ǰװ�����µ�һ���ӽڵ㣬��ȡ��ע����
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
			//vector<myParameter*> focusedval;
			for (myParameter* para : paras) {
				if (para->bFocused) {
					ParameterValue temp;
					temp.sMapParamName = scompname + "." + para->sFullName;
					m_data.push_back(make_pair(temp, 0));
				}
			}
		}
	}
	//m_table->setItemDelegate();
	indexmap.clear();

	//fillParatable();
}

//�Զ����ع�ע
void ParaDockWidget::LoadParatable()
{
	DocumentCommon* m_doc = mywid->getDocument();
	if (!m_doc)
	{
		QMessageBox msgBox;
		msgBox.setText("�ĵ���ȡʧ��");
		msgBox.exec();
		return;
	}

	auto cur = m_doc->GetComponent();
	
	//������ǰװ�����µ�һ���ӽڵ㣬��ȡ��ע����
	vector<ParameterValue> vPVs;
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
			//vector<myParameter*> focusedval;
			for (myParameter* para : paras) {
				if (para->bFocused) {
					ParameterValue temp;
					temp.sMapParamName = scompname + "." + para->sFullName;
					temp.sParamName = "";
					temp.sParamValue = para->defaultValue;
					vPVs.push_back(temp);
				}
			}
		}
	}

	initParatable();
	if (!vPVs.empty()) {
		unordered_map<string, int> map;
		int size = m_data.size();
		for (int i = 0; i < size; i++)
			map.emplace(m_data[i].first.sMapParamName, i);
		indexmap.clear();
		int idx = 0;
		for (auto para : vPVs)
		{
			int pos = map[para.sMapParamName];
			indexmap.emplace(idx++, to_string(pos));
			m_data[pos].first.sParamName = para.sParamName;
			m_data[pos].first.sParamValue = para.sParamValue;
			m_data[pos].second = 1;
		}
	}

	fillParatable();
}

//��ȡ�ĵ����в�������
void ParaDockWidget::readParatable(bool bCompute)
{
	DocumentCommon* m_doc= mywid->getDocument();
	if (!m_doc)
	{
		QMessageBox msgBox;
		msgBox.setText("�ĵ���ȡʧ��");
		msgBox.exec();
		return;
	}
	//3.�ٶ�ȡˢ��
	//auto vec = dynamic_cast<M3dMdl_DS*>(m_doc->GetDS())->GetAllParameterValues();
	auto vec = ((DocumentModel*)m_doc)->GetAllParameterValues();
	initParatable();
	if (!vec.empty()) {
		unordered_map<string, int> map;	
		int size = m_data.size();
		for (int i = 0; i < size; i++)
			map.emplace(m_data[i].first.sMapParamName, i);
		indexmap.clear();
		int idx = 0;
		for (auto para:vec)
		{
			int pos = map[para.sMapParamName];
			indexmap.emplace(idx++,to_string(pos) );
			m_data[pos].first.sParamName = para.sParamName;
			m_data[pos].first.sParamValue = para.sParamValue;
			m_data[pos].second = 1;
		}
	}
	 
	fillParatable();
}

//�Զ����㣺�ȱ��浱ǰ�������룬�ٸ��¼��㣬��ˢ����ʾ����ȡ��
void ParaDockWidget::autoComputeParatable()
{
	DocumentCommon* m_doc = mywid->getDocument();
	if (!m_doc)
	{
		QMessageBox msgBox;
		msgBox.setText("�ĵ���ȡʧ��");
		msgBox.exec();
		return;
	}
	if (1)
	{
		//1.�ȱ���
		saveParatable();

		//2.���¼���
		auto vParms = ((DocumentModel*)m_doc)->GetAllParameterValues();
		vector<ParameterValue> newPVs;
		for (int i = 0; i < vParms.size(); i++)
		{
			ParameterValue pv = vParms[i];
			QString sMapName = pv.sMapParamName.c_str();
			QStringList qsl = sMapName.split(".");
			QString sComName = qsl[0];
			QString sComParName = qsl[1];
			vector<string> vNames = { "r","r_CM","r_ab","r_ba","n","nX","n_X","nY","n_Y","nZ","n_Z" };
			if (std::find(vNames.begin(), vNames.end(), sComParName.toStdString()) != vNames.end())
			{
				QString sNewVlu = ((DocumentModel*)m_doc)->ReComputeParamValue(sComName, sComParName);
				pv.sParamValue = sNewVlu.toStdString();
			}
			newPVs.push_back(pv);
		}
		((DocumentModel*)m_doc)->SetParameterValues(newPVs);
	}

	//3.�ٶ�ȡˢ��
	//auto vec = dynamic_cast<M3dMdl_DS*>(m_doc->GetDS())->GetAllParameterValues();
	auto vec = ((DocumentModel*)m_doc)->GetAllParameterValues();
	initParatable();
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

	fillParatable();
}

//����ӳ����������
void ParaDockWidget::fillParatable()
{
	m_table->clear();

	QStringList header;
	m_table->setColumnCount(3);
	header << "�������" << "�½�����" << "����ֵ";
	m_table->setHorizontalHeaderLabels(header);
	m_table->setMinimumWidth(350);
	m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	m_table->setItemDelegateForColumn(0, new ListDelegate(this));
	m_table->setRowCount(indexmap.size());
	int i = 0;
	for (auto itr = indexmap.begin(); itr !=indexmap.end(); itr++) {
		m_table->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(m_data[stoi((*itr).second)].first.sMapParamName)));
		m_table->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(m_data[stoi((*itr).second)].first.sParamName)));
		m_table->setItem(i++, 2, new QTableWidgetItem(QString::fromStdString(m_data[stoi((*itr).second)].first.sParamValue)));
	}
}

//����������õ��ĵ���
void ParaDockWidget::saveParatable()
{
	DocumentCommon* docCommon = mywid->getDocument();
	if (!docCommon->IsModel())
	{
		QMessageBox msgBox;
		msgBox.setText("�ĵ���ȡʧ��");
		msgBox.exec();
		return;
	}
	currentItemChangedSLOT(nullptr, m_table->currentItem());
	//M3dMdl_DS* m_DS = dynamic_cast<M3dMdl_DS*>(m_doc->GetDS());
	//if (!m_DS)
	//{
	//	QMessageBox msgBox;
	//	msgBox.setText("M3dMdl_DS��ȡʧ��");
	//	msgBox.exec();
	//	return;
	//}

	vector<ParameterValue> vec;
	for (auto itr = indexmap.begin(); itr != indexmap.end(); ++itr)
		vec.push_back(m_data[stoi((*itr).second)].first);
	((DocumentModel*)docCommon)->SetParameterValues(vec);
	//for(auto data:m_data)
	
}


void ParaDockWidget::currentItemChangedSLOT(QTableWidgetItem* last,QTableWidgetItem* cur)
{
	if (!cur) return;
	int r = cur->row();
	/*static int times = 0;
	if (times == 1) {
		times = 0; return;
	}
	times++;*/
	if (indexmap.find(r) == indexmap.end())return;
	int idx = stoi(indexmap.at(cur->row()));
	//auto s = cur->text();
	//int col = cur->column();
	switch (cur->column()) 
	{
	case 0: 
		//�����ȫ���н��ɴ�����
		break;
		//(cur->data(Qt::EditRole)).toStdString();
	case 1:
		m_data[idx].first.sParamName = cur->text().toStdString();
		break;
	case 2:
		m_data[idx].first.sParamValue = cur->text().toStdString();
		break;
	}
}

//������������
void ParaDockWidget::addline()
{
	if (indexmap.size() >= m_data.size())
	{
		QMessageBox msgBox;
		msgBox.setText("���й�ע�����������");
		msgBox.exec();
		return;
	}
	if (m_table->rowCount() >= m_data.size())
	{
		QMessageBox msgBox;
		msgBox.setText("��������ѳ�����������Ĺ�ע��������");
		msgBox.exec();
		return;
	}
	m_table->setRowCount(m_table->rowCount() + 1);
}

//�����ɾȥĩβһ��
void ParaDockWidget::delline()
{
	if (m_table->rowCount() == 0) {
		QMessageBox msgBox;
		msgBox.setText("�������޶����п�ɾ��");
		msgBox.exec();
		return;
	}
	int idx = m_table->rowCount() - 1;
	if (indexmap.find(idx) != indexmap.end()) {
		m_data[stoi(indexmap.at(idx))].second = 0;
		indexmap.erase(indexmap.find(idx));
		//indexmap.reserve(m_table->rowCount()-1);
	}
	m_table->removeRow(idx);
	//m_table->setRowCount(m_table->rowCount() - 1);
}

void ParaDockWidget::close() {
	//saveParatable();
	m_dock = nullptr;
	//m_sender->setChecked(false);
}

//���������б�
QWidget* ListDelegate::createEditor(QWidget* parent,
	const QStyleOptionViewItem& /* option */,
	const QModelIndex& index) const
{
	//if (index.sibling(index.row(), 1).data() == " ") return nullptr;
	QComboBox* editor = new QComboBox(parent);
	auto map = m_Pd->getmap();
	auto m_data = m_Pd->getdata();
	//unordered_map<int, int>::iterator idx;
	int size = (*m_data).size();
	my_map->clear();
	//int counts = 0;
	for (int idx = 0; idx != size; idx++) {
		//��δ��ӣ����������б�
		if ((*m_data).at(idx).second == false)
		{
			string s = (*m_data).at(idx).first.sMapParamName;
			editor->addItem(QString::fromStdString(s));
			my_map->emplace(s, idx);
			//editor->setItemData(counts++, idx, Qt::UserRole+1);
		}
	}
	QMessageBox msgBox; //QString s;
	/*for(int i=0;i<editor->maxVisibleItems();i++)
		s+= (editor->itemData(i,Qt::UserRole).toString());
	msgBox.setText(s);
	msgBox.exec();*/
	
	//editor->addItem("");
	//editor->addItem("����ע");

	return editor;
}

void ListDelegate::setEditorData(QWidget* editor,
	const QModelIndex& index) const
{
	QString text = index.model()->data(index, Qt::EditRole).toString();
	//QTableWidgetItem* cur = m_Pd->m_table->currentItem();
	//int val = cur->data(Qt::UserRole).toInt();
	//auto model = index.model();
	int value = index.model()->data(index, Qt::UserRole).toInt();
	QComboBox* combobox = static_cast<QComboBox*>(editor);
	combobox->setCurrentIndex(combobox->findText(text));
}

void ListDelegate::setModelData(QWidget* editor,
	QAbstractItemModel* model,
	const QModelIndex& index)const
{
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	QString text = comboBox->currentText();
	if (text == "") return;
	//int c= comboBox->currentData(Qt::UserRole).toInt();
	
	auto map = m_Pd->getmap();
	int idx = my_map->at(text.toStdString());
	//����ӳ�䲢��¼�����
	auto m_data = m_Pd->getdata();
	//��ԭ���޸�
	if (map->find(index.row()) != map->end())
		m_data->at(stoi(map->at(index.row()))).second = false;
	//������ӳ��
	map->emplace(index.row(),to_string(idx)) ;
	m_data->at(idx).second = true;
	model->setData(index, text, Qt::EditRole);
}
void ListDelegate::updateEditorGeometry(QWidget* editor,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}