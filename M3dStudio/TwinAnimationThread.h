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
    StoredData* m_pStoredData = nullptr; // ֮ǰ��ΪNULL������֮��֪���Ƿ��������

private:
    Handle(AIS_InteractiveContext) m_context;
    DocumentModel* m_pModel;
    vector<Component*> m_vComponents;      //ȫ������������ǲ���������˶�
    QMutex pauseLock;
    string LastReceivedMsg;                //��һ���յ�����Ϣ

    bool GetAllMultibodyColsData(DocumentModel* pMdl, vector<string>& parameternames, map<string, vector<int>>& mapCompnameCols, \
        map<string, int>& mapCompRotationCol, map<string, int>& mapCompTranslationCol, map<string, vector<int>>& mapFramebPosCols);  //��ȡ���λ����������Ϣ
    int GetComponentConnectType(DocumentModel* pMdl, const string& sColName, string& sCompFullName);                                 //����������ȡ�е�����
    vector<int> GetAllCompCols(const string& sCompFullName, const vector<string>& parameternames);                                   //��ȡ�������+frame_a������
    vector<int> GetAllCompCols3(const string& sCompFullName, const vector<string>& parameternames);                                  //��ȡ�������+frame_a������
    gp_Trsf GetTrsfFromCols(const vector<string>& nowdata, const vector<int>& CompnameCols);                                         //�ӵ�ǰ�������л�ȡgp_Trsf
    gp_Pnt GetPntFromCols(const vector<string>& nowdata, const vector<int>& CompnameFrameBPosCols);                                  //�ӵ�ǰ�������л�ȡgp_Pnt
    vector<double> GetPlacementValues(const vector<string>& nowdata, const vector<int>& vCols);
    void DisplaceScaleComponent(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
        int i, int k_vec, vector<gp_Vec>& vecDeltaP0, map<string, double> mapCompRab, gp_Pnt ptFrameB);                              //ʵ������ķ�����λ�˰ڷ�
    void DisplaceComponent_old(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
        int i, int k_vec, vector<gp_Vec>& vecDeltaP0);                                                                               //�����˶�����,�ɰ�
    void DisplaceRComponent(DocumentModel* pModel, Component* pCom, const char* sRigid, gp_Trsf& trsf, double rotRad, \
        int i, int k_vec, vector<gp_Vec>& vecDeltaP0);                                                                               //�ȸ����˶�������ת
    void RotateComponent(DocumentModel* pModel, Component* pCom, double rotRad);                                                     //��ת����

    // ����������صĺ���
    void initNeuralNetwork(vector<pair<NeuralNetworkInfo, bool>>& vecNetworkInfos, vector<string>& parameternames, \
        map<int, vector<int>>& transferTIdx, map<int, vector<int>>& netInputIdx);                                //��ʼ��������   
    void runNeuralNetwork(vector<pair<NeuralNetworkInfo, bool>>& vecNetworkInfos, vector<string>& datas, \
        map<int, vector<int>>& transferTIdx, map<int, vector<int>>& netInputIdx);                                //�������� 
    vector<float> eulerToMatrixT(vector<float>& eulerAngles);

public:
    void SetContext(Handle(AIS_InteractiveContext) pContext) { m_context = pContext; }
    void SetDocumentModel(DocumentModel* pMdl) { m_pModel = pMdl; }
    void SetComponents(vector<Component*>& vComps);
    string GetLastReceivedMsg() { return LastReceivedMsg; }          //��ȡ��һ���յ�����Ϣ

    void closeThread();//ֹͣ�̲߳�ʹ�����ظ�����ʼλ��

signals:
    void rep();
    void SendReceivedMsg();


protected:
    void run(); // ���߳����

};