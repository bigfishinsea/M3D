/****************************************************************************
**
** Wu YZ
** 基本思路：模拟多文档，其实单文档
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
	//初始化材料信息，放在此处吧
	InitMaterialInfo();

	setWindowTitle(tr("M3d建模与仿真一体化平台"));
	//setUnifiedTitleAndToolBarOnMac(true);
	showMaximized();
	//设置图标
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
		
	//退出模型加载线程
	if (m_pLibLoadThread)
	{
		m_pLibLoadThread->exit();
		//delete m_pLibLoadThread; //删除出错，算了
	}
	//删除加载的模型: 在库还没有加载完成时，出现异常！！！
	for (int i = 0; i < g_vModelicaModels.size(); i++)
	{
		delete g_vModelicaModels[i];
	}
}

void MainWindow::newFile(int type)
{
	//新建两种文档的一种
	//int newType = type;
	//打开对话框选择
	if (type == 0)
	{
		NewDocDlg selDlg(this);
		selDlg.exec();
		if (selDlg.bAccepted)
		{
			// newType = selDlg.iSelItem + 1;
			type = selDlg.iSelItem + 1;
		}

		//如果当前有文档，则重启一个应用
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
	{   //新建文档;
		switch (nDocType)
		{
		case 1:
			//零件
			CreateComInterface();
			setWindowTitle(tr("M3d建模与仿真一体化平台——未命名零件模型"));
			break;
		case 2:
			//系统
			CreateMdlInterface();
			setWindowTitle(tr("M3d建模与仿真一体化平台——未命名系统模型"));
			break;
		default:
			QMessageBox::information(this, "提示", "新建文档错误", QMessageBox::Ok);
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
	//材料的种类：
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
	//材料的名称
#define NUMOFMAT 26
	string sMaterialNames[NUMOFMAT] = { "Brass", "Bronze", "Copper","Gold", \
		"Pewter","Plastered","Plastified","Silver","Steel","Stone",\
		"ShinyPlastified","Satin","Metalized","Ionized","Chrome","Aluminum",\
		"Obsidian","Neon","Jade","Charcoal","Water","Glass","Diamond","Transparent",\
		"Default","UserDefined" };
	//for(int i=0; i< NUMOFMAT;i++)
	//	g_sMaterialNames.push_back(sMaterialNames[i]);

	string sMaterialChnNames[NUMOFMAT] = { "黄铜","青铜","铜", "金", \
		"锡", "灰泥", "塑料","银","钢","石头",\
		"光泽塑料","绸缎","金属","电离物","铬","铝",\
		"黑曜石","氖气","翡翠","木炭","水","玻璃","钻石","透明物",\
		"缺省材料","自定义材料" };
	//for (int i = 0; i < NUMOFMAT; i++)
	//	g_sMaterialChnNames.push_back(sMaterialChnNames[i]);

	//材料的密度：单位 Kg/mm3，绘图的单位为 m,采用国际单位制！！！
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

	//材料的颜色
	double fMaterialColor[NUMOFMAT][3] = { /*黄色*/{1.0,1.0,0.0},/*青色*/{0.0,1.0,1.0},/*铜*/{186 / 255.0,110 / 255.0,64 / 255.0},/*金色*/{218 / 255.0,178 / 255.0,115 / 255.0},\
		/*锡*/{220 / 255., 223 / 255., 227 / 255.}, { 1.0, 0.5, 0. }, { 0.5, 0.5,0.5 }, /*银白*/{ 233 / 255.,233 / 255.,216 / 255. }, /*钢铁*/{ 118 / 255.,119 / 255.,120 / 255.}, {0.3,0.3,0.3},\
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

	//材料的透明度
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

