#include "voxelWidget.h"
#include "global.h"
#include <QStringListModel>
#include <QButtonGroup>
#include "voxel.h"
#include <QStandardItemModel>
#include <AIS_Shape.hxx>
#include <BRepAlgoAPI_Common.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <BRepAlgoAPI_Cut.hxx>
#include "DocumentCommon.h"

//球
pageSphere::pageSphere()
{

}

pageSphere::pageSphere(QWidget* parnt) : QWidget(parnt)
{
    InitUi();
}

void pageSphere::InitUi()
{
    QString sPatten = "^\\((-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?\\)$";
    QRegExp pntExp(sPatten);//三维点坐标正则表达式
    QValidator* pntValid = new QRegExpValidator(pntExp, this);
    QRegExp realExp("^(-?\\d+)(\\.\\d+)?$");//实型数正则表达式
    //^(-?\\d+)(\\.\\d+)?$
    QValidator* realValid = new QRegExpValidator(realExp, this);

    centerLabel = new QLabel(this);
    centerLabel->setText("球心点");
    centerLabel->setGeometry(30, 30, 200, 40);
    
    centerInput = new QLineEdit(this);
    centerInput->setText("(0,0,0)");
    centerInput->setValidator(pntValid);
    centerInput->setGeometry(110, 30, 200, 40);

    radiueLabel = new QLabel(this);
    radiueLabel->setText("球半径");
    radiueLabel->setGeometry(30, 80, 200, 40);

    radiusInput = new QLineEdit(this);
    radiusInput->setText("10.0");
    radiusInput->setValidator(realValid);
    //QDoubleValidator* dblValid = new QDoubleValidator();
    //radiusInput->setValidator(dblValid);
    radiusInput->setGeometry(110, 80, 200, 40);
}

bool pageSphere::UpdateUi(bool bUpdateUI)
{
    if (bUpdateUI)
    {//球心
        QString sCenter = centerInput->text();
        bool b = GetPoint(sCenter, m_center);

        QString sRadius = radiusInput->text();
        m_radius = sRadius.toDouble();
    }
    else {
        //用数据更新界面
        QString sCenter = GetPointString(m_center);
        centerInput->setText(sCenter);

        QString sRadius = DoubleToQString(m_radius);
        //sRadius = sRadius.setNum(m_radius, 'f', 2);
        radiusInput->setText(sRadius);
    }

    return true;
}

void pageSphere::SetData(voxel* aVox)
{
    assert(aVox->GetType() == SPHERE);

    mySphere* aSphere = (mySphere*)aVox;
    m_center = aSphere->GetCenter();
    m_radius = aSphere->GetRadius();

    UpdateUi(false);
}

//用界面刷新数据
void pageSphere::GetData(voxel* aVox)
{
    UpdateUi();

    assert(aVox->GetType() == SPHERE);

    mySphere* aSphere = (mySphere*)aVox;
    aSphere->SetCenter(m_center);
    aSphere->SetRadius(m_radius);

}


//长方体
pageBox::pageBox()
{}

pageBox::pageBox(QWidget* parnt)
    : QWidget( parnt )
{
    InitUi();
}

bool pageSphere::CheckInput()
{
    return true;
}

