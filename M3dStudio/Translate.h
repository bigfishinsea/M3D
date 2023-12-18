#pragma once

#ifndef TRANSLATE_H
#define TRANSLATE_H
#include "DocumentComponent.h"
#include <Standard_WarningsDisable.hxx>
#include <QObject>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <TopTools_HSequenceOfShape.hxx>

class TranslateDlg;

class __declspec(dllexport) Translate : public QObject
{
	Q_OBJECT

public:
	enum { FormatBREP, FormatIGES, FormatSTEP, FormatVRML, FormatSTL };

	Translate(QObject*);
	~Translate();

	QString                               info() const;
	bool importModel(Handle(TopTools_HSequenceOfShape) & shapes, const Handle(AIS_InteractiveContext)& ic);

protected:
	virtual bool                              displayShSequence(const Handle(AIS_InteractiveContext)&,
		const Handle(TopTools_HSequenceOfShape)&);
	QString                                   selectFileName(const int, const bool);

public:
	static Handle(TopTools_HSequenceOfShape)   getShapes(const Handle(AIS_InteractiveContext)&);
	void   ExportBREP(const Handle(TopTools_HSequenceOfShape)&);
private:
	TranslateDlg* getDialog(const int, const bool);

	Handle(TopTools_HSequenceOfShape)		  importModel(const int format, const QString& file);
	Handle(TopTools_HSequenceOfShape)         importBREP(const QString&);
	Handle(TopTools_HSequenceOfShape)         importIGES(const QString&);
	Handle(TopTools_HSequenceOfShape)         importSTEP(const QString&);

	bool exportBREP(const QString&, const Handle(TopTools_HSequenceOfShape)&);
	bool exportIGES(const QString&, const Handle(TopTools_HSequenceOfShape)&);
	bool exportSTEP(const QString&, const Handle(TopTools_HSequenceOfShape)&);
	bool exportSTL(const QString&, const Handle(TopTools_HSequenceOfShape)&);
	bool exportVRML(const QString&, const Handle(TopTools_HSequenceOfShape)&);

	bool checkFacetedBrep(const Handle(TopTools_HSequenceOfShape)&);

protected:
	TranslateDlg* myDlg;
	QString       myInfo;
};

#endif

