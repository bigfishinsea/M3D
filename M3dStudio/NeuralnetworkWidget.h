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

	//1.CSV�ļ�·�����ã����ڶ�ȡ�������ƣ�
	QLabel* defCsvPath;
	QLineEdit* showCsvPath;
	//����һ��ѡ��csv�ļ�����·��
	QLabel* step1;
	QPushButton* PushButtonSelCsvPath;
	//�������ѡ����������ʽ
	QLabel* step2;
	QButtonGroup* groupButtonWays;              //ѡ�����ϵͳ�������������
	QRadioButton* ButtonT33;                    //ѡ�񳣹���������������תΪŷ���ǣ�
	QRadioButton* ButtonEuler;                  //ѡ��ŷ�������
	QRadioButton* ButtonNone;                   //ѡ��������
	int MethodID = 0;                           //����ID
	//����������ȡcsv�ļ�
	QLabel* step3;
	QPushButton* PushButtonReadCsv;
	QString csvPath;

	//2.�����������
	QLabel* ShowVariable;
	QPushButton* PushButtonSave;                    //���������������ĸ��ĺ�����  
	QTreeView* outputtreeview;                      //��������ͼ
	QStandardItemModel* outputtreemodel;            //������ģ��
	void CreateOutputModel();                       //����������������ģ��
	QStringList allOutputNames;                     //��������������
	
	//��ʱ��ʡ���������
	//ListTreeModel* Attentreemodel;                  //��������������������ģ��
	//void CreateAttenModel();                        //���������Ŀ��

	//3.���������Ϣչʾ
	QLabel* ShowMessage;
	QTextEdit* InputOutputMessage;                      //������Ϣ��
	QString MessageDisplay;                             //������Ϣ
	vector<string> InputNames;                          //������Ҫ����ı�������
	vector<string> OutputNames;                         //��������ı�������
	vector<std::tuple<string, double, double>> ControlVars;    //��������Ԫ��  ���� - ��Сֵ - ���ֵ
	void ShowVarsMessage();                             //�ڷ�����Ϣ��չʾ��Ϣ

	//4.����ѵ����
	QPushButton* PushButtonGenTrainSet;                 //����ѵ����
	vector<vector<double>> latin_hypercube(int N, int n, vector<pair<double, double>>& ranges, bool restricted = true);        //����������������N-����ֵ���� n-��������

	
	//5.������ѵ��������
	QPushButton* PushButtonGenFANN;                     //����������


};

//������ʵ��QTreeview�еĹ�ע�����б�
class CheckBoxDelegateNetwork : public QStyledItemDelegate
{
	Q_OBJECT
public:
	CheckBoxDelegateNetwork(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	//�����ĸ��Զ��巽��
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const;

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

};

//������ʵ��QTreeview�еĹ�ע�����б�
class CheckBoxDelegateControl : public QStyledItemDelegate
{
	Q_OBJECT
public:
	CheckBoxDelegateControl(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	//�����ĸ��Զ��巽��
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const;

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

};
