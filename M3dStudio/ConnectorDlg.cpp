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
		msgBox.setText("δ����ģ����Ϣ��");
		msgBox.exec();
		return;
	}

	ConnectorNames = m_cyberInfo->GetConnectorNames();// �ӿ���
	ConnectorTypes = m_cyberInfo->GetConnectorTypes();// �ӿ�����

	//Add by Wu
	string sMdlName = m_cyberInfo->GetMdlName();
	ModelicaModel* pMdl = ModelicaModel::LookupModel(sMdlName.c_str());
	if (pMdl && pMdl->m_sType == "connector")
	{
		ConnectorNames.push_back("MainConnector");
		ConnectorTypes.push_back(sMdlName);
	}

	int connNum = ConnectorNames.size();

	//DefinedConnName�洢�Ѷ�����Ľӿ���
	vector<Connector*> vConnectors = myDocument3d->GetAllConnectors();
	vector<string>().swap(DefinedConnName);//���Ԫ�ز������ڴ�
	for (int i = 0; i < vConnectors.size(); i++)
	{
		Connector* conn = vConnectors[i];
		DefinedConnName.push_back(conn->GetConnName());
	}

	if (m_cyberInfo == nullptr)
	{
		QMessageBox::information(NULL, "��ʾ", "�뵼��Modelica�ļ�������", QMessageBox::Ok);
		QTimer::singleShot(0, this, SLOT(close()));
	}
	else if(DefinedConnName.size()== connNum)
	{
		QMessageBox::information(NULL, "��ʾ", "�ӿ���ȫ������", QMessageBox::Ok);
		QTimer::singleShot(0, this, SLOT(close()));
	}
	else
	{
		connDlgInit();
	}
}

void ConnectorDlg::connDlgInit()
{
	setWindowTitle(tr("�ӿڶ���"));
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
	selConnector = new QLabel("ѡ��ӿ�"); //ѡ��ӿ� ������
	font = selConnector->font();//�Ӵ�����
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

	defType = new QLabel("���巽ʽ"); //���巽ʽ ������
	defType->setFont(font);
	defType->setMinimumHeight(25);
	connDefType = new QComboBox(this);
	connDefType->addItem("3����");
	connDefType->addItem("2����");
	connDefType->setStyleSheet(BtnStyle);
	connDefType->setMinimumHeight(25);
	connect(modelicaConnector, SIGNAL(activated(QString)), this, SLOT(connTypeChanged()));

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
	connect(okBtn, &QPushButton::clicked, this, &ConnectorDlg::Accept);

	//Ϊ5�ֽӿڶ��巽ʽ��ѡȡ�������������������ڷ��鲼��
	//1.3����---Frame
	page1 = new QWidget(this);
	page1Layout = new QVBoxLayout;
	feature11 = new QLabel("1.ԭ�㣺");
	feature11->setMinimumHeight(20);
	value11 = new FeatureButton(this);
	value11->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value11->setStyleSheet(BtnStyle);
	value11->setMinimumHeight(25);
	feature12 = new QLabel("2.z�᷽���ϵĵ㣺");
	feature12->setMinimumHeight(20);
	value12 = new FeatureButton(this);
	value12->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value12->setStyleSheet(BtnStyle);
	value12->setMinimumHeight(25);
	feature13 = new QLabel("3.xozƽ���ϵĵ㣺");
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

	//2.2����---Frame
	page2 = new QWidget(this);
	page2Layout = new QVBoxLayout;
	feature21 = new QLabel("1.z�᣺");
	feature21->setMinimumHeight(25);
	value21 = new FeatureButton(this);
	value21->setFeatureType(TopAbs_ShapeEnum::TopAbs_EDGE);
	value21->setStyleSheet(BtnStyle);
	value21->setMinimumHeight(25);
	feature22 = new QLabel("2.x�᣺");
	feature22->setMinimumHeight(25);
	value22 = new FeatureButton(this);
	value22->setFeatureType(TopAbs_ShapeEnum::TopAbs_EDGE);
	value22->setStyleSheet(BtnStyle);
	value22->setMinimumHeight(25);
	reverDir = new QCheckBox("z�ᷴ��",this);
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
	feature31 = new QLabel("1.ԭ�㣺");
	feature31->setMinimumHeight(25);
	value31 = new FeatureButton(this);
	value31->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value31->setStyleSheet(BtnStyle);
	value31->setMinimumHeight(25);
	feature32 = new QLabel("2.z�᷽���ϵĵ㣺");
	feature32->setMinimumHeight(25);
	value32 = new FeatureButton(this);
	value32->setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	value32->setStyleSheet(BtnStyle);
	value32->setMinimumHeight(25);
	reverDir = new QCheckBox("z�ᷴ��", this);
	reverDir->setMinimumHeight(30);
	page3Layout->addWidget(feature31);
	page3Layout->addWidget(value31);
	page3Layout->addWidget(feature32);
	page3Layout->addWidget(value32);
	page3Layout->addWidget(reverDir);
	page3Layout->addStretch();
	page3Layout->setContentsMargins(0, 0, 0, 0);
	page3->setLayout(page3Layout);

	//4.ԭ��---һά�ӿ�
	page4 = new QWidget(this);
	page4Layout = new QVBoxLayout;
	feature41 = new QLabel("ԭ�㣺");
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

	//��ӵ�������
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
	
	//���ݽӿ��������������Ӷ��ҵ��ӿ�����
	vector<string>::iterator result = std::find(ConnectorNames.begin(), ConnectorNames.end(), str_connName);
	int connIndex = distance(ConnectorNames.begin(), result);
	string str_connType = ConnectorTypes[connIndex];

	if (IsVectorFound(Frame_ModelicaType, str_connType))
	{
		connDefType->addItem("3����");
		connDefType->addItem("2����");
	}
	else if (IsVectorFound(Flange_ModelicaType, str_connType))
	{
		connDefType->addItem("ԭ���z��");
	}
	else if (IsVectorFound(oneD_ModelicaType, str_connType))
	{
		connDefType->addItem("ԭ��");
	}
	else if (str_connType.compare("Modelica.Mechanics.MultiBody.Examples.Systems.RobotR3.Components.AxisControlBus") == 0)
	{
		connDefType->addItem("ԭ���z��");
	}
	else if (str_connType.compare("Modelica.Mechanics.MultiBody.Examples.Systems.RobotR3.Components.ControlBus") == 0)
	{
		connDefType->addItem("ԭ���z��");
	}
}

