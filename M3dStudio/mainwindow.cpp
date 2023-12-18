/****************************************************************************
**
** Wu YZ
** ����˼·��ģ����ĵ�����ʵ���ĵ�
****************************************************************************/
#include "DigitaltwinsWidget.h"
#include "DocumentModel.h"
#include "M3d.h"
#include <QtWidgets>

#include "mainwindow.h"
#include "NewDocDlg.h"
#include "DocumentComponent.h"
//#include "DocumentModel.h"
#include "View.h"
#include "GeomWidget.h"
#include "Translate.h"
#include "CyberInfoDockWidget.h"
#include "ConsDockWidget.h"
#include "ModelLibWidget.h"
#include "AssemblyTreeWidget.h"
#include "DatumPointDlg.h"
#include "DatumLineDlg.h"
#include "ConnectorDlg.h"
#include "materialSetDlg.h"
#include "voxelWidget.h"
//#include "M3dMdl_DS.h"
#include "ParaDockWidget.h"
#include "ComAttriDlg.h"
#include "libLoadThread.h"
#include "LibPathsSetDlg.h"
#include "AnimationWidget.h"
//#include "DigitaltwinsWidget.h"
#include "NeuralnetworkWidget.h"
#include <BRepBuilderAPI_Transform.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include "global.h"
#include "SimulateSetDlg.h"
#include "MarkingSurfDlg.h"

//vector<string> g_sMaterialNames;
//vector<string> g_sMaterialChnNames;
//vector<double> g_fMaterialDensity;
//vector<vector<double> > g_fMaterialColor;
//vector<double> g_fMaterialTransparency;
vector<myMaterial> g_vMaterials;

MainWindow::MainWindow()
{
	myDocument3d = nullptr;
	myGeomWidget = nullptr;
	myParadock = nullptr;
	m_ParaDock = nullptr;
	m_pAssemblyTreeWidget = nullptr;
	nDocType = 0;
	undoAct = nullptr;
	redoAct = nullptr;
	voxelDockWidget = nullptr;
	m_pLibLoadThread = nullptr;

	createActions0();
	createStatusBar();
	updateMenus();

	readSettings();
	//��ʼ��������Ϣ�����ڴ˴���
	InitMaterialInfo();

	setWindowTitle(tr("M3d��ģ�����һ�廯ƽ̨"));
	//setUnifiedTitleAndToolBarOnMac(true);
	showMaximized();
	//����ͼ��
	const QIcon M3dIcon = QIcon::fromTheme("m3d", QIcon("images/m3d.png"));
	setWindowIcon(M3dIcon);
}


MainWindow::~MainWindow()
{
	if (myParadock)
	{
		delete myParadock;
		myParadock = nullptr;
	}
	if (m_pAssemblyTreeWidget) {
		delete m_pAssemblyTreeWidget;
		m_pAssemblyTreeWidget = nullptr;
	}
		
	//�˳�ģ�ͼ����߳�
	if (m_pLibLoadThread)
	{
		m_pLibLoadThread->exit();
		//delete m_pLibLoadThread; //ɾ����������
	}
	//ɾ�����ص�ģ��: �ڿ⻹û�м������ʱ�������쳣������
	for (int i = 0; i < g_vModelicaModels.size(); i++)
	{
		delete g_vModelicaModels[i];
	}
}

void MainWindow::newFile(int type)
{
	//�½������ĵ���һ��
	//int newType = type;
	//�򿪶Ի���ѡ��
	if (type == 0)
	{
		NewDocDlg selDlg(this);
		selDlg.exec();
		if (selDlg.bAccepted)
		{
			// newType = selDlg.iSelItem + 1;
			type = selDlg.iSelItem + 1;
		}

		//�����ǰ���ĵ���������һ��Ӧ��
		if (myDocument3d != nullptr)
		{
			QString cmdLine = QCoreApplication::applicationFilePath();
			cmdLine += " --new ";
			cmdLine += QString::number(type);
			QProcess* pProcess = new QProcess();
			pProcess->start(cmdLine);
			return;
		}
	}

	nDocType = type;

	if (nDocType > 0)
	{   //�½��ĵ�;
		switch (nDocType)
		{
		case 1:
			//���
			CreateComInterface();
			setWindowTitle(tr("M3d��ģ�����һ�廯ƽ̨����δ�������ģ��"));
			break;
		case 2:
			//ϵͳ
			CreateMdlInterface();
			setWindowTitle(tr("M3d��ģ�����һ�廯ƽ̨����δ����ϵͳģ��"));
			break;
		default:
			QMessageBox::information(this, "��ʾ", "�½��ĵ�����", QMessageBox::Ok);
			break;
		}
	}
}

void MainWindow::newDoc()
{
	newFile(0);
}

void MainWindow::InitMaterialInfo()
{
	//���ϵ����ࣺ
	/*
	  Graphic3d_NameOfMaterial_Brass,           //!< Brass        (Physic)
	  Graphic3d_NameOfMaterial_Bronze,          //!< Bronze       (Physic)
	  Graphic3d_NameOfMaterial_Copper,          //!< Copper       (Physic)
	  Graphic3d_NameOfMaterial_Gold,            //!< Gold         (Physic)
	  Graphic3d_NameOfMaterial_Pewter,          //!< Pewter       (Physic)
	  Graphic3d_NameOfMaterial_Plastered,       //!< Plastered    (Generic)
	  Graphic3d_NameOfMaterial_Plastified,      //!< Plastified   (Generic)
	  Graphic3d_NameOfMaterial_Silver,          //!< Silver       (Physic)
	  Graphic3d_NameOfMaterial_Steel,           //!< Steel        (Physic)
	  Graphic3d_NameOfMaterial_Stone,           //!< Stone        (Physic)
	  Graphic3d_NameOfMaterial_ShinyPlastified, //!< Shiny Plastified (Generic)
	  Graphic3d_NameOfMaterial_Satin,           //!< Satin        (Generic)
	  Graphic3d_NameOfMaterial_Metalized,       //!< Metalized    (Generic)
	  Graphic3d_NameOfMaterial_Ionized,         //!< Ionized      (Generic)
	  Graphic3d_NameOfMaterial_Chrome,          //!< Chrome       (Physic)
	  Graphic3d_NameOfMaterial_Aluminum,        //!< Aluminum     (Physic)
	  Graphic3d_NameOfMaterial_Obsidian,        //!< Obsidian     (Physic)
	  Graphic3d_NameOfMaterial_Neon,            //!< Neon         (Physic)
	  Graphic3d_NameOfMaterial_Jade,            //!< Jade         (Physic)
	  Graphic3d_NameOfMaterial_Charcoal,        //!< Charcoal     (Physic)
	  Graphic3d_NameOfMaterial_Water,           //!< Water        (Physic)
	  Graphic3d_NameOfMaterial_Glass,           //!< Glass        (Physic)
	  Graphic3d_NameOfMaterial_Diamond,         //!< Diamond      (Physic)
	  Graphic3d_NameOfMaterial_Transparent,     //!< Transparent  (Physic)
	  Graphic3d_NameOfMaterial_DEFAULT,         //!< Default      (Generic);
	  Graphic3d_NameOfMaterial_UserDefined,     //!< User-defined (Physic);
	*/
	//���ϵ�����
#define NUMOFMAT 26
	string sMaterialNames[NUMOFMAT] = { "Brass", "Bronze", "Copper","Gold", \
		"Pewter","Plastered","Plastified","Silver","Steel","Stone",\
		"ShinyPlastified","Satin","Metalized","Ionized","Chrome","Aluminum",\
		"Obsidian","Neon","Jade","Charcoal","Water","Glass","Diamond","Transparent",\
		"Default","UserDefined" };
	//for(int i=0; i< NUMOFMAT;i++)
	//	g_sMaterialNames.push_back(sMaterialNames[i]);

	string sMaterialChnNames[NUMOFMAT] = { "��ͭ","��ͭ","ͭ", "��", \
		"��", "����", "����","��","��","ʯͷ",\
		"��������","���","����","������","��","��",\
		"����ʯ","����","���","ľ̿","ˮ","����","��ʯ","͸����",\
		"ȱʡ����","�Զ������" };
	//for (int i = 0; i < NUMOFMAT; i++)
	//	g_sMaterialChnNames.push_back(sMaterialChnNames[i]);

	//���ϵ��ܶȣ���λ Kg/mm3����ͼ�ĵ�λΪ m,���ù��ʵ�λ�ƣ�����
	double fMaterialDensity[NUMOFMAT] = { 8.6E+3, 8.9E+3, 8.96E+3, 19.32E+3, \
		7.28E+3, 2.5E+3, 0.9E+3, 10.49E+3, 7.85E+3, 2.8E+3, \
		0.9E+3, 0.1E+3, 7.6E-3, 7.8E+3, 7.19E+3, 2.7E+3, \
		3.5E+3, 0.01E+3, 3.4E+3,0.5E+3, 1.0E+3, 2.5E+3, 3.52E+3, 2.5E+3, \
		7.6E+3, 1.0E+3 };
	//double fMaterialDensity[NUMOFMAT] = { 8.6E-6, 8.9E-6, 8.96E-6, 19.32E-6, \
		7.28E-6, 2.5E-6, 0.9E-6, 10.49E-6, 7.85E-6, 2.8E-6, \
		0.9E-6, 0.1E-6, 7.6E-3, 7.8E-6, 7.19E-6, 2.7E-6, \
		3.5E-6, 0.01E-6, 3.4E-6,0.5E-6, 1.0E-6, 2.5E-6, 3.52E-6, 2.5E-6, \
		7.6E-6, 1.0E-6 };
	//for (int i = 0; i < NUMOFMAT; i++)
	//	g_fMaterialDensity.push_back(fMaterialDensity[i]);

	//���ϵ���ɫ
	double fMaterialColor[NUMOFMAT][3] = { /*��ɫ*/{1.0,1.0,0.0},/*��ɫ*/{0.0,1.0,1.0},/*ͭ*/{186 / 255.0,110 / 255.0,64 / 255.0},/*��ɫ*/{218 / 255.0,178 / 255.0,115 / 255.0},\
		/*��*/{220 / 255., 223 / 255., 227 / 255.}, { 1.0, 0.5, 0. }, { 0.5, 0.5,0.5 }, /*����*/{ 233 / 255.,233 / 255.,216 / 255. }, /*����*/{ 118 / 255.,119 / 255.,120 / 255.}, {0.3,0.3,0.3},\
		{0.9, 0.9, 0.9}, { 0.1,0.2,0.2 }, { 0.6,0.6,0.6 }, { 0.4,0.4,0.4 }, { 220 / 255.,230 / 255.,240 / 255.}, { 220 / 255.,223 / 255.,227 / 255.},\
		{1,1,1}, {1,0.1,0}, {0,1,0}, {0.9,0.9,0.95}, {0.01,0.01,0.01}, {0.1,0.1,0.1}, {0.2,0.2,0.1}, {0.1,0.1,0.1},\
		{0.5,0.5,0.5}, {0.3,0.4,0.5} };
	//for (int i = 0; i < NUMOFMAT; i++)
	//{
	//	vector<double> f3_i;
	//	for (int j = 0; j < 3; j++)
	//		f3_i.push_back(fMaterialColor[i][j]);
	//	g_fMaterialColor.push_back(f3_i);
	//}

	//���ϵ�͸����
	double fMaterialTransparency[NUMOFMAT] = { 0., 0, 0, 0, \
		0, 0, 0, 0, 0, 0, \
		50, 0, 0, 0, 0, 0, \
		0, 95, 0, 0, 100, 90, 40, 100, \
		0, 0 };
	//for (int i = 0; i < NUMOFMAT; i++)
	//	g_fMaterialTransparency.push_back(fMaterialTransparency[i]);

	for (int i = 0; i < NUMOFMAT; i++)
	{
		myMaterial mat_i;
		mat_i.sMaterialName = sMaterialNames[i];
		mat_i.sMaterialChnName = sMaterialChnNames[i];
		mat_i.fMaterialDensity = fMaterialDensity[i];
		for (int j = 0; j < 3; j++)
		{
			mat_i.v3MaterialColor.push_back(fMaterialColor[i][j]);
		}
		mat_i.fMaterialTransparency = fMaterialTransparency[i];
		g_vMaterials.push_back(mat_i);
	}
}

