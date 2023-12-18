#pragma once
#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

#ifndef DOCUMENT_COMPONENT_H
#define DOCUMENT_COMPONENT_H

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QList>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <TDocStd_Document.hxx>
#include <Standard_WarningsRestore.hxx>
#include "documentcommon.h"
#include "Connector.h"
#include "geoGonstraint.h"
#include "cyberInfo.h"

class MainWindow;
class TopTools_HSequenceOfShape;
class TDocStd_Application;

//! Implements visualization of samples content
class DocumentComponent : public DocumentCommon
{
public:
	DocumentComponent();
	DocumentComponent(MainWindow*);
	~DocumentComponent();

public slots:
	void objColor();
	//void objMaterial();

public:
	virtual void createNewDoc();
	//void open();
	virtual bool loadFile(const QString fileName);
	virtual bool save();
	virtual bool saveAs();
	virtual bool saveFile(const QString fileName);
	virtual void undo();
	virtual void redo();

	//存取数据
	//template<typename dtype> void serializeData(dtype, bool bSave);
	//存取几何
	virtual void serializeShapes(Handle(TopTools_HSequenceOfShape) theShapes, bool bSave = true);
	//void SaveShapes(Handle(TopTools_HSequenceOfShape) theShapes);
	virtual void ShowShapes(Handle(TopTools_HSequenceOfShape) theShapes);
	virtual void DisplayShapes(vector<TopoDS_Shape>&);
	virtual void SetMainShape(const TopoDS_Shape& aSolid);
	virtual void UpdateDisplay(bool b=true);

	//设置选择模式
	virtual void setSelectMode(QString sMode);
	virtual bool canUndo();
	virtual bool canRedo();

	virtual bool HideSelected();
	virtual bool DeleteSelected();
	virtual vector<Connector*> GetConnectors() const;
	void SetSelectedMaterial(int i);

	//void detect(int, int, bool);
	//测试函数
	void MakeBox();
	void MakeSpring();
	void ExportBREP();

private:
	void SetObjColor(TopoDS_Shape& aShape, Quantity_Color& aClr);
	void SetShapeTexture(const TopoDS_Shape& aShape, const char* sJpgFile);

	//从原来的M3dCom_DS移植
private:
	//根据形状类型，增加到相应的树节点上；存储关系表hm
	void AddShape(myShapeType t0, const TopoDS_Shape& aShape);
	//从hm得到Shape的Label
	TDF_Label GetShapeLabelandID(const TopoDS_Shape& theShape, vector<int>& ID) const;
	TDF_Label GetShapeLabel(const TopoDS_Shape& theShape) const;
	//得到TopoDS_Shape的ID ,如[3,3]，先从hashmap得到label,再从树上遍历找到label，获得序号；或直接由Label的tag得到
	vector<int> GetShapeID(const TopoDS_Shape& aShape) const;
	//由ID得到形状
	TopoDS_Shape GetShapeFromID(vector<int>& vID) const;
	TopoDS_Shape GetShapeFromID0(vector<int>& vID) const;
	//更新hashmap的形状表:由文档的内容更新hmShapes，打开文档读取时候;Undo/Redo
	void UpdateShapes();
	//获得全部Shapes，在读文件后，获得全部的TopoDS_Shape用于显示
	vector<TopoDS_Shape> GetAllShapes() const;

public:
	//得到全部几何的组合体
	TopoDS_Shape GetCompCompound() const;

	//加入主实体，用于存取物性参数: 零件的主实体只有一个!!!
	void SetMainSolid(const TopoDS_Shape& theShape, Graphic3d_NameOfMaterial mat = Graphic3d_NameOfMaterial::Graphic3d_NameOfMaterial_Steel);
	//清空文档
	void Clear();
	//给选中的形状贴图
	void Texture();
	//获得主实体
	TopoDS_Shape GetMainSolid(Graphic3d_NameOfMaterial& mat) const;
	//得到TopoDS_Shape的命名，如“面3”
	virtual string GetShapeName(const TopoDS_Shape& aShape) const;
	//由名字得到形状
	TopoDS_Shape GetShapeFromName(string theName) const;
	TopoDS_Shape DocumentComponent::GetShapeFromName(string& theName, const TopoDS_Shape& mainSolid) const;

	//2.接口存储相关
public:
	//增加接口，在Root下增加子节点存储各接口
	void AddConnector(Connector* pConn);//先Generate()创建形状
	//由形状得到接口的信息；此new Connector，用户根据需要delete,如果该形状不是Connector，则返回null
	Connector* GetConnector(const TopoDS_Shape& connShape) const;
	//更新已有接口
	bool UpdateConnector(const TopoDS_Shape& connShape, Connector* pCon);
	//删除接口
	bool DeleteConnector(const TopoDS_Shape& connShape);

	//增加\获得\更新、删除参考点、线
	void AddDatumPoint(const TopoDS_Shape& connShape, DatumPoint* pDPt);
	DatumPoint* GetDatumPoint(const TopoDS_Shape& connShape);
	bool UpdateDatumPoint(const TopoDS_Shape& connShape, DatumPoint* pDPt);
	bool DeleteDatumPoint(const TopoDS_Shape& connShape);
	void AddDatumLine(const TopoDS_Shape& connShape, DatumLine* pDPt);
	DatumLine* GetDatumLine(const TopoDS_Shape& connShape);
	bool UpdateDatumLine(const TopoDS_Shape& connShape, DatumLine* pDPt);
	bool DeleteDatumLine(const TopoDS_Shape& connShape);

