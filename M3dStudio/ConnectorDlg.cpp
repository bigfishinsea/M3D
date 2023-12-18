#include "M3d.h"
#include "GeomWidget.h"
#include "ConnectorDlg.h"
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Solid.hxx>
#include <Geom_Line.hxx>
#include <BRepGProp_Face.hxx>
#include <GeomAPI_IntSS.hxx>
#include <Geom_Plane.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <ProjLib.hxx>
#include <ElSLib.hxx>
#include <GeomAPI_IntCS.hxx>

#include<qpushbutton.h>
#include <QMessageBox>
#include "mainwindow.h"
#include <algorithm>
#include "ModelicaModel.h"


vector<string> Frame_ModelicaType;
vector<string> Flange_ModelicaType;
vector<string> oneD_ModelicaType;
vector<string> DefinedConnName;

ConnectorDlg::ConnectorDlg(QWidget* parent) :QDialog(parent)
{
	myDocument3d = (DocumentComponent*)qobject_cast<MainWindow*>(parent)->getDocument();
	myContext = myDocument3d->getContext();
	//m_pComDS = myDocument3d->GetComDS();
	m_cyberInfo = myDocument3d->GetCyberInfo();

	if (!m_cyberInfo)
	{
		QMessageBox msgBox;
		msgBox.setText("未定义模型信息！");
		msgBox.exec();
		return;
	}

	ConnectorNames = m_cyberInfo->GetConnectorNames();// 接口名
	ConnectorTypes = m_cyberInfo->GetConnectorTypes();// 接口类型

	//Add by Wu
	string sMdlName = m_cyberInfo->GetMdlName();
	ModelicaModel* pMdl = ModelicaModel::LookupModel(sMdlName.c_str());
	if (pMdl && pMdl->m_sType == "connector")
	{
		ConnectorNames.push_back("MainConnector");
		ConnectorTypes.push_back(sMdlName);
	}

	int connNum = ConnectorNames.size();

	//DefinedConnName存储已定义过的接口名
	vector<Connector*> vConnectors = myDocument3d->GetAllConnectors();
	vector<string>().swap(DefinedConnName);//清除元素并回收内存
	for (int i = 0; i < vConnectors.size(); i++)
	{
		Connector* conn = vConnectors[i];
		DefinedConnName.push_back(conn->GetConnName());
	}

	if (m_cyberInfo == nullptr)
	{
		QMessageBox::information(NULL, "提示", "请导入Modelica文件并保存", QMessageBox::Ok);
		QTimer::singleShot(0, this, SLOT(close()));
	}
	else if(DefinedConnName.size()== connNum)
	{
		QMessageBox::information(NULL, "提示", "接口已全部定义", QMessageBox::Ok);
		QTimer::singleShot(0, this, SLOT(close()));
	}
	else
	{
		connDlgInit();
	}
}

