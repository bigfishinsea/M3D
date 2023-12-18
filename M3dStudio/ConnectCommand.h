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
	//����connector1��connector2
	Connector* connector1;
	Connector* connector2;
	gp_Ax3 Ax3From;//connector1������
	gp_Ax3 Ax3To;//connector2������
};

#endif // !CONNECTCOMMAND_H__
