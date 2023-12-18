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

    static double GetStarttime();                   //��÷��濪ʼʱ��
    static double GetEndtime();                     //��÷������ʱ��
    static int GetOpenModelica_NumberOfIntervals(); //���openmodelica��ģ��������
    static string GetOpenModelica_Method();         //���openmodelica����ⷽ��
    static int GetOpenModelica_OutputMethodID();    //���openmodelica�������ʽ��0Ϊ������б�����1Ϊ���������������صı���
    static int GetMWorks_NumberOfIntervals();       //���MWorks������������
    static string GetMWorks_Method();               //���MWorks����ⷽ��
    static int GetMWorks_OutputMethodID();          //���MWorks�������ʽ��0Ϊ������б�����1Ϊ���������������صı���

public slots:


protected:


private slots:
    void Accept();
    void ClickResetButton();

private:

    void dpDlgInit();

private:
    
    QVBoxLayout* form;       //���崹ֱ����
    QHBoxLayout* hlayout;    //ȷ����ȡ����ťˮƽ����

    //1.��ͬ����
    QLabel* CommonSet;
    QLabel* SetStartTime;
    QLabel* SetEndTime;
    QLineEdit* StartTimeEdit;
    QLineEdit* EndTimeEdit;
    double starttime;
    double endtime;

    //2.opemmodelica����
    QLabel* OpenmodelicaSet;
    QLabel* SetIntervalsNum;             //����ģ��������
    QLineEdit* IntervalsNumEdit;         //ģ���������༭��
    QLabel* SetMethod;                   //���÷��淽ʽ�������֣�dassl,euler,rungekutta
    QComboBox* cbo_Method;               //���淽ʽѡ��������
    int OpenModelica_NumberOfIntervals;
    int Openmodelica_Method_ID;
    int Openmodelica_OutputMethodID;              //�����ʽID
    QButtonGroup* om_groupButtonOutput;           //ѡ�������ʽ�����ȫ������/������ֱ�����
    QRadioButton* om_ButtonAllVariables;          //ȫ������
    QRadioButton* om_ButtonAnimationVariables;    //�붯����ر���

    //3.MWorks����
    QLabel* MWorksSet;
    QLabel* SetIntegral_step;            //���û��ֲ���
    QLineEdit* NumberOfIntervalsEdit;    //�����������༭��
    QLabel* SetAlgo;                     //���÷��淽ʽ��������
    QComboBox* cbo_Algo;                 //���淽ʽѡ��������
    int MWorks_NumberOfIntervals;
    int MWorks_Method_ID;
    QButtonGroup* groupButtonOutput;           //ѡ�������ʽ�����ȫ������/������ֱ�����
    QRadioButton* ButtonAllVariables;          //ȫ������
    QRadioButton* ButtonAnimationVariables;    //�붯����ر���
    int MWorks_OutputMethodID;                 //�����ʽID

    //4.�ָ�Ĭ�����ð�ť
    QPushButton* RestoreDefaultSet;            //�ָ�Ĭ������

    
    
    
    
    QFont font;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QString BtnStyle;

};