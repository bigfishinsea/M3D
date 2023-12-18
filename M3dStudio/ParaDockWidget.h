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
	
	//取得参数表
	QDockWidget* getWidget() {
		return m_dock;
	};
	vector<pair<ParameterValue, bool>>* getdata() { return &m_data; }
	unordered_map<int, string>* getmap() { return &indexmap; }

private slots:
	//更新表格内容
	void currentItemChangedSLOT(QTableWidgetItem*,QTableWidgetItem*);

	//参数表末尾插入一行
	void addline();

	//参数表末尾删除一行
	void delline();

	//关闭悬浮窗
	void close();

	//从文档中获取参数表
	void readParatable(bool b=true);

	//保存参数表
	void saveParatable();
	//自动加载关注的参数
	void LoadParatable();
	//自动计算物性参数
	void autoComputeParatable();

private:
	//初始化参数表
	void initParatable();

	//填写参数表
	void fillParatable();

	QAction* m_sender;
	MainWindow* mywid;
	QTableWidget* m_table;
	QDockWidget* m_dock;
	//当前装配体的一级子组件的关注参数列表,bool变量表示是否已显示
	vector<pair<ParameterValue,bool>> m_data;
	//当前参数表上显示的行与对应m_data之间的映射
	unordered_map<int, string> indexmap;

};

//代理技术实现QTableWidget中的关注下拉列表
class ListDelegate : public QStyledItemDelegate
{
	friend class ParaDockWidget;
	Q_OBJECT
public:
	ListDelegate(ParaDockWidget* dock, QObject* parent = 0) : QStyledItemDelegate(parent), m_Pd(dock) {
		my_map = new unordered_map<string, int>();
	}

	//重载四个自定义方法
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