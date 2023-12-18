#pragma once

#ifndef AssemblyTreeWidget_H
#define AssemblyTreeWidget_H

//#include "DocumentModel.h"

#include <QFrame>
#include <QTreewidget>
#include <QTablewidget>
#include <unordered_map>
#include <AIS_InteractiveContext.hxx>
class DocumentModel;
//class DocumentCommon;
class M3dMdl_DS;
class Component;
class QTimer;
class QMouseEvent;
class AIS_ColoredShape;
class ImageShowDlg;

class AssemblyTreeWidget :public QTreeWidget
{
	Q_OBJECT
public:
	AssemblyTreeWidget(QWidget* parent = nullptr);
	~AssemblyTreeWidget();
	void init();
	//更新显示
	void UpdateAssemblyTree(Component* pRoot);
	void HighLightItem(const char* csItem);

	void addComp(Component* mycomp) {
		addItem(this->topLevelItem(0), mycomp);
	}
protected:
	virtual QSize sizeHint() const override;
	//virtual void mouseReleaseEvent(QMouseEvent* evt) override;
	virtual void highlight(QTreeWidgetItem*);

signals:
protected:
	//区分左键的单击和双击事件
	virtual void mousePressEvent(QMouseEvent*)  override;
	virtual void mouseReleaseEvent(QMouseEvent*)  override;
	virtual void mouseMoveEvent(QMouseEvent*)  override;
	virtual void leaveEvent(QEvent*)  override;

private slots:

	void deleteComponent();
	void renameComponent();
	void repostureComponent();
	void curItemChangedSlot(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void onTimeout();

private:
	void init_menu();
	//
	void show_menu(QMouseEvent* event);
	TopoDS_Shape getTopoDS(Component*);
	//长按显示二级及以下组件
	void pickupComp();
	//顶层节点，组节点，叶子节点
	enum ItemType { itTopItem = 1001, itGroupItem, itLeafItem };
	//图片列，文字列
	enum ColType { colItem = 0 };

	void initTree();
	//void initTable();
	QPixmap GetPixmap(QTreeWidgetItem* theItem);

	//往节点下添加item
	QTreeWidgetItem* addItem(QTreeWidgetItem* ,Component*);
	//递归的往节点下添加组件
	void dfs(QTreeWidgetItem*, Component*);
	//删除组件
	void del(QTreeWidgetItem* myItem = NULL);
	
	//显示组件关键参数
	//void showTable(Component*);
	//调整物理参数
	//void changeTable();
	
	DocumentModel* myDocument3d;
	QWidget* Parent;
	
	ImageShowDlg* myImageShowDlg;
	//QTableWidget* myTablewid;
	//QTreeWidget* myTreewid;
	Component* m_root;
	Handle(AIS_ColoredShape) m_obj;
	//从column映射到cyberInfo.的映射表
	//std::unordered_map<int, int> index;
	int count  ;
	bool m_bTimeIsUp;
	QPoint m_globalPos;
	QTreeWidgetItem* m_curr_item;

	//添加定时器，用来区分单击和双击
	QTimer* m_timer;
	int m_Clicked;
	QTreeWidgetItem* previous;
	QTreeWidgetItem* current;
	QMenu* menu;
};

#endif // !AssemblyTreeWidget_H