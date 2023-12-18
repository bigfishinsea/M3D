#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QComboBox>
#include <vector>
#include <string>
#include "FeatureButton.h"
#include <QSettings>
#include <QButtonGroup>
#include "qradiobutton.h"
using namespace std;


class SimulateSetDlg :public QDialog
{
    Q_OBJECT

public:
    SimulateSetDlg(QWidget* parent = 0);
    ~SimulateSetDlg();

    static double GetStarttime();                   //获得仿真开始时间
    static double GetEndtime();                     //获得仿真结束时间
    static int GetOpenModelica_NumberOfIntervals(); //获得openmodelica的模拟间隔数量
    static string GetOpenModelica_Method();         //获得openmodelica的求解方法
    static int GetOpenModelica_OutputMethodID();    //获得openmodelica的输出方式，0为输出所有变量，1为输出与驱动动画相关的变量
    static int GetMWorks_NumberOfIntervals();       //获得MWorks的输出区间个数
    static string GetMWorks_Method();               //获得MWorks的求解方法
    static int GetMWorks_OutputMethodID();          //获得MWorks的输出方式，0为输出所有变量，1为输出与驱动动画相关的变量

public slots:


protected:


private slots:
    void Accept();
    void ClickResetButton();

private:

    void dpDlgInit();

private:
    
    QVBoxLayout* form;       //总体垂直布局
    QHBoxLayout* hlayout;    //确定、取消按钮水平布局

    //1.共同设置
    QLabel* CommonSet;
    QLabel* SetStartTime;
    QLabel* SetEndTime;
    QLineEdit* StartTimeEdit;
    QLineEdit* EndTimeEdit;
    double starttime;
    double endtime;

    //2.opemmodelica设置
    QLabel* OpenmodelicaSet;
    QLabel* SetIntervalsNum;             //设置模拟间隔数量
    QLineEdit* IntervalsNumEdit;         //模拟间隔数量编辑框
    QLabel* SetMethod;                   //设置仿真方式，共三种：dassl,euler,rungekutta
    QComboBox* cbo_Method;               //仿真方式选择下拉框
    int OpenModelica_NumberOfIntervals;
    int Openmodelica_Method_ID;
    int Openmodelica_OutputMethodID;              //输出方式ID
    QButtonGroup* om_groupButtonOutput;           //选择输出方式（输出全部变量/输出部分变量）
    QRadioButton* om_ButtonAllVariables;          //全部变量
    QRadioButton* om_ButtonAnimationVariables;    //与动画相关变量

    //3.MWorks设置
    QLabel* MWorksSet;
    QLabel* SetIntegral_step;            //设置积分步长
    QLineEdit* NumberOfIntervalsEdit;    //输出区间个数编辑框
    QLabel* SetAlgo;                     //设置仿真方式，共八种
    QComboBox* cbo_Algo;                 //仿真方式选择下拉框
    int MWorks_NumberOfIntervals;
    int MWorks_Method_ID;
    QButtonGroup* groupButtonOutput;           //选择输出方式（输出全部变量/输出部分变量）
    QRadioButton* ButtonAllVariables;          //全部变量
    QRadioButton* ButtonAnimationVariables;    //与动画相关变量
    int MWorks_OutputMethodID;                 //输出方式ID

    //4.恢复默认设置按钮
    QPushButton* RestoreDefaultSet;            //恢复默认设置

    
    
    
    
    QFont font;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QString BtnStyle;

};