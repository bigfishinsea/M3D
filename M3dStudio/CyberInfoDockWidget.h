//#include <Afxcoll.h> 
//#include <atlstr.h>
//#define _AFXDLL

#include"MainWindow.h"
#include<qmessagebox.h>
#include <stack>
#include <Standard_WarningsRestore.hxx>
#include<QTextEdit>
#include<QDockWidget>
#include<QLineEdit>
#include<QLabel>
#include<QLineEdit>
#include<QPushButton>
#include<QTableView>
#include<QCheckBox>
#include<QHBoxLayout>
#include <QVBoxLayout>
#include<QStandardItemModel>
#include<QTreeWidget>
#include "ParaTreeModel.h"
#include <QTableWidget>
#include<QAction>
#include <QHeaderView>
#include "cyberInfo.h"

//class MainWindow;
class CyberInfoDockWidget : public  QDockWidget {
	Q_OBJECT
public:
	static bool m_closed;

	MainWindow* mywid;

	CyberInfoDockWidget(MainWindow* mywidget, QAction* sender);
	~CyberInfoDockWidget();

	//创建悬浮窗框架
	void CreateTable(MainWindow*);

	//创建参数表
	void updateTable1();

	//创建输出表
	void updateTable2();
	
	//判断是否为MO文件
	bool IsMOFile(QString sMoFilePath);

	//返回悬浮窗供主窗口调用
	QDockWidget* getdock();

	//打开MO文件
	void OpenMo();

	//创建接口信息栏
	void createConInfo();

	//填充接口信息
	void updateConInfo();

	//获取MO模型全名
	QString GetMdlname(QString sMoFilePath);

	//辅助函数
	void ParaDFS(vector<myParameter*>& m_para, TreeItem* root, string s = "");
	void OutputDFS(vector<myOutputVar*>& m_para, TreeItem* root, string s ="");
	
	//资源回收
	void shutdown();

	bool isclosed() { return m_closed; }
public slots:

	//从视图数据构建CyberInfo并保存至文档
	void saveCyberInfo();

	//从文档CyberInfo构建两个表
	bool readCyberInfo(CyberInfo* pCyberInfo=nullptr);

	//悬浮窗的销毁函数
	void close();

	//关联mo模型更新表
	void	updatetable();

	//关联mo模型
	void	ConnectMo();

	//清空参数表关注
	void	SetTableZero1();

	//清空变量表关注
	void	SetTableZero2();

private:
	
	//主窗口触发信息窗口按钮
	QAction* m_sender;

	//Label文本控件
	QList<QLabel*> LabLists;
	
	//窗口控件
	QList<QWidget*> WidgLists;

	//MO模型名
	QString MoFilename;

	//悬浮窗
	QDockWidget* dock1;

	//参数表视图
	QTreeView* paratreeview;

	//变量树视图
	QTreeView* outputtreeview;

	//参数树模型
	ListTreeModel* paratreemodel;

	//变量树模型
	outputTreeModel* outputtreemodel;

	//接口信息
	vector<pair<QString, QString>> m_cons;

	//接口信息表
	QTableWidget* m_cons_table;
	
	//可编辑控件
	QLineEdit* MdlName;

	//
	//bool m_closed;

};
