//2021.09.16
//Wu YZ
#include "M3d.h"
#include "DocumentCommon.h"
#include "Component.h"
#include "Transparency.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QColor>
#include <QColorDialog>
#include <QStatusBar>
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
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_Name.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_DriverTable.hxx>
#include <TDF_ChildIterator.hxx>
#include <qfiledialog.h>
#include <Standard_WarningsRestore.hxx>

#include "mainwindow.h"
#include "translate.h"
#include <BinDrivers.hxx>
#include <TNaming_Builder.hxx>
#include <qmessagebox.h>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TNaming_Tool.hxx>
//
#include <PCDM_RetrievalDriver.hxx>
#include <PCDM_StorageDriver.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <Geom_Transformation.hxx>
#include <TopExp_Explorer.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Reference.hxx>
#include <BRepTools_ReShape.hxx>
#include <AIS_ColoredShape.hxx>
#include <qbuffer.h>
#include "global.h"
#include <AIS_ViewCube.hxx>
#include "AdaptorVec_AIS.h"
#include <Prs3d_LineAspect.hxx>
#include <V3d_View.hxx>
#include "LibPathsSetDlg.h"

//#include <BRepTools.hxx>
//#include <iostream>
//#include <fstream>
//using namespace std;

// =======================================================================
// function : Viewer
// purpose  :
// =======================================================================
Handle(V3d_Viewer) DocumentCommon::Viewer(const Standard_ExtString,
	const Standard_CString,
	const Standard_Real theViewSize,
	const V3d_TypeOfOrientation theViewProj,
	const Standard_Boolean theComputedMode,
	const Standard_Boolean theDefaultComputedMode)
{
	static Handle(OpenGl_GraphicDriver) aGraphicDriver;
	if (aGraphicDriver.IsNull())
	{
		Handle(Aspect_DisplayConnection) aDisplayConnection;
#if !defined(_WIN32) && !defined(__WIN32__) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX))
		aDisplayConnection = new Aspect_DisplayConnection(OSD_Environment("DISPLAY").Value());
#endif
		aGraphicDriver = new OpenGl_GraphicDriver(aDisplayConnection);
	}

	Handle(V3d_Viewer) aViewer = new V3d_Viewer(aGraphicDriver);
	aViewer->SetDefaultViewSize(theViewSize);
	aViewer->SetDefaultViewProj(theViewProj);
	aViewer->SetComputedMode(theComputedMode);
	aViewer->SetDefaultComputedMode(theDefaultComputedMode);

	return aViewer;
}

DocumentCommon::DocumentCommon(MainWindow* theApp)
	: QObject(theApp),
	myContextIsEmpty(true)
{
	TCollection_ExtendedString a3DName("Visu3D");

	myViewer = Viewer(a3DName.ToExtString(), "", 1000.0, V3d_XposYnegZpos, Standard_True, Standard_True);

	myViewer->SetDefaultLights();
	myViewer->SetLightOn();

	myContext = nullptr; // new AIS_InteractiveContext(myViewer);
	m_IsModel = false;

	//m_pDocRoot = thelbl;
	m_pCompRoot = nullptr;
	m_pMutex = new QMutex();

	//创建OCAF文档
	//createNewDoc();
	m_bModified = false;
}

DocumentCommon::~DocumentCommon()
{
	//TDF_Label lbl = m3dOcafDoc->Main()
	Clear();

	delete m_pMutex;
}

//显示全局坐标系
void DocumentCommon::DisplayGlobalCoordsys()
{
	Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
	myContext->Display(aViewCube, true);

	if(!m_bShowOxyz)
		return;

	gp_Ax3 Ax3;
	double theLength, arrowLength;
	//GetAx3Length(theLength, arrowLength);

	theLength = LibPathsSetDlg::GetGlobalSysSize();
	arrowLength = theLength / 3;

	Handle(AdaptorVec_AIS) z_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.Direction(), theLength, arrowLength, true);
	z_Axis->SetText("  Z");
	//Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 2);
	//z_Axis->SetAspect(lineAspect);
	z_Axis->SetColor(Quantity_NOC_ALICEBLUE);
	myContext->Display(z_Axis, Standard_True);
	Handle(AdaptorVec_AIS) x_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.XDirection(), theLength, arrowLength, true);
	x_Axis->SetText("  X");
	//Handle_Prs3d_LineAspect lineAspect2 = new Prs3d_LineAspect(Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2);
	//x_Axis->SetAspect(lineAspect2);
	myContext->Display(x_Axis, Standard_False);
	Handle(AdaptorVec_AIS) y_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.YDirection(), theLength, arrowLength, true);
	y_Axis->SetText("  Y");
	//Handle_Prs3d_LineAspect lineAspect3 = new Prs3d_LineAspect(Quantity_NOC_BLUE, Aspect_TOL_SOLID, 2);
	//y_Axis->SetAspect(lineAspect3);
	myContext->Display(y_Axis, Standard_False);
}

