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

class ImageShowDlg :
    public QDialog
{
   Q_OBJECT

private:
    QPixmap m_Pixmap;
    QLabel* myLabel;

public:
    ImageShowDlg(QWidget* parent = 0);
    ~ImageShowDlg();

    void SetPixmap(QPixmap pm);
};
