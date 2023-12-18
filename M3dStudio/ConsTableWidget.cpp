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
	QAction* del_text = new QAction("ɾ��Ԫ��", this);
	QTableWidgetItem* curr_item = this->itemAt(p);
	//connectһ��Ҫд��ǰ�棬������ֹ���ʧ��!!!!!!
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
	//ѡ���˵�R��,��C�еĵ�Ԫ��
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
			QMessageBox().about(this, tr("��ʾ"), tr("δѡ��ʵ��"));
		}
		else
		{
			TopoDS_Shape selectedShape = MyContext->SelectedShape();
			item->setText(QString::fromStdString(myConsDocument->GetShapeName(selectedShape)));
			//ȡ��ѡ��״̬
			MyContext->ClearSelected(true);
		}
	}
	else
	{
		QMessageBox::about(this, tr("��ʾ"), tr("��Ԫ����ѡ��,�����ظ����"));
	}
	//����R,C
	R = -1, C = -1;
}

//�����Ԫ�񣬽���Ԫ���ʵ�������ͬʱȡ��֮ǰʵ��ĸ���
//���Լ��ֵ���Ǹ���Ԫ�񣬽�һ���е�����ʵ�������ͬʱȡ��֮ǰʵ��ĸ���
//�������������⣡����������������ѡ��֮��ʹ��
void ConsTableWidget::show_selectedshape()
{
	//������ǵ�R�е�C��,�������item1����item2����ʱ������
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
		//���Ը������ܣ����ǳ���contextδѡ�����
		if (!curr_shape.IsNull())
		{
			//context->SetAutoActivateSelection(false);
			//context->ClearSelected(true);
			//context->UpdateSelected(true);

			//��Ϊ��Ա����
			/*Handle(AIS_Shape)*/ ais_shape = new AIS_Shape(curr_shape);
			//context->AddOrRemoveSelected(ais_shape, true);
			//context->Activate(ais_shape);
			context->SetColor(ais_shape, Quantity_NOC_HOTPINK, Standard_False);
			context->Display(ais_shape, true);
			//context->SetSelected(ais_shape,true);
			//context->UpdateCurrentViewer();
			//if (!context->IsSelected(ais_shape))
			//{
			//	QMessageBox::about(this, tr(""), tr("ûѡ��"));
			//}
			//context->HilightSelected(true);
		}
	}
	//����R,C
	R = -1, C = -1;
}

