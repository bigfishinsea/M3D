#include "moTreeDlg.h"
#include "cyberInfo.h"
#include <qheaderview.h>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStack>
#include "ModelicaModel.h"
#include <QMessageBox>

Q_DECLARE_METATYPE(ModelicaFileModel*);
Q_DECLARE_METATYPE(ModelicaModel*);

extern ModelicaFileModel* CreateMoModels(QString sMoFile);

string GetSentenceFromPosToChar(string& subStr, int& pos, char to)
{
	string rets;

	int nXKH = 0; //()
	int nZKH = 0; //[]
	int nYH = 0; //"

	char c;
	int i = pos;
	int len = subStr.length();
	while (i < len) {
		c = subStr[i++];

		//如果括号都匹配
		if (c == to && nXKH == 0 && nZKH == 0 && (nYH % 2) == 0) {
			rets += c;
			break;
		}
		//记录
		if (c == '(' && nYH%2==0) nXKH++; 
		if (c == ')' && nYH%2==0) nXKH--;
		if (c == '[' && nYH%2==0) nZKH++;
		if (c == ']' && nYH%2==0) nZKH--;
		if (c == '\"') nYH++;

		//去注释
		bool bComment = false;
		if (i == 2 && subStr[i - 2] == '/' && c == '/')
		{
			bComment = true;
		}
		if (i > 2 && subStr[i - 3] == '\n' && subStr[i - 2] == '/' && c == '/') //行注释
		{
			bComment = true;
		}
		if(bComment)
		{
			while (i < len)
			{
				c = subStr[i++];
				if (c == '\r' || c == '\n')
				{
					break;
				}
			}
		}
		if (i>=2 && subStr[i - 2] == '/' && c == '*') //块注释
		{
			while (i < len)
			{
				c = subStr[i++];
				if (i >= 2 && subStr[i - 2] == '*' && c == '/')
				{
					break;
				}
			}
		}
		if (i+3<len && c == '<' && (subStr[i] == 'h' || subStr[i] == 'H') && (subStr[i+1] == 't' || subStr[i+1] == 'H') &&
			(subStr[i+2] == 'm' || subStr[i+2] == 'M') && (subStr[i+3] == 'l' || subStr[i+3] == 'L'))//HTML注释
		{
			while (i < len-4)
			{
				c = subStr[i++];
				if (c == '<' && subStr[i]=='/' && (subStr[i+1] == 'h' || subStr[i+1] == 'H') && (subStr[i + 2] == 't' || subStr[i + 2] == 'H') &&
					(subStr[i + 3] == 'm' || subStr[i + 3] == 'M') && (subStr[i + 4] == 'l' || subStr[i + 4] == 'L'))
				{
					i += 6;
					c = ' ';
					break;
				}
			}
		}

		rets += c;
	}
	pos = i;

	return rets;
}

//找前缀
QString FindPrefix(const char* sMoPath)
{
	QString theMdlPrix;

	//当前路径的package.mo
	QString sCurrentPackage = sMoPath;
	if (sCurrentPackage.indexOf("package.mo") > 0)
	{//d:/w1/w2/package.mo
		;
	}
	else {//d:/w1/w2/abc.mo
		theMdlPrix = QFileInfo(sMoPath).baseName(); //abc
		sCurrentPackage.replace(theMdlPrix, "package"); //d:/w1/w2/package.mo
	}

	//向上找
	QString thePackageMO = sCurrentPackage;
	while (!QFileInfo(thePackageMO).exists())
	{
		//获得前缀
		int n1 = thePackageMO.lastIndexOf('/');
		assert(n1 > 0);
		if (n1 < 0)
		{
			break;
		}
		QString sPath = thePackageMO.left(n1);
		int n0 = thePackageMO.lastIndexOf('/');
		assert(n0 > 0);
		if (n0 < 0)
		{
			break;
		}
		if (theMdlPrix.isEmpty())
		{
			theMdlPrix = sPath.right(sPath.length() - n0 - 1); //w2
		}
		else {
			theMdlPrix = sPath.right(sPath.length() - n0 - 1) + "." + theMdlPrix; //w2.abc
		}

		//往上找
		thePackageMO = sPath.left(n0);
		thePackageMO += "/";
		thePackageMO += "package.mo"; //d:/w1/package.mo
	}

	return theMdlPrix;
}

