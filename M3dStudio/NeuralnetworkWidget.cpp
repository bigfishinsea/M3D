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

	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//设置整体布局为垂直布局
	VerticalLayout->setDirection(QBoxLayout::TopToBottom);

	//1、CSV文件路径设置（用于读取变量名称）
	defCsvPath = new QLabel("csv文件路径");
	font = defCsvPath->font();//加粗字体
	font.setBold(true);
	defCsvPath->setFont(font);
	defCsvPath->setMinimumHeight(25);
	//---------------------------------------------------------------
	step1 = new QLabel("步骤一：选择csv文件所在路径");
	step1->setMinimumHeight(25);

	PushButtonSelCsvPath = new QPushButton(this);
	PushButtonSelCsvPath->setText("选择");
	PushButtonSelCsvPath->setFixedSize(75, 25);
	connect(PushButtonSelCsvPath, SIGNAL(clicked()), this, SLOT(ClickButtonSelCsvPath()));

	showCsvPath = new QLineEdit;
	showCsvPath->setReadOnly(false); //可编辑
	showCsvPath->setMinimumHeight(25);
	//---------------------------------------------------------------
	step2 = new QLabel("步骤二：选择多体输出方式");
	step2->setMinimumHeight(25);

	ButtonT33 = new QRadioButton(this);
	ButtonT33->setText("常规矩阵");
	ButtonNone = new QRadioButton(this);
	ButtonNone->setText("不默认选择");
	ButtonEuler = new QRadioButton(this);
	ButtonEuler->setText("欧拉角");
	groupButtonWays = new QButtonGroup(this);
	groupButtonWays->addButton(ButtonT33, 0);//id为0
	groupButtonWays->addButton(ButtonEuler, 1);//id为1
	groupButtonWays->addButton(ButtonNone, 2);//id为2
	groupButtonWays->button(MethodID)->setChecked(true); //默认选中上次选择的求解器
	//---------------------------------------------------------------
	step3 = new QLabel("步骤三：读取csv文件");
	step3->setMinimumHeight(25);

	PushButtonReadCsv = new QPushButton(this);
	PushButtonReadCsv->setText("读取");
	PushButtonReadCsv->setFixedSize(75, 25);
	connect(PushButtonReadCsv, SIGNAL(clicked()), this, SLOT(ClickButtonReadCsv()));

	//2.输入输出项树
	ShowVariable = new QLabel("设定输入项与输出项");
	font = ShowVariable->font();//加粗字体
	font.setBold(true);
	ShowVariable->setFont(font);
	ShowVariable->setMinimumHeight(25);

	PushButtonSave = new QPushButton(this);
	PushButtonSave->setText("保存");
	PushButtonSave->setFixedSize(75, 25);
	connect(PushButtonSave, SIGNAL(clicked()), this, SLOT(ClickButtonSave()));

	//构造view
	outputtreeview = new QTreeView(this);
	//1.QTreeView常用设置项
	outputtreeview->setSelectionBehavior(QTreeView::SelectRows);		 //一次选中整行
	outputtreeview->setSelectionMode(QTreeView::SingleSelection);        //单选，配合上面的整行就是一次选单行
	outputtreeview->header()->setStretchLastSection(true);               //最后一列自适应宽度
	outputtreeview->header()->setDefaultAlignment(Qt::AlignCenter);      //列头文字默认居中对齐
	//构造outputtreemodel
	outputtreemodel = new QStandardItemModel(this);
	//设置表头
	outputtreemodel->setHorizontalHeaderLabels(QStringList() << "输出项名称" << "类别" << "是否控制项" << "最小值" << "最大值");
	outputtreeview->setModel(outputtreemodel);
	outputtreeview->setItemDelegateForColumn(1, new CheckBoxDelegateNetwork(outputtreeview));
	outputtreeview->setItemDelegateForColumn(2, new CheckBoxDelegateControl(outputtreeview));

	//3.输入输出信息展示
	ShowMessage = new QLabel("输入输出信息展示");
	font = ShowMessage->font();//加粗字体
	font.setBold(true);
	ShowMessage->setFont(font);
	ShowMessage->setMinimumHeight(25);

	InputOutputMessage = new QTextEdit(this);
	InputOutputMessage->setText("所设置的输入项和输出项信息如下\n只有控制项包含最大最小值\n最大最小值缺省为0\n");

	//4.生成训练集
	PushButtonGenTrainSet = new QPushButton(this);
	PushButtonGenTrainSet->setText("生成训练集");
	PushButtonGenTrainSet->setFixedSize(250, 25);
	connect(PushButtonGenTrainSet, SIGNAL(clicked()), this, SLOT(ClickButtonGenTrainSet()));

	//5.建立并训练神经网络
	PushButtonGenFANN = new QPushButton(this);
	PushButtonGenFANN->setText("建立并训练神经网络");
	PushButtonGenFANN->setFixedSize(250, 25);
	// connect(PushButtonGenFANN, SIGNAL(clicked()), this, SLOT(ClickButtonGenFANN()));
	connect(PushButtonGenFANN, SIGNAL(clicked()), this, SLOT(ClickButtonGenPyNN()));

	//布局
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
		if (vartype == "输入项")
		{
			QStandardItem* item0 = outputtreemodel->item(i, 0);
			string varname = item0->text().toStdString();
			InputNames.push_back(varname);
		}
		else if (vartype == "输出项")
		{
			QStandardItem* item0 = outputtreemodel->item(i, 0);
			string varname = item0->text().toStdString();
			OutputNames.push_back(varname);
		}

		QStandardItem* item2 = outputtreemodel->item(i, 2);
		string isControl = item2->text().toStdString();
		if (isControl == "是")
		{
			//最小值
			QStandardItem* item3 = outputtreemodel->item(i, 3);
			string str_minValue = item3->text().toStdString();
			double minValue = 0;
			if (str_minValue != "")
			{
				minValue = std::stod(str_minValue);
			}
			//最大值
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
	MessageDisplay += "所设置的输入项和输出项信息如下\n只有控制项包含最大最小值\n最大最小值缺省为0\n";
	//所有控制项
	MessageDisplay += "所有控制项：\n";
	for (int i = 0; i < ControlVars.size(); i++)
	{
		string name = std::get<0>(ControlVars[i]);
		string minval = std::to_string(std::get<1>(ControlVars[i]));
		string maxval = std::to_string(std::get<2>(ControlVars[i]));
		QString print = QString::fromStdString(name) + "     " + QString::fromStdString(minval) + "     " + QString::fromStdString(maxval) + "\n";
		MessageDisplay += print;
	}
	
	//所有输入项
	MessageDisplay += "所有输入项：\n";
	for (int i = 0; i < InputNames.size(); i++)
	{
		MessageDisplay += QString::fromStdString(InputNames[i]);
		MessageDisplay += "\n";
	}

	//所有输出项
	MessageDisplay += "所有输出项：\n";
	for (int i = 0; i < OutputNames.size(); i++)
	{
		MessageDisplay += QString::fromStdString(OutputNames[i]);
		MessageDisplay += "\n";
	}
	
	InputOutputMessage->setText(MessageDisplay);
}

