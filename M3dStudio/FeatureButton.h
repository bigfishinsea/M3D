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
    QTimer* m_timer; //��Ӷ�ʱ�����������ֵ�����˫��
    int m_Clicked; //��־λ����¼����Ĵ���
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
    //��дmousePressEvent�¼�
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseDoubleClickEvent(QMouseEvent* event);

signals:
    //�Զ���clicked()�ź�,��mousePressEvent�¼�����ʱ����
    void clicked();

public slots:
    void slotClicked(); //�����ۺ���
    void OnvalueChange(QString& text);//���ݱ仯ʱ����Ӧ����
};