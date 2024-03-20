#include "NeuralnetworkWidget.h"
#include "DocumentComponent.h"
#include <QAction>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QMessageBox>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <string>
#include <set>
#include <QMenu>
#include <QKeyEvent>
#include <QTextCursor>
#include <QFileDialog>
#include "CSV_Parser.h"
#include <random>
#include "../FANN/include/doublefann.h"
#pragma comment(lib, "../FANN/bin/fanndoubled.lib")
#include <iostream>
#include <iomanip>


using namespace std;
using namespace CSV_Parser;

NeuralnetworkWidget::NeuralnetworkWidget(QWidget* parent) :Parent(parent)
{
	this->m_doc = (DocumentCommon*)qobject_cast<MainWindow*>(parent)->getDocument();

	this->setContextMenuPolicy(Qt::CustomContextMenu);

	setMinimumWidth(300);
	setMaximumWidth(600);
	//setFixedWidth(400);

	dpWidInit();
}

void NeuralnetworkWidget::dpWidInit()
{

	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//�������岼��Ϊ��ֱ����
	VerticalLayout->setDirection(QBoxLayout::TopToBottom);

	//1��CSV�ļ�·�����ã����ڶ�ȡ�������ƣ�
	defCsvPath = new QLabel("csv�ļ�·��");
	font = defCsvPath->font();//�Ӵ�����
	font.setBold(true);
	defCsvPath->setFont(font);
	defCsvPath->setMinimumHeight(25);
	//---------------------------------------------------------------
	step1 = new QLabel("����һ��ѡ��csv�ļ�����·��");
	step1->setMinimumHeight(25);

	PushButtonSelCsvPath = new QPushButton(this);
	PushButtonSelCsvPath->setText("ѡ��");
	PushButtonSelCsvPath->setFixedSize(75, 25);
	connect(PushButtonSelCsvPath, SIGNAL(clicked()), this, SLOT(ClickButtonSelCsvPath()));

	showCsvPath = new QLineEdit;
	showCsvPath->setReadOnly(false); //�ɱ༭
	showCsvPath->setMinimumHeight(25);
	//---------------------------------------------------------------
	step2 = new QLabel("�������ѡ����������ʽ");
	step2->setMinimumHeight(25);

	ButtonT33 = new QRadioButton(this);
	ButtonT33->setText("�������");
	ButtonNone = new QRadioButton(this);
	ButtonNone->setText("��Ĭ��ѡ��");
	ButtonEuler = new QRadioButton(this);
	ButtonEuler->setText("ŷ����");
	groupButtonWays = new QButtonGroup(this);
	groupButtonWays->addButton(ButtonT33, 0);//idΪ0
	groupButtonWays->addButton(ButtonEuler, 1);//idΪ1
	groupButtonWays->addButton(ButtonNone, 2);//idΪ2
	groupButtonWays->button(MethodID)->setChecked(true); //Ĭ��ѡ���ϴ�ѡ��������
	//---------------------------------------------------------------
	step3 = new QLabel("����������ȡcsv�ļ�");
	step3->setMinimumHeight(25);

	PushButtonReadCsv = new QPushButton(this);
	PushButtonReadCsv->setText("��ȡ");
	PushButtonReadCsv->setFixedSize(75, 25);
	connect(PushButtonReadCsv, SIGNAL(clicked()), this, SLOT(ClickButtonReadCsv()));

	//2.�����������
	ShowVariable = new QLabel("�趨�������������");
	font = ShowVariable->font();//�Ӵ�����
	font.setBold(true);
	ShowVariable->setFont(font);
	ShowVariable->setMinimumHeight(25);

	PushButtonSave = new QPushButton(this);
	PushButtonSave->setText("����");
	PushButtonSave->setFixedSize(75, 25);
	connect(PushButtonSave, SIGNAL(clicked()), this, SLOT(ClickButtonSave()));

	//����view
	outputtreeview = new QTreeView(this);
	//1.QTreeView����������
	outputtreeview->setSelectionBehavior(QTreeView::SelectRows);		 //һ��ѡ������
	outputtreeview->setSelectionMode(QTreeView::SingleSelection);        //��ѡ�������������о���һ��ѡ����
	outputtreeview->header()->setStretchLastSection(true);               //���һ������Ӧ���
	outputtreeview->header()->setDefaultAlignment(Qt::AlignCenter);      //��ͷ����Ĭ�Ͼ��ж���
	//����outputtreemodel
	outputtreemodel = new QStandardItemModel(this);
	//���ñ�ͷ
	outputtreemodel->setHorizontalHeaderLabels(QStringList() << "���������" << "���" << "�Ƿ������" << "��Сֵ" << "���ֵ");
	outputtreeview->setModel(outputtreemodel);
	outputtreeview->setItemDelegateForColumn(1, new CheckBoxDelegateNetwork(outputtreeview));
	outputtreeview->setItemDelegateForColumn(2, new CheckBoxDelegateControl(outputtreeview));

	//3.���������Ϣչʾ
	ShowMessage = new QLabel("���������Ϣչʾ");
	font = ShowMessage->font();//�Ӵ�����
	font.setBold(true);
	ShowMessage->setFont(font);
	ShowMessage->setMinimumHeight(25);

	InputOutputMessage = new QTextEdit(this);
	InputOutputMessage->setText("�����õ���������������Ϣ����\nֻ�п�������������Сֵ\n�����СֵȱʡΪ0\n");

	//4.����ѵ����
	PushButtonGenTrainSet = new QPushButton(this);
	PushButtonGenTrainSet->setText("����ѵ����");
	PushButtonGenTrainSet->setFixedSize(250, 25);
	connect(PushButtonGenTrainSet, SIGNAL(clicked()), this, SLOT(ClickButtonGenTrainSet()));

	//5.������ѵ��������
	PushButtonGenFANN = new QPushButton(this);
	PushButtonGenFANN->setText("������ѵ��������");
	PushButtonGenFANN->setFixedSize(250, 25);
	// connect(PushButtonGenFANN, SIGNAL(clicked()), this, SLOT(ClickButtonGenFANN()));
	connect(PushButtonGenFANN, SIGNAL(clicked()), this, SLOT(ClickButtonGenPyNN()));

	//����
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(defCsvPath);
	
	QHBoxLayout* HorinztalLayoutStep1 = new QHBoxLayout(this);
	HorinztalLayoutStep1->addWidget(step1);
	HorinztalLayoutStep1->addWidget(PushButtonSelCsvPath);
	QHBoxLayout* HorinztalLayoutStep12 = new QHBoxLayout(this);
	HorinztalLayoutStep12->addWidget(showCsvPath);
	QHBoxLayout* HorinztalLayoutStep2 = new QHBoxLayout(this);
	HorinztalLayoutStep2->addWidget(step2);
	QHBoxLayout* HorinztalLayoutStep22 = new QHBoxLayout(this);
	HorinztalLayoutStep22->addWidget(ButtonT33);
	HorinztalLayoutStep22->addWidget(ButtonEuler);
	HorinztalLayoutStep22->addWidget(ButtonNone);
	QHBoxLayout* HorinztalLayoutStep3 = new QHBoxLayout(this);
	HorinztalLayoutStep3->addWidget(step3);
	HorinztalLayoutStep3->addWidget(PushButtonReadCsv);
	
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->addWidget(ShowVariable);
	HorinztalLayout3->addWidget(PushButtonSave);
	QHBoxLayout* HorinztalLayout4 = new QHBoxLayout(this);
	HorinztalLayout4->addWidget(outputtreeview);
	QHBoxLayout* HorinztalLayout5 = new QHBoxLayout(this);
	HorinztalLayout5->addWidget(ShowMessage);
	QHBoxLayout* HorinztalLayout6 = new QHBoxLayout(this);
	HorinztalLayout6->addWidget(InputOutputMessage);
	QHBoxLayout* HorinztalLayout7 = new QHBoxLayout(this);
	HorinztalLayout7->addWidget(PushButtonGenTrainSet);
	QHBoxLayout* HorinztalLayout8 = new QHBoxLayout(this);
	HorinztalLayout8->addWidget(PushButtonGenFANN);



	VerticalLayout->setSpacing(15);
	VerticalLayout->addLayout(HorinztalLayout1);
	VerticalLayout->addLayout(HorinztalLayoutStep1);
	VerticalLayout->addLayout(HorinztalLayoutStep12);
	VerticalLayout->addLayout(HorinztalLayoutStep2);
	VerticalLayout->addLayout(HorinztalLayoutStep22);
	VerticalLayout->addLayout(HorinztalLayoutStep3);
	VerticalLayout->addLayout(HorinztalLayout3);
	VerticalLayout->addLayout(HorinztalLayout4);
	VerticalLayout->addLayout(HorinztalLayout5);
	VerticalLayout->addLayout(HorinztalLayout6);
	VerticalLayout->addLayout(HorinztalLayout7);
	VerticalLayout->addLayout(HorinztalLayout8);



	this->setLayout(VerticalLayout);
}

