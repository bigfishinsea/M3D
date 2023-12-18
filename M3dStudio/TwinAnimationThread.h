#pragma once
#include "DDSData.h"
#include "DocumentModel.h"
#include "M3d.h"
#include <QThread>
#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <QMutex>

class Component;
class DocumentModel;
class DigitaltwinsWidget;
class NeuralNetworkInfo;

class TwinAnimationThread : public QThread
{
    Q_OBJECT
public:
    TwinAnimationThread(QObject* parent = 0);
    StoredData* m_pStoredData = nullptr; // 之前是为NULL，改完之后不知道是否会有问题

private:
    Handle(AIS_InteractiveContext) m_context;
    DocumentModel* m_pModel;
    vector<Component*> m_vComponents;      //全部零件，不能是部件，相对运动
    QMutex pauseLock;
    string LastReceivedMsg;                //上一条收到的消息

    bool GetAllMultibodyColsData(DocumentModel* pMdl, vector<string>& parameternames, map<string, vector<int>>& mapCompnameCols, \
        map<string, int>& mapCompRotationCol, map<string, int>& mapCompTranslationCol, map<string, vector<int>>& mapFramebPosCols);  //读取组件位姿所在列信息
    int GetComponentConnectType(DocumentModel* pMdl, const string& sColName, string& sCompFullName);                                 //根据列名获取列的类型
    vector<int> GetAllCompCols(const string& sCompFullName, const vector<string>& parameternames);                                   //获取组件名称+frame_a所在列
    vector<int> GetAllCompCols3(const string& sCompFullName, const vector<string>& parameternames);                                  //获取组件名称+frame_a所在列
    gp_Trsf GetTrsfFromCols(const vector<string>& nowdata, const vector<int>& CompnameCols);                                         //从当前数据列中获取gp_Trsf
    gp_Pnt GetPntFromCols(const vector<string>& nowdata, const vector<int>& CompnameFrameBPosCols);                                  //从当前数据列中获取gp_Pnt
    vector<double> GetPlacementValues(const vector<string>& nowdata, const vector<int>& vCols);
    void DisplaceScaleComponent(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
        int i, int k_vec, vector<gp_Vec>& vecDeltaP0, map<string, double> mapCompRab, gp_Pnt ptFrameB);                              //实现组件的放缩和位姿摆放
    void DisplaceComponent_old(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
        int i, int k_vec, vector<gp_Vec>& vecDeltaP0);                                                                               //刚体运动部件,旧版
    void DisplaceRComponent(DocumentModel* pModel, Component* pCom, const char* sRigid, gp_Trsf& trsf, double rotRad, \
        int i, int k_vec, vector<gp_Vec>& vecDeltaP0);                                                                               //先刚体运动，再自转
    void RotateComponent(DocumentModel* pModel, Component* pCom, double rotRad);                                                     //旋转部件

    // 与神经网络相关的函数
    void initNeuralNetwork(vector<pair<NeuralNetworkInfo, bool>>& vecNetworkInfos, vector<string>& parameternames, \
        map<int, vector<int>>& transferTIdx, map<int, vector<int>>& netInputIdx);                                //初始化神经网络   
    void runNeuralNetwork(vector<pair<NeuralNetworkInfo, bool>>& vecNetworkInfos, vector<string>& datas, \
        map<int, vector<int>>& transferTIdx, map<int, vector<int>>& netInputIdx);                                //跑神经网络 
    vector<float> eulerToMatrixT(vector<float>& eulerAngles);

public:
    void SetContext(Handle(AIS_InteractiveContext) pContext) { m_context = pContext; }
    void SetDocumentModel(DocumentModel* pMdl) { m_pModel = pMdl; }
    void SetComponents(vector<Component*>& vComps);
    string GetLastReceivedMsg() { return LastReceivedMsg; }          //获取上一条收到的信息

    void closeThread();//停止线程并使部件回复到初始位置

signals:
    void rep();
    void SendReceivedMsg();


protected:
    void run(); // 新线程入口

};