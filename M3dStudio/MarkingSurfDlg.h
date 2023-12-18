#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <gp_Ax3.hxx>
#include <AIS_InteractiveContext.hxx>
#include <QLineEdit>
#include <QDoubleSpinBox>
using namespace std;

class DocumentComponent;

class MarkingSurfDlg :
    public QDialog
{
    Q_OBJECT

public:
    MarkingSurfDlg(QWidget* parent = 0);
    ~MarkingSurfDlg();

private slots:
    void ClickButtonAccept();
    void ClickButtonCancel();

private:
    DocumentComponent* myDocument3d;

    void markingDlgInit();
    void drawSphere();

    //1.球面定义 -- 点中心及球半径
    QLabel* defCenterX;
    QDoubleSpinBox* xSpinbox;
    QLabel* defCenterY;
    QDoubleSpinBox* ySpinbox;
    QLabel* defCenterZ;
    QDoubleSpinBox* zSpinbox;
    QLabel* defRadius;
    QDoubleSpinBox* rSpinbox;

    QPushButton* PushButtonAccept;
    QPushButton* PushButtonCancel;
};