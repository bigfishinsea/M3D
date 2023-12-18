#pragma once
#include "M3d.h"
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtWidgets/qtreewidget.h>

class CyberInfo;
class ModelicaModel;

//Modelica�ļ�ģ��
class ModelicaFileModel
{
public:
    ModelicaFileModel() { pParent = nullptr; }

    string sName; //ģ�ͼ���;
    string sTypeName; //ģ����������class, model, ...
    string sMoPath; //ģ������mo�ļ�����
    int nStartLine; //ģ����mo�ļ��е���ʼ�У�
    int nEndLine;   //ģ����mo����ֹ�кţ�

    ModelicaFileModel* pParent;//��ģ��
    list<ModelicaFileModel*> lstChildren; //��ģ�ͣ�

    ~ModelicaFileModel() 
    {
        auto itr = lstChildren.begin();
        for (; itr != lstChildren.end(); ++itr)
        {
            ModelicaFileModel* pMdl = *itr;
            delete pMdl;
        }
    }
};

class MoTreeDlg :
    public QDialog
{
    Q_OBJECT

public:
    MoTreeDlg(QString path, QWidget* parent = 0);
    ~MoTreeDlg();

    QString GetModelFullName() const;
    CyberInfo* GetCyberInfo() const;
    bool bAccepted;

private:
    QTreeWidget* myTreeWidget;
    QPushButton* pushButtonOK;
    QPushButton* pushButtonCancel;

    QString m_sMdlFullName; //ģ��ȫ��
    CyberInfo* m_pCyberInfo;
    QString m_sMoPath;
    ModelicaFileModel* m_theModel; //��
    ModelicaModel* m_pModel;

    QIcon m_classIcon[8];

    void InitUi();
    void ShowModelTree();
    void RecursiveAddItems(QTreeWidgetItem* theItem, ModelicaModel* theModel);

    void ShowModelTree(ModelicaFileModel* pModel);
    void RecursiveAddItems(QTreeWidgetItem* theItem, ModelicaFileModel* theModel);

private slots:
    //void modelSelectChaged();
    void dblClickOnTreeItem();
    void Accept();
    void Accept0();
};
