#pragma once
#include <QMouseEvent>
#include <qpushbutton.h>
#include <QTimer.h>
#include <TopoDS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>

class DocumentComponent;

class FeatureButton : public QPushButton
{
    Q_OBJECT
private:
    QTimer* m_timer; //添加定时器，用来区分单击和双击
    int m_Clicked; //标志位，记录点击的次数
    DocumentComponent* myDocument3d;
    TopoDS_Shape shpSel;
    //M3dCom_DS* pCom;
    std::string sType;
    TopAbs_ShapeEnum  fType;

    Handle(AIS_InteractiveObject) myAISObj;

public:
    explicit FeatureButton(QWidget* parent = 0);
    void setFeatureType(TopAbs_ShapeEnum type);
    TopAbs_ShapeEnum getFeatureType();
    TopoDS_Shape getFeatureShape();
    std::string getFeatureName();
    void initFeatureButton();

protected:
    //重写mousePressEvent事件
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

signals:
    //自定义clicked()信号,在mousePressEvent事件发生时触发
    void clicked();

public slots:
    void slotClicked(); //单击槽函数
    void OnvalueChange(QString& text);//内容变化时的响应函数
};