#include "M3d.h"
#include "GeomWidget.h"
#include "DatumPointDlg.h"
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>
#include <Geom_CartesianPoint.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GeomAPI_IntCS.hxx>
#include <Geom_Plane.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <qpushbutton.h>
#include <QMessageBox>

#include "mainwindow.h"
//#include "M3dCom_DS.h"

DatumPointDlg::DatumPointDlg(QWidget* parent) :QDialog(parent)
{
	myDocument3d = (DocumentComponent*)qobject_cast<MainWindow*>(parent)->getDocument();
	//pCom = myDocument3d->GetComDS();
	myContext = myDocument3d->getContext();
	myview = qobject_cast<MainWindow*>(parent)->getGeomWidget()->GetView();
	dpDlgInit();
}

void DatumPointDlg::dpDlgInit()
{
	setWindowTitle(tr("基准点定义"));
	BtnStyle = QString(
		//正常状态
		"QPushButton{"
		"background-color:white;"//背景颜色和透明度
		"color:black;"//字体颜色
		"border: 2px inset rgb(128,128,128);"//边框宽度、样式以及颜色
		"font: 18px;"//字体类型和字体大小
		"text-align: left;"
		"}"
		//按下时的状态
		"QPushButton:pressed{"
		"background-color:rgb(255,255,255);"
		"color:rgb(0,0，0);"
		"}"
		//下拉框
		"QComboBox{"
		"font: 18px;"
		"}");

	form = new QVBoxLayout(this);
	defType = new QLabel("定义方式 "); //定义方式 下拉框
	font = defType->font();//加粗字体
	font.setBold(true);
	defType->setFont(font);
	defType->setMinimumHeight(25);
	pointDefType = new QComboBox(this);
	pointDefType->addItem("点偏移");
	pointDefType->addItem("面中心");
	pointDefType->addItem("交叉点");
	pointDefType->addItem("中点");
	pointDefType->addItem("质心");
	pointDefType->addItem("投影点");
	pointDefType->setStyleSheet(BtnStyle);
	pointDefType->setMinimumHeight(25);
	selectFeature = new QLabel("特征选取");
	selectFeature->setFont(font);
	selectFeature->setMinimumHeight(25);

	//增加ok，cancel
	okBtn = new QPushButton(this);
	okBtn->setText("插入");
	cancelBtn = new QPushButton(this);
	cancelBtn->setText("关闭");
	hlayout = new QHBoxLayout(this);
	hlayout->addWidget(okBtn);
	hlayout->addWidget(cancelBtn);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);
	connect(okBtn, &QPushButton::clicked, this, &DatumPointDlg::Accept);


	//为6种基准定义方式需选取的特征创建容器，用于分组布局
	//1.点偏移
	page1 = new QWidget(this);
	page1Layout = new QVBoxLayout;
	feature11 = new QLabel("顶点：");
	feature11->setMinimumHeight(20);
	value11 = new FeatureButton(this);
	value11->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value11->setStyleSheet(BtnStyle);
	value11->setMinimumHeight(25);
	connect(value11, &FeatureButton::clicked, myview, &View::createSelectCmd);
	//value11->setObjectName("value");
	//value11->installEventFilter(this);
	//connect(value11, SIGNAL(clicked()), this, SLOT(pointClickEvent()));
	feature12 = new QLabel("向量：");
	feature12->setMinimumHeight(20);

	QLabel* xLabel = new QLabel("x:");
	xLabel->setFixedSize(10, 25);
	xSpinbox = new QDoubleSpinBox(this);
	double MaxRange = 100000.0;
	xSpinbox->setRange(-MaxRange, MaxRange);
	xSpinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	xSpinbox->setAlignment(Qt::AlignCenter);
	xSpinbox->setDecimals(3);
	QLabel* yLabel = new QLabel("y:");
	yLabel->setFixedSize(10, 25);
	ySpinbox = new QDoubleSpinBox(this);
	ySpinbox->setRange(-MaxRange, MaxRange);
	ySpinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	ySpinbox->setAlignment(Qt::AlignCenter);
	ySpinbox->setDecimals(3);
	QLabel* zLabel = new QLabel("z:");
	zLabel->setFixedSize(10, 25);
	zSpinbox = new QDoubleSpinBox(this);
	zSpinbox->setRange(-MaxRange, MaxRange);
	zSpinbox->setButtonSymbols(QAbstractSpinBox::NoButtons);
	zSpinbox->setAlignment(Qt::AlignCenter);
	zSpinbox->setDecimals(3);
	vecLayout = new QHBoxLayout(this);
	vecLayout->addWidget(xLabel);
	vecLayout->addWidget(xSpinbox);
	vecLayout->addWidget(yLabel);
	vecLayout->addWidget(ySpinbox);
	vecLayout->addWidget(zLabel);
	vecLayout->addWidget(zSpinbox);

	page1Layout->addWidget(feature11);
	page1Layout->addWidget(value11);
	page1Layout->addWidget(feature12);
	page1Layout->addLayout(vecLayout);
	page1->setLayout(page1Layout);
	page1Layout->setContentsMargins(0, 0, 0, 0);
	//2.面中心
	page2 = new QWidget(this);
	page2Layout = new QVBoxLayout;
	feature21 = new QLabel("面：");
	feature21->setMinimumHeight(25);
	value21 = new FeatureButton(this);
	value21->setFeatureType(TopAbs_ShapeEnum::TopAbs_FACE);
	value21->setStyleSheet(BtnStyle);
	value21->setMinimumHeight(25);
	connect(value21, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page2Layout->addWidget(feature21);
	page2Layout->addWidget(value21);
	page2Layout->addStretch();
	page2Layout->setContentsMargins(0, 0, 0, 0);
	page2->setLayout(page2Layout);
	//3.交叉点
	page3 = new QWidget(this);
	page3Layout = new QVBoxLayout;
	feature31 = new QLabel("边：");
	feature31->setMinimumHeight(25);
	value31 = new FeatureButton(this);
	value31->setFeatureType(TopAbs_ShapeEnum::TopAbs_EDGE);
	value31->setStyleSheet(BtnStyle);
	value31->setMinimumHeight(25);
	connect(value31, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature32 = new QLabel("面：");
	feature32->setMinimumHeight(25);
	value32 = new FeatureButton(this);
	value32->setFeatureType(TopAbs_ShapeEnum::TopAbs_FACE);
	value32->setStyleSheet(BtnStyle);
	value32->setMinimumHeight(25);
	connect(value32, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page3Layout->addWidget(feature31);
	page3Layout->addWidget(value31);
	page3Layout->addWidget(feature32);
	page3Layout->addWidget(value32);
	page3Layout->setContentsMargins(0, 0, 0, 0);
	page3->setLayout(page3Layout);
	//4.中点
	page4 = new QWidget(this);
	page4Layout = new QVBoxLayout;
	feature41 = new QLabel("顶点：");
	feature41->setMinimumHeight(25);
	value41 = new FeatureButton(this);
	value41->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value41->setStyleSheet(BtnStyle);
	value41->setMinimumHeight(25);
	connect(value41, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature42 = new QLabel("顶点：");
	feature42->setMinimumHeight(25);
	value42 = new FeatureButton(this);
	value42->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value42->setStyleSheet(BtnStyle);
	value42->setMinimumHeight(25);
	connect(value42, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page4Layout->addWidget(feature41);
	page4Layout->addWidget(value41);
	page4Layout->addWidget(feature42);
	page4Layout->addWidget(value42);
	page4Layout->setContentsMargins(0, 0, 0, 0);
	page4->setLayout(page4Layout);
	//5.质心
	page5 = new QWidget(this);
	page5Layout = new QVBoxLayout;
	feature51 = new QLabel("体：");
	feature51->setMinimumHeight(25);
	value51 = new FeatureButton(this);
	value51->setFeatureType(TopAbs_ShapeEnum::TopAbs_SOLID);
	value51->setStyleSheet(BtnStyle);
	value51->setMinimumHeight(25);
	connect(value51, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page5Layout->addWidget(feature51);
	page5Layout->addWidget(value51);
	page5Layout->addStretch();
	page5Layout->setContentsMargins(0, 0, 0, 0);
	page5->setLayout(page5Layout);
	//6.投影点
	page6 = new QWidget(this);
	page6Layout = new QVBoxLayout;
	feature61 = new QLabel("顶点：");
	feature61->setMinimumHeight(25);
	value61 = new FeatureButton(this);
	value61->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value61->setStyleSheet(BtnStyle);
	value61->setMinimumHeight(25);
	connect(value61, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature62 = new QLabel("平面：");
	feature62->setMinimumHeight(25);
	value62 = new FeatureButton(this);
	value62->setFeatureType(TopAbs_ShapeEnum::TopAbs_FACE);
	value62->setStyleSheet(BtnStyle);
	value62->setMinimumHeight(25);
	connect(value62, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page6Layout->addWidget(feature61);
	page6Layout->addWidget(value61);
	page6Layout->addWidget(feature62);
	page6Layout->addWidget(value62);
	page6Layout->setContentsMargins(0, 0, 0, 0);
	page6->setLayout(page6Layout);

	StackedWidget = new QStackedLayout(this);
	StackedWidget->addWidget(page1);
	StackedWidget->addWidget(page2);
	StackedWidget->addWidget(page3);
	StackedWidget->addWidget(page4);
	StackedWidget->addWidget(page5);
	StackedWidget->addWidget(page6);
	connect(pointDefType, SIGNAL(activated(int)), StackedWidget, SLOT(setCurrentIndex(int)));

	//添加到布局中
	form->addWidget(defType);
	form->addWidget(pointDefType);
	form->addWidget(selectFeature);
	form->addLayout(StackedWidget);
	form->addLayout(hlayout);

	setLayout(form);
	setFixedWidth(300);
	form->setContentsMargins(20, 20, 20, 20);
}

DatumPointDlg::~DatumPointDlg()
{

}

DocumentComponent* DatumPointDlg::getDocumentFromDlg()
{
	return myDocument3d;
}

void DatumPointDlg::Accept()
{
	//DatumPoint Datum;
	//DatumPoint* DatumPoint = &Datum;
	DatumPoint* DatumPoint = new class DatumPoint();
	
	gp_Dir aX_Direction(1.0, 0.0, 0.0);
	gp_Dir aY_Direction(0.0, 1.0, 0.0);
	gp_Dir aZ_Direction(0.0, 0.0, 1.0);

	//1. 点偏移：由选取的点和输入的偏移向量，计算基准点
	if (pointDefType->currentText() == tr("点偏移"))
	{
		//给基准点结构体赋值，用于文档存储
		DatumPoint->SetDptType(VERTEX_OFFSET);
		DatumPoint->SetShape1Name(value11->getFeatureName().data());
		vector<double> val;
		val.push_back(xSpinbox->value());
		val.push_back(ySpinbox->value());
		val.push_back(zSpinbox->value());
		DatumPoint->SetOffsetValue(val);

		TopoDS_Shape vShape = value11->getFeatureShape();
		if (vShape.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex = TopoDS::Vertex(vShape);
		gp_Pnt theLoc = BRep_Tool::Pnt(Vertex);

		//平移变换
		gp_Trsf trsfTranslate;
		gp_Vec dirTranslate = gp_Vec(xSpinbox->value(), ySpinbox->value(), zSpinbox->value());
		trsfTranslate.SetTranslation(dirTranslate);
		gp_Pnt newLoc = theLoc.Transformed(trsfTranslate);//新点
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(newLoc));//给基准点结构体赋值，用于文档存储

		datPoint = new AIS_Point(new Geom_CartesianPoint(newLoc));

		value11->initFeatureButton();
		xSpinbox->setValue(0.0);
		ySpinbox->setValue(0.0);
		zSpinbox->setValue(0.0);
	}
	//2. 面中心：由选取的面，计算面的中点，即基准点
	if (pointDefType->currentText() == tr("面中心"))
	{
		//给基准点结构体赋值，用于文档存储
		DatumPoint->SetDptType(FACE_CENTER);
		DatumPoint->SetShape1Name(value21->getFeatureName().data());
		
		TopoDS_Shape fShape = value21->getFeatureShape();
		if (fShape.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Face Face = TopoDS::Face(fShape);
		BRepGProp_Face analysisFace(Face);
		gp_Pnt faceCenter;
		Standard_Real umin, umax, vmin, vmax;
		analysisFace.Bounds(umin, umax, vmin, vmax);//获取拓扑面的参数范围
		Standard_Real midU, midV;
		midU = (umin + umax) / 2;//拓扑面的参数中点
		midV = (vmin + vmax) / 2;
		gp_Vec norm;//法向量
		analysisFace.Normal(midU, midV, faceCenter, norm);//返回面参数中点的坐标及其法向
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(faceCenter));//给基准点结构体赋值，用于文档存储

		datPoint = new AIS_Point(new Geom_CartesianPoint(faceCenter));

		value21->initFeatureButton();
	}
	//3. 交叉点：由选取的边和面，计算曲线与平面的交点，即基准点
	if (pointDefType->currentText() == tr("交叉点"))
	{
		//给基准点结构体赋值，用于文档存储
		DatumPoint->SetDptType(INTER_POINT);
		DatumPoint->SetShape1Name(value31->getFeatureName().data());
		DatumPoint->SetShape2Name(value32->getFeatureName().data());
		
		TopoDS_Shape eShape = value31->getFeatureShape();
		TopoDS_Shape fShape = value32->getFeatureShape();
		if (eShape.IsNull() || fShape.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Edge Edge = TopoDS::Edge(eShape);
		TopoDS_Face Face = TopoDS::Face(fShape);
		Standard_Real first, last;
		Handle(Geom_Curve) aGCurve = BRep_Tool::Curve(Edge, first, last);
		Handle(Geom_Surface) aGSurface = BRep_Tool::Surface(Face);

		GeomAPI_IntCS CS(aGCurve, aGSurface);
		Standard_Integer NbPoints = 0;
		if (CS.IsDone())
		{
			NbPoints = CS.NbPoints();
			if (NbPoints == 0)
			{
				QMessageBox::information(NULL, "提示", "交点数为0，请重新选取特征", QMessageBox::Ok);
				return;
			}
			gp_Pnt aPoint = CS.Point(1);
			DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(aPoint));//给基准点结构体赋值，用于文档存储
			datPoint = new AIS_Point(new Geom_CartesianPoint(aPoint));
			//for (int k = 1; k <= NbPoints; k++)
			//{
			//	gp_Pnt aPoint = CS.Point(k);
			//	// do something with the point
			//}

			value31->initFeatureButton();
			value32->initFeatureButton();
		}
		else
		{
			QMessageBox::information(NULL, "提示", "求交失败", QMessageBox::Ok);
			return;
		}
	}
	//4. 中点：由选取的两个点，计算中点作为基准点
	if (pointDefType->currentText() == tr("中点"))
	{
		//给基准点结构体赋值，用于文档存储
		DatumPoint->SetDptType(MID_POINT);
		DatumPoint->SetShape1Name(value41->getFeatureName().data());
		DatumPoint->SetShape2Name(value42->getFeatureName().data());
		
		TopoDS_Shape vShape1 = value41->getFeatureShape();
		TopoDS_Shape vShape2 = value42->getFeatureShape();
		if (vShape1.IsNull() || vShape2.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex1 = TopoDS::Vertex(vShape1);
		TopoDS_Vertex Vertex2 = TopoDS::Vertex(vShape2);
		gp_Pnt point1 = BRep_Tool::Pnt(Vertex1);
		gp_Pnt point2 = BRep_Tool::Pnt(Vertex2);
		gp_Pnt middlePoint = gp_Pnt((point1.X() + point2.X()) / 2, (point1.Y() + point2.Y()) / 2, (point1.Z() + point2.Z()) / 2);
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(middlePoint));//给基准点结构体赋值，用于文档存储
		
		datPoint = new AIS_Point(new Geom_CartesianPoint(middlePoint));

		value41->initFeatureButton();
		value42->initFeatureButton();
	}
	//5. 质心：由选取的体，计算质心基准点
	if (pointDefType->currentText() == tr("质心"))
	{
		//给基准点结构体赋值，用于文档存储
		DatumPoint->SetDptType(CENTRIC_POINT);
		DatumPoint->SetShape1Name(value51->getFeatureName().data());
		
		TopoDS_Shape sShape = value51->getFeatureShape();
		if (sShape.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Solid Solid = TopoDS::Solid(sShape);

		//计算全局属性
		GProp_GProps aGProps;
		BRepGProp::VolumeProperties(Solid, aGProps);
		Standard_Real aVolume = aGProps.Mass();
		gp_Pnt aCOM = aGProps.CentreOfMass();//质心
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(aCOM));//给基准点结构体赋值，用于文档存储

		datPoint = new AIS_Point(new Geom_CartesianPoint(aCOM));

		value51->initFeatureButton();
	}
	//6. 投影点：由选取的点和面，计算点到面的投影点作为基准点
	if (pointDefType->currentText() == tr("投影点"))
	{
		//给基准点结构体赋值，用于文档存储
		DatumPoint->SetDptType(PROJECT_POINT);
		DatumPoint->SetShape1Name(value61->getFeatureName().data());
		DatumPoint->SetShape2Name(value62->getFeatureName().data());
		
		TopoDS_Shape vShape = value61->getFeatureShape();
		TopoDS_Shape fShape = value62->getFeatureShape();
		if (vShape.IsNull() || fShape.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex = TopoDS::Vertex(vShape);
		TopoDS_Face Face = TopoDS::Face(fShape);
		gp_Pnt point = BRep_Tool::Pnt(Vertex);
		Handle(Geom_Surface) aGSurface = BRep_Tool::Surface(Face);
		Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aGSurface);
		if (aPlane.IsNull())
		{
			QMessageBox::information(NULL, "提示", "选取的特征面须为平面", QMessageBox::Ok);
			return;
		}
		gp_Pln agpPlane = aPlane->Pln();       //将TopoDS_Face面转换为gp_Pln平面
		gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(agpPlane, point);//投影的二维坐标
		gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
			ConvertedPointOnPlane.Y(), agpPlane);//将二维坐标转换为基于OCC三维坐标选定面上的坐标
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(ResultPoint));//给基准点结构体赋值，用于文档存储

		datPoint = new AIS_Point(new Geom_CartesianPoint(ResultPoint));

		value61->initFeatureButton();
		value62->initFeatureButton();
	}

	myDocument3d->AddDatumPoint(DatumPoint->GetShape(), DatumPoint);//增加参考点，存储到文档
	delete DatumPoint;
	DatumPoint = NULL;
	//myContext->SetColor(datPoint, Quantity_NOC_YELLOW, Standard_True);
	//myContext->Display(datPoint, Standard_True);
	////myContext->UpdateCurrentViewer();
	myDocument3d->UpdateDisplay();

	//QDialog::close();
}


//bool DatumPointDlg::eventFilter(QObject* obj, QEvent* event)
//{
//	if (obj==value11)
//	{
//		if (event->type() == QEvent::ContextMenu)  // 82  鼠标右击事件 ContextMenu
//		{
//			QMessageBox::information(NULL, "提示", "右键", QMessageBox::Ok);
//			return true;
//		}
//		else
//		{
//			QMouseEvent* me = (QMouseEvent*)event;
//			if (me->button() == Qt::LeftButton)
//			{
//				QMessageBox::information(NULL, "提示", "左键", QMessageBox::Ok);
//				return true;
//			}
//
//		}
//	}
//
//	//if (event->type() == 4)  // 82  鼠标右击事件 ContextMe
//	//{
//	//	QMessageBox::information(NULL, "提示", "双击", QMessageBox::Ok);
//	//}
//	//if (event->type() == 3)  // 82  鼠标右击事件 ContextMe
//	//{
//	//	QMessageBox::information(NULL, "提示", "release", QMessageBox::Ok);
//	//}
//	return QWidget::eventFilter(obj, event);
//}