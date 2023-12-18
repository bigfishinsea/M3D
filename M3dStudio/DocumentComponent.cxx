//2021.09.16
//Wu YZ
#include "M3d.h"
#include "DocumentComponent.h"
#include "Transparency.h"
#include <QInputDialog>
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
#include <TDataStd_BooleanArray.hxx>
#include <TDataStd_IntegerArray.hxx>
#include <TNaming_NamedShape.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Real.hxx>
#include <TDataStd_ExtStringArray.hxx>
#include <TDataStd_Name.hxx>
#include <TFunction_Function.hxx>
#include <TFunction_DriverTable.hxx>
#include <TDF_ChildIterator.hxx>
#include <qfiledialog.h>
#include "mainwindow.h"
#include "translate.h"
#include <BinDrivers.hxx>
#include <TNaming_Builder.hxx>
#include <qmessagebox.h>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TNaming_Tool.hxx>
#include <GProp_GProps.hxx>
#include <GProp_PrincipalProps.hxx>
#include <gp_Pnt.hxx>
#include <BRepGProp.hxx>
#include <gp_Lin2d.hxx>
#include <GCE2d_MakeSegment.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Edge.hxx>
#include <BRepTools.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GC_MakeSegment.hxx>
//
#include <PCDM_RetrievalDriver.hxx>
#include <PCDM_StorageDriver.hxx>
#include <AIS_ConnectedInteractive.hxx>
#include <Geom_Transformation.hxx>
#include <TopExp_Explorer.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Reference.hxx>
#include <Standard_WarningsRestore.hxx>
#include <Prs3d_LineAspect.hxx>
#include "AdaptorVec_AIS.h"
#include "global.h"
#include "Component.h"
#include <BRepBuilderAPI_Transform.hxx>
#include <QMessageBox.h>
#include <AIS_ViewCube.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepOffsetAPI_MakePipe.hxx>
#include <gp_Circ.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <Geom_BSplineCurve.hxx>
#include <AIS_TexturedShape.hxx>
#include "LibPathsSetDlg.h"

//#include <BRepTools.hxx>
//#include <iostream>
//#include <fstream>
//using namespace std;

/////////////////////////////////////////////////////////////
//贴图
Handle(AIS_TexturedShape) Texturize(const TopoDS_Shape& aShape, TCollection_AsciiString aTFileName, \
	Standard_Real toScaleU = 1, Standard_Real toScaleV = 1, Standard_Real toRepeatU = 1, Standard_Real toRepeatV = 1, \
	Standard_Real originU = 1, Standard_Real originV = 1)
{
	// create a textured presentation object for aShape
	Handle(AIS_TexturedShape) aTShape = new AIS_TexturedShape(aShape);
	aTShape->SetTextureFileName(aTFileName);

	// do other initialization of AIS_TexturedShape

	aTShape->SetTextureMapOn();
	aTShape->SetTextureScale(Standard_True, toScaleU, toScaleV);
	aTShape->SetTextureRepeat(Standard_True, toRepeatU, toRepeatV);
	aTShape->SetTextureOrigin(Standard_True, originU, originV);
	aTShape->SetDisplayMode(3); // mode 3 is "textured" mode
	
	// 将底色设置为白色，三个分量的取值区间均为[0, 1]
	vector<int> vClr = { 1, 1, 1 };
	Quantity_Color clr(vClr[0], vClr[1], vClr[2], Quantity_TOC_RGB);
	aTShape->SetColor(clr);

	return aTShape;
}

// =======================================================================
// function : Viewer
// purpose  :
// =======================================================================
DocumentComponent::DocumentComponent()
{
	m_IsModel = false;
}

DocumentComponent::DocumentComponent(MainWindow* theApp) 
    : DocumentCommon(theApp)
{
    createNewDoc();
}

DocumentComponent::~DocumentComponent()
{

}

void DocumentComponent::createNewDoc()
{
	DocumentCommon::createNewDoc();
    //anOcaf_Application = new TDocStd_Application;
    //anOcaf_Application->DefineFormat("m3dmdl", "M3d system Format", "m3dmdl"/*, \
    //    new NewDocumentFormat_RetrievalDriver(), new NewDocumentFormat_StorageDriver()*/,nullptr,nullptr);

    //anOcaf_Application->DefineFormat("m3dcom", "M3d Component Format", "m3dcom"/*, \
    //    new NewDocumentFormat_RetrievalDriver(), new NewDocumentFormat_StorageDriver()*/, nullptr, nullptr);

    //anOcaf_Application->NewDocument("m3dcom"/*"BinOcaf"*/, m3dOcafDoc);//先缺省，后再改
    //TPrsStd_AISViewer::New(m3dOcafDoc->Main(), myViewer);

    //m_pDS = new DocumentComponent(m3dOcafDoc->Main());

    ////Handle(AIS_InteractiveContext) anAisContext;
    //TPrsStd_AISViewer::Find(m3dOcafDoc->Main(), myContext/*anAisContext*/);
    ////anAisContext->SetDisplayMode(AIS_Shaded, Standard_True);
    //myContext->SetDisplayMode(AIS_Shaded, Standard_True);
    ////myContext = anAisContext;

    //// Set the maximum number of available "undo" actions
    //m3dOcafDoc->SetUndoLimit(30);
}

//显示形状
void DocumentComponent::DisplayShapes(vector<TopoDS_Shape>& vShapes)
{
	DocumentCommon::DisplayShapes(vShapes);
    //vector<TopoDS_Shape>::iterator iter;
    //for (iter = vShapes.begin(); iter != vShapes.end(); ++iter)
    //{
    //    TopoDS_Shape aShape = *iter;
    //    Handle(AIS_InteractiveObject) anAisIO = new AIS_Shape(aShape);
    //    myContext->Display(anAisIO, false);
    //}
}

//打开加载文档
bool DocumentComponent::loadFile(const QString fileName)
{
	return DocumentCommon::loadFile(fileName);
    //////测试
    ////BRepPrimAPI_MakeBox mkBox(gp_Pnt(0, 0, 0), 20, 50, 10);
    ////TopoDS_Shape ResultShape = mkBox.Shape();
    ////Handle(AIS_Shape) myAISObject = new AIS_Shape(ResultShape);
    ////myContext->Display(myAISObject, false);

    ////Handle(TDocStd_Application) anOcaf_Application = new TDocStd_Application;
    //// load persistence
    //BinDrivers::DefineFormat(anOcaf_Application);
    //// Look for already opened
    //if (anOcaf_Application->IsInSession(fileName.toStdU16String().data()))
    //{
    //    //myResult << "Document: " << myFileName << " is already in session" << std::endl;
    //    return false;
    //}
    //// Open the document in the current application
    //PCDM_ReaderStatus aReaderStatus = anOcaf_Application->Open(fileName.toStdU16String().data(), m3dOcafDoc); //toUtf8()
    //if (aReaderStatus == PCDM_RS_OK)
    //{
    //    //myContext->RemoveAll(false);
    //    //TPrsStd_AISViewer::New(m3dOcafDoc->Main(), myViewer);
    //    //TPrsStd_AISViewer::Find(m3dOcafDoc->Main(), myContext);

    //    //myContext->SetAutoActivateSelection(true);
    //    // Retrieve the current viewer selector
    //    // const Handle(StdSelect_ViewerSelector3d)& aMainSelector = myContext->MainSelector();
    //    // Set the flag to allow overlap detection
    //    //aMainSelector->AllowOverlapDetection(true);
    //    //myContext->SetDisplayMode(AIS_Shaded, Standard_True);

    //    ////测试
    //    //BRepPrimAPI_MakeBox mkBox(gp_Pnt(0, 0, 0), 20, 30, 40);
    //    //TopoDS_Shape ResultShape = mkBox.Shape();
    //    //Handle(AIS_Shape) myAISObject = new AIS_Shape(ResultShape);
    //    //myContext->Display(myAISObject, false);
    //    //myContext->Activate(myAISObject, 0);

    //    // Set the maximum number of available "undo" actions
    //    m_pDS->SetRoot(m3dOcafDoc->Main());
    //    m3dOcafDoc->SetUndoLimit(30);

    //    // Display the presentations (which was not stored in the document)
    //    //vector<TopoDS_Shape> vShapes = m_pComDS->GetAllShapes();
    //    //DisplayShapes(vShapes);
    //    //DisplayPresentation();
    //    UpdateDisplay();
    //    
    //    myfileName = fileName;
    //    return true;
    //}
    //else
    //{
    //    //myResult << "Error! The file wasn't opened. PCDM_ReaderStatus: " << aReaderStatus << std::endl;
    //    return false;
    //}
}

bool DocumentComponent::save()
{
	return DocumentCommon::save();

    //if (myfileName.isEmpty())
    //{
    //   return saveAs();
    //}

    //return saveFile(myfileName);
}

bool DocumentComponent::saveAs()
{
	return DocumentCommon::saveAs();

    //QWidget* mainWnd = (QWidget*)parent();
    //QString qsFilter = "零件(*.m3dcom)";
    //if (nDocType == 2)
    //{
    //    qsFilter = "系统(*.m3dmdl)";
    //}

    //QString gfileName = QFileDialog::getSaveFileName(mainWnd,QString(),QString(),qsFilter);
    //if (!gfileName.isEmpty())
    //{
    //    //fileName = gfileName;
    //    //自动追加后缀
    //    if (nDocType == 1) {
    //        if (QFileInfo(gfileName).suffix().toLower() != "m3dcom")
    //        {
    //            gfileName += ".m3dcom";
    //        }
    //    }
    //    else if (nDocType == 2)
    //    {
    //        if (QFileInfo(gfileName).suffix().toLower() != "m3dmdl")
    //        {
    //            gfileName += ".m3dmdl";
    //        }
    //    }

    //    return saveFile(gfileName);
    //}

    //return false;
}

//从文档存取形状
void DocumentComponent::serializeShapes(Handle(TopTools_HSequenceOfShape) theShapes, bool bSave)
{
	DocumentCommon::serializeShapes(theShapes, bSave);

    //if (bSave)
    //{
    //    //将形状数据存入文档
    //    m3dOcafDoc->NewCommand();
    //    for (int i = 1; i <= theShapes->Length(); i++)
    //    {
    //        TDF_Label aLabel = TDF_TagSource::NewChild(m3dOcafDoc->Main());
    //        TDataStd_Name::Set(aLabel, "Naming_NamedShapes");

    //        //对theShapes[i]的每一个顶点、边、面进行处理
    //        TopoDS_Shape aShape = theShapes->Value(i);
    //        //先遍历所有的面
    //        TopExp_Explorer Ex;
    //        int iChild = 1;
    //        for (Ex.Init(aShape, TopAbs_FACE); Ex.More(); Ex.Next()) {
    //            TopoDS_Shape face_i = Ex.Current();
    //            TDF_Label lab_i = aLabel.FindChild(iChild++);
    //            TNaming_Builder B_i(lab_i);
    //            B_i.Generated(face_i);

    //            //加入显示
    //            Handle(AIS_Shape) ais_i = new AIS_Shape(face_i);
    //            myContext->Display(ais_i, false);
    //        }

    //        //原来代码注释掉
    //        //TNaming_Builder B(aLabel);
    //        //B.Generated(theShapes->Value(i));

    //        //// Get the TPrsStd_AISPresentation of the new box TNaming_NamedShape
    //        //Handle(TPrsStd_AISPresentation) anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
    //        //// Display it
    //        //anAisPresentation->Display(1);
    //        //// Attach an integer attribute to aLabel to memorize it's displayed
    //        //TDataStd_Integer::Set(aLabel, i);
    //    }

    //    myContext->UpdateCurrentViewer();

    //    m3dOcafDoc->CommitCommand(); 
    //}
    //else {
    //    //从文档读出形状

    //}
}