void DocumentCommon::createNewDoc()
{
	anOcaf_Application = new TDocStd_Application;
	anOcaf_Application->DefineFormat("m3dmdl", "M3d system Format", "m3dmdl"/*, \
		new NewDocumentFormat_RetrievalDriver(), new NewDocumentFormat_StorageDriver()*/, nullptr, nullptr);

	anOcaf_Application->DefineFormat("m3dcom", "M3d Component Format", "m3dcom"/*, \
		new NewDocumentFormat_RetrievalDriver(), new NewDocumentFormat_StorageDriver()*/, nullptr, nullptr);

	anOcaf_Application->NewDocument("m3dcom"/*"BinOcaf"*/, m3dOcafDoc);//先缺省，后再改

	m_pDocRoot = m3dOcafDoc->Main();
	TPrsStd_AISViewer::New(m_pDocRoot, myViewer);//????
	//Handle(AIS_InteractiveContext) anAisContext;
	TPrsStd_AISViewer::Find(m_pDocRoot, myContext/*anAisContext*/);
	//anAisContext->SetDisplayMode(AIS_Shaded, Standard_True);
	myContext->SetDisplayMode(AIS_Shaded, Standard_True);
	//Retrieve the current viewer selector
	const Handle(StdSelect_ViewerSelector3d)& aMainSelector = myContext->MainSelector();
	//Set the flag to allow overlap detection
	aMainSelector->AllowOverlapDetection(true);
	// Set the maximum number of available "undo" actions
	m3dOcafDoc->SetUndoLimit(30);

	//UpdateDisplay();
	DisplayGlobalCoordsys();

	//显示视方体
	//Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
	/*
	m_context->SetDisplayMode(AIS_Shaded, true);   //设置显示模式为阴影
	H_AisViewCube->SetBoxSideLabel(V3d_Xpos, "Right");
	H_AisViewCube->SetBoxSideLabel(V3d_Ypos, "Back");
	H_AisViewCube->SetBoxSideLabel(V3d_Zpos, "Top");
	H_AisViewCube->SetBoxSideLabel(V3d_Xneg, "Left");
	H_AisViewCube->SetBoxSideLabel(V3d_Yneg, "Front");
	H_AisViewCube->SetBoxSideLabel(V3d_Zneg, "Bottoom");
	H_AisViewCube->SetTransparency(0.8);
	H_AisViewCube->SetTextColor(Quantity_Color(Quantity_NOC_MATRABLUE));
	H_AisViewCube->SetFontHeight(20);
	H_AisViewCube->SetMaterial(Graphic3d_MaterialAspect(Graphic3d_NOM_GOLD));
	H_AisViewCube->SetTransformPersistence(
		new Graphic3d_TransformPers(
			Graphic3d_TMF_TriedronPers,
			Aspect_TOTP_RIGHT_UPPER,
			Graphic3d_Vec2i(100, 100)));*/
	//myContext->Display(aViewCube, true);
}

//显示形状
void DocumentCommon::DisplayShapes(vector<TopoDS_Shape>& vShapes)
{
	vector<TopoDS_Shape>::iterator iter;
	for (iter = vShapes.begin(); iter != vShapes.end(); ++iter)
	{
		TopoDS_Shape aShape = *iter;
		Handle(AIS_InteractiveObject) anAisIO = new AIS_Shape(aShape);
		myContext->Display(anAisIO, false);
	}
}

//打开加载文档
bool DocumentCommon::loadFile(const QString fileName)
{
	////测试
	//BRepPrimAPI_MakeBox mkBox(gp_Pnt(0, 0, 0), 20, 50, 10);
	//TopoDS_Shape ResultShape = mkBox.Shape();
	//Handle(AIS_Shape) myAISObject = new AIS_Shape(ResultShape);
	//myContext->Display(myAISObject, false);

	//Handle(TDocStd_Application) anOcaf_Application = new TDocStd_Application;
	// load persistence
	BinDrivers::DefineFormat(anOcaf_Application);
	// Look for already opened
	if (anOcaf_Application->IsInSession(fileName.toStdU16String().data()))
	{
		//myResult << "Document: " << myFileName << " is already in session" << std::endl;
		return false;
	}
	// Open the document in the current application
	PCDM_ReaderStatus aReaderStatus = anOcaf_Application->Open(fileName.toStdU16String().data(), m3dOcafDoc);
	if (aReaderStatus == PCDM_RS_OK)
	{
		//myContext->RemoveAll(false);
		//TPrsStd_AISViewer::New(m3dOcafDoc->Main(), myViewer);
		//TPrsStd_AISViewer::Find(m3dOcafDoc->Main(), myContext);

		//myContext->SetAutoActivateSelection(true);
		// Retrieve the current viewer selector
		// const Handle(StdSelect_ViewerSelector3d)& aMainSelector = myContext->MainSelector();
		// Set the flag to allow overlap detection
		//aMainSelector->AllowOverlapDetection(true);
		//myContext->SetDisplayMode(AIS_Shaded, Standard_True);

		////测试
		//BRepPrimAPI_MakeBox mkBox(gp_Pnt(0, 0, 0), 20, 30, 40);
		//TopoDS_Shape ResultShape = mkBox.Shape();
		//Handle(AIS_Shape) myAISObject = new AIS_Shape(ResultShape);
		//myContext->Display(myAISObject, false);
		//myContext->Activate(myAISObject, 0);

		// Set the maximum number of available "undo" actions
		SetRoot(m3dOcafDoc->Main());
		m3dOcafDoc->SetUndoLimit(30);

		// Display the presentations (which was not stored in the document)
		//vector<TopoDS_Shape> vShapes = m_pComDS->GetAllShapes();
		//DisplayShapes(vShapes);
		//DisplayPresentation();

		myfileName = fileName;

		UpdateDisplay();

		return true;
	}
	else
	{
		//myResult << "Error! The file wasn't opened. PCDM_ReaderStatus: " << aReaderStatus << std::endl;
		return false;
	}
}