//递归
QFileInfoList RecurseGetFileList(QString path)
{
	QDir dir(path);
	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

	for (int i = 0; i != folder_list.size(); i++)
	{
		QString name = folder_list.at(i).absoluteFilePath();
		QFileInfoList child_file_list = RecurseGetFileList(name);
		file_list.append(child_file_list);
	}

	return file_list;
}

//得到package.mo同一目录下的全部mo文件，包括子目录
list<QString> GetMoFiles(QString sCurrentPackage)
{
	list<QString> lstMoFiles;

	if (!QFileInfo(sCurrentPackage).exists())
	{
		return lstMoFiles;
	}

	QString sPath = sCurrentPackage;
	sPath.replace("/package.mo", "");

	QFileInfoList files = RecurseGetFileList(sPath);
	for (int i = 0; i < files.size(); i++)
	{
		QString sFileFullName = files.at(i).absoluteFilePath();
		if (sFileFullName.indexOf(".mo") > 0)
		{
			lstMoFiles.push_back(sFileFullName);
		}
	}

	return lstMoFiles;
}

//递归创建model，放入pMdl下，thePackage路径下 D:/w1/w2/package.mo
void RecurseCreateMoModel(ModelicaFileModel* pMdl, const char* thePackagePath)
{
	if (!QFileInfo(thePackagePath).exists())
	{
		return;
	}

	//得到路径
	QString path = thePackagePath;
	path.replace("/package.mo", "");

	QDir dir(path);
	QFileInfoList file_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	QFileInfoList folder_list = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);

	//找到当前路径下的子路径，递归
	for (int i = 0; i != folder_list.size(); i++)
	{
		QString subPath = folder_list.at(i).absoluteFilePath();
		QString sBaseName = folder_list.at(i).baseName();
		QString subPackage = subPath;
		subPackage += "/package.mo";

		if (!QFileInfo(subPackage).exists())
		{
			continue;
		}
		//ModelicaFileModel* pSubMdl = new ModelicaFileModel();
		//pSubMdl->sFullName = pMdl->sFullName + "." + sBaseName.toStdString();
		//pSubMdl->sMoPath = subPackage.toStdString();
		ModelicaFileModel* pSubMdl = CreateMoModels(subPackage);
		RecurseCreateMoModel(pSubMdl, subPackage.toStdString().c_str());

		pMdl->lstChildren.push_back(pSubMdl);
		pSubMdl->pParent = pMdl;
	}

	//对单个mo文件处理
	for (int i = 0; i != file_list.size(); i++)
	{
		QString subPath = file_list.at(i).absoluteFilePath();
		//QString sBaseName = folder_list.at(i).baseName();
		//ModelicaFileModel* pChldMdl = new ModelicaFileModel();
		//pChldMdl->sFullName = pMdl->sFullName + "." + sBaseName.toStdString();
		//pChldMdl->sMoPath = subPath.toStdString();
		if (subPath.indexOf(".mo") > 0 && subPath != thePackagePath)
		{
			ModelicaFileModel* pChldMdl = CreateMoModels(subPath);
			pMdl->lstChildren.push_back(pChldMdl);
			pChldMdl->pParent = pMdl;
		}
	}
}

