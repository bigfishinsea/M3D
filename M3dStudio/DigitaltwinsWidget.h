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

	StoredData* GetStoredData() { return m_pStoredData; };//��ô������ݵ�ָ��
	bool getIsUseNet();
	bool getCanLoadNet();
	vector<pair<NeuralNetworkInfo, bool>> getNetworkInfos() { return m_data; } // �����������Ϣ


private slots:
	void ClickButtonInitMatch();
	void ClickButtonCloseMatch();
	void ShowReceivedMsg();
	void ClickButtonUpdateVariable();
	void ClickButtonOnlyShow();
	void ClickButtonSave();
	void slotTreeMenu(const QPoint& pos);//������ʾ�����Ҽ��˵�
	void MenuGeneCurves();
	void ClickButtonNoUseNet();
	void ClickButtonAddLine();
	void ClickButtonDelLine();
	void ClickButtonCheckConfig();
	void currentItemChangedSLOT(QTableWidgetItem*, QTableWidgetItem*);    //���±������
	

public slots:
	


private:
	void dpWidInit();
	QWidget* Parent;
	DocumentCommon* m_doc;
	QFont font;
	Component* m_pCompRoot;

	//1.�����ƥ����
	QLabel* MatchComputer;
	//����id�����������Ϣid�������ж����
	QLabel* LocalMachineIdShow;                     //����idչʾ
	QLineEdit* LocalMachineIdEdit;                  //����id�༭��
	QLabel* ReceivedMessageIdShow;                  //���������Ϣid�������ж����չʾ
	QLineEdit* ReceivedMessageIdEdit;               //���������Ϣid�������ж�����༭���������Ϣ��;�ָ�
	
	QButtonGroup* groupButtonDiscoveryMethod;    //ѡ��ͬ���ַ�ʽ������ʽ���֡��˵��˷��֣�
	QRadioButton* ButtonRTPS;                    //�˵��˷��ַ�ʽ��Ĭ�ϣ�
	QRadioButton* ButtonDCPSInfoRepo;            //����ʽ���ַ�ʽ
	int MethodID = 0;                            //���ַ�ʽID
	QProcess* myProcess;                         //�ⲿ���ó��������ڼ���ʽ���ַ�ʽ�¿���DCPS����
	bool DCPSopen = false;                       //��־DCPS�����Ƿ���
	bool initMatch = false;                      //��־�Ƿ�����˳�ʼ������ֹ���³�ʼ������
	
	//��ʼ��������ƥ�䰴ť
	QPushButton* PushButtonInitMatch;               //��ʼ��ƥ�䰴ť
	QPushButton* PushButtonCloseMatch;              //����ƥ�䰴ť


	//2.�����������
	QLabel* LoadNeuralNetwork;
	QPushButton* PushButtonNoUseNet;
	QPushButton* PushButtonAddLine;
	QPushButton* PushButtonDelLine;
	QPushButton* PushButtonCheckConfig;     //��������磬Ŀǰ�յ��������ܷ񸲸����������룬�����ܸ��ǣ��������粻���ã��������ʾ
	QTableWidget* m_table;
	vector<pair<NeuralNetworkInfo, bool>> m_data;   //false������ɾ����true����δɾ��
	unordered_map<int, int> indexmap;               //��ǰ������ʾ�������Ӧm_data֮���ӳ��
	void fillPathtable();                           //����ļ�·����

	bool isUseNet = false;                 // �Ƿ�ʹ�����������Ԥ��
	std::mutex mtxUseNet;
	bool canLoadNet = false;               // �Ƿ���������������������ָ�Ƿ���Կ�ʼload .pt������ģ�ͣ����ع��̷��������������߳��У�
	std::mutex mtxLoadNet;


	
	//3.������ʾ��
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
	void OutputDFS(vector<myOutputVar*>& m_para, TreeItem* root, string s = "");      //��������
	QStandardItemModel* AttenOutputTreeModel;       //���ı�����ģ��
	QStringList AttenOutPutList_FullName;           //��ע����������ȫ����
	QStringList outputList_Attention;               //��ע������������״�ṹ��
	vector<int> ColumnCount;                        //������ߵı�������csv�ļ�������
	StoredData* m_pStoredData;                      //�洢�������ݵ�ָ��
													
	
	//4.������Ϣ��ʾ��
	QLabel* ShowMessage;
	QTextEdit* ReceivedMessage;                     //���յ���Ϣ��
	QString MessageDisplay;                         //���յ���Ϣ




};


//������ʾ�Ի���
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
	QLineEdit* VariableNameEdit;      //����������ʾ��
	QLabel* ParaSelectTips;           //����ѡ����ʾ
	DynamicChart* chart;              //����ͼ
	QComboBox* cbo_Para;              //����ѡ��������
	QChartView* chartview;            //����ͼʾ
	DigitaltwinsWidget* m_parent;     //ָ��DigitaltwinsWidget��ָ��
	
	QList<QPointF> pointlist;
	int nowcol = -1;
	double max_value;
	double min_value;
	double max_time;
	int pretoreadRow = 0;
	double lasttime;

	/* ����ģ������ʵʱ���ݵĶ�ʱ�� */
	QTimer* m_timer;

};



//Chart��ʵ��
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
	DynamicChart(QWidget* parent = 0, QString _chartname = "����ͼ");
	~DynamicChart() {}
	void setAxis(QString _xname, qreal _xmin, qreal _xmax, int _xtickc, \
		QString _yname, qreal _ymin, qreal _ymax, int _ytickc);
	void buildChart(QList<QPointF> pointlist);
	void ChangeData(QList<QPointF> pointlist);
	void AddData(QList<QPointF> pointlist);
};