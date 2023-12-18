/****************************************************************************
**Wu YZ 2021.09.19
****************************************************************************/
#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QKeyEvent>
QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QMdiArea;
class QMdiSubWindow;
QT_END_NAMESPACE

class DocumentCommon;
class GeomWidget;
class DatumPointDlg;
class DatumLineDlg;
class ConnectorDlg;
class AssemblyTreeWidget;
class ModelLibWidget;
class ConsDockWidget;
class CyberInfoDockWidget;
class voxelWidget;

class QSplitter;
class ParaDockWidget;
class ComAttriDlg;
class LibLoadThread;
class AnimationWidget;
class DigitaltwinsWidget;
class NeuralnetworkWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    void newFile(int type = 0);
    bool openFile(const QString &fileName);
    bool saveFile(const QString& fileName);
    DocumentCommon* getDocument();
    GeomWidget* getGeomWidget () const;
    void insertCom(const QString& fileName = "");
    //����װ����
    void UpdateAssemblyTree();

protected:
    void closeEvent(QCloseEvent *event) override;
    
private slots:
    void newDoc();
    void open();
    void Close();
    void save();
    void saveAs();
    void import();
    void updateRecentFileActions();
    void openRecentFile();

    void undo();
    void redo();
    void selectVertex();
    void selectEdge();
    void selectFace();
    void selectBody();
    void selectConnector();
    void selectAny();

    void DeleteSelected();   
    void Scaling();
    void connScaling();
    void Texture();

    void showConsDockwin();
    void regenGeometry();

    void showCyberDockwin();
    void addMachConn();
    void addElecConn();
    void addHydrConn();
    void addContConn();
    //lqq
    void defConnDlg();
    void addMarkingSurfDlg();
    void defDatumPoint();
    void defDatumLine();

    void systemSet();
    void ShowOxyz(); //��ʾȫ������ϵ

    void about();
    void updateMenus();
    void updateWindowMenu();
    void SetColor();
    void SetMaterial();

    void undoEnable(bool b);

    //
    void insert();
    void attributeSetting();
    void assembly();
    void adjust();
    void setpara();
    void setsimulate();
    void connection();
    void modelCheck();
    void simulation();
    void digitaltwins();
    void neuralnetwork();
    void animation();

    void OnStatusChanged(const QString&);

    //���Ժ���
    void MakeBox();
    void MakeSpring();
    void ExportBREP();

    //ˢ����ʾ
    void repaint0();
    
private:
    enum { MaxRecentFiles = 5 };

    //����ʱȱʡ�˵�
    void createActions0();

    //���������ģ����
    void CreateComInterface();

    //����ϵͳ��ģ����
    void CreateMdlInterface();

    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool loadFile(const QString &fileName);
    static bool hasRecentFiles();
    void prependToRecentFiles(const QString &fileName);
    void setRecentFilesVisible(bool visible);
    void InitMaterialInfo(); 

    QMenu *windowMenu;
    QAction *newAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *recentFileActs[MaxRecentFiles];
    QAction *recentFileSeparator;
    QAction *recentFileSubMenuAct;
    QAction* undoAct;
    QAction* redoAct;
    QAction *closeAct;
    QAction *windowMenuSeparatorAct;
    //ѡ������Act,��Ҫ����ı�
    QAction* selVtxAct;
    QAction* selEdgeAct;
    QAction* selFaceAct;
    QAction* selBodyAct;
    QAction* selConnectorAct;
    QAction* selAnyAct;
    QAction* colorAct;
    QAction* simulationAndsolveAct;
    QAction* digitalTwinsAct;
    QAction* neuralNetworkAct;
    QToolBar* postProcessToolBar;

    //ɾ��Act
    QAction* delAct;

    //�¼�ϵͳ����
    QAction* rotaAct;
    QAction* panAct;
    QAction* zoominAct;
    QAction* zoomoutAct;
    QAction* addAct;

    //�����ģʽ��,��Ҫ����ð�ť�Ƿ���
    QAction* showConsDockwinAct;
    QAction* showCyberDockwinAct;
    QAction* showVoxelDockwinAct;
    QAction* showOxyzAct;
    //ϵͳģʽ�µ�ģ�Ϳ�Widget
    ModelLibWidget* m_pModelLibWidget;
    //ϵͳģʽ�µ�װ��Widget
    AssemblyTreeWidget* m_pAssemblyTreeWidget;
 
    //�ļ��˵��͹�����
    QMenu* fileMenu;
    QToolBar* fileToolBar;
    //ѡ�񹤾���
    QMenu* subSelmenu;
    //ɾ��������
    QMenu* subDelmenu;
    //QMenu* subPartDelmenu;
private:
    int nDocType;
    DocumentCommon* myDocument3d;
    GeomWidget* myGeomWidget;
    ConsDockWidget* consdockwidget;
    voxelWidget* voxelDockWidget;
    CyberInfoDockWidget* myParadock;
    QSplitter* mainSplitter;
    ParaDockWidget* m_ParaDock;
    AnimationWidget* m_AnimationWidget;
    DigitaltwinsWidget* m_DigitaltwinsWidget;
    NeuralnetworkWidget* m_NeuralnetworkWidget;
    //vector< ParameterValue> m_vec;
    LibLoadThread* m_pLibLoadThread;

protected:
    virtual void keyReleaseEvent(QKeyEvent* event) override;

public:
    AssemblyTreeWidget* GetAssemblyTreeWidget() const { return m_pAssemblyTreeWidget; }
    AnimationWidget* GetAnimationWidget() const { return m_AnimationWidget; }
    DigitaltwinsWidget* GetDigitaltwinsWidget() const { return m_DigitaltwinsWidget; }
};

#endif