//�����ǰ�б༭���ĵ������½����̺�newһ�������ģ�ĵ�
void MainWindow::CreateComInterface()
{
	//QMessageBox::information(this, "��ʾ", "�����ģ", QMessageBox::Ok);

	//�����˵�
	//1.��ɾȥȫ���˵�
	menuBar()->clear();

	//ɾ���ļ��˵��͹�����
	//fileMenu->clear();
	removeToolBar(fileToolBar);

	//2.����ļ��˵��͹����������䶯��
	fileMenu = menuBar()->addMenu(tr("�ļ�")); //&File
	fileToolBar = addToolBar(tr("File"));

	//2.1 �½�
	const QIcon newIcon = QIcon::fromTheme("document-new", QIcon("images/new.png"));
	QAction* newAct = new QAction(newIcon, tr("�½�..."), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("�½��ĵ�"));
	connect(newAct, &QAction::triggered, this, &MainWindow::newDoc);
	fileMenu->addAction(newAct);
	//2.2 ��
	const QIcon openIcon = QIcon::fromTheme("document-open", QIcon("images/open.png"));
	QAction* openAct = new QAction(openIcon, tr("��..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("���ĵ�"));
	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	fileMenu->addAction(openAct);
	//2.3 ����
	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon("images/save.png"));
	QAction* saveAct = new QAction(saveIcon, tr("����"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("�����ĵ�"));
	connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	fileMenu->addAction(saveAct);
	//2.4 ���
	const QIcon saveasIcon = QIcon::fromTheme("document-save-as", QIcon("images/saveas.png"));
	QAction* saveasAct = new QAction(saveasIcon, tr("���..."), this);
	saveasAct->setShortcuts(QKeySequence::SaveAs);
	saveasAct->setStatusTip(tr("�ĵ����Ϊ"));
	connect(saveasAct, &QAction::triggered, this, &MainWindow::saveAs);
	fileMenu->addAction(saveasAct);
	//2.5 import����
	const QIcon importIcon = QIcon::fromTheme("document-import", QIcon("images/import.png"));
	QAction* importAct = new QAction(importIcon, tr("����..."), this);
	//importAct->setShortcuts(QKeySequence::import);
	importAct->setStatusTip(tr("���뼸��"));
	connect(importAct, &QAction::triggered, this, &MainWindow::import);
	fileMenu->addAction(importAct);
	//2.6 export����������Ҫ��ʱ��ʵ��

	//�ָ���
	//2.7 ����ĵ�
	fileMenu->addSeparator();
	QMenu* recentMenu = fileMenu->addMenu(tr("����ĵ�"));
	connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
	recentFileSubMenuAct = recentMenu->menuAction();
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileActs[i]->setVisible(false);
	}
	recentFileSeparator = fileMenu->addSeparator();
	setRecentFilesVisible(MainWindow::hasRecentFiles());
	fileMenu->addSeparator();
	//�ָ���
	//2.8 �˳�
	const QIcon exitIcon = QIcon::fromTheme("application-exit");
	QAction* exitAct = fileMenu->addAction(exitIcon, tr("�˳�"), qApp, & QApplication::closeAllWindows);
	//QAction * exitAct = new QAction(exitIcon, tr("�˳�"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("�˳�"));
	fileMenu->addAction(exitAct);
	//connect(exitAct, &QAction::triggered, this, &MainWindow::Close);
	//connect(this, SIGNAL(closeEvent(QCloseEvent*)), this, SLOT(Close()));

	//3. ���� �༭ �˵�
	QMenu* editMenu = menuBar()->addMenu(tr("�༭"));
	//3.1 undo
	const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon("images/undo.png"));
	undoAct = new QAction(undoIcon, tr("����"), this);
	undoAct->setShortcuts(QKeySequence::Undo);
	undoAct->setStatusTip(tr("������һ�β���"));
	connect(undoAct, &QAction::triggered, this, &MainWindow::undo);
	editMenu->addAction(undoAct);
	//ʹ��undo
	undoAct->setEnabled(false);
	//connect(editMenu, &QMenu::aboutToShow, this, &MainWindow::undoEnable);
	//3.2 redo
	const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon("images/redo.png"));
	redoAct = new QAction(redoIcon, tr("����"), this);
	redoAct->setShortcuts(QKeySequence::Redo);
	redoAct->setStatusTip(tr("�ָ���һ�β���"));
	connect(redoAct, &QAction::triggered, this, &MainWindow::redo);
	editMenu->addAction(redoAct);
	redoAct->setEnabled(false);
	//3.3 ɾ��: ��׼������ʵ��
	editMenu->addSeparator();
	const QIcon delIcon = QIcon::fromTheme("edit-delete");
	delAct = new QAction(delIcon, tr("ɾ��"), this);
	delAct->setStatusTip(tr("ɾ��ѡ�е�ʵ��"));
	//delAct->setCheckable(false);
	connect(delAct, &QAction::triggered, this, &MainWindow::DeleteSelected);
	editMenu->addAction(delAct);

	//����һ�������������
	const QIcon scaleIcon = QIcon::fromTheme("edit-scale");
	QAction * scaleAct = new QAction(scaleIcon, tr("����"), this);
	scaleAct->setStatusTip(tr("����ʵ������������"));
	connect(scaleAct, &QAction::triggered, this, &MainWindow::Scaling);
	editMenu->addAction(scaleAct);

	//����һ���ӿڷ�������
	const QIcon connScaleIcon = QIcon::fromTheme("edit-connscale");
	QAction* connScaleAct = new QAction(connScaleIcon, tr("�ӿڷ���"), this);
	connScaleAct->setStatusTip(tr("�Խӿڽ��з���"));
	connect(connScaleAct, &QAction::triggered, this, &MainWindow::connScaling);
	editMenu->addAction(connScaleAct);

	//����һ����״������ͼ����
	const QIcon textureIcon = QIcon::fromTheme("edit-texture");
	QAction* textureAct = new QAction(textureIcon, tr("������ͼ"), this);
	textureAct->setStatusTip(tr("����״����������ͼ"));
	connect(textureAct, &QAction::triggered, this, &MainWindow::Texture);
	editMenu->addAction(textureAct);

	//3.4 ѡ�񣺸���ʵ������
	editMenu->addSeparator();
	QMenu* selectMenu = editMenu->addMenu(tr("ѡ��ʵ������"));
	//QAction * selectSubMenuAct = selectMenu->menuAction();
	//����
	const QIcon selVtxIcon = QIcon::fromTheme("edit-select-vertex"/*, QIcon("images/redo.png")*/);
	selVtxAct = new QAction(selVtxIcon, tr("����"), this);
	//selVtxAct->setShortcuts(QKeySequence::Redo);
	selVtxAct->setStatusTip(tr("ѡ�񶥵�����"));
	selVtxAct->setCheckable(true);
	connect(selVtxAct, &QAction::triggered, this, &MainWindow::selectVertex);
	selectMenu->addAction(selVtxAct);
	//��
	const QIcon selEdgeIcon = QIcon::fromTheme("edit-select-edge"/*, QIcon("images/redo.png")*/);
	selEdgeAct = new QAction(selEdgeIcon, tr("���"), this);
	selEdgeAct->setStatusTip(tr("ѡ���������"));
	selEdgeAct->setCheckable(true);
	connect(selEdgeAct, &QAction::triggered, this, &MainWindow::selectEdge);
	selectMenu->addAction(selEdgeAct);
	//��
	const QIcon selFaceIcon = QIcon::fromTheme("edit-select-face"/*, QIcon("images/redo.png")*/);
	selFaceAct = new QAction(selFaceIcon, tr("��"), this);
	selFaceAct->setStatusTip(tr("ѡ��������"));
	selFaceAct->setCheckable(true);
	connect(selFaceAct, &QAction::triggered, this, &MainWindow::selectFace);
	selectMenu->addAction(selFaceAct);
	//��
	const QIcon selBodyIcon = QIcon::fromTheme("edit-select-body"/*, QIcon("images/redo.png")*/);
	selBodyAct = new QAction(selBodyIcon, tr("��"), this);
	selBodyAct->setStatusTip(tr("ѡ��������"));
	selBodyAct->setCheckable(true);
	connect(selBodyAct, &QAction::triggered, this, &MainWindow::selectBody);
	selectMenu->addAction(selBodyAct);
	//�ӿ�
	const QIcon selConnectorIcon = QIcon::fromTheme("edit-select-connector"/*, QIcon("images/redo.png")*/);
	selConnectorAct = new QAction(selConnectorIcon, tr("�ӿ�"), this);
	selConnectorAct->setStatusTip(tr("ѡ��ӿ�����"));
	connect(selConnectorAct, &QAction::triggered, this, &MainWindow::selectConnector);
	selConnectorAct->setCheckable(true);
	selectMenu->addAction(selConnectorAct);
	//��һ
	const QIcon selAnyIcon = QIcon::fromTheme("edit-select-any"/*, QIcon("images/redo.png")*/);
	selAnyAct = new QAction(selAnyIcon, tr("��һ"), this);
	selAnyAct->setStatusTip(tr("ѡ����һ����"));
	selAnyAct->setCheckable(true);
	selAnyAct->setChecked(true);
	connect(selAnyAct, &QAction::triggered, this, &MainWindow::selectAny);
	selectMenu->addAction(selAnyAct);
	
	//4. ���� Լ�� �˵�
	QMenu* constriantMenu = menuBar()->addMenu(tr("Լ��"));
	//��ʾ������Լ������ ͣ������
	const QIcon showConsDockwinIcon = QIcon::fromTheme("constraint-show");
	showConsDockwinAct = new QAction(showConsDockwinIcon, tr("Լ��������"), this);
	showConsDockwinAct->setCheckable(true);
	showConsDockwinAct->setStatusTip(tr("��ʾ/�ر�Լ��������"));
	connect(showConsDockwinAct, &QAction::triggered, this, &MainWindow::showConsDockwin);
	constriantMenu->addAction(showConsDockwinAct);
	//---------------
	constriantMenu->addSeparator();
	//�ع�
	const QIcon regenIcon = QIcon::fromTheme("constraint-regen");
	showVoxelDockwinAct = new QAction(regenIcon, tr("�ع�����"), this);
	showVoxelDockwinAct->setCheckable(true);
	showVoxelDockwinAct->setStatusTip(tr("��Լ���ع�����"));
	connect(showVoxelDockwinAct, &QAction::triggered, this, &MainWindow::regenGeometry);
	constriantMenu->addAction(showVoxelDockwinAct);

	//5. ���� ��Ϣ �˵�
	QMenu* cyberMenu = menuBar()->addMenu(tr("��Ϣ"));
	//��ʾ��Ϣ������
	const QIcon showCyberDockwinIcon = QIcon::fromTheme("cyber-show");
	showCyberDockwinAct = new QAction(showCyberDockwinIcon, tr("��Ϣ������"), this);
	showCyberDockwinAct->setCheckable(true);
	showCyberDockwinAct->setStatusTip(tr("��ʾ/�ر���Ϣ������"));
	connect(showCyberDockwinAct, &QAction::triggered, this, &MainWindow::showCyberDockwin);
	cyberMenu->addAction(showCyberDockwinAct);
	//���ӽӿ�
	cyberMenu->addSeparator();
	const QIcon addConnIcon = QIcon::fromTheme("cyber-conn");
	QAction* addConnAct = new QAction(addConnIcon, tr("��ӽӿ�"), this);
	addConnAct->setStatusTip(tr("��ӽӿ�"));
	connect(addConnAct, &QAction::triggered, this, &MainWindow::defConnDlg);
	cyberMenu->addAction(addConnAct);
	//��ӱ����
	cyberMenu->addSeparator();
	const QIcon addMarkingIcon = QIcon::fromTheme("cyber-marking");
	QAction* addMarkingAct = new QAction(addMarkingIcon, tr("��ӱ����"), this);
	addMarkingAct->setStatusTip(tr("��ӱ����"));
	connect(addMarkingAct, &QAction::triggered, this, &MainWindow::addMarkingSurfDlg);
	cyberMenu->addAction(addMarkingAct);
	//QMenu* connectorMenu = cyberMenu->addMenu(tr("��ӽӿ�"));
	////��е
	//const QIcon addMachCon = QIcon::fromTheme("cyber-conn-mach"/*, QIcon("images/redo.png")*/);
	//QAction* addMachAct = new QAction(addMachCon, tr("��е..."), this);
	//addMachAct->setStatusTip(tr("��ӻ�е�ӿ�"));
	////addMachAct->setCheckable(true);
	//connect(addMachAct, &QAction::triggered, this, &MainWindow::addMachConn);
	//connectorMenu->addAction(addMachAct);
	////���ӵ���
	//const QIcon addElecCon = QIcon::fromTheme("cyber-conn-elec"/*, QIcon("images/redo.png")*/);
	//QAction* addElecAct = new QAction(addMachCon, tr("���ӵ���..."), this);
	//addElecAct->setStatusTip(tr("��ӵ��ӵ����ӿ�"));
	////addMachAct->setCheckable(true);
	//connect(addElecAct, &QAction::triggered, this, &MainWindow::addElecConn);
	//connectorMenu->addAction(addElecAct);
	////Һѹ
	//const QIcon addhydrCon = QIcon::fromTheme("cyber-conn-hydr"/*, QIcon("images/redo.png")*/);
	//QAction* addhydrAct = new QAction(addhydrCon, tr("Һѹ..."), this);
	//addhydrAct->setStatusTip(tr("���Һѹ�ӿ�"));
	//connect(addhydrAct, &QAction::triggered, this, &MainWindow::addHydrConn);
	//connectorMenu->addAction(addhydrAct);
	////����
	//const QIcon addcontCon = QIcon::fromTheme("cyber-conn-contr"/*, QIcon("images/redo.png")*/);
	//QAction* addcontAct = new QAction(addcontCon, tr("����..."), this);
	//addcontAct->setStatusTip(tr("��ӿ��ƽӿ�"));
	//connect(addcontAct, &QAction::triggered, this, &MainWindow::addContConn);
	//connectorMenu->addAction(addcontAct);
	//��׼����  lqq
	cyberMenu->addSeparator();
	QMenu* datumMenu = cyberMenu->addMenu(tr("��׼����"));
	//��׼��
	const QIcon datumPointIcon = QIcon::fromTheme("cyber-datum-point");
	QAction* datumPointAct = new QAction(datumPointIcon, tr("��׼��"), this);
	datumPointAct->setStatusTip(tr("�����׼��"));
	connect(datumPointAct, &QAction::triggered, this, &MainWindow::defDatumPoint);
	datumMenu->addAction(datumPointAct);
	//��׼��
	const QIcon datumLineIcon = QIcon::fromTheme("cyber-refer-Line");
	QAction* datumLineAct = new QAction(datumLineIcon, tr("��׼��"), this);
	datumLineAct->setStatusTip(tr("�����׼��"));
	connect(datumLineAct, &QAction::triggered, this, &MainWindow::defDatumLine);
	datumMenu->addAction(datumLineAct);

	//6. ����ϵͳ���ò˵�;
	QMenu* setMenu = menuBar()->addMenu(tr("����"));
	//ϵͳ����
	const QIcon sysSetIcon = QIcon::fromTheme("set-system");
	QAction* sysSetAct = new QAction(sysSetIcon, tr("ϵͳ����"), this);
	sysSetAct->setStatusTip(tr("ϵͳ����"));
	connect(sysSetAct, &QAction::triggered, this, &MainWindow::systemSet);
	setMenu->addAction(sysSetAct);
	//������ɫ����
	const QIcon backColorIcon = QIcon::fromTheme("set-systemColor");
	QAction* backColorAct = new QAction(backColorIcon, tr("������ɫ����"), this);
	backColorAct->setStatusTip(tr("������ɫ����"));
	connect(backColorAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView(); \
		view->onBackground(); \
		}); 
	setMenu->addAction(backColorAct);

	//������ͼ
	const QIcon backTextureIcon = QIcon::fromTheme("set-systemBackFig");
	QAction* backTextureAct = new QAction(backTextureIcon, tr("����ͼƬ����"), this);
	backTextureAct->setStatusTip(tr("����ͼƬ����"));
	connect(backTextureAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView(); \
		view->onEnvironmentMap(); \
		});
	setMenu->addAction(backTextureAct);
	setMenu->addSeparator();
	//��ʾȫ������ϵ
	const QIcon showOxyzIcon = QIcon::fromTheme("set-ShowOxyz");
	showOxyzAct = new QAction(showOxyzIcon, tr("��ʾȫ������ϵ"), this);
	showOxyzAct->setStatusTip(tr("��ʾȫ������ϵ"));
	showOxyzAct->setCheckable(true);
	showOxyzAct->setChecked(false);
	connect(showOxyzAct, &QAction::triggered, this, &MainWindow::ShowOxyz);
	setMenu->addAction(showOxyzAct);


	//7. ���Ӱ����˵�
	QMenu* helpMenu = menuBar()->addMenu(tr("����"));
	QAction* aboutAct = helpMenu->addAction(tr("���ڣ�&A��"), this, &MainWindow::about);
	aboutAct->setStatusTip(tr("��ʾ���ڶԻ���"));
	helpMenu->addSeparator();
	helpMenu->addAction(tr("������"), this, &MainWindow::MakeBox);
	helpMenu->addAction(tr("����"), this, &MainWindow::MakeSpring);
	helpMenu->addAction(tr("���Brep"), this, &MainWindow::ExportBREP);

	//8.����������
	//8.1�ļ�������: �½����򿪡����桢���뼴�ɣ����಻Ҫ
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);
	//fileToolBar->addAction(saveasAct);
	fileToolBar->addAction(importAct);
	///����ʵ���������������ѡ��ʵ���߿�/����ʾ��Color��Material��Tranparent����ɾ��ȡ��ɾ����
	QToolBar* objEditToolBar = addToolBar(tr("objEdit"));
	//--1.��ɫ
	const QIcon colorIcon = QIcon::fromTheme("objEdit-color", QIcon("images/tool_color.png"));
	colorAct = new QAction(colorIcon, tr("��ɫ"), this);
	//colorAct->setShortcuts(QKeySequence::Undo);
	colorAct->setStatusTip(tr("���ö�����ɫ"));
	connect(colorAct, &QAction::triggered, this, &MainWindow::SetColor);
	//colorAct->setEnabled(false);
	objEditToolBar->addAction(colorAct);
	//--2.����
	const QIcon matIcon = QIcon::fromTheme("objEdit-material", QIcon("images/tool_material.png"));
	QAction* matAct = new QAction(matIcon, tr("����"), this);
	matAct->setStatusTip(tr("���ö������"));
	connect(matAct, &QAction::triggered, this, &MainWindow::SetMaterial);
	objEditToolBar->addAction(matAct);

	//8.2 �༭������������undo������redo,ѡ��select
	QToolBar* editToolBar = addToolBar(tr("edit"));
	editToolBar->addAction(undoAct);
	editToolBar->addAction(redoAct);

	////ɾ��������
	//subDelmenu = new QMenu("ɾ��",this);
	//subDelmenu->addAction(delConnAct);
	////subPartDelmenu->addAction(delEdgeAct);
	////subDelmenu->addMenu(subPartDelmenu);
	//subDelmenu->addAction(delCompAct);
	//editToolBar->addAction(subDelmenu->menuAction());

	subSelmenu = new QMenu("ѡ��", this);
	//submenu->setIcon(genIcon(iconSize(), 1, Qt::black, dpr));
	subSelmenu->addAction(selAnyAct);// (genIcon(iconSize(), "A", Qt::blue, dpr), "A");
	subSelmenu->addAction(selVtxAct);
	subSelmenu->addAction(selEdgeAct);
	subSelmenu->addAction(selFaceAct);
	subSelmenu->addAction(selBodyAct);
	subSelmenu->addAction(selConnectorAct);
	editToolBar->addAction(subSelmenu->menuAction());
	//8.3 Լ��������
	QToolBar* consToolBar = addToolBar(tr("constraint"));
	consToolBar->addAction(showConsDockwinAct);
	consToolBar->addAction(showVoxelDockwinAct);
	//8.4 ��Ϣ������
	QToolBar* cyberToolBar = addToolBar(tr("cyber"));
	cyberToolBar->addAction(showCyberDockwinAct);
	cyberToolBar->addAction(addConnAct);
	cyberToolBar->addAction(addMarkingAct);
	//QMenu* submenuConn = new QMenu("��ӽӿ�", this);
	//submenuConn->addAction(addMachAct);// (genIcon(iconSize(), "A", Qt::blue, dpr), "A");
	//submenuConn->addAction(addElecAct);
	//submenuConn->addAction(addhydrAct);
	//submenuConn->addAction(addcontAct);
	//cyberToolBar->addAction(submenuConn->menuAction());
	//��׼���幤���� lqq
	QMenu* submenuDatum = new QMenu("��׼����", this);
	submenuDatum->addAction(datumPointAct);
	submenuDatum->addAction(datumLineAct);
	cyberToolBar->addAction(submenuDatum->menuAction());

	//9.��������
	QFrame* aViewFrame = new QFrame;
	aViewFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	aViewFrame->setLineWidth(3);
	QVBoxLayout* aViewLayout = new QVBoxLayout(aViewFrame);
	aViewLayout->setContentsMargins(0, 0, 0, 0);
	myDocument3d = new DocumentComponent(this);
	//myDocument3d->setDocType(nDocType);
	myGeomWidget = new GeomWidget((DocumentComponent*)myDocument3d, aViewFrame);
	aViewLayout->addWidget(myGeomWidget);
	myGeomWidget->setContentsMargins(0, 0, 0, 0);
	setCentralWidget(myGeomWidget);

	selectAny();
	//�ĵ������Ժ���ܽ�������
	//connect(colorAct, &QAction::triggered, this->myDocument3d, &DocumentCommon::objColor);

	//����modelica��ļ���
	vector<string> vLibPaths;
	vLibPaths = LibPathsSetDlg::GetModelLibPaths();//�������л�ȡ
	//����
	vLibPaths.push_back("C:/Program Files/MWorks.Sysplorer 2020/Library/Modelica 3.2.1/Modelica/package.mo");
	m_pLibLoadThread = new LibLoadThread(this);
	m_pLibLoadThread->SetLibPaths(vLibPaths);
	m_pLibLoadThread->start();
}


