#pragma once
#include "DocumentModel.h"
#include "MainWindow.h"
#include <Standard_WarningsRestore.hxx>
#include <QDockWidget>
#include <QTableWidget>
#include <unordered_map>
#include <QStyledItemDelegate>
#include "Component.h"
#include <QLabel>
#include "FeatureButton.h"
//#include "DocumentModel.h"
#include <QSpinBox>
#include "animationThread.h"
#include <QStandardItemModel>
#include <QTreeView>
#include "ParaTreeModel.h"
#include <QSortFilterProxyModel>
#include <QPoint>
#include <QDialog>

#include <QChart>
#include <QChartView>
#include <QSplineSeries>
#include <QHBoxLayout>
#include <QValueAxis>
#include <QTextEdit>
#include <QButtonGroup>
#include "qradiobutton.h"

QT_CHARTS_USE_NAMESPACE
using namespace std;

class Chart;
class SimulInfoSendDlg;

class AnimationWidget :public QWidget {
	Q_OBJECT
public:
	//AnimationWidget(MainWindow* mywidget);
	AnimationWidget(QWidget* parent = nullptr);
	~AnimationWidget();

	//QDockWidget* getWidget() {
	//	return m_dock;
	//};
	double GetAnimationSpeed();//获得动画播放速率
	double speed = 1;//设置速度倍率
	QString GetCSVFilePath();//{ return sCSVFile; }
	void ShowPostMessage(bool isSimulate, bool isSimulateSuccess);
	void ClearMessage();
	SimulInfoSendDlg* GetSimulInfoSendDlg() { return InfoSendDlg; };
	bool haveCreatedInfoSendDlg = false;                //标识是否创建过信息发送对话框


private slots:
	void ClickButtonPlay();
	void ClickButtonPause();
	void ClickButtonResume();
	void ClickButtonReset();
	void ClickButtonSpeedAdjust();
	void ClickButtonUpdateVariable();
	void ClickButtonOnlyShow();
	void ClickButtonSave();
	void slotTreeMenu(const QPoint& pos);
	void MenuGeneCurves();
	void ClickButtonShowAllPara();
	void ClickButtonShowAttenPara();
	void ClickButtonSavePara();
	void ClickButtonResimulate();
	void slotParaTreeMenu(const QPoint& pos);
	void AddParaAtten();
	void CancelParaAtten();
	void ClickButtonSendSimData();
	void MsgCSVReadStart();           //csv文件数据预读开始
	void MsgCSVReadFinish();         //csv文件数据预读完成

public slots:
	void GetAnimationMessage(bool isEnd);
	void exctReadStandardError();
	void exctReadStandardOutput();
	void exctEnd(int, QProcess::ExitStatus);


private:
	void dpWidInit();
	QDockWidget* m_dock;
	MainWindow* mywid;
	//QTableWidget* m_table;
	QFont font;
	AnimationThread* m_pAnimateThread;//动画线程
	QWidget* Parent;
	DocumentCommon* m_doc;
	DocumentModel* myDocument3d;
	Component* m_pCompRoot;
	QString MoFilename;                   //MO模型名
	QString sCSVFile;                     //CSV文件路径


	//1.动画调节栏
	QLabel* SetAnimation;
	QPushButton* PushButtonPlay;
	QPushButton* PushButtonPause;
	QPushButton* PushButtonResume;
	QPushButton* PushButtonReset;
	QLabel* SetSpeed;
	QDoubleSpinBox* SpeedAdjust;

	//2.变量显示栏
	QLabel* ShowVariable;
	QTreeView* outputtreeview;                      //变量树视图
	outputTreeModel* outputtreemodel;               //变量树模型
	void CreateOutputModel();                       //构造outputtreemodel
	bool readCyberInfo(CyberInfo* pCyberInfo);      //读取各部件的CyberInfo
	QPushButton* PushButtonUpdateVariable;          //更新显示变量
	QStringList m_outputList;                       //输出变量链表
	string sCompName;                               //组件名称
	QPushButton* PushButtonOnlyShow;                //只显示关注按钮
	QPushButton* PushButtonSave;                    //保存对关注的更改
	//QSortFilterProxyModel* mProxyModel;             //代理model
	void OutputDFS(vector<myOutputVar*>& m_para, TreeItem* root, string s = "");      //辅助函数
	QStandardItemModel* AttenOutputTreeModel = nullptr;       //关心变量树模型
	QStringList AttenOutPutList_FullName;           //关注变量链表（带全名）
	QStringList outputList_Attention;               //关注变量链表（带树状结构）
	vector<int> ColumnCount;                        //输出曲线的变量所在csv文件的列数

	//3.参数显示栏
	QLabel* ShowParameter;
	QTreeView* paratreeview;                            //参数表视图
	ListTreeModel* Allparatreemodel;                    //包含所有参数的参数树模型
	void CreateAllParaModel();                          //构造Allparatreemodel
	bool readCyberInfo_para(CyberInfo* pCyberInfo);     //读取各部件的CyberInfo的参数信息
	QStandardItemModel* paratreemodel;                  //参数树模型   
	QPushButton* PushButtonShowAllPara;                 //显示所有参数
	QStringList m_paraList;                             //参数链表
	QPushButton* PushButtonShowAttenPara;               //显示关注参数
	QPushButton* PushButtonSavePara;                    //保存修改按钮
	vector<pair<ParameterValue, bool>> m_data;          //当前装配体的一级子组件的关注参数列表,bool变量表示是否已显示
	unordered_map<int, string> indexmap;                //当前参数表上显示的行与对应m_data之间的映射
	QPushButton* PushButtonResimulate;                  //重新仿真按钮
	bool eventFilter(QObject* target, QEvent* event);   //事件过滤器

