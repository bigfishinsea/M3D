#include <QtWidgets>

#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>

#include "ConsDockWidget.h"
#include "M3d.h"
#include "DocumentComponent.h"
//#include "M3dCom_DS.h"
#include "mainwindow.h"
#include "ConsTableWidget.h"

ConsDockWidget::ConsDockWidget(QWidget* parent = nullptr) :QDockWidget(parent)
{
	consDocument3d = (DocumentComponent * )qobject_cast<MainWindow*>(parent)->getDocument();
	consTableWidget = new ConsTableWidget(this);
	init();
}

void ConsDockWidget::OnrestoreConstraints()
{
	consTableWidget->restoreConstraints();
}

void ConsDockWidget::OnsaveConstraints()
{
	consTableWidget->saveConstraints();
}

void ConsDockWidget::OnaddConstraint()
{
	consTableWidget->addConstraint();
}

void ConsDockWidget::OndelConstraint()
{
	consTableWidget->delConstraint();
}

void ConsDockWidget::OnclearAllConstraints()
{
	consTableWidget->clearAllConstraints();
}

void ConsDockWidget::OnunCheck()
{
	consTableWidget->unCheck();
}

ConsDockWidget::~ConsDockWidget()
{
	if (consTableWidget)
	{
		delete consTableWidget;
		consTableWidget = nullptr;
	}
}

void ConsDockWidget::init()
{
	this->setWindowTitle(tr("约束管理"));
	this->setFeatures(QDockWidget::NoDockWidgetFeatures);

	QPushButton* addConstraint = new QPushButton(tr("添加约束"));
	connect(addConstraint, SIGNAL(clicked()), this, SLOT(OnaddConstraint()));
	QPushButton* delConstraint = new QPushButton(tr("删除约束"));
	connect(delConstraint, SIGNAL(clicked()), this, SLOT(OndelConstraint()));
	QPushButton* clearAllConstraint = new QPushButton(tr("清空所有约束"));
	connect(clearAllConstraint, SIGNAL(clicked()), this, SLOT(OnclearAllConstraints()));
	QPushButton* unCheck = new QPushButton(tr("取消选中"));
	connect(unCheck, SIGNAL(clicked()), this, SLOT(OnunCheck()));
	QPushButton* restoreConstraint = new QPushButton(tr("恢复约束"));
	connect(restoreConstraint, SIGNAL(clicked()), this, SLOT(OnrestoreConstraints()));
	QPushButton* saveConstraint = new QPushButton(tr("应用"));
	connect(saveConstraint, SIGNAL(clicked()), this, SLOT(OnsaveConstraints()));

	QVBoxLayout* layout = new QVBoxLayout();
	QHBoxLayout* buttonlayout1 = new QHBoxLayout();
	QHBoxLayout* buttonlayout2 = new QHBoxLayout();

	//按钮布局,分为两行
	buttonlayout1->addStretch(10);
	buttonlayout1->addWidget(addConstraint);
	buttonlayout1->addStretch(1);
	buttonlayout1->addWidget(delConstraint);
	buttonlayout1->addStretch(1);
	buttonlayout1->addWidget(clearAllConstraint);
	buttonlayout1->addStretch(1);
	buttonlayout1->addWidget(unCheck);
	buttonlayout1->addStretch(10);

	buttonlayout2->addStretch(10);
	buttonlayout2->addWidget(restoreConstraint);
	buttonlayout2->addStretch(2);
	buttonlayout2->addWidget(saveConstraint);
	buttonlayout2->addStretch(10);

	//总布局
	layout->addWidget(consTableWidget);
	layout->setSpacing(20);
	layout->addLayout(buttonlayout1);
	layout->addLayout(buttonlayout2);

	QWidget* widget = new QWidget();
	this->setWidget(widget);
	widget->setLayout(layout);
}

DocumentComponent* ConsDockWidget::getConsDocument()
{
	return consDocument3d;
}
