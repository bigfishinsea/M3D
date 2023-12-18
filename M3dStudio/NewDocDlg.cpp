#include "M3d.h"
#include "NewDocDlg.h"
#include<qpushbutton.h>

NewDocDlg::NewDocDlg(QWidget* parent)
{
    bAccepted = false;

    //列表：零件、系统
    //QListWidgetItem* lwi1 = new QListWidgetItem("零件模型");//创建列表项目
    //lwi1->setTextAlignment(Qt::AlignLeft);//设置文本对齐方式

    //QListWidget* lw=new QListWidget(this);//创建列表控件
    lstSelect = new QListWidget(this);//创建列表控件
    //lw->setViewMode(QListView::IconMode);//设置显示模式为图标模式--项目横放
    lstSelect->setViewMode(QListView::ListMode);//设置显示模式为列表模式--竖放
    //lstSelect->addItem(lwi1);//给列表控件添加一个项目

    QStringList list;
    list << "零件模型" << "系统模型";  //给list增加项目
    //无法设置对齐
    lstSelect->addItems(list);//给列表控件添加一些项目
    lstSelect->setFixedSize(300, 50);
    lstSelect->setCurrentRow(0);

    setFixedSize(300, 120);

    //增加ok，cancel
    okBtn = new QPushButton(this);
    okBtn->setText("确定");
    okBtn->move(100, 80);
    cancelBtn = new QPushButton(this);
    cancelBtn->setText("取消");
    cancelBtn->move(200, 80);

    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);
    connect(lstSelect, &QListWidget::doubleClicked, this, &NewDocDlg::Accept);
    connect(okBtn, &QPushButton::clicked, this, &NewDocDlg::Accept);
}

void NewDocDlg::Accept()
{
    bAccepted = true;

    iSelItem = lstSelect->currentRow();

    QDialog::close();
}