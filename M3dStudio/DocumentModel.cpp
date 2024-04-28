//2021.11.08
//Wu YZ
#include "DDSData.h"
#include "DocumentModel.h"

#include "M3d.h"
#include "global.h"
//#include "DocumentModel.h"
#include "Transparency.h"
#include "mainwindow.h"
#include "translate.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QStatusBar>
#include <Standard_WarningsRestore.hxx>
#include <AIS_ViewCube.hxx>
#include <AIS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <OSD_Environment.hxx>
#include <TDocStd_Application.hxx>
#include <TCollection_AsciiString.hxx>
#include <TPrsStd_AISViewer.hxx>
#include <TPrsStd_AISPresentation.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_DriverTable.hxx>
#include <TDF_ChildIterator.hxx>
#include <qfiledialog.h>
#include <BinDrivers.hxx>
#include <TNaming_Builder.hxx>
#include <qmessagebox.h>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TNaming_Tool.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
//
#include <PCDM_RetrievalDriver.hxx>
#include <PCDM_StorageDriver.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <Geom_Transformation.hxx>
#include <TopExp_Explorer.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Reference.hxx>
#include <Prs3d_LineAspect.hxx>
#include "AdaptorVec_AIS.h"
#include <BRepBuilderAPI_Transform.hxx>
#include <Geom_BezierCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include "AIS_myManipulator.hxx"
#include "Connector.h"
#include "AssemblyTreeWidget.h"
#include "LibPathsSetDlg.h"
#include <set>
#include <queue>
#include "animationThread.h"
#include "CSV_Parser.h"
#include "AnimationWidget.h"
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include "omp.h"
#include "time.h"
#include "UpdateThread.h"
#include "SimulateSetDlg.h"
#include <QFileInfo>
#include <QDateTime>

using namespace CSV_Parser;
using namespace std;

//#include <BRepTools.hxx>
//#include <iostream>
//#include <fstream>
//using namespace std;


DocumentModel::DocumentModel(MainWindow* theApp)
	: DocumentCommon(theApp)
{
	m_IsModel = true;
	createNewDoc();

	m_pAnimateThread = new AnimationThread(this);
	m_pTwinAniThread = new TwinAnimationThread(this);
}

DocumentModel::~DocumentModel()
{
	m_pAnimateThread->wait();  //����ȴ��߳̽���
	m_pTwinAniThread->wait();
	delete m_pCompRoot;
	delete m_pAnimateThread;
	delete m_pTwinAniThread;
}

//������ʲô...
void DocumentModel::Clear()
{
	m_pDocRoot.ForgetAllAttributes();
}

void DocumentModel::createNewDoc()
{
	DocumentCommon::createNewDoc();

	//anOcaf_Application = new TDocStd_Application;
	//anOcaf_Application->DefineFormat("m3dmdl", "M3d system Format", "m3dmdl"/*, \
    //    new NewDocumentFormat_RetrievalDriver(), new NewDocumentFormat_StorageDriver()*/, nullptr, nullptr);

	//anOcaf_Application->NewDocument("m3dmdl"/*"BinOcaf"*/, m3dOcafDoc);//��ȱʡ�����ٸ�
	//TPrsStd_AISViewer::New(m3dOcafDoc->Main(), myViewer);

	//m_pDS = new DocumentModel(m3dOcafDoc->Main());

	////Handle(AIS_InteractiveContext) anAisContext;
	//TPrsStd_AISViewer::Find(m3dOcafDoc->Main(), myContext/*anAisContext*/);
	////anAisContext->SetDisplayMode(AIS_Shaded, Standard_True);
	//myContext->SetDisplayMode(AIS_Shaded, Standard_True);
	////myContext = anAisContext;

	//// Set the maximum number of available "undo" actions
	//m3dOcafDoc->SetUndoLimit(30);

	m_pCompRoot = new Component(this);
	//m_pDS->SetCompRoot(m_pCompRoot);
	m_pCompRoot->SetCompName("Untitled");
	//m_pCompRoot->SetIsAssebly(true);
}

//��������ļ����͸��ڵ㣬�Զ����������
string DocumentModel::GenerateCompName(Component* pCom)
{
	QString sFileName = pCom->GetCompFile();
	string sfilePre = QFileInfo(sFileName).baseName().toStdString();

	Component* pParent = pCom->GetParent();
	list<Component*> lstChilds = pParent->GetChilds();
	list<Component*>::iterator itr;
	unsigned int num = 1; 
	string compName;

	while (1)
	{
		bool bHave = false;
		compName = sfilePre;
		compName += to_string(num);
		for (itr = lstChilds.begin(); itr != lstChilds.end(); ++itr)
		{
			Component* pChild = *itr;
			if (compName == pChild->GetCompName())
			{
				bHave = true;
				break;
			}
		}
		if (bHave)
		{
			//������ڣ����Զ���һ
			num++;
		}
		else {
			break;
		}
	}

	return compName;
}