//判断行是否是class定义开始，如果是，返回模型简名和类型
bool IsClassDefining(QString sLine, QString & sNameReturn, QString& sTypeReturn)
{
	sLine.replace("replaceable ", " ");
	sLine.replace("expandable ", " ");
	sLine.replace("encapsulated ", " ");
	sLine.replace("partial ", " ");
	//去掉回车deng
	sLine.replace("\n", " ");
	sLine.replace("\t", " ");
	sLine = sLine.trimmed();

	sLine.replace("  ", " "); //只能有一个空格
	if (sLine.indexOf("package") == 0)
	{
		sTypeReturn = "package";

		int pos = sLine.indexOf("package");
		sLine = sLine.right(sLine.length() - pos - 8);

		sLine = sLine.trimmed();
		pos = sLine.indexOf(" ");
		int pos1 = sLine.indexOf('\"');
		if (pos1 > 0 && pos1 < pos)
		{
			pos = pos1;
		}

		sNameReturn = sLine.left(pos);

		return true;
	}
	if (sLine.indexOf("class") == 0)
	{
		sTypeReturn = "class";

		int pos = sLine.indexOf("class");
		sLine = sLine.right(sLine.length() - pos - 6);

		sLine = sLine.trimmed();
		pos = sLine.indexOf(" ");
		sNameReturn = sLine.left(pos);

		return true;
	}
	if (sLine.indexOf("model") == 0 )
	{
		sTypeReturn = "model";

		int pos = sLine.indexOf("model");
		sLine = sLine.right(sLine.length() - pos - 6);

		sLine = sLine.trimmed();
		pos = sLine.indexOf(" ");
		sNameReturn = sLine.left(pos);

		return true;
	}
	if (sLine.indexOf("block") == 0 )
	{
		sTypeReturn = "block";

		int pos = sLine.indexOf("block");
		sLine = sLine.right(sLine.length() - pos - 6);

		sLine = sLine.trimmed();
		pos = sLine.indexOf(" ");
		sNameReturn = sLine.left(pos);

		return true;
	}
	if (sLine.indexOf("function") == 0)
	{
		sTypeReturn = "function";

		int pos = sLine.indexOf("function");
		sLine = sLine.right(sLine.length() - pos - 9);

		sLine = sLine.trimmed();
		pos = sLine.indexOf(" ");
		sNameReturn = sLine.left(pos);

		return true;
	}
	if (sLine.indexOf("connector") == 0 )
	{
		sTypeReturn = "connector";

		int pos = sLine.indexOf("connector");
		sLine = sLine.right(sLine.length() - pos - 10);

		sLine = sLine.trimmed();
		pos = sLine.indexOf(" ");
		sNameReturn = sLine.left(pos);

		return true;
	}
	if (sLine.indexOf("record") == 0)
	{
		sTypeReturn = "record";

		int pos = sLine.indexOf("record");
		sLine = sLine.right(sLine.length() - pos - 7);

		sLine = sLine.trimmed();
		pos = sLine.indexOf(" ");
		sNameReturn = sLine.left(pos);

		return true;
	}
	if (sLine.indexOf("type") == 0)
	{
		sTypeReturn = "type";

		int pos = sLine.indexOf("type");
		sLine = sLine.right(sLine.length() - pos - 5);

		sLine = sLine.trimmed();
		pos = sLine.indexOf(" ");
		sNameReturn = sLine.left(pos);

		return true;
	}

	return false;
}

bool IsClassEnd(QString sLine, QString sModelName)
{
	sLine = sLine.trimmed();
	sLine.replace("  ", " ");

	QString sEndSen = "end " + sModelName + ";";
	if (sLine.indexOf(sEndSen) == 0)
	{
		return true;
	}

	return false;
}

