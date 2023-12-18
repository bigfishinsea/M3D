#include "M3d.h"
#include "GeomWidget.h"
#include "DatumLineDlg.h"
#include "DatumPointDlg.h"
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>
#include <Geom_Line.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <GeomAPI_IntSS.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <gp_Cylinder.hxx>
#include <Geom_CartesianPoint.hxx>

#include<qpushbutton.h>
#include <QMessageBox>

#include "mainwindow.h"
//#include "M3dCom_DS.h"

DatumLineDlg::DatumLineDlg(QWidget* parent) :QDialog(parent)
{
	myDocument3d = (DocumentComponent*)qobject_cast<MainWindow*>(parent)->getDocument();
	//pCom = myDocument3d->GetComDS();
	myContext = myDocument3d->getContext();
	myview = qobject_cast<MainWindow*>(parent)->getGeomWidget()->GetView();
	dlDlgInit();
}

void DatumLineDlg::dlDlgInit()
{
	setWindowTitle(tr("基准轴定义"));
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
	lineDefType = new QComboBox(this);
	lineDefType->addItem("线偏移");
	lineDefType->addItem("经过两点");
	lineDefType->addItem("圆柱轴线");
	lineDefType->addItem("面交线");
	lineDefType->addItem("面垂直");
	lineDefType->setStyleSheet(BtnStyle);
	lineDefType->setMinimumHeight(25);
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
	connect(okBtn, &QPushButton::clicked, this, &DatumLineDlg::Accept);

	//为5种基准定义方式需选取的特征创建容器，用于分组布局
	//1.线偏移
	page1 = new QWidget(this);
	page1Layout = new QVBoxLayout;
	feature11 = new QLabel("边：");
	feature11->setMinimumHeight(20);
	value11 = new FeatureButton(this);
	value11->setFeatureType(TopAbs_ShapeEnum::TopAbs_EDGE);
	value11->setStyleSheet(BtnStyle);
	value11->setMinimumHeight(25);
	connect(value11, &FeatureButton::clicked, myview, &View::createSelectCmd);
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
	//2.经过两点
	page2 = new QWidget(this);
	page2Layout = new QVBoxLayout;
	feature21 = new QLabel("顶点：");
	feature21->setMinimumHeight(25);
	value21 = new FeatureButton(this);
	value21->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value21->setStyleSheet(BtnStyle);
	value21->setMinimumHeight(25);
	connect(value21, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature22 = new QLabel("顶点：");
	feature22->setMinimumHeight(25);
	value22 = new FeatureButton(this);
	value22->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value22->setStyleSheet(BtnStyle);
	value22->setMinimumHeight(25);
	connect(value22, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page2Layout->addWidget(feature21);
	page2Layout->addWidget(value21);
	page2Layout->addWidget(feature22);
	page2Layout->addWidget(value22);
	page2Layout->setContentsMargins(0, 0, 0, 0);
	page2->setLayout(page2Layout);
	//3.圆柱轴线
	page3 = new QWidget(this);
	page3Layout = new QVBoxLayout;
	feature31 = new QLabel("圆柱面：");
	feature31->setMinimumHeight(25);
	value31 = new FeatureButton(this);
	value31->setFeatureType(TopAbs_ShapeEnum::TopAbs_FACE);
	value31->setStyleSheet(BtnStyle);
	value31->setMinimumHeight(25);
	connect(value31, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page3Layout->addWidget(feature31);
	page3Layout->addWidget(value31);
	page3Layout->addStretch();
	page3Layout->setContentsMargins(0, 0, 0, 0);
	page3->setLayout(page3Layout);
	//4.面交线
	page4 = new QWidget(this);
	page4Layout = new QVBoxLayout;
	feature41 = new QLabel("面：");
	feature41->setMinimumHeight(25);
	value41 = new FeatureButton(this);
	value41->setFeatureType(TopAbs_ShapeEnum::TopAbs_FACE);
	value41->setStyleSheet(BtnStyle);
	value41->setMinimumHeight(25);
	connect(value41, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature42 = new QLabel("面：");
	feature42->setMinimumHeight(25);
	value42 = new FeatureButton(this);
	value42->setFeatureType(TopAbs_ShapeEnum::TopAbs_FACE);
	value42->setStyleSheet(BtnStyle);
	value42->setMinimumHeight(25);
	connect(value42, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page4Layout->addWidget(feature41);
	page4Layout->addWidget(value41);
	page4Layout->addWidget(feature42);
	page4Layout->addWidget(value42);
	page4Layout->setContentsMargins(0, 0, 0, 0);
	page4->setLayout(page4Layout);
	//5.面垂直
	page5 = new QWidget(this);
	page5Layout = new QVBoxLayout;
	feature51 = new QLabel("顶点：");
	feature51->setMinimumHeight(25);
	value51 = new FeatureButton(this);
	value51->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value51->setStyleSheet(BtnStyle);
	value51->setMinimumHeight(25);
	connect(value51, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature52 = new QLabel("平面：");
	feature52->setMinimumHeight(25);
	value52 = new FeatureButton(this);
	value52->setFeatureType(TopAbs_ShapeEnum::TopAbs_FACE);
	value52->setStyleSheet(BtnStyle);
	value52->setMinimumHeight(25);
	connect(value52, &FeatureButton::clicked, myview, &View::createSelectCmd);
	page5Layout->addWidget(feature51);
	page5Layout->addWidget(value51);
	page5Layout->addWidget(feature52);
	page5Layout->addWidget(value52);
	page5Layout->setContentsMargins(0, 0, 0, 0);
	page5->setLayout(page5Layout);

	StackedWidget = new QStackedLayout(this);
	StackedWidget->addWidget(page1);
	StackedWidget->addWidget(page2);
	StackedWidget->addWidget(page3);
	StackedWidget->addWidget(page4);
	StackedWidget->addWidget(page5);
	connect(lineDefType, SIGNAL(activated(int)), StackedWidget, SLOT(setCurrentIndex(int)));

	//添加到布局中
	form->addWidget(defType);
	form->addWidget(lineDefType);
	form->addWidget(selectFeature);
	form->addLayout(StackedWidget);
	form->addLayout(hlayout);

	setLayout(form);
	setFixedWidth(300);
	form->setContentsMargins(20, 20, 20, 20);
}

DatumLineDlg::~DatumLineDlg()
{

}

DocumentComponent* DatumLineDlg::getDocumentFromDlg()
{
	return myDocument3d;
}

void DatumLineDlg::Accept()
{
	DatumLine* DatumLine = new class DatumLine();
	
	gp_Dir aX_Direction(1.0, 0.0, 0.0);
	gp_Dir aY_Direction(0.0, 1.0, 0.0);
	gp_Dir aZ_Direction(0.0, 0.0, 1.0);

	//1. 线偏移：由选取的边和输入的偏移向量，计算基准轴
	if (lineDefType->currentText() == tr("线偏移"))
	{
		//给基准轴结构体赋值，用于文档存储
		DatumLine->SetDlnType(EDGE_OFFSET);
		DatumLine->SetShape1Name(value11->getFeatureName().data());
		vector<double> val;
		val.push_back(xSpinbox->value());
		val.push_back(ySpinbox->value());
		val.push_back(zSpinbox->value());
		DatumLine->SetOffsetValue(val);
		
		TopoDS_Shape eShape = value11->getFeatureShape();
		if (eShape.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Edge Edge = TopoDS::Edge(eShape);
		Standard_Real first, last;
		Handle(Geom_Curve) aCurve = BRep_Tool::Curve(Edge, first, last);
		gp_Pnt StrPnt = aCurve->Value(first); //EDGE的首位端点
		gp_Pnt EndPnt = aCurve->Value(last);
		gp_Vec Vector = gp_Vec(StrPnt, EndPnt);
		gp_Dir dir = gp_Dir(Vector);
		gp_Lin theLine = gp_Lin(StrPnt, dir);//直线经过EDGE的首位端点

		//平移变换
		gp_Trsf trsfTranslate;
		gp_Vec dirTranslate = gp_Vec(xSpinbox->value(), ySpinbox->value(), zSpinbox->value());
		trsfTranslate.SetTranslation(dirTranslate);
		gp_Lin newLine = theLine.Transformed(trsfTranslate);//新线
		DatumLine->SetShape(BRepBuilderAPI_MakeEdge(newLine, first, last));//给基准轴结构体赋值，用于文档存储

		value11->initFeatureButton();
		xSpinbox->setValue(0.0);
		ySpinbox->setValue(0.0);
		zSpinbox->setValue(0.0);
	}
	//2. 经过两点：由选取的两个顶点，计算经过两点的基准轴
	if (lineDefType->currentText() == tr("经过两点"))
	{
		//给基准轴结构体赋值，用于文档存储
		DatumLine->SetDlnType(THROUGH2POINTS);
		DatumLine->SetShape1Name(value21->getFeatureName().data());
		DatumLine->SetShape2Name(value22->getFeatureName().data());
		
		TopoDS_Shape vShape1 = value21->getFeatureShape();
		TopoDS_Shape vShape2 = value22->getFeatureShape();
		if (vShape1.IsNull() || vShape2.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex1 = TopoDS::Vertex(vShape1);
		TopoDS_Vertex Vertex2 = TopoDS::Vertex(vShape2);
		gp_Pnt point1 = BRep_Tool::Pnt(Vertex1);
		gp_Pnt point2 = BRep_Tool::Pnt(Vertex2);
		gp_Vec Vector = gp_Vec(point1, point2);
		gp_Dir dir = gp_Dir(Vector);
		gp_Lin Line = gp_Lin(point1, dir);
		Standard_Real distance = point1.Distance(point2);
		DatumLine->SetShape(BRepBuilderAPI_MakeEdge(Line, -0.5* distance, 1.5* distance));

		value21->initFeatureButton();
		value22->initFeatureButton();
	}
	//3. 圆柱轴线：由选取的圆柱面，计算圆柱体轴线作为基准轴
	if (lineDefType->currentText() == tr("圆柱轴线"))
	{
		//给基准轴结构体赋值，用于文档存储
		DatumLine->SetDlnType(CYLINDER_AXIS);
		DatumLine->SetShape1Name(value31->getFeatureName().data());
		
		TopoDS_Shape fShape = value31->getFeatureShape();
		if (fShape.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Face Face = TopoDS::Face(fShape);
		Handle(Geom_Surface) cylindrical = BRep_Tool::Surface(Face);
		Handle(Geom_CylindricalSurface) aCylindrical = Handle(Geom_CylindricalSurface)::DownCast(cylindrical);
		if (aCylindrical.IsNull())
		{
			QMessageBox::information(NULL, "提示", "选取的特征面须为圆柱面", QMessageBox::Ok);
			return;
		}
		gp_Cylinder Cylinder = aCylindrical->Cylinder();
		gp_Dir CylinderAxisDir = Cylinder.Axis().Direction();
		gp_Pnt Loc = Cylinder.Location();

		gp_Lin Line = gp_Lin(Loc, CylinderAxisDir);
		BRepGProp_Face analysisFace(Face);
		Standard_Real umin, umax, vmin, vmax;
		analysisFace.Bounds(umin, umax, vmin, vmax);//获取拓扑面的参数范围

		DatumLine->SetShape(BRepBuilderAPI_MakeEdge(Line, vmin-0.25*(vmax- vmin), vmax+ 0.25 * (vmax - vmin)));//给基准轴结构体赋值，用于文档存储
		
		value31->initFeatureButton();
	}
	//4. 面交线：由选取的两个面，计算交线作为基准轴
	if (lineDefType->currentText() == tr("面交线"))
	{
		//给基准轴结构体赋值，用于文档存储
		DatumLine->SetDlnType(FACES_INTER);
		DatumLine->SetShape1Name(value41->getFeatureName().data());
		DatumLine->SetShape2Name(value42->getFeatureName().data());
		
		TopoDS_Shape fShape1 = value41->getFeatureShape();
		TopoDS_Shape fShape2 = value42->getFeatureShape();
		if (fShape1.IsNull() || fShape2.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Face Face1 = TopoDS::Face(fShape1);
		TopoDS_Face Face2 = TopoDS::Face(fShape2);
		Handle(Geom_Surface) aFirstPlane = BRep_Tool::Surface(Face1);
		Handle(Geom_Surface) aSecondPlane = BRep_Tool::Surface(Face2);
		GeomAPI_IntSS aPlaneIntersector(aFirstPlane, aSecondPlane, Precision::Confusion());

		if (!aPlaneIntersector.IsDone() || aPlaneIntersector.NbLines() == 0)
		{
			QMessageBox::information(NULL, "提示", "求交失败，请重新选择特征", QMessageBox::Ok);
			return;
		}
		else
		{
			// 获得交线
			Handle(Geom_Curve) anIntersectCurve = aPlaneIntersector.Line(1);
			Handle(Geom_Line) anIntersectLine = Handle(Geom_Line)::DownCast(anIntersectCurve);
			if (anIntersectLine.IsNull())
			{
				QMessageBox::information(NULL, "提示", "交线不为直线，请重新选择特征", QMessageBox::Ok);
				return;
			}

			gp_Lin anIntersectLin = anIntersectLine->Lin();
			DatumLine->SetShape(BRepBuilderAPI_MakeEdge(anIntersectLin, -100,100));//给基准轴结构体赋值，用于文档存储

			value41->initFeatureButton();
			value42->initFeatureButton();
		}
	}
	//5. 面垂直：由选取的顶点和平面，计算垂直于面且经过点的轴线作为基准轴
	if (lineDefType->currentText() == tr("面垂直"))
	{
		//给基准轴结构体赋值，用于文档存储
		DatumLine->SetDlnType(FACES_INTER);
		DatumLine->SetShape1Name(value51->getFeatureName().data());
		DatumLine->SetShape2Name(value52->getFeatureName().data());
		
		TopoDS_Shape vShape = value51->getFeatureShape();
		TopoDS_Shape fShape = value52->getFeatureShape();
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

		BRepGProp_Face analysisFace(Face);
		gp_Pnt faceCenter;
		Standard_Real umin, umax, vmin, vmax;
		analysisFace.Bounds(umin, umax, vmin, vmax);//获取拓扑面的参数范围
		Standard_Real midU, midV;
		midU = (umin + umax) / 2;//拓扑面的参数中点
		midV = (vmin + vmax) / 2;
		gp_Vec norm;//法向量
		analysisFace.Normal(midU, midV, faceCenter, norm);//返回面参数中点的坐标及其法向

		gp_Dir dir = gp_Dir(norm);
		gp_Lin Line = gp_Lin(point, dir);
		gp_Pln plane = aPlane->Pln();
		Standard_Real distance = plane.Distance(point);
		if (distance == 0)
		{
			distance = 1;
		}
		DatumLine->SetShape(BRepBuilderAPI_MakeEdge(Line, -0.5*distance, 1.5*distance));//给基准轴结构体赋值，用于文档存储

		value51->initFeatureButton();
		value52->initFeatureButton();
	}
	datLine = new AIS_Shape(DatumLine->GetShape());
	myDocument3d->AddDatumLine(DatumLine->GetShape(), DatumLine);//增加参考点，存储到文档
	delete DatumLine;
	DatumLine = NULL;
	////设置线型：黄色，点画线
	//Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_YELLOW, Aspect_TOL_DOTDASH, 1);
	//datLine->Attributes()->SetWireAspect(lineAspect);
	//myContext->Display(datLine, Standard_True);
	////myContext->UpdateCurrentViewer();
	myDocument3d->UpdateDisplay();

	//QDialog::close();
}