void DocumentModel::Assemble(Component* pCom1, Connector* pCon1, Component* pCom2, Connector* pCon2)
{
	// ����װ��ͼ
	unordered_map<string, string> assembly_graph;
	unordered_map<string, unordered_set<string>> name_map;
	// �����������,װ����A->B->C,B�����A�е�����Ϣ,����B���A���ˣ���ʱB�����C�ĵ�����Ϣ��δ����������B����SetDeltaTransform�Ƿ����ң�
	// ������:�������curr_comp�Ľӿڷ����������ĳ��assembly_comp�ĵ�����Ϣ,��ô���õ�����Ϣ��������,curr_comp��deltaTransform��last_comp��Ϊ׼��ͨ�����ұ���Ĺ�ϣ��������һ���Ƿ����������һ���ĵ���
	// deltaTransform���������һ��װ����ߵ��˵����
	unordered_map<string, pair<string, vector<double>>> last_reverse_deltatrans_record; // ���curr_comp�������assembly_comp�ĵ���,��������
	unordered_map<string, gp_Ax3> origin_Ax3_record; // �洢��װ��Լ���ӿڱ任֮ǰ��ȫ��gp_Ax3
	for (auto constraint : GetAllConnConstraints())
	{
		assembly_graph[constraint.component1 + '+' + constraint.connector1] = constraint.component2 + '+' + constraint.connector2;
		assembly_graph[constraint.component2 + '+' + constraint.connector2] = constraint.component1 + '+' + constraint.connector1;
		name_map[constraint.component1].emplace(constraint.component1 + '+' + constraint.connector1);
		name_map[constraint.component2].emplace(constraint.component2 + '+' + constraint.connector2);
	}
	// ͳ��������Ҫ�任��comp
	unordered_set<string> trans_comp_name;

	//�ж�ÿһ������Ƿ���й����˲���,������й�����,��Ҫ�ж�װ������װ��˳��͵��˵�������ϵ
	//�洢�����+�ӿ���
	queue<string> Q;
	unordered_set<string> visited;//�ж��Ƿ��Ѿ������
	Q.emplace(pCom2->GetCompName() + '+' + pCon2->GetConnName());
	visited.emplace(pCom2->GetCompName());
	while (!Q.empty())
	{
		string curr_name = Q.front();
		int curr_pos = curr_name.find('+');
		string curr_comp_name = curr_name.substr(0, curr_pos);
		string curr_conn_name = curr_name.substr(curr_pos + 1, curr_name.length() - curr_pos - 1);
		Q.pop();
		Component* curr_comp = m_pCompRoot->GetChild(curr_comp_name.c_str());
		for (auto& other_name : name_map[curr_comp_name])
		{
			int other_pos = other_name.find('+');
			string other_conn_name = other_name.substr(other_pos + 1, other_name.length() - other_pos - 1);
			Connector* other_conn = dynamic_cast<DocumentComponent*>(curr_comp->GetDocument())->GetConnector(other_conn_name.c_str());
			// other_conn��globalAx3
			gp_Ax3 other_globalAx3 = GetCompConnGlobalAx3(curr_comp_name.c_str(), other_conn_name.c_str());
			// ��¼
			origin_Ax3_record.emplace(other_name, other_globalAx3);
			// �ҵ�����װ��Ľӿ�
			if (other_conn_name != curr_conn_name)
			{
				//�ҵ�����������������ͽӿ�
				string assembly_name = assembly_graph[other_name];
				int pos = assembly_name.find('+');
				string assembly_comp_name = assembly_name.substr(0, pos);
				string assembly_conn_name = assembly_name.substr(pos + 1, assembly_name.length() - pos - 1);
				if (visited.count(assembly_comp_name))
				{
					continue;
				}
				visited.emplace(assembly_comp_name);
				Component* assembly_comp = m_pCompRoot->GetChild(assembly_comp_name.c_str());
				Connector* assembly_conn = dynamic_cast<DocumentComponent*>(assembly_comp->GetDocument())->GetConnector(assembly_conn_name.c_str());
				// �õ�curr_comp��deltaTransv15
				vector<double> curr_comp_v15 = GetCompDeltaTransformV15(curr_comp_name.c_str());
				// ���curr_comp_v15��ǰ9��ֵ��assembly_comp_conn��gp_Ax3���,����ڱ任,��Ҫ��assembly_comp��һ���෴�ı任,�ȼ�¼����
				gp_Ax3 assembly_globalAx3 = GetCompConnGlobalAx3(assembly_comp_name.c_str(), assembly_conn_name.c_str());
				if (fabs(curr_comp_v15[0] - assembly_globalAx3.Location().X()) < LINEAR_TOL && fabs(curr_comp_v15[1] - assembly_globalAx3.Location().Y()) < LINEAR_TOL && fabs(curr_comp_v15[2] - assembly_globalAx3.Location().Z()) < LINEAR_TOL \
					&& fabs(curr_comp_v15[3] - assembly_globalAx3.Direction().X()) < LINEAR_TOL && fabs(curr_comp_v15[4] - assembly_globalAx3.Direction().Y()) < LINEAR_TOL && fabs(curr_comp_v15[5] - assembly_globalAx3.Direction().Z()) < LINEAR_TOL \
					&& fabs(curr_comp_v15[6] - assembly_globalAx3.XDirection().X()) < LINEAR_TOL && fabs(curr_comp_v15[7] - assembly_globalAx3.XDirection().Y()) < LINEAR_TOL && fabs(curr_comp_v15[8] - assembly_globalAx3.XDirection().Z()) < LINEAR_TOL)
				{
					// assembly_comp��Ҫ�ı任������
					last_reverse_deltatrans_record[assembly_comp_name] = { curr_comp_name + '+' + other_conn_name,curr_comp_v15 };
					// curr_comp��deltaTrans���
					vector<double> reset_v15{ 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
					curr_comp->SetDeltaTransform(reset_v15);
				}
				//���뵽������
				Q.emplace(assembly_name);
			}
		}

		// ����curr_comp,��������last_comp+last_conn�е��˱任,����Ҫ����(last_comp+last_connֱ��ͨ��װ��ͼ�õ�)
		if (last_reverse_deltatrans_record.count(curr_comp_name))
		{
			string last_comp_conn_name = last_reverse_deltatrans_record[curr_comp_name].first;
			int last_pos = last_comp_conn_name.find('+');
			string last_comp_name = last_comp_conn_name.substr(0, last_pos);
			string last_conn_name = last_comp_conn_name.substr(last_pos + 1, last_comp_conn_name.length() - last_pos - 1);
			// �õ�last_comp_conn��ǰ��globalAx3
			gp_Ax3 last_comp_conn_gAx3_current = GetCompConnGlobalAx3(last_comp_name.c_str(), last_conn_name.c_str());
			// �ҵ�����Ҫ���б任����һ��װ��ӿں�v15����
			vector<double> v15_pre = last_reverse_deltatrans_record[curr_comp_name].second;
			vector<double> curr_displament = { last_comp_conn_gAx3_current.Location().X(),last_comp_conn_gAx3_current.Location().Y(), last_comp_conn_gAx3_current.Location().Z(), \
				last_comp_conn_gAx3_current.Direction().X(), last_comp_conn_gAx3_current.Direction().Y(), last_comp_conn_gAx3_current.Direction().Z(), \
				last_comp_conn_gAx3_current.XDirection().X(), last_comp_conn_gAx3_current.XDirection().Y(), last_comp_conn_gAx3_current.XDirection().Z() };
			vector<double> new_v15(curr_displament);
			// curr_comp��Ҫ�����last_comp��һ���෴�ı任
			for (int i = 9; i < 15; ++i)
			{
				new_v15.push_back(-v15_pre[i]);
			}
			curr_comp->SetDeltaTransform(new_v15);

			// ����curr_comp���б任
			gp_Ax3 oAx3;
			gp_Trsf oTrans;
			oTrans.SetDisplacement(oAx3, last_comp_conn_gAx3_current);
			// curr_conn�ֲ��ķ��任
			gp_Trsf tempTrans;
			gp_Ax3 curr_conn_localAx3 = GetCompConnLocalAx3(curr_comp_name.c_str(), curr_conn_name.c_str());
			tempTrans.SetDisplacement(curr_conn_localAx3, oAx3);
			curr_comp->SetOriginTransform(oTrans * tempTrans);
		}
		else
		{
			// last_comp���curr_compû�е��˱任
			// ����curr_comp���б任
			string last_comp_conn_name = assembly_graph[curr_name];
			int last_pos = last_comp_conn_name.find('+');
			string last_comp_name = last_comp_conn_name.substr(0, last_pos);
			string last_conn_name = last_comp_conn_name.substr(last_pos + 1, last_comp_conn_name.length() - last_pos - 1);
			gp_Ax3 oAx3;
			gp_Trsf oTrans;
			gp_Ax3 last_comp_conn_gAx3_current = GetCompConnGlobalAx3(last_comp_name.c_str(), last_conn_name.c_str());
			oTrans.SetDisplacement(oAx3, last_comp_conn_gAx3_current);
			// curr_conn�ֲ��ķ��任
			gp_Trsf tempTrans;
			gp_Ax3 curr_conn_localAx3 = GetCompConnLocalAx3(curr_comp_name.c_str(), curr_conn_name.c_str());
			tempTrans.SetDisplacement(curr_conn_localAx3, oAx3);
			curr_comp->SetOriginTransform(oTrans * tempTrans);
			// ���curr_comp�����last_comp�е�����Ϣ,��Ҫ����
			if (origin_Ax3_record.count(last_comp_conn_name))
			{
				gp_Ax3 origin_Ax3 = origin_Ax3_record[last_comp_conn_name];
				Component* last_comp = m_pCompRoot->GetChild(last_comp_name.c_str());
				Connector* last_conn = dynamic_cast<DocumentComponent*>(last_comp->GetDocument())->GetConnector(last_conn_name.c_str());
				gp_Ax3 new_Ax3 = GetCompConnGlobalAx3(last_comp_name.c_str(), last_conn_name.c_str());
				vector<double> curr_v15 = GetCompDeltaTransformV15(curr_comp_name.c_str());
				if (fabs(curr_v15[0] - origin_Ax3.Location().X()) < LINEAR_TOL && fabs(curr_v15[1] - origin_Ax3.Location().Y()) < LINEAR_TOL && fabs(curr_v15[2] - origin_Ax3.Location().Z()) < LINEAR_TOL\
					&& fabs(curr_v15[3] - origin_Ax3.Direction().X()) < LINEAR_TOL && fabs(curr_v15[4] - origin_Ax3.Direction().Y()) < LINEAR_TOL && fabs(curr_v15[5] - origin_Ax3.Direction().Z()) < LINEAR_TOL\
					&& fabs(curr_v15[6] - origin_Ax3.XDirection().X()) < LINEAR_TOL && fabs(curr_v15[7] - origin_Ax3.XDirection().Y()) < LINEAR_TOL && fabs(curr_v15[8] - origin_Ax3.XDirection().Z()) < LINEAR_TOL)
				{
					// ��Ҫ����curr_v15
					curr_v15[0] = new_Ax3.Location().X(), curr_v15[1] = new_Ax3.Location().Y(), curr_v15[2] = new_Ax3.Location().Z();
					curr_v15[3] = new_Ax3.Direction().X(), curr_v15[4] = new_Ax3.Direction().Y(), curr_v15[5] = new_Ax3.Direction().Z();
					curr_v15[6] = new_Ax3.XDirection().X(), curr_v15[7] = new_Ax3.XDirection().Y(), curr_v15[8] = new_Ax3.XDirection().Z();
					// ����6����̬��Ϣ����Ҫ�ı�
					curr_comp->SetDeltaTransform(curr_v15);
				}
			}
		}
		trans_comp_name.insert(curr_comp_name);
	}
	pCom2->Display(myContext);
	regenerateConn(pCom2);
	for (auto name : trans_comp_name)
	{
		Component* comp = m_pCompRoot->GetChild(name.c_str());
		comp->Display(myContext);
		regenerateConn(comp);
	}
}

void DocumentModel::regenerateConn(Component* change_component)
{
	vector<ConnectCurve> vCurves = GetAllConnectCurves();
	for (auto itrCC = vCurves.begin(); itrCC != vCurves.end(); ++itrCC)
	{
		if (change_component && (change_component->GetCompName() == itrCC->component1 || change_component->GetCompName() == itrCC->component2))
		{
			Component* comp1 = m_pCompRoot->GetChild(itrCC->component1.c_str());
			Connector* conn1 = dynamic_cast<DocumentComponent*>(comp1->GetDocument())->GetConnector(itrCC->connector1.c_str());
			Component* comp2 = m_pCompRoot->GetChild(itrCC->component2.c_str());
			Connector* conn2 = dynamic_cast<DocumentComponent*>(comp2->GetDocument())->GetConnector(itrCC->connector2.c_str());
			//ɾ��ԭ�е�������
			DeleteConnectCurve(*itrCC);
			deleteConncurvesAIS(comp1, conn1, comp2, conn2);
			//�����µ�������
			generateConncurvesAIS(comp1, conn1, comp2, conn2);
		}
	}
}

void DocumentModel::generateConncurvesAIS(Component* comp1, Connector* connector1, Component* comp2, Connector* connector2)
{
	//�������ո�ʽҪ����ַ���
	string curr_name = comp1->GetCompName() + '+' + connector1->GetConnName() + '#' + comp2->GetCompName() + '+' + connector2->GetConnName();

	//�������ӱ�
	TopoDS_Edge connEdge;
	//�õ��ӿ�1�ͽӿ�2��Z�᷽��
	vector<double> displament1 = connector1->GetDisplacement();
	gp_Ax3 conAx1 = MakeAx3(displament1);
	gp_Trsf Origintransform1 = comp1->GetOriginTransform();
	gp_Trsf DeltaTransform1 = comp1->GetDeltaTransform();
	gp_Trsf temptransform1 = DeltaTransform1 * Origintransform1;
	conAx1.Transform(temptransform1);
	vector<double> displament2 = connector2->GetDisplacement();
	gp_Ax3 conAx2 = MakeAx3(displament2);
	gp_Trsf Origintransform2 = comp2->GetOriginTransform();
	gp_Trsf DeltaTransform2 = comp2->GetDeltaTransform();
	gp_Trsf temptransform2 = DeltaTransform2 * Origintransform2;
	conAx2.Transform(temptransform2);
	gp_Pnt Loc1 = conAx1.Location();
	gp_Pnt Loc2 = conAx2.Location();
	//ȡ���ʱ����Ҫ���¿���,�����ڽǵ㴦��ȡֵ,��ѡ������⣿
	QVector<gp_Pnt> points;
	Standard_Real X1 = Loc1.X(), Y1 = Loc1.Y(), Z1 = Loc1.Z();
	Standard_Real X2 = Loc2.X(), Y2 = Loc2.Y(), Z2 = Loc2.Z();
	points.push_back(Loc1);
	points.push_back(gp_Pnt(X1, Y2, Z1));
	points.push_back(gp_Pnt(X1, Y2, Z2));
	points.push_back(Loc2);
	TColgp_Array1OfPnt Array(1, points.size());
	for (int i = 0; i < points.size(); ++i)
	{
		Array.SetValue(i + 1, points[i]);
	}
	Handle(Geom_BezierCurve) Bezier = new Geom_BezierCurve(Array);
	BRepBuilderAPI_MakeEdge ME(Bezier);
	connEdge = ME;
	//�浵
	ConnectCurve cc{ ConnectCurveType::single,comp1->GetCompName(),comp2->GetCompName(),connector1->GetConnName(),connector2->GetConnName(),{Loc1,gp_Pnt(X1,Y2,Z1),gp_Pnt(X1,Y2,Z2),Loc2},connEdge };
	//����ǿ�������,��������ʾ����
	//��ô�ж�?ͨ���ж��ǲ���MainConnector?
	if (connector1 == comp1->GetMainConnector() || connector2 == comp2->GetMainConnector())
	{
		cc.ccType = ConnectCurveType::controlBus;
		generateAuxiliaryConncurvesAIS(comp1, connector1, comp2, connector2);
	}
	AddConnectCurve(cc);
	//��ʾ������
	Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(connEdge);
	//���뵽��ϣ����
	connectcurves_map[curr_name].push_back(shpConn);
	myContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
	myContext->Display(shpConn, Standard_False);
	myContext->UpdateCurrentViewer();
	myContext->ClearSelected(true);
}

void DocumentModel::generateAuxiliaryConncurvesAIS(Component* comp1, Connector* connector1, Component* comp2, Connector* connector2)
{
	//�������ո�ʽҪ����ַ���
	string curr_name = comp1->GetCompName() + '+' + connector1->GetConnName() + '#' + comp2->GetCompName() + '+' + connector2->GetConnName();
	//�õ��ӿ�1�ͽӿ�2��Z�᷽��
	vector<double> displament1 = connector1->GetDisplacement();
	gp_Ax3 conAx1 = MakeAx3(displament1);
	gp_Trsf Origintransform1 = comp1->GetOriginTransform();
	gp_Trsf DeltaTransform1 = comp1->GetDeltaTransform();
	gp_Trsf temptransform1 = DeltaTransform1 * Origintransform1;
	conAx1.Transform(temptransform1);
	vector<double> displament2 = connector2->GetDisplacement();
	gp_Ax3 conAx2 = MakeAx3(displament2);
	gp_Trsf Origintransform2 = comp2->GetOriginTransform();
	gp_Trsf DeltaTransform2 = comp2->GetDeltaTransform();
	gp_Trsf temptransform2 = DeltaTransform2 * Origintransform2;
	conAx2.Transform(temptransform2);
	gp_Pnt Loc1 = conAx1.Location();
	gp_Pnt Loc2 = conAx2.Location();
	//ȡ���ʱ����Ҫ���¿���,�����ڽǵ㴦��ȡֵ,��ѡ������⣿
	Standard_Real X1 = Loc1.X(), Y1 = Loc1.Y(), Z1 = Loc1.Z();
	Standard_Real X2 = Loc2.X(), Y2 = Loc2.Y(), Z2 = Loc2.Z();
	//�����һ��
	TopoDS_Edge connEdge1;
	QVector<gp_Pnt> points1{ gp_Pnt(X1 - 0.05,Y1,Z1),gp_Pnt(X1 - 0.05,Y2,Z1),gp_Pnt(X1 - 0.05,Y2,Z2),gp_Pnt(X2 - 0.05,Y2,Z2) };
	TColgp_Array1OfPnt Array1(1, points1.size());
	for (int i = 0; i < points1.size(); ++i)
	{
		Array1.SetValue(i + 1, points1[i]);
	}
	Handle(Geom_BezierCurve) Bezier1 = new Geom_BezierCurve(Array1);
	BRepBuilderAPI_MakeEdge ME1(Bezier1);
	connEdge1 = ME1;
	Handle(AIS_InteractiveObject) shpConn1 = new AIS_Shape(connEdge1);
	myContext->SetColor(shpConn1, Quantity_NOC_YELLOW, Standard_False);
	myContext->Display(shpConn1, Standard_False);
	myContext->UpdateCurrentViewer();
	//���뵽��ϣ����
	connectcurves_map[curr_name].push_back(shpConn1);
	//�ұ���һ��
	TopoDS_Edge connEdge2;
	QVector<gp_Pnt> points2{ gp_Pnt(X1 - 0.05,Y1,Z1),gp_Pnt(X1 - 0.05,Y2,Z1),gp_Pnt(X1 - 0.05,Y2,Z2),gp_Pnt(X2 - 0.05,Y2,Z2) };
	TColgp_Array1OfPnt Array2(1, points2.size());
	for (int i = 0; i < points2.size(); ++i)
	{
		Array2.SetValue(i + 1, points2[i]);
	}
	Handle(Geom_BezierCurve) Bezier2 = new Geom_BezierCurve(Array2);
	BRepBuilderAPI_MakeEdge ME2(Bezier2);
	connEdge2 = ME2;
	Handle(AIS_InteractiveObject) shpConn2 = new AIS_Shape(connEdge2);
	myContext->SetColor(shpConn2, Quantity_NOC_YELLOW, Standard_False);
	myContext->Display(shpConn2, Standard_False);
	myContext->UpdateCurrentViewer();
	//���뵽��ϣ����
	connectcurves_map[curr_name].push_back(shpConn2);
}

void DocumentModel::deleteConncurvesAIS(Component* comp1, Connector* conn1, Component* comp2, Connector* conn2)
{
	string curr_name = comp1->GetCompName() + '+' + conn1->GetConnName() + '#' + comp2->GetCompName() + '+' + conn2->GetConnName();
	//�ӹ�ϣ�����ҵ����е���ʾʵ��
	vector<Handle(AIS_InteractiveObject)> connCurvesAIS = connectcurves_map[curr_name];
	for (auto iter = connCurvesAIS.begin(); iter != connCurvesAIS.end(); ++iter)
	{
		myContext->Erase(*iter, true);
	}
	//�ӹ�ϣ����ɾ��
	connectcurves_map.erase(curr_name);
	myContext->UpdateCurrentViewer();
}

//װ�䣺ȱʡ������ϵ�غ�,����
void DocumentModel::Assemble2Components()
{
	Component* pCom1 = nullptr; //�ƶ�����һ��
	Component* pCom2 = nullptr; //�ƶ��ڶ���
	Connector* pConn1, *pConn2;

	gp_Ax3 Ax3From, Ax3To;
	//�õ�����ѡ�е�ʵ�壬��Connector
	int cnt = myContext->NbSelected();
	if (myContext->NbSelected() != 2)
	{
		return;
	}
	vector<TopoDS_Shape> v2;
	for (myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
	{
		Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
		if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
			v2.push_back(shape);
		}
	}

	//�õ�Connector��Ax3
	if (m_pCompRoot)
	{
		list<Component*> comChilds = m_pCompRoot->GetChilds();
		list<Component*>::iterator itr;
		for (itr = comChilds.begin(); itr != comChilds.end(); ++itr)
		{
			Component* pComChild = *itr;
			gp_Trsf orgTrsf = pComChild->GetOriginTransform();
			gp_Trsf deltTrsf = pComChild->GetDeltaTransform();

			Connector* pConn = pComChild->GetConnector(v2[0]);
			if (pConn)
			{
				Ax3From = MakeAx3(pConn->GetDisplacement());
				Ax3From.Transform(orgTrsf);
				Ax3From.Transform(deltTrsf);
				pCom1 = pComChild;
				pConn1 = pConn;
			}
			pConn = pComChild->GetConnector(v2[1]);
			if (pConn)
			{
				Ax3To = MakeAx3(pConn->GetDisplacement());
				Ax3To.Transform(orgTrsf);
				Ax3To.Transform(deltTrsf);
				pCom2 = pComChild;
				pConn2 = pConn;
			}
		}
	}

	assert(pCom2 != nullptr);
	//Assemble(pCom2, pConn2, Ax3To, Ax3From);
	//��¼װ��Լ��
	ConnectConstraint cc;
	cc.component1 = pCom1->GetCompName();
	cc.component2 = pCom2->GetCompName();
	cc.connector1 = pConn1->GetConnName();
	cc.connector2 = pConn2->GetConnName();
	AddConnConstraint(cc);

	// myContext->UpdateCurrentViewer();
}

//�������
//��Ҫ����
void DocumentModel::insertComponent()
{
	//���ļ��Ի���ѡ���ļ� *.m3dcom��m3dmdl
	const QString fileName = QFileDialog::getOpenFileName((QWidget*)parent(), QString(), QString(), "���(*.m3dcom);;ϵͳ(*.m3dmdl)");
	if (fileName.isEmpty())
	{
		return;
	}
	insertComponent(fileName.toStdString().c_str());
}

void DocumentModel::insertComponent(const char* sFile)
{
	QString sFileName = sFile;
	Component* pComChild;
	if (QFileInfo(sFileName).suffix().toLower() == "m3dmdl")
	{
		DocumentModel* pMdl_Child = ReadModelFile(sFile);
		pComChild = pMdl_Child->GenerateComponent("");
		pMdl_Child->SetCompRoot(pComChild);
	}
	else {
		DocumentComponent* pCom_DS = DocumentComponent::ReadComponentFile(sFile);
		if (pCom_DS->IsWorld())
		{
			if (HasWorldComponent())
			{
				QMessageBox::information(NULL, "�������", "world������ܶ�β��롣");
				return;
			}
		}
		pComChild = pCom_DS->GenerateComponent();
		pCom_DS->SetCompRoot(pComChild);
	}

	//��¼���һ�����������
	gp_Pnt ptLast = { 0,0,0 };
	list<Component*> lstComps = m_pCompRoot->GetChilds();
	if (lstComps.size() > 0)
	{
		Component* pLastCom = lstComps.back();
		TopoDS_Shape theLastShp = pLastCom->GetCompound();
		GProp_GProps aGProps;
		BRepGProp::VolumeProperties(theLastShp, aGProps);
		//��������
		ptLast = aGProps.CentreOfMass();
	}

	m_pCompRoot->AddChild(pComChild);

	//�����㲿��������֣�������������
	string nComponent = GenerateCompName(pComChild);
	//���⴦�������World,������Ϊworld
	if (pComChild->IsWorld())
	{
		nComponent = "world";
	}
	pComChild->SetCompName(nComponent.c_str());

	//�ĵ�����
	InsertComponent(pComChild);
	if (pComChild->IsWorld())
	{
		ComponentAttrib attrib;
		attrib.bInner = true;
		SetComponentAttrib(nComponent.c_str(), attrib);
	}

	//pComChild->Display(myContext,false);

	//�ƶ�һ��: ����ǰ������������һ����������غϣ��������̫Զ
	vector<double> v15 = { 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
	gp_Pnt ptCOM = { 0,0,0 };
	vector<Connector*> vConns = pComChild->GetConnectors()/*GetDocument()->GetConnectors()*/;
	if (0) //vConns.size() > 0) ������
	{
		TopoDS_Shape theShp = vConns[0]->GetShape();
		GProp_GProps aGProps;
		BRepGProp::VolumeProperties(theShp, aGProps);
		ptCOM= aGProps.CentreOfMass();
	}
	else {
		TopoDS_Shape theShp = pComChild->GetCompound();
		GProp_GProps aGProps;
		BRepGProp::VolumeProperties(theShp, aGProps);
		ptCOM = aGProps.CentreOfMass();
	}
	v15[9] = ptLast.X() - ptCOM.X();
	v15[10] = ptLast.Y() - ptCOM.Y();
	v15[11] = ptLast.Z() - ptCOM.Z();
	pComChild->SetDeltaTransform(v15);

	//UpdateDisplay1();
	pComChild->Display(myContext);

	//ˢ����ʾװ����
	MainWindow* pWnd = (MainWindow*)parent();
	pWnd->UpdateAssemblyTree();
}

//����mdl_DS�����ݣ�����װ���塢��ʾ
void DocumentModel::UpdateDisplay(bool bRegen)
{
	if (!bRegen)
	{
		UpdateDisplay1();
		return;
	}

	//���ĵ�������Component����m_pCompRoot
	QString sPath = QFileInfo(myfileName).absolutePath();
	m_pCompRoot = GenerateComponent(sPath);

	//map<string, CompBaseInfo> mapComps = GetAllCompInfos();
	//map<string, CompBaseInfo>::iterator itr = mapComps.begin();
	//for (; itr != mapComps.end(); ++itr)
	//{
	//    CompBaseInfo comInfo = itr->second;
	//    ReadFile(comInfo.sCompFile.c_str());
	//}
	//m_mpFileDC.insert(make_pair(myfileName.toStdString(), this));
	    
	string sCompName = QFileInfo(myfileName).baseName().toStdString();
    if (sCompName.empty())
    {
        sCompName = "Untitled";
    }
    m_pCompRoot->SetCompName(sCompName.c_str());

	//��ʾm_pCompRoot
	UpdateDisplay1();

	//Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
	////myContext->ResetLocation(aViewCube);
	//gp_Trsf trsf;
	//myContext->SetLocation(aViewCube, trsf);
	//myContext->Display(aViewCube, true);
}

//�ȴ�����һ�㲿����Compound,��¼������״-��ɫ����״-͸���ȣ���¼�ο��㡢�ߺͽӿ�
//��ʾ����ʾColoredShape����ʾ�ο��㡢�ο��ߺͷǵ�һ��ӿڣ���ʾ��һ��ӿ�
//����Component����ʾʵ�弯ˢ����ʾ
//��װ�䡢����ʱ�����Բ��õ��˺�����ֱ�ӶԸ������������ʾ��ˢ����ʾ����
void DocumentModel::UpdateDisplay1()
{
	////��������������������壬����ԭʼλ�˷���
	//m_pCompRoot->GenerateCompound(transf);
	////������ѡ�񽻻����󣺵�һ������ͽӿ�
	////AIS_ColoredShape����һ��������������ӿڣ����ӿ�
	//m_pCompRoot->GenerateAISObjects(transf);
	////��ʾ��������
	//m_pCompRoot->Display(myContext, transf);

	myContext->RemoveAll(false);

	//�Ե�ǰװ���µ���װ����õ����ǵ�����壬��ʾ
	list<Component*> lstComps = m_pCompRoot->GetChilds();
	list<Component*>::iterator itr = lstComps.begin();

	//��listת��Ϊvector�Ա��ڶ��̵߳ķ���
	vector<Component*> vecComps;
	for (; itr != lstComps.end(); itr++)
	{
		Component* pCom = *itr;
		vecComps.push_back(pCom);
	}
	vector<Component*>::iterator iter = vecComps.begin();

	//����ʾ���
	for (int i = 0; i < vecComps.size(); ++i)
	{
		Component* pCom = *(iter + i);
		vector<Handle(AIS_InteractiveObject)> m_vAIS = pCom->GetAIS();
		for (int i = 0; i < m_vAIS.size(); i++)
		{
			myContext->Remove(m_vAIS[i], false);
		}
	}
	iter = vecComps.begin();//���������ٴ��������


	//��ȡ���ݸ�����ʾ��
	bool m_bStop = false;
	bool m_bRunning = false;
	int countThread = 0;//��¼�Ѿ����е��߳�����
	int maxWorkingThread = 4;//���ͬʱ�������߳�����
	set<UpdateThread*> sThreads;
	//��ʼִ��������;
	while (1)
	{

		if (m_bStop)
		{
			break;
		}

		m_bRunning = true;

		//ִ��һ�μ���: ���������߳���ĿС���趨�Ĳ�����ʱ�������̣߳�
		if (sThreads.size() < maxWorkingThread && countThread < vecComps.size())//ÿ���������4���߳�;
		{
			//����������ʾ����;
			UpdateThread* pThread = new UpdateThread();
			sThreads.insert(pThread);
			pThread->SetContext(myContext);
			pThread->SetComponent(*(iter + countThread));
			countThread++;
			pThread->start();
		}


		//�ж��߳��Ƿ�ִ����ϣ���ִ����ϵ��߳�ȡ��
		//�������ݣ��õ�������������
		for (set<UpdateThread*>::iterator iterSet = sThreads.begin(); iterSet != sThreads.end();)
		{
			UpdateThread* pThread = *iterSet;
			if (pThread->m_bStop == true)
			{
				set<UpdateThread*>::iterator itmp = iterSet;//����Ҫɾ����ָ��
				iterSet++;
				//delete(*itmp);
				sThreads.erase(itmp);
			}
			else
			{
				iterSet++;
			}
		}


		//���sThreadsΪ�գ���m_bStop��Ϊtrue,�˳���ѭ��
		if (sThreads.empty())
		{
			m_bStop = true;
		}
	}

	//�������ʾ
	iter = vecComps.begin();
	//#pragma omp parallel for num_threads(4)
	for (int i = 0; i < vecComps.size(); ++i)
	{
		Component* pCom = *(iter + i);
		vector<Handle(AIS_InteractiveObject)> vAIS = pCom->GetAIS();
		for (int i = 0; i < vAIS.size(); i++)
		{
			myContext->Display(vAIS[i], Standard_False);
		}
	}

	//��ʾ������
	vector<ConnectCurve> vCurves = GetAllConnectCurves();
	//����������߱��޸�,��Ҫ�������������ߵ�����ʵ��
	auto itrCC = vCurves.begin();
	for (; itrCC != vCurves.end(); ++itrCC)
	{
		ConnectCurve cc = *itrCC;
		TopoDS_Shape theShp = cc.shape;
		Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(theShp);
		//����ʾ��ʵ�尴��������ʽ���뵽��ϣ����
		string curr_name = itrCC->component1 + '+' + itrCC->connector1 + '#' + itrCC->component2 + '+' + itrCC->connector2;
		connectcurves_map[curr_name].push_back(shpConn);
		myContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
		myContext->Display(shpConn, Standard_False);
		//����ǿ�������,��Ҫ���ɸ���������
		if (itrCC->ccType == 3)
		{
			Component* comp1 = m_pCompRoot->GetChild(itrCC->component1.c_str());
			Connector* conn1 = dynamic_cast<DocumentComponent*>(comp1->GetDocument())->GetConnector(itrCC->connector1.c_str());
			Component* comp2 = m_pCompRoot->GetChild(itrCC->component2.c_str());
			Connector* conn2 = dynamic_cast<DocumentComponent*>(comp2->GetDocument())->GetConnector(itrCC->connector2.c_str());
			generateAuxiliaryConncurvesAIS(comp1, conn1, comp1, conn2);
		}
		//myDisplayList.insert(make_pair(pCom->GetCompName(), shpConn));
	}
	DisplayGlobalCoordsys();
	myContext->UpdateCurrentViewer();

	//ˢ����ʾװ����
	MainWindow* pWnd = (MainWindow*)parent();
	pWnd->UpdateAssemblyTree();
}

void DocumentModel::ReadFile(const char* csfileName)
{
	QString sFileName = csfileName;
	if (QFileInfo(sFileName).suffix().toLower() == "m3dmdl") //װ��
	{
		//���ļ��������ĵ�
		DocumentModel* pMdl_DS = DocumentModel::ReadModelFile(csfileName);

		////�����ĵ��������������
		//m_mpFileDC.insert(make_pair(csfileName, pMdl_DS));

		vector<CompBaseInfo> mapComps = pMdl_DS->GetAllCompInfos();
		vector<CompBaseInfo>::iterator itr = mapComps.begin();
		for (; itr != mapComps.end(); ++itr)
		{
			CompBaseInfo comInfo = *itr;
			ReadFile(comInfo.sCompFile.c_str());
		}
	}
	else //���
	{
		DocumentComponent* pCom_DS = DocumentComponent::ReadComponentFile(csfileName);
		//m_mpFileDC.insert(make_pair(csfileName, pCom_DS));
	}
}
////��Ҫ����
////�ƻ�����Undo/Redoʱ���ã����ĵ���ȡ��ˢ����ʾ
//void DocumentModel::UpdateDisplay1()
//{
//    //gp_Trsf Trsf; //ȱʡ���죬��λ����
//    //m_pCompRoot->Display(myContext, Trsf);
//    myContext->RemoveAll(false);
//    myDisplayList.clear();
//
//    delete m_pCompRoot; //�������
//    m_pCompRoot = GenerateComponent(myfileName.toStdString().c_str());
//    string sCompName = QFileInfo(myfileName).baseName().toStdString();
//    if (sCompName.empty())
//    {
//        sCompName = "Untitled";
//    }
//    m_pCompRoot->SetCompName(sCompName.c_str());
//    //SetCompRoot(m_pCompRoot);
//
//    //�Ե�ǰװ���µ���װ����õ����ǵ�����壬��ʾ
//    list<Component*> lstComps = m_pCompRoot->GetChilds();
//    list<Component*>::iterator itr = lstComps.begin();
//    for (; itr != lstComps.end(); ++itr)
//    {
//        Component* pCom = *itr;
//        vector<ShapeMaterial> shpMats; //��״�Ĳ���;
//        vector<ShapeTransparency> shpTransparencys; //��״��͸����;
//        vector<ShapeColor> shpColors;//��״����ɫ��
//        vector<TopoDS_Shape> shpDatumPts; //�任��Ĳο���
//        vector<TopoDS_Shape> shpDatumLns; //�任��Ĳο���
//        vector<TopoDS_Shape> shpConnectos; //�任��Ľӿ�
//        vector<gp_Ax3> coordSys; //�任��Ľӿ�����ϵͳ��ʾ
//        TopoDS_Shape comShape = pCom->GenerateCompound1(shpMats, shpTransparencys, shpColors, shpDatumPts, shpDatumLns, shpConnectos, coordSys);//GetCompound();
//        //vector<double> v18 = pCom->GetDisplacement();
//        //gp_Trsf trans1 = MakeTransform(v18);
//        //BRepBuilderAPI_Transform myBRepTransformation(comShape, trans1);
//        //TopoDS_Shape TransformedShape = myBRepTransformation.Shape();
//
//        Handle(AIS_InteractiveObject) shpAIS = new AIS_Shape(comShape);
//        myContext->Display(shpAIS, false);
//        myDisplayList.insert(make_pair(pCom->GetCompName(), shpAIS));
//
//        //��ʾ����;
//        for (int i = 0; i < shpMats.size(); i++)
//        {
//            TopoDS_Shape theShp = shpMats[i].theShape;
//            Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(theShp);
//            myContext->Display(theAIS, false);
//            myContext->SetMaterial(theAIS, shpMats[i].theMaterial, false);
//        }
//        //��ʾ͸����
//        for (int i = 0; i < shpTransparencys.size(); i++)
//        {
//            TopoDS_Shape theShp = shpTransparencys[i].theShape;
//            Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(theShp);
//            myContext->Display(theAIS, false);
//            myContext->SetTransparency(theAIS, shpTransparencys[i].transparent, false);
//        }
//        //��ʾ��ɫ��
//        for (int i = 0; i < shpColors.size(); i++)
//        {
//            TopoDS_Shape theShp = shpColors[i].theShape;
//            Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(theShp);
//            myContext->Display(theAIS, false);
//            myContext->SetColor(theAIS, shpColors[i].theColor, false);
//        }
//        //��ʾ�ο���
//        for (int i = 0; i < shpDatumPts.size(); i++)
//        {
//            TopoDS_Shape theShp = shpDatumPts[i];
//            Handle(AIS_InteractiveObject) datPoint = new AIS_Shape(theShp);
//            myContext->SetColor(datPoint, Quantity_NOC_YELLOW, Standard_True);
//            myContext->Display(datPoint, Standard_True);
//        }
//        //��ʾ�ο���
//        for (int i = 0; i < shpDatumLns.size(); i++)
//        {
//            TopoDS_Shape theShp = shpDatumLns[i];
//            Handle(AIS_InteractiveObject) datLine = new AIS_Shape(theShp);
//            Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_YELLOW, Aspect_TOL_DOTDASH, 1);
//            datLine->Attributes()->SetWireAspect(lineAspect);
//            myContext->Display(datLine, Standard_True);
//        }
//        //��ʾ�ӿ�
//        for (int i = 0; i < shpConnectos.size(); i++)
//        {
//            TopoDS_Shape theShp = shpConnectos[i];
//            Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(theShp);
//            myContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_True);
//            myContext->Display(shpConn, Standard_True);
//        }
//        //��ʾ�ӿ�����ϵͳ
//        for (int i = 0; i < coordSys.size(); i++)
//        {
//            gp_Ax3 Ax3 = coordSys[i];
//
			//double theLength, arrowLength;
			//GetAx3Length(theLength, arrowLength);
			//Handle(AdaptorVec_AIS) z_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.Direction(), theLength, arrowLength);
			//z_Axis->SetText("  Z");
			//myContext->Display(z_Axis, Standard_True);
			//Handle(AdaptorVec_AIS) x_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.XDirection(), theLength, arrowLength);
			//x_Axis->SetText("  X");
			//myContext->Display(x_Axis, Standard_True);
			//Handle(AdaptorVec_AIS) y_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.YDirection(), theLength, arrowLength);
			//y_Axis->SetText("  Y");
			//myContext->Display(y_Axis, Standard_True);
//        }
//    }
//
//    //��ʾ������
//    vector<ConnectCurve> vCurves = GetAllConnectCurves();
//    auto itrCC = vCurves.begin();
//    for (; itrCC != vCurves.end(); ++itrCC)
//    {
//        ConnectCurve cc = *itrCC;
//        TopoDS_Shape theShp = cc.shape;
//        Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(theShp);
//        myContext->SetColor(shpConn, Quantity_NOC_GREEN, Standard_True);
//        myContext->Display(shpConn, Standard_True);
//    }
//
//    myContext->UpdateCurrentViewer();
//
//    //ˢ����ʾװ����
//    MainWindow* pWnd = (MainWindow*)parent();
//    pWnd->UpdateAssemblyTree();
//}

bool DocumentModel::saveFile(const QString gfileName)
{
	if (!DocumentCommon::saveFile(gfileName))
	{
		return false;
	}

	//�������������
	m_pCompRoot->SetCompName(QFileInfo(myfileName).baseName().toStdString().c_str());

	//ˢ����ʾװ����
	MainWindow* pWnd = (MainWindow*)parent();
	pWnd->UpdateAssemblyTree();

	////����ӳ���
	//m_mpFileDC.insert(make_pair(myfileName.toStdString(), this));

	return true;
}

//���ĵ������ݣ����������
//ֻ�������ι�ϵ: ���������������ĵ��������
Component* DocumentModel::GenerateComponent(const QString& currPath/*DocumentCommon* pDoc*/)
{
	Component* pComRet = nullptr;

	if (1)//pDoc->IsModel()) //װ��
	{
		DocumentModel* pMdl_DS = this;// (DocumentModel*)pDoc;
		////�����ĵ��������������
		//pComRet = pMdl_DS->GenerateTopComponent(); //ÿ�����������Ϣ����
		pComRet = new Component(this);// pDoc);

		vector<CompBaseInfo> mapComps = pMdl_DS->GetAllCompInfos();
		vector<CompBaseInfo>::iterator itr2 = mapComps.begin();
		for (; itr2 != mapComps.end(); ++itr2)
		{
			CompBaseInfo comInfo = *itr2;
			QString sFileName = comInfo.sCompFile.c_str();
			if (!QFileInfo(sFileName).exists())
			{
				//��ʾ�ļ������ڣ��ڵ�ǰ·��������
				//QMessageBox::information(NULL, "ģ���ļ�δ�ҵ�", sFileName, QMessageBox::Ok);
				
				sFileName = currPath + tr("/") +  QFileInfo(sFileName).fileName();
				if (!QFileInfo(sFileName).exists())
				{
					QMessageBox::information(NULL, "ģ���ļ���ʧ����", "ͬһ·����Ҳδ�ҵ���", QMessageBox::Ok);
					continue;
				}
			}
			Component* pComChild;
			if (QFileInfo(sFileName).suffix().toLower() == "m3dmdl")
			{
				DocumentModel* pMdl_Child = ReadModelFile(sFileName.toStdString().c_str()/*comInfo.sCompFile.c_str()*/);
				if (!pMdl_Child)
					continue;

				pComChild = pMdl_Child->GenerateComponent(currPath);
				pMdl_Child->SetCompRoot(pComChild);
			}
			else {
				DocumentComponent* pCom_DS = DocumentComponent::ReadComponentFile(sFileName.toStdString().c_str()/*comInfo.sCompFile.c_str()*/);
				if (!pCom_DS)
					continue;

				pComChild = pCom_DS->GenerateComponent();
				pCom_DS->SetCompRoot(pComChild);
			}
			pComChild->SetCompName(comInfo.sCompName.c_str());
			pComRet->AddChild(pComChild);
		}
	}
	else //���
	{
		DocumentComponent* pCom_DS = DocumentComponent::ReadComponentFile(/*pDoc*/this->GetFileName().toStdString().c_str());
		if (pCom_DS)
		{
			pComRet = pCom_DS->GenerateComponent();
			pCom_DS->SetCompRoot(pComRet);
		}
	}

	return pComRet;
}

//���ļ�������Component�����
////��Ҫ����
//Component* DocumentModel::GenerateComponent(const char* csFileName)
//{
//    Component* pComRet = nullptr;
//
//    QString sFileName = csFileName;  
//    if (sFileName.isEmpty() || QFileInfo(sFileName).suffix().toLower() == "m3dmdl") //װ��
//    {
//        DocumentModel* pMdl_DS;
//        if (sFileName.isEmpty())
//        {
//            pMdl_DS = this;
//        }
//        else {
//            //���ļ��������ĵ�
//            map<string, DocumentCommon*>::iterator itr = m_mpFileDC.find(csFileName);
//            assert(itr != m_mpFileDC.end());
//            pMdl_DS = (DocumentModel*)itr->second; //DocumentModel::ReadFile(csFileName);
//        }
//        //�����ĵ��������������
//        pComRet = pMdl_DS->GenerateTopComponent(); //ÿ�����������Ϣ����
//        //pComRet->SetIsAssebly(true);
//
//        map<string, CompBaseInfo> mapComps = pMdl_DS->GetAllCompInfos();
//        map<string, CompBaseInfo>::iterator itr2 = mapComps.begin();
//        for (; itr2 != mapComps.end(); ++itr2)
//        {
//            CompBaseInfo comInfo = itr2->second;
//            Component * pComChild = GenerateComponent(comInfo.sCompFile.c_str());
//            pComChild->SetCompName(comInfo.sCompName.c_str());
//            //pComChild->SetCompFile(comInfo.sCompFile.c_str());
//            //pComChild->SetDisplacement(comInfo.vCompDisplacement);
//            pComRet->AddChild(pComChild);
//        }
//    }
//    else //���
//    {
//        map<string, DocumentCommon*>::iterator itr = m_mpFileDC.find(csFileName);
//        assert(itr != m_mpFileDC.end());
//        DocumentComponent* pCom_DS = (DocumentComponent*)itr->second; //M3dCom_DS::ReadFile(csFileName);
//        pComRet = pCom_DS->GenerateComponent();
//        //pComRet->SetIsAssebly(false);
//    }
//
//    return pComRet;
//}

void DocumentModel::SetMainShape(const TopoDS_Shape& aSolid)
{

}

//����ѡ�����
bool DocumentModel::HideSelected()
{
	if (!myContext->NbSelected())
		return false;

	myContext->InitSelected();
	while (myContext->MoreSelected())
	{
		Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
		if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
			string sName = GetShapeName(shape);

			//�������
			Component* pCom = m_pCompRoot->GetChild(sName.c_str());
			if (pCom)
			{
				auto vAis = pCom->GetAIS();
				for (int i = 0; i < vAis.size(); i++)
				{
					myContext->Erase(vAis[i], true);
				}
			}
			else {
				//�������������ɾ��
				myContext->NextSelected();
				continue;
			}
		}
	}

	return true;
}

//ɾ��ѡ������
bool DocumentModel::DeleteSelected()
{
	if (!myContext->NbSelected())
		return false;

	myContext->InitSelected();
	while ( myContext->MoreSelected() )
	{
		Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
		if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
			string sName = GetShapeName(shape);

			//ɾ�����
			Component* pCom = m_pCompRoot->GetChild(sName.c_str());
			if (pCom)
			{
				//���ĵ�ɾ��
				DeleteComponent(pCom);

				//��ComRootɾ��
				m_pCompRoot->DeleteChild(sName.c_str());

				myContext->NextSelected();
				//myContext->Erase(obj, true);
			}
			//ɾ��������
			else if (sName.substr(0, 8) == "connect(")
			{
				//��ComRootɾ��

				//���ĵ�ɾ��
				ConnectCurve* pCurve = GetConnectCurve(shape);
				DeleteConnectCurve(*pCurve);
				delete pCurve;

				myContext->NextSelected();
			}
			else {
				//�������������ɾ��
				myContext->NextSelected();
				continue;
			}
		}
	}

	UpdateDisplay1();//�����¶��ļ�

	return true;
}

