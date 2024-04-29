#pragma once
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#include "TwinAnimationThread.h"
#include "ErrorMonitorThread.h"
#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QList>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>
#include <Standard_WarningsRestore.hxx>
#include "DocumentCommon.h"
#include "Component.h"
#include <QProcess>
#include <unordered_map>
#include <unordered_set>

class MainWindow;
class Component;
class AnimationThread;
class TwinAnimationThread;
class ErrorMonitorThread;


class DocumentModel : public DocumentCommon
{
public:
    DocumentModel() { m_IsModel = true; }
    DocumentModel(MainWindow* parent);
    ~DocumentModel();

    virtual void Clear();
    virtual void createNewDoc();
    virtual bool DeleteSelected();
	virtual bool HideSelected();
    void HilightComponent(const char* sComName);
    void SelectComponent(const char* sComName);
	void RenameComponent(const QString& oldName, const QString& newName);
	void HighLightSelectedComponent();
	//�Ѿ���world��������
	bool HasWorldComponent() const;
    
private:
	//map<string, DocumentCommon*> m_mpFileDC;
    //gp_Ax2 ax2;
	//�Ӳ���ֵ��ӳ���������������ò�������Ϣ
	myParameter* GetParameterInfo(const string& sMapName) const;

private:
    //�����ĵ�ˢ����ʾ
    void UpdateDisplay1();
	//��ը��ʾ
	void UpdateDisplay2();
    //���ļ����õ�ȫ���ļ�-�ĵ�ӳ���
    void ReadFile(const char* fileName);
	////������ʾ������б�
	//map<string, Handle(AIS_InteractiveObject)> myDisplayList;

public://����
    //���벿�������������
    void insertComponent();
    void insertComponent(const char * sFile);
    void Assemble2Components();
    void Connect2Components();
    void adjustComponent();
	void adjustComponent0();

    //Component* GenerateComponent(const char * sFileName);
	Component* GenerateComponent(const QString & currPath/*DocumentCommon* pDoc*/);
    virtual void UpdateDisplay(bool b=true);
    virtual bool saveFile(const QString gfileName);
    virtual void SetMainShape(const TopoDS_Shape& aSolid);
    static string GenerateCompName(Component* pCom);

	void Assemble(Component* pCom1, Connector* pCon1, Component* pCom2, Connector* pCon2);
	//��������������
	void regenerateConn(Component* change_component);
	//ȡ���������ɺ���
	//����������AIS_InteractiveObject����ʾ
	void generateConncurvesAIS(Component* comp1, Connector* conn1, Component* comp2, Connector* conn2);
	//���ɸ���������
	void generateAuxiliaryConncurvesAIS(Component* comp1, Connector* conn1, Component* comp2, Connector* conn2);
	//ɾ����Ӧ��������AIS_InteractiveObject
	void deleteConncurvesAIS(Component* comp1, Connector* conn1, Component* comp2, Connector* conn2);

	//�������
	void SetComponentAttrib(const char* sComName, ComponentAttrib attrib);
	ComponentAttrib GetComponentAttrib(const char* sComName) const;

	//���������CyberInfo
	void ReplaceCyberInfo();

	//��ԭ����M3dMdl_DS����
private:
	void DeleteCompConnectCurves(const char* sCompName);
	void DeleteCompConnConstraits(const char* sCompName);
	void DeleteCompParameterValues(const char* sCompName);

public:
	//���ļ�������ļ���M3dMdl_DS����
	static DocumentModel* ReadModelFile(const char* sFileName);
	//Component* GenerateTopComponent();
	//map<string, CompInfo> GetComponentsInfo()const { return m_mapCompInfo; }

private:
	//���������Ϣ��
	//map<string, CompInfo> m_mapCompInfo;
	//list<string> m_componentsNames; //�������
	//list<vector<double> >  m_componentsDisplayments; //���λ��
	//list<string> m_componentsFiles; //����ļ���

	////���֮��Ľӿ�Լ����������Ϣ��
	//vector<ConnectConstraint> m_lstConnectConstraints; //�ӿ�Լ��
	//vector<ConnectCurve> m_lstConnectCurves; //������

