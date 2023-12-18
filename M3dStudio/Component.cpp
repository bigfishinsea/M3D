#include "DocumentModel.h"
#include "M3d.h"
#include "Component.h"
#include "global.h"
#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <Prs3d_LineAspect.hxx>
#include "Connector.h"
#include "DocumentComponent.h"
//#include "DocumentModel.h"
#include "AdaptorVec_AIS.h"
#include "ModelicaModel.h"
#include <set>
#include <AIS_TexturedShape.hxx>
#include "LibPathsSetDlg.h"

//////////////////////////////////////////////////////
extern Handle(AIS_TexturedShape) Texturize(const TopoDS_Shape& aShape, TCollection_AsciiString aTFileName, \
    Standard_Real toScaleU, Standard_Real toScaleV, Standard_Real toRepeatU, Standard_Real toRepeatV, \
    Standard_Real originU, Standard_Real originV);

/// <summary>
/// //////////////
/// </summary>
ComponentAttrib::ComponentAttrib()
{    //variablity
    iVariablity = 0; //0-缺省；1-Constant; 2-parameter; 3-Discrete
    //Casuality
    iCasuality = 0; //0-None; 1-Input; 2-Output
    //ConnectorMember
    iConnectorMember = 0;//0-常规; 1-flow; 2-stream
    //Properties
    bFinal = false;
    bProtected = false;
    bReplaceable = false;
    //Inner/Outer
    bInner = false;
    bOuter = false;
}

/////////////////////////////////////////////////
///
//Component对象
///
Component::Component(DocumentCommon* pDoc, Component* parent)
{
    m_pDoc = pDoc;
    m_parent = parent;

    ////缺省位姿:FromSystem，ToSystem
    //vector<double> v0 = { 0,0,0,0,0,1,1,0,0,  0,0,0,0,0,1,1,0,0 };
    //m_transform = v0;
}

Component::~Component()
{
    //删除子组件，自动递归
    list<Component*>::iterator itr = m_lstChildren.begin();
    for (; itr != m_lstChildren.end(); ++itr)
    {
        Component* pCom = *itr;
        delete pCom;
    }

    DeleteAllConnectors();
}

void Component::SetCompName(const char* name)
{
    m_sCompName = name;
}

string Component::GetCompFullName() const
{
    string sFullName = m_sCompName;

    Component* pParent = GetParent();
    while (pParent)
    {
        sFullName = pParent->GetCompName() + "." + sFullName;
        pParent = pParent->GetParent();
    }

    return sFullName;
}

bool Component::IsWorld() const
{
    if (IsAssembly())
    {
        return false;
    }

    return ((DocumentComponent*)m_pDoc)->IsWorld();
}

bool Component::IsDeformable() const
{
    bool bDeformable = false;

    //判断组件
    if (IsAssembly())
    {
        return false;
    }

    //可以增加
    set<string> setDeformabelTypes = {"Modelica.Mechanics.MultiBody.Joints.FreeMotionScalarInit", \
        "Modelica.Mechanics.MultiBody.Forces.ForceAndTorque",\
        "Modelica.Mechanics.MultiBody.Forces.Spring",\
        "Modelica.Mechanics.MultiBody.Forces.Damper",\
        "Modelica.Mechanics.MultiBody.Joints.FreeMotion",\
        "Modelica.Mechanics.MultiBody.Joints.Prismatic",\
        "Modelica.Mechanics.MultiBody.Forces.Force",\
        "Modelica.Mechanics.MultiBody.Forces.Torque",\
        "Modelica.Mechanics.MultiBody.Forces.SpringDamperParallel",\
        "Modelica.Mechanics.MultiBody.Forces.SpringDamperSeries",\
        "Modelica.Mechanics.MultiBody.Forces.LineForceWithMass",\
        "Modelica.Mechanics.MultiBody.Forces.LineForceWithTwoMasses"\
    };
    DocumentComponent* pDocComp = (DocumentComponent*)m_pDoc;
    CyberInfo* pInfo = pDocComp->GetCyberInfo();

    if (pInfo && setDeformabelTypes.find(pInfo->GetMdlName())!= setDeformabelTypes.end())
    {
        bDeformable = true;
    }

    return bDeformable;
}

bool Component::IsConnector() const
{
    bool bConn = false;

    //判断组件
    if (IsAssembly())
    {
        return false;
    }

    DocumentComponent* pDocComp = (DocumentComponent*)m_pDoc;
    CyberInfo* pInfo = pDocComp->GetCyberInfo();
    //string sMdlFullName = pInfo->GetMdlName();
    
    if(pInfo && pInfo->GetMdlType() == "connector") //(sMdlFullName.find("Connector") != string::npos || sMdlFullName.find("connector") != string::npos)
        //测试,以模型名字来；后面需要获得该模型是否为connector类型
    {
        return true;
    }

    return bConn;
}

//得到主连接口
Connector* Component::GetMainConnector() const
{
    Connector* pRetComp = nullptr;

    //如果不是接口类零件，直接返回空
    if (!IsConnector())
    {
        return nullptr;
    }

    //找到零件（是接口）的主接口
    DocumentComponent* pDocComp = (DocumentComponent*)m_pDoc;
    vector<Connector*> vConntors = pDocComp->GetAllConnectors();

    //如果只有一个接口的接口类，那么它的接口就是主接口
    if (vConntors.size() == 1)
    {
        pRetComp = vConntors[0];
    }
    else {
        //名字中含有".MainConnector"
        for (int i = 0; i < vConntors.size(); i++)
        {
            Connector* pConn = vConntors[i];
            string sConnName = pConn->GetConnName();
            if (sConnName == "MainConnector") // .find(".MainConnector") != string::npos)
            {
                pRetComp = pConn;
                //break;
            }
            else {
                delete pConn;
            }
        }
    }

    return pRetComp;
}