void NeuralnetworkWidget::CreateOutputModel()
{
	//最后构造modeltree																													 
	for (int i = 0; i < allOutputNames.size(); i++)
	{
		QString outputname = allOutputNames[i];
		string strname = outputname.toStdString();
		bool addoutput = false;
		int idx1 = strname.find(".frame_a.r_0"); 
		if (idx1 != string::npos && MethodID != 2)
		{
			// 这里没有考虑子装配体，考虑子装配体的话，需要重写
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
				// 这里没有考虑子装配体，考虑子装配体的话，需要重写
				// FIXME
				string lastname = ".frame_a.R.T[_, _]";
				string firstname = strname.substr(0, strname.length() - lastname.length());
				if (firstname.find(".") == string::npos) {
					addoutput = true;
				}
			}
		}

		//一级节点，加入mModel
		QList<QStandardItem*> items1;
		QStandardItem* item1 = new QStandardItem(outputname);
		//QStandardItem* item2 = new QStandardItem();
		QStandardItem* item2;
		if (addoutput == true)
		{
			item2 = new QStandardItem("输出项");
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
	//输出项名称列设置为不可编辑
	outputtreeview->setItemDelegateForColumn(0, new DisableEditor(outputtreeview));
	outputtreeview->expandAll();
}

//生成训练集
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
	int n = 200; //先设置样本数目默认500，之后再进行调整
	vector<vector<double>> samples = latin_hypercube(dimension, n, ranges, false); //默认先不使用拒绝采样


	if (m_doc && m_doc->IsModel())
	{
		//函数待修改
		//增加一个一键增加输出项的功能
		((DocumentModel*)m_doc)->GenTrainSet(ControlVarsNames, samples, InputNames, OutputNames);
	}
}