void MainWindow::showCyberDockwin()
{
	////myParadock = nullptr;
	//QAction* act = qobject_cast<QAction*>(sender());
	//if (act->isChecked()) {
	//	if (myParadock&&!myParadock->isclosed()) {
	//		CyberInfoDockWidget* t = myParadock;
	//		delete t;
	//		myParadock = nullptr;
	//	}
	//	myParadock = new CyberInfoDockWidget(this,act);
	//	addDockWidget(Qt::RightDockWidgetArea, myParadock->getdock());
	//}
	//else if (myParadock) {
	//	CyberInfoDockWidget* t = myParadock;
	//	delete t;
	//	myParadock =nullptr;
	//}
	if (showCyberDockwinAct->isChecked())
	{
		if (myParadock)
		{
			myParadock->getdock()->show();
		}
		else {
			myParadock = new CyberInfoDockWidget(this, showCyberDockwinAct);
			addDockWidget(Qt::RightDockWidgetArea, myParadock->getdock());
		}
	}
	else
	{
		//voxelDockWidget->close();
		myParadock->getdock()->hide();
	}
}

void MainWindow::systemSet()
{
	LibPathsSetDlg* sysDlg = new LibPathsSetDlg(this);
	//int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // ��ȡ�������߶�
	//int diaWidth = sysDlg->width();
	//int diaHeight = sysDlg->height();
	//QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(-800, 0));//���öԻ���λ��
	//sysDlg->setGeometry(globalPos.x() + this->width() - diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
	sysDlg->show();
}

