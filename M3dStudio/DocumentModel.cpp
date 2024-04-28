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
	m_pAnimateThread->wait();  //必须等待线程结束
	m_pTwinAniThread->wait();
	delete m_pCompRoot;
	delete m_pAnimateThread;
	delete m_pTwinAniThread;
}

//得做点什么...
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

	//anOcaf_Application->NewDocument("m3dmdl"/*"BinOcaf"*/, m3dOcafDoc);//先缺省，后再改
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

//由组件的文件名和父节点，自动创建组件名
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
			//如果存在，则自动加一
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
	// 生成装配图
	unordered_map<string, string> assembly_graph;
	unordered_map<string, unordered_set<string>> name_map;
	// 考虑以下情况,装配链A->B->C,B相对于A有调姿信息,再让B相对A调姿，此时B相对于C的调姿信息如何处理？如果都对B进行SetDeltaTransform是否会混乱？
	// 处理方法:如果遍历curr_comp的接口发现有相对于某个assembly_comp的调姿信息,那么将该调姿信息保留起来,curr_comp的deltaTransform以last_comp的为准，通过查找保存的哈希表看它的上一个是否有相对于上一个的调姿
	// deltaTransform都是针对上一个装配或者调姿的组件
	unordered_map<string, pair<string, vector<double>>> last_reverse_deltatrans_record; // 如果curr_comp有相对于assembly_comp的调姿,保存下来
	unordered_map<string, gp_Ax3> origin_Ax3_record; // 存储有装配约束接口变换之前的全局gp_Ax3
	for (auto constraint : GetAllConnConstraints())
	{
		assembly_graph[constraint.component1 + '+' + constraint.connector1] = constraint.component2 + '+' + constraint.connector2;
		assembly_graph[constraint.component2 + '+' + constraint.connector2] = constraint.component1 + '+' + constraint.connector1;
		name_map[constraint.component1].emplace(constraint.component1 + '+' + constraint.connector1);
		name_map[constraint.component2].emplace(constraint.component2 + '+' + constraint.connector2);
	}
	// 统计所有需要变换的comp
	unordered_set<string> trans_comp_name;

	//判断每一个组件是否进行过调姿操作,如果进行过调姿,需要判断装配链的装配顺序和调姿的依赖关系
	//存储零件名+接口名
	queue<string> Q;
	unordered_set<string> visited;//判断是否已经处理过
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
			// other_conn的globalAx3
			gp_Ax3 other_globalAx3 = GetCompConnGlobalAx3(curr_comp_name.c_str(), other_conn_name.c_str());
			// 记录
			origin_Ax3_record.emplace(other_name, other_globalAx3);
			// 找到其他装配的接口
			if (other_conn_name != curr_conn_name)
			{
				//找到与该组件相连的组件和接口
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
				// 得到curr_comp的deltaTransv15
				vector<double> curr_comp_v15 = GetCompDeltaTransformV15(curr_comp_name.c_str());
				// 如果curr_comp_v15的前9个值和assembly_comp_conn的gp_Ax3相等,则存在变换,需要给assembly_comp做一个相反的变换,先记录下来
				gp_Ax3 assembly_globalAx3 = GetCompConnGlobalAx3(assembly_comp_name.c_str(), assembly_conn_name.c_str());
				if (fabs(curr_comp_v15[0] - assembly_globalAx3.Location().X()) < LINEAR_TOL && fabs(curr_comp_v15[1] - assembly_globalAx3.Location().Y()) < LINEAR_TOL && fabs(curr_comp_v15[2] - assembly_globalAx3.Location().Z()) < LINEAR_TOL \
					&& fabs(curr_comp_v15[3] - assembly_globalAx3.Direction().X()) < LINEAR_TOL && fabs(curr_comp_v15[4] - assembly_globalAx3.Direction().Y()) < LINEAR_TOL && fabs(curr_comp_v15[5] - assembly_globalAx3.Direction().Z()) < LINEAR_TOL \
					&& fabs(curr_comp_v15[6] - assembly_globalAx3.XDirection().X()) < LINEAR_TOL && fabs(curr_comp_v15[7] - assembly_globalAx3.XDirection().Y()) < LINEAR_TOL && fabs(curr_comp_v15[8] - assembly_globalAx3.XDirection().Z()) < LINEAR_TOL)
				{
					// assembly_comp需要的变换存起来
					last_reverse_deltatrans_record[assembly_comp_name] = { curr_comp_name + '+' + other_conn_name,curr_comp_v15 };
					// curr_comp的deltaTrans清空
					vector<double> reset_v15{ 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
					curr_comp->SetDeltaTransform(reset_v15);
				}
				//加入到队列中
				Q.emplace(assembly_name);
			}
		}

		// 对于curr_comp,如果相对于last_comp+last_conn有调姿变换,则需要更新(last_comp+last_conn直接通过装配图得到)
		if (last_reverse_deltatrans_record.count(curr_comp_name))
		{
			string last_comp_conn_name = last_reverse_deltatrans_record[curr_comp_name].first;
			int last_pos = last_comp_conn_name.find('+');
			string last_comp_name = last_comp_conn_name.substr(0, last_pos);
			string last_conn_name = last_comp_conn_name.substr(last_pos + 1, last_comp_conn_name.length() - last_pos - 1);
			// 得到last_comp_conn当前的globalAx3
			gp_Ax3 last_comp_conn_gAx3_current = GetCompConnGlobalAx3(last_comp_name.c_str(), last_conn_name.c_str());
			// 找到它需要进行变换的上一个装配接口和v15矩阵
			vector<double> v15_pre = last_reverse_deltatrans_record[curr_comp_name].second;
			vector<double> curr_displament = { last_comp_conn_gAx3_current.Location().X(),last_comp_conn_gAx3_current.Location().Y(), last_comp_conn_gAx3_current.Location().Z(), \
				last_comp_conn_gAx3_current.Direction().X(), last_comp_conn_gAx3_current.Direction().Y(), last_comp_conn_gAx3_current.Direction().Z(), \
				last_comp_conn_gAx3_current.XDirection().X(), last_comp_conn_gAx3_current.XDirection().Y(), last_comp_conn_gAx3_current.XDirection().Z() };
			vector<double> new_v15(curr_displament);
			// curr_comp需要相对于last_comp做一个相反的变换
			for (int i = 9; i < 15; ++i)
			{
				new_v15.push_back(-v15_pre[i]);
			}
			curr_comp->SetDeltaTransform(new_v15);

			// 对于curr_comp进行变换
			gp_Ax3 oAx3;
			gp_Trsf oTrans;
			oTrans.SetDisplacement(oAx3, last_comp_conn_gAx3_current);
			// curr_conn局部的反变换
			gp_Trsf tempTrans;
			gp_Ax3 curr_conn_localAx3 = GetCompConnLocalAx3(curr_comp_name.c_str(), curr_conn_name.c_str());
			tempTrans.SetDisplacement(curr_conn_localAx3, oAx3);
			curr_comp->SetOriginTransform(oTrans * tempTrans);
		}
		else
		{
			// last_comp相对curr_comp没有调姿变换
			// 对于curr_comp进行变换
			string last_comp_conn_name = assembly_graph[curr_name];
			int last_pos = last_comp_conn_name.find('+');
			string last_comp_name = last_comp_conn_name.substr(0, last_pos);
			string last_conn_name = last_comp_conn_name.substr(last_pos + 1, last_comp_conn_name.length() - last_pos - 1);
			gp_Ax3 oAx3;
			gp_Trsf oTrans;
			gp_Ax3 last_comp_conn_gAx3_current = GetCompConnGlobalAx3(last_comp_name.c_str(), last_conn_name.c_str());
			oTrans.SetDisplacement(oAx3, last_comp_conn_gAx3_current);
			// curr_conn局部的反变换
			gp_Trsf tempTrans;
			gp_Ax3 curr_conn_localAx3 = GetCompConnLocalAx3(curr_comp_name.c_str(), curr_conn_name.c_str());
			tempTrans.SetDisplacement(curr_conn_localAx3, oAx3);
			curr_comp->SetOriginTransform(oTrans * tempTrans);
			// 如果curr_comp相对于last_comp有调姿信息,需要更新
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
					// 需要更新curr_v15
					curr_v15[0] = new_Ax3.Location().X(), curr_v15[1] = new_Ax3.Location().Y(), curr_v15[2] = new_Ax3.Location().Z();
					curr_v15[3] = new_Ax3.Direction().X(), curr_v15[4] = new_Ax3.Direction().Y(), curr_v15[5] = new_Ax3.Direction().Z();
					curr_v15[6] = new_Ax3.XDirection().X(), curr_v15[7] = new_Ax3.XDirection().Y(), curr_v15[8] = new_Ax3.XDirection().Z();
					// 其余6个姿态信息不需要改变
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
			//删除原有的连接线
			DeleteConnectCurve(*itrCC);
			deleteConncurvesAIS(comp1, conn1, comp2, conn2);
			//生成新的连接线
			generateConncurvesAIS(comp1, conn1, comp2, conn2);
		}
	}
}

void DocumentModel::generateConncurvesAIS(Component* comp1, Connector* connector1, Component* comp2, Connector* connector2)
{
	//构建按照格式要求的字符串
	string curr_name = comp1->GetCompName() + '+' + connector1->GetConnName() + '#' + comp2->GetCompName() + '+' + connector2->GetConnName();

	//建立连接边
	TopoDS_Edge connEdge;
	//得到接口1和接口2的Z轴方向
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
	//取点的时候需要重新考虑,尽量在角点处多取值,多选会出问题？
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
	//存档
	ConnectCurve cc{ ConnectCurveType::single,comp1->GetCompName(),comp2->GetCompName(),connector1->GetConnName(),connector2->GetConnName(),{Loc1,gp_Pnt(X1,Y2,Z1),gp_Pnt(X1,Y2,Z2),Loc2},connEdge };
	//如果是控制总线,左右再显示两个
	//怎么判断?通过判断是不是MainConnector?
	if (connector1 == comp1->GetMainConnector() || connector2 == comp2->GetMainConnector())
	{
		cc.ccType = ConnectCurveType::controlBus;
		generateAuxiliaryConncurvesAIS(comp1, connector1, comp2, connector2);
	}
	AddConnectCurve(cc);
	//显示连接线
	Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(connEdge);
	//加入到哈希表中
	connectcurves_map[curr_name].push_back(shpConn);
	myContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
	myContext->Display(shpConn, Standard_False);
	myContext->UpdateCurrentViewer();
	myContext->ClearSelected(true);
}

