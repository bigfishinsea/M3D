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
	//已经有world定义了吗
	bool HasWorldComponent() const;
    
private:
	//map<string, DocumentCommon*> m_mpFileDC;
    //gp_Ax2 ax2;
	//从参数值的映射组件参数名，获得参数的信息
	myParameter* GetParameterInfo(const string& sMapName) const;

private:
    //根据文档刷新显示
    void UpdateDisplay1();
	//爆炸显示
	void UpdateDisplay2();
    //读文件，得到全部文件-文档映射表
    void ReadFile(const char* fileName);
	////加入显示的零件列表
	//map<string, Handle(AIS_InteractiveObject)> myDisplayList;

public://命令
    //插入部件（包括零件）
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
	//重新生成连接线
	void regenerateConn(Component* change_component);
	//取消重新生成函数
	//生成连接线AIS_InteractiveObject并显示
	void generateConncurvesAIS(Component* comp1, Connector* conn1, Component* comp2, Connector* conn2);
	//生成辅助连接线
	void generateAuxiliaryConncurvesAIS(Component* comp1, Connector* conn1, Component* comp2, Connector* conn2);
	//删除对应的连接线AIS_InteractiveObject
	void deleteConncurvesAIS(Component* comp1, Connector* conn1, Component* comp2, Connector* conn2);

	//组件属性
	void SetComponentAttrib(const char* sComName, ComponentAttrib attrib);
	ComponentAttrib GetComponentAttrib(const char* sComName) const;

	//更改组件的CyberInfo
	void ReplaceCyberInfo();

	//从原来的M3dMdl_DS过来
private:
	void DeleteCompConnectCurves(const char* sCompName);
	void DeleteCompConnConstraits(const char* sCompName);
	void DeleteCompParameterValues(const char* sCompName);

public:
	//读文件，获得文件的M3dMdl_DS对象
	static DocumentModel* ReadModelFile(const char* sFileName);
	//Component* GenerateTopComponent();
	//map<string, CompInfo> GetComponentsInfo()const { return m_mapCompInfo; }

private:
	//组件基本信息：
	//map<string, CompInfo> m_mapCompInfo;
	//list<string> m_componentsNames; //组件名字
	//list<vector<double> >  m_componentsDisplayments; //组件位姿
	//list<string> m_componentsFiles; //组件文件名

	////组件之间的接口约束和连接信息：
	//vector<ConnectConstraint> m_lstConnectConstraints; //接口约束
	//vector<ConnectCurve> m_lstConnectCurves; //连接线

	////参数设置信息：
	//vector<ParameterValue> m_lstParamValues; //参数设置

	//动画线程
	AnimationThread* m_pAnimateThread;

	//孪生动画线程
	TwinAnimationThread* m_pTwinAniThread;

	//故障监测线程
	ErrorMonitorThread* m_pErrorMonitorThread;

public:
	virtual string GetShapeName(const TopoDS_Shape& shpSel) const;
	virtual vector<Connector*> GetConnectors() const;
	virtual void timerEvent(QTimerEvent* event) override;

	//主要接口函数
	bool InsertComponent(Component* pCom); //插入组件到文档
	//Component* GetComponent(TopoDS_Shape& shape); //得到组件
	//Connector* GetComponentConnector(TopoDS_Shape& shape); //得到接口，只搜索组件的第一层接口，其子孙的接口不搜索；
	void DeleteComponent(Component* pCom); //删除组件

	void AddConnConstraint(ConnectConstraint& cc); //增加接口约束
	void DeleteConnConstraint(ConnectConstraint& cc); //删除接口约束
	void SetConnConstraints(vector<ConnectConstraint>& vConnConstrs); //设置全部接口约束，在接口约束界面“更新”时调用
	vector<ConnectConstraint> GetAllConnConstraints() const;

	void AddConnectCurve(ConnectCurve& pCurve); //增加一条连接线
	ConnectCurve* GetConnectCurve(const TopoDS_Shape& shape) const;//拾取一条连接线
	void DeleteConnectCurve(ConnectCurve& cc); //删除一条连接线
	vector<ConnectCurve> GetAllConnectCurves() const;

	void AddParameterValue(ParameterValue& pv, int i=0);
	void DeleteParameterValue(string sParamName);
	void SetParameterValues(vector<ParameterValue>& vPVs);
	vector<ParameterValue> GetAllParameterValues() const;
	bool GetParameterValue(const char* sParName, ParameterValue& pv) const;

public:
	//查找全部组件的基础信息
	vector<CompBaseInfo> GetAllCompInfos() const;
	//对组件进行变换
	string GetLinkRigid(const char* sComName) const;
	//void SetComponentDisplacement(Component* pCom, vector<double>& v18);
	void SetCompOriginTransform(const char* sCompName, gp_Trsf transf);
	void SetCompDeltaTransform(const char* sCompName, vector<double>& v15/*gp_Trsf transf*/);
	gp_Trsf GetCompOriginTransform(const char* sCompName) const;
	gp_Trsf GetCompDeltaTransform(const char* sCompName) const;
	gp_Ax3 GetCompConnGlobalAx3(const char* sComName, const char* sConnName);
	gp_Ax3 GetCompConnGlobalAx3(const char* sComName, int nth); //得到第nth个
	gp_Ax3 GetCompConnLocalAx3(const char* sComName, const char* sConnName);
	vector<double> GetCompDeltaTransformV15(const char* sCompName) const;