void MainWindow::addMachConn()
{

}

void MainWindow::addElecConn()
{

}

void MainWindow::addHydrConn()
{

}

void MainWindow::addContConn()
{

}

void MainWindow::defConnDlg()
{
	ConnectorDlg* dconnDlg = new ConnectorDlg(this);
	int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // ��ȡ�������߶�
	int diaWidth = dconnDlg->width();
	int diaHeight = dconnDlg->height();
	QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//���öԻ���λ��
	dconnDlg->setGeometry(globalPos.x() + this->width() - diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
	dconnDlg->show();
}

void MainWindow::addMarkingSurfDlg()
{
	MarkingSurfDlg* markDlg = new MarkingSurfDlg(this);
	int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // ��ȡ�������߶�
	int diaWidth = markDlg->width();
	int diaHeight = markDlg->height();
	QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//���öԻ���λ��
	markDlg->setGeometry(globalPos.x() + this->width() - 2.5 * diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
	markDlg->show();
}

//lqq
void MainWindow::defDatumPoint()
{
	DatumPointDlg* dptDlg = new DatumPointDlg(this);
	int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // ��ȡ�������߶�
	int diaWidth = dptDlg->width();
	int diaHeight = dptDlg->height();
	QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//���öԻ���λ��
	dptDlg->setGeometry(globalPos.x() + this->width() - diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
	dptDlg->show();
}

void MainWindow::defDatumLine()
{
	DatumLineDlg* dliDlg = new DatumLineDlg(this);
	int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // ��ȡ�������߶�
	int diaWidth = dliDlg->width();
	int diaHeight = dliDlg->height();
	QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//���öԻ���λ��
	dliDlg->setGeometry(globalPos.x() + this->width() - diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
	dliDlg->show();
}

void MainWindow::showConsDockwin()
{
	if (showConsDockwinAct->isChecked())
	{
		consdockwidget = new ConsDockWidget(this);
		addDockWidget(Qt::RightDockWidgetArea, consdockwidget);
	}
	else
	{
		consdockwidget->close();
	}
}

//���ؽ�ģ�뼸���ع�
void MainWindow::regenGeometry()
{
	if (showVoxelDockwinAct->isChecked())
	{
		if (voxelDockWidget)
		{
			voxelDockWidget->show();
		}
		else {
			voxelDockWidget = new voxelWidget(this, myDocument3d);
			addDockWidget(Qt::RightDockWidgetArea, voxelDockWidget);
		}
	}
	else
	{
		//voxelDockWidget->close();
		voxelDockWidget->hide();
	}
}

void MainWindow::selectAny()
{
	//�������checkȥ��
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(true);

	//�ı�toolbar����ʾ
	subSelmenu->setTitle("��һ");

	//����OCC��ѡ��
	myDocument3d->setSelectMode(tr("��һ"));
}

void MainWindow::selectVertex()
{
	//�������checkȥ��
	selVtxAct->setChecked(true);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(false);

	//�ı�toolbar����ʾ
	subSelmenu->setTitle("����");

	//����OCC��ѡ��
	myDocument3d->setSelectMode(tr("����"));
}

void MainWindow::selectEdge()
{
	//�������checkȥ��
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(true);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(false);

	//�ı�toolbar����ʾ
	subSelmenu->setTitle("���");

	//����OCC��ѡ��
	myDocument3d->setSelectMode(tr("���"));
}

void MainWindow::selectFace()
{
	//�������checkȥ��
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(true);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(false);

	//�ı�toolbar����ʾ
	subSelmenu->setTitle("��");

	//����OCC��ѡ��
	myDocument3d->setSelectMode(tr("��"));
}

void MainWindow::selectBody()
{
	//�������checkȥ��
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(true);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(false);

	//�ı�toolbar����ʾ
	subSelmenu->setTitle("��");

	//����OCC��ѡ��
	myDocument3d->setSelectMode(tr("��"));
}

void MainWindow::selectConnector()
{
	//�������checkȥ��
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(true);
	selAnyAct->setChecked(false);

	//�ı�toolbar����ʾ
	subSelmenu->setTitle("�ӿ�");

	//����OCC��ѡ��
	myDocument3d->setSelectMode(tr("�ӿ�"));
}

void MainWindow::undo()
{
	myDocument3d->undo();
	updateMenus();
}

void MainWindow::undoEnable(bool b)
{
	undoAct->setEnabled(true);
}

void MainWindow::redo()
{
	myDocument3d->redo();
	updateMenus();
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
	int keyValue = event->nativeVirtualKey();
	if (keyValue == 46)
	{
		DeleteSelected();
	}
}

//������ͼ
void MainWindow::Texture()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		DocumentComponent* pComDoc = (DocumentComponent*)myDocument3d;
		pComDoc->Texture();
	}
}

//�������
void MainWindow::Scaling()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		//�������������
		DocumentComponent* pComDoc = (DocumentComponent*)myDocument3d;
		Graphic3d_NameOfMaterial mat;
		TopoDS_Shape mainSolid = pComDoc->GetMainSolid(mat);

		bool ok = false;
		double scale = QInputDialog::getDouble(this, tr("������״����"), tr("�������"), 1, 1e-6, 1e6, 3, &ok);
		if (ok)
		{
			gp_Trsf transf;
			GProp_GProps aGProps;
			BRepGProp::VolumeProperties(mainSolid, aGProps);
			//��������
			gp_Pnt aCOM = aGProps.CentreOfMass();
			transf.SetScale(aCOM, scale);
			BRepBuilderAPI_Transform myBRepTransformation(mainSolid, transf);
			TopoDS_Shape trsfMainShape = myBRepTransformation.Shape();
			pComDoc->SetMainShape(trsfMainShape);
			pComDoc->UpdateDisplay();
			
		}
	}
}

void MainWindow::connScaling()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		//�Խӿ�����
		DocumentComponent* pComDoc = (DocumentComponent*)myDocument3d;
		bool ok = false;
		double scale = QInputDialog::getDouble(this, tr("�ӿ�����"), tr("�������"), 1, 1e-6, 1e6, 3, &ok);
		if (ok)
		{
			vector<Connector*> vConntrs = pComDoc->GetAllConnectors();
			for (int i = 0; i < vConntrs.size(); i++)
			{
				Connector* conn = vConntrs[i];
				TopoDS_Shape shp_i = conn->GetShape();
				//�Ӹ���������
				gp_Ax3 Ax3 = MakeAx3(conn->GetDisplacement());
				gp_Trsf transf;
				transf.SetScale(Ax3.Location(), scale);
				BRepBuilderAPI_Transform myBRepTransformation(shp_i, transf);
				TopoDS_Shape trsfshp_i = myBRepTransformation.Shape();
				conn->SetShape(trsfshp_i);
				pComDoc->UpdateConnector(shp_i, conn);

				pComDoc->UpdateDisplay();
			}
		}
	}
}

void MainWindow::DeleteSelected()
{
	//bool bAct = myGeomWidget->isActiveWindow();

	QWidget* wdt = focusWidget();
	if(wdt != myGeomWidget->GetView())
	{
		return;
	}

	if (myDocument3d && myDocument3d->DeleteSelected())
	{
		//myDocument3d->UpdateDisplay();
		return;
	}

	QMessageBox msgBox;
	msgBox.setText("ֻ��ѡ��ο�ʵ���ӿڽ���ɾ��!");
	msgBox.exec();
}

