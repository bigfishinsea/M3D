//2021.09.16

#if !defined _WIN32
#define QT_CLEAN_NAMESPACE         /* avoid definition of INT32 and INT8 */
#endif

#include "DocumentModel.h"
#include "View.h"
#include "DocumentCommon.h"
//#include "DocumentModel.h"
#include "OcctWindow.h"
#include "Transparency.h"
#include "AssemblyCommand.h"
#include "RepostureCommand.h"
#include "ConnectCommand.h"
#include "SelectCommand.h"
#include "FeatureButton.h"

#include <Standard_WarningsDisable.hxx>
#include <QApplication>
#include <QBoxLayout>
#include <QFileInfo>
#include <QFileDialog>
#include <QMouseEvent>
#include <QMdiSubWindow>
#include <QColorDialog>
#include <QCursor>
#include <QPainter>
#include <QStyleFactory>
#include <Standard_WarningsRestore.hxx>
#include <SelectMgr_EntityOwner.hxx>
#include <StdSelect_BRepOwner.hxx>
#include <AIS_Shape.hxx>
#include <TopoDS_Shape.hxx>
#include <QToolTip.h>
#include "AdaptorVec_AIS.h"
//#include "M3dCom_DS.h"
//#include "M3dMdl_DS.h"
#include <BRepPrimAPI_MakeBox.hxx>


#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
#include <QX11Info>
#endif

#include <Aspect_DisplayConnection.hxx>
#include <Graphic3d_GraphicDriver.hxx>
#include <Graphic3d_TextureEnv.hxx>

namespace
{
	//! Map Qt buttons bitmask to virtual keys.
	static Aspect_VKeyMouse qtMouseButtons2VKeys(Qt::MouseButtons theButtons)
	{
		Aspect_VKeyMouse aButtons = Aspect_VKeyMouse_NONE;
		if ((theButtons & Qt::LeftButton) != 0)
		{
			aButtons |= Aspect_VKeyMouse_LeftButton;
		}
		if ((theButtons & Qt::MiddleButton) != 0)
		{
			aButtons |= Aspect_VKeyMouse_MiddleButton;
		}
		if ((theButtons & Qt::RightButton) != 0)
		{
			aButtons |= Aspect_VKeyMouse_RightButton;
		}
		return aButtons;
	}

	//! Map Qt mouse modifiers bitmask to virtual keys.
	static Aspect_VKeyFlags qtMouseModifiers2VKeys(Qt::KeyboardModifiers theModifiers)
	{
		Aspect_VKeyFlags aFlags = Aspect_VKeyFlags_NONE;
		if ((theModifiers & Qt::ShiftModifier) != 0)
		{
			aFlags |= Aspect_VKeyFlags_SHIFT;
		}
		if ((theModifiers & Qt::ControlModifier) != 0)
		{
			aFlags |= Aspect_VKeyFlags_CTRL;
		}
		if ((theModifiers & Qt::AltModifier) != 0)
		{
			aFlags |= Aspect_VKeyFlags_ALT;
		}
		return aFlags;
	}

	static QCursor* defCursor = NULL;
	static QCursor* handCursor = NULL;
	static QCursor* panCursor = NULL;
	static QCursor* globPanCursor = NULL;
	static QCursor* zoomCursor = NULL;
	static QCursor* rotCursor = NULL;

}

View::View(const Handle(AIS_InteractiveContext)& theContext, bool theIs3dView, QWidget* theParent)
	: QWidget(theParent),
	myIsRaytracing(false),
	myIsShadowsEnabled(true),
	myIsReflectionsEnabled(false),
	myIsAntialiasingEnabled(false),
	myIs3dView(theIs3dView),
	myBackMenu(NULL),
	m_pCmd(nullptr)
	
{
#if !defined(_WIN32) && (!defined(__APPLE__) || defined(MACOSX_USE_GLX)) && QT_VERSION < 0x050000
	XSynchronize(x11Info().display(), true);
#endif
	myContext = theContext;
	myCurZoom = 0;

	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);

	myDefaultGestures = myMouseGestureMap;
	myCurrentMode = CurrentAction3d_Nothing;
	setMouseTracking(true);

	initViewActions();
	initCursors();

	setBackgroundRole(QPalette::NoRole);//NoBackground );
	// set focus policy to threat QContextMenuEvent from keyboard
	setFocusPolicy(Qt::StrongFocus);
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	init();
}

