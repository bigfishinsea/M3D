#pragma once
#include <QDialog.h>
#include <QListWidget>

class NewDocDlg :
    public QDialog
{
    Q_OBJECT

public:
    NewDocDlg(QWidget* parent = 0);
    bool bAccepted;
    int  iSelItem;

private slots:
    void Accept();
   
private:
    QListWidget * lstSelect;
    QPushButton * okBtn;
    QPushButton * cancelBtn;
};