//�����ǰ�б༭���ĵ������½����̺���newϵͳ��ģ�ĵ�
void MainWindow::CreateMdlInterface()
{
	//����
	//QMessageBox::information(this, "��ʾ", "ϵͳ��ģ", QMessageBox::Ok);

	//�����˵�
	//CreateMdlMenus();
	//1.��ɾȥȫ���˵�
	menuBar()->clear();

	//ɾ��������
	removeToolBar(fileToolBar);

	//2.����ļ��˵����䶯��
	fileMenu = menuBar()->addMenu(tr("�ļ�"));
	//����Ҫ������
	//fileToolBar = addToolBar(tr("File"));

	//2.1�½�
	const QIcon newIcon = QIcon::fromTheme("document-new", QIcon("images/new.png"));
	QAction* newAct = new QAction(newIcon, tr("�½�..."), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("�½��ĵ�"));
	//����
	connect(newAct, &QAction::triggered, this, &MainWindow::newDoc);
	fileMenu->addAction(newAct);

	//2.2��
	const QIcon openIcon = QIcon::fromTheme("document-open", QIcon("images/open.png"));
	QAction* openAct = new QAction(openIcon, tr("��..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("���ĵ�"));
	//����
	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	fileMenu->addAction(openAct);

	//2.3����
	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon("images/save.png"));
	QAction* saveAct = new QAction(saveIcon, tr("����"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("�����ĵ�"));
	//����
	connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	fileMenu->addAction(saveAct);

	//2.4���
	const QIcon saveasIcon = QIcon::fromTheme("document-save-as", QIcon("images/saveas.png"));
	QAction* saveasAct = new QAction(saveasIcon, tr("���..."), this);
	saveasAct->setShortcuts(QKeySequence::SaveAs);
	saveasAct->setStatusTip(tr("�ĵ����Ϊ"));
    //����
	connect(saveasAct, &QAction::triggered, this, &MainWindow::saveAs);
	fileMenu->addAction(saveasAct);

	//�ָ���
	//2.5����ĵ�
	fileMenu->addSeparator();
	QMenu* recentMenu = fileMenu->addMenu(tr("����ĵ�"));
	//����
	recentFileSubMenuAct = recentMenu->menuAction();
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileActs[i]->setVisible(false);
	}
	recentFileSeparator = fileMenu->addSeparator();
	setRecentFilesVisible(MainWindow::hasRecentFiles());
	fileMenu->addSeparator();
	//�ָ���
	//2.6 �˳�
	const QIcon exitIcon = QIcon::fromTheme("application-exit");
	QAction* exitAct = fileMenu->addAction(exitIcon, tr("�˳�"), qApp, &QApplication::closeAllWindows);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("�˳�"));
	fileMenu->addAction(exitAct);

	//3.���� �༭ �˵�
	QMenu* editMenu = menuBar()->addMenu(tr("�༭"));
	//3.1 undo
	const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon("images/undo.png"));
	undoAct = new QAction(undoIcon, tr("����"), this);
	undoAct->setShortcuts(QKeySequence::Undo);
	undoAct->setStatusTip(tr("������һ�β���"));
	//����
	editMenu->addAction(undoAct);
	undoAct->setEnabled(false);

	//3.2 redo
	const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon("images/redo.png"));
	redoAct = new QAction(redoIcon, tr("����"), this);
	redoAct->setShortcuts(QKeySequence::Redo);
	redoAct->setStatusTip(tr("�ָ���һ�β���"));
	connect(redoAct, &QAction::triggered, this, &MainWindow::redo);
	editMenu->addAction(redoAct);
	redoAct->setEnabled(false);

	//3.3 ѡ�񣺸���ʵ������
	editMenu->addSeparator();
	QMenu* selectMenu = editMenu->addMenu(tr("ѡ��ʵ������"));
	//����
	const QIcon selVtxIcon = QIcon::fromTheme("edit-select-vertex");
	selVtxAct = new QAction(selVtxIcon, tr("����"), this);
	selVtxAct->setStatusTip(tr("ѡ�񶥵�����"));
	selVtxAct->setCheckable(true);
	connect(selVtxAct, &QAction::triggered, this, &MainWindow::selectVertex);
	selectMenu->addAction(selVtxAct);
	//��
	const QIcon selEdgeIcon = QIcon::fromTheme("edit-select-edge");
	selEdgeAct = new QAction(selEdgeIcon, tr("���"), this);
	selEdgeAct->setStatusTip(tr("ѡ���������"));
	selEdgeAct->setCheckable(true);
	connect(selEdgeAct, &QAction::triggered, this, &MainWindow::selectEdge);
	selectMenu->addAction(selEdgeAct);
	//��
	const QIcon selFaceIcon = QIcon::fromTheme("edit-select-face");
	selFaceAct = new QAction(selFaceIcon, tr("��"), this);
	selFaceAct->setStatusTip(tr("ѡ��������"));
	selFaceAct->setCheckable(true);
	connect(selFaceAct, &QAction::triggered, this, &MainWindow::selectFace);
	selectMenu->addAction(selFaceAct);
	//��
	const QIcon selBodyIcon = QIcon::fromTheme("edit-select-body");
	selBodyAct = new QAction(selBodyIcon, tr("��"), this);
	selBodyAct->setStatusTip(tr("ѡ��������"));
	selBodyAct->setCheckable(true);
	connect(selBodyAct, &QAction::triggered, this, &MainWindow::selectBody);
	selectMenu->addAction(selBodyAct);
	//�ӿ�
	const QIcon selConnectorIcon = QIcon::fromTheme("edit-select-connector");
	selConnectorAct = new QAction(selConnectorIcon, tr("�ӿ�"), this);
	selConnectorAct->setStatusTip(tr("ѡ��ӿ�����"));
	connect(selConnectorAct, &QAction::triggered, this, &MainWindow::selectConnector);
	selConnectorAct->setCheckable(true);
	selectMenu->addAction(selConnectorAct);
	//��һ
	const QIcon selAnyIcon = QIcon::fromTheme("edit-select-any");
	selAnyAct = new QAction(selAnyIcon, tr("��һ"), this);
	selAnyAct->setStatusTip(tr("ѡ����һ����"));
	selAnyAct->setCheckable(true);
	selAnyAct->setChecked(true);
	connect(selAnyAct, &QAction::triggered, this, &MainWindow::selectAny);
	selectMenu->addAction(selAnyAct);

	//3.4��ת
	editMenu->addSeparator();
	const QIcon rotaIcon = QIcon::fromTheme("rotate", QIcon("images/cursor_rotate"));
	rotaAct = new QAction(rotaIcon, tr("��ת"), this);
	rotaAct->setStatusTip(tr("��ת"));
	//����
	editMenu->addAction(rotaAct);

	//3.5ƽ��
	const QIcon panIcon = QIcon::fromTheme("pan"/*,QIcon("images/")*/);
	panAct = new QAction(panIcon, tr("ƽ��"), this);
	panAct->setStatusTip(tr("ƽ��"));
	//����
	editMenu->addAction(panAct);

	//3.6�Ŵ�
	const QIcon zoominIcon = QIcon::fromTheme("zoom-in", QIcon("images/cursor_zoom"));
	zoominAct = new QAction(zoominIcon, tr("�Ŵ�"), this);
	zoominAct->setStatusTip(tr("�Ŵ�"));
	//����
	editMenu->addAction(zoominAct);

	//3.7��С
	const QIcon zoomoutIcon = QIcon::fromTheme("zoom-out", QIcon("images/cursor_zoom"));
	zoomoutAct = new QAction(zoomoutIcon, tr("��С"), this);
	zoomoutAct->setStatusTip(tr("��С"));
	//����
	editMenu->addAction(zoomoutAct);

	//3.8���
	editMenu->addSeparator();
	const QIcon addIcon = QIcon::fromTheme("add"/*,QIcon("images")*/);
	addAct = new QAction(addIcon, tr("���"), this);
	addAct->setStatusTip(tr("���"));
	//����
	editMenu->addAction(addAct);

	//3.9ɾ��
	const QIcon delIcon = QIcon::fromTheme("delete", QIcon("images/tool_delete"));
	delAct = new QAction(delIcon, tr("ɾ��"), this);
	delAct->setStatusTip(tr("ɾ��"));
	//����
	editMenu->addAction(delAct);

	//4.���� ���� �˵�
	QMenu* connectMenu = menuBar()->addMenu(tr("����"));
	//��������
	const QIcon estabconnectIcon = QIcon::fromTheme("connection-establish");
	QAction* estabconnectAct = new QAction(estabconnectIcon, tr("��������"), this);
	estabconnectAct->setStatusTip(tr("��������"));
	//����
	connectMenu->addAction(estabconnectAct);
	//---------------
	connectMenu->addSeparator();
	//������
	QMenu* connector = connectMenu->addMenu(tr("������"));
	//�˶���
	const QIcon kenimaticIcon = QIcon::fromTheme("kenimaticIcon-pair");
	QAction* kenimaticAct = new QAction(kenimaticIcon, tr("�˶���"), this);
	kenimaticAct->setStatusTip(tr("�˶���"));
	kenimaticAct->setCheckable(true);
	//����
	connector->addAction(kenimaticAct);
	//��������
	const QIcon controlBUSIcon = QIcon::fromTheme("control-BUS");
	QAction* controlBUSAct = new QAction(controlBUSIcon, tr("��������BUS"), this);
	controlBUSAct->setStatusTip(tr("��������"));
	controlBUSAct->setCheckable(true);
	//����
	connector->addAction(controlBUSAct);

	//5.����
	QMenu* simulationMenu = menuBar()->addMenu(tr("����"));
	//���ɴ��벢���
	const QIcon generatecodeIcon = QIcon::fromTheme("code-generation");
	QAction* generatecodeAct = new QAction(generatecodeIcon, tr("ģ�ͼ��"), this);
	generatecodeAct->setStatusTip(tr("ģ�ͼ��"));
	//����
	simulationMenu->addAction(generatecodeAct);
	//�������
	const QIcon simulationAndsolveIcon = QIcon::fromTheme("simulation-solve");
	simulationAndsolveAct = new QAction(simulationAndsolveIcon, tr("�������"), this);
	simulationAndsolveAct->setStatusTip(tr("�������"));
	simulationAndsolveAct->setCheckable(true);
	simulationAndsolveAct->setChecked(false);
	simulationAndsolveAct->setText(tr("�������"));
	//����
	simulationMenu->addAction(simulationAndsolveAct);
	//��������
	const QIcon digitalTwinsIcon = QIcon::fromTheme("digital-twins");
	digitalTwinsAct = new QAction(digitalTwinsIcon, tr("��������"), this);
	digitalTwinsAct->setStatusTip(tr("��������"));
	digitalTwinsAct->setCheckable(true);
	digitalTwinsAct->setChecked(false);
	digitalTwinsAct->setText(tr("��������"));
	//����
	simulationMenu->addAction(digitalTwinsAct);
	// �����繹��
	const QIcon neuralNetworkIcon = QIcon::fromTheme("neural-network");
	neuralNetworkAct = new QAction(digitalTwinsIcon, tr("�����繹��"), this);
	neuralNetworkAct->setStatusTip(tr("�����繹��"));
	neuralNetworkAct->setCheckable(true);
	neuralNetworkAct->setChecked(false);
	neuralNetworkAct->setText(tr("�����繹��"));
	//����
	simulationMenu->addAction(neuralNetworkAct);
	////��������
	//const QIcon AnimationIcon = QIcon::fromTheme("animation-driven");
	//QAction* AnimationAct = new QAction(AnimationIcon, tr("��������"), this);
	//AnimationAct->setStatusTip(tr("��������"));
	////����
	//simulationMenu->addAction(AnimationAct);

	//6. ����ϵͳ���ò˵�;
	QMenu* setMenu = menuBar()->addMenu(tr("����"));
	//ϵͳ����
	const QIcon sysSetIcon = QIcon::fromTheme("set-system");
	QAction* sysSetAct = new QAction(sysSetIcon, tr("ϵͳ����"), this);
	sysSetAct->setStatusTip(tr("ϵͳ����"));
	connect(sysSetAct, &QAction::triggered, this, &MainWindow::systemSet);
	setMenu->addAction(sysSetAct);
	//������ɫ����
	const QIcon backColorIcon = QIcon::fromTheme("set-systemColor");
	QAction* backColorAct = new QAction(backColorIcon, tr("������ɫ����"), this);
	backColorAct->setStatusTip(tr("������ɫ����"));
	connect(backColorAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView(); \
		view->onBackground(); \
		});
	setMenu->addAction(backColorAct);

	//������ͼ
	const QIcon backTextureIcon = QIcon::fromTheme("set-systemBackFig");
	QAction* backTextureAct = new QAction(backTextureIcon, tr("����ͼƬ����"), this);
	backTextureAct->setStatusTip(tr("����ͼƬ����"));
	connect(backTextureAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView(); \
		view->onEnvironmentMap(); \
		});
	setMenu->addAction(backTextureAct);
	setMenu->addSeparator();
	//��ʾȫ������ϵ
	const QIcon showOxyzIcon = QIcon::fromTheme("set-ShowOxyz");
	showOxyzAct = new QAction(showOxyzIcon, tr("��ʾȫ������ϵ"), this);
	showOxyzAct->setStatusTip(tr("��ʾȫ������ϵ"));
	showOxyzAct->setCheckable(true);
	showOxyzAct->setChecked(false);
	connect(showOxyzAct, &QAction::triggered, this, &MainWindow::ShowOxyz);
	setMenu->addAction(showOxyzAct);

	//7. ���Ӱ����˵�
	QMenu* helpMenu = menuBar()->addMenu(tr("����"));
	QAction* aboutAct = helpMenu->addAction(tr("���ڣ�&A��"), this, &MainWindow::about);
	aboutAct->setStatusTip(tr("��ʾ���ڶԻ���"));


	//����������
	//�����������ݣ����롢װ�䡢���ӡ����ˡ���顢����
	//8.1�������������
	QToolBar* MdleditToolBar = addToolBar(tr("edit"));
	QAction* insertAct = new QAction(tr("����"), this);
	insertAct->setStatusTip(tr("����"));
	connect(insertAct, &QAction::triggered, this, &MainWindow::insert);
	MdleditToolBar->addAction(insertAct);//����

	//�������
	QAction* attributeAct = new QAction(tr("�������"), this);
	attributeAct->setStatusTip(tr("�������"));
	connect(attributeAct, &QAction::triggered, this, &MainWindow::attributeSetting);
	MdleditToolBar->addAction(attributeAct);

	//select
	subSelmenu = new QMenu("ѡ��", this);
	//submenu->setIcon(genIcon(iconSize(), 1, Qt::black, dpr));
	subSelmenu->addAction(selAnyAct);// (genIcon(iconSize(), "A", Qt::blue, dpr), "A");
	subSelmenu->addAction(selVtxAct);
	subSelmenu->addAction(selEdgeAct);
	subSelmenu->addAction(selFaceAct);
	subSelmenu->addAction(selBodyAct);
	subSelmenu->addAction(selConnectorAct);
	MdleditToolBar->addAction(subSelmenu->menuAction());

	//8.2װ�乤����
	QToolBar* assemblyToolBar = addToolBar(tr("assembly"));
	QAction* assemblyAct = new QAction(tr("װ��"), this);
	assemblyAct->setStatusTip(tr("װ��"));
	/*connect(assemblyAct, &QAction::triggered, this, &MainWindow::assembly);*/
	connect(assemblyAct, &QAction::triggered, [&]() {
		myGeomWidget->GetView()->NewAssembleCommand();
	});
	assemblyToolBar->addAction(assemblyAct);

	//����
	QAction* ConnectAct = new QAction(tr("����"), this);
	ConnectAct->setStatusTip(tr("����"));
	/*connect(ConnectAct, &QAction::triggered, this, &MainWindow::connection);*/
	connect(ConnectAct, &QAction::triggered, [&]() {
		View* view = myGeomWidget->GetView();
		view->NewConnectCommand();
	});
	assemblyToolBar->addAction(ConnectAct);

	//����
	QAction* postureAct = new QAction(tr("����"), this);
	postureAct->setStatusTip(tr("����"));
	//connect(postureAct, &QAction::triggered, this, &MainWindow::adjust);
	//����
	connect(postureAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView();\
		view->NewRepostureCommand();\
	});
	assemblyToolBar->addAction(postureAct);

	//8.3���湤����
	QToolBar* simulationToolBar = addToolBar(tr("simulation"));
	//��������
	QAction* ParaAct = new QAction(tr("��������"), this);
	ParaAct->setStatusTip(tr("��������"));
	connect(ParaAct, &QAction::triggered, this, &MainWindow::setpara);
	simulationMenu->addAction(ParaAct);
	simulationToolBar->addAction(ParaAct);
	//��������
	QAction* SimulateSetAct = new QAction(tr("��������"), this);
	SimulateSetAct->setStatusTip(tr("��������"));
	connect(SimulateSetAct, &QAction::triggered, this, &MainWindow::setsimulate);
	simulationMenu->addAction(SimulateSetAct);
	//����������ģ�ͼ��
	connect(generatecodeAct, &QAction::triggered, this, &MainWindow::modelCheck);
	simulationToolBar->addAction(generatecodeAct);
	//�������
	connect(simulationAndsolveAct, &QAction::triggered, this, &MainWindow::simulation);
	simulationToolBar->addAction(simulationAndsolveAct);
	//��������
	connect(digitalTwinsAct, &QAction::triggered, this, &MainWindow::digitaltwins);
	simulationToolBar->addAction(digitalTwinsAct);
	//�����繹��
	connect(neuralNetworkAct, &QAction::triggered, this, &MainWindow::neuralnetwork);
	simulationToolBar->addAction(neuralNetworkAct);
	//��������
	postProcessToolBar = addToolBar(tr("postProcess"));
	//����
	QAction* animationAct = new QAction(tr("����"), this);
	animationAct->setStatusTip(tr("�˶�ģ��"));
	connect(animationAct, &QAction::triggered, this, &MainWindow::animation);
	//simulationMenu->addAction(animationAct);
	postProcessToolBar->addAction(animationAct);
	postProcessToolBar->hide();

	//��������
	//�ڶ�myGeomWidget��������ʱ��������
	QFrame* aViewFrame = new QFrame(this);
	aViewFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	aViewFrame->setLineWidth(3);

	//ʹ�ò����޷���ɼȶ�Ҫ��ʹ�÷��Ѵ���
	//�����Ѵ���
	//QSplitter* 
	mainSplitter = new QSplitter(Qt::Horizontal);
	//��߷��Ѵ���
	QSplitter* frameSplitter = new QSplitter(Qt::Vertical);
	frameSplitter->setOpaqueResize(true);
	frameSplitter->setLineWidth(5);

	myDocument3d = new DocumentModel(this);
	//myDocument3d->setDocType(nDocType);

	//��߷��������
	m_pModelLibWidget = new ModelLibWidget(this);
	frameSplitter->addWidget(m_pModelLibWidget);
	m_pAssemblyTreeWidget = new AssemblyTreeWidget(this);
	frameSplitter->addWidget(m_pAssemblyTreeWidget);
	m_AnimationWidget = new AnimationWidget(this);
	frameSplitter->addWidget(m_AnimationWidget);
	m_AnimationWidget->hide();
	m_DigitaltwinsWidget = new DigitaltwinsWidget(this);
	frameSplitter->addWidget(m_DigitaltwinsWidget);
	m_DigitaltwinsWidget->hide();
	m_NeuralnetworkWidget = new NeuralnetworkWidget(this);
	frameSplitter->addWidget(m_NeuralnetworkWidget);
	m_NeuralnetworkWidget->hide();

	myGeomWidget = new GeomWidget(myDocument3d, aViewFrame);
	connect(this->myGeomWidget->GetView(), &View::StateChanged, this, &MainWindow::OnStatusChanged);
	mainSplitter->addWidget(frameSplitter);
	mainSplitter->addWidget(myGeomWidget);
	
	frameSplitter->setStretchFactor(0, 3);
	frameSplitter->setStretchFactor(1, 5);
	this->setCentralWidget(mainSplitter);
}