bool DocumentCommon::save()
{
	if (myfileName.isEmpty())
	{
		return saveAs();
	}

	return saveFile(myfileName);
}

bool DocumentCommon::saveAs()
{
	QWidget* mainWnd = (QWidget*)parent();
	QString qsFilter = "零件(*.m3dcom)";
	if (m_IsModel)
	{
		qsFilter = "系统(*.m3dmdl)";
	}

	QString gfileName = QFileDialog::getSaveFileName(mainWnd, QString(), QString(), qsFilter);
	if (!gfileName.isEmpty())
	{
		//fileName = gfileName;
		//自动追加后缀
		if (!m_IsModel) {
			if (QFileInfo(gfileName).suffix().toLower() != "m3dcom")
			{
				gfileName += ".m3dcom";
			}
		}
		else //if (nDocType == 2)
		{
			if (QFileInfo(gfileName).suffix().toLower() != "m3dmdl")
			{
				gfileName += ".m3dmdl";
			}
		}

		return saveFile(gfileName);
	}

	return false;
}

//从文档存取形状
void DocumentCommon::serializeShapes(Handle(TopTools_HSequenceOfShape) theShapes, bool bSave)
{
	if (bSave)
	{
		//将形状数据存入文档
		m3dOcafDoc->NewCommand();
		for (int i = 1; i <= theShapes->Length(); i++)
		{
			TDF_Label aLabel = TDF_TagSource::NewChild(m3dOcafDoc->Main());
			TDataStd_Name::Set(aLabel, "Naming_NamedShapes");

			//对theShapes[i]的每一个顶点、边、面进行处理
			TopoDS_Shape aShape = theShapes->Value(i);
			//先遍历所有的面
			TopExp_Explorer Ex;
			int iChild = 1;
			for (Ex.Init(aShape, TopAbs_FACE); Ex.More(); Ex.Next()) {
				TopoDS_Shape face_i = Ex.Current();
				TDF_Label lab_i = aLabel.FindChild(iChild++);
				TNaming_Builder B_i(lab_i);
				B_i.Generated(face_i);

				//加入显示
				Handle(AIS_Shape) ais_i = new AIS_Shape(face_i);
				myContext->Display(ais_i, false);
			}

			//原来代码注释掉
			//TNaming_Builder B(aLabel);
			//B.Generated(theShapes->Value(i));

			//// Get the TPrsStd_AISPresentation of the new box TNaming_NamedShape
			//Handle(TPrsStd_AISPresentation) anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
			//// Display it
			//anAisPresentation->Display(1);
			//// Attach an integer attribute to aLabel to memorize it's displayed
			//TDataStd_Integer::Set(aLabel, i);
		}

		myContext->UpdateCurrentViewer();

		m3dOcafDoc->CommitCommand();
	}
	else {
		//从文档读出形状

	}
}

//bool 保存
bool DocumentCommon::saveFile(const QString gfileName)
{
	////先将当前几何注入文档
	//TDF_Label aLabel = TDF_TagSource::NewChild(m3dOcafDoc->Main());
	//TNaming_Builder B(aLabel);
	//Handle(TopTools_HSequenceOfShape) shapes = Translate::getShapes(myContext);
	//if (shapes)
	//{
	//    for (int i = 1; i <= shapes->Length(); i++)
	//    {
	//        B.Generated(shapes->Value(i));
	//    }
	//}

	//Handle(TDocStd_Application) anOcaf_Application = new TDocStd_Application;
	BinDrivers::DefineFormat(anOcaf_Application);
	m3dOcafDoc->ChangeStorageFormat("BinOcaf"); //自动追加??

	//先检查是否存在，如果有提醒覆盖
	//此处覆盖，会产生*.cbf的后缀！！！！！bug  
	// Saves the document in the current application
	PCDM_StoreStatus aStoreStatus = anOcaf_Application->SaveAs(m3dOcafDoc, gfileName.toStdU16String().data());
	if (aStoreStatus == PCDM_SS_OK)
	{
		//去掉.cbf
		QString f0 = gfileName;
		f0 += ".cbf";
		QFile file0(f0);
		QFile::remove(gfileName);
		file0.rename(gfileName);
		myfileName = gfileName;
		QString sTitle = tr("M3d建模与仿真一体化平台——");
		sTitle += myfileName;
		QWidget* mainWnd = (QWidget*)parent();
		mainWnd->setWindowTitle(sTitle);

		SetModified(false);
		return true;
	}
	else
	{
		QMessageBox::information(NULL, "提示", "存储错误", QMessageBox::Ok);
		return false;
	}
}