QString Component::GetCompFile() const
{
    //string sFileName;

    return m_pDoc->GetFileName();
}

void Component::AddChild(Component* pCom)
{
    m_lstChildren.push_back(pCom);
    pCom->SetParent(this);
}

Connector* Component::GetConnector(const TopoDS_Shape& shp) const
{
    for (int i = 0; i < m_vConnectors.size(); i++)
    {
        Connector* conn = m_vConnectors[i];

        if (shp.IsPartner(conn->GetShape()))
        {
            return conn;
        }
    }

    //vector<Connector*> vConnectors = m_pDoc->GetConnectors();
    //for (int i = 0; i < vConnectors.size(); i++)
    //{
    //    Connector* conn = vConnectors[i];

    //    if (shp.IsPartner(conn->GetShape()))
    //    {
    //        return conn;
    //    }
    //    else {
    //        delete conn;
    //    }
    //}

    return nullptr;
}

//查找子组件
Component* Component::GetChild(const char* sName)
{
    list<Component*>::iterator itr = m_lstChildren.begin();
    for (; itr != m_lstChildren.end(); ++itr)
    {
        Component* pComChild = *itr;
        if (pComChild->GetCompName() == sName)
        {
            return pComChild;
        }
    }

    return nullptr;
}

//删除子组件
void Component::DeleteChild(const char* sChild)
{
    Component* pChild = GetChild(sChild);
    if (pChild == nullptr)
        return;

    list<Component*>::iterator it = find(m_lstChildren.begin(), m_lstChildren.end(), pChild); // 查找list中是否有元素“10”
    if (it != m_lstChildren.end()) // 找到了
    {
        m_lstChildren.erase(it);
    }

    ////从map中删除？？？
    ////m_mapCompInfo.find(sChild);
    //m_mapCompInfo.erase(sChild);

    ////下面删除依赖的连接或装配约束。。。。。。
    //DeleteCompConnectConstraints(sChild);
    //DeleteCompConnectCurves(sChild);
    ////参数
    //DeleteCompParamValues(sChild);

    //最后
    delete pChild;
}


////层次显示组件
//void Component::Display(Handle(AIS_InteractiveContext) theContext, gp_Trsf Trsf)
//{
//    if (m_IsAssembly)
//    {
//        //对每个子部件进行
//        list<Component*>::iterator itr = m_lstChildren.begin();
//        for (; itr != m_lstChildren.end(); ++itr)
//        {
//            Component* pComChild = *itr;
//            vector<double> vDisp = pComChild->GetDisplacement();
//            gp_Trsf trsf_i = MakeTransform(vDisp);
//            pComChild->Display(theContext, Trsf * trsf_i);
//            //部件要形成一个Compound，为了选择
//        }
//    }
//    else //零件
//    {
//        Handle(AIS_InteractiveObject) shpAIS = new AIS_Shape(m_shpCompound);
//        theContext->SetLocation(shpAIS, Trsf);
//        theContext->Display(shpAIS, false);
//        //设置材料显示
//        theContext->SetMaterial(shpAIS, m_material, false);
//        //设置颜色...
//        DisplayAllShapeColors(theContext);
//    }
//}
//
////零件中图元的颜色设置
//void Component::DisplayAllShapeColors(Handle(AIS_InteractiveContext) context)
//{
//    vector<ShapeColor>::iterator itr;
//    for (itr = m_mapShpColor.begin(); itr != m_mapShpColor.end(); ++itr)
//    {
//        //string theName = itr->first; //不需要了
//        ShapeColor sc = *itr;// ->second;
//        TopoDS_Shape theShp = sc.theShape;
//        Quantity_Color theColor = sc.theColor;
//
//        Handle(AIS_InteractiveObject) AIS0 = new AIS_Shape(theShp);
//        context->SetColor(AIS0, theColor, false);
//    }
//}