void View::NewAssembleCommand()
{
	//如果之前有命令，先清除
	if (m_pCmd)
	{
		EndCommand();
	}
	m_pCmd = new AssemblyCommand(this);
}

void View::NewRepostureCommand()
{
	//如果之前有命令，先清除
	if (m_pCmd)
	{
		EndCommand();
	}
	m_pCmd = new RepostureCommand(this);
}

void View::NewConnectCommand()
{
	//如果之前有命令，先清除
	if (m_pCmd)
	{
		EndCommand();
	}
	m_pCmd = new ConnectCommand(this);
}

void View::NewSelectCommand()
{
	//如果之前有命令，先清除
	if (m_pCmd)
	{
		EndCommand();
	}
	m_pCmd = new SelectCommand(this);
}

void View::EndCommand()
{
	delete m_pCmd;
	m_pCmd = nullptr;
}

void View::init()
{
	m_pDoc = nullptr;

	if (myV3dView.IsNull())
	{
		myV3dView = myContext->CurrentViewer()->CreateView();
	}

	Handle(OcctWindow) hWnd = new OcctWindow(this);
	myV3dView->SetWindow(hWnd);
	//显示纹理
	//myV3dView->SetSurfaceDetail(V3d_TEX_ALL);

	if (!hWnd->IsMapped())
	{
		hWnd->Map();
	}

	if (myIs3dView)
	{
		SetAllowRotation(Standard_True);
		myV3dView->SetBackgroundColor(Quantity_Color(0.0, 0.0, 0.3, Quantity_TOC_RGB));
		//显示坐标系框架
		//myV3dView->TriedronDisplay();
	}
	else
	{
		SetAllowRotation(Standard_False);
		myV3dView->SetBackgroundColor(Quantity_Color(0.0, 0.2, 0.0, Quantity_TOC_RGB));
		myV3dView->SetProj(V3d_Zpos);
	}

	myV3dView->MustBeResized();

	if (myIsRaytracing)
	{
		myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;
	}

	popMenu = new QMenu(this);
	QAction* hideAct = new QAction(tr("隐藏"), this);
	QAction* reposAct = new QAction(tr("调姿"), this);
	QAction* delAct = new QAction(tr("删除"), this);
	QAction* replaceAct = new QAction(tr("更改关联..."), this);
	//为添加关联函数
	connect(hideAct, &QAction::triggered, this, &View::HideSelected);
	connect(delAct, &QAction::triggered, this, &View::DeleteSelected);
	connect(reposAct, &QAction::triggered, this, &View::NewRepostureCommand);
	connect(replaceAct, &QAction::triggered, this, &View::ReplaceCyberInfo);
	popMenu->addAction(hideAct);
	popMenu->addAction(delAct);
	popMenu->addAction(reposAct);
	popMenu->addAction(replaceAct);
	replaceAct->setEnabled(false);

	//Handle(V3d_Viewer) myViewer = myContext->CurrentViewer();
	//BRepPrimAPI_MakeBox mBox1(gp_Pnt(0, 0, 0), 20, 50, 20);
	//TopoDS_Shape reShape1 = mBox1.Shape();
	//aIO1 = new AIS_Shape(reShape1);
	//context1 = new AIS_InteractiveContext(myViewer);
	//context1->Display(aIO1, true);
	//context1->SetDisplayMode(AIS_Shaded, Standard_True);
	//context1->SetAutoActivateSelection(true);
	////context1->UpdateCurrentViewer();

	//BRepPrimAPI_MakeBox mBox2(gp_Pnt(10, 0, 0), 20, 50, 20);
	//TopoDS_Shape reShape2 = mBox2.Shape();
	//Handle(AIS_InteractiveObject) aIO = new AIS_Shape(reShape2);
	//Handle(AIS_InteractiveContext) context2 = new AIS_InteractiveContext(myViewer);
	//context2->Display(aIO, true);
	////context2->UpdateCurrentViewer();
}

