#include <QtWidgets>

#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>
#include <AIS_Shape.hxx>
#include <AIS_InteractiveObject.hxx>
#include <SelectMgr_SelectableObject.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include "ConsDockWidget.h"
#include "M3d.h"
#include "DocumentComponent.h"
#include "mainwindow.h"
#include "ConsTableWidget.h"

ConsTableWidget::ConsTableWidget(QWidget* parent) :QTableWidget(parent), m_Clicked(0),R(-1),C(-1)
{
	myConsDocument = qobject_cast<ConsDockWidget*>(parent)->getConsDocument();
	init();
	m_timer = new QTimer(this);
	connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));
}
 
void ConsTableWidget::show_menu(const QPoint& p)
{
	QMenu* menu = new QMenu(this);
	QAction* del_text = new QAction("删除元素", this);
	QTableWidgetItem* curr_item = this->itemAt(p);
	//connect一定要写到前面，否则出现关联失败!!!!!!
	connect(del_text, &QAction::triggered, this, [=]() {
		if (curr_item == nullptr || (curr_item && curr_item->text() == tr("")))
		{
			return;
		}
		else
		{
			int row = curr_item->row();
			int col = curr_item->column();
			curr_item->setText(tr(""));
			QComboBox* box = (QComboBox*)(this->cellWidget(row, 0));
			box->setCurrentIndex(0);
			this->item(row, 3)->setText(tr(""));
		}
	});
	if (curr_item)
	{
		menu->addAction(del_text);
		menu->exec(QCursor::pos());
	}
}

void ConsTableWidget::AddShape()
{
	//选中了第R行,第C列的单元格
	if (C != 1 && C != 2)
	{
		return;
	}
	QTableWidgetItem* item = this->item(R, C);
	Handle(AIS_InteractiveContext) MyContext = myConsDocument->getContext();
	if (item == nullptr || (item && item->text() == tr("")))
	{
		MyContext->InitSelected();
		if (!MyContext->HasSelectedShape())
		{
			QMessageBox().about(this, tr("提示"), tr("未选中实体"));
		}
		else
		{
			TopoDS_Shape selectedShape = MyContext->SelectedShape();
			item->setText(QString::fromStdString(myConsDocument->GetShapeName(selectedShape)));
			//取消选中状态
			MyContext->ClearSelected(true);
		}
	}
	else
	{
		QMessageBox::about(this, tr("提示"), tr("该元素已选择,不能重复添加"));
	}
	//重置R,C
	R = -1, C = -1;
}

//点击单元格，将单元格的实体高亮，同时取消之前实体的高亮
//点击约束值的那个单元格，将一整行的两个实体高亮，同时取消之前实体的高亮
//高亮部分有问题！！！！！！尝试在选择之后使用
void ConsTableWidget::show_selectedshape()
{
	//点击的是第R行第C列,如果不是item1或者item2则暂时不处理
	if (C != 1 && C != 2)
	{
		return;
	}
	QTableWidgetItem* curr_item = this->item(R, C);
	if (curr_item == nullptr || (curr_item && curr_item->text() == tr("")))
	{
		return;
	}
	else
	{
		Handle(AIS_InteractiveContext) context = myConsDocument->getContext();
		std::string curr_shape_name = curr_item->text().toStdString();
		TopoDS_Shape curr_shape = myConsDocument->GetShapeFromName(curr_shape_name);
		//测试高亮功能，但是出现context未选中情况
		if (!curr_shape.IsNull())
		{
			//context->SetAutoActivateSelection(false);
			//context->ClearSelected(true);
			//context->UpdateSelected(true);

			//改为成员变量
			/*Handle(AIS_Shape)*/ ais_shape = new AIS_Shape(curr_shape);
			//context->AddOrRemoveSelected(ais_shape, true);
			//context->Activate(ais_shape);
			context->SetColor(ais_shape, Quantity_NOC_HOTPINK, Standard_False);
			context->Display(ais_shape, true);
			//context->SetSelected(ais_shape,true);
			//context->UpdateCurrentViewer();
			//if (!context->IsSelected(ais_shape))
			//{
			//	QMessageBox::about(this, tr(""), tr("没选中"));
			//}
			//context->HilightSelected(true);
		}
	}
	//重置R,C
	R = -1, C = -1;
}

