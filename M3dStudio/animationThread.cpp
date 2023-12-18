#include "DocumentModel.h"
#include "animationThread.h"
#include "Component.h"
#include <QTime>
#include "CSV_Parser.h"
//#include "DocumentModel.h"
#include "AnimationWidget.h"
#include "mainwindow.h"

using namespace CSV_Parser;

AnimationThread::AnimationThread(QObject* pParent)
	: QThread(pParent)
{
	m_bRunning = false;
	m_bPause = false;
	m_bStop = false;
}

void AnimationThread::SetComponents(vector<Component*>& vComps)
{
	m_vComponents = vComps;
}

void AnimationThread::SetCSVFilePath(const QString& sFile)
{
	m_sCSVFilePath = sFile;
}

//旋转部件
void RotateComponent(DocumentModel* pModel, Component* pCom, double rotRad)
{
	//绕着frame_a的Z轴旋转
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	gp_Ax1 ax1 = ax31.Axis();
	gp_Trsf trsf;
	trsf.SetRotation(ax1, rotRad);

	pModel->SetCompLocation(pCom, trsf);
}

//实现组件的放缩和位姿摆放
void DisplaceScaleComponent(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
	int i, int k_vec, vector<gp_Vec>& vecDeltaP0, map<string, double> mapCompRab, gp_Pnt ptFrameB)
{
	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3行4列

	gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	if (i == 0) //计算点的移动
	{
		gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//因为rev引起的位移
		vecDeltaP0.push_back(vec0);

		//记录每个frame_b的初始位置
	}
	p0To.Translate(vecDeltaP0[k_vec]);
	ax3To.SetLocation(p0To);

	axFrom.SetLocation(ax31.Location());
	gp_Trsf trsfFrom_TO;
	trsfFrom_TO.SetDisplacement(axFrom, ax3To);//绝对位置，相对姿态

	//计算比例
	gp_Vec vRab1 = gp_Vec(p0To, ptFrameB);
	double newLen = vRab1.Magnitude();
	auto itr = mapCompRab.find(pCom->GetCompName());
	assert(itr != mapCompRab.end());
	double oldLen = itr->second;
	double scale = newLen / oldLen;  
	//只能全局放缩！
	gp_Trsf trsfScale;
	trsfScale.SetScale(ax3To.Location(), scale);
	pModel->SetCompLocation(pCom, trsfScale*trsfFrom_TO);
}

//刚体运动部件
void DisplaceComponent(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf)
{
	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3行4列

	//gp_Ax3 axFrom = pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 axFrom0 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");
	//gp_Ax3 axFrom;
	//axFrom.SetLocation(axFrom0.Location());

	//gp_Trsf trsfFrom_TO;
	//trsfFrom_TO.SetDisplacement(axFrom, ax3To);//绝对位置\姿态
	//pModel->SetCompLocation(pCom, trsfFrom_TO);

	trsf.SetValues(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3), 0/*trsf.Value(1, 4) - axFrom0.Location().X()*/, \
		trsf.Value(2, 1), trsf.Value(2, 2), trsf.Value(2, 3), 0/*trsf.Value(2, 4) - axFrom0.Location().Y()*/, \
		trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3), 0/*trsf.Value(3, 4) - axFrom0.Location().Z()*/);
	
	//先平移到原点
	gp_Trsf trsfTr0; //从当前点，移动到原点
	gp_Pnt Po(0, 0, 0);
	trsfTr0.SetTranslation(axFrom0.Location(), Po);

	//再选转 trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//再平移到计算的位置
	gp_Trsf trsfTr1; //从当前点，移动到原点
	trsfTr1.SetTranslation(Po, p0To);
	
	pModel->SetCompLocation(pCom, trsfTr1*trsf*trsfTr0);
}

