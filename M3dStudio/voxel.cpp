#include "voxel.h"
#include <BRepPrimAPI_MakeSphere.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>

voxel::voxel()
{

}

voxel::~voxel()
{

}

////
mySphere::mySphere()
{
	m_type = SPHERE;
	m_center = gp_Pnt(0, 0, 0);
	m_radius = 10;
}

mySphere::mySphere(gp_Pnt center, double r)
{
	m_type = SPHERE;

	m_center = center;
	m_radius = r;
}

mySphere::~mySphere()
{
}

void mySphere::SetRadius(double r)
{
	m_radius = r;
}

double mySphere::GetRadius()const
{
	return m_radius;
}

void mySphere::SetCenter(gp_Pnt p0)
{
	m_center = p0;
}

gp_Pnt mySphere::GetCenter() const
{
	return m_center;
}

TopoDS_Shape mySphere::MakeShape()
{
	return BRepPrimAPI_MakeSphere(m_center, m_radius);
}

///
myBox::myBox()
{
	m_type = BOX;
}

myBox::myBox(double L, double W, double H)
{
	m_type = BOX;
	m_len = L;
	m_wid = W;
	m_height = H;
	m_corner = gp_Pnt(0, 0, 0);
	m_XDir = gp_Dir(1, 0, 0);
	m_ZDir = gp_Dir(0, 0, 1);
}

myBox::~myBox()
{

}

void myBox::SetData(double L, double W, double H, gp_Pnt corner, gp_Dir xdir, gp_Dir zDir)
{
	m_len = L;
	m_wid = W;
	m_height = H;
	m_corner = corner;
	m_XDir = xdir;
	m_ZDir = zDir;
}

void myBox::GetData(double& L, double& W, double& H, gp_Pnt& corner, gp_Dir& xdir, gp_Dir& zDir)
{
	L = m_len;
	W = m_wid;
	H = m_height;
	corner = m_corner;
	xdir = m_XDir;
	zDir = m_ZDir;
}

TopoDS_Shape myBox::MakeShape()
{
	gp_Ax2 Ax2;
	Ax2.SetLocation(m_corner);
	Ax2.SetXDirection(m_XDir);
	Ax2.SetDirection(m_ZDir);
	return BRepPrimAPI_MakeBox(Ax2, m_len, m_wid, m_height);
}

////
myCylinder::myCylinder()
{
	m_type = CYLINDER;
	m_btnCenter = gp_Pnt(0, 0, 0);
	m_radius = 10;
	m_hDir = gp_Dir(0,0,1);
	m_height = 30;
}

myCylinder::myCylinder(double r, double h)
{
	m_type = CYLINDER;
	m_btnCenter = gp_Pnt(0, 0, 0);
	m_radius = r;
	m_hDir = gp_Dir(0, 0, 1);
	m_height = h;
}

myCylinder::~myCylinder()
{

}

void myCylinder::SetData(double r, double h, gp_Pnt p0, gp_Dir hDir)
{
	m_radius = r;
	m_height = h;
	m_btnCenter = p0;
	m_hDir = hDir;
}
void myCylinder::GetData(double& r, double& h, gp_Pnt& p0, gp_Dir& hDir)
{
	r = m_radius;
	h = m_height;
	p0 = m_btnCenter;
	hDir = m_hDir;
}

TopoDS_Shape myCylinder::MakeShape()
{
	gp_Ax2 Ax2;
	Ax2.SetLocation(m_btnCenter);
	Ax2.SetDirection(m_hDir);
	return BRepPrimAPI_MakeCylinder(Ax2, m_radius, m_height);
}