NeuralnetworkWidget::~NeuralnetworkWidget()
{

}

void NeuralnetworkWidget::ClickButtonSelCsvPath()
{
	csvPath = QFileDialog::getOpenFileName(this, tr("Select a .csv file"), QString(), tr("csv Files(*.csv);;All files(*.*)"));
	showCsvPath->setText(csvPath);
}

void NeuralnetworkWidget::ClickButtonReadCsv()
{	
	QString csvFilePath = showCsvPath->text();
	allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	MethodID = groupButtonWays->checkedId();
	CreateOutputModel();
}

void NeuralnetworkWidget::ClickButtonSave()
{
	InputNames.clear();
	OutputNames.clear();
	ControlVars.clear();
	for (int i = 0; i < outputtreemodel->rowCount(); i++)
	{
		QStandardItem* item1 = outputtreemodel->item(i, 1);
		string vartype = item1->text().toStdString();
		if (vartype == "������")
		{
			QStandardItem* item0 = outputtreemodel->item(i, 0);
			string varname = item0->text().toStdString();
			InputNames.push_back(varname);
		}
		else if (vartype == "�����")
		{
			QStandardItem* item0 = outputtreemodel->item(i, 0);
			string varname = item0->text().toStdString();
			OutputNames.push_back(varname);
		}

		QStandardItem* item2 = outputtreemodel->item(i, 2);
		string isControl = item2->text().toStdString();
		if (isControl == "��")
		{
			//��Сֵ
			QStandardItem* item3 = outputtreemodel->item(i, 3);
			string str_minValue = item3->text().toStdString();
			double minValue = 0;
			if (str_minValue != "")
			{
				minValue = std::stod(str_minValue);
			}
			//���ֵ
			QStandardItem* item4 = outputtreemodel->item(i, 4);
			string str_maxValue = item4->text().toStdString();
			double maxValue = 0;
			if (str_maxValue != "")
			{
				maxValue = std::stod(str_maxValue);
			}

			QStandardItem* item0 = outputtreemodel->item(i, 0);
			string varname = item0->text().toStdString();
			
			std::tuple<string, double, double> mytuple(varname, minValue, maxValue);
			ControlVars.push_back(mytuple);
		}
	}
	ShowVarsMessage();
}

