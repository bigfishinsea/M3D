#pragma once

#ifndef DOCUMENT_COMMON_OVERVIEW_H
#define DOCUMENT_COMMON_OVERVIEW_H

#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <QList>
#include <AIS_InteractiveContext.hxx>
#include <V3d_Viewer.hxx>
#include <TDocStd_Document.hxx>
#include <Standard_WarningsRestore.hxx>
//#include "M3d_DS.h"
#include "m3d.h"
#include <BRepTools.hxx>
#include<Ais_Shape.hxx>
#include <qmutex.h>

class MainWindow;
class TopTools_HSequenceOfShape;
class TDocStd_Application;
class Component;
class Connector;

//! Implements visualization of samples content
class DocumentCommon : public QObject
{
	Q_OBJECT

protected:
	TDF_Label m_pDocRoot;
	Component* m_pCompRoot;
	bool m_IsModel; //װ����
	bool myContextIsEmpty;
	Handle(AIS_InteractiveContext) myContext;
	QString                  myfileName;
	//OCAF�ĵ�
	Handle(TDocStd_Document) m3dOcafDoc;
	QMutex* m_pMutex;

	bool m_bModified;
	bool m_bShowOxyz;
	
protected:
	Handle(V3d_Viewer)  myViewer;
	Handle(TDocStd_Application) anOcaf_Application;
	//map<string, Handle(AIS_InteractiveObject)> myDisplayList; //��ʾʵ��

public:
	DocumentCommon() { m_bModified = false; }
	DocumentCommon(MainWindow*);
	~DocumentCommon();

	QMutex* GetMutex() const { return m_pMutex; }
	const Handle(AIS_InteractiveContext)& getContext() { return myContext; }

	const Handle(V3d_Viewer)& getViewer() { return myViewer; }

	void setViewer(const Handle(V3d_Viewer)& theViewer) { myViewer = theViewer; }

	void SetObjects(const NCollection_Vector<Handle(AIS_InteractiveObject)>& theObjects,
		Standard_Boolean theDisplayShaded = Standard_False);

	bool IsEmpty() const { return myContextIsEmpty; }
	bool IsModel() const { return m_IsModel; }

	//���ݵ�ǰ�Ӵ��ı����������ͷ�����������ʾ����
	void GetAx3Length(double& len, double& arrowLen);
	//��ýӿڵĻ�׼����
	double GetConnectorBaseLength();

	bool IsModified() const { return m_bModified; }
	void SetModified(bool b = true) { m_bModified = b; }

signals:
	void selectionChanged();
	void sendCloseDocument(DocumentCommon*);

	//public slots:
	//  void objColor();

protected:
	Handle(V3d_Viewer) Viewer(const Standard_ExtString theName,
		const Standard_CString theDomain,
		const Standard_Real theViewSize,
		const V3d_TypeOfOrientation theViewProj,
		const Standard_Boolean theComputedMode,
		const Standard_Boolean theDefaultComputedMode);
	void DisplayPresentation();
	void AddRefColor(TDF_Label& aLabel, Quantity_Color& aClr);
	void DisplayGlobalCoordsys();

public:
	virtual void Clear();
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
	virtual void SetMainShape(const TopoDS_Shape& aSolid) {}
	virtual void UpdateDisplay(bool b=true) {}

	////�����ĵ����ͣ�ֱ����bIsAssembly�õ�
	//void setDocType(int nType);

	//����ѡ��ģʽ
	virtual void setSelectMode(QString sMode);
	virtual bool canUndo();
	virtual bool canRedo();

    //ɾ��
	virtual bool DeleteSelected() { return true; }
	virtual bool HideSelected() { return true; }

	//��ýӿ�
	virtual vector<Connector*> GetConnectors() const = 0;
	QString GetFileName() const { return myfileName; }
	void SetFileName(const char* sName) { myfileName = sName; }

	//����Ƿ�ѡ��ʵ��
    //bool SelectShape();
    //void ShiftSelectObject(const Standard_Integer flag);

	//��ԭ��M3d_DS����
public:
	void SetRoot(TDF_Label root) { m_pDocRoot = root; }
	TDF_Label GetRoot() const { return m_pDocRoot; }
	virtual string GetShapeName(const TopoDS_Shape& shpSel) const;
	void SetCompRoot(Component* pCom) { m_pCompRoot = pCom; }
	Component* GetComponent() { return m_pCompRoot; }

	//λͼ��ȡ
	void SavePixmap(QPixmap& pm);
	QPixmap GetPixmap() const;

	//��ʾȫ������ϵ
	void ShowOxyz(bool b);

protected:
	//������̬����
	//static TDF_Label LookupForShapeLabelFromFather(const TDF_Label& father, const TopoDS_Shape& theShape, int& nth);
	//����static����������ת��
	static string GetShapeNameFromID(vector<int>& theID);
	static vector<int> GetShapeIDFromName(string& theName);
};

#endif