//�����������������
void DocumentModel::Connect2Components()
{
	//�õ�����ѡ�е�ʵ�壬��Connector
	int cnt = myContext->NbSelected();
	if (myContext->NbSelected() != 2)
	{
		return;
	}
	vector<TopoDS_Shape> v2;
	for (myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
	{
		Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
		if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
			v2.push_back(shape);
		}
	}

	//
	Component* pCom1 = nullptr;
	Component* pCom2 = nullptr;
	Connector* pConn1 = nullptr;
	Connector* pConn2 = nullptr;

	if (m_pCompRoot)
	{
		list<Component*> comChilds = m_pCompRoot->GetChilds();
		list<Component*>::iterator itr;
		for (itr = comChilds.begin(); itr != comChilds.end(); ++itr)
		{
			Component* pComChild = *itr;
			Connector* pConn = pComChild->GetConnector(v2[0]);
			if (pConn)
			{
				pCom1 = pComChild;
				pConn1 = pConn;
			}
			pConn = pComChild->GetConnector(v2[1]);
			if (pConn)
			{
				pConn2 = pConn;
				pCom2 = pComChild;
			}
		}
	}

	assert(pCom1 != nullptr && pCom2 != nullptr && pConn1 != nullptr && pConn2 != nullptr);
	if (pCom1 == nullptr || pCom2 == nullptr || pConn1 == nullptr || pConn2 == nullptr)
	{
		return;
	}

	//�������ӱ�,��Bezier����
	TopoDS_Edge connEdge;
	gp_Pnt P1 = pConn1->GetCenter();
	gp_Pnt P2 = pConn2->GetCenter();
	TColgp_Array1OfPnt array(1, 2);
	array.SetValue(1, P1);
	array.SetValue(2, P2);
	Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(array);
	BRepBuilderAPI_MakeEdge ME(curve);
	connEdge = ME;

	//��¼���ĵ�
	ConnectCurve cc;
	cc.ccType = ConnectCurveType::single;
	cc.component1 = pCom1->GetCompName();
	cc.component2 = pCom2->GetCompName();
	cc.connector1 = pConn1->GetConnName();
	cc.connector2 = pConn2->GetConnName();
	cc.shape = connEdge;
	cc.lstPoints.push_back(P1);
	cc.lstPoints.push_back(P2);
	AddConnectCurve(cc);

	UpdateDisplay1();
}

void DocumentModel::HilightComponent(const char* sComName)
{

}

void DocumentModel::HighLightSelectedComponent()
{
	MainWindow* pWnd = (MainWindow*)parent();
	AssemblyTreeWidget* pAssemWnd = pWnd->GetAssemblyTreeWidget();

	//�ȵõ�ѡ�е����
	if (!myContext->NbSelected())
	{
		pAssemWnd->HighLightItem("");
		return;
	}	

	myContext->InitSelected();//ֻ����һ����
	Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
	if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
	{
		TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
		string sName = GetShapeName(shape);

		//���
		Component* pCom = m_pCompRoot->GetChild(sName.c_str());
		if (pCom)
		{
			//���Ը�Item����װ������
			pAssemWnd->HighLightItem(sName.c_str());
		}
	}
}

//�������
void DocumentModel::RenameComponent(const QString& oldName, const QString& newName)
{
	//�������
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//��������
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompNameI = ExtString2string(s);

		if (oldName == sCompNameI.c_str())
		{//�����ĵ����������
			theLabel.ForgetAttribute(nameAttr);
			TDataStd_Name::Set(theLabel, newName.toStdString().c_str());
			break;
		}
	}

	//�ӿڣ�װ�䣩Լ������
	TDF_Label connLabel = m_pDocRoot.FindChild(2, false);
	if (!connLabel.IsNull())
	{
		Standard_Integer ncount = connLabel.NbChildren();
		for (int i = 1; i <= ncount; i++)
		{
			ConnectConstraint cc;
			TDF_Label lbl_i = connLabel.FindChild(i, false);

			//����
			Handle(TDataStd_Integer) iType;
			lbl_i.FindAttribute(TDataStd_Integer::GetID(), iType);
			if (iType.IsNull())
			{
				continue;
			}
			cc.kpType = (Kinematic_pair_type)iType->Get();
			//ȫ��
			Handle(TDataStd_Name) nameAttr;
			lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
			if (nameAttr.IsNull())
			{
				continue;
			}
			TCollection_ExtendedString s = nameAttr->Get();
			string sFullName = ExtString2string(s);
			string c1, c2, conn1, conn2;
			Make4Names(sFullName, c1, c2, conn1, conn2);
			if (c1 == oldName.toStdString() || c2 == oldName.toStdString())
			{
				if (c1 == oldName.toStdString())
				{
					c1 = newName.toStdString();
				}
				if (c2 == oldName.toStdString())
				{
					c2 = newName.toStdString();
				}
				//������֯ȫ��
				string ccName = Make1Name(c1, c2, conn1, conn2);
				lbl_i.ForgetAttribute(nameAttr);
				TDataStd_Name::Set(lbl_i, ccName.c_str());
			}
		}
	}

	//�����߲���
	TDF_Label connCurveLabel = m_pDocRoot.FindChild(3, false);
	if (!connCurveLabel.IsNull())
	{
		Standard_Integer ncount = connCurveLabel.NbChildren();
		for (int i = 1; i <= ncount; i++)
		{
			TDF_Label lbl_i = connCurveLabel.FindChild(i, false);

			if (lbl_i.IsNull())
			{
				continue;
			}

			//����
			Handle(TDataStd_Name) nameAttr;
			lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
			if (nameAttr.IsNull())
			{
				continue;
			}
			TCollection_ExtendedString s = nameAttr->Get();
			string sCurveName = ExtString2string(s);
			//��������
			string sCom1, sCom2, sConn1, sConn2;
			Make4Names(sCurveName, sCom1, sCom2, sConn1, sConn2);
			if (sCom1 == oldName.toStdString() || sCom2 == oldName.toStdString())
			{
				if (sCom1 == oldName.toStdString())
				{
					sCom1 = newName.toStdString();
				}
				if (sCom2 == oldName.toStdString())
				{
					sCom2 = newName.toStdString();
				}
				//������֯ȫ��
				string ccName = Make1Name(sCom1, sCom2, sConn1, sConn2);
				lbl_i.ForgetAttribute(nameAttr);
				TDataStd_Name::Set(lbl_i, ccName.c_str());
			}
		}
	}

	//�������ò���
	TDF_Label parLabel = m_pDocRoot.FindChild(4, false);
	if (!parLabel.IsNull())
	{
		Standard_Integer ncount = parLabel.NbChildren();
		for (int i = 1; i <= ncount; i++)
		{
			TDF_Label lbl_i = parLabel.FindChild(i, false);

			//ӳ������
			Handle(TDataStd_Name) nameAttr;
			lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
			if (nameAttr.IsNull())
			{
				continue;
			}
			TCollection_ExtendedString s = nameAttr->Get();
			string sMapName = ExtString2string(s);

			QString oldNameDot = oldName + ".";
			if (sMapName.find(oldNameDot.toStdString().c_str()) == 0)
			{
				sMapName.replace(0, oldName.length(), newName.toStdString().c_str());
				lbl_i.ForgetAttribute(nameAttr);
				TDataStd_Name::Set(lbl_i, sMapName.c_str());
			}
		}
	}
}

void DocumentModel::SelectComponent(const char* sComName)
{
	//Component* pCom = m_pCompRoot->GetChild(sComName);
	//auto itr = myDisplayList.find(sComName);
	if (1)//itr != myDisplayList.end())
	{
		//Handle(AIS_InteractiveObject) ais0 = itr->second;
		//myContext->SetSelected(ais0, true);
		//myContext->Hilight(ais0, true);//������ʾ�ˣ�����ɾ��
		AIS_ListOfInteractive aisLst;
		myContext->DisplayedObjects(aisLst);
		AIS_ListOfInteractive::iterator itr = aisLst.begin();
		for (; itr != aisLst.end(); ++itr)
		{
			Handle(AIS_InteractiveObject) aI = *itr;
			Handle(AIS_Shape) ais_shp = Handle(AIS_Shape)::DownCast(aI);

			if (ais_shp.IsNull())
			{
				continue;
			}

			TopoDS_Shape shp = ais_shp->Shape();
			string sName = GetShapeName(shp);
			if (sName == sComName)
			{
				myContext->SetSelected(aI, true);
			}
		}
	}
}

//bool DocumentModel::HasExit(const string& name)
//{
//    return myDisplayList.find(name) != myDisplayList.end();
//}
//
//Handle(AIS_InteractiveObject) DocumentModel::findfromMap(const string& name)
//{
//    return myDisplayList[name];
//}

//����
void DocumentModel::adjustComponent0()
{
	//��ѡ�е�������Ե�һ���ӿ�����ϵ��Z����ת30��
	int cnt = myContext->NbSelected();
	if (cnt != 1)
	{
		return;
	}

	myContext->InitSelected(); 
	Handle(AIS_InteractiveObject) theAIS = myContext->SelectedInteractive();
	TopoDS_Shape theSelShape = Handle(AIS_Shape)::DownCast(theAIS)->Shape();

	Component* pCom = nullptr;
	list<Component*> lstComps = m_pCompRoot->GetChilds();
	auto itr = lstComps.begin();
	for (; itr != lstComps.end(); ++itr)
	{
		pCom = *itr;
		if (pCom->GetCompound() == theSelShape)
		{
			break;
		}
	}
	if (!pCom) return;

	//��ת
	DocumentCommon* pDoc = pCom->GetDocument();
	vector<Connector*> vConn = pDoc->GetConnectors();
	if (vConn.size() == 0) return;

	//�Ե�һ���ӿ�
	Connector* pConn1 = vConn[0];
	vector<double> vDisp = pConn1->GetDisplacement();
	gp_Trsf originTrf = pCom->GetOriginTransform();
	//gp_Trsf delTrsf = pCom->GetDeltaTransform();
	gp_Ax3 ax3 = MakeAx3(vDisp);
	ax3.Transform(originTrf/**delTrsf*/);
	//gp_Dir zDir = ax3.Direction();
	gp_Ax1 ax1 = ax3.Axis();

	static int i = 0;
	gp_Trsf transf;
	transf.SetRotation(ax1, 30 * PI *i++ / 180);
	//pCom->SetDeltaTransform(transf);

	//������ת���
	vector<double> v15(15);
	v15[0] = ax3.Location().X(); v15[1] = ax3.Location().Y(); v15[2] = ax3.Location().Z();
	v15[3] = ax3.Direction().X(); v15[4] = ax3.Direction().Y(); v15[5] = ax3.Direction().Z();
	v15[6] = ax3.XDirection().X(); v15[7] = ax3.XDirection().Y(); v15[8] = ax3.XDirection().Z();
	v15[9] = 0; v15[10] = 0; v15[11] = 0;
	v15[12] = 0; v15[13] = 0; v15[14] = 30 * PI * i++ / 180;
	pCom->SetDeltaTransform(v15);
	//����
	//gp_Trsf trsf2 = pCom->GetDeltaTransform();
	UpdateDisplay1();
	return;
	
	transf = MakeTransFromV15(v15);
	
	BRepBuilderAPI_Transform myBRepTransformation(theSelShape, transf);
    TopoDS_Shape TransformedShape = myBRepTransformation.Shape();
	Handle(AIS_InteractiveObject) theNewAIS = new AIS_Shape(TransformedShape);
	myContext->Display(theNewAIS, true);
	myContext->Erase(theAIS, true);
	return;

	vector<Handle(AIS_InteractiveObject)> vAIS = pCom->GetAIS();
	for (int i = 0; i < vAIS.size(); i++)
	{
		//myContext->SetColor(theAIS, Quantity_NOC_RED, false);
		myContext->SetLocation(vAIS[i], transf);
		myContext->Display(vAIS[i], true);
	}
}