void DocumentModel::generateAuxiliaryConncurvesAIS(Component* comp1, Connector* connector1, Component* comp2, Connector* connector2)
{
	//构建按照格式要求的字符串
	string curr_name = comp1->GetCompName() + '+' + connector1->GetConnName() + '#' + comp2->GetCompName() + '+' + connector2->GetConnName();
	//得到接口1和接口2的Z轴方向
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
	//取点的时候需要重新考虑,尽量在角点处多取值,多选会出问题？
	Standard_Real X1 = Loc1.X(), Y1 = Loc1.Y(), Z1 = Loc1.Z();
	Standard_Real X2 = Loc2.X(), Y2 = Loc2.Y(), Z2 = Loc2.Z();
	//左边那一条
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
	//加入到哈希表中
	connectcurves_map[curr_name].push_back(shpConn1);
	//右边那一条
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
	//加入到哈希表中
	connectcurves_map[curr_name].push_back(shpConn2);
}

void DocumentModel::deleteConncurvesAIS(Component* comp1, Connector* conn1, Component* comp2, Connector* conn2)
{
	string curr_name = comp1->GetCompName() + '+' + conn1->GetConnName() + '#' + comp2->GetCompName() + '+' + conn2->GetConnName();
	//从哈希表中找到所有的显示实体
	vector<Handle(AIS_InteractiveObject)> connCurvesAIS = connectcurves_map[curr_name];
	for (auto iter = connCurvesAIS.begin(); iter != connCurvesAIS.end(); ++iter)
	{
		myContext->Erase(*iter, true);
	}
	//从哈希表中删除
	connectcurves_map.erase(curr_name);
	myContext->UpdateCurrentViewer();
}

//装配：缺省的坐标系重合,测试
void DocumentModel::Assemble2Components()
{
	Component* pCom1 = nullptr; //移动到第一个
	Component* pCom2 = nullptr; //移动第二个
	Connector* pConn1, *pConn2;

	gp_Ax3 Ax3From, Ax3To;
	//得到两个选中的实体，是Connector
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

	//得到Connector的Ax3
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
	//记录装配约束
	ConnectConstraint cc;
	cc.component1 = pCom1->GetCompName();
	cc.component2 = pCom2->GetCompName();
	cc.connector1 = pConn1->GetConnName();
	cc.connector2 = pConn2->GetConnName();
	AddConnConstraint(cc);

	// myContext->UpdateCurrentViewer();
}

//插入组件
//重要函数
void DocumentModel::insertComponent()
{
	//打开文件对话框，选择文件 *.m3dcom或m3dmdl
	const QString fileName = QFileDialog::getOpenFileName((QWidget*)parent(), QString(), QString(), "零件(*.m3dcom);;系统(*.m3dmdl)");
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
				QMessageBox::information(NULL, "插入错误", "world组件不能多次插入。");
				return;
			}
		}
		pComChild = pCom_DS->GenerateComponent();
		pCom_DS->SetCompRoot(pComChild);
	}

	//记录最后一个组件的形心
	gp_Pnt ptLast = { 0,0,0 };
	list<Component*> lstComps = m_pCompRoot->GetChilds();
	if (lstComps.size() > 0)
	{
		Component* pLastCom = lstComps.back();
		TopoDS_Shape theLastShp = pLastCom->GetCompound();
		GProp_GProps aGProps;
		BRepGProp::VolumeProperties(theLastShp, aGProps);
		//质心坐标
		ptLast = aGProps.CentreOfMass();
	}

	m_pCompRoot->AddChild(pComChild);

	//产生零部件序号名字，可以重新设置
	string nComponent = GenerateCompName(pComChild);
	//特殊处理，如果是World,则名字为world
	if (pComChild->IsWorld())
	{
		nComponent = "world";
	}
	pComChild->SetCompName(nComponent.c_str());

	//文档操作
	InsertComponent(pComChild);
	if (pComChild->IsWorld())
	{
		ComponentAttrib attrib;
		attrib.bInner = true;
		SetComponentAttrib(nComponent.c_str(), attrib);
	}

	//pComChild->Display(myContext,false);

	//移动一下: 将当前的组件重心与第一个组件重心重合，以免距离太远
	vector<double> v15 = { 0,0,0,0,0,1,1,0,0,0,0,0,0,0,0 };
	gp_Pnt ptCOM = { 0,0,0 };
	vector<Connector*> vConns = pComChild->GetConnectors()/*GetDocument()->GetConnectors()*/;
	if (0) //vConns.size() > 0) ？？？
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

	//刷新显示装配树
	MainWindow* pWnd = (MainWindow*)parent();
	pWnd->UpdateAssemblyTree();
}

//根据mdl_DS的内容，读子装配体、显示
void DocumentModel::UpdateDisplay(bool bRegen)
{
	if (!bRegen)
	{
		UpdateDisplay1();
		return;
	}

	//读文档，创建Component对象m_pCompRoot
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

	//显示m_pCompRoot
	UpdateDisplay1();

	//Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
	////myContext->ResetLocation(aViewCube);
	//gp_Trsf trsf;
	//myContext->SetLocation(aViewCube, trsf);
	//myContext->Display(aViewCube, true);
}