void ConnectorDlg::connDlgInit()
{
	setWindowTitle(tr("接口定义"));
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
	selConnector = new QLabel("选择接口"); //选择接口 下拉框
	font = selConnector->font();//加粗字体
	font.setBold(true);
	selConnector->setFont(font);
	selConnector->setMinimumHeight(25);
	modelicaConnector = new QComboBox(this);
	for (int k = 0; k < ConnectorNames.size(); k++)
	{
		if (!IsVectorFound(DefinedConnName, ConnectorNames[k]))
		{
			modelicaConnector->addItem(QString::fromStdString(ConnectorNames[k]));
		}
	}
	modelicaConnector->setStyleSheet(BtnStyle);
	modelicaConnector->setMinimumHeight(25);

	defType = new QLabel("定义方式"); //定义方式 下拉框
	defType->setFont(font);
	defType->setMinimumHeight(25);
	connDefType = new QComboBox(this);
	connDefType->addItem("3个点");
	connDefType->addItem("2条线");
	connDefType->setStyleSheet(BtnStyle);
	connDefType->setMinimumHeight(25);
	connect(modelicaConnector, SIGNAL(activated(QString)), this, SLOT(connTypeChanged()));

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
	connect(okBtn, &QPushButton::clicked, this, &ConnectorDlg::Accept);

	//为5种接口定义方式需选取的特征创建容器，用于分组布局
	//1.3个点---Frame
	page1 = new QWidget(this);
	page1Layout = new QVBoxLayout;
	feature11 = new QLabel("1.原点：");
	feature11->setMinimumHeight(20);
	value11 = new FeatureButton(this);
	value11->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value11->setStyleSheet(BtnStyle);
	value11->setMinimumHeight(25);
	feature12 = new QLabel("2.z轴方向上的点：");
	feature12->setMinimumHeight(20);
	value12 = new FeatureButton(this);
	value12->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value12->setStyleSheet(BtnStyle);
	value12->setMinimumHeight(25);
	feature13 = new QLabel("3.xoz平面上的点：");
	feature13->setMinimumHeight(20);
	value13 = new FeatureButton(this);
	value13->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value13->setStyleSheet(BtnStyle);
	value13->setMinimumHeight(25);
	page1Layout->addWidget(feature11);
	page1Layout->addWidget(value11);
	page1Layout->addWidget(feature12);
	page1Layout->addWidget(value12);
	page1Layout->addWidget(feature13);
	page1Layout->addWidget(value13);
	page1->setLayout(page1Layout);
	page1Layout->setContentsMargins(0, 0, 0, 0);

	//2.2条线---Frame
	page2 = new QWidget(this);
	page2Layout = new QVBoxLayout;
	feature21 = new QLabel("1.z轴：");
	feature21->setMinimumHeight(25);
	value21 = new FeatureButton(this);
	value21->setFeatureType(TopAbs_ShapeEnum::TopAbs_EDGE);
	value21->setStyleSheet(BtnStyle);
	value21->setMinimumHeight(25);
	feature22 = new QLabel("2.x轴：");
	feature22->setMinimumHeight(25);
	value22 = new FeatureButton(this);
	value22->setFeatureType(TopAbs_ShapeEnum::TopAbs_EDGE);
	value22->setStyleSheet(BtnStyle);
	value22->setMinimumHeight(25);
	reverDir = new QCheckBox("z轴反向",this);
	reverDir->setMinimumHeight(30);
	page2Layout->addWidget(feature21);
	page2Layout->addWidget(value21);
	page2Layout->addWidget(feature22);
	page2Layout->addWidget(value22);
	page2Layout->addWidget(reverDir);
	page2Layout->addStretch();
	page2Layout->setContentsMargins(0, 0, 0, 0);
	page2->setLayout(page2Layout);

	//3.Flange
	page3 = new QWidget(this);
	page3Layout = new QVBoxLayout;
	feature31 = new QLabel("1.原点：");
	feature31->setMinimumHeight(25);
	value31 = new FeatureButton(this);
	value31->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value31->setStyleSheet(BtnStyle);
	value31->setMinimumHeight(25);
	feature32 = new QLabel("2.z轴方向上的点：");
	feature32->setMinimumHeight(25);
	value32 = new FeatureButton(this);
	value32->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value32->setStyleSheet(BtnStyle);
	value32->setMinimumHeight(25);
	reverDir = new QCheckBox("z轴反向", this);
	reverDir->setMinimumHeight(30);
	page3Layout->addWidget(feature31);
	page3Layout->addWidget(value31);
	page3Layout->addWidget(feature32);
	page3Layout->addWidget(value32);
	page3Layout->addWidget(reverDir);
	page3Layout->addStretch();
	page3Layout->setContentsMargins(0, 0, 0, 0);
	page3->setLayout(page3Layout);

	//4.原点---一维接口
	page4 = new QWidget(this);
	page4Layout = new QVBoxLayout;
	feature41 = new QLabel("原点：");
	feature41->setMinimumHeight(25);
	value41 = new FeatureButton(this);
	value41->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value41->setStyleSheet(BtnStyle);
	value41->setMinimumHeight(25);
	page4Layout->addWidget(feature41);
	page4Layout->addWidget(value41);
	page4Layout->addStretch();
	page4Layout->setContentsMargins(0, 0, 0, 0);
	page4->setLayout(page4Layout);

	StackedWidget = new QStackedLayout(this);
	StackedWidget->addWidget(page1);
	StackedWidget->addWidget(page2);
	StackedWidget->addWidget(page3);
	StackedWidget->addWidget(page4);
	connect(connDefType, SIGNAL(activated(QString)), this, SLOT(defTypeChanged()));

	//添加到布局中
    form->addWidget(selConnector);
    form->addWidget(modelicaConnector);
    form->addWidget(defType);
	form->addWidget(connDefType);
	form->addWidget(selectFeature);
	form->addLayout(StackedWidget);
	form->addLayout(hlayout);

	setLayout(form);
	setFixedWidth(300);
	form->setContentsMargins(20, 20, 20, 20);

	modelicaTypeInit();
	connTypeChanged();
	defTypeChanged();
}

