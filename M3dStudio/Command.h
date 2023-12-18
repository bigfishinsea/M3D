#pragma once

#ifndef COMMAND_H
#define COMMAND_H

#include "M3d.h"
#include <AIS_InteractiveContext.hxx>
#include "View.h"

class DocumentModel;
class Component;
class Connector;
class reposturedialog;

//�����࣬�������
class Command
{
public:
	Command(View*);
	virtual ~Command();

	virtual void OnLeftButtonDown() = 0;
	virtual void OnRightButtonDown() = 0;
	virtual void OnLeftButtonRelease() = 0;
	virtual void OnRightButtonRelease() = 0;
	virtual void OnMouseMove() = 0;

	void CancelCommand();

	virtual void init();
	//������,�ڵ���������ʵ��
	virtual Connector* GetConnector() const;
	virtual Component* GetComponent() const;
	virtual DocumentCommon* GetCmdDocument() { return myDocumentCommon; }

public:
	void setCoordinate(double x, double y) { curr_x = x, curr_y = y; }
	//�ṩ�ӿ�,���ѡ�����������
	//����������������������
	const string getSelectedname();
	View* getCommandView() const { return view; }
	Component* getComproot() const { return m_Comproot; }

protected:
	//��¼��������� 
	int m_step;
	Handle(AIS_InteractiveContext) myContext;
	View* view;

	DocumentCommon* myDocumentCommon;
	Component* m_Comproot;

	//��Ҫ�����¼�������
	double curr_x;
	double curr_y;
};

#endif // !COMMAND_H