//����
void DocumentModel::adjustComponent()
{
	int cnt = myContext->NbSelected();
	if (myContext->NbSelected() < 1)
	{
		return;
	}
	Handle(AIS_InteractiveObject) aisObj;
	for (myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
	{
		aisObj = myContext->SelectedInteractive();
		if (aisObj->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			//TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
			break;
		}
	}

	//����ѡ�е����shape
	Handle(AIS_myManipulator) aManipulator = new AIS_myManipulator();
	//����ƽ��
	//aManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Translation, Standard_False);
	//aManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Translation, Standard_False);
	//aManipulator->SetPart(2, AIS_ManipulatorMode::AIS_MM_Translation, Standard_False);
	//����scaling
	aManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);
	aManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);
	aManipulator->SetPart(2, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);

	//������ת
	//aManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Rotation, Standard_False);
	//aManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Rotation, Standard_False);
	aManipulator->Attach(aisObj);

	//����
	aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Translation);
	aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Rotation);
	//aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Scaling);

	//����
	aManipulator->SetModeActivationOnDetection(Standard_True);
	myContext->UpdateCurrentViewer();
}

DocumentModel* DocumentModel::ReadModelFile(const char* csFile)
{
	DocumentModel* pMdl = nullptr;
	QString fileName = csFile;

	Handle(TDocStd_Application) anOcaf_Application = new TDocStd_Application;
	// load persistence
	BinDrivers::DefineFormat(anOcaf_Application);
	// Look for already opened
	if (anOcaf_Application->IsInSession(fileName.toStdU16String().data())) //toUtf8
	{
		return pMdl;
	}
	// Open the document in the current application
	Handle(TDocStd_Document) anM3dOcafDoc;
	PCDM_ReaderStatus aReaderStatus = anOcaf_Application->Open(fileName.toStdU16String().data(), anM3dOcafDoc);
	if (aReaderStatus == PCDM_RS_OK)
	{
		pMdl = new DocumentModel();
		pMdl->SetFileName(csFile);
		pMdl->SetRoot(anM3dOcafDoc->Main());
	}

	return pMdl;
}

///
////���ĵ������ݣ�����һ���������Component���󣬲��ݹ�
//Component* DocumentModel::GenerateTopComponent()
//{
//    Component* pComRet = new Component(this);
//
//    //����ĸ�����������ļ������������λ��
//    //�����Ϣ���ӽڵ��У��˴�������...
//    m_mapCompInfo = GetAllCompInfos(); //���ĵ�
//    //pComRet->SetComponentsInfo(m_mapCompInfo);
//
//    ////����Ľӿ�Լ��
//    //m_lstConnectConstraints = GetAllConnConstraints();
//    ////pComRet->SetCompConnConstraints(m_lstConnectConstraints);
//
//    ////�����������
//    //m_lstConnectCurves = GetAllConnectCurves();
//    ////pComRet->SetCompConnections(m_lstConnectCurves);
//
//    ////��������
//    //m_lstParamValues = GetAllParameterValues();
//    ////pComRet->SetParamterValues(m_lstParamValues);
//
//    return pComRet;
//}

//����������ĵ�
//λ����Ϣȱʡ��ԭλ��
//�˴�ֻ�洢����ģ����֡��ļ�ȫ����λ��
bool DocumentModel::InsertComponent(Component* pCom)
{
	SetModified();

	//����Ľڵ����Ϊ 1
	TDF_Label compLabel = m_pDocRoot.FindChild(1);
	//����ĸ���������ע�⣺���ܰ�����ɾ���ģ�Ҳ������Undo��Ч��
	//�ж�һ���ڵ��Ƿ���Ч���õ�һ�������Ƿ��óɹ����ж�
	int nCount = compLabel.NbChildren();

	//�½�һ��Label
	TDF_Label theLabel = compLabel.FindChild(nCount + 1);

	//д����������
	TDataStd_Name::Set(theLabel, pCom->GetCompName().c_str());

	//д���ļ�·��
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(theLabel, 1, 1);
	aStrArray->SetValue(1, pCom->GetCompFile().toStdU16String().c_str());

	//д��λ��
	Handle(TDataStd_RealArray) aRealArray = TDataStd_RealArray::Set(theLabel, 1, 12);
	gp_Trsf trsnf;// = pCom->GetOriginTransform(); //����������ȱʡ�ڵ�λ���󣬲��任;
	vector<double> v12 = GetTransformValues12(trsnf);
	for (int i = 1; i <= 12; i++)
	{
		aRealArray->SetValue(i, v12[i - 1]);
	}

	////��¼һ��
	//CompBaseInfo ci;
	//ci.sCompName = pCom->GetCompName();
	//ci.sCompFile = pCom->GetCompFile().toStdString();
	//ci.vCompDisplacement = pCom->GetDisplacement();
	//m_mapCompInfo.insert(make_pair(ci.sCompName, ci));

	return true;
}

//���ĵ�ɾ�����
void DocumentModel::DeleteComponent(Component* pCom)
{
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);

	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompName = ExtString2string(s);

		if (sCompName == pCom->GetCompName())
		{
			//ֻ��������û��ɾ���ڵ�Ľӿں���
			theLabel.ForgetAttribute(TDataStd_Name::GetID());
		}
	}

	////ɾ�������Ϣ����֤��Ϣһ����
	//m_mapCompInfo.erase(pCom->GetCompName().c_str());

	//����ɾ���������ص�������
	DeleteCompConnectCurves(pCom->GetCompName().c_str());

	//ɾ��װ��Լ��
	DeleteCompConnConstraits(pCom->GetCompName().c_str());

	//ɾ����������Ϣ
	DeleteCompParameterValues(pCom->GetCompName().c_str());
}

//ɾ�����������������
void DocumentModel::DeleteCompConnectCurves(const char* sCompName)
{
	vector<ConnectCurve> vCCs = GetAllConnectCurves();
	for (int i = 0; i < vCCs.size(); i++)
	{
		ConnectCurve ci = vCCs[i];
		if (ci.component1 == sCompName || ci.component2 == sCompName)
		{
			DeleteConnectCurve(ci);
		}
	}
}

//ɾ�������������װ��Լ��
void DocumentModel::DeleteCompConnConstraits(const char* sCompName)
{
	vector<ConnectConstraint> vCCs = GetAllConnConstraints();
	for (int i = 0; i < vCCs.size(); i++)
	{
		ConnectConstraint ci = vCCs[i];
		if (ci.component1 == sCompName || ci.component2 == sCompName)
		{
			DeleteConnConstraint(ci);
		}
	}
}

//ɾ���������صĲ���
void DocumentModel::DeleteCompParameterValues(const char* sCompName)
{
	vector<ParameterValue> vPVs = GetAllParameterValues();
	for (int i = 0; i < vPVs.size(); i++)
	{
		ParameterValue p_i = vPVs[i];
		string sCompDot = sCompName;
		sCompDot += ".";
		if (p_i.sMapParamName.find(sCompDot) != string::npos)
		{
			DeleteParameterValue(p_i.sMapParamName);
		}
	}
}

//
//ֻ��ȡ��һ��������Ϣ
//�������<�����������ļ���������ı任����>
vector<CompBaseInfo> DocumentModel::GetAllCompInfos() const
{
	vector<CompBaseInfo> mapRet;
	//����Ľڵ����Ϊ 1
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	if (compLabel.IsNull())
	{
		return mapRet;
	}

	//����ĸ���������ע�⣺���ܰ�����ɾ���ģ�Ҳ������Undo��Ч��
	//�ж�һ���ڵ��Ƿ���Ч���õ�һ�������Ƿ��óɹ����ж�
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label lbl_i = compLabel.FindChild(i, false);

		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompName = ExtString2string(s);

		Handle(TDataStd_ExtStringArray) aStrArray;
		lbl_i.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
		TCollection_ExtendedString s1 = aStrArray->Value(1);
		string sCompFileName = ExtString2string(s1);

		Handle(TDataStd_RealArray) clr;
		lbl_i.FindAttribute(TDataStd_RealArray::GetID(), clr);
		vector<double> vDisp18;
		for (int i = 1; i <= 18; i++)
		{
			vDisp18.push_back(clr->Value(i));
		}

		//��һ��
		CompBaseInfo ci;
		ci.sCompName = sCompName;
		ci.sCompFile = sCompFileName;
		ci.v18Transf = vDisp18;
		mapRet.push_back(ci);
	}

	return mapRet;
}

//װ���»��ʵ������
//ֻ��ע����һ���㲿�������֣���һ���㲿���Ľӿ�ȫ��
string DocumentModel::GetShapeName(const TopoDS_Shape& shp) const
{
	string retStr;

	if (m_pCompRoot)
	{
		list<Component*> comChilds = m_pCompRoot->GetChilds();
		list<Component*>::iterator itr = comChilds.begin();
		bool bFound = false;
		for (; itr != comChilds.end(); ++itr)
		{
			Component* pComChild = *itr;
			if (shp.IsPartner(pComChild->GetCompound()))
			{
				retStr = pComChild->GetCompName();
				bFound = true;
				break;
			}
		}
		if (!bFound)
		{//�ҽӿ�
			for (itr = comChilds.begin(); itr != comChilds.end(); ++itr)
			{
				Component* pComChild = *itr;
				//�������װ���壬���ȡ��ӿ�����������ӿ�
				if (pComChild->IsAssembly())
				{
					list<Component*> lstSubComps = pComChild->GetChilds();
					auto itrSub = lstSubComps.begin();
					for (; itrSub != lstSubComps.end(); ++itrSub)
					{
						Component* pSubChild = *itrSub;
						if (!pSubChild->IsAssembly() && pSubChild->IsConnector())
						{
							//��ýӿ�������Ľӿ�
							Connector* pConn = pSubChild->GetConnector(shp);
							if (pConn)
							{
								retStr = pComChild->GetCompName();
								retStr += ".";
								retStr += pSubChild->GetCompName();// GetName();
								bFound = true;
								break;
							}
						}
					}
				}
				else {
					Connector* pConn = pComChild->GetConnector(shp);
					if (pConn)
					{
						retStr = pComChild->GetCompName();
						retStr += ".";
						retStr += pConn->GetConnName();// GetName();
						bFound = true;
						break;
					}
				}
			}
		}
		//
		if (!bFound)
		{
			//������������,�˴����ĵ���õģ��е��Ť��������
			ConnectCurve* pCurve = GetConnectCurve(shp);
			if (pCurve)
			{
				retStr = "connect(";
				string s1 = pCurve->component1 + "." + pCurve->connector1;
				retStr += s1;
				retStr += ",";
				string s2 = pCurve->component2 + "." + pCurve->connector2;
				retStr += s2;
				retStr += ")";
				bFound = true;
			}
		}
	}

	return retStr;
}

//λ��
void DocumentModel::SetCompOriginTransform(const char* sCompName, gp_Trsf transf)
{
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	//����ĸ���������ע�⣺���ܰ�����ɾ���ģ�Ҳ������Undo��Ч��
	//�ж�һ���ڵ��Ƿ���Ч���õ�һ�������Ƿ��óɹ����ж�
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//д����������
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);

		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompNameI = ExtString2string(s);

		if (sCompName == sCompNameI)
		{
			//λ��
			//theLabel.ForgetAttribute(TDataStd_RealArray::GetID());
			vector<double> v12 = GetTransformValues12(transf);
			Handle(TDataStd_RealArray) aRealArray = TDataStd_RealArray::Set(theLabel, 1, 18);
			for (int i = 1; i <= 12; i++)
			{
				aRealArray->SetValue(i, v12[i - 1]);
			}
		}
	}
}

void DocumentModel::SetCompDeltaTransform(const char* sCompName, vector<double>& v15 /*gp_Trsf transf*/)
{
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	if (compLabel.IsNull())
	{
		return;
	}

	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//д����������
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);

		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompNameI = ExtString2string(s);

		if (sCompName == sCompNameI)
		{
			TDF_Label delLable = theLabel.FindChild(1);

			Handle(TDataStd_RealArray) aRealArray = TDataStd_RealArray::Set(delLable, 1, 15);
			for (int i = 1; i <= 15; i++)
			{
				aRealArray->SetValue(i, v15[i - 1]);
			}
		}
	}
}

//��8�������洢
void DocumentModel::SetComponentAttrib(const char* sComName, ComponentAttrib attrib)
{
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	if (compLabel.IsNull())
	{
		return;
	}

	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//д����������
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);

		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompNameI = ExtString2string(s);

		if (sComName == sCompNameI)
		{
			TDF_Label delLable = theLabel.FindChild(2); //1-i-2

			Handle(TDataStd_IntegerArray) aRealArray = TDataStd_IntegerArray::Set(delLable, 1, 8);
			aRealArray->SetValue(1, attrib.bFinal);
			aRealArray->SetValue(2, attrib.bInner);
			aRealArray->SetValue(3, attrib.bOuter);
			aRealArray->SetValue(4, attrib.bProtected);
			aRealArray->SetValue(5, attrib.bReplaceable);
			aRealArray->SetValue(6, attrib.iCasuality);
			aRealArray->SetValue(7, attrib.iConnectorMember);
			aRealArray->SetValue(8, attrib.iVariablity);
		}
	}
}

ComponentAttrib DocumentModel::GetComponentAttrib(const char* sComName) const
{
	ComponentAttrib attrib;
	vector<int> v8;

	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//д����������
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);

		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompNameI = ExtString2string(s);

		if (sComName == sCompNameI)
		{
			TDF_Label delLable = theLabel.FindChild(2, false);

			if (delLable.IsNull())
			{
				//return v8;
				return attrib;
			}
			//���λ��
			Handle(TDataStd_IntegerArray) aIntArray;
			delLable.FindAttribute(TDataStd_IntegerArray::GetID(), aIntArray);

			attrib.bFinal = aIntArray->Value(1);
			attrib.bInner = aIntArray->Value(2);
			attrib.bOuter = aIntArray->Value(3);
			attrib.bProtected = aIntArray->Value(4);
			attrib.bReplaceable = aIntArray->Value(5);
			attrib.iCasuality = aIntArray->Value(6);
			attrib.iConnectorMember = aIntArray->Value(7);
			attrib.iVariablity = aIntArray->Value(8);

			return attrib;
		}
	}

	return attrib;
}

gp_Trsf DocumentModel::GetCompOriginTransform(const char* sCompName) const
{
	gp_Trsf retTrans;

	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	//����ĸ���������ע�⣺���ܰ�����ɾ���ģ�Ҳ������Undo��Ч��
	//�ж�һ���ڵ��Ƿ���Ч���õ�һ�������Ƿ��óɹ����ж�
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//д����������
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);

		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompNameI = ExtString2string(s);

		if (sCompName == sCompNameI)
		{
			//λ��
			Handle(TDataStd_RealArray) aRealArray;
			theLabel.FindAttribute(TDataStd_RealArray::GetID(), aRealArray);
			vector<double> v12;
			for (int i = 1; i <= 12; i++)
			{
				v12.push_back(aRealArray->Value(i));
			}
			retTrans = MakeTransFromV12(v12);
		}
	}

	return retTrans;
}

 gp_Trsf DocumentModel::GetCompDeltaTransform(const char* sCompName) const
{
	 gp_Trsf retTrans;

	 vector<double> v15 = GetCompDeltaTransformV15(sCompName);
	 if (v15.size() < 15)
	 {
		 return retTrans;
	 }

	 retTrans = MakeTransFromV15(v15);

	 return retTrans;
}

 vector<double> DocumentModel::GetCompDeltaTransformV15(const char* sCompName) const
{
	vector<double> v15;

	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	//����ĸ���������ע�⣺���ܰ�����ɾ���ģ�Ҳ������Undo��Ч��
	//�ж�һ���ڵ��Ƿ���Ч���õ�һ�������Ƿ��óɹ����ж�
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//д����������
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);

		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompNameI = ExtString2string(s);

		if (sCompName == sCompNameI)
		{
			TDF_Label delLable = theLabel.FindChild(1, false);

			if (delLable.IsNull())
			{
				return v15;
			}
			//���λ��
			Handle(TDataStd_RealArray) aRealArray;
			delLable.FindAttribute(TDataStd_RealArray::GetID(), aRealArray);
			
			for (int i = 1; i <= 15; i++)
			{
				v15.push_back(aRealArray->Value(i));
			}
			break;
		}
	}

	return v15;
}

//void DocumentModel::SetComponentDisplacement(Component* pCom, vector<double>& v18)
//{
//    TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
//    //����ĸ���������ע�⣺���ܰ�����ɾ���ģ�Ҳ������Undo��Ч��
//    //�ж�һ���ڵ��Ƿ���Ч���õ�һ�������Ƿ��óɹ����ж�
//    int nCount = compLabel.NbChildren();
//
//    for (int i = 1; i <= nCount; i++)
//    {
//        TDF_Label theLabel = compLabel.FindChild(i, false);
//        //д����������
//        Handle(TDataStd_Name) nameAttr;
//        theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);
//
//        if (nameAttr.IsNull())
//        {
//            continue;
//        }
//        TCollection_ExtendedString s = nameAttr->Get();
//        string sCompName = ExtString2string(s);
//
//        if (sCompName == pCom->GetCompName())
//        {
//            //λ��
//            //theLabel.ForgetAttribute(TDataStd_RealArray::GetID());
//            Handle(TDataStd_RealArray) aRealArray = TDataStd_RealArray::Set(theLabel, 1, 18);
//            for (int i = 1; i <= 18; i++)
//            {
//                aRealArray->SetValue(i, v18[i - 1]);
//            }
//        }
//    }
//}

///
///�ӿ�Լ��
/// 
//ɾ��һ�ӿ�Լ��
void DocumentModel::DeleteConnConstraint(ConnectConstraint& cc)
{
	//װ��Լ���Ľڵ����Ϊ 2
	TDF_Label connLabel = m_pDocRoot.FindChild(2, false);
	if (connLabel.IsNull())
	{
		return;
	}

	Standard_Integer ncount = connLabel.NbChildren();
	for (int i = 1; i <= ncount; i++)
	{
		TDF_Label lbl_i = connLabel.FindChild(i, false);

		//����
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		//�ϳ�һ������ A1.p1-A2.p1
		string sNameOne = Make1Name(cc.component1, cc.component2, cc.connector1, cc.connector2);
		if (sCurveName == sNameOne)
		{
			lbl_i.ForgetAllAttributes(true);
			break;
		}
	}
}

//���ӽӿ�Լ��
void DocumentModel::AddConnConstraint(ConnectConstraint& cc)
{
	//�ӿ�Լ���Ľڵ����Ϊ 2
	TDF_Label connLabel = m_pDocRoot.FindChild(2);
	int nCount = connLabel.NbChildren();

	//�½�һ��Label
	TDF_Label theLabel;
	int i = 1;
	for (i = 1; i <= nCount; i++)
	{
		ConnectConstraint cc;
		TDF_Label lbl_i = connLabel.FindChild(i, false);
		if (lbl_i.IsNull())
		{
			theLabel = connLabel.FindChild(i);
			break;
		}
		//����
		Handle(TDataStd_Integer) iType;
		lbl_i.FindAttribute(TDataStd_Integer::GetID(), iType);
		if (iType.IsNull())
		{
			theLabel = connLabel.FindChild(i);
			break;
		}
	}
	if(i>nCount)
		theLabel = connLabel.FindChild(nCount + 1);

	//����
	TDataStd_Integer::Set(theLabel, cc.kpType);

	//д���������� A1.p1-A2.p2
	string ccName = Make1Name(cc.component1, cc.component2, cc.connector1, cc.connector2);
	TDataStd_Name::Set(theLabel, ccName.c_str());

	//nCount = connLabel.NbChildren();
}

//����ȫ���ӿ�Լ�����ڽӿ�Լ�����桰���¡�ʱ����
void DocumentModel::SetConnConstraints(vector<ConnectConstraint>& vConnConstrs)
{
	//�ӿ�Լ���Ľڵ����Ϊ 2
	TDF_Label connLabel = m_pDocRoot.FindChild(2);
	//������ӽڵ������
	connLabel.ForgetAllAttributes(true);

	for (int i = 0; i < vConnConstrs.size(); i++)
	{
		AddConnConstraint(vConnConstrs[i]);
	}
}

//���ȫ���Ľӿ�Լ��
vector<ConnectConstraint> DocumentModel::GetAllConnConstraints() const
{
	vector<ConnectConstraint> vRet;

	TDF_Label connLabel = m_pDocRoot.FindChild(2, false);
	if (connLabel.IsNull())
	{
		return vRet;
	}

	Standard_Integer ncount = connLabel.NbChildren();
	set<string> setConnConsts;
	for (int i = 1; i <= ncount; i++)
	{
		ConnectConstraint cc;
		TDF_Label lbl_i = connLabel.FindChild(i, false);
		if (lbl_i.IsNull())
		{
			continue;
		}
		//����
		Handle(TDataStd_Integer) iType;
		lbl_i.FindAttribute(TDataStd_Integer::GetID(), iType);
		if (iType.IsNull())
		{
			continue;
		}
		cc.kpType = (Kinematic_pair_type)iType->Get();
		//quanming
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sFullName = ExtString2string(s);
		if (setConnConsts.find(sFullName) == setConnConsts.end())
		{//�����ظ�;
			setConnConsts.insert(sFullName);
			string c1, c2, conn1, conn2;
			Make4Names(sFullName, c1, c2, conn1, conn2);
			cc.component1 = c1;
			cc.component2 = c2;
			cc.connector1 = conn1;
			cc.connector2 = conn2;

			vRet.push_back(cc);
		}
	}
	return vRet;
}