void DocumentCommon::SetObjects(const NCollection_Vector<Handle(AIS_InteractiveObject)>& theObjects,
	Standard_Boolean theDisplayShaded)
{
	myContext->RemoveAll(Standard_False);
	myContextIsEmpty = theObjects.IsEmpty();

	for (NCollection_Vector<Handle(AIS_InteractiveObject)>::Iterator anIter(theObjects);
		anIter.More(); anIter.Next())
	{
		const Handle(AIS_InteractiveObject)& anObject = anIter.Value();
		if (!theDisplayShaded)
		{
			myContext->Display(anObject, Standard_False);
		}
		else
		{
			myContext->Display(anObject, AIS_Shaded, 0, Standard_False);
		}
	}
	myViewer->Redraw();

	//AIS_ListOfInteractive anAisObjectsList;
	//myContext->DisplayedObjects(anAisObjectsList);
	//for(AIS_ListOfInteractive::Iterator anIter(anAisObjectsList); 
	//    anIter.More(); anIter.Next())
	//{
	//  const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
	//  Quantity_Color aShapeColor;
	//  myContext->Color(anAisObject, aShapeColor);
	//  myResult << "A Current shape color: Red = " << aShapeColor.Red()
	//           << " Green = " << aShapeColor.Green() << " Blue = " << aShapeColor.Blue() << std::endl;
	//  aShapeColor.SetValues(0.8, 0.1, 0.1, Quantity_TOC_RGB);
	//  myContext->SetColor(anAisObject, aShapeColor, Standard_False);
	//  myResult << "A New shape color: Red = " << aShapeColor.Red()
	//           << " Green = " << aShapeColor.Green() << " Blue = " << aShapeColor.Blue() << std::endl;
	//}

}

void DocumentCommon::Clear()
{
	myContext->RemoveAll(Standard_True);
	myContextIsEmpty = true;
}

void DocumentCommon::undo()
{
	if (m3dOcafDoc->Undo())
	{
		m3dOcafDoc->CommitCommand();
		//从label中读取shapes刷新显示
		myContext->RemoveAll(Standard_False);
		//Handle(AIS_InteractiveContext) aContext;
		//TPrsStd_AISViewer::Find(m3dOcafDoc->Main(), aContext);
		//aContext->SetDisplayMode(AIS_Shaded, Standard_True);
		//myContext = aContext;

		//DisplayPresentation();
		UpdateDisplay();

		myContext->UpdateCurrentViewer();
	}
}

bool DocumentCommon::canUndo()
{
	return (!m3dOcafDoc->GetUndos().IsEmpty());
}

void DocumentCommon::redo()
{
	if (m3dOcafDoc->Redo())
	{
		m3dOcafDoc->CommitCommand();

		myContext->RemoveAll(Standard_False);

		//DisplayPresentation();
		UpdateDisplay();

		myContext->UpdateCurrentViewer();
	}
}

bool DocumentCommon::canRedo()
{
	return (!m3dOcafDoc->GetRedos().IsEmpty());
}

void DocumentCommon::DisplayPresentation()
{
	TDF_Label aRootlabel = m3dOcafDoc->Main();

	//存取形状;
	TDF_Label theShapes_Lbl;
	for (TDF_ChildIterator it(aRootlabel); it.More(); it.Next())
	{
		TDF_Label aL = it.Value();
		Handle(TDataStd_Name) theName;
		if (aL.FindAttribute(TDataStd_Name::GetID(), theName))
		{
			if (theName->Get() == "Naming_NamedShapes")
			{
				theShapes_Lbl = aL;
				break;
			}
		}
	}
	//TDF_Label theShapes_Lbl = aRootlabel.FindChild(1);
	//显示所有Shapes，缺省颜色;
	for (TDF_ChildIterator it(theShapes_Lbl); it.More(); it.Next())
	{
		TDF_Label aLabel = it.Value();
		Handle(TNaming_NamedShape) aNamedShape;
		if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
		{
			continue;
		}

		//Handle(TDataStd_Integer) aDataInteger;
		//// To know if the object was displayed
		//if (aLabel.FindAttribute(TDataStd_Integer::GetID(), aDataInteger))
		//{
		//    if (!aDataInteger->Get())
		//    {
		//        continue;
		//    }
		//}

		TopoDS_Shape shape = TNaming_Tool::GetShape(aNamedShape);
		Handle(AIS_InteractiveObject) anAisIO = new AIS_Shape(shape);

		////如果有颜色属性，则显示颜色;
		//Handle(TDataStd_RealArray) clorAttr;
		//if (aLabel.FindAttribute(TDataStd_RealArray::GetID(), clorAttr))
		//{
		//    Quantity_Color CSFColor(clorAttr->Value(1), clorAttr->Value(2), clorAttr->Value(3), Quantity_TOC_RGB);
		//    //Handle(AIS_Shape) aShapePrs = new AIS_Shape(shape);
		//    //aShapePrs->SetColor(CSFColor);
		//    //myContext->Display(aShapePrs, false);
		//    anAisIO->SetColor(CSFColor);
		//}

		//Handle(TDataStd_Real) realRed;
		//if (aLabel.FindAttribute(TDataStd_Real::GetID(), realRed))
		//{
		//    Quantity_Color CSFColor(realRed->Get(), 0.0, 0.0, Quantity_TOC_RGB);
		//    //Handle(AIS_Shape) aShapePrs = new AIS_Shape(shape);
		//    //aShapePrs->SetColor(CSFColor);
		//    //myContext->Display(aShapePrs, false);
		//    anAisIO->SetColor(CSFColor);
		//}
		myContext->Display(anAisIO, false);

		//filebuf fb;
		//fb.open("text.txt", ios::out);
		//Standard_OStream os(&fb);
		//BRepTools::Dump(shape, os);
		//fb.close();
		//break;

		////源代码，注释
		//Handle(TPrsStd_AISPresentation) anAisPresentation;
		//if (!aLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), anAisPresentation))
		//{
		//    anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
		//}
		//anAisPresentation->SetColor(Quantity_NOC_ORANGE);
		//anAisPresentation->Display(1);

		////Handle(TDF_Attribute) attr;
		////if (aLabel.FindAttribute(it.Value(), attr))
		////{
		////    TopoDS_Shape shape = Handle(TNaming_NamedShape)::DownCast(attr)->Get();
		////    myContext->Display(new AIS_Shape(shape), false);
		////}
	}

	//显示参考面的颜色
	TDF_Label colorLbl = aRootlabel.FindChild(10);
	for (TDF_ChildIterator it(colorLbl); it.More(); it.Next())
	{
		TDF_Label clr_i = it.Value();
		//得到它的引用TDF
		Handle(TDF_Reference) refClr;
		clr_i.FindAttribute(TDF_Reference::GetID(), refClr);
		//得到引用TDF的Shape
		TDF_Label orign = refClr->Get();
		//显示该Shape的颜色
		Handle(TNaming_NamedShape) aNamedShape1;
		orign.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		TopoDS_Shape shape = TNaming_Tool::GetShape(aNamedShape1);
		Handle(AIS_InteractiveObject) anAisI1 = new AIS_Shape(shape);

		Handle(TDataStd_RealArray) clorAttr;
		clr_i.FindAttribute(TDataStd_RealArray::GetID(), clorAttr);
		Quantity_Color CSFColor(clorAttr->Value(1), clorAttr->Value(2), clorAttr->Value(3), Quantity_TOC_RGB);
		anAisI1->SetColor(CSFColor);
		myContext->Display(anAisI1, false);
	}

	myContext->UpdateCurrentViewer();
}