void ConnectorDlg::modelicaTypeInit()
{
#define frame_NUM 4
	string sFrameNames[frame_NUM] = { "Modelica.Mechanics.MultiBody.Interfaces.Frame","Modelica.Mechanics.MultiBody.Interfaces.Frame_resolve", \
		"Modelica.Mechanics.MultiBody.Interfaces.Frame_a","Modelica.Mechanics.MultiBody.Interfaces.Frame_b" };
	for (int i = 0; i < frame_NUM; i++)
		Frame_ModelicaType.push_back(sFrameNames[i]);

#define flange_NUM 9
	string sFlangeNames[flange_NUM] = { "Modelica.Mechanics.Rotational.Interfaces.Flange", \
		"Modelica.Mechanics.Rotational.Interfaces.Flange_a","Modelica.Mechanics.Rotational.Interfaces.Flange_b",\
	    "Modelica.Mechanics.MultiBody.Interfaces.FlangeWithBearing","Modelica.Mechanics.Rotational.Interfaces.Support",\
		"Modelica.Mechanics.Translational.Interfaces.Flange","Modelica.Mechanics.Translational.Interfaces.Support",\
		"Modelica.Mechanics.Translational.Interfaces.Flange_a","Modelica.Mechanics.Translational.Interfaces.Flange_b"};
	for (int i = 0; i < flange_NUM; i++)
		Flange_ModelicaType.push_back(sFlangeNames[i]);

#define oneD_NUM 67
	string s1DNames[oneD_NUM] = { "Modelica.Blocks.Interfaces.RealInput", "Modelica.Blocks.Interfaces.RealOutput",\
		"Modelica.Blocks.Interfaces.BooleanInput", "Modelica.Blocks.Interfaces.BooleanOutput", \
		"Modelica.Blocks.Interfaces.IntegerInput", "Modelica.Blocks.Interfaces.IntegerOutput", \
		"Modelica.Electrical.Analog.Interfaces.Pin","Modelica.Electrical.Analog.Interfaces.PositivePin",\
		"Modelica.Electrical.Analog.Interfaces.NegativePin","Modelica.Thermal.HeatTransfer.Interfaces.HeatPort",\
		"Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_a", "Modelica.Thermal.HeatTransfer.Interfaces.HeatPort_b",\
		"Modelica.Blocks.Interfaces.RealVectorInput", "Modelica.Blocks.Interfaces.RealVectorOutput",\
		"Modelica.Blocks.Interfaces.BooleanVectorInput","Modelica.Blocks.Interfaces.IntegerVectorInput",\
		"Modelica.ComplexBlocks.Interfaces.ComplexInput","Modelica.ComplexBlocks.Interfaces.ComplexOutput",\
	    "Modelica.StateGraph.Interfaces.Step_in","Modelica.StateGraph.Interfaces.Step_out",\
		"Modelica.StateGraph.Interfaces.Transition_in","Modelica.StateGraph.Interfaces.Transition_out",\
		"Modelica.StateGraph.Interfaces.CompositeStep_resume", "Modelica.StateGraph.Interfaces.CompositeStep_suspend",\
		"Modelica.StateGraph.Interfaces.CompositeStepStatePort_in", "Modelica.StateGraph.Interfaces.CompositeStepStatePort_out",\
		"Modelica.Electrical.Digital.Interfaces.DigitalSignal", "Modelica.Electrical.Digital.Interfaces.DigitalInput",\
		"Modelica.Electrical.Digital.Interfaces.DigitalOutput", "Modelica.Electrical.Machines.Interfaces.SpacePhasor",\
		"Modelica.Electrical.MultiPhase.Interfaces.Plug", "Modelica.Electrical.MultiPhase.Interfaces.PositivePlug",\
		"Modelica.Electrical.MultiPhase.Interfaces.NegativePlug", "Modelica.Electrical.Machines.Interfaces.ThermalPortTransformer",\
	    "Modelica.Electrical.QuasiStationary.SinglePhase.Interfaces.Pin","Modelica.Electrical.QuasiStationary.SinglePhase.Interfaces.PositivePin",\
		"Modelica.Electrical.QuasiStationary.SinglePhase.Interfaces.NegativePin", "Modelica.Electrical.QuasiStationary.MultiPhase.Interfaces.Plug",\
		"Modelica.Electrical.QuasiStationary.MultiPhase.Interfaces.PositivePlug", "Modelica.Electrical.QuasiStationary.MultiPhase.Interfaces.NegativePlug",\
		"Modelica.Electrical.Spice3.Interfaces.InductiveCouplePinIn", "Modelica.Electrical.Spice3.Interfaces.InductiveCouplePinOut",\
		"Modelica.Magnetic.FluxTubes.Interfaces.MagneticPort", "Modelica.Magnetic.FluxTubes.Interfaces.PositiveMagneticPort",\
		"Modelica.Magnetic.FluxTubes.Interfaces.NegativeMagneticPort", "Modelica.Magnetic.FundamentalWave.Interfaces.MagneticPort",\
		"Modelica.Magnetic.FundamentalWave.Interfaces.NegativeMagneticPort", "Modelica.Magnetic.FundamentalWave.Interfaces.PositiveMagneticPort",\
		"Modelica.Magnetic.QuasiStatic.FluxTubes.Interfaces.MagneticPort", "Modelica.Magnetic.QuasiStatic.FluxTubes.Interfaces.PositiveMagneticPort",\
		"Modelica.Magnetic.QuasiStatic.FluxTubes.Interfaces.NegativeMagneticPort", "Modelica.Magnetic.QuasiStatic.FundamentalWave.Interfaces.MagneticPort",\
		"Modelica.Magnetic.QuasiStatic.FundamentalWave.Interfaces.PositiveMagneticPort", "Modelica.Magnetic.QuasiStatic.FundamentalWave.Interfaces.NegativeMagneticPort",\
		"Modelica.Fluid.Interfaces.FluidPort", "Modelica.Fluid.Interfaces.FluidPort_a",\
		"Modelica.Fluid.Interfaces.FluidPort_b", "Modelica.Fluid.Interfaces.FluidPorts_a",\
		"Modelica.Fluid.Interfaces.FluidPorts_b", "Modelica.Fluid.Interfaces.HeatPorts_a",\
		"Modelica.Fluid.Interfaces.HeatPorts_b", "Modelica.Media.Interfaces.PartialMedium.BaseProperties.InputAbsolutePressure",\
		"Modelica.Media.Interfaces.PartialMedium.BaseProperties.InputSpecificEnthalpy", "Modelica.Media.Interfaces.PartialMedium.BaseProperties.InputMassFraction",\
		"Modelica.Thermal.FluidHeatFlow.Interfaces.FlowPort", "Modelica.Thermal.FluidHeatFlow.Interfaces.FlowPort_a",\
		"Modelica.Thermal.FluidHeatFlow.Interfaces.FlowPort_b" };
	for (int i = 0; i < oneD_NUM; i++)
		oneD_ModelicaType.push_back(s1DNames[i]);
}

