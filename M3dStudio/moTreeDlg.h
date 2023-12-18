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

//Modelica文件模型
class ModelicaFileModel
{
public:
    ModelicaFileModel() { pParent = nullptr; }

    string sName; //模型简名;
    string sTypeName; //模型类型名：class, model, ...
    string sMoPath; //模型所在mo文件名；
    int nStartLine; //模型在mo文件中的起始行；
    int nEndLine;   //模型在mo中终止行号；

    ModelicaFileModel* pParent;//父模型
    list<ModelicaFileModel*> lstChildren; //子模型；

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

    QString m_sMdlFullName; //模型全名
    CyberInfo* m_pCyberInfo;
    QString m_sMoPath;
    ModelicaFileModel* m_theModel; //旧
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
