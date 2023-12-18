#pragma once
//这是体素建模界面
/********************************************************************************
** Form generated from reading UI file 'voxel.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/
#include "M3d.h"
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QStackedLayout>
#include <QtWidgets/QDockWidget>
#include <AIS_InteractiveContext.hxx>
#include <gp_Pnt.hxx>

class voxel;
class DocumentCommon;

//球
class pageSphere : public QWidget
{
    Q_OBJECT

public:
    pageSphere();
    pageSphere(QWidget* parent);
    ~pageSphere() {};

private:
    gp_Pnt m_center;
    double m_radius;

private:
    QLabel* centerLabel;
    QLabel* radiueLabel;
    QLineEdit* centerInput;
    QLineEdit* radiusInput;

public:
    void SetCenter(gp_Pnt& p0) { m_center = p0; }
    gp_Pnt GetCenter() const { return m_center; }
    void SetRadius(double r0) { m_radius = r0; }
    double GetRadius() const { return m_radius; }

    void InitUi();
    //检查输入合法性
    bool CheckInput();
    //根据界面更新数据
    bool UpdateUi(bool bUpdateUI = true);
    //用aVox数据刷新界面
    void SetData(voxel* aVox);
    //用界面刷新数据
    void GetData(voxel* aVox);
};

//长方体
class pageBox : public QWidget
{
    Q_OBJECT

public:
    pageBox();
    pageBox(QWidget* prnt);
    ~pageBox() { ; }

private:
    QLabel* lblCorner;
    QLineEdit* cornerInput;
    QLabel* lblZDir;
    QLabel* lblXDir;
    QLineEdit* XdirInput;
    QLineEdit* ZdirInput;
    QLabel* lblLen;
    QLineEdit* lenInput;
    QLabel* lblWid;
    QLineEdit* widInput;    
    QLabel* lblHeight;
    QLineEdit* heightInput;

private:
    gp_Pnt m_corner; //角点
    gp_Dir m_ZDir; //高度方向
    gp_Dir m_XDir; //长度方向
    double m_length;
    double m_width;
    double m_height;

public:
    void SetCorner(gp_Pnt& p0) { m_corner = p0; }
    gp_Pnt GetCorner() const { return m_corner; }
    void SetZDir(gp_Dir& v3) { m_ZDir = v3; }
    gp_Dir GetZDir() const { return m_ZDir; }
    void SetXDir(gp_Dir& v3) { m_XDir = v3; }
    gp_Dir GetXDir() const { return m_XDir; }

    void SetLength(double l0) { m_length = l0; }
    double GetLength() const { return m_length; }
    void SetWidth(double w0) { m_width = w0; }
    double GetWidth() const { return m_width; }
    void SetHeight(double h0) { m_height = h0; }
    double GetHeight() const { return m_height; }

    void InitUi();
    //检查输入合法性
    bool CheckInput();
    //根据界面更新数据
    bool UpdateUi(bool bUpdateUI = true);
    //用aVox数据刷新界面
    void SetData(voxel* aVox);
    //用界面刷新数据
    void GetData(voxel* aVox);
};

//圆柱体
class pageCylinder : public QWidget
{
    Q_OBJECT

public:
    pageCylinder();
    pageCylinder(QWidget* prnt);
    ~pageCylinder() { ; }

private:
    QLabel* lblBtnCenter;
    QLineEdit* btnCenterInput;
    QLabel* lblZDir;
    QLineEdit* ZdirInput;
    QLabel* lblRadius;
    QLineEdit* radiusInput;
    QLabel* lblHeight;
    QLineEdit* heightInput;

private:
    gp_Pnt m_btnCenter; //底面中心点
    gp_Dir m_ZDir; //高度方向
    double m_radius;
    double m_height;

public:
    void SetBtnCorner(gp_Pnt& p0) { m_btnCenter = p0; }
    gp_Pnt GetBtnCorner() const { return m_btnCenter; }
    void SetZDir(gp_Dir& v3) { m_ZDir = v3; }
    gp_Dir GetZDir() const { return m_ZDir; }
    void SetRadius(double l0) { m_radius = l0; }
    double GetRadius() const { return m_radius; }
    void SetHeight(double h0) { m_height = h0; }
    double GetHeight() const { return m_height; }

    void InitUi();
    //检查输入合法性
    bool CheckInput();
    //根据界面更新数据
    bool UpdateUi(bool bUpdateUI = true);
    //用aVox数据刷新界面
    void SetData(voxel* aVox);
    //用界面刷新数据
    void GetData(voxel* aVox);
};

//体素建模总界面
class voxelWidget : 
    public QDockWidget
{
    Q_OBJECT

private:
    list<voxel*> m_lstVoxels;
    DocumentCommon* myDocument;
    Handle(AIS_InteractiveContext) myContext;
    Handle(AIS_InteractiveObject)  myVoxShape; //体素的临时显示
    Handle(AIS_InteractiveObject)  myCompoundShape; //全部体素组合体显示

public:
    voxelWidget();
    //voxelWidget(QWidget* prnt, list<voxel*>& lstVoxels);
    voxelWidget(QWidget* prnt, DocumentCommon* doc3d);
    ~voxelWidget();

private:
    int m_radioItem;
    QVBoxLayout* verticalLayout;

    QHBoxLayout* horizontalLayout;
    QStackedLayout* StackedWidget;

    //horizontalLayout
    QListView* listView; //显示已经生成的特征
    QWidget* widgetRight;

    //widgetRight
    QGroupBox* groupBox;
    QPushButton* pushButtonAdd; //增加特征
    QComboBox* comboBox; //特征类型：球、长方体、圆柱体、...

    QButtonGroup* radioGroup;
    QRadioButton* radioButtonUnion;
    QRadioButton* radioButtonCommon;
    QRadioButton* radioButtonSub1; //被差
    QRadioButton* radioButtonSub2; //主差

    QPushButton* pushButtonApply;  //应用，执行当前特征操作
    QPushButton* pushButtonDelete; //删除特征
    QPushButton* pushButtonRegen;  //重构全部特征

    //下面的堆栈
    pageSphere  * pageSphere1;
    pageBox     * pageBox1;
    pageCylinder* pageCyl1;

    void InitUi();

private slots:
    void voxelTypeChanged();
    void voxelAdd();
    void voxelDelete();
    void voxelApply();
    void voxelRegen();
    void voxelSelectChanged();
    void radioClicked(QAbstractButton* buttn);

public:
    bool CheckInput();
    //根据界面更新数据
    bool UpdateUi(bool bUpdateUI=true);
    void UpdateVoxelName(voxel*);
    //根据数据更新界面
    void SetData(voxel* aVox);
    //用界面刷新数据
    void GetData(voxel* aVox);
    //用一组体素刷新界面，从文档读入时使用
    void SetListVoxels(list<voxel*> lstVoxes);
    list<voxel*> GetVoxels() const;
};