	//增加\获得\更新\删除标记面
	void AddMarkingSurf(const TopoDS_Shape& connShape, MarkingSurface* pMSt);
	MarkingSurface* GetMarkingSurf(const TopoDS_Shape& connShape);
	bool UpdateMarkingSurf(const TopoDS_Shape& connShape, MarkingSurface* pMSt);
	bool DeleteMarkingSurf(const TopoDS_Shape& connShape);
	bool isMarkingSurf(string& name); //根据名字判断是否是标记面

	//3.颜色存取相关
public:
	//向文档设置某形状的颜色，找到其对应的Label，增加其颜色属性
	void SetShapeColor(const TopoDS_Shape& theShape, Quantity_Color theColor);
	//从文档获得某形状的颜色
	Quantity_Color GetShapeColor(const TopoDS_Shape& theShape) const;
	map<string, vector<double> > GetColorMap();
	map<string, string > GetTextureMap();

	//4.约束存取相关
private:
	//向文档增加约束，增加相应的TDF_Label子节点
	void AddConstraint(TDF_Label& parent, int nth, myConstraint& pConstraint);
	//从文档删除某个约束,第几个
	bool DeleteConstraint(int theConsID);
	//更新第theID个约束
	bool UpdateConstraint(int theID, myConstraint& pConstraint);
	//得到第几个约束, 返回null，或有值，用户负责删除
	myConstraint* GetConstraint(TDF_Label& parent, int nTh) const;
	//得到第几个约束的两个实体元素
	vector<TopoDS_Shape> GetConstraintShapes(int nTh) const;
	//约束的形状
	//......
public:
	//清除当前的约束，设置新的约束们
	void SetConstraints(vector<myConstraint>& vConstraints);
	//得到全部约束
	vector<myConstraint> GetAllConstraints() const;

	//5.材料和透明度
public:
	//设置零件材料,以整型属性存储到TDF_Label(solid)根
	void SetMaterial(Graphic3d_NameOfMaterial nMaterial);
	//从文档获得材料
	Graphic3d_NameOfMaterial GetMaterial() const;
	//设置透明度,同 材料
	void SetTransparency(double nTransp);
	//获得零件透明度
	double  GetTransparency() const;

	//6.物性信息存取:质心[3]，质量，转动惯量[3X3]/2
private:
	//设置物性参数,存储在根节点,作为Attribute
	void SetPhysicalProperty(myPhysicalProperty& par);
public:
	//从文档获得物性参数
	myPhysicalProperty GetPhysicalProperty() const;
	//计算质量
	double ComputeM();
	//计算frame_a到质心
	gp_Vec ComputeR_CM();
	//计算frame_a到frame_b
	gp_Vec ComputeR_ab();
	//计算I
	gp_Mat ComputeMoment();

	//7.Cyber信息数据存取
private:
	void AddParameter(TDF_Label& parLabel, int nIndex, myParameter* aPar);
	void AddOutputVar(TDF_Label& parLabel, int nIndex, myOutputVar* aVar);
	myParameter* GetParameter(TDF_Label& parLabel, int nth) const;
	myOutputVar* GetOutputVar(TDF_Label& parLabel, int nth) const;
public:
	//界面Update时，设置Cyber信息到文档
	bool SetCyberInfo(CyberInfo* pCyber);
	//从文档获取Cyber信息,如果失败返回null,此函数new CyberInfo，用户自己delete
	CyberInfo* GetCyberInfo() const;
	//读文件,lingjian
	static DocumentComponent* ReadComponentFile(const char* file);
	//创建Component对象
	Component* GenerateComponent();
	vector<DatumPoint*> GetAllDatumPoints() const;
	vector<DatumLine*>  GetAllDatumLines() const;
	vector<MarkingSurface*> GetAllMarkingSurface() const;
	vector<Connector*>  GetAllConnectors() const;
	Connector* GetConnector(const char* sName) const;
	vector<ShapeColor> GetAllShapeColors() const;

	//删除引用了元素sName的对象
	void DeleteRefObjects(const char* sName);
	bool IsWorld() const;

private:
	bool LookForColorLabel(const char* shpName, TDF_Label& theLabel);
	bool LookForTextureLabel(const char* shpName, TDF_Label& theLabel);
	static TDF_Label LookupForShapeLabelFromFather(const TDF_Label& father, const TopoDS_Shape& theShape, int& nth);
	static Connector* CreateConnectorFromLabel(TDF_Label theLabel);
	static bool LookForShape(const TopoDS_Shape& parent, TopAbs_ShapeEnum type, int nth, TopoDS_Shape& retShape);
	static bool LookForShapeFromParentLabel(TDF_Label parent, int nth, TopoDS_Shape& ret);
	static DatumPoint* CreateDatumPointFromLabel(TDF_Label theLabel);
	static DatumLine* CreateDatumLineFromLabel(TDF_Label theLabel);
	static MarkingSurface* CreateMarkingSurfFromLabel(TDF_Label theLabel);
};

#endif