/*
//vector<ShapeMaterial> shpMats; //形状的材料;
//vector<ShapeTransparency> shpTransparencys; //形状的透明度;
//vector<ShapeColor> shpColors;//形状的颜色；
//vector<TopoDS_Shape> shpDatumPts; //变换后的参考点
//vector<TopoDS_Shape> shpDatumLns; //变换后的参考线
//vector<TopoDS_Shape> shpConnectos; //变换后的接口
//vector<gp_Ax3> coordSys; //变换后的接口坐标系统显示
TopoDS_Shape Component::GenerateCompound1(vector<ShapeMaterial>& shpMats,
    vector<ShapeTransparency>& shpTransparencys, vector<ShapeColor>& shpColors,
    vector<TopoDS_Shape>& shpDatumPts, vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectors,
    vector<gp_Ax3>& coordSys)
{
    TopoDS_Shape shpRet;

    if (m_IsAssembly)
    {
        //对每个子部件进行,组合成一个Compound
        TopoDS_Compound theCompound;
        BRep_Builder aBuilder;
        aBuilder.MakeCompound(theCompound);

        list<Component*>::iterator itr = m_lstChildren.begin();
        for (; itr != m_lstChildren.end(); ++itr)
        {
            Component* pComChild = *itr;
            TopoDS_Shape shpChild = pComChild->GenerateCompound();

            //TopoDS_Shape S = BRepBuilderAPI_MakeWedge(60., 100., 80., 20.);
            gp_Trsf theTransformation = MakeTransform(m_transform);
            //gp_Ax3 ax3_1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
            //gp_Ax3 ax3_2(gp_Pnt(60, 60, 60), gp_Dir(1, 1, 1));
            //theTransformation.SetDisplacement(ax3_1, ax3_2);
            BRepBuilderAPI_Transform myBRepTransformation(shpChild, theTransformation);
            TopoDS_Shape TransformedShape = myBRepTransformation.Shape();

            aBuilder.Add(theCompound, TransformedShape);
        }
        shpRet = theCompound;
    }
    else //零件
    {
        gp_Trsf trans1 = MakeTransform(m_transform);
        BRepBuilderAPI_Transform myBRepTransformation(m_shpCompound, trans1);
        shpRet = myBRepTransformation.Shape();

        //设置材料显示
        ShapeMaterial sm0;
        sm0.theMaterial = m_material;
        sm0.theShape = shpRet;
        shpMats.push_back(sm0);
        //设置透明度...
        //设置颜色...
        for (int i = 0; i < m_mapShpColor.size(); i++)
        {
            //变换
            TopoDS_Shape shc_i = m_mapShpColor[i].theShape;
            ShapeColor sc_i;
            BRepBuilderAPI_Transform myBRepTransformation(shc_i, trans1);
            sc_i.theShape = myBRepTransformation.Shape();
            sc_i.theColor = m_mapShpColor[i].theColor;
            shpColors.push_back(sc_i);
        }
        //基准点
        for (int i = 0; i < m_vDatumPts.size(); i++)
        {
            TopoDS_Shape shDt_i = m_vDatumPts[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, trans1);
            shpDatumPts.push_back(myBRepTransformation.Shape());
        }
        //基准线
        for (int i = 0; i < m_vDatumLns.size(); i++)
        {
            TopoDS_Shape shDt_i = m_vDatumLns[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, trans1);
            shpDatumLns.push_back(myBRepTransformation.Shape());
        }
        //接口
        for (int i = 0; i < m_vConnectors.size(); i++)
        {
            Connector* pConn = m_vConnectors[i];
            //shpConnectors.push_back(pConn->GetShape());
            TopoDS_Shape shDt_i = pConn->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, trans1);
            shpConnectors.push_back(myBRepTransformation.Shape());

            //Ax3
            vector<double> v9 = pConn->GetDisplacement();
            gp_Ax3 ax3 = MakeAx3(v9).Transformed(trans1);
            coordSys.push_back(ax3);
        }
    }

    m_shpCompound = shpRet;

    return shpRet;
}
*/

void Component::DeleteAllConnectors()
{
    for (int i = 0; i < m_vConnectors.size(); i++)
    {
        delete m_vConnectors[i];
    }

    m_vConnectors.clear();
}