void ConsTableWidget::OnConsType(const QString& text)
{
	QComboBox* consBox = qobject_cast<QComboBox*>(this->sender());
	if (consBox == nullptr)
	{
		return;
	}
	//确定是第几行的约束类型要改变
	int x = consBox->frameGeometry().x();
	int y = consBox->frameGeometry().y();
	QModelIndex index = this->indexAt(QPoint(x, y));
	int R = index.row();
	QTableWidgetItem* item1 = this->item(R, 1);
	QTableWidgetItem* item2 = this->item(R, 2);
	//如果元素1或者元素2为空的话，不允许添加约束
	if ((item1 == nullptr || (item1 && item1->text() == tr(""))) || (item2 == nullptr || (item2 && item2->text() == tr(""))))
	{
		if (text != "请选择")
		{
			QMessageBox().about(this, tr("提示"), tr("请先选择元素1和元素2"));
			consBox->setCurrentIndex(0);
		}
		return;
	}
	else
	{
		//目前的约束类型有：平行(面面平行),垂直(面面垂直),相切，重合
		//首先判断item1和item2的元素类型，从而确定所添加的约束是否合理
		//枚举所有约束，不符合条件就报错
		//以面面平行为例，必须元素1和元素2都为TopoDS_FACE才可以，否则无法添加
		TopoDS_Shape shape1 = myConsDocument->GetShapeFromName(item1->text().toStdString());
		TopoDS_Shape shape2 = myConsDocument->GetShapeFromName(item2->text().toStdString());
		if (text == tr("面面平行"))
		{
			if (shape1.ShapeType() != TopAbs_FACE || shape2.ShapeType() != TopAbs_FACE)
			{
				QMessageBox().about(this, tr("提示"), tr("约束无法正确添加，请检查元素1和元素2的类型"));
				consBox->setCurrentIndex(0);
			}
		}
	}
}

void ConsTableWidget::OnConsValue(const QString& text)
{
	QComboBox* consBox = qobject_cast<QComboBox*>(this->sender());
	if (consBox == nullptr)
	{
		return;
	}
	//确定是第几行的约束类型改变了
	int x = consBox->frameGeometry().x();
	int y = consBox->frameGeometry().y();
	QModelIndex index = this->indexAt(QPoint(x, y));
	int R = index.row();
	QTableWidgetItem* item3 = this->item(R, 3);

	//如果不是尺寸约束，不允许修改约束值
	if (text != QString("长度") && text != QString("角度"))
	{
		item3->setFlags(item3->flags() & ~Qt::ItemIsEditable);
	}
	else
	{
		item3->setFlags(item3->flags() | Qt::ItemIsEditable);
	}
}

void ConsTableWidget::onTimeout()
{
	m_timer->stop();
	if (m_Clicked == 1)
	{
		//单击事件
		//鼠标单击某单元格，如果单元格代表item1或者item2的话，判断是否为空对选中实体进行高亮显示
		show_selectedshape();
	}
	else if (m_Clicked == 2)
	{
		//双击事件
		//鼠标双击某单元格，代表选中该行，发出信号，如果元素1或元素2为空的话，选择导入模型的面、边、点
		AddShape();
	}
	m_Clicked = 0;
}

void ConsTableWidget::init()
{
	this->setFocusPolicy(Qt::NoFocus);
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
	this->setSelectionMode(QAbstractItemView::SingleSelection);
	this->setColumnCount(5);

	QStringList headerText;
	headerText << "约束类型" << "元素1" << "元素2" << "约束值" << "是否起作用";
	this->setHorizontalHeaderLabels(headerText);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//设置右键点击发出信号
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(show_menu(const QPoint&)));

	//读取已经存在的约束
	readConstraints();
}

//打开之前保存的约束时用到
void ConsTableWidget::readConstraints()
{
	//M3dCom_DS* myM3dCOM_DS = myConsDocument->GetComDS();
	vector<myConstraint> vCons = myConsDocument->GetAllConstraints();
	record = vCons;
	if (vCons.empty())
	{
		return;
	}
	readrecord();
}

void ConsTableWidget::readrecord()
{
	for (int i = 0; i < record.size(); ++i)
	{
		this->addConstraint();
		QTableWidgetItem* item1 = this->item(i, 1);
		item1->setText(QString::fromStdString(record[i].shapeName1));
		QTableWidgetItem* item2 = this->item(i, 2);
		item2->setText(QString::fromStdString(record[i].shapeName2));
		QTableWidgetItem* item3 = this->item(i, 3);
		if (record[i].consValue != 0.0)
		{
			item3->setText(QString::number(record[i].consValue));
		}
		QComboBox* consBox = qobject_cast<QComboBox*>(this->cellWidget(i, 0));
		myConstraintType consT = record[i].consType;
		std::string tname = consType2Name[consT];
		QString text = QString::fromStdString(tname);
		consBox->setCurrentText(text);

		QComboBox* activeBox = qobject_cast<QComboBox*>(this->cellWidget(i, 4));
		activeBox->setCurrentIndex(record[i].bActive ? 0 : 1);
	}
}

void ConsTableWidget::restoreConstraints()
{
	//读取record中的约束
	while (this->rowCount())
	{
		this->removeRow(0);
	}
	readrecord();
}