	////����������Ϣ��
	//vector<ParameterValue> m_lstParamValues; //��������

	//�����߳�
	AnimationThread* m_pAnimateThread;

	//���������߳�
	TwinAnimationThread* m_pTwinAniThread;

	//���ϼ���߳�
	ErrorMonitorThread* m_pErrorMonitorThread;

public:
	virtual string GetShapeName(const TopoDS_Shape& shpSel) const;
	virtual vector<Connector*> GetConnectors() const;
	virtual void timerEvent(QTimerEvent* event) override;

	//��Ҫ�ӿں���
	bool InsertComponent(Component* pCom); //����������ĵ�
	//Component* GetComponent(TopoDS_Shape& shape); //�õ����
	//Connector* GetComponentConnector(TopoDS_Shape& shape); //�õ��ӿڣ�ֻ��������ĵ�һ��ӿڣ�������Ľӿڲ�������
	void DeleteComponent(Component* pCom); //ɾ�����

	void AddConnConstraint(ConnectConstraint& cc); //���ӽӿ�Լ��
	void DeleteConnConstraint(ConnectConstraint& cc); //ɾ���ӿ�Լ��
	void SetConnConstraints(vector<ConnectConstraint>& vConnConstrs); //����ȫ���ӿ�Լ�����ڽӿ�Լ�����桰���¡�ʱ����
	vector<ConnectConstraint> GetAllConnConstraints() const;

	void AddConnectCurve(ConnectCurve& pCurve); //����һ��������
	ConnectCurve* GetConnectCurve(const TopoDS_Shape& shape) const;//ʰȡһ��������
	void DeleteConnectCurve(ConnectCurve& cc); //ɾ��һ��������
	vector<ConnectCurve> GetAllConnectCurves() const;

	void AddParameterValue(ParameterValue& pv, int i=0);
	void DeleteParameterValue(string sParamName);
	void SetParameterValues(vector<ParameterValue>& vPVs);
	vector<ParameterValue> GetAllParameterValues() const;
	bool GetParameterValue(const char* sParName, ParameterValue& pv) const;

public:
	//����ȫ������Ļ�����Ϣ
	vector<CompBaseInfo> GetAllCompInfos() const;
	//��������б任
	string GetLinkRigid(const char* sComName) const;
	//void SetComponentDisplacement(Component* pCom, vector<double>& v18);
	void SetCompOriginTransform(const char* sCompName, gp_Trsf transf);
	void SetCompDeltaTransform(const char* sCompName, vector<double>& v15/*gp_Trsf transf*/);
	gp_Trsf GetCompOriginTransform(const char* sCompName) const;
	gp_Trsf GetCompDeltaTransform(const char* sCompName) const;
	gp_Ax3 GetCompConnGlobalAx3(const char* sComName, const char* sConnName);
	gp_Ax3 GetCompConnGlobalAx3(const char* sComName, int nth); //�õ���nth��
	gp_Ax3 GetCompConnLocalAx3(const char* sComName, const char* sConnName);
	vector<double> GetCompDeltaTransformV15(const char* sCompName) const;
public:
	bool InitCheckModel(); //���ģ�ͣ��ӿڵ�������ƥ�䣻
	bool GenerateModelicaCode(string& sReturnCode, int level); //����Modelica����
	bool CompileModel(string& sFileName, string& sModelFullName); //����ģ�ͣ����ɷ�����
	bool SimulateModel(/*string& sModelFullName, string& sResFileName*/); //��������,�õ�����ļ�
	void CloseSimulate();
	void Animating(); //����
	void SetCompLocation(Component* pCom, gp_Trsf trs);
	void testLocation(Component* pCom);
	QString ReComputeParamValue(const QString& sComName, const QString& sComParName);
																		  //�ɽ���ļ���ñ�����ʱ������ֵ
	vector<vector<double> > GetVariableTimeSeries(const char* sResFileName, const char* sVarName);
	//�ӽ���ļ���������λ����Ϣ��ʱ�����У�ÿ��ʱ�̶�Ӧ�� ToSystem gn_Ax3
	vector<vector<double> > GetComponentDisplacement(const char* sResFileName, const char* sCompName);