bool ConnectorDlg::IsVectorFound(vector<string> arr, string content)
{
	vector<string>::iterator result = std::find(arr.begin(), arr.end(), content);
	if (result == arr.end())
	{
		return false;
	}
	else
	{
		return true;
	}
}

void ConnectorDlg::connTypeChanged()
{
	connDefType->clear();
	QString connName = modelicaConnector->currentText();
	string str_connName = connName.toStdString();
	
	//根据接口名查找索引，从而找到接口类型
	vector<string>::iterator result = std::find(ConnectorNames.begin(), ConnectorNames.end(), str_connName);
	int connIndex = distance(ConnectorNames.begin(), result);
	string str_connType = ConnectorTypes[connIndex];

	if (IsVectorFound(Frame_ModelicaType, str_connType))
	{
		connDefType->addItem("3个点");
		connDefType->addItem("2条线");
	}
	else if (IsVectorFound(Flange_ModelicaType, str_connType))
	{
		connDefType->addItem("原点和z轴");
	}
	else if (IsVectorFound(oneD_ModelicaType, str_connType))
	{
		connDefType->addItem("原点");
	}
	else if (str_connType.compare("Modelica.Mechanics.MultiBody.Examples.Systems.RobotR3.Components.AxisControlBus") == 0)
	{
		connDefType->addItem("原点和z轴");
	}
	else if (str_connType.compare("Modelica.Mechanics.MultiBody.Examples.Systems.RobotR3.Components.ControlBus") == 0)
	{
		connDefType->addItem("原点和z轴");
	}
}

