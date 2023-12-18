#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <AIS_Line.hxx>
#include <AIS_ColoredShape.hxx>
//#include "mainwindow.h"
//#include "M3dCom_DS.h"
#include "FeatureButton.h"
//#include "DocumentComponent.h"

class DocumentComponent;
class M3dCom_DS;
class View;

class DatumLineDlg :
    public QDialog
{
    Q_OBJECT

public:
    DatumLineDlg(QWidget* parent = 0);
    ~DatumLineDlg();

    DocumentComponent* getDocumentFromDlg();

    Handle(AIS_Shape) datLine;

private slots:
    void Accept();

private:
    DocumentComponent* myDocument3d;
    Handle(AIS_InteractiveContext) myContext;
    TopoDS_Shape shpSel;
    //M3dCom_DS* pCom;
    std::string sType;
    View* myview;

    void dlDlgInit();

private:
    QVBoxLayout* form;
    QHBoxLayout* hlayout;
    QLabel* defType;
    QComboBox* lineDefType;
    QFont font;
    QLabel* selectFeature;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QString BtnStyle;

    QWidget* page1;
    QVBoxLayout* page1Layout;
    QLabel* feature11;
    QLabel* feature12;
    FeatureButton* value11;
    QDoubleSpinBox* xSpinbox;
    QDoubleSpinBox* ySpinbox;
    QDoubleSpinBox* zSpinbox;
    QHBoxLayout* vecLayout;

    QWidget* page2;
    QVBoxLayout* page2Layout;
    QLabel* feature21;
    QLabel* feature22;
    FeatureButton* value21;
    FeatureButton* value22;

    QWidget* page3;
    QVBoxLayout* page3Layout;
    QLabel* feature31;
    FeatureButton* value31;

    QWidget* page4;
    QVBoxLayout* page4Layout;
    QLabel* feature41;
    QLabel* feature42;
    FeatureButton* value41;
    FeatureButton* value42;

    QWidget* page5;
    QVBoxLayout* page5Layout;
    QLabel* feature51;
    QLabel* feature52;
    FeatureButton* value51;
    FeatureButton* value52;

    QStackedLayout* StackedWidget;
};
