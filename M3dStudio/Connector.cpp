/////////////////////////////////////////////////
#include "Connector.h"
//#include <TopoDS.hxx>
//#include <TopoDS_Shape.hxx>
//#include <TopoDS_Vertex.hxx>
//#include <TopoDS_Edge.hxx>
//#include <TopoDS_Face.hxx>
//#include <TNaming_Builder.hxx>
//#include <BRep_Tool.hxx>
//#include <TDataStd.hxx>
//#include <TDataStd_Name.hxx>
//#include <TNaming_NamedShape.hxx>
//#include <TDataStd_Integer.hxx>
//#include <TDataStd_Real.hxx>
//#include <TDataStd_RealArray.hxx>
//#include <TNaming_Tool.hxx>
//#include <TDF_ChildIterator.hxx>
//#include <Graphic3d_NameOfMaterial.hxx>
//#include <BrepGProp.hxx>
//#include <GProp_GProps.hxx>
//#include <GProp_PrincipalProps.hxx>
//#include <TopExp_Explorer.hxx>
//#include <TDataStd_ExtStringArray.hxx>
//#include <TDataStd_BooleanArray.hxx>
//#include "global.h"
#include <assert.h>


//////////////////////////////////////////////
//class Connector
//////////////////////////////////////////////
Connector::Connector()
{
	m_bZ_Reversed = false;
}

Connector::Connector(string& sType)
{
	m_sConnType = sType;
}

Connector::~Connector()
{

}

gp_Pnt Connector::GetCenter() const
{
	gp_Pnt p0(m_displacement[0], m_displacement[1], m_displacement[2]);

	return p0;
}

void Connector::SetConnType(string sType)
{
	m_sConnType = sType;
}

string Connector::GetConnType() const
{
	return m_sConnType;
}

void Connector::SetDefineType(myConnectDefineType vType)
{
	m_defType = vType;
}

myConnectDefineType Connector::GetDefineType() const
{
	return m_defType;
}

//图元引用
void Connector::SetRefEnts(vector<string>& vRefs)
{
	m_shapeRefs = vRefs;
}

vector<string> Connector::GetRefEnts() const
{
	return m_shapeRefs;
}

////得到位姿6
//vector<double> Connector::GetPosOrent() const
//{
//	vector<double> retVec;
//
//	retVec = m_origin;
//	for (int i = 0; i < m_orentation.size(); i++)
//	{
//		retVec.push_back(m_orentation[i]);
//	}
//
//	return retVec;
//}
//
//void Connector::SetPosOrient(vector<double>& v6)
//{
//	assert(v6.size() == 6);
//
//	for (int i = 0; i < 3; i++)
//	{
//		m_origin.push_back(v6[i]);
//	}
//
//	for (int i = 3; i < 6; i++)
//	{
//		m_orentation.push_back(v6[i]);
//	}
//}