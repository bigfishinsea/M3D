// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#ifndef VIEW_H
#define VIEW_H

#include <functional>

#include <Standard_WarningsDisable.hxx>
#include <QAction>
#include <QList>
#include <QMenu>
#include <QToolBar>
#include <QWidget>
#include <Standard_WarningsRestore.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_ViewController.hxx>
#include <V3d_View.hxx>
#include <QPushButton>

class TopoDS_Shape;
class DocumentCommon;
class Command;

enum CurrentAction3d 
{ 
  CurrentAction3d_Nothing, 
  CurrentAction3d_DynamicZooming, 
  CurrentAction3d_WindowZooming,
  CurrentAction3d_DynamicPanning, 
  CurrentAction3d_GlobalPanning, 
  CurrentAction3d_DynamicRotation, 
  CurrentAction3d_ObjectDececting 
};
enum ViewAction 
{ 
  ViewAction_FitAll, 
  ViewAction_FitArea, 
  ViewAction_Zoom, 
  ViewAction_Pan, 
  ViewAction_GlobalPan, 
  ViewAction_Front, 
  ViewAction_Back, 
  ViewAction_Top, 
  ViewAction_Bottom,
  ViewAction_Left, 
  ViewAction_Right, 
  ViewAction_Axo, 
  ViewAction_Rotation, 
  ViewAction_Reset, 
  ViewAction_HlrOff, 
  ViewAction_HlrOn, 
  ViewAction_Shading, 
  ViewAction_Wireframe, 
  ViewAction_Transparency 
};
enum RaytraceAction 
{ 
  RaytraceAction_Raytracing, 
  RaytraceAction_Shadows, 
  RaytraceAction_Reflections, 
  RaytraceAction_Antialiasing 
};

//! Qt widget containing V3d_View and toolbar with view manipulation buttons.
//! Also use AIS_ViewController for redirecting user input (mouse, keyboard)
//! into 3D viewer events (rotation, panning, zooming)
class View: public QWidget, protected AIS_ViewController
{
  Q_OBJECT
public:
  View (const Handle(AIS_InteractiveContext)& theContext, bool theIs3dView, QWidget* theParent);

  ~View()
  {
    delete myBackMenu;
  }

  virtual void    init();
  QList<QAction*> getViewActions();
  QAction*        getViewAction(ViewAction theAction);
  QList<QAction*> getRaytraceActions();
  QAction*        getRaytraceAction(RaytraceAction theAction);

  void EnableRaytracing();
  void DisableRaytracing();

  void SetRaytracedShadows (bool theState);
  void SetRaytracedReflections (bool theState);
  void SetRaytracedAntialiasing (bool theState);

  bool IsRaytracingMode() const { return myIsRaytracing; }
  bool IsShadowsEnabled() const { return myIsShadowsEnabled; }
  bool IsReflectionsEnabled() const { return myIsReflectionsEnabled; }
  bool IsAntialiasingEnabled() const { return myIsAntialiasingEnabled; }

  //初始化装配命令
  void NewAssembleCommand();
  //初始化调姿命令
  void NewRepostureCommand();
  //初始化连接命令
  void NewConnectCommand();
  //初始化选择命令
  void NewSelectCommand();
  //消除命令
  void EndCommand();

  //提供接口
  Handle(AIS_InteractiveContext) GetContext() { return myContext; }
  DocumentCommon* GetDocument() { return m_pDoc; }


  static QString GetMessages(int type,TopAbs_ShapeEnum aSubShapeType, TopAbs_ShapeEnum aShapeType);
  static QString GetShapeType(TopAbs_ShapeEnum aShapeType);

  Standard_EXPORT static void OnButtonuseraction(int ExerciceSTEP, Handle(AIS_InteractiveContext)& );
  Standard_EXPORT static void DoSelection(int Id, Handle(AIS_InteractiveContext)& );
  Standard_EXPORT static void OnSetSelectionMode(Handle(AIS_InteractiveContext)&,
                                                  Standard_Integer&,
                                                  TopAbs_ShapeEnum& SelectionMode,
                                                  Standard_Boolean& );
  virtual QPaintEngine* paintEngine() const;
  const Handle(V3d_View)& getView() const { return myV3dView; }
signals:
  void selectionChanged();
  //更改任何命令状态信号
  void StateChanged(const QString& curr_state);
  //发送选择的内容
  void sendContext(const QString& text);
  //通知发送者内容改变,做出响应
  void sendertextChange(QString& text);

public slots:
  void fitAll();
  void axo();
  void hlrOn();
  void hlrOff();
  void shading();
  void wireframe();
  void onTransparency();

  void updateToggled( bool );
  void onBackground();
  void onEnvironmentMap();
  void onRaytraceAction();
  //接收传过来的选择内容
  void OnsendSelectContext(QString& text);
  //产生选择命令
  void createSelectCmd();

private slots:
    void onTransparencyChanged(int theVal);
    void HideSelected();
    void DeleteSelected();
    void ReplaceCyberInfo();

protected:
  virtual void paintEvent( QPaintEvent* ) Standard_OVERRIDE;
  virtual void resizeEvent( QResizeEvent* ) Standard_OVERRIDE;
  virtual void mousePressEvent( QMouseEvent* ) Standard_OVERRIDE;
  virtual void mouseReleaseEvent(QMouseEvent* ) Standard_OVERRIDE;
  virtual void mouseMoveEvent( QMouseEvent* ) Standard_OVERRIDE;
  virtual void wheelEvent(QWheelEvent* ) Standard_OVERRIDE; 

  virtual void addItemInPopup( QMenu* );

  Handle(AIS_InteractiveContext)& getContext() { return myContext; }

  void activateCursor( const CurrentAction3d );

  CurrentAction3d getCurrentMode() const { return myCurrentMode; }

private:
  void initCursors();
  void initViewActions();
  void initRaytraceActions();
  void DisplayGlobalCoordsys(double scale=1);
  QAction* RegisterAction(QString theIconPath, QString thePromt);

private:
  bool myIsRaytracing;
  bool myIsShadowsEnabled;
  bool myIsReflectionsEnabled;
  bool myIsAntialiasingEnabled;

  bool myIs3dView;

  Handle(V3d_View)                myV3dView;
  Handle(AIS_InteractiveContext)  myContext;
  AIS_MouseGestureMap             myDefaultGestures;
  Graphic3d_Vec2i                 myClickPos;

  //添加的命令类抽象基类
  Command* m_pCmd;
  //选择命令发过来的对象
  QPushButton* select_sender = nullptr;

  void updateView();

  //! Setup mouse gestures.
  void defineMouseGestures();

  //! Set current action.
  void setCurrentAction (CurrentAction3d theAction)
  {
    myCurrentMode = theAction;
    defineMouseGestures();
  }

  //! Handle selection changed event.
  void OnSelectionChanged(const Handle(AIS_InteractiveContext)& theCtx,
                          const Handle(V3d_View)& theView) Standard_OVERRIDE;
  CurrentAction3d                 myCurrentMode;
  Standard_Real                   myCurZoom;
  QMap<ViewAction, QAction*>      myViewActions;
  QMap<RaytraceAction, QAction*>  myRaytraceActions;
  QMenu*                          myBackMenu;
  QMenu* popMenu; //弹出菜单
  QToolBar*                       myViewBar;

  private:
      DocumentCommon* m_pDoc;

  public:
      void SetDocument(DocumentCommon* pDoc) { m_pDoc = pDoc; }

  //private: //测试
  //    Handle(AIS_InteractiveObject) aIO1;
  //    Handle(AIS_InteractiveContext) context1;
 };

#endif