void NeuralnetworkWidget::ShowVarsMessage()
{
	MessageDisplay.clear();
	MessageDisplay += "�����õ���������������Ϣ����\nֻ�п�������������Сֵ\n�����СֵȱʡΪ0\n";
	//���п�����
	MessageDisplay += "���п����\n";
	for (int i = 0; i < ControlVars.size(); i++)
	{
		string name = std::get<0>(ControlVars[i]);
		string minval = std::to_string(std::get<1>(ControlVars[i]));
		string maxval = std::to_string(std::get<2>(ControlVars[i]));
		QString print = QString::fromStdString(name) + "     " + QString::fromStdString(minval) + "     " + QString::fromStdString(maxval) + "\n";
		MessageDisplay += print;
	}
	
	//����������
	MessageDisplay += "���������\n";
	for (int i = 0; i < InputNames.size(); i++)
	{
		MessageDisplay += QString::fromStdString(InputNames[i]);
		MessageDisplay += "\n";
	}

	//���������
	MessageDisplay += "��������\n";
	for (int i = 0; i < OutputNames.size(); i++)
	{
		MessageDisplay += QString::fromStdString(OutputNames[i]);
		MessageDisplay += "\n";
	}
	
	InputOutputMessage->setText(MessageDisplay);
}

void NeuralnetworkWidget::CreateOutputModel()
{
	//�����modeltree																													 
	for (int i = 0; i < allOutputNames.size(); i++)
	{
		QString outputname = allOutputNames[i];
		string strname = outputname.toStdString();
		bool addoutput = false;
		int idx1 = strname.find(".frame_a.r_0"); 
		if (idx1 != string::npos && MethodID != 2)
		{
			// ����û�п�����װ���壬������װ����Ļ�����Ҫ��д
			// FIXME
			string lastname = ".frame_a.r_0[_]";
			string firstname = strname.substr(0, strname.length() - lastname.length());
			if (firstname.find(".") == string::npos) {
				addoutput = true;
			}
		}
		int idxAlpha = strname.find(".Alpha");
		int idxBeta = strname.find(".Beta");
		int idxGamma = strname.find(".Gamma");
		if (idxAlpha != string::npos || idxBeta != string::npos || idxGamma != string::npos)
		{
			if (MethodID == 1)
			{
				addoutput = true;
			}
		}
		int idx3 = strname.find(".frame_a.R.T");
		if (idx3 != string::npos)
		{
			if (MethodID == 0)
			{
				// ����û�п�����װ���壬������װ����Ļ�����Ҫ��д
				// FIXME
				string lastname = ".frame_a.R.T[_, _]";
				string firstname = strname.substr(0, strname.length() - lastname.length());
				if (firstname.find(".") == string::npos) {
					addoutput = true;
				}
			}
		}

		//һ���ڵ㣬����mModel
		QList<QStandardItem*> items1;
		QStandardItem* item1 = new QStandardItem(outputname);
		//QStandardItem* item2 = new QStandardItem();
		QStandardItem* item2;
		if (addoutput == true)
		{
			item2 = new QStandardItem("�����");
		}
		else
		{
			item2 = new QStandardItem();
		}
		QStandardItem* item3 = new QStandardItem();
		QStandardItem* item4 = new QStandardItem();
		QStandardItem* item5 = new QStandardItem();
		items1.append(item1);
		items1.append(item2);
		items1.append(item3);
		items1.append(item4);
		items1.append(item5);
		outputtreemodel->appendRow(items1);
	}
	outputtreeview->setModel(outputtreemodel);
	outputtreeview->setEditTriggers(QTreeView::DoubleClicked);
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
	outputtreeview->setItemDelegateForColumn(0, new DisableEditor(outputtreeview));
	outputtreeview->expandAll();
}

