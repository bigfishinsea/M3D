#pragma once

#ifndef ModelLibWidget_H
#define ModelLibWidget_H

//#include "DocumentModel.h"
//#include "M3dMdl_DS.h"

#include <QFrame>
#include <QTreewidget>

class QLabel;
class QPushButton;
class DocumentModel;
class ImageShowDlg;

class ModelLibWidget :public QFrame
{
	Q_OBJECT
public:
	ModelLibWidget(QWidget* parent = nullptr);

	void init();

protected:
	virtual QSize sizeHint() const override;

private slots:
	//右键弹出菜单
	void show_menu();
	//插入组件
	void insert();
	//添加
	void addItem();
	void curItemChangedSlot(QTreeWidgetItem* current, QTreeWidgetItem* previous);
	void mouseMoveSlot(QMouseEvent*);
	void enterItem(QTreeWidgetItem* item, int col);

private:
	DocumentModel* myDocument3d;
	QWidget* Parent;
	ImageShowDlg* myImageShowDlg;

	QLabel* WorkPathLabel;
	QPushButton* BrowseButton;
	QTreeWidget* myTreewid;

	//顶层节点，组节点，叶子节点
	enum ItemType { itTopItem = 1001, itGroupItem, itLeafItem };
	//图片列，文字列
	enum ColType { colItem = 0 };
	//树初始化
	void initTree();
	//获取最后一个文件夹名
	QString getFinalfolder(const QString& dir);
	//检查是否有模型文件
	//bool isExistmdl(const QString& dir);
	//遍历文件夹获取模型文件路径
	QList<QString> getmdl(const QString& );
	//
	void additem(QString);
	QPixmap GetPixmap(QTreeWidgetItem* theItem);

	//添加组节点
	QTreeWidgetItem* addGroupItem(QTreeWidgetItem* , const QString& );
	//添加叶子节点
	void addLeafItem(QTreeWidgetItem* ,const QString&);
	//运行叶子节点
	void runLeafItem();
	//递归遍历文件夹函数
	bool dfs(QTreeWidgetItem* , const QString&);

signals:
protected:
	//virtual void mousePressEvent(QMouseEvent*)  override;
	//virtual void mouseReleaseEvent(QMouseEvent*)  override;
	//virtual void mouseMoveEvent(QMouseEvent*)  override;
	virtual void leaveEvent(QEvent*)  override;
};

#endif // !ModelLibWidget_H