//根据mo文件，创建模型树: 模型名字由文件中读取得到，不由路径名！！！
//一种是 .../package.mo
//另一种是，.../abc.mo
ModelicaFileModel* CreateMoModels(QString sMoFile)
{
	ModelicaFileModel* pModel = nullptr;

	if (!QFileInfo(sMoFile).exists())
	{
		return pModel;
	}

	//得到前缀
	//QString sPrefix = FindPrefix(sMoFile.toStdString().c_str());

	QFile qFile(sMoFile);
	qFile.open(QIODevice::ReadOnly);
	string sFullText = qFile.readAll();
	qFile.close();

	QStack<ModelicaFileModel*> mdlStack;
	//int nrefCount = 0; //统计引号个数;
	//int nkhCount = 0; //括号
	int pos = 0;
	while ( pos < sFullText.length()/*!qFile.atEnd()*/)
	{
		//QString sLine = qFile.readLine();

		//sLine = sLine.trimmed();
		//if (sLine.isEmpty()) continue; //空行

		//nrefCount += sLine.count("\"");
		//nkhCount += sLine.count("(");
		//nkhCount -= sLine.count(")");

		////引号和括号成对
		//if (nrefCount % 2 != 0) continue;
		//if (nkhCount != 0) continue;

		//先读一行，如果是模型定义则处理，否则读完一句;
		int oldPos = pos;
		QString sLine = GetSentenceFromPosToChar(sFullText, oldPos, '\n').c_str();
		QString sName, sType;
		if (IsClassDefining(sLine, sName, sType) )
		{
			ModelicaFileModel* pMdl = new ModelicaFileModel();
			pMdl->sName = /*sPrefix.toStdString() + "." +*/ sName.toStdString();
			pMdl->sMoPath = sMoFile.toStdString();

			if (mdlStack.size() == 0)
			{
				pModel = pMdl; //这是顶模型
			}
			else
			{
				ModelicaFileModel* pParent = mdlStack.top();
				if (pParent)
				{
					pParent->lstChildren.push_back(pMdl);
					pMdl->pParent = pParent;
				}	
			}

			mdlStack.push(pMdl);
			pos = oldPos;
			continue; //继续读;
		}

		//如果是方程或算法开始，则全部读完
		sLine = sLine.trimmed();
		if (sLine.indexOf("equation") == 0 || sLine.indexOf("algorithm") == 0 || sLine.indexOf("initial algorithm") == 0)
		{
			pos = oldPos;
			while (pos < sFullText.length())
			{
				sLine = GetSentenceFromPosToChar(sFullText, pos, ';').c_str();
				sLine = sLine.trimmed();
				if (sLine.indexOf("end ") == 0)
				{
					break;
				}
			}
		}

		//读一句
		sLine = GetSentenceFromPosToChar(sFullText, pos, ';').c_str();

		if (mdlStack.size() > 0)
		{
			ModelicaFileModel* pTopMdl = mdlStack.top();
			QString sShName = pTopMdl->sName.c_str();
			if (IsClassEnd(sLine, sShName))
			{
				mdlStack.pop();
			}
		}
	}

	return pModel;
}

//向上读取模型的前缀
//文件内的模型树，获得当前文件的顶模型，构造其子模型
ModelicaFileModel* GenerateModelicaFileModelTree(const char * sMoPath)
{
	ModelicaFileModel* theModel = nullptr;

	//1.向上找前缀
	QString theMdlPrix = FindPrefix(sMoPath);

	QString qMoPath = sMoPath;
	//2.向下读：如果是package.mo，则读取当前目录mo文件，并向下读取，构建模型树
	if (qMoPath.indexOf("package.mo")>0 && QFileInfo(sMoPath).exists())
	{
		//theModel = new ModelicaFileModel();
		//theModel->sMoPath = sMoPath;
		//theModel->sFullName = theMdlPrix.toStdString();
		//先创建package.mo的模型树
		theModel = CreateMoModels(sMoPath);
		//递归找该路径下的全部mo文件和子目录，并创建models
		RecurseCreateMoModel(theModel, sMoPath);
		////得到package文件路径下的全部mo文件，包括自己
		//list<QString> lstMoFiles = GetMoFiles(qMoPath);
		//auto itr = lstMoFiles.begin();
		//for (; itr != lstMoFiles.end(); ++itr)
		//{
		//	QString sMoFile = *itr;
		//	//创建单个mo文件的模型树
		//	ModelicaFileModel* pMdel = CreateMoModels(sMoFile);
		//	theModel->lstChildren.push_back(pMdel);
		//}
	}
	else {
		//否则，直接读单个mo文件，构造ModelicaFileModel树结构
		theModel = CreateMoModels(sMoPath);
	}

	return theModel;
}

