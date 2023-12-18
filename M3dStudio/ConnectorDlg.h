#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <gp_Ax3.hxx>
#include <AIS_InteractiveContext.hxx>
#include "FeatureButton.h"
#include "TopoDS_Compound.hxx"
#include "AIS_InteractiveContext.hxx"
using namespace std;

class Connector;
class DocumentComponent;
class M3dCom_DS;
class CyberInfo;

class ConnectorDlg :
    public QDialog
{
    Q_OBJECT

public:
    ConnectorDlg(QWidget* parent = 0);
    ~ConnectorDlg();

    DocumentComponent* getDocumentFromDlg();


public slots:
    void Accept();
    void connTypeChanged();
    void defTypeChanged(); 

private:
    DocumentComponent* myDocument3d;
    Handle(AIS_InteractiveContext) myContext;
    vector<string> ConnectorNames;
    vector<string> ConnectorTypes;
    TopoDS_Shape shpSel;
    //M3dCom_DS* m_pComDS;
    CyberInfo* m_cyberInfo;
    std::string sType;
    gp_Ax3 Ax3;

    void connDlgInit();
    void modelicaTypeInit();
    bool IsVectorFound(vector<string> arr, string content);
    TopoDS_Compound makeAxisControlBus(const gp_Ax2& Axes, const Standard_Real R, const Standard_Real H);

private:
    QVBoxLayout* form;
    QHBoxLayout* hlayout;
    QLabel* selConnector;
    QComboBox* modelicaConnector;
    QLabel* defType;
    QComboBox* connDefType;
    QFont font;
    QLabel* selectFeature;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QString BtnStyle;

    QWidget* page1;
    QVBoxLayout* page1Layout;
    QLabel* feature11;
    QLabel* feature12;
    QLabel* feature13;
    FeatureButton* value11;
    FeatureButton* value12;
    FeatureButton* value13;
    
    QWidget* page2;
    QVBoxLayout* page2Layout;
    QLabel* feature21;
    QLabel* feature22;
    FeatureButton* value21;
    FeatureButton* value22;
    QCheckBox* reverDir;

    QWidget* page3;
    QVBoxLayout* page3Layout;
    QLabel* feature31;
    QLabel* feature32;
    FeatureButton* value31;
    FeatureButton* value32;

    QWidget* page4;
    QVBoxLayout* page4Layout;
    QLabel* feature41;
    FeatureButton* value41;

    QStackedLayout* StackedWidget;
};