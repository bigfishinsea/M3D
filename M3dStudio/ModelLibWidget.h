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
	//�Ҽ������˵�
	void show_menu();
	//�������
	void insert();
	//���
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

	//����ڵ㣬��ڵ㣬Ҷ�ӽڵ�
	enum ItemType { itTopItem = 1001, itGroupItem, itLeafItem };
	//ͼƬ�У�������
	enum ColType { colItem = 0 };
	//����ʼ��
	void initTree();
	//��ȡ���һ���ļ�����
	QString getFinalfolder(const QString& dir);
	//����Ƿ���ģ���ļ�
	//bool isExistmdl(const QString& dir);
	//�����ļ��л�ȡģ���ļ�·��
	QList<QString> getmdl(const QString& );
	//
	void additem(QString);
	QPixmap GetPixmap(QTreeWidgetItem* theItem);

	//�����ڵ�
	QTreeWidgetItem* addGroupItem(QTreeWidgetItem* , const QString& );
	//���Ҷ�ӽڵ�
	void addLeafItem(QTreeWidgetItem* ,const QString&);
	//����Ҷ�ӽڵ�
	void runLeafItem();
	//�ݹ�����ļ��к���
	bool dfs(QTreeWidgetItem* , const QString&);

signals:
protected:
	//virtual void mousePressEvent(QMouseEvent*)  override;
	//virtual void mouseReleaseEvent(QMouseEvent*)  override;
	//virtual void mouseMoveEvent(QMouseEvent*)  override;
	virtual void leaveEvent(QEvent*)  override;
};

#endif // !ModelLibWidget_H