void pageBox::InitUi()
{
    QRegExp pntExp("^\\((-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?\\)$");//三维点坐标正则表达式
    QValidator* pntValid = new QRegExpValidator(pntExp, this);
    QRegExp realExp("^(-?\\d+)(\\.\\d+)?$");//实型数正则表达式
    QValidator* realValid = new QRegExpValidator(realExp, this);

    lblCorner = new QLabel(this);
    lblCorner->setText("角点");
    lblCorner->setGeometry(10, 10, 200, 40);
    cornerInput = new QLineEdit(this);
    cornerInput->setText("(0,0,0)");
    cornerInput->setValidator(pntValid);
    cornerInput->setGeometry(110, 10, 200, 40);

    lblZDir = new QLabel(this);
    lblZDir->setText("高度方向");
    lblZDir->setGeometry(10, 60, 200, 40);
    ZdirInput = new QLineEdit(this);
    ZdirInput->setText("(0,0,1)");
    ZdirInput->setValidator(pntValid);
    ZdirInput->setGeometry(110, 60, 200, 40);

    lblXDir = new QLabel(this);
    lblXDir->setText("长度方向");
    lblXDir->setGeometry(10, 110, 200, 40);
    XdirInput = new QLineEdit(this);
    XdirInput->setText("(1,0,0)");
    XdirInput->setValidator(pntValid);
    XdirInput->setGeometry(110, 110, 200, 40);

    lblLen = new QLabel(this);
    lblLen->setText("长度");
    lblLen->setGeometry(10, 160, 200, 40);
    lenInput = new QLineEdit(this);
    lenInput->setText("20");
    lenInput->setValidator(realValid);
    lenInput->setGeometry(110, 160, 200, 40);

    lblWid = new QLabel(this);
    lblWid->setText("宽度");
    lblWid->setGeometry(10, 210, 200, 40);
    widInput = new QLineEdit(this);
    widInput->setText("10");
    widInput->setValidator(realValid);
    widInput->setGeometry(110, 210, 200, 40);

    lblHeight = new QLabel(this);
    lblHeight->setText("高度");
    lblHeight->setGeometry(10, 260, 200, 40);
    heightInput = new QLineEdit(this);
    heightInput->setText("30");
    heightInput->setValidator(realValid);
    heightInput->setGeometry(110, 260, 200, 40);
}

bool pageBox::CheckInput()
{
    return true;
}

bool pageBox::UpdateUi(bool bUpdateUI)
{
    if (bUpdateUI)
    {
        //角点
        QString sCorner = cornerInput->text();
        bool b = GetPoint(sCorner, m_corner);
        if (!b)
        {
            return false;
        }

        //Z方向
        QString sZDir = ZdirInput->text();
        b = GetDirection(sZDir, m_ZDir);

        //X方向
        QString sXDir = XdirInput->text();
        b = GetDirection(sXDir, m_XDir);

        //长度
        QString sLen = lenInput->text();
        m_length = sLen.toDouble();

        //宽度
        QString sWid = widInput->text();
        m_width = sWid.toDouble();

        //高度
        QString sHit = heightInput->text();
        m_height = sHit.toDouble();
    }
    else {
        QString sCorner = GetPointString(m_corner);
        cornerInput->setText(sCorner);

        QString sZDir = GetPointString(m_ZDir);
        ZdirInput->setText(sZDir);

        QString sXDir = GetPointString(m_XDir);
        XdirInput->setText(sXDir);

        QString sLen = DoubleToQString(m_length);
        lenInput->setText(sLen);

        QString sWid = DoubleToQString(m_width);
        widInput->setText(sWid);

        QString sHit = DoubleToQString(m_height);
        heightInput->setText(sHit);
    }

    return true;
}

//用aVox数据刷新界面
void pageBox::SetData(voxel* aVox)
{
    assert(aVox->GetType() == BOX);

    myBox* aBox = (myBox*)aVox;

    aBox->GetData(m_length, m_width, m_height, m_corner, m_XDir, m_ZDir);

    UpdateUi(false);
}

//用界面刷新数据
void pageBox::GetData(voxel* aVox)
{
    UpdateUi();

    assert(aVox->GetType() == BOX);

    myBox* aBox = (myBox*)aVox;

    aBox->SetData(m_length, m_width, m_height, m_corner, m_XDir, m_ZDir);
}

/////
//圆柱体
pageCylinder::pageCylinder()
{}

pageCylinder::pageCylinder(QWidget* parnt)
    : QWidget(parnt)
{
    InitUi();
}

