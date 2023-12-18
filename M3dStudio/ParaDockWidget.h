#pragma once
#include"MainWindow.h"
#include <Standard_WarningsRestore.hxx>
#include<QDockWidget>
//#include"M3dMdl_DS.h"
#include<QTableWidget>
//#include "M3dCom_DS.h"
#include<unordered_map>
#include<QStyledItemDelegate>
#include"Component.h"
//class M3dMdl_DL;
//class QTableWidget;
using namespace std;
class ParaDockWidget : public  QWidget {
	Q_OBJECT
public:
	ParaDockWidget(MainWindow* mywidget, QAction* sender);
	~ParaDockWidget();
	
	//ȡ�ò�����
	QDockWidget* getWidget() {
		return m_dock;
	};
	vector<pair<ParameterValue, bool>>* getdata() { return &m_data; }
	unordered_map<int, string>* getmap() { return &indexmap; }

private slots:
	//���±������
	void currentItemChangedSLOT(QTableWidgetItem*,QTableWidgetItem*);

	//������ĩβ����һ��
	void addline();

	//������ĩβɾ��һ��
	void delline();

	//�ر�������
	void close();

	//���ĵ��л�ȡ������
	void readParatable(bool b=true);

	//���������
	void saveParatable();
	//�Զ����ع�ע�Ĳ���
	void LoadParatable();
	//�Զ��������Բ���
	void autoComputeParatable();

private:
	//��ʼ��������
	void initParatable();

	//��д������
	void fillParatable();

	QAction* m_sender;
	MainWindow* mywid;
	QTableWidget* m_table;
	QDockWidget* m_dock;
	//��ǰװ�����һ��������Ĺ�ע�����б�,bool������ʾ�Ƿ�����ʾ
	vector<pair<ParameterValue,bool>> m_data;
	//��ǰ����������ʾ�������Ӧm_data֮���ӳ��
	unordered_map<int, string> indexmap;

};

//������ʵ��QTableWidget�еĹ�ע�����б�
class ListDelegate : public QStyledItemDelegate
{
	friend class ParaDockWidget;
	Q_OBJECT
public:
	ListDelegate(ParaDockWidget* dock, QObject* parent = 0) : QStyledItemDelegate(parent), m_Pd(dock) {
		my_map = new unordered_map<string, int>();
	}

	//�����ĸ��Զ��巽��
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index)const;

	void setEditorData(QWidget* editor, const QModelIndex& index)const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index)const;

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;
private:
	ParaDockWidget* m_Pd;
	 unordered_map<string, int>* my_map;
};