//void DocumentCommon::detect(int x, int y, bool b2)
//{
//    myContext->MoveTo(x, y, myViewer, b2);
//}

void DocumentCommon::setSelectMode(QString sMode)
{
	//myContext->ClearSelected(true);
	myContext->Deactivate();
	myContext->SetAutomaticHilight(true);

	if (sMode == "顶点")
	{
		// activates decomposition of shapes into faces
		myContext->Activate(AIS_Shape::SelectionMode(TopAbs_VERTEX));
		//const int aSubShapeSelMode = AIS_Shape::SelectionMode(TopAbs_Face);
		//myContext->Activate(aShapePrs, aSubShapeSelMode);

		//myContext->SetSelectionModeActive(OCTopAbs_ShapeEnum.TopAbs_VERTEX);
	}
	else if (sMode == "棱边")
	{
		myContext->Activate(AIS_Shape::SelectionMode(TopAbs_EDGE));
	}
	else if (sMode == "面")
	{
		myContext->Activate(AIS_Shape::SelectionMode(TopAbs_FACE));
	}
	else if (sMode == "体")
	{
		myContext->Activate(AIS_Shape::SelectionMode(TopAbs_SOLID));
	}
	else if (sMode == "任一")
	{
		myContext->Activate(AIS_Shape::SelectionMode(TopAbs_SHAPE));
	}
	else //接口
	{

	}
}

//向文档增加一个颜色节点，增加其引用label和颜色值;
void DocumentCommon::AddRefColor(TDF_Label& aLabel0, Quantity_Color& aClr)
{
	TDF_Label aLabel = m3dOcafDoc->Main();
	//TDataStd_Name::Set(aLabel, "Refer_Colors");

	//第10个存储了颜色;
	TDF_Label colorLbl = aLabel.FindChild(10);
	TDF_Label newChild = colorLbl.NewChild();
	Handle(TDF_Reference) newChildRef = TDF_Reference::Set(newChild, aLabel0);
	Handle(TDataStd_RealArray) aRealArray_Color = TDataStd_RealArray::Set(newChild, 1, 3);
	aRealArray_Color->SetValue(1, aClr.Red());
	aRealArray_Color->SetValue(2, aClr.Green());
	aRealArray_Color->SetValue(3, aClr.Blue());
}

//bool DocumentCommon::RemoveConn()
//{
//    if (!myContext->NbSelected()) return false;
//    myContext->InitSelected();
//    picked = myContext->SelectedInteractive();
//    Handle(AIS_Shape) myshape(Handle(AIS_Shape)::DownCast(picked));
//    bool flag=(dynamic_cast<M3dCom_DS*>(m_pDS))->DeleteConnector(myshape->Shape());
//    UpdateDisplay();
//    return flag;
//}
//
//bool DocumentCommon::RemoveSelshape() {
//    if (!myContext->NbSelected()) return false;
//    myContext->InitSelected();
//    picked = myContext->SelectedInteractive();
//    myContext->Remove(picked, true);
//}