void View::DisplayGlobalCoordsys(double scale)
{
	//Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
	//myContext->Display(aViewCube, true);

	gp_Ax3 Ax3;
	double theLength=3, arrowLength=1;
	//GetAx3Length(theLength, arrowLength);

	theLength = theLength / scale;
	arrowLength = theLength / scale;

	Handle(AdaptorVec_AIS) z_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.Direction(), theLength, arrowLength);
	z_Axis->SetText("  Z");
	//Handle_Prs3d_LineAspect lineAspect = new Prs3d_LineAspect(Quantity_NOC_RED, Aspect_TOL_SOLID, 2);
	//z_Axis->SetAspect(lineAspect);
	myContext->Display(z_Axis, Standard_True);
	Handle(AdaptorVec_AIS) x_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.XDirection(), theLength, arrowLength);
	x_Axis->SetText("  X");
	//Handle_Prs3d_LineAspect lineAspect2 = new Prs3d_LineAspect(Quantity_NOC_GREEN, Aspect_TOL_SOLID, 2);
	//x_Axis->SetAspect(lineAspect2);
	myContext->Display(x_Axis, Standard_False);
	Handle(AdaptorVec_AIS) y_Axis = new AdaptorVec_AIS(Ax3.Location(), Ax3.YDirection(), theLength, arrowLength);
	y_Axis->SetText("  Y");
	//Handle_Prs3d_LineAspect lineAspect3 = new Prs3d_LineAspect(Quantity_NOC_BLUE, Aspect_TOL_SOLID, 2);
	//y_Axis->SetAspect(lineAspect3);
	myContext->Display(y_Axis, Standard_False);
}

void View::paintEvent(QPaintEvent*)
{
	myV3dView->InvalidateImmediate();
	
	//DisplayGlobalCoordsys(myV3dView->Scale());
	FlushViewEvents(myContext, myV3dView, true);
}

void View::resizeEvent(QResizeEvent*)
{
	if (!myV3dView.IsNull())
	{
		myV3dView->MustBeResized();
	}
}

void View::OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx,
	const Handle(V3d_View)& theView)
{
	Q_UNUSED(theCtx)
		Q_UNUSED(theView)
}

void View::fitAll()
{
	myV3dView->FitAll();
	myV3dView->ZFitAll();
	myV3dView->Redraw();
}

void View::axo()
{
	if (myIs3dView)
	{
		myV3dView->SetProj(V3d_XposYnegZpos);
	}
}

void View::hlrOff()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	myV3dView->SetComputedMode(Standard_False);
	myV3dView->Redraw();
	QAction* aShadingAction = getViewAction(ViewAction_Shading);
	aShadingAction->setEnabled(true);
	QAction* aWireframeAction = getViewAction(ViewAction_Wireframe);
	aWireframeAction->setEnabled(true);
	QApplication::restoreOverrideCursor();
}

void View::hlrOn()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	myV3dView->SetComputedMode(Standard_True);
	myV3dView->Redraw();
	QAction* aShadingAction = getViewAction(ViewAction_Shading);
	aShadingAction->setEnabled(false);
	QAction* aWireframeAction = getViewAction(ViewAction_Wireframe);
	aWireframeAction->setEnabled(false);
	QApplication::restoreOverrideCursor();
}

void View::shading()
{
	myContext->SetDisplayMode(1, Standard_True);
}

void View::wireframe()
{
	myContext->SetDisplayMode(0, Standard_True);
}

void View::SetRaytracedShadows(bool theState)
{
	myV3dView->ChangeRenderingParams().IsShadowEnabled = theState;
	myIsShadowsEnabled = theState;
	myContext->UpdateCurrentViewer();
}

void View::SetRaytracedReflections(bool theState)
{
	myV3dView->ChangeRenderingParams().IsReflectionEnabled = theState;
	myIsReflectionsEnabled = theState;
	myContext->UpdateCurrentViewer();
}