//建立并训练神经网络
//不能报告训练状态，可以看看怎么改一下
//加一个测试训练效果的对话框
void NeuralnetworkWidget::ClickButtonGenFANN()
{
	const unsigned int num_input = InputNames.size();       // 输入项个数
	const unsigned int num_output = OutputNames.size();      // 输出项个数
	//const unsigned int num_input = 6;       // 输入项个数
	//const unsigned int num_output = 96;      // 输出项个数
	const unsigned int num_layers = 5;      //神经网络层数（包括输入输出层）
	const unsigned int num_neurons_hidden1 = 6;       //表示隐藏层有6个神经元
	const unsigned int num_neurons_hidden2 = 32;       //表示隐藏层有32个神经元
	const unsigned int num_neurons_hidden3 = 48;       //表示隐藏层有48个神经元
	const float desired_error = (const float)0.00000001;
	const unsigned int max_epochs = 500; // 最多执行次数
	const unsigned int epochs_between_reports = 10;   // 报告频率

	struct fann* ann;

	//int     Num = 200;
	//float   Mf = Num * 3.f;
	//int     i;
	//double  a, b, c;
	//FILE* fp;
	//fopen_s(&fp, "add.fann", "w");
	//fprintf_s(fp, "%d 2 1\n", Num);
	//// 生成训练文件
	//for (i = 1; i <= Num; i++) {
	//    // 生成2个数, 要求在(0,1)之间
	//    a = i / Mf;
	//    b = (i + 1) / Mf;
	//    c = a + b;    // 要求在(0,1)之间

	//    // 输入内容写到训练文件
	//    fprintf_s(fp, "%lf %lf\n%lf\n", a, b, c);
	//}
	//fclose(fp);
	// 样本训练
	//ann = fann_create_standard(num_layers, num_input, num_neurons_hidden1, num_output);
	ann = fann_create_standard(num_layers, num_input, num_neurons_hidden1, num_neurons_hidden2, num_neurons_hidden3, num_output);
	//不完全连接的
	//ann = fann_create_sparse(0.5, num_layers, num_input, num_neurons_hidden1, num_output);

	//设定所有隐藏层的神经元激活函数
	/*fann_set_activation_function_hidden设定所有隐藏层的所有神经元的激活函数，虽然这次我们只有一层隐藏层，
	但当有多层隐藏层时可以用fann_set_activation_function_hidden批量设置激活函数为同一个
	 */
	 //fann_set_activation_function_hidden(ann, FANN_LINEAR);
	fann_set_activation_function_layer(ann, FANN_LINEAR, 1);//记输入层为0,则该隐藏层为1
	fann_set_activation_function_layer(ann, FANN_LINEAR, 2);//记输入层为0,则该隐藏层为1
	fann_set_activation_function_layer(ann, FANN_SIGMOID, 3);//记输入层为0,则该隐藏层为1
	//设定输出层的激活函数
	fann_set_activation_function_output(ann, FANN_LINEAR);

	string fannfilepath = ((DocumentModel*)m_doc)->GetfannSavePath();
	string netfilepath = fannfilepath + ".net";
	//fann_train_on_file(ann, "robotnn0426.fann", max_epochs, epochs_between_reports, desired_error);
	//fann_save(ann, "robotnn0426.fann.net");
	fann_train_on_file(ann, fannfilepath.c_str(), max_epochs, epochs_between_reports, desired_error);
	fann_save(ann, netfilepath.c_str());
	fann_destroy(ann);
}

// 建立并训练Pytorch神经网络的提示
void NeuralnetworkWidget::ClickButtonGenPyNN() {
	QMessageBox msgBox;
	msgBox.setText("请在当前文件路径下，找到pytorchtrain.py文件，更改数据集进行训练！");
	msgBox.exec();
	return;
}

//拉丁超立方采样函数  N-输入值数量（维度）   n-样本数目
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

//创建输入输出下拉列表
QWidget* CheckBoxDelegateNetwork::createEditor(QWidget* parent,
	const QStyleOptionViewItem& /* option */,
	const QModelIndex& index) const
{
	QComboBox* editor = new QComboBox(parent);
	editor->addItem("输入项");
	editor->addItem("输出项");
	editor->addItem("非输入/输出");

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

//创建控制项下拉列表
QWidget* CheckBoxDelegateControl::createEditor(QWidget* parent,
	const QStyleOptionViewItem& /* option */,
	const QModelIndex& index) const
{
	QComboBox* editor = new QComboBox(parent);
	editor->addItem("是");
	editor->addItem("否");

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