//关键函数
TopoDS_Shape Component::GenerateColoredCompound(gp_Trsf theTransf, \
    vector<ShapeTransparency>& shpTransparencys, \
    vector<ShapeColor>& shpColors, vector<ShapeTexture>& shpTextures, vector<TopoDS_Shape>& shpDatumPts, \
    vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectos,\
    vector<gp_Ax3>& coordSys,\
    vector<TopoDS_Shape>& shpMarkSurf)
{
    TopoDS_Shape shpRet;

    if (m_pDoc->IsModel())
    {
        //对每个子部件进行,组合成一个Compound
        TopoDS_Compound theCompound;
        BRep_Builder aBuilder;
        aBuilder.MakeCompound(theCompound);

        list<Component*>::iterator itr = m_lstChildren.begin();
        for (; itr != m_lstChildren.end(); ++itr)
        {
            Component* pComChild = *itr;
            gp_Trsf originTransf = pComChild->GetOriginTransform(); //原始变换
            gp_Trsf delteTransf = pComChild->GetDeltaTransform();   //增量变换：调姿产生的
            //TopoDS_Shape shpChild = pComChild->GenerateCompound(theTransf * originTransf * delteTransf);
            TopoDS_Shape shpChild = pComChild->GenerateColoredCompound(theTransf*delteTransf* originTransf, shpTransparencys,\
                shpColors,shpTextures, shpDatumPts, shpDatumLns, shpConnectos, coordSys, shpMarkSurf);
            aBuilder.Add(theCompound, shpChild);

            //把接口也加到复合体中去
            for (int iCons = 0; iCons < shpConnectos.size(); iCons++)
            {
                aBuilder.Add(theCompound, shpConnectos[iCons]);
            }
        }

        ////部件里的接口，是 Connector类零件的主接口
        //vector<Connector*> vConnectors = ((DocumentModel*)m_pDoc)->GetConnectors();
        //auto itrConn = vConnectors.begin();
        //for (; itrConn != vConnectors.end(); ++itrConn)
        //{
        //    Connector* conn = *itrConn;
        //    TopoDS_Shape theShp = conn->GetShape();
        //    BRepBuilderAPI_Transform myBRepTransformation(theShp, theTransf);
        //    TopoDS_Shape trasShp = myBRepTransformation.Shape();

        //    //aBuilder.Add(theCompound, trasShp);
        //    //加到接口;
        //    shpConnectos.push_back(trasShp);
        //    conn->SetShape(trasShp);
        //    AddConnector(conn);

        //    //Ax3
        //    vector<double> v9 = conn->GetDisplacement();
        //    gp_Ax3 Ax3 = MakeAx3(v9).Transformed(theTransf);
        //    coordSys.push_back(Ax3);
        //}

        //还有连接线
        vector<ConnectCurve> vCurves = ((DocumentModel*)m_pDoc)->GetAllConnectCurves();
        auto itrCC = vCurves.begin();
        for (; itrCC != vCurves.end(); ++itrCC)
        {
            ConnectCurve cc = *itrCC;
            TopoDS_Shape theShp = cc.shape;

            BRepBuilderAPI_Transform myBRepTransformation(theShp, theTransf);
            TopoDS_Shape trasShp = myBRepTransformation.Shape();

            aBuilder.Add(theCompound, trasShp);
            //加到颜色表里吧
            ShapeColor sc;
            sc.theShape = trasShp;
            sc.theColor = Quantity_NOC_BLUE;
            shpColors.push_back(sc);
        }

        shpRet = theCompound;
    }
    else //零件
    {
        DocumentComponent* pDocComp = (DocumentComponent*)GetDocument();
        //TopoDS_Shape shpComp = pDocComp->GetCompCompound();

        //BRepBuilderAPI_Transform myBRepTransformation(shpComp, theTransf);
        //shpRet = myBRepTransformation.Shape();

        //零件的材料：透明度、颜色
       //主实体视见体：材料，包括透明度、颜色等
        Graphic3d_NameOfMaterial theMat;
        TopoDS_Shape theMainShp = pDocComp->GetMainSolid(theMat);
        //矩阵变换
        BRepBuilderAPI_Transform myBRepTransformation(theMainShp, theTransf);
        shpRet = myBRepTransformation.Shape();
        //设置颜色
        Standard_Real C1 = g_vMaterials[theMat].v3MaterialColor[0];
        Standard_Real C2 = g_vMaterials[theMat].v3MaterialColor[1];
        Standard_Real C3 = g_vMaterials[theMat].v3MaterialColor[2];
        Quantity_Color theColor(C1, C2, C3, Quantity_TOC_RGB);
        ShapeColor sc;
        sc.theShape = shpRet;
        sc.theColor = theColor;
        shpColors.push_back(sc);
        //设置透明度
        double aTransparency = g_vMaterials[theMat].fMaterialTransparency;
        ShapeTransparency str;
        str.theShape = shpRet;
        str.transparent = aTransparency;
        shpTransparencys.push_back(str);

        //处理面的颜色...
        map<string, vector<double> > mpColors = pDocComp->GetColorMap();
        map<string, vector<double> >::iterator itre = mpColors.begin();
        for (; itre != mpColors.end(); ++itre)
        {
            string sName = itre->first;
            vector<double> vClr = itre->second;
            //从变换后的主实体获得颜色面
            TopoDS_Shape aShape = pDocComp->GetShapeFromName(sName, shpRet);
            Quantity_Color clr(vClr[0], vClr[1], vClr[2], Quantity_TOC_RGB);
            ShapeColor sc;
            sc.theShape = aShape;
            sc.theColor = clr;
            shpColors.push_back(sc);
        }

        //处理纹理...
        map<string, string > mpTextures = pDocComp->GetTextureMap();
        map<string, string >::iterator itrtext = mpTextures.begin();
        for (; itrtext != mpTextures.end(); ++itrtext)
        {
            string sName = itrtext->first;
            string sFile = itrtext->second;

            //从变换后的主实体获得颜色面
            //TopoDS_Shape aShape = pDocComp->GetShapeFromName(sName, shpRet);
            //ShapeTexture st;
            //st.theShape = aShape;
            //st.sTextFile = sFile;
            //shpTextures.push_back(st);

            // shpRet里面本身就进行了一次转换了
            TopoDS_Shape aShape = pDocComp->GetShapeFromName(sName, shpRet);
            // 如果是标记面，需要进行一次矩阵变换
            if (pDocComp->isMarkingSurf(sName))
            {
                BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransf);
                TopoDS_Shape theTexShp = myBRepTransformation.Shape();
                ShapeTexture st;
                st.theShape = theTexShp;
                st.sTextFile = sFile;
                shpTextures.push_back(st);
            }
            else
            {
                ShapeTexture st;
                st.theShape = aShape;
                st.sTextFile = sFile;
                shpTextures.push_back(st);
            }
        }

        //处理基准点
        vector<DatumPoint*> vDatumPts = pDocComp->GetAllDatumPoints();
        for (int i = 0; i < vDatumPts.size(); i++)
        {
            TopoDS_Shape shDt_i = vDatumPts[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, theTransf);
            TopoDS_Shape theDptShp = myBRepTransformation.Shape();
            shpDatumPts.push_back(theDptShp);
        }

        //基准线
        vector<DatumLine*> vDatumLns = pDocComp->GetAllDatumLines();
        for (int i = 0; i < vDatumLns.size(); i++)
        {
            TopoDS_Shape shDt_i = vDatumLns[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, theTransf);
            TopoDS_Shape theDlnShp = myBRepTransformation.Shape();

            shpDatumLns.push_back(theDlnShp);
        }
        //标记面
        vector<MarkingSurface*> vMarkSurfs = pDocComp->GetAllMarkingSurface();
        for (int i = 0; i < vMarkSurfs.size(); i++)
        {
            TopoDS_Shape shMs_i = vMarkSurfs[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shMs_i, theTransf);
            TopoDS_Shape theMsShp = myBRepTransformation.Shape();

            shpMarkSurf.push_back(theMsShp);
        }

        //接口及其局部坐标系
        vector<Connector*> vConnectors = pDocComp->GetAllConnectors();
        DeleteAllConnectors();
        for (int i = 0; i < vConnectors.size(); i++)
        {
            Connector* pConn = vConnectors[i];
            TopoDS_Shape shDt_i = pConn->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, theTransf);
            TopoDS_Shape theConnShp = myBRepTransformation.Shape();
            shpConnectos.push_back(theConnShp);
            pConn->SetShape(theConnShp);
            AddConnector(pConn);

            //Ax3
            vector<double> v9 = pConn->GetDisplacement();
            gp_Ax3 Ax3 = MakeAx3(v9).Transformed(theTransf);
            coordSys.push_back(Ax3);

            //delete pConn;
        }
    }

    m_shpCompound = shpRet;

    return shpRet;
}

