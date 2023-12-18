#include "M3d.h"
#include "GeomWidget.h"
#include "DatumPointDlg.h"
#include "DatumLineDlg.h"
#include "ConnectorDlg.h"
#include <QMessageBox>
#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>

FeatureButton::FeatureButton(QWidget* parent) :
	QPushButton(parent)
{
	if (typeid(*parent) == typeid(DatumPointDlg))
	{
		myDocument3d = qobject_cast<DatumPointDlg*>(parent)->getDocumentFromDlg();
	}
	else if (typeid(*parent) == typeid(DatumLineDlg))
	{
		myDocument3d = qobject_cast<DatumLineDlg*>(parent)->getDocumentFromDlg();
	}
	else if (typeid(*parent) == typeid(ConnectorDlg))
	{
		myDocument3d = (DocumentComponent *) qobject_cast<ConnectorDlg*>(parent)->getDocumentFromDlg();
	}
	m_timer = new QTimer(this);
	setFeatureType(TopAbs_ShapeEnum::TopAbs_VERTEX);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(slotClicked()));
}

void FeatureButton::setFeatureType(TopAbs_ShapeEnum type)
{
	fType = type;
}

TopAbs_ShapeEnum FeatureButton::getFeatureType()
{
	return fType;
}

TopoDS_Shape FeatureButton::getFeatureShape()
{
	return shpSel;
}

std::string FeatureButton::getFeatureName()
{
	return sType;
}

void FeatureButton::initFeatureButton()
{
	shpSel.Nullify();
	sType.clear();
	this->setText(nullptr);
}

void FeatureButton::mouseReleaseEvent(QMouseEvent* event)
{
	if (!myAISObj.IsNull())
	{
		Handle(AIS_InteractiveContext) contx = myDocument3d->getContext();
		contx->Erase(myAISObj, true);
	}
	emit FeatureButton::clicked();
	QPushButton::mouseReleaseEvent(event);
}

// 重写mousePressEvent事件,检测事件类型是不是点击了鼠标左键或右键
void FeatureButton::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_timer->start(300);

	}
	if (event->button() == Qt::RightButton)
	{
		if (this->text() != nullptr)
		{
			this->setText(nullptr);
		}
		return;
	}
	// 将该事件传给父类处理
	QPushButton::mousePressEvent(event);
}

void FeatureButton::mouseDoubleClickEvent(QMouseEvent* event)
{
	m_timer->stop();
	Handle(AIS_InteractiveContext) pContext = myDocument3d->getContext();
	pContext->InitSelected();
	//Current = pContext->SelectedInteractive();
	shpSel = pContext->SelectedShape();
	if (shpSel.IsNull())
	{
		QMessageBox::information(NULL, tr("提示"), tr("所选特征为空"), QMessageBox::Ok);
		return;
	}
	//pCom = myDocument3d->GetComDS();
	sType = myDocument3d->GetShapeName(shpSel);
	//TopAbs_ShapeEnum nn = shpSel.ShapeType();

	if (shpSel.ShapeType() == fType)
	{
		this->setText(QString::fromStdString(sType));
	}
	else
	{
		QMessageBox::information(NULL, tr("提示"), tr("所选特征类型错误"), QMessageBox::Ok);
		return;
	}
}

void FeatureButton::OnvalueChange(QString& text)
{
	this->setText(text);
	shpSel = myDocument3d->GetShapeFromName(text.toStdString());
	if (shpSel.IsNull())
	{
		QMessageBox::information(NULL, tr("提示"), tr("所选特征为空"), QMessageBox::Ok);
		return;
	}
	//pCom = myDocument3d->GetComDS();
	sType = text.toStdString();
	//TopAbs_ShapeEnum nn = shpSel.ShapeType();

	if (shpSel.ShapeType() == fType)
	{
		this->setText(QString::fromStdString(sType));
	}
	else
	{
		QMessageBox::information(NULL, tr("提示"), tr("所选特征类型错误"), QMessageBox::Ok);
		return;
	}
}

void FeatureButton::slotClicked()
{
	m_timer->stop();
	
	if (shpSel.IsNull())
	{
		//设置选择模式
		if (fType == TopAbs_VERTEX)
		{
			myDocument3d->setSelectMode(tr("顶点"));
		}
		else if (fType == TopAbs_EDGE)
		{
			myDocument3d->setSelectMode(tr("棱边"));
		}
		else if (fType == TopAbs_FACE)
		{
			myDocument3d->setSelectMode(tr("面"));
		}
		else if (fType == TopAbs_SOLID)
		{
			myDocument3d->setSelectMode(tr("体"));
		}
	}
	else {
		//显示选中实体
		myAISObj = new AIS_Shape(shpSel);
		Handle(AIS_InteractiveContext) pContext = myDocument3d->getContext();
		pContext->SetColor(myAISObj, Quantity_NOC_HOTPINK, Standard_False);
		//myAISObj->SetWidth(5);
		pContext->Display(myAISObj, Standard_True);
	}

	/*Handle(AIS_InteractiveContext) pContext = myDocument3d->getContext();
	Handle(AIS_InteractiveObject) Current;
	pContext->InitSelected();
	Current = pContext->SelectedInteractive();*/

	//pContext->HilightSelected(true);
	//pContext->HilightSelected(Current->GlobalSelOwner());
			// 设置模型高亮的风格
	//Handle(Prs3d_Drawer) t_hilight_style = pContext->HighlightStyle(); // 获取高亮风格
	//t_hilight_style->SetMethod(Aspect_TOHM_COLOR);  // 颜色显示方式
	//t_hilight_style->SetColor(Quantity_NOC_LIGHTYELLOW);    // 设置高亮颜色
	//t_hilight_style->SetDisplayMode(1); // 整体高亮
	//t_hilight_style->SetTransparency(0.2f); // 设置透明度

	/*Handle(AIS_Shape) shpSel_ais = new AIS_Shape(shpSel);
	shpSel_ais->SetHilightMode(3);
	pContext->SetColor(shpSel_ais, Quantity_NOC_BLUE3, Standard_False);
	pContext->Display(shpSel_ais, Standard_True);*/

}