//����ѵ����
void NeuralnetworkWidget::ClickButtonGenTrainSet()
{
	vector<string> ControlVarsNames;
	vector<pair<double, double>> ranges;
	for (int i = 0; i < ControlVars.size(); i++)
	{
		string name = std::get<0>(ControlVars[i]);
		double minval = std::get<1>(ControlVars[i]);
		double maxval = std::get<2>(ControlVars[i]);
		ControlVarsNames.push_back(name);
		ranges.push_back(make_pair(minval, maxval));
	}

	int dimension = ControlVarsNames.size();
	int n = 200; //������������ĿĬ��500��֮���ٽ��е���
	vector<vector<double>> samples = latin_hypercube(dimension, n, ranges, false); //Ĭ���Ȳ�ʹ�þܾ�����


	if (m_doc && m_doc->IsModel())
	{
		//�������޸�
		//����һ��һ�����������Ĺ���
		((DocumentModel*)m_doc)->GenTrainSet(ControlVarsNames, samples, InputNames, OutputNames);
	}
}

//������ѵ��������
//���ܱ���ѵ��״̬�����Կ�����ô��һ��
//��һ������ѵ��Ч���ĶԻ���
void NeuralnetworkWidget::ClickButtonGenFANN()
{
	const unsigned int num_input = InputNames.size();       // ���������
	const unsigned int num_output = OutputNames.size();      // ��������
	//const unsigned int num_input = 6;       // ���������
	//const unsigned int num_output = 96;      // ��������
	const unsigned int num_layers = 5;      //�����������������������㣩
	const unsigned int num_neurons_hidden1 = 6;       //��ʾ���ز���6����Ԫ
	const unsigned int num_neurons_hidden2 = 32;       //��ʾ���ز���32����Ԫ
	const unsigned int num_neurons_hidden3 = 48;       //��ʾ���ز���48����Ԫ
	const float desired_error = (const float)0.00000001;
	const unsigned int max_epochs = 500; // ���ִ�д���
	const unsigned int epochs_between_reports = 10;   // ����Ƶ��

	struct fann* ann;

	//int     Num = 200;
	//float   Mf = Num * 3.f;
	//int     i;
	//double  a, b, c;
	//FILE* fp;
	//fopen_s(&fp, "add.fann", "w");
	//fprintf_s(fp, "%d 2 1\n", Num);
	//// ����ѵ���ļ�
	//for (i = 1; i <= Num; i++) {
	//    // ����2����, Ҫ����(0,1)֮��
	//    a = i / Mf;
	//    b = (i + 1) / Mf;
	//    c = a + b;    // Ҫ����(0,1)֮��

	//    // ��������д��ѵ���ļ�
	//    fprintf_s(fp, "%lf %lf\n%lf\n", a, b, c);
	//}
	//fclose(fp);
	// ����ѵ��
	//ann = fann_create_standard(num_layers, num_input, num_neurons_hidden1, num_output);
	ann = fann_create_standard(num_layers, num_input, num_neurons_hidden1, num_neurons_hidden2, num_neurons_hidden3, num_output);
	//����ȫ���ӵ�
	//ann = fann_create_sparse(0.5, num_layers, num_input, num_neurons_hidden1, num_output);

	//�趨�������ز����Ԫ�����
	/*fann_set_activation_function_hidden�趨�������ز��������Ԫ�ļ��������Ȼ�������ֻ��һ�����ز㣬
	�����ж�����ز�ʱ������fann_set_activation_function_hidden�������ü����Ϊͬһ��
	 */
	 //fann_set_activation_function_hidden(ann, FANN_LINEAR);
	fann_set_activation_function_layer(ann, FANN_LINEAR, 1);//�������Ϊ0,������ز�Ϊ1
	fann_set_activation_function_layer(ann, FANN_LINEAR, 2);//�������Ϊ0,������ز�Ϊ1
	fann_set_activation_function_layer(ann, FANN_SIGMOID, 3);//�������Ϊ0,������ز�Ϊ1
	//�趨�����ļ����
	fann_set_activation_function_output(ann, FANN_LINEAR);

	string fannfilepath = ((DocumentModel*)m_doc)->GetfannSavePath();
	string netfilepath = fannfilepath + ".net";
	//fann_train_on_file(ann, "robotnn0426.fann", max_epochs, epochs_between_reports, desired_error);
	//fann_save(ann, "robotnn0426.fann.net");
	fann_train_on_file(ann, fannfilepath.c_str(), max_epochs, epochs_between_reports, desired_error);
	fann_save(ann, netfilepath.c_str());
	fann_destroy(ann);
}