//读文档
//递归建立组件及其子组件的形状：组件(主实体+基准+接口）+ 连接
//不考虑显示的颜色、透明度等
TopoDS_Shape Component::GenerateCompound(gp_Trsf theTransf)
{
    TopoDS_Shape shpRet;

    if (m_pDoc->IsModel())
    {
        //对每个子部件进行,组合成一个Compound
        TopoDS_Compound theCompound;
        BRep_Builder aBuilder;
        aBuilder.MakeCompound(theCompound);

        list<Component*>::iterator itr = m_lstChildren.begin();
        for (; itr != m_lstChildren.end(); ++itr)
        {
            Component* pComChild = *itr;
            gp_Trsf originTransf = pComChild->GetOriginTransform(); //原始变换
            gp_Trsf delteTransf = pComChild->GetDeltaTransform();   //增量变换：调姿产生的
            TopoDS_Shape shpChild = pComChild->GenerateCompound(theTransf* delteTransf* originTransf);

            aBuilder.Add(theCompound, shpChild);
        }
        //还有连接线

        shpRet = theCompound;
    }
    else //零件
    {
        DocumentComponent* pDocComp = (DocumentComponent*)GetDocument();
        TopoDS_Shape shpComp = pDocComp->GetCompCompound();

        BRepBuilderAPI_Transform myBRepTransformation(shpComp, theTransf);
        shpRet = myBRepTransformation.Shape();
    }

    m_shpCompound = shpRet;

    return shpRet;
}

//关键函数
//递归创建各个零件的视见体
//存储在Map中 <string, vector<AIS_Shape>>
//例如 { <a.b.c1, <Ais1,Ais2,...>>, ...}
void Component::GenerateAISObjects(gp_Trsf theTransform, vector<Component*>& vComponents)
{
    if (IsAssembly())
    {
        //对每个子部件进行,递归
        list<Component*>::iterator itr = m_lstChildren.begin();
        for (; itr != m_lstChildren.end(); ++itr)
        {
            Component* pComChild = *itr;
            gp_Trsf chdTransf = pComChild->GetOriginTransform();
            gp_Trsf chdDelTrasf = pComChild->GetDeltaTransform();
            pComChild->GenerateAISObjects(theTransform* chdDelTrasf *chdTransf, vComponents);
        }
        //赞不考虑连接线了！！！
    }
    else //零件
    {
        //记录平坦化零件的Component指针
        vComponents.push_back(this);
        m_vAIS.clear();

        DocumentComponent* pCompDoc = (DocumentComponent*)m_pDoc;

        //主实体视见体：材料，包括透明度、颜色等
        Graphic3d_NameOfMaterial theMat;
        TopoDS_Shape theMainShp = pCompDoc->GetMainSolid(theMat);
        //矩阵变换
        BRepBuilderAPI_Transform myBRepTransformation(theMainShp, theTransform);
        TopoDS_Shape TransformedShape = myBRepTransformation.Shape();
        Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(TransformedShape);
        theAIS->SetMaterial(theMat);
        //设置颜色
        Standard_Real C1 = g_vMaterials[theMat].v3MaterialColor[0];
        Standard_Real C2 = g_vMaterials[theMat].v3MaterialColor[1];
        Standard_Real C3 = g_vMaterials[theMat].v3MaterialColor[2];
        Quantity_Color theColor(C1, C2, C3, Quantity_TOC_RGB);
        theAIS->SetColor(theColor);
        //设置透明度
        double aTransparency = g_vMaterials[theMat].fMaterialTransparency;
        theAIS->SetTransparency(aTransparency);
        //加入视见体集合
        m_vAIS.push_back(theAIS);

        //处理面的颜色...此处需要改进
        map<string, vector<double> > mpColors = pCompDoc->GetColorMap();
        map<string, vector<double> >::iterator itre = mpColors.begin();
        for (; itre != mpColors.end(); ++itre)
        {
            string sName = itre->first;
            vector<double> vClr = itre->second;
            TopoDS_Shape aShape = pCompDoc->GetShapeFromName(sName);
            BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransform);
            TopoDS_Shape theClrShape = myBRepTransformation.Shape();

            Quantity_Color clr(vClr[0], vClr[1], vClr[2], Quantity_TOC_RGB);
            Handle(AIS_Shape) aShapePrs = new AIS_Shape(theClrShape);
            aShapePrs->SetColor(clr);
            m_vAIS.push_back(aShapePrs);
        }

        //处理纹理...
        map<string, string > mpTextures = pCompDoc->GetTextureMap();
        map<string, string >::iterator itrtext = mpTextures.begin();
        for (; itrtext != mpTextures.end(); ++itrtext)
        {
            string sName = itrtext->first;
            string sFilePath = itrtext->second;
            TopoDS_Shape aShape = pCompDoc->GetShapeFromName(sName);
            BRepBuilderAPI_Transform myBRepTransformation(aShape, theTransform);
            TopoDS_Shape theTextShape = myBRepTransformation.Shape();

            const TCollection_AsciiString anUtf8Path(sFilePath.c_str());
            Handle(AIS_TexturedShape) aTFace = Texturize(theTextShape, anUtf8Path, 1, 1, 1, 1, 1, 1);

            m_vAIS.push_back(aTFace);
        }

        ////处理基准点
        //vector<DatumPoint*> vDatumPts = pCompDoc->GetAllDatumPoints();
        //for (int i = 0; i < vDatumPts.size(); i++)
        //{
        //    TopoDS_Shape shDt_i = vDatumPts[i]->GetShape();
        //    BRepBuilderAPI_Transform myBRepTransformation(shDt_i, theTransform);
        //    TopoDS_Shape theDptShp = myBRepTransformation.Shape();
        //    Handle(AIS_Shape) aShapePrs = new AIS_Shape(theDptShp);
        //    aShapePrs->SetColor(Quantity_NOC_YELLOW);
        //    m_vAIS.push_back(aShapePrs);
        //}

        ////基准线
        //vector<DatumLine*> vDatumLns = pCompDoc->GetAllDatumLines();
        //for (int i = 0; i < vDatumLns.size(); i++)
        //{
        //    TopoDS_Shape shDt_i = vDatumLns[i]->GetShape();
        //    BRepBuilderAPI_Transform myBRepTransformation(shDt_i, theTransform);
        //    TopoDS_Shape theDlnShp = myBRepTransformation.Shape();
        //    Handle(AIS_Shape) aShapePrs = new AIS_Shape(theDlnShp);
        //    Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_YELLOW, Aspect_TOL_DOTDASH, 1);
        //    aShapePrs->Attributes()->SetWireAspect(lineAspect);
        //    m_vAIS.push_back(aShapePrs);
        //}

        //接口及其局部坐标系
        vector<Connector*> vConnectors = pCompDoc->GetAllConnectors();
        for (int i = 0; i < vConnectors.size(); i++)
        {
            Connector* pConn = vConnectors[i];
            TopoDS_Shape shDt_i = pConn->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, theTransform);
            TopoDS_Shape theDptShp = myBRepTransformation.Shape();
            Handle(AIS_Shape) aShapePrs = new AIS_Shape(theDptShp);
            aShapePrs->SetColor(Quantity_NOC_YELLOW);
            m_vAIS.push_back(aShapePrs);

            ////Ax3, 隐去
            //vector<double> v9 = pConn->GetDisplacement();
            //gp_Ax3 Ax3 = MakeAx3(v9).Transformed(theTransform);
            //double theLength, arrowLength;
            //m_pDoc->GetAx3Length(theLength, arrowLength);
            //Handle(AdaptorVec_AIS) z_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.Direction(), theLength, arrowLength);
            //z_Axis->SetText("  Z");
            //m_vAIS.push_back(z_Axis);
            //Handle(AdaptorVec_AIS) x_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.XDirection(), theLength, arrowLength);
            //x_Axis->SetText("  X");
            //m_vAIS.push_back(x_Axis);
            //Handle(AdaptorVec_AIS) y_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.YDirection(), theLength, arrowLength);
            //y_Axis->SetText("  Y");
            //m_vAIS.push_back(y_Axis);
        }
    }
}

