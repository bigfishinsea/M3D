#include "materialSetDlg.h"
#include "M3d.h"
#include "DocumentComponent.h"

MaterialsSetDlg::MaterialsSetDlg(QWidget* parent) : QDialog(parent)
{
    InitUi();
}

MaterialsSetDlg::~MaterialsSetDlg()
{

}

void MaterialsSetDlg::InitUi()
{
    if (this->objectName().isEmpty())
        this->setObjectName(QString::fromUtf8("MaterialsSettingDlg"));
    this->resize(790, 300);
    //pushButton = new QPushButton(MaterialsSettingDlg);
    //pushButton->setObjectName(QString::fromUtf8("pushButton"));
    //pushButton->setGeometry(QRect(10, 10, 130, 50));
    //pushButton_2 = new QPushButton(MaterialsSettingDlg);
    //pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
    //pushButton_2->setGeometry(QRect(170, 10, 131, 51));
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            //g_vMaterials[i*5+j].sMaterialName
            pushButton[i * 5 + j] = new QPushButton(this);
            pushButton[i * 5 + j]->setObjectName(g_vMaterials[i * 5 + j].sMaterialName.c_str());
            pushButton[i * 5 + j]->setText(g_vMaterials[i * 5 + j].sMaterialChnName.c_str());
            pushButton[i * 5 + j]->setGeometry(QRect(10+j*160, 10+i*50, 130, 50));
            QString sBgdClr = "background: rgb(";
            for (int k = 0; k < 3; k++)
            {
                double v_k = g_vMaterials[i * 5 + j].v3MaterialColor[k] * 255;
                QString sD;
                sD.setNum(v_k, 'f', 2);
                sBgdClr += sD;

                if (k == 2)
                {
                    sBgdClr += ")";
                }
                else
                {
                    sBgdClr += ",";
                }
            }
            sBgdClr += ";color: purple;font: bold; font-size:20px";
            pushButton[i * 5 + j]->setStyleSheet(sBgdClr); 
            
            QObject::connect(pushButton[i * 5 + j], SIGNAL(clicked()),this, SLOT(matSelected()));
        }
    }
    
    //QObject::connect(buttonBox, SIGNAL(accepted()), MaterialsSettingDlg, SLOT(accept()));
    //QObject::connect(buttonBox, SIGNAL(rejected()), MaterialsSettingDlg, SLOT(reject()));

    QMetaObject::connectSlotsByName(this);
}

void MaterialsSetDlg::matSelected()
{
    QPushButton* btn = qobject_cast<QPushButton*>(sender());

    QString objName = btn->objectName();
    
    for (int i = 0; i < 25; i++)
    {
        if (objName == g_vMaterials[i].sMaterialName.c_str() )
        {
            //执行设置材料的操作
            myDocument->SetSelectedMaterial(i);

            break;
        }
    }
}
