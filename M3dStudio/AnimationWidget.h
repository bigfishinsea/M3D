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
	double GetAnimationSpeed();//��ö�����������
	double speed = 1;//�����ٶȱ���
	QString GetCSVFilePath();//{ return sCSVFile; }
	void ShowPostMessage(bool isSimulate, bool isSimulateSuccess);
	void ClearMessage();
	SimulInfoSendDlg* GetSimulInfoSendDlg() { return InfoSendDlg; };
	bool haveCreatedInfoSendDlg = false;                //��ʶ�Ƿ񴴽�����Ϣ���ͶԻ���


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
	void MsgCSVReadStart();           //csv�ļ�����Ԥ����ʼ
	void MsgCSVReadFinish();         //csv�ļ�����Ԥ�����

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
	AnimationThread* m_pAnimateThread;//�����߳�
	QWidget* Parent;
	DocumentCommon* m_doc;
	DocumentModel* myDocument3d;
	Component* m_pCompRoot;
	QString MoFilename;                   //MOģ����
	QString sCSVFile;                     //CSV�ļ�·��


	//1.����������
	QLabel* SetAnimation;
	QPushButton* PushButtonPlay;
	QPushButton* PushButtonPause;
	QPushButton* PushButtonResume;
	QPushButton* PushButtonReset;
	QLabel* SetSpeed;
	QDoubleSpinBox* SpeedAdjust;

	//2.������ʾ��
	QLabel* ShowVariable;
	QTreeView* outputtreeview;                      //��������ͼ
	outputTreeModel* outputtreemodel;               //������ģ��
	void CreateOutputModel();                       //����outputtreemodel
	bool readCyberInfo(CyberInfo* pCyberInfo);      //��ȡ��������CyberInfo
	QPushButton* PushButtonUpdateVariable;          //������ʾ����
	QStringList m_outputList;                       //�����������
	string sCompName;                               //�������
	QPushButton* PushButtonOnlyShow;                //ֻ��ʾ��ע��ť
	QPushButton* PushButtonSave;                    //����Թ�ע�ĸ���
	//QSortFilterProxyModel* mProxyModel;             //����model
	void OutputDFS(vector<myOutputVar*>& m_para, TreeItem* root, string s = "");      //��������
	QStandardItemModel* AttenOutputTreeModel = nullptr;       //���ı�����ģ��
	QStringList AttenOutPutList_FullName;           //��ע����������ȫ����
	QStringList outputList_Attention;               //��ע������������״�ṹ��
	vector<int> ColumnCount;                        //������ߵı�������csv�ļ�������

	//3.������ʾ��
	QLabel* ShowParameter;
	QTreeView* paratreeview;                            //��������ͼ
	ListTreeModel* Allparatreemodel;                    //�������в����Ĳ�����ģ��
	void CreateAllParaModel();                          //����Allparatreemodel
	bool readCyberInfo_para(CyberInfo* pCyberInfo);     //��ȡ��������CyberInfo�Ĳ�����Ϣ
	QStandardItemModel* paratreemodel;                  //������ģ��   
	QPushButton* PushButtonShowAllPara;                 //��ʾ���в���
	QStringList m_paraList;                             //��������
	QPushButton* PushButtonShowAttenPara;               //��ʾ��ע����
	QPushButton* PushButtonSavePara;                    //�����޸İ�ť
	vector<pair<ParameterValue, bool>> m_data;          //��ǰװ�����һ��������Ĺ�ע�����б�,bool������ʾ�Ƿ�����ʾ
	unordered_map<int, string> indexmap;                //��ǰ����������ʾ�������Ӧm_data֮���ӳ��
	QPushButton* PushButtonResimulate;                  //���·��水ť
	bool eventFilter(QObject* target, QEvent* event);   //�¼�������

	//4.������Ϣ��ʾ��
	QLabel* ShowMessage;
	QTextEdit* AnimationMessage;                        //������Ϣ��
	QString MessageDisplay;                             //������Ϣ
	string readFileIntoString(char* filename);          //���ļ������Ϣ��string

	//5.������Ϣͬ��������
	QPushButton* PushButtonSendSimData;                 //��ʾ���в���
	SimulInfoSendDlg* InfoSendDlg = nullptr;            //������Ϣ���ͶԻ���



};