void View::onRaytraceAction()
{
	QAction* aSentBy = (QAction*)sender();

	if (aSentBy == myRaytraceActions.value(RaytraceAction_Raytracing))
	{
		bool aState = myRaytraceActions.value(RaytraceAction_Raytracing)->isChecked();

		QApplication::setOverrideCursor(Qt::WaitCursor);
		if (aState)
			EnableRaytracing();
		else
			DisableRaytracing();
		QApplication::restoreOverrideCursor();
	}

	if (aSentBy == myRaytraceActions.value(RaytraceAction_Shadows))
	{
		bool aState = myRaytraceActions.value(RaytraceAction_Shadows)->isChecked();
		SetRaytracedShadows(aState);
	}

	if (aSentBy == myRaytraceActions.value(RaytraceAction_Reflections))
	{
		bool aState = myRaytraceActions.value(RaytraceAction_Reflections)->isChecked();
		SetRaytracedReflections(aState);
	}

	if (aSentBy == myRaytraceActions.value(RaytraceAction_Antialiasing))
	{
		bool aState = myRaytraceActions.value(RaytraceAction_Antialiasing)->isChecked();
		SetRaytracedAntialiasing(aState);
	}
}

void View::OnsendSelectContext(QString& text)
{
	//select_sender->setText(text);
	connect(this, &View::sendertextChange, dynamic_cast<FeatureButton*>(select_sender), &FeatureButton::OnvalueChange);
	emit sendertextChange(text);
	disconnect(this, &View::sendertextChange, dynamic_cast<FeatureButton*>(select_sender), &FeatureButton::OnvalueChange);
}

void View::createSelectCmd()
{
	select_sender = dynamic_cast<QPushButton*>(this->sender());
	this->NewSelectCommand();
}

//反走样：改为隐藏/显示全部
void View::SetRaytracedAntialiasing(bool theState)
{
	//myV3dView->ChangeRenderingParams().IsAntialiasingEnabled = theState;
	//myIsAntialiasingEnabled = theState;
	
	if (myContext->NbSelected() > 0/*theState*/) //隐藏
	{
		//得到选中的组件
		myContext->EraseSelected(true);
	}
	else { //显示全部
		myContext->DisplayAll(Standard_True);
	}
	myContext->UpdateCurrentViewer();
}

void View::EnableRaytracing()
{
	if (!myIsRaytracing)
	{
		myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RAYTRACING;
	}
	myIsRaytracing = true;
	myContext->UpdateCurrentViewer();
}

void View::DisableRaytracing()
{
	if (myIsRaytracing)
	{
		myV3dView->ChangeRenderingParams().Method = Graphic3d_RM_RASTERIZATION;
	}
	myIsRaytracing = false;
	myContext->UpdateCurrentViewer();
}

void View::updateToggled(bool isOn)
{
	QAction* sentBy = (QAction*)sender();
	if (!isOn)
	{
		return;
	}

	foreach(QAction * anAction, myViewActions)
	{
		if (anAction && (anAction != sentBy))
		{
			anAction->setCheckable(true);
			anAction->setChecked(false);
		}
		else
		{
			if (sentBy == myViewActions.value(ViewAction_FitArea))
				setCursor(*handCursor);
			else if (sentBy == myViewActions.value(ViewAction_Zoom))
				setCursor(*zoomCursor);
			else if (sentBy == myViewActions.value(ViewAction_Pan))
				setCursor(*panCursor);
			else if (sentBy == myViewActions.value(ViewAction_GlobalPan))
				setCursor(*globPanCursor);
			else if (sentBy == myViewActions.value(ViewAction_Rotation))
				setCursor(*rotCursor);
			else
				setCursor(*defCursor);

			sentBy->setCheckable(false);
		}
	}
}

void View::initCursors()
{
	if (!defCursor)
		defCursor = new QCursor(Qt::ArrowCursor);
	if (!handCursor)
		handCursor = new QCursor(Qt::PointingHandCursor);
	if (!panCursor)
		panCursor = new QCursor(Qt::SizeAllCursor);
	if (!globPanCursor)
		globPanCursor = new QCursor(Qt::CrossCursor);
	if (!zoomCursor)
		zoomCursor = new QCursor(QPixmap("images/cursor_zoom.png"));
	if (!rotCursor)
		rotCursor = new QCursor(QPixmap("images/cursor_rotate.png"));
}

