#pragma once
#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <AIS_Point.hxx>
#include <vector>
#include <string>
#include "FeatureButton.h"
#include <QSettings>
#include <QButtonGroup>
#include "qradiobutton.h"
using namespace std;


class LibPathsSetDlg :public QDialog
{
    Q_OBJECT

public:
    LibPathsSetDlg(QWidget* parent = 0);
    ~LibPathsSetDlg();

    Handle(AIS_Point) datPoint;

    static vector<string> GetModelLibPaths();                   //获得*.mo模型库路径
    static string GetWorkDir();                                 //获得仿真工作目录路径
    static string GetSolverPath();                              //获得求解器文件夹路径
    static string GetSolverDisk();                              //获得求解器所在磁盘
    static double GetGlobalSysSize();                           //获得全局坐标系尺寸
    static double GetLocalSysSize();                            //获得局部坐标系尺寸
    static double GetConnSize();                                //获得接口尺寸
    static string GetSolverName();                              //获得求解器名称

public slots:
    void ClickButton1();
    void ClickButton2();
    void ClickButton3();

protected:


private slots:
    void Accept();

private:

    void dpDlgInit();
    static void CutString(string line, vector<string>& subline, char a);//根据字符切分string，兼容最前最后存在字符

private:
    QVBoxLayout* form;       //总体垂直布局
    QHBoxLayout* hlayout;    //确定、取消按钮水平布局

    //1、modelica模型库路径添加1
    QString dirpath1;
    QString ShowModelLibPath = ";";
    vector<string> ModelLibPath;
    int change;//判断该对话框是否有修改
    QHBoxLayout* inputPath1;
    QLabel* defPath1;
    QLineEdit* showPath1;
    QPushButton* PushButton1;

    //2、仿真工作文件夹路径设置
    QString WorkList2;
    //string worklist;
    int change_worklist;
    QHBoxLayout* inputWorkList2;
    QLabel* defWorkList2;
    QLineEdit* showWorkList2;
    QPushButton* PushButton2;

    //3、仿真求解器所在文件夹路径设置
    QString SolverPath3;
    int change_solverpath;
    QHBoxLayout* inputSolverPath3;
    QLabel* defSolverPath3;
    QLineEdit* showSolverPath3;
    QPushButton* PushButton3;

    QButtonGroup* groupButtonSolvers;           //选择所选求解器的类型
    QRadioButton* ButtonOpenmodelica;           //openmodelica选择框
    QRadioButton* ButtonMWorks;                 //MWorks选择框
    QHBoxLayout* SelectSolver;
    int SolverID;                               //求解器ID


    //4.全局坐标系尺寸设置
    double GlobalSysSize4;
    QHBoxLayout* inputSysSize;
    QLabel* defGlobalSysSize4;
    QLineEdit* showGlobalSysSize4;

    //5.局部坐标系尺寸设置
    double LocalSysSize5;
    QLabel* defLocalSysSize5;
    QLineEdit* showLocalSysSize5;

    //6.接口尺寸设置
    double ConnSize6;
    QLabel* defConnSize6;
    QLineEdit* showConnSize6;


    QComboBox* pointDefType;
    QFont font;
    QLabel* selectFeature;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QString BtnStyle;

};