void MainWindow::open()
{
	//���ĵ�ʱ�������ǰ�ĵ�Ϊ�գ����ڵ�ǰ���̴�
	//�����½����̣��൱��ִ�� M3dStudio filename
	const QString fileName = QFileDialog::getOpenFileName(this, QString(), QString(), "ģ���ļ�(*.m3dcom *.m3dmdl);;���(*.m3dcom);;ϵͳ(*.m3dmdl)");
	if (!fileName.isEmpty())
		openFile(fileName);
}

bool MainWindow::openFile(const QString& fileName)
{
	//�����ǰ���ĵ��򿪣����½�����;
	if (myDocument3d != nullptr)
	{
		QString cmdLine = QCoreApplication::applicationFilePath();
		cmdLine += " --open \"";
		cmdLine += fileName;
		cmdLine += "\"";
		QProcess* pProcess = new QProcess();
		pProcess->start(cmdLine);
		return true;
	}

	//int fileType = 0; //ȱʡΪ���
	if (QFileInfo(fileName).suffix().toLower() == "m3dcom")
	{
		//fileType = 1;
		nDocType = 1;
	}
	else if (QFileInfo(fileName).suffix().toLower() == "m3dmdl")
	{
		//fileType = 2;
		nDocType = 2;
	}
	newFile(nDocType/*fileType*/);

	const bool succeeded = loadFile(fileName);
	if (succeeded)
	{
		statusBar()->showMessage(tr("File loaded"), 2000);
		QString sTitle = tr("M3d��ģ�����һ�廯ƽ̨����");
		sTitle += fileName;
		setWindowTitle(sTitle);
	}
	else {
		statusBar()->showMessage(tr("File load failed"), 2000);
	}

	myGeomWidget->FitAll();

	return succeeded;
}

DocumentCommon* MainWindow::getDocument()
{
	return myDocument3d;
}

GeomWidget* MainWindow::getGeomWidget() const
{
	return myGeomWidget;
}




bool MainWindow::loadFile(const QString& fileName)
{
	const bool succeeded = myDocument3d->loadFile(fileName);
	if (!succeeded)
		return false;

	MainWindow::prependToRecentFiles(fileName);
	return succeeded;
}