public:
	bool InitCheckModel(); //检查模型，接口的连接与匹配；
	bool GenerateModelicaCode(string& sReturnCode, int level); //生成Modelica代码
	bool CompileModel(string& sFileName, string& sModelFullName); //编译模型，生成仿真器
	bool SimulateModel(/*string& sModelFullName, string& sResFileName*/); //仿真运行,得到结果文件
	void CloseSimulate();
	void Animating(); //动画
	void SetCompLocation(Component* pCom, gp_Trsf trs);
	void testLocation(Component* pCom);
	QString ReComputeParamValue(const QString& sComName, const QString& sComParName);
																		  //由结果文件获得变量的时间序列值
	vector<vector<double> > GetVariableTimeSeries(const char* sResFileName, const char* sVarName);
	//从结果文件获得组件的位姿信息，时间序列：每个时刻对应的 ToSystem gn_Ax3
	vector<vector<double> > GetComponentDisplacement(const char* sResFileName, const char* sCompName);

	void AnimatingPause();//暂停动画线程
	void AnimatingResume();//暂停后恢复动画
	void AnimatingReset();//重置动画
	double GetAnimatingTime();//获取当前动画的时间数据
	vector<string> GetAnimatingDatas();//获取当前动画的放映数据
	
	void GenerateSimulateWorkingDirectory();            //创建仿真工作目录文件夹
	void GenerateMosFile(const QString& moFile);        //生成仿真*.mos脚本文件
	void CallSolver(const QString& SaveComparePath);    //调用求解器，执行mos文件，检查结果文件生成
	bool ReSimulateModel();                             //修改参数后重新仿真模型
	void GenerateReMosFile(const QString& moFile);      //生成重新仿真的脚本文件
	void ReCallSolver(const QString& SaveComparePath);  //调用求解器，执行重新仿真mos文件
	QProcess* myProcess;                                //外部调用程序
	vector<QString> AniWidGetMessage();                 //仿真后处理窗口获取仿真信息（错误信息和输出信息）
	vector<QString> AnimationMessage;                   //模型仿真信息 
	void SetPostMessage();                              //设置后处理信息

	bool TwinDriveModel();                              //利用外部传输来的数据驱动孪生模型

private://动画需要的
	int m_iStep;
	vector<double> m_timeSeries;
	map<string, vector<gp_Trsf>> m_mapCompPlacement;
	vector<Component*> m_vDescendants;

public://与数字孪生相关的函数
	int InitSubsAndListner(int discoveryMehodID, bool startMonitor);     //初始化订阅和监听
	int EndSubsAndListner();                                             //结束订阅和监听
	string GetReceivedMsg();                                             //获取接收到的信息
	TwinAnimationThread* GetTwinAniThread() { return m_pTwinAniThread; };          //获取孪生动画线程的指针
	ErrorMonitorThread* getErrorMonitorThread() { return m_pErrorMonitorThread; }  // 获取故障监测线程的指针
	LPCWSTR stringToLPCWSTR(std::string orig);

	void GenTrainSet(vector<string>& ControlVarsNames, vector<vector<double>>& samples, vector<string>& InputNames, vector<string>& OutputNames);      //生成神经网络训练集
	void GenTrainpyFile(vector<string>& ControlVarsNames, vector<vector<double>>& samples, vector<string>& InputNames, vector<string>& OutputNames);   //生成训练的.py文件
	void CallSolverTrain(const QString& SaveComparePath);                                //调用训练的求解器
	void GenTrainSetFromCSV(const QString& SaveComparePath, int samplesize, int inputsize);             //读取csv文件，生成仿真数据集
	void GenTrainSetFromCSVLayer(const QString& SaveComparePath, int samplesize);                       //依照层次生成神经网络，测试用
	bool GenPyTrainSetFromCSVDiffComp(const QString& saveComparePath, int samplesize, vector<string>& InputNames, vector<string>& OutputNames);    // 生成供Pytorch训练用的神经网络训练集，按照不同组件分别生成
	bool GenPyTrainSetFromCSVDiffCompNew(const QString& saveComparePath, int samplesize, vector<string>& InputNames, vector<string>& OutputNames);    // 支持旋转矩阵转欧拉角，支持混合表达
	void GenLayerSet(FILE* fp, string fannFilePath, string inputvar1, string inputvar2, string outputvar, const QStringList& allOutputNames, const QString& SaveComparePath, int samplesize, int rowsbycsv);
	string GetfannSavePath(); //获得mo文件的存储路径
	vector<double> matrixTToEuler(vector<vector<double>>& T);           //旋转矩阵转欧拉角

private:
	//组件+接口#组件+接口 《-》 连接线的AIS_InteractiveObject
	unordered_map<string, vector<Handle(AIS_InteractiveObject)>> connectcurves_map;

public slots:
	void repaint0();
	void exctReadStandardError();
	void exctReadStandardOutput();
	void exctEnd(int, QProcess::ExitStatus);
};