QList<QAction*> View::getViewActions()
{
	initViewActions();
	return myViewActions.values();
}

QList<QAction*>  View::getRaytraceActions()
{
	initRaytraceActions();
	return myRaytraceActions.values();
}

QAction* View::getViewAction(ViewAction theAction)
{
	return myViewActions.value(theAction);
}

QAction* View::getRaytraceAction(RaytraceAction theAction)
{
	return myRaytraceActions.value(theAction);
}

/*!
  Get paint engine for the OpenGL viewer. [ virtual public ]
*/
QPaintEngine* View::paintEngine() const
{
	return 0;
}

QAction* View::RegisterAction(QString theIconPath, QString thePromt)
{
	QAction* anAction = new QAction(QPixmap(theIconPath), thePromt, this);
	anAction->setToolTip(thePromt);
	anAction->setStatusTip(thePromt);
	return anAction;
}

void View::initViewActions()
{
	if (!myViewActions.empty())
		return;
	myViewActions[ViewAction_FitAll] = RegisterAction("images/view_fitall.png", tr("Fit all"));
	connect(myViewActions[ViewAction_FitAll], SIGNAL(triggered()), this, SLOT(fitAll()));
	if (myIs3dView)
	{
		myViewActions[ViewAction_Axo] = RegisterAction("images/view_axo.png", tr("Isometric"));
		connect(myViewActions[ViewAction_Axo], SIGNAL(triggered()), this, SLOT(axo()));

		QActionGroup* aShadingActionGroup = new QActionGroup(this);
		QAction* aShadingAction = RegisterAction("images/tool_shading.png", tr("Shading"));
		connect(aShadingAction, SIGNAL(triggered()), this, SLOT(shading()));
		aShadingAction->setCheckable(true);
		aShadingActionGroup->addAction(aShadingAction);
		myViewActions[ViewAction_Shading] = aShadingAction;

		QAction* aWireframeAction = RegisterAction("images/tool_wireframe.png", tr("Wireframe"));
		connect(aWireframeAction, SIGNAL(triggered()), this, SLOT(wireframe()));
		aWireframeAction->setCheckable(true);
		aShadingActionGroup->addAction(aWireframeAction);
		myViewActions[ViewAction_Wireframe] = aWireframeAction;

		QActionGroup* aHlrActionGroup = new QActionGroup(this);
		QAction* aHlrOffAction = RegisterAction("images/view_comp_off.png", tr("HLR off"));
		connect(aHlrOffAction, SIGNAL(triggered()), this, SLOT(hlrOff()));
		aHlrOffAction->setCheckable(true);
		aHlrActionGroup->addAction(aHlrOffAction);
		myViewActions[ViewAction_HlrOff] = aHlrOffAction;

		QAction* aHlrOnAction = RegisterAction("images/view_comp_on.png", tr("HLR on"));
		connect(aHlrOnAction, SIGNAL(triggered()), this, SLOT(hlrOn()));
		aHlrOnAction->setCheckable(true);
		aHlrActionGroup->addAction(aHlrOnAction);
		myViewActions[ViewAction_HlrOn] = aHlrOnAction;

		QAction* aTransparencyAction = RegisterAction("images/tool_transparency.png", tr("Transparency"));
		connect(aTransparencyAction, SIGNAL(triggered()), this, SLOT(onTransparency()));
		myViewActions[ViewAction_Transparency] = aTransparencyAction;
	}
}