//刚体运动部件,旧版
void DisplaceComponent_old(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf,\
	int i, int k_vec, vector<gp_Vec>& vecDeltaP0)
{
	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3行4列

	gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	if (i == 0) //计算点的移动
	{
		gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//因为rev引起的位移
		vecDeltaP0.push_back(vec0);
	}
	p0To.Translate(vecDeltaP0[k_vec]);
	/*ax3To.SetLocation(p0To);
	
	axFrom.SetLocation(ax31.Location());
	gp_Trsf trsfFrom_TO;
	trsfFrom_TO.SetDisplacement(axFrom, ax3To);//绝对位置，相对姿态
	pModel->SetCompLocation(pCom, trsfFrom_TO);*/

	trsf.SetValues(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3), 0/*trsf.Value(1, 4) - axFrom0.Location().X()*/, \
		trsf.Value(2, 1), trsf.Value(2, 2), trsf.Value(2, 3), 0/*trsf.Value(2, 4) - axFrom0.Location().Y()*/, \
		trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3), 0/*trsf.Value(3, 4) - axFrom0.Location().Z()*/);

	//先平移到原点
	gp_Trsf trsfTr0; //从当前点，移动到原点
	gp_Pnt Po(0, 0, 0);
	trsfTr0.SetTranslation(ax31.Location(), Po);

	//再旋转 trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//再平移到计算的位置
	gp_Trsf trsfTr1; //从当前点，移动到原点
	trsfTr1.SetTranslation(Po, p0To);

	pModel->SetCompLocation(pCom, trsfTr1 * trsf * trsfTr0);
}

//使用高斯消元法对矩阵进行求逆
#define N 3
void Gaussian_elimination(float arr[N][N])
{
	int i, j, k;
	float W[N][2 * N], result[N][N];
	float tem_1, tem_2, tem_3;

	// 对矩阵右半部分进行扩增
	for (i = 0; i < N; i++) {
		for (j = 0; j < 2 * N; j++) {
			if (j < N) {
				W[i][j] = (float)arr[i][j];
			}
			else {
				W[i][j] = (float)(j - N == i ? 1 : 0);
			}
		}
	}

	for (i = 0; i < N; i++)
	{
		// 判断矩阵第一行第一列的元素是否为0，若为0，继续判断第二行第一列元素，直到不为0，将其加到第一行
		if (((int)W[i][i]) == 0)
		{
			for (j = i + 1; j < N; j++)
			{
				if (((int)W[j][i]) != 0) break;
			}
			if (j == N)
			{
				break;
			}
			//将前面为0的行加上后面某一行
			for (k = 0; k < 2 * N; k++)
			{
				W[i][k] += W[j][k];
			}
		}

		//将前面行首位元素置1
		tem_1 = W[i][i];
		for (j = 0; j < 2 * N; j++)
		{
			W[i][j] = W[i][j] / tem_1;
		}

		//将后面所有行首位元素置为0
		for (j = i + 1; j < N; j++)
		{
			tem_2 = W[j][i];
			for (k = i; k < 2 * N; k++)
			{
				W[j][k] = W[j][k] - tem_2 * W[i][k];
			}
		}
	}

	// 将矩阵前半部分标准化
	for (i = N - 1; i >= 0; i--)
	{
		for (j = i - 1; j >= 0; j--)
		{
			tem_3 = W[j][i];
			for (k = i; k < 2 * N; k++)
			{
				W[j][k] = W[j][k] - tem_3 * W[i][k];
			}
		}
	}

	//得出逆矩阵
	for (i = 0; i < N; i++)
	{
		for (j = N; j < 2 * N; j++)
		{
			result[i][j - N] = W[i][j];
		}
	}

	arr = result;
}