//如果当前有编辑的文档，则新建进程后new一个零件建模文档
void MainWindow::CreateComInterface()
{
	//QMessageBox::information(this, "提示", "零件建模", QMessageBox::Ok);

	//关联菜单
	//1.先删去全部菜单
	menuBar()->clear();

	//删除文件菜单和工具条
	//fileMenu->clear();
	removeToolBar(fileToolBar);

	//2.添加文件菜单和工具条，及其动作
	fileMenu = menuBar()->addMenu(tr("文件")); //&File
	fileToolBar = addToolBar(tr("File"));

	//2.1 新建
	const QIcon newIcon = QIcon::fromTheme("document-new", QIcon("images/new.png"));
	QAction* newAct = new QAction(newIcon, tr("新建..."), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("新建文档"));
	connect(newAct, &QAction::triggered, this, &MainWindow::newDoc);
	fileMenu->addAction(newAct);
	//2.2 打开
	const QIcon openIcon = QIcon::fromTheme("document-open", QIcon("images/open.png"));
	QAction* openAct = new QAction(openIcon, tr("打开..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("打开文档"));
	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	fileMenu->addAction(openAct);
	//2.3 保存
	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon("images/save.png"));
	QAction* saveAct = new QAction(saveIcon, tr("保存"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("保存文档"));
	connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	fileMenu->addAction(saveAct);
	//2.4 另存
	const QIcon saveasIcon = QIcon::fromTheme("document-save-as", QIcon("images/saveas.png"));
	QAction* saveasAct = new QAction(saveasIcon, tr("另存..."), this);
	saveasAct->setShortcuts(QKeySequence::SaveAs);
	saveasAct->setStatusTip(tr("文档另存为"));
	connect(saveasAct, &QAction::triggered, this, &MainWindow::saveAs);
	fileMenu->addAction(saveasAct);
	//2.5 import导入
	const QIcon importIcon = QIcon::fromTheme("document-import", QIcon("images/import.png"));
	QAction* importAct = new QAction(importIcon, tr("导入..."), this);
	//importAct->setShortcuts(QKeySequence::import);
	importAct->setStatusTip(tr("导入几何"));
	connect(importAct, &QAction::triggered, this, &MainWindow::import);
	fileMenu->addAction(importAct);
	//2.6 export导出：不重要暂时不实现

	//分隔符
	//2.7 最近文档
	fileMenu->addSeparator();
	QMenu* recentMenu = fileMenu->addMenu(tr("最近文档"));
	connect(recentMenu, &QMenu::aboutToShow, this, &MainWindow::updateRecentFileActions);
	recentFileSubMenuAct = recentMenu->menuAction();
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileActs[i]->setVisible(false);
	}
	recentFileSeparator = fileMenu->addSeparator();
	setRecentFilesVisible(MainWindow::hasRecentFiles());
	fileMenu->addSeparator();
	//分隔符
	//2.8 退出
	const QIcon exitIcon = QIcon::fromTheme("application-exit");
	QAction* exitAct = fileMenu->addAction(exitIcon, tr("退出"), qApp, & QApplication::closeAllWindows);
	//QAction * exitAct = new QAction(exitIcon, tr("退出"), this);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("退出"));
	fileMenu->addAction(exitAct);
	//connect(exitAct, &QAction::triggered, this, &MainWindow::Close);
	//connect(this, SIGNAL(closeEvent(QCloseEvent*)), this, SLOT(Close()));

	//3. 增加 编辑 菜单
	QMenu* editMenu = menuBar()->addMenu(tr("编辑"));
	//3.1 undo
	const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon("images/undo.png"));
	undoAct = new QAction(undoIcon, tr("撤销"), this);
	undoAct->setShortcuts(QKeySequence::Undo);
	undoAct->setStatusTip(tr("撤销上一次操作"));
	connect(undoAct, &QAction::triggered, this, &MainWindow::undo);
	editMenu->addAction(undoAct);
	//使能undo
	undoAct->setEnabled(false);
	//connect(editMenu, &QMenu::aboutToShow, this, &MainWindow::undoEnable);
	//3.2 redo
	const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon("images/redo.png"));
	redoAct = new QAction(redoIcon, tr("重做"), this);
	redoAct->setShortcuts(QKeySequence::Redo);
	redoAct->setStatusTip(tr("恢复上一次操作"));
	connect(redoAct, &QAction::triggered, this, &MainWindow::redo);
	editMenu->addAction(redoAct);
	redoAct->setEnabled(false);
	//3.3 删除: 基准面和组件实体
	editMenu->addSeparator();
	const QIcon delIcon = QIcon::fromTheme("edit-delete");
	delAct = new QAction(delIcon, tr("删除"), this);
	delAct->setStatusTip(tr("删除选中的实体"));
	//delAct->setCheckable(false);
	connect(delAct, &QAction::triggered, this, &MainWindow::DeleteSelected);
	editMenu->addAction(delAct);

	//增加一个整体放缩功能
	const QIcon scaleIcon = QIcon::fromTheme("edit-scale");
	QAction * scaleAct = new QAction(scaleIcon, tr("放缩"), this);
	scaleAct->setStatusTip(tr("对主实体进行整体放缩"));
	connect(scaleAct, &QAction::triggered, this, &MainWindow::Scaling);
	editMenu->addAction(scaleAct);

	//增加一个接口放缩功能
	const QIcon connScaleIcon = QIcon::fromTheme("edit-connscale");
	QAction* connScaleAct = new QAction(connScaleIcon, tr("接口放缩"), this);
	connScaleAct->setStatusTip(tr("对接口进行放缩"));
	connect(connScaleAct, &QAction::triggered, this, &MainWindow::connScaling);
	editMenu->addAction(connScaleAct);

	//增加一个形状纹理贴图功能
	const QIcon textureIcon = QIcon::fromTheme("edit-texture");
	QAction* textureAct = new QAction(textureIcon, tr("纹理贴图"), this);
	textureAct->setStatusTip(tr("对形状进行纹理贴图"));
	connect(textureAct, &QAction::triggered, this, &MainWindow::Texture);
	editMenu->addAction(textureAct);

	//3.4 选择：各种实体类型
	editMenu->addSeparator();
	QMenu* selectMenu = editMenu->addMenu(tr("选择实体类型"));
	//QAction * selectSubMenuAct = selectMenu->menuAction();
	//顶点
	const QIcon selVtxIcon = QIcon::fromTheme("edit-select-vertex"/*, QIcon("images/redo.png")*/);
	selVtxAct = new QAction(selVtxIcon, tr("顶点"), this);
	//selVtxAct->setShortcuts(QKeySequence::Redo);
	selVtxAct->setStatusTip(tr("选择顶点类型"));
	selVtxAct->setCheckable(true);
	connect(selVtxAct, &QAction::triggered, this, &MainWindow::selectVertex);
	selectMenu->addAction(selVtxAct);
	//边
	const QIcon selEdgeIcon = QIcon::fromTheme("edit-select-edge"/*, QIcon("images/redo.png")*/);
	selEdgeAct = new QAction(selEdgeIcon, tr("棱边"), this);
	selEdgeAct->setStatusTip(tr("选择棱边类型"));
	selEdgeAct->setCheckable(true);
	connect(selEdgeAct, &QAction::triggered, this, &MainWindow::selectEdge);
	selectMenu->addAction(selEdgeAct);
	//面
	const QIcon selFaceIcon = QIcon::fromTheme("edit-select-face"/*, QIcon("images/redo.png")*/);
	selFaceAct = new QAction(selFaceIcon, tr("面"), this);
	selFaceAct->setStatusTip(tr("选择面类型"));
	selFaceAct->setCheckable(true);
	connect(selFaceAct, &QAction::triggered, this, &MainWindow::selectFace);
	selectMenu->addAction(selFaceAct);
	//体
	const QIcon selBodyIcon = QIcon::fromTheme("edit-select-body"/*, QIcon("images/redo.png")*/);
	selBodyAct = new QAction(selBodyIcon, tr("体"), this);
	selBodyAct->setStatusTip(tr("选择体类型"));
	selBodyAct->setCheckable(true);
	connect(selBodyAct, &QAction::triggered, this, &MainWindow::selectBody);
	selectMenu->addAction(selBodyAct);
	//接口
	const QIcon selConnectorIcon = QIcon::fromTheme("edit-select-connector"/*, QIcon("images/redo.png")*/);
	selConnectorAct = new QAction(selConnectorIcon, tr("接口"), this);
	selConnectorAct->setStatusTip(tr("选择接口类型"));
	connect(selConnectorAct, &QAction::triggered, this, &MainWindow::selectConnector);
	selConnectorAct->setCheckable(true);
	selectMenu->addAction(selConnectorAct);
	//任一
	const QIcon selAnyIcon = QIcon::fromTheme("edit-select-any"/*, QIcon("images/redo.png")*/);
	selAnyAct = new QAction(selAnyIcon, tr("任一"), this);
	selAnyAct->setStatusTip(tr("选择任一类型"));
	selAnyAct->setCheckable(true);
	selAnyAct->setChecked(true);
	connect(selAnyAct, &QAction::triggered, this, &MainWindow::selectAny);
	selectMenu->addAction(selAnyAct);
	
	//4. 增加 约束 菜单
	QMenu* constriantMenu = menuBar()->addMenu(tr("约束"));
	//显示、隐藏约束管理 停靠窗口
	const QIcon showConsDockwinIcon = QIcon::fromTheme("constraint-show");
	showConsDockwinAct = new QAction(showConsDockwinIcon, tr("约束管理窗口"), this);
	showConsDockwinAct->setCheckable(true);
	showConsDockwinAct->setStatusTip(tr("显示/关闭约束管理窗口"));
	connect(showConsDockwinAct, &QAction::triggered, this, &MainWindow::showConsDockwin);
	constriantMenu->addAction(showConsDockwinAct);
	//---------------
	constriantMenu->addSeparator();
	//重构
	const QIcon regenIcon = QIcon::fromTheme("constraint-regen");
	showVoxelDockwinAct = new QAction(regenIcon, tr("重构几何"), this);
	showVoxelDockwinAct->setCheckable(true);
	showVoxelDockwinAct->setStatusTip(tr("由约束重构几何"));
	connect(showVoxelDockwinAct, &QAction::triggered, this, &MainWindow::regenGeometry);
	constriantMenu->addAction(showVoxelDockwinAct);

	//5. 增加 信息 菜单
	QMenu* cyberMenu = menuBar()->addMenu(tr("信息"));
	//显示信息管理窗口
	const QIcon showCyberDockwinIcon = QIcon::fromTheme("cyber-show");
	showCyberDockwinAct = new QAction(showCyberDockwinIcon, tr("信息管理窗口"), this);
	showCyberDockwinAct->setCheckable(true);
	showCyberDockwinAct->setStatusTip(tr("显示/关闭信息管理窗口"));
	connect(showCyberDockwinAct, &QAction::triggered, this, &MainWindow::showCyberDockwin);
	cyberMenu->addAction(showCyberDockwinAct);
	//增加接口
	cyberMenu->addSeparator();
	const QIcon addConnIcon = QIcon::fromTheme("cyber-conn");
	QAction* addConnAct = new QAction(addConnIcon, tr("添加接口"), this);
	addConnAct->setStatusTip(tr("添加接口"));
	connect(addConnAct, &QAction::triggered, this, &MainWindow::defConnDlg);
	cyberMenu->addAction(addConnAct);
	//添加标记面
	cyberMenu->addSeparator();
	const QIcon addMarkingIcon = QIcon::fromTheme("cyber-marking");
	QAction* addMarkingAct = new QAction(addMarkingIcon, tr("添加标记面"), this);
	addMarkingAct->setStatusTip(tr("添加标记面"));
	connect(addMarkingAct, &QAction::triggered, this, &MainWindow::addMarkingSurfDlg);
	cyberMenu->addAction(addMarkingAct);
	//QMenu* connectorMenu = cyberMenu->addMenu(tr("添加接口"));
	////机械
	//const QIcon addMachCon = QIcon::fromTheme("cyber-conn-mach"/*, QIcon("images/redo.png")*/);
	//QAction* addMachAct = new QAction(addMachCon, tr("机械..."), this);
	//addMachAct->setStatusTip(tr("添加机械接口"));
	////addMachAct->setCheckable(true);
	//connect(addMachAct, &QAction::triggered, this, &MainWindow::addMachConn);
	//connectorMenu->addAction(addMachAct);
	////电子电气
	//const QIcon addElecCon = QIcon::fromTheme("cyber-conn-elec"/*, QIcon("images/redo.png")*/);
	//QAction* addElecAct = new QAction(addMachCon, tr("电子电气..."), this);
	//addElecAct->setStatusTip(tr("添加电子电气接口"));
	////addMachAct->setCheckable(true);
	//connect(addElecAct, &QAction::triggered, this, &MainWindow::addElecConn);
	//connectorMenu->addAction(addElecAct);
	////液压
	//const QIcon addhydrCon = QIcon::fromTheme("cyber-conn-hydr"/*, QIcon("images/redo.png")*/);
	//QAction* addhydrAct = new QAction(addhydrCon, tr("液压..."), this);
	//addhydrAct->setStatusTip(tr("添加液压接口"));
	//connect(addhydrAct, &QAction::triggered, this, &MainWindow::addHydrConn);
	//connectorMenu->addAction(addhydrAct);
	////控制
	//const QIcon addcontCon = QIcon::fromTheme("cyber-conn-contr"/*, QIcon("images/redo.png")*/);
	//QAction* addcontAct = new QAction(addcontCon, tr("控制..."), this);
	//addcontAct->setStatusTip(tr("添加控制接口"));
	//connect(addcontAct, &QAction::triggered, this, &MainWindow::addContConn);
	//connectorMenu->addAction(addcontAct);
	//基准定义  lqq
	cyberMenu->addSeparator();
	QMenu* datumMenu = cyberMenu->addMenu(tr("基准定义"));
	//基准点
	const QIcon datumPointIcon = QIcon::fromTheme("cyber-datum-point");
	QAction* datumPointAct = new QAction(datumPointIcon, tr("基准点"), this);
	datumPointAct->setStatusTip(tr("定义基准点"));
	connect(datumPointAct, &QAction::triggered, this, &MainWindow::defDatumPoint);
	datumMenu->addAction(datumPointAct);
	//基准轴
	const QIcon datumLineIcon = QIcon::fromTheme("cyber-refer-Line");
	QAction* datumLineAct = new QAction(datumLineIcon, tr("基准轴"), this);
	datumLineAct->setStatusTip(tr("定义基准轴"));
	connect(datumLineAct, &QAction::triggered, this, &MainWindow::defDatumLine);
	datumMenu->addAction(datumLineAct);

	//6. 增加系统设置菜单;
	QMenu* setMenu = menuBar()->addMenu(tr("设置"));
	//系统设置
	const QIcon sysSetIcon = QIcon::fromTheme("set-system");
	QAction* sysSetAct = new QAction(sysSetIcon, tr("系统设置"), this);
	sysSetAct->setStatusTip(tr("系统设置"));
	connect(sysSetAct, &QAction::triggered, this, &MainWindow::systemSet);
	setMenu->addAction(sysSetAct);
	//背景颜色设置
	const QIcon backColorIcon = QIcon::fromTheme("set-systemColor");
	QAction* backColorAct = new QAction(backColorIcon, tr("背景颜色设置"), this);
	backColorAct->setStatusTip(tr("背景颜色设置"));
	connect(backColorAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView(); \
		view->onBackground(); \
		}); 
	setMenu->addAction(backColorAct);

	//背景贴图
	const QIcon backTextureIcon = QIcon::fromTheme("set-systemBackFig");
	QAction* backTextureAct = new QAction(backTextureIcon, tr("背景图片设置"), this);
	backTextureAct->setStatusTip(tr("背景图片设置"));
	connect(backTextureAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView(); \
		view->onEnvironmentMap(); \
		});
	setMenu->addAction(backTextureAct);
	setMenu->addSeparator();
	//显示全局坐标系
	const QIcon showOxyzIcon = QIcon::fromTheme("set-ShowOxyz");
	showOxyzAct = new QAction(showOxyzIcon, tr("显示全局坐标系"), this);
	showOxyzAct->setStatusTip(tr("显示全局坐标系"));
	showOxyzAct->setCheckable(true);
	showOxyzAct->setChecked(false);
	connect(showOxyzAct, &QAction::triggered, this, &MainWindow::ShowOxyz);
	setMenu->addAction(showOxyzAct);


	//7. 增加帮助菜单
	QMenu* helpMenu = menuBar()->addMenu(tr("帮助"));
	QAction* aboutAct = helpMenu->addAction(tr("关于（&A）"), this, &MainWindow::about);
	aboutAct->setStatusTip(tr("显示关于对话框"));
	helpMenu->addSeparator();
	helpMenu->addAction(tr("长方体"), this, &MainWindow::MakeBox);
	helpMenu->addAction(tr("弹簧"), this, &MainWindow::MakeSpring);
	helpMenu->addAction(tr("输出Brep"), this, &MainWindow::ExportBREP);

	//8.关联工具条
	//8.1文件工具条: 新建、打开、保存、输入即可，其余不要
	fileToolBar->addAction(newAct);
	fileToolBar->addAction(openAct);
	fileToolBar->addAction(saveAct);
	//fileToolBar->addAction(saveasAct);
	fileToolBar->addAction(importAct);
	///插入实体操作工具条：对选中实体线框/面显示、Color，Material，Tranparent隐藏删除取消删除等
	QToolBar* objEditToolBar = addToolBar(tr("objEdit"));
	//--1.颜色
	const QIcon colorIcon = QIcon::fromTheme("objEdit-color", QIcon("images/tool_color.png"));
	colorAct = new QAction(colorIcon, tr("颜色"), this);
	//colorAct->setShortcuts(QKeySequence::Undo);
	colorAct->setStatusTip(tr("设置对象颜色"));
	connect(colorAct, &QAction::triggered, this, &MainWindow::SetColor);
	//colorAct->setEnabled(false);
	objEditToolBar->addAction(colorAct);
	//--2.材料
	const QIcon matIcon = QIcon::fromTheme("objEdit-material", QIcon("images/tool_material.png"));
	QAction* matAct = new QAction(matIcon, tr("材料"), this);
	matAct->setStatusTip(tr("设置对象材料"));
	connect(matAct, &QAction::triggered, this, &MainWindow::SetMaterial);
	objEditToolBar->addAction(matAct);

	//8.2 编辑工具条：撤销undo，重做redo,选择select
	QToolBar* editToolBar = addToolBar(tr("edit"));
	editToolBar->addAction(undoAct);
	editToolBar->addAction(redoAct);

	////删除工具条
	//subDelmenu = new QMenu("删除",this);
	//subDelmenu->addAction(delConnAct);
	////subPartDelmenu->addAction(delEdgeAct);
	////subDelmenu->addMenu(subPartDelmenu);
	//subDelmenu->addAction(delCompAct);
	//editToolBar->addAction(subDelmenu->menuAction());

	subSelmenu = new QMenu("选择", this);
	//submenu->setIcon(genIcon(iconSize(), 1, Qt::black, dpr));
	subSelmenu->addAction(selAnyAct);// (genIcon(iconSize(), "A", Qt::blue, dpr), "A");
	subSelmenu->addAction(selVtxAct);
	subSelmenu->addAction(selEdgeAct);
	subSelmenu->addAction(selFaceAct);
	subSelmenu->addAction(selBodyAct);
	subSelmenu->addAction(selConnectorAct);
	editToolBar->addAction(subSelmenu->menuAction());
	//8.3 约束工具条
	QToolBar* consToolBar = addToolBar(tr("constraint"));
	consToolBar->addAction(showConsDockwinAct);
	consToolBar->addAction(showVoxelDockwinAct);
	//8.4 信息工具条
	QToolBar* cyberToolBar = addToolBar(tr("cyber"));
	cyberToolBar->addAction(showCyberDockwinAct);
	cyberToolBar->addAction(addConnAct);
	cyberToolBar->addAction(addMarkingAct);
	//QMenu* submenuConn = new QMenu("添加接口", this);
	//submenuConn->addAction(addMachAct);// (genIcon(iconSize(), "A", Qt::blue, dpr), "A");
	//submenuConn->addAction(addElecAct);
	//submenuConn->addAction(addhydrAct);
	//submenuConn->addAction(addcontAct);
	//cyberToolBar->addAction(submenuConn->menuAction());
	//基准定义工具条 lqq
	QMenu* submenuDatum = new QMenu("基准定义", this);
	submenuDatum->addAction(datumPointAct);
	submenuDatum->addAction(datumLineAct);
	cyberToolBar->addAction(submenuDatum->menuAction());

	//9.关联窗口
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
	//文档创建以后才能建立连接
	//connect(colorAct, &QAction::triggered, this->myDocument3d, &DocumentCommon::objColor);

	//启动modelica库的加载
	vector<string> vLibPaths;
	vLibPaths = LibPathsSetDlg::GetModelLibPaths();//从设置中获取
	//测试
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
	//int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // 获取标题栏高度
	//int diaWidth = sysDlg->width();
	//int diaHeight = sysDlg->height();
	//QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(-800, 0));//设置对话框位置
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
	int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // 获取标题栏高度
	int diaWidth = dconnDlg->width();
	int diaHeight = dconnDlg->height();
	QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//设置对话框位置
	dconnDlg->setGeometry(globalPos.x() + this->width() - diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
	dconnDlg->show();
}