//void DocumentCommon::SetObjColor(TopoDS_Shape& aShape, Quantity_Color& aClr)
//{
//    m_pDS->SetShapeColor(aShape, aClr);
//    /*
//    TDF_Label aRootlabel = m3dOcafDoc->Main();
//
//    //存取形状;
//    TDF_Label theShapes_Lbl;
//    for (TDF_ChildIterator it(aRootlabel); it.More(); it.Next())
//    {
//        TDF_Label aL = it.Value();
//        Handle(TDataStd_Name) theName;
//        if (aL.FindAttribute(TDataStd_Name::GetID(), theName))
//        {
//            if (theName->Get() == "Naming_NamedShapes")
//            {
//                theShapes_Lbl = aL;
//                break;
//            }
//        }
//    }
//    if (theShapes_Lbl.IsNull())
//    {
//        assert(false);
//        return;
//    }
//
//    for (TDF_ChildIterator it(theShapes_Lbl); it.More(); it.Next())
//    {
//        TDF_Label aLabel = it.Value();
//        Handle(TNaming_NamedShape) aNamedShape;
//        if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
//        {
//            continue;
//        }
//        TopoDS_Shape shape = TNaming_Tool::GetShape(aNamedShape);
//
//        //找到了这个shape,增加一个颜色属性
//        if (aShape == shape)
//        {
//            //Handle(TDataStd_RealArray) aRealArray_Color = TDataStd_RealArray::Set(aLabel, 1, 3);
//            //aRealArray_Color->SetValue(1, aClr.Red());
//            //aRealArray_Color->SetValue(2, aClr.Green());
//            //aRealArray_Color->SetValue(3, aClr.Blue());
//
//            //添加颜色引用
//            AddRefColor(aLabel, aClr);
//
//            //TDataStd_Real::Set(aLabel, 1.0);
//        }
//    }
//    */
//}

//void DocumentCommon::objColor()
//{
//    //myContext->SetDisplayMode(1, false);
//    Handle(AIS_InteractiveObject) Current;
//    QColor MSColor;
//    myContext->InitSelected();
//    Current = myContext->SelectedInteractive();
//    if (!Current)
//    {
//        QMessageBox::information(NULL, "提示", "请选择图元", QMessageBox::Ok);
//        return;
//    }
//    if (Current&&Current->HasColor()) {
//        Quantity_Color CSFColor;
//        myContext->Color(Current, CSFColor);
//        MSColor = QColor(CSFColor.Red() * 255., CSFColor.Green() * 255., CSFColor.Blue() * 255.);
//    }
//    else {
//        MSColor = QColor(255, 255, 255);
//    }
//    QColor color = QColorDialog::getColor(MSColor);
//    if (color.isValid())
//    {
//        m3dOcafDoc->NewCommand();
//
//        Quantity_Color CSFColor(color.red() / 255., color.green() / 255., color.blue() / 255., Quantity_TOC_RGB);
//        for (; myContext->MoreSelected(); myContext->NextSelected())
//        {
//            TopoDS_Shape ats = myContext->SelectedShape();
//            if (1) //ats.ShapeType() == TopAbs_FACE)
//            {
//                //不需要显示了，在UpdateDisyplay
//                //Handle(AIS_Shape) aShapePrs = new AIS_Shape(ats);
//                //aShapePrs->SetColor(CSFColor);
//                SetObjColor(ats, CSFColor);
//                //myContext->Redisplay(aShapePrs, true);
//
//                //TopLoc_Location aLocation;
//                //Handle(AIS_ConnectedInteractive) theTransformedDisplay = new AIS_ConnectedInteractive();
//                //theTransformedDisplay->Connect(aShapePrs, aLocation);
//
//                //Handle(Geom_Transformation) theMove = new Geom_Transformation(aLocation.Transformation());
//                //myContext->Display(theTransformedDisplay, Standard_False);
//                //myContext->Display(aShapePrs, Standard_False);
//            }
//
//            //Handle(AIS_InteractiveObject) io = myContext->SelectedInteractive();
//            //myContext->SetColor(io, CSFColor, Standard_False);
//            //io->SetColor(CSFColor);
//        }
//        //myContext->UpdateCurrentViewer();
//        m3dOcafDoc->CommitCommand();
//    }
//
//    UpdateDisplay();
//}
//
//void DocumentCommon::MakeBox()
//{
//    BRepPrimAPI_MakeBox mkBox(gp_Pnt(0, 0, 0), 20, 50, 10);
//    TopoDS_Shape ResultShape = mkBox.Shape();
//    Handle(AIS_Shape) myAISObject = new AIS_Shape(ResultShape);
//    myContext->Display(myAISObject, false);
//}
//
//void DocumentCommon::ExportBREP()
//{
//    Handle(TopTools_HSequenceOfShape) shapes;
//    //遍历myContext实体?????????????????
//    //暂时用选择集来
//    Handle(TopTools_HSequenceOfShape) aSequence;
//    //除非先设置全部实体被选中
//    bool bHavenoSelects = true;
//    for (myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
//    {
//        Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
//        if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
//        {
//            TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
//            if (shapes.IsNull())
//                shapes = new TopTools_HSequenceOfShape();
//            shapes->Append(shape);
//        }
//
//        bHavenoSelects = false;
//    }
//    if(bHavenoSelects)
//    {
//        return;
//    }
//
//    Translate* anTrans = new Translate(this);
//    anTrans->ExportBREP(shapes);
//    delete anTrans;
//}