void View::initRaytraceActions()
{
	if (!myRaytraceActions.empty())
	{
		return;
	}

	QAction* aRayTraceAction = RegisterAction("images/raytracing.png", tr("Ray-tracing"));
	connect(aRayTraceAction, SIGNAL(triggered()), this, SLOT(onRaytraceAction()));
	myRaytraceActions[RaytraceAction_Raytracing] = aRayTraceAction;
	aRayTraceAction->setCheckable(true);
	aRayTraceAction->setChecked(false);

	QAction* aShadowAction = RegisterAction("images/shadows.png", tr("Shadows"));
	connect(aShadowAction, SIGNAL(triggered()), this, SLOT(onRaytraceAction()));
	myRaytraceActions[RaytraceAction_Shadows] = aShadowAction;
	aShadowAction->setCheckable(true);
	aShadowAction->setChecked(true);

	QAction* aReflectAction = RegisterAction("images/reflections.png", tr("Reflections"));
	connect(aReflectAction, SIGNAL(triggered()), this, SLOT(onRaytraceAction()));
	myRaytraceActions[RaytraceAction_Reflections] = aReflectAction;
	aReflectAction->setCheckable(true);
	aReflectAction->setChecked(false);

	QAction* anAntiAliasingAction = RegisterAction("images/antialiasing.png", tr("Anti-aliasing"));
	connect(anAntiAliasingAction, SIGNAL(triggered()), this, SLOT(onRaytraceAction()));
	myRaytraceActions[RaytraceAction_Antialiasing] = anAntiAliasingAction;
	anAntiAliasingAction->setCheckable(true);
	anAntiAliasingAction->setChecked(false);
}

void View::activateCursor(const CurrentAction3d theMode)
{
	QCursor* aCursor = defCursor;
	switch (theMode)
	{
	case CurrentAction3d_DynamicPanning:  aCursor = panCursor; break;
	case CurrentAction3d_DynamicZooming:  aCursor = zoomCursor; break;
	case CurrentAction3d_DynamicRotation: aCursor = rotCursor; break;
	case CurrentAction3d_GlobalPanning:   aCursor = globPanCursor; break;
	case CurrentAction3d_WindowZooming:   aCursor = handCursor; break;
	case CurrentAction3d_Nothing:         aCursor = defCursor; break;
	default:
		break;
	}
	setCursor(*aCursor);
}

void View::mousePressEvent(QMouseEvent* theEvent)
{
	Qt::MouseButtons aMouseButtons = theEvent->buttons();
	const Graphic3d_Vec2i aPnt(theEvent->pos().x(), theEvent->pos().y());
	const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(theEvent->modifiers());
	if (!myV3dView.IsNull()
		&& UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(aMouseButtons), aFlags, false))
	{
		updateView();
	}
	myClickPos = aPnt;

	//添加此函数否则选中实体出现问题
	//FlushViewEvents(myContext, myV3dView, Standard_True);
	if (theEvent->button() == Qt::LeftButton)
	{
		if (m_pCmd)
		{
			m_pCmd->setCoordinate(theEvent->pos().x(), theEvent->pos().y());
			m_pCmd->OnLeftButtonDown();
		}
	}
	//右键取消命令
	if (theEvent->button() == Qt::RightButton)
	{
		if (m_pCmd)
		{
			m_pCmd->OnRightButtonDown();
			delete m_pCmd;
			m_pCmd = nullptr;

			this->StateChanged(QString("操作取消"));

		}
		else {
			;
		}
	}
}

void View::mouseReleaseEvent(QMouseEvent* theEvent)
{
	Qt::MouseButtons aMouseButtons = theEvent->buttons();
	const Graphic3d_Vec2i aPnt(theEvent->pos().x(), theEvent->pos().y());
	const Aspect_VKeyFlags aFlags = qtMouseModifiers2VKeys(theEvent->modifiers());
	if (!myV3dView.IsNull()
		&& UpdateMouseButtons(aPnt, qtMouseButtons2VKeys(aMouseButtons), aFlags, false))
	{
		updateView();
	}

	if (myCurrentMode == CurrentAction3d_GlobalPanning)
	{
		myV3dView->Place(aPnt.x(), aPnt.y(), myCurZoom);
	}
	if (myCurrentMode != CurrentAction3d_Nothing)
	{
		setCurrentAction(CurrentAction3d_Nothing);
	}

	//添加此函数否则选中实体出现问题
	FlushViewEvents(myContext, myV3dView, Standard_True);
	if (theEvent->button() == Qt::LeftButton)
	{
		if (m_pCmd)
		{
			m_pCmd->setCoordinate(theEvent->pos().x(), theEvent->pos().y());
			m_pCmd->OnLeftButtonRelease();
		}
	}
	//处理选中的组件item亮显
	if (m_pDoc->IsModel())
	{
		DocumentModel* pMdlDoc = (DocumentModel*)m_pDoc;
		pMdlDoc->HighLightSelectedComponent();
	}

	if (theEvent->button() == Qt::RightButton)
	{
		//弹出右键菜单
		myContext->InitSelected();
		if (myContext->HasSelectedShape() && myContext->NbSelected() == 1)
		{
			//
			popMenu->popup(theEvent->globalPos());
		}
	}
}