//�õ��������ӵ����ȫ��
string DocumentModel::GetLinkRigid(const char* sComName) const
{
	vector<ConnectConstraint> vCC = GetAllConnConstraints();
	for (int i = 0; i < vCC.size(); i++)
	{
		ConnectConstraint cc = vCC[i];
		if (cc.component1 == sComName)
		{
			return cc.component2;
		}

		if (cc.component2 == sComName)
		{
			return cc.component1;
		}
	}
	return "";
}
///
///������
/// 
/// ����������
void DocumentModel::AddConnectCurve(ConnectCurve& pCurve) //����һ��������
{
	//lianjie�Ľڵ����Ϊ 3
	TDF_Label connLabel = m_pDocRoot.FindChild(3);
	int nCount = connLabel.NbChildren();

	TDF_Label theLabel;
	int i;
	for ( i = 1; i <= nCount; i++)
	{
		theLabel = connLabel.FindChild(i, false);
		if (theLabel.IsNull())
		{
			theLabel = connLabel.FindChild(i, true);
			break;
		}
		else {
			Handle(TDataStd_Name) nameAttr;
			theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);
			if (nameAttr.IsNull())
			{
				theLabel = connLabel.FindChild(i, true);
				break;
			}
		}
	}

	if (i > nCount)
	{
		//�½�һ��Label
		theLabel = connLabel.FindChild(nCount + 1);
	}

	//����
	TDataStd_Integer::Set(theLabel, pCurve.ccType);

	//д���������� A1.p1-A2.p2
	string ccName = Make1Name(pCurve.component1, pCurve.component2, pCurve.connector1, pCurve.connector2);
	TDataStd_Name::Set(theLabel, ccName.c_str());

	//д����
	TNaming_Builder B_i(theLabel);
	TopoDS_Shape connShape = pCurve.shape;
	B_i.Generated(connShape);

	//д���ϵ��
	int nNum = 1 + pCurve.lstPoints.size() * 3;
	Handle(TDataStd_RealArray) aRealArray = TDataStd_RealArray::Set(theLabel, 1, nNum);
	aRealArray->SetValue(1, pCurve.lstPoints.size());
	auto itr = pCurve.lstPoints.begin();
	i = 2;
	for (; itr != pCurve.lstPoints.end(); ++itr)
	{
		gp_Pnt pt3d = *itr;
		aRealArray->SetValue(i++, pt3d.X());
		aRealArray->SetValue(i++, pt3d.Y());
		aRealArray->SetValue(i++, pt3d.Z());
	}
}

//ʰȡһ��������
ConnectCurve* DocumentModel::GetConnectCurve(const TopoDS_Shape& theShape) const
{
	ConnectCurve* ccRet = nullptr;

	//lianjie�Ľڵ����Ϊ 3
	TDF_Label connLabel = m_pDocRoot.FindChild(3, false);
	if (connLabel.IsNull())
	{
		return ccRet;
	}

	Standard_Integer ncount = connLabel.NbChildren();
	for (int i = 1; i <= ncount; i++)
	{
		TDF_Label lbl_i = connLabel.FindChild(i, false);
		if (lbl_i.IsNull())
		{
			continue;
		}

		//����
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		//����
		Handle(TNaming_NamedShape) aNamedShape1;
		lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		if (aNamedShape1.IsNull())
		{
			continue;
		}
		TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

		if (!shapej.IsNull() && shapej.IsSame(theShape))
		{
			ccRet = new ConnectCurve();
			//����
			Handle(TDataStd_Integer) iType;
			lbl_i.FindAttribute(TDataStd_Integer::GetID(), iType);
			ccRet->ccType = (ConnectCurveType)iType->Get();

			//��������
			string sCom1, sCom2, sConn1, sConn2;
			Make4Names(sCurveName, sCom1, sCom2, sConn1, sConn2);
			ccRet->component1 = sCom1;
			ccRet->component2 = sCom2;
			ccRet->connector1 = sConn1;
			ccRet->connector2 = sConn2;
			//��
			Handle(TDataStd_RealArray) clr;
			lbl_i.FindAttribute(TDataStd_RealArray::GetID(), clr);
			int nNum = clr->Value(1);
			for (int i = 0; i < nNum; i++)
			{
				gp_Pnt pTi(clr->Value(i * 3 + 1), clr->Value(i * 3 + 2), clr->Value(i * 3 + 3));
				ccRet->lstPoints.push_back(pTi);
			}

			ccRet->shape = shapej;
			break;
		}
	}

	return ccRet;
}

//ɾ��һ��������
void DocumentModel::DeleteConnectCurve(ConnectCurve& cc)
{
	//lianjie�Ľڵ����Ϊ 3
	TDF_Label connLabel = m_pDocRoot.FindChild(3, false);
	if (connLabel.IsNull())
	{
		return;
	}

	Standard_Integer ncount = connLabel.NbChildren();
	for (int i = 1; i <= ncount; i++)
	{
		TDF_Label lbl_i = connLabel.FindChild(i, false);

		//����
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		//�ϳ�һ������ A1.p1-A2.p1
		string sNameOne = Make1Name(cc.component1, cc.component2, cc.connector1, cc.connector2);
		if (sCurveName == sNameOne)
		{
			lbl_i.ForgetAllAttributes(true);
			break;
		}
	}
}

//���ȫ���Ľӿ�Լ��
vector<ConnectCurve> DocumentModel::GetAllConnectCurves() const
{
	vector<ConnectCurve> vRet;

	//lianjie�Ľڵ����Ϊ 3
	TDF_Label connLabel = m_pDocRoot.FindChild(3, false);
	if (connLabel.IsNull())
	{
		return vRet;
	}

	Standard_Integer ncount = connLabel.NbChildren();
	for (int i = 1; i <= ncount; i++)
	{
		TDF_Label lbl_i = connLabel.FindChild(i, false);

		if (lbl_i.IsNull())
		{
			continue;
		}

		//����
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		//����
		Handle(TNaming_NamedShape) aNamedShape1;
		lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		if (aNamedShape1.IsNull())
		{
			continue;
		}
		TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

		if (!shapej.IsNull())
		{
			ConnectCurve cc;
			//����
			Handle(TDataStd_Integer) iType;
			lbl_i.FindAttribute(TDataStd_Integer::GetID(), iType);
			cc.ccType = (ConnectCurveType)iType->Get();

			//��������
			string sCom1, sCom2, sConn1, sConn2;
			Make4Names(sCurveName, sCom1, sCom2, sConn1, sConn2);
			cc.component1 = sCom1;
			cc.component2 = sCom2;
			cc.connector1 = sConn1;
			cc.connector2 = sConn2;
			//��
			Handle(TDataStd_RealArray) clr;
			lbl_i.FindAttribute(TDataStd_RealArray::GetID(), clr);
			int nNum = clr->Value(1);
			for (int i = 0; i < nNum; i++)
			{
				gp_Pnt pTi(clr->Value(i * 3 + 1), clr->Value(i * 3 + 2), clr->Value(i * 3 + 3));
				cc.lstPoints.push_back(pTi);
			}

			cc.shape = shapej;
			vRet.push_back(cc);
		}
	}

	return vRet;
}

///
///�������ã�ӳ��������ǹؼ���
///
/// ����һ������
void DocumentModel::AddParameterValue(ParameterValue& pv, int iLabel)
{
	//�����Ľڵ����Ϊ 4
	TDF_Label connLabel = m_pDocRoot.FindChild(4);
	int nCount = connLabel.NbChildren();

	//�½�һ��Label
	int iLab = iLabel;
	if (iLab == 0)
	{
		iLab = nCount + 1;
	}
	TDF_Label theLabel = connLabel.FindChild(iLab);

	//����ӳ������Ĳ�����
	TDataStd_Name::Set(theLabel, pv.sMapParamName.c_str());

	//������ �� ����ֵ
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(theLabel, 1, 2);
	aStrArray->SetValue(1, pv.sParamName.c_str());
	aStrArray->SetValue(2, pv.sParamValue.c_str());
	//nCount = connLabel.NbChildren();
}

//ɾ��һ������
void DocumentModel::DeleteParameterValue(string sMapParamName)
{
	TDF_Label parLabel = m_pDocRoot.FindChild(4, false);
	if (parLabel.IsNull())
	{
		return;
	}

	Standard_Integer ncount = parLabel.NbChildren();
	for (int i = 1; i <= ncount; i++)
	{
		TDF_Label lbl_i = parLabel.FindChild(i, false);

		//����
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		if (sCurveName == sMapParamName)
		{
			lbl_i.ForgetAllAttributes(true);
			break;
		}
	}
}

//����ȫ������
void DocumentModel::SetParameterValues(vector<ParameterValue>& vPVs)
{
	auto itr = vPVs.begin();

	//�����Ľڵ����Ϊ 4
	TDF_Label parLabel = m_pDocRoot.FindChild(4,false);
	if (!parLabel.IsNull())
	{//�����ȫ������ֵ����
		parLabel.ForgetAllAttributes();
		parLabel.Nullify();
	}

	int i = 1;
	for (; itr != vPVs.end(); ++itr)
	{
		ParameterValue pv = *itr;
		AddParameterValue(pv,i++);
	}
}

//���ݲ���������ò������еĲ���ֵ��Ϣ
bool DocumentModel::GetParameterValue(const char* sParName, ParameterValue& pv0) const
{
	vector<ParameterValue> vPVs = GetAllParameterValues();

	auto itr = vPVs.begin();
	for (; itr != vPVs.end(); ++itr)
	{
		ParameterValue pv = *itr;
		if (pv.sParamName == sParName)
		{
			pv0 = pv;
			return true;
		}
	}

	return false;
}

//���ȫ������
vector<ParameterValue> DocumentModel::GetAllParameterValues() const
{
	vector<ParameterValue> vRet;

	TDF_Label parLabel = m_pDocRoot.FindChild(4, false);
	if (parLabel.IsNull())
	{
		return vRet;
	}

	Standard_Integer ncount = parLabel.NbChildren();
	for (int i = 1; i <= ncount; i++)
	{
		TDF_Label lbl_i = parLabel.FindChild(i, false);
		if (lbl_i.IsNull())
		{
			continue;
		}
		//ӳ������
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sMapName = ExtString2string(s);

		//��������ֵ
		Handle(TDataStd_ExtStringArray) aStrArray;
		lbl_i.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
		TCollection_ExtendedString s1 = aStrArray->Value(1);
		TCollection_ExtendedString s2 = aStrArray->Value(2);
		string sParName = ExtString2string(s1);
		string sParValue = ExtString2string(s2);

		ParameterValue pvi;
		pvi.sMapParamName = sMapName;
		pvi.sParamName = sParName;
		pvi.sParamValue = sParValue;

		vRet.push_back(pvi);
	}

	return vRet;
}


bool DocumentModel::HasWorldComponent() const
{
	auto map1 = GetAllCompInfos();
	auto itr = map1.begin();
	for (; itr != map1.end(); ++itr)
	{
		CompBaseInfo cbi = *itr;
		if (cbi.sCompName == "world")
		{
			return true;
		}
	}

	return false;
}

vector<Connector*> DocumentModel::GetConnectors() const
{
	//���װ����Ľӿ�
	vector<Connector*> vConnectors;

	//��õ�ǰ����
	list<Component*> lstChilds = m_pCompRoot->GetChilds();
	auto itr = lstChilds.begin();
	for (; itr != lstChilds.end(); ++itr)
	{
		Component* pComp = *itr;

		if (pComp->IsConnector())
		{
			//�õ��ӿ���������ӿ�
			Connector* pConn = pComp->GetMainConnector();
			vConnectors.push_back(pConn);
		}
	}

	return vConnectors;
}

gp_Ax3 DocumentModel::GetCompConnLocalAx3(const char* sComName, const char* sConnName)
{
	gp_Ax3 lAx3;

	Component* pCom = m_pCompRoot->GetChild(sComName);
	
	//�õ����Frame_a�ӿ�Ax3
	DocumentCommon* pDoc = (DocumentCommon*)pCom->GetDocument();
	if (pDoc->IsModel())
	{
		assert(false);
		return lAx3;
	}
	DocumentComponent* pComDoc = (DocumentComponent*)pDoc;
	Connector* pConn = pComDoc->GetConnector(sConnName);
	if (!pConn)
	{
		return lAx3;
	}

	//Ax3��ȫ������ϵ��
	vector<double> v9 = pConn->GetDisplacement();
	lAx3 = MakeAx3(v9);

	return lAx3;
}
gp_Ax3 DocumentModel::GetCompConnGlobalAx3(const char* sComName, const char* sConnName)
{
	gp_Ax3 gAx3;

	Component* pCom = m_pCompRoot->GetChild(sComName);
	gp_Trsf orTrsf = pCom->GetOriginTransform();
	gp_Trsf delTrsf = pCom->GetDeltaTransform();

	//�õ����Frame_a�ӿ�Ax3
	DocumentCommon* pDoc = (DocumentCommon*)pCom->GetDocument();
	if (pDoc->IsModel())
	{
		assert(false);
		return gAx3;
	}
	DocumentComponent* pComDoc = (DocumentComponent*)pDoc;
	Connector* pConn = pComDoc->GetConnector(sConnName);
	if (!pConn)
	{
		return gAx3;
	}

	//Ax3��ȫ������ϵ��
	vector<double> v9 = pConn->GetDisplacement();
	gAx3 = MakeAx3(v9);
	gAx3.Transform(delTrsf * orTrsf);
	delete pConn;

	return gAx3;
}

gp_Ax3 DocumentModel::GetCompConnGlobalAx3(const char* sComName, int nth)
{
	gp_Ax3 gAx3;

	Component* pCom = m_pCompRoot->GetChild(sComName);
	gp_Trsf orTrsf = pCom->GetOriginTransform();
	gp_Trsf delTrsf = pCom->GetDeltaTransform();

	//�õ����Frame_a�ӿ�Ax3
	DocumentCommon* pDoc = (DocumentCommon*)pCom->GetDocument();
	if (pDoc->IsModel())
	{
		assert(false);
		return gAx3;
	}
	DocumentComponent* pComDoc = (DocumentComponent*)pDoc;
	vector<Connector*> vConn = pComDoc->GetAllConnectors();
	Connector* pConn = nullptr;
	for (int k=0; k<vConn.size(); k++)
	{
		if (k == nth)
		{
			pConn = vConn[k];
		}
		else {
			delete vConn[k];
		}
	}

	if (!pConn)
	{
		return gAx3;
	}

	//Ax3��ȫ������ϵ��
	vector<double> v9 = pConn->GetDisplacement();
	gAx3 = MakeAx3(v9);
	gAx3.Transform(delTrsf * orTrsf);
	delete pConn;

	return gAx3;
}

//�Զ��������ֵ
//vector<string> vNames = { "r","r_CM","r_ab","r_ba","n","nX","n_X","nY","n_Y","nZ","n_Z" };
QString DocumentModel::ReComputeParamValue(const QString& sComName, const QString& sComParName)
{
	QString theValue;

	//�ȵõ��������
	QString sMapParName = sComName + "." + sComParName;
	myParameter* theParam = GetParameterInfo(sMapParName.toStdString());
	assert(theParam);
	//�õ�����ı任����
	gp_Ax3 gAx3 = GetCompConnGlobalAx3(sComName.toStdString().c_str(), "frame_a");

	//������������ֵ�任
	gp_Pnt p0;
	QString sValue = theParam->defaultValue.c_str();
	sValue.replace("{", "(");
	sValue.replace("}", ")");
	if (!GetPoint(sValue, p0))
	{
		assert(false);
		theValue = theParam->defaultValue.c_str();
		return theValue;
	}

	gp_Trsf trsf;
	gp_Ax3 Ax30;
	trsf.SetDisplacement(Ax30, gAx3);
	gp_Pnt Porigin(0,0,0);
	Porigin.Transform(trsf);
	p0.Transform(trsf);
	gp_Vec v0(Porigin, p0);
	theValue = GetVectorString(v0);
	theValue.replace("(", "{");
	theValue.replace(")", "}");

	return theValue;
}

//ģ�ͼ��
bool DocumentModel::InitCheckModel()
{
	vector<ConnectCurve> vCCs = GetAllConnectCurves();//������
	vector<ConnectConstraint> vCCt = GetAllConnConstraints();//�ӿ�Լ��
	set<string> sConnected;//�����ӽӿ����Ƶļ���
	set<string> sConnectors;//���нӿڵļ���
	//1.��������ߡ��ӿ�Լ�����ӵ����нӿ����Ƽ���sConnected
	for (vector<ConnectCurve>::iterator it = vCCs.begin(); it != vCCs.end(); ++it)
	{
		sConnected.insert(it->connector1);
		sConnected.insert(it->connector2);
	}
	for (vector<ConnectConstraint>::iterator it = vCCt.begin(); it != vCCt.end(); ++it)
	{
		sConnected.insert(it->connector1);
		sConnected.insert(it->connector2);
	}
	//2.��ô����ӵ����нӿ����Ƽ���sConnectors
	list<Component*> lstChilds = m_pCompRoot->GetChilds();
	for (list<Component*>::iterator it = lstChilds.begin(); it != lstChilds.end(); ++it)
	{
		Component* pComp = *it;
		DocumentCommon* m_pDoc = pComp->GetDocument();
		DocumentComponent* pDocComp = (DocumentComponent*)m_pDoc;
		vector<Connector*> vConntors = pDocComp->GetAllConnectors();
		for (vector<Connector*>::iterator it2 = vConntors.begin(); it2 != vConntors.end(); ++it2)
		{
			Connector* aaa = *it2;
			sConnectors.insert(aaa->GetConnName());
		}
	}
	if (sConnected == sConnectors)
	{
		//QMessageBox::information(NULL, "��ʾ", "���нӿ������ӣ����Է��棡", QMessageBox::Ok);
		return true;
	}
	else if (sConnected.size() > sConnectors.size())
	{
		//QMessageBox::information(NULL, "����", "δ֪���������ӽӿ����������нӿ���Ŀ", QMessageBox::Ok);
		return true;
	}
	else
	{
		string NotConnected;
		vector<string> notconnected;
		set_difference(sConnectors.begin(), sConnectors.end(), sConnected.begin(), sConnected.end(), back_inserter(notconnected));
		for (vector<string>::iterator it = notconnected.begin(); it != notconnected.end(); ++it)
		{
			NotConnected = NotConnected + *it + ";";
		}
		string b = ";";
		int length = strlen(NotConnected.c_str()) - strlen(b.c_str());
		string ReminderMessage = "�ӿ�" + NotConnected.substr(0, length) + "δ���ӣ�";
		QMessageBox::information(NULL, "��ʾ", QString::fromStdString(ReminderMessage), QMessageBox::Ok);
		return true;
	}

}

//�رշ���
void DocumentModel::CloseSimulate()
{
	//1.�ָ���ģ����

	//2.�ָ�������ʾ��ѡ��ģʽ
	UpdateDisplay1();
}

//ģ�ͷ���
bool DocumentModel::SimulateModel()
{
	//���жϽ���ļ�(*.csv)����ʱ���뵱ǰģ��(*.m3dmdl)����ʱ����һ�����£��Դ��ж��Ƿ���з���
	//QString file = "E:/workspace/Data/ColorTransfer.txt";
	//QFileInfo info(file);
	//qint64 size = info.size();//�ļ���С,��λbyte
	//QDateTime created = info.created();//����ʱ��
	//QDateTime lastModified = info.lastModified();//����޸�ʱ��
	//QDateTime lastRead = info.lastRead();//����ȡʱ��
	//qDebug() << size;
	//qDebug() << created;
	//qDebug() << lastModified;
	//qDebug() << lastRead;
	//bool isDir = info.isDir();//�Ƿ���Ŀ¼
	//bool isFile = info.isFile();//�Ƿ����ļ�
	//bool isSymLink = info.isSymLink();//�Ƿ��Ƿ�������
	//bool isHidden = info.isHidden();//�Ƿ�����
	//bool isReadable = info.isReadable();//�Ƿ�ɶ�
	//bool isWritable = info.isWritable();//�Ƿ��д
	//bool isExecutable = info.isExecutable();//�Ƿ��ǿ�ִ���ļ�
	//qDebug() << isDir << isFile << isSymLink << isHidden <<
	//	isReadable << isWritable << isExecutable;

	//��ȡ����ļ���Ϣ
	QString qsresultfile;
	string SolverName = LibPathsSetDlg::GetSolverName();
	if (SolverName == "openmodelica") {
		string solverpath = LibPathsSetDlg::GetSolverPath();
		if (solverpath.back() != '/')
		{
			solverpath += "/";
		}
		string sresultfile = solverpath + m_pCompRoot->GetCompName() + "_res.csv";
		qsresultfile = QString::fromStdString(sresultfile);
	}
	else {
		string sWorkDir = LibPathsSetDlg::GetWorkDir();
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//�Բ�ͬģ���в�ͬ�ļ��е����·��
		string sresultfile = saveCompareRaw + "Result.csv";
		qsresultfile = QString::fromStdString(sresultfile);
	}
	QFileInfo resultinfo(qsresultfile);
	QDateTime resultcreated = resultinfo.lastModified();//����޸�ʱ��

	//��ȡģ���ļ���Ϣ
	QString qsmodelfile = GetFileName();
	QFileInfo modelinfo(qsmodelfile);
	QDateTime modellastModified = modelinfo.lastModified();//����޸�ʱ��
	
	uint retime = resultcreated.toTime_t();
	uint motime = modellastModified.toTime_t();
	int tRet = retime - motime;

	bool isSimulate = false;              //�ж��Ƿ�����˷���

	MainWindow* pMain = (MainWindow*)parent();
	AnimationWidget* m_AniWid = pMain->GetAnimationWidget();
	if (tRet < 0 || resultcreated.isNull())
	{
		isSimulate = true;
		//1.���ģ���Ƿ������������﷨���
		if (!InitCheckModel())
		{
			return false;
		}

		//2.����Modelica�����mo�ļ�
		string sCode;
		if (!GenerateModelicaCode(sCode, 0))
		{
			QMessageBox::information(NULL, "����", "�������ɴ���");
			return false;
		}
		//������д���ļ����ڹ���·����
		QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";//�Բ�ͬģ�ͷֱ����ļ��е����·��
		bool flag = CreateDirectory(saveCompareRaw.toStdWString().c_str(), NULL);
		QString sFilePath = saveCompareRaw;
		sFilePath += m_pCompRoot->GetCompName().c_str();
		sFilePath += ".mo";
		QFile moFile(sFilePath);
		if (!moFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::information(NULL, "����", "mo�ļ��洢����");
			return false;
		}
		QTextStream txtOutput(&moFile);
		txtOutput << sCode.c_str() << endl;
		moFile.close();

		//3.���ɷ���mos�ű��ļ�(openmodelica)/py�ű��ļ�(MWorks)
		GenerateMosFile(sFilePath);

		//4.�����������ִ��mos�ļ���������ļ�����
		//���������֮ǰ�����AnimationWidget����Ϣ��
		m_AniWid->ClearMessage();
		CallSolver(saveCompareRaw);

		//5.�л�������������: ��߲����ͱ��������½����ߴ��ڣ�����������
	}

	//6.���㲿����ɢ����Ϊ������������ʾ�壬������������
	UpdateDisplay2();

	//������Ϣ����ʾ�����ж��Ƿ�����˷��棬���жϷ����Ƿ�ɹ�
	if (isSimulate == false) {
		//���û�н��з��棬��ʾ��Ϣ�����������£��Զ����ع�ע�����͹�ע����
		m_AniWid->ShowPostMessage(false, true);
	}

	return true;
}

