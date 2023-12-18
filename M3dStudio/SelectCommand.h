#pragma once

#ifndef SELECTCOMMAND
#define SELECTCOMMAND

#include "Command.h"
#include <QObject>

class SelectCommand :public QObject, public Command
{
	Q_OBJECT
public:
	SelectCommand(View*);
	virtual ~SelectCommand();

	virtual void init();

	void OnLeftButtonDown() override;
	void OnRightButtonDown() override;
	void OnLeftButtonRelease() override;
	void OnRightButtonRelease() override;
	void OnMouseMove() override;

signals:
	void sendSelectContext(QString& text);

};

#endif // !SELECTCOMMAND