//向文档设置主Solid
//void DocumentCommon::SetMainShape(const TopoDS_Shape& aSolid)
//{
//    //清除
//    myContext->RemoveAll(false);
//
//    //设置
//    m3dOcafDoc->NewCommand();
//    m_pComDS->SetMainSolid(aSolid);
//    m3dOcafDoc->CommitCommand();
//}

////更新显示，根据文档的内容
//void DocumentCommon::UpdateDisplay()
//{
//    //获得Shape，即主Solid，并显示
//    Graphic3d_NameOfMaterial Material;
//    TopoDS_Shape aShape = m_pComDS->GetMainSolid(Material);
//    if (aShape.IsNull())
//    {
//        myContextIsEmpty = true;
//        return;
//    }
//    myContextIsEmpty = false;
//
//    Handle(AIS_Shape) myAISObject = new AIS_Shape(aShape);
//    //设置材料
//    myContext->SetMaterial(myAISObject, Material, Standard_False);
//    //设置颜色
//    Quantity_Color CFL(g_fMaterialColor[Material][0], g_fMaterialColor[Material][1], g_fMaterialColor[Material][2], Quantity_TOC_RGB);
//    myContext->SetColor(myAISObject, CFL, Standard_False);
//    //设置透明度
//    double aTransparency = g_fMaterialTransparency[Material];
//    myContext->SetTransparency(myAISObject, aTransparency, Standard_False);
//    myContext->Display(myAISObject, false);
//
//    //获取颜色设置，并显示
//    map<string, vector<double> > mpColors = m_pComDS->GetColorMap();
//    map<string, vector<double> >::iterator itre = mpColors.begin();
//    for (; itre != mpColors.end(); ++itre)
//    {
//        string sName = itre->first;
//        vector<double> vClr = itre->second;
//        TopoDS_Shape aShape = m_pComDS->GetShapeFromName(sName);
//        Quantity_Color clr(vClr[0], vClr[1], vClr[2], Quantity_TOC_RGB);
//        Handle(AIS_Shape) aShapePrs = new AIS_Shape(aShape);
//        aShapePrs->SetColor(clr);
//        myContext->Display(aShapePrs, false);
//    }
//
//    //获取约束，并显示???
//
//    //获取参考元素，并显示
//
//    //获得Connectors，并显示
//    myContext->UpdateCurrentViewer();
//}

/*
void DocumentCommon::SaveShapes(Handle(TopTools_HSequenceOfShape) theShapes)
{
	m3dOcafDoc->NewCommand();

	assert(theShapes->Length() == 1);
	for (int i = 1; i <= theShapes->Length(); i++)
	{
		//对theShapes[i]的每一个顶点、边、面进行处理
		TopoDS_Shape aShape = theShapes->Value(i);
		TopExp_Explorer Ex;
		//遍历所有顶点
		int iIndex = 0; TopoDS_Shape vtx0;
		for (Ex.Init(aShape, TopAbs_VERTEX); Ex.More(); Ex.Next()) {
			TopoDS_Shape vtx_i = Ex.Current();
			m_pComDS->AddShape(myShapeType::NM_VERTEX, vtx_i);
			if (iIndex == 0)
			{
				vtx0 = vtx_i;
			}
			else {
				bool b0;// = vtx0.IsEqual(vtx_i);
				//if (b0)
				//{
				//    assert(false);
				//}
				b0 = vtx0.IsSame(vtx_i);
				//if (b0)
				//{
				//    assert(false);
				//}
				//b0 = vtx0.IsPartner(vtx_i);
				//if (b0)
				//{
				//    assert(false);
				//}
			}
			iIndex++;
		}
		//遍历所有bian
		iIndex = 0;
		for (Ex.Init(aShape, TopAbs_EDGE); Ex.More(); Ex.Next()) {
			TopoDS_Shape vtx_i = Ex.Current();
			m_pComDS->AddShape(myShapeType::NM_EDGE, vtx_i);
			if (iIndex == 0)
			{
				vtx0 = vtx_i;
			}
			else {
				bool b0 = vtx0.IsSame(vtx_i);
				if (b0)
				{
				   //assert(false);
				}
			}
			iIndex++;
		}
		//遍历所有的面
		int iChild = 1;
		for (Ex.Init(aShape, TopAbs_FACE); Ex.More(); Ex.Next()) {
			TopoDS_Shape face_i = Ex.Current();
			m_pComDS->AddShape(myShapeType::NM_FACE, face_i);
		}

		//加入体
		assert(aShape.ShapeType() == TopAbs_SOLID);
		if (aShape.ShapeType() == TopAbs_SOLID)
		{
			m_pComDS->AddShape(myShapeType::NM_SOLID, aShape);
		}
	}
	m3dOcafDoc->CommitCommand();
}
*/

void DocumentCommon::ShowShapes(Handle(TopTools_HSequenceOfShape) theShapes)
{
	for (int i = 1; i <= theShapes->Length(); i++)
	{
		TopoDS_Shape aShape = theShapes->Value(i);
		Handle(AIS_Shape) myAISObject = new AIS_Shape(aShape);
		myContext->Display(myAISObject, false);
	}
}