void pageCylinder::InitUi()
{
    QRegExp pntExp("^\\((-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?[,](-?\\d+)(\\.\\d+)?\\)$");//三维点坐标正则表达式
    QValidator* pntValid = new QRegExpValidator(pntExp, this);
    QRegExp realExp("^(-?\\d+)(\\.\\d+)?$");//实型数正则表达式
    QValidator* realValid = new QRegExpValidator(realExp, this);

    lblBtnCenter = new QLabel(this);
    lblBtnCenter->setText("底面中心");
    lblBtnCenter->setGeometry(10, 10, 200, 40);
    btnCenterInput = new QLineEdit(this);
    btnCenterInput->setText("(0,0,0)");
    btnCenterInput->setValidator(pntValid);
    btnCenterInput->setGeometry(110, 10, 200, 40);

    lblZDir = new QLabel(this);
    lblZDir->setText("高度方向");
    lblZDir->setGeometry(10, 60, 200, 40);
    ZdirInput = new QLineEdit(this);
    ZdirInput->setText("(0,0,1)");
    ZdirInput->setValidator(pntValid);
    ZdirInput->setGeometry(110, 60, 200, 40);

    lblRadius = new QLabel(this);
    lblRadius->setText("半径");
    lblRadius->setGeometry(10, 110, 200, 40);
    radiusInput = new QLineEdit(this);
    radiusInput->setText("20");
    radiusInput->setValidator(realValid);
    radiusInput->setGeometry(110, 110, 200, 40);

    lblHeight = new QLabel(this);
    lblHeight->setText("高度");
    lblHeight->setGeometry(10, 160, 200, 40);
    heightInput = new QLineEdit(this);
    heightInput->setText("30");
    heightInput->setValidator(realValid);
    heightInput->setGeometry(110, 160, 200, 40);
}

bool pageCylinder::CheckInput()
{
    return true;
}

bool pageCylinder::UpdateUi(bool bUpdateUI)
{
    if (bUpdateUI)
    {
        //底面中心
        QString sCenter = btnCenterInput->text();
        bool b = GetPoint(sCenter, m_btnCenter);
        if (!b)
        {
            return false;
        }

        //Z方向
        QString sZDir = ZdirInput->text();
        b = GetDirection(sZDir, m_ZDir);

        //半径
        QString sRadius = radiusInput->text();
        m_radius = sRadius.toDouble();

        //高度
        QString sHit = heightInput->text();
        m_height = sHit.toDouble();
    }
    else {
        QString sBtnCenter = GetPointString(m_btnCenter);
        btnCenterInput->setText(sBtnCenter);

        QString sZdir = GetPointString(m_ZDir);
        ZdirInput->setText(sZdir);

        QString sRadius = DoubleToQString(m_radius);
        radiusInput->setText(sRadius);

        QString sHeiht = DoubleToQString(m_height);
        heightInput->setText(sHeiht);
    }

    return true;
}

//用aVox数据刷新界面
void pageCylinder::SetData(voxel* aVox)
{
    assert(aVox->GetType() == CYLINDER);

    myCylinder* aCyl = (myCylinder*)aVox;

    aCyl->GetData(m_radius,m_height,m_btnCenter,m_ZDir);

    UpdateUi(false);
}
//用界面刷新数据
void pageCylinder::GetData(voxel* aVox)
{
    UpdateUi();

    assert(aVox->GetType() == CYLINDER);

    myCylinder* aCyl = (myCylinder*)aVox;

    aCyl->SetData(m_radius, m_height, m_btnCenter, m_ZDir);
}

////////////////////////////////////
//主界面，体素建模
voxelWidget::voxelWidget()
{}

voxelWidget::voxelWidget(QWidget* prnt, DocumentCommon* doc3d) : QDockWidget( prnt)
{
    m_radioItem = BASE;
    myContext = doc3d->getContext();
    myDocument = doc3d;

    InitUi();
}

voxelWidget::~voxelWidget()
{}

