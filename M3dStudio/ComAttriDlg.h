#pragma once
#include "DocumentModel.h"
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGroupBox>
#include <qpushbutton.h>
#include <AIS_InteractiveContext.hxx>
//#include "DocumentModel.h"

class ComAttriDlg :
    public QDialog
{
    Q_OBJECT

public:
    ComAttriDlg(QWidget* parent = 0);
    ~ComAttriDlg();

public slots:
    void Accept();

private:
    DocumentModel* myDocument3d;
    string compName;

    void comAttriDlgInit();
    void setAttriValue();

private:
    QVBoxLayout* form;
    QHBoxLayout* pro_cas;
    QHBoxLayout* var_con;
    QGridLayout* grid;
    QString BtnStyle;
    QPushButton* okBtn;
    QPushButton* cancelBtn;
    QHBoxLayout* hlayout;

    //����
    QVBoxLayout* pro_form;
    QGroupBox* pro_box;
    QCheckBox* Final;
    QCheckBox* Protected;
    QCheckBox* Replaceable;

    //�ɱ���
    QVBoxLayout* var_form;
    QGroupBox* var_box;
    QButtonGroup* var_group;
    QRadioButton* Constant;
    QRadioButton* Parameter;
    QRadioButton* Discrete;
    QRadioButton* Default;

    //��̬����
    QVBoxLayout* dyn_form;
    QGroupBox* dyn_box;
    QCheckBox* Inner;
    QCheckBox* Outer;

    //�����
    QVBoxLayout* cas_form;
    QGroupBox* cas_box;
    QButtonGroup* cas_group;
    QRadioButton* Input;
    QRadioButton* Output;
    QRadioButton* None;

    //��������Ա
    QVBoxLayout* con_form;
    QGroupBox* con_box;
    QButtonGroup* con_group;
    QRadioButton* Flow;
    QRadioButton* Stream;
    QRadioButton* Custom;

};