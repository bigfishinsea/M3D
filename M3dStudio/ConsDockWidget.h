#pragma once

#ifndef CONSDOCKWIDGET_H
#define CONSDOCKWIDGET_H

#include <QDockWidget>

QT_BEGIN_NAMESPACE
class QTableWidget;
class QTimer;
class QMouseEvent;
QT_END_NAMESPACE

class DocumentComponent;
class GeomWidget;
class mainwindow;
class ConsTableWidget;

class ConsDockWidget :public QDockWidget
{
	Q_OBJECT

public:
	ConsDockWidget(QWidget* parent);
	~ConsDockWidget();

	void init();

	//给ConsTableWidget提供DocumentCommon接口
	DocumentComponent* getConsDocument();

protected:

private slots:
	//读取约束
	void OnrestoreConstraints();
	//保存约束
	void OnsaveConstraints();
	//添加约束
	void OnaddConstraint();
	//删除约束
	void OndelConstraint();
	//清空所有约束
	void OnclearAllConstraints();
	//设为未选中任一一行
	void OnunCheck();

private:
	//获取ConsDockWidget所依附的MainWindow的DocumentCommon指针
	DocumentComponent* consDocument3d;
	//约束表
	ConsTableWidget* consTableWidget;
};

#endif // !CONSDOCKWIDGET_H