//先创建第一层部件的Compound,记录各个形状-颜色和形状-透明度；记录参考点、线和接口
//显示：显示ColoredShape，显示参考点、参考线和非第一层接口；显示第一层接口
//根据Component的显示实体集刷新显示
//在装配、调姿时，可以不用调此函数；直接对该子组件进行显示或刷新显示即可
void DocumentModel::UpdateDisplay1()
{
	////创建各个层次组件的组合体，并在原始位姿放置
	//m_pCompRoot->GenerateCompound(transf);
	////创建可选择交互对象：第一层组件和接口
	////AIS_ColoredShape：第一层组件（不包括接口）、接口
	//m_pCompRoot->GenerateAISObjects(transf);
	////显示交互对象
	//m_pCompRoot->Display(myContext, transf);

	myContext->RemoveAll(false);

	//对当前装配下的子装配体得到他们的组合体，显示
	list<Component*> lstComps = m_pCompRoot->GetChilds();
	list<Component*>::iterator itr = lstComps.begin();

	//将list转化为vector以便于多线程的访问
	vector<Component*> vecComps;
	for (; itr != lstComps.end(); itr++)
	{
		Component* pCom = *itr;
		vecComps.push_back(pCom);
	}
	vector<Component*>::iterator iter = vecComps.begin();

	//将显示清空
	for (int i = 0; i < vecComps.size(); ++i)
	{
		Component* pCom = *(iter + i);
		vector<Handle(AIS_InteractiveObject)> m_vAIS = pCom->GetAIS();
		for (int i = 0; i < m_vAIS.size(); i++)
		{
			myContext->Remove(m_vAIS[i], false);
		}
	}
	iter = vecComps.begin();//将迭代器再次置于起点


	//读取数据更新显示集
	bool m_bStop = false;
	bool m_bRunning = false;
	int countThread = 0;//记录已经进行的线程总数
	int maxWorkingThread = 4;//最多同时工作的线程总数
	set<UpdateThread*> sThreads;
	//开始执行批处理;
	while (1)
	{

		if (m_bStop)
		{
			break;
		}

		m_bRunning = true;

		//执行一次计算: 当启动的线程数目小于设定的并行数时，启动线程；
		if (sThreads.size() < maxWorkingThread && countThread < vecComps.size())//每次最多启动4个线程;
		{
			//启动更新显示进程;
			UpdateThread* pThread = new UpdateThread();
			sThreads.insert(pThread);
			pThread->SetContext(myContext);
			pThread->SetComponent(*(iter + countThread));
			countThread++;
			pThread->start();
		}


		//判断线程是否执行完毕，将执行完毕的线程取出
		//遍历数据，用迭代器遍历数据
		for (set<UpdateThread*>::iterator iterSet = sThreads.begin(); iterSet != sThreads.end();)
		{
			UpdateThread* pThread = *iterSet;
			if (pThread->m_bStop == true)
			{
				set<UpdateThread*>::iterator itmp = iterSet;//保存要删除的指针
				iterSet++;
				//delete(*itmp);
				sThreads.erase(itmp);
			}
			else
			{
				iterSet++;
			}
		}


		//如果sThreads为空，将m_bStop置为true,退出死循环
		if (sThreads.empty())
		{
			m_bStop = true;
		}
	}

	//最后再显示
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

	//显示连接线
	vector<ConnectCurve> vCurves = GetAllConnectCurves();
	//如果有连接线被修改,需要重新生成连接线的拓扑实体
	auto itrCC = vCurves.begin();
	for (; itrCC != vCurves.end(); ++itrCC)
	{
		ConnectCurve cc = *itrCC;
		TopoDS_Shape theShp = cc.shape;
		Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(theShp);
		//将显示的实体按照命名格式加入到哈希表中
		string curr_name = itrCC->component1 + '+' + itrCC->connector1 + '#' + itrCC->component2 + '+' + itrCC->connector2;
		connectcurves_map[curr_name].push_back(shpConn);
		myContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
		myContext->Display(shpConn, Standard_False);
		//如果是控制总线,需要生成辅助连接线
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

	//刷新显示装配树
	MainWindow* pWnd = (MainWindow*)parent();
	pWnd->UpdateAssemblyTree();
}

void DocumentModel::ReadFile(const char* csfileName)
{
	QString sFileName = csfileName;
	if (QFileInfo(sFileName).suffix().toLower() == "m3dmdl") //装配
	{
		//读文件，创建文档
		DocumentModel* pMdl_DS = DocumentModel::ReadModelFile(csfileName);

		////根据文档，创建顶层组件
		//m_mpFileDC.insert(make_pair(csfileName, pMdl_DS));

		vector<CompBaseInfo> mapComps = pMdl_DS->GetAllCompInfos();
		vector<CompBaseInfo>::iterator itr = mapComps.begin();
		for (; itr != mapComps.end(); ++itr)
		{
			CompBaseInfo comInfo = *itr;
			ReadFile(comInfo.sCompFile.c_str());
		}
	}
	else //零件
	{
		DocumentComponent* pCom_DS = DocumentComponent::ReadComponentFile(csfileName);
		//m_mpFileDC.insert(make_pair(csfileName, pCom_DS));
	}
}
////重要函数
////计划：在Undo/Redo时调用，从文档读取来刷新显示
//void DocumentModel::UpdateDisplay1()
//{
//    //gp_Trsf Trsf; //缺省构造，单位矩阵
//    //m_pCompRoot->Display(myContext, Trsf);
//    myContext->RemoveAll(false);
//    myDisplayList.clear();
//
//    delete m_pCompRoot; //先清除掉
//    m_pCompRoot = GenerateComponent(myfileName.toStdString().c_str());
//    string sCompName = QFileInfo(myfileName).baseName().toStdString();
//    if (sCompName.empty())
//    {
//        sCompName = "Untitled";
//    }
//    m_pCompRoot->SetCompName(sCompName.c_str());
//    //SetCompRoot(m_pCompRoot);
//
//    //对当前装配下的子装配体得到他们的组合体，显示
//    list<Component*> lstComps = m_pCompRoot->GetChilds();
//    list<Component*>::iterator itr = lstComps.begin();
//    for (; itr != lstComps.end(); ++itr)
//    {
//        Component* pCom = *itr;
//        vector<ShapeMaterial> shpMats; //形状的材料;
//        vector<ShapeTransparency> shpTransparencys; //形状的透明度;
//        vector<ShapeColor> shpColors;//形状的颜色；
//        vector<TopoDS_Shape> shpDatumPts; //变换后的参考点
//        vector<TopoDS_Shape> shpDatumLns; //变换后的参考线
//        vector<TopoDS_Shape> shpConnectos; //变换后的接口
//        vector<gp_Ax3> coordSys; //变换后的接口坐标系统显示
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
//        //显示材料;
//        for (int i = 0; i < shpMats.size(); i++)
//        {
//            TopoDS_Shape theShp = shpMats[i].theShape;
//            Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(theShp);
//            myContext->Display(theAIS, false);
//            myContext->SetMaterial(theAIS, shpMats[i].theMaterial, false);
//        }
//        //显示透明度
//        for (int i = 0; i < shpTransparencys.size(); i++)
//        {
//            TopoDS_Shape theShp = shpTransparencys[i].theShape;
//            Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(theShp);
//            myContext->Display(theAIS, false);
//            myContext->SetTransparency(theAIS, shpTransparencys[i].transparent, false);
//        }
//        //显示颜色；
//        for (int i = 0; i < shpColors.size(); i++)
//        {
//            TopoDS_Shape theShp = shpColors[i].theShape;
//            Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(theShp);
//            myContext->Display(theAIS, false);
//            myContext->SetColor(theAIS, shpColors[i].theColor, false);
//        }
//        //显示参考点
//        for (int i = 0; i < shpDatumPts.size(); i++)
//        {
//            TopoDS_Shape theShp = shpDatumPts[i];
//            Handle(AIS_InteractiveObject) datPoint = new AIS_Shape(theShp);
//            myContext->SetColor(datPoint, Quantity_NOC_YELLOW, Standard_True);
//            myContext->Display(datPoint, Standard_True);
//        }
//        //显示参考线
//        for (int i = 0; i < shpDatumLns.size(); i++)
//        {
//            TopoDS_Shape theShp = shpDatumLns[i];
//            Handle(AIS_InteractiveObject) datLine = new AIS_Shape(theShp);
//            Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_YELLOW, Aspect_TOL_DOTDASH, 1);
//            datLine->Attributes()->SetWireAspect(lineAspect);
//            myContext->Display(datLine, Standard_True);
//        }
//        //显示接口
//        for (int i = 0; i < shpConnectos.size(); i++)
//        {
//            TopoDS_Shape theShp = shpConnectos[i];
//            Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(theShp);
//            myContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_True);
//            myContext->Display(shpConn, Standard_True);
//        }
//        //显示接口坐标系统
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
//    //显示连接线
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
//    //刷新显示装配树
//    MainWindow* pWnd = (MainWindow*)parent();
//    pWnd->UpdateAssemblyTree();
//}

bool DocumentModel::saveFile(const QString gfileName)
{
	if (!DocumentCommon::saveFile(gfileName))
	{
		return false;
	}

	//设置组件的名字
	m_pCompRoot->SetCompName(QFileInfo(myfileName).baseName().toStdString().c_str());

	//刷新显示装配树
	MainWindow* pWnd = (MainWindow*)parent();
	pWnd->UpdateAssemblyTree();

	////增加映射表
	//m_mpFileDC.insert(make_pair(myfileName.toStdString(), this));

	return true;
}

//读文档的内容，创建组件树
//只构建树形关系: 组件名，组件所属文档，子组件
Component* DocumentModel::GenerateComponent(const QString& currPath/*DocumentCommon* pDoc*/)
{
	Component* pComRet = nullptr;

	if (1)//pDoc->IsModel()) //装配
	{
		DocumentModel* pMdl_DS = this;// (DocumentModel*)pDoc;
		////根据文档，创建顶层组件
		//pComRet = pMdl_DS->GenerateTopComponent(); //每个子组件的信息不在
		pComRet = new Component(this);// pDoc);

		vector<CompBaseInfo> mapComps = pMdl_DS->GetAllCompInfos();
		vector<CompBaseInfo>::iterator itr2 = mapComps.begin();
		for (; itr2 != mapComps.end(); ++itr2)
		{
			CompBaseInfo comInfo = *itr2;
			QString sFileName = comInfo.sCompFile.c_str();
			if (!QFileInfo(sFileName).exists())
			{
				//提示文件不存在，在当前路径下搜索
				//QMessageBox::information(NULL, "模型文件未找到", sFileName, QMessageBox::Ok);
				
				sFileName = currPath + tr("/") +  QFileInfo(sFileName).fileName();
				if (!QFileInfo(sFileName).exists())
				{
					QMessageBox::information(NULL, "模型文件丢失错误", "同一路径下也未找到！", QMessageBox::Ok);
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
	else //零件
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

//读文件，创建Component组件树
////重要函数
//Component* DocumentModel::GenerateComponent(const char* csFileName)
//{
//    Component* pComRet = nullptr;
//
//    QString sFileName = csFileName;  
//    if (sFileName.isEmpty() || QFileInfo(sFileName).suffix().toLower() == "m3dmdl") //装配
//    {
//        DocumentModel* pMdl_DS;
//        if (sFileName.isEmpty())
//        {
//            pMdl_DS = this;
//        }
//        else {
//            //读文件，创建文档
//            map<string, DocumentCommon*>::iterator itr = m_mpFileDC.find(csFileName);
//            assert(itr != m_mpFileDC.end());
//            pMdl_DS = (DocumentModel*)itr->second; //DocumentModel::ReadFile(csFileName);
//        }
//        //根据文档，创建顶层组件
//        pComRet = pMdl_DS->GenerateTopComponent(); //每个子组件的信息不在
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
//    else //零件
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

//隐藏选中组件
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

			//隐藏零件
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
				//其余情况不允许删除
				myContext->NextSelected();
				continue;
			}
		}
	}

	return true;
}

//删除选中物体
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

			//删除零件
			Component* pCom = m_pCompRoot->GetChild(sName.c_str());
			if (pCom)
			{
				//从文档删除
				DeleteComponent(pCom);

				//从ComRoot删除
				m_pCompRoot->DeleteChild(sName.c_str());

				myContext->NextSelected();
				//myContext->Erase(obj, true);
			}
			//删除连接线
			else if (sName.substr(0, 8) == "connect(")
			{
				//从ComRoot删除

				//从文档删除
				ConnectCurve* pCurve = GetConnectCurve(shape);
				DeleteConnectCurve(*pCurve);
				delete pCurve;

				myContext->NextSelected();
			}
			else {
				//其余情况不允许删除
				myContext->NextSelected();
				continue;
			}
		}
	}

	UpdateDisplay1();//不重新读文件

	return true;
}

//建立两个组件的连接
void DocumentModel::Connect2Components()
{
	//得到两个选中的实体，是Connector
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

	//建立连接边,画Bezier曲线
	TopoDS_Edge connEdge;
	gp_Pnt P1 = pConn1->GetCenter();
	gp_Pnt P2 = pConn2->GetCenter();
	TColgp_Array1OfPnt array(1, 2);
	array.SetValue(1, P1);
	array.SetValue(2, P2);
	Handle(Geom_BezierCurve) curve = new Geom_BezierCurve(array);
	BRepBuilderAPI_MakeEdge ME(curve);
	connEdge = ME;

	//记录到文档
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

	//先得到选中的组件
	if (!myContext->NbSelected())
	{
		pAssemWnd->HighLightItem("");
		return;
	}	

	myContext->InitSelected();//只处理一个吧
	Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
	if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
	{
		TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
		string sName = GetShapeName(shape);

		//零件
		Component* pCom = m_pCompRoot->GetChild(sName.c_str());
		if (pCom)
		{
			//亮显该Item，在装配树上
			pAssemWnd->HighLightItem(sName.c_str());
		}
	}
}

