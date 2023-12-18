#pragma once

#ifndef ASSEMBLYCOMMAND_H
#define ASSEMBLYCOMMAND_H

#include "Command.h"

class View;
class Connector;

class AssemblyCommand :public Command
{
public:
	AssemblyCommand(View*);
	virtual ~AssemblyCommand();

	void OnLeftButtonDown() override;
	void OnRightButtonDown() override;
	void OnLeftButtonRelease() override;
	void OnRightButtonRelease() override;
	void OnMouseMove() override;

	list<Component*> GetAllNotAssembly(Component*);

	void release();

private:
	void DFS(Component* comp, list<Component*>& l);

	DocumentModel* myDocumentModel;
	//½«connector2ÒÆ¶¯µ½connector1
	Component* pCom1;
	Connector* connector1;
	Connector* connector2;
};

#endif // !ASSEMBLYCOMMAND_H
