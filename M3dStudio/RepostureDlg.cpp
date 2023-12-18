#include "DocumentModel.h"
#include "RepostureCommand.h"
#include "RepostureDlg.h"
#include "Connector.h"
#include "Component.h"
//#include "DocumentModel.h"
#include "DocumentComponent.h"
#include "global.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QPushButton>
#include <QValidator>
#include <QRegExp>
#include <QLabel>
#include <QMessageBox>
#include <QString>
#include <queue>


reposturedialog::reposturedialog(QWidget* parent, Command* p_Cmd, string mtype) :QDialog(parent), _p_Cmd(p_Cmd),_mtype(mtype)
{
	title_label = nullptr;
	title_line = nullptr;
	consConn_label = nullptr;
	consConnBox = nullptr;

	origin_label = nullptr;
	origin_line = nullptr;
	AxisZ_label = nullptr;
	Axis_Z = nullptr;
	AxisX_label = nullptr;
	Axis_X = nullptr;

	adjust_label = nullptr;
	offset_label = nullptr;
	delta_label = nullptr;
	slide_label_x = nullptr;
	slide_spin_x = nullptr;
	spin_line_xdelta = nullptr;
	slide_label_y = nullptr;
	slide_spin_y = nullptr;
	spin_line_ydelta = nullptr;
	slide_label_z = nullptr;
	slide_spin_z = nullptr;
	spin_line_zdelta = nullptr;
	rotate_label_x = nullptr;
	rotate_spin_x = nullptr;
	rotate_line_xdelta = nullptr;
	rotate_label_y = nullptr;
	rotate_spin_y = nullptr;
	rotate_line_ydelta = nullptr;
	rotate_label_z = nullptr;
	rotate_spin_z = nullptr;
	rotate_line_zdelta = nullptr;
	v15_record.resize(15, 0.0);
	last_record = { 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
	last_change.resize(6, 0.0);
	change_all.resize(6, 0.0);
	first_read.resize(6.0, true);
	change_consComboBox = false;
	init();
}

reposturedialog::~reposturedialog()
{
	
}

void reposturedialog::init()
{
	this->setWindowTitle(tr("���˴���"));
	//���һ����ֱ�ܲ���
	QVBoxLayout* mainlayout = new QVBoxLayout(this);

	//���Ⲽ��
	QHBoxLayout* title_layout = new QHBoxLayout;
	title_label = new QLabel(tr("���"));
	title_label->setAlignment(Qt::AlignCenter);
	title_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	title_line = new QLineEdit;
	title_line->setText(GetSelectedname());
	title_line->setAlignment(Qt::AlignCenter);
	title_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	title_line->setReadOnly(true);
	consConn_label = new QLabel(tr("�ӿ�"));
	consConn_label->setAlignment(Qt::AlignCenter);
	consConn_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// �ҵ�������װ������нӿ�
	for (auto constraint : dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetAllConnConstraints())
	{
		assembly_graph[constraint.component1 + '+' + constraint.connector1] = constraint.component2 + '+' + constraint.connector2;
		assembly_graph[constraint.component2 + '+' + constraint.connector2] = constraint.component1 + '+' + constraint.connector1;
		name_map[constraint.component1].emplace(constraint.component1 + '+' + constraint.connector1);
		name_map[constraint.component2].emplace(constraint.component2 + '+' + constraint.connector2);
	}

	QStringList conns_record; // ��¼�����������װ���ϵ�Ľӿ�
	if (name_map[_p_Cmd->GetComponent()->GetCompName()].size() > 1)
	{
		conns_record.push_back(QString("��ѡ��"));
	}
	for (auto comp_conn_name : name_map[_p_Cmd->GetComponent()->GetCompName()])
	{
		if (assembly_graph.count(comp_conn_name))
		{
			conns_record.push_back(QString::fromStdString(comp_conn_name));
		}
	}
	consConnBox = new QComboBox(this);
	consConnBox->addItems(conns_record);

	title_layout->addWidget(title_label);
	title_layout->addWidget(title_line);
	title_layout->addWidget(consConn_label);
	title_layout->addWidget(consConnBox);
	
	//���ñ���
	title_layout->setStretchFactor(title_label, 2);
	title_layout->setStretchFactor(title_line, 3);
	title_layout->setStretchFactor(consConn_label, 2);
	title_layout->setStretchFactor(consConnBox, 3);

	//������ʽ���������������ʽ
	QRegExp exp("^\\((-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?\\)$");
	QRegExp real_exp("^\\d+(\\.\\d+)?$");
	QRegExpValidator* expvalidator = new QRegExpValidator(exp);
	QRegExpValidator* real_expvalidator = new QRegExpValidator(real_exp);
	//ȥ��С����֮���β0
	QRegExp remove_zero_exp("(\\.)?0+$");

	QGroupBox* coordinate_group = new QGroupBox(tr("����ϵ"));
	QHBoxLayout* coordinate_layout = new QHBoxLayout;
	origin_label = new QLabel(tr("ԭ��"));
	origin_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	origin_label->setAlignment(Qt::AlignCenter);
	origin_line = new QLineEdit;
	origin_line->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	origin_line->setAlignment(Qt::AlignCenter);
	origin_line->setValidator(expvalidator);
	connect(origin_line, &QLineEdit::textChanged, this, &reposturedialog::On_LineEdit_ValidChanged);
	//���ó�ʼ����
	origin_line->setText("(0,0,0)");

	AxisZ_label = new QLabel(tr("Z�᷽��"));
	AxisZ_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	AxisZ_label->setAlignment(Qt::AlignCenter);
	Axis_Z = new QLineEdit;
	Axis_Z->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	Axis_Z->setAlignment(Qt::AlignCenter);
	Axis_Z->setValidator(expvalidator);
	connect(Axis_Z, &QLineEdit::textChanged, this, &reposturedialog::On_LineEdit_ValidChanged);
	//���ó�ʼ����
	Axis_Z->setText("(0,0,1)");

	AxisX_label = new QLabel(tr("X�᷽��"));
	AxisX_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	AxisX_label->setAlignment(Qt::AlignCenter);
	Axis_X = new QLineEdit;
	Axis_X->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	Axis_X->setAlignment(Qt::AlignCenter);
	Axis_X->setValidator(expvalidator);
	connect(Axis_X, &QLineEdit::textChanged, this, &reposturedialog::On_LineEdit_ValidChanged);
	//���ó�ʼ����
	Axis_X->setText("(1,0,0)");
	
	coordinate_layout->addWidget(origin_label);
	coordinate_layout->addWidget(origin_line);
	coordinate_layout->addWidget(AxisZ_label);
	coordinate_layout->addWidget(Axis_Z);
	coordinate_layout->addWidget(AxisX_label);
	coordinate_layout->addWidget(Axis_X);
	coordinate_layout->setStretchFactor(origin_label, 1);
	coordinate_layout->setStretchFactor(origin_line, 2);
	coordinate_layout->setStretchFactor(AxisZ_label, 1);
	coordinate_layout->setStretchFactor(Axis_Z, 2);
	coordinate_layout->setStretchFactor(AxisX_label, 1);
	coordinate_layout->setStretchFactor(Axis_X, 2);
	coordinate_group->setLayout(coordinate_layout);

	QGroupBox* adjust_group = new QGroupBox(tr("����"));
	QVBoxLayout* adjust_mainlayout = new QVBoxLayout;
	//7��QHBoxLayout
	QHBoxLayout* adjust_layout = new QHBoxLayout;
	adjust_label = new QLabel(tr("��������"));
	adjust_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	adjust_label->setAlignment(Qt::AlignCenter);
	offset_label = new QLabel(tr("ƫ����"));
	offset_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	offset_label->setAlignment(Qt::AlignCenter);
	delta_label = new QLabel(tr("��������"));
	delta_label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	delta_label->setAlignment(Qt::AlignCenter);
	adjust_layout->addWidget(adjust_label);
	adjust_layout->addWidget(offset_label);
	adjust_layout->addWidget(delta_label);
	adjust_layout->setStretchFactor(adjust_label, 3);
	adjust_layout->setStretchFactor(offset_label, 5);
	adjust_layout->setStretchFactor(delta_label, 2);

	QHBoxLayout* adjust_layout1 = new QHBoxLayout;
	slide_label_x = new QLabel(tr("Xƽ��"));
	slide_label_x->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	slide_label_x->setAlignment(Qt::AlignCenter);
	slide_spin_x = new QDoubleSpinBox;
	slide_spin_x->setAlignment(Qt::AlignCenter);
	slide_spin_x->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	slide_spin_x->setSingleStep(0.0);
	slide_spin_x->setRange(-200, 200);//��������ô��ķ�Χ��
	slide_spin_x->setKeyboardTracking(false);
	//slide_spin_x->setSuffix(tr(" (����)"));
	//connect
	connect(slide_spin_x, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &reposturedialog::My_DoubleSpinBox_ValidChanged1,Qt::UniqueConnection);
	spin_line_xdelta = new QLineEdit;
	spin_line_xdelta->setValidator(real_expvalidator);
	spin_line_xdelta->setText(QString::number(0.0));
	spin_line_xdelta->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spin_line_xdelta->setAlignment(Qt::AlignCenter);
	connect(spin_line_xdelta, &QLineEdit::textChanged,[this](QString str) {
		if (str.isEmpty())
			spin_line_xdelta->setText(QString::number(0));
		else
			slide_spin_x->setSingleStep(str.toDouble());
	});
	adjust_layout1->addWidget(slide_label_x);
	adjust_layout1->addWidget(slide_spin_x);
	adjust_layout1->addWidget(spin_line_xdelta);
	adjust_layout1->setStretchFactor(slide_label_x, 3);
	adjust_layout1->setStretchFactor(slide_spin_x, 5);
	adjust_layout1->setStretchFactor(spin_line_xdelta, 2);

	QHBoxLayout* adjust_layout2 = new QHBoxLayout;
	slide_label_y = new QLabel(tr("Yƽ��"));
	slide_label_y->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	slide_label_y->setAlignment(Qt::AlignCenter);
	slide_spin_y = new QDoubleSpinBox;
	slide_spin_y->setAlignment(Qt::AlignCenter);
	slide_spin_y->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	slide_spin_y->setSingleStep(0.0);
	slide_spin_y->setRange(-200, 200);//��������ô��ķ�Χ��
	slide_spin_y->setKeyboardTracking(false);
	//slide_spin_y->setSuffix(tr(" (����)"));
	connect(slide_spin_y, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &reposturedialog::My_DoubleSpinBox_ValidChanged1, Qt::UniqueConnection);
	spin_line_ydelta = new QLineEdit;
	spin_line_ydelta->setValidator(real_expvalidator);
	spin_line_ydelta->setText(QString::number(0.0));
	spin_line_ydelta->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spin_line_ydelta->setAlignment(Qt::AlignCenter);
	connect(spin_line_ydelta, &QLineEdit::textChanged, [this](QString str) {
		if (str.isEmpty())
			spin_line_ydelta->setText(QString::number(0));
		else
			slide_spin_y->setSingleStep(str.toDouble());
	});
	adjust_layout2->addWidget(slide_label_y);
	adjust_layout2->addWidget(slide_spin_y);
	adjust_layout2->addWidget(spin_line_ydelta);
	adjust_layout2->setStretchFactor(slide_label_y, 3);
	adjust_layout2->setStretchFactor(slide_spin_y, 5);
	adjust_layout2->setStretchFactor(spin_line_ydelta, 2);

	QHBoxLayout* adjust_layout3 = new QHBoxLayout;
	slide_label_z = new QLabel(tr("Zƽ��"));
	slide_label_z->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	slide_label_z->setAlignment(Qt::AlignCenter);
	slide_spin_z = new QDoubleSpinBox;
	slide_spin_z->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	slide_spin_z->setAlignment(Qt::AlignCenter);
	slide_spin_z->setSingleStep(0.0);
	slide_spin_z->setRange(-200, 200);//��������ô��ķ�Χ��
	slide_spin_z->setKeyboardTracking(false);
	//slide_spin_z->setSuffix(tr(" (����)"));
	connect(slide_spin_z, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &reposturedialog::My_DoubleSpinBox_ValidChanged1, Qt::UniqueConnection);
	spin_line_zdelta = new QLineEdit;
	spin_line_zdelta->setValidator(real_expvalidator);
	spin_line_zdelta->setText(QString::number(0.0));
	spin_line_zdelta->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	spin_line_zdelta->setAlignment(Qt::AlignCenter);
	connect(spin_line_zdelta, &QLineEdit::textChanged, [this](QString str) {
		if (str.isEmpty())
			spin_line_zdelta->setText(QString::number(0));
		else
			slide_spin_z->setSingleStep(str.toDouble());
	});
	adjust_layout3->addWidget(slide_label_z);
	adjust_layout3->addWidget(slide_spin_z);
	adjust_layout3->addWidget(spin_line_zdelta);
	adjust_layout3->setStretchFactor(slide_label_z, 3);
	adjust_layout3->setStretchFactor(slide_spin_z, 5);
	adjust_layout3->setStretchFactor(spin_line_zdelta, 2);

	QHBoxLayout* adjust_layout4 = new QHBoxLayout;
	rotate_label_x = new QLabel(tr("X��ת"));
	rotate_label_x->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_label_x->setAlignment(Qt::AlignCenter);
	rotate_spin_x = new QDoubleSpinBox;
	rotate_spin_x->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_spin_x->setAlignment(Qt::AlignCenter);
	rotate_spin_x->setSingleStep(0.0);
	rotate_spin_x->setKeyboardTracking(false);
	//rotate_spin_x->setSuffix(tr(" (�Ƕ�)"));
	connect(rotate_spin_x, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &reposturedialog::My_DoubleSpinBox_ValidChanged1, Qt::UniqueConnection);
	rotate_spin_x->setRange(-180, 180);
	rotate_spin_x->setWrapping(true);
	rotate_line_xdelta = new QLineEdit;
	rotate_line_xdelta->setValidator(real_expvalidator);
	rotate_line_xdelta->setText(QString::number(0.0));
	rotate_line_xdelta->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_line_xdelta->setAlignment(Qt::AlignCenter);
	connect(rotate_line_xdelta, &QLineEdit::textChanged, [this](QString str) {
		if (str.isEmpty())
			rotate_line_xdelta->setText(QString::number(0));
		else
			rotate_spin_x->setSingleStep(str.toDouble());
	});
	adjust_layout4->addWidget(rotate_label_x);
	adjust_layout4->addWidget(rotate_spin_x);
	adjust_layout4->addWidget(rotate_line_xdelta);
	adjust_layout4->setStretchFactor(rotate_label_x, 3);
	adjust_layout4->setStretchFactor(rotate_spin_x, 5);
	adjust_layout4->setStretchFactor(rotate_line_xdelta, 2);

	QHBoxLayout* adjust_layout5 = new QHBoxLayout;
	rotate_label_y = new QLabel(tr("Y��ת"));
	rotate_label_y->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_label_y->setAlignment(Qt::AlignCenter);
	rotate_spin_y = new QDoubleSpinBox;
	rotate_spin_y->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_spin_y->setAlignment(Qt::AlignCenter);
	rotate_spin_y->setSingleStep(0.0);
	rotate_spin_y->setKeyboardTracking(false);
	//rotate_spin_y->setSuffix(tr(" (�Ƕ�)"));
	connect(rotate_spin_y, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &reposturedialog::My_DoubleSpinBox_ValidChanged1, Qt::UniqueConnection);
	rotate_spin_y->setRange(-180, 180);
	rotate_spin_y->setWrapping(true);
	rotate_line_ydelta = new QLineEdit;
	rotate_line_ydelta->setValidator(real_expvalidator);
	rotate_line_ydelta->setText(QString::number(0.0));
	rotate_line_ydelta->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_line_ydelta->setAlignment(Qt::AlignCenter);
	connect(rotate_line_ydelta, &QLineEdit::textChanged, [this](QString str) {
		if (str.isEmpty())
			rotate_line_ydelta->setText(QString::number(0));
		else
			rotate_spin_y->setSingleStep(str.toDouble());
	});
	adjust_layout5->addWidget(rotate_label_y);
	adjust_layout5->addWidget(rotate_spin_y);
	adjust_layout5->addWidget(rotate_line_ydelta);
	adjust_layout5->setStretchFactor(rotate_label_y, 3);
	adjust_layout5->setStretchFactor(rotate_spin_y, 5);
	adjust_layout5->setStretchFactor(rotate_line_ydelta, 2);

	QHBoxLayout* adjust_layout6 = new QHBoxLayout;
	rotate_label_z = new QLabel(tr("Z��ת"));
	rotate_label_z->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_label_z->setAlignment(Qt::AlignCenter);
	rotate_spin_z = new QDoubleSpinBox;
	rotate_spin_z->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_spin_z->setAlignment(Qt::AlignCenter);
	rotate_spin_z->setSingleStep(0.0);
	rotate_spin_z->setKeyboardTracking(false);
	//rotate_spin_z->setSuffix(tr(" (�Ƕ�)"));
	connect(rotate_spin_z, static_cast<void(QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged), this, &reposturedialog::My_DoubleSpinBox_ValidChanged1, Qt::UniqueConnection);
	rotate_spin_z->setRange(-180, 180);
	rotate_spin_z->setWrapping(true);
	rotate_line_zdelta = new QLineEdit;
	rotate_line_zdelta->setValidator(real_expvalidator);
	rotate_line_zdelta->setText(QString::number(0.0));
	rotate_line_zdelta->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	rotate_line_zdelta->setAlignment(Qt::AlignCenter);
	connect(rotate_line_zdelta, &QLineEdit::textChanged, [this](QString str) {
		if (str.isEmpty())
			rotate_line_zdelta->setText(QString::number(0));
		else
			rotate_spin_z->setSingleStep(str.toDouble());
	});
	adjust_layout6->addWidget(rotate_label_z);
	adjust_layout6->addWidget(rotate_spin_z);
	adjust_layout6->addWidget(rotate_line_zdelta);
	adjust_layout6->setStretchFactor(rotate_label_z, 3);
	adjust_layout6->setStretchFactor(rotate_spin_z, 5);
	adjust_layout6->setStretchFactor(rotate_line_zdelta, 2);

	adjust_mainlayout->addLayout(adjust_layout);
	adjust_mainlayout->addLayout(adjust_layout1);
	adjust_mainlayout->addLayout(adjust_layout2);
	adjust_mainlayout->addLayout(adjust_layout3);
	adjust_mainlayout->addLayout(adjust_layout4);
	adjust_mainlayout->addLayout(adjust_layout5);
	adjust_mainlayout->addLayout(adjust_layout6);
	adjust_mainlayout->setStretchFactor(adjust_layout, 1);
	adjust_mainlayout->setStretchFactor(adjust_layout1, 2);
	adjust_mainlayout->setStretchFactor(adjust_layout2, 2);
	adjust_mainlayout->setStretchFactor(adjust_layout3, 2);
	adjust_mainlayout->setStretchFactor(adjust_layout4, 2);
	adjust_mainlayout->setStretchFactor(adjust_layout5, 2);
	adjust_mainlayout->setStretchFactor(adjust_layout6, 2);
	adjust_group->setLayout(adjust_mainlayout);

	//���������ť:�ָ�,����
	QPushButton* restore_button = new QPushButton(tr("�ָ�"));
	restore_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(restore_button, &QPushButton::clicked, this, &reposturedialog::On_restorebutton_clicked);
	QPushButton* close_button = new QPushButton(tr("���沢�ر�"));
	close_button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	connect(close_button, &QPushButton::clicked, this, &reposturedialog::On_savebutton_clicked);
	QHBoxLayout* button_layout = new QHBoxLayout;
	button_layout->addStretch(1);
	button_layout->addWidget(restore_button);
	button_layout->addStretch(1);
	button_layout->addWidget(close_button);
	button_layout->addStretch(1);

	if (consConnBox->count() > 1)
	{
		QVariant v(0);
		consConnBox->setItemData(0, v, Qt::UserRole - 1);
		vector<int> hasworld_index;
		int index = 0;
		// ö��ÿһ��
		for (auto iter = conns_record.begin(); iter != conns_record.end(); ++iter)
		{
			string comp_conn_name = iter->toStdString();
			if (comp_conn_name == "��ѡ��")
			{
				++index;
				continue;
			}
			queue<string> Q;
			Q.emplace(comp_conn_name);
			bool hasWorld = false;
			unordered_set<string> visited;
			while (!Q.empty())
			{
				auto curr_comp_conn_name = Q.front();
				Q.pop();
				int pos = curr_comp_conn_name.find('+');
				string curr_comp_name = curr_comp_conn_name.substr(0, pos);
				string curr_conn_name = curr_comp_conn_name.substr(pos + 1, curr_comp_conn_name.length() - pos - 1);
				if (curr_comp_name == "world")
				{
					hasWorld = true;
					break;
				}
				visited.insert(curr_comp_name);
				for (auto other_comp_conn_name : name_map[curr_comp_name])
				{
					int other_pos = other_comp_conn_name.find('+');
					string other_conn_name = other_comp_conn_name.substr(pos + 1, other_comp_conn_name.length() - pos - 1);
					if (other_conn_name == curr_conn_name)
					{
						continue;
					}
					string assembly_comp_conn_name = assembly_graph[other_comp_conn_name];
					int assembly_pos = assembly_comp_conn_name.find('+');
					string assembly_comp = assembly_comp_conn_name.substr(0, assembly_pos);
					if (!visited.count(assembly_comp))
					{
						Q.emplace(assembly_comp_conn_name);
					}
				}
			}
			if (hasWorld)
			{
				hasworld_index.push_back(index);
			}
			++index;
		}
		for (int i : hasworld_index)
		{
			QVariant vt(0);
			consConnBox->setItemData(i, vt, Qt::UserRole - 1);
		}
	}
	if (consConnBox->count() == 1)
	{
		// ȷ����Ӧ�Ľӿ�
		string curr_comp_conn_name = consConnBox->currentText().toStdString();
		int pos = curr_comp_conn_name.find('+');
		string curr_comp_name = curr_comp_conn_name.substr(0, pos);
		string curr_conn_name = curr_comp_conn_name.substr(pos + 1, curr_comp_conn_name.length() - pos - 1);
		Component* curr_comp = _p_Cmd->getComproot()->GetChild(curr_comp_name.c_str());
		Connector* curr_conn = dynamic_cast<DocumentComponent*>(curr_comp->GetDocument())->GetConnector(curr_conn_name.c_str());
		dynamic_cast<RepostureCommand*>(_p_Cmd)->SetConnector(curr_conn);
		string assembly_comp_conn_name = assembly_graph[curr_comp_conn_name];
		int assembly_pos = assembly_comp_conn_name.find('+');
		string assembly_comp_name = assembly_comp_conn_name.substr(0, assembly_pos);
		string assembly_conn_name = assembly_comp_conn_name.substr(assembly_pos + 1, assembly_comp_conn_name.length() - assembly_pos - 1);
		Component* assembly_comp = _p_Cmd->getComproot()->GetChild(assembly_comp_name.c_str());
		dynamic_cast<RepostureCommand*>(_p_Cmd)->SetConsComponent(assembly_comp);
		Connector* assembly_conn = dynamic_cast<DocumentComponent*>(assembly_comp->GetDocument())->GetConnector(assembly_conn_name.c_str());
		dynamic_cast<RepostureCommand*>(_p_Cmd)->SetConsConnector(assembly_conn);
		// ����ѡ��Ľӿ������ж����õ�ģʽ
		// ͨ���ӿڽ��е���,��Ҫ�жϽӿڵ�����
		// �򸱡��ƶ�������ת��
		string conntype_name = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConnector()->GetConnType();
		//��ȡ��ʾ���͵��ֶ�,������֮����ַ���
		int index = 0;
		int cnt = 0;
		while (cnt < 2)
		{
			if (conntype_name[index] == '.')
			{
				cnt++;
			}
			index++;
		}
		string type_name;
		while (conntype_name[index] != '.')
		{
			type_name += conntype_name[index++];
		}
		if (type_name == "Rotational")
		{
			this->SetRotatePattarn();
		}
		else if (type_name == "Translational")
		{
			this->SetSlidePattarn();
		}
		else if (type_name == "��")
		{
			this->SetSpherePattern();
		}
		else
		{
			//ͳһ�������������,���ǽӿ�������Ҫ��ȡ�����ܸı�
			this->SetOtherPattern();
		}
	}
	else
	{
		connect(consConnBox, &QComboBox::currentTextChanged, [this]() {
			if (consConnBox->currentIndex() == 0)
			{
				QMessageBox::warning(this, QString("warning"), QString("��ѡ����ȷ�Ľӿ�"));
				return;
			}
			change_consComboBox = true;
			// ȷ����Ӧ�Ľӿ�
			string curr_comp_conn_name = consConnBox->currentText().toStdString();
			int pos = curr_comp_conn_name.find('+');
			string curr_comp_name = curr_comp_conn_name.substr(0, pos);
			string curr_conn_name = curr_comp_conn_name.substr(pos + 1, curr_comp_conn_name.length() - pos - 1);
			Component* curr_comp = _p_Cmd->getComproot()->GetChild(curr_comp_name.c_str());
			Connector* curr_conn = dynamic_cast<DocumentComponent*>(curr_comp->GetDocument())->GetConnector(curr_conn_name.c_str());
			dynamic_cast<RepostureCommand*>(_p_Cmd)->SetConnector(curr_conn);
			string assembly_comp_conn_name = assembly_graph[curr_comp_conn_name];
			int assembly_pos = assembly_comp_conn_name.find('+');
			string assembly_comp_name = assembly_comp_conn_name.substr(0, assembly_pos);
			string assembly_conn_name = assembly_comp_conn_name.substr(assembly_pos + 1, assembly_comp_conn_name.length() - assembly_pos - 1);
			Component* assembly_comp = _p_Cmd->getComproot()->GetChild(assembly_comp_name.c_str());
			dynamic_cast<RepostureCommand*>(_p_Cmd)->SetConsComponent(assembly_comp);
			Connector* assembly_conn = dynamic_cast<DocumentComponent*>(assembly_comp->GetDocument())->GetConnector(assembly_conn_name.c_str());
			dynamic_cast<RepostureCommand*>(_p_Cmd)->SetConsConnector(assembly_conn);
			// ����ѡ��Ľӿ������ж����õ�ģʽ
			// ͨ���ӿڽ��е���,��Ҫ�жϽӿڵ�����
			// �򸱡��ƶ�������ת��
			string conntype_name = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConnector()->GetConnType();
			//��ȡ��ʾ���͵��ֶ�,������֮����ַ���
			int index = 0;
			int cnt = 0;
			while (cnt < 2)
			{
				if (conntype_name[index] == '.')
				{
					cnt++;
				}
				index++;
			}
			string type_name;
			while (conntype_name[index] != '.')
			{
				type_name += conntype_name[index++];
			}
			if (type_name == "Rotational")
			{
				this->SetRotatePattarn();
			}
			else if (type_name == "Translational")
			{
				this->SetSlidePattarn();
			}
			else if (type_name == "��")
			{
				this->SetSpherePattern();
			}
			else
			{
				//ͳһ�������������,���ǽӿ�������Ҫ��ȡ�����ܸı�
				this->SetOtherPattern();
			}
			});
	}

	//�ܲ���
	mainlayout->addLayout(title_layout);
	mainlayout->addWidget(coordinate_group);
	mainlayout->addWidget(adjust_group);
	mainlayout->addLayout(button_layout);
	mainlayout->setStretchFactor(title_layout, 1);
	mainlayout->setStretchFactor(coordinate_group, 2);
	mainlayout->setStretchFactor(adjust_group, 6);
	mainlayout->setStretchFactor(button_layout, 1);
}

QString reposturedialog::GetSelectedname()
{
	return QString::fromStdString(_p_Cmd->getSelectedname());
}

double reposturedialog::GetSelectedConnectorOrigin_X()
{
	return v9_read[0];
}

double reposturedialog::GetSelectedConnectorOrigin_Y()
{
	return v9_read[1];
}

double reposturedialog::GetSelectedConnectorOrigin_Z()
{
	return v9_read[2];
}

double reposturedialog::GetSelectedConnectorAxisZ_X()
{
	return v9_read[3];
}

double reposturedialog::GetSelectedConnectorAxisZ_Y()
{
	return v9_read[4];
}

double reposturedialog::GetSelectedConnectorAxisZ_Z()
{
	return v9_read[5];
}

double reposturedialog::GetSelectedConnectorAxisX_X()
{
	return v9_read[6];
}

double reposturedialog::GetSelectedConnectorAxisX_Y()
{
	return v9_read[7];
}

double reposturedialog::GetSelectedConnectorAxisX_Z()
{
	return v9_read[8];
}

void reposturedialog::closeEvent(QCloseEvent*)
{
	if (consConnBox->currentText() == QString("��ѡ��"))
	{
		return;
	}
	dynamic_cast<RepostureCommand*>(_p_Cmd)->clearstep();
	// ����װ��ͼ
	unordered_map<string, string> assembly_graph;
	unordered_map<string, unordered_set<string>> name_map;
	// deltaTransform���������һ��װ����ߵ��˵����
	unordered_map<string, pair<string, vector<double>>> last_reverse_deltatrans_record; // ���curr_comp�������assembly_comp�ĵ���,��������
	unordered_map<string, gp_Ax3> origin_Ax3_record; // �洢��װ��Լ���ӿڱ任֮ǰ��ȫ��gp_Ax3
	for (auto constraint : dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetAllConnConstraints())
	{
		assembly_graph[constraint.component1 + '+' + constraint.connector1] = constraint.component2 + '+' + constraint.connector2;
		assembly_graph[constraint.component2 + '+' + constraint.connector2] = constraint.component1 + '+' + constraint.connector1;
		name_map[constraint.component1].emplace(constraint.component1 + '+' + constraint.connector1);
		name_map[constraint.component2].emplace(constraint.component2 + '+' + constraint.connector2);
	}
	// ͳ��������Ҫ�任��comp
	unordered_set<string> trans_comp_name;
	vector<double> v15;
	for (int i = 0; i < 9; ++i)
	{
		v15.push_back(v9_read[i]);
	}
	for (int i = 0; i < 6; ++i)
	{
		v15.push_back(change_all[i]);
	}
	// ��һ���䶯������������v15����任
	// �任������ͽӿ�
	Component* comp = _p_Cmd->GetComponent();
	Connector* conn = _p_Cmd->GetConnector();
	// comp��deltatransformV15
	vector<double> comp_pre_v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(comp->GetCompName().c_str());
	//��������5.10
	unordered_set<string> visited;
	queue<string> Q;
	visited.insert(comp->GetCompName());
	for (auto& other_name : name_map[comp->GetCompName()])
	{
		if (other_name != comp->GetCompName() + '+' + conn->GetConnName())
		{
			int other_pos = other_name.find('+');
			string other_conn_name = other_name.substr(other_pos + 1, other_name.length() - other_pos - 1);
			Connector* other_conn = dynamic_cast<DocumentComponent*>(comp->GetDocument())->GetConnector(other_conn_name.c_str());
			// other_conn��globalAx3
			gp_Ax3 other_globalAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(comp->GetCompName().c_str(), other_conn_name.c_str());
			// ��¼
			origin_Ax3_record.emplace(other_name, other_globalAx3);
			// �ҵ�����װ������
			string assembly_comp_conn_name = assembly_graph[other_name];
			int assembly_pos = assembly_comp_conn_name.find('+');
			string assembly_comp_name = assembly_comp_conn_name.substr(0, assembly_pos);
			string assembly_conn_name = assembly_comp_conn_name.substr(assembly_pos + 1, assembly_comp_conn_name.length() - assembly_pos - 1);
			Component* assembly_comp = _p_Cmd->getComproot()->GetChild(assembly_comp_name.c_str());
			Connector* assembly_conn = dynamic_cast<DocumentComponent*>(assembly_comp->GetDocument())->GetConnector(assembly_conn_name.c_str());
			// assembly_conn��globalAx3
			gp_Ax3 assembly_globalAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(assembly_comp_name.c_str(), assembly_conn_name.c_str());
			// ���curr_comp���assembly_comp_conn�е�����Ϣ
			if (fabs(comp_pre_v15[0] - assembly_globalAx3.Location().X()) < LINEAR_TOL && fabs(comp_pre_v15[1] - assembly_globalAx3.Location().Y()) < LINEAR_TOL && fabs(comp_pre_v15[2] - assembly_globalAx3.Location().Z()) < LINEAR_TOL \
				&& fabs(comp_pre_v15[3] - assembly_globalAx3.Direction().X()) < LINEAR_TOL && fabs(comp_pre_v15[4] - assembly_globalAx3.Direction().Y()) < LINEAR_TOL && fabs(comp_pre_v15[5] - assembly_globalAx3.Direction().Z()) < LINEAR_TOL \
				&& fabs(comp_pre_v15[6] - assembly_globalAx3.XDirection().X()) < LINEAR_TOL && fabs(comp_pre_v15[7] - assembly_globalAx3.XDirection().Y()) < LINEAR_TOL && fabs(comp_pre_v15[8] - assembly_globalAx3.XDirection().Z()) < LINEAR_TOL)
			{
				last_reverse_deltatrans_record[assembly_comp_name] = { comp->GetCompName() + '+' + other_conn_name,comp_pre_v15 };
			}
			// ���뵽������
			Q.emplace(assembly_comp_conn_name);
		}
	}
	// �任comp���
	// comp��OriginTransform�ı�,��consComp��consConn
	Component* conscomponent = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsComponent();
	Connector* consconnector = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsConnector();
	// consconn�����conn��û�е�����Ϣ
	gp_Ax3 consconn_gAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(conscomponent->GetCompName().c_str(), consconnector->GetConnName().c_str());
	vector<double> cons_v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(conscomponent->GetCompName().c_str());
	gp_Ax3 conn_gAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(comp->GetCompName().c_str(), conn->GetConnName().c_str());
	gp_Ax3 comp_oAx3;
	gp_Trsf comp_oTrans;
	comp_oTrans.SetDisplacement(comp_oAx3, consconn_gAx3);
	// comp��conn�ֲ��任
	gp_Ax3 conn_lAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnLocalAx3(comp->GetCompName().c_str(), conn->GetConnName().c_str());
	gp_Ax3 comp_lAx3;
	gp_Trsf temp_trans;
	temp_trans.SetDisplacement(conn_lAx3, comp_lAx3);
	comp->SetOriginTransform(comp_oTrans * temp_trans);
	gp_Trsf compOtrans = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompOriginTransform(comp->GetCompName().c_str());
	if (fabs(cons_v15[0] - conn_gAx3.Location().X()) < LINEAR_TOL && fabs(cons_v15[1] - conn_gAx3.Location().Y()) < LINEAR_TOL && fabs(cons_v15[2] - conn_gAx3.Location().Z()) < LINEAR_TOL \
		&& fabs(cons_v15[3] - conn_gAx3.Direction().X()) < LINEAR_TOL && fabs(cons_v15[4] - conn_gAx3.Direction().Y()) < LINEAR_TOL && fabs(cons_v15[5] - conn_gAx3.Direction().Z()) < LINEAR_TOL \
		&& fabs(cons_v15[6] - conn_gAx3.XDirection().X()) < LINEAR_TOL && fabs(cons_v15[7] - conn_gAx3.XDirection().Y()) < LINEAR_TOL && fabs(cons_v15[8] - conn_gAx3.XDirection().Z()) < LINEAR_TOL)
	{
		// ����connΪcons�ķ��任,������cons��v15��Ӧλ�ü�ȥ��Ӧֵ
		for (int i = 9; i < 15; ++i)
		{
			v15[i] -= cons_v15[i];
		}
	}
	comp->SetDeltaTransform(v15);
	// ������������б任,�����漰���˵�v15,ֻ����װ�������е���,��װ�������ͬ
	while (!Q.empty())
	{
		string curr_name = Q.front();
		int curr_pos = curr_name.find('+');
		string curr_comp_name = curr_name.substr(0, curr_pos);
		string curr_conn_name = curr_name.substr(curr_pos + 1, curr_name.length() - curr_pos - 1);
		Q.pop();
		Component* curr_comp = _p_Cmd->getComproot()->GetChild(curr_comp_name.c_str());
		// ���ڸ�����������ӿ�
		for (auto& other_name : name_map[curr_comp_name])
		{
			int other_pos = other_name.find('+');
			string other_conn_name = other_name.substr(other_pos + 1, other_name.length() - other_pos - 1);
			Connector* other_conn = dynamic_cast<DocumentComponent*>(curr_comp->GetDocument())->GetConnector(other_conn_name.c_str());
			// other_conn��globalAx3
			gp_Ax3 other_globalAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(curr_comp_name.c_str(), other_conn_name.c_str());
			// ��¼
			origin_Ax3_record.emplace(other_name, other_globalAx3);
			if (other_conn_name != curr_conn_name)
			{
				// �ҵ�����������������ͽӿ�
				string assembly_name = assembly_graph[other_name];
				int pos = assembly_name.find('+');
				string assembly_comp_name = assembly_name.substr(0, pos);
				string assembly_conn_name = assembly_name.substr(pos + 1, assembly_name.length() - pos - 1);
				// ������֮ǰ���ʹ���
				if (visited.count(assembly_comp_name))
				{
					continue;
				}
				visited.insert(assembly_comp_name);
				Component* assembly_comp = _p_Cmd->getComproot()->GetChild(assembly_comp_name.c_str());
				Connector* assembly_conn = dynamic_cast<DocumentComponent*>(assembly_comp->GetDocument())->GetConnector(assembly_conn_name.c_str());
				// �õ�curr_comp��deltaTransv15
				vector<double> curr_comp_v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(curr_comp_name.c_str());
				// ���curr_comp_v15��ǰ9��ֵ��assembly_comp_conn��gp_Ax3���,����ڱ任,��Ҫ��assembly_comp��һ���෴�ı任,�ȼ�¼����
				gp_Ax3 assembly_globalAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(assembly_comp_name.c_str(), assembly_conn_name.c_str());
				if (fabs(curr_comp_v15[0] - assembly_globalAx3.Location().X()) < LINEAR_TOL && fabs(curr_comp_v15[1] - assembly_globalAx3.Location().Y()) < LINEAR_TOL && fabs(curr_comp_v15[2] - assembly_globalAx3.Location().Z()) < LINEAR_TOL \
					&& fabs(curr_comp_v15[3] - assembly_globalAx3.Direction().X()) < LINEAR_TOL && fabs(curr_comp_v15[4] - assembly_globalAx3.Direction().Y()) < LINEAR_TOL && fabs(curr_comp_v15[5] - assembly_globalAx3.Direction().Z()) < LINEAR_TOL \
					&& fabs(curr_comp_v15[6] - assembly_globalAx3.XDirection().X()) < LINEAR_TOL && fabs(curr_comp_v15[7] - assembly_globalAx3.XDirection().Y()) < LINEAR_TOL && fabs(curr_comp_v15[8] - assembly_globalAx3.XDirection().Z()) < LINEAR_TOL)
				{
					// assembly_comp��Ҫ�ı任������
					last_reverse_deltatrans_record[assembly_comp_name] = { curr_comp_name + '+' + other_conn_name,curr_comp_v15 };
					// curr_comp��deltaTrans���
					vector<double> reset_v15{ 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
					curr_comp->SetDeltaTransform(reset_v15);
				}
				//���뵽������
				Q.emplace(assembly_name);
			}
		}
		// ����curr_comp,��������last_comp+last_conn�е��˱任,����Ҫ����(last_comp+last_connֱ��ͨ��װ��ͼ�õ�)
		if (last_reverse_deltatrans_record.count(curr_comp_name))
		{
			string last_comp_conn_name = last_reverse_deltatrans_record[curr_comp_name].first;
			int last_pos = last_comp_conn_name.find('+');
			string last_comp_name = last_comp_conn_name.substr(0, last_pos);
			string last_conn_name = last_comp_conn_name.substr(last_pos + 1, last_comp_conn_name.length() - last_pos - 1);
			// �õ�last_comp_conn��ǰ��globalAx3
			gp_Ax3 last_comp_conn_gAx3_current = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(last_comp_name.c_str(), last_conn_name.c_str());
			// �ҵ�����Ҫ���б任����һ��װ��ӿں�v15����
			vector<double> v15_pre = last_reverse_deltatrans_record[curr_comp_name].second;
			vector<double> curr_displament = { last_comp_conn_gAx3_current.Location().X(),last_comp_conn_gAx3_current.Location().Y(), last_comp_conn_gAx3_current.Location().Z(), \
				last_comp_conn_gAx3_current.Direction().X(), last_comp_conn_gAx3_current.Direction().Y(), last_comp_conn_gAx3_current.Direction().Z(), \
				last_comp_conn_gAx3_current.XDirection().X(), last_comp_conn_gAx3_current.XDirection().Y(), last_comp_conn_gAx3_current.XDirection().Z() };
			vector<double> new_v15(curr_displament);
			// curr_comp��Ҫ�����last_comp��һ���෴�ı任
			for (int i = 9; i < 15; ++i)
			{
				new_v15.push_back(-v15_pre[i]);
			}
			curr_comp->SetDeltaTransform(new_v15);

			// ����curr_comp���б任
			gp_Ax3 oAx3;
			gp_Trsf oTrans;
			oTrans.SetDisplacement(oAx3, last_comp_conn_gAx3_current);
			// curr_conn�ֲ��ķ��任
			gp_Trsf tempTrans;
			gp_Ax3 curr_conn_localAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnLocalAx3(curr_comp_name.c_str(), curr_conn_name.c_str());
			tempTrans.SetDisplacement(curr_conn_localAx3, oAx3);
			curr_comp->SetOriginTransform(oTrans * tempTrans);
		}
		else if(assembly_graph.count(curr_comp_name+'+'+curr_conn_name))
		{
			// last_comp���curr_compû�е��˱任
			// ����curr_comp���б任
			string last_comp_conn_name = assembly_graph[curr_name];
			int last_pos = last_comp_conn_name.find('+');
			string last_comp_name = last_comp_conn_name.substr(0, last_pos);
			string last_conn_name = last_comp_conn_name.substr(last_pos + 1, last_comp_conn_name.length() - last_pos - 1);
			gp_Ax3 oAx3;
			gp_Trsf oTrans;
			gp_Ax3 last_comp_conn_gAx3_current = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(last_comp_name.c_str(), last_conn_name.c_str());
			oTrans.SetDisplacement(oAx3, last_comp_conn_gAx3_current);
			// curr_conn�ֲ��ķ��任
			gp_Trsf tempTrans;
			gp_Ax3 curr_conn_localAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnLocalAx3(curr_comp_name.c_str(), curr_conn_name.c_str());
			tempTrans.SetDisplacement(curr_conn_localAx3, oAx3);
			curr_comp->SetOriginTransform(oTrans* tempTrans);
			// ���curr_comp�����last_comp�е�����Ϣ,��Ҫ����
			if (origin_Ax3_record.count(last_comp_conn_name))
			{
				gp_Ax3 origin_Ax3 = origin_Ax3_record[last_comp_conn_name];
				Component* last_comp = dynamic_cast<RepostureCommand*>(_p_Cmd)->getComproot()->GetChild(last_comp_name.c_str());
				Connector* last_conn = dynamic_cast<DocumentComponent*>(last_comp->GetDocument())->GetConnector(last_conn_name.c_str());
				gp_Ax3 new_Ax3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(last_comp_name.c_str(), last_conn_name.c_str());
				vector<double> curr_v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(curr_comp_name.c_str());
				if (fabs(curr_v15[0] - origin_Ax3.Location().X()) < LINEAR_TOL && fabs(curr_v15[1] - origin_Ax3.Location().Y()) < LINEAR_TOL && fabs(curr_v15[2] - origin_Ax3.Location().Z()) < LINEAR_TOL\
					&& fabs(curr_v15[3] - origin_Ax3.Direction().X()) < LINEAR_TOL && fabs(curr_v15[4] - origin_Ax3.Direction().Y()) < LINEAR_TOL && fabs(curr_v15[5] - origin_Ax3.Direction().Z()) < LINEAR_TOL\
					&& fabs(curr_v15[6] - origin_Ax3.XDirection().X()) < LINEAR_TOL && fabs(curr_v15[7] - origin_Ax3.XDirection().Y()) < LINEAR_TOL && fabs(curr_v15[8] - origin_Ax3.XDirection().Z()) < LINEAR_TOL)
				{
					// ��Ҫ����curr_v15
					curr_v15[0] = new_Ax3.Location().X(), curr_v15[1] = new_Ax3.Location().Y(), curr_v15[2] = new_Ax3.Location().Z();
					curr_v15[3] = new_Ax3.Direction().X(), curr_v15[4] = new_Ax3.Direction().Y(), curr_v15[5] = new_Ax3.Direction().Z();
					curr_v15[6] = new_Ax3.XDirection().X(), curr_v15[7] = new_Ax3.XDirection().Y(), curr_v15[8] = new_Ax3.XDirection().Z();
					// ����6����̬��Ϣ����Ҫ�ı�
					curr_comp->SetDeltaTransform(curr_v15);
				}
			}
		}
		trans_comp_name.insert(curr_comp_name);
	}
	gp_Trsf compOtrans1 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompOriginTransform(comp->GetCompName().c_str());
	vector<double> v15t = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(comp->GetCompName().c_str());
	gp_Ax3 compAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(comp->GetCompName().c_str(), "frame_b");
	comp->Display(_p_Cmd->getCommandView()->GetContext());
	dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->regenerateConn(comp);
	for (auto name : trans_comp_name)
	{
		Component* comp1 = _p_Cmd->getComproot()->GetChild(name.c_str());
		gp_Trsf compOtranst = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompOriginTransform(comp1->GetCompName().c_str());
		vector<double> v15t = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(comp1->GetCompName().c_str());
		comp1->Display(_p_Cmd->getCommandView()->GetContext());
		dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->regenerateConn(comp1);
	}
	this->_p_Cmd->getCommandView()->StateChanged(QString("�������"));
}

void reposturedialog::CommonSettings()
{
	getConDisp();
	QRegExp remove_zero_exp("(\\.)?0+$");
	QString origin_s1 = QString::number(GetSelectedConnectorOrigin_X(), 'f', 3).replace(remove_zero_exp, "");
	QString origin_s2 = QString::number(GetSelectedConnectorOrigin_Y(), 'f', 3).replace(remove_zero_exp, "");
	QString origin_s3 = QString::number(GetSelectedConnectorOrigin_Z(), 'f', 3).replace(remove_zero_exp, "");
	QString origin_s = "(" + origin_s1 + "," + origin_s2 + "," + origin_s3 + ")";
	origin_line->blockSignals(true);
	origin_line->setText(origin_s);
	origin_line->blockSignals(false);
	QString Z_s1 = QString::number(GetSelectedConnectorAxisZ_X(), 'f', 3).replace(remove_zero_exp, "");
	QString Z_s2 = QString::number(GetSelectedConnectorAxisZ_Y(), 'f', 3).replace(remove_zero_exp, "");
	QString Z_s3 = QString::number(GetSelectedConnectorAxisZ_Z(), 'f', 3).replace(remove_zero_exp, "");
	QString Z_s = "(" + Z_s1 + "," + Z_s2 + "," + Z_s3 + ")";
	Axis_Z->blockSignals(true);
	Axis_Z->setText(Z_s);
	Axis_Z->blockSignals(false);
	QString X_s1 = QString::number(GetSelectedConnectorAxisX_X(), 'f', 3).replace(remove_zero_exp, "");
	QString X_s2 = QString::number(GetSelectedConnectorAxisX_Y(), 'f', 3).replace(remove_zero_exp, "");
	QString X_s3 = QString::number(GetSelectedConnectorAxisX_Z(), 'f', 3).replace(remove_zero_exp, "");
	QString X_s = "(" + X_s1 + "," + X_s2 + "," + X_s3 + ")";
	Axis_X->blockSignals(true);
	Axis_X->setText(X_s);
	Axis_X->blockSignals(false);

	origin_line->setReadOnly(true);
	Axis_Z->setReadOnly(true);
	Axis_X->setReadOnly(true);
}

//void reposturedialog::SetFreePattern()
//{
//	//��ȡ֮ǰ������ƽ����ת��Ϣ
//	vector<double> v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(_p_Cmd->getSelectedname().c_str());
//	if (v15.empty())
//	{
//		slide_spin_x->setValue(0);
//		slide_spin_y->setValue(0);
//		slide_spin_z->setValue(0);
//		rotate_spin_x->setValue(0);
//		rotate_spin_y->setValue(0);
//		rotate_spin_z->setValue(0);
//	}
//	else
//	{
//		slide_spin_x->setValue(v15[9]);
//		last_record[9] = v15[9];
//		last_change[0] = v15[9];
//		slide_spin_y->setValue(v15[10]);
//		last_record[10] = v15[10];
//		last_change[1] = v15[10];
//		slide_spin_z->setValue(v15[11]);
//		last_record[11] = v15[11];
//		last_change[2] = v15[11];
//		rotate_spin_x->setValue(v15[12] * 180 / PI);
//		last_record[12] = v15[12] * 180 / PI;
//		last_change[3] = last_record[12];
//		rotate_spin_y->setValue(v15[13] * 180 / PI);
//		last_record[13] = v15[13] * 180 / PI;
//		last_change[4] = last_record[13];
//		rotate_spin_z->setValue(v15[14] * 180 / PI);
//		last_record[14] = v15[14] * 180 / PI;
//		last_change[5] = last_record[14];
//	}
//}

void reposturedialog::SetRotatePattarn()
{
	CommonSettings();

	slide_label_x->setDisabled(true);
	slide_spin_x->setDisabled(true);
	spin_line_xdelta->setDisabled(true);
	slide_label_y->setDisabled(true);
	slide_spin_y->setDisabled(true);
	spin_line_ydelta->setDisabled(true);
	slide_label_z->setDisabled(true);
	slide_spin_z->setDisabled(true);
	spin_line_zdelta->setDisabled(true);

	rotate_label_x->setDisabled(true);
	rotate_spin_x->setDisabled(true);
	rotate_line_xdelta->setDisabled(true);
	rotate_label_y->setDisabled(true);
	rotate_spin_y->setDisabled(true);
	rotate_line_ydelta->setDisabled(true);

	//���֮ǰ����תֵ
	vector<double> v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(_p_Cmd->getSelectedname().c_str());
	// �Ƿ�v15�����ǰ9��ֵ�Ǳ�װ��ӿڵ�����?
	Component* conscomp = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsComponent();
	Connector* consconn = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsConnector();
	gp_Ax3 consAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(conscomp->GetCompName().c_str(), consconn->GetConnName().c_str());
	bool flag = false;
	if (fabs(v15[0] - consAx3.Location().X()) < LINEAR_TOL && fabs(v15[1] - consAx3.Location().Y()) < LINEAR_TOL && fabs(v15[2] - consAx3.Location().Z()) < LINEAR_TOL\
		&& fabs(v15[3] - consAx3.Direction().X()) < LINEAR_TOL && fabs(v15[4] - consAx3.Direction().Y()) < LINEAR_TOL && fabs(v15[5] - consAx3.Direction().Z()) < LINEAR_TOL\
		&& fabs(v15[6] - consAx3.XDirection().X()) < LINEAR_TOL && fabs(v15[7] - consAx3.XDirection().Y()) < LINEAR_TOL && fabs(v15[8] - consAx3.XDirection().Z()) < LINEAR_TOL)
	{
		flag = true;
	}
	if (v15.empty() || flag == false)
	{
		first_read[5] = false;
		rotate_spin_z->setValue(0);
		change_consComboBox = false;
	}
	else
	{
		if (v15[14] == 0)
		{
			first_read[5] = false;
		}
		rotate_spin_z->setValue(v15[14] * 180 / PI);
		last_record[14] = v15[14] * 180 / PI;
		last_change[5] = last_record[14];
		if (v15[9] == 0 && v15[10] == 0 && v15[11] == 0 && v15[12] == 0 && v15[13] == 0 && v15[14] == 0)
		{
			change_consComboBox = false;
		}
	}
}

void reposturedialog::SetSlidePattarn()
{
	CommonSettings();

	slide_label_x->setDisabled(true);
	slide_spin_x->setDisabled(true);
	spin_line_xdelta->setDisabled(true);
	slide_label_y->setDisabled(true);
	slide_spin_y->setDisabled(true);
	spin_line_ydelta->setDisabled(true);

	rotate_label_x->setDisabled(true);
	rotate_spin_x->setDisabled(true);
	rotate_line_xdelta->setDisabled(true);
	rotate_label_y->setDisabled(true);
	rotate_spin_y->setDisabled(true);
	rotate_line_ydelta->setDisabled(true);
	rotate_label_z->setDisabled(true);
	rotate_spin_z->setDisabled(true);
	rotate_line_zdelta->setDisabled(true);

	//���֮ǰ��ƽ��ֵ
	vector<double> v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(_p_Cmd->getSelectedname().c_str());
	// �Ƿ�v15�����ǰ9��ֵ�Ǳ�װ��ӿڵ�����?
	Component* conscomp = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsComponent();
	Connector* consconn = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsConnector();
	gp_Ax3 consAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(conscomp->GetCompName().c_str(), consconn->GetConnName().c_str());
	bool flag = false;
	if (fabs(v15[0] - consAx3.Location().X()) < LINEAR_TOL && fabs(v15[1] - consAx3.Location().Y()) < LINEAR_TOL && fabs(v15[2] - consAx3.Location().Z()) < LINEAR_TOL\
		&& fabs(v15[3] - consAx3.Direction().X()) < LINEAR_TOL && fabs(v15[4] - consAx3.Direction().Y()) < LINEAR_TOL && fabs(v15[5] - consAx3.Direction().Z()) < LINEAR_TOL\
		&& fabs(v15[6] - consAx3.XDirection().X()) < LINEAR_TOL && fabs(v15[7] - consAx3.XDirection().Y()) < LINEAR_TOL && fabs(v15[8] - consAx3.XDirection().Z()) < LINEAR_TOL)
	{
		flag = true;
	}
	if (v15.empty() || flag == false)
	{
		first_read[2] = false;
		slide_spin_z->setValue(0);
		change_consComboBox = false;
	}
	else
	{
		if (v15[11] == 0)
		{
			first_read[2] = false;
		}
		slide_spin_z->setValue(v15[11]);
		last_record[11] = v15[11];
		last_record[2] = last_change[11];
		if (v15[9] == 0 && v15[10] == 0 && v15[11] == 0 && v15[12] == 0 && v15[13] == 0 && v15[14] == 0)
		{
			change_consComboBox = false;
		}
	}
}

void reposturedialog::SetSpherePattern()
{
	//X,Y,Z���������ת
	CommonSettings();

	slide_label_x->setDisabled(true);
	slide_spin_x->setDisabled(true);
	spin_line_xdelta->setDisabled(true);
	slide_label_y->setDisabled(true);
	slide_spin_y->setDisabled(true);
	spin_line_ydelta->setDisabled(true);
	slide_label_z->setDisabled(true);
	slide_spin_z->setDisabled(true);
	spin_line_zdelta->setDisabled(true);

	//���֮ǰ����תֵ
	vector<double> v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(_p_Cmd->getSelectedname().c_str());
	// �Ƿ�v15�����ǰ9��ֵ�Ǳ�װ��ӿڵ�����?
	Component* conscomp = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsComponent();
	Connector* consconn = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsConnector();
	gp_Ax3 consAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(conscomp->GetCompName().c_str(), consconn->GetConnName().c_str());
	bool flag = false;
	if (fabs(v15[0] - consAx3.Location().X()) < LINEAR_TOL && fabs(v15[1] - consAx3.Location().Y()) < LINEAR_TOL && fabs(v15[2] - consAx3.Location().Z()) < LINEAR_TOL\
		&& fabs(v15[3] - consAx3.Direction().X()) < LINEAR_TOL && fabs(v15[4] - consAx3.Direction().Y()) < LINEAR_TOL && fabs(v15[5] - consAx3.Direction().Z()) < LINEAR_TOL\
		&& fabs(v15[6] - consAx3.XDirection().X()) < LINEAR_TOL && fabs(v15[7] - consAx3.XDirection().Y()) < LINEAR_TOL && fabs(v15[8] - consAx3.XDirection().Z()) < LINEAR_TOL)
	{
		flag = true;
	}
	if (v15.empty() || flag == false)
	{
		first_read[3] = false;
		rotate_spin_x->setValue(0);
		first_read[4] = false;
		rotate_spin_y->setValue(0);
		first_read[5] = false;
		rotate_spin_z->setValue(0);
		change_consComboBox = false;
	}
	else
	{
		if (v15[12] == 0)
		{
			first_read[3] = false;
		}
		rotate_spin_x->setValue(v15[12] * 180 / PI);
		last_record[12] = v15[12] * 180 / PI;
		last_change[3] = last_record[12];
		if (v15[13] == 0)
		{
			first_read[4] = false;
		}
		rotate_spin_y->setValue(v15[13] * 180 / PI);
		last_record[13] = v15[13] * 180 / PI;
		last_change[4] = last_record[13];
		if (v15[14] == 0)
		{
			first_read[5] = false;
		}
		rotate_spin_z->setValue(v15[14] * 180 / PI);
		last_record[14] = v15[14] * 180 / PI;
		last_change[5] = last_record[14];
		if (v15[9] == 0 && v15[10] == 0 && v15[11] == 0 && v15[12] == 0 && v15[13] == 0 && v15[14] == 0)
		{
			change_consComboBox = false;
		}
	}
}

void reposturedialog::SetOtherPattern()
{
	//�����ӿ�,�ӿ���ص�v9�������޸�,����ƽ�ƺ���ת�������޸�
	CommonSettings();
	//��ȡ֮ǰ������ƽ����ת��Ϣ
	vector<double> v15 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompDeltaTransformV15(_p_Cmd->getSelectedname().c_str());
	// �Ƿ�v15�����ǰ9��ֵ�Ǳ�װ��ӿڵ�����?
	Component* conscomp = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsComponent();
	Connector* consconn = dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsConnector();
	gp_Ax3 consAx3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(conscomp->GetCompName().c_str(), consconn->GetConnName().c_str());
	bool flag = false;
	if (fabs(v15[0] - consAx3.Location().X()) < LINEAR_TOL && fabs(v15[1] - consAx3.Location().Y()) < LINEAR_TOL && fabs(v15[2] - consAx3.Location().Z()) < LINEAR_TOL\
		&& fabs(v15[3] - consAx3.Direction().X()) < LINEAR_TOL && fabs(v15[4] - consAx3.Direction().Y()) < LINEAR_TOL && fabs(v15[5] - consAx3.Direction().Z()) < LINEAR_TOL\
		&& fabs(v15[6] - consAx3.XDirection().X()) < LINEAR_TOL && fabs(v15[7] - consAx3.XDirection().Y()) < LINEAR_TOL && fabs(v15[8] - consAx3.XDirection().Z()) < LINEAR_TOL)
	{
		flag = true;
	}
	// v15Ϊ�ջ���Լ���ӿڲ��ǵ�ǰѡ��Ľӿ�,֤�����ڵ�ǰѡ��Ľӿ�û�е�����Ϣ
	if (v15.empty() || flag == false)
	{
		slide_spin_x->setValue(0);
		first_read[0] = false;
		slide_spin_y->setValue(0);
		first_read[1] = false;
		slide_spin_z->setValue(0);
		first_read[2] = false;
		rotate_spin_x->setValue(0);
		first_read[3] = false;
		rotate_spin_y->setValue(0);
		first_read[4] = false;
		rotate_spin_z->setValue(0);
		first_read[5] = false;
		change_consComboBox = false;
	}
	else
	{
		//��Ҫ�ж��ǲ���һ��ʼ�����������ݵ���
		if (v15[9] == 0)
		{
			first_read[0] = false;
		}
		slide_spin_x->setValue(v15[9]);
		last_record[9] = v15[9];
		last_change[0] = v15[9];
		if (v15[10] == 0)
		{
			first_read[1] = false;
		}
		slide_spin_y->setValue(v15[10]);
		last_record[10] = v15[10];
		last_change[1] = v15[10];
		if (v15[11] == 0)
		{
			first_read[2] = false;
		}
		slide_spin_z->setValue(v15[11]);
		last_record[11] = v15[11];
		last_change[2] = v15[11];
		if (v15[12] == 0)
		{
			first_read[3] = false;
		}
		rotate_spin_x->setValue(v15[12] * 180 / PI);
		last_record[12] = v15[12] * 180 / PI;
		last_change[3] = last_record[12];
		if (v15[13] == 0)
		{
			first_read[4] = false;
		}
		rotate_spin_y->setValue(v15[13] * 180 / PI);
		last_record[13] = v15[13] * 180 / PI;
		last_change[4] = last_record[13];
		if (v15[14] == 0)
		{
			first_read[5] = false;
		}
		rotate_spin_z->setValue(v15[14] * 180 / PI);
		last_record[14] = v15[14] * 180 / PI;
		last_change[5] = last_record[14];
		if (v15[9] == 0 && v15[10] == 0 && v15[11] == 0 && v15[12] == 0 && v15[13] == 0 && v15[14] == 0)
		{
			change_consComboBox = false;
		}
	}
}

//�õ���װ��ӿڵ���������
void reposturedialog::getConDisp()
{
	Connector* consconnector = dynamic_cast<RepostureCommand*>(this->_p_Cmd)->GetConsConnector();
	Component* conscomponent = dynamic_cast<RepostureCommand*>(this->_p_Cmd)->GetConsComponent();
	gp_Ax3 ax3 = dynamic_cast<DocumentModel*>(_p_Cmd->GetCmdDocument())->GetCompConnGlobalAx3(conscomponent->GetCompName().c_str(), consconnector->GetConnName().c_str());
	gp_Pnt t_pnt = ax3.Location();
	gp_Dir t_N1 = ax3.Direction();
	gp_Dir t_X1 = ax3.XDirection();
	v9_read[0] = t_pnt.X();
	v9_read[1] = t_pnt.Y();
	v9_read[2] = t_pnt.Z();
	v9_read[3] = t_N1.X();
	v9_read[4] = t_N1.Y();
	v9_read[5] = t_N1.Z();
	v9_read[6] = t_X1.X();
	v9_read[7] = t_X1.Y();
	v9_read[8] = t_X1.Z();
	last_record[0] = t_pnt.X();
	last_record[1] = t_pnt.Y();
	last_record[2] = t_pnt.Z();
	last_record[3] = t_N1.X();
	last_record[4] = t_N1.Y();
	last_record[5] = t_N1.Z();
	last_record[6] = t_X1.X();
	last_record[7] = t_X1.Y();
	last_record[8] = t_X1.Z();
}

void reposturedialog::On_savebutton_clicked()
{
	this->close();
	this->_p_Cmd->getCommandView()->StateChanged(QString("�������"));
}

void reposturedialog::On_restorebutton_clicked()
{
	QRegExp remove_zero_exp("(\\.)?0+$");
	QString origin_s = "(" + QString::number(last_record[0], 'f', 3).replace(remove_zero_exp, "") + "," + QString::number(last_record[1], 'f', 3).replace(remove_zero_exp, "") + "," + QString::number(last_record[2], 'f', 3).replace(remove_zero_exp, "") + ")";
	origin_line->setText(origin_s);
	QString Z_s = "(" + QString::number(last_record[3], 'f', 3).replace(remove_zero_exp, "") + "," + QString::number(last_record[4], 'f', 3).replace(remove_zero_exp, "") + "," + QString::number(last_record[5], 'f', 3).replace(remove_zero_exp, "") + ")";
	Axis_Z->setText(Z_s);
	QString X_s = "(" + QString::number(last_record[6], 'f', 3).replace(remove_zero_exp, "") + "," + QString::number(last_record[7], 'f', 3).replace(remove_zero_exp, "") + "," + QString::number(last_record[8], 'f', 3).replace(remove_zero_exp, "") + ")";
	Axis_X->setText(X_s);
	//����ƽ�ƺ���ת��Ϣ
	slide_spin_x->setValue(last_record[9]);
	slide_spin_y->setValue(last_record[10]);
	slide_spin_z->setValue(last_record[11]);
	rotate_spin_x->setValue(last_record[12]);
	rotate_spin_y->setValue(last_record[13]);
	rotate_spin_z->setValue(last_record[14]);
}

vector<double> local2global(const vector<double>& v9, const vector<double>& trans)
{
	assert(v9.size() == 9);
	assert(trans.size() == 3);
	//����Z���X�����Y��
	vector<double> AxisZ(v9.begin() + 3, v9.begin() + 6);
	vector<double> AxisX(v9.begin() + 6, v9.end());
	double y1 = AxisX[2] * AxisZ[1] - AxisX[1] * AxisZ[2];
	double y2 = AxisX[0] * AxisZ[2] - AxisX[2] * AxisZ[0];
	double y3 = AxisX[1] * AxisZ[0] - AxisX[0] * AxisZ[1];
	//תΪȫ������ı仯
	double g_x = AxisX[0] * trans[0] + AxisX[1] * trans[1] + AxisX[2] * trans[2];
	double g_y = y1 * trans[0] + y2 * trans[1] + y3 * trans[2];
	double g_z = AxisZ[0] * trans[0] + AxisZ[1] * trans[1] + AxisZ[2] * trans[2];
	return { g_x,g_y,g_z };
}

void reposturedialog::On_LineEdit_ValidChanged(const QString& text)
{
	QLineEdit* Sender = qobject_cast<QLineEdit*>(this->sender());
	QRegExp exp("^\\((-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?\\)$");
	exp.indexIn(text);
	double val1 = (exp.cap(1) + '.' + exp.cap(2)).toDouble();
	double val2 = (exp.cap(3) + '.' + exp.cap(4)).toDouble();
	double val3 = (exp.cap(5) + '.' + exp.cap(6)).toDouble();
	//�����
	if (Sender == origin_line)
	{
		v9_read[0] = val1;
		v9_read[1] = val2;
		v9_read[2] = val3;
	}
	else if (Sender == Axis_Z)
	{
		v9_read[3] = val1;
		v9_read[4] = val2;
		v9_read[5] = val3;
	}
	else if (Sender == Axis_X)
	{
		v9_read[6] = val1;
		v9_read[7] = val2;
		v9_read[8] = val3;
	}
}

void reposturedialog::tempChange(gp_Trsf trans)
{
	unordered_set<string> visited;
	//��������5.10
	for (auto ais : _p_Cmd->GetComponent()->GetAIS())
	{
		_p_Cmd->GetCmdDocument()->getContext()->SetLocation(ais, trans);
		_p_Cmd->GetCmdDocument()->getContext()->Update(ais, Standard_True);
	}
	//����װ�����ķ����·���ϵ�����ʵ��任
	//obj��Ӧ������ʵ�������
	queue<string> Q;
	Q.push(_p_Cmd->GetComponent()->GetCompName());
	visited.insert(_p_Cmd->GetComponent()->GetCompName());
	while (!Q.empty())
	{
		string curr_comp_name = Q.front();
		Q.pop();
		// �������������װ��Լ���Ľӿ�����
		for (auto curr_comp_conn_name : name_map[curr_comp_name])
		{
			string assembly_comp_conn_name = assembly_graph[curr_comp_conn_name];
			int assembly_pos = assembly_comp_conn_name.find('+');
			string assembly_comp_name = assembly_comp_conn_name.substr(0, assembly_pos);
			string assembly_conn_name = assembly_comp_conn_name.substr(assembly_pos, assembly_comp_conn_name.length() - assembly_pos - 1);
			Component* assembly_comp = this->_p_Cmd->getComproot()->GetChild(assembly_comp_name.c_str());
			if (visited.count(assembly_comp_name) || assembly_comp == dynamic_cast<RepostureCommand*>(_p_Cmd)->GetConsComponent())
			{
				continue;
			}
			visited.insert(assembly_comp_name);
			vector<Handle(AIS_InteractiveObject)> myAIS = assembly_comp->GetAIS();
			for (auto ais : myAIS)
			{
				_p_Cmd->GetCmdDocument()->getContext()->SetLocation(ais, trans);
				_p_Cmd->GetCmdDocument()->getContext()->Update(ais, Standard_True);
			}
			Q.emplace(assembly_comp_name);
		}
		//�ҵ��������ϵ����нӿ�
		//vector<Connector*> conns = curr_comp->GetConnectors();
		//for (auto conn : conns)
		//{
		//	string curr_conn_name = conn->GetConnName();
		//	string curr_comp_conn_name = curr_comp_name + '+' + curr_conn_name;
		//	if (assembly_graph.count(curr_comp_conn_name))
		//	{
		//		//�����������ӿ������װ�䵽����
		//		string next_assembly_name = direct_assembly_graph[curr_comp_conn_name];
		//		int next_pos = next_assembly_name.find('+');
		//		string next_assembly_comp_name = next_assembly_name.substr(0, next_pos);
		//		Component* assembly_comp = _p_Cmd->getComproot()->GetChild(next_assembly_comp_name.c_str());
		//		//�Ը��������AIS_InteractiveObject����
		//		vector<Handle(AIS_InteractiveObject)> myAIS = assembly_comp->GetAIS();
		//		assert(myAIS.size() > 0);
		//		for (auto ais : myAIS)
		//		{
		//			_p_Cmd->GetCmdDocument()->getContext()->SetLocation(ais, trans);
		//			_p_Cmd->GetCmdDocument()->getContext()->Update(ais, Standard_True);
		//		}
		//		Q.emplace(next_assembly_comp_name);
		//	}
		//}
	}
}

void reposturedialog::My_DoubleSpinBox_ValidChanged1(double val)
{
	QDoubleSpinBox* Sender = qobject_cast<QDoubleSpinBox*>(this->sender());
	if (consConnBox->currentText() == QString("��ѡ��") && (fabs(slide_spin_x->text().toDouble()) > LINEAR_TOL || fabs(slide_spin_y->text().toDouble()) > LINEAR_TOL || fabs(slide_spin_z->text().toDouble()) > LINEAR_TOL\
		|| fabs(rotate_spin_x->text().toDouble()) > LINEAR_TOL || fabs(rotate_spin_y->text().toDouble()) > LINEAR_TOL || fabs(rotate_spin_z->text().toDouble()) > LINEAR_TOL))
	{
		QMessageBox::information(nullptr, "��ʾ", "��ѡ����ʵĵ��˽ӿ�");
		slide_spin_x->setValue(last_record[9]);
		slide_spin_y->setValue(last_record[10]);
		slide_spin_z->setValue(last_record[11]);
		rotate_spin_x->setValue(last_record[12]);
		rotate_spin_y->setValue(last_record[13]);
		rotate_spin_z->setValue(last_record[14]);
		return;
	}
	//�����
	if (Sender == slide_spin_x)
	{
		slide_spin_x->setEnabled(false);
		vector<double> disp1 = v9_read;
		//����ƽ�����
		vector<double> v15(15);
		v15[0] = disp1[0]; v15[1] = disp1[1]; v15[2] = disp1[2];
		v15[3] = disp1[3]; v15[4] = disp1[4]; v15[5] = disp1[5];
		v15[6] = disp1[6]; v15[7] = disp1[7]; v15[8] = disp1[8];
		v15[9] = val - last_record[9]; v15[10] = last_change[1] - last_record[10]; v15[11] = last_change[2] - last_record[11];
		v15[12] = last_change[3] * PI / 180 - last_record[12] * PI / 180; v15[13] = last_change[4] * PI / 180 - last_record[13] * PI / 180; v15[14] = last_change[5] * PI / 180 - last_record[14] * PI / 180;
		last_change[0] = val;
		change_all[0] = val;
		Handle(AIS_InteractiveObject) obj = _p_Cmd->GetCmdDocument()->getContext()->SelectedInteractive();
		gp_Trsf trans = MakeTransFromV15(v15);
		if (!first_read[0] && change_consComboBox == false)
		{
			//_p_Cmd->GetCmdDocument()->getContext()->SetLocation(obj, trans);
			//_p_Cmd->GetCmdDocument()->getContext()->Update(obj, Standard_True);
			tempChange(trans);
		}
		else
		{
			if (change_consComboBox)
				change_consComboBox = false;
			first_read[0] = false;
		}
		slide_spin_x->setEnabled(true);
	}
	else if (Sender == slide_spin_y)
	{
		slide_spin_y->setEnabled(false);
		vector<double> disp1 = v9_read;
		//����ƽ�����
		vector<double> v15(15);
		v15[0] = disp1[0]; v15[1] = disp1[1]; v15[2] = disp1[2];
		v15[3] = disp1[3]; v15[4] = disp1[4]; v15[5] = disp1[5];
		v15[6] = disp1[6]; v15[7] = disp1[7]; v15[8] = disp1[8];
		v15[9] = last_change[0] - last_record[9]; v15[10] = val - last_record[10]; v15[11] = last_change[2] - last_record[11];
		v15[12] = last_change[3] * PI / 180 - last_record[12] * PI / 180; v15[13] = last_change[4] * PI / 180 - last_record[13] * PI / 180; v15[14] = last_change[5] * PI / 180 - last_record[14] * PI / 180;
		last_change[1] = val;
		change_all[1] = val;
		Handle(AIS_InteractiveObject) obj = _p_Cmd->GetCmdDocument()->getContext()->SelectedInteractive();
		gp_Trsf trans = MakeTransFromV15(v15);
		if (!first_read[1] && change_consComboBox == false)
		{
			//_p_Cmd->GetCmdDocument()->getContext()->SetLocation(obj, trans);
			//_p_Cmd->GetCmdDocument()->getContext()->Update(obj, Standard_True);
			tempChange(trans);
		}
		else
		{
			if (change_consComboBox)
				change_consComboBox = false;
			first_read[1] = false;
		}
		slide_spin_y->setEnabled(true);
	}
	else if (Sender == slide_spin_z)
	{
		slide_spin_z->setEnabled(false);
		vector<double> disp1 = v9_read;
		//����ƽ�����
		vector<double> v15(15);
		v15[0] = disp1[0]; v15[1] = disp1[1]; v15[2] = disp1[2];
		v15[3] = disp1[3]; v15[4] = disp1[4]; v15[5] = disp1[5];
		v15[6] = disp1[6]; v15[7] = disp1[7]; v15[8] = disp1[8];
		v15[9] = last_change[0] - last_record[9]; v15[10] = last_change[1] - last_record[10]; v15[11] = val - last_record[11];
		v15[12] = last_change[3] * PI / 180 - last_record[12] * PI / 180; v15[13] = last_change[4] * PI / 180 - last_record[13] * PI / 180; v15[14] = last_change[5] * PI / 180 - last_record[14] * PI / 180;
		last_change[2] = val;
		change_all[2] = val;
		Handle(AIS_InteractiveObject) obj = _p_Cmd->GetCmdDocument()->getContext()->SelectedInteractive();
		gp_Trsf trans = MakeTransFromV15(v15);
		if (!first_read[2] && change_consComboBox == false)
		{
			//_p_Cmd->GetCmdDocument()->getContext()->SetLocation(obj, trans);
			//_p_Cmd->GetCmdDocument()->getContext()->Update(obj, Standard_True);
			tempChange(trans);
		}
		else
		{
			if (change_consComboBox)
				change_consComboBox = false;
			first_read[2] = false;
		}
		slide_spin_z->setEnabled(true);
	}
	else if (Sender == rotate_spin_x)
	{
		rotate_spin_x->setEnabled(false);
		vector<double> disp1 = v9_read;
		//������ת���
		vector<double> v15(15);
		v15[0] = disp1[0]; v15[1] = disp1[1]; v15[2] = disp1[2];
		v15[3] = disp1[3]; v15[4] = disp1[4]; v15[5] = disp1[5];
		v15[6] = disp1[6]; v15[7] = disp1[7]; v15[8] = disp1[8];
		v15[9] = last_change[0] - last_record[9]; v15[10] = last_change[1] - last_record[10]; v15[11] = last_change[2] - last_record[11];
		v15[12] = val * PI / 180 - last_record[12] * PI / 180; v15[13] = last_change[4] * PI / 180 - last_record[13] * PI / 180; v15[14] = last_change[5] * PI / 180 - last_record[14] * PI / 180;
		last_change[3] = val ;
		change_all[3] = val * PI / 180;
		Handle(AIS_InteractiveObject) obj = _p_Cmd->GetCmdDocument()->getContext()->SelectedInteractive();
		gp_Trsf trans = MakeTransFromV15(v15);
		if (!first_read[3] && change_consComboBox == false)
		{
			//_p_Cmd->GetCmdDocument()->getContext()->SetLocation(obj, trans);
			//_p_Cmd->GetCmdDocument()->getContext()->Update(obj, Standard_True);
			tempChange(trans);
		}
		else
		{
			if (change_consComboBox)
				change_consComboBox = false;
			first_read[3] = false;
		}
		rotate_spin_x->setEnabled(true);
	}
	else if (Sender == rotate_spin_y)
	{
		rotate_spin_y->setEnabled(false);
		vector<double> disp1 = v9_read;
		vector<double> v15(15);
		v15[0] = disp1[0]; v15[1] = disp1[1]; v15[2] = disp1[2];
		v15[3] = disp1[3]; v15[4] = disp1[4]; v15[5] = disp1[5];
		v15[6] = disp1[6]; v15[7] = disp1[7]; v15[8] = disp1[8];
		v15[9] = last_change[0] - last_record[9]; v15[10] = last_change[1] - last_record[10]; v15[11] = last_change[2] - last_record[11];
		v15[12] = last_change[3] * PI / 180 - last_record[12] * PI / 180; v15[13] = val * PI / 180 - last_record[13] * PI / 180; v15[14] = last_change[5] * PI / 180 - last_record[14] * PI / 180;
		last_change[4] = val ;
		change_all[4] = val * PI / 180;
		gp_Trsf trans = MakeTransFromV15(v15);
		if (!first_read[4] && change_consComboBox == false)
		{
			tempChange(trans);
		}
		else
		{
			if (change_consComboBox)
				change_consComboBox = false;
			first_read[4] = false;
		}
		rotate_spin_y->setEnabled(true);
	}
	else if (Sender == rotate_spin_z)
	{
		rotate_spin_z->setEnabled(false);
		vector<double> disp1 = v9_read;
		//�Ƕ�ת���ɻ���
		vector<double> v15(15);
		v15[0] = disp1[0]; v15[1] = disp1[1]; v15[2] = disp1[2];
		v15[3] = disp1[3]; v15[4] = disp1[4]; v15[5] = disp1[5];
		v15[6] = disp1[6]; v15[7] = disp1[7]; v15[8] = disp1[8];
		v15[9] = last_change[0] - last_record[9]; v15[10] = last_change[1] - last_record[10]; v15[11] = last_change[2] - last_record[11];
		v15[12] = last_change[3] * PI / 180 - last_record[12] * PI / 180; v15[13] = last_change[4] * PI / 180 - last_record[13] * PI / 180; v15[14] = val * PI / 180 - last_record[14] * PI / 180;
		//v15[9] = last_change[0]; v15[10] = last_change[1]; v15[11] = last_change[2];
		//v15[12] = last_change[3] * PI / 180; v15[13] = last_change[4] * PI / 180; v15[14] = val * PI / 180;
		//vector<double> v15_trans(v15);
		//v15_trans[14] -= last_record[14] * PI / 180;
		last_change[5] = val ;
		change_all[5] = val * PI / 180;
		Handle(AIS_InteractiveObject) obj = _p_Cmd->GetCmdDocument()->getContext()->SelectedInteractive();
		gp_Trsf trans = MakeTransFromV15(v15);
		if (!first_read[5] && change_consComboBox == false)
		{
			//_p_Cmd->GetCmdDocument()->getContext()->SetLocation(obj, trans);
			//_p_Cmd->GetCmdDocument()->getContext()->Update(obj, Standard_True);
			tempChange(trans);
		}
		else
		{
			if (change_consComboBox)
				change_consComboBox = false;
			first_read[5] = false;
		}
		rotate_spin_z->setEnabled(true);
	}
}

void QAbstractSpinBox::wheelEvent(QWheelEvent* e)
{
	/*��д��֯ͨ�����ֵĹ����޸�ֵ*/
}