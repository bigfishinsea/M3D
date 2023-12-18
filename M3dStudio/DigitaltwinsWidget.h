#pragma once
#include "NeuralNetworkInfo.h"
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
#include <QSpinBox>
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
#include <mutex>

QT_CHARTS_USE_NAMESPACE
using namespace std;

class DynamicChart;


class DigitaltwinsWidget :public QWidget {
	Q_OBJECT
public:
	//AnimationWidget(MainWindow* mywidget);
	DigitaltwinsWidget(QWidget* parent = nullptr);
	~DigitaltwinsWidget();

	StoredData* GetStoredData() { return m_pStoredData; };//获得储存数据的指针
	bool getIsUseNet();
	bool getCanLoadNet();
	vector<pair<NeuralNetworkInfo, bool>> getNetworkInfos() { return m_data; } // 获得神经网络信息


private slots:
	void ClickButtonInitMatch();
	void ClickButtonCloseMatch();
	void ShowReceivedMsg();
	void ClickButtonUpdateVariable();
	void ClickButtonOnlyShow();
	void ClickButtonSave();
	void slotTreeMenu(const QPoint& pos);//变量显示栏的右键菜单
	void MenuGeneCurves();
	void ClickButtonNoUseNet();
	void ClickButtonAddLine();
	void ClickButtonDelLine();
	void ClickButtonCheckConfig();
	void currentItemChangedSLOT(QTableWidgetItem*, QTableWidgetItem*);    //更新表格内容
	

public slots:
	


private:
	void dpWidInit();
	QWidget* Parent;
	DocumentCommon* m_doc;
	QFont font;
	Component* m_pCompRoot;

	//1.计算机匹配栏
	QLabel* MatchComputer;
	//本机id、允许接收消息id（可能有多个）
	QLabel* LocalMachineIdShow;                     //本机id展示
	QLineEdit* LocalMachineIdEdit;                  //本机id编辑栏
	QLabel* ReceivedMessageIdShow;                  //允许接收消息id（可能有多个）展示
	QLineEdit* ReceivedMessageIdEdit;               //允许接收消息id（可能有多个）编辑栏，多个消息用;分隔
	
	QButtonGroup* groupButtonDiscoveryMethod;    //选择不同发现方式（集中式发现、端到端发现）
	QRadioButton* ButtonRTPS;                    //端到端发现方式（默认）
	QRadioButton* ButtonDCPSInfoRepo;            //集中式发现方式
	int MethodID = 0;                            //发现方式ID
	QProcess* myProcess;                         //外部调用程序，用于在集中式发现方式下开启DCPS服务
	bool DCPSopen = false;                       //标志DCPS服务是否开启
	bool initMatch = false;                      //标志是否进行了初始化，防止重新初始化报错
	
	//初始化及结束匹配按钮
	QPushButton* PushButtonInitMatch;               //初始化匹配按钮
	QPushButton* PushButtonCloseMatch;              //结束匹配按钮


	//2.神经网络加载栏
	QLabel* LoadNeuralNetwork;
	QPushButton* PushButtonNoUseNet;
	QPushButton* PushButtonAddLine;
	QPushButton* PushButtonDelLine;
	QPushButton* PushButtonCheckConfig;     //检查神经网络，目前收到的数据能否覆盖神经网络输入，若不能覆盖，则神经网络不可用，需输出提示
	QTableWidget* m_table;
	vector<pair<NeuralNetworkInfo, bool>> m_data;   //false代表已删除，true代表未删除
	unordered_map<int, int> indexmap;               //当前表上显示的行与对应m_data之间的映射
	void fillPathtable();                           //填充文件路径表

	bool isUseNet = false;                 // 是否使用神经网络进行预测
	std::mutex mtxUseNet;
	bool canLoadNet = false;               // 是否满足加载神经网络的条件（指是否可以开始load .pt神经网络模型，加载过程发生在孪生动画线程中）
	std::mutex mtxLoadNet;


	
	//3.变量显示栏
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
	void OutputDFS(vector<myOutputVar*>& m_para, TreeItem* root, string s = "");      //辅助函数
	QStandardItemModel* AttenOutputTreeModel;       //关心变量树模型
	QStringList AttenOutPutList_FullName;           //关注变量链表（带全名）
	QStringList outputList_Attention;               //关注变量链表（带树状结构）
	vector<int> ColumnCount;                        //输出曲线的变量所在csv文件的列数
	StoredData* m_pStoredData;                      //存储接收数据的指针
													
	
	//4.接受信息提示栏
	QLabel* ShowMessage;
	QTextEdit* ReceivedMessage;                     //已收到信息框
	QString MessageDisplay;                         //已收到消息




};


//曲线显示对话框
class DynamicCurveDisplayDlg : public QDialog
{
	Q_OBJECT
public:
	DynamicCurveDisplayDlg(string VariableName, vector<int> ColumnCount, DigitaltwinsWidget* parent);
	~DynamicCurveDisplayDlg();

signals:

private slots:
	void ParaChanged();
	void slotTimeout();

private:
	void CurveDisplayDlgInit(string VariableName, vector<int> ColumnCount);
	QString BtnStyle;
	QLabel* VariaNameShow;
	QLineEdit* VariableNameEdit;      //变量名称显示栏
	QLabel* ParaSelectTips;           //参数选择提示
	DynamicChart* chart;              //曲线图
	QComboBox* cbo_Para;              //参数选择下拉框
	QChartView* chartview;            //曲线图示
	DigitaltwinsWidget* m_parent;     //指向DigitaltwinsWidget的指针
	
	QList<QPointF> pointlist;
	int nowcol = -1;
	double max_value;
	double min_value;
	double max_time;
	int pretoreadRow = 0;
	double lasttime;

	/* 用于模拟生成实时数据的定时器 */
	QTimer* m_timer;

};



//Chart类实现
class DynamicChart : public QWidget
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
	DynamicChart(QWidget* parent = 0, QString _chartname = "曲线图");
	~DynamicChart() {}
	void setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc, \
		QString _yname, qreal _ymin, qreal _ymax, int _ytickc);
	void buildChart(QList<QPointF> pointlist);
	void ChangeData(QList<QPointF> pointlist);
	void AddData(QList<QPointF> pointlist);
};