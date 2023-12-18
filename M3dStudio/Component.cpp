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
    iVariablity = 0; //0-ȱʡ��1-Constant; 2-parameter; 3-Discrete
    //Casuality
    iCasuality = 0; //0-None; 1-Input; 2-Output
    //ConnectorMember
    iConnectorMember = 0;//0-����; 1-flow; 2-stream
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
//Component����
///
Component::Component(DocumentCommon* pDoc, Component* parent)
{
    m_pDoc = pDoc;
    m_parent = parent;

    ////ȱʡλ��:FromSystem��ToSystem
    //vector<double> v0 = { 0,0,0,0,0,1,1,0,0,  0,0,0,0,0,1,1,0,0 };
    //m_transform = v0;
}

Component::~Component()
{
    //ɾ����������Զ��ݹ�
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

    //�ж����
    if (IsAssembly())
    {
        return false;
    }

    //��������
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

    //�ж����
    if (IsAssembly())
    {
        return false;
    }

    DocumentComponent* pDocComp = (DocumentComponent*)m_pDoc;
    CyberInfo* pInfo = pDocComp->GetCyberInfo();
    //string sMdlFullName = pInfo->GetMdlName();
    
    if(pInfo && pInfo->GetMdlType() == "connector") //(sMdlFullName.find("Connector") != string::npos || sMdlFullName.find("connector") != string::npos)
        //����,��ģ����������������Ҫ��ø�ģ���Ƿ�Ϊconnector����
    {
        return true;
    }

    return bConn;
}

