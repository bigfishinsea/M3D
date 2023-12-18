#pragma once
#include "m3d.h"
#include "gp_Ax3.hxx"
#include "gp_Trsf.hxx"
#include <TopoDS_Shape.hxx>

enum voxelType
{
	SPHERE = 0,
	BOX,
	CYLINDER,
	WEDHE,
	CONE
};

enum boolOperator
{
	BASE=0,
	UNION,
	COMMAN,
	SUB1,//被差掉
	SUB2 //主差原来的
};

class voxel
{
protected:
	string    m_name; 
	voxelType m_type;
	boolOperator m_boolType;

public:
	voxel();
	~voxel();

	voxelType GetType() const { return m_type; }
	void SetType(int nType) { m_type = (voxelType)nType; }
	boolOperator GetBoolType() const { return m_boolType; }
	void SetBoolType(boolOperator type) { m_boolType = type; }
	virtual TopoDS_Shape MakeShape() = 0;
	void SetName(const char* sName) { m_name = sName; }
	string GetName() const { return m_name; }
};

class mySphere : public voxel
{
public:
	mySphere();
	mySphere(gp_Pnt center, double r);
	~mySphere();

	void SetRadius(double r);
	double GetRadius()const;
	void SetCenter(gp_Pnt p0);
	gp_Pnt GetCenter() const;

	virtual TopoDS_Shape MakeShape();

private://半球？球冠？
	gp_Pnt m_center;
	double m_radius;
};

class myBox : public voxel
{
public:
	myBox();
	myBox(double L, double W, double H);
	~myBox();

private:
	double m_len;
	double m_wid;
	double m_height;
	gp_Pnt m_corner;
	gp_Dir m_XDir;
	gp_Dir m_ZDir;

public:
	void SetData(double L, double W, double H, gp_Pnt corner, gp_Dir xdir, gp_Dir zDir);
	void GetData(double& L, double& W, double& H, gp_Pnt& corner, gp_Dir& xdir, gp_Dir& zDir);

	virtual TopoDS_Shape MakeShape();
};

class myCylinder : public voxel
{
public:
	myCylinder(double r, double h);
	myCylinder();
	~myCylinder();

private:
	double m_radius;
	double m_height;
	gp_Pnt m_btnCenter;
	gp_Dir m_hDir;
	
public:
	void SetData(double r, double h, gp_Pnt p0, gp_Dir hDir);
	void GetData(double &r, double& h, gp_Pnt& p0, gp_Dir& hDir);
	virtual TopoDS_Shape MakeShape();
};