//刚体运动部件，231108更改版
void DisplaceComponent_new(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
	int i, int k_vec, vector<gp_Vec>& vecDeltaP0, vector<gp_Trsf>& vecDirectionP0)
{
	//gp_Ax3 ax3To;
	//gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	//ax3To.SetLocation(p0To);
	//gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	//ax3To.SetXDirection(XTo);
	//gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	//ax3To.SetDirection(ZTo);  //3行4列

	//gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	//gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	//if (i == 0) //计算点的移动
	//{
	//	gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//因为rev引起的位移
	//	vecDeltaP0.push_back(vec0);
	//	vecDirectionP0.push_back(ax3To);
	//	//记录每个frame_b的初始位置
	//}
	//p0To.Translate(vecDeltaP0[k_vec]);
	//ax3To.SetLocation(p0To);
	// 
	//axFrom.SetLocation(ax31.Location());
	//axFrom.SetXDirection(vecDirectionP0[k_vec].XDirection());
	//axFrom.SetDirection(vecDirectionP0[k_vec].Direction());
	//gp_Trsf trsfFrom_TO;
	//trsfFrom_TO.SetDisplacement(axFrom, ax3To);
	//
	//pModel->SetCompLocation(pCom, trsfFrom_TO);

	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3行4列

	gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	if (i == 0) //计算点的移动
	{
		gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//因为rev引起的位移
		vecDeltaP0.push_back(vec0);

		float a[3][3] = { {trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3)}, {trsf.Value(2, 1), trsf.Value(2, 2), trsf.Value(2, 3)}, {trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3)} };
		Gaussian_elimination(a);
		gp_Trsf originTrsf;
		originTrsf.SetValues(a[0][0], a[0][1], a[0][2], 0, \
			a[1][0], a[1][1], a[1][2], 0, \
			a[2][0], a[2][1], a[2][2], 0);
		vecDirectionP0.push_back(originTrsf);

		pModel->testLocation(pCom);
	}
	p0To.Translate(vecDeltaP0[k_vec]);

	trsf.SetValues(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3), 0/*trsf.Value(1, 4) - axFrom0.Location().X()*/, \
		trsf.Value(2, 1), trsf.Value(2, 2), trsf.Value(2, 3), 0/*trsf.Value(2, 4) - axFrom0.Location().Y()*/, \
		trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3), 0/*trsf.Value(3, 4) - axFrom0.Location().Z()*/);

	//先平移到原点
	gp_Trsf trsfTr0; //从当前点，移动到原点
	gp_Pnt Po(0, 0, 0);
	trsfTr0.SetTranslation(ax31.Location(), Po);

	//进行初始变换
	gp_Trsf originTrsf = vecDirectionP0[k_vec];
	originTrsf.Invert();
	
	//再旋转 trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//再平移到计算的位置
	gp_Trsf trsfTr1; //从当前点，移动到原点
	trsfTr1.SetTranslation(Po, p0To);

	pModel->SetCompLocation(pCom, trsfTr1 * trsf * originTrsf * trsfTr0);
}

//先刚体运动，再自转
void DisplaceRComponent(DocumentModel* pModel, Component* pCom, const char * sRigid, gp_Trsf& trsf, double rotRad,\
	int i, int k_vec, vector<gp_Vec>& vecDeltaP0)
{
	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3行4列

	gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(sRigid, "frame_a");

	if (i == 0) //计算点的移动
	{
		gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//因为rev引起的位移
		vecDeltaP0.push_back(vec0);
	}
	p0To.Translate(vecDeltaP0[k_vec]);
	//ax3To.SetLocation(p0To);

	//axFrom.SetLocation(ax31.Location());
	gp_Trsf trsfFrom_TO;
	//trsfFrom_TO.SetDisplacement(axFrom, ax3To);//绝对位置，相对姿态
	
	trsf.SetValues(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3), 0/*trsf.Value(1, 4) - axFrom0.Location().X()*/, \
		trsf.Value(2, 1), trsf.Value(2, 2), trsf.Value(2, 3), 0/*trsf.Value(2, 4) - axFrom0.Location().Y()*/, \
		trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3), 0/*trsf.Value(3, 4) - axFrom0.Location().Z()*/);

	//先平移到原点
	gp_Trsf trsfTr0; //从当前点，移动到原点
	gp_Pnt Po(0, 0, 0);
	trsfTr0.SetTranslation(ax31.Location(), Po);

	//再选转 trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//再平移到计算的位置
	gp_Trsf trsfTr1; //从当前点，移动到原点
	trsfTr1.SetTranslation(Po, p0To);

	//flange_a变换后的位置
	gp_Ax3 ax33 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), 1);
	//ax33.Transform(trsfFrom_TO);
	//gp_Trsf trsf3;
	//trsf3.SetRotation(ax33.Axis(), rotRad);
	//pModel->SetCompLocation(pCom, trsf3 * trsfTr1 * trsf * trsfTr0);

	//简单一点，先就位置一致
	gp_Trsf trsfTrans; //从当前点，移动刚体的frame_a目标点
	trsfTrans.SetTranslation(ax33.Location(), p0To);
	pModel->SetCompLocation(pCom, trsfTrans);
}