void voxelWidget::InitUi()
{
    this->setWindowTitle(tr("体素建模"));
    this->setFeatures(QDockWidget::NoDockWidgetFeatures);

	//上下布局
	verticalLayout = new QVBoxLayout();
    verticalLayout->setContentsMargins(0, 0, 0, 0);

	//上面的窗口
    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
    listView = new QListView();
    listView->setObjectName(QString::fromUtf8("listView"));
    QStringListModel* model = new QStringListModel();
    listView->setModel(model);       //useList是个QListView
    connect(listView, &QListView::clicked,this, &voxelWidget::voxelSelectChanged);
    //listView->setGeometry(QRect(10, 10, 200, 150));
    horizontalLayout->addWidget(listView,100);

    QVBoxLayout * vLayout1 = new QVBoxLayout();
    widgetRight = new QWidget();
    //widgetRight->setGeometry(QRect(300, 10, 400, 200));
    //widgetRight->setObjectName(QString::fromUtf8("widgetRight"));
    groupBox = new QGroupBox(widgetRight);
    groupBox->setObjectName(QString::fromUtf8("groupBox"));
    groupBox->setTitle("增加体素");
    //groupBox->setGeometry(QRect(5, 5, 300, 150));
    groupBox->setMinimumHeight(300);
    pushButtonAdd = new QPushButton(groupBox);
    pushButtonAdd->setText("添加");
    pushButtonAdd->setObjectName(QString::fromUtf8("pushButtonAdd"));
    connect(pushButtonAdd, &QPushButton::clicked, this, &voxelWidget::voxelAdd);
    pushButtonAdd->setGeometry(QRect(30, 30, 60, 30));
    comboBox = new QComboBox(groupBox);
    comboBox->addItem("Sphere"); comboBox->addItem("Box"); comboBox->addItem("Cylinder");
    comboBox->setObjectName(QString::fromUtf8("comboBoxType"));
    comboBox->setGeometry(QRect(90, 30, 90, 30));
    connect(comboBox, SIGNAL(activated(QString)), this, SLOT(voxelTypeChanged()));
    radioGroup = new QButtonGroup(groupBox);
    radioGroup->setExclusive(true);
    radioButtonUnion = new QRadioButton(groupBox);
    radioButtonUnion->setText("并");
    radioButtonUnion->setObjectName(QString::fromUtf8("radioButtonUnion"));
    radioButtonUnion->setGeometry(QRect(60, 80, 60, 25));
    radioButtonUnion->setChecked(true);
    radioButtonUnion->setEnabled(false);
    radioGroup->addButton(radioButtonUnion);
    radioButtonCommon = new QRadioButton(groupBox);
    radioButtonCommon->setText("交");
    radioButtonCommon->setObjectName(QString::fromUtf8("radioButtonCommon"));
    radioButtonCommon->setGeometry(QRect(60, 120, 60, 25));
    radioButtonCommon->setEnabled(false);
    radioGroup->addButton(radioButtonCommon);
    radioButtonSub1 = new QRadioButton(groupBox);
    radioButtonSub1->setText("被差");
    radioButtonSub1->setObjectName(QString::fromUtf8("radioButtonSub1"));
    radioButtonSub1->setGeometry(QRect(60, 160, 60, 25));
    radioButtonSub1->setEnabled(false);
    radioGroup->addButton(radioButtonSub1);
    radioButtonSub2 = new QRadioButton(groupBox);
    radioButtonSub2->setText("主差");
    radioButtonSub2->setObjectName(QString::fromUtf8("radioButtonSub2"));
    radioButtonSub2->setGeometry(QRect(60, 200, 60, 25));
    radioButtonSub2->setEnabled(false);
    radioGroup->addButton(radioButtonSub2);
    connect(radioGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(radioClicked(QAbstractButton*)));
    pushButtonApply = new QPushButton(widgetRight);
    pushButtonApply->setObjectName(QString::fromUtf8("pushButtonApply"));
    pushButtonApply->setGeometry(QRect(30, 260, 90, 25));
    pushButtonApply->setText("应用");
    connect(pushButtonApply, &QPushButton::clicked, this, &voxelWidget::voxelApply);
    pushButtonDelete = new QPushButton(widgetRight);
    pushButtonDelete->setObjectName(QString::fromUtf8("pushButtonDelete"));
    //pushButtonDelete->setGeometry(QRect(30, 220, 93, 28));
    pushButtonDelete->setText("删除");
    connect(pushButtonDelete, &QPushButton::clicked, this, &voxelWidget::voxelDelete);
    pushButtonRegen = new QPushButton(widgetRight);
    pushButtonRegen->setObjectName(QString::fromUtf8("pushButtonRegen"));
    //pushButtonRegen->setGeometry(QRect(30, 250, 93, 28));
    pushButtonRegen->setText("重构");
    connect(pushButtonRegen, &QPushButton::clicked, this, &voxelWidget::voxelRegen);

    vLayout1->addWidget(groupBox);
    vLayout1->addSpacing(30);
    vLayout1->addWidget(pushButtonDelete);
    vLayout1->addSpacing(30);
    vLayout1->addWidget(pushButtonRegen);
    vLayout1->addSpacing(10);
    widgetRight->setLayout(vLayout1);
    horizontalLayout->addWidget(widgetRight,100);
    //horizontalLayout->addLayout(vLayout1,100);

    verticalLayout->addLayout(horizontalLayout);

    //下面的窗口
    StackedWidget = new QStackedLayout(this);
    pageSphere1 = new pageSphere(this);
    StackedWidget->addWidget(pageSphere1);
    pageBox1 = new pageBox(this);
    StackedWidget->addWidget(pageBox1);
    pageCyl1 = new pageCylinder(this);
    StackedWidget->addWidget(pageCyl1);
    verticalLayout->addLayout(StackedWidget);

    //retranslateUi(Form);
    QMetaObject::connectSlotsByName(this);

    //总布局
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->setSpacing(20);
    verticalLayout->addLayout(StackedWidget);

    QWidget* widget = new QWidget();
    this->setWidget(widget);
    widget->setMinimumWidth(400);
    widget->setMaximumWidth(600);
    widget->setLayout(verticalLayout);
}

