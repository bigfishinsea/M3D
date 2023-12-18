#include "M3d.h"
#include "ModelWidget.h"
#include "mainwindow.h"
#include"DocumentModel.h"
#include "DocumentComponent.h"
#include <QtWidgets>
#include "ImageShowDlg.h"

ModelWidget::ModelWidget(QWidget* parent) :QFrame(parent), Parent(parent)
{
	this->myDocument3d = (DocumentModel *) qobject_cast<MainWindow*>(parent)->getDocument();
	init();

	myImageShowDlg = new ImageShowDlg(this);
	this->setMouseTracking(true);
}

void ModelWidget::init()
{
	this->setFrameStyle(QFrame::Panel | QFrame::Plain);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &ModelWidget::customContextMenuRequested, this, &ModelWidget::show_menu);

	WorkPathLabel = new QLabel();
	WorkPathLabel->setText("......");
	WorkPathLabel->setScaledContents(true);
	WorkPathLabel->setStyleSheet(" background-color: white ;");

	BrowseButton = new QPushButton();
	BrowseButton->setText("...");
	BrowseButton->setMaximumSize(30, 20);
	connect(BrowseButton, &QPushButton::clicked, this, &ModelWidget::addItem);
	
	QHBoxLayout* HorizontalLayout1= new QHBoxLayout();
	HorizontalLayout1->addWidget(WorkPathLabel);
	HorizontalLayout1->addWidget(BrowseButton);

	myTreewid = new QTreeWidget(this);
	myTreewid->setMouseTracking(true);
	myTreewid->setIconSize(QSize(50,31));
	connect(myTreewid, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
		this, SLOT(curItemChangedSlot(QTreeWidgetItem*, QTreeWidgetItem*)));
	connect(myTreewid, SIGNAL(itemEntered(QTreeWidgetItem *, int)),
		this, SLOT(enterItem(QTreeWidgetItem *, int)));

	myTreewid->setHeaderItem(new QTreeWidgetItem({ tr("当前文件夹")}));
	QVBoxLayout* HorizontalLayout2 = new QVBoxLayout(this);
	HorizontalLayout2->addLayout(HorizontalLayout1);
	HorizontalLayout2->addWidget(myTreewid);
	this->setLayout(HorizontalLayout2);
	initTree();
	additem(QDir::currentPath());
}

QSize ModelWidget::sizeHint() const
{
	return QSize(Parent->width() / 3, Parent->height() / 2);
}

void ModelWidget::show_menu()
{
	QMenu* menu = new QMenu(this);
	QAction* openAct = new QAction(tr("打开"), this);
	QAction* insertAct = new QAction(tr("插入"), this);
	//为两者添加关联函数
	connect(insertAct, &QAction::triggered, this, &ModelWidget::insert);
	connect(openAct, &QAction::triggered, this, &ModelWidget::runLeafItem);
	menu->addAction(openAct);
	menu->addSeparator();
	menu->addAction(insertAct);
	menu->exec(QCursor::pos());
}

void ModelWidget::initTree() {
	//myTreewid->setAutoExpandDelay();
	myTreewid->clear();
	QString dataStr;
	QIcon icon;
	icon.addFile("images/library.png"); 
	QTreeWidgetItem* item = new QTreeWidgetItem(ModelWidget::itTopItem);
	item->setIcon(colItem, icon);
	item->setText(colItem, "模型库");
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
	//item->setCheckState(colItem, Qt::Checked);
	item->setData(colItem, Qt::UserRole, QVariant(dataStr));
	item->setData(colItem, Qt::UserRole+1, QVariant("itTopItem"));
	myTreewid->addTopLevelItem(item);
	//
}

QString ModelWidget::getFinalfolder(const QString& fullPathName)
{
	int cnt = fullPathName.length(); //字符串长度
	int i = fullPathName.lastIndexOf("/");//  最后一次出现的位置
	QString str = fullPathName.right(cnt - i - 1); //获得最后的文件夹的名称
	return str;
}


QList<QString> ModelWidget::getmdl(const QString& filepath)
{
	QDir* dir = new QDir(filepath);
	QStringList filter;
	QString format1 = "*.m3dcom";
	QString format2 = "*.m3dmdl";
	filter << format1<<format2;
	QList<QFileInfo>* fomatInfoList = new QList<QFileInfo>(dir->entryInfoList(filter));
	QList<QString> receiveFilePath;
	for (int i = 0; i < fomatInfoList->count(); i++) {
		receiveFilePath << fomatInfoList->at(i).filePath();
	}
	return receiveFilePath;
}

void ModelWidget::addItem()
{
	QString dir;
	dir = QFileDialog::getExistingDirectory();
	additem(dir);
}

void ModelWidget::additem(QString dir)
{
	if (!dir.isEmpty()) {
		initTree();
		QTreeWidgetItem* parItem = myTreewid->topLevelItem(0);
		if (!dfs(parItem, dir))
		{
			QMessageBox msg;
			msg.setText("选取的文件夹缺少有效的模型文件");
			msg.exec();
			return;
		}
		WorkPathLabel->setText(dir);
		myTreewid->expandAll();
	}
}

//遍历当前文件夹下所有文件夹和节点
//对每一个文件夹进行遍历
//对叶子文件夹判断是否存在模型
//不存在返回false
//上一个文件夹所有子文件夹返回false，检查自身是否存在模型
//存在添加到根节点上
//