//组件改名
void DocumentModel::RenameComponent(const QString& oldName, const QString& newName)
{
	//组件部分
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//名字属性
		Handle(TDataStd_Name) nameAttr;
		theLabel.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCompNameI = ExtString2string(s);

		if (oldName == sCompNameI.c_str())
		{//更改文档中组件名字
			theLabel.ForgetAttribute(nameAttr);
			TDataStd_Name::Set(theLabel, newName.toStdString().c_str());
			break;
		}
	}

	//接口（装配）约束部分
	TDF_Label connLabel = m_pDocRoot.FindChild(2, false);
	if (!connLabel.IsNull())
	{
		Standard_Integer ncount = connLabel.NbChildren();
		for (int i = 1; i <= ncount; i++)
		{
			ConnectConstraint cc;
			TDF_Label lbl_i = connLabel.FindChild(i, false);

			//类型
			Handle(TDataStd_Integer) iType;
			lbl_i.FindAttribute(TDataStd_Integer::GetID(), iType);
			if (iType.IsNull())
			{
				continue;
			}
			cc.kpType = (Kinematic_pair_type)iType->Get();
			//全名
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
				//重新组织全名
				string ccName = Make1Name(c1, c2, conn1, conn2);
				lbl_i.ForgetAttribute(nameAttr);
				TDataStd_Name::Set(lbl_i, ccName.c_str());
			}
		}
	}

	//连接线部分
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

			//名字
			Handle(TDataStd_Name) nameAttr;
			lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
			if (nameAttr.IsNull())
			{
				continue;
			}
			TCollection_ExtendedString s = nameAttr->Get();
			string sCurveName = ExtString2string(s);
			//各个名字
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
				//重新组织全名
				string ccName = Make1Name(sCom1, sCom2, sConn1, sConn2);
				lbl_i.ForgetAttribute(nameAttr);
				TDataStd_Name::Set(lbl_i, ccName.c_str());
			}
		}
	}

	//参数设置部分
	TDF_Label parLabel = m_pDocRoot.FindChild(4, false);
	if (!parLabel.IsNull())
	{
		Standard_Integer ncount = parLabel.NbChildren();
		for (int i = 1; i <= ncount; i++)
		{
			TDF_Label lbl_i = parLabel.FindChild(i, false);

			//映射名字
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
		//myContext->Hilight(ais0, true);//高亮显示了，不能删除
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

//调试
void DocumentModel::adjustComponent0()
{
	//对选中的组件，以第一个接口坐标系的Z轴旋转30度
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

	//旋转
	DocumentCommon* pDoc = pCom->GetDocument();
	vector<Connector*> vConn = pDoc->GetConnectors();
	if (vConn.size() == 0) return;

	//对第一个接口
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

	//下面旋转组件
	vector<double> v15(15);
	v15[0] = ax3.Location().X(); v15[1] = ax3.Location().Y(); v15[2] = ax3.Location().Z();
	v15[3] = ax3.Direction().X(); v15[4] = ax3.Direction().Y(); v15[5] = ax3.Direction().Z();
	v15[6] = ax3.XDirection().X(); v15[7] = ax3.XDirection().Y(); v15[8] = ax3.XDirection().Z();
	v15[9] = 0; v15[10] = 0; v15[11] = 0;
	v15[12] = 0; v15[13] = 0; v15[14] = 30 * PI * i++ / 180;
	pCom->SetDeltaTransform(v15);
	//调试
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

//测试
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

	//调整选中的组件shape
	Handle(AIS_myManipulator) aManipulator = new AIS_myManipulator();
	//禁用平移
	//aManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Translation, Standard_False);
	//aManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Translation, Standard_False);
	//aManipulator->SetPart(2, AIS_ManipulatorMode::AIS_MM_Translation, Standard_False);
	//禁用scaling
	aManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);
	aManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);
	aManipulator->SetPart(2, AIS_ManipulatorMode::AIS_MM_Scaling, Standard_False);

	//禁用旋转
	//aManipulator->SetPart(0, AIS_ManipulatorMode::AIS_MM_Rotation, Standard_False);
	//aManipulator->SetPart(1, AIS_ManipulatorMode::AIS_MM_Rotation, Standard_False);
	aManipulator->Attach(aisObj);

	//启用
	aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Translation);
	aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Rotation);
	//aManipulator->EnableMode(AIS_ManipulatorMode::AIS_MM_Scaling);

	//激活
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
////由文档的内容，建立一个顶层组件Component对象，不递归
//Component* DocumentModel::GenerateTopComponent()
//{
//    Component* pComRet = new Component(this);
//
//    //组件的各个子组件：文件名、组件名、位姿
//    //组件信息在子节点中，此处不处理...
//    m_mapCompInfo = GetAllCompInfos(); //读文档
//    //pComRet->SetComponentsInfo(m_mapCompInfo);
//
//    ////组件的接口约束
//    //m_lstConnectConstraints = GetAllConnConstraints();
//    ////pComRet->SetCompConnConstraints(m_lstConnectConstraints);
//
//    ////组件的连接线
//    //m_lstConnectCurves = GetAllConnectCurves();
//    ////pComRet->SetCompConnections(m_lstConnectCurves);
//
//    ////参数设置
//    //m_lstParamValues = GetAllParameterValues();
//    ////pComRet->SetParamterValues(m_lstParamValues);
//
//    return pComRet;
//}

//插入组件到文档
//位姿信息缺省的原位置
//此处只存储组件的：名字、文件全名、位姿
bool DocumentModel::InsertComponent(Component* pCom)
{
	SetModified();

	//组件的节点序号为 1
	TDF_Label compLabel = m_pDocRoot.FindChild(1);
	//组件的个数！！！注意：可能包含是删除的，也可能是Undo无效的
	//判断一个节点是否有效，用第一个属性是否获得成功来判断
	int nCount = compLabel.NbChildren();

	//新建一个Label
	TDF_Label theLabel = compLabel.FindChild(nCount + 1);

	//写入名字属性
	TDataStd_Name::Set(theLabel, pCom->GetCompName().c_str());

	//写入文件路径
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(theLabel, 1, 1);
	aStrArray->SetValue(1, pCom->GetCompFile().toStdU16String().c_str());

	//写入位姿
	Handle(TDataStd_RealArray) aRealArray = TDataStd_RealArray::Set(theLabel, 1, 12);
	gp_Trsf trsnf;// = pCom->GetOriginTransform(); //插入的组件，缺省在单位矩阵，不变换;
	vector<double> v12 = GetTransformValues12(trsnf);
	for (int i = 1; i <= 12; i++)
	{
		aRealArray->SetValue(i, v12[i - 1]);
	}

	////记录一下
	//CompBaseInfo ci;
	//ci.sCompName = pCom->GetCompName();
	//ci.sCompFile = pCom->GetCompFile().toStdString();
	//ci.vCompDisplacement = pCom->GetDisplacement();
	//m_mapCompInfo.insert(make_pair(ci.sCompName, ci));

	return true;
}

//从文档删除组件
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
			//只能忘掉，没有删除节点的接口函数
			theLabel.ForgetAttribute(TDataStd_Name::GetID());
		}
	}

	////删除组件信息，保证信息一致性
	//m_mapCompInfo.erase(pCom->GetCompName().c_str());

	//下面删除与组件相关的连接线
	DeleteCompConnectCurves(pCom->GetCompName().c_str());

	//删除装配约束
	DeleteCompConnConstraits(pCom->GetCompName().c_str());

	//删除参数表信息
	DeleteCompParameterValues(pCom->GetCompName().c_str());
}

//删除与组件相连的连接
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

//删除与组件相连的装配约束
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

//删除与组件相关的参数
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
//只获取第一层的组件信息
//组件名，<组件名，组件文件名，组件的变换矩阵>
vector<CompBaseInfo> DocumentModel::GetAllCompInfos() const
{
	vector<CompBaseInfo> mapRet;
	//组件的节点序号为 1
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	if (compLabel.IsNull())
	{
		return mapRet;
	}

	//组件的个数！！！注意：可能包含是删除的，也可能是Undo无效的
	//判断一个节点是否有效，用第一个属性是否获得成功来判断
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

		//记一下
		CompBaseInfo ci;
		ci.sCompName = sCompName;
		ci.sCompFile = sCompFileName;
		ci.v18Transf = vDisp18;
		mapRet.push_back(ci);
	}

	return mapRet;
}

