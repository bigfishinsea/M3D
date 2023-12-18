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

	//��ȡ����
	//template<typename dtype> void serializeData(dtype, bool bSave);
	//��ȡ����
	virtual void serializeShapes(Handle(TopTools_HSequenceOfShape) theShapes, bool bSave = true);
	//void SaveShapes(Handle(TopTools_HSequenceOfShape) theShapes);
	virtual void ShowShapes(Handle(TopTools_HSequenceOfShape) theShapes);
	virtual void DisplayShapes(vector<TopoDS_Shape>&);
	virtual void SetMainShape(const TopoDS_Shape& aSolid);
	virtual void UpdateDisplay(bool b=true);

	//����ѡ��ģʽ
	virtual void setSelectMode(QString sMode);
	virtual bool canUndo();
	virtual bool canRedo();

	virtual bool HideSelected();
	virtual bool DeleteSelected();
	virtual vector<Connector*> GetConnectors() const;
	void SetSelectedMaterial(int i);

	//void detect(int, int, bool);
	//���Ժ���
	void MakeBox();
	void MakeSpring();
	void ExportBREP();

private:
	void SetObjColor(TopoDS_Shape& aShape, Quantity_Color& aClr);
	void SetShapeTexture(const TopoDS_Shape& aShape, const char* sJpgFile);

	//��ԭ����M3dCom_DS��ֲ
private:
	//������״���ͣ����ӵ���Ӧ�����ڵ��ϣ��洢��ϵ��hm
	void AddShape(myShapeType t0, const TopoDS_Shape& aShape);
	//��hm�õ�Shape��Label
	TDF_Label GetShapeLabelandID(const TopoDS_Shape& theShape, vector<int>& ID) const;
	TDF_Label GetShapeLabel(const TopoDS_Shape& theShape) const;
	//�õ�TopoDS_Shape��ID ,��[3,3]���ȴ�hashmap�õ�label,�ٴ����ϱ����ҵ�label�������ţ���ֱ����Label��tag�õ�
	vector<int> GetShapeID(const TopoDS_Shape& aShape) const;
	//��ID�õ���״
	TopoDS_Shape GetShapeFromID(vector<int>& vID) const;
	TopoDS_Shape GetShapeFromID0(vector<int>& vID) const;
	//����hashmap����״��:���ĵ������ݸ���hmShapes�����ĵ���ȡʱ��;Undo/Redo
	void UpdateShapes();
	//���ȫ��Shapes���ڶ��ļ��󣬻��ȫ����TopoDS_Shape������ʾ
	vector<TopoDS_Shape> GetAllShapes() const;

public:
	//�õ�ȫ�����ε������
	TopoDS_Shape GetCompCompound() const;

	//������ʵ�壬���ڴ�ȡ���Բ���: �������ʵ��ֻ��һ��!!!
	void SetMainSolid(const TopoDS_Shape& theShape, Graphic3d_NameOfMaterial mat = Graphic3d_NameOfMaterial::Graphic3d_NameOfMaterial_Steel);
	//����ĵ�
	void Clear();
	//��ѡ�е���״��ͼ
	void Texture();
	//�����ʵ��
	TopoDS_Shape GetMainSolid(Graphic3d_NameOfMaterial& mat) const;
	//�õ�TopoDS_Shape���������硰��3��
	virtual string GetShapeName(const TopoDS_Shape& aShape) const;
	//�����ֵõ���״
	TopoDS_Shape GetShapeFromName(string theName) const;
	TopoDS_Shape DocumentComponent::GetShapeFromName(string& theName, const TopoDS_Shape& mainSolid) const;

	//2.�ӿڴ洢���
public:
	//���ӽӿڣ���Root�������ӽڵ�洢���ӿ�
	void AddConnector(Connector* pConn);//��Generate()������״
	//����״�õ��ӿڵ���Ϣ����new Connector���û�������Ҫdelete,�������״����Connector���򷵻�null
	Connector* GetConnector(const TopoDS_Shape& connShape) const;
	//�������нӿ�
	bool UpdateConnector(const TopoDS_Shape& connShape, Connector* pCon);
	//ɾ���ӿ�
	bool DeleteConnector(const TopoDS_Shape& connShape);

	//����\���\���¡�ɾ���ο��㡢��
	void AddDatumPoint(const TopoDS_Shape& connShape, DatumPoint* pDPt);
	DatumPoint* GetDatumPoint(const TopoDS_Shape& connShape);
	bool UpdateDatumPoint(const TopoDS_Shape& connShape, DatumPoint* pDPt);
	bool DeleteDatumPoint(const TopoDS_Shape& connShape);
	void AddDatumLine(const TopoDS_Shape& connShape, DatumLine* pDPt);
	DatumLine* GetDatumLine(const TopoDS_Shape& connShape);
	bool UpdateDatumLine(const TopoDS_Shape& connShape, DatumLine* pDPt);
	bool DeleteDatumLine(const TopoDS_Shape& connShape);

	//����\���\����\ɾ�������
	void AddMarkingSurf(const TopoDS_Shape& connShape, MarkingSurface* pMSt);
	MarkingSurface* GetMarkingSurf(const TopoDS_Shape& connShape);
	bool UpdateMarkingSurf(const TopoDS_Shape& connShape, MarkingSurface* pMSt);
	bool DeleteMarkingSurf(const TopoDS_Shape& connShape);
	bool isMarkingSurf(string& name); //���������ж��Ƿ��Ǳ����

	//3.��ɫ��ȡ���
public:
	//���ĵ�����ĳ��״����ɫ���ҵ����Ӧ��Label����������ɫ����
	void SetShapeColor(const TopoDS_Shape& theShape, Quantity_Color theColor);
	//���ĵ����ĳ��״����ɫ
	Quantity_Color GetShapeColor(const TopoDS_Shape& theShape) const;
	map<string, vector<double> > GetColorMap();
	map<string, string > GetTextureMap();

	//4.Լ����ȡ���
private:
	//���ĵ�����Լ����������Ӧ��TDF_Label�ӽڵ�
	void AddConstraint(TDF_Label& parent, int nth, myConstraint& pConstraint);
	//���ĵ�ɾ��ĳ��Լ��,�ڼ���
	bool DeleteConstraint(int theConsID);
	//���µ�theID��Լ��
	bool UpdateConstraint(int theID, myConstraint& pConstraint);
	//�õ��ڼ���Լ��, ����null������ֵ���û�����ɾ��
	myConstraint* GetConstraint(TDF_Label& parent, int nTh) const;
	//�õ��ڼ���Լ��������ʵ��Ԫ��
	vector<TopoDS_Shape> GetConstraintShapes(int nTh) const;
	//Լ������״
	//......
public:
	//�����ǰ��Լ���������µ�Լ����
	void SetConstraints(vector<myConstraint>& vConstraints);
	//�õ�ȫ��Լ��
	vector<myConstraint> GetAllConstraints() const;

	//5.���Ϻ�͸����
public:
	//�����������,���������Դ洢��TDF_Label(solid)��
	void SetMaterial(Graphic3d_NameOfMaterial nMaterial);
	//���ĵ���ò���
	Graphic3d_NameOfMaterial GetMaterial() const;
	//����͸����,ͬ ����
	void SetTransparency(double nTransp);
	//������͸����
	double  GetTransparency() const;

	//6.������Ϣ��ȡ:����[3]��������ת������[3X3]/2
private:
	//�������Բ���,�洢�ڸ��ڵ�,��ΪAttribute
	void SetPhysicalProperty(myPhysicalProperty& par);
public:
	//���ĵ�������Բ���
	myPhysicalProperty GetPhysicalProperty() const;
	//��������
	double ComputeM();
	//����frame_a������
	gp_Vec ComputeR_CM();
	//����frame_a��frame_b
	gp_Vec ComputeR_ab();
	//����I
	gp_Mat ComputeMoment();

	//7.Cyber��Ϣ���ݴ�ȡ
private:
	void AddParameter(TDF_Label& parLabel, int nIndex, myParameter* aPar);
	void AddOutputVar(TDF_Label& parLabel, int nIndex, myOutputVar* aVar);
	myParameter* GetParameter(TDF_Label& parLabel, int nth) const;
	myOutputVar* GetOutputVar(TDF_Label& parLabel, int nth) const;
public:
	//����Updateʱ������Cyber��Ϣ���ĵ�
	bool SetCyberInfo(CyberInfo* pCyber);
	//���ĵ���ȡCyber��Ϣ,���ʧ�ܷ���null,�˺���new CyberInfo���û��Լ�delete
	CyberInfo* GetCyberInfo() const;
	//���ļ�,lingjian
	static DocumentComponent* ReadComponentFile(const char* file);
	//����Component����
	Component* GenerateComponent();
	vector<DatumPoint*> GetAllDatumPoints() const;
	vector<DatumLine*>  GetAllDatumLines() const;
	vector<MarkingSurface*> GetAllMarkingSurface() const;
	vector<Connector*>  GetAllConnectors() const;
	Connector* GetConnector(const char* sName) const;
	vector<ShapeColor> GetAllShapeColors() const;

	//ɾ��������Ԫ��sName�Ķ���
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