//���뼸��
void MainWindow::import()
{
	if (!myDocument3d->IsEmpty())
	{
		//����֮ǰ���
		if (QMessageBox::question(this, "��ʾ", "����ǰҪ�����ǰ���κ��ĵ�������ݣ��Ƿ������", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
		{
			return;
		}
	}

	Translate* anTrans = new Translate(this);
	Handle(AIS_InteractiveContext) context = myDocument3d->getContext();
	Handle(TopTools_HSequenceOfShape) shapes;
	bool stat = anTrans->importModel(shapes, context);
	if (stat && !shapes.IsNull())
	{
		//������ļ�����״�����ĵ�
		//myDocument3d->serializeShapes(shapes);
		if (shapes->Length() > 1)
		{//�����״��������1����ֻȡ��һ��
			QMessageBox::information(this, "����", "����������������1��ȡ��һ����", QMessageBox::Ok);
		}
		TopoDS_Shape aShape = shapes->Value(1);
		//myDocument3d->SaveShapes(shapes);
		//����ʵ������ĵ�
		myDocument3d->SetMainShape(aShape);
		//myDocument3d->ShowShapes(shapes);
		//�����ĵ�������ʾ
		myDocument3d->UpdateDisplay();

		myGeomWidget->FitAll();
		updateMenus();
	}

	delete anTrans;
}

////�������
//void MainWindow::export()
//{
//    //��ʱ��ʵ��
//}

static inline QString recentFilesKey() { return QStringLiteral("recentFileList"); }
static inline QString fileKey() { return QStringLiteral("file"); }

static QStringList readRecentFiles(QSettings& settings)
{
	QStringList result;
	const int count = settings.beginReadArray(recentFilesKey());
	for (int i = 0; i < count; ++i) {
		settings.setArrayIndex(i);
		result.append(settings.value(fileKey()).toString());
	}
	settings.endArray();
	return result;
}

static void writeRecentFiles(const QStringList& files, QSettings& settings)
{
	const int count = files.size();
	settings.beginWriteArray(recentFilesKey());
	for (int i = 0; i < count; ++i) {
		settings.setArrayIndex(i);
		settings.setValue(fileKey(), files.at(i));
	}
	settings.endArray();
}

bool MainWindow::hasRecentFiles()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const int count = settings.beginReadArray(recentFilesKey());
	settings.endArray();
	return count > 0;
}

void MainWindow::prependToRecentFiles(const QString& fileName)
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

	const QStringList oldRecentFiles = readRecentFiles(settings);
	QStringList recentFiles = oldRecentFiles;
	recentFiles.removeAll(fileName);
	recentFiles.prepend(fileName);
	if (oldRecentFiles != recentFiles)
		writeRecentFiles(recentFiles, settings);

	setRecentFilesVisible(!recentFiles.isEmpty());
}

void MainWindow::setRecentFilesVisible(bool visible)
{
	recentFileSubMenuAct->setVisible(visible);
	recentFileSeparator->setVisible(visible);
}

void MainWindow::updateRecentFileActions()
{
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());

	const QStringList recentFiles = readRecentFiles(settings);
	const int count = qMin(int(MaxRecentFiles), recentFiles.size());
	int i = 0;
	for (; i < count; ++i) {
		const QString fileName = QFileInfo(recentFiles.at(i)).fileName();
		recentFileActs[i]->setText(tr("&%1 %2").arg(i + 1).arg(fileName));
		recentFileActs[i]->setData(recentFiles.at(i));
		recentFileActs[i]->setVisible(true);
	}
	for (; i < MaxRecentFiles; ++i)
		recentFileActs[i]->setVisible(false);
}

void MainWindow::openRecentFile()
{
	if (const QAction* action = qobject_cast<const QAction*>(sender()))
		openFile(action->data().toString());
}

void MainWindow::save()
{
	//�洢λͼ
	QPixmap pm;
	//pm = QPixmap::grabWidget(this/*myGeomWidget*/);
	pm = QPixmap::grabWindow(0);
	//pm.save("temp.png", "PNG");
	//pm.load("D:/M3drepos/M3d/M3dStudio/images/import.png");
	if (myDocument3d)
	{
		myDocument3d->SavePixmap(pm);
	}

	//���浱ǰ�ĵ������ݵ�ǰ�ĵ�����;
	if (myDocument3d && myDocument3d->save())
	{
		statusBar()->showMessage(tr("�ļ�����ɹ�."), 2000);
	}
}

void MainWindow::saveAs()
{
	//�洢λͼ
	QPixmap pm;
	pm.grabWidget(myGeomWidget);
	if (myDocument3d)
	{
		myDocument3d->SavePixmap(pm);
	}

	//���Ϊ���½������ͬ���͵��ĵ�
	if (myDocument3d && myDocument3d->saveAs())
		statusBar()->showMessage(tr("�ļ����ɹ�."), 2000);
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("����M3d"),
		tr("����Modelica����ά��ģ-ϵͳ����һ�廯ƽ̨\n���пƼ���ѧ��Ȩ����"));
}

void MainWindow::updateMenus()
{
	//    bool hasMdiChild = (activeMdiChild() != 0);
	//    saveAct->setEnabled(hasMdiChild);
	//    saveAsAct->setEnabled(hasMdiChild);
	//#ifndef QT_NO_CLIPBOARD
	//    pasteAct->setEnabled(hasMdiChild);
	//#endif
	//    closeAct->setEnabled(hasMdiChild);
	//    closeAllAct->setEnabled(hasMdiChild);
	//    tileAct->setEnabled(hasMdiChild);
	//    cascadeAct->setEnabled(hasMdiChild);
	//    nextAct->setEnabled(hasMdiChild);
	//    previousAct->setEnabled(hasMdiChild);
	//    windowMenuSeparatorAct->setVisible(hasMdiChild);

		//undo redo��ʹ��
	if (undoAct)
	{
		undoAct->setEnabled(myDocument3d->canUndo());
	}
	if (redoAct)
	{
		redoAct->setEnabled(myDocument3d->canRedo());
	}

	//������ɫ
	bool OneOrMoreIsShadingOrWireframe = false;
	if (myDocument3d)
	{
		for (myDocument3d->getContext()->InitSelected(); myDocument3d->getContext()->MoreSelected(); myDocument3d->getContext()->NextSelected())
			if (myDocument3d->getContext()->IsDisplayed(myDocument3d->getContext()->SelectedInteractive(), 0)
				|| myDocument3d->getContext()->IsDisplayed(myDocument3d->getContext()->SelectedInteractive(), 1))
				OneOrMoreIsShadingOrWireframe = true;
		//colorAct->setEnabled(OneOrMoreIsShadingOrWireframe);
		colorAct->setEnabled(true);
	}
}

void MainWindow::updateWindowMenu()
{
	/*   windowMenu->clear();
	   windowMenu->addAction(closeAct);
	   windowMenu->addAction(closeAllAct);
	   windowMenu->addSeparator();
	   windowMenu->addAction(tileAct);
	   windowMenu->addAction(cascadeAct);
	   windowMenu->addSeparator();
	   windowMenu->addAction(nextAct);
	   windowMenu->addAction(previousAct);
	   windowMenu->addAction(windowMenuSeparatorAct);

	   QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
	   windowMenuSeparatorAct->setVisible(!windows.isEmpty());

	   for (int i = 0; i < windows.size(); ++i) {
		   QMdiSubWindow *mdiSubWindow = windows.at(i);
		   MdiChild *child = qobject_cast<MdiChild *>(mdiSubWindow->widget());

		   QString text;
		   if (i < 9) {
			   text = tr("&%1 %2").arg(i + 1)
								  .arg(child->userFriendlyCurrentFile());
		   } else {
			   text = tr("%1 %2").arg(i + 1)
								 .arg(child->userFriendlyCurrentFile());
		   }
		   QAction *action = windowMenu->addAction(text, mdiSubWindow, [this, mdiSubWindow]() {
			   mdiArea->setActiveSubWindow(mdiSubWindow);
		   });
		   action->setCheckable(true);
		   action ->setChecked(child == activeMdiChild());
	   }*/
}



void MainWindow::createActions0()
{
	fileMenu = menuBar()->addMenu(tr("�ļ�"));
	fileToolBar = addToolBar(tr("File"));

	const QIcon newIcon = QIcon::fromTheme("document-new", QIcon("images/new.png"));
	newAct = new QAction(newIcon, tr("�½�..."), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("�½��ĵ�"));
	connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
	fileMenu->addAction(newAct);
	fileToolBar->addAction(newAct);

	const QIcon openIcon = QIcon::fromTheme("document-open", QIcon("images/open.png"));
	QAction* openAct = new QAction(openIcon, tr("��..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("���ĵ�"));
	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	fileMenu->addAction(openAct);
	fileToolBar->addAction(openAct);

	//const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
	//saveAct = new QAction(saveIcon, tr("&Save"), this);
	//saveAct->setShortcuts(QKeySequence::Save);
	//saveAct->setStatusTip(tr("Save the document to disk"));
	//connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	//fileToolBar->addAction(saveAct);

	//const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
	//saveAsAct = new QAction(saveAsIcon, tr("Save &As..."), this);
	//saveAsAct->setShortcuts(QKeySequence::SaveAs);
	//saveAsAct->setStatusTip(tr("Save the document under a new name"));
	//connect(saveAsAct, &QAction::triggered, this, &MainWindow::saveAs);
	//fileMenu->addAction(saveAsAct);

	fileMenu->addSeparator();

	QMenu* recentMenu = fileMenu->addMenu(tr("����ĵ�"));
	connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
	recentFileSubMenuAct = recentMenu->menuAction();

	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileActs[i]->setVisible(false);
	}

	recentFileSeparator = fileMenu->addSeparator();

	setRecentFilesVisible(MainWindow::hasRecentFiles());

	//fileMenu->addAction(tr("Switch layout direction"), this, &MainWindow::switchLayoutDirection);

	fileMenu->addSeparator();

	//! [0]
	const QIcon exitIcon = QIcon::fromTheme("application-exit");
	QAction* exitAct = fileMenu->addAction(exitIcon, tr("�˳�"), qApp, &QApplication::closeAllWindows);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("�˳�"));
	fileMenu->addAction(exitAct);
	//! [0]

	//#ifndef QT_NO_CLIPBOARD
	//    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
	//    QToolBar *editToolBar = addToolBar(tr("Edit"));
	//
	//    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
	//    cutAct = new QAction(cutIcon, tr("Cu&t"), this);
	//    cutAct->setShortcuts(QKeySequence::Cut);
	//    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
	//                            "clipboard"));
	//    connect(cutAct, &QAction::triggered, this, &MainWindow::cut);
	//    editMenu->addAction(cutAct);
	//    editToolBar->addAction(cutAct);
	//
	//    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
	//    copyAct = new QAction(copyIcon, tr("&Copy"), this);
	//    copyAct->setShortcuts(QKeySequence::Copy);
	//    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
	//                             "clipboard"));
	//    connect(copyAct, &QAction::triggered, this, &MainWindow::copy);
	//    editMenu->addAction(copyAct);
	//    editToolBar->addAction(copyAct);
	//
	//    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
	//    pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
	//    pasteAct->setShortcuts(QKeySequence::Paste);
	//    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
	//                              "selection"));
	//    connect(pasteAct, &QAction::triggered, this, &MainWindow::paste);
	//    editMenu->addAction(pasteAct);
	//    editToolBar->addAction(pasteAct);
	//#endif

	 /*   windowMenu = menuBar()->addMenu(tr("&Window"));
		connect(windowMenu, &QMenu::aboutToShow, this, &MainWindow::updateWindowMenu);

		closeAct = new QAction(tr("Cl&ose"), this);
		closeAct->setStatusTip(tr("Close the active window"));*/
		/*   connect(closeAct, &QAction::triggered,
				   mdiArea, &QMdiArea::closeActiveSubWindow);

		   closeAllAct = new QAction(tr("Close &All"), this);
		   closeAllAct->setStatusTip(tr("Close all the windows"));
		   connect(closeAllAct, &QAction::triggered, mdiArea, &QMdiArea::closeAllSubWindows);

		   tileAct = new QAction(tr("&Tile"), this);
		   tileAct->setStatusTip(tr("Tile the windows"));
		   connect(tileAct, &QAction::triggered, mdiArea, &QMdiArea::tileSubWindows);

		   cascadeAct = new QAction(tr("&Cascade"), this);
		   cascadeAct->setStatusTip(tr("Cascade the windows"));
		   connect(cascadeAct, &QAction::triggered, mdiArea, &QMdiArea::cascadeSubWindows);

		   nextAct = new QAction(tr("Ne&xt"), this);
		   nextAct->setShortcuts(QKeySequence::NextChild);
		   nextAct->setStatusTip(tr("Move the focus to the next window"));
		   connect(nextAct, &QAction::triggered, mdiArea, &QMdiArea::activateNextSubWindow);

		   previousAct = new QAction(tr("Pre&vious"), this);
		   previousAct->setShortcuts(QKeySequence::PreviousChild);
		   previousAct->setStatusTip(tr("Move the focus to the previous "
										"window"));
		   connect(previousAct, &QAction::triggered, mdiArea, &QMdiArea::activatePreviousSubWindow);*/

	windowMenuSeparatorAct = new QAction(this);
	windowMenuSeparatorAct->setSeparator(true);

	updateWindowMenu();

	menuBar()->addSeparator();

	QMenu* helpMenu = menuBar()->addMenu(tr("������&H��"));

	QAction* aboutAct = helpMenu->addAction(tr("���ڣ�&A��"), this, &MainWindow::about);
	aboutAct->setStatusTip(tr("��ʾ���ڶԻ���"));

}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("׼����"));
}