//对组件进行调姿
void Component::SetDeltaTransform(vector<double>& v15 /*gp_Trsf theTransform*/)
{
    /*m_DeltaTransform = theTransform;*/

    DocumentModel* pMdl = (DocumentModel*)m_parent->GetDocument();

    pMdl->SetCompDeltaTransform(m_sCompName.c_str(), v15);
}

gp_Trsf Component::GetDeltaTransform() const
{
    gp_Trsf theTransf;

    DocumentModel* pMdl = (DocumentModel*)m_parent->GetDocument();

    return pMdl->GetCompDeltaTransform(m_sCompName.c_str());
}

//对组件装配，设置原始姿态
void Component::SetOriginTransform(gp_Trsf theTransform)
{
    DocumentModel* pMdl = (DocumentModel*)m_parent->GetDocument();

    pMdl->SetCompOriginTransform(m_sCompName.c_str(), theTransform);
}


//得到组件原始的变换矩阵，只能从父组件那获得
gp_Trsf Component::GetOriginTransform() const
{
    gp_Trsf theTransf;
    
    DocumentModel* pMdl = (DocumentModel * )m_parent->GetDocument();
    //map<string,CompBaseInfo> mpCompInfo = pMdl->GetAllCompInfos();
    //auto itr = mpCompInfo.find(m_sCompName);
    //assert(itr != mpCompInfo.end());
    //vector<double> v18 = itr->second.v18Transf;
    //theTransf = MakeTransform(v18);
    theTransf = pMdl->GetCompOriginTransform(m_sCompName.c_str());

    return theTransf;
}

