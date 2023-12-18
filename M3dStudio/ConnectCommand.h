#pragma once
#ifndef CONNECTCOMMAND_H__
#define CONNECTCOMMAND_H__

#include "Command.h"

class View;

class ConnectCommand :public Command
{
public:
	ConnectCommand(View*);
	virtual ~ConnectCommand();

	void OnLeftButtonDown() override;
	void OnRightButtonDown() override;
	void OnLeftButtonRelease() override;
	void OnRightButtonRelease() override;
	void OnMouseMove() override;

	void release();

private:
	Component* comp1;
	Component* comp2;
	//连接connector1和connector2
	Connector* connector1;
	Connector* connector2;
	gp_Ax3 Ax3From;//connector1的坐标
	gp_Ax3 Ax3To;//connector2的坐标
};

#endif // !CONNECTCOMMAND_H__