void ConnectorDlg::defTypeChanged()
{
	QString defType = connDefType->currentText();
	if (defType == tr("3����"))
	{
		StackedWidget->setCurrentIndex(0);
	}
	else if (defType == tr("2����"))
	{
		StackedWidget->setCurrentIndex(1);
	}
	else if (defType == tr("ԭ���z��"))
	{
		StackedWidget->setCurrentIndex(2);
	}
	else if (defType == tr("ԭ��"))
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

	Connector* Connector1 = new Connector();//�ӿ�
	vector<double> displacement;//λ��
	vector<string> RefEnts;//����ʵ��

	gp_Dir aX_Direction(1.0, 0.0, 0.0);
	gp_Dir aY_Direction(0.0, 1.0, 0.0);
	gp_Dir aZ_Direction(0.0, 0.0, 1.0);
	Standard_Real theLength = 5.0;
	Standard_Real arrowLength = 0.3;

	QString connName = modelicaConnector->currentText();
	string str_connName = connName.toStdString();
	QString defType = connDefType->currentText();

	//���ݽӿ��������������Ӷ��ҵ��ӿ�����
	vector<string>::iterator result = std::find(ConnectorNames.begin(), ConnectorNames.end(), str_connName);
	int connIndex = distance(ConnectorNames.begin(), result);
	string str_connType = ConnectorTypes[connIndex];

	//1.3����---Frame������������box
	//����ѡȡ��������ԭ�㣻z���ϵĵ㣻xozƽ���ϵĵ㣬����Frame�ӿڵ���ά����ϵ����������ϵ��
	//����ԭ���z���ϵĵ��ȷ��z���������������xoyƽ�棻��xoz_point��xoyƽ��ͶӰ���õ�x�᷽���ϵĵ㣬��ȷ��x��
	if (defType == tr("3����"))
	{
		Connector1->SetDefineType(THREE_POINTS);

		//�ӽ����ȡ����shape
		TopoDS_Shape vShape1 = value11->getFeatureShape();
		TopoDS_Shape vShape2 = value12->getFeatureShape();
		TopoDS_Shape vShape3 = value13->getFeatureShape();

		//�洢����ʵ��
		RefEnts.push_back(value11->getFeatureName());
		RefEnts.push_back(value12->getFeatureName());
		RefEnts.push_back(value13->getFeatureName());

		if (vShape1.IsNull() || vShape2.IsNull() || vShape3.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex1 = TopoDS::Vertex(vShape1);
		TopoDS_Vertex Vertex2 = TopoDS::Vertex(vShape2);
		TopoDS_Vertex Vertex3 = TopoDS::Vertex(vShape3);
		gp_Pnt origin = BRep_Tool::Pnt(Vertex1);
		gp_Pnt z_point = BRep_Tool::Pnt(Vertex2);
		gp_Pnt xoz_point = BRep_Tool::Pnt(Vertex3);

		gp_Vec zVector = gp_Vec(origin, z_point);
		gp_Dir z_dir = gp_Dir(zVector);//z��
		gp_Pln xoy_plane = gp_Pln(origin, z_dir);//xoyƽ��
		//��xoz_point��xoyƽ��ͶӰ���õ�x�᷽���ϵĵ㣬��ȷ��x��
		gp_Pnt2d ConvertedPointOnPlane = ProjLib::Project(xoy_plane, xoz_point);//ͶӰ�Ķ�ά����
		gp_Pnt x_point = ElSLib::Value(ConvertedPointOnPlane.X(),
			ConvertedPointOnPlane.Y(), xoy_plane);//����ά����ת��Ϊ����OCC��ά����ѡ�����ϵ�����
		gp_Vec xVector = gp_Vec(origin, x_point);
		gp_Dir x_dir = gp_Dir(xVector);//x��
		gp_Ax2 Ax2 = gp_Ax2(origin, z_dir, x_dir);//����������ȷ���Ľӿڵ���ά��������ϵ
		Ax3 = gp_Ax3(Ax2);//�洢λ��

		//����box
		Standard_Real dz = baseLen;
		Standard_Real dx = 3 * dz;
		Standard_Real dy = 2 * dz;
		gp_Vec trans = gp_Vec((-dx / 2) * x_dir + (-dy / 2) * Ax2.YDirection()+ (-dz / 2) * z_dir);
		gp_Pnt boxCorner = origin.Translated(trans);
		gp_Ax2 box_Ax2 = gp_Ax2(boxCorner, z_dir, x_dir);//���ڹ���box����ά��������ϵ
		TopoDS_Shape topo_box = BRepPrimAPI_MakeBox(box_Ax2, dx, dy, dz).Shape();
		//Handle(AIS_Shape) ais_box = new AIS_Shape(topo_box);
		//myContext->Display(ais_box, Standard_True);

		Connector1->SetShape(topo_box);

		value11->initFeatureButton();
		value12->initFeatureButton();
		value13->initFeatureButton();
	}

	//2.2����---Frame������������box
	//����ѡȡ��������z�᣻x�ᣬ����Frame�ӿڵ���ά����ϵ����������ϵ��
	//x���z�᷽������ѡȡ��edge����ȷ���ģ�ѡȡ������edge��������Ϊֱ��������
	//�����x��������ʵ���Կ�Ϊ����xoyƽ����ƽ����x����ߡ�������
	else if (defType == tr("2����"))
	{
		Connector1->SetDefineType(TWO_EDGES);

		TopoDS_Shape vShape1 = value21->getFeatureShape();
		TopoDS_Shape vShape2 = value22->getFeatureShape();

		//�洢����ʵ��
		RefEnts.push_back(value21->getFeatureName());
		RefEnts.push_back(value22->getFeatureName());

		if (vShape1.IsNull() || vShape2.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Edge zAxis = TopoDS::Edge(vShape1);
		TopoDS_Edge xAxis = TopoDS::Edge(vShape2);
		Standard_Real z_first, z_last;
		Handle(Geom_Curve) zCurve = BRep_Tool::Curve(zAxis, z_first, z_last);
		Handle(Geom_Line) zLine = Handle(Geom_Line)::DownCast(zCurve);
		gp_Pnt zStrPnt = zCurve->Value(z_first); //EDGE����λ�˵�
		gp_Pnt zEndPnt = zCurve->Value(z_last);
		gp_Vec zVector = gp_Vec(zStrPnt, zEndPnt);
		gp_Dir z_dir = gp_Dir(zVector);//z��
		//����ѡ��z�ᷴ��
		if (reverDir->isChecked() == true)
		{
			zVector.Reverse();
			z_dir.Reverse();
			Connector1->SetZ_Reverse();
		}

		Standard_Real x_first, x_last;
		Handle(Geom_Curve) xCurve = BRep_Tool::Curve(xAxis, x_first, x_last);
		Handle(Geom_Line) xLine = Handle(Geom_Line)::DownCast(xCurve);
		gp_Pnt xStrPnt = xCurve->Value(x_first); //EDGE����λ�˵�
		gp_Pnt xEndPnt = xCurve->Value(x_last);
		gp_Vec xVector = gp_Vec(xStrPnt, xEndPnt);
		gp_Dir x_dir = gp_Dir(xVector);//x��
		if (xLine.IsNull()|| zLine.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϊֱ�ߣ�������ѡ������", QMessageBox::Ok);
			return;
		}
		if (!z_dir.IsNormal(x_dir,Precision::Angular()))
		{
			QMessageBox::information(NULL, "��ʾ", "������������������ѡ������", QMessageBox::Ok);
			return;
		}

		//����ԭ��
		gp_Pln xoy_plane = gp_Pln(xStrPnt, z_dir);
		gp_Pnt origin;//ԭ��
		Handle(Geom_Plane) Geom_xoy_plane = new Geom_Plane(xoy_plane);
		GeomAPI_IntCS CS(zLine, Geom_xoy_plane);
		Standard_Integer NbPoints = 0;
		if (CS.IsDone())
		{
			NbPoints = CS.NbPoints();
			if (NbPoints == 0)
			{
				QMessageBox::information(NULL, "��ʾ", "����ʧ��", QMessageBox::Ok);
				return;
			}
			origin = CS.Point(1);
		}
		//����������ȷ���Ľӿڵ���ά��������ϵ
		gp_Ax2 Ax2 = gp_Ax2(origin, z_dir, x_dir);
		//�洢λ��
		Ax3 = gp_Ax3(Ax2);

		//����box
		Standard_Real dz = baseLen;
		Standard_Real dx = 3 * dz;
		Standard_Real dy = 2 * dz;
		gp_Vec trans = gp_Vec((-dx / 2) * x_dir + (-dy / 2) * Ax2.YDirection() + (-dz / 2) * z_dir);
		gp_Pnt boxCorner = origin.Translated(trans);
		gp_Ax2 box_Ax2 = gp_Ax2(boxCorner, z_dir, x_dir);//���ڹ���box����ά��������ϵ
		TopoDS_Shape topo_box = BRepPrimAPI_MakeBox(box_Ax2, dx, dy, dz).Shape();
		//Handle(AIS_Shape) ais_box = new AIS_Shape(topo_box);
		//myContext->Display(ais_box, Standard_True);

		Connector1->SetShape(topo_box);

		value21->initFeatureButton();
		value22->initFeatureButton();
		reverDir->setCheckState(Qt::Unchecked);
	}
	//3.Flange������Բ���壻���߽ӿڣ�����������+����
	//����ѡȡ��������ԭ�㣻z�᷽���ϵĵ㣬��ȷ��z��������x���y�᷽��ȱʡ���ɣ����Ӷ������Flange�ӿڵ���ά����ϵ
	else if (defType == tr("ԭ���z��"))
	{
	Connector1->SetDefineType(POINT_X_AXIS);

		TopoDS_Shape vShape1 = value31->getFeatureShape();
		TopoDS_Shape vShape2 = value32->getFeatureShape();

		//�洢����ʵ��
		RefEnts.push_back(value31->getFeatureName());
		RefEnts.push_back(value32->getFeatureName());

		if (vShape1.IsNull() || vShape2.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex1 = TopoDS::Vertex(vShape1);
		TopoDS_Vertex Vertex2 = TopoDS::Vertex(vShape2);
		gp_Pnt origin = BRep_Tool::Pnt(Vertex1);
		gp_Pnt z_point = BRep_Tool::Pnt(Vertex2);
		gp_Vec zVector = gp_Vec(origin, z_point);//z��
		gp_Dir z_dir = gp_Dir(zVector);
		//����ѡ��z�ᷴ��
		if (reverDir->isChecked() == true)
		{
			zVector.Reverse();
			z_dir.Reverse();
			Connector1->SetZ_Reverse();
		}

		gp_Ax2 Ax2 = gp_Ax2(origin, z_dir);//�ӿڵ���ά����ϵ
		Ax3 = gp_Ax3(Ax2);//�洢

		Standard_Real R = baseLen;//Բ����뾶���߶�
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
			QMessageBox::information(NULL, "��ʾ", "����ģ������", QMessageBox::Ok);
			return;
		}
		//else if (connectorType == tr("ControlBus"))
		//{
		//	Standard_Real N = 6;  // AxisControlBus����
		//	Standard_Real r = R / (sin(M_PI / N));
		//	vector<TopoDS_Shape> vShapes;
		//	
		//	TopoDS_Compound aRes;  // ����ģ��
		//	BRep_Builder aBuilder;
		//	aBuilder.MakeCompound(aRes);

		//	//����ControlBus���壬��N��AxisControlBus��ɣ��ֲ�����n���εĶ��㴦
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
	//4.ԭ��---һά�ӿڣ���������
	else if (defType == tr("ԭ��"))
	{
		Connector1->SetDefineType(ONE_POINT);

		TopoDS_Shape vShape = value41->getFeatureShape();
		//�洢����ʵ��
		RefEnts.push_back(value41->getFeatureName());

		if (vShape.IsNull())
		{
			QMessageBox::information(NULL, "��ʾ", "������Ϣ������", QMessageBox::Ok);
			return;
		}
		TopoDS_Vertex Vertex = TopoDS::Vertex(vShape);
		gp_Pnt theLoc = BRep_Tool::Pnt(Vertex);
		Ax3 = gp_Ax3(theLoc, aZ_Direction, aX_Direction);

		Standard_Real R = baseLen;//�뾶
		TopoDS_Shape topo_sphere = BRepPrimAPI_MakeSphere(theLoc, R).Shape();
		//Handle(AIS_Shape) ais_sphere = new AIS_Shape(topo_sphere);
		//myContext->Display(ais_sphere, Standard_True);

		Connector1->SetShape(topo_sphere);
		value41->initFeatureButton();
	}
	
	//���ݽӿ��������������Ӷ��ҵ��ӿ�����
	vector<string>::iterator res = std::find(ConnectorNames.begin(), ConnectorNames.end(), str_connName);
	int Index = distance(ConnectorNames.begin(), res);

	Connector1->SetConnName(str_connName.c_str());//�洢�ӿ���
	Connector1->SetConnType(ConnectorTypes[Index]);//�洢�ӿ����ͣ�Modelicaȫ��

	//λ��
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
	Connector1->SetDisplacement(displacement);//�洢λ��
	Connector1->SetRefEnts(RefEnts);//�洢����ʵ��
	
	//�洢�ӿ���Ϣ
	myDocument3d->AddConnector(Connector1);
	delete Connector1;
	myDocument3d->UpdateDisplay();

	////��������ϵ
	//Handle(AdaptorVec_AIS) z_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.Direction(), theLength, arrowLength);
	//z_Axis->SetText("  Z");
	//myContext->Display(z_Axis, Standard_True);
	//Handle(AdaptorVec_AIS) x_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.XDirection(), theLength, arrowLength);
	//x_Axis->SetText("  X");
	//myContext->Display(x_Axis, Standard_True);
	//Handle(AdaptorVec_AIS) y_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.YDirection(), theLength, arrowLength);
	//y_Axis->SetText("  Y");
	//myContext->Display(y_Axis, Standard_True);
	
	//��Ҫˢ�½ӿ������б�
	modelicaConnector->removeItem(modelicaConnector->currentIndex());
	if (modelicaConnector->count() == 0)
	{
		QDialog::close();
	}
}

//����AxisControlBus���壬Բ��+����
TopoDS_Compound ConnectorDlg::makeAxisControlBus(const gp_Ax2& Axes, const Standard_Real R, const Standard_Real H)
{
	TopoDS_Shape topo_cylinder = BRepPrimAPI_MakeCylinder(Axes, R, H).Shape();
	gp_Vec trans = gp_Vec(H * Axes.Direction());
	gp_Pnt theLoc = Axes.Location().Translated(trans);
	TopoDS_Shape topo_sphere = BRepPrimAPI_MakeSphere(theLoc, R).Shape();
	
	// ����ģ��
	TopoDS_Compound aRes;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(aRes);
	aBuilder.Add(aRes, topo_cylinder);
	aBuilder.Add(aRes, topo_sphere);
	return aRes;
}