//关键函数
//显示组件
void Component::Display(Handle(AIS_InteractiveContext) theContext, bool bRegen)
{
    //先清除
    for (int i = 0; i < m_vAIS.size(); i++)
    {
        theContext->Remove(m_vAIS[i], false);
    }

    vector<Handle(AIS_InteractiveObject)> vAIS;
    //vector<ShapeMaterial> shpMats; //形状的材料;
    vector<ShapeTransparency> shpTransparencys; //形状的透明度;
    vector<ShapeColor> shpColors;//形状的颜色；
    vector<ShapeTexture> shpTextures;//纹理
    vector<TopoDS_Shape> shpDatumPts; //变换后的参考点
    vector<TopoDS_Shape> shpDatumLns; //变换后的参考线
    vector<TopoDS_Shape> shpConnectos; //变换后的接口
    vector<TopoDS_Shape> shpMarkingSurface; //变换后的标记面
    vector<gp_Ax3> coordSys; //变换后的接口坐标系统显示
    gp_Trsf originTransf = GetOriginTransform(); //原始变换
    gp_Trsf delteTransf = GetDeltaTransform();
    gp_Trsf transf = delteTransf* originTransf;
    //TopoDS_Shape comShape = GenerateCompound(transf);
    TopoDS_Shape comShape = GenerateColoredCompound(transf, shpTransparencys, shpColors, shpTextures, shpDatumPts, shpDatumLns, shpConnectos, coordSys, shpMarkingSurface);//GetCompound();

    Handle(AIS_ColoredShape) shpAIS = new AIS_ColoredShape(comShape);
    vAIS.push_back(shpAIS);
    //Handle(AIS_InteractiveObject) shpAIS0 = new AIS_Shape(comShape);
    //myDisplayList.insert(make_pair(pCom->GetCompName(), shpAIS));

    ////显示材料;
    //for (int i = 0; i < shpMats.size(); i++)
    //{
    //    TopoDS_Shape theShp = shpMats[i].theShape;
    //    Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(theShp);
    //    myContext->Display(theAIS, false);
    //    myContext->SetMaterial(theAIS, shpMats[i].theMaterial, false);
    //}
    //显示透明度
    for (int i = 0; i < shpTransparencys.size(); i++)
    {
        TopoDS_Shape theShp_i = shpTransparencys[i].theShape;
        shpAIS->SetCustomTransparency(theShp_i, shpTransparencys[i].transparent);
    }
    //显示颜色；
    for (int i = 0; i < shpColors.size(); i++)
    {
        TopoDS_Shape theShp_i = shpColors[i].theShape;
        shpAIS->SetCustomColor(theShp_i, shpColors[i].theColor);
    }
    //显示纹理
    for (int i = 0; i < shpTextures.size(); i++)
    {
        TopoDS_Shape theShp_i = shpTextures[i].theShape;
        string sFilePath = shpTextures[i].sTextFile;
        const TCollection_AsciiString anUtf8Path(sFilePath.c_str());
        Handle(AIS_TexturedShape) aTFace = Texturize(theShp_i, anUtf8Path,1,1,1,1,1,1);
        vAIS.push_back(aTFace);
    }
    //显示参考点
    for (int i = 0; i < shpDatumPts.size(); i++)
    {
        TopoDS_Shape theShp_i = shpDatumPts[i];
        shpAIS->SetCustomColor(theShp_i, Quantity_NOC_YELLOW);
    }
    //显示参考线
    for (int i = 0; i < shpDatumLns.size(); i++)
    {
        TopoDS_Shape theShp = shpDatumLns[i];
        Handle(AIS_Shape) datLine = new AIS_Shape(theShp);
        Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_YELLOW, Aspect_TOL_DOTDASH, 1);
        datLine->Attributes()->SetWireAspect(lineAspect);
        Standard_Integer theDispMode = theContext->DisplayMode();
        //显示，但不允许选择
        //theContext->Display(datLine, theDispMode, -1, Standard_False);
        vAIS.push_back(datLine);
    }
    ////显示标记面
    //for (int i = 0; i < shpMarkingSurface.size(); i++)
    //{
    //    TopoDS_Shape theShp = shpMarkingSurface[i];
    //    Handle(AIS_Shape) MarkSurf = new AIS_Shape(theShp);
    //    vAIS.push_back(MarkSurf);
    //}

    //获得当前Component的主接口shape集合
    vector<TopoDS_Shape> mainConnectorShape;
    auto itrCom = m_lstChildren.begin();
    for (; itrCom != m_lstChildren.end(); ++itrCom)
    {
        Component* pCom = *itrCom;
        Connector * pMain = pCom->GetMainConnector();
        if (pMain)
        {
            mainConnectorShape.push_back(pMain->GetShape());
        }
        delete pMain;
    }

    if (m_lstChildren.size() == 0)
    {
        //零件时,     //显示本零件体的接口
        DocumentModel* pMdl = (DocumentModel*)m_parent->GetDocument();
        vector<ConnectConstraint> vecCC = pMdl->GetAllConnConstraints();
        for (int i = 0; i < m_vConnectors.size(); i++)
        {
            //TopoDS_Shape theShp = shpConnectos[i];
            Connector* pConn = m_vConnectors[i]; // GetConnector(theShp);
            TopoDS_Shape theShp = pConn->GetShape();
            //加个比例缩放
            //gp_Ax3 Ax3 = coordSys[i];
            ////gp_Ax3 Ax3 = MakeAx3(pConn->GetDisplacement());//用这个接口位置不对，不知道为什么
            //gp_Trsf transf;
            //double scale = LibPathsSetDlg::GetConnSize();
            //transf.SetScale(Ax3.Location(), scale);
            //BRepBuilderAPI_Transform myBRepTransformation(theShp, transf);
            //TopoDS_Shape trsfshp = myBRepTransformation.Shape();
            Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(theShp/*trsfshp*/);
            if (pConn)
            {//第一层的接口，显示并允许选择
                //theContext->Display(shpConn, Standard_False);
                string Com_Conn = GetCompName();
                Com_Conn += "." + pConn->GetConnName();
                //myDisplayList.insert(make_pair(Com_Conn, shpAIS));
            //pConn->SetShape(trsfshp);
            //if (!m_pDoc->IsModel())
            //{
            //    DocumentComponent* pDocComp = (DocumentComponent*)m_pDoc;
            //    pDocComp->UpdateConnector(theShp, pConn);
            //}
             //显示装配约束的接口
                bool bFind = false;
                for (auto cc : vecCC)
                {
                    string sCom1Name = cc.component1;
                    string sCom2Name = cc.component2;
                    string sConn1Name = cc.connector1;
                    string sConn2Name = cc.connector2;
                    //Connector* pConn = m_pCompRoot->GetChild(sCom1Name.c_str())->GetConnector(sConn1Name.c_str())
                    if ((sCom1Name == m_sCompName && sConn1Name == pConn->GetConnName()) || \
                        (sCom2Name == m_sCompName && sConn2Name == pConn->GetConnName()))
                    {
                        bFind = true;
                        break;
                    }
                }
                if (bFind)
                {
                    theContext->SetColor(shpConn, Quantity_NOC_RED, Standard_False);
                }
                else
                {
                    theContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
                }

                //delete pConn;
            }
            else
            {//其余只能显示
                theContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
                ;// theContext->Display(shpConn, theContext->DisplayMode(), -1, Standard_False);
            }
            vAIS.push_back(shpConn);
        }
    }
    //显示各个零件的接口,暂时都用黄色表示
    //DocumentModel* pMdl = (DocumentModel*)m_parent->GetDocument();
    //vector<ConnectConstraint> vecCC = pMdl->GetAllConnConstraints();
    else
    {
        for (int i = 0; i < shpConnectos.size(); i++)
        {
            TopoDS_Shape theShp = shpConnectos[i];
            //Connector* pConn = GetConnector(theShp);

            //如果是主接口
            bool bMain = false;
            for (int iConn = 0; iConn < mainConnectorShape.size(); iConn++)
            {
                if (theShp.IsPartner(mainConnectorShape[iConn]))
                {
                    bMain = true;
                    break;
                }
            }
            if (bMain) //find(mainConnectorShape.begin(),mainConnectorShape.end(), theShp) != mainConnectorShape.end())
            {
                Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(theShp);
                theContext->SetColor(shpConn, Quantity_NOC_GREEN, Standard_False);
                vAIS.push_back(shpConn);
            }
            else //if (pConn)
            {
                //    string Com_Conn = GetCompName();
                //    Com_Conn += "." + pConn->GetConnName();

                // //显示装配约束的接口
                //    bool bFind = false;
                //    for (auto cc : vecCC)
                //    {
                //        string sCom1Name = cc.component1;
                //        string sCom2Name = cc.component2;
                //        string sConn1Name = cc.connector1;
                //        string sConn2Name = cc.connector2;
                //        //Connector* pConn = m_pCompRoot->GetChild(sCom1Name.c_str())->GetConnector(sConn1Name.c_str())
                //        if ((sCom1Name == m_sCompName && sConn1Name == pConn->GetConnName()) || \
                //            (sCom2Name == m_sCompName && sConn2Name == pConn->GetConnName()))
                //        {
                //            bFind = true;
                //            break;
                //        }
                //    }
                //    if (bFind)
                //    {
                //        //theContext->SetColor(shpConn, Quantity_NOC_RED, Standard_False);
                //        shpAIS->SetCustomColor(theShp, Quantity_NOC_RED);
                //    }
                //    else
                //    {
                //        //theContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
                //        shpAIS->SetCustomColor(theShp, Quantity_NOC_YELLOW);
                //    }
                //    //delete pConn;
                //}
                //else
                //{
                    //theContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
                shpAIS->SetCustomColor(theShp, Quantity_NOC_YELLOW);
            }
        }//for
    }

    //显示零件的接口坐标系统
    for (int i = 0; i < coordSys.size(); i++)
    {
        gp_Ax3 Ax3 = coordSys[i];

        double theLength, arrowLength;
        m_pDoc->GetAx3Length(theLength, arrowLength);
        Handle(AdaptorVec_AIS) z_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.Direction(), theLength, arrowLength);
        z_Axis->SetText("  Z");
        //theContext->Display(z_Axis, Standard_True);
        vAIS.push_back(z_Axis);
        Handle(AdaptorVec_AIS) x_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.XDirection(), theLength, arrowLength);
        x_Axis->SetText("  X");
        //theContext->Display(x_Axis, Standard_False);
        vAIS.push_back(x_Axis);
        Handle(AdaptorVec_AIS) y_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.YDirection(), theLength, arrowLength);
        y_Axis->SetText("  Y");
        //theContext->Display(y_Axis, Standard_False);
        vAIS.push_back(y_Axis);
    }

 

    if (bRegen)
    {
        for (int i = 0; i < vAIS.size(); i++)
        {
            //theContext->SetLocation(vAIS[i], delteTransf);
            theContext->Display(vAIS[i], Standard_True);
        }
    }

    SetAIS(vAIS);
}

