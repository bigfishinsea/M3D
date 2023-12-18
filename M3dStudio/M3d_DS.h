#pragma once
#include "m3d.h"
#include <TDF_Label.hxx>
#include <TopoDS_Shape.hxx>
#include <qpixmap.h>

class Component;

class M3d_DS
{
public:
	M3d_DS() {}
	M3d_DS(TDF_Label thelbl);

protected:
	//�ĵ���Root�ڵ�ָ�룻
	//Handle(TDF_Label) m_pDocRoot; //ʹ��ָ�������
	TDF_Label m_pDocRoot;
	Component* m_pCompRoot;

public:
	void SetRoot(TDF_Label root) { m_pDocRoot = root; }
	TDF_Label GetRoot() const { return m_pDocRoot; }
	virtual string GetShapeName(const TopoDS_Shape & shpSel) const;
	void SetCompRoot(Component* pCom) { m_pCompRoot = pCom; }
	Component* GetComponent() { return m_pCompRoot; }

	//λͼ��ȡ
	void SavePixmap(QPixmap& pm);
	QPixmap GetPixmap() const;

protected:
	//������̬����
	static TDF_Label LookupForShapeLabelFromFather(const TDF_Label& father, const TopoDS_Shape& theShape, int& nth);
	//����static����������ת��
	static string GetShapeNameFromID(vector<int>& theID);
	static vector<int> GetShapeIDFromName(string& theName);
};

