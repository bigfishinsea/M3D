#pragma once

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

class DocumentComponent;

class MaterialsSetDlg : 
    public QDialog
{
    Q_OBJECT

public:

    MaterialsSetDlg(QWidget* parent=0);
    ~MaterialsSetDlg();

    void SetDocument(DocumentComponent* pDoc) { myDocument = pDoc; }

private:
    //QDialogButtonBox* buttonBox;
 
    QPushButton* pushButton[25];
    DocumentComponent* myDocument;
 
    void InitUi();

private slots:
    void matSelected();

// setupUi

/*    void retranslateUi(QDialog* MaterialsSettingDlg)
    {
        MaterialsSettingDlg->setWindowTitle(QApplication::translate("MaterialsSettingDlg", "Material Set", nullptr));
        pushButton_3->setText(QApplication::translate("MaterialsSettingDlg", "PushButton", nullptr));
        pushButton_2->setText(QApplication::translate("MaterialsSettingDlg", "PushButton", nullptr));
        pushButton_4->setText(QApplication::translate("MaterialsSettingDlg", "PushButton", nullptr));
        toolButton->setText(QApplication::translate("MaterialsSettingDlg", "...", nullptr));
        radioButton->setText(QApplication::translate("MaterialsSettingDlg", "RadioButton", nullptr));
        pushButton_5->setText(QApplication::translate("MaterialsSettingDlg", "PushButton", nullptr));
        pushButton->setText(QApplication::translate("MaterialsSettingDlg", "PushButton", nullptr));
        pushButton_6->setText(QApplication::translate("MaterialsSettingDlg", "PushButton", nullptr));
        pushButton_7->setText(QApplication::translate("MaterialsSettingDlg", "PushButton", nullptr));
        label->setText(QApplication::translate("MaterialsSettingDlg", "TextLabel", nullptr));
    } */// retranslateUi
};
