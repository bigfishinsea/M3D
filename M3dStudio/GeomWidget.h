//2021.09.16
//Wu YZ M3d-SOFTWARE

#ifndef GEOMWIDGET_H
#define GEOMWIDGET_H

#include "View.h"
#include "DocumentComponent.h"

#include <Standard_WarningsDisable.hxx>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

class ApplicationCommon;
class QStackedWidget;

//! Qt widget for organize 3D  documents
class GeomWidget : public QWidget
{
  Q_OBJECT
public:
  GeomWidget(DocumentCommon* theDocument3d,
             QWidget* theParent = nullptr);

  void FitAll();

  Handle(V3d_View) Get3dView() { return myView3d->getView(); }
  View* GetView() { return myView3d; }

  void Show3d();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent*) Standard_OVERRIDE;

private slots:
    void OnStateChanged(const QString&);

private:
  View*    myView3d;
 
  QWidget* my3dVidget;
  QStackedWidget* myStackWidget;

  DocumentCommon* myDocument3d;

  QString status;

  //virtual void mouseMoveEvent(QMouseEvent* evt);
};

#endif //GEOMWIDGET_H
