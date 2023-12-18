#pragma once
#include <string>

using namespace std;

enum myShapeType
{
	NM_DEFAULT = 0,
	NM_VERTEX = 1,
	NM_EDGE = 2,
	NM_FACE = 3,
	NM_SOLID = 4,
	NM_CONNECTOR = 5,    //接口，用面类型，以属性Name区分
	NM_DATUMPOINT = 6, //参考点
	NM_DATUMLINE = 7,   //参考线
	NM_DATUMPLANE = 8,   //参考面
};

//约束类型,不全需补充...
enum myConstraintType
{
	PARALLEL = 1,
	CONCINE = 2,
	TAGENT = 3,
	DIM_LENGTH = 4,
	DIM_ANGLE = 5
};

//约束结构体
struct myConstraint
{
	int nIndex; //约束的序号
	myConstraintType consType;  //约束类型
	//vector<int>      shapeID1;  //第一个元素
	string           shapeName1;
	//vector<int>      shapeID2;  //第二个元素
	string           shapeName2;
	double           consValue; //约束值，尺寸约束
	bool             bActive;   //是否起作用
};