//bool 保存
bool DocumentComponent::saveFile(const QString gfileName)
{
	//检查模型是否正确
	vector<Connector*> vConns = GetConnectors();
	CyberInfo* pCyber = GetCyberInfo();
	vector<string> vConnNames = pCyber->GetConnectorNames();
	if (vConns.size() != vConnNames.size())
	{
		//QMessageBox::information(NULL, "错误", "接口不匹配", QMessageBox::Ok);
		if(QMessageBox::information(NULL, "错误", "接口个数不匹配，是否继续保存？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
			return false;
	}

	vector<string> vConnTypes = pCyber->GetConnectorTypes();
	for (int i = 0; i < vConns.size(); i++)
	{
		Connector* pConn = vConns[i];
		if (std::find(vConnTypes.begin(), vConnTypes.end(), pConn->GetConnType()) == vConnTypes.end())
		{
			if (QMessageBox::information(NULL, "错误", "接口类型不匹配，是否继续保存？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
				return false;
		}
		if (std::find(vConnNames.begin(), vConnNames.end(), pConn->GetConnName()) == vConnNames.end())
		{
			if (QMessageBox::information(NULL, "错误", "接口名称不匹配，是否继续保存？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
				return false;
		}
	}

    return DocumentCommon::saveFile(gfileName);

    //////先将当前几何注入文档
    ////TDF_Label aLabel = TDF_TagSource::NewChild(m3dOcafDoc->Main());
    ////TNaming_Builder B(aLabel);
    ////Handle(TopTools_HSequenceOfShape) shapes = Translate::getShapes(myContext);
    ////if (shapes)
    ////{
    ////    for (int i = 1; i <= shapes->Length(); i++)
    ////    {
    ////        B.Generated(shapes->Value(i));
    ////    }
    ////}

    ////Handle(TDocStd_Application) anOcaf_Application = new TDocStd_Application;
    //BinDrivers::DefineFormat(anOcaf_Application);
    //m3dOcafDoc->ChangeStorageFormat("BinOcaf"); //自动追加??
    ////先检查是否存在，如果有提醒覆盖
    ////此处覆盖，会产生*.cbf的后缀！！！！！bug  
    //// Saves the document in the current application
    //PCDM_StoreStatus aStoreStatus = anOcaf_Application->SaveAs(m3dOcafDoc, gfileName.toStdU16String().data()); //toutf8
    //if (aStoreStatus == PCDM_SS_OK)
    //{
    //    //去掉.cbf
    //    QString f0 = gfileName;
    //    f0 += ".cbf"; 
    //    QFile file0(f0);
    //    QFile::remove(gfileName);
    //    file0.rename(gfileName);
    //    myfileName = gfileName;
    //    QString sTitle = tr("M3d建模与仿真一体化平台——");
    //    sTitle += myfileName;
    //    QWidget* mainWnd = (QWidget*)parent();
    //    mainWnd->setWindowTitle(sTitle);
    //    return true;
    //}
    //else
    //{
    //    QMessageBox::information(NULL, "提示", "存储错误", QMessageBox::Ok);
    //    return false;
    //}
}

void DocumentComponent::undo()
{
	DocumentCommon::undo();

    //if (m3dOcafDoc->Undo())
    //{
    //    m3dOcafDoc->CommitCommand();
    //    //从label中读取shapes刷新显示
    //    myContext->RemoveAll(Standard_False);
    //    //Handle(AIS_InteractiveContext) aContext;
    //    //TPrsStd_AISViewer::Find(m3dOcafDoc->Main(), aContext);
    //    //aContext->SetDisplayMode(AIS_Shaded, Standard_True);
    //    //myContext = aContext;
    //    
    //    //DisplayPresentation();
    //    UpdateDisplay();

    //    myContext->UpdateCurrentViewer();
    //}
}

bool DocumentComponent::canUndo()
{
	return DocumentCommon::canUndo();

    //return (!m3dOcafDoc->GetUndos().IsEmpty());
}

void DocumentComponent::redo()
{
	DocumentCommon::redo();
    //if (m3dOcafDoc->Redo())
    //{
    //    m3dOcafDoc->CommitCommand();

    //    myContext->RemoveAll(Standard_False);

    //    //DisplayPresentation();
    //    UpdateDisplay();

    //    myContext->UpdateCurrentViewer();
    //}
}

bool DocumentComponent::canRedo()
{
	return DocumentCommon::canRedo();
    //return (!m3dOcafDoc->GetRedos().IsEmpty());
}

//void DocumentComponent::DisplayPresentation()
//{
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
//    //TDF_Label theShapes_Lbl = aRootlabel.FindChild(1);
//    //显示所有Shapes，缺省颜色;
//    for (TDF_ChildIterator it(theShapes_Lbl); it.More(); it.Next())
//    {
//        TDF_Label aLabel = it.Value();
//        Handle(TNaming_NamedShape) aNamedShape;
//        if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
//        {
//            continue;
//        }
//        
//        //Handle(TDataStd_Integer) aDataInteger;
//        //// To know if the object was displayed
//        //if (aLabel.FindAttribute(TDataStd_Integer::GetID(), aDataInteger))
//        //{
//        //    if (!aDataInteger->Get())
//        //    {
//        //        continue;
//        //    }
//        //}
//
//        TopoDS_Shape shape = TNaming_Tool::GetShape(aNamedShape);
//        Handle(AIS_InteractiveObject) anAisIO = new AIS_Shape(shape);
//
//        ////如果有颜色属性，则显示颜色;
//        //Handle(TDataStd_RealArray) clorAttr;
//        //if (aLabel.FindAttribute(TDataStd_RealArray::GetID(), clorAttr))
//        //{
//        //    Quantity_Color CSFColor(clorAttr->Value(1), clorAttr->Value(2), clorAttr->Value(3), Quantity_TOC_RGB);
//        //    //Handle(AIS_Shape) aShapePrs = new AIS_Shape(shape);
//        //    //aShapePrs->SetColor(CSFColor);
//        //    //myContext->Display(aShapePrs, false);
//        //    anAisIO->SetColor(CSFColor);
//        //}
//
//        //Handle(TDataStd_Real) realRed;
//        //if (aLabel.FindAttribute(TDataStd_Real::GetID(), realRed))
//        //{
//        //    Quantity_Color CSFColor(realRed->Get(), 0.0, 0.0, Quantity_TOC_RGB);
//        //    //Handle(AIS_Shape) aShapePrs = new AIS_Shape(shape);
//        //    //aShapePrs->SetColor(CSFColor);
//        //    //myContext->Display(aShapePrs, false);
//        //    anAisIO->SetColor(CSFColor);
//        //}
//        myContext->Display(anAisIO, false);
//
//        //filebuf fb;
//        //fb.open("text.txt", ios::out);
//        //Standard_OStream os(&fb);
//        //BRepTools::Dump(shape, os);
//        //fb.close();
//        //break;
//
//        ////源代码，注释
//        //Handle(TPrsStd_AISPresentation) anAisPresentation;
//        //if (!aLabel.FindAttribute(TPrsStd_AISPresentation::GetID(), anAisPresentation))
//        //{
//        //    anAisPresentation = TPrsStd_AISPresentation::Set(aLabel, TNaming_NamedShape::GetID());
//        //}
//        //anAisPresentation->SetColor(Quantity_NOC_ORANGE);
//        //anAisPresentation->Display(1);
//
//        //Handle(TDF_Attribute) attr;
//        //if (aLabel.FindAttribute(it.Value(), attr))
//        //{
//        //    TopoDS_Shape shape = Handle(TNaming_NamedShape)::DownCast(attr)->Get();
//        //    myContext->Display(new AIS_Shape(shape), false);
//        //}
//    }
//
//    //显示参考面的颜色
//    TDF_Label colorLbl = aRootlabel.FindChild(10);
//    for (TDF_ChildIterator it(colorLbl); it.More(); it.Next())
//    {
//        TDF_Label clr_i = it.Value();
//        //得到它的引用TDF
//        Handle(TDF_Reference) refClr;
//        clr_i.FindAttribute(TDF_Reference::GetID(), refClr);
//        //得到引用TDF的Shape
//        TDF_Label orign = refClr->Get();
//        //显示该Shape的颜色
//        Handle(TNaming_NamedShape) aNamedShape1;
//        orign.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
//        TopoDS_Shape shape = TNaming_Tool::GetShape(aNamedShape1);
//        Handle(AIS_InteractiveObject) anAisI1 = new AIS_Shape(shape);
//        
//        Handle(TDataStd_RealArray) clorAttr;
//        clr_i.FindAttribute(TDataStd_RealArray::GetID(), clorAttr);
//        Quantity_Color CSFColor(clorAttr->Value(1), clorAttr->Value(2), clorAttr->Value(3), Quantity_TOC_RGB);
//        anAisI1->SetColor(CSFColor);
//        myContext->Display(anAisI1, false);
//    }
//
//    myContext->UpdateCurrentViewer();
//}

//void DocumentCommon::detect(int x, int y, bool b2)
//{
//    myContext->MoveTo(x, y, myViewer, b2);
//}

void DocumentComponent::setSelectMode(QString sMode)
{
	DocumentCommon::setSelectMode(sMode);

    ////myContext->ClearSelected(true);
    //myContext->Deactivate();
    //myContext->SetAutomaticHilight(true);

    //if (sMode == "顶点")
    //{
    //    // activates decomposition of shapes into faces
    //     myContext->Activate(AIS_Shape::SelectionMode(TopAbs_VERTEX));
    //    //const int aSubShapeSelMode = AIS_Shape::SelectionMode(TopAbs_Face);
    //    //myContext->Activate(aShapePrs, aSubShapeSelMode);
 
    //    //myContext->SetSelectionModeActive(OCTopAbs_ShapeEnum.TopAbs_VERTEX);
    //}
    //else if (sMode == "棱边")
    //{
    //    myContext->Activate(AIS_Shape::SelectionMode(TopAbs_EDGE));
    //}
    //else if (sMode == "面")
    //{
    //    myContext->Activate(AIS_Shape::SelectionMode(TopAbs_FACE));
    //}
    //else if (sMode == "体")
    //{
    //    myContext->Activate(AIS_Shape::SelectionMode(TopAbs_SOLID));
    //}
    //else if (sMode == "任一")
    //{
    //    myContext->Activate(AIS_Shape::SelectionMode(TopAbs_SHAPE));
    //}
    //else //接口
    //{

    //}
}

////向文档增加一个颜色节点，增加其引用label和颜色值;
//void DocumentComponent::AddRefColor(TDF_Label& aLabel0, Quantity_Color& aClr)
//{
//    TDF_Label aLabel = m3dOcafDoc->Main();
//    //TDataStd_Name::Set(aLabel, "Refer_Colors");
//
//    //第10个存储了颜色;
//    TDF_Label colorLbl = aLabel.FindChild(10);
//    TDF_Label newChild = colorLbl.NewChild();
//    Handle(TDF_Reference) newChildRef = TDF_Reference::Set(newChild, aLabel0);
//    Handle(TDataStd_RealArray) aRealArray_Color = TDataStd_RealArray::Set(newChild, 1, 3);
//    aRealArray_Color->SetValue(1, aClr.Red());
//    aRealArray_Color->SetValue(2, aClr.Green());
//    aRealArray_Color->SetValue(3, aClr.Blue());
//}


void DocumentComponent::SetObjColor(TopoDS_Shape& aShape, Quantity_Color& aClr)
{
    SetShapeColor(aShape, aClr);
    /*
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
    if (theShapes_Lbl.IsNull())
    {
        assert(false);
        return;
    }

    for (TDF_ChildIterator it(theShapes_Lbl); it.More(); it.Next())
    {
        TDF_Label aLabel = it.Value();
        Handle(TNaming_NamedShape) aNamedShape;
        if (!aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape))
        {
            continue;
        }
        TopoDS_Shape shape = TNaming_Tool::GetShape(aNamedShape);

        //找到了这个shape,增加一个颜色属性
        if (aShape == shape)
        {
            //Handle(TDataStd_RealArray) aRealArray_Color = TDataStd_RealArray::Set(aLabel, 1, 3);
            //aRealArray_Color->SetValue(1, aClr.Red());
            //aRealArray_Color->SetValue(2, aClr.Green());
            //aRealArray_Color->SetValue(3, aClr.Blue());

            //添加颜色引用
            AddRefColor(aLabel, aClr);

            //TDataStd_Real::Set(aLabel, 1.0);
        }
    }
    */
}

void DocumentComponent::SetSelectedMaterial(int iMaterial)
{
    //myContext->SetDisplayMode(1, false);
    Handle(AIS_InteractiveObject) Current;
    QColor MSColor;
    myContext->InitSelected();
    Current = myContext->SelectedInteractive();
    if (!Current)
    {
        QMessageBox::information(NULL, "提示", "请选择图元", QMessageBox::Ok);
        return;
    }
    //
    Handle(AIS_Shape) myAISObject = Handle(AIS_Shape)::DownCast(Current);
    //设置材料
    myContext->SetMaterial(myAISObject, (Graphic3d_NameOfMaterial)iMaterial, Standard_False);
    //设置颜色
    Standard_Real C1 = g_vMaterials[iMaterial].v3MaterialColor[0];
    Standard_Real C2 = g_vMaterials[iMaterial].v3MaterialColor[1];
    Standard_Real C3 = g_vMaterials[iMaterial].v3MaterialColor[2];
    Quantity_Color CFL(C1, C2, C3, Quantity_TOC_RGB);
    myContext->SetColor(myAISObject, CFL, Standard_False);
    //设置透明度
    double aTransparency = g_vMaterials[iMaterial].fMaterialTransparency / 100;
    myContext->SetTransparency(myAISObject, aTransparency, Standard_False);
    myContext->Display(myAISObject, true);

    //文档操作
    SetMaterial((Graphic3d_NameOfMaterial)iMaterial);
}

void DocumentComponent::objColor()
{
    //myContext->SetDisplayMode(1, false);
    Handle(AIS_InteractiveObject) Current;
    QColor MSColor;
    myContext->InitSelected();
    Current = myContext->SelectedInteractive();
    if (!Current)
    {
        QMessageBox::information(NULL, "提示", "请选择图元", QMessageBox::Ok);
        return;
    }
    if (Current&&Current->HasColor()) {
        Quantity_Color CSFColor;
        myContext->Color(Current, CSFColor);
        MSColor = QColor(CSFColor.Red() * 255., CSFColor.Green() * 255., CSFColor.Blue() * 255.);
    }
    else {
        MSColor = QColor(255, 255, 255);
    }
    QColor color = QColorDialog::getColor(MSColor);
    if (color.isValid())
    {
        m3dOcafDoc->NewCommand();

        Quantity_Color CSFColor(color.red() / 255., color.green() / 255., color.blue() / 255., Quantity_TOC_RGB);
        for (; myContext->MoreSelected(); myContext->NextSelected())
        {
            TopoDS_Shape ats = myContext->SelectedShape();
            if (1) //ats.ShapeType() == TopAbs_FACE)
            {
                //不需要显示了，在UpdateDisyplay
                //Handle(AIS_Shape) aShapePrs = new AIS_Shape(ats);
                //aShapePrs->SetColor(CSFColor);
                SetObjColor(ats, CSFColor);
                //myContext->Redisplay(aShapePrs, true);

                //TopLoc_Location aLocation;
                //Handle(AIS_ConnectedInteractive) theTransformedDisplay = new AIS_ConnectedInteractive();
                //theTransformedDisplay->Connect(aShapePrs, aLocation);

                //Handle(Geom_Transformation) theMove = new Geom_Transformation(aLocation.Transformation());
                //myContext->Display(theTransformedDisplay, Standard_False);
                //myContext->Display(aShapePrs, Standard_False);
            }

            //Handle(AIS_InteractiveObject) io = myContext->SelectedInteractive();
            //myContext->SetColor(io, CSFColor, Standard_False);
            //io->SetColor(CSFColor);
        }
        //myContext->UpdateCurrentViewer();
        m3dOcafDoc->CommitCommand();
    }

    UpdateDisplay();
}

//创建弹簧
void DocumentComponent::MakeSpring()
{
	//先创建螺旋线 
	QString dlgTitle = "构造螺旋线";
	QString txtLabel = "输入R";
	float defaultValue = 0.5;
	float minValue = 0, maxValue = 100; //范围
	int decimals = 2;//小数点位数
	bool ok = false;
	float inputValue = QInputDialog::getDouble(NULL, dlgTitle, txtLabel,
		defaultValue, minValue, maxValue, decimals, &ok);
	if (!ok) //确认选择
		return;
	float R = inputValue;
		
	txtLabel = "输入圈数n";
	inputValue = QInputDialog::getDouble(NULL, dlgTitle, txtLabel,
		defaultValue, minValue, maxValue, decimals, &ok);
	if (!ok) //确认选择
		return;
	float n = inputValue;

	txtLabel = "输入高度h";
	inputValue = QInputDialog::getDouble(NULL, dlgTitle, txtLabel,
		defaultValue, minValue, maxValue, decimals, &ok);
	if (!ok) //确认选择
		return;
	float h = inputValue;
	//pipe构造弹簧
	txtLabel = "输入钢丝半径r";
	inputValue = QInputDialog::getDouble(NULL, dlgTitle, txtLabel,
		defaultValue, minValue, maxValue, decimals, &ok);
	if (!ok) //确认选择
		return;
	float r = inputValue;

	//构造插值B样条曲线来近似螺旋线
	TColgp_Array1OfPnt Pnts1(1, int(n*6));
	for (int i = 1; i <= int(n * 6); i++)
	{
		float Xi = R * cos((i-1) * M_PI / 3);
		float Yi = R * sin((i-1) * M_PI / 3);
		float Zi = (i-1) * h / n / 6;
		Pnts1(i) = gp_Pnt(Xi, Yi, Zi);
	}
	
	GeomAPI_PointsToBSpline PTBS1(Pnts1);
	Handle(Geom_BSplineCurve) C1 = PTBS1.Curve();

	//Handle_Geom_CylindricalSurface aCylinder = new Geom_CylindricalSurface(gp::XOY(), inputValue);
	//gp_Lin2d aLine2d(gp_Pnt2d(0.0, 0.0), gp_Dir2d(1.0, h/2./M_PI/R));
	//GCE2d_MakeSegment ams(aLine2d, 0.0, M_PI * 2.0*R*n + h); //大于实际长度
	//Handle(Geom2d_TrimmedCurve) aSegment = ams.Value();
	//TopoDS_Edge aHelixEdge = BRepBuilderAPI_MakeEdge(aSegment, aCylinder, 0.0, h);// 2. * M_PI).Edge();

	TopoDS_Edge aHelixEdge = BRepBuilderAPI_MakeEdge(C1).Edge();
	Handle(AIS_InteractiveObject) helixAIS = new AIS_Shape(aHelixEdge);
	myContext->Display(helixAIS, true);

	//TopoDS_Edge E = BRepBuilderAPI_MakeEdge(curve);
	TopoDS_Wire W = BRepBuilderAPI_MakeWire(aHelixEdge);
	gp_Circ c = gp_Circ(gp_Ax2(gp_Pnt(R, 0., 0.), gp_Dir(0.,1 /*n*2*M_PI*R*/,0 /*h*/)), r);
	TopoDS_Edge Ec = BRepBuilderAPI_MakeEdge(c);
	TopoDS_Wire Wc = BRepBuilderAPI_MakeWire(Ec);
	TopoDS_Face F = BRepBuilderAPI_MakeFace(gp_Pln(gp::ZOX()), Wc);
	Handle(AIS_InteractiveObject) circleAIS = new AIS_Shape(F);
	myContext->Display(circleAIS, true);
	TopoDS_Shape S = BRepOffsetAPI_MakePipe(W, F);

	//显示
	Handle(AIS_ColoredShape) myAISObject = new AIS_ColoredShape(S);
	myContext->Display(myAISObject, true);

	SetMainShape(S);
	UpdateDisplay();
}

void DocumentComponent::MakeBox()
{
	TopoDS_Compound theCompound0;
	BRep_Builder aBuilder0;
	aBuilder0.MakeCompound(theCompound0);

	TopoDS_Compound theCompound;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(theCompound);

	BRepPrimAPI_MakeBox mkBox(gp_Pnt(0, 0, 0), 20, 50, 10);
    TopoDS_Shape ResultShape1 = mkBox.Shape();
	BRepPrimAPI_MakeBox mkBox2(gp_Pnt(0, 50, 0), 30, 20, 50);
	TopoDS_Shape ResultShape2 = mkBox2.Shape();

	aBuilder.Add(theCompound, ResultShape1);
	aBuilder.Add(theCompound, ResultShape2);

	aBuilder0.Add(theCompound0, theCompound);
	BRepPrimAPI_MakeBox mkBox3(gp_Pnt(0, 0, 50), 20, 10, 100);
	TopoDS_Shape ResultShape3 = mkBox3.Shape();
	aBuilder0.Add(theCompound0, ResultShape3);

    Handle(AIS_ColoredShape) myAISObject = new AIS_ColoredShape(theCompound0);
	myAISObject->SetCustomColor(ResultShape3, Quantity_NOC_BLUE);
	myAISObject->SetCustomColor(ResultShape1, Quantity_NOC_RED);
	myAISObject->SetCustomColor(ResultShape2, Quantity_NOC_GREEN);

	Handle(AIS_InteractiveContext) context1 = new AIS_InteractiveContext(myViewer);
	context1->Display(myAISObject, true);
	context1->UpdateCurrentViewer();

	BRepPrimAPI_MakeBox mBox2(gp_Pnt(10, 0, 0), 20, 50, 20);
	TopoDS_Shape reShape2 = mBox2.Shape();
	Handle(AIS_InteractiveObject) aIO = new AIS_Shape(reShape2);
	Handle(AIS_InteractiveContext) context2 = new AIS_InteractiveContext(myViewer);
	context2->Display(aIO, true);
	context2->UpdateCurrentViewer();
}

void DocumentComponent::ExportBREP()
{
    Handle(TopTools_HSequenceOfShape) shapes;
    //遍历myContext实体?????????????????
    //暂时用选择集来
    Handle(TopTools_HSequenceOfShape) aSequence;
    //除非先设置全部实体被选中
    bool bHavenoSelects = true;
    for (myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
    {
        Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
        if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
        {
            TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
            if (shapes.IsNull())
                shapes = new TopTools_HSequenceOfShape();
            shapes->Append(shape);
        }

        bHavenoSelects = false;
    }
    if(bHavenoSelects)
    {
        return;
    }

    Translate* anTrans = new Translate(this);
    anTrans->ExportBREP(shapes);
    delete anTrans;
}

//向文档设置主Solid
void DocumentComponent::SetMainShape(const TopoDS_Shape& aSolid)
{
    //清除
    myContext->RemoveAll(false);

    //设置
    m3dOcafDoc->NewCommand();
    //清空文档内容
    Clear();
    //设置主实体
    SetMainSolid(aSolid);

    m3dOcafDoc->CommitCommand();
}

//更新显示，根据文档的内容
//此处b没用到
void DocumentComponent::UpdateDisplay(bool b)
{
    //获得Shape，即主Solid，并显示
    Graphic3d_NameOfMaterial iMaterial;
    TopoDS_Shape aShape = GetMainSolid(iMaterial);
    if (aShape.IsNull())
    {
		myContext->RemoveAll(true);
        myContextIsEmpty = true;

		DisplayGlobalCoordsys();
        return;
    }
    myContextIsEmpty = false;
    myContext->RemoveAll(true);

    Handle(AIS_ColoredShape) myAISObject = new AIS_ColoredShape(aShape);
    //设置材料
    myContext->SetMaterial(myAISObject, iMaterial, Standard_False);
    //设置颜色
    Standard_Real C1 = g_vMaterials[iMaterial].v3MaterialColor[0];
    Standard_Real C2 = g_vMaterials[iMaterial].v3MaterialColor[1];
    Standard_Real C3 = g_vMaterials[iMaterial].v3MaterialColor[2];
    Quantity_Color CFL(C1, C2, C3, Quantity_TOC_RGB);
    myContext->SetColor(myAISObject, CFL, Standard_False);
    //设置透明度
    double aTransparency = g_vMaterials[iMaterial].fMaterialTransparency;
    myContext->SetTransparency(myAISObject, aTransparency, Standard_False);
    //myContext->SetSelected(myAISObject, true);
    //myContext->Hilight(myAISObject, true);

    //获取颜色设置，并显示
    map<string, vector<double> > mpColors = GetColorMap();
    map<string, vector<double> >::iterator itre = mpColors.begin();
    for (; itre != mpColors.end(); ++itre)
    {
        string sName = itre->first;
        vector<double> vClr = itre->second;
        TopoDS_Shape aShape = GetShapeFromName(sName);
        Quantity_Color clr(vClr[0], vClr[1], vClr[2], Quantity_TOC_RGB);
        //Handle(AIS_Shape) aShapePrs = new AIS_Shape(aShape);
        //aShapePrs->SetColor(clr);
        //myContext->Display(aShapePrs, false);
		myAISObject->SetCustomColor(aShape,clr);
    }
	myContext->Display(myAISObject, false);

	vector<MarkingSurface*> vMss = GetAllMarkingSurface();
	for (int i = 0; i < vMss.size(); i++)
	{
		MarkingSurface* Msi = vMss[i];
		TopoDS_Shape shp_i = Msi->GetShape(); //Generate();
		Handle(AIS_InteractiveObject) markingSurf = new AIS_Shape(shp_i);
		myContext->SetColor(markingSurf, Quantity_NOC_YELLOW, Standard_True);
		myContext->Display(markingSurf, Standard_True);
		delete Msi;
	}
	vMss.clear();

	//显示纹理
	map<string, string > mpTextures = GetTextureMap();
	map<string, string >::iterator itrTxu = mpTextures.begin();
	for (; itrTxu != mpTextures.end(); ++itrTxu)
	{
		string sName = itrTxu->first;
		string sFile = itrTxu->second;
		QString sFilePath = sFile.c_str();

		if (!QFileInfo(sFile.c_str()).exists())
		{
			//如果没找到，则在 myfile路径找
			sFilePath = QFileInfo(sFile.c_str()).fileName();
			sFilePath = QFileInfo(myfileName).absolutePath() + "/" + sFilePath;
			if (!QFileInfo(sFilePath).exists())
			{
				sFilePath = QFileInfo(myfileName).absolutePath() + "/images/" + QFileInfo(sFile.c_str()).fileName();
			}
		}

		if (QFileInfo(sFilePath).exists())
		{
			TopoDS_Shape aShape = GetShapeFromName(sName);
			const TCollection_AsciiString anUtf8Path(sFilePath.toStdU16String().data());
			Handle(AIS_TexturedShape) aTFace = Texturize(aShape, anUtf8Path);
			myContext->Display(aTFace, false);
			//myContext->Display(aTFace, Standard_True);
		}
	}

    //获取约束，并显示???

    //获取参考元素，并显示
    vector<DatumPoint*> vPts = GetAllDatumPoints();
    for (int i = 0; i < vPts.size(); i++)
    {
        DatumPoint* pti = vPts[i];
        TopoDS_Shape shp_i = pti->GetShape(); //Generate();
        Handle(AIS_InteractiveObject) datPoint = new AIS_Shape(shp_i);
        myContext->SetColor(datPoint, Quantity_NOC_YELLOW, Standard_True);
        myContext->Display(datPoint, Standard_True);
        delete pti;
    }
    vPts.clear();
    vector<DatumLine*> vLns = GetAllDatumLines();
    for (int i = 0; i < vLns.size(); i++)
    {
        DatumLine* lni = vLns[i];
        TopoDS_Shape shp_i = lni->GetShape();
        Handle(AIS_InteractiveObject) datLine = new AIS_Shape(shp_i);
      
        Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_YELLOW, Aspect_TOL_DOTDASH, 1);
        datLine->Attributes()->SetWireAspect(lineAspect);
        myContext->Display(datLine, Standard_True);

        delete lni;
    }
    vLns.clear();

    //获得Connectors，并显示
    vector<Connector*> vConntrs = GetAllConnectors();
    for (int i = 0; i < vConntrs.size(); i++)
    {
        Connector* conn = vConntrs[i];
        TopoDS_Shape shp_i = conn->GetShape();
		gp_Ax3 Ax3 = MakeAx3(conn->GetDisplacement());
		//接口不缩放吧，否则造成选择不上
		////加个比例缩放
		//gp_Trsf transf;
		//double scale = LibPathsSetDlg::GetConnSize();
		//transf.SetScale(Ax3.Location(), scale);
		//BRepBuilderAPI_Transform myBRepTransformation(shp_i, transf);
		//TopoDS_Shape trsfshp_i = myBRepTransformation.Shape();
		//	//conn->SetShape(trsfshp_i);
		//	//UpdateConnector(shp_i, conn);

        Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(shp_i/*trsfshp_i*/);
		myContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_True);
        myContext->Display(shpConn, Standard_True);

        //绘制坐标系
        double theLength, arrowLength;
        GetAx3Length(theLength, arrowLength);
        Handle(AdaptorVec_AIS) z_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.Direction(), theLength, arrowLength);
        z_Axis->SetText("  Z");
        myContext->Display(z_Axis, Standard_True);
        Handle(AdaptorVec_AIS) x_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.XDirection(), theLength, arrowLength);
        x_Axis->SetText("  X");
        myContext->Display(x_Axis, Standard_True);
        Handle(AdaptorVec_AIS) y_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.YDirection(), theLength, arrowLength);
        y_Axis->SetText("  Y");
        myContext->Display(y_Axis, Standard_True);

        delete conn;
    }
    vConntrs.clear();

	DisplayGlobalCoordsys();
	//Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
	//myContext->Display(aViewCube, true);
    myContext->UpdateCurrentViewer();
}

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

void DocumentComponent::ShowShapes(Handle(TopTools_HSequenceOfShape) theShapes)
{
	DocumentCommon::ShowShapes(theShapes);

    //for (int i = 1; i <= theShapes->Length(); i++)
    //{
    //    TopoDS_Shape aShape = theShapes->Value(i);
    //    Handle(AIS_Shape) myAISObject = new AIS_Shape(aShape);
    //    myContext->Display(myAISObject, false);
    //}
}

bool DocumentComponent::HideSelected()
{
	return false;
}

//删除选中物体
bool DocumentComponent::DeleteSelected()
{
    if (!myContext->NbSelected()) 
        return false;

    for (myContext->InitSelected(); myContext->MoreSelected(); myContext->NextSelected())
    {
        Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
        if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
        {
            TopoDS_Shape shape = Handle(AIS_Shape)::DownCast(obj)->Shape();
            string sName = GetShapeName(shape);
			
			if (sName == "Solid_1") //主实体删除
			{
				if (QMessageBox::question(nullptr, tr("提示"), tr("要清除当前几何和文档相关内容，是否继续？"), QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::No)
				{
					return false;
				}
				TDF_Label lSolid = m_pDocRoot.FindChild(1, true);
				lSolid.ForgetAllAttributes();
				//UpdateDisplay();
			}
			//删除接口
			else if ( GetConnector(sName.c_str()) )
			{
				DeleteConnector(shape);
			}
			//删除基准点
            else if (sName.substr(0, 11) == "DatumPoint_")
            {
                DeleteDatumPoint(shape);
            }
            else if (sName.substr(0, 10) == "DatumLine_")
                //删除基准线
            {
                DeleteDatumLine(shape);
            }
			else if (sName.substr(0, 11) == "DatumPlane_")
			{
				DeleteMarkingSurf(shape);
			}
            else {
                //其余情况不允许删除
                return false;
            }

			//删除引用为实体名sName的对象：参考点、参考线、接口、几何约束
			DeleteRefObjects(sName.c_str());
        }
    }

    UpdateDisplay();
    return true;
}

/////////////////////////////////////////////////
// class DocumentComponent 从M3dCom_DS
/////////////////////////////////////////////////
//根据形状类型，增加到相应的树节点上；存储关系表hm
void DocumentComponent::AddShape(myShapeType t0, const TopoDS_Shape& aShape)
{
	TDF_Label lab_add;

	//找到形状节点，如果没有，则创建
	TDF_Label lShapes = m_pDocRoot.FindChild(1); //tag=1，是形状shapes
	TopAbs_ShapeEnum shpType = aShape.ShapeType();

	switch (t0)
	{
	case NM_VERTEX:
	{//找到Vertexes节点
		TopoDS_Vertex vtx = TopoDS::Vertex(aShape);
		gp_Pnt ptXYZ = BRep_Tool::Pnt(vtx);
		TDF_Label lVertexes = lShapes.FindChild(1); //tag=1是Vertexes
		Standard_Integer iChilds = lVertexes.NbChildren();
		//如果没有，则直接增加一个
		if (iChilds == 0)
		{
			TDF_Label lblV1 = lVertexes.FindChild(1);
			//设置LblV1的属性值
			Handle(TDataStd_RealArray) pt3 = TDataStd_RealArray::Set(lblV1, 1, 3);
			pt3->SetValue(1, ptXYZ.Coord().X());
			pt3->SetValue(2, ptXYZ.Coord().Y());
			pt3->SetValue(3, ptXYZ.Coord().Z());
			//插入节点
			lab_add = lblV1.FindChild(1);
		}
		else {
			//如果已经有，则找有没有坐标相同的顶点Vertex，如果有，加到下面，如果没有，则添加
			int i = 0;
			for (i = 0; i < iChilds; i++)
			{
				TDF_Label iLabel = lVertexes.FindChild(i + 1);//一定有了
				//得到iLabel的属性坐标
				Handle(TDataStd_RealArray) gpt;
				if (iLabel.FindAttribute(TDataStd_RealArray::GetID(), gpt))
				{
					gp_Pnt pt3(gpt->Value(1), gpt->Value(2), gpt->Value(3));
					if (ptXYZ.IsEqual(pt3, LINEAR_TOL))
					{
						//找到了，则加在这个节点下面
						Standard_Integer nCount = iLabel.NbChildren();
						lab_add = iLabel.FindChild(nCount + 1);
						break;
					}
				}
				else
				{
					//出错!!!
				}
			}
			//没有找到相同坐标的顶点，则新建子节点，并设置属性
			if (i == iChilds)
			{
				iChilds++;
				TDF_Label lblVi = lVertexes.FindChild(iChilds);
				Handle(TDataStd_RealArray) pt3 = TDataStd_RealArray::Set(lblVi, 1, 3);
				pt3->SetValue(1, ptXYZ.Coord().X());
				pt3->SetValue(2, ptXYZ.Coord().Y());
				pt3->SetValue(3, ptXYZ.Coord().Z());
				//插入节点
				lab_add = lblVi.FindChild(1);
			}
		}
	}
	break;
	case NM_EDGE:
	{
		//找到Edges节点
		TDF_Label lEdges = lShapes.FindChild(2); //tag=2是Edges
		Standard_Integer iChilds = lEdges.NbChildren();
		iChilds++;
		//真正的节点
		lab_add = lEdges.FindChild(iChilds);
	}
	break;
	case NM_FACE:
	{
		//找到Faces节点
		TDF_Label lFaces = lShapes.FindChild(3); //tag=3是Faces
		Standard_Integer iChilds = lFaces.NbChildren();
		iChilds++;
		//真正的节点
		lab_add = lFaces.FindChild(iChilds);
	}
	break;
	case NM_SOLID:
	{
		//找到Solid节点，只有一个
		TDF_Label lSolid = lShapes.FindChild(4); //tag=4是Solid
		lab_add = lSolid;// .FindChild(1);
	}
	break;
	case NM_CONNECTOR:
	{
		//找到Connectors节点
		TDF_Label lConnectors = m_pDocRoot.FindChild(2); //tag=2是Connectors
		Standard_Integer iChilds = lConnectors.NbChildren();
		iChilds++;
		//真正的节点
		lab_add = lConnectors.FindChild(iChilds);
	}
	break;
	default:
		break;
	}

	//将形状命名，附加到Label
	TNaming_Builder B_i(lab_add);
	B_i.Generated(aShape);

	//建立索引
	//m_hmShapes.insert(make_pair(0, &lab_add));
	//m_mapShapes.insert(make_pair(aShape, lab_add));
}

//从父节点找下面子节点，形状为theShape的节点Label
//nth返回第几个;
TDF_Label DocumentComponent::LookupForShapeLabelFromFather(const TDF_Label& father, const TopoDS_Shape& theShape, int& nth)
{
	TDF_Label retLabel;

	Standard_Integer ncount = father.NbChildren();
	for (int i = 0; i < ncount; i++)
	{
		TDF_Label lbl_i = father.FindChild(i + 1, false);
		if (lbl_i.IsNull())
		{
			continue;
		}

		Handle(TNaming_NamedShape) aNamedShape1;
		lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		if (aNamedShape1.IsNull())
		{
			continue;
		}
		TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

		if (theShape.IsSame(shapej))
		{//找到了形状
			retLabel = lbl_i;
			nth = i + 1;
			break;
		}
	}

	return retLabel;
}

//获得形状的label和ID
TDF_Label DocumentComponent::GetShapeLabelandID(const TopoDS_Shape& theShape, vector<int>& retID) const
{
	TDF_Label retLabel;
	TDF_Label lShapes = m_pDocRoot.FindChild(1);

	retID.clear();
	TopAbs_ShapeEnum shpType = theShape.ShapeType();
	switch (shpType)
	{
	case TopAbs_ShapeEnum::TopAbs_VERTEX:
	{
		retID.push_back(1);
		TopoDS_Vertex vtx = TopoDS::Vertex(theShape);
		gp_Pnt ptXYZ = BRep_Tool::Pnt(vtx);
		//先定位到Vertexes
		TDF_Label lVertexes = lShapes.FindChild(1, false);
		Standard_Integer ncount = lVertexes.NbChildren();
		for (int i = 0; i < ncount; i++)
		{
			TDF_Label lbl_i = lVertexes.FindChild(i + 1, false);
			Handle(TDataStd_RealArray) gpnt;
			lbl_i.FindAttribute(TDataStd_RealArray::GetID(), gpnt);
			gp_Pnt p3(gpnt->Value(1), gpnt->Value(2), gpnt->Value(3));
			if (ptXYZ.IsEqual(p3, LINEAR_TOL))
			{
				//比对同顶点下面的
				//theShape.IsSame();??????????????????????
				//theShape.IsEqual();?????????????????????
				retID.push_back(i + 1);//此处有风险！！！
				int nth;
				retLabel = LookupForShapeLabelFromFather(lbl_i, theShape, nth);
				//顶点先用3个数组存储吧！！！！！！！！！！
				retID.push_back(nth);
				//Standard_Integer n2 = lbl_i.NbChildren();
				//for (int j = 0; j < n2; j++)
				//{
				//	TDF_Label lbl_i_j = lbl_i.FindChild(j + 1, false);
				//	Handle(TNaming_NamedShape) aNamedShape1;
				//	lbl_i_j.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
				//	TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

				//	if (theShape.IsSame(shapej))
				//	{//找到了形状
				//		retLabel = lbl_i_j;
				//		break;
				//	}
				//}
				break;
			}
		}
		break;
	}
	case TopAbs_ShapeEnum::TopAbs_EDGE:
	{
		retID.push_back(2);
		//先定位到Edges
		TDF_Label lEdges = lShapes.FindChild(2, false);
		int nth;
		retLabel = LookupForShapeLabelFromFather(lEdges, theShape, nth);
		retID.push_back(nth);
		//Standard_Integer ncount = lEdges.NbChildren();
		//for (int i = 0; i < ncount; i++)
		//{
		//	TDF_Label lbl_i = lEdges.FindChild(i + 1, false);
		//	Handle(TNaming_NamedShape) aNamedShape1;
		//	lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		//	TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

		//	if (theShape.IsSame(shapej))
		//	{//找到了形状
		//		retLabel = lbl_i;
		//		break;
		//	}
		//}

		break;
	}
	case TopAbs_ShapeEnum::TopAbs_FACE:
	{
		retID.push_back(3);
		//先定位到Faces
		TDF_Label lFaces = lShapes.FindChild(3, false);
		int nth;
		retLabel = LookupForShapeLabelFromFather(lFaces, theShape, nth);
		retID.push_back(nth);

		break;
	}
	case TopAbs_ShapeEnum::TopAbs_SOLID:
	{
		//注意：Connector也在这里识别！！！
		TDF_Label lSolid = lShapes.FindChild(4, false);
		Handle(TNaming_NamedShape) aNamedShape1;
		lSolid.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

		if (theShape.IsEqual(shapej))
		{//找到了形状为主实体
			retLabel = lSolid;
			retID.push_back(4);
			retID.push_back(1);
			break;
		}

		//否则找Connectors
		retID.push_back(5);
		TDF_Label lConnectors = m_pDocRoot.FindChild(2); //tag=2是Connectors
		int nth;
		retLabel = LookupForShapeLabelFromFather(lConnectors, theShape, nth);
		retID.push_back(nth);
		assert(!retLabel.IsNull());

		break;
	}
	default:
		break;
	}

	return retLabel;
}

///*从hm得到Shape的Label
//改为直接从文档树上搜索
TDF_Label DocumentComponent::GetShapeLabel(const TopoDS_Shape& theShape) const
{
	vector<int> IDs;
	TDF_Label retLabel = GetShapeLabelandID(theShape, IDs);

	return retLabel;
}

//得到TopoDS_Shape的ID ,如[3,3]，从树上遍历找到label，获得序号；或直接由Label的tag得到
vector<int> DocumentComponent::GetShapeID(const TopoDS_Shape& aShape) const
{//用IsSame比较！！！！！！！！！
	vector<int> retVec;
	//TDF_Label lbl1 = GetShapeLabelandID(aShape, retVec);

	Graphic3d_NameOfMaterial mat;
	TopoDS_Shape mainSolid = GetMainSolid(mat);

	TopAbs_ShapeEnum shpType = aShape.ShapeType();
	switch (shpType)
	{
	case TopAbs_ShapeEnum::TopAbs_VERTEX: //IsSame
	{//顶点、参考点
		TopExp_Explorer Ex;
		//遍历所有顶点
		int nIndex = 0;
		for (Ex.Init(mainSolid, TopAbs_VERTEX); Ex.More(); Ex.Next()) {
			TopoDS_Shape vtx_i = Ex.Current();
			nIndex++;
			if (aShape.IsSame(vtx_i))
			{
				retVec.push_back(1);/////
				retVec.push_back(nIndex);
				return retVec;
			}
		}
		//如果没找到，则可能是参考点
		TDF_Label datumsLbl = m_pDocRoot.FindChild(3, false);
		if (datumsLbl.IsNull())
		{
			return retVec;
		}
		TDF_Label datumsPt = datumsLbl.FindChild(1, false);
		if (datumsPt.IsNull())
		{
			return retVec;
		}
		int nth = 0;
		TDF_Label theLabel = LookupForShapeLabelFromFather(datumsPt, aShape, nth);
		if (!theLabel.IsNull())
		{
			retVec.push_back(6);/////
			retVec.push_back(nth);
			return retVec;
		}
		break;
	}
	case TopAbs_ShapeEnum::TopAbs_EDGE:
	{//边、参考线
		TopExp_Explorer Ex;
		//遍历所有edge
		int nIndex = 0;
		for (Ex.Init(mainSolid, TopAbs_EDGE); Ex.More(); Ex.Next()) {
			TopoDS_Shape vtx_i = Ex.Current();
			nIndex++;
			if (aShape.IsSame(vtx_i))
			{
				retVec.push_back(2);/////
				retVec.push_back(nIndex);
				return retVec;
			}
		}
		//如果没找到，则可能是参考点
		TDF_Label datumsLbl = m_pDocRoot.FindChild(3, false);
		if (datumsLbl.IsNull())
		{
			return retVec;
		}
		TDF_Label datumsLn = datumsLbl.FindChild(2, false);
		if (datumsLn.IsNull())
		{
			return retVec;
		}
		int nth = 0;
		TDF_Label theLabel = LookupForShapeLabelFromFather(datumsLn, aShape, nth);
		if (!theLabel.IsNull())
		{
			retVec.push_back(7);/////
			retVec.push_back(nth);
			return retVec;
		}
		break;
	}
	case TopAbs_ShapeEnum::TopAbs_FACE:
	{//面、参考面
		TopExp_Explorer Ex;
		//遍历所有edge
		int nIndex = 0;
		for (Ex.Init(mainSolid, TopAbs_FACE); Ex.More(); Ex.Next()) {
			TopoDS_Shape vtx_i = Ex.Current();
			nIndex++;
			if (aShape.IsSame(vtx_i))
			{
				retVec.push_back(3);/////
				retVec.push_back(nIndex);
				return retVec;
			}
		}
		//如果没找到，则可能是参考面
		TDF_Label datumsLbl = m_pDocRoot.FindChild(3, false);
		if (datumsLbl.IsNull())
		{
			return retVec;
		}
		TDF_Label datumsPln = datumsLbl.FindChild(3, false);
		if (datumsPln.IsNull())
		{
			return retVec;
		}
		int nth = 0;
		TDF_Label theLabel = LookupForShapeLabelFromFather(datumsPln, aShape, nth);
		if (!theLabel.IsNull())
		{
			retVec.push_back(8);/////
			retVec.push_back(nth);
			return retVec;
		}
		break;
	}
	case TopAbs_ShapeEnum::TopAbs_COMPOUND:
	case TopAbs_ShapeEnum::TopAbs_SOLID:
	{//主Solid、接口
		if (aShape.IsSame(mainSolid))
		{
			retVec.push_back(4);/////
			retVec.push_back(1);
			return retVec;
		}
		//否则是接口
		TDF_Label connectorsLbl = m_pDocRoot.FindChild(4, false);
		if (connectorsLbl.IsNull())
		{
			break;
		}
		int nth = 0;
		TDF_Label theLabel = LookupForShapeLabelFromFather(connectorsLbl, aShape, nth);
		if (!theLabel.IsNull())
		{
			retVec.push_back(5);/////
			retVec.push_back(nth);
			return retVec;
		}
		break;
	}
	default:
		break;
	}

	//assert(false);
	//没有找到，为空
	return retVec;
}

//得到TopoDS_Shape的命名，如“面3”，由类型遍历，快
string DocumentComponent::GetShapeName(const TopoDS_Shape& aShape) const
{
	string retStr;
	vector<int> retVec = GetShapeID(aShape);
	//TDF_Label lbl1 = GetShapeLabelandID(aShape, retVec);

	retStr = GetShapeNameFromID(retVec);
	//接口的特殊处理，暂时这样吧
	if (retStr.length()>9 && retStr.find("Connector")==0)
	{
		Connector* pCon = GetConnector(aShape);
		retStr = pCon->GetConnName();
		delete pCon;
	}

	return retStr;
}

//从父实体中找第Nth个子实体
bool DocumentComponent::LookForShape(const TopoDS_Shape& parent, TopAbs_ShapeEnum type, int nth, TopoDS_Shape& retShape)
{
	TopExp_Explorer Ex;
	int ni = 0;
	for (Ex.Init(parent, type); Ex.More(); Ex.Next()) {
		TopoDS_Shape shp_i = Ex.Current();
		ni++;
		if (nth == ni)
		{
			retShape = shp_i;
			return true;
		}
	}

	return false;
}

//从一个父节点下面找子节点，它的第nth个形状
bool DocumentComponent::LookForShapeFromParentLabel(TDF_Label parent, int nth, TopoDS_Shape& ret)
{
	Standard_Integer ncount = parent.NbChildren();
	if (nth > ncount)
	{
		return false;
	}

	TDF_Label lbl_i = parent.FindChild(nth, false);
	Handle(TNaming_NamedShape) aNamedShape1;
	lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
	ret = TNaming_Tool::GetShape(aNamedShape1);

	return true;
}

//由ID得到形状
TopoDS_Shape DocumentComponent::GetShapeFromID(vector<int>& vID) const
{
	TopoDS_Shape retShape;
	Handle(TNaming_NamedShape) theNamedShape;
	int nType = vID[0];
	int nIndex = vID[1];

	Graphic3d_NameOfMaterial mat;
	TopoDS_Shape mainSolid = GetMainSolid(mat);

	switch (nType)
	{
	case 1:
	{
		//Vertex
		if (LookForShape(mainSolid, TopAbs_VERTEX, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 2:
	{
		//Edge
		if (LookForShape(mainSolid, TopAbs_EDGE, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 3:
	{
		//FAce
		if (LookForShape(mainSolid, TopAbs_FACE, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 4: //主Solid
	{
		assert(nIndex == 1);
		return mainSolid;
	}
	case 5: //接口
	{
		TDF_Label connectorsLbl = m_pDocRoot.FindChild(4, false);
		if (LookForShapeFromParentLabel(connectorsLbl, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 6: //参考点
	{
		TDF_Label datumsLbl = m_pDocRoot.FindChild(3, false);
		TDF_Label datumsV = datumsLbl.FindChild(1, false);
		if (LookForShapeFromParentLabel(datumsV, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 7: //参考线
	{
		TDF_Label datumsLbl = m_pDocRoot.FindChild(3, false);
		TDF_Label datumsE = datumsLbl.FindChild(2, false);
		if (LookForShapeFromParentLabel(datumsE, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 8: //参考面
	{
		TDF_Label datumsLbl = m_pDocRoot.FindChild(3, false);
		TDF_Label datumsF = datumsLbl.FindChild(3, false);
		if (LookForShapeFromParentLabel(datumsF, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	default:
		break;
	}

	assert(false);

	return retShape;
}

//原版本
TopoDS_Shape DocumentComponent::GetShapeFromID0(vector<int>& vID) const
{
	TopoDS_Shape retShape;
	Handle(TNaming_NamedShape) theNamedShape;
	int nType = vID[0];
	int nIndex = vID[1];

	TDF_Label lblShapes = m_pDocRoot.FindChild(1, false);
	assert(!lblShapes.IsNull());

	switch (nType)
	{
	case 1:
	{
		//Vertex
		TDF_Label lVertexes = lblShapes.FindChild(1, false);
		assert(!lVertexes.IsNull());
		//找到节点
		TDF_Label theLabel = lVertexes.FindChild(nIndex, false);
		assert(!theLabel.IsNull());

		//增加一层???????????????????????????????????
		TDF_Label theLabel_1 = lVertexes.FindChild(1, false);
		bool b0 = theLabel_1.FindAttribute(TNaming_NamedShape::GetID(), theNamedShape);
		assert(b0);

		break;
	}
	case 2:
	{
		//Edge
		TDF_Label lEdges = lblShapes.FindChild(2, false);
		assert(!lEdges.IsNull());
		//找到节点
		TDF_Label theLabel = lEdges.FindChild(nIndex, false);
		assert(!theLabel.IsNull());

		bool b0 = theLabel.FindAttribute(TNaming_NamedShape::GetID(), theNamedShape);
		assert(b0);
		break;
	}
	case 3:
	{
		//Face
		TDF_Label lFaces = lblShapes.FindChild(3, false);
		assert(!lFaces.IsNull());
		//找到节点
		TDF_Label theLabel = lFaces.FindChild(nIndex, false);
		assert(!theLabel.IsNull());

		bool b0 = theLabel.FindAttribute(TNaming_NamedShape::GetID(), theNamedShape);
		assert(b0);
		break;
	}
	case 4:
	{
		//Solid
		TDF_Label lSolid = lblShapes.FindChild(4, false);
		assert(!lSolid.IsNull());
		//找到节点
		TDF_Label theLabel = lSolid;

		bool b0 = theLabel.FindAttribute(TNaming_NamedShape::GetID(), theNamedShape);
		assert(b0);
		break;
	}
	case 5:
	{
		//Connector
		TDF_Label lConnectors = m_pDocRoot.FindChild(2, false);
		assert(!lConnectors.IsNull());
		//找到节点
		TDF_Label theLabel = lConnectors.FindChild(nIndex, false);
		assert(!theLabel.IsNull());

		bool b0 = theLabel.FindAttribute(TNaming_NamedShape::GetID(), theNamedShape);
		assert(b0);
		break;
	}
	default:
		break;
	}

	retShape = TNaming_Tool::GetShape(theNamedShape);
	return retShape;
}

//由名字得到形状
TopoDS_Shape DocumentComponent::GetShapeFromName(string& theName, const TopoDS_Shape & mainSolid) const
{
	TopoDS_Shape retShape;
	vector<int> vID = GetShapeIDFromName(theName);
	
	int nType = vID[0];
	int nIndex = vID[1];

	switch (nType)
	{
	case 1:
	{
		//Vertex
		if (LookForShape(mainSolid, TopAbs_VERTEX, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 2:
	{
		//Edge
		if (LookForShape(mainSolid, TopAbs_EDGE, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 3:
	{
		//FAce
		if (LookForShape(mainSolid, TopAbs_FACE, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}
	case 4: //主Solid
	{
		assert(nIndex == 1);
		return mainSolid;
	}
	case 8: //标记面
		TDF_Label datumsLbl = m_pDocRoot.FindChild(3, false);
		TDF_Label datumsF = datumsLbl.FindChild(3, false);
		if (LookForShapeFromParentLabel(datumsF, nIndex, retShape))
		{
			return retShape;
		}
		break;
	}

	return retShape;
}

bool DocumentComponent::isMarkingSurf(string& name)
{
	vector<int> vID = GetShapeIDFromName(name);
	int nType = vID[0];
	if (nType == 8)
	{
		return true;
	}
	return false;
}

//由名字得到形状
TopoDS_Shape DocumentComponent::GetShapeFromName(string theName) const
{
	TopoDS_Shape retShape;
	vector<int> vID = GetShapeIDFromName(theName);
	retShape = GetShapeFromID(vID);

	return retShape;
}

//更新hashmap的形状表:由文档的内容更新hmShapes，打开文档读取时候;Undo/Redo
void DocumentComponent::UpdateShapes()
{
	//此函数暂时不需要，因为免去了hash存储
}

//所有形状的组合体，不管显示
TopoDS_Shape DocumentComponent::GetCompCompound() const
{
	TopoDS_Compound theCompound;
	BRep_Builder aBuilder;
	aBuilder.MakeCompound(theCompound);

	//主实体
	Graphic3d_NameOfMaterial mat;
	TopoDS_Shape theMain = GetMainSolid(mat);
	aBuilder.Add(theCompound, theMain);

	//处理基准点
	vector<DatumPoint*> vDatumPts = GetAllDatumPoints();
	for (int i = 0; i < vDatumPts.size(); i++)
	{
		TopoDS_Shape shDt_i = vDatumPts[i]->GetShape();

		aBuilder.Add(theCompound, shDt_i);
	}

	//基准线
	vector<DatumLine*> vDatumLns = GetAllDatumLines();
	for (int i = 0; i < vDatumLns.size(); i++)
	{
		TopoDS_Shape shDt_i = vDatumLns[i]->GetShape();
		aBuilder.Add(theCompound, shDt_i);
	}

	//接口及其局部坐标系
	vector<Connector*> vConnectors = GetAllConnectors();
	for (int i = 0; i < vConnectors.size(); i++)
	{
		Connector* pConn = vConnectors[i];
		TopoDS_Shape shDt_i = pConn->GetShape();		
		aBuilder.Add(theCompound, shDt_i);	//参考点
	}

	return theCompound;
}

//获得全部Shapes，在读文件后，获得全部的TopoDS_Shape用于显示
//是不是只需要加载一个Solid，其余的面、边和顶点都自定加入？？？
//另外加载Connectors
vector<TopoDS_Shape> DocumentComponent::GetAllShapes() const
{
	vector<TopoDS_Shape> vShapes;

	//先只返回Solid
	Graphic3d_NameOfMaterial mat;
	TopoDS_Shape theSolid = GetMainSolid(mat);
	vShapes.push_back(theSolid);

	//得到Connectors
	TDF_Label lConnectors = m_pDocRoot.FindChild(2, false);
	if (!lConnectors.IsNull())
	{
		for (TDF_ChildIterator it(lConnectors); it.More(); it.Next())
		{
			TDF_Label aConn = it.Value();
			Handle(TNaming_NamedShape) theConnNameShape;
			if (aConn.FindAttribute(TNaming_NamedShape::GetID(), theConnNameShape))
			{
				TopoDS_Shape aShape = TNaming_Tool::GetShape(theConnNameShape);
				vShapes.push_back(aShape);
			}
		}
	}
	return vShapes;
}

//加入主实体，用于存取物性参数: 零件的主实体只有一个!!!
//同时，设置缺省材料 钢，并计算初始的物性参数存储到文档
//材料可以重设，物性参数在材料重设或几何参数化改变时重新计算刷新文档内容
void DocumentComponent::SetMainSolid(const TopoDS_Shape& theShape, Graphic3d_NameOfMaterial mat)
{
	SetModified();

	//先存储形状
	TDF_Label lSolid = m_pDocRoot.FindChild(1, true);
	//Handle(TNaming_NamedShape) theNamedShape;
	//bool b0 = lSolid.FindAttribute(TNaming_NamedShape::GetID(), theNamedShape);
	//if (b0)
	//{
	//	TopoDS_Shape tpShp = TNaming_Tool::GetShape(theNamedShape);
	//}
	lSolid.ForgetAllAttributes();

	TNaming_Builder B_i(lSolid);
	B_i.Generated(theShape);

	//存储缺省材料：钢, 密度/颜色/透明度,先存储三个数组，便于查找
	//Graphic3d_NameOfMaterial mat = Graphic3d_NameOfMaterial::Graphic3d_NameOfMaterial_Steel;
	int nIndex = (int)mat;
	Standard_Real dens = g_vMaterials[nIndex].fMaterialDensity;
	Standard_Real aTransparency = g_vMaterials[nIndex].fMaterialTransparency;
	Standard_Real C1 = g_vMaterials[nIndex].v3MaterialColor[0];
	Standard_Real C2 = g_vMaterials[nIndex].v3MaterialColor[1];
	Standard_Real C3 = g_vMaterials[nIndex].v3MaterialColor[2];
	Quantity_Color clr(C1, C2, C3, Quantity_TOC_RGB);
	//aTransparency = myCurrentIC->SelectedInteractive()->Transparency();
	//myCurrentIC->SetMaterial(myCurrentIC->SelectedInteractive(), (Graphic3d_NameOfMaterial)(Material), Standard_False);
	//myCurrentIC->SetTransparency(myCurrentIC->SelectedInteractive(), aTransparency, Standard_False);
	TDF_Label matTypeLbl = lSolid;// .FindChild(1);
	TDataStd_Integer::Set(matTypeLbl, mat);
	//材料的其他属性（名称密度颜色透明度）不用存储

//	//存储缺省的物性参数：体积（质量）、质心、惯量: 
//	//也不用存储，用的时候直接计算！！！！！
//#include <GProp_GProps.hxx>
//#include <GProp_PrincipalProps.hxx>
//#include <gp_Pnt.hxx>
//#include <BRepGProp.hxx>
//	TDF_Label propLbl = lSolid.FindChild(2);
//	// Retrieve volume properties from the face.
//	GProp_GProps aGProps;
//	BRepGProp::VolumeProperties(theShape, aGProps);
//	Standard_Real aVolume = aGProps.Mass();
//	gp_Pnt aCOM = aGProps.CentreOfMass();
//	Standard_Real anIx, anIy, anIz;
//	aGProps.StaticMoments(anIx, anIy, anIz);
//	gp_Mat aMOI = aGProps.MatrixOfInertia();
//	gp_Trsf trsf;
//	GProp_PrincipalProps aPProps = aGProps.PrincipalProperties();
//	Standard_Real anIxx, anIyy, anIzz;
//	aPProps.Moments(anIxx, anIyy, anIzz);
//	Standard_Real aRxx, aRyy, aRzz;
//	aPProps.RadiusOfGyration(aRxx, aRyy, aRzz);
//	const gp_Vec& anAxis1 = aPProps.FirstAxisOfInertia();
//	const gp_Vec& anAxis2 = aPProps.SecondAxisOfInertia();
//	const gp_Vec& anAxis3 = aPProps.ThirdAxisOfInertia();
}

//获得零件的物性参数：质心、质量、转动惯量
myPhysicalProperty DocumentComponent::GetPhysicalProperty() const
{
	myPhysicalProperty mpp;
	assert(false);

	return mpp;
}

//获得主实体
TopoDS_Shape DocumentComponent::GetMainSolid(Graphic3d_NameOfMaterial& mat) const
{
	TopoDS_Shape retShapePtr;
	//Solid
	TDF_Label lSolid = m_pDocRoot.FindChild(1, false);
	if (!lSolid.IsNull())
	{
		Handle(TNaming_NamedShape) theNamedShape;
		bool b0 = lSolid.FindAttribute(TNaming_NamedShape::GetID(), theNamedShape);
		if (b0)
		{
			retShapePtr = TNaming_Tool::GetShape(theNamedShape);
		}

		TDF_Label lMat = lSolid;// .FindChild(1, false); //直接放在Solid
		Handle(TDataStd_Integer) iMat;
		b0 = lMat.FindAttribute(TDataStd_Integer::GetID(), iMat);
		if (b0)
		{
			mat = (Graphic3d_NameOfMaterial)iMat->Get();
		}
	}

	return retShapePtr;
}

//找名字为
bool DocumentComponent::LookForColorLabel(const char* shpName, TDF_Label& theLabel)
{
	TDF_Label clrLabel = m_pDocRoot.FindChild(2, false);
	if (clrLabel.IsNull())
	{
		return false;
	}

	int nCountChilds = clrLabel.NbChildren();
	for (int i = 1; i <= nCountChilds; i++)
	{
		TDF_Label lbl_i = clrLabel.FindChild(i, false);
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		//if (!nameAttr.IsNull() && nameAttr->Get() == shpName)
		if (nameAttr.IsNull() || nameAttr->Get() == shpName)
		{
			theLabel = lbl_i;
			return true;
		}
	}

	return false;
}

//Texture : 7
bool DocumentComponent::LookForTextureLabel(const char* shpName, TDF_Label& theLabel)
{
	TDF_Label clrLabel = m_pDocRoot.FindChild(7, false);
	if (clrLabel.IsNull())
	{
		return false;
	}

	int nCountChilds = clrLabel.NbChildren();
	for (int i = 1; i <= nCountChilds; i++)
	{
		TDF_Label lbl_i = clrLabel.FindChild(i, false);
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull() || nameAttr->Get() == shpName)
		{
			theLabel = lbl_i;
			return true;
		}
	}

	return false;
}

//在文档记录实体纹理：
void DocumentComponent::SetShapeTexture(const TopoDS_Shape& theShape, const char* sJPGfileName)
{
	TDF_Label theLabel;// = GetShapeLabel(theShape); //此出可能有bug
	string shpName = GetShapeName(theShape);
	bool b0 = LookForTextureLabel(shpName.c_str(), theLabel);
	if (!b0)
	{

		TDF_Label clrLabel = m_pDocRoot.FindChild(7);
		int nCountChilds = clrLabel.NbChildren();
		theLabel = clrLabel.FindChild(nCountChilds + 1);
	}

	TDataStd_Name::Set(theLabel, shpName.c_str());
	//纹理图片
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(theLabel, 1, 1);
	aStrArray->SetValue(1, sJPGfileName);
}

//在文档记录实体颜色：设置颜色命令时调用
void DocumentComponent::SetShapeColor(const TopoDS_Shape& theShape, Quantity_Color theColor)
{
	TDF_Label theLabel;// = GetShapeLabel(theShape); //此出可能有bug
	string shpName = GetShapeName(theShape);
	bool b0 = LookForColorLabel(shpName.c_str(), theLabel);
	if (!b0)
	{
		TDF_Label clrLabel = m_pDocRoot.FindChild(2);
		int nCountChilds = clrLabel.NbChildren();
		theLabel = clrLabel.FindChild(nCountChilds + 1);
	}
	TDataStd_Name::Set(theLabel, shpName.c_str());
	Handle(TDataStd_RealArray) aRealArray_Color = TDataStd_RealArray::Set(theLabel, 1, 3);
	aRealArray_Color->SetValue(1, theColor.Red());
	aRealArray_Color->SetValue(2, theColor.Green());
	aRealArray_Color->SetValue(3, theColor.Blue());
}

//从文档获取实体颜色：读文件显示时调用
Quantity_Color DocumentComponent::GetShapeColor(const TopoDS_Shape& theShape) const
{
	TDF_Label theLabel = GetShapeLabel(theShape); //此出可能有bug
	Handle(TDataStd_RealArray) clorAttr;
	theLabel.FindAttribute(TDataStd_RealArray::GetID(), clorAttr);
	Quantity_Color CSFColor(clorAttr->Value(1), clorAttr->Value(2), clorAttr->Value(3), Quantity_TOC_RGB);

	return CSFColor;
}

map<string, vector<double> > DocumentComponent::GetColorMap()
{
	map<string, vector<double> > mapRetn;

	TDF_Label clrLabel = m_pDocRoot.FindChild(2, false);
	if (clrLabel.IsNull())
	{
		return mapRetn;
	}

	int nCountChilds = clrLabel.NbChildren();
	for (int i = 1; i <= nCountChilds; i++)
	{
		TDF_Label lbl_i = clrLabel.FindChild(i, false);
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		//string Name = (const char*)nameAttr->Get();
		TCollection_ExtendedString s = nameAttr->Get();
		string Name = ExtString2string(s);

		Handle(TDataStd_RealArray) clr;
		lbl_i.FindAttribute(TDataStd_RealArray::GetID(), clr);
		vector<double> CFL;
		CFL.push_back(clr->Value(1));
		CFL.push_back(clr->Value(2));
		CFL.push_back(clr->Value(3));
		mapRetn.insert(make_pair(Name, CFL));
	}

	return mapRetn;
}
//获取全部纹理信息
map<string, string > DocumentComponent::GetTextureMap()
{
	map<string, string > mapRetn;

	TDF_Label clrLabel = m_pDocRoot.FindChild(7, false);
	if (clrLabel.IsNull())
	{
		return mapRetn;
	}

	int nCountChilds = clrLabel.NbChildren();
	for (int i = 1; i <= nCountChilds; i++)
	{
		TDF_Label lbl_i = clrLabel.FindChild(i, false);
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}

		TCollection_ExtendedString s = nameAttr->Get();
		string Name = ExtString2string(s);

		Handle(TDataStd_ExtStringArray) aStrArray;
		lbl_i.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
		TCollection_ExtendedString s1 = aStrArray->Value(1);
		string str1 = ExtString2string(s1);

		mapRetn.insert(make_pair(Name, str1.c_str()));
	}

	return mapRetn;
}

/////////////////
////约束相关
//
void DocumentComponent::AddConstraint(TDF_Label& parent, int nth, myConstraint& pConstraint)
{
	TDF_Label nthLabel = parent.FindChild(nth);
	//形状，类型，是否起作用，元素1，2，值
	//形状暂不考虑
	//序号和类型
	//TDataStd_Integer::Set(nthLabel, pConstraint.consType);
	Handle(TDataStd_IntegerArray) aIntArray = TDataStd_IntegerArray::Set(nthLabel, 1, 2);
	aIntArray->SetValue(1, pConstraint.nIndex);
	aIntArray->SetValue(2, pConstraint.consType);

	//两个元素名字
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(nthLabel, 1, 2);
	aStrArray->SetValue(1, pConstraint.shapeName1.c_str());
	aStrArray->SetValue(2, pConstraint.shapeName2.c_str());
	//是否起作用
	Handle(TDataStd_BooleanArray) aBoolArray = TDataStd_BooleanArray::Set(nthLabel, 1, 1);
	aBoolArray->SetValue(1, pConstraint.bActive);
	//值
	TDataStd_Real::Set(nthLabel, pConstraint.consValue);
}

void DocumentComponent::SetConstraints(vector<myConstraint>& vConstraints)
{
	//5 - Constrains
	TDF_Label consLabel = m_pDocRoot.FindChild(5);
	//先清除子节点
	//无法清除
	//只能在Contraints根节点设置一个整数记录
	//TDataStd_Integer nCount;
	//nCount.Set(vConstraints.size());
	TDataStd_Integer::Set(consLabel, vConstraints.size());

	vector<myConstraint>::iterator iter;
	int nIndex = 1;
	for (iter = vConstraints.begin(); iter != vConstraints.end(); ++iter)
	{
		myConstraint aCon = *iter;
		AddConstraint(consLabel, nIndex, aCon);
		nIndex++;
	}
}

//
bool DocumentComponent::DeleteConstraint(int nIndex)
{
	bool bFound = false;

	TDF_Label ConsLbl = m_pDocRoot.FindChild(5, false);
	int nCount = ConsLbl.NbChildren();
	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label nthLabel = ConsLbl.FindChild(i, false);

		//形状，类型，是否起作用，元素1，2，值
		//Handle(TDataStd_Integer) iType;
		//nthLabel.FindAttribute(TDataStd_Integer::GetID(), iType);
		Handle(TDataStd_IntegerArray) aIntArray;
		nthLabel.FindAttribute(TDataStd_IntegerArray::GetID(), aIntArray);
		if (aIntArray.IsNull())
			continue;

		int nth = aIntArray->Value(1);
		if (nIndex == nth)
		{
			nthLabel.ForgetAllAttributes();
			bFound = true;
			break;
		}
	}

	return bFound;
}

//得到第几个约束
myConstraint* DocumentComponent::GetConstraint(TDF_Label& parent, int nTh) const
{
	myConstraint* retCons = nullptr;

	//TDF_Label nthLabel = parent.FindChild(nTh, false);
	//if (nthLabel.IsNull())
	//{
	//	return  retCons;
	//}
	int nCount = parent.NbChildren();
	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label nthLabel = parent.FindChild(i, false);

		//形状，类型，是否起作用，元素1，2，值
		//Handle(TDataStd_Integer) iType;
		//nthLabel.FindAttribute(TDataStd_Integer::GetID(), iType);
		Handle(TDataStd_IntegerArray) aIntArray;
		nthLabel.FindAttribute(TDataStd_IntegerArray::GetID(), aIntArray);
		if (aIntArray.IsNull())
			continue;

		int nIndex = aIntArray->Value(1);
		if (nIndex == nTh)
		{
			myConstraintType atype = (myConstraintType)aIntArray->Value(2);

			//两个元素名字
			Handle(TDataStd_ExtStringArray) aStrArray;
			nthLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
			TCollection_ExtendedString s1 = aStrArray->Value(1);
			string str1 = ExtString2string(s1);
			TCollection_ExtendedString s2 = aStrArray->Value(2);
			string str2 = ExtString2string(s2);

			//是否起作用
			Handle(TDataStd_BooleanArray) aBoolArray;
			nthLabel.FindAttribute(TDataStd_BooleanArray::GetID(), aBoolArray);
			bool bActive = aBoolArray->Value(1);
			//值
			Handle(TDataStd_Real) fVal;
			nthLabel.FindAttribute(TDataStd_Real::GetID(), fVal);
			double value0 = fVal->Get();

			retCons = new myConstraint;
			retCons->nIndex = nIndex;
			retCons->bActive = bActive;
			retCons->consType = atype;
			retCons->shapeName1 = str1;
			retCons->shapeName2 = str2;
			retCons->consValue = value0;
		}
	}

	return retCons;
}

//得到全部约束
vector<myConstraint> DocumentComponent::GetAllConstraints() const
{
	vector<myConstraint> vRet;
	//5 - Constrains
	TDF_Label consLabel = m_pDocRoot.FindChild(5, false);
	if (consLabel.IsNull()) {
		return vRet;
	}
	Handle(TDataStd_Integer) nCount;
	consLabel.FindAttribute(TDataStd_Integer::GetID(), nCount);
	if (nCount.IsNull())
	{
		return vRet;
	}
	int num = nCount->Get();
	for (int i = 1; i <= num; i++)
	{
		myConstraint* aCons = GetConstraint(consLabel, i);
		if (aCons)
		{
			vRet.push_back(*aCons);
			delete aCons;
		}
	}

	return vRet;
}

////////////////////////////////////
///材料、透明度
void DocumentComponent::SetMaterial(Graphic3d_NameOfMaterial nMaterial)
{
	TDF_Label lsolid = m_pDocRoot.FindChild(1);
	TDataStd_Integer::Set(lsolid, nMaterial);
}

//从文档获得材料
Graphic3d_NameOfMaterial DocumentComponent::GetMaterial() const
{
	Graphic3d_NameOfMaterial aMatType = Graphic3d_NameOfMaterial::Graphic3d_NameOfMaterial_Steel;

	TDF_Label lMat = m_pDocRoot.FindChild(1);// .FindChild(1, false); //直接放在Solid
	Handle(TDataStd_Integer) iMat;
	bool b0 = lMat.FindAttribute(TDataStd_Integer::GetID(), iMat);
	if (b0)
	{
		aMatType = (Graphic3d_NameOfMaterial)iMat->Get();
	}

	return aMatType;
}

//设置透明度,同 材料
void DocumentComponent::SetTransparency(double nTransp)
{
	//暂不用
	assert(false);
}

//获得零件透明度,暂不用
double  DocumentComponent::GetTransparency() const
{
	double dTransp = 0;

	assert(false);

	return dTransp;
}

/////////////////////////////////////////
//CyberInfo
//界面Update时，设置Cyber信息到文档
void DocumentComponent::AddParameter(TDF_Label& parLabel, int nIndex, myParameter* aPar)
{
	TDF_Label nthLabel = parLabel.FindChild(nIndex);
	//缺省值，名字，单位，关注名，是否关注
	nthLabel.ForgetAllAttributes();

	//缺省值
	//TDataStd_Real::Set(nthLabel, aPar->defaultValue);
	//名字，单位，关注名、类型、维度
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(nthLabel, 1, 6);
	aStrArray->SetValue(1, aPar->sFullName.c_str());
	aStrArray->SetValue(2, aPar->sUnit.c_str());
	aStrArray->SetValue(3, aPar->sFocusName.c_str());
	aStrArray->SetValue(4, aPar->sType.c_str());
	aStrArray->SetValue(5, aPar->sDimension.c_str());
	aStrArray->SetValue(6, aPar->defaultValue.c_str());
	//是否关注
	Handle(TDataStd_BooleanArray) aBoolArray = TDataStd_BooleanArray::Set(nthLabel, 1, 1);
	aBoolArray->SetValue(1, aPar->bFocused);
}

void DocumentComponent::AddOutputVar(TDF_Label& parLabel, int nIndex, myOutputVar* aVar)
{
	TDF_Label nthLabel = parLabel.FindChild(nIndex);
	//名字，类型，关注名，是否关注
	nthLabel.ForgetAllAttributes();

	//名字，类型，关注名、还有类型、维度，暂时不考虑
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(nthLabel, 1, 3);
	aStrArray->SetValue(1, aVar->sFullName.c_str());
	aStrArray->SetValue(2, aVar->sType.c_str());
	aStrArray->SetValue(3, aVar->sFocusName.c_str());
	//...

	//是否关注
	Handle(TDataStd_BooleanArray) aBoolArray = TDataStd_BooleanArray::Set(nthLabel, 1, 1);
	aBoolArray->SetValue(1, aVar->bFocused);
}

bool DocumentComponent::SetCyberInfo(CyberInfo* pCyber)
{
	TDF_Label cyberLabel = m_pDocRoot.FindChild(6);

	//模型全名，参数集合，变量集合，接口类型列表
	TDF_Label nmLabel = cyberLabel.FindChild(1);
	TDF_Label parLabel = cyberLabel.FindChild(2);
	TDF_Label varLabel = cyberLabel.FindChild(3);
	TDF_Label connLabel = cyberLabel.FindChild(4);

	//全名
	nmLabel.ForgetAllAttributes();
	//TDataStd_Name::Set(nmLabel, pCyber->GetMdlName().c_str());
	Handle(TDataStd_ExtStringArray) aStrArray0 = TDataStd_ExtStringArray::Set(nmLabel, 1, 2);
	aStrArray0->SetValue(1, pCyber->GetMdlName().c_str());
	aStrArray0->SetValue(2, pCyber->GetMdlType().c_str());

	//参数
	vector<myParameter*> vPars = pCyber->GetParameters();
	TDataStd_Integer::Set(parLabel, vPars.size()); //设置参数个数
	vector<myParameter*>::iterator itr;
	int nIndex = 1;
	for (itr = vPars.begin(); itr != vPars.end(); ++itr)
	{
		myParameter* aPar = *itr;
		AddParameter(parLabel, nIndex, aPar);
		nIndex++;
	}
	//变量
	vector<myOutputVar*> vVars = pCyber->GetOutputVars();
	TDataStd_Integer::Set(varLabel, vVars.size());
	vector<myOutputVar*>::iterator itr1;
	nIndex = 1;
	for (itr1 = vVars.begin(); itr1 != vVars.end(); ++itr1)
	{
		myOutputVar* aVar = *itr1;
		AddOutputVar(varLabel, nIndex, aVar);
		nIndex++;
	}
	//接口类型和名字
	vector<string> vConTypes = pCyber->GetConnectorTypes();
	vector<string> vConNames = pCyber->GetConnectorNames();
	int nNum = vConTypes.size();
	TDataStd_Integer::Set(connLabel, nNum * 2); //接口个数

	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(connLabel, 1, nNum * 2);
	for (int i = 0; i < nNum; i++)
	{
		aStrArray->SetValue(i + 1, vConTypes[i].c_str());
	}
	for (int i = nNum; i < 2 * nNum; i++)
	{
		aStrArray->SetValue(i + 1, vConNames[i - nNum].c_str());
	}

	return true;
}

//从文档获取Cyber信息,如果失败返回null,此函数new CyberInfo，用户自己delete
CyberInfo* DocumentComponent::GetCyberInfo() const
{
	CyberInfo* pCyber = nullptr;

	TDF_Label cyberLabel = m_pDocRoot.FindChild(6, false);

	if (cyberLabel.IsNull())
	{
		return pCyber;
	}

	//模型全名，参数集合，变量集合，接口类型列表
	TDF_Label nmLabel = cyberLabel.FindChild(1, false);
	TDF_Label parLabel = cyberLabel.FindChild(2, false);
	TDF_Label varLabel = cyberLabel.FindChild(3, false);
	TDF_Label connLabel = cyberLabel.FindChild(4, false);
	if (nmLabel.IsNull())
	{
		return pCyber;
	}

	//模型全名,类型
	string sFullName, sTypeName;
	//Handle(TDataStd_Name) theMdlName;
	//nmLabel.FindAttribute(TDataStd_Name::GetID(), theMdlName);
	//if (theMdlName.IsNull())
	//{
	//	return pCyber;
	//}
	//TCollection_ExtendedString tcs = theMdlName->Get();
	//sFullName = ExtString2string(tcs);
	Handle(TDataStd_ExtStringArray) aStrArray0;
	nmLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray0);
	if (!aStrArray0.IsNull())
	{
		TCollection_ExtendedString s10 = aStrArray0->Value(1);
		sFullName = ExtString2string(s10);
		TCollection_ExtendedString s20 = aStrArray0->Value(2);
		sTypeName = ExtString2string(s20);
	}

	//参数
	vector<myParameter*> vPars;
	//获得参数个数
	Handle(TDataStd_Integer) ncont;
	parLabel.FindAttribute(TDataStd_Integer::GetID(), ncont);
	if (ncont.IsNull())
	{
		return pCyber;
	}

	int nCount = ncont->Get();
	for (int i = 1; i <= nCount; i++)
	{
		myParameter* aPar = GetParameter(parLabel, i);
		vPars.push_back(aPar);
	}

	//变量
	vector<myOutputVar*> vVars;
	//获得变量个数
	Handle(TDataStd_Integer) ncont1;
	varLabel.FindAttribute(TDataStd_Integer::GetID(), ncont1);
	if (ncont1.IsNull())
	{
		return pCyber;
	}

	int nCount1 = ncont1->Get();
	for (int i = 1; i <= nCount1; i++)
	{
		myOutputVar* aVar = GetOutputVar(varLabel, i);
		vVars.push_back(aVar);
	}

	//接口类型和名字
	vector<string> vTypes;
	vector<string> vNames;
	//获得接口个数
	Handle(TDataStd_Integer) ncont2;
	connLabel.FindAttribute(TDataStd_Integer::GetID(), ncont2);
	int nCount2 = ncont2->Get();
	Handle(TDataStd_ExtStringArray) aStrArray;
	connLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
	for (int i = 1; i <= nCount2 / 2; i++)
	{
		TCollection_ExtendedString s1 = aStrArray->Value(i);
		string str1 = ExtString2string(s1);
		vTypes.push_back(str1);
	}
	for (int i = nCount2 / 2 + 1; i <= nCount2; i++)
	{
		TCollection_ExtendedString s1 = aStrArray->Value(i);
		string str1 = ExtString2string(s1);
		vNames.push_back(str1);
	}

	pCyber = new CyberInfo(nullptr);
	pCyber->SetMdlName(sFullName);
	pCyber->SetMdlType(sTypeName.c_str());
	pCyber->SetParameters(vPars);
	pCyber->SetOutputVars(vVars);
	pCyber->SetConncTypes(vTypes);
	pCyber->SetConncNames(vNames);

	return pCyber;
}

myParameter* DocumentComponent::GetParameter(TDF_Label& parLabel, int nth) const
{
	myParameter* pPar = nullptr;

	TDF_Label nthLabel = parLabel.FindChild(nth, false);
	if (nthLabel.IsNull())
	{
		return  pPar;
	}
	//缺省值
	//Handle(TDataStd_Real) tdsr;
	//nthLabel.FindAttribute(TDataStd_Real::GetID(), tdsr);
	//if (tdsr.IsNull())
	//{
	//	return  pPar;
	//}
	//double dVal = tdsr->Get();
	//名字，单位，关注名、类型、维度
	Handle(TDataStd_ExtStringArray) aStrArray;// = TDataStd_ExtStringArray::Set(nthLabel, 1, 3);
	nthLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
	int n0 = aStrArray->Length();
	TCollection_ExtendedString tcn1 = aStrArray->Value(1);
	TCollection_ExtendedString tcn2 = aStrArray->Value(2);
	TCollection_ExtendedString tcn3 = aStrArray->Value(3);
	TCollection_ExtendedString tcn4;
	TCollection_ExtendedString tcn5;
	TCollection_ExtendedString tcn6;

	string sFullName = ExtString2string(tcn1);
	string sUnit = ExtString2string(tcn2);
	string sFocusName = ExtString2string(tcn3);
	string sType;
	string sDim;
	string sDefaultValue;

	if (aStrArray->Length() > 3)
	{
		tcn4 = aStrArray->Value(4);
		tcn5 = aStrArray->Value(5);
		tcn6 = aStrArray->Value(6);
		sType = ExtString2string(tcn4);
		sDim = ExtString2string(tcn5);
		sDefaultValue = ExtString2string(tcn6);
	}

	//是否guanzhu
	Handle(TDataStd_BooleanArray) aBoolArray;// = TDataStd_BooleanArray::Set(nthLabel, 1, 1);
	nthLabel.FindAttribute(TDataStd_BooleanArray::GetID(), aBoolArray);
	bool bFocused = aBoolArray->Value(1);

	pPar = new myParameter;
	pPar->sFullName = sFullName;
	pPar->defaultValue = sDefaultValue;
	pPar->bFocused = bFocused;
	pPar->sUnit = sUnit;
	pPar->sFocusName = sFocusName;
	pPar->sType = sType;
	pPar->sDimension = sDim;

	return pPar;
}
myOutputVar* DocumentComponent::GetOutputVar(TDF_Label& varLabel, int nth) const
{
	myOutputVar* pVar = nullptr;

	TDF_Label nthLabel = varLabel.FindChild(nth, false);
	if (nthLabel.IsNull())
	{
		return  pVar;
	}

	//名字，类型，关注名
	Handle(TDataStd_ExtStringArray) aStrArray;// = TDataStd_ExtStringArray::Set(nthLabel, 1, 3);
	nthLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
	if (aStrArray.IsNull())
	{
		return  pVar;
	}
	TCollection_ExtendedString tcn1 = aStrArray->Value(1);
	TCollection_ExtendedString tcn2 = aStrArray->Value(2);
	TCollection_ExtendedString tcn3 = aStrArray->Value(3);
	string sFullName = ExtString2string(tcn1);
	string sType = ExtString2string(tcn2);
	string sFocusName = ExtString2string(tcn3);
	//是否guanzhu
	Handle(TDataStd_BooleanArray) aBoolArray;// = TDataStd_BooleanArray::Set(nthLabel, 1, 1);
	nthLabel.FindAttribute(TDataStd_BooleanArray::GetID(), aBoolArray);
	bool bFocused = aBoolArray->Value(1);

	pVar = new myOutputVar;
	pVar->sFullName = sFullName;
	pVar->bFocused = bFocused;
	pVar->sType = sType;
	pVar->sFocusName = sFocusName;

	return pVar;
}

/// 参考点存取
/// 增，获、改、删
void DocumentComponent::AddDatumPoint(const TopoDS_Shape& dptShape, DatumPoint* pDPt)
{
	//定位到 参考点
	TDF_Label lblDatum = m_pDocRoot.FindChild(3);
	TDF_Label lblDatumPoints = lblDatum.FindChild(1); //第一类是参考点
	//获得参考点已有多少
	int nCount = lblDatumPoints.NbChildren();
	//定位到：要么是空属性（已删除的），要么增加一个
	bool bFindNull = false;
	TDF_Label theLabel;
	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label iLabel = lblDatumPoints.FindChild(i, false);
		if (iLabel.IsNull())
		{
			theLabel = lblDatumPoints.FindChild(i);
			bFindNull = true;
			break;
		}

		Handle(TDataStd_Integer) nType;
		if (!iLabel.FindAttribute(TDataStd_Integer::GetID(), nType) || nType.IsNull())
		{
			bFindNull = true;
			theLabel = iLabel; //就使用这个空的
			break;
		}
	}
	if (!bFindNull)
	{
		theLabel = lblDatumPoints.FindChild(nCount + 1); //如果没有空的，增加
	}

	//形状、类型，元素1、2，偏移值
	//形状
	TNaming_Builder B_i(theLabel);
	B_i.Generated(dptShape);
	//类型
	TDataStd_Integer::Set(theLabel, pDPt->GetDptType());
	//两个元素名字
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(theLabel, 1, 2);
	aStrArray->SetValue(1, pDPt->GetShape1Name().c_str());
	aStrArray->SetValue(2, pDPt->GetShape2Name().c_str());
	//pianyi值
	Handle(TDataStd_RealArray) pt3 = TDataStd_RealArray::Set(theLabel, 1, 3);
	pt3->SetValue(1, pDPt->GetOffsetValue()[0]);
	pt3->SetValue(2, pDPt->GetOffsetValue()[1]);
	pt3->SetValue(3, pDPt->GetOffsetValue()[2]);
}

void DocumentComponent::AddMarkingSurf(const TopoDS_Shape& markingShape, MarkingSurface* pMSt)
{
	TDF_Label lblDatum = m_pDocRoot.FindChild(3);
	TDF_Label lblMarkingSurf = lblDatum.FindChild(3);
	//获得参考点已有多少
	int nCount = lblMarkingSurf.NbChildren();
	//定位到：要么是空属性（已删除的），要么增加一个
	bool bFindNull = false;
	TDF_Label theLabel;
	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label iLabel = lblMarkingSurf.FindChild(i, false);
		Handle(TDataStd_Integer) nType;
		if (!iLabel.FindAttribute(TDataStd_Integer::GetID(), nType) || nType.IsNull())
		{
			bFindNull = true;
			theLabel = iLabel; //就使用这个空的
			break;
		}
	}
	if (!bFindNull)
	{
		theLabel = lblMarkingSurf.FindChild(nCount + 1); //如果没有空的，增加
	}

	//形状、类型，半径，偏移值
	//形状
	TNaming_Builder B_i(theLabel);
	B_i.Generated(markingShape);
	//类型
	TDataStd_Integer::Set(theLabel, pMSt->GetMsurfType());
	//半径
	Handle(TDataStd_RealArray) rArray = TDataStd_RealArray::Set(theLabel, 1, 2);
	rArray->SetValue(1, pMSt->GetRadius());
	//偏移值
	Handle(TDataStd_RealArray) pt3 = TDataStd_RealArray::Set(theLabel, 1, 3);
	pt3->SetValue(1, pMSt->GetOffsetValue()[0]);
	pt3->SetValue(2, pMSt->GetOffsetValue()[1]);
	pt3->SetValue(3, pMSt->GetOffsetValue()[2]);
}

//从Label创建一个基准点
DatumPoint* DocumentComponent::CreateDatumPointFromLabel(TDF_Label theLabel)
{
	//获得类型，元素1，2，值
	Handle(TDataStd_Integer) iType;
	theLabel.FindAttribute(TDataStd_Integer::GetID(), iType);
	if (iType.IsNull())  return nullptr;
	myDatumPointType atype = (myDatumPointType)iType->Get();

	//两个元素名字
	Handle(TDataStd_ExtStringArray) aStrArray;
	theLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
	TCollection_ExtendedString s1 = aStrArray->Value(1);
	string str1 = ExtString2string(s1);
	TCollection_ExtendedString s2 = aStrArray->Value(2);
	string str2 = ExtString2string(s2);
	//值
	Handle(TDataStd_RealArray) clr;
	theLabel.FindAttribute(TDataStd_RealArray::GetID(), clr);
	vector<double> offXYZ;
	offXYZ.push_back(clr->Value(1));
	offXYZ.push_back(clr->Value(2));
	offXYZ.push_back(clr->Value(3));

	DatumPoint* pDpt = new DatumPoint();
	pDpt->SetDptType(atype);
	pDpt->SetShape1Name(str1.c_str());
	pDpt->SetShape2Name(str2.c_str());
	pDpt->SetOffsetValue(offXYZ);

	return pDpt;
}

//从文档得到数据
DatumPoint* DocumentComponent::GetDatumPoint(const TopoDS_Shape& dptShape)
{
	//定位到 参考点
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	TDF_Label lblDatumPoints = lblDatum.FindChild(1, false); //第一类是参考点

	//由形状定位
	int nth = 0;
	TDF_Label theLabel = LookupForShapeLabelFromFather(lblDatumPoints, dptShape, nth);
	if (theLabel.IsNull())
	{
		return nullptr;
	}

	DatumPoint* pDpt = CreateDatumPointFromLabel(theLabel);
	pDpt->SetShape(dptShape);

	return pDpt;
}

//先删除，再增加
bool DocumentComponent::UpdateDatumPoint(const TopoDS_Shape& dptShape, DatumPoint* pDPt)
{
	if (!DeleteDatumPoint(dptShape))
	{
		return false;
	}

	AddDatumPoint(dptShape, pDPt);

	return true;
}

//只能删除属性 forgetall
bool DocumentComponent::DeleteDatumPoint(const TopoDS_Shape& dptShape)
{
	//定位到 参考点
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	TDF_Label lblDatumPoints = lblDatum.FindChild(1, false); //第一类是参考点

	//由形状定位
	int nth = 0;
	TDF_Label theLabel = LookupForShapeLabelFromFather(lblDatumPoints, dptShape, nth);
	if (!theLabel.IsNull())
	{
		theLabel.ForgetAllAttributes();
		return true;
	}
	//没有找到
	return false;
}

//从Label创建一个标记面
MarkingSurface* DocumentComponent::CreateMarkingSurfFromLabel(TDF_Label theLabel)
{
	//获得类型，半径，值
	Handle(TDataStd_Integer) iType;
	theLabel.FindAttribute(TDataStd_Integer::GetID(), iType);
	if (iType.IsNull())  return nullptr;
	myMarkingSurfaceType atype = (myMarkingSurfaceType)iType->Get();

	//两个元素名字
	Handle(TDataStd_RealArray) rArray;
	theLabel.FindAttribute(TDataStd_RealArray::GetID(), rArray);
	TCollection_ExtendedString s1 = rArray->Value(1);
	double radius = rArray->Value(1);
	//值
	Handle(TDataStd_RealArray) clr;
	theLabel.FindAttribute(TDataStd_RealArray::GetID(), clr);
	vector<double> offXYZ;
	offXYZ.push_back(clr->Value(1));
	offXYZ.push_back(clr->Value(2));
	offXYZ.push_back(clr->Value(3));

	MarkingSurface* pMSt = new MarkingSurface();
	pMSt->SetMsurfType(atype);
	pMSt->SetRadius(radius);
	pMSt->SetOffsetValue(offXYZ);

	return pMSt;
}

MarkingSurface* DocumentComponent::GetMarkingSurf(const TopoDS_Shape& connShape)
{
	//定位到标记面
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	TDF_Label lblMarkingSurf = lblDatum.FindChild(3, false);

	//由形状定位
	int nth = 0;
	TDF_Label theLabel = LookupForShapeLabelFromFather(lblMarkingSurf, connShape, nth);
	if (theLabel.IsNull())
	{
		return nullptr;
	}

	MarkingSurface* pMSt = CreateMarkingSurfFromLabel(theLabel);
	pMSt->SetShape(connShape);

	return pMSt;

}

bool DocumentComponent::UpdateMarkingSurf(const TopoDS_Shape& connShape, MarkingSurface* pMSt)
{
	if (!DeleteMarkingSurf(connShape))
	{
		return false;
	}

	AddMarkingSurf(connShape, pMSt);

	return true;
}

bool DocumentComponent::DeleteMarkingSurf(const TopoDS_Shape& connShape)
{
	//定位到 参考点
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	TDF_Label lblMarkingSurf = lblDatum.FindChild(3, false); //第三类是标记面

	//由形状定位
	int nth = 0;
	TDF_Label theLabel = LookupForShapeLabelFromFather(lblMarkingSurf, connShape, nth);
	if (!theLabel.IsNull())
	{
		theLabel.ForgetAllAttributes();
		return true;
	}
	//没有找到
	return false;
}

/// 参考线存取
/// 增，获、改、删
void DocumentComponent::AddDatumLine(const TopoDS_Shape& dptShape, DatumLine* pDLine)
{
	//定位到 参考xian
	TDF_Label lblDatum = m_pDocRoot.FindChild(3);
	TDF_Label lblDatumPoints = lblDatum.FindChild(2); //第2类是参考xiian
	//获得参考点已有多少
	int nCount = lblDatumPoints.NbChildren();
	//定位到：要么是空属性（已删除的），要么增加一个
	bool bFindNull = false;
	TDF_Label theLabel;
	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label iLabel = lblDatumPoints.FindChild(i, false);
		Handle(TDataStd_Integer) nType;
		if (!iLabel.FindAttribute(TDataStd_Integer::GetID(), nType) || nType.IsNull())
		{
			bFindNull = true;
			theLabel = iLabel; //就使用这个空的
			break;
		}
	}
	if (!bFindNull)
	{
		theLabel = lblDatumPoints.FindChild(nCount + 1); //如果没有空的，增加
	}

	//形状、类型，元素1、2，偏移值
	//形状
	TNaming_Builder B_i(theLabel);
	B_i.Generated(dptShape);
	//类型
	TDataStd_Integer::Set(theLabel, pDLine->GetDlnType());
	//两个元素名字
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(theLabel, 1, 2);
	aStrArray->SetValue(1, pDLine->GetShape1Name().c_str());
	aStrArray->SetValue(2, pDLine->GetShape2Name().c_str());
	//pianyi值
	Handle(TDataStd_RealArray) pt3 = TDataStd_RealArray::Set(theLabel, 1, 3);
	pt3->SetValue(1, pDLine->GetOffsetValue()[0]);
	pt3->SetValue(2, pDLine->GetOffsetValue()[1]);
	pt3->SetValue(3, pDLine->GetOffsetValue()[2]);
}

DatumLine* DocumentComponent::CreateDatumLineFromLabel(TDF_Label theLabel)
{
	//获得类型，元素1，2，值
	Handle(TDataStd_Integer) iType;
	theLabel.FindAttribute(TDataStd_Integer::GetID(), iType);
	if (iType.IsNull())  return nullptr;
	myDatumLineType atype = (myDatumLineType)iType->Get();

	//两个元素名字
	Handle(TDataStd_ExtStringArray) aStrArray;
	theLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
	TCollection_ExtendedString s1 = aStrArray->Value(1);
	string str1 = ExtString2string(s1);
	TCollection_ExtendedString s2 = aStrArray->Value(2);
	string str2 = ExtString2string(s2);
	//值
	Handle(TDataStd_RealArray) clr;
	theLabel.FindAttribute(TDataStd_RealArray::GetID(), clr);
	vector<double> offXYZ;
	offXYZ.push_back(clr->Value(1));
	offXYZ.push_back(clr->Value(2));
	offXYZ.push_back(clr->Value(3));

	DatumLine* pDln = new DatumLine();
	pDln->SetDlnType(atype);
	pDln->SetShape1Name(str1.c_str());
	pDln->SetShape2Name(str2.c_str());
	pDln->SetOffsetValue(offXYZ);

	return pDln;
}

//从文档得到数据
DatumLine* DocumentComponent::GetDatumLine(const TopoDS_Shape& dptShape)
{
	//定位到 参考
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	TDF_Label lblDatumPoints = lblDatum.FindChild(2, false); //第2类是

	//由形状定位
	int nth = 0;
	TDF_Label theLabel = LookupForShapeLabelFromFather(lblDatumPoints, dptShape, nth);
	if (theLabel.IsNull())
	{
		return nullptr;
	}

	DatumLine* pDln = CreateDatumLineFromLabel(theLabel);
	pDln->SetShape(dptShape);

	return pDln;
}

//先删除，再增加
bool DocumentComponent::UpdateDatumLine(const TopoDS_Shape& dptShape, DatumLine* pDPt)
{
	if (!DeleteDatumLine(dptShape))
	{
		return false;
	}

	AddDatumLine(dptShape, pDPt);

	return true;
}

//只能删除属性 forgetall
bool DocumentComponent::DeleteDatumLine(const TopoDS_Shape& dptShape)
{
	//定位到 参考
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	TDF_Label lblDatumPoints = lblDatum.FindChild(2, false); //第一类是参考点

	//由形状定位
	int nth = 0;
	TDF_Label theLabel = LookupForShapeLabelFromFather(lblDatumPoints, dptShape, nth);
	if (!theLabel.IsNull())
	{
		theLabel.ForgetAllAttributes();
		return true;
	}
	//没有找到
	return false;
}

//增加接口，在Root下增加子节点存储各接口
void DocumentComponent::AddConnector(Connector* pConn)//先Generate()创建形状
{
	//定位到 接口
	TDF_Label lblConnectors = m_pDocRoot.FindChild(4);
	//获得接口已有多少
	int nCount = lblConnectors.NbChildren();
	//定位到：要么是空属性（已删除的），要么增加一个
	bool bFindNull = false;
	TDF_Label theLabel;
	for (int i = 1; i <= nCount; i++)
	{
		TDF_Label iLabel = lblConnectors.FindChild(i, false);
		Handle(TDataStd_Integer) nType;
		if (!iLabel.FindAttribute(TDataStd_Integer::GetID(), nType) || nType.IsNull())
		{
			bFindNull = true;
			theLabel = iLabel; //就使用这个空的
			break;
		}
	}
	if (!bFindNull)
	{
		theLabel = lblConnectors.FindChild(nCount + 1); //如果没有空的，增加
	}

	//形状、接口类型、定义类型，元素1、2（3），位姿6
	//形状
	TNaming_Builder B_i(theLabel);
	TopoDS_Shape connShape = pConn->GetShape();
	B_i.Generated(connShape);
	//接口类型
	TDataStd_Name::Set(theLabel, pConn->GetConnType().c_str());
	//定义类型
	TDataStd_Integer::Set(theLabel, pConn->GetDefineType());
	//接口名字、1个~三个 元素名字
	vector<string> vRefNames = pConn->GetRefEnts();
	Handle(TDataStd_ExtStringArray) aStrArray = TDataStd_ExtStringArray::Set(theLabel, 1, 1 + vRefNames.size());
	aStrArray->SetValue(1, pConn->GetConnName().c_str());

	if (vRefNames.size() >= 1)
	{
		aStrArray->SetValue(2, vRefNames[0].c_str());

		if (vRefNames.size() >= 2)
		{
			aStrArray->SetValue(3, vRefNames[1].c_str());
			if (vRefNames.size() == 3)
			{
				aStrArray->SetValue(4, vRefNames[2].c_str());
			}
		}
	}
	//Z反向
	Handle(TDataStd_BooleanArray) aBoolArray = TDataStd_BooleanArray::Set(theLabel, 1, 1);
	aBoolArray->SetValue(1, pConn->IsZ_Reverse());

	//位姿
	Handle(TDataStd_RealArray) pt9 = TDataStd_RealArray::Set(theLabel, 1, 9);
	vector<double> vPosOrent = pConn->GetDisplacement();
	for (int j = 0; j < vPosOrent.size(); j++)
	{
		pt9->SetValue(j + 1, vPosOrent[j]);
	}
}

Connector* DocumentComponent::CreateConnectorFromLabel(TDF_Label theLabel)
{
	//获得接口类型
	Handle(TDataStd_Name) extName;
	theLabel.FindAttribute(TDataStd_Name::GetID(), extName);
	if (extName.IsNull())
	{
		return nullptr;
	}
	string sConnType = ExtString2string(extName->Get());
	//获得定义类型
	Handle(TDataStd_Integer) iType;
	theLabel.FindAttribute(TDataStd_Integer::GetID(), iType);
	if (iType.IsNull())  return nullptr;
	myConnectDefineType atype = (myConnectDefineType)iType->Get();

	//接口名字、1-3个元素名字
	Handle(TDataStd_ExtStringArray) aStrArray;
	theLabel.FindAttribute(TDataStd_ExtStringArray::GetID(), aStrArray);
	//此处需要处理，根据类型
	vector<string> vRefNames;
	TCollection_ExtendedString s1 = aStrArray->Value(1);
	string sConnName = ExtString2string(s1);
	TCollection_ExtendedString s2 = aStrArray->Value(2);
	string str2 = ExtString2string(s2);
	vRefNames.push_back(str2);
	if (atype != myConnectDefineType::ONE_POINT && aStrArray->Length() >= 3)
	{
		TCollection_ExtendedString s3 = aStrArray->Value(3);
		string str3 = ExtString2string(s3);
		vRefNames.push_back(str3);
		if (atype == myConnectDefineType::THREE_POINTS && aStrArray->Length() >= 4)
		{//需要三个参考实体
			TCollection_ExtendedString s4 = aStrArray->Value(4);
			string str4 = ExtString2string(s4);
			vRefNames.push_back(str4);
		}
	}

	//9ge值，位姿
	Handle(TDataStd_RealArray) clr;
	theLabel.FindAttribute(TDataStd_RealArray::GetID(), clr);
	vector<double> dbl9;
	for (int j = 0; j < 9; j++)
	{
		dbl9.push_back(clr->Value(j + 1));
	}

	//Z反向
	Handle(TDataStd_BooleanArray) aBoolArray;
	theLabel.FindAttribute(TDataStd_BooleanArray::GetID(), aBoolArray);
	bool bActive = aBoolArray->Value(1);

	Connector* pConn = new Connector();
	pConn->SetConnType(sConnType);
	pConn->SetDefineType(atype);
	pConn->SetConnName(sConnName.c_str());
	pConn->SetRefEnts(vRefNames);
	pConn->SetZ_Reverse(bActive);
	pConn->SetDisplacement(dbl9);

	return pConn;
}

//由形状得到接口的信息；此new Connector，用户根据需要delete,如果该形状不是Connector，则返回null
Connector* DocumentComponent::GetConnector(const TopoDS_Shape& connShape) const
{
	//定位到 
	TDF_Label lblConnector = m_pDocRoot.FindChild(4, false);

	//由形状定位
	int nth = 0;
	TDF_Label theLabel = LookupForShapeLabelFromFather(lblConnector, connShape, nth);
	if (theLabel.IsNull())
	{
		return nullptr;
	}

	Connector* pConn = CreateConnectorFromLabel(theLabel);
	pConn->SetShape(connShape);

	return pConn;
}

//更新已有接口
bool DocumentComponent::UpdateConnector(const TopoDS_Shape& connShape, Connector* pCon)
{
	if (!DeleteConnector(connShape))
	{
		return false;
	}

	AddConnector(pCon);
	return true;
}

//删除接口
bool DocumentComponent::DeleteConnector(const TopoDS_Shape& connShape)
{
	//定位到
	TDF_Label lblConnector = m_pDocRoot.FindChild(4, false);

	//由形状定位
	int nth = 0;
	TDF_Label theLabel = LookupForShapeLabelFromFather(lblConnector, connShape, nth);
	if (!theLabel.IsNull())
	{
		theLabel.ForgetAllAttributes();
		return true;
	}
	//没有找到
	return false;
}

//读文件*.m3dcom，建立文档对象
DocumentComponent* DocumentComponent::ReadComponentFile(const char* file)
{
	DocumentComponent* pMdl = nullptr;
	QString fileName = file;

	Handle(TDocStd_Application) anOcaf_Application = new TDocStd_Application;
	// load persistence
	BinDrivers::DefineFormat(anOcaf_Application);
	// Look for already opened
	if (anOcaf_Application->IsInSession(fileName.toStdU16String().data()))
	{
		return pMdl;
	}
	// Open the document in the current application
	Handle(TDocStd_Document) anM3dOcafDoc;
	PCDM_ReaderStatus aReaderStatus = anOcaf_Application->Open(fileName.toStdU16String().data(), anM3dOcafDoc);
	if (aReaderStatus == PCDM_RS_OK)
	{
		pMdl = new DocumentComponent();
		pMdl->SetFileName(file);
		pMdl->SetRoot(anM3dOcafDoc->Main());
	}

	return pMdl;
}

vector<Connector*> DocumentComponent::GetConnectors() const
{
	return GetAllConnectors();
}

//根据文档内容，构建Component零件对象
Component* DocumentComponent::GenerateComponent()
{
	Component* pCom = new Component(this);
	SetCompRoot(pCom);

	////是否装配、组件名、文件、位姿等信息，都在外面赋值

	////约束信息??????暂时不去实现
	//vector<myConstraint> geomConstraints = GetAllConstraints();
	//pCom->SetGeomConstraints(geomConstraints);

	////零件主实体
	//Graphic3d_NameOfMaterial mat;
	//TopoDS_Shape mainSolid = GetMainSolid(mat);
	//pCom->SetMaterial(mat);
	////基准
	//vector<DatumPoint*> vDatumPts = GetAllDatumPoints();
	//pCom->SetDatumPoints(vDatumPts);
	//vector<DatumLine*> vDatumLns = GetAllDatumLines();
	//pCom->SetDatumLines(vDatumLns);
	////接口
	//vector<Connector*> vConnectors = GetAllConnectors();
	//pCom->SetConnectors(vConnectors);

	////创建形状组合体
	//TopoDS_Compound theCompound;
	//BRep_Builder aBuilder;
	//aBuilder.MakeCompound(theCompound);
	//aBuilder.Add(theCompound, mainSolid);
	//for (int i = 0; i < vDatumPts.size(); i++)
	//{
	//	//REM by WU，这几个已经单独显示，不再加入到零件中
	//	;// aBuilder.Add(theCompound, vDatumPts[i]->GetShape());
	//}
	//for (int i = 0; i < vDatumLns.size(); i++)
	//{
	//	//REM by WU，这几个已经单独显示，不再加入到零件中
	//	;// aBuilder.Add(theCompound, vDatumLns[i]->GetShape());
	//}
	//for (int j = 0; j < vConnectors.size(); j++)
	//{
	//	//REM by WU，这几个已经单独显示，不再加入到零件中
	//	;// aBuilder.Add(theCompound, vConnectors[j]->GetShape());
	//}
	//pCom->SetShape(theCompound);

	////颜色
	//vector<ShapeColor> mapShpColor = GetAllShapeColors();
	//pCom->SetShapeColors(mapShpColor);

	////CyberInfo
	//CyberInfo* aCyber = GetCyberInfo();
	//pCom->SetCyberInfo(aCyber);

	return pCom;
}

vector<DatumPoint*> DocumentComponent::GetAllDatumPoints() const
{
	vector<DatumPoint*> vRet;

	//定位到 参考点
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	if (lblDatum.IsNull())
	{
		return vRet;
	}
	TDF_Label lblDatumPoints = lblDatum.FindChild(1, false); //第一类是参考点
	if (lblDatumPoints.IsNull())
	{
		return vRet;
	}

	TDF_Label retLabel;
	Standard_Integer ncount = lblDatumPoints.NbChildren();
	for (int i = 0; i < ncount; i++)
	{
		TDF_Label lbl_i = lblDatumPoints.FindChild(i + 1, false);
		if (lbl_i.IsNull()) continue;

		Handle(TNaming_NamedShape) aNamedShape1;
		lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		if (aNamedShape1.IsNull())
		{
			continue;
		}
		TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

		if (!shapej.IsNull())
		{
			DatumPoint* dpt = CreateDatumPointFromLabel(lbl_i);
			dpt->SetShape(shapej);
			vRet.push_back(dpt);
		}
	}

	return vRet;
}

vector<DatumLine*>  DocumentComponent::GetAllDatumLines() const
{
	vector<DatumLine*> vRet;

	//定位到 参考xian
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	if (lblDatum.IsNull())
	{
		return vRet;
	}
	TDF_Label lblDatumLines = lblDatum.FindChild(2, false); //第一类是参考
	if (lblDatumLines.IsNull())
	{
		return vRet;
	}

	Standard_Integer ncount = lblDatumLines.NbChildren();
	for (int i = 0; i < ncount; i++)
	{
		TDF_Label lbl_i = lblDatumLines.FindChild(i + 1, false);
		Handle(TNaming_NamedShape) aNamedShape1;
		lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		if (!aNamedShape1.IsNull())
		{
			TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

			if (!shapej.IsNull())
			{
				DatumLine* dln = CreateDatumLineFromLabel(lbl_i);
				dln->SetShape(shapej);
				vRet.push_back(dln);
			}
		}
	}

	return vRet;
}

vector<MarkingSurface*> DocumentComponent::GetAllMarkingSurface() const
{
	vector<MarkingSurface*> vRet;

	//定位到 参考点
	TDF_Label lblDatum = m_pDocRoot.FindChild(3, false);
	if (lblDatum.IsNull())
	{
		return vRet;
	}
	TDF_Label lblMarkingSurface = lblDatum.FindChild(3, false); //第三类是标记面
	if (lblMarkingSurface.IsNull())
	{
		return vRet;
	}

	TDF_Label retLabel;
	Standard_Integer ncount = lblMarkingSurface.NbChildren();
	for (int i = 0; i < ncount; i++)
	{
		TDF_Label lbl_i = lblMarkingSurface.FindChild(i + 1, false);
		if (lbl_i.IsNull()) continue;

		Handle(TNaming_NamedShape) aNamedShape1;
		lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		if (aNamedShape1.IsNull())
		{
			continue;
		}
		TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

		if (!shapej.IsNull())
		{
			MarkingSurface* mst = CreateMarkingSurfFromLabel(lbl_i);
			mst->SetShape(shapej);
			vRet.push_back(mst);
		}
	}

	return vRet;
}

//根据名字得到接口
Connector* DocumentComponent::GetConnector(const char* sName) const
{
	Connector* pConnRet = nullptr;

	vector<Connector*> vRet = GetAllConnectors();
	for (Connector* pConn : vRet)
	{
		if (pConn->GetConnName() == sName)
		{
			pConnRet = pConn;
		}
		else {
			delete pConn;
		}
	}

	return pConnRet;
}

vector<Connector*>  DocumentComponent::GetAllConnectors() const
{
	vector<Connector*> vRet;

	//定位到 参考接口
	TDF_Label lblConnector = m_pDocRoot.FindChild(4, false);
	if (lblConnector.IsNull())
	{
		return vRet;
	}

	Standard_Integer ncount = lblConnector.NbChildren();
	for (int i = 0; i < ncount; i++)
	{
		TDF_Label lbl_i = lblConnector.FindChild(i + 1, false);
		Handle(TNaming_NamedShape) aNamedShape1;
		lbl_i.FindAttribute(TNaming_NamedShape::GetID(), aNamedShape1);
		if (aNamedShape1.IsNull())
		{
			continue;
		}
		TopoDS_Shape shapej = TNaming_Tool::GetShape(aNamedShape1);

		if (!shapej.IsNull())
		{
			Connector* dln = CreateConnectorFromLabel(lbl_i);
			dln->SetShape(shapej);
			vRet.push_back(dln);
		}
	}

	return vRet;
}

//不能用string来寻找了，在装配体中
vector<ShapeColor> DocumentComponent::GetAllShapeColors() const
{
	vector<ShapeColor> mapRet;

	TDF_Label clrLabel = m_pDocRoot.FindChild(2, false);
	if (clrLabel.IsNull())
	{
		return mapRet;
	}

	int nCountChilds = clrLabel.NbChildren();
	for (int i = 1; i <= nCountChilds; i++)
	{
		TDF_Label lbl_i = clrLabel.FindChild(i, false);
		Handle(TDataStd_Name) nameAttr;
		lbl_i.FindAttribute(TDataStd_Name::GetID(), nameAttr);
		if (nameAttr.IsNull())
		{
			continue;
		}
		//string Name = (const char*)nameAttr->Get();
		TCollection_ExtendedString s = nameAttr->Get();
		string Name = ExtString2string(s);

		Handle(TDataStd_RealArray) clr;
		lbl_i.FindAttribute(TDataStd_RealArray::GetID(), clr);
		Quantity_Color CFL(clr->Value(1), clr->Value(2), clr->Value(3), Quantity_TOC_RGB);
		TopoDS_Shape shp = GetShapeFromName(Name);
		ShapeColor sc;
		sc.theColor = CFL;
		sc.theShape = shp;
		//mapRet.insert(make_pair(Name, sc));
		mapRet.push_back(sc);
	}

	return mapRet;
}

//删除 引用 对象
void DocumentComponent::DeleteRefObjects(const char* sName)
{
	//参考点
	vector<DatumPoint*> vPts = GetAllDatumPoints();
	for (int i = 0; i < vPts.size(); i++)
	{
		DatumPoint* dpt = vPts[i];
		if (dpt->GetShape1Name() == sName || dpt->GetShape2Name() == sName)
		{
			DeleteDatumPoint(dpt->GetShape());
		}
		delete dpt;
	}
	vPts.clear();

	//参考线
	vector<DatumLine*> vLns = GetAllDatumLines();
	for (int i = 0; i < vLns.size(); i++)
	{
		DatumLine* dpt = vLns[i];
		if (dpt->GetShape1Name() == sName || dpt->GetShape2Name() == sName)
		{
			DeleteDatumLine(dpt->GetShape());
		}
		delete dpt;
	}
	vLns.clear();

	//接口
	vector<Connector*> vCtrs = GetAllConnectors();
	for (int i = 0; i < vCtrs.size(); i++)
	{
		Connector* dpt = vCtrs[i];
		vector<string> vRefs = dpt->GetRefEnts();
		for (int j = 0; j < vRefs.size(); j++)
		{
			if (vRefs[j] == sName)
			{
				DeleteConnector(dpt->GetShape());
				break;
			}
		}
		delete dpt;
	}
	vCtrs.clear();

	//几何约束
	vector<myConstraint> vConstrs = GetAllConstraints();
	for (int i = 0; i < vConstrs.size(); i++)
	{
		myConstraint ctrnt = vConstrs[i];
		if (ctrnt.shapeName1 == sName || ctrnt.shapeName2 == sName)
		{
			DeleteConstraint(ctrnt.nIndex);
		}
	}
}

void DocumentComponent::Clear()
{
	m_pDocRoot.ForgetAllAttributes(true);
}

//计算质量 = 体积*密度
double DocumentComponent::ComputeM()
{
	double m = 0;
	Graphic3d_NameOfMaterial theMat;
	TopoDS_Shape theMainSolid = GetMainSolid(theMat);
	GProp_GProps aGProps;
	BRepGProp::VolumeProperties(theMainSolid, aGProps);
	Standard_Real aVolume = aGProps.Mass();

	double density = g_vMaterials[theMat].fMaterialDensity;
	m = aVolume * density;

	return m;
}

//计算frame_a到质心
//将质心放在frame_a的局部坐标系下
gp_Vec DocumentComponent::ComputeR_CM()
{
	gp_Vec vR_CM;

	Graphic3d_NameOfMaterial theMat;
	TopoDS_Shape theMainSolid = GetMainSolid(theMat);
	GProp_GProps aGProps;
	BRepGProp::VolumeProperties(theMainSolid, aGProps);
	//质心坐标
	gp_Pnt aCOM = aGProps.CentreOfMass(); 

	//frame_a坐标
	Connector* pConn = GetConnector("frame_a");
	if (pConn)
	{
		gp_Pnt pFrame_a = pConn->GetCenter();

		vR_CM = gp_Vec(pFrame_a, aCOM);
	}
	else{
		gp_Pnt p0(0, 0, 0);
		vR_CM = gp_Vec(aCOM,p0);
	}
	//变换
	vector<double> v9 = pConn->GetDisplacement();
	gp_Ax3 Ax3_a = MakeAx3(v9);
	gp_Ax3 Ax3_O; //原点坐标系
	gp_Trsf trsf;
	trsf.SetDisplacement(Ax3_a, Ax3_O);
	vR_CM.Transform(trsf);

	delete pConn;

	return vR_CM;
}

//计算frame_a到frame_b
//并将r放到frame_a的局部坐标系下
gp_Vec DocumentComponent::ComputeR_ab()
{
	gp_Vec r_ab;
	Connector* pConnA = GetConnector("frame_a");
	Connector* pConnB = GetConnector("frame_b");
	if (pConnA && pConnB)
	{
		gp_Pnt pA = pConnA->GetCenter();
		gp_Pnt pB = pConnB->GetCenter();
		r_ab = gp_Vec(pA, pB);
	}

	//变换
	vector<double> v9 = pConnA->GetDisplacement();
	gp_Ax3 Ax3_a = MakeAx3(v9);
	gp_Ax3 Ax3_O; //原点坐标系
	gp_Trsf trsf;
	trsf.SetDisplacement(Ax3_a, Ax3_O);
	r_ab.Transform(trsf);

	if (pConnA) delete pConnA;
	if (pConnB) delete pConnB;

	return r_ab;
}

//计算I
//原点在质心，坐标轴与frame_a(如果没有全局坐标系XYZ）对齐
gp_Mat DocumentComponent::ComputeMoment()
{
	gp_Mat matI;

	Graphic3d_NameOfMaterial theMat;
	TopoDS_Shape theMainSolid = GetMainSolid(theMat);
	GProp_GProps aGProps;
	BRepGProp::VolumeProperties(theMainSolid, aGProps);

	Connector* pConn = GetConnector("frame_a");
	if (pConn)
	{
		gp_Pnt aCOM = aGProps.CentreOfMass();

		gp_Trsf theTransform;
		gp_Ax3 Ax3_1; //原点 在质心，坐标系与全局坐标系对齐
		Ax3_1.SetLocation(aCOM);
		gp_Ax3 Ax3_2; //原点在质心，坐标轴与frame_a对齐
		vector<double> v9 = pConn->GetDisplacement();
		Ax3_2 = MakeAx3(v9);
		Ax3_2.SetLocation(aCOM);

		theTransform.SetDisplacement(Ax3_1, Ax3_2);
		BRepBuilderAPI_Transform myBRepTransformation(theMainSolid, theTransform);
		TopoDS_Shape theTransformedSolid = myBRepTransformation.Shape();
		BRepGProp::VolumeProperties(theTransformedSolid, aGProps);

		delete pConn;
	}

	//惯性积单位 Kg*m2，应该乘以密度
	double density = g_vMaterials[theMat].fMaterialDensity;
	matI = aGProps.MatrixOfInertia()*density;

	return matI;
}

bool DocumentComponent::IsWorld() const
{
	CyberInfo* pInfo = GetCyberInfo();
	if (!pInfo)
	{
		return false;
	}

	string sMdlFullName = pInfo->GetMdlName();

	delete pInfo;

	if (sMdlFullName == "Modelica.Mechanics.MultiBody.World")
	{
		return true;
	}

	return false;
}


void DocumentComponent::Texture()
{
	//判断选中的单一形状是否为 面 或 体
	myContext->InitSelected();
	if (myContext->HasSelectedShape() && myContext->NbSelected() == 1)
	{
		Handle(AIS_InteractiveObject) obj = myContext->SelectedInteractive();
		if (obj->IsKind(STANDARD_TYPE(AIS_Shape)))
		{
			TopoDS_Shape theShape = myContext->SelectedShape(); // Handle(AIS_Shape)::DownCast(obj)->Shape();
			//TopAbs_ShapeEnum theType = theShape.ShapeType();
			//选择位图文件
			QString fileName = QFileDialog::getOpenFileName(NULL, tr("Open File"), "",
				tr("All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)"));
			if (!fileName.isEmpty())
			{
				//执行贴图
				const TCollection_AsciiString anUtf8Path(fileName.toStdU16String().data());
				Handle(AIS_TexturedShape) aTFace = Texturize(theShape, anUtf8Path);
				myContext->Display(aTFace, true);

				//添加到文档
				SetShapeTexture(theShape, fileName.toStdString().c_str());
			}
		}
	}
	else {
		QMessageBox::information(NULL, "选择面或体", "请选择1个面或体元素!");
	}
}
