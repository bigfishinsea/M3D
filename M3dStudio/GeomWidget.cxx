//Wu YZ
//M3d-SOFTWARE
#include "M3d.h"
#include "GeomWidget.h"
#include <Standard_WarningsDisable.hxx>
#include <TopoDS_Shape.hxx>
#include <QFrame>
#include <QBoxLayout>
#include <QTextEdit>
#include <QStackedLayout>
#include <QToolBar>
#include <QStackedWidget>
#include <QMessageBox>
#include <Standard_WarningsRestore.hxx>

GeomWidget::GeomWidget (DocumentCommon* theDocument3d,
                         QWidget* theParent)
: QWidget (theParent),
  myDocument3d(theDocument3d)
{
  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  aMainLayout->setContentsMargins(0, 0, 0, 0);

  my3dVidget = new QWidget;
  QVBoxLayout* a3dLayout = new QVBoxLayout(my3dVidget);
  a3dLayout->setContentsMargins(0, 0, 0, 0);
  a3dLayout->setSpacing(0);
  myView3d = new View(myDocument3d->getContext(), true, my3dVidget);
  //myView3d->SetM3dDS(myDocument3d->GetDS());
  myView3d->SetDocument(myDocument3d);
  QToolBar* aToolBar3d = new QToolBar;
  aToolBar3d->addActions(myView3d->getViewActions());
  aToolBar3d->addSeparator();
  aToolBar3d->addActions(myView3d->getRaytraceActions());
  a3dLayout->addWidget(aToolBar3d);
  a3dLayout->addWidget(myView3d);

  myStackWidget = new QStackedWidget(this);
  aMainLayout->addWidget(myStackWidget);
  //myStackWidget->addWidget(my2dVidget);
  myStackWidget->addWidget(my3dVidget);
  connect(this->GetView(), &View::StateChanged, this, &GeomWidget::OnStateChanged);

  FitAll();
}

void GeomWidget::mouseDoubleClickEvent(QMouseEvent*)
{
    Handle(AIS_InteractiveContext) pContext = myDocument3d->getContext();
    Handle(AIS_InteractiveObject) Current;
    pContext->InitSelected();
    Current = pContext->SelectedInteractive();
    TopoDS_Shape shpSel = pContext->SelectedShape();
    bool bHasSel = pContext->HasSelectedShape();
    if (shpSel.IsNull())
    {
        return;
    }
    //M3d_DS * pCom = myDocument3d->GetDS();
    string sType = myDocument3d->GetShapeName(shpSel);
    QMessageBox::information(this, "实体类型", sType.c_str(), QMessageBox::Ok);
}

void GeomWidget::FitAll()
{
  //if (myDocument2d->IsEmpty())
    Show3d();
  //else
    //Show2d();
}

void GeomWidget::Show3d()
{
  myView3d->axo();
  myView3d->fitAll();
  QAction* aShadingAction = myView3d->getViewAction(ViewAction_Shading);
  aShadingAction->trigger();
  aShadingAction->setChecked(true);
  QAction* aHlrOffAction = myView3d->getViewAction(ViewAction_HlrOff);
  aHlrOffAction->trigger();
  aHlrOffAction->setChecked(true);
  myStackWidget->setCurrentWidget(my3dVidget);
  setStatusTip("Mouse buttons: Right-Zoom, Middle-Pan, Left-Rotate");
}

void GeomWidget::OnStateChanged(const QString& str)
{
    this->status = str;
    if (status.isEmpty())
        setStatusTip("Mouse buttons: Right-Zoom, Middle-Pan, Left-Rotate");
    else
        setStatusTip(status);
}

//void GeomWidget::mouseMoveEvent(QMouseEvent* evt)
//{
//    myDocument3d->detect(evt->x(), evt->y(), true);
//}

//void GeomWidget::Show2d()
//{
//  myView2d->fitAll();
//  myStackWidget->setCurrentWidget(my2dVidget);
//  setStatusTip("Mouse buttons: Right-Zoom, Middle-Pan");
//}