void ConsTableWidget::OnConsType(const QString& text)
{
	QComboBox* consBox = qobject_cast<QComboBox*>(this->sender());
	if (consBox == nullptr)
	{
		return;
	}
	//ȷ���ǵڼ��е�Լ������Ҫ�ı�
	int x = consBox->frameGeometry().x();
	int y = consBox->frameGeometry().y();
	QModelIndex index = this->indexAt(QPoint(x, y));
	int R = index.row();
	QTableWidgetItem* item1 = this->item(R, 1);
	QTableWidgetItem* item2 = this->item(R, 2);
	//���Ԫ��1����Ԫ��2Ϊ�յĻ������������Լ��
	if ((item1 == nullptr || (item1 && item1->text() == tr(""))) || (item2 == nullptr || (item2 && item2->text() == tr(""))))
	{
		if (text != "��ѡ��")
		{
			QMessageBox().about(this, tr("��ʾ"), tr("����ѡ��Ԫ��1��Ԫ��2"));
			consBox->setCurrentIndex(0);
		}
		return;
	}
	else
	{
		//Ŀǰ��Լ�������У�ƽ��(����ƽ��),��ֱ(���洹ֱ),���У��غ�
		//�����ж�item1��item2��Ԫ�����ͣ��Ӷ�ȷ������ӵ�Լ���Ƿ����
		//ö������Լ���������������ͱ���
		//������ƽ��Ϊ��������Ԫ��1��Ԫ��2��ΪTopoDS_FACE�ſ��ԣ������޷����
		TopoDS_Shape shape1 = myConsDocument->GetShapeFromName(item1->text().toStdString());
		TopoDS_Shape shape2 = myConsDocument->GetShapeFromName(item2->text().toStdString());
		if (text == tr("����ƽ��"))
		{
			if (shape1.ShapeType() != TopAbs_FACE || shape2.ShapeType() != TopAbs_FACE)
			{
				QMessageBox().about(this, tr("��ʾ"), tr("Լ���޷���ȷ��ӣ�����Ԫ��1��Ԫ��2������"));
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
	//ȷ���ǵڼ��е�Լ�����͸ı���
	int x = consBox->frameGeometry().x();
	int y = consBox->frameGeometry().y();
	QModelIndex index = this->indexAt(QPoint(x, y));
	int R = index.row();
	QTableWidgetItem* item3 = this->item(R, 3);

	//������ǳߴ�Լ�����������޸�Լ��ֵ
	if (text != QString("����") && text != QString("�Ƕ�"))
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
		//�����¼�
		//��굥��ĳ��Ԫ�������Ԫ�����item1����item2�Ļ����ж��Ƿ�Ϊ�ն�ѡ��ʵ����и�����ʾ
		show_selectedshape();
	}
	else if (m_Clicked == 2)
	{
		//˫���¼�
		//���˫��ĳ��Ԫ�񣬴���ѡ�и��У������źţ����Ԫ��1��Ԫ��2Ϊ�յĻ���ѡ����ģ�͵��桢�ߡ���
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
	headerText << "Լ������" << "Ԫ��1" << "Ԫ��2" << "Լ��ֵ" << "�Ƿ�������";
	this->setHorizontalHeaderLabels(headerText);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//�����Ҽ���������ź�
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(show_menu(const QPoint&)));

	//��ȡ�Ѿ����ڵ�Լ��
	readConstraints();
}

//��֮ǰ�����Լ��ʱ�õ�
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
	//��ȡrecord�е�Լ��
	while (this->rowCount())
	{
		this->removeRow(0);
	}
	readrecord();
}

void ConsTableWidget::saveConstraints()
{
	//����Լ��
	//M3dCom_DS* myM3dCOM_DS = myConsDocument->GetComDS();
	vector<myConstraint> vCons;
	//���ȱ���Լ����������ڲ�������Լ������Ҫ��ȫ
	int row = this->rowCount();
	for (int i = 0; i < row; ++i)
	{
		QComboBox* box = qobject_cast<QComboBox*>(this->cellWidget(i, 0));
		bool flag1 = box->currentIndex() == 0;
		bool flag2 = (box->currentText() == tr("����") || box->currentIndex() == tr("�Ƕ�")) && (this->item(i, 3)->text() == tr(""));
		if (flag1 || flag2)
		{
			QMessageBox::about(this, tr("��ʾ"), tr("�벹ȫԼ����ɾ����Լ��"));
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
	//������е��к�
	int rowCnt = this->rowCount();
	this->insertRow(rowCnt);
	//����ÿһ�е�����
	//����Լ��������QComboBox��ʾ��Ԫ��1��Ԫ��2��Լ��ֵ,�Ƿ���������QComboBox��ʾ,Ĭ��Ϊ������
	QComboBox* consBox = new QComboBox();
	QStringList consBox_list;
	consBox_list.append(tr("��ѡ��"));
	consBox_list.append(tr("��ֱ"));
	//����
	consBox_list.append(tr("���洹ֱ"));
	consBox_list.append(tr("ƽ��"));
	//����
	consBox_list.append(tr("����ƽ��"));
	consBox_list.append(tr("����"));
	consBox_list.append(tr("�غ�"));
	consBox_list.append(tr("����"));
	consBox_list.append(tr("�Ƕ�"));
	consBox->addItems(consBox_list);

	this->setCellWidget(rowCnt, 0, consBox);

	QComboBox* activeBox = new QComboBox();
	QStringList active_list;
	active_list.append(tr("��"));
	active_list.append(tr("��"));
	activeBox->addItems(active_list);

	this->setCellWidget(rowCnt, 4, activeBox);

	//ÿ���һ�У���Ҫ���ø��е�1�к�2�в��ɱ༭
	QTableWidgetItem* item1 = new QTableWidgetItem();
	item1->setFlags(item1->flags() & (~Qt::ItemIsEditable));
	this->setItem(rowCnt, 1, item1);

	QTableWidgetItem* item2 = new QTableWidgetItem();
	item2->setFlags(item2->flags() & (~Qt::ItemIsEditable));
	this->setItem(rowCnt, 2, item2);

	QTableWidgetItem* item3 = new QTableWidgetItem();
	item3->setFlags(item3->flags() & (~Qt::ItemIsEditable));
	this->setItem(rowCnt, 3, item3);

	//�ж����Լ�����͸ı䣬�Ƿ����Ҫ������������ָ���"��ѡ��"
	connect(consBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(OnConsType(const QString&)), Qt::UniqueConnection);
	//�жϵ�Լ�����͸ı�֮��Լ��ֵ�Ƿ���Ա��༭
	connect(consBox, SIGNAL(currentTextChanged(const QString&)), this, SLOT(OnConsValue(const QString&)), Qt::UniqueConnection);
}

void ConsTableWidget::delConstraint()
{
	int curr_row = this->currentRow();
	//δѡ�����
	if (curr_row == -1)
	{
		QMessageBox::about(this, tr(""), tr("δѡ���κ�Լ��"));
		return;
	}
	//�����Ի����Ƿ�Ҫɾ��Լ��
	QMessageBox::StandardButton delbtn = QMessageBox::question(this, tr("��ʾ"), tr("�Ƿ�ɾ����Լ��?"), QMessageBox::Yes | QMessageBox::No);
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
		QMessageBox().about(this, tr("��ʾ"), tr("Լ���б������"));
		return;
	}
	//�����Ի����Ƿ�Ҫ���Լ��
	QMessageBox::StandardButton clrBox = QMessageBox::question(this, tr("��ʾ"), tr("�Ƿ��������Լ��?"), QMessageBox::Yes | QMessageBox::No);
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