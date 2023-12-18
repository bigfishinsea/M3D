#include "M3d.h"
#include "ModelLibWidget.h"
#include "mainwindow.h"
#include"DocumentModel.h"
#include "DocumentComponent.h"
#include <QtWidgets>
#include "ImageShowDlg.h"
#include "omp.h"
#include <QSettings>

QSettings setting_CurrentFolder("HKEY_CURRENT_USER\\SOFTWARE\\M3d\\Test", QSettings::NativeFormat);
static QString temp_CurrentFolder = setting_CurrentFolder.value("CurrentFolder", "").toString();

ModelLibWidget::ModelLibWidget(QWidget* parent) :QFrame(parent), Parent(parent)
{
	this->myDocument3d = (DocumentModel *) qobject_cast<MainWindow*>(parent)->getDocument();
	init();

	myImageShowDlg = new ImageShowDlg(this);
	this->setMouseTracking(true);
}

void ModelLibWidget::init()
{
	this->setFrameStyle(QFrame::Panel | QFrame::Plain);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, &ModelLibWidget::customContextMenuRequested, this, &ModelLibWidget::show_menu);

	WorkPathLabel = new QLabel();
	WorkPathLabel->setText("......");
	WorkPathLabel->setScaledContents(true);
	WorkPathLabel->setStyleSheet(" background-color: white ;");

	BrowseButton = new QPushButton();
	BrowseButton->setText("...");
	BrowseButton->setMaximumSize(30, 20);
	connect(BrowseButton, &QPushButton::clicked, this, &ModelLibWidget::addItem);
	
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

	myTreewid->setHeaderItem(new QTreeWidgetItem({ tr("��ǰ�ļ���")}));
	QVBoxLayout* HorizontalLayout2 = new QVBoxLayout(this);
	HorizontalLayout2->addLayout(HorizontalLayout1);
	HorizontalLayout2->addWidget(myTreewid);
	this->setLayout(HorizontalLayout2);
	initTree();
	additem(QDir::currentPath());

	if (temp_CurrentFolder != "")
	{
		additem(temp_CurrentFolder);
	}
}

QSize ModelLibWidget::sizeHint() const
{
	return QSize(Parent->width() / 3, Parent->height() / 2);
}

void ModelLibWidget::show_menu()
{
	QMenu* menu = new QMenu(this);
	QAction* openAct = new QAction(tr("��"), this);
	QAction* insertAct = new QAction(tr("����"), this);
	//Ϊ������ӹ�������
	connect(insertAct, &QAction::triggered, this, &ModelLibWidget::insert);
	connect(openAct, &QAction::triggered, this, &ModelLibWidget::runLeafItem);
	menu->addAction(openAct);
	menu->addSeparator();
	menu->addAction(insertAct);
	menu->exec(QCursor::pos());
}

void ModelLibWidget::initTree() {
	//myTreewid->setAutoExpandDelay();
	myTreewid->clear();
	QString dataStr;
	QIcon icon;
	icon.addFile("images/library.png"); 
	QTreeWidgetItem* item = new QTreeWidgetItem(ModelLibWidget::itTopItem);
	item->setIcon(colItem, icon);
	item->setText(colItem, "ģ�Ϳ�");
	item->setFlags( Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
	//item->setCheckState(colItem, Qt::Checked);
	item->setData(colItem, Qt::UserRole, QVariant(dataStr));
	item->setData(colItem, Qt::UserRole+1, QVariant("itTopItem"));
	myTreewid->addTopLevelItem(item);
	//
}

QString ModelLibWidget::getFinalfolder(const QString& fullPathName)
{
	int cnt = fullPathName.length(); //�ַ�������
	int i = fullPathName.lastIndexOf("/");//  ���һ�γ��ֵ�λ��
	QString str = fullPathName.right(cnt - i - 1); //��������ļ��е�����
	return str;
}


QList<QString> ModelLibWidget::getmdl(const QString& filepath)
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

void ModelLibWidget::addItem()
{
	QString dir;
	dir = QFileDialog::getExistingDirectory();
	setting_CurrentFolder.setValue("CurrentFolder", dir);
	additem(dir);
}

void ModelLibWidget::additem(QString dir)
{
	if (!dir.isEmpty()) {
		initTree();
		QTreeWidgetItem* parItem = myTreewid->topLevelItem(0);
		if (!dfs(parItem, dir))
		{
			QMessageBox msg;
			msg.setText("ѡȡ���ļ���ȱ����Ч��ģ���ļ�");
			msg.exec();
			return;
		}
		WorkPathLabel->setText(dir);
		//myTreewid->expandAll();
		myTreewid->expandItem(parItem);
	}
}

//������ǰ�ļ����������ļ��кͽڵ�
//��ÿһ���ļ��н��б���
//��Ҷ���ļ����ж��Ƿ����ģ��
//�����ڷ���false
//��һ���ļ����������ļ��з���false����������Ƿ����ģ��
//������ӵ����ڵ���
//

bool ModelLibWidget::dfs(QTreeWidgetItem* parItem,const QString& filepath) {
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
	vector<QString> vec_receiveFilePath;
	for (auto itr = receiveFilePath.begin(); itr < receiveFilePath.end(); itr++)
	{
		vec_receiveFilePath.push_back(*itr);
	}
	omp_set_num_threads(4);
#pragma omp parallel for
	for (int i = 0; i < vec_receiveFilePath.size(); ++i)
	{
		addLeafItem(parItem, vec_receiveFilePath[i]);
	}	
	//for (auto itr = receiveFilePath.begin(); itr < receiveFilePath.end(); itr++)
	//	addLeafItem(parItem, *itr);
	return flag;
}

QTreeWidgetItem* ModelLibWidget::addGroupItem(QTreeWidgetItem* parItem, const QString& dir)
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
void ModelLibWidget::addLeafItem(QTreeWidgetItem* parItem, const QString& dir)
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
void ModelLibWidget::runLeafItem()
{
	MainWindow*  mywid=dynamic_cast<MainWindow*>(Parent);
	QTreeWidgetItem* parItem = myTreewid->currentItem();
	mywid->openFile(parItem->data(colItem, Qt::UserRole).toString());
}

void ModelLibWidget::insert()
{
	QTreeWidgetItem* parItem = myTreewid->currentItem();
	if (!parItem) {
		return;
	}
	QString fileName = parItem->data(colItem, Qt::UserRole).toString();
	if(fileName!="")
		(dynamic_cast<MainWindow*> (Parent))->insertCom(fileName);

}

void ModelLibWidget::curItemChangedSlot(QTreeWidgetItem* current, QTreeWidgetItem* previous)
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

	//����
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

void ModelLibWidget::leaveEvent(QEvent* event)
{
	myImageShowDlg->hide();
}

void ModelLibWidget::mouseMoveSlot(QMouseEvent* event)
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

QPixmap ModelLibWidget::GetPixmap(QTreeWidgetItem* theItem)
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

void ModelLibWidget::enterItem(QTreeWidgetItem* item,int col)
{

}