#pragma once
#include "DocumentModel.h"
#include "MainWindow.h"
#include <Standard_WarningsRestore.hxx>
#include <QDockWidget>
#include <QTableWidget>
#include <unordered_map>
#include <QStyledItemDelegate>
#include "Component.h"
#include <QLabel>
#include "FeatureButton.h"
#include <QSpinBox>
#include <QStandardItemModel>
#include <QTreeView>
#include "ParaTreeModel.h"
#include <QSortFilterProxyModel>
#include <QPoint>
#include <QDialog>
#include <QtWidgets>

using namespace std;

class NeuralnetworkWidget :public QWidget {
	Q_OBJECT
public:
	NeuralnetworkWidget(QWidget* parent = nullptr);
	~NeuralnetworkWidget();



private slots:
	void ClickButtonSelCsvPath();
	void ClickButtonReadCsv();
	void ClickButtonSave();
	void ClickButtonGenTrainSet();
	void ClickButtonGenFANN();
	void ClickButtonGenPyNN();
	


public slots:



private:
	void dpWidInit();
	QWidget* Parent;
	DocumentCommon* m_doc;
	QFont font;
	Component* m_pCompRoot;

	//1.CSV文件路径设置（用于读取变量名称）
	QLabel* defCsvPath;
	QLineEdit* showCsvPath;
	//步骤一：选择csv文件所在路径
	QLabel* step1;
	QPushButton* PushButtonSelCsvPath;
	//步骤二：选择多体输出方式
	QLabel* step2;
	QButtonGroup* groupButtonWays;              //选择多体系统输出变量的类型
	QRadioButton* ButtonT33;                    //选择常规矩阵输出（后续需转为欧拉角）
	QRadioButton* ButtonEuler;                  //选择欧拉角输出
	QRadioButton* ButtonNone;                   //选择均不输出
	int MethodID = 0;                           //方法ID
	//步骤三：读取csv文件
	QLabel* step3;
	QPushButton* PushButtonReadCsv;
	QString csvPath;

	//2.输入输出项树
	QLabel* ShowVariable;
	QPushButton* PushButtonSave;                    //保存对输入项、输出项的更改和设置  
	QTreeView* outputtreeview;                      //变量树视图
	QStandardItemModel* outputtreemodel;            //变量树模型
	void CreateOutputModel();                       //构造包含所有输出的模型
	QStringList allOutputNames;                     //所有输出项的名称
	
	//暂时先省略这个功能
	//ListTreeModel* Attentreemodel;                  //包含关心输入项输出项的模型
	//void CreateAttenModel();                        //构造关心项目树

	//3.输入输出信息展示
	QLabel* ShowMessage;
	QTextEdit* InputOutputMessage;                      //仿真信息框
	QString MessageDisplay;                             //仿真信息
	vector<string> InputNames;                          //所有需要输入的变量名称
	vector<string> OutputNames;                         //所有输出的变量名称
	vector<std::tuple<string, double, double>> ControlVars;    //控制项三元组  名称 - 最小值 - 最大值
	void ShowVarsMessage();                             //在仿真信息框展示信息

	//4.生成训练集
	QPushButton* PushButtonGenTrainSet;                 //生成训练集
	vector<vector<double>> latin_hypercube(int N, int n, vector<pair<double, double>>& ranges, bool restricted = true);        //拉丁超立方采样，N-输入值数量 n-样本数量

	
	//5.建立并训练神经网络
	QPushButton* PushButtonGenFANN;                     //生成神经网络


};

//代理技术实现QTreeview中的关注下拉列表
class CheckBoxDelegateNetwork : public QStyledItemDelegate
{
	Q_OBJECT
public:
	CheckBoxDelegateNetwork(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	//重载四个自定义方法
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const;

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

};

//代理技术实现QTreeview中的关注下拉列表
class CheckBoxDelegateControl : public QStyledItemDelegate
{
	Q_OBJECT
public:
	CheckBoxDelegateControl(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	//重载四个自定义方法
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const;

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

};