bool voxelWidget::CheckInput()
{
    return true;
}

bool voxelWidget::UpdateUi(bool bUpdateUI)
{
    return true;
}

//自动产生体素名称
void voxelWidget::UpdateVoxelName(voxel* aVox)
{
    QString sVox = "Vox_";

    if (m_lstVoxels.size() == 0)
    {
        sVox += "1";
        aVox->SetName(sVox.toStdString().c_str());
        return;
    }

    int i = 1;
    while (1)
    {
        string sVox_i = sVox.toStdString();
        sVox_i += to_string(i++);

        bool bFound = false;
        auto itr = m_lstVoxels.begin();
        for (; itr != m_lstVoxels.end(); ++itr)
        {
            voxel* pVox = *itr;

            if (pVox->GetName() == sVox_i)
            {
                //aVox->SetName(sVox_i.c_str());
                //return;
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            aVox->SetName(sVox_i.c_str());
            break;
        }
    } 
}

//根据数据更新界面: 类型和布尔操作，体素的数据
void voxelWidget::SetData(voxel* aVox)
{
    //设置类型
    comboBox->setCurrentIndex(aVox->GetType());

    //设置布尔操作
    boolOperator btyp = aVox->GetBoolType();
    switch (btyp)
    {
    case BASE:
        radioButtonUnion->setChecked(true);
        radioButtonUnion->setEnabled(false);
        radioButtonCommon->setEnabled(false);
        radioButtonSub1->setEnabled(false);
        radioButtonSub2->setEnabled(false);
        break;
    case UNION:
        radioButtonUnion->setChecked(true);
        radioButtonUnion->setEnabled(true);
        radioButtonCommon->setEnabled(true);
        radioButtonSub1->setEnabled(true);
        radioButtonSub2->setEnabled(true);
        break;
    case COMMAN:
        radioButtonUnion->setChecked(false);
        radioButtonUnion->setEnabled(true);
        radioButtonCommon->setEnabled(true);
        radioButtonSub1->setEnabled(true);
        radioButtonCommon->setChecked(true);
        radioButtonSub2->setEnabled(true);
        break; 
    case SUB1:
        radioButtonUnion->setChecked(false);
        radioButtonUnion->setEnabled(true);
        radioButtonCommon->setEnabled(true);
        radioButtonSub1->setEnabled(true);
        radioButtonSub1->setChecked(true);
        radioButtonSub2->setEnabled(true);
        break;
    case SUB2:
        radioButtonUnion->setEnabled(true);
        radioButtonCommon->setEnabled(true);
        radioButtonSub1->setEnabled(true);
        radioButtonSub2->setChecked(true);
        radioButtonSub2->setEnabled(true);
        break;
    default:
        break;
    }

    //设置体素的数据
    voxelTypeChanged();
    switch (aVox->GetType())
    {
    case SPHERE:
        pageSphere1->SetData(aVox);
        break;
    case BOX:
        pageBox1->SetData(aVox);
        break;
    case CYLINDER:
        pageCyl1->SetData(aVox);
        break;
    default:
        break;
    }
}

//用界面刷新数据
void voxelWidget::GetData(voxel* aVox)
{
    //获得类型
    int nType = comboBox->currentIndex();
    aVox->SetType(nType);

    //获得布尔类型
    aVox->SetBoolType((boolOperator)m_radioItem);

    //获得体素数据
    switch (aVox->GetType())
    {
    case SPHERE:
        pageSphere1->GetData(aVox);
        break;
    case BOX:
        pageBox1->GetData(aVox);
        break;
    case CYLINDER:
        pageCyl1->GetData(aVox);
        break;
    default:
        break;
    }
}

//用一组体素刷新界面，从文档读入时使用
void voxelWidget::SetListVoxels(list<voxel*> lstVoxes)
{
    //增加listView
    QStringListModel* smdl = (QStringListModel*)listView->model();
    QStringList strlist;
    auto itr = lstVoxes.begin();
    for (; itr != lstVoxes.end(); ++itr)
    {
        voxel* pVox = *itr;
        strlist.append(pVox->GetName().c_str());
    }
    smdl->setStringList(strlist);

    //选中第一个
    //QStandardItemModel* lstVewModl = new QStandardItemModel(strlist.size(), 1);// 行列
    //listView->setModel(lstVewModl);
    QStandardItemModel* lstVewModl = (QStandardItemModel*)listView->model();
    QModelIndex qindex = lstVewModl->index(0, 0);   //默认选中 index=0
    listView->setCurrentIndex(qindex);

    //更新显示第一个
    voxelSelectChanged();
}

list<voxel*> voxelWidget::GetVoxels() const
{
    return m_lstVoxels;
}

//体素类型改变响应函数
void voxelWidget::voxelTypeChanged()
{
    int nType = comboBox->currentIndex();
    
    StackedWidget->setCurrentIndex(nType);
}

void voxelWidget::voxelAdd()
{
    //增加一个体素到链表，自动编号给体素命名vox1,vox2,...
    voxel* aVoxel = nullptr;
    switch (comboBox->currentIndex())
    {
    case 0:
        aVoxel = new mySphere();
        pageSphere1->GetData(aVoxel);
        break;
    case 1:
        aVoxel = new myBox();
        pageBox1->GetData(aVoxel);
        break;
    case 2:
        aVoxel = new myCylinder();
        pageCyl1->GetData(aVoxel);
        break;
    default:
        break;
    }

    assert(aVoxel);

    //获得主界面的数据：也就是 布尔操作 类型
    GetData(aVoxel);
    UpdateVoxelName(aVoxel);//更新AVoxel的名字
    m_lstVoxels.push_back(aVoxel);

    ////刷新界面
    //SetListVoxels(m_lstVoxels);
    QStringListModel* smdl = (QStringListModel*) listView->model();
    QStringList strlist = smdl->stringList();
    strlist.append(aVoxel->GetName().c_str());
    smdl->setStringList(strlist);

    //设置选择项目
    //QStandardItemModel* lstVewModl = new QStandardItemModel(m_lstVoxels.size(), 1);// 行列
    //listView->setModel(lstVewModl);
    QStandardItemModel* lstVewModl = (QStandardItemModel*)listView->model();
    int index = m_lstVoxels.size() - 1;
    QModelIndex qindex = lstVewModl->index(index, 0);   //默认选中 index
    listView->setCurrentIndex(qindex);

     //只要增加了，就允许选择bool操作类型，第一个不允许选择，只能是机体base
    //radioButtonUnion->setChecked(true);
    radioButtonUnion->setEnabled(true);
    radioButtonCommon->setEnabled(true);
    radioButtonSub1->setEnabled(true);
    radioButtonSub2->setEnabled(true);

    //恢复
    if (m_radioItem == 0)
    {
        m_radioItem = 1;
    }

    voxelRegen();
}

//用当前listView中的选中行进行更新
//如果listView为空，或者没有选中行，则不操作
void voxelWidget::voxelApply()
{
    //根据界面的内容，更新数据
    QStringListModel* smdl = (QStringListModel*)listView->model();
    QStringList strlist = smdl->stringList();
    if (strlist.empty())
    {
        return;
    }

    QModelIndex index = listView->currentIndex();
    int theRow = index.row();
    if (theRow < 0 || theRow > strlist.size())
    {
        return;
    }

    QString sVox = strlist.at(theRow);
    auto itr = m_lstVoxels.begin();
    voxel* theVox = nullptr;
    for (; itr != m_lstVoxels.end(); ++itr)
    {
        voxel* pVox = *itr;
        if (sVox == pVox->GetName().c_str())
        {
            theVox = pVox;
            break;
        }
    }
    if (theVox == nullptr)
    {
        return;
    }

    //需要代替lst中的元素
    voxel* newVox = nullptr;
    int nType = comboBox->currentIndex();
    if (nType == 0)
    {
        newVox = new mySphere();
    } 
    else if (nType == 1)
    {
        newVox = new myBox();
    }
    else if(nType == 2)
    {
        newVox = new myCylinder();
    }

    newVox->SetName(theVox->GetName().c_str());
    m_lstVoxels.insert(itr, newVox);
    m_lstVoxels.erase(itr);

    GetData(newVox);

    delete theVox;

    //如果是第一个，则为基体
    if (theRow == 0)
    {
        newVox->SetBoolType(BASE);
    }

    voxelRegen();

    ////显示更新的体素
    //TopoDS_Shape shp = newVox->MakeShape();
    //myVoxShape = new AIS_Shape(shp);
    //myContext->Display(myVoxShape, true);
}

void voxelWidget::voxelDelete()
{
    QStringListModel* smdl = (QStringListModel*)listView->model();
    QStringList strlist = smdl->stringList();
    if (strlist.empty())
    {
        return;
    }
    QModelIndex index = listView->currentIndex();
    if (!index.isValid() || index.row() < 0 || index.row() > strlist.size())
    {
        return;
    }

    QString sVox = strlist.at(index.row());
    strlist.removeAt(index.row());
    
    smdl->setStringList(strlist);
    
    if (strlist.count() == 0)
    {
        radioButtonUnion->setChecked(true);
        radioButtonUnion->setEnabled(false);
        radioButtonCommon->setEnabled(false);
        radioButtonSub1->setEnabled(false);
        radioButtonSub2->setEnabled(false);
    }
    else {
        radioButtonUnion->setChecked(true);
        radioButtonUnion->setEnabled(true);
        radioButtonCommon->setEnabled(true);
        radioButtonSub1->setEnabled(true);
        radioButtonSub2->setEnabled(true);
    }

    //从列表m_lstVoxels中删去
    auto itr = m_lstVoxels.begin();
    voxel* pVox;
    for (; itr != m_lstVoxels.end(); ++itr)
    {
        pVox = *itr;
        if (pVox->GetName().c_str() == sVox)
        {
            break;
        }
    }
    m_lstVoxels.erase(itr);
    delete pVox;

    //如果删除的是第一个体素，则第二个设置为基体
    if (index.row() == 0 && m_lstVoxels.size() > 0)
    {
        auto itr2 = m_lstVoxels.begin();
        voxel* pVox = *itr2;
        pVox->SetBoolType(BASE);
    }

    //选中下一个，或最后一个
    //QStandardItemModel* lstVewModl = new QStandardItemModel(strlist.size(), 1);// 行列
    //listView->setModel(lstVewModl);
    QStandardItemModel* lstVewModl = (QStandardItemModel*)listView->model();
    int theRow = index.row();
    if (theRow == strlist.size())
    {
        theRow--;
    }
    QModelIndex qindex = lstVewModl->index(theRow, 0);   //默认选中 index=0
    listView->setCurrentIndex(qindex);

    voxelSelectChanged();

    voxelRegen();
}

//从头到尾重构几何，并显示
void voxelWidget::voxelRegen()
{
    auto itr = m_lstVoxels.begin();
    TopoDS_Shape wholeShp;
    for (; itr != m_lstVoxels.end(); ++itr)
    {
        voxel* pVox = *itr;
        TopoDS_Shape shp = pVox->MakeShape();
        if (pVox->GetBoolType() == BASE)
        {
            wholeShp = shp;
        }
        else if (pVox->GetBoolType() == UNION)
        {
            wholeShp = BRepAlgoAPI_Fuse(wholeShp, shp);
        }
        else if (pVox->GetBoolType() == COMMAN)
        {
            wholeShp = BRepAlgoAPI_Common(wholeShp, shp);
        }
        else if (pVox->GetBoolType() == SUB1) //被差
        {
            wholeShp = BRepAlgoAPI_Cut(wholeShp, shp);
        }
        else if (pVox->GetBoolType() == SUB2) //主差
        {
            wholeShp = BRepAlgoAPI_Cut(shp, wholeShp);
        }
    }

    myCompoundShape = new AIS_Shape(wholeShp);
    //myContext->Erase(myVoxShape,true);
    myContext->RemoveAll(true);
    myContext->Display(myCompoundShape, true);

    myDocument->SetMainShape(wholeShp);
    myDocument->UpdateDisplay();
}

//列表行选中的响应函数
void voxelWidget::voxelSelectChanged()
{
    QStringListModel* smdl = (QStringListModel*)listView->model();
    QStringList strlist = smdl->stringList();
    if (strlist.empty())
    {
        return;
    }

    //strlist.at();
    QModelIndex index = listView->currentIndex();
    //index.row();
    QVariant varIndex = smdl->data(index);
    QString sVox = varIndex.toString();

    voxel* pVox = nullptr;
    auto itr = m_lstVoxels.begin();
    for (; itr != m_lstVoxels.end(); ++itr)
    {
        voxel* pVox_i = *itr;
        if (pVox_i->GetName().c_str() == sVox)
        {
            pVox = pVox_i;
            break;
        }
    }    
    assert(pVox);

    //
    SetData(pVox);

    if (pVox->GetType() == SPHERE)
    {
        comboBox->setCurrentIndex(0);
        voxelTypeChanged();
        pageSphere1->SetData(pVox);
    }
    else if (pVox->GetType() == BOX)
    {
        comboBox->setCurrentIndex(1);
        voxelTypeChanged();
        pageBox1->SetData(pVox);
    }
    else if (pVox->GetType() == CYLINDER)
    {
        comboBox->setCurrentIndex(2);
        voxelTypeChanged();
        pageCyl1->SetData(pVox);
    }
}

void voxelWidget::radioClicked(QAbstractButton* buttn)
{
    QString sButton = buttn->text();
    if (sButton == "并")
    {
        m_radioItem = UNION;
    }
    else if (sButton == "交")
    {
        m_radioItem = COMMAN;
    }
    else if (sButton == "被差")
    {
        m_radioItem = SUB1;
    }
    else if (sButton == "主差")
    {
        m_radioItem = SUB2;
    }
}
