#pragma once
//�������ؽ�ģ����
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

//��
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
    //�������Ϸ���
    bool CheckInput();
    //���ݽ����������
    bool UpdateUi(bool bUpdateUI = true);
    //��aVox����ˢ�½���
    void SetData(voxel* aVox);
    //�ý���ˢ������
    void GetData(voxel* aVox);
};

//������
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
    gp_Pnt m_corner; //�ǵ�
    gp_Dir m_ZDir; //�߶ȷ���
    gp_Dir m_XDir; //���ȷ���
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
    //�������Ϸ���
    bool CheckInput();
    //���ݽ����������
    bool UpdateUi(bool bUpdateUI = true);
    //��aVox����ˢ�½���
    void SetData(voxel* aVox);
    //�ý���ˢ������
    void GetData(voxel* aVox);
};

//Բ����
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
    gp_Pnt m_btnCenter; //�������ĵ�
    gp_Dir m_ZDir; //�߶ȷ���
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
    //�������Ϸ���
    bool CheckInput();
    //���ݽ����������
    bool UpdateUi(bool bUpdateUI = true);
    //��aVox����ˢ�½���
    void SetData(voxel* aVox);
    //�ý���ˢ������
    void GetData(voxel* aVox);
};

//���ؽ�ģ�ܽ���
class voxelWidget : 
    public QDockWidget
{
    Q_OBJECT

private:
    list<voxel*> m_lstVoxels;
    DocumentCommon* myDocument;
    Handle(AIS_InteractiveContext) myContext;
    Handle(AIS_InteractiveObject)  myVoxShape; //���ص���ʱ��ʾ
    Handle(AIS_InteractiveObject)  myCompoundShape; //ȫ�������������ʾ

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
    QListView* listView; //��ʾ�Ѿ����ɵ�����
    QWidget* widgetRight;

    //widgetRight
    QGroupBox* groupBox;
    QPushButton* pushButtonAdd; //��������
    QComboBox* comboBox; //�������ͣ��򡢳����塢Բ���塢...

    QButtonGroup* radioGroup;
    QRadioButton* radioButtonUnion;
    QRadioButton* radioButtonCommon;
    QRadioButton* radioButtonSub1; //����
    QRadioButton* radioButtonSub2; //����

    QPushButton* pushButtonApply;  //Ӧ�ã�ִ�е�ǰ��������
    QPushButton* pushButtonDelete; //ɾ������
    QPushButton* pushButtonRegen;  //�ع�ȫ������

    //����Ķ�ջ
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
    //���ݽ����������
    bool UpdateUi(bool bUpdateUI=true);
    void UpdateVoxelName(voxel*);
    //�������ݸ��½���
    void SetData(voxel* aVox);
    //�ý���ˢ������
    void GetData(voxel* aVox);
    //��һ������ˢ�½��棬���ĵ�����ʱʹ��
    void SetListVoxels(list<voxel*> lstVoxes);
    list<voxel*> GetVoxels() const;
};