void DocumentCommon::GetAx3Length(double& len, double& arrowLen)
{
	//myViewer->InitActiveViews();
	//Handle(V3d_View) view = myViewer->ActiveView();
	double scale = 1; // view->Scale();
	len = 0.3 * scale;
	arrowLen = 0.1 * scale;

	//需要从设置中获得
	len = LibPathsSetDlg::GetLocalSysSize();
	arrowLen = 0.3 * len;
}

double DocumentCommon::GetConnectorBaseLength()
{	
	//myViewer->InitActiveViews();
	//Handle(V3d_View) view = myViewer->ActiveView();
	double scale = 1;// view->Scale();
	//需要从设置中获得
	//return 0.01;

	return LibPathsSetDlg::GetConnSize();
}

//从父节点找下面子节点，形状为theShape的节点Label
////nth返回第几个;
//TDF_Label DocumentCommon::LookupForShapeLabelFromFather(const TDF_Label& father, const TopoDS_Shape& theShape, int& nth)
//{
//	TDF_Label retLabel;
//
//	Standard_Integer ncount = father.NbChildren();
//	for (int i = 0; i < ncount; i++)
//	{
//		TDF_Label lbl_i = father.FindChild(i + 1, false);
//		Handle(TNaming_NamedShape) aNamedShape1;
//		lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
//		if (aNamedShape1.IsNull())
//		{
//			continue;
//		}
//		TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);
//
//		if (theShape.IsSame(shapej))
//		{//找到了形状
//			retLabel = lbl_i;
//			nth = i + 1;
//			break;
//		}
//	}
//
//	return retLabel;
//}

//两个static函数，用于转换
string DocumentCommon::GetShapeNameFromID(vector<int>& theID)
{
	string retStr;

	if (theID.size() == 0)
	{
		return retStr;
	}

	int nShapeType = theID[0];

	switch (nShapeType)
	{
	case 1:
	{
		retStr = "Vertex_";
		break;
	}
	case 2:
	{
		retStr = "Edge_";
		break;
	}
	case 3:
	{
		retStr = "Face_";
		break;
	}
	case 4:
	{
		retStr = "Solid_";
		break;
	}
	case 5:
	{
		//assert(false);//此处有改动
		retStr = "Connector_";
		break;
	}
	case 6:
	{
		retStr = "DatumPoint_";
		break;
	}
	case 7:
	{
		retStr = "DatumLine_";
		break;
	}
	case 8:
	{
		retStr = "DatumPlane_";
		break;
	}default:
		break;
	}

	retStr += std::to_string(theID[1]);

	return retStr;
}

vector<int> DocumentCommon::GetShapeIDFromName(string& theName)
{
	vector<int> vRet;

	int pos = theName.find('_');

	string leftType = theName.substr(0, pos);
	if (leftType == "Vertex")
	{
		vRet.push_back(1);
	}
	else if (leftType == "Edge")
	{
		vRet.push_back(2);
	}
	else if (leftType == "Face")
	{
		vRet.push_back(3);
	}
	else if (leftType == "Solid")
	{
		vRet.push_back(4);
	}
	else if (leftType == "Connector")
	{
		vRet.push_back(5);
	}
	else if (leftType == "DatumPoint")
	{
		vRet.push_back(6);
	}
	else if (leftType == "DatumLine")
	{
		vRet.push_back(7);
	}
	else if (leftType == "DatumPlane")
	{
		vRet.push_back(8);
	}

	string sRightNum = theName.substr(pos + 1, theName.length());
	int num = atoi(sRightNum.c_str());
	vRet.push_back(num);

	return vRet;
}

string DocumentCommon::GetShapeName(const TopoDS_Shape& shp) const
{
	return "";
}

//将位图转换成字节流，存到root
void DocumentCommon::SavePixmap(QPixmap& pm)
{
	QByteArray barr = QByteArray();
	QBuffer buf(&barr);
	bool b0 = buf.open(QIODevice::WriteOnly);
	bool b1 = pm.save(&buf,"PNG");

	//可以用byteArray，这里转换QString
	//QString s = QString(barr.toBase64());
	QString s = QString(buf.data().toBase64());

	TDataStd_Name::Set(m_pDocRoot, s.toStdU16String().data());
}

//从文档root获得位图
QPixmap DocumentCommon::GetPixmap() const
{
	QPixmap pm;

	Handle(TDataStd_Name) nameAttr;
	m_pDocRoot.FindAttribute(TDataStd_Name::GetID(), nameAttr);

	if (nameAttr.IsNull())
	{
		//测试
		bool bload = pm.load("D:/M3drepos/M3d/M3dStudio/images/lamp.png");
		return pm;
	}

	TCollection_ExtendedString s = nameAttr->Get();
	string sData = ExtString2string(s);

	QByteArray byteArray = QByteArray::fromBase64(sData.c_str());//???
	pm.loadFromData((const uchar*)byteArray.data(), byteArray.size());

	return pm;
}

void DocumentCommon::ShowOxyz(bool bShow)
{
	m_bShowOxyz = bShow;

	UpdateDisplay(false);
}