bool ModelWidget::dfs(QTreeWidgetItem* parItem,const QString& filepath) {
	QDir* dir = new QDir(filepath);
	//dir->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
	QFileInfoList list = dir->entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);
	auto root = parItem;
	bool flag =false;
	for (auto itr = list.begin(); itr != list.end(); ++itr) {
		bool flag1 = false;
		auto path = (*itr).filePath();
		auto temp=addGroupItem(parItem, (*itr).filePath());
		flag1|=dfs(temp, path);
		if (flag1)
		{
			parItem->addChild(temp);
		}
		flag |= flag1;
	}
	QStringList filter;
	QString format1 = "*.m3dcom";
	QString format2 = "*.m3dmdl";
	filter << format1 << format2;
	//dir->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
	QList<QFileInfo>* fomatInfoList = new QList<QFileInfo>(dir->entryInfoList(filter));
	QList<QString> receiveFilePath;
	for (int i = 0; i < fomatInfoList->count(); i++) {
		receiveFilePath << fomatInfoList->at(i).filePath();
	}
	flag |= (!receiveFilePath.isEmpty());
	for (auto itr = receiveFilePath.begin(); itr < receiveFilePath.end(); itr++)
		addLeafItem(parItem, *itr);
	return flag;
}

QTreeWidgetItem* ModelWidget::addGroupItem(QTreeWidgetItem* parItem, const QString& dir)
{

	QTreeWidgetItem* item = new QTreeWidgetItem(itGroupItem);
	QIcon icon;
	icon.addFile("images/folder.png");
	//parItem->addChild(item);
	item->setText(colItem,getFinalfolder(dir));
	
	item->setIcon(colItem, icon);
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
	//item->setCheckState(colItem, Qt::Checked);
	item->setData(colItem, Qt::UserRole, QVariant(dir));
	item->setData(colItem, Qt::UserRole + 1, QVariant("itGroupItem"));

	return item;
	
}
void ModelWidget::addLeafItem(QTreeWidgetItem* parItem, const QString& dir)
{
	QTreeWidgetItem* item = new QTreeWidgetItem(itLeafItem);
	QIcon icon;
	icon.addFile("images/model.png");
	parItem->addChild(item);
	item->setText(colItem, getFinalfolder(dir));
	//MainWindow* temp = new MainWindow();
	DocumentCommon* dm;
	if (dir.back() == 'l') 
	{
		dm = DocumentModel::ReadModelFile(dir.toStdString().c_str());
	}
	else
	{
		dm = DocumentComponent::ReadComponentFile(dir.toStdString().c_str());
	}
	item->setIcon(colItem,dm->GetPixmap());
	
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
	item->setData(colItem, Qt::UserRole, QVariant(dir));
	item->setData(colItem, Qt::UserRole + 1, QVariant("itLeafItem"));
	parItem->addChild(item);
}
void ModelWidget::runLeafItem()
{
	MainWindow*  mywid=dynamic_cast<MainWindow*>(Parent);
	QTreeWidgetItem* parItem = myTreewid->currentItem();
	mywid->openFile(parItem->data(colItem, Qt::UserRole).toString());
}

void ModelWidget::insert()
{
	QTreeWidgetItem* parItem = myTreewid->currentItem();
	if (!parItem) {
		return;
	}
	QString fileName = parItem->data(colItem, Qt::UserRole).toString();
	if(fileName!="")
		(dynamic_cast<MainWindow*> (Parent))->insertCom(fileName);

}

void ModelWidget::curItemChangedSlot(QTreeWidgetItem* current, QTreeWidgetItem* previous)
{
	if (!current) return;
	/*if (!current) return;
	static int times = 0;
	/if (times == 1) {
		times = 0; return;
	}*/
	if (previous)previous->setBackground(0, QBrush());
	QBrush m_brush = QBrush(QColor(193, 210, 240));
	current->setBackground(0, m_brush);
	//mouseReleaseEvent(current);
	//times++;

	//测试
	QTreeWidgetItem* curr_item = current;
	QString sItemText = curr_item->text(0);

	QPixmap pm = GetPixmap(curr_item);
	if (!pm.isNull())
	{
		myImageShowDlg->setWindowTitle(sItemText);
		myImageShowDlg->setGeometry(QCursor::pos().x() + 50, QCursor::pos().y(), 810, 510);
		myImageShowDlg->SetPixmap(pm);
		myImageShowDlg->show();
	}
}

void ModelWidget::leaveEvent(QEvent* event)
{
	myImageShowDlg->hide();
}

void ModelWidget::mouseMoveSlot(QMouseEvent* event)
{
	static QTreeWidgetItem* old_item;

	QModelIndex index = myTreewid->indexAt(event->pos());
	if (!index.isValid()) {
		//QToolTip::hideText();
		myImageShowDlg->hide();
		return;
	}

	QTreeWidgetItem* curr_item = myTreewid->itemAt(event->pos());
	QString sItemText = curr_item->text(0);
	//QToolTip::showText(event->globalPos(), sItemText);

	if (curr_item == old_item)
	{
		return;
	}

	QPixmap pm = GetPixmap(curr_item);
	if (!pm.isNull())
	{
		myImageShowDlg->setWindowTitle(sItemText);
		myImageShowDlg->setGeometry(event->globalPos().x() + 50, event->globalPos().y(), 810, 510);
		myImageShowDlg->SetPixmap(pm);
		myImageShowDlg->show();
	}
	old_item = curr_item;
}

QPixmap ModelWidget::GetPixmap(QTreeWidgetItem* theItem)
{
	QPixmap pm;
	QString fileName = theItem->data(colItem, Qt::UserRole).toString();

	DocumentCommon* dm;
	if (!fileName.isEmpty() && fileName.back() == 'l')
	{
		dm = DocumentModel::ReadModelFile(fileName.toStdString().c_str());
	}
	else
	{
		dm = DocumentComponent::ReadComponentFile(fileName.toStdString().c_str());
	}

	if(dm)
		pm = dm->GetPixmap();

	return pm;
}

void ModelWidget::enterItem(QTreeWidgetItem* item,int col)
{

}