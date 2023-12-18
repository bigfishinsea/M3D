#pragma once
#include <QAbstractItemModel>
#include <QAbstractItemView>
#include <QMouseEvent>
#include<QInputDialog>
#include <QComboBox>
#include "cyberInfo.h"
#include<QStyledItemDelegate>
#include <QModelIndex>
#include <QVariant>
#include <QVector>
#include <QStringList>
#include "M3d.h"

#define FullNamecol 0
#define sTypecol 1
#define defaultValcol 2
#define sUnitcol 3
#define sDimensioncol 4
#define bFocusedcol 5
#define sFocusNamecol 6 
static const int ParatreeMaxCols =7;
static const int OutputtreeMaxCols = 4;
//class CyberInfo;
//树模型的节点类
class TreeItem
{
public:
    explicit TreeItem(const QVector<QVariant>& data,bool flag=true,TreeItem* parentItem = nullptr);
    ~TreeItem();

    void appendChild(TreeItem* child);

    TreeItem* child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItem* parentItem();
    void clearItem();
    void setData(int,  QVariant);
private:
    //每一个树节点包含一个子节点数组，值数组和指向父节点的指针 
    QVector<TreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    TreeItem* m_parentItem;
    bool flag;
};

//树模型
class TreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit TreeModel(const QString& data, QObject* parent = nullptr);
    TreeModel(QObject* parent = nullptr);
    ~TreeModel();

    QVariant data(const QModelIndex& index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role);
    //void setData(const QModelIndex& index)
    bool buildModel(CyberInfo*);
    void buildhelp(QVector<TreeItem*>& ,QVector<int>&,int, QVector<QVariant>&);
    QModelIndex index(int row, int column,
        const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    void ReSet(TreeModel* model, int, int);
    void setupModelData(const QStringList& lines, TreeItem* parent);
    void SetZero(TreeItem*, int, int);
    bool f() { return true; };
    void setflag(bool f) { flag = f; }
    bool empty() { return rootItem==nullptr||rootItem->child(0)==nullptr; }
    TreeItem* getrootItem() { return rootItem; }
    void setrootItem(TreeItem* root) { rootItem = root; }
    vector < pair<QString, QString> > getm_cons(){ return m_cons; }
private:
    vector<string> parname;
    TreeItem* rootItem;
    bool flag;
    vector < pair<QString,QString> > m_cons;
};

//参数树模型
class ListTreeModel:public TreeModel{
    public:
        
        ListTreeModel(const QString& data, QObject* parent = nullptr);
        bool f (){
            return true;
        };
};

class outputTreeModel :public TreeModel {
public:
    outputTreeModel(const QString& data, QObject* parent = nullptr);
    bool f() {
        return false;
    };
};

//代理技术实现QTreeview中的关注下拉列表
class CheckBoxDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    CheckBoxDelegate(QObject* parent = 0) : QStyledItemDelegate(parent) {}
    
    //重载四个自定义方法
    QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
        const QModelIndex& index) const;

    void setEditorData(QWidget* editor, const QModelIndex& index) const;
    void setModelData(QWidget* editor, QAbstractItemModel* model,
        const QModelIndex& index) const;

    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option,
        const QModelIndex& index) const;

};