//������Ϣ���ͶԻ���
class SimulInfoSendDlg :public QDialog
{
	Q_OBJECT
public:
	SimulInfoSendDlg(QString CSVFilePath, QDialog* parent = nullptr);
	~SimulInfoSendDlg();
	//void SendSimulateData(double time, vector<string> vdata);
	void SetCSVPath(QString csvpath);
	DocumentCommon* m_doc;                //�̳�AnimationWidget���ĵ�
	bool issending = false;               //Ϊtrueʱ������Ϣ��Ϊfalseʱ������
	vector<string> getSendVarsNames() { return sendVars; }   //��ȡ���ͱ���������

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
	QString m_sCSVFilePath;                 //CSV�ļ�·��

	
	//1.������Ŀ�Ļ���id
	QLabel* LocalMessageIdShow;             //������ϢId��ȫ��Ψһ�������ڽ��ն���Ϣ����
	QLineEdit* LocalMessageIdEdit;          //������ϢId�༭��
	QLabel* DestinationMachineIdShow;       //Ŀ�Ļ���idչʾ
	QLineEdit* DestinationMachineIdEdit;    //Ŀ�Ļ���id�༭�������ж��Ŀ�Ļ���id����;�ָ���

	QButtonGroup* groupButtonDiscoveryMethod;    //ѡ��ͬ���ַ�ʽ������ʽ���֡��˵��˷��֣�
	QRadioButton* ButtonRTPS;                    //�˵��˷��ַ�ʽ��Ĭ�ϣ�
	QRadioButton* ButtonDCPSInfoRepo;            //����ʽ���ַ�ʽ
	int MethodID = 0;                            //���ַ�ʽID
	bool initMatch = false;                      //��־�Ƿ�����˳�ʼ������ֹ���³�ʼ������

	//ѡ�����ֶ�
	QLabel* SelectSendVars;
	QPushButton* PushButtonSelectAll;                  //ȫѡ
	QPushButton* PushButtonSelectNone;                 //ȡ��ȫѡ
	QPushButton* PushButtonShowSelected;               //չʾ��ѡ���͵��ֶ�
	QTreeView* sendvarstreeview;                       //�����ֶ�����ͼ
	QStandardItemModel* sendvarstreemodel;             //�����ֶ���ģ��
	QStandardItemModel* selectedtreemodel = nullptr;   //��ѡ�ֶ���ģ��
	void CreateSendVarsModel();                        //����ģ�ͣ���ʼ��ʱʹ��
	vector<string> sendVars;                           //�����ֶε�����


	//2.��ʼ��������ƥ�䰴ť
	QPushButton* PushButtonInitMatch;               //��ʼ��ƥ�䰴ť
	QPushButton* PushButtonCloseMatch;              //����ƥ�䰴ť

	//3.������Ϣʵʱ��ʾ
	QLabel* ShowSendMessage;
	QTextEdit* SendNowMessages;                     //������Ϣ��
	QString SendedMessages;                         //�ѷ�����Ϣ


};


//������ʵ��QTreeview�еĹ�ע�����б�
class CheckBoxDelegateIsSend : public QStyledItemDelegate
{
	Q_OBJECT
public:
	CheckBoxDelegateIsSend(QObject* parent = 0) : QStyledItemDelegate(parent) {}

	//�����ĸ��Զ��巽��
	QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

	void setEditorData(QWidget* editor, const QModelIndex& index) const;
	void setModelData(QWidget* editor, QAbstractItemModel* model,
		const QModelIndex& index) const;

	void updateEditorGeometry(QWidget* editor,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const;

};


//������ʾ�Ի���
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
	QLineEdit* VariableNameEdit;      //����������ʾ��
	QLabel* ParaSelectTips;           //����ѡ����ʾ
	Chart* chart;                     //����ͼ
	QComboBox* cbo_Para;              //����ѡ��������
	QChartView* chartview;            //����ͼʾ
	QString m_sCSVFilePath;           //CSV�ļ�·��
	AnimationWidget* m_parent;        //ָ��AnimationWidget��ָ��

};



//Chart��ʵ��
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
	//���������
	QString xname;
	qreal xmin;
	qreal xmax;
	int xtickc;
	QString yname;
	qreal ymin;
	qreal ymax;
	int ytickc;

public:
	Chart(QWidget* parent = 0, QString _chartname = "����ͼ");
	~Chart() {}
	void setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc, \
		QString _yname, qreal _ymin, qreal _ymax, int _ytickc);
	void buildChart(QList<QPointF> pointlist);

};