MoTreeDlg::MoTreeDlg(QString sPath, QWidget* parent)
	: QDialog(parent)
{
	m_sMoPath = sPath;
	bAccepted = false;
	m_theModel = nullptr;

	QString sPngFile;
	sPngFile = "images/class.png";
	m_classIcon[0].addFile(sPngFile);
	sPngFile = "images/model.png";
	m_classIcon[1].addFile(sPngFile);
	sPngFile = "images/block.png";
	m_classIcon[2].addFile(sPngFile);
	sPngFile = "images/connector.png";
	m_classIcon[3].addFile(sPngFile);
	sPngFile = "images/package.png";
	m_classIcon[4].addFile(sPngFile);
	sPngFile = "images/function.png";
	m_classIcon[5].addFile(sPngFile);
	sPngFile = "images/type.png";
	m_classIcon[6].addFile(sPngFile);
	sPngFile = "images/record.png";
	m_classIcon[7].addFile(sPngFile);

	InitUi();
}

MoTreeDlg::~MoTreeDlg()
{
	//清除ModelicaFileModel
	delete m_theModel;
}

void MoTreeDlg::InitUi(/*QString sPath*/)
{
	setGeometry(600, 300, 500, 460);
	setFixedSize(500, 460);

	//树控件
	myTreeWidget = new QTreeWidget(this);
	myTreeWidget->setGeometry(10, 10, 480, 390);
	myTreeWidget->header()->setVisible(false);

	//OK_Cancel
	pushButtonOK = new QPushButton(this);
	pushButtonOK->setGeometry(200, 420, 80, 30);
	pushButtonOK->setText("确定");
	pushButtonCancel = new QPushButton(this);
	pushButtonCancel->setText("取消");
	pushButtonCancel->setGeometry(320, 420, 80, 30);
	
	connect(pushButtonCancel, &QPushButton::clicked, this, &QDialog::close);
	connect(myTreeWidget, &QTreeWidget::itemDoubleClicked, this, &MoTreeDlg::Accept);
	connect(pushButtonOK, &QPushButton::clicked, this, &MoTreeDlg::Accept);

	////产生模型树
	//m_theModel = GenerateModelicaFileModelTree(m_sMoPath.toStdString().c_str());
	////显示树
	//ShowModelTree(m_theModel);
	ShowModelTree();
}

void MoTreeDlg::RecursiveAddItems(QTreeWidgetItem* pParent, ModelicaFileModel* pModel)
{
	list<ModelicaFileModel*>::iterator itr = pModel->lstChildren.begin();
	for (; itr != pModel->lstChildren.end(); ++itr)
	{
		ModelicaFileModel* pChildMdl = *itr;

		QTreeWidgetItem* pItem = new QTreeWidgetItem();
		pItem->setText(0, pChildMdl->sName.c_str());
		pItem->setData(0, Qt::UserRole, QVariant::fromValue(pChildMdl));

		pParent->addChild(pItem);
		RecursiveAddItems(pItem, pChildMdl);
	}
}

void MoTreeDlg::ShowModelTree(ModelicaFileModel* pModel)
{
	////测试
	//pModel->sFullName = "Modelica";

	QTreeWidgetItem* pRoot = new QTreeWidgetItem();
	pRoot->setText(0,pModel->sName.c_str());
	pRoot->setData(0, Qt::UserRole, QVariant::fromValue(pModel));

	myTreeWidget->addTopLevelItem(pRoot);
	RecursiveAddItems(pRoot, pModel);
}

//显示全局加载的模型库
void MoTreeDlg::ShowModelTree()
{
	for (int i = 0; i < g_vModelicaModels.size(); i++)
	{
		ModelicaModel* pModel = g_vModelicaModels[i];
		QTreeWidgetItem* pRoot = new QTreeWidgetItem();
		pRoot->setText(0, pModel->m_sName.c_str());
		pRoot->setData(0, Qt::UserRole, QVariant::fromValue(pModel));

		myTreeWidget->addTopLevelItem(pRoot);
		RecursiveAddItems(pRoot, pModel);
		pRoot->setExpanded(true);
	}
}

