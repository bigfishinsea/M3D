#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QLabel>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <AIS_Point.hxx>
//#include "mainwindow.h"
//#include "M3dCom_DS.h"
#include "FeatureButton.h"
//#include "DocumentComponent.h"

class M3dCom_DS;
class DocumentComponent;
class View;

class DatumPointDlg :
    public QDialog
{
    Q_OBJECT

public:
    DatumPointDlg(QWidget* parent = 0);
    ~DatumPointDlg();
    DocumentComponent* getDocumentFromDlg();

    Handle(AIS_Point) datPoint;


protected:


private slots:
    void Accept();
    //void pointClickEvent();

private:
    DocumentComponent* myDocument3d;
    Handle(AIS_InteractiveContext) myContext;
    View* myview;
    TopoDS_Shape shpSel;
    M3dCom_DS* pCom;
    std::string sType;

    void dpDlgInit();

private:
    QVBoxLayout* form;
    QHBoxLayout* hlayout;
    QLabel* defType;
    QComboBox* pointDefType;
    QFont font;
    QLabel* selectFeature;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QString BtnStyle;

    QWidget* page1;
    QVBoxLayout* page1Layout;
    QLabel* feature11;
    QLabel* feature12;
    FeatureButton* value11;//
    QDoubleSpinBox* xSpinbox;
    QDoubleSpinBox* ySpinbox;
    QDoubleSpinBox* zSpinbox;
    QHBoxLayout* vecLayout;

    QWidget* page2;
    QVBoxLayout* page2Layout;
    QLabel* feature21;
    FeatureButton* value21;//

    QWidget* page3;
    QVBoxLayout* page3Layout;
    QLabel* feature31;
    QLabel* feature32;
    FeatureButton* value31;//
    FeatureButton* value32;//

    QWidget* page4;
    QVBoxLayout* page4Layout;
    QLabel* feature41;
    QLabel* feature42;
    FeatureButton* value41;//
    FeatureButton* value42;//

    QWidget* page5;
    QVBoxLayout* page5Layout;
    QLabel* feature51;
    FeatureButton* value51;//

    QWidget* page6;
    QVBoxLayout* page6Layout;
    QLabel* feature61;
    QLabel* feature62;
    FeatureButton* value61;//
    FeatureButton* value62;//

    QStackedLayout* StackedWidget;
};
