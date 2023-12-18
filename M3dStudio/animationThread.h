#pragma once
#include "M3d.h"
#include <QThread>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <QMutex>

class Component;
class DocumentModel;
class AnimationWidget;

class AnimationThread : public QThread
{
    Q_OBJECT
public:
    AnimationThread(QObject* parent = 0);
    bool m_bRunning; //�Ƿ�������
    bool m_bPause;   //��ͣ
    bool m_bStop;    //ֹͣ
    double m_speedK; //�����ٶȱ���

    //�봫��������ص���������
    double datatime;
    vector<string> AllDatas;

private:
    Handle(AIS_InteractiveContext) m_context;
    DocumentModel* m_pModel;
    //ȫ������������ǲ���������˶�
    vector<Component*> m_vComponents;
    //�������ļ�
    QString m_sCSVFilePath;
    QMutex pauseLock;


public:
    void SetContext(Handle(AIS_InteractiveContext) pContext) { m_context = pContext; }
    void SetDocumentModel(DocumentModel* pMdl) { m_pModel = pMdl; }
    void SetComponents(vector<Component*>& vComps);
    void SetCSVFilePath(const QString& sFile);
    bool IsRunning() { return m_bRunning; }

    void SetPause(bool b0) { m_bPause = b0; }
    void SetStop(bool b0) { m_bStop = b0; }
    void SetSpeedK(double k) { m_speedK = k; }

    void pauseThread();//��ͣ�߳�
    void resumeThread();//�ָ��߳�
    void closeThread();//ֹͣ�̲߳�ʹ�����ظ�����ʼλ��

signals:
    void rep();
    void SendSimulateMsg();
    void SendCSVReadStart();
    void SendCSVReadFinish();

protected:
    void run(); // ���߳����
};