void MoTreeDlg::RecursiveAddItems(QTreeWidgetItem* pParentItem, ModelicaModel* theModel)
{
	auto itr = theModel->m_vEnbeddedModels.begin();
	for (; itr != theModel->m_vEnbeddedModels.end(); ++itr)
	{
		ModelicaModel* pChildMdl = *itr;

		QTreeWidgetItem* pItem = new QTreeWidgetItem();
		pItem->setText(0, pChildMdl->m_sName.c_str());
		pItem->setData(0, Qt::UserRole, QVariant::fromValue(pChildMdl));

		QString qsType = pChildMdl->m_sType.c_str();
		if (qsType == "class")
		{
			pItem->setIcon(0, m_classIcon[0]);
		}
		else if(qsType == "model")
		{
			pItem->setIcon(0, m_classIcon[1]);
		}
		else if (qsType == "block")
		{
			pItem->setIcon(0, m_classIcon[2]);
		}
		else if (qsType == "connector")
		{
			pItem->setIcon(0, m_classIcon[3]);
		}
		else if (qsType == "package")
		{
			pItem->setIcon(0, m_classIcon[4]);
		}
		else if (qsType == "function")
		{
			pItem->setIcon(0, m_classIcon[5]);
		}
		else if (qsType == "type")
		{
			pItem->setIcon(0, m_classIcon[6]);
		}
		else if (qsType == "record")
		{
			pItem->setIcon(0, m_classIcon[7]);
		}
		//pItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);

		pParentItem->addChild(pItem);
		RecursiveAddItems(pItem, pChildMdl);
	}
}

void MoTreeDlg::Accept0()
{
	bAccepted = true;

	QTreeWidgetItem * theItem = myTreeWidget->currentItem();
	ModelicaFileModel* theMdl = (theItem->data(0, Qt::UserRole)).value<ModelicaFileModel*>();
	m_sMdlFullName = theMdl->sName.c_str();
	ModelicaFileModel* parentMdl = theMdl->pParent;
	while (parentMdl)
	{
		QString sName = parentMdl->sName.c_str();
		m_sMdlFullName = sName + "." + m_sMdlFullName;

		parentMdl = parentMdl->pParent;
	}

	//读取选中模型的全部信息
	m_pCyberInfo = new CyberInfo(nullptr,m_sMdlFullName.toStdString().c_str());
	m_pCyberInfo->BuildCyberInfo(m_sMoPath.toStdString().c_str());
	m_pCyberInfo->Flattening();

	QDialog::close();
}

void MoTreeDlg::dblClickOnTreeItem()
{
	Accept();
}

QString MoTreeDlg::GetModelFullName() const
{
	return m_sMdlFullName;
}

CyberInfo* MoTreeDlg::GetCyberInfo() const
{
	return m_pCyberInfo;
}

void MoTreeDlg::Accept()
{
	bAccepted = true;

	QTreeWidgetItem* theItem = myTreeWidget->currentItem();
	ModelicaModel* theMdl = (theItem->data(0, Qt::UserRole)).value<ModelicaModel*>();
	
	//对模型进行筛选
	if (theMdl->m_sType == "package" || theMdl->m_sType == "fuction" || \
		theMdl->m_sType == "type" || theMdl->m_sType == "record")
	{
		QMessageBox msg;
		msg.setText("关联的类型只允许class,model,block或connector");
		msg.exec();
		return;
	}
	
	m_sMdlFullName = theMdl->GetFullName().c_str();

	theMdl->Parse();
	m_pCyberInfo = theMdl->GenerateCyberInfo();
	m_pCyberInfo->SetMdlName(m_sMdlFullName.toStdString());

	QDialog::close();
}
