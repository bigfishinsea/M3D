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
	//������ʾ
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
	//��������ĵ�����˫���¼�
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
	//������ʾ�������������
	void pickupComp();
	//����ڵ㣬��ڵ㣬Ҷ�ӽڵ�
	enum ItemType { itTopItem = 1001, itGroupItem, itLeafItem };
	//ͼƬ�У�������
	enum ColType { colItem = 0 };

	void initTree();
	//void initTable();
	QPixmap GetPixmap(QTreeWidgetItem* theItem);

	//���ڵ������item
	QTreeWidgetItem* addItem(QTreeWidgetItem* ,Component*);
	//�ݹ�����ڵ���������
	void dfs(QTreeWidgetItem*, Component*);
	//ɾ�����
	void del(QTreeWidgetItem* myItem = NULL);
	
	//��ʾ����ؼ�����
	//void showTable(Component*);
	//�����������
	//void changeTable();
	
	DocumentModel* myDocument3d;
	QWidget* Parent;
	
	ImageShowDlg* myImageShowDlg;
	//QTableWidget* myTablewid;
	//QTreeWidget* myTreewid;
	Component* m_root;
	Handle(AIS_ColoredShape) m_obj;
	//��columnӳ�䵽cyberInfo.��ӳ���
	//std::unordered_map<int, int> index;
	int count  ;
	bool m_bTimeIsUp;
	QPoint m_globalPos;
	QTreeWidgetItem* m_curr_item;

	//��Ӷ�ʱ�����������ֵ�����˫��
	QTimer* m_timer;
	int m_Clicked;
	QTreeWidgetItem* previous;
	QTreeWidgetItem* current;
	QMenu* menu;
};

#endif // !AssemblyTreeWidget_H