//���ú�����Ϣ
void DocumentModel::SetPostMessage()
{
	bool isSimulateSuccess = false;       //��������˷��棬�жϷ����Ƿ�ɹ���ͨ��������ļ����������ж�
	
	//��ȡ����ļ���Ϣ
	QString qsresultfile;
	string SolverName = LibPathsSetDlg::GetSolverName();
	if (SolverName == "openmodelica") {
		string solverpath = LibPathsSetDlg::GetSolverPath();
		if (solverpath.back() != '/')
		{
			solverpath += "/";
		}
		string sresultfile = solverpath + m_pCompRoot->GetCompName() + "_res.csv";
		qsresultfile = QString::fromStdString(sresultfile);
	}
	else {
		string sWorkDir = LibPathsSetDlg::GetWorkDir();
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//�Բ�ͬģ���в�ͬ�ļ��е����·��
		string sresultfile = saveCompareRaw + "Result.csv";
		qsresultfile = QString::fromStdString(sresultfile);
	}
	QFileInfo resultinfo(qsresultfile);
	QDateTime resultcreated = resultinfo.lastModified();//����޸�ʱ��

	//��ȡģ���ļ���Ϣ
	QString qsmodelfile = GetFileName();
	QFileInfo modelinfo(qsmodelfile);
	QDateTime modellastModified = modelinfo.lastModified();//����޸�ʱ��

	uint retime = resultcreated.toTime_t();
	uint motime = modellastModified.toTime_t();
	int tRet = retime - motime;

	
	//������ļ�������
	if (resultinfo.isFile()) {
		if (tRet > 0) {
			isSimulateSuccess = true;
		}
	}

	//�����˷��棬�������ɹ������ع�ע�ı����Ͳ���
	MainWindow* pMain = (MainWindow*)parent();
	AnimationWidget* m_AniWid = pMain->GetAnimationWidget();
	if (isSimulateSuccess == true) {
		m_AniWid->ShowPostMessage(true, true);
	}
	else {
		m_AniWid->ShowPostMessage(true, false);
	}
}

bool DocumentModel::ReSimulateModel()
{
	QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
	if (sWorkDir.back() != '/')
	{
		sWorkDir += "/";
	}
	QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";//�Բ�ͬģ�ͷֱ����ļ��е����·��
	QString sFilePath = saveCompareRaw;
	sFilePath += m_pCompRoot->GetCompName().c_str();
	sFilePath += ".mo";

	//1.�������·����mos�ļ�
	GenerateReMosFile(sFilePath);

	//2.�����������ִ��mos�ļ�
	ReCallSolver(saveCompareRaw);

	//3.���㲿����ɢ
	UpdateDisplay2();

	return true;
}

void DocumentModel::GenerateReMosFile(const QString& sMoFilePath)
{
	string SolverName = LibPathsSetDlg::GetSolverName();
	if (SolverName == "openmodelica")
	{
		vector<ParameterValue> vecPara = GetAllParameterValues();
		string mosfile;
		string first = "print(\"Loading the Current model...\");\n";
		string second = "loadFile(\"";
		second += sMoFilePath.toStdString();
		second += "\");\n";
		string third = "getErrorString();\n";

		//�Ը�������ֵ�������ã����ɾȥҲ��Ӱ����򣬵���Ϊ�˿������ֵ��
		string forth = "print(\"Reassign the Parameter...\");\n";
		for (int i = 0; i < vecPara.size(); i++)
		{
			string MapParamName = vecPara[i].sMapParamName;
			string ParamValue = vecPara[i].sParamValue;
			replace(MapParamName.begin(), MapParamName.end(), '.', '_');
			//��һ�εĸ�ֵ���
			string AssignState = MapParamName + ":=" + ParamValue + ";\n";
			forth += AssignState;
		}
		forth += "print(\"call the generated simulation code to produce a result file name_res.csv\");\n";


		string fifth = "system(\"" + m_pCompRoot->GetCompName() + " -override=\"";
		//�Ը�����ע����ֵ���б���
		for (int j = 0; j < vecPara.size(); j++)
		{
			//�жϲ����Ƿ�Ϊ����ṹ���ǵĻ��뵥������
			string ParamValue = vecPara[j].sParamValue;
			string sign = "{";
			string::size_type idx;
			idx = ParamValue.find(sign);//��ParamValue�в���sign
			if (idx == string::npos)//�����ڣ���Ϊһ�������������������
			{
				string AssignState = "+\"" + vecPara[j].sMapParamName + "=" + vecPara[j].sParamValue + ",\"";
				fifth += AssignState;
			}
			else
			{
				//�Ƚ�{1,2,3}->,1,2,3,�����ַ����Ľ�ȡ
				replace(ParamValue.begin(), ParamValue.end(), '{', ',');
				replace(ParamValue.begin(), ParamValue.end(), '}', ',');
				int num = count(ParamValue.begin(), ParamValue.end(), ',');//,����4�Σ�����3��������Ҫ��ֵ
				QString qsParamValue = QString::fromStdString(ParamValue);
				for (int i = 1; i < num; i++)
				{
					string ParaName = vecPara[j].sMapParamName + "[" + std::to_string(i) + "]";
					QString QParaValue = qsParamValue.section(',', i, i);
					string ParaValue = QParaValue.toStdString();
					string AssignState = "+\"" + ParaName + "=" + ParaValue + ",\"";
					fifth += AssignState;
				}
			}
		}
		string OutputState = "+\" -r=" + m_pCompRoot->GetCompName() + "_res.csv\");\n";
		fifth += OutputState;

		string sixth = "getErrorString();\n";
		mosfile = first + second + third + forth + fifth + sixth;


		//������д���ļ�
		QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";
		string file = saveCompareRaw.toStdString() + "ReSimulateScript.mos";
		fstream deletefile(file, ios::out);//��ԭ���������
		ofstream ofile(file, std::ios::app);  //���ı�����д���ļ�
		ofile << mosfile << endl;   //д���ļ�
		ofile.close();   //�ر��ļ�
	}
	
}

void DocumentModel::ReCallSolver(const QString& SaveComparePath)
{
	string SolverName = LibPathsSetDlg::GetSolverName();
	if (SolverName == "openmodelica")
	{
		string disk_solver = LibPathsSetDlg::GetSolverDisk();
		string path_solver = "cd " + LibPathsSetDlg::GetSolverPath();
		//string disk_solver = "d:";                         //��������ڴ���ΪD��
		//string path_solver = "cd D:\\openmodelica\\bin";   //����������ļ�·��
		string order_solver = "omc " + SaveComparePath.toStdString() + "ReSimulateScript.mos";
		//string order_solver = "omc C:\\Users\\dell\\Desktop\\newfiles\\Script.mos";   //���������mos�ļ�
		//string solver = disk_solver + "&" + path_solver + "&" + order_solver + "&" + "pause";        //&Ϊ����֮������ӷ��ţ�ʹ��system����ʱ��Ҫ��"pause"
		string solver = disk_solver + "&" + path_solver + "&" + order_solver;                          //&Ϊ����֮������ӷ���
		//system(solver.c_str());

		QStringList argument;
		argument << "/c" << QString::fromStdString(solver);
		myProcess->start("cmd.exe", argument);
	}
}

void DocumentModel::exctReadStandardError()
{
	AnimationMessage[0].clear();
	AnimationMessage[1].clear();
	qDebug() << "exctReadStandardError()";
	QString error = QString::fromLocal8Bit(myProcess->readAllStandardOutput());
	qDebug() << error;
	AnimationMessage[0] = error;

}

vector<QString> DocumentModel::AniWidGetMessage()
{
	return AnimationMessage;
}

void DocumentModel::exctReadStandardOutput()
{
	AnimationMessage[0].clear();
	AnimationMessage[1].clear();
	qDebug() << "exctReadStandardOutput()";
	QString output = QString::fromLocal8Bit(myProcess->readAllStandardOutput());
	qDebug() << output;
	AnimationMessage[1] = output;
}

void DocumentModel::exctEnd(int exitCode, QProcess::ExitStatus exitStatus)
{
	qDebug() << "finished";
	qDebug() << exitCode;// �����ó����main���ص�int
	qDebug() << exitStatus;// QProcess::ExitStatus(NormalExit)
	qDebug() << "finished-output-readAll:";
	QString AllMessage = QString::fromLocal8Bit(myProcess->readAll());
	qDebug() << AllMessage;
	AnimationMessage[2] = AllMessage;
	qDebug() << "finished-output-readAllStandardOutput:";
	qDebug() << QString::fromLocal8Bit(myProcess->readAllStandardOutput());

}

void DocumentModel::UpdateDisplay2()
{
	myContext->RemoveAll(false);

	gp_Trsf thansfI;
	vector<Component*> vComponents;
	m_pCompRoot->GenerateAISObjects(thansfI, vComponents);

	auto itr = vComponents.begin();
	for (; itr != vComponents.end(); ++itr)
	{
		Component* pCom = *itr;
		string sCompFullName = pCom->GetCompFullName();
		vector<Handle(AIS_InteractiveObject)> vAIS = pCom->GetAIS();

		for (int i = 0; i < vAIS.size(); i++)
		{
			Handle(AIS_InteractiveObject) theAis = vAIS[i];
			myContext->Display(theAis, false);
		}
	}
	DisplayGlobalCoordsys();
	myContext->UpdateCurrentViewer();
}

//�������湤��Ŀ¼�ļ���
void DocumentModel::GenerateSimulateWorkingDirectory()
{
	string folderPath = LibPathsSetDlg::GetWorkDir();
	//string folderPath = "C:\\Users\\dell\\Desktop\\newfiles";//�ڸ�·���´���һ���µ��ļ���
	string command;
	command = "mkdir -p " + folderPath;
	system(command.c_str());
}

//���ɷ���*.mos�ű��ļ�
void DocumentModel::GenerateMosFile(const QString& sMoFilePath)
{
	string SolverName = LibPathsSetDlg::GetSolverName();
	if (SolverName == "openmodelica")
	{
		string mosfile;
		string first = "// load the file\n";//����*.mos���ļ�����
		string second = "print(\"Loading Modelica\");\n";
		//string third = "loadModel(Modelica); //new OMC version stopped importing Modelica model\n";//���ص�4.0.0�Ŀ�
		//string sSolverPath = LibPathsSetDlg::GetSolverPath(); 
		//if (sSolverPath.back() != '/')
		//{
		//	sSolverPath += "/";
		//}
		//sSolverPath = sSolverPath.substr(0, sSolverPath.length() - 4);
		//string third = "loadFile(\"" + sSolverPath + "lib/omlibrary/Modelica 3.2.3/package.mo\");\n";
		string third;
		vector<string> AllLibPaths;
		AllLibPaths.clear();
		AllLibPaths = LibPathsSetDlg::GetModelLibPaths();
		for (int i = 0; i < AllLibPaths.size(); i++)
		{
			string statement = "loadFile(\"" + AllLibPaths[i] + "\");\n";
			third += statement;
		}
		string AddLib1 = "print(\"Loading Complex\");\n";
		//AddLib1 += "loadModel(Complex);\n";
		string AddLib2 = "print(\"Loading ModelicaServices\");\n";
		//AddLib2 += "loadModel(ModelicaServices);\n";
		string forth = "print(\"Loading the Current model...\");\n";
		string fifth = "loadFile(\"";
		fifth += sMoFilePath.toStdString();
		fifth += "\");\n";
		string sixth = "getErrorString();\nprint(\"Simulating the Current model...\");\n";
		string seventh = "simulate(";
		seventh += m_pCompRoot->GetCompName();
		//seventh += ", stopTime = 5,outputFormat = \"csv\");\n";
		seventh += ",startTime=";
		seventh += to_string(SimulateSetDlg::GetStarttime());
		seventh += ",stopTime=";
		seventh += to_string(SimulateSetDlg::GetEndtime());
		seventh += ",numberOfIntervals=";
		seventh += to_string(SimulateSetDlg::GetOpenModelica_NumberOfIntervals());
		seventh += ",method=\"";
		seventh += SimulateSetDlg::GetOpenModelica_Method();
		if (SimulateSetDlg::GetOpenModelica_OutputMethodID() == 0)
		{
			seventh += "\",outputFormat=\"csv\");\n";
		}
		else
		{
			seventh += "\",outputFormat=\"csv\",variableFilter=\"^.*frame_[ab]\\.r_0.*|.*frame_[ab]\\.R\\.T.*$\");\n";
		}
		string eighth = "getErrorString();\n";
		mosfile = first + second + third + AddLib1 + AddLib2 + forth + fifth + sixth + seventh + eighth;
		QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";
		string file = saveCompareRaw.toStdString() + "SimulateScript.mos";
		fstream deletefile(file, ios::out);//��ԭ���������
		//string file = "C:\\Users\\dell\\Desktop\\newfiles\\test02.mos";
		//fstream deletefile("C:\\Users\\dell\\Desktop\\newfiles\\test02.mos", ios::out);//��ԭ���������
		ofstream ofile(file, std::ios::app);  //���ı�����д���ļ�
		ofile << mosfile << endl;   //д���ļ�
		ofile.close();   //�ر��ļ�
	}
	else if(SolverName == "MWorks")
	{
		string pyfile;
		string first = "#����ָ����Modelicaģ���ļ�\n";
		string second;
		vector<string> AllLibPaths;
		AllLibPaths.clear();
		AllLibPaths = LibPathsSetDlg::GetModelLibPaths();
		for (int i = 0; i < AllLibPaths.size(); i++)
		{
			string statement = "print(OpenModelFile(r\"" + AllLibPaths[i] + "\",True))\n";
			second += statement;
		}
		string third = "#����ģ��\n";
		string sWorkDir = LibPathsSetDlg::GetWorkDir();
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//�Բ�ͬģ���в�ͬ�ļ��е����·��
		
		string forth = "print(SimulateModel(model_name='" + m_pCompRoot->GetCompName() + "',start_time=" + to_string(SimulateSetDlg::GetStarttime()) + ",stop_time=" + to_string(SimulateSetDlg::GetEndtime());
		forth += ",number_of_intervals=" + to_string(SimulateSetDlg::GetMWorks_NumberOfIntervals()) + ",algo=" + SimulateSetDlg::GetMWorks_Method() + ",result_file = r\"" + saveCompareRaw + "\"))\n";
		string fifth = "#��������ļ�\n";
		string sixth;
		if (SimulateSetDlg::GetMWorks_OutputMethodID() == 0)
		{
			sixth = "print(ExportResult(r'" + saveCompareRaw + "Result.csv','csv','',False))\n";
		}
		else
		{
			string AllAniVariables;
			AllAniVariables += "[";
			list<Component*> lstChilds = m_pCompRoot->GetChilds();
				for (list<Component*>::iterator it = lstChilds.begin(); it != lstChilds.end(); ++it)
			{
				Component* pComp = *it;
				string CompName = pComp->GetCompName();
				if (CompName != "world")
				{
					if (pComp->IsDeformable())//����Ǳ������Ҫ���frame_b�������
					{
						AllAniVariables += "'" + CompName + ".frame_b.r_0[1]',";
						AllAniVariables += "'" + CompName + ".frame_b.r_0[2]',";
						AllAniVariables += "'" + CompName + ".frame_b.r_0[3]',";
					}
					AllAniVariables += "'" + CompName + ".frame_a.r_0[1]',";
					AllAniVariables += "'" + CompName + ".frame_a.r_0[2]',";
					AllAniVariables += "'" + CompName + ".frame_a.r_0[3]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[1, 1]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[1, 2]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[1, 3]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[2, 1]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[2, 2]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[2, 3]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[3, 1]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[3, 2]',";
					AllAniVariables += "'" + CompName + ".frame_a.R.T[3, 3]'";
					if (it != (--lstChilds.end()))
					{
						AllAniVariables += ",";
					}
				}
			}
			AllAniVariables += "]";
			sixth = "print(ExportResult(r'" + saveCompareRaw + "Result.csv','csv'," + AllAniVariables + ",False))\n";
		}
		string seventh = "SaveScreen(r\"" + saveCompareRaw + "RecordOutput.txt\")\n";
		pyfile = first + second + third + forth + fifth + sixth + seventh;
		string file = saveCompareRaw + "SimulateCmd.py";
		fstream deletefile(file, ios::out);//��ԭ���������
		ofstream ofile(file, std::ios::app);  //���ı�����д���ļ�
		ofile << pyfile << endl;   //д���ļ�
		ofile.close();   //�ر��ļ�
	}
	else
	{
		QMessageBox::information(NULL, "��ʾ", "�������������δ����ȷ���ɷ���ű��ļ�", QMessageBox::Ok);
	}
}

//�����������ִ��mos�ļ���������ļ�����
void DocumentModel::CallSolver(const QString& SaveComparePath)
{
	string SolverName = LibPathsSetDlg::GetSolverName();
	string solver;
	if (SolverName == "openmodelica")
	{
		string disk_solver = LibPathsSetDlg::GetSolverDisk();
		string path_solver = "cd " + LibPathsSetDlg::GetSolverPath();
		//string disk_solver = "d:";                         //��������ڴ���ΪD��
		//string path_solver = "cd D:\\openmodelica\\bin";   //����������ļ�·��
		string order_solver = "omc " + SaveComparePath.toStdString() + "SimulateScript.mos";
		//string order_solver = "omc C:\\Users\\dell\\Desktop\\newfiles\\Script.mos";   //���������mos�ļ�
		//string solver = disk_solver + "&" + path_solver + "&" + order_solver + "&" + "pause";        //&Ϊ����֮������ӷ���
		solver = disk_solver + "&" + path_solver + "&" + order_solver;       //&Ϊ����֮������ӷ���
		//system(solver.c_str());
		//system("d:&cd D:\\openmodelica\\bin&omc C:\\Users\\dell\\Desktop\\newfiles\\Script.mos");
	}
	else if (SolverName == "MWorks")
	{
		string disk_solver = LibPathsSetDlg::GetSolverDisk();
		string path_solver = "cd " + LibPathsSetDlg::GetSolverPath();
		QString sFilePath = SaveComparePath;

		//0306���Ը���
		//sFilePath += "robottest";
		sFilePath += m_pCompRoot->GetCompName().c_str();
		sFilePath += ".mo";
		string scriptPath = SaveComparePath.toStdString();
		scriptPath += "SimulateCmd.py";
		string order_solver = "mworks -q -l " + sFilePath.toStdString() + " -r " + scriptPath;
		solver = disk_solver + "&" + path_solver + "&" + order_solver;
		//solver = "D:&cd D:/MWorks/MWorks.Sysplorer 2021/Bin64&mworks -q -l C:/Users/dell/Desktop/MWorks_test/p2.mo -r C:/Users/dell/Desktop/MWorks_test/cmdtest.py";
		//system(solver.c_str());
	}
	

	MainWindow* pMain = (MainWindow*)parent();
	AnimationWidget* m_AniWid = pMain->GetAnimationWidget();
	myProcess = new QProcess(this);

	connect(myProcess, SIGNAL(readyReadStandardError()), m_AniWid, SLOT(exctReadStandardError()));
	connect(myProcess, SIGNAL(readyReadStandardOutput()), m_AniWid, SLOT(exctReadStandardOutput()));
	connect(myProcess, SIGNAL(finished(int, QProcess::ExitStatus)), m_AniWid, SLOT(exctEnd(int, QProcess::ExitStatus)));
	//���MWorks��������readyReadStandardError()�ź�,ͨ��.py�ű������������RecordOutput.txt�ļ���Ȼ����ļ��������
	//connect(myProcess, SIGNAL(readyReadStandardOutput()), this,SLOT({qDebug() << myProcess->readAllStandardOutput()}));

	AnimationMessage.push_back("");
	AnimationMessage.push_back("");
	AnimationMessage.push_back("");

	myProcess->setReadChannel(QProcess::StandardOutput);
	
	//0423
	//myProcess->setReadChannel(QProcess::StandardError);
	//myProcess->setReadChannelMode(QProcess::MergedChannels);//�ϲ�ͨ��
	//myProcess->setProcessChannelMode(QProcess::ForwardedChannels);//ת��ͨ��

	QStringList argument;
	//string order_argument = SaveComparePath.toStdString() + "SimulateScript.mos";
	//argument << QString::fromStdString(order_argument);
	//string sSolverPath = LibPathsSetDlg::GetSolverPath();
	//if (sSolverPath.back() != '/' && sSolverPath.back() != '\\')
	//{
	//	sSolverPath += "/";
	//}
	//string order_exe = sSolverPath + "omc.exe";
	//myProcess->start(QString::fromStdString(order_exe), argument);

	if (SolverName == "openmodelica")
	{
		argument << "/c" << QString::fromStdString(solver);
		myProcess->start("cmd.exe", argument);
	}
	else if (SolverName == "MWorks")
	{
		argument << "/c" << QString::fromStdString(solver);
		myProcess->start("cmd.exe", argument);
	}
	
}


