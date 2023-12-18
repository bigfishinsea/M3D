#pragma once

#ifndef CONSTABLEWIDGET_H
#define CONSTABLEWIDGET_H

#include <QTableWidget>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <string>
#include <unordered_map>
#include <AIS_Shape.hxx>
#include "geoGonstraint.h"
//#include "M3dCom_DS.h"

QT_BEGIN_NAMESPACE
class QTimer;
class QMouseEvent;
QT_END_NAMESPACE

class DocumentComponent;
class GeomWidget;
class mainwindow;
class ConsDockWidget;
//class M3dCom_DS;

class ConsTableWidget :public QTableWidget
{
	Q_OBJECT

public:
	ConsTableWidget(QWidget* parent);

	void init();
	void readConstraints();
	void readrecord();
	void restoreConstraints();
	void saveConstraints();
	void addConstraint();
	void delConstraint();
	void clearAllConstraints();
	void unCheck();
	void AddShape();
	void show_selectedshape();

signals:

protected:
	//区分左键的单击和双击事件
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;

private slots:
	//右键显示删除菜单
	void show_menu(const QPoint&);

	void OnConsType(const QString&);
	void OnConsValue(const QString&);

	void onTimeout();

private:
	//添加定时器，用来区分单击和双击
	QTimer* m_timer;
	//标志位，记录点击的次数
	int m_Clicked;
	Handle(AIS_Shape) ais_shape;
	DocumentComponent* myConsDocument;
	int R, C;//记录鼠标点击选中的单元格的位置序号
	//在调用readConstraints之后将record赋值为getConstraints得到的约束,每次保存之后都更新，恢复约束就读取record中的约束
	vector<myConstraint> record;
	//可以使用QHash,之后根据约束类型再进行扩充
	unordered_map<string, myConstraintType> consName2Type{ {std::string("面面平行"),PARALLEL},{string("相切"),TAGENT},{string("长度"),DIM_LENGTH},{string("角度"),DIM_ANGLE} };
	unordered_map<myConstraintType, string> consType2Name{ {PARALLEL,std::string("面面平行")},{TAGENT,string("相切")},{DIM_LENGTH,string("长度")},{DIM_ANGLE,string("角度")} };
};

#endif // !CONSTABLEWIDGET_H