#include "M3d.h"
#include "AssemblyWidget.h"
#include "mainwindow.h"
#include "global.h"
#include <QtWidgets>
#include<QMouseEvent>
#include <AIS_ColoredShape.hxx>
//#include "DocumentCommon.h"
#include "DocumentModel.h"
#include "ImageShowDlg.h"
#include <QInputDialog>

Q_DECLARE_METATYPE(Component*);

AssemblyWidget::AssemblyWidget(QWidget* parent) : Parent(parent)
,m_Clicked(0),m_obj(nullptr),previous(nullptr),current(nullptr)
{
	m_timer = new QTimer(this);
	
	myDocument3d = 
		(DocumentModel*) qobject_cast<MainWindow*>(parent)->getDocument();
	
	connect(m_timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

	init();
	this->setMouseTracking(true);

	myImageShowDlg = new ImageShowDlg(this);

	bTimeIsUp = false;
}

void AssemblyWidget::UpdateAssemblyTree(Component* pRoot)
{
	m_root = pRoot;

	init();
}

AssemblyWidget::~AssemblyWidget()
{
	delete myImageShowDlg;
}

void AssemblyWidget::init()
{
	count = 0;
	this->setIconSize(QSize(50, 31));
	this->setFrameStyle(QFrame::Panel | QFrame::Plain);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	//connect(this, &AssemblyWidget::customContextMenuRequested, this, &AssemblyWidget::show_menu);

	this->setHeaderItem(new QTreeWidgetItem({ tr("��ǰװ����") }));
	initTree();
	init_menu();
	setMinimumWidth(300);
	setMaximumWidth(500);
	current = nullptr;
}

//������������֣�����item
void AssemblyWidget::HighLightItem(const char* csItem)
{
	//����ȫ���ڵ�
	QList<QTreeWidgetItem*> theItems = this->findItems("", Qt::MatchContains | Qt::MatchRecursive);
	auto itr = theItems.begin();
	for (; itr != theItems.end(); ++itr)
	{
		QTreeWidgetItem* pItem = *itr;
		pItem->setBackground(0, QBrush());
		
		QTreeWidgetItem* pParent = pItem->parent();
		if (!pParent) {
			continue;
		}
		QTreeWidgetItem* pGrandPa = pParent->parent();
		if (pGrandPa) {//ֻ�����һ��
			continue;
		}

		QString sItemText = pItem->text(0);
		if (sItemText == csItem)
		{
			QBrush m_brush = QBrush(QColor(193, 210, 240));
			pItem->setBackground(0, m_brush);
		}
	}
}

//�ڵ����
void AssemblyWidget::curItemChangedSlot(QTreeWidgetItem* cur, QTreeWidgetItem* pre)
{
	if (!cur) return;
	static int times = 0;
	if (times == 1) {
		times = 0; return;
	}
	if (pre)pre->setBackground(0, QBrush());
	QBrush m_brush = QBrush(QColor(193, 210, 240));
	current = cur;
	previous = pre;
	current->setBackground(0, m_brush);
	highlight(current);
	times++;
}

void AssemblyWidget::onTimeout()
{
	if (m_Clicked == 1) {
		pickupComp();
	}
	m_Clicked = 0;

	bTimeIsUp = true;
}

QPixmap AssemblyWidget::GetPixmap(QTreeWidgetItem* theItem)
{
	QPixmap pm;

	Component* mycomp = (theItem->data(colItem, Qt::UserRole)).value<Component*>();
	if (mycomp)
		pm = mycomp->GetDocument()->GetPixmap();

	return pm;
}

void AssemblyWidget::leaveEvent(QEvent* event)
{
	myImageShowDlg->hide();
}

void AssemblyWidget::mouseMoveEvent(QMouseEvent* event)
{
	static QTreeWidgetItem* old_item;

	QModelIndex index = this->indexAt(event->pos());
	if (!index.isValid()) {
		//QToolTip::hideText();
		myImageShowDlg->hide();
		return;
	}

	QTreeWidgetItem* curr_item = this->itemAt(event->pos());
	QString sItemText = curr_item->text(0);
	//QToolTip::showText(event->globalPos(), sItemText);

	if (curr_item != old_item)
	{
		if (false && bTimeIsUp) //�Ȳ���ʾ�ˣ�
		{
			QPixmap pm = GetPixmap(curr_item);
			if (!pm.isNull())
			{
				myImageShowDlg->setWindowTitle(sItemText);
				myImageShowDlg->setGeometry(event->globalPos().x() + 50, event->globalPos().y(), 810, 510);
				myImageShowDlg->SetPixmap(pm);
				myImageShowDlg->show();
			}
			old_item = curr_item;
			bTimeIsUp = false;
		}
	}
}

void AssemblyWidget::mousePressEvent(QMouseEvent* event)
{
	//current=currentItem();
	QModelIndex index = this->indexAt(event->pos());
	if (!index.isValid()) return;
	previous = current;
	if (previous)previous->setBackground(0, QBrush());
	//��õ�current����Ч
	auto item= this->itemAt(event->pos());
	if (item)current = item;
	//if (!cur)return;
	//current = cur;
	//if (!current)return;
	QBrush m_brush = QBrush(QColor(193, 210, 240));
	current->setBackground(0, m_brush);
	highlight(current);
	
	if (event->button() == Qt::LeftButton)
	{
		if (!m_timer->isActive())
		{
			m_timer->start(300);
		}
		m_Clicked++;
		//QPoint pos = event->pos();
		//QTreeWidgetItem* curr_item = this->itemAt(pos);
		//if (curr_item == nullptr)
		//{
		//	return;
		//}
		
	}
	else if (event->button() == Qt::RightButton) 
	{
		QTreeWidgetItem* pParent = item->parent();
		if (pParent)
		{
			QTreeWidgetItem* pGrandParent = pParent->parent();
			if (pParent && !pGrandParent)
			{
				show_menu(event);
			}
		}
	}
}

void AssemblyWidget::mouseReleaseEvent(QMouseEvent* event)
{
	//MainWindow::mouseReleaseEvent(event);
	m_timer->stop();
	m_Clicked = 0;
	if (!m_obj)return;
	Handle(AIS_InteractiveContext) m_context = ((DocumentCommon*)myDocument3d)->getContext();
	m_context->Remove(m_obj, Standard_True);
	//m_obj->Delete();
	m_obj = nullptr;
	//m_context->UpdateCurrentViewer();
	
}

void AssemblyWidget::pickupComp()
{
	//��ʱ����ʱ�������ж��Ƿ�Ϊһ�������ֱ����ѡ����������interobject��ѡ
	if (this == nullptr)return;
	//QTreeWidgetItem* cur = this->currentItem();
	if(current == nullptr) return;
	Component* mycomp=(current->data(colItem, Qt::UserRole)).value<Component*>();
	if(!mycomp)return ;
	//Component* m_root = myDocument3d->GetComponent();
	//��ǰ���Ϊһ���������Ϊװ����
	if (mycomp == m_root->GetChild(&(mycomp->GetCompName()).at(0))||mycomp->IsAssembly())
	{
		highlight(current);
	}
	else {
		//TopoDS_Shape m_ts=mycomp->GenerateCompound();
		//M3dCom_DS* m_DS = (M3dCom_DS*)((DocumentCommon*)myDocument3d)->GetDS();
		//if (!m_DS) return;
		//TopoDS_Shape m_ts=mycomp->GenerateCompound();
		//TopoDS_Shape m_ts = m_DS->GetShapeFromName(mycomp->GetCompName());
		TopoDS_Shape m_ts = getTopoDS(mycomp);
		m_obj = new AIS_ColoredShape(m_ts);
		Handle(AIS_InteractiveContext) m_context=((DocumentCommon*)myDocument3d)->getContext();
		//m_context->ClearCurrents(Standard_True);
		if(!m_context->IsDisplayed(m_obj))
		m_context->Display(m_obj, Standard_False);
		
		m_context->SetColor(m_obj, Quantity_NOC_YELLOW, Standard_True);
		//m_context->UpdateCurrentViewer();
	}
	//GetCompound();
	//mycomp->
	
}

TopoDS_Shape AssemblyWidget::getTopoDS(Component* pCom) {
	vector<ShapeMaterial> shpMats; //��״�Ĳ���;
	vector<ShapeTransparency> shpTransparencys; //��״��͸����;
	vector<ShapeColor> shpColors;//��״����ɫ��
	vector<TopoDS_Shape> shpDatumPts; //�任��Ĳο���
	vector<TopoDS_Shape> shpDatumLns; //�任��Ĳο���
	vector<TopoDS_Shape> shpConnectos; //�任��Ľӿ�
	vector<gp_Ax3> coordSys; //�任��Ľӿ�����ϵͳ��ʾ
	TopoDS_Shape comShape = pCom->GetCompound();
		//GenerateCompound1(shpMats, shpTransparencys, shpColors, shpDatumPts, shpDatumLns, shpConnectos, coordSys);//GetCompound();
	return comShape;
}

QSize AssemblyWidget::sizeHint() const
{
	return QSize(Parent->width() / 3, Parent->height() / 2);
}
void AssemblyWidget::initTree()
{
	this->clear();
	QString dataStr;
	QIcon icon;
	icon.addFile("images/Assemblylibrary.png");
	/*QTreeWidgetItem* item = new QTreeWidgetItem(AssemblyWidget::itTopItem);
	item->setIcon(colItem, icon);
	item->setText(colItem, "װ����");
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
	
	item->setData(colItem, Qt::UserRole, QVariant(dataStr));
	item->setData(colItem, Qt::UserRole + 1, QVariant("itTopItem"));
	this->addTopLevelItem(item);*/

	//����ǰ�ĵ�����װ���壬ֱ�Ӷ�ȡ
	if (myDocument3d /*&& myDocument3d->GetDS()*/)
	{
		Component* myComp = myDocument3d->GetComponent();
		dfs(this->topLevelItem(colItem), myComp);

	}
	//if()
}
void AssemblyWidget::dfs(QTreeWidgetItem* parItem,Component* myComp)
{
	if (!myComp) return;
	QTreeWidgetItem* cur=addItem(parItem, myComp);

	auto vec = myComp->GetChilds();
	//showTable(myComp);
	for (auto itr = vec.begin(); itr != vec.end(); itr++) {
		dfs(cur, *itr);
		//showTable(*itr);
	}
	this->expandAll();
}

/*void AssemblyWidget::showTable(Component* myComp)
{
	CyberInfo* cur=myComp->GetCyberInfo();
	if (!cur) return;
	auto paravec = cur->GetParameters();
	for (auto itr = paravec.begin(); itr != paravec.end(); itr++) {
		if ((*itr)->bFocused == true) {
			myTablewid->setItem(count, 1, new QTableWidgetItem(QString::fromStdString((*itr)->sFocusName)));
			myTablewid->setItem(count++, 1, new QTableWidgetItem((*itr)->defaultValue));
		}
	}
}*/

QTreeWidgetItem* AssemblyWidget::addItem(QTreeWidgetItem* parItem,Component* myComp)
{
	int type=1002 ;
	if (!parItem) type = 1001;
	else if (myComp->GetChilds().empty()) type = 1003;
	QTreeWidgetItem* item = new QTreeWidgetItem(type);
	//QIcon icon;
	/*QString s = "images/";
	switch (type) {
		case 1001 : 
		case 1003 : 
			s += "Assembly"; 
			break;
		case 1002 :
			s += "Component";
	}
	s += ".png";*/
	//icon.addFile(s);
	//parItem->addChild(item);
	QString title;
	if (myComp->GetCompName() == "")
		title = myComp->GetCompFile();
	else 
		title = QString::fromStdString(myComp->GetCompName());
	item->setText(colItem, title);
	QIcon icon;
	if (title != "Untitled")
	icon=QIcon(myComp->GetDocument()->GetPixmap());
	item->setIcon(colItem, icon);
	item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
	//item->setCheckState(colItem, Qt::Checked);
	item->setData(colItem, Qt::UserRole, QVariant::fromValue(myComp));
	item->setData(colItem, Qt::UserRole + 1, QVariant(type==1003?"itGroupItem":(type==1001?"itTopItem":"itLeafItem")));
	if (!parItem) {
		this->takeTopLevelItem(colItem);
		this->addTopLevelItem(item);
	}
		else parItem->addChild(item);
	return item;
}

void AssemblyWidget::init_menu() {
	menu = new QMenu(Parent);
	QAction* renameAct = new QAction(tr("������"), this);
	QAction* delAct = new QAction(tr("ɾ  ��"), this);
	//Ϊ��ӹ�������
	connect(renameAct, &QAction::triggered, this, &AssemblyWidget::renameComponent);
	connect(delAct, &QAction::triggered, this, &AssemblyWidget::deleteComponent);
	menu->addAction(renameAct);
	menu->addAction(delAct);
}


void AssemblyWidget::show_menu(QMouseEvent* event)
{
	//auto item = this->currentItem();
	//if (this->currentItem() == NULL)return;
	menu->popup(event->globalPos());
}

void AssemblyWidget::highlight(QTreeWidgetItem* parItem)
{
	//QTreeWidgetItem* parItem = this->currentItem();
	if (!parItem) {
		return;
	}
	QTreeWidgetItem* pParent = parItem->parent();
	if (!pParent) {
		return;
	}

	QTreeWidgetItem* pGrandParent = pParent->parent();
	if (pGrandParent) {
		return;
	}

	QString compName = parItem->text(0);
	if (!compName.isEmpty())
	{
		//������ʾ�㲿��
		myDocument3d->SelectComponent(compName.toStdString().c_str());
	}
}

void AssemblyWidget::deleteComponent()
{
	//QTreeWidgetItem* parItem = this->currentItem();
	del(current);
}

//����
//���桢�ڴ桢�ĵ�
void AssemblyWidget::renameComponent()
{
	if (!current ){
		return;
	}

	QString compName = current->text(0);// parItem->data(colItem, Qt::UserRole).toString();
	if (!compName.isEmpty())
	{
		QString dlgTitle = "���������";
		QString txtLabel = "�������µ������";
		QString defaultInput = compName;
		QLineEdit::EchoMode echoMode = QLineEdit::Normal;//������������
		//QLineEdit::EchoMode echoMode=QLineEdit::Password;//��������
		bool ok = false;
		QString text = QInputDialog::getText(this, dlgTitle, txtLabel, echoMode, defaultInput, &ok);
		if (ok)
		{
			if (!IsLegalName(text))
			{
				QMessageBox::information(this, "����������", "�Ƿ����������", QMessageBox::Ok);
				return;
			}

			//�����������
			Component* theComponent = (current->data(colItem, Qt::UserRole)).value<Component*>();
			assert(theComponent);
			Component* pParent = theComponent->GetParent();
			if (!pParent)
			{
				QMessageBox::information(this, "����������", "��������ܸ�����", QMessageBox::Ok);
				return;
			}

			list<Component*> levChilds = pParent->GetChilds();
			auto itrChld = levChilds.begin();
			for (; itrChld != levChilds.end(); ++itrChld)
			{
				Component* pChild = *itrChld;
				if (text == pChild->GetCompName().c_str())
				{
					QMessageBox::information(this, "����������", "�������������", QMessageBox::Ok);
					return;
				}
			}

			//�������
			current->setText(0, text);
			//�ڴ����
			theComponent->SetCompName(text.toStdString().c_str());

			//�����ĵ�
			myDocument3d->RenameComponent(compName, text);
		}
	}
}

void AssemblyWidget::del(QTreeWidgetItem* myItem)
{
	if (!myItem) {//|| myItem == this->topLevelItem(0)) 
		return;
	}
	QString compName = myItem->text(0);// parItem->data(colItem, Qt::UserRole).toString();
	if (!compName.isEmpty())
	{
		//ѡ���㲿��
		//M3dMdl_DS* m_DS = (M3dMdl_DS*)((DocumentCommon*)myDocument3d)->GetDS();
		Component* mycomponet = (current->data(colItem, Qt::UserRole)).value<Component*>();
		if (!mycomponet/*->IsAssembly()*/) return;
		previous = nullptr;
		current = nullptr;

		//m_DS->DeleteComponent(mycomponet);
		myDocument3d->DeleteComponent(mycomponet);
		myDocument3d->SelectComponent(compName.toStdString().c_str());
		myDocument3d->DeleteSelected();
	}
}