//
void DocumentModel::SetCompLocation(Component* pCom, gp_Trsf trsf)
{
	vector<Handle(AIS_InteractiveObject)> vAIS = pCom->GetAIS();
	for (int j = 0; j < vAIS.size(); j++)
	{
		myContext->SetLocation(vAIS[j], trsf);
	}
}

void DocumentModel::testLocation(Component* pCom)
{
	vector<Handle(AIS_InteractiveObject)> vAIS = pCom->GetAIS();
	for (int j = 0; j < vAIS.size(); j++)
	{
		gp_Trsf trsf = myContext->Location(vAIS[j]);
		int breakpoint = 0;
	}
}

//����
void DocumentModel::Animating()
{
	////�ȵõ�ʱ������ֵ
	//m_pCompRoot->GetAllDescendants(m_vDescendants);
	//QString sCSVFile0 = "C:/Program Files/OpenModelica1.16.1-64bit/bin/Modelica.Mechanics.MultiBody.Examples.Elementary.Pendulum_res.csv";
	//bool b0 = FileParse::GetAllMultibodyPlacement(sCSVFile0, m_timeSeries, m_mapCompPlacement);
	//if (!b0)
	//{
	//	//��ʾ����;
	//	assert(false);
	//	return;
	//}

	//startTimer(100);
	//m_iStep = 0;
	//return;

	//1.����Ƿ����ڶ���������ǣ����˳�
	//if (m_pAnimateThread->isRunning())
	if (m_pAnimateThread->m_bRunning == true)
	{
		return;
	}

	//2.�ȼ�⹤��Ŀ¼���Ƿ��н���ļ�
	string SolverName = LibPathsSetDlg::GetSolverName();
	QString sCSVFile;
	if (SolverName == "openmodelica")
	{
		sCSVFile = LibPathsSetDlg::GetSolverPath().c_str();
		if (sCSVFile.back() != '/')
		{
			sCSVFile += "/";
		}
		sCSVFile += m_pCompRoot->GetCompName().c_str();
		sCSVFile += "_res.csv";
	}
	else if (SolverName == "MWorks")
	{
		sCSVFile = LibPathsSetDlg::GetWorkDir().c_str();
		if (sCSVFile.back() != '/')
		{
			sCSVFile += "/";
		}
		sCSVFile += m_pCompRoot->GetCompName().c_str();
		sCSVFile += "/";
		sCSVFile += "Result.csv";
	}
	

	//3.���ö����̵߳�����
	MainWindow* pWnd = (MainWindow*)parent();
	AnimationWidget* m_AniWid = pWnd->GetAnimationWidget();
	double speed = m_AniWid->GetAnimationSpeed();
	
	m_pAnimateThread->SetCSVFilePath(sCSVFile);
	vector<Component*> vDescendants;
	m_pCompRoot->GetAllDescendants(vDescendants);
	m_pAnimateThread->SetComponents(vDescendants);
	m_pAnimateThread->SetContext(myContext);
	m_pAnimateThread->SetDocumentModel(this);
	m_pAnimateThread->SetSpeedK(speed);

	//4.���������߳�
	MainWindow* pMain = (MainWindow*)parent();
	connect(m_pAnimateThread, SIGNAL(rep()), pMain, SLOT(repaint0()), Qt::BlockingQueuedConnection);
	//connect(m_pAnimateThread, SIGNAL(rep()), this, SLOT(repaint0()), Qt::QueuedConnection);
	m_pAnimateThread->start();
}

void DocumentModel::timerEvent(QTimerEvent* event)
{
	//2.�����˶�
	if (m_iStep < m_timeSeries.size())
	{
		//����t0ʱ�̸����㲿����λ��
		auto itr = m_vDescendants.begin();
		for (; itr != m_vDescendants.end(); ++itr)
		{
			Component* pCom = *itr;
			if (1)//pCom->IsMultibodyComponent())//����ǻ�е���岿��
			{
				gp_Trsf trsf;
				string sComFullName = pCom->GetCompFullName();
				//ȥ��ǰ׺��ϵͳ��
				int posDot = sComFullName.find('.');
				assert(posDot > 0);
				sComFullName = sComFullName.substr(posDot + 1);
				auto itr_i = m_mapCompPlacement.find(sComFullName);
				if (itr_i != m_mapCompPlacement.end())
				{
					vector<gp_Trsf> vTrsf = itr_i->second;
					trsf = vTrsf[m_iStep];
					SetCompLocation(pCom, trsf);
				}
			}
		}
		myContext->UpdateCurrentViewer();

		m_iStep++;
	}
}

void DocumentModel::repaint0()
{
	myContext->UpdateCurrentViewer();
}

//��ͣ�����߳�
void DocumentModel::AnimatingPause()
{
	//1.����Ƿ����ڶ�����������ͣ
	if (m_pAnimateThread->m_bRunning == true && m_pAnimateThread->m_bPause == false)
	{
		m_pAnimateThread->pauseThread();
	}
	else { return; }
}

//��ͣ��ָ�����
void DocumentModel::AnimatingResume()
{
	if (m_pAnimateThread->m_bRunning == true && m_pAnimateThread->m_bPause == true)
	{
		m_pAnimateThread->resumeThread();
	}
	else { return; }
}

//���ö���
void DocumentModel::AnimatingReset()
{
	if (m_pAnimateThread->m_bPause == true || m_pAnimateThread->m_bRunning == false)
	{
		m_pAnimateThread->closeThread();
		myContext->UpdateCurrentViewer();                 //������������ʼλ��
		if (m_pAnimateThread != nullptr)
		{
			m_pAnimateThread->terminate();                //ֹͣ�߳�
			m_pAnimateThread->wait();
			delete m_pAnimateThread;
			m_pAnimateThread = nullptr;                   //���³�ʼ��
		}
		m_pAnimateThread = new AnimationThread(this);
		m_pAnimateThread->m_bRunning = false;
	}
	else { return; }
}

//��ȡ��ǰ����������
double DocumentModel::GetAnimatingTime()
{
	return m_pAnimateThread->datatime;
}
vector<string> DocumentModel::GetAnimatingDatas()
{
	return m_pAnimateThread->AllDatas;
}

//��ѡ�е����(�������������������CyberInfo
void DocumentModel::ReplaceCyberInfo()
{
	assert(false);
}

//������������
bool DocumentModel::TwinDriveModel() {
	//���㲿����ɢ����Ϊ������������ʾ�壬������������
	UpdateDisplay2();
	return true;
}

//��ʼ�����ĺͼ���
int DocumentModel::InitSubsAndListner(int discoveryMehodID) {
	//������ָ�봫�����������߳�
	StoredData* m_pStoredData = new StoredData();
	if (m_pTwinAniThread->m_pStoredData != nullptr) {
		delete m_pTwinAniThread->m_pStoredData;
	}
	m_pTwinAniThread->m_pStoredData = m_pStoredData;
	
	//�������������̵߳�����
	vector<Component*> vDescendants;
	m_pCompRoot->GetAllDescendants(vDescendants);
	m_pTwinAniThread->SetComponents(vDescendants);
	m_pTwinAniThread->SetContext(myContext);
	m_pTwinAniThread->SetDocumentModel(this);

	//�������������߳�
	MainWindow* pMain = (MainWindow*)parent();
	connect(m_pTwinAniThread, SIGNAL(rep()), pMain, SLOT(repaint0()), Qt::BlockingQueuedConnection);
	m_pTwinAniThread->start();
	return InitSubscribe(m_pStoredData, discoveryMehodID);
}

//�������ĺͼ���
int DocumentModel::EndSubsAndListner() {
	m_pTwinAniThread->closeThread();
	myContext->UpdateCurrentViewer();                 //������������ʼλ��
	if (m_pTwinAniThread != nullptr)
	{
		m_pTwinAniThread->terminate();                //ֹͣ�߳�
		m_pTwinAniThread->wait();
		delete m_pTwinAniThread;
		m_pTwinAniThread = nullptr;                   //���³�ʼ��
	}
	m_pTwinAniThread = new TwinAnimationThread(this);
	return EndSubscribe();
}

//��ȡ���յ�����Ϣ
string DocumentModel::GetReceivedMsg() {
	return m_pTwinAniThread->GetLastReceivedMsg();
}

//����������ѵ����
void DocumentModel::GenTrainSet(vector<string>& ControlVarsNames, vector<vector<double>>& samples, vector<string>& InputNames, vector<string>& OutputNames)
{	
	QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
	if (sWorkDir.back() != '/')
	{
		sWorkDir += "/";
	}
	QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";//�Բ�ͬģ�ͷֱ����ļ��е����·��
	
	//����ѵ���ű��ļ�   MWorks .py�ļ�
	GenTrainpyFile(ControlVarsNames, samples, InputNames, OutputNames);

	//ִ�нű������ɶ��Result.csv�ļ�	
	CallSolverTrain(saveCompareRaw);
	
	//��ȡResult.csv�ļ���������Щ�ļ����ɷ���ѵ�������ݼ��ļ�
	//�ɰ棬fann�汾
	//int samplesize = samples.size();
	//int inputsize = InputNames.size();
	//GenTrainSetFromCSV(saveCompareRaw, samplesize, inputsize);
	//GenTrainSetFromCSVLayer(saveCompareRaw, samplesize);

	//�°棬����Pytorch
	int samplesize = samples.size(); // ��������csv�ļ�����
	//bool issuccess = GenPyTrainSetFromCSVDiffComp(saveCompareRaw, samplesize, InputNames, OutputNames);
	bool issuccess = GenPyTrainSetFromCSVDiffCompNew(saveCompareRaw, samplesize, InputNames, OutputNames);
	if (issuccess) {
		QString outputmsg = "ѵ�������ɳɹ�������" + saveCompareRaw + "dataset·���½��в鿴��";
		QMessageBox msgBox;
		msgBox.setText(outputmsg);
		msgBox.exec();
		return;
	} else {
		QMessageBox msgBox;
		msgBox.setText("Error��ѵ��������ʧ�ܣ�");
		msgBox.exec();
		return;
	}
}

//���mo�ļ��Ĵ洢·��
string DocumentModel::GetfannSavePath()
{
	QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
	if (sWorkDir.back() != '/')
	{
		sWorkDir += "/";
	}
	QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";//�Բ�ͬģ�ͷֱ����ļ��е����·��
	string fannFilePath = saveCompareRaw.toStdString() + m_pCompRoot->GetCompName().c_str() + ".fann";
	return fannFilePath;
}

void DocumentModel::GenTrainpyFile(vector<string>& ControlVarsNames, vector<vector<double>>& samples, vector<string>& InputNames, vector<string>& OutputNames)
{
	string pyfile;
	string first = "# Load the modelica file from the path you set\n";
	string second;
	vector<string> AllLibPaths;
	AllLibPaths.clear();
	AllLibPaths = LibPathsSetDlg::GetModelLibPaths();
	for (int i = 0; i < AllLibPaths.size(); i++)
	{
		string statement = "print(OpenModelFile(r\"" + AllLibPaths[i] + "\",True))\n";
		second += statement;
	}
	string third = "# Simulate model\n";
	string sWorkDir = LibPathsSetDlg::GetWorkDir();
	if (sWorkDir.back() != '/')
	{
		sWorkDir += "/";
	}
	string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//�Բ�ͬģ���в�ͬ�ļ��е����·��

	string forth = "print(SimulateModel(model_name='" + m_pCompRoot->GetCompName() + "',start_time=" + to_string(SimulateSetDlg::GetStarttime()) + ",stop_time=" + to_string(SimulateSetDlg::GetEndtime());
	forth += ",number_of_intervals=" + to_string(SimulateSetDlg::GetMWorks_NumberOfIntervals()) + ",algo=" + SimulateSetDlg::GetMWorks_Method() + "))\n";
	
	string fifth = "# Export result file\n";
	string AllAniVariables;
	AllAniVariables += "[";
	
	//1.���õ�������
	for (int i = 0; i < InputNames.size(); i++)
	{
		AllAniVariables += "'" + InputNames[i] + "',";
	}

	//2.���˶�/������ص������
	for (int i = 0; i < OutputNames.size(); i++)
	{
		AllAniVariables += "'" + OutputNames[i] + "'";
		if (i != OutputNames.size() - 1)
		{
			AllAniVariables += ",";
		}
	}

	AllAniVariables += "]";

	string newfloder = saveCompareRaw + "Train/";
	bool flag = CreateDirectory(stringToLPCWSTR(newfloder), NULL);
	string sixth = "print(ExportResult(r'" + saveCompareRaw + "Train/Result.csv','csv'," + AllAniVariables + ",False))\n";


	//���ݼ�����
	vector<string> SampleSentence;
	for (int i = 0; i < samples.size(); i++)
	{
		string nowsentence;
		for (int j = 0; j < samples[i].size(); j++)
		{
			string SetInitialValue = "SetInitialValue('" + ControlVarsNames[j] + "'," + std::to_string(samples[i][j]) + ")\n";
			nowsentence += SetInitialValue;
		}
		nowsentence += forth;
		string output = "print(ExportResult(r'" + saveCompareRaw + "Train/Result" + std::to_string(i) + ".csv','csv'," + AllAniVariables + ",False))\n";
		nowsentence += output;
		SampleSentence.push_back(nowsentence);
	}

	
	pyfile = first + second + third + forth + fifth + sixth;
	for (int i = 0; i < SampleSentence.size(); i++)
	{
		pyfile += SampleSentence[i];
	}
	string seventh = "SaveScreen(r\"" + saveCompareRaw + "RecordOutput.txt\")\n";
	pyfile += seventh;
	string file = saveCompareRaw + "TrainSimulateCmd.py";
	fstream deletefile(file, ios::out);//��ԭ���������
	ofstream ofile(file, std::ios::app);  //���ı�����д���ļ�
	ofile << pyfile << endl;   //д���ļ�
	ofile.close();   //�ر��ļ�
}


void DocumentModel::CallSolverTrain(const QString& SaveComparePath)
{
	string disk_solver = LibPathsSetDlg::GetSolverDisk();
	string path_solver = "cd " + LibPathsSetDlg::GetSolverPath();
	QString sFilePath = SaveComparePath;


	//0306���Ը���
	//sFilePath += "robottest";
	sFilePath += m_pCompRoot->GetCompName().c_str();
	sFilePath += ".mo";
	string scriptPath = SaveComparePath.toStdString();
	scriptPath += "TrainSimulateCmd.py";
	string order_solver = "mworks -q -l " + sFilePath.toStdString() + " -r " + scriptPath;
	string solver = disk_solver + "&" + path_solver + "&" + order_solver;

	QProcess* TrainProcess = new QProcess(this);
	QStringList argument;
	argument << "/c" << QString::fromStdString(solver);
	TrainProcess->execute("cmd.exe", argument);//��������
}

// ���ɹ�Pytorchѵ���õ�������ѵ���������ղ�ͬ����ֱ�����
bool DocumentModel::GenPyTrainSetFromCSVDiffComp(const QString& saveComparePath, int samplesize, vector<string>& InputNames, vector<string>& OutputNames)
{
	// ��������������ƣ��Բ�ͬ�������з��࣬��ͬ����ı����ֵ���ͬ��
	set<string> compNames;
	map<string, vector<string>> compNameToVarNames;
	for (int i = 0; i < OutputNames.size(); i++) {
		string outputname = OutputNames[i];
		string nowcompname = "";
		for (int i = 0; i < outputname.length(); i++) {
			if (outputname.at(i) != '.') {
				nowcompname.push_back(outputname.at(i));
			} else {
				break;
			}
		}
		compNames.insert(nowcompname);
		compNameToVarNames[nowcompname].push_back(outputname);
	}

	// ��������������ṹ���ļ�
	for (auto it = compNames.begin(); it != compNames.end(); it++)
	{
		CSV_Parser::Sheet sheet;
		// ���ñ�����
		sheet.name = QString::fromStdString(*it);
		
		// ���õ�һ�������������
		QStringList strListNum;
		strListNum.append(QString::fromStdString(std::to_string(InputNames.size())));
		strListNum.append(QString::fromStdString(std::to_string(compNameToVarNames[*it].size())));
		sheet.data.append(strListNum);


		// ���õڶ��еĲ�������
		QStringList strList;
		for (string inputname : InputNames) {
			strList.append(QString::fromStdString(inputname));
		}
		for (int i = 0; i < compNameToVarNames[*it].size(); i++) {
			strList.append(QString::fromStdString(compNameToVarNames[*it][i]));
		}
		sheet.data.append(strList);

		string newfloder = saveComparePath.toStdString() + "netfrm/";
		bool flag = CreateDirectory(stringToLPCWSTR(newfloder), NULL);

		// ����������ṹ�ļ�
		QString netFrmPath = saveComparePath + "netfrm/netfrm_" + QString::fromStdString(*it) + ".csv";
		bool isnowsuccess = FileParse::generateCSV(netFrmPath, sheet);
	}

	// ��Բ�ͬ���������ͬsheet����generateCSV�ļ�ʹ��
	map<string, CSV_Parser::Sheet> compNameToSheet;
	for (auto it = compNames.begin(); it != compNames.end(); it++)
	{
		CSV_Parser::Sheet sheet;
		// ���ñ�����
		sheet.name = QString::fromStdString(*it);
		// ���õ�һ�еĲ�������
		QStringList strList;
		for (string inputname : InputNames) {
			strList.append(QString::fromStdString(inputname));
		}
		for (int i = 0; i < compNameToVarNames[*it].size(); i++) {
			strList.append(QString::fromStdString(compNameToVarNames[*it][i]));
		}
		sheet.data.append(strList);
		compNameToSheet[*it] = qMove(sheet);
	}

	// �����������Ƶ�������������ӳ��
	map<string, int> varNameToidx;
	QString csvFilePath = saveComparePath + "Train/Result.csv";
	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	for (int col = 0; col < allOutputNames.size(); col++)
	{
		string colname = allOutputNames[col].toStdString();
		varNameToidx[colname] = col;		
	}

	// ��ȡ.csv�ļ��������������
	int rowsbycsv = 30; // ÿ��csv�ļ�ȡ30������
	for (int i = 0; i < samplesize; i++)
	{
		QString csvFilePath = saveComparePath + "Train/Result" + QString::fromStdString(std::to_string(i)) + ".csv";
		vector<QStringList> AllRows = FileParse::GetDataByNums(csvFilePath, rowsbycsv);
		for (int row = 0; row < AllRows.size(); row++)
		{
			QStringList rowdata = AllRows[row];
			for (auto it = compNameToVarNames.begin(); it != compNameToVarNames.end(); it++) {
				pair<string, vector<string>> nowpair = *it;
				string compname = nowpair.first;
				vector<string> varnames = nowpair.second;
				// ��ͬ�����ϲ�ͬ������
				QStringList strList;
				for (string inputname : InputNames) {
					int colidx = varNameToidx[inputname];
					strList.append(rowdata[colidx]);
				}
				for (string varname : varnames) {
					int colidx = varNameToidx[varname];
					strList.append(rowdata[colidx]);
				}
				compNameToSheet[compname].data.append(strList);
			}
		}
	}

	string newfloder = saveComparePath.toStdString() + "dataset/";
	bool flag = CreateDirectory(stringToLPCWSTR(newfloder), NULL);
	bool issuccess = true;
	// ������ݼ��ļ�
	for (auto it = compNameToSheet.begin(); it != compNameToSheet.end(); it++)
	{
		pair<string, CSV_Parser::Sheet> nowpair = *it;
		string compname = nowpair.first;
		QString dataSetPath = saveComparePath + "dataset/dataset_" + QString::fromStdString(compname) + ".csv";
		bool isnowsuccess = FileParse::generateCSV(dataSetPath, nowpair.second);
		if (!isnowsuccess) {
			issuccess = isnowsuccess;
		}
	}
	return issuccess;
}

vector<double> DocumentModel::matrixTToEuler(vector<vector<double>>& T) {
	vector<double> euler(3, 0.0);
	euler[0] = atan2(-1 * T[3][1], sqrt(T[1][1] * T[1][1] + T[2][1] * T[2][1]));
	euler[1] = atan2(T[2][1] / cos(euler[0]), T[1][1] / cos(euler[0]));
	euler[2] = atan2(T[3][2] / cos(euler[0]), T[3][3] / cos(euler[0]));
	return euler;
}