// ������ѵ��Pytorch���������ʾ
void NeuralnetworkWidget::ClickButtonGenPyNN() {
	QMessageBox msgBox;
	msgBox.setText("���ڵ�ǰ�ļ�·���£��ҵ�pytorchtrain.py�ļ����������ݼ�����ѵ����");
	msgBox.exec();
	return;
}

//������������������  N-����ֵ������ά�ȣ�   n-������Ŀ
vector<vector<double>> NeuralnetworkWidget::latin_hypercube(int N, int n, vector<pair<double, double>>& ranges, bool restricted) {
	// Initialize a vector of size N to hold the indices of each dimension
	vector<vector<int>> indices(N, vector<int>(n));

	// Initialize a vector of size n to hold the sampled values for each dimension
	vector<double> values(n);

	// Generate a random permutation of indices for each dimension
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < n; j++) {
			indices[i][j] = j;
		}
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		shuffle(indices[i].begin(), indices[i].end(), default_random_engine(seed));
	}

	// Calculate the step size for each dimension
	vector<double> step_sizes(N);
	for (int i = 0; i < N; i++) {
		step_sizes[i] = (ranges[i].second - ranges[i].first) / n;
	}

	// Generate the Latin hypercube sample
	vector<vector<double>> sample(n, vector<double>(N));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < N; j++) {
			values[i] = (indices[j][i] + 0.5) * step_sizes[j] + ranges[j].first;
			sample[i][j] = values[i];
		}
	}

	// Randomly permute the columns if restricted is true
	if (restricted) {
		for (int i = 0; i < N; i++) {
			shuffle(sample.begin(), sample.end(), default_random_engine());
		}
	}

	return sample;
}