void MainWindow::SetColor()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		((DocumentComponent*)myDocument3d)->objColor();
	}
	updateMenus();
}

void MainWindow::SetMaterial()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		//((DocumentComponent*)myDocument3d)->objMaterial();
		MaterialsSetDlg* matDlg = new MaterialsSetDlg(this);
		matDlg->SetDocument((DocumentComponent*)myDocument3d);
		int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // ��ȡ�������߶�
		int diaWidth = matDlg->width();
		int diaHeight = matDlg->height();
		QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//���öԻ���λ��
		matDlg->setGeometry(globalPos.x() + this->width() - diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
		matDlg->show();
	}
	updateMenus();
}

void MainWindow::readSettings()
{
	//���봰������
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
	if (geometry.isEmpty()) {
		const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
		resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
		move((availableGeometry.width() - width()) / 2,
			(availableGeometry.height() - height()) / 2);
	}
	else {
		restoreGeometry(geometry);
	}
}

void MainWindow::writeSettings()
{
	//���洰������
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("geometry", saveGeometry());
}

//���Ժ���
void MainWindow::MakeBox()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		((DocumentComponent*)myDocument3d)->MakeBox();
	}
}
void MainWindow::MakeSpring()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		((DocumentComponent*)myDocument3d)->MakeSpring();
	}
}
void MainWindow::ExportBREP()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		((DocumentComponent*)myDocument3d)->ExportBREP();
	}
}

/// <summary>
/// ���벿��
/// </summary>
void MainWindow::insert()
{
	insertCom();
}


void MainWindow::insertCom(const QString& fileName)
{
	if (myDocument3d && myDocument3d->IsModel())
	{
		if (fileName == "")
			((DocumentModel*)myDocument3d)->insertComponent();
		else
			((DocumentModel*)myDocument3d)->insertComponent(fileName.toStdString().c_str());
	}
}

void MainWindow::attributeSetting()
{
	ComAttriDlg* attributeDlg = new ComAttriDlg(this);
	attributeDlg->show();
}

//װ���������
void MainWindow::assembly()
{
	if (myDocument3d && myDocument3d->IsModel())
	{
		((DocumentModel*)myDocument3d)->Assemble2Components();
	}
}

void MainWindow::UpdateAssemblyTree()
{
	if (myDocument3d && myDocument3d->IsModel())
	{
		DocumentModel* pDocMdl = (DocumentModel*)myDocument3d;
		Component* pRoot = pDocMdl->GetComponent();

		m_pAssemblyTreeWidget->UpdateAssemblyTree(pRoot);
	}
}

//��������
void MainWindow::connection()
{
	if (myDocument3d && myDocument3d->IsModel())
	{
		((DocumentModel*)myDocument3d)->Connect2Components();
	}
}

//����
void MainWindow::adjust()
{
	if (myDocument3d && myDocument3d->IsModel())
	{
		((DocumentModel*)myDocument3d)->adjustComponent();
	}
}

//��������
void MainWindow::setpara()
{
	if (m_ParaDock!=nullptr&&m_ParaDock->getWidget()) return;
	//static int ticks = 1;
	QAction* act = qobject_cast<QAction*>(sender());
	m_ParaDock = new ParaDockWidget(this, act);
	addDockWidget(Qt::RightDockWidgetArea, m_ParaDock->getWidget());

}

//��������
void MainWindow::setsimulate()
{
	SimulateSetDlg* simulatesetDlg = new SimulateSetDlg(this);
	simulatesetDlg->show();
}

//ģ����������
void MainWindow::modelCheck()
{
	//����
	if (myDocument3d && myDocument3d->IsModel())
	{
		((DocumentModel*)myDocument3d)->InitCheckModel();
	}
}

void MainWindow::simulation()
{
	if (!simulationAndsolveAct->isChecked())
	{
		//simulationAndsolveAct->setChecked(false);
		simulationAndsolveAct->setText("�������");

		//�ر�ģ�ͷ���
		if (myDocument3d && myDocument3d->IsModel())
		{
			((DocumentModel*)myDocument3d)->CloseSimulate();
		}
		//postProcessToolBar->hide();
		m_AnimationWidget->hide();
		m_pModelLibWidget->show();
		m_pAssemblyTreeWidget->show();
	}
	else {
		//simulationAndsolveAct->setChecked(true);

		//�����ʱ�����������ˣ���ر���
		if (digitalTwinsAct->isChecked())
		{
			digitalTwinsAct->setChecked(false);
			digitalTwinsAct->setText("��������");
			//�ر�ģ������
			if (myDocument3d && myDocument3d->IsModel())
			{
				//�о�����ͨ�õ�
				((DocumentModel*)myDocument3d)->CloseSimulate();
			}
			m_DigitaltwinsWidget->hide();
		}
		
		simulationAndsolveAct->setText("�˳�����");

		//ģ�ͷ���
		if (myDocument3d && myDocument3d->IsModel())
		{
			((DocumentModel*)myDocument3d)->SimulateModel();
		}
		//postProcessToolBar->show();
		m_AnimationWidget->show();
		m_pModelLibWidget->hide();
		m_pAssemblyTreeWidget->hide();
	}
}

void MainWindow::digitaltwins()
{
	if (!digitalTwinsAct->isChecked())
	{
		digitalTwinsAct->setText("��������");

		//�ر�ģ������
		if (myDocument3d && myDocument3d->IsModel())
		{
			//�о�����ͨ�õ�
			((DocumentModel*)myDocument3d)->CloseSimulate();
		}
		m_DigitaltwinsWidget->hide();
		m_pModelLibWidget->show();
		m_pAssemblyTreeWidget->show();
	}
	else {
		//�����ʱģ�ͷ�����ˣ���ر���
		if (simulationAndsolveAct->isChecked())
		{
			simulationAndsolveAct->setChecked(false);
			simulationAndsolveAct->setText("�������");
			//�ر�ģ�ͷ���
			if (myDocument3d && myDocument3d->IsModel())
			{
				((DocumentModel*)myDocument3d)->CloseSimulate();
			}
			m_AnimationWidget->hide();
		}

		digitalTwinsAct->setText("�˳�����");

		//��������ģ��
		if (myDocument3d && myDocument3d->IsModel())
		{
			//��д
			((DocumentModel*)myDocument3d)->TwinDriveModel();
		}
		m_DigitaltwinsWidget->show();
		m_pModelLibWidget->hide();
		m_pAssemblyTreeWidget->hide();
	}

}

void MainWindow::neuralnetwork()
{
	if (!neuralNetworkAct->isChecked())
	{
		neuralNetworkAct->setText("�����繹��");

		m_NeuralnetworkWidget->hide();
		m_pModelLibWidget->show();
		m_pAssemblyTreeWidget->show();
	}
	else {
		//�����ʱģ�ͷ�����ˣ���ر���
		if (simulationAndsolveAct->isChecked())
		{
			simulationAndsolveAct->setChecked(false);
			simulationAndsolveAct->setText("�������");
			//�ر�ģ�ͷ���
			if (myDocument3d && myDocument3d->IsModel())
			{
				((DocumentModel*)myDocument3d)->CloseSimulate();
			}
			m_AnimationWidget->hide();
		}

		//�����ʱ�����������ˣ���ر���
		if (digitalTwinsAct->isChecked())
		{
			digitalTwinsAct->setChecked(false);
			digitalTwinsAct->setText("��������");
			//�ر�ģ������
			if (myDocument3d && myDocument3d->IsModel())
			{
				//�о�����ͨ�õ�
				((DocumentModel*)myDocument3d)->CloseSimulate();
			}
			m_DigitaltwinsWidget->hide();
		}

		neuralNetworkAct->setText("�˳�ѵ��");


		m_NeuralnetworkWidget->show();
		m_pModelLibWidget->hide();
		m_pAssemblyTreeWidget->hide();
	}
}

void MainWindow::OnStatusChanged(const QString& str)
{
	if (!str.isEmpty())
		statusBar()->showMessage(str);
}

//������ʾ
void MainWindow::animation()
{
	//����ڶ���ִ�й����У����˳�
	//���������߳�
	if (myDocument3d && myDocument3d->IsModel())
	{
		((DocumentModel*)myDocument3d)->Animating();
	}
}

void MainWindow::repaint0()
{
	if (myDocument3d && myDocument3d->IsModel())
	{
		((DocumentModel*)myDocument3d)->repaint0();
	}
}

void MainWindow::Close()
{
	if (QMessageBox::question(this, "�˳�ϵͳ", "�˳�ϵͳ����ǰ�ĵ������ı䣬��Ҫ������", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes)
	{
		close();
	}
}

void MainWindow::closeEvent(QCloseEvent* event)
{
	if (myDocument3d)
	{
		//Close();
		if (myDocument3d->IsModified())
		{
			QMessageBox::StandardButton sb = QMessageBox::question(this, "�˳�ϵͳ", "�˳�ϵͳ����ǰ�ĵ������ı䣬��Ҫ������", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
			if (sb == QMessageBox::Yes)
			{
				save();
				event->accept();
			}
			else if (sb == QMessageBox::No)
			{
				event->accept();
			}
			else {//ȡ��
				event->ignore();
			}
		}
		else
			event->accept();
	}
	else {
		event->accept();
	}
}

void MainWindow::ShowOxyz()
{
	if (myDocument3d)
	{
		myDocument3d->ShowOxyz(showOxyzAct->isChecked());
	}
}
