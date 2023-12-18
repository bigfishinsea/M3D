#include "M3d.h"
#include "NewDocDlg.h"
#include<qpushbutton.h>

NewDocDlg::NewDocDlg(QWidget* parent)
{
    bAccepted = false;

    //�б������ϵͳ
    //QListWidgetItem* lwi1 = new QListWidgetItem("���ģ��");//�����б���Ŀ
    //lwi1->setTextAlignment(Qt::AlignLeft);//�����ı����뷽ʽ

    //QListWidget* lw=new QListWidget(this);//�����б�ؼ�
    lstSelect = new QListWidget(this);//�����б�ؼ�
    //lw->setViewMode(QListView::IconMode);//������ʾģʽΪͼ��ģʽ--��Ŀ���
    lstSelect->setViewMode(QListView::ListMode);//������ʾģʽΪ�б�ģʽ--����
    //lstSelect->addItem(lwi1);//���б�ؼ����һ����Ŀ

    QStringList list;
    list << "���ģ��" << "ϵͳģ��";  //��list������Ŀ
    //�޷����ö���
    lstSelect->addItems(list);//���б�ؼ����һЩ��Ŀ
    lstSelect->setFixedSize(300, 50);
    lstSelect->setCurrentRow(0);

    setFixedSize(300, 120);

    //����ok��cancel
    okBtn = new QPushButton(this);
    okBtn->setText("ȷ��");
    okBtn->move(100, 80);
    cancelBtn = new QPushButton(this);
    cancelBtn->setText("ȡ��");
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