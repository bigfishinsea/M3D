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

	//�������������
	void CreateTable(MainWindow*);

	//����������
	void updateTable1();

	//���������
	void updateTable2();
	
	//�ж��Ƿ�ΪMO�ļ�
	bool IsMOFile(QString sMoFilePath);

	//�����������������ڵ���
	QDockWidget* getdock();

	//��MO�ļ�
	void OpenMo();

	//�����ӿ���Ϣ��
	void createConInfo();

	//���ӿ���Ϣ
	void updateConInfo();

	//��ȡMOģ��ȫ��
	QString GetMdlname(QString sMoFilePath);

	//��������
	void ParaDFS(vector<myParameter*>& m_para, TreeItem* root, string s = "");
	void OutputDFS(vector<myOutputVar*>& m_para, TreeItem* root, string s ="");
	
	//��Դ����
	void shutdown();

	bool isclosed() { return m_closed; }
public slots:

	//����ͼ���ݹ���CyberInfo���������ĵ�
	void saveCyberInfo();

	//���ĵ�CyberInfo����������
	bool readCyberInfo(CyberInfo* pCyberInfo=nullptr);

	//�����������ٺ���
	void close();

	//����moģ�͸��±�
	void	updatetable();

	//����moģ��
	void	ConnectMo();

	//��ղ������ע
	void	SetTableZero1();

	//��ձ������ע
	void	SetTableZero2();

private:
	
	//�����ڴ�����Ϣ���ڰ�ť
	QAction* m_sender;

	//Label�ı��ؼ�
	QList<QLabel*> LabLists;
	
	//���ڿؼ�
	QList<QWidget*> WidgLists;

	//MOģ����
	QString MoFilename;

	//������
	QDockWidget* dock1;

	//��������ͼ
	QTreeView* paratreeview;

	//��������ͼ
	QTreeView* outputtreeview;

	//������ģ��
	ListTreeModel* paratreemodel;

	//������ģ��
	outputTreeModel* outputtreemodel;

	//�ӿ���Ϣ
	vector<pair<QString, QString>> m_cons;

	//�ӿ���Ϣ��
	QTableWidget* m_cons_table;
	
	//�ɱ༭�ؼ�
	QLineEdit* MdlName;

	//
	//bool m_closed;

};
