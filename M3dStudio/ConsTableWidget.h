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
	//��������ĵ�����˫���¼�
	void mousePressEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;

private slots:
	//�Ҽ���ʾɾ���˵�
	void show_menu(const QPoint&);

	void OnConsType(const QString&);
	void OnConsValue(const QString&);

	void onTimeout();

private:
	//��Ӷ�ʱ�����������ֵ�����˫��
	QTimer* m_timer;
	//��־λ����¼����Ĵ���
	int m_Clicked;
	Handle(AIS_Shape) ais_shape;
	DocumentComponent* myConsDocument;
	int R, C;//��¼�����ѡ�еĵ�Ԫ���λ�����
	//�ڵ���readConstraints֮��record��ֵΪgetConstraints�õ���Լ��,ÿ�α���֮�󶼸��£��ָ�Լ���Ͷ�ȡrecord�е�Լ��
	vector<myConstraint> record;
	//����ʹ��QHash,֮�����Լ�������ٽ�������
	unordered_map<string, myConstraintType> consName2Type{ {std::string("����ƽ��"),PARALLEL},{string("����"),TAGENT},{string("����"),DIM_LENGTH},{string("�Ƕ�"),DIM_ANGLE} };
	unordered_map<myConstraintType, string> consType2Name{ {PARALLEL,std::string("����ƽ��")},{TAGENT,string("����")},{DIM_LENGTH,string("����")},{DIM_ANGLE,string("�Ƕ�")} };
};

#endif // !CONSTABLEWIDGET_H