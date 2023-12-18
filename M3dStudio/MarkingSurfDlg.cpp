#include "MarkingSurfDlg.h"
#include "mainwindow.h"
#include <TopoDS_Face.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <gp_Sphere.hxx>
#include "M3d.h"
#include "DocumentComponent.h"
#include <QPushButton>
#include "Connector.h"


MarkingSurfDlg::MarkingSurfDlg(QWidget* parent) :QDialog(parent)
{
	myDocument3d = (DocumentComponent*)qobject_cast<MainWindow*>(parent)->getDocument();
	markingDlgInit();
}

MarkingSurfDlg::~MarkingSurfDlg()
{

}

void MarkingSurfDlg::markingDlgInit()
{
	setWindowTitle(tr("添加标记面"));
	
	QVBoxLayout* VerticalLayout = new QVBoxLayout(this);//设置整体布局为垂直布局
	VerticalLayout->setDirection(QBoxLayout::TopToBottom);

	// X
	defCenterX = new QLabel("中心点X");
	defCenterX->setMinimumHeight(25);
	xSpinbox = new QDoubleSpinBox(this);
	double MaxRange = 100000.0;
	xSpinbox->setRange(-MaxRange, MaxRange);
	xSpinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	xSpinbox->setAlignment(Qt::AlignCenter);
	xSpinbox->setDecimals(3);

	// Y
	defCenterY = new QLabel("中心点Y");
	defCenterY->setMinimumHeight(25);
	ySpinbox = new QDoubleSpinBox(this);
	ySpinbox->setRange(-MaxRange, MaxRange);
	ySpinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	ySpinbox->setAlignment(Qt::AlignCenter);
	ySpinbox->setDecimals(3);

	// Z
	defCenterZ = new QLabel("中心点Z");
	defCenterZ->setMinimumHeight(25);
	zSpinbox = new QDoubleSpinBox(this);
	zSpinbox->setRange(-MaxRange, MaxRange);
	zSpinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	zSpinbox->setAlignment(Qt::AlignCenter);
	zSpinbox->setDecimals(3);

	// R
	defRadius = new QLabel("半径R");
	defRadius->setMinimumHeight(25);
	rSpinbox = new QDoubleSpinBox(this);
	rSpinbox->setRange(0.0, MaxRange);
	rSpinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	rSpinbox->setAlignment(Qt::AlignCenter);
	rSpinbox->setDecimals(3);


	PushButtonAccept = new QPushButton(this);
	PushButtonAccept->setText("确定");
	PushButtonAccept->setFixedSize(75, 25);
	connect(PushButtonAccept, SIGNAL(clicked()), this, SLOT(ClickButtonAccept()));

	PushButtonCancel = new QPushButton(this);
	PushButtonCancel->setText("取消");
	PushButtonCancel->setFixedSize(75, 25);
	connect(PushButtonCancel, SIGNAL(clicked()), this, SLOT(ClickButtonCancel()));

	//布局
	QHBoxLayout* HorinztalLayout1 = new QHBoxLayout(this);
	HorinztalLayout1->addWidget(defCenterX);
	HorinztalLayout1->addWidget(xSpinbox);
	QHBoxLayout* HorinztalLayout2 = new QHBoxLayout(this);
	HorinztalLayout2->addWidget(defCenterY);
	HorinztalLayout2->addWidget(ySpinbox);
	QHBoxLayout* HorinztalLayout3 = new QHBoxLayout(this);
	HorinztalLayout3->addWidget(defCenterZ);
	HorinztalLayout3->addWidget(zSpinbox);
	QHBoxLayout* HorinztalLayout4 = new QHBoxLayout(this);
	HorinztalLayout4->addWidget(defRadius);
	HorinztalLayout4->addWidget(rSpinbox);
	QHBoxLayout* HorinztalLayout5 = new QHBoxLayout(this);
	HorinztalLayout5->addWidget(PushButtonCancel);
	HorinztalLayout5->addWidget(PushButtonAccept);


	VerticalLayout->setSpacing(15);
	VerticalLayout->addLayout(HorinztalLayout1);
	VerticalLayout->addLayout(HorinztalLayout2);
	VerticalLayout->addLayout(HorinztalLayout3);
	VerticalLayout->addLayout(HorinztalLayout4);
	VerticalLayout->addLayout(HorinztalLayout5);


	this->setLayout(VerticalLayout);
}

void MarkingSurfDlg::drawSphere()
{
	// Make a face from a sphere with the center
	// at [0.0, 0.0, 10.0] and R = 5.
	gp_Sphere aSphere(gp_Ax3(gp_Pnt(0.0, 0.0, 10.0), gp::DZ()), 5.0);
	TopoDS_Face aFaceSphere = BRepBuilderAPI_MakeFace(aSphere);
	// 要加入DocumentComponet
	// UpdateDisplay()
	
}

void MarkingSurfDlg::ClickButtonAccept()
{
	if (rSpinbox->value() == 0)
	{
		return;
	}
	
	MarkingSurface* markingsurf = new MarkingSurface();
	markingsurf->SetMsurfType(SPHERE);
	vector<double> val;
	val.push_back(xSpinbox->value());
	val.push_back(ySpinbox->value());
	val.push_back(zSpinbox->value());
	markingsurf->SetOffsetValue(val);
	markingsurf->SetRadius(rSpinbox->value());
	gp_Sphere aSphere(gp_Ax3(gp_Pnt(val[0], val[1], val[2]), gp::DZ()), rSpinbox->value());
	markingsurf->SetShape(BRepBuilderAPI_MakeFace(aSphere));//给标记面结构体赋值，用于文档存储

	xSpinbox->setValue(0.0);
	ySpinbox->setValue(0.0);
	zSpinbox->setValue(0.0);
	rSpinbox->setValue(0.0);

	myDocument3d->AddMarkingSurf(markingsurf->GetShape(), markingsurf);//增加标记面，存储到文档
	delete markingsurf;
	markingsurf = NULL;
	myDocument3d->UpdateDisplay();
}

void MarkingSurfDlg::ClickButtonCancel()
{
	QDialog::close();
}