//�õ������ӿ�
Connector* Component::GetMainConnector() const
{
    Connector* pRetComp = nullptr;

    //������ǽӿ��������ֱ�ӷ��ؿ�
    if (!IsConnector())
    {
        return nullptr;
    }

    //�ҵ�������ǽӿڣ������ӿ�
    DocumentComponent* pDocComp = (DocumentComponent*)m_pDoc;
    vector<Connector*> vConntors = pDocComp->GetAllConnectors();

    //���ֻ��һ���ӿڵĽӿ��࣬��ô���Ľӿھ������ӿ�
    if (vConntors.size() == 1)
    {
        pRetComp = vConntors[0];
    }
    else {
        //�����к���".MainConnector"
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

//���������
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

//ɾ�������
void Component::DeleteChild(const char* sChild)
{
    Component* pChild = GetChild(sChild);
    if (pChild == nullptr)
        return;

    list<Component*>::iterator it = find(m_lstChildren.begin(), m_lstChildren.end(), pChild); // ����list���Ƿ���Ԫ�ء�10��
    if (it != m_lstChildren.end()) // �ҵ���
    {
        m_lstChildren.erase(it);
    }

    ////��map��ɾ��������
    ////m_mapCompInfo.find(sChild);
    //m_mapCompInfo.erase(sChild);

    ////����ɾ�����������ӻ�װ��Լ��������������
    //DeleteCompConnectConstraints(sChild);
    //DeleteCompConnectCurves(sChild);
    ////����
    //DeleteCompParamValues(sChild);

    //���
    delete pChild;
}


////�����ʾ���
//void Component::Display(Handle(AIS_InteractiveContext) theContext, gp_Trsf Trsf)
//{
//    if (m_IsAssembly)
//    {
//        //��ÿ���Ӳ�������
//        list<Component*>::iterator itr = m_lstChildren.begin();
//        for (; itr != m_lstChildren.end(); ++itr)
//        {
//            Component* pComChild = *itr;
//            vector<double> vDisp = pComChild->GetDisplacement();
//            gp_Trsf trsf_i = MakeTransform(vDisp);
//            pComChild->Display(theContext, Trsf * trsf_i);
//            //����Ҫ�γ�һ��Compound��Ϊ��ѡ��
//        }
//    }
//    else //���
//    {
//        Handle(AIS_InteractiveObject) shpAIS = new AIS_Shape(m_shpCompound);
//        theContext->SetLocation(shpAIS, Trsf);
//        theContext->Display(shpAIS, false);
//        //���ò�����ʾ
//        theContext->SetMaterial(shpAIS, m_material, false);
//        //������ɫ...
//        DisplayAllShapeColors(theContext);
//    }
//}
//
////�����ͼԪ����ɫ����
//void Component::DisplayAllShapeColors(Handle(AIS_InteractiveContext) context)
//{
//    vector<ShapeColor>::iterator itr;
//    for (itr = m_mapShpColor.begin(); itr != m_mapShpColor.end(); ++itr)
//    {
//        //string theName = itr->first; //����Ҫ��
//        ShapeColor sc = *itr;// ->second;
//        TopoDS_Shape theShp = sc.theShape;
//        Quantity_Color theColor = sc.theColor;
//
//        Handle(AIS_InteractiveObject) AIS0 = new AIS_Shape(theShp);
//        context->SetColor(AIS0, theColor, false);
//    }
//}

/*
//vector<ShapeMaterial> shpMats; //��״�Ĳ���;
//vector<ShapeTransparency> shpTransparencys; //��״��͸����;
//vector<ShapeColor> shpColors;//��״����ɫ��
//vector<TopoDS_Shape> shpDatumPts; //�任��Ĳο���
//vector<TopoDS_Shape> shpDatumLns; //�任��Ĳο���
//vector<TopoDS_Shape> shpConnectos; //�任��Ľӿ�
//vector<gp_Ax3> coordSys; //�任��Ľӿ�����ϵͳ��ʾ
TopoDS_Shape Component::GenerateCompound1(vector<ShapeMaterial>& shpMats,
    vector<ShapeTransparency>& shpTransparencys, vector<ShapeColor>& shpColors,
    vector<TopoDS_Shape>& shpDatumPts, vector<TopoDS_Shape>& shpDatumLns, vector<TopoDS_Shape>& shpConnectors,
    vector<gp_Ax3>& coordSys)
{
    TopoDS_Shape shpRet;

    if (m_IsAssembly)
    {
        //��ÿ���Ӳ�������,��ϳ�һ��Compound
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
    else //���
    {
        gp_Trsf trans1 = MakeTransform(m_transform);
        BRepBuilderAPI_Transform myBRepTransformation(m_shpCompound, trans1);
        shpRet = myBRepTransformation.Shape();

        //���ò�����ʾ
        ShapeMaterial sm0;
        sm0.theMaterial = m_material;
        sm0.theShape = shpRet;
        shpMats.push_back(sm0);
        //����͸����...
        //������ɫ...
        for (int i = 0; i < m_mapShpColor.size(); i++)
        {
            //�任
            TopoDS_Shape shc_i = m_mapShpColor[i].theShape;
            ShapeColor sc_i;
            BRepBuilderAPI_Transform myBRepTransformation(shc_i, trans1);
            sc_i.theShape = myBRepTransformation.Shape();
            sc_i.theColor = m_mapShpColor[i].theColor;
            shpColors.push_back(sc_i);
        }
        //��׼��
        for (int i = 0; i < m_vDatumPts.size(); i++)
        {
            TopoDS_Shape shDt_i = m_vDatumPts[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, trans1);
            shpDatumPts.push_back(myBRepTransformation.Shape());
        }
        //��׼��
        for (int i = 0; i < m_vDatumLns.size(); i++)
        {
            TopoDS_Shape shDt_i = m_vDatumLns[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, trans1);
            shpDatumLns.push_back(myBRepTransformation.Shape());
        }
        //�ӿ�
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

//�ؼ�����
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
        //��ÿ���Ӳ�������,��ϳ�һ��Compound
        TopoDS_Compound theCompound;
        BRep_Builder aBuilder;
        aBuilder.MakeCompound(theCompound);

        list<Component*>::iterator itr = m_lstChildren.begin();
        for (; itr != m_lstChildren.end(); ++itr)
        {
            Component* pComChild = *itr;
            gp_Trsf originTransf = pComChild->GetOriginTransform(); //ԭʼ�任
            gp_Trsf delteTransf = pComChild->GetDeltaTransform();   //�����任�����˲�����
            //TopoDS_Shape shpChild = pComChild->GenerateCompound(theTransf * originTransf * delteTransf);
            TopoDS_Shape shpChild = pComChild->GenerateColoredCompound(theTransf*delteTransf* originTransf, shpTransparencys,\
                shpColors,shpTextures, shpDatumPts, shpDatumLns, shpConnectos, coordSys, shpMarkSurf);
            aBuilder.Add(theCompound, shpChild);

            //�ѽӿ�Ҳ�ӵ���������ȥ
            for (int iCons = 0; iCons < shpConnectos.size(); iCons++)
            {
                aBuilder.Add(theCompound, shpConnectos[iCons]);
            }
        }

        ////������Ľӿڣ��� Connector����������ӿ�
        //vector<Connector*> vConnectors = ((DocumentModel*)m_pDoc)->GetConnectors();
        //auto itrConn = vConnectors.begin();
        //for (; itrConn != vConnectors.end(); ++itrConn)
        //{
        //    Connector* conn = *itrConn;
        //    TopoDS_Shape theShp = conn->GetShape();
        //    BRepBuilderAPI_Transform myBRepTransformation(theShp, theTransf);
        //    TopoDS_Shape trasShp = myBRepTransformation.Shape();

        //    //aBuilder.Add(theCompound, trasShp);
        //    //�ӵ��ӿ�;
        //    shpConnectos.push_back(trasShp);
        //    conn->SetShape(trasShp);
        //    AddConnector(conn);

        //    //Ax3
        //    vector<double> v9 = conn->GetDisplacement();
        //    gp_Ax3 Ax3 = MakeAx3(v9).Transformed(theTransf);
        //    coordSys.push_back(Ax3);
        //}

        //����������
        vector<ConnectCurve> vCurves = ((DocumentModel*)m_pDoc)->GetAllConnectCurves();
        auto itrCC = vCurves.begin();
        for (; itrCC != vCurves.end(); ++itrCC)
        {
            ConnectCurve cc = *itrCC;
            TopoDS_Shape theShp = cc.shape;

            BRepBuilderAPI_Transform myBRepTransformation(theShp, theTransf);
            TopoDS_Shape trasShp = myBRepTransformation.Shape();

            aBuilder.Add(theCompound, trasShp);
            //�ӵ���ɫ�����
            ShapeColor sc;
            sc.theShape = trasShp;
            sc.theColor = Quantity_NOC_BLUE;
            shpColors.push_back(sc);
        }

        shpRet = theCompound;
    }
    else //���
    {
        DocumentComponent* pDocComp = (DocumentComponent*)GetDocument();
        //TopoDS_Shape shpComp = pDocComp->GetCompCompound();

        //BRepBuilderAPI_Transform myBRepTransformation(shpComp, theTransf);
        //shpRet = myBRepTransformation.Shape();

        //����Ĳ��ϣ�͸���ȡ���ɫ
       //��ʵ���Ӽ��壺���ϣ�����͸���ȡ���ɫ��
        Graphic3d_NameOfMaterial theMat;
        TopoDS_Shape theMainShp = pDocComp->GetMainSolid(theMat);
        //����任
        BRepBuilderAPI_Transform myBRepTransformation(theMainShp, theTransf);
        shpRet = myBRepTransformation.Shape();
        //������ɫ
        Standard_Real C1 = g_vMaterials[theMat].v3MaterialColor[0];
        Standard_Real C2 = g_vMaterials[theMat].v3MaterialColor[1];
        Standard_Real C3 = g_vMaterials[theMat].v3MaterialColor[2];
        Quantity_Color theColor(C1, C2, C3, Quantity_TOC_RGB);
        ShapeColor sc;
        sc.theShape = shpRet;
        sc.theColor = theColor;
        shpColors.push_back(sc);
        //����͸����
        double aTransparency = g_vMaterials[theMat].fMaterialTransparency;
        ShapeTransparency str;
        str.theShape = shpRet;
        str.transparent = aTransparency;
        shpTransparencys.push_back(str);

        //���������ɫ...
        map<string, vector<double> > mpColors = pDocComp->GetColorMap();
        map<string, vector<double> >::iterator itre = mpColors.begin();
        for (; itre != mpColors.end(); ++itre)
        {
            string sName = itre->first;
            vector<double> vClr = itre->second;
            //�ӱ任�����ʵ������ɫ��
            TopoDS_Shape aShape = pDocComp->GetShapeFromName(sName, shpRet);
            Quantity_Color clr(vClr[0], vClr[1], vClr[2], Quantity_TOC_RGB);
            ShapeColor sc;
            sc.theShape = aShape;
            sc.theColor = clr;
            shpColors.push_back(sc);
        }

        //��������...
        map<string, string > mpTextures = pDocComp->GetTextureMap();
        map<string, string >::iterator itrtext = mpTextures.begin();
        for (; itrtext != mpTextures.end(); ++itrtext)
        {
            string sName = itrtext->first;
            string sFile = itrtext->second;

            //�ӱ任�����ʵ������ɫ��
            //TopoDS_Shape aShape = pDocComp->GetShapeFromName(sName, shpRet);
            //ShapeTexture st;
            //st.theShape = aShape;
            //st.sTextFile = sFile;
            //shpTextures.push_back(st);

            // shpRet���汾��ͽ�����һ��ת����
            TopoDS_Shape aShape = pDocComp->GetShapeFromName(sName, shpRet);
            // ����Ǳ���棬��Ҫ����һ�ξ���任
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

        //�����׼��
        vector<DatumPoint*> vDatumPts = pDocComp->GetAllDatumPoints();
        for (int i = 0; i < vDatumPts.size(); i++)
        {
            TopoDS_Shape shDt_i = vDatumPts[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, theTransf);
            TopoDS_Shape theDptShp = myBRepTransformation.Shape();
            shpDatumPts.push_back(theDptShp);
        }

        //��׼��
        vector<DatumLine*> vDatumLns = pDocComp->GetAllDatumLines();
        for (int i = 0; i < vDatumLns.size(); i++)
        {
            TopoDS_Shape shDt_i = vDatumLns[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shDt_i, theTransf);
            TopoDS_Shape theDlnShp = myBRepTransformation.Shape();

            shpDatumLns.push_back(theDlnShp);
        }
        //�����
        vector<MarkingSurface*> vMarkSurfs = pDocComp->GetAllMarkingSurface();
        for (int i = 0; i < vMarkSurfs.size(); i++)
        {
            TopoDS_Shape shMs_i = vMarkSurfs[i]->GetShape();
            BRepBuilderAPI_Transform myBRepTransformation(shMs_i, theTransf);
            TopoDS_Shape theMsShp = myBRepTransformation.Shape();

            shpMarkSurf.push_back(theMsShp);
        }

        //�ӿڼ���ֲ�����ϵ
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

//���ĵ�
//�ݹ齨������������������״�����(��ʵ��+��׼+�ӿڣ�+ ����
//��������ʾ����ɫ��͸���ȵ�
TopoDS_Shape Component::GenerateCompound(gp_Trsf theTransf)
{
    TopoDS_Shape shpRet;

    if (m_pDoc->IsModel())
    {
        //��ÿ���Ӳ�������,��ϳ�һ��Compound
        TopoDS_Compound theCompound;
        BRep_Builder aBuilder;
        aBuilder.MakeCompound(theCompound);

        list<Component*>::iterator itr = m_lstChildren.begin();
        for (; itr != m_lstChildren.end(); ++itr)
        {
            Component* pComChild = *itr;
            gp_Trsf originTransf = pComChild->GetOriginTransform(); //ԭʼ�任
            gp_Trsf delteTransf = pComChild->GetDeltaTransform();   //�����任�����˲�����
            TopoDS_Shape shpChild = pComChild->GenerateCompound(theTransf* delteTransf* originTransf);

            aBuilder.Add(theCompound, shpChild);
        }
        //����������

        shpRet = theCompound;
    }
    else //���
    {
        DocumentComponent* pDocComp = (DocumentComponent*)GetDocument();
        TopoDS_Shape shpComp = pDocComp->GetCompCompound();

        BRepBuilderAPI_Transform myBRepTransformation(shpComp, theTransf);
        shpRet = myBRepTransformation.Shape();
    }

    m_shpCompound = shpRet;

    return shpRet;
}

//�ؼ�����
//�ݹ鴴������������Ӽ���
//�洢��Map�� <string, vector<AIS_Shape>>
//���� { <a.b.c1, <Ais1,Ais2,...>>, ...}
void Component::GenerateAISObjects(gp_Trsf theTransform, vector<Component*>& vComponents)
{
    if (IsAssembly())
    {
        //��ÿ���Ӳ�������,�ݹ�
        list<Component*>::iterator itr = m_lstChildren.begin();
        for (; itr != m_lstChildren.end(); ++itr)
        {
            Component* pComChild = *itr;
            gp_Trsf chdTransf = pComChild->GetOriginTransform();
            gp_Trsf chdDelTrasf = pComChild->GetDeltaTransform();
            pComChild->GenerateAISObjects(theTransform* chdDelTrasf *chdTransf, vComponents);
        }
        //�޲������������ˣ�����
    }
    else //���
    {
        //��¼ƽ̹�������Componentָ��
        vComponents.push_back(this);
        m_vAIS.clear();

        DocumentComponent* pCompDoc = (DocumentComponent*)m_pDoc;

        //��ʵ���Ӽ��壺���ϣ�����͸���ȡ���ɫ��
        Graphic3d_NameOfMaterial theMat;
        TopoDS_Shape theMainShp = pCompDoc->GetMainSolid(theMat);
        //����任
        BRepBuilderAPI_Transform myBRepTransformation(theMainShp, theTransform);
        TopoDS_Shape TransformedShape = myBRepTransformation.Shape();
        Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(TransformedShape);
        theAIS->SetMaterial(theMat);
        //������ɫ
        Standard_Real C1 = g_vMaterials[theMat].v3MaterialColor[0];
        Standard_Real C2 = g_vMaterials[theMat].v3MaterialColor[1];
        Standard_Real C3 = g_vMaterials[theMat].v3MaterialColor[2];
        Quantity_Color theColor(C1, C2, C3, Quantity_TOC_RGB);
        theAIS->SetColor(theColor);
        //����͸����
        double aTransparency = g_vMaterials[theMat].fMaterialTransparency;
        theAIS->SetTransparency(aTransparency);
        //�����Ӽ��弯��
        m_vAIS.push_back(theAIS);

        //���������ɫ...�˴���Ҫ�Ľ�
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

        //��������...
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

        ////�����׼��
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

        ////��׼��
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

        //�ӿڼ���ֲ�����ϵ
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

            ////Ax3, ��ȥ
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

//��������е���
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

//�����װ�䣬����ԭʼ��̬
void Component::SetOriginTransform(gp_Trsf theTransform)
{
    DocumentModel* pMdl = (DocumentModel*)m_parent->GetDocument();

    pMdl->SetCompOriginTransform(m_sCompName.c_str(), theTransform);
}


//�õ����ԭʼ�ı任����ֻ�ܴӸ�����ǻ��
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

//�ؼ�����
//��ʾ���
void Component::Display(Handle(AIS_InteractiveContext) theContext, bool bRegen)
{
    //�����
    for (int i = 0; i < m_vAIS.size(); i++)
    {
        theContext->Remove(m_vAIS[i], false);
    }

    vector<Handle(AIS_InteractiveObject)> vAIS;
    //vector<ShapeMaterial> shpMats; //��״�Ĳ���;
    vector<ShapeTransparency> shpTransparencys; //��״��͸����;
    vector<ShapeColor> shpColors;//��״����ɫ��
    vector<ShapeTexture> shpTextures;//����
    vector<TopoDS_Shape> shpDatumPts; //�任��Ĳο���
    vector<TopoDS_Shape> shpDatumLns; //�任��Ĳο���
    vector<TopoDS_Shape> shpConnectos; //�任��Ľӿ�
    vector<TopoDS_Shape> shpMarkingSurface; //�任��ı����
    vector<gp_Ax3> coordSys; //�任��Ľӿ�����ϵͳ��ʾ
    gp_Trsf originTransf = GetOriginTransform(); //ԭʼ�任
    gp_Trsf delteTransf = GetDeltaTransform();
    gp_Trsf transf = delteTransf* originTransf;
    //TopoDS_Shape comShape = GenerateCompound(transf);
    TopoDS_Shape comShape = GenerateColoredCompound(transf, shpTransparencys, shpColors, shpTextures, shpDatumPts, shpDatumLns, shpConnectos, coordSys, shpMarkingSurface);//GetCompound();

    Handle(AIS_ColoredShape) shpAIS = new AIS_ColoredShape(comShape);
    vAIS.push_back(shpAIS);
    //Handle(AIS_InteractiveObject) shpAIS0 = new AIS_Shape(comShape);
    //myDisplayList.insert(make_pair(pCom->GetCompName(), shpAIS));

    ////��ʾ����;
    //for (int i = 0; i < shpMats.size(); i++)
    //{
    //    TopoDS_Shape theShp = shpMats[i].theShape;
    //    Handle(AIS_InteractiveObject) theAIS = new AIS_Shape(theShp);
    //    myContext->Display(theAIS, false);
    //    myContext->SetMaterial(theAIS, shpMats[i].theMaterial, false);
    //}
    //��ʾ͸����
    for (int i = 0; i < shpTransparencys.size(); i++)
    {
        TopoDS_Shape theShp_i = shpTransparencys[i].theShape;
        shpAIS->SetCustomTransparency(theShp_i, shpTransparencys[i].transparent);
    }
    //��ʾ��ɫ��
    for (int i = 0; i < shpColors.size(); i++)
    {
        TopoDS_Shape theShp_i = shpColors[i].theShape;
        shpAIS->SetCustomColor(theShp_i, shpColors[i].theColor);
    }
    //��ʾ����
    for (int i = 0; i < shpTextures.size(); i++)
    {
        TopoDS_Shape theShp_i = shpTextures[i].theShape;
        string sFilePath = shpTextures[i].sTextFile;
        const TCollection_AsciiString anUtf8Path(sFilePath.c_str());
        Handle(AIS_TexturedShape) aTFace = Texturize(theShp_i, anUtf8Path,1,1,1,1,1,1);
        vAIS.push_back(aTFace);
    }
    //��ʾ�ο���
    for (int i = 0; i < shpDatumPts.size(); i++)
    {
        TopoDS_Shape theShp_i = shpDatumPts[i];
        shpAIS->SetCustomColor(theShp_i, Quantity_NOC_YELLOW);
    }
    //��ʾ�ο���
    for (int i = 0; i < shpDatumLns.size(); i++)
    {
        TopoDS_Shape theShp = shpDatumLns[i];
        Handle(AIS_Shape) datLine = new AIS_Shape(theShp);
        Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_YELLOW, Aspect_TOL_DOTDASH, 1);
        datLine->Attributes()->SetWireAspect(lineAspect);
        Standard_Integer theDispMode = theContext->DisplayMode();
        //��ʾ����������ѡ��
        //theContext->Display(datLine, theDispMode, -1, Standard_False);
        vAIS.push_back(datLine);
    }
    ////��ʾ�����
    //for (int i = 0; i < shpMarkingSurface.size(); i++)
    //{
    //    TopoDS_Shape theShp = shpMarkingSurface[i];
    //    Handle(AIS_Shape) MarkSurf = new AIS_Shape(theShp);
    //    vAIS.push_back(MarkSurf);
    //}

    //��õ�ǰComponent�����ӿ�shape����
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
        //���ʱ,     //��ʾ�������Ľӿ�
        DocumentModel* pMdl = (DocumentModel*)m_parent->GetDocument();
        vector<ConnectConstraint> vecCC = pMdl->GetAllConnConstraints();
        for (int i = 0; i < m_vConnectors.size(); i++)
        {
            //TopoDS_Shape theShp = shpConnectos[i];
            Connector* pConn = m_vConnectors[i]; // GetConnector(theShp);
            TopoDS_Shape theShp = pConn->GetShape();
            //�Ӹ���������
            //gp_Ax3 Ax3 = coordSys[i];
            ////gp_Ax3 Ax3 = MakeAx3(pConn->GetDisplacement());//������ӿ�λ�ò��ԣ���֪��Ϊʲô
            //gp_Trsf transf;
            //double scale = LibPathsSetDlg::GetConnSize();
            //transf.SetScale(Ax3.Location(), scale);
            //BRepBuilderAPI_Transform myBRepTransformation(theShp, transf);
            //TopoDS_Shape trsfshp = myBRepTransformation.Shape();
            Handle(AIS_InteractiveObject) shpConn = new AIS_Shape(theShp/*trsfshp*/);
            if (pConn)
            {//��һ��Ľӿڣ���ʾ������ѡ��
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
             //��ʾװ��Լ���Ľӿ�
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
            {//����ֻ����ʾ
                theContext->SetColor(shpConn, Quantity_NOC_YELLOW, Standard_False);
                ;// theContext->Display(shpConn, theContext->DisplayMode(), -1, Standard_False);
            }
            vAIS.push_back(shpConn);
        }
    }
    //��ʾ��������Ľӿ�,��ʱ���û�ɫ��ʾ
    //DocumentModel* pMdl = (DocumentModel*)m_parent->GetDocument();
    //vector<ConnectConstraint> vecCC = pMdl->GetAllConnConstraints();
    else
    {
        for (int i = 0; i < shpConnectos.size(); i++)
        {
            TopoDS_Shape theShp = shpConnectos[i];
            //Connector* pConn = GetConnector(theShp);

            //��������ӿ�
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

                // //��ʾװ��Լ���Ľӿ�
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

    //��ʾ����Ľӿ�����ϵͳ
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

////��ʾ��������������: ����Ա任
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
//        //��ʾ���
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

//��������CyberInfo,�����������
CyberInfo* Component::GetCyberInfo() const
{
    if (IsAssembly())
    {
        return nullptr;
    }

    DocumentComponent* pDoc = (DocumentComponent * )m_pDoc;
    return pDoc->GetCyberInfo();
}