void AnimationThread::run()
{
	m_bRunning = true;

	//匹配AnimationWidget,便于之后向AnimationWidget同步发送数据
	MainWindow* pWnd = (MainWindow*)m_pModel->parent();
	AnimationWidget* m_AniWid = pWnd->GetAnimationWidget();
	SimulInfoSendDlg* InfoSendDlg = m_AniWid->GetSimulInfoSendDlg();
	connect(this, SIGNAL(SendCSVReadStart()), m_AniWid, SLOT(MsgCSVReadStart()), Qt::BlockingQueuedConnection);
	connect(this, SIGNAL(SendCSVReadFinish()), m_AniWid, SLOT(MsgCSVReadFinish()), Qt::BlockingQueuedConnection);

	emit SendCSVReadStart();

	//1.读结果数据----此过程可以放在外面，先读数据
	//先得到时间序列值
	vector<double> timeSeries;
	map<string, vector<gp_Trsf>> mapCompPlacement;
	map<string, vector<double>> mapCompRotation;
	map<string, vector<double>> mapCompTranslation;
	map<string, vector<gp_Pnt>> mapFramebPos;
	bool b0 = FileParse::GetAllMultibodyPlacement(m_pModel,m_sCSVFilePath, timeSeries, mapCompPlacement,mapCompRotation, mapCompTranslation, mapFramebPos);
	if (!b0)
	{
		//提示错误;
		assert(false);
		return;
	}

	vector<int> sendVarsCols; // 发送变量的索引(列号)数组
	if (m_AniWid->haveCreatedInfoSendDlg)
	{
		connect(this, SIGNAL(SendSimulateMsg()), InfoSendDlg, SLOT(SendSimulateData()), Qt::BlockingQueuedConnection);
		vector<string> sendVarsNames = InfoSendDlg->getSendVarsNames();
		QStringList allVariablesNames = FileParse::GetAllVariablesNames(m_sCSVFilePath);
		unordered_map<string, int> nameToCol;
		for (int col = 0; col < allVariablesNames.length(); col++) {
			string colname = allVariablesNames[col].toStdString();
			nameToCol[colname] = col;
		}

		for (string sendvarname : sendVarsNames) {
			sendVarsCols.push_back(nameToCol[sendvarname]);
		}
	}

	//OpenDDS发送数据预读，否则边动画边读取数据效率太低，时延太长
	map<double, vector<string>> timeToDatas;
	FileParse::GetDDSDatasFromCols(m_sCSVFilePath, sendVarsCols, timeToDatas);

	emit SendCSVReadFinish();

	//QStringList timeStrLst = FileParse::GetDataSeries(m_sCSVFilePath, "time");
	//for (int i = 0; i < timeStrLst.size(); i++)
	//{
	//	double d_i = timeStrLst.at(i).toDouble();
	//	timeSeries.push_back(d_i);
	//}
	//再读出全部多体部件的位姿--绝对位姿
	//auto itr = m_vComponents.begin();
	//for (; itr != m_vComponents.end(); ++itr)
	//{
	//	Component* pCom = *itr;
	//	if (pCom->IsMultibodyComponent())//如果是机械多体部件
	//	{
	//		string sComFulName = pCom->GetCompFullName();
	//		//得到Modelica.Mechanics.MultiBody.Interfaces.Frame_a类型接口的组件名
	//		string sFrame_aName = pCom->GetFrame_aName();
	//		string sColName = sComFulName + "." + sFrame_aName + ".";

	//		//位置body.frame_a.r_0[1]
	//		string sPosXcolName, sPosYcolName, sPosZcolName;
	//		sPosXcolName = sColName + "r_0[1]";
	//		QStringList slPosX = FileParse::GetDataSeries(m_sCSVFilePath, sPosXcolName.c_str());
	//		sPosYcolName = sColName + "r_0[2]";
	//		QStringList slPosY = FileParse::GetDataSeries(m_sCSVFilePath, sPosYcolName.c_str());
	//		sPosZcolName = sColName + "r_0[3]";
	//		QStringList slPosZ = FileParse::GetDataSeries(m_sCSVFilePath, sPosZcolName.c_str());
	//		//姿态...
	//	}
	//}
	//while (!m_bStop)
	{

		//2.驱动运动
		vector<gp_Vec> vecDeltaP0;                //记录frame_a的偏移
		
		// FIXME 新增，须测试
		vector<gp_Trsf> vecDirectionP0;
		
		map<string, double> mapCompRab;           //记录frame_a到frame_b的r的初始长度
		map<int, double> TimeDifferences;         //记录休息的时间间隔
		double LastStep_time = 0;                 //记录上一步的时间
		int CurrentStepInterval = 1;              //记录当前步长间隔
		for (int i = 0; i < timeSeries.size();)
		{
			if (m_bStop)
				break;

			//先记录起点时刻
			QTime t0 = QTime::currentTime();
			//QMutex* pMutex = m_pModel->GetMutex();

			//放置t0时刻各个零部件的位置
			auto itr = m_vComponents.begin();
			int k_vec = 0;
			for (; itr != m_vComponents.end(); ++itr)
			{
				Component* pCom = *itr;
				if (1)//pCom->IsMultibodyComponent())//如果是机械多体部件
				{
					gp_Trsf trsf;
					string sComFullName = pCom->GetCompFullName();
					//去掉前缀的系统名
					int posDot = sComFullName.find('.');
					assert(posDot > 0);
					sComFullName = sComFullName.substr(posDot + 1);
					auto itr_i = mapCompPlacement.find(sComFullName);
					if (itr_i != mapCompPlacement.end())
					{//多体运动
						vector<gp_Trsf> vTrsf = itr_i->second;
						trsf = vTrsf[i]; //frame_a的最终位姿
						//如果是变形零件，则放缩它！
						if (pCom->IsDeformable())
						{
							//增加放缩
							auto itr_fb = mapFramebPos.find(sComFullName);
							assert(itr_fb != mapFramebPos.end());
							vector<gp_Pnt> vFBP0 = itr_fb->second;
							gp_Pnt frameBp0 = vFBP0[i];

							//frame_a的位置
							gp_Pnt p0FA = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
							if (i == 0) //初始长度
							{
								gp_Vec rab = gp_Vec(p0FA, frameBp0);
								double r_ab = rab.Magnitude();
								mapCompRab.insert(make_pair(pCom->GetCompName(), r_ab));
							}

							DisplaceScaleComponent(m_pModel, pCom, trsf, i, k_vec++, vecDeltaP0, mapCompRab, frameBp0);
						}
						else
						{
							//DisplaceComponent_old(m_pModel, pCom, trsf, i, k_vec++, vecDeltaP0);
							DisplaceComponent_new(m_pModel, pCom, trsf, i, k_vec++, vecDeltaP0, vecDirectionP0);
						}
					}
					else {
						//旋转
						auto itr_R = mapCompRotation.find(sComFullName);
						if (itr_R != mapCompRotation.end())
						{//旋转运动
							double rotRad = itr_R->second[i];
							//得到连接的刚体
							string sLinkRigid = m_pModel->GetLinkRigid(sComFullName.c_str());
							if (!sLinkRigid.empty())
							{
								//跟随刚体一起运动
								auto itr_i = mapCompPlacement.find(sLinkRigid);
								if (itr_i != mapCompPlacement.end())
								{//刚体运动
									vector<gp_Trsf> vTrsf = itr_i->second;
									trsf = vTrsf[i]; //frame_a的最终位姿
									DisplaceRComponent(m_pModel, pCom, sLinkRigid.c_str(), trsf, rotRad, i, k_vec++, vecDeltaP0);
								}
							}
							else {
								//自己转动
								double rotRad = itr_R->second[i];
								RotateComponent(m_pModel, pCom, rotRad);
							}
						}

						else {
							//平移
							auto itr_T = mapCompTranslation.find(sComFullName);
							if (itr_T != mapCompTranslation.end())
							{//运动
								assert(false);
							}
						}
					}
				}
			}
			//放快一点
			//if (i % 2 == 0)
			{
				pauseLock.lock();
				emit rep();
				pauseLock.unlock();
			}

			
			if (m_AniWid->haveCreatedInfoSendDlg)
			{
				//只有issending为true时才发布数据
				if (InfoSendDlg->issending == true)
				{
					//向AnimationWidget发送数据
					//方式一：发送时才从CSV文件里读取数据
					//QStringList DataRows = FileParse::GetDataByTime(m_sCSVFilePath, timeSeries[i]);
					////清空AllDatas
					//AllDatas.clear();
					//for (int col : sendVarsCols) {
					//	string data = DataRows[col].toStdString();
					//	AllDatas.push_back(data);
					//}

					//方式二：文件预读
					AllDatas = timeToDatas[timeSeries[i]];

					datatime = timeSeries[i];
					emit SendSimulateMsg();
					//InfoSendDlg->SendSimulateData(timeSeries[i], AllDatas);
				}
			}
			
			
			
			//执行一步完的时刻
			QTime t1 = QTime::currentTime();
			//如果这个时间长度短于 时间序列跨度*speedK，则暂停一下差值
			//时间差（以毫秒计）
			qint64 intervalTimeMS = t0.msecsTo(t1);
			
			//旧版控制速度的方法
			////m_speedK = 1;
			//double m_speedK_calcul = m_speedK < 1 ? m_speedK : 1;//m_speedK<1时用msleep控制速度，>1时抽取数据控制速度
			//double TimeNeed = (double)10 / (double)m_speedK_calcul;
			//if (intervalTimeMS < TimeNeed)
			//{
			//	double TimeDifference = TimeNeed - intervalTimeMS;
			//	TimeDifferences[i] = TimeDifference;
			//	QThread::msleep(TimeDifference);
			//}

			//if (m_speedK > 1 && m_speedK <= 2)
			//{
			//	double extract = (m_speedK * 10) / (m_speedK * 10 - 10);//每extract个数据抽取1个数据
			//	// //向下取整
			//	//cout << (int)a << endl;
			//	////向上取整
			//	//cout << (a > (int)a ? (int)a + 1 : (int)a) << endl;
			//	////四舍五入
			//	//cout << (int)(a + 0.5) << endl;
			//	int i_extract = (int)(extract + 0.5);
			//	if (i % i_extract == 0)
			//	{
			//		i++;
			//	}
			//}
			//
			////倍速2.0往上，就是每n个数据保留一个数据
			//if (m_speedK > 2)
			//{
			//	int i_extract = m_speedK > (int)m_speedK ? (int)m_speedK + 1 : (int)m_speedK;//每i_extarct个数据保留一个数据
			//	if (i % i_extract == 0)
			//	{
			//		i = i + (i_extract - 1);
			//	}
			//}

			//把数据发送放在计时后面，动画效果非常卡，把它挪到前面去试试
			//if (InfoSendDlg != nullptr)
			//{
			//	//向AnimationWidget发送数据
			//	QStringList DataRows = FileParse::GetDataByTime(m_sCSVFilePath, timeSeries[i]);
			//	//清空AllDatas
			//	vector<string> newvec;
			//	AllDatas = newvec;
			//	for (int i = 0; i < DataRows.size(); i++)
			//	{
			//		string Data = DataRows.at(i).toStdString();
			//		AllDatas.push_back(Data);
			//	}
			//	datatime = timeSeries[i];
			//	emit SendSimulateMsg();
			//	//InfoSendDlg->SendSimulateData(timeSeries[i], AllDatas);
			//}

			
			//新版速度控制（每一步都计算消耗时间，通过对消耗时间的分析动态调整当前步数间隔）
			double CurrentStep_time = timeSeries[i];
			double TimeNeed = (double)1000 * (CurrentStep_time - LastStep_time) / (double)m_speedK;   //*1000是因为秒->毫秒的单位转换
			//如果当前消耗时间小于所需时间且目前步长为1，那么步长为1保持不变
			if (intervalTimeMS < TimeNeed && CurrentStepInterval == 1)
			{
				double TimeDifference = TimeNeed - intervalTimeMS;
				QThread::msleep(TimeDifference);
				i = i + CurrentStepInterval;
			}
			//如果当前消耗时间小于所需时间且目前步长大于1，分情况判断步长是否需要缩小
			else if (intervalTimeMS < TimeNeed && CurrentStepInterval > 1)
			{
				double TimeDifference = TimeNeed - intervalTimeMS;
				QThread::msleep(TimeDifference);
				if (TimeDifference * CurrentStepInterval <= TimeNeed)
				{
					i = i + CurrentStepInterval;
				}
				else
				{
					CurrentStepInterval--;
					i = i + CurrentStepInterval;
				}
			}
			//如果消耗时间大于所需时间，那么增加步长
			else if (intervalTimeMS > TimeNeed)
			{
				CurrentStepInterval++;
				i = i + CurrentStepInterval;
			}
			LastStep_time = CurrentStep_time;
		}
	}

	m_bRunning = false;
}

void AnimationThread::pauseThread()
{
	this->pauseLock.lock();
	m_bPause = true;
}

void AnimationThread::resumeThread()
{
	this->pauseLock.unlock();
	m_bPause = false;
}

void AnimationThread::closeThread()
{
	//使运动停止
	this->pauseLock.tryLock();

	//将物体置为初始位置
	auto itr2 = m_vComponents.begin();
	for (; itr2 != m_vComponents.end(); ++itr2)
	{
		Component* pCom = *itr2;
		gp_Trsf trsf2;
		m_pModel->SetCompLocation(pCom, trsf2);
	}
}