//装配下获得实体名字
//只关注：第一层零部件的名字，第一层零部件的接口全名
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
		{//找接口
			for (itr = comChilds.begin(); itr != comChilds.end(); ++itr)
			{
				Component* pComChild = *itr;
				//如果是子装配体，则获取其接口类零件的主接口
				if (pComChild->IsAssembly())
				{
					list<Component*> lstSubComps = pComChild->GetChilds();
					auto itrSub = lstSubComps.begin();
					for (; itrSub != lstSubComps.end(); ++itrSub)
					{
						Component* pSubChild = *itrSub;
						if (!pSubChild->IsAssembly() && pSubChild->IsConnector())
						{
							//获得接口类零件的接口
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
			//下面找连接线,此处从文档获得的，有点别扭？？？？
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

//位姿
void DocumentModel::SetCompOriginTransform(const char* sCompName, gp_Trsf transf)
{
	TDF_Label compLabel = m_pDocRoot.FindChild(1, false);
	//组件的个数！！！注意：可能包含是删除的，也可能是Undo无效的
	//判断一个节点是否有效，用第一个属性是否获得成功来判断
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//写入名字属性
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
			//位姿
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
		//写入名字属性
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

//用8个整数存储
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
		//写入名字属性
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
		//写入名字属性
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
			//相对位姿
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
	//组件的个数！！！注意：可能包含是删除的，也可能是Undo无效的
	//判断一个节点是否有效，用第一个属性是否获得成功来判断
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//写入名字属性
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
			//位姿
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
	//组件的个数！！！注意：可能包含是删除的，也可能是Undo无效的
	//判断一个节点是否有效，用第一个属性是否获得成功来判断
	int nCount = compLabel.NbChildren();

	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label theLabel = compLabel.FindChild(i, false);
		//写入名字属性
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
			//相对位姿
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
//    //组件的个数！！！注意：可能包含是删除的，也可能是Undo无效的
//    //判断一个节点是否有效，用第一个属性是否获得成功来判断
//    int nCount = compLabel.NbChildren();
//
//    for (int i = 1; i <= nCount; i++)
//    {
//        TDF_Label theLabel = compLabel.FindChild(i, false);
//        //写入名字属性
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
//            //位姿
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
///接口约束
/// 
//删除一接口约束
void DocumentModel::DeleteConnConstraint(ConnectConstraint& cc)
{
	//装配约束的节点序号为 2
	TDF_Label connLabel = m_pDocRoot.FindChild(2, false);
	if (connLabel.IsNull())
	{
		return;
	}

	Standard_Integer ncount = connLabel.NbChildren();
	for (int i = 1; i <= ncount; i++)
	{
		TDF_Label lbl_i = connLabel.FindChild(i, false);

		//名字
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		//合成一个名字 A1.p1-A2.p1
		string sNameOne = Make1Name(cc.component1, cc.component2, cc.connector1, cc.connector2);
		if (sCurveName == sNameOne)
		{
			lbl_i.ForgetAllAttributes(true);
			break;
		}
	}
}

//增加接口约束
void DocumentModel::AddConnConstraint(ConnectConstraint& cc)
{
	//接口约束的节点序号为 2
	TDF_Label connLabel = m_pDocRoot.FindChild(2);
	int nCount = connLabel.NbChildren();

	//新建一个Label
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
		//类型
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

	//类型
	TDataStd_Integer::Set(theLabel, cc.kpType);

	//写入名字属性 A1.p1-A2.p2
	string ccName = Make1Name(cc.component1, cc.component2, cc.connector1, cc.connector2);
	TDataStd_Name::Set(theLabel, ccName.c_str());

	//nCount = connLabel.NbChildren();
}

//设置全部接口约束，在接口约束界面“更新”时调用
void DocumentModel::SetConnConstraints(vector<ConnectConstraint>& vConnConstrs)
{
	//接口约束的节点序号为 2
	TDF_Label connLabel = m_pDocRoot.FindChild(2);
	//先清除子节点点属性
	connLabel.ForgetAllAttributes(true);

	for (int i = 0; i < vConnConstrs.size(); i++)
	{
		AddConnConstraint(vConnConstrs[i]);
	}
}

//获得全部的接口约束
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
		//类型
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
		{//避免重复;
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

//得到刚性连接的组件全名
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
///连接线
/// 
/// 增加连接线
void DocumentModel::AddConnectCurve(ConnectCurve& pCurve) //增加一条连接线
{
	//lianjie的节点序号为 3
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
		//新建一个Label
		theLabel = connLabel.FindChild(nCount + 1);
	}

	//类型
	TDataStd_Integer::Set(theLabel, pCurve.ccType);

	//写入名字属性 A1.p1-A2.p2
	string ccName = Make1Name(pCurve.component1, pCurve.component2, pCurve.connector1, pCurve.connector2);
	TDataStd_Name::Set(theLabel, ccName.c_str());

	//写几何
	TNaming_Builder B_i(theLabel);
	TopoDS_Shape connShape = pCurve.shape;
	B_i.Generated(connShape);

	//写入点系列
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

//拾取一条连接线
ConnectCurve* DocumentModel::GetConnectCurve(const TopoDS_Shape& theShape) const
{
	ConnectCurve* ccRet = nullptr;

	//lianjie的节点序号为 3
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

		//名字
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		//几何
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
			//类型
			Handle(TDataStd_Integer) iType;
			lbl_i.FindAttribute(TDataStd_Integer::GetID(), iType);
			ccRet->ccType = (ConnectCurveType)iType->Get();

			//各个名字
			string sCom1, sCom2, sConn1, sConn2;
			Make4Names(sCurveName, sCom1, sCom2, sConn1, sConn2);
			ccRet->component1 = sCom1;
			ccRet->component2 = sCom2;
			ccRet->connector1 = sConn1;
			ccRet->connector2 = sConn2;
			//点
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

//删除一条连接线
void DocumentModel::DeleteConnectCurve(ConnectCurve& cc)
{
	//lianjie的节点序号为 3
	TDF_Label connLabel = m_pDocRoot.FindChild(3, false);
	if (connLabel.IsNull())
	{
		return;
	}

	Standard_Integer ncount = connLabel.NbChildren();
	for (int i = 1; i <= ncount; i++)
	{
		TDF_Label lbl_i = connLabel.FindChild(i, false);

		//名字
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		//合成一个名字 A1.p1-A2.p1
		string sNameOne = Make1Name(cc.component1, cc.component2, cc.connector1, cc.connector2);
		if (sCurveName == sNameOne)
		{
			lbl_i.ForgetAllAttributes(true);
			break;
		}
	}
}

//获得全部的接口约束
vector<ConnectCurve> DocumentModel::GetAllConnectCurves() const
{
	vector<ConnectCurve> vRet;

	//lianjie的节点序号为 3
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

		//名字
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sCurveName = ExtString2string(s);

		//几何
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
			//类型
			Handle(TDataStd_Integer) iType;
			lbl_i.FindAttribute(TDataStd_Integer::GetID(), iType);
			cc.ccType = (ConnectCurveType)iType->Get();

			//各个名字
			string sCom1, sCom2, sConn1, sConn2;
			Make4Names(sCurveName, sCom1, sCom2, sConn1, sConn2);
			cc.component1 = sCom1;
			cc.component2 = sCom2;
			cc.connector1 = sConn1;
			cc.connector2 = sConn2;
			//点
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
///参数设置：映射参数名是关键字
///
/// 增加一个参数
void DocumentModel::AddParameterValue(ParameterValue& pv, int iLabel)
{
	//参数的节点序号为 4
	TDF_Label connLabel = m_pDocRoot.FindChild(4);
	int nCount = connLabel.NbChildren();

	//新建一个Label
	int iLab = iLabel;
	if (iLab == 0)
	{
		iLab = nCount + 1;
	}
	TDF_Label theLabel = connLabel.FindChild(iLab);

	//参数映射组件的参数名
	TDataStd_Name::Set(theLabel, pv.sMapParamName.c_str());

	//参数名 和 参数值
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(theLabel, 1, 2);
	aStrArray->SetValue(1, pv.sParamName.c_str());
	aStrArray->SetValue(2, pv.sParamValue.c_str());
	//nCount = connLabel.NbChildren();
}

//删除一个参数
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

		//名字
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

//设置全部参数
void DocumentModel::SetParameterValues(vector<ParameterValue>& vPVs)
{
	auto itr = vPVs.begin();

	//参数的节点序号为 4
	TDF_Label parLabel = m_pDocRoot.FindChild(4,false);
	if (!parLabel.IsNull())
	{//先清空全部参数值设置
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

//根据参数名，获得参数表中的参数值信息
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

//获得全部参数
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
		//映射名字
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		TCollection_ExtendedString s = nameAttr->Get();
		string sMapName = ExtString2string(s);

		//参数名和值
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
	//获得装配体的接口
	vector<Connector*> vConnectors;

	//获得当前部件
	list<Component*> lstChilds = m_pCompRoot->GetChilds();
	auto itr = lstChilds.begin();
	for (; itr != lstChilds.end(); ++itr)
	{
		Component* pComp = *itr;

		if (pComp->IsConnector())
		{
			//得到接口零件的主接口
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
	
	//得到组件Frame_a接口Ax3
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

	//Ax3在全局坐标系下
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

	//得到组件Frame_a接口Ax3
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

	//Ax3在全局坐标系下
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

	//得到组件Frame_a接口Ax3
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

	//Ax3在全局坐标系下
	vector<double> v9 = pConn->GetDisplacement();
	gAx3 = MakeAx3(v9);
	gAx3.Transform(delTrsf * orTrsf);
	delete pConn;

	return gAx3;
}

//自动计算参数值
//vector<string> vNames = { "r","r_CM","r_ab","r_ba","n","nX","n_X","nY","n_Y","nZ","n_Z" };
QString DocumentModel::ReComputeParamValue(const QString& sComName, const QString& sComParName)
{
	QString theValue;

	//先得到组件参数
	QString sMapParName = sComName + "." + sComParName;
	myParameter* theParam = GetParameterInfo(sMapParName.toStdString());
	assert(theParam);
	//得到组件的变换矩阵
	gp_Ax3 gAx3 = GetCompConnGlobalAx3(sComName.toStdString().c_str(), "frame_a");

	//将参数的坐标值变换
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

//模型检查
bool DocumentModel::InitCheckModel()
{
	vector<ConnectCurve> vCCs = GetAllConnectCurves();//连接线
	vector<ConnectConstraint> vCCt = GetAllConnConstraints();//接口约束
	set<string> sConnected;//已连接接口名称的集合
	set<string> sConnectors;//所有接口的集合
	//1.获得连接线、接口约束连接的所有接口名称集合sConnected
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
	//2.获得待连接的所有接口名称集合sConnectors
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
		//QMessageBox::information(NULL, "提示", "所有接口已连接，可以仿真！", QMessageBox::Ok);
		return true;
	}
	else if (sConnected.size() > sConnectors.size())
	{
		//QMessageBox::information(NULL, "错误", "未知错误！已连接接口数大于所有接口数目", QMessageBox::Ok);
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
		string ReminderMessage = "接口" + NotConnected.substr(0, length) + "未连接！";
		QMessageBox::information(NULL, "提示", QString::fromStdString(ReminderMessage), QMessageBox::Ok);
		return true;
	}

}

//关闭仿真
void DocumentModel::CloseSimulate()
{
	//1.恢复建模界面

	//2.恢复部件显示与选择模式
	UpdateDisplay1();
}

//模型仿真
bool DocumentModel::SimulateModel()
{
	//先判断结果文件(*.csv)生成时间与当前模型(*.m3dmdl)生成时间哪一个更新，以此判断是否进行仿真
	//QString file = "E:/workspace/Data/ColorTransfer.txt";
	//QFileInfo info(file);
	//qint64 size = info.size();//文件大小,单位byte
	//QDateTime created = info.created();//创建时间
	//QDateTime lastModified = info.lastModified();//最后修改时间
	//QDateTime lastRead = info.lastRead();//最后读取时间
	//qDebug() << size;
	//qDebug() << created;
	//qDebug() << lastModified;
	//qDebug() << lastRead;
	//bool isDir = info.isDir();//是否是目录
	//bool isFile = info.isFile();//是否是文件
	//bool isSymLink = info.isSymLink();//是否是符号链接
	//bool isHidden = info.isHidden();//是否隐藏
	//bool isReadable = info.isReadable();//是否可读
	//bool isWritable = info.isWritable();//是否可写
	//bool isExecutable = info.isExecutable();//是否是可执行文件
	//qDebug() << isDir << isFile << isSymLink << isHidden <<
	//	isReadable << isWritable << isExecutable;

	//获取结果文件信息
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
		string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//对不同模型有不同文件夹的相对路径
		string sresultfile = saveCompareRaw + "Result.csv";
		qsresultfile = QString::fromStdString(sresultfile);
	}
	QFileInfo resultinfo(qsresultfile);
	QDateTime resultcreated = resultinfo.lastModified();//最后修改时间

	//获取模型文件信息
	QString qsmodelfile = GetFileName();
	QFileInfo modelinfo(qsmodelfile);
	QDateTime modellastModified = modelinfo.lastModified();//最后修改时间
	
	uint retime = resultcreated.toTime_t();
	uint motime = modellastModified.toTime_t();
	int tRet = retime - motime;

	bool isSimulate = false;              //判断是否进行了仿真

	MainWindow* pMain = (MainWindow*)parent();
	AnimationWidget* m_AniWid = pMain->GetAnimationWidget();
	if (tRet < 0 || resultcreated.isNull())
	{
		isSimulate = true;
		//1.检查模型是否完整，不做语法检查
		if (!InitCheckModel())
		{
			return false;
		}

		//2.生成Modelica代码和mo文件
		string sCode;
		if (!GenerateModelicaCode(sCode, 0))
		{
			QMessageBox::information(NULL, "错误", "代码生成错误");
			return false;
		}
		//将代码写入文件，在工作路径下
		QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";//对不同模型分别建立文件夹的相对路径
		bool flag = CreateDirectory(saveCompareRaw.toStdWString().c_str(), NULL);
		QString sFilePath = saveCompareRaw;
		sFilePath += m_pCompRoot->GetCompName().c_str();
		sFilePath += ".mo";
		QFile moFile(sFilePath);
		if (!moFile.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			QMessageBox::information(NULL, "错误", "mo文件存储错误");
			return false;
		}
		QTextStream txtOutput(&moFile);
		txtOutput << sCode.c_str() << endl;
		moFile.close();

		//3.生成仿真mos脚本文件(openmodelica)/py脚本文件(MWorks)
		GenerateMosFile(sFilePath);

		//4.调用求解器，执行mos文件，检查结果文件生成
		//调用求解器之前先清除AnimationWidget的消息框
		m_AniWid->ClearMessage();
		CallSolver(saveCompareRaw);

		//5.切换至仿真后处理界面: 左边参数和变量，可新建曲线窗口，动画工具条
	}

	//6.将零部件打散，成为各个单独的显示体，便于驱动动画
	UpdateDisplay2();

	//界面信息的显示：先判断是否进行了仿真，再判断仿真是否成功
	if (isSimulate == false) {
		//如果没有进行仿真，显示信息：仿真结果最新，自动加载关注变量和关注参数
		m_AniWid->ShowPostMessage(false, true);
	}

	return true;
}

//设置后处理信息
void DocumentModel::SetPostMessage()
{
	bool isSimulateSuccess = false;       //如果进行了仿真，判断仿真是否成功，通过检查结果文件的生成来判断
	
	//获取结果文件信息
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
		string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//对不同模型有不同文件夹的相对路径
		string sresultfile = saveCompareRaw + "Result.csv";
		qsresultfile = QString::fromStdString(sresultfile);
	}
	QFileInfo resultinfo(qsresultfile);
	QDateTime resultcreated = resultinfo.lastModified();//最后修改时间

	//获取模型文件信息
	QString qsmodelfile = GetFileName();
	QFileInfo modelinfo(qsmodelfile);
	QDateTime modellastModified = modelinfo.lastModified();//最后修改时间

	uint retime = resultcreated.toTime_t();
	uint motime = modellastModified.toTime_t();
	int tRet = retime - motime;

	
	//检查结果文件的生成
	if (resultinfo.isFile()) {
		if (tRet > 0) {
			isSimulateSuccess = true;
		}
	}

	//进行了仿真，如果仿真成功，加载关注的变量和参数
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
	QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";//对不同模型分别建立文件夹的相对路径
	QString sFilePath = saveCompareRaw;
	sFilePath += m_pCompRoot->GetCompName().c_str();
	sFilePath += ".mo";

	//1.生成重新仿真的mos文件
	GenerateReMosFile(sFilePath);

	//2.调用求解器，执行mos文件
	ReCallSolver(saveCompareRaw);

	//3.将零部件打散
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

		//对各个参数值进行设置（这段删去也不影响程序，单纯为了看清楚赋值）
		string forth = "print(\"Reassign the Parameter...\");\n";
		for (int i = 0; i < vecPara.size(); i++)
		{
			string MapParamName = vecPara[i].sMapParamName;
			string ParamValue = vecPara[i].sParamValue;
			replace(MapParamName.begin(), MapParamName.end(), '.', '_');
			//这一段的赋值语句
			string AssignState = MapParamName + ":=" + ParamValue + ";\n";
			forth += AssignState;
		}
		forth += "print(\"call the generated simulation code to produce a result file name_res.csv\");\n";


		string fifth = "system(\"" + m_pCompRoot->GetCompName() + " -override=\"";
		//对各个关注参数值进行遍历
		for (int j = 0; j < vecPara.size(); j++)
		{
			//判断参数是否为数组结构，是的话须单独处理
			string ParamValue = vecPara[j].sParamValue;
			string sign = "{";
			string::size_type idx;
			idx = ParamValue.find(sign);//在ParamValue中查找sign
			if (idx == string::npos)//不存在，即为一般参数，不是数组类型
			{
				string AssignState = "+\"" + vecPara[j].sMapParamName + "=" + vecPara[j].sParamValue + ",\"";
				fifth += AssignState;
			}
			else
			{
				//先将{1,2,3}->,1,2,3,便于字符串的截取
				replace(ParamValue.begin(), ParamValue.end(), '{', ',');
				replace(ParamValue.begin(), ParamValue.end(), '}', ',');
				int num = count(ParamValue.begin(), ParamValue.end(), ',');//,出现4次，则有3个变量需要赋值
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


		//将代码写入文件
		QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";
		string file = saveCompareRaw.toStdString() + "ReSimulateScript.mos";
		fstream deletefile(file, ios::out);//将原有数据清除
		ofstream ofile(file, std::ios::app);  //将文本数据写入文件
		ofile << mosfile << endl;   //写入文件
		ofile.close();   //关闭文件
	}
	
}

void DocumentModel::ReCallSolver(const QString& SaveComparePath)
{
	string SolverName = LibPathsSetDlg::GetSolverName();
	if (SolverName == "openmodelica")
	{
		string disk_solver = LibPathsSetDlg::GetSolverDisk();
		string path_solver = "cd " + LibPathsSetDlg::GetSolverPath();
		//string disk_solver = "d:";                         //求解器所在磁盘为D盘
		//string path_solver = "cd D:\\openmodelica\\bin";   //求解器所在文件路径
		string order_solver = "omc " + SaveComparePath.toStdString() + "ReSimulateScript.mos";
		//string order_solver = "omc C:\\Users\\dell\\Desktop\\newfiles\\Script.mos";   //求解命令，求解mos文件
		//string solver = disk_solver + "&" + path_solver + "&" + order_solver + "&" + "pause";        //&为命令之间的连接符号，使用system函数时需要加"pause"
		string solver = disk_solver + "&" + path_solver + "&" + order_solver;                          //&为命令之间的连接符号
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
	qDebug() << exitCode;// 被调用程序的main返回的int
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

//创建仿真工作目录文件夹
void DocumentModel::GenerateSimulateWorkingDirectory()
{
	string folderPath = LibPathsSetDlg::GetWorkDir();
	//string folderPath = "C:\\Users\\dell\\Desktop\\newfiles";//在该路径下创建一个新的文件夹
	string command;
	command = "mkdir -p " + folderPath;
	system(command.c_str());
}

//生成仿真*.mos脚本文件
void DocumentModel::GenerateMosFile(const QString& sMoFilePath)
{
	string SolverName = LibPathsSetDlg::GetSolverName();
	if (SolverName == "openmodelica")
	{
		string mosfile;
		string first = "// load the file\n";//定义*.mos的文件内容
		string second = "print(\"Loading Modelica\");\n";
		//string third = "loadModel(Modelica); //new OMC version stopped importing Modelica model\n";//加载的4.0.0的库
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
		fstream deletefile(file, ios::out);//将原有数据清除
		//string file = "C:\\Users\\dell\\Desktop\\newfiles\\test02.mos";
		//fstream deletefile("C:\\Users\\dell\\Desktop\\newfiles\\test02.mos", ios::out);//将原有数据清除
		ofstream ofile(file, std::ios::app);  //将文本数据写入文件
		ofile << mosfile << endl;   //写入文件
		ofile.close();   //关闭文件
	}
	else if(SolverName == "MWorks")
	{
		string pyfile;
		string first = "#加载指定的Modelica模型文件\n";
		string second;
		vector<string> AllLibPaths;
		AllLibPaths.clear();
		AllLibPaths = LibPathsSetDlg::GetModelLibPaths();
		for (int i = 0; i < AllLibPaths.size(); i++)
		{
			string statement = "print(OpenModelFile(r\"" + AllLibPaths[i] + "\",True))\n";
			second += statement;
		}
		string third = "#仿真模型\n";
		string sWorkDir = LibPathsSetDlg::GetWorkDir();
		if (sWorkDir.back() != '/')
		{
			sWorkDir += "/";
		}
		string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//对不同模型有不同文件夹的相对路径
		
		string forth = "print(SimulateModel(model_name='" + m_pCompRoot->GetCompName() + "',start_time=" + to_string(SimulateSetDlg::GetStarttime()) + ",stop_time=" + to_string(SimulateSetDlg::GetEndtime());
		forth += ",number_of_intervals=" + to_string(SimulateSetDlg::GetMWorks_NumberOfIntervals()) + ",algo=" + SimulateSetDlg::GetMWorks_Method() + ",result_file = r\"" + saveCompareRaw + "\"))\n";
		string fifth = "#导出结果文件\n";
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
					if (pComp->IsDeformable())//如果是变形零件要获得frame_b相关数据
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
		fstream deletefile(file, ios::out);//将原有数据清除
		ofstream ofile(file, std::ios::app);  //将文本数据写入文件
		ofile << pyfile << endl;   //写入文件
		ofile.close();   //关闭文件
	}
	else
	{
		QMessageBox::information(NULL, "提示", "求解器名称有误，未能正确生成仿真脚本文件", QMessageBox::Ok);
	}
}

//调用求解器，执行mos文件，检查结果文件生成
void DocumentModel::CallSolver(const QString& SaveComparePath)
{
	string SolverName = LibPathsSetDlg::GetSolverName();
	string solver;
	if (SolverName == "openmodelica")
	{
		string disk_solver = LibPathsSetDlg::GetSolverDisk();
		string path_solver = "cd " + LibPathsSetDlg::GetSolverPath();
		//string disk_solver = "d:";                         //求解器所在磁盘为D盘
		//string path_solver = "cd D:\\openmodelica\\bin";   //求解器所在文件路径
		string order_solver = "omc " + SaveComparePath.toStdString() + "SimulateScript.mos";
		//string order_solver = "omc C:\\Users\\dell\\Desktop\\newfiles\\Script.mos";   //求解命令，求解mos文件
		//string solver = disk_solver + "&" + path_solver + "&" + order_solver + "&" + "pause";        //&为命令之间的连接符号
		solver = disk_solver + "&" + path_solver + "&" + order_solver;       //&为命令之间的连接符号
		//system(solver.c_str());
		//system("d:&cd D:\\openmodelica\\bin&omc C:\\Users\\dell\\Desktop\\newfiles\\Script.mos");
	}
	else if (SolverName == "MWorks")
	{
		string disk_solver = LibPathsSetDlg::GetSolverDisk();
		string path_solver = "cd " + LibPathsSetDlg::GetSolverPath();
		QString sFilePath = SaveComparePath;

		//0306测试更改
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
	//外调MWorks触发不了readyReadStandardError()信号,通过.py脚本将输出保存至RecordOutput.txt文件，然后从文件里读数据
	//connect(myProcess, SIGNAL(readyReadStandardOutput()), this,SLOT({qDebug() << myProcess->readAllStandardOutput()}));

	AnimationMessage.push_back("");
	AnimationMessage.push_back("");
	AnimationMessage.push_back("");

	myProcess->setReadChannel(QProcess::StandardOutput);
	
	//0423
	//myProcess->setReadChannel(QProcess::StandardError);
	//myProcess->setReadChannelMode(QProcess::MergedChannels);//合并通道
	//myProcess->setProcessChannelMode(QProcess::ForwardedChannels);//转发通道

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

//动画
void DocumentModel::Animating()
{
	////先得到时间序列值
	//m_pCompRoot->GetAllDescendants(m_vDescendants);
	//QString sCSVFile0 = "C:/Program Files/OpenModelica1.16.1-64bit/bin/Modelica.Mechanics.MultiBody.Examples.Elementary.Pendulum_res.csv";
	//bool b0 = FileParse::GetAllMultibodyPlacement(sCSVFile0, m_timeSeries, m_mapCompPlacement);
	//if (!b0)
	//{
	//	//提示错误;
	//	assert(false);
	//	return;
	//}

	//startTimer(100);
	//m_iStep = 0;
	//return;

	//1.检查是否正在动画，如果是，则退出
	//if (m_pAnimateThread->isRunning())
	if (m_pAnimateThread->m_bRunning == true)
	{
		return;
	}

	//2.先检测工作目录下是否有结果文件
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
	

	//3.设置动画线程的数据
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

	//4.启动动画线程
	MainWindow* pMain = (MainWindow*)parent();
	connect(m_pAnimateThread, SIGNAL(rep()), pMain, SLOT(repaint0()), Qt::BlockingQueuedConnection);
	//connect(m_pAnimateThread, SIGNAL(rep()), this, SLOT(repaint0()), Qt::QueuedConnection);
	m_pAnimateThread->start();
}

void DocumentModel::timerEvent(QTimerEvent* event)
{
	//2.驱动运动
	if (m_iStep < m_timeSeries.size())
	{
		//放置t0时刻各个零部件的位置
		auto itr = m_vDescendants.begin();
		for (; itr != m_vDescendants.end(); ++itr)
		{
			Component* pCom = *itr;
			if (1)//pCom->IsMultibodyComponent())//如果是机械多体部件
			{
				gp_Trsf trsf;
				string sComFullName = pCom->GetCompFullName();
				//去掉前缀的系统名
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

//暂停动画线程
void DocumentModel::AnimatingPause()
{
	//1.检测是否正在动画，是则暂停
	if (m_pAnimateThread->m_bRunning == true && m_pAnimateThread->m_bPause == false)
	{
		m_pAnimateThread->pauseThread();
	}
	else { return; }
}

//暂停后恢复动画
void DocumentModel::AnimatingResume()
{
	if (m_pAnimateThread->m_bRunning == true && m_pAnimateThread->m_bPause == true)
	{
		m_pAnimateThread->resumeThread();
	}
	else { return; }
}

//重置动画
void DocumentModel::AnimatingReset()
{
	if (m_pAnimateThread->m_bPause == true || m_pAnimateThread->m_bRunning == false)
	{
		m_pAnimateThread->closeThread();
		myContext->UpdateCurrentViewer();                 //更新物体至初始位置
		if (m_pAnimateThread != nullptr)
		{
			m_pAnimateThread->terminate();                //停止线程
			m_pAnimateThread->wait();
			delete m_pAnimateThread;
			m_pAnimateThread = nullptr;                   //重新初始化
		}
		m_pAnimateThread = new AnimationThread(this);
		m_pAnimateThread->m_bRunning = false;
	}
	else { return; }
}

//获取当前动画的数据
double DocumentModel::GetAnimatingTime()
{
	return m_pAnimateThread->datatime;
}
vector<string> DocumentModel::GetAnimatingDatas()
{
	return m_pAnimateThread->AllDatas;
}

//对选中的组件(必须是零件），更改其CyberInfo
void DocumentModel::ReplaceCyberInfo()
{
	assert(false);
}

//数字孪生驱动
bool DocumentModel::TwinDriveModel() {
	//将零部件打散，成为各个单独的显示体，便于驱动动画
	UpdateDisplay2();
	return true;
}

//初始化订阅和监听
int DocumentModel::InitSubsAndListner(int discoveryMehodID) {
	//将数据指针传给孪生动画线程
	StoredData* m_pStoredData = new StoredData();
	if (m_pTwinAniThread->m_pStoredData != nullptr) {
		delete m_pTwinAniThread->m_pStoredData;
	}
	m_pTwinAniThread->m_pStoredData = m_pStoredData;
	
	//设置孪生动画线程的数据
	vector<Component*> vDescendants;
	m_pCompRoot->GetAllDescendants(vDescendants);
	m_pTwinAniThread->SetComponents(vDescendants);
	m_pTwinAniThread->SetContext(myContext);
	m_pTwinAniThread->SetDocumentModel(this);

	//启动孪生动画线程
	MainWindow* pMain = (MainWindow*)parent();
	connect(m_pTwinAniThread, SIGNAL(rep()), pMain, SLOT(repaint0()), Qt::BlockingQueuedConnection);
	m_pTwinAniThread->start();
	return InitSubscribe(m_pStoredData, discoveryMehodID);
}

//结束订阅和监听
int DocumentModel::EndSubsAndListner() {
	m_pTwinAniThread->closeThread();
	myContext->UpdateCurrentViewer();                 //更新物体至初始位置
	if (m_pTwinAniThread != nullptr)
	{
		m_pTwinAniThread->terminate();                //停止线程
		m_pTwinAniThread->wait();
		delete m_pTwinAniThread;
		m_pTwinAniThread = nullptr;                   //重新初始化
	}
	m_pTwinAniThread = new TwinAnimationThread(this);
	return EndSubscribe();
}

//获取接收到的信息
string DocumentModel::GetReceivedMsg() {
	return m_pTwinAniThread->GetLastReceivedMsg();
}

//生成神经网络训练集
void DocumentModel::GenTrainSet(vector<string>& ControlVarsNames, vector<vector<double>>& samples, vector<string>& InputNames, vector<string>& OutputNames)
{	
	QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
	if (sWorkDir.back() != '/')
	{
		sWorkDir += "/";
	}
	QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";//对不同模型分别建立文件夹的相对路径
	
	//生成训练脚本文件   MWorks .py文件
	GenTrainpyFile(ControlVarsNames, samples, InputNames, OutputNames);

	//执行脚本，生成多个Result.csv文件	
	CallSolverTrain(saveCompareRaw);
	
	//读取Result.csv文件，利用这些文件生成仿真训练的数据集文件
	//旧版，fann版本
	//int samplesize = samples.size();
	//int inputsize = InputNames.size();
	//GenTrainSetFromCSV(saveCompareRaw, samplesize, inputsize);
	//GenTrainSetFromCSVLayer(saveCompareRaw, samplesize);

	//新版，利用Pytorch
	int samplesize = samples.size(); // 待解析的csv文件个数
	//bool issuccess = GenPyTrainSetFromCSVDiffComp(saveCompareRaw, samplesize, InputNames, OutputNames);
	bool issuccess = GenPyTrainSetFromCSVDiffCompNew(saveCompareRaw, samplesize, InputNames, OutputNames);
	if (issuccess) {
		QString outputmsg = "训练集生成成功！请在" + saveCompareRaw + "dataset路径下进行查看。";
		QMessageBox msgBox;
		msgBox.setText(outputmsg);
		msgBox.exec();
		return;
	} else {
		QMessageBox msgBox;
		msgBox.setText("Error！训练集生成失败！");
		msgBox.exec();
		return;
	}
}

//获得mo文件的存储路径
string DocumentModel::GetfannSavePath()
{
	QString sWorkDir = LibPathsSetDlg::GetWorkDir().c_str(); //"D:/M3drepos/M3d/SimWork"
	if (sWorkDir.back() != '/')
	{
		sWorkDir += "/";
	}
	QString saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName().c_str() + "/";//对不同模型分别建立文件夹的相对路径
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
	string saveCompareRaw = sWorkDir + m_pCompRoot->GetCompName() + "/";//对不同模型有不同文件夹的相对路径

	string forth = "print(SimulateModel(model_name='" + m_pCompRoot->GetCompName() + "',start_time=" + to_string(SimulateSetDlg::GetStarttime()) + ",stop_time=" + to_string(SimulateSetDlg::GetEndtime());
	forth += ",number_of_intervals=" + to_string(SimulateSetDlg::GetMWorks_NumberOfIntervals()) + ",algo=" + SimulateSetDlg::GetMWorks_Method() + "))\n";
	
	string fifth = "# Export result file\n";
	string AllAniVariables;
	AllAniVariables += "[";
	
	//1.设置的输入量
	for (int i = 0; i < InputNames.size(); i++)
	{
		AllAniVariables += "'" + InputNames[i] + "',";
	}

	//2.和运动/动画相关的输出量
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


	//数据集数据
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
	fstream deletefile(file, ios::out);//将原有数据清除
	ofstream ofile(file, std::ios::app);  //将文本数据写入文件
	ofile << pyfile << endl;   //写入文件
	ofile.close();   //关闭文件
}


void DocumentModel::CallSolverTrain(const QString& SaveComparePath)
{
	string disk_solver = LibPathsSetDlg::GetSolverDisk();
	string path_solver = "cd " + LibPathsSetDlg::GetSolverPath();
	QString sFilePath = SaveComparePath;


	//0306测试更改
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
	TrainProcess->execute("cmd.exe", argument);//阻塞调用
}

// 生成供Pytorch训练用的神经网络训练集，按照不同组件分别生成
bool DocumentModel::GenPyTrainSetFromCSVDiffComp(const QString& saveComparePath, int samplesize, vector<string>& InputNames, vector<string>& OutputNames)
{
	// 根据输出变量名称，对不同变量进行分类，不同组件的变量分到不同组
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

	// 创建描述神经网络结构的文件
	for (auto it = compNames.begin(); it != compNames.end(); it++)
	{
		CSV_Parser::Sheet sheet;
		// 设置表名称
		sheet.name = QString::fromStdString(*it);
		
		// 设置第一行输入输出个数
		QStringList strListNum;
		strListNum.append(QString::fromStdString(std::to_string(InputNames.size())));
		strListNum.append(QString::fromStdString(std::to_string(compNameToVarNames[*it].size())));
		sheet.data.append(strListNum);


		// 设置第二行的参数名称
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

		// 生成神经网络结构文件
		QString netFrmPath = saveComparePath + "netfrm/netfrm_" + QString::fromStdString(*it) + ".csv";
		bool isnowsuccess = FileParse::generateCSV(netFrmPath, sheet);
	}

	// 针对不同组件创建不同sheet，给generateCSV文件使用
	map<string, CSV_Parser::Sheet> compNameToSheet;
	for (auto it = compNames.begin(); it != compNames.end(); it++)
	{
		CSV_Parser::Sheet sheet;
		// 设置表名称
		sheet.name = QString::fromStdString(*it);
		// 设置第一行的参数名称
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

	// 建立变量名称到数据列索引的映射
	map<string, int> varNameToidx;
	QString csvFilePath = saveComparePath + "Train/Result.csv";
	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	for (int col = 0; col < allOutputNames.size(); col++)
	{
		string colname = allOutputNames[col].toStdString();
		varNameToidx[colname] = col;		
	}

	// 读取.csv文件，进行数据填充
	int rowsbycsv = 30; // 每个csv文件取30行数据
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
				// 不同组件组合不同行数据
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
	// 输出数据集文件
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

// 支持旋转矩阵转欧拉角，支持混合表达
bool DocumentModel::GenPyTrainSetFromCSVDiffCompNew(const QString& saveComparePath, int samplesize, vector<string>& InputNames, vector<string>& OutputNames)
{
	// 根据输出变量名称，对不同变量进行分类，不同组件的变量分到不同组
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

	// 创建描述神经网络结构的文件
	for (auto it = compNames.begin(); it != compNames.end(); it++)
	{
		CSV_Parser::Sheet sheet;
		// 设置表名称
		sheet.name = QString::fromStdString(*it);

		// 设置第一行输入输出个数
		QStringList strListNum;
		strListNum.append(QString::fromStdString(std::to_string(InputNames.size())));
		//strListNum.append(QString::fromStdString(std::to_string(compNameToVarNames[*it].size())));
		strListNum.append(QString::fromStdString(std::to_string(6)));//六自由度
		sheet.data.append(strListNum);


		// 设置第二行的参数名称
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

		// 生成神经网络结构文件
		QString netFrmPath = saveComparePath + "netfrm/netfrm_" + QString::fromStdString(*it) + ".csv";
		bool isnowsuccess = FileParse::generateCSV(netFrmPath, sheet);
	}

	// 针对不同组件创建不同sheet，给generateCSV文件使用
	map<string, CSV_Parser::Sheet> compNameToSheet;
	for (auto it = compNames.begin(); it != compNames.end(); it++)
	{
		CSV_Parser::Sheet sheet;
		// 设置表名称
		sheet.name = QString::fromStdString(*it);
		// 设置第一行的参数名称
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

	// 建立变量名称到数据列索引的映射
	map<string, int> varNameToidx;
	QString csvFilePath = saveComparePath + "Train/Result.csv";
	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	for (int col = 0; col < allOutputNames.size(); col++)
	{
		string colname = allOutputNames[col].toStdString();
		varNameToidx[colname] = col;
	}

	// 读取.csv文件，进行数据填充
	int rowsbycsv = 60; // 每个csv文件取30行数据
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
				// 不同组件组合不同行数据
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
					// 需要进行旋转矩阵 -> 欧拉角
					// 先填充位置坐标
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
	// 输出数据集文件
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
	//普通版本
	//FILE* fp;
	//string fannFilePath = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + ".fann";

	//QString csvFilePath = SaveComparePath + "Train/Result.csv";
	//QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	//int outputsize = allOutputNames.size() - inputsize - 1;   //输出变量个数 = 列数 - 输入变量个数 - Time(1)

	//int rowsbycsv = 30; //每个文件里取几行

	////fopen_s(&fp, "add.fann", "w");
	//fopen_s(&fp, fannFilePath.c_str(), "w");
	//fprintf_s(fp, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
	//// 生成训练文件
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
	
	//文件数多的普通版本
	//for (int filenum = 3; filenum <= 10; filenum++)
	//{
	//	FILE* fp;
	//	string fannFilePath = SaveComparePath.toStdString() + m_pCompRoot->GetCompName().c_str() + std::to_string(filenum) + ".fann";

	//	QString csvFilePath = SaveComparePath + "Train500_" + QString::fromStdString(std::to_string(filenum)) + "/Result.csv";
	//	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	//	int outputsize = allOutputNames.size() - inputsize - 1;   //输出变量个数 = 列数 - 输入变量个数 - Time(1)

	//	int rowsbycsv = 30; //每个文件里取几行

	//	//fopen_s(&fp, "add.fann", "w");
	//	fopen_s(&fp, fannFilePath.c_str(), "w");
	//	fprintf_s(fp, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
	//	// 生成训练文件
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

	//根据组件划分的训练版本（以BodyShape8为例）
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

	//	int rowsbycsv = 30; //每个文件里取几行

	//	//fopen_s(&fp, "add.fann", "w");
	//	fopen_s(&fp, fannFilePath.c_str(), "w");
	//	fprintf_s(fp, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
	//	// 生成训练文件
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

	//根据point划分的训练版本，以BodyShape8为例
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

		int rowsbycsv = 30; //每个文件里取几行


		fopen_s(&fpx, fannFilePathx.c_str(), "w");
		fprintf_s(fpx, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
		fopen_s(&fpy, fannFilePathy.c_str(), "w");
		fprintf_s(fpy, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
		fopen_s(&fpz, fannFilePathz.c_str(), "w");
		fprintf_s(fpz, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
		fopen_s(&fpr, fannFilePathr.c_str(), "w");
		fprintf_s(fpr, "%d %d %d\n", samplesize * rowsbycsv, inputsize, outputsize);
		// 生成训练文件
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

//依照层次生成神经网络
void DocumentModel::GenTrainSetFromCSVLayer(const QString& SaveComparePath, int samplesize)
{
	QString csvFilePath = SaveComparePath + "Train/Result.csv";
	QStringList allOutputNames = FileParse::GetAllVariablesNames(csvFilePath);
	int rowsbycsv = 30; //每个文件里取几行
	
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
	// 生成训练文件
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
