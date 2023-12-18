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
    bool m_bRunning; //是否在运行
    bool m_bPause;   //暂停
    bool m_bStop;    //停止
    double m_speedK; //播放速度比例

    //与传输数据相关的两个变量
    double datatime;
    vector<string> AllDatas;

private:
    Handle(AIS_InteractiveContext) m_context;
    DocumentModel* m_pModel;
    //全部零件，不能是部件，相对运动
    vector<Component*> m_vComponents;
    //仿真结果文件
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

    void pauseThread();//暂停线程
    void resumeThread();//恢复线程
    void closeThread();//停止线程并使部件回复到初始位置

signals:
    void rep();
    void SendSimulateMsg();
    void SendCSVReadStart();
    void SendCSVReadFinish();

protected:
    void run(); // 新线程入口
};