void ConsTableWidget::saveConstraints()
{
	//保存约束
	//M3dCom_DS* myM3dCOM_DS = myConsDocument->GetComDS();
	vector<myConstraint> vCons;
	//首先遍历约束表，如果存在不完整的约束，则要求补全
	int row = this->rowCount();
	for (int i = 0; i < row; ++i)
	{
		QComboBox* box = qobject_cast<QComboBox*>(this->cellWidget(i, 0));
		bool flag1 = box->currentIndex() == 0;
		bool flag2 = (box->currentText() == tr("长度") || box->currentIndex() == tr("角度")) && (this->item(i, 3)->text() == tr(""));
		if (flag1 || flag2)
		{
			QMessageBox::about(this, tr("提示"), tr("请补全约束或删除空约束"));
			return;
		}
		else
		{
			myConstraintType myConType = consName2Type[box->currentText().toStdString()];
			std::string shapeName1 = this->item(i, 1)->text().toStdString();
			std::string shapeName2 = this->item(i, 2)->text().toStdString();
			QTableWidgetItem* item3 = this->item(i, 3);
			double consValue = item3->text() == tr("") ? 0.0 : item3->text().toDouble();
			QComboBox* activeBox = qobject_cast<QComboBox*>(this->cellWidget(i, 4));
			bool bActive = activeBox->currentIndex() == 0;
			myConstraint myCon{i+1, myConType,shapeName1,shapeName2,consValue,bActive };
			vCons.push_back(myCon);
		}
	}
	record = vCons;
	myConsDocument->SetConstraints(vCons);
}

void ConsTableWidget::addConstraint()
{
	//新添加行的行号
	int rowCnt = this->rowCount();
	this->insertRow(rowCnt);
	//设置每一列的属性
	//其中约束类型用QComboBox表示，元素1、元素2、约束值,是否起作用用QComboBox表示,默认为起作用
	QComboBox* consBox = new QComboBox();
	QStringList consBox_list;
	consBox_list.append(tr("请选择"));
	consBox_list.append(tr("垂直"));
	//测试
	consBox_list.append(tr("面面垂直"));
	consBox_list.append(tr("平行"));
	//测试
	consBox_list.append(tr("面面平行"));
	consBox_list.append(tr("相切"));
	consBox_list.append(tr("重合"));
	consBox_list.append(tr("长度"));
	consBox_list.append(tr("角度"));
	consBox->addItems(consBox_list);

	this->setCellWidget(rowCnt, 0, consBox);

	QComboBox* activeBox = new QComboBox();
	QStringList active_list;
	active_list.append(tr("是"));
	active_list.append(tr("否"));
	activeBox->addItems(active_list);

	this->setCellWidget(rowCnt, 4, activeBox);

	//每添加一行，都要设置该行的1列和2列不可编辑
	QTableWidgetItem* item1 = new QTableWidgetItem();
	item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
	this->setItem(rowCnt, 1, item1);

	QTableWidgetItem* item2 = new QTableWidgetItem();
	item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
	this->setItem(rowCnt, 2, item2);

	QTableWidgetItem* item3 = new QTableWidgetItem();
	item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
	this->setItem(rowCnt, 3, item3);

	//判断如果约束类型改变，是否符合要求，若不符合则恢复成"请选择"
	connect(consBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(OnConsType(const QString&)), Qt::UniqueConnection);
	//判断当约束类型改变之后，约束值是否可以被编辑
	connect(consBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(OnConsValue(const QString&)), Qt::UniqueConnection);
}

void ConsTableWidget::delConstraint()
{
	int curr_row = this->currentRow();
	//未选中情况
	if (curr_row == -1)
	{
		QMessageBox::about(this, tr(""), tr("未选中任何约束"));
		return;
	}
	//弹出对话框，是否要删除约束
	QMessageBox::StandardButton delbtn = QMessageBox::question(this, tr("提示"), tr("是否删除该约束?"), QMessageBox::Yes | QMessageBox::No);
	if (delbtn == QMessageBox::Yes)
	{
		this->removeRow(curr_row);
		this->setCurrentCell(-1, -1);
	}
}

void ConsTableWidget::clearAllConstraints()
{
	int rowCnt = this->rowCount();
	if (rowCnt == 0)
	{
		QMessageBox().about(this, tr("提示"), tr("约束列表已清空"));
		return;
	}
	//弹出对话框，是否要清空约束
	QMessageBox::StandardButton clrBox = QMessageBox::question(this, tr("提示"), tr("是否清空所有约束?"), QMessageBox::Yes | QMessageBox::No);
	if (clrBox == QMessageBox::Yes)
	{
		for (int i = rowCnt - 1; i >= 0; --i)
		{
			this->removeRow(i);
		}
	}
}

void ConsTableWidget::unCheck()
{
	this->setCurrentCell(-1, -1);
}

void ConsTableWidget::mouseReleaseEvent(QMouseEvent* event)
{
	if (!ais_shape.IsNull())
	{
		Handle(AIS_InteractiveContext) context = myConsDocument->getContext();
		context->Erase(ais_shape, true);
	}

	QTableWidget::mouseReleaseEvent(event);
}

void ConsTableWidget::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (!m_timer->isActive())
		{
			m_timer->start(300);
		}
		m_Clicked++;
		QPoint pos = event->pos();
		QTableWidgetItem* curr_item = this->itemAt(pos);
		if (curr_item == nullptr)
		{
			return;
		}
		else
		{
			R = curr_item->row();
			C = curr_item->column();
			if (C != 1 && C != 2)
			{
				QTableWidget::mousePressEvent(event);
			}
		}
	}
}