void ConnectorDlg::defTypeChanged()
{
	QString defType = connDefType->currentText();
	if (defType == tr("3个点"))
	{
		StackedWidget->setCurrentIndex(0);
	}
	else if (defType == tr("2条线"))
	{
		StackedWidget->setCurrentIndex(1);
	}
	else if (defType == tr("原点和z轴"))
	{
		StackedWidget->setCurrentIndex(2);
	}
	else if (defType == tr("原点"))
	{
		StackedWidget->setCurrentIndex(3);
	}
}

ConnectorDlg::~ConnectorDlg()
{

}

DocumentComponent* ConnectorDlg::getDocumentFromDlg()
{
	return myDocument3d;
}

void ConnectorDlg::Accept()
{
	double baseLen = 0.05;
	baseLen = myDocument3d->GetConnectorBaseLength();

	Connector* Connector1 = new Connector();//接口
	vector<double> displacement;//位姿
	vector<string> RefEnts;//引用实体

	gp_Dir aX_Direction(1.0, 0.0, 0.0);
	gp_Dir aY_Direction(0.0, 1.0, 0.0);
	gp_Dir aZ_Direction(0.0, 0.0, 1.0);
	Standard_Real theLength = 5.0;
	Standard_Real arrowLength = 0.3;

	QString connName = modelicaConnector->currentText();
	string str_connName = connName.toStdString();
	QString defType = connDefType->currentText();

	//根据接口名查找索引，从而找到接口类型
	vector<string>::iterator result = std::find(ConnectorNames.begin(), ConnectorNames.end(), str_connName);
	int connIndex = distance(ConnectorNames.begin(), result);
	string str_connType = ConnectorTypes[connIndex];

	//1.3个点---Frame，绘制立方体box
	//根据选取的特征：原点；z轴上的点；xoz平面上的点，计算Frame接口的三维坐标系（右手坐标系）
	//根据原点和z轴上的点可确定z轴向量，并计算出xoy平面；将xoz_point向xoy平面投影，得到x轴方向上的点，即确定x轴
	if (defType == tr("3个点"))
	{
		Connector1->SetDefineType(THREE_POINTS);

		//从界面获取特征shape
		TopoDS_Shape vShape1 = value11->getFeatureShape();
		TopoDS_Shape vShape2 = value12->getFeatureShape();
		TopoDS_Shape vShape3 = value13->getFeatureShape();

		//存储引用实体
		RefEnts.push_back(value11->getFeatureName());
		RefEnts.push_back(value12->getFeatureName());
		RefEnts.push_back(value13->getFeatureName());

		if (vShape1.IsNull() || vShape2.IsNull() || vShape3.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex1 = TopoDS::Vertex(vShape1);
		TopoDS_Vertex Vertex2 = TopoDS::Vertex(vShape2);
		TopoDS_Vertex Vertex3 = TopoDS::Vertex(vShape3);
		gp_Pnt origin = BRep_Tool::Pnt(Vertex1);
		gp_Pnt z_point = BRep_Tool::Pnt(Vertex2);
		gp_Pnt xoz_point = BRep_Tool::Pnt(Vertex3);

		gp_Vec zVector = gp_Vec(origin, z_point);
		gp_Dir z_dir = gp_Dir(zVector);//z轴
		gp_Pln xoy_plane = gp_Pln(origin, z_dir);//xoy平面
		//将xoz_point向xoy平面投影，得到x轴方向上的点，即确定x轴
		gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(xoy_plane, xoz_point);//投影的二维坐标
		gp_Pnt x_point = ElSLib::Value(ConvertedPointOnPlane.X(),
			ConvertedPointOnPlane.Y(), xoy_plane);//将二维坐标转换为基于OCC三维坐标选定面上的坐标
		gp_Vec xVector = gp_Vec(origin, x_point);
		gp_Dir x_dir = gp_Dir(xVector);//x轴
		gp_Ax2 Ax2 = gp_Ax2(origin, z_dir, x_dir);//由输入特征确定的接口的三维右手坐标系
		Ax3 = gp_Ax3(Ax2);//存储位姿

		//构建box
		Standard_Real dz = baseLen;
		Standard_Real dx = 3 * dz;
		Standard_Real dy = 2 * dz;
		gp_Vec trans = gp_Vec((-dx / 2) * x_dir + (-dy / 2) * Ax2.YDirection()+ (-dz / 2) * z_dir);
		gp_Pnt boxCorner = origin.Translated(trans);
		gp_Ax2 box_Ax2 = gp_Ax2(boxCorner, z_dir, x_dir);//用于构建box的三维右手坐标系
		TopoDS_Shape topo_box = BRepPrimAPI_MakeBox(box_Ax2, dx, dy, dz).Shape();
		//Handle(AIS_Shape) ais_box = new AIS_Shape(topo_box);
		//myContext->Display(ais_box, Standard_True);

		Connector1->SetShape(topo_box);

		value11->initFeatureButton();
		value12->initFeatureButton();
		value13->initFeatureButton();
	}

	//2.2条线---Frame，绘制立方体box
	//根据选取的特征：z轴；x轴，计算Frame接口的三维坐标系（右手坐标系）
	//x轴和z轴方向是由选取的edge方向确定的，选取的两个edge特征必须为直线且正交
	//这里的x轴特征其实可以宽泛为【在xoy平面且平行于x轴的线】。。。
	else if (defType == tr("2条线"))
	{
		Connector1->SetDefineType(TWO_EDGES);

		TopoDS_Shape vShape1 = value21->getFeatureShape();
		TopoDS_Shape vShape2 = value22->getFeatureShape();

		//存储引用实体
		RefEnts.push_back(value21->getFeatureName());
		RefEnts.push_back(value22->getFeatureName());

		if (vShape1.IsNull() || vShape2.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Edge zAxis = TopoDS::Edge(vShape1);
		TopoDS_Edge xAxis = TopoDS::Edge(vShape2);
		Standard_Real z_first, z_last;
		Handle(Geom_Curve) zCurve = BRep_Tool::Curve(zAxis, z_first, z_last);
		Handle(Geom_Line) zLine = Handle(Geom_Line)::DownCast(zCurve);
		gp_Pnt zStrPnt = zCurve->Value(z_first); //EDGE的首位端点
		gp_Pnt zEndPnt = zCurve->Value(z_last);
		gp_Vec zVector = gp_Vec(zStrPnt, zEndPnt);
		gp_Dir z_dir = gp_Dir(zVector);//z轴
		//若勾选了z轴反向
		if (reverDir->isChecked() == true)
		{
			zVector.Reverse();
			z_dir.Reverse();
			Connector1->SetZ_Reverse();
		}

		Standard_Real x_first, x_last;
		Handle(Geom_Curve) xCurve = BRep_Tool::Curve(xAxis, x_first, x_last);
		Handle(Geom_Line) xLine = Handle(Geom_Line)::DownCast(xCurve);
		gp_Pnt xStrPnt = xCurve->Value(x_first); //EDGE的首位端点
		gp_Pnt xEndPnt = xCurve->Value(x_last);
		gp_Vec xVector = gp_Vec(xStrPnt, xEndPnt);
		gp_Dir x_dir = gp_Dir(xVector);//x轴
		if (xLine.IsNull()|| zLine.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征不为直线，请重新选择特征", QMessageBox::Ok);
			return;
		}
		if (!z_dir.IsNormal(x_dir,Precision::Angular()))
		{
			QMessageBox::information(NULL, "提示", "特征不正交，请重新选择特征", QMessageBox::Ok);
			return;
		}

		//计算原点
		gp_Pln xoy_plane = gp_Pln(xStrPnt, z_dir);
		gp_Pnt origin;//原点
		Handle(Geom_Plane) Geom_xoy_plane = new Geom_Plane(xoy_plane);
		GeomAPI_IntCS CS(zLine, Geom_xoy_plane);
		Standard_Integer NbPoints = 0;
		if (CS.IsDone())
		{
			NbPoints = CS.NbPoints();
			if (NbPoints == 0)
			{
				QMessageBox::information(NULL, "提示", "计算失败", QMessageBox::Ok);
				return;
			}
			origin = CS.Point(1);
		}
		//由输入特征确定的接口的三维右手坐标系
		gp_Ax2 Ax2 = gp_Ax2(origin, z_dir, x_dir);
		//存储位姿
		Ax3 = gp_Ax3(Ax2);

		//构建box
		Standard_Real dz = baseLen;
		Standard_Real dx = 3 * dz;
		Standard_Real dy = 2 * dz;
		gp_Vec trans = gp_Vec((-dx / 2) * x_dir + (-dy / 2) * Ax2.YDirection() + (-dz / 2) * z_dir);
		gp_Pnt boxCorner = origin.Translated(trans);
		gp_Ax2 box_Ax2 = gp_Ax2(boxCorner, z_dir, x_dir);//用于构建box的三维右手坐标系
		TopoDS_Shape topo_box = BRepPrimAPI_MakeBox(box_Ax2, dx, dy, dz).Shape();
		//Handle(AIS_Shape) ais_box = new AIS_Shape(topo_box);
		//myContext->Display(ais_box, Standard_True);

		Connector1->SetShape(topo_box);

		value21->initFeatureButton();
		value22->initFeatureButton();
		reverDir->setCheckState(Qt::Unchecked);
	}
	//3.Flange，绘制圆柱体；总线接口，绘制立方体+半球
	//根据选取的特征：原点；z轴方向上的点，可确定z轴向量（x轴和y轴方向缺省即可），从而计算出Flange接口的三维坐标系
	else if (defType == tr("原点和z轴"))
	{
	Connector1->SetDefineType(POINT_X_AXIS);

		TopoDS_Shape vShape1 = value31->getFeatureShape();
		TopoDS_Shape vShape2 = value32->getFeatureShape();

		//存储引用实体
		RefEnts.push_back(value31->getFeatureName());
		RefEnts.push_back(value32->getFeatureName());

		if (vShape1.IsNull() || vShape2.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex1 = TopoDS::Vertex(vShape1);
		TopoDS_Vertex Vertex2 = TopoDS::Vertex(vShape2);
		gp_Pnt origin = BRep_Tool::Pnt(Vertex1);
		gp_Pnt z_point = BRep_Tool::Pnt(Vertex2);
		gp_Vec zVector = gp_Vec(origin, z_point);//z轴
		gp_Dir z_dir = gp_Dir(zVector);
		//若勾选了z轴反向
		if (reverDir->isChecked() == true)
		{
			zVector.Reverse();
			z_dir.Reverse();
			Connector1->SetZ_Reverse();
		}

		gp_Ax2 Ax2 = gp_Ax2(origin, z_dir);//接口的三维坐标系
		Ax3 = gp_Ax3(Ax2);//存储

		Standard_Real R = baseLen;//圆柱体半径、高度
		Standard_Real H = 3 * R;
		gp_Vec trans = gp_Vec((-H / 2) * z_dir);
		gp_Ax2 cylinder_Ax2 = Ax2.Translated(trans);
		if (IsVectorFound(Flange_ModelicaType, str_connType))
		{
			TopoDS_Shape topo_cylinder = BRepPrimAPI_MakeCylinder(cylinder_Ax2, R, H).Shape();

			Connector1->SetShape(topo_cylinder);

			value31->initFeatureButton();
			value32->initFeatureButton();
			reverDir->setCheckState(Qt::Unchecked);
		}
		else if ((str_connType.compare("Modelica.Mechanics.MultiBody.Examples.Systems.RobotR3.Components.AxisControlBus") == 0)\
			|| (str_connType.compare("Modelica.Mechanics.MultiBody.Examples.Systems.RobotR3.Components.ControlBus")==0))
		{
			TopoDS_Compound topo_axisCB = makeAxisControlBus(cylinder_Ax2, R, H);

			Connector1->SetShape(topo_axisCB);

			value31->initFeatureButton();
			value32->initFeatureButton();
			reverDir->setCheckState(Qt::Unchecked);
		}
		else
		{
			QMessageBox::information(NULL, "提示", "错误：模型类型", QMessageBox::Ok);
			return;
		}
		//else if (connectorType == tr("ControlBus"))
		//{
		//	Standard_Real N = 6;  // AxisControlBus数量
		//	Standard_Real r = R / (sin(M_PI / N));
		//	vector<TopoDS_Shape> vShapes;
		//	
		//	TopoDS_Compound aRes;  // 整合模型
		//	BRep_Builder aBuilder;
		//	aBuilder.MakeCompound(aRes);

		//	//创建ControlBus形体，由N个AxisControlBus组成，分布在正n边形的顶点处
		//	for (int i = 1; i <= N; i++)
		//	{
		//		gp_Vec trans = gp_Vec(r * (cos(2*M_PI*i/N)*Ax2.XDirection()+ sin(2 * M_PI * i / N) * Ax2.YDirection()));
		//		gp_Ax2 shapeAx2 = Ax2.Translated(trans);
		//		TopoDS_Compound topo_axisCB = makeAxisControlBus(shapeAx2, R, H);
		//		vShapes.push_back(topo_axisCB);
		//		aBuilder.Add(aRes, topo_axisCB);
		//	}
		//	Handle(AIS_Shape) ais_ControlBus = new AIS_Shape(aRes);
		//	myContext->Display(ais_ControlBus, Standard_True);
		//}
	}
	//4.原点---一维接口，绘制球体
	else if (defType == tr("原点"))
	{
		Connector1->SetDefineType(ONE_POINT);

		TopoDS_Shape vShape = value41->getFeatureShape();
		//存储引用实体
		RefEnts.push_back(value41->getFeatureName());

		if (vShape.IsNull())
		{
			QMessageBox::information(NULL, "提示", "特征信息不完整", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex = TopoDS::Vertex(vShape);
		gp_Pnt theLoc = BRep_Tool::Pnt(Vertex);
		Ax3 = gp_Ax3(theLoc, aZ_Direction, aX_Direction);

		Standard_Real R = baseLen;//半径
		TopoDS_Shape topo_sphere = BRepPrimAPI_MakeSphere(theLoc, R).Shape();
		//Handle(AIS_Shape) ais_sphere = new AIS_Shape(topo_sphere);
		//myContext->Display(ais_sphere, Standard_True);

		Connector1->SetShape(topo_sphere);
		value41->initFeatureButton();
	}
	
	//根据接口名查找索引，从而找到接口类型
	vector<string>::iterator res = std::find(ConnectorNames.begin(), ConnectorNames.end(), str_connName);
	int Index = distance(ConnectorNames.begin(), res);

	Connector1->SetConnName(str_connName.c_str());//存储接口名
	Connector1->SetConnType(ConnectorTypes[Index]);//存储接口类型，Modelica全名

	//位姿
	gp_Pnt dOrigin = Ax3.Location();
	gp_Dir dZdir = Ax3.Direction();
	gp_Dir dXdir = Ax3.XDirection();
	displacement.push_back(dOrigin.X());
	displacement.push_back(dOrigin.Y());
	displacement.push_back(dOrigin.Z());
	displacement.push_back(dZdir.X());
	displacement.push_back(dZdir.Y());
	displacement.push_back(dZdir.Z());
	displacement.push_back(dXdir.X());
	displacement.push_back(dXdir.Y());
	displacement.push_back(dXdir.Z());
	Connector1->SetDisplacement(displacement);//存储位姿
	Connector1->SetRefEnts(RefEnts);//存储引用实体
	
	//存储接口信息
	myDocument3d->AddConnector(Connector1);
	delete Connector1;
	myDocument3d->UpdateDisplay();

	////绘制坐标系
	//Handle(AdaptorVec_AIS) z_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.Direction(), theLength, arrowLength);
	//z_Axis->SetText("  Z");
	//myContext->Display(z_Axis, Standard_True);
	//Handle(AdaptorVec_AIS) x_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.XDirection(), theLength, arrowLength);
	//x_Axis->SetText("  X");
	//myContext->Display(x_Axis, Standard_True);
	//Handle(AdaptorVec_AIS) y_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.YDirection(), theLength, arrowLength);
	//y_Axis->SetText("  Y");
	//myContext->Display(y_Axis, Standard_True);
	
	//需要刷新接口类型列表
	modelicaConnector->removeItem(modelicaConnector->currentIndex());
	if (modelicaConnector->count() == 0)
	{
		QDialog::close();
	}
}

//创建AxisControlBus形体，圆柱+球体
TopoDS_Compound ConnectorDlg::makeAxisControlBus(const gp_Ax2& Axes, const Standard_Real R, const Standard_Real H)
{
	TopoDS_Shape topo_cylinder = BRepPrimAPI_MakeCylinder(Axes, R, H).Shape();
	gp_Vec trans = gp_Vec(H * Axes.Direction());
	gp_Pnt theLoc = Axes.Location().Translated(trans);
	TopoDS_Shape topo_sphere = BRepPrimAPI_MakeSphere(theLoc, R).Shape();
	
	// 整合模型
	TopoDS_Compound aRes;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(aRes);
	aBuilder.Add(aRes, topo_cylinder);
	aBuilder.Add(aRes, topo_sphere);
	return aRes;
}