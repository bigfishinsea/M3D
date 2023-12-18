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

//命令类，抽象基类
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
	//无意义,在调姿命令中实现
	virtual Connector* GetConnector() const;
	virtual Component* GetComponent() const;
	virtual DocumentCommon* GetCmdDocument() { return myDocumentCommon; }

public:
	void setCoordinate(double x, double y) { curr_x = x, curr_y = y; }
	//提供接口,获得选中组件的名字
	//不可以随意更改组件的名字
	const string getSelectedname();
	View* getCommandView() const { return view; }
	Component* getComproot() const { return m_Comproot; }

protected:
	//记录命令操作步 
	int m_step;
	Handle(AIS_InteractiveContext) myContext;
	View* view;

	DocumentCommon* myDocumentCommon;
	Component* m_Comproot;

	//需要接收事件的坐标
	double curr_x;
	double curr_y;
};

#endif // !COMMAND_H