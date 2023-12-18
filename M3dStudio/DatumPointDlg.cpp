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
	setWindowTitle(tr("��׼�㶨��"));
	BtnStyle = QString(
		//����״̬
		"QPushButton{"
		"background-color:white;"//������ɫ��͸����
		"color:black;"//������ɫ
		"border: 2px inset rgb(128,128,128);"//�߿��ȡ���ʽ�Լ���ɫ
		"font: 18px;"//�������ͺ������С
		"text-align: left;"
		"}"
		//����ʱ��״̬
		"QPushButton:pressed{"
		"background-color:rgb(255,255,255);"
		"color:rgb(0,0��0);"
		"}"
		//������
		"QComboBox{"
		"font: 18px;"
		"}");

	form = new QVBoxLayout(this);
	defType = new QLabel("���巽ʽ "); //���巽ʽ ������
	font = defType->font();//�Ӵ�����
	font.setBold(true);
	defType->setFont(font);
	defType->setMinimumHeight(25);
	pointDefType = new QComboBox(this);
	pointDefType->addItem("��ƫ��");
	pointDefType->addItem("������");
	pointDefType->addItem("�����");
	pointDefType->addItem("�е�");
	pointDefType->addItem("����");
	pointDefType->addItem("ͶӰ��");
	pointDefType->setStyleSheet(BtnStyle);
	pointDefType->setMinimumHeight(25);
	selectFeature = new QLabel("����ѡȡ");
	selectFeature->setFont(font);
	selectFeature->setMinimumHeight(25);

	//����ok��cancel
	okBtn = new QPushButton(this);
	okBtn->setText("����");
	cancelBtn = new QPushButton(this);
	cancelBtn->setText("�ر�");
	hlayout = new QHBoxLayout(this);
	hlayout->addWidget(okBtn);
	hlayout->addWidget(cancelBtn);
	connect(cancelBtn, &QPushButton::clicked, this, &QDialog::close);
	connect(okBtn, &QPushButton::clicked, this, &DatumPointDlg::Accept);


	//Ϊ6�ֻ�׼���巽ʽ��ѡȡ�������������������ڷ��鲼��
	//1.��ƫ��
	page1 = new QWidget(this);
	page1Layout = new QVBoxLayout;
	feature11 = new QLabel("���㣺");
	feature11->setMinimumHeight(20);
	value11 = new FeatureButton(this);
	value11->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value11->setStyleSheet(BtnStyle);
	value11->setMinimumHeight(25);
	connect(value11, &FeatureButton::clicked, myview, &View::createSelectCmd);
	//value11->setObjectName("value");
	//value11->installEventFilter(this);
	//connect(value11, SIGNAL(clicked()), this, SLOT(pointClickEvent()));
	feature12 = new QLabel("������");
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
	//2.������
	page2 = new QWidget(this);
	page2Layout = new QVBoxLayout;
	feature21 = new QLabel("�棺");
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
	//3.�����
	page3 = new QWidget(this);
	page3Layout = new QVBoxLayout;
	feature31 = new QLabel("�ߣ�");
	feature31->setMinimumHeight(25);
	value31 = new FeatureButton(this);
	value31->setFeatureType(TopAbs_ShapeEnum::TopAbs_EDGE);
	value31->setStyleSheet(BtnStyle);
	value31->setMinimumHeight(25);
	connect(value31, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature32 = new QLabel("�棺");
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
	//4.�е�
	page4 = new QWidget(this);
	page4Layout = new QVBoxLayout;
	feature41 = new QLabel("���㣺");
	feature41->setMinimumHeight(25);
	value41 = new FeatureButton(this);
	value41->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value41->setStyleSheet(BtnStyle);
	value41->setMinimumHeight(25);
	connect(value41, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature42 = new QLabel("���㣺");
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
	//5.����
	page5 = new QWidget(this);
	page5Layout = new QVBoxLayout;
	feature51 = new QLabel("�壺");
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
	//6.ͶӰ��
	page6 = new QWidget(this);
	page6Layout = new QVBoxLayout;
	feature61 = new QLabel("���㣺");
	feature61->setMinimumHeight(25);
	value61 = new FeatureButton(this);
	value61->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value61->setStyleSheet(BtnStyle);
	value61->setMinimumHeight(25);
	connect(value61, &FeatureButton::clicked, myview, &View::createSelectCmd);
	feature62 = new QLabel("ƽ�棺");
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

	//��ӵ�������
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

	//1. ��ƫ�ƣ���ѡȡ�ĵ�������ƫ�������������׼��
	if (pointDefType->currentText() == tr("��ƫ��"))
	{
		//����׼��ṹ�帳ֵ�������ĵ��洢
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
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex = TopoDS::Vertex(vShape);
		gp_Pnt theLoc = BRep_Tool::Pnt(Vertex);

		//ƽ�Ʊ任
		gp_Trsf trsfTranslate;
		gp_Vec dirTranslate = gp_Vec(xSpinbox->value(), ySpinbox->value(), zSpinbox->value());
		trsfTranslate.SetTranslation(dirTranslate);
		gp_Pnt newLoc = theLoc.Transformed(trsfTranslate);//�µ�
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(newLoc));//����׼��ṹ�帳ֵ�������ĵ��洢

		datPoint = new AIS_Point(new Geom_CartesianPoint(newLoc));

		value11->initFeatureButton();
		xSpinbox->setValue(0.0);
		ySpinbox->setValue(0.0);
		zSpinbox->setValue(0.0);
	}
	//2. �����ģ���ѡȡ���棬��������е㣬����׼��
	if (pointDefType->currentText() == tr("������"))
	{
		//����׼��ṹ�帳ֵ�������ĵ��洢
		DatumPoint->SetDptType(FACE_CENTER);
		DatumPoint->SetShape1Name(value21->getFeatureName().data());
		
		TopoDS_Shape fShape = value21->getFeatureShape();
		if (fShape.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Face Face = TopoDS::Face(fShape);
		BRepGProp_Face analysisFace(Face);
		gp_Pnt faceCenter;
		Standard_Real umin, umax, vmin, vmax;
		analysisFace.Bounds(umin, umax, vmin, vmax);//��ȡ������Ĳ�����Χ
		Standard_Real midU, midV;
		midU = (umin + umax) / 2;//������Ĳ����е�
		midV = (vmin + vmax) / 2;
		gp_Vec norm;//������
		analysisFace.Normal(midU, midV, faceCenter, norm);//����������е�����꼰�䷨��
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(faceCenter));//����׼��ṹ�帳ֵ�������ĵ��洢

		datPoint = new AIS_Point(new Geom_CartesianPoint(faceCenter));

		value21->initFeatureButton();
	}
	//3. ����㣺��ѡȡ�ıߺ��棬����������ƽ��Ľ��㣬����׼��
	if (pointDefType->currentText() == tr("�����"))
	{
		//����׼��ṹ�帳ֵ�������ĵ��洢
		DatumPoint->SetDptType(INTER_POINT);
		DatumPoint->SetShape1Name(value31->getFeatureName().data());
		DatumPoint->SetShape2Name(value32->getFeatureName().data());
		
		TopoDS_Shape eShape = value31->getFeatureShape();
		TopoDS_Shape fShape = value32->getFeatureShape();
		if (eShape.IsNull() || fShape.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
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
				QMessageBox::information(NULL, "��ʾ", "������Ϊ0��������ѡȡ����", QMessageBox::Ok);
				return;
			}
			gp_Pnt aPoint = CS.Point(1);
			DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(aPoint));//����׼��ṹ�帳ֵ�������ĵ��洢
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
			QMessageBox::information(NULL, "��ʾ", "��ʧ��", QMessageBox::Ok);
			return;
		}
	}
	//4. �е㣺��ѡȡ�������㣬�����е���Ϊ��׼��
	if (pointDefType->currentText() == tr("�е�"))
	{
		//����׼��ṹ�帳ֵ�������ĵ��洢
		DatumPoint->SetDptType(MID_POINT);
		DatumPoint->SetShape1Name(value41->getFeatureName().data());
		DatumPoint->SetShape2Name(value42->getFeatureName().data());
		
		TopoDS_Shape vShape1 = value41->getFeatureShape();
		TopoDS_Shape vShape2 = value42->getFeatureShape();
		if (vShape1.IsNull() || vShape2.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex1 = TopoDS::Vertex(vShape1);
		TopoDS_Vertex Vertex2 = TopoDS::Vertex(vShape2);
		gp_Pnt point1 = BRep_Tool::Pnt(Vertex1);
		gp_Pnt point2 = BRep_Tool::Pnt(Vertex2);
		gp_Pnt middlePoint = gp_Pnt((point1.X() + point2.X()) / 2, (point1.Y() + point2.Y()) / 2, (point1.Z() + point2.Z()) / 2);
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(middlePoint));//����׼��ṹ�帳ֵ�������ĵ��洢
		
		datPoint = new AIS_Point(new Geom_CartesianPoint(middlePoint));

		value41->initFeatureButton();
		value42->initFeatureButton();
	}
	//5. ���ģ���ѡȡ���壬�������Ļ�׼��
	if (pointDefType->currentText() == tr("����"))
	{
		//����׼��ṹ�帳ֵ�������ĵ��洢
		DatumPoint->SetDptType(CENTRIC_POINT);
		DatumPoint->SetShape1Name(value51->getFeatureName().data());
		
		TopoDS_Shape sShape = value51->getFeatureShape();
		if (sShape.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Solid Solid = TopoDS::Solid(sShape);

		//����ȫ������
		GProp_GProps aGProps;
		BRepGProp::VolumeProperties(Solid, aGProps);
		Standard_Real aVolume = aGProps.Mass();
		gp_Pnt aCOM = aGProps.CentreOfMass();//����
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(aCOM));//����׼��ṹ�帳ֵ�������ĵ��洢

		datPoint = new AIS_Point(new Geom_CartesianPoint(aCOM));

		value51->initFeatureButton();
	}
	//6. ͶӰ�㣺��ѡȡ�ĵ���棬����㵽���ͶӰ����Ϊ��׼��
	if (pointDefType->currentText() == tr("ͶӰ��"))
	{
		//����׼��ṹ�帳ֵ�������ĵ��洢
		DatumPoint->SetDptType(PROJECT_POINT);
		DatumPoint->SetShape1Name(value61->getFeatureName().data());
		DatumPoint->SetShape2Name(value62->getFeatureName().data());
		
		TopoDS_Shape vShape = value61->getFeatureShape();
		TopoDS_Shape fShape = value62->getFeatureShape();
		if (vShape.IsNull() || fShape.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex = TopoDS::Vertex(vShape);
		TopoDS_Face Face = TopoDS::Face(fShape);
		gp_Pnt point = BRep_Tool::Pnt(Vertex);
		Handle(Geom_Surface) aGSurface = BRep_Tool::Surface(Face);
		Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aGSurface);
		if (aPlane.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "ѡȡ����������Ϊƽ��", QMessageBox::Ok);
			return;
		}
		gp_Pln agpPlane = aPlane->Pln();       //��TopoDS_Face��ת��Ϊgp_Plnƽ��
		gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(agpPlane, point);//ͶӰ�Ķ�ά����
		gp_Pnt ResultPoint = ElSLib::Value(ConvertedPointOnPlane.X(),
			ConvertedPointOnPlane.Y(), agpPlane);//����ά����ת��Ϊ����OCC��ά����ѡ�����ϵ�����
		DatumPoint->SetShape(BRepBuilderAPI_MakeVertex(ResultPoint));//����׼��ṹ�帳ֵ�������ĵ��洢

		datPoint = new AIS_Point(new Geom_CartesianPoint(ResultPoint));

		value61->initFeatureButton();
		value62->initFeatureButton();
	}

	myDocument3d->AddDatumPoint(DatumPoint->GetShape(), DatumPoint);//���Ӳο��㣬�洢���ĵ�
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
//		if (event->type() == QEvent::ContextMenu)  // 82  ����һ��¼� ContextMenu
//		{
//			QMessageBox::information(NULL, "��ʾ", "�Ҽ�", QMessageBox::Ok);
//			return true;
//		}
//		else
//		{
//			QMouseEvent* me = (QMouseEvent*)event;
//			if (me->button() == Qt::LeftButton)
//			{
//				QMessageBox::information(NULL, "��ʾ", "���", QMessageBox::Ok);
//				return true;
//			}
//
//		}
//	}
//
//	//if (event->type() == 4)  // 82  ����һ��¼� ContextMe
//	//{
//	//	QMessageBox::information(NULL, "��ʾ", "˫��", QMessageBox::Ok);
//	//}
//	//if (event->type() == 3)  // 82  ����һ��¼� ContextMe
//	//{
//	//	QMessageBox::information(NULL, "��ʾ", "release", QMessageBox::Ok);
//	//}
//	return QWidget::eventFilter(obj, event);
//}