//����������������б�
QWidget* CheckBoxDelegateNetwork::createEditor(QWidget* parent,
	const QStyleOptionViewItem& /* option */,
	const QModelIndex& index) const
{
	QComboBox* editor = new QComboBox(parent);
	editor->addItem("������");
	editor->addItem("�����");
	editor->addItem("������/���");

	return editor;
}

void CheckBoxDelegateNetwork::setEditorData(QWidget* editor,
	const QModelIndex& index) const
{
	QString text = index.model()->data(index, Qt::EditRole).toString();
	int value = index.model()->data(index, Qt::EditRole).toInt();
	QComboBox* combobox = static_cast<QComboBox*>(editor);
	combobox->setCurrentIndex(combobox->findText(text));
}

void CheckBoxDelegateNetwork::setModelData(QWidget* editor,
	QAbstractItemModel* model,
	const QModelIndex& index) const
{
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	QString text = comboBox->currentText();

	model->setData(index, text, Qt::EditRole);
}
void CheckBoxDelegateNetwork::updateEditorGeometry(QWidget* editor,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}

//���������������б�
QWidget* CheckBoxDelegateControl::createEditor(QWidget* parent,
	const QStyleOptionViewItem& /* option */,
	const QModelIndex& index) const
{
	QComboBox* editor = new QComboBox(parent);
	editor->addItem("��");
	editor->addItem("��");

	return editor;
}

void CheckBoxDelegateControl::setEditorData(QWidget* editor,
	const QModelIndex& index) const
{
	QString text = index.model()->data(index, Qt::EditRole).toString();
	int value = index.model()->data(index, Qt::EditRole).toInt();
	QComboBox* combobox = static_cast<QComboBox*>(editor);
	combobox->setCurrentIndex(combobox->findText(text));
}

void CheckBoxDelegateControl::setModelData(QWidget* editor,
	QAbstractItemModel* model,
	const QModelIndex& index) const
{
	QComboBox* comboBox = static_cast<QComboBox*>(editor);

	QString text = comboBox->currentText();

	model->setData(index, text, Qt::EditRole);
}
void CheckBoxDelegateControl::updateEditorGeometry(QWidget* editor,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