	void AnimatingPause();//��ͣ�����߳�
	void AnimatingResume();//��ͣ��ָ�����
	void AnimatingReset();//���ö���
	double GetAnimatingTime();//��ȡ��ǰ������ʱ������
	vector<string> GetAnimatingDatas();//��ȡ��ǰ�����ķ�ӳ����
	
	void GenerateSimulateWorkingDirectory();            //�������湤��Ŀ¼�ļ���
	void GenerateMosFile(const QString& moFile);        //���ɷ���*.mos�ű��ļ�
	void CallSolver(const QString& SaveComparePath);    //�����������ִ��mos�ļ���������ļ�����
	bool ReSimulateModel();                             //�޸Ĳ��������·���ģ��
	void GenerateReMosFile(const QString& moFile);      //�������·���Ľű��ļ�
	void ReCallSolver(const QString& SaveComparePath);  //�����������ִ�����·���mos�ļ�
	QProcess* myProcess;                                //�ⲿ���ó���
	vector<QString> AniWidGetMessage();                 //��������ڻ�ȡ������Ϣ��������Ϣ�������Ϣ��
	vector<QString> AnimationMessage;                   //ģ�ͷ�����Ϣ 
	void SetPostMessage();                              //���ú�����Ϣ

	bool TwinDriveModel();                              //�����ⲿ��������������������ģ��

private://������Ҫ��
	int m_iStep;
	vector<double> m_timeSeries;
	map<string, vector<gp_Trsf>> m_mapCompPlacement;
	vector<Component*> m_vDescendants;

public://������������صĺ���
	int InitSubsAndListner(int discoveryMehodID, bool startMonitor);     //��ʼ�����ĺͼ���
	int EndSubsAndListner();                                             //�������ĺͼ���
	string GetReceivedMsg();                                             //��ȡ���յ�����Ϣ
	TwinAnimationThread* GetTwinAniThread() { return m_pTwinAniThread; };          //��ȡ���������̵߳�ָ��
	ErrorMonitorThread* getErrorMonitorThread() { return m_pErrorMonitorThread; }  // ��ȡ���ϼ���̵߳�ָ��
	LPCWSTR stringToLPCWSTR(std::string orig);

	void GenTrainSet(vector<string>& ControlVarsNames, vector<vector<double>>& samples, vector<string>& InputNames, vector<string>& OutputNames);      //����������ѵ����
	void GenTrainpyFile(vector<string>& ControlVarsNames, vector<vector<double>>& samples, vector<string>& InputNames, vector<string>& OutputNames);   //����ѵ����.py�ļ�
	void CallSolverTrain(const QString& SaveComparePath);                                //����ѵ���������
	void GenTrainSetFromCSV(const QString& SaveComparePath, int samplesize, int inputsize);             //��ȡcsv�ļ������ɷ������ݼ�
	void GenTrainSetFromCSVLayer(const QString& SaveComparePath, int samplesize);                       //���ղ�����������磬������
	bool GenPyTrainSetFromCSVDiffComp(const QString& saveComparePath, int samplesize, vector<string>& InputNames, vector<string>& OutputNames);    // ���ɹ�Pytorchѵ���õ�������ѵ���������ղ�ͬ����ֱ�����
	bool GenPyTrainSetFromCSVDiffCompNew(const QString& saveComparePath, int samplesize, vector<string>& InputNames, vector<string>& OutputNames);    // ֧����ת����תŷ���ǣ�֧�ֻ�ϱ��
	void GenLayerSet(FILE* fp, string fannFilePath, string inputvar1, string inputvar2, string outputvar, const QStringList& allOutputNames, const QString& SaveComparePath, int samplesize, int rowsbycsv);
	string GetfannSavePath(); //���mo�ļ��Ĵ洢·��
	vector<double> matrixTToEuler(vector<vector<double>>& T);           //��ת����תŷ����

private:
	//���+�ӿ�#���+�ӿ� ��-�� �����ߵ�AIS_InteractiveObject
	unordered_map<string, vector<Handle(AIS_InteractiveObject)>> connectcurves_map;

public slots:
	void repaint0();
	void exctReadStandardError();
	void exctReadStandardOutput();
	void exctEnd(int, QProcess::ExitStatus);
};