void View::mouseMoveEvent(QMouseEvent* theEvent)
{
	static QString sTiptext;

	//QMutex* pMutex = m_pDoc->GetMutex();
	//pMutex->lock();

	Qt::MouseButtons aMouseButtons = theEvent->buttons();
	const Graphic3d_Vec2i aNewPos(theEvent->pos().x(), theEvent->pos().y());
	if (!myV3dView.IsNull()
	  && UpdateMousePosition(aNewPos, qtMouseButtons2VKeys(aMouseButtons), qtMouseModifiers2VKeys(theEvent->modifiers()), false))
	{
		;// updateView();
	}

	//myContext->MoveTo(theEvent->pos().x(), theEvent->pos().y(), myV3dView, true);
	TopoDS_Shape shp;// = myContext->DetectedCurrentShape();
	//Handle(AIS_InteractiveObject) obj = myContext->DetectedCurrentObject();
	//Handle(AIS_InteractiveObject) obj1 = myContext->DetectedInteractive();
	//TopoDS_Shape shp = myContext->DetectedShape();
	if (myContext->HasDetected())
	{
		Handle(SelectMgr_EntityOwner) anOwner = myContext->DetectedOwner();
		Handle(AIS_InteractiveObject) anObj = Handle(AIS_InteractiveObject)::DownCast(anOwner->Selectable());
		if (Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast(anOwner))
		{
			shp = aBRepOwner->Shape();
		}

		Handle(AIS_Shape) S = Handle(AIS_Shape)::DownCast(anObj);
		if (!shp.IsNull() && m_pDoc)//(!S.IsNull())
		{
			//TopoDS_Shape shp = S->Shape();
			QString tipStr =  m_pDoc->GetShapeName(shp).c_str();
			if (sTiptext != tipStr)
			{
				QToolTip::showText(theEvent->globalPos(), tipStr);
				sTiptext = tipStr;
			}
		}
		else {
			QToolTip::hideText();
			sTiptext = "";
		}
	}
	else {
		QToolTip::hideText();
		sTiptext = "";
	}

	//添加此函数否则选中实体出现问题

	FlushViewEvents(myContext, myV3dView, Standard_True);
	if (m_pCmd)
	{
		//m_pCmd->setCoordinate(theEvent->pos().x(), theEvent->pos().y());
		m_pCmd->OnMouseMove();
	}

	updateView();

	//pMutex->unlock();
}

//==============================================================================
//function : wheelEvent
//purpose  :
//==============================================================================
void View::wheelEvent(QWheelEvent* theEvent)
{
	const Graphic3d_Vec2i aPos(theEvent->pos().x(), theEvent->pos().y());
	if (!myV3dView.IsNull()
		&& UpdateZoom(Aspect_ScrollDelta(aPos, theEvent->delta() / 8)))
	{
		updateView();
	}
}

// =======================================================================
// function : updateView
// purpose  :
// =======================================================================
void View::updateView()
{
	update();

	//if (context1)
	//{
	//	context1->Display(aIO1, true);
	//	context1->SetDisplayMode(AIS_Shaded, Standard_True);
	//	context1->SetAutoActivateSelection(true);
	//}
}

