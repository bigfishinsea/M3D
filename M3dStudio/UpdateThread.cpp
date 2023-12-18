#include "UpdateThread.h"
#include "Component.h"
#include <QTime>

UpdateThread::UpdateThread(QObject* pParent)
	: QThread(pParent)
{
	m_bRunning = false;
	m_bPause = false;
	m_bStop = false;
	m_Component = nullptr;
}


void UpdateThread::run()
{
	m_bRunning = true;

	m_Component->Display(m_context, false);

	m_bRunning = false;
	m_bStop = true;
}