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
	NM_CONNECTOR = 5,    //�ӿڣ��������ͣ�������Name����
	NM_DATUMPOINT = 6, //�ο���
	NM_DATUMLINE = 7,   //�ο���
	NM_DATUMPLANE = 8,   //�ο���
};

//Լ������,��ȫ�貹��...
enum myConstraintType
{
	PARALLEL = 1,
	CONCINE = 2,
	TAGENT = 3,
	DIM_LENGTH = 4,
	DIM_ANGLE = 5
};

//Լ���ṹ��
struct myConstraint
{
	int nIndex; //Լ�������
	myConstraintType consType;  //Լ������
	//vector<int>      shapeID1;  //��һ��Ԫ��
	string           shapeName1;
	//vector<int>      shapeID2;  //�ڶ���Ԫ��
	string           shapeName2;
	double           consValue; //Լ��ֵ���ߴ�Լ��
	bool             bActive;   //�Ƿ�������
};