	//4.仿真信息提示栏
	QLabel* ShowMessage;
	QTextEdit* AnimationMessage;                        //仿真信息框
	QString MessageDisplay;                             //仿真信息
	string readFileIntoString(char* filename);          //从文件里读信息到string

	//5.仿真信息同步发送栏
	QPushButton* PushButtonSendSimData;                 //显示所有参数
	SimulInfoSendDlg* InfoSendDlg = nullptr;            //仿真信息发送对话框



};


//仿真信息发送对话框
class SimulInfoSendDlg :public QDialog
{
	Q_OBJECT
public:
	SimulInfoSendDlg(QString CSVFilePath, QDialog* parent = nullptr);
	~SimulInfoSendDlg();
	//void SendSimulateData(double time, vector<string> vdata);
	void SetCSVPath(QString csvpath);
	DocumentCommon* m_doc;                //继承AnimationWidget的文档
	bool issending = false;               //为true时发送信息，为false时不发送
	vector<string> getSendVarsNames() { return sendVars; }   //获取发送变量的名称

signals:
	// void rep();

private slots:
	void ClickButtonInitMatch();
	void ClickButtonCloseMatch();
	void SendSimulateData();
	void ClickButtonSelectAll();
	void ClickButtonSelectNone();
	void ClickButtonShowSelected();


private:
	QDialog* Parent;
	void SimulInfoSendDlgInit();
	QFont font;
	QString BtnStyle;
	QString m_sCSVFilePath;                 //CSV文件路径

	
	//1.本机、目的机器id
	QLabel* LocalMessageIdShow;             //本地消息Id（全局唯一），用于接收端消息过滤
	QLineEdit* LocalMessageIdEdit;          //本地消息Id编辑栏
	QLabel* DestinationMachineIdShow;       //目的机器id展示
	QLineEdit* DestinationMachineIdEdit;    //目的机器id编辑栏（可有多个目的机器id，用;分隔）

	QButtonGroup* groupButtonDiscoveryMethod;    //选择不同发现方式（集中式发现、端到端发现）
	QRadioButton* ButtonRTPS;                    //端到端发现方式（默认）
	QRadioButton* ButtonDCPSInfoRepo;            //集中式发现方式
	int MethodID = 0;                            //发现方式ID
	bool initMatch = false;                      //标志是否进行了初始化，防止重新初始化报错

	//选择发送字段
	QLabel* SelectSendVars;
	QPushButton* PushButtonSelectAll;                  //全选
	QPushButton* PushButtonSelectNone;                 //取消全选
	QPushButton* PushButtonShowSelected;               //展示已选择发送的字段
	QTreeView* sendvarstreeview;                       //发送字段树视图
	QStandardItemModel* sendvarstreemodel;             //发送字段树模型
	QStandardItemModel* selectedtreemodel = nullptr;   //已选字段树模型
	void CreateSendVarsModel();                        //构造模型，初始化时使用
	vector<string> sendVars;                           //发送字段的名称


	//2.初始化及结束匹配按钮
	QPushButton* PushButtonInitMatch;               //初始化匹配按钮
	QPushButton* PushButtonCloseMatch;              //结束匹配按钮

	//3.发送消息实时显示
	QLabel* ShowSendMessage;
	QTextEdit* SendNowMessages;                     //发送信息框
	QString SendedMessages;                         //已发送信息


};


//代理技术实现QTreeview中的关注下拉列表
class CheckBoxDelegateIsSend : public QStyledItemDelegate
{
	Q_OBJECT
public:
	CheckBoxDelegateIsSend(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	//重载四个自定义方法
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const;

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

};


//曲线显示对话框
class CurveDisplayDlg : public QDialog
{
	Q_OBJECT
public:
	CurveDisplayDlg(string VariableName, vector<int> ColumnCount, AnimationWidget* parent);
	~CurveDisplayDlg();

signals:

private slots:
	void ParaChanged();

private:
	void CurveDisplayDlgInit(string VariableName, vector<int> ColumnCount);
	QString BtnStyle;
	QLabel* VariaNameShow;
	QLineEdit* VariableNameEdit;      //变量名称显示栏
	QLabel* ParaSelectTips;           //参数选择提示
	Chart* chart;                     //曲线图
	QComboBox* cbo_Para;              //参数选择下拉框
	QChartView* chartview;            //曲线图示
	QString m_sCSVFilePath;           //CSV文件路径
	AnimationWidget* m_parent;        //指向AnimationWidget的指针

};



//Chart类实现
class Chart : public QWidget
{
	Q_OBJECT

public:
	QChart* qchart;

private:
	QSplineSeries* series;
	QHBoxLayout* layout;
	QValueAxis* axisX;
	QValueAxis* axisY;

	QString chartname;
	//坐标轴参数
	QString xname;
	qreal xmin;
	qreal xmax;
	int xtickc;
	QString yname;
	qreal ymin;
	qreal ymax;
	int ytickc;

public:
	Chart(QWidget* parent = 0, QString _chartname = "曲线图");
	~Chart() {}
	void setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc, \
		QString _yname, qreal _ymin, qreal _ymax, int _ytickc);
	void buildChart(QList<QPointF> pointlist);

};