// ֧����ת����תŷ���ǣ�֧�ֻ�ϱ��
bool DocumentModel::GenPyTrainSetFromCSVDiffCompNew(const QString& saveComparePath, int samplesize, vector<string>& InputNames, vector<string>& OutputNames)
{
	// ��������������ƣ��Բ�ͬ�������з��࣬��ͬ����ı����ֵ���ͬ��
	set<string> compNames;
	map<string, vector<string>> compNameToVarNames;
	for (int i = 0; i < OutputNames.size(); i++) {
		string outputname = OutputNames[i];
		string nowcompname = "";
		for (int i = 0; i < outputname.length(); i++) {
			if (outputname.at(i) != '.') {
				nowcompname.push_back(outputname.at(i));
			}
			else {
				break;
			}
		}
		compNames.insert(nowcompname);
		compNameToVarNames[nowcompname].push_back(outputname);
	}

	// ��������������ṹ���ļ�
	for (auto it = compNames.begin(); it != compNames.end(); it++)
	{
		CSV_Parser::Sheet sheet;
		// ���ñ�����
		sheet.name = QString::fromStdString(*it);

		// ���õ�һ�������������
		QStringList strListNum;
		strListNum.append(QString::fromStdString(std::to_string(InputNames.size())));
		//strListNum.append(QString::fromStdString(std::to_string(compNameToVarNames[*it].size())));
		strListNum.append(QString::fromStdString(std::to_string(6)));//�����ɶ�
		sheet.data.append(strListNum);


		// ���õڶ��еĲ�������
		QStringList strList;
		for (string inputname : InputNames) {
			strList.append(QString::fromStdString(inputname));
		}
		if (compNameToVarNames[*it].size() == 6) {
			for (int i = 0; i < compNameToVarNames[*it].size(); i++) {
				strList.append(QString::fromStdString(compNameToVarNames[*it][i]));
			}
		}
		else {
			strList.append(QString::fromStdString(*it) + ".frame_a.r_0[1]");
			strList.append(QString::fromStdString(*it) + ".frame_a.r_0[2]");
			strList.append(QString::fromStdString(*it) + ".frame_a.r_0[3]");
			strList.append(QString::fromStdString(*it) + ".Alpha");
			strList.append(QString::fromStdString(*it) + ".Beta");
			strList.append(QString::fromStdString(*it) + ".Gamma");
		}
		sheet.data.append(strList);

		string newfloder = saveComparePath.toStdString() + "netfrm/";
		bool flag = CreateDirectory(stringToLPCWSTR(newfloder), NULL);

		// ����������ṹ�ļ�
		QString netFrmPath = saveComparePath + "netfrm/netfrm_" + QString::fromStdString(*it) + ".csv";
		bool isnowsuccess = FileParse::generateCSV(netFrmPath, sheet);
	}

	// ��Բ�ͬ���������ͬsheet����generateCSV�ļ�ʹ��
	map<string, CSV_Parser::Sheet> compNameToSheet;
	for (auto it = compNames.begin(); it != compNames.end(); it++)
	{
		CSV_Parser::Sheet sheet;
		// ���ñ�����
		sheet.name = QString::fromStdString(*it);
		// ���õ�һ�еĲ�������
		QStringList strList;
		for (string inputname : InputNames) {
			strList.append(QString::fromStdString(inputname));
		}
		if (compNameToVarNames[*it].size() == 6) {
			for (int i = 0; i < compNameToVarNames[*it].size(); i++) {
				strList.append(QString::fromStdString(compNameToVarNames[*it][i]));
			}
		}
		else {
			strList.append(QString::fromStdString(*it) + ".frame_a.r_0[1]");
			strList.append(QString::fromStdString(*it) + ".frame_a.r_0[2]");
			strList.append(QString::fromStdString(*it) + ".frame_a.r_0[3]");
			strList.append(QString::fromStdString(*it) + ".Alpha");
			strList.append(QString::fromStdString(*it) + ".Beta");
			strList.append(QString::fromStdString(*it) + ".Gamma");
		}
		sheet.data.append(strList);
		compNameToSheet[*it] = qMove(sheet);
	}

	// �����������Ƶ�������������ӳ��
	map<string, int> varNameToidx;
	QString csvFilePath = saveComparePath + "Train/Result.csv";
	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	for (int col = 0; col < allOutputNames.size(); col++)
	{
		string colname = allOutputNames[col].toStdString();
		varNameToidx[colname] = col;
	}

	// ��ȡ.csv�ļ��������������
	int rowsbycsv = 60; // ÿ��csv�ļ�ȡ30������
	for (int i = 0; i < samplesize; i++)
	{
		QString csvFilePath = saveComparePath + "Train/Result" + QString::fromStdString(std::to_string(i)) + ".csv";
		vector<QStringList> AllRows = FileParse::GetDataByNums(csvFilePath, rowsbycsv);
		for (int row = 0; row < AllRows.size(); row++)
		{
			QStringList rowdata = AllRows[row];
			for (auto it = compNameToVarNames.begin(); it != compNameToVarNames.end(); it++) {
				pair<string, vector<string>> nowpair = *it;
				string compname = nowpair.first;
				vector<string> varnames = nowpair.second;
				// ��ͬ�����ϲ�ͬ������
				QStringList strList;
				for (string inputname : InputNames) {
					int colidx = varNameToidx[inputname];
					strList.append(rowdata[colidx]);
				}
				if(varnames.size() == 6) {
					for (string varname : varnames) {
						int colidx = varNameToidx[varname];
						strList.append(rowdata[colidx]);
					}
				}
				else {
					// ��Ҫ������ת���� -> ŷ����
					// �����λ������
					strList.append(rowdata[varNameToidx[compname + ".frame_a.r_0[1]"]]);
					strList.append(rowdata[varNameToidx[compname + ".frame_a.r_0[2]"]]);
					strList.append(rowdata[varNameToidx[compname + ".frame_a.r_0[3]"]]);
					vector<vector<double>> T(4, vector<double>(4, 0.0));
					T[1][1] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[1, 1]"]].toStdString().c_str());
					T[1][2] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[1, 2]"]].toStdString().c_str());
					T[1][3] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[1, 3]"]].toStdString().c_str());
					T[2][1] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[2, 1]"]].toStdString().c_str());
					T[2][2] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[2, 2]"]].toStdString().c_str());
					T[2][3] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[2, 3]"]].toStdString().c_str());
					T[3][1] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[3, 1]"]].toStdString().c_str());
					T[3][2] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[3, 2]"]].toStdString().c_str());
					T[3][3] = stod(rowdata[varNameToidx[compname + ".frame_a.R.T[3, 3]"]].toStdString().c_str());
					vector<double> euler = matrixTToEuler(T);
					strList.append(QString::fromStdString(std::to_string(euler[0])));
					strList.append(QString::fromStdString(std::to_string(euler[1])));
					strList.append(QString::fromStdString(std::to_string(euler[2])));
				}
				compNameToSheet[compname].data.append(strList);
			}
		}
	}

	string newfloder = saveComparePath.toStdString() + "dataset/";
	bool flag = CreateDirectory(stringToLPCWSTR(newfloder), NULL);
	bool issuccess = true;
	// ������ݼ��ļ�
	for (auto it = compNameToSheet.begin(); it != compNameToSheet.end(); it++)
	{
		pair<string, CSV_Parser::Sheet> nowpair = *it;
		string compname = nowpair.first;
		QString dataSetPath = saveComparePath + "dataset/dataset_" + QString::fromStdString(compname) + ".csv";
		bool isnowsuccess = FileParse::generateCSV(dataSetPath, nowpair.second);
		if (!isnowsuccess) {
			issuccess = isnowsuccess;
		}
	}
	return issuccess;
}

void DocumentModel::GenTrainSetFromCSV(const QString& SaveComparePath, int samplesize, int inputsize)
{
	//��ͨ�汾
	//FILE* fp;
	//string fannFilePath = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + ".fann";

	//QString csvFilePath = SaveComparePath + "Train/Result.csv";
	//QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	//int outputsize = allOutputNames.size() - inputsize - 1;   //����������� = ���� - ����������� - Time(1)

	//int rowsbycsv = 30; //ÿ���ļ���ȡ����

	////fopen_s(&fp, "add.fann", "w");
	//fopen_s(&fp, fannFilePath.c_str(), "w");
	//fprintf_s(fp, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
	//// ����ѵ���ļ�
	//for (int i = 0; i < samplesize; i++)
	//{
	//	QString csvFilePath = SaveComparePath + "Train/Result" + QString::fromStdString(std::to_string(i)) + ".csv";
	//	vector<QStringList> AllRows = FileParse::GetDataByNums(csvFilePath, rowsbycsv);
	//	for (int row = 0; row < AllRows.size(); row++)
	//	{
	//		QStringList rowdata = AllRows[row];
	//		for (int j = 1; j < 1 + inputsize; j++)
	//		{
	//			double data = rowdata[j].toDouble();
	//			fprintf_s(fp, "%.5f ", data);
	//		}
	//		fprintf_s(fp, "\n");

	//		for (int k = 1 + inputsize; k < allOutputNames.size(); k++)
	//		{
	//			double data = rowdata[k].toDouble();
	//			fprintf_s(fp, "%.5f ", data);
	//		}
	//		fprintf_s(fp, "\n");
	//	}
	//}
	//fclose(fp);
	
	//�ļ��������ͨ�汾
	//for (int filenum = 3; filenum <= 10; filenum++)
	//{
	//	FILE* fp;
	//	string fannFilePath = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + std::to_string(filenum) + ".fann";

	//	QString csvFilePath = SaveComparePath + "Train500_" + QString::fromStdString(std::to_string(filenum)) + "/Result.csv";
	//	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	//	int outputsize = allOutputNames.size() - inputsize - 1;   //����������� = ���� - ����������� - Time(1)

	//	int rowsbycsv = 30; //ÿ���ļ���ȡ����

	//	//fopen_s(&fp, "add.fann", "w");
	//	fopen_s(&fp, fannFilePath.c_str(), "w");
	//	fprintf_s(fp, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
	//	// ����ѵ���ļ�
	//	for (int i = 0; i < samplesize; i++)
	//	{
	//		QString csvFilePath = SaveComparePath + "Train500_" + QString::fromStdString(std::to_string(filenum)) + "/Result" + QString::fromStdString(std::to_string(i)) + ".csv";
	//		vector<QStringList> AllRows = FileParse::GetDataByNums(csvFilePath, rowsbycsv);
	//		for (int row = 0; row < AllRows.size(); row++)
	//		{
	//			QStringList rowdata = AllRows[row];
	//			for (int j = 1; j < 1 + inputsize; j++)
	//			{
	//				double data = rowdata[j].toDouble();
	//				fprintf_s(fp, "%.5f ", data);
	//			}
	//			fprintf_s(fp, "\n");

	//			for (int k = 1 + inputsize; k < allOutputNames.size(); k++)
	//			{
	//				double data = rowdata[k].toDouble();
	//				fprintf_s(fp, "%.5f ", data);
	//			}
	//			fprintf_s(fp, "\n");
	//		}
	//	}
	//	fclose(fp);
	//}

	//����������ֵ�ѵ���汾����BodyShape8Ϊ����
	//for (int filenum = 1; filenum <= 10; filenum++)
	//{
	//	FILE* fp;
	//	string fannFilePath = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "_BodyShape8_" + std::to_string(filenum) + ".fann";

	//	QString csvFilePath = SaveComparePath + "Train500_" + QString::fromStdString(std::to_string(filenum)) + "/Result.csv";
	//	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	//	int outputsize = 12;
	//	vector<int> outputidx;
	//	for (int i = 0; i < allOutputNames.size(); i++)
	//	{
	//		string nowname = allOutputNames[i].toStdString();
	//		if (nowname.find("myBodyShape8") != string::npos)
	//		{
	//			outputidx.push_back(i);
	//		}
	//	}

	//	int rowsbycsv = 30; //ÿ���ļ���ȡ����

	//	//fopen_s(&fp, "add.fann", "w");
	//	fopen_s(&fp, fannFilePath.c_str(), "w");
	//	fprintf_s(fp, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
	//	// ����ѵ���ļ�
	//	for (int i = 0; i < samplesize; i++)
	//	{
	//		QString csvFilePath = SaveComparePath + "Train500_" + QString::fromStdString(std::to_string(filenum)) + "/Result" + QString::fromStdString(std::to_string(i)) + ".csv";
	//		vector<QStringList> AllRows = FileParse::GetDataByNums(csvFilePath, rowsbycsv);
	//		for (int row = 0; row < AllRows.size(); row++)
	//		{
	//			QStringList rowdata = AllRows[row];
	//			for (int j = 1; j < 1 + inputsize; j++)
	//			{
	//				double data = rowdata[j].toDouble();
	//				fprintf_s(fp, "%.5f ", data);
	//			}
	//			fprintf_s(fp, "\n");

	//			for (int k = 0; k < outputidx.size(); k++)
	//			{
	//				int idx = outputidx[k];
	//				double data = rowdata[idx].toDouble();
	//				fprintf_s(fp, "%.5f ", data);
	//			}
	//			fprintf_s(fp, "\n");
	//		}
	//	}
	//	fclose(fp);
	//}

	//����point���ֵ�ѵ���汾����BodyShape8Ϊ��
	int filenum = 11;
	//for (int filenum = 1; filenum <= 10; filenum++)
	{
		FILE* fpx;
		FILE* fpy;
		FILE* fpz;
		FILE* fpr;
		string fannFilePathx = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "_BodyShape8_pointx_" + std::to_string(filenum) + ".fann";
		string fannFilePathy = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "_BodyShape8_pointy_" + std::to_string(filenum) + ".fann";
		string fannFilePathz = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "_BodyShape8_pointz_" + std::to_string(filenum) + ".fann";
		string fannFilePathr = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "_BodyShape8_pointr_" + std::to_string(filenum) + ".fann";

		QString csvFilePath = SaveComparePath + "Train500_" + QString::fromStdString(std::to_string(filenum)) + "/Result.csv";
		QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
		int outputsize = 3;
		vector<int> outputidx_x;
		vector<int> outputidx_y;
		vector<int> outputidx_z;
		vector<int> outputidx_r;
		for (int i = 0; i < allOutputNames.size(); i++)
		{
			string nowname = allOutputNames[i].toStdString();
			if (nowname.find("myBodyShape8.point_x") != string::npos)
			{
				outputidx_x.push_back(i);
			}
			if (nowname.find("myBodyShape8.point_y") != string::npos)
			{
				outputidx_y.push_back(i);
			}
			if (nowname.find("myBodyShape8.point_z") != string::npos)
			{
				outputidx_z.push_back(i);
			}
			if (nowname.find("myBodyShape8.body.frame_a.r_0") != string::npos)
			{
				outputidx_r.push_back(i);
			}
		}

		int rowsbycsv = 30; //ÿ���ļ���ȡ����


		fopen_s(&fpx, fannFilePathx.c_str(), "w");
		fprintf_s(fpx, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
		fopen_s(&fpy, fannFilePathy.c_str(), "w");
		fprintf_s(fpy, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
		fopen_s(&fpz, fannFilePathz.c_str(), "w");
		fprintf_s(fpz, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
		fopen_s(&fpr, fannFilePathr.c_str(), "w");
		fprintf_s(fpr, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
		// ����ѵ���ļ�
		for (int i = 0; i < samplesize; i++)
		{
			QString csvFilePath = SaveComparePath + "Train500_" + QString::fromStdString(std::to_string(filenum)) + "/Result" + QString::fromStdString(std::to_string(i)) + ".csv";
			vector<QStringList> AllRows = FileParse::GetDataByNums(csvFilePath, rowsbycsv);
			for (int row = 0; row < AllRows.size(); row++)
			{
				QStringList rowdata = AllRows[row];
				for (int j = 1; j < 1 + inputsize; j++)
				{
					double data = rowdata[j].toDouble();
					fprintf_s(fpx, "%.5f ", data);
					fprintf_s(fpy, "%.5f ", data);
					fprintf_s(fpz, "%.5f ", data);
					fprintf_s(fpr, "%.5f ", data);
				}
				fprintf_s(fpx, "\n");
				fprintf_s(fpy, "\n");
				fprintf_s(fpz, "\n");
				fprintf_s(fpr, "\n");

				for (int k = 0; k < outputidx_x.size(); k++)
				{
					int idx = outputidx_x[k];
					double data = rowdata[idx].toDouble();
					fprintf_s(fpx, "%.5f ", data);
				}
				for (int k = 0; k < outputidx_y.size(); k++)
				{
					int idx = outputidx_y[k];
					double data = rowdata[idx].toDouble();
					fprintf_s(fpy, "%.5f ", data);
				}
				for (int k = 0; k < outputidx_z.size(); k++)
				{
					int idx = outputidx_z[k];
					double data = rowdata[idx].toDouble();
					fprintf_s(fpz, "%.5f ", data);
				}
				for (int k = 0; k < outputidx_r.size(); k++)
				{
					int idx = outputidx_r[k];
					double data = rowdata[idx].toDouble();
					fprintf_s(fpr, "%.5f ", data);
				}
				fprintf_s(fpx, "\n");
				fprintf_s(fpy, "\n");
				fprintf_s(fpz, "\n");
				fprintf_s(fpr, "\n");
			}
		}
		fclose(fpx);
		fclose(fpy);
		fclose(fpz);
		fclose(fpr);
	}
}

//���ղ������������
void DocumentModel::GenTrainSetFromCSVLayer(const QString& SaveComparePath, int samplesize)
{
	QString csvFilePath = SaveComparePath + "Train/Result.csv";
	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	int rowsbycsv = 30; //ÿ���ļ���ȡ����
	
	FILE* fp1;
	string fannFilePath1 = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "layer1.fann";
	string inputvar11 = "axis1";
	string inputvar12 = "myBodyShape1";
	string outputvar1 = "myBodyShape2";
	GenLayerSet(fp1, fannFilePath1, inputvar11, inputvar12, outputvar1, allOutputNames, SaveComparePath, samplesize, rowsbycsv);

	FILE* fp2;
	string fannFilePath2 = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "layer2.fann";
	string inputvar21 = "axis2";
	string inputvar22 = "myBodyShape2";
	string outputvar2 = "myBodyShape3";
	GenLayerSet(fp2, fannFilePath2, inputvar21, inputvar22, outputvar2, allOutputNames, SaveComparePath, samplesize, rowsbycsv);

	FILE* fp3;
	string fannFilePath3 = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "layer3.fann";
	string inputvar31 = "axis3";
	string inputvar32 = "myBodyShape3";
	string outputvar3 = "myBodyShape4";
	GenLayerSet(fp3, fannFilePath3, inputvar31, inputvar32, outputvar3, allOutputNames, SaveComparePath, samplesize, rowsbycsv);

	FILE* fp4;
	string fannFilePath4 = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "layer4.fann";
	string inputvar41 = "axis4";
	string inputvar42 = "myBodyShape4";
	string outputvar4 = "myBodyShape5";
	GenLayerSet(fp4, fannFilePath4, inputvar41, inputvar42, outputvar4, allOutputNames, SaveComparePath, samplesize, rowsbycsv);

	FILE* fp5;
	string fannFilePath5 = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "layer5.fann";
	string inputvar51 = "axis5";
	string inputvar52 = "myBodyShape5";
	string outputvar5 = "myBodyShape6";
	GenLayerSet(fp5, fannFilePath5, inputvar51, inputvar52, outputvar5, allOutputNames, SaveComparePath, samplesize, rowsbycsv);

	FILE* fp6;
	string fannFilePath6 = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + "layer6.fann";
	string inputvar61 = "axis6";
	string inputvar62 = "myBodyShape6";
	string outputvar6 = "myBodyShape7";
	GenLayerSet(fp6, fannFilePath6, inputvar61, inputvar62, outputvar6, allOutputNames, SaveComparePath, samplesize, rowsbycsv);
}

void DocumentModel::GenLayerSet(FILE* fp, string fannFilePath, string inputvar1, string inputvar2, string outputvar, const QStringList& allOutputNames, const QString& SaveComparePath, int samplesize, int rowsbycsv)
{
	vector<int> inputcols;
	vector<int> outputcols;
	for (int col = 0; col < allOutputNames.size(); col++)
	{
		string colname = allOutputNames[col].toStdString();
		int idx1 = colname.find(inputvar1);
		int idx2 = colname.find(inputvar2);
		int idx3 = colname.find(outputvar);
		if (idx1 != string::npos || idx2 != string::npos)
		{
			inputcols.push_back(col);
		}
		else if (idx3 != string::npos)
		{
			outputcols.push_back(col);
		}
	}
	fopen_s(&fp, fannFilePath.c_str(), "w");
	fprintf_s(fp, "%d %d %d\n", samplesize * rowsbycsv, inputcols.size(), outputcols.size());
	// ����ѵ���ļ�
	for (int i = 0; i < samplesize; i++)
	{
		QString csvFilePath = SaveComparePath + "Train/Result" + QString::fromStdString(std::to_string(i)) + ".csv";
		vector<QStringList> AllRows = FileParse::GetDataByNums(csvFilePath, rowsbycsv);
		for (int row = 0; row < AllRows.size(); row++)
		{
			QStringList rowdata = AllRows[row];
			for (int idx = 0; idx < inputcols.size(); idx++)
			{
				double data = rowdata[inputcols[idx]].toDouble();
				fprintf_s(fp, "%.5f ", data);
			}
			fprintf_s(fp, "\n");

			for (int idx = 0; idx < outputcols.size(); idx++)
			{
				double data = rowdata[outputcols[idx]].toDouble();
				fprintf_s(fp, "%.5f ", data);
			}
			fprintf_s(fp, "\n");
		}
	}
	fclose(fp);
}

LPCWSTR DocumentModel::stringToLPCWSTR(std::string orig)
{
	size_t origsize = orig.length() + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
	mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);

	return wcstring;
}