void View::defineMouseGestures()
{
	myMouseGestureMap.Clear();
	AIS_MouseGesture aRot = AIS_MouseGesture_RotateOrbit;
	activateCursor(myCurrentMode);
	switch (myCurrentMode)
	{
	case CurrentAction3d_Nothing:
	{
		myMouseGestureMap = myDefaultGestures;
		break;
	}
	case CurrentAction3d_DynamicZooming:
	{
		myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Zoom);
		break;
	}
	case CurrentAction3d_GlobalPanning:
	{
		break;
	}
	case CurrentAction3d_WindowZooming:
	{
		myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_ZoomWindow);
		break;
	}
	case CurrentAction3d_DynamicPanning:
	{
		myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, AIS_MouseGesture_Pan);
		break;
	}
	case CurrentAction3d_DynamicRotation:
	{
		myMouseGestureMap.Bind(Aspect_VKeyMouse_LeftButton, aRot);
		break;
	}
	default:
	{
		break;
	}
	}
}

void View::addItemInPopup(QMenu* theMenu)
{
	Q_UNUSED(theMenu)
}

void View::onBackground()
{
	QColor aColor;
	Standard_Real R1;
	Standard_Real G1;
	Standard_Real B1;
	myV3dView->BackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
	aColor.setRgb((Standard_Integer)(R1 * 255), (Standard_Integer)(G1 * 255), (Standard_Integer)(B1 * 255));

	QColor aRetColor = QColorDialog::getColor(aColor);
	if (aRetColor.isValid())
	{
		R1 = aRetColor.red() / 255.;
		G1 = aRetColor.green() / 255.;
		B1 = aRetColor.blue() / 255.;
		myV3dView->SetBackgroundColor(Quantity_TOC_RGB, R1, G1, B1);
	}
	myV3dView->SetBackgroundImage(Handle(Graphic3d_Texture2D)());
	myV3dView->Redraw();
}

void View::onEnvironmentMap()
{
	if (1)//myBackMenu->actions().at(1)->isChecked())
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "",
			tr("All Image Files (*.bmp *.gif *.jpg *.jpeg *.png *.tga)"));

		const TCollection_AsciiString anUtf8Path(fileName.toStdU16String().data());
		//Handle(Graphic3d_TextureEnv) aTexture = new Graphic3d_TextureEnv(anUtf8Path);
		//myV3dView->SetTextureEnv(aTexture);
		myV3dView->SetBackgroundImage(fileName.toStdString().c_str(), Aspect_FM_STRETCH, true);
	}
	else
	{
		myV3dView->SetTextureEnv(Handle(Graphic3d_TextureEnv)());
	}

	myV3dView->Redraw();
}

void View::onTransparency()
{
	AIS_ListOfInteractive anAisObjectsList;
	myContext->DisplayedObjects(anAisObjectsList);
	if (anAisObjectsList.Extent() == 0)
	{
		return;
	}

	double aTranspValue = anAisObjectsList.First()->Transparency();
	DialogTransparency aDlg(this);
	aDlg.setValue(int(aTranspValue * 10));
	connect(&aDlg, SIGNAL(sendTransparencyChanged(int)), SLOT(onTransparencyChanged(int)));
	aDlg.exec();
}

void View::onTransparencyChanged(int theVal)
{
	AIS_ListOfInteractive anAisObjectsList;
	myContext->DisplayedObjects(anAisObjectsList);
	double aTranspValue = theVal / 10.;
	for (AIS_ListOfInteractive::Iterator anIter(anAisObjectsList);
		anIter.More(); anIter.Next())
	{
		const Handle(AIS_InteractiveObject)& anAisObject = anIter.Value();
		myContext->SetTransparency(anAisObject, aTranspValue, Standard_False);
	}
	myContext->UpdateCurrentViewer();
}

void View::DeleteSelected()
{
	if (m_pDoc)
	{
		m_pDoc->DeleteSelected();
	}
}

void View::HideSelected()
{
	if (m_pDoc)
	{
		m_pDoc->HideSelected();
	}
}

void View::ReplaceCyberInfo()
{
	if (m_pDoc && m_pDoc->IsModel())
	{
		((DocumentModel*)m_pDoc)->ReplaceCyberInfo();
	}
}
