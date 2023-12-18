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

    static vector<string> GetModelLibPaths();                   //���*.moģ�Ϳ�·��
    static string GetWorkDir();                                 //��÷��湤��Ŀ¼·��
    static string GetSolverPath();                              //���������ļ���·��
    static string GetSolverDisk();                              //�����������ڴ���
    static double GetGlobalSysSize();                           //���ȫ������ϵ�ߴ�
    static double GetLocalSysSize();                            //��þֲ�����ϵ�ߴ�
    static double GetConnSize();                                //��ýӿڳߴ�
    static string GetSolverName();                              //������������

public slots:
    void ClickButton1();
    void ClickButton2();
    void ClickButton3();

protected:


private slots:
    void Accept();

private:

    void dpDlgInit();
    static void CutString(string line, vector<string>& subline, char a);//�����ַ��з�string��������ǰ�������ַ�

private:
    QVBoxLayout* form;       //���崹ֱ����
    QHBoxLayout* hlayout;    //ȷ����ȡ����ťˮƽ����

    //1��modelicaģ�Ϳ�·�����1
    QString dirpath1;
    QString ShowModelLibPath = ";";
    vector<string> ModelLibPath;
    int change;//�жϸöԻ����Ƿ����޸�
    QHBoxLayout* inputPath1;
    QLabel* defPath1;
    QLineEdit* showPath1;
    QPushButton* PushButton1;

    //2�����湤���ļ���·������
    QString WorkList2;
    //string worklist;
    int change_worklist;
    QHBoxLayout* inputWorkList2;
    QLabel* defWorkList2;
    QLineEdit* showWorkList2;
    QPushButton* PushButton2;

    //3����������������ļ���·������
    QString SolverPath3;
    int change_solverpath;
    QHBoxLayout* inputSolverPath3;
    QLabel* defSolverPath3;
    QLineEdit* showSolverPath3;
    QPushButton* PushButton3;

    QButtonGroup* groupButtonSolvers;           //ѡ����ѡ�����������
    QRadioButton* ButtonOpenmodelica;           //openmodelicaѡ���
    QRadioButton* ButtonMWorks;                 //MWorksѡ���
    QHBoxLayout* SelectSolver;
    int SolverID;                               //�����ID


    //4.ȫ������ϵ�ߴ�����
    double GlobalSysSize4;
    QHBoxLayout* inputSysSize;
    QLabel* defGlobalSysSize4;
    QLineEdit* showGlobalSysSize4;

    //5.�ֲ�����ϵ�ߴ�����
    double LocalSysSize5;
    QLabel* defLocalSysSize5;
    QLineEdit* showLocalSysSize5;

    //6.�ӿڳߴ�����
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