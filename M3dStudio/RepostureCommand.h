#pragma once

#ifndef REPOSTURECOMMAND_H
#define REPOSTURECOMMAND_H

#include "Command.h"

class AIS_Manipulator;
class Component;

class RepostureCommand:public Command
{
public:
	RepostureCommand(View*);
	virtual ~RepostureCommand();

	void OnLeftButtonDown() override;
	void OnRightButtonDown() override;
	void OnLeftButtonRelease() override;
	void OnRightButtonRelease() override;
	void OnMouseMove() override;

	void InitReposture();
	void CreateManipulator();

	virtual Connector* GetConnector() const { return connector; }
	void SetConnector(Connector* conn) { connector = conn; }
	virtual Component* GetComponent() const { return component; }
	Connector* GetConsConnector() const { return consconnector; }
	Component* GetConsComponent() const { return conscomponent; }
	void SetConsComponent(Component* comp) { conscomponent = comp; }
	void SetConsConnector(Connector* conn) { consconnector = conn; }

	void clearstep() { m_step = 0; }

private:
	Connector* connector;
	Component* component;
	Connector* consconnector;
	Component* conscomponent;
	reposturedialog* _dialog;
	Handle(AIS_Manipulator) aManipulator;
	bool isFreeAdjust;//是否自由体用操作手柄调姿
	gp_Trsf allTrans;//操作手柄总的变换

};

#endif // !REPOSTURECOMMAND_H