////显示组件，包括子组件: 做相对变换
//void Component::Display(Handle(AIS_InteractiveContext) theContext, gp_Trsf delTrans)
//{
//    if (IsAssembly())
//    {
//        auto itr = m_lstChildren.begin();
//        for (; itr != m_lstChildren.end(); ++itr)
//        {
//            Component* pChild = *itr;
//            gp_Trsf chdTransf = pChild->GetDeltaTransform();
//            pChild->Display(theContext, delTrans*chdTransf);
//        }
//    }
//    else {
//        //显示零件
//        for (NCollection_Vector<Handle(AIS_InteractiveObject)>::Iterator anIter(m_theAISObjects);
//            anIter.More(); anIter.Next())
//        {
//            const Handle(AIS_InteractiveObject)& anObject = anIter.Value();
//            theContext->SetLocation(anObject, delTrans);
//            theContext->Display(anObject, Standard_True);
//        }
//    }
//}

bool Component::IsAssembly() const
{
    return m_pDoc->IsModel();
}

void Component::GetAllDescendants(vector<Component*>& vComps)
{
    auto itr = m_lstChildren.begin();
    for (; itr != m_lstChildren.end(); ++itr)
    {
        Component* pCom = *itr;
        if (pCom->IsAssembly())
        {
            pCom->GetAllDescendants(vComps);
        }
        else {
            vComps.push_back(pCom);
        }
    }
}

//由组件获得CyberInfo,必须是零件；
CyberInfo* Component::GetCyberInfo() const
{
    if (IsAssembly())
    {
        return nullptr;
    }

    DocumentComponent* pDoc = (DocumentComponent * )m_pDoc;
    return pDoc->GetCyberInfo();
}