void MainWindow::addMarkingSurfDlg()
{
	MarkingSurfDlg* markDlg = new MarkingSurfDlg(this);
	int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // 获取标题栏高度
	int diaWidth = markDlg->width();
	int diaHeight = markDlg->height();
	QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//设置对话框位置
	markDlg->setGeometry(globalPos.x() + this->width() - 2.5 * diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
	markDlg->show();
}

//lqq
void MainWindow::defDatumPoint()
{
	DatumPointDlg* dptDlg = new DatumPointDlg(this);
	int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // 获取标题栏高度
	int diaWidth = dptDlg->width();
	int diaHeight = dptDlg->height();
	QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//设置对话框位置
	dptDlg->setGeometry(globalPos.x() + this->width() - diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
	dptDlg->show();
}

void MainWindow::defDatumLine()
{
	DatumLineDlg* dliDlg = new DatumLineDlg(this);
	int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // 获取标题栏高度
	int diaWidth = dliDlg->width();
	int diaHeight = dliDlg->height();
	QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//设置对话框位置
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

//体素建模与几何重构
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
	//对其余的check去掉
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(true);

	//改变toolbar的显示
	subSelmenu->setTitle("任一");

	//设置OCC的选择
	myDocument3d->setSelectMode(tr("任一"));
}

void MainWindow::selectVertex()
{
	//对其余的check去掉
	selVtxAct->setChecked(true);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(false);

	//改变toolbar的显示
	subSelmenu->setTitle("顶点");

	//设置OCC的选择
	myDocument3d->setSelectMode(tr("顶点"));
}

void MainWindow::selectEdge()
{
	//对其余的check去掉
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(true);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(false);

	//改变toolbar的显示
	subSelmenu->setTitle("棱边");

	//设置OCC的选择
	myDocument3d->setSelectMode(tr("棱边"));
}

void MainWindow::selectFace()
{
	//对其余的check去掉
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(true);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(false);

	//改变toolbar的显示
	subSelmenu->setTitle("面");

	//设置OCC的选择
	myDocument3d->setSelectMode(tr("面"));
}

void MainWindow::selectBody()
{
	//对其余的check去掉
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(true);
	selConnectorAct->setChecked(false);
	selAnyAct->setChecked(false);

	//改变toolbar的显示
	subSelmenu->setTitle("体");

	//设置OCC的选择
	myDocument3d->setSelectMode(tr("体"));
}

void MainWindow::selectConnector()
{
	//对其余的check去掉
	selVtxAct->setChecked(false);
	selEdgeAct->setChecked(false);
	selFaceAct->setChecked(false);
	selBodyAct->setChecked(false);
	selConnectorAct->setChecked(true);
	selAnyAct->setChecked(false);

	//改变toolbar的显示
	subSelmenu->setTitle("接口");

	//设置OCC的选择
	myDocument3d->setSelectMode(tr("接口"));
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

//纹理贴图
void MainWindow::Texture()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		DocumentComponent* pComDoc = (DocumentComponent*)myDocument3d;
		pComDoc->Texture();
	}
}

//整体放缩
void MainWindow::Scaling()
{
	if (myDocument3d && !myDocument3d->IsModel())
	{
		//对零件整体缩放
		DocumentComponent* pComDoc = (DocumentComponent*)myDocument3d;
		Graphic3d_NameOfMaterial mat;
		TopoDS_Shape mainSolid = pComDoc->GetMainSolid(mat);

		bool ok = false;
		double scale = QInputDialog::getDouble(this, tr("整体形状缩放"), tr("输入比例"), 1, 1e-6, 1e6, 3, &ok);
		if (ok)
		{
			gp_Trsf transf;
			GProp_GProps aGProps;
			BRepGProp::VolumeProperties(mainSolid, aGProps);
			//质心坐标
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
		//对接口缩放
		DocumentComponent* pComDoc = (DocumentComponent*)myDocument3d;
		bool ok = false;
		double scale = QInputDialog::getDouble(this, tr("接口缩放"), tr("输入比例"), 1, 1e-6, 1e6, 3, &ok);
		if (ok)
		{
			vector<Connector*> vConntrs = pComDoc->GetAllConnectors();
			for (int i = 0; i < vConntrs.size(); i++)
			{
				Connector* conn = vConntrs[i];
				TopoDS_Shape shp_i = conn->GetShape();
				//加个比例缩放
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
	msgBox.setText("只能选择参考实体或接口进行删除!");
	msgBox.exec();
}

//如果当前有编辑的文档，则新建进程后再new系统建模文档
void MainWindow::CreateMdlInterface()
{
	//测试
	//QMessageBox::information(this, "提示", "系统建模", QMessageBox::Ok);

	//关联菜单
	//CreateMdlMenus();
	//1.先删去全部菜单
	menuBar()->clear();

	//删除工具条
	removeToolBar(fileToolBar);

	//2.添加文件菜单及其动作
	fileMenu = menuBar()->addMenu(tr("文件"));
	//不需要工具条
	//fileToolBar = addToolBar(tr("File"));

	//2.1新建
	const QIcon newIcon = QIcon::fromTheme("document-new", QIcon("images/new.png"));
	QAction* newAct = new QAction(newIcon, tr("新建..."), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("新建文档"));
	//关联
	connect(newAct, &QAction::triggered, this, &MainWindow::newDoc);
	fileMenu->addAction(newAct);

	//2.2打开
	const QIcon openIcon = QIcon::fromTheme("document-open", QIcon("images/open.png"));
	QAction* openAct = new QAction(openIcon, tr("打开..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("打开文档"));
	//关联
	connect(openAct, &QAction::triggered, this, &MainWindow::open);
	fileMenu->addAction(openAct);

	//2.3保存
	const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon("images/save.png"));
	QAction* saveAct = new QAction(saveIcon, tr("保存"), this);
	saveAct->setShortcuts(QKeySequence::Save);
	saveAct->setStatusTip(tr("保存文档"));
	//关联
	connect(saveAct, &QAction::triggered, this, &MainWindow::save);
	fileMenu->addAction(saveAct);

	//2.4另存
	const QIcon saveasIcon = QIcon::fromTheme("document-save-as", QIcon("images/saveas.png"));
	QAction* saveasAct = new QAction(saveasIcon, tr("另存..."), this);
	saveasAct->setShortcuts(QKeySequence::SaveAs);
	saveasAct->setStatusTip(tr("文档另存为"));
    //关联
	connect(saveasAct, &QAction::triggered, this, &MainWindow::saveAs);
	fileMenu->addAction(saveasAct);

	//分隔符
	//2.5最近文档
	fileMenu->addSeparator();
	QMenu* recentMenu = fileMenu->addMenu(tr("最近文档"));
	//关联
	recentFileSubMenuAct = recentMenu->menuAction();
	for (int i = 0; i < MaxRecentFiles; ++i) {
		recentFileActs[i] = recentMenu->addAction(QString(), this, &MainWindow::openRecentFile);
		recentFileActs[i]->setVisible(false);
	}
	recentFileSeparator = fileMenu->addSeparator();
	setRecentFilesVisible(MainWindow::hasRecentFiles());
	fileMenu->addSeparator();
	//分隔符
	//2.6 退出
	const QIcon exitIcon = QIcon::fromTheme("application-exit");
	QAction* exitAct = fileMenu->addAction(exitIcon, tr("退出"), qApp, &QApplication::closeAllWindows);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("退出"));
	fileMenu->addAction(exitAct);

	//3.增加 编辑 菜单
	QMenu* editMenu = menuBar()->addMenu(tr("编辑"));
	//3.1 undo
	const QIcon undoIcon = QIcon::fromTheme("edit-undo", QIcon("images/undo.png"));
	undoAct = new QAction(undoIcon, tr("撤销"), this);
	undoAct->setShortcuts(QKeySequence::Undo);
	undoAct->setStatusTip(tr("撤销上一次操作"));
	//关联
	editMenu->addAction(undoAct);
	undoAct->setEnabled(false);

	//3.2 redo
	const QIcon redoIcon = QIcon::fromTheme("edit-redo", QIcon("images/redo.png"));
	redoAct = new QAction(redoIcon, tr("重做"), this);
	redoAct->setShortcuts(QKeySequence::Redo);
	redoAct->setStatusTip(tr("恢复上一次操作"));
	connect(redoAct, &QAction::triggered, this, &MainWindow::redo);
	editMenu->addAction(redoAct);
	redoAct->setEnabled(false);

	//3.3 选择：各种实体类型
	editMenu->addSeparator();
	QMenu* selectMenu = editMenu->addMenu(tr("选择实体类型"));
	//顶点
	const QIcon selVtxIcon = QIcon::fromTheme("edit-select-vertex");
	selVtxAct = new QAction(selVtxIcon, tr("顶点"), this);
	selVtxAct->setStatusTip(tr("选择顶点类型"));
	selVtxAct->setCheckable(true);
	connect(selVtxAct, &QAction::triggered, this, &MainWindow::selectVertex);
	selectMenu->addAction(selVtxAct);
	//边
	const QIcon selEdgeIcon = QIcon::fromTheme("edit-select-edge");
	selEdgeAct = new QAction(selEdgeIcon, tr("棱边"), this);
	selEdgeAct->setStatusTip(tr("选择棱边类型"));
	selEdgeAct->setCheckable(true);
	connect(selEdgeAct, &QAction::triggered, this, &MainWindow::selectEdge);
	selectMenu->addAction(selEdgeAct);
	//面
	const QIcon selFaceIcon = QIcon::fromTheme("edit-select-face");
	selFaceAct = new QAction(selFaceIcon, tr("面"), this);
	selFaceAct->setStatusTip(tr("选择面类型"));
	selFaceAct->setCheckable(true);
	connect(selFaceAct, &QAction::triggered, this, &MainWindow::selectFace);
	selectMenu->addAction(selFaceAct);
	//体
	const QIcon selBodyIcon = QIcon::fromTheme("edit-select-body");
	selBodyAct = new QAction(selBodyIcon, tr("体"), this);
	selBodyAct->setStatusTip(tr("选择体类型"));
	selBodyAct->setCheckable(true);
	connect(selBodyAct, &QAction::triggered, this, &MainWindow::selectBody);
	selectMenu->addAction(selBodyAct);
	//接口
	const QIcon selConnectorIcon = QIcon::fromTheme("edit-select-connector");
	selConnectorAct = new QAction(selConnectorIcon, tr("接口"), this);
	selConnectorAct->setStatusTip(tr("选择接口类型"));
	connect(selConnectorAct, &QAction::triggered, this, &MainWindow::selectConnector);
	selConnectorAct->setCheckable(true);
	selectMenu->addAction(selConnectorAct);
	//任一
	const QIcon selAnyIcon = QIcon::fromTheme("edit-select-any");
	selAnyAct = new QAction(selAnyIcon, tr("任一"), this);
	selAnyAct->setStatusTip(tr("选择任一类型"));
	selAnyAct->setCheckable(true);
	selAnyAct->setChecked(true);
	connect(selAnyAct, &QAction::triggered, this, &MainWindow::selectAny);
	selectMenu->addAction(selAnyAct);

	//3.4旋转
	editMenu->addSeparator();
	const QIcon rotaIcon = QIcon::fromTheme("rotate", QIcon("images/cursor_rotate"));
	rotaAct = new QAction(rotaIcon, tr("旋转"), this);
	rotaAct->setStatusTip(tr("旋转"));
	//关联
	editMenu->addAction(rotaAct);

	//3.5平移
	const QIcon panIcon = QIcon::fromTheme("pan"/*,QIcon("images/")*/);
	panAct = new QAction(panIcon, tr("平移"), this);
	panAct->setStatusTip(tr("平移"));
	//关联
	editMenu->addAction(panAct);

	//3.6放大
	const QIcon zoominIcon = QIcon::fromTheme("zoom-in", QIcon("images/cursor_zoom"));
	zoominAct = new QAction(zoominIcon, tr("放大"), this);
	zoominAct->setStatusTip(tr("放大"));
	//关联
	editMenu->addAction(zoominAct);

	//3.7缩小
	const QIcon zoomoutIcon = QIcon::fromTheme("zoom-out", QIcon("images/cursor_zoom"));
	zoomoutAct = new QAction(zoomoutIcon, tr("缩小"), this);
	zoomoutAct->setStatusTip(tr("缩小"));
	//关联
	editMenu->addAction(zoomoutAct);

	//3.8添加
	editMenu->addSeparator();
	const QIcon addIcon = QIcon::fromTheme("add"/*,QIcon("images")*/);
	addAct = new QAction(addIcon, tr("添加"), this);
	addAct->setStatusTip(tr("添加"));
	//关联
	editMenu->addAction(addAct);

	//3.9删除
	const QIcon delIcon = QIcon::fromTheme("delete", QIcon("images/tool_delete"));
	delAct = new QAction(delIcon, tr("删除"), this);
	delAct->setStatusTip(tr("删除"));
	//关联
	editMenu->addAction(delAct);

	//4.增加 连接 菜单
	QMenu* connectMenu = menuBar()->addMenu(tr("连接"));
	//建立连接
	const QIcon estabconnectIcon = QIcon::fromTheme("connection-establish");
	QAction* estabconnectAct = new QAction(estabconnectIcon, tr("建立连接"), this);
	estabconnectAct->setStatusTip(tr("建立连接"));
	//关联
	connectMenu->addAction(estabconnectAct);
	//---------------
	connectMenu->addSeparator();
	//连接器
	QMenu* connector = connectMenu->addMenu(tr("连接器"));
	//运动副
	const QIcon kenimaticIcon = QIcon::fromTheme("kenimaticIcon-pair");
	QAction* kenimaticAct = new QAction(kenimaticIcon, tr("运动副"), this);
	kenimaticAct->setStatusTip(tr("运动副"));
	kenimaticAct->setCheckable(true);
	//关联
	connector->addAction(kenimaticAct);
	//控制总线
	const QIcon controlBUSIcon = QIcon::fromTheme("control-BUS");
	QAction* controlBUSAct = new QAction(controlBUSIcon, tr("控制总线BUS"), this);
	controlBUSAct->setStatusTip(tr("控制总线"));
	controlBUSAct->setCheckable(true);
	//关联
	connector->addAction(controlBUSAct);

	//5.仿真
	QMenu* simulationMenu = menuBar()->addMenu(tr("仿真"));
	//生成代码并检查
	const QIcon generatecodeIcon = QIcon::fromTheme("code-generation");
	QAction* generatecodeAct = new QAction(generatecodeIcon, tr("模型检查"), this);
	generatecodeAct->setStatusTip(tr("模型检查"));
	//关联
	simulationMenu->addAction(generatecodeAct);
	//仿真求解
	const QIcon simulationAndsolveIcon = QIcon::fromTheme("simulation-solve");
	simulationAndsolveAct = new QAction(simulationAndsolveIcon, tr("仿真求解"), this);
	simulationAndsolveAct->setStatusTip(tr("仿真求解"));
	simulationAndsolveAct->setCheckable(true);
	simulationAndsolveAct->setChecked(false);
	simulationAndsolveAct->setText(tr("仿真求解"));
	//关联
	simulationMenu->addAction(simulationAndsolveAct);
	//数字孪生
	const QIcon digitalTwinsIcon = QIcon::fromTheme("digital-twins");
	digitalTwinsAct = new QAction(digitalTwinsIcon, tr("孪生驱动"), this);
	digitalTwinsAct->setStatusTip(tr("孪生驱动"));
	digitalTwinsAct->setCheckable(true);
	digitalTwinsAct->setChecked(false);
	digitalTwinsAct->setText(tr("孪生驱动"));
	//关联
	simulationMenu->addAction(digitalTwinsAct);
	// 神经网络构建
	const QIcon neuralNetworkIcon = QIcon::fromTheme("neural-network");
	neuralNetworkAct = new QAction(digitalTwinsIcon, tr("神经网络构建"), this);
	neuralNetworkAct->setStatusTip(tr("神经网络构建"));
	neuralNetworkAct->setCheckable(true);
	neuralNetworkAct->setChecked(false);
	neuralNetworkAct->setText(tr("神经网络构建"));
	//关联
	simulationMenu->addAction(neuralNetworkAct);
	////动画驱动
	//const QIcon AnimationIcon = QIcon::fromTheme("animation-driven");
	//QAction* AnimationAct = new QAction(AnimationIcon, tr("动画驱动"), this);
	//AnimationAct->setStatusTip(tr("动画驱动"));
	////关联
	//simulationMenu->addAction(AnimationAct);

	//6. 增加系统设置菜单;
	QMenu* setMenu = menuBar()->addMenu(tr("设置"));
	//系统设置
	const QIcon sysSetIcon = QIcon::fromTheme("set-system");
	QAction* sysSetAct = new QAction(sysSetIcon, tr("系统设置"), this);
	sysSetAct->setStatusTip(tr("系统设置"));
	connect(sysSetAct, &QAction::triggered, this, &MainWindow::systemSet);
	setMenu->addAction(sysSetAct);
	//背景颜色设置
	const QIcon backColorIcon = QIcon::fromTheme("set-systemColor");
	QAction* backColorAct = new QAction(backColorIcon, tr("背景颜色设置"), this);
	backColorAct->setStatusTip(tr("背景颜色设置"));
	connect(backColorAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView(); \
		view->onBackground(); \
		});
	setMenu->addAction(backColorAct);

	//背景贴图
	const QIcon backTextureIcon = QIcon::fromTheme("set-systemBackFig");
	QAction* backTextureAct = new QAction(backTextureIcon, tr("背景图片设置"), this);
	backTextureAct->setStatusTip(tr("背景图片设置"));
	connect(backTextureAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView(); \
		view->onEnvironmentMap(); \
		});
	setMenu->addAction(backTextureAct);
	setMenu->addSeparator();
	//显示全局坐标系
	const QIcon showOxyzIcon = QIcon::fromTheme("set-ShowOxyz");
	showOxyzAct = new QAction(showOxyzIcon, tr("显示全局坐标系"), this);
	showOxyzAct->setStatusTip(tr("显示全局坐标系"));
	showOxyzAct->setCheckable(true);
	showOxyzAct->setChecked(false);
	connect(showOxyzAct, &QAction::triggered, this, &MainWindow::ShowOxyz);
	setMenu->addAction(showOxyzAct);

	//7. 增加帮助菜单
	QMenu* helpMenu = menuBar()->addMenu(tr("帮助"));
	QAction* aboutAct = helpMenu->addAction(tr("关于（&A）"), this, &MainWindow::about);
	aboutAct->setStatusTip(tr("显示关于对话框"));


	//关联工具条
	//工具条的内容：插入、装配、连接、调姿、检查、仿真
	//8.1插入操作工具条
	QToolBar* MdleditToolBar = addToolBar(tr("edit"));
	QAction* insertAct = new QAction(tr("插入"), this);
	insertAct->setStatusTip(tr("插入"));
	connect(insertAct, &QAction::triggered, this, &MainWindow::insert);
	MdleditToolBar->addAction(insertAct);//关联

	//组件属性
	QAction* attributeAct = new QAction(tr("组件属性"), this);
	attributeAct->setStatusTip(tr("组件属性"));
	connect(attributeAct, &QAction::triggered, this, &MainWindow::attributeSetting);
	MdleditToolBar->addAction(attributeAct);

	//select
	subSelmenu = new QMenu("选择", this);
	//submenu->setIcon(genIcon(iconSize(), 1, Qt::black, dpr));
	subSelmenu->addAction(selAnyAct);// (genIcon(iconSize(), "A", Qt::blue, dpr), "A");
	subSelmenu->addAction(selVtxAct);
	subSelmenu->addAction(selEdgeAct);
	subSelmenu->addAction(selFaceAct);
	subSelmenu->addAction(selBodyAct);
	subSelmenu->addAction(selConnectorAct);
	MdleditToolBar->addAction(subSelmenu->menuAction());

	//8.2装配工具条
	QToolBar* assemblyToolBar = addToolBar(tr("assembly"));
	QAction* assemblyAct = new QAction(tr("装配"), this);
	assemblyAct->setStatusTip(tr("装配"));
	/*connect(assemblyAct, &QAction::triggered, this, &MainWindow::assembly);*/
	connect(assemblyAct, &QAction::triggered, [&]() {
		myGeomWidget->GetView()->NewAssembleCommand();
	});
	assemblyToolBar->addAction(assemblyAct);

	//连接
	QAction* ConnectAct = new QAction(tr("连接"), this);
	ConnectAct->setStatusTip(tr("连接"));
	/*connect(ConnectAct, &QAction::triggered, this, &MainWindow::connection);*/
	connect(ConnectAct, &QAction::triggered, [&]() {
		View* view = myGeomWidget->GetView();
		view->NewConnectCommand();
	});
	assemblyToolBar->addAction(ConnectAct);

	//调姿
	QAction* postureAct = new QAction(tr("调姿"), this);
	postureAct->setStatusTip(tr("调姿"));
	//connect(postureAct, &QAction::triggered, this, &MainWindow::adjust);
	//关联
	connect(postureAct, &QAction::triggered, [&]() {\
		View* view = myGeomWidget->GetView();\
		view->NewRepostureCommand();\
	});
	assemblyToolBar->addAction(postureAct);

	//8.3仿真工具条
	QToolBar* simulationToolBar = addToolBar(tr("simulation"));
	//参数设置
	QAction* ParaAct = new QAction(tr("参数设置"), this);
	ParaAct->setStatusTip(tr("参数设置"));
	connect(ParaAct, &QAction::triggered, this, &MainWindow::setpara);
	simulationMenu->addAction(ParaAct);
	simulationToolBar->addAction(ParaAct);
	//仿真设置
	QAction* SimulateSetAct = new QAction(tr("仿真设置"), this);
	SimulateSetAct->setStatusTip(tr("仿真设置"));
	connect(SimulateSetAct, &QAction::triggered, this, &MainWindow::setsimulate);
	simulationMenu->addAction(SimulateSetAct);
	//代码生成与模型检查
	connect(generatecodeAct, &QAction::triggered, this, &MainWindow::modelCheck);
	simulationToolBar->addAction(generatecodeAct);
	//仿真求解
	connect(simulationAndsolveAct, &QAction::triggered, this, &MainWindow::simulation);
	simulationToolBar->addAction(simulationAndsolveAct);
	//数字孪生
	connect(digitalTwinsAct, &QAction::triggered, this, &MainWindow::digitaltwins);
	simulationToolBar->addAction(digitalTwinsAct);
	//神经网络构建
	connect(neuralNetworkAct, &QAction::triggered, this, &MainWindow::neuralnetwork);
	simulationToolBar->addAction(neuralNetworkAct);
	//后处理工具条
	postProcessToolBar = addToolBar(tr("postProcess"));
	//动画
	QAction* animationAct = new QAction(tr("动画"), this);
	animationAct->setStatusTip(tr("运动模拟"));
	connect(animationAct, &QAction::triggered, this, &MainWindow::animation);
	//simulationMenu->addAction(animationAct);
	postProcessToolBar->addAction(animationAct);
	postProcessToolBar->hide();

	//关联窗口
	//在对myGeomWidget进行析构时出现问题
	QFrame* aViewFrame = new QFrame(this);
	aViewFrame->setFrameStyle(QFrame::Panel | QFrame::Sunken);
	aViewFrame->setLineWidth(3);

	//使用布局无法完成既定要求，使用分裂窗口
	//主分裂窗口
	//QSplitter* 
	mainSplitter = new QSplitter(Qt::Horizontal);
	//左边分裂窗口
	QSplitter* frameSplitter = new QSplitter(Qt::Vertical);
	frameSplitter->setOpaqueResize(true);
	frameSplitter->setLineWidth(5);

	myDocument3d = new DocumentModel(this);
	//myDocument3d->setDocType(nDocType);

	//左边分裂器添加
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
	//打开文档时，如果当前文档为空，则在当前进程打开
	//否则，新建进程，相当于执行 M3dStudio filename
	const QString fileName = QFileDialog::getOpenFileName(this, QString(), QString(), "模型文件(*.m3dcom *.m3dmdl);;零件(*.m3dcom);;系统(*.m3dmdl)");
	if (!fileName.isEmpty())
		openFile(fileName);
}

bool MainWindow::openFile(const QString& fileName)
{
	//如果当前有文档打开，则新进进程;
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

	//int fileType = 0; //缺省为零件
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
		QString sTitle = tr("M3d建模与仿真一体化平台——");
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

//导入几何
void MainWindow::import()
{
	if (!myDocument3d->IsEmpty())
	{
		//导入之前清除
		if (QMessageBox::question(this, "提示", "导入前要清除当前几何和文档相关内容，是否继续？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
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
		//将导入的几何形状存入文档
		//myDocument3d->serializeShapes(shapes);
		if (shapes->Length() > 1)
		{//如果形状数量大于1，则只取第一个
			QMessageBox::information(this, "警告", "导入的形体个数大于1，取第一个！", QMessageBox::Ok);
		}
		TopoDS_Shape aShape = shapes->Value(1);
		//myDocument3d->SaveShapes(shapes);
		//将主实体存入文档
		myDocument3d->SetMainShape(aShape);
		//myDocument3d->ShowShapes(shapes);
		//根据文档更新显示
		myDocument3d->UpdateDisplay();

		myGeomWidget->FitAll();
		updateMenus();
	}

	delete anTrans;
}

////输出几何
//void MainWindow::export()
//{
//    //暂时不实现
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
	//存储位图
	QPixmap pm;
	//pm = QPixmap::grabWidget(this/*myGeomWidget*/);
	pm = QPixmap::grabWindow(0);
	//pm.save("temp.png", "PNG");
	//pm.load("D:/M3drepos/M3d/M3dStudio/images/import.png");
	if (myDocument3d)
	{
		myDocument3d->SavePixmap(pm);
	}

	//保存当前文档，根据当前文档类型;
	if (myDocument3d && myDocument3d->save())
	{
		statusBar()->showMessage(tr("文件保存成功."), 2000);
	}
}

void MainWindow::saveAs()
{
	//存储位图
	QPixmap pm;
	pm.grabWidget(myGeomWidget);
	if (myDocument3d)
	{
		myDocument3d->SavePixmap(pm);
	}

	//另存为与新建或打开相同类型的文档
	if (myDocument3d && myDocument3d->saveAs())
		statusBar()->showMessage(tr("文件另存成功."), 2000);
}

void MainWindow::about()
{
	QMessageBox::about(this, tr("关于M3d"),
		tr("基于Modelica的三维建模-系统仿真一体化平台\n华中科技大学版权所有"));
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

		//undo redo的使能
	if (undoAct)
	{
		undoAct->setEnabled(myDocument3d->canUndo());
	}
	if (redoAct)
	{
		redoAct->setEnabled(myDocument3d->canRedo());
	}

	//对象颜色
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
	fileMenu = menuBar()->addMenu(tr("文件"));
	fileToolBar = addToolBar(tr("File"));

	const QIcon newIcon = QIcon::fromTheme("document-new", QIcon("images/new.png"));
	newAct = new QAction(newIcon, tr("新建..."), this);
	newAct->setShortcuts(QKeySequence::New);
	newAct->setStatusTip(tr("新建文档"));
	connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
	fileMenu->addAction(newAct);
	fileToolBar->addAction(newAct);

	const QIcon openIcon = QIcon::fromTheme("document-open", QIcon("images/open.png"));
	QAction* openAct = new QAction(openIcon, tr("打开..."), this);
	openAct->setShortcuts(QKeySequence::Open);
	openAct->setStatusTip(tr("打开文档"));
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

	QMenu* recentMenu = fileMenu->addMenu(tr("最近文档"));
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
	QAction* exitAct = fileMenu->addAction(exitIcon, tr("退出"), qApp, &QApplication::closeAllWindows);
	exitAct->setShortcuts(QKeySequence::Quit);
	exitAct->setStatusTip(tr("退出"));
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

	QMenu* helpMenu = menuBar()->addMenu(tr("帮助（&H）"));

	QAction* aboutAct = helpMenu->addAction(tr("关于（&A）"), this, &MainWindow::about);
	aboutAct->setStatusTip(tr("显示关于对话框"));

}

void MainWindow::createStatusBar()
{
	statusBar()->showMessage(tr("准备好"));
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
		int iTitleBarHeight = style()->pixelMetric(QStyle::PM_TitleBarHeight) + 12;   // 获取标题栏高度
		int diaWidth = matDlg->width();
		int diaHeight = matDlg->height();
		QPoint globalPos = myGeomWidget->mapToGlobal(QPoint(0, 0));//设置对话框位置
		matDlg->setGeometry(globalPos.x() + this->width() - diaWidth, globalPos.y() + fileToolBar->height() + iTitleBarHeight, diaWidth, diaHeight);
		matDlg->show();
	}
	updateMenus();
}

void MainWindow::readSettings()
{
	//读入窗口设置
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
	//保存窗口设置
	QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	settings.setValue("geometry", saveGeometry());
}

//测试函数
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
/// 插入部件
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

//装配两个组件
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

//建立连线
void MainWindow::connection()
{
	if (myDocument3d && myDocument3d->IsModel())
	{
		((DocumentModel*)myDocument3d)->Connect2Components();
	}
}

//调姿
void MainWindow::adjust()
{
	if (myDocument3d && myDocument3d->IsModel())
	{
		((DocumentModel*)myDocument3d)->adjustComponent();
	}
}

//参数设置
void MainWindow::setpara()
{
	if (m_ParaDock!=nullptr&&m_ParaDock->getWidget()) return;
	//static int ticks = 1;
	QAction* act = qobject_cast<QAction*>(sender());
	m_ParaDock = new ParaDockWidget(this, act);
	addDockWidget(Qt::RightDockWidgetArea, m_ParaDock->getWidget());

}

//仿真设置
void MainWindow::setsimulate()
{
	SimulateSetDlg* simulatesetDlg = new SimulateSetDlg(this);
	simulatesetDlg->show();
}

//模型生成与检查
void MainWindow::modelCheck()
{
	//测试
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
		simulationAndsolveAct->setText("仿真求解");

		//关闭模型仿真
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

		//如果这时孪生驱动打开了，则关闭它
		if (digitalTwinsAct->isChecked())
		{
			digitalTwinsAct->setChecked(false);
			digitalTwinsAct->setText("孪生驱动");
			//关闭模型驱动
			if (myDocument3d && myDocument3d->IsModel())
			{
				//感觉可以通用的
				((DocumentModel*)myDocument3d)->CloseSimulate();
			}
			m_DigitaltwinsWidget->hide();
		}
		
		simulationAndsolveAct->setText("退出仿真");

		//模型仿真
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
		digitalTwinsAct->setText("孪生驱动");

		//关闭模型驱动
		if (myDocument3d && myDocument3d->IsModel())
		{
			//感觉可以通用的
			((DocumentModel*)myDocument3d)->CloseSimulate();
		}
		m_DigitaltwinsWidget->hide();
		m_pModelLibWidget->show();
		m_pAssemblyTreeWidget->show();
	}
	else {
		//如果这时模型仿真打开了，则关闭它
		if (simulationAndsolveAct->isChecked())
		{
			simulationAndsolveAct->setChecked(false);
			simulationAndsolveAct->setText("仿真求解");
			//关闭模型仿真
			if (myDocument3d && myDocument3d->IsModel())
			{
				((DocumentModel*)myDocument3d)->CloseSimulate();
			}
			m_AnimationWidget->hide();
		}

		digitalTwinsAct->setText("退出孪生");

		//孪生驱动模型
		if (myDocument3d && myDocument3d->IsModel())
		{
			//待写
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
		neuralNetworkAct->setText("神经网络构建");

		m_NeuralnetworkWidget->hide();
		m_pModelLibWidget->show();
		m_pAssemblyTreeWidget->show();
	}
	else {
		//如果这时模型仿真打开了，则关闭它
		if (simulationAndsolveAct->isChecked())
		{
			simulationAndsolveAct->setChecked(false);
			simulationAndsolveAct->setText("仿真求解");
			//关闭模型仿真
			if (myDocument3d && myDocument3d->IsModel())
			{
				((DocumentModel*)myDocument3d)->CloseSimulate();
			}
			m_AnimationWidget->hide();
		}

		//如果这时孪生驱动打开了，则关闭它
		if (digitalTwinsAct->isChecked())
		{
			digitalTwinsAct->setChecked(false);
			digitalTwinsAct->setText("孪生驱动");
			//关闭模型驱动
			if (myDocument3d && myDocument3d->IsModel())
			{
				//感觉可以通用的
				((DocumentModel*)myDocument3d)->CloseSimulate();
			}
			m_DigitaltwinsWidget->hide();
		}

		neuralNetworkAct->setText("退出训练");


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

//动画显示
void MainWindow::animation()
{
	//如果在动画执行过程中，则退出
	//启动动画线程
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
	if (QMessageBox::question(this, "退出系统", "退出系统，当前文档发生改变，需要保存吗？", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes) == QMessageBox::Yes)
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
			QMessageBox::StandardButton sb = QMessageBox::question(this, "退出系统", "退出系统，当前文档发生改变，需要保存吗？", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
			if (sb == QMessageBox::Yes)
			{
				save();
				event->accept();
			}
			else if (sb == QMessageBox::No)
			{
				event->accept();
			}
			else {//取消
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
