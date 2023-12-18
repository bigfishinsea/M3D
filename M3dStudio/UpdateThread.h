#pragma once
#include "M3d.h"
#include <QThread>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>

class Component;
class DocumentModel;

class UpdateThread : public QThread
{
    Q_OBJECT
public:
    UpdateThread(QObject* parent = 0);
    bool m_bRunning; //�Ƿ�������
    bool m_bPause;   //��ͣ
    bool m_bStop;    //ֹͣ

private:
    Handle(AIS_InteractiveContext) m_context;
    Component* m_Component;

public:
    void SetContext(Handle(AIS_InteractiveContext) pContext) { m_context = pContext; }
    void SetComponent(Component*& Comp) { m_Component = Comp; }
    bool IsRunning() { return m_bRunning; }

signals:
    void rep();

protected:
    void run(); // ���߳����
};
