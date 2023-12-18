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
	//文档的Root节点指针；
	//Handle(TDF_Label) m_pDocRoot; //使用指针出问题
	TDF_Label m_pDocRoot;
	Component* m_pCompRoot;

public:
	void SetRoot(TDF_Label root) { m_pDocRoot = root; }
	TDF_Label GetRoot() const { return m_pDocRoot; }
	virtual string GetShapeName(const TopoDS_Shape & shpSel) const;
	void SetCompRoot(Component* pCom) { m_pCompRoot = pCom; }
	Component* GetComponent() { return m_pCompRoot; }

	//位图存取
	void SavePixmap(QPixmap& pm);
	QPixmap GetPixmap() const;

protected:
	//几个静态函数
	static TDF_Label LookupForShapeLabelFromFather(const TDF_Label& father, const TopoDS_Shape& theShape, int& nth);
	//两个static函数，用于转换
	static string GetShapeNameFromID(vector<int>& theID);
	static vector<int> GetShapeIDFromName(string& theName);
};

