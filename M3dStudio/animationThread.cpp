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

//��ת����
void RotateComponent(DocumentModel* pModel, Component* pCom, double rotRad)
{
	//����frame_a��Z����ת
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	gp_Ax1 ax1 = ax31.Axis();
	gp_Trsf trsf;
	trsf.SetRotation(ax1, rotRad);

	pModel->SetCompLocation(pCom, trsf);
}

//ʵ������ķ�����λ�˰ڷ�
void DisplaceScaleComponent(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
	int i, int k_vec, vector<gp_Vec>& vecDeltaP0, map<string, double> mapCompRab, gp_Pnt ptFrameB)
{
	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3��4��

	gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	if (i == 0) //�������ƶ�
	{
		gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//��Ϊrev�����λ��
		vecDeltaP0.push_back(vec0);

		//��¼ÿ��frame_b�ĳ�ʼλ��
	}
	p0To.Translate(vecDeltaP0[k_vec]);
	ax3To.SetLocation(p0To);

	axFrom.SetLocation(ax31.Location());
	gp_Trsf trsfFrom_TO;
	trsfFrom_TO.SetDisplacement(axFrom, ax3To);//����λ�ã������̬

	//�������
	gp_Vec vRab1 = gp_Vec(p0To, ptFrameB);
	double newLen = vRab1.Magnitude();
	auto itr = mapCompRab.find(pCom->GetCompName());
	assert(itr != mapCompRab.end());
	double oldLen = itr->second;
	double scale = newLen / oldLen;  
	//ֻ��ȫ�ַ�����
	gp_Trsf trsfScale;
	trsfScale.SetScale(ax3To.Location(), scale);
	pModel->SetCompLocation(pCom, trsfScale*trsfFrom_TO);
}

//�����˶�����
void DisplaceComponent(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf)
{
	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3��4��

	//gp_Ax3 axFrom = pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 axFrom0 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");
	//gp_Ax3 axFrom;
	//axFrom.SetLocation(axFrom0.Location());

	//gp_Trsf trsfFrom_TO;
	//trsfFrom_TO.SetDisplacement(axFrom, ax3To);//����λ��\��̬
	//pModel->SetCompLocation(pCom, trsfFrom_TO);

	trsf.SetValues(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3), 0/*trsf.Value(1, 4) - axFrom0.Location().X()*/, \
		trsf.Value(2, 1), trsf.Value(2, 2), trsf.Value(2, 3), 0/*trsf.Value(2, 4) - axFrom0.Location().Y()*/, \
		trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3), 0/*trsf.Value(3, 4) - axFrom0.Location().Z()*/);
	
	//��ƽ�Ƶ�ԭ��
	gp_Trsf trsfTr0; //�ӵ�ǰ�㣬�ƶ���ԭ��
	gp_Pnt Po(0, 0, 0);
	trsfTr0.SetTranslation(axFrom0.Location(), Po);

	//��ѡת trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//��ƽ�Ƶ������λ��
	gp_Trsf trsfTr1; //�ӵ�ǰ�㣬�ƶ���ԭ��
	trsfTr1.SetTranslation(Po, p0To);
	
	pModel->SetCompLocation(pCom, trsfTr1*trsf*trsfTr0);
}

//�����˶�����,�ɰ�
void DisplaceComponent_old(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf,\
	int i, int k_vec, vector<gp_Vec>& vecDeltaP0)
{
	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3��4��

	gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	if (i == 0) //�������ƶ�
	{
		gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//��Ϊrev�����λ��
		vecDeltaP0.push_back(vec0);
	}
	p0To.Translate(vecDeltaP0[k_vec]);
	/*ax3To.SetLocation(p0To);
	
	axFrom.SetLocation(ax31.Location());
	gp_Trsf trsfFrom_TO;
	trsfFrom_TO.SetDisplacement(axFrom, ax3To);//����λ�ã������̬
	pModel->SetCompLocation(pCom, trsfFrom_TO);*/

	trsf.SetValues(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3), 0/*trsf.Value(1, 4) - axFrom0.Location().X()*/, \
		trsf.Value(2, 1), trsf.Value(2, 2), trsf.Value(2, 3), 0/*trsf.Value(2, 4) - axFrom0.Location().Y()*/, \
		trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3), 0/*trsf.Value(3, 4) - axFrom0.Location().Z()*/);

	//��ƽ�Ƶ�ԭ��
	gp_Trsf trsfTr0; //�ӵ�ǰ�㣬�ƶ���ԭ��
	gp_Pnt Po(0, 0, 0);
	trsfTr0.SetTranslation(ax31.Location(), Po);

	//����ת trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//��ƽ�Ƶ������λ��
	gp_Trsf trsfTr1; //�ӵ�ǰ�㣬�ƶ���ԭ��
	trsfTr1.SetTranslation(Po, p0To);

	pModel->SetCompLocation(pCom, trsfTr1 * trsf * trsfTr0);
}

//ʹ�ø�˹��Ԫ���Ծ����������
#define N 3
void Gaussian_elimination(float arr[N][N])
{
	int i, j, k;
	float W[N][2 * N], result[N][N];
	float tem_1, tem_2, tem_3;

	// �Ծ����Ұ벿�ֽ�������
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
		// �жϾ����һ�е�һ�е�Ԫ���Ƿ�Ϊ0����Ϊ0�������жϵڶ��е�һ��Ԫ�أ�ֱ����Ϊ0������ӵ���һ��
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
			//��ǰ��Ϊ0���м��Ϻ���ĳһ��
			for (k = 0; k < 2 * N; k++)
			{
				W[i][k] += W[j][k];
			}
		}

		//��ǰ������λԪ����1
		tem_1 = W[i][i];
		for (j = 0; j < 2 * N; j++)
		{
			W[i][j] = W[i][j] / tem_1;
		}

		//��������������λԪ����Ϊ0
		for (j = i + 1; j < N; j++)
		{
			tem_2 = W[j][i];
			for (k = i; k < 2 * N; k++)
			{
				W[j][k] = W[j][k] - tem_2 * W[i][k];
			}
		}
	}

	// ������ǰ�벿�ֱ�׼��
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

	//�ó������
	for (i = 0; i < N; i++)
	{
		for (j = N; j < 2 * N; j++)
		{
			result[i][j - N] = W[i][j];
		}
	}

	arr = result;
}


//�����˶�������231108���İ�
void DisplaceComponent_new(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
	int i, int k_vec, vector<gp_Vec>& vecDeltaP0, vector<gp_Trsf>& vecDirectionP0)
{
	//gp_Ax3 ax3To;
	//gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	//ax3To.SetLocation(p0To);
	//gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	//ax3To.SetXDirection(XTo);
	//gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	//ax3To.SetDirection(ZTo);  //3��4��

	//gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	//gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	//if (i == 0) //�������ƶ�
	//{
	//	gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//��Ϊrev�����λ��
	//	vecDeltaP0.push_back(vec0);
	//	vecDirectionP0.push_back(ax3To);
	//	//��¼ÿ��frame_b�ĳ�ʼλ��
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
	ax3To.SetDirection(ZTo);  //3��4��

	gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	if (i == 0) //�������ƶ�
	{
		gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//��Ϊrev�����λ��
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

	//��ƽ�Ƶ�ԭ��
	gp_Trsf trsfTr0; //�ӵ�ǰ�㣬�ƶ���ԭ��
	gp_Pnt Po(0, 0, 0);
	trsfTr0.SetTranslation(ax31.Location(), Po);

	//���г�ʼ�任
	gp_Trsf originTrsf = vecDirectionP0[k_vec];
	originTrsf.Invert();
	
	//����ת trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//��ƽ�Ƶ������λ��
	gp_Trsf trsfTr1; //�ӵ�ǰ�㣬�ƶ���ԭ��
	trsfTr1.SetTranslation(Po, p0To);

	pModel->SetCompLocation(pCom, trsfTr1 * trsf * originTrsf * trsfTr0);
}

//�ȸ����˶�������ת
void DisplaceRComponent(DocumentModel* pModel, Component* pCom, const char * sRigid, gp_Trsf& trsf, double rotRad,\
	int i, int k_vec, vector<gp_Vec>& vecDeltaP0)
{
	gp_Ax3 ax3To;
	gp_Pnt p0To = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
	ax3To.SetLocation(p0To);
	gp_Dir XTo = gp_Dir(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3));
	ax3To.SetXDirection(XTo);
	gp_Dir ZTo = gp_Dir(trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3));
	ax3To.SetDirection(ZTo);  //3��4��

	gp_Ax3 axFrom;// = m_pModel->GetCompConnLocalAx3(pCom->GetCompName().c_str(), "frame_a");
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(sRigid, "frame_a");

	if (i == 0) //�������ƶ�
	{
		gp_Vec vec0 = gp_Vec(p0To, ax31.Location());//��Ϊrev�����λ��
		vecDeltaP0.push_back(vec0);
	}
	p0To.Translate(vecDeltaP0[k_vec]);
	//ax3To.SetLocation(p0To);

	//axFrom.SetLocation(ax31.Location());
	gp_Trsf trsfFrom_TO;
	//trsfFrom_TO.SetDisplacement(axFrom, ax3To);//����λ�ã������̬
	
	trsf.SetValues(trsf.Value(1, 1), trsf.Value(1, 2), trsf.Value(1, 3), 0/*trsf.Value(1, 4) - axFrom0.Location().X()*/, \
		trsf.Value(2, 1), trsf.Value(2, 2), trsf.Value(2, 3), 0/*trsf.Value(2, 4) - axFrom0.Location().Y()*/, \
		trsf.Value(3, 1), trsf.Value(3, 2), trsf.Value(3, 3), 0/*trsf.Value(3, 4) - axFrom0.Location().Z()*/);

	//��ƽ�Ƶ�ԭ��
	gp_Trsf trsfTr0; //�ӵ�ǰ�㣬�ƶ���ԭ��
	gp_Pnt Po(0, 0, 0);
	trsfTr0.SetTranslation(ax31.Location(), Po);

	//��ѡת trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//��ƽ�Ƶ������λ��
	gp_Trsf trsfTr1; //�ӵ�ǰ�㣬�ƶ���ԭ��
	trsfTr1.SetTranslation(Po, p0To);

	//flange_a�任���λ��
	gp_Ax3 ax33 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), 1);
	//ax33.Transform(trsfFrom_TO);
	//gp_Trsf trsf3;
	//trsf3.SetRotation(ax33.Axis(), rotRad);
	//pModel->SetCompLocation(pCom, trsf3 * trsfTr1 * trsf * trsfTr0);

	//��һ�㣬�Ⱦ�λ��һ��
	gp_Trsf trsfTrans; //�ӵ�ǰ�㣬�ƶ������frame_aĿ���
	trsfTrans.SetTranslation(ax33.Location(), p0To);
	pModel->SetCompLocation(pCom, trsfTrans);
}


void AnimationThread::run()
{
	m_bRunning = true;

	//ƥ��AnimationWidget,����֮����AnimationWidgetͬ����������
	MainWindow* pWnd = (MainWindow*)m_pModel->parent();
	AnimationWidget* m_AniWid = pWnd->GetAnimationWidget();
	SimulInfoSendDlg* InfoSendDlg = m_AniWid->GetSimulInfoSendDlg();
	connect(this, SIGNAL(SendCSVReadStart()), m_AniWid, SLOT(MsgCSVReadStart()), Qt::BlockingQueuedConnection);
	connect(this, SIGNAL(SendCSVReadFinish()), m_AniWid, SLOT(MsgCSVReadFinish()), Qt::BlockingQueuedConnection);

	emit SendCSVReadStart();

	//1.���������----�˹��̿��Է������棬�ȶ�����
	//�ȵõ�ʱ������ֵ
	vector<double> timeSeries;
	map<string, vector<gp_Trsf>> mapCompPlacement;
	map<string, vector<double>> mapCompRotation;
	map<string, vector<double>> mapCompTranslation;
	map<string, vector<gp_Pnt>> mapFramebPos;
	bool b0 = FileParse::GetAllMultibodyPlacement(m_pModel,m_sCSVFilePath, timeSeries, mapCompPlacement,mapCompRotation, mapCompTranslation, mapFramebPos);
	if (!b0)
	{
		//��ʾ����;
		assert(false);
		return;
	}

	vector<int> sendVarsCols; // ���ͱ���������(�к�)����
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

	//OpenDDS��������Ԥ��������߶����߶�ȡ����Ч��̫�ͣ�ʱ��̫��
	map<double, vector<string>> timeToDatas;
	FileParse::GetDDSDatasFromCols(m_sCSVFilePath, sendVarsCols, timeToDatas);

	emit SendCSVReadFinish();

	//QStringList timeStrLst = FileParse::GetDataSeries(m_sCSVFilePath, "time");
	//for (int i = 0; i < timeStrLst.size(); i++)
	//{
	//	double d_i = timeStrLst.at(i).toDouble();
	//	timeSeries.push_back(d_i);
	//}
	//�ٶ���ȫ�����岿����λ��--����λ��
	//auto itr = m_vComponents.begin();
	//for (; itr != m_vComponents.end(); ++itr)
	//{
	//	Component* pCom = *itr;
	//	if (pCom->IsMultibodyComponent())//����ǻ�е���岿��
	//	{
	//		string sComFulName = pCom->GetCompFullName();
	//		//�õ�Modelica.Mechanics.MultiBody.Interfaces.Frame_a���ͽӿڵ������
	//		string sFrame_aName = pCom->GetFrame_aName();
	//		string sColName = sComFulName + "." + sFrame_aName + ".";

	//		//λ��body.frame_a.r_0[1]
	//		string sPosXcolName, sPosYcolName, sPosZcolName;
	//		sPosXcolName = sColName + "r_0[1]";
	//		QStringList slPosX = FileParse::GetDataSeries(m_sCSVFilePath, sPosXcolName.c_str());
	//		sPosYcolName = sColName + "r_0[2]";
	//		QStringList slPosY = FileParse::GetDataSeries(m_sCSVFilePath, sPosYcolName.c_str());
	//		sPosZcolName = sColName + "r_0[3]";
	//		QStringList slPosZ = FileParse::GetDataSeries(m_sCSVFilePath, sPosZcolName.c_str());
	//		//��̬...
	//	}
	//}
	//while (!m_bStop)
	{

		//2.�����˶�
		vector<gp_Vec> vecDeltaP0;                //��¼frame_a��ƫ��
		
		// FIXME �����������
		vector<gp_Trsf> vecDirectionP0;
		
		map<string, double> mapCompRab;           //��¼frame_a��frame_b��r�ĳ�ʼ����
		map<int, double> TimeDifferences;         //��¼��Ϣ��ʱ����
		double LastStep_time = 0;                 //��¼��һ����ʱ��
		int CurrentStepInterval = 1;              //��¼��ǰ�������
		for (int i = 0; i < timeSeries.size();)
		{
			if (m_bStop)
				break;

			//�ȼ�¼���ʱ��
			QTime t0 = QTime::currentTime();
			//QMutex* pMutex = m_pModel->GetMutex();

			//����t0ʱ�̸����㲿����λ��
			auto itr = m_vComponents.begin();
			int k_vec = 0;
			for (; itr != m_vComponents.end(); ++itr)
			{
				Component* pCom = *itr;
				if (1)//pCom->IsMultibodyComponent())//����ǻ�е���岿��
				{
					gp_Trsf trsf;
					string sComFullName = pCom->GetCompFullName();
					//ȥ��ǰ׺��ϵͳ��
					int posDot = sComFullName.find('.');
					assert(posDot > 0);
					sComFullName = sComFullName.substr(posDot + 1);
					auto itr_i = mapCompPlacement.find(sComFullName);
					if (itr_i != mapCompPlacement.end())
					{//�����˶�
						vector<gp_Trsf> vTrsf = itr_i->second;
						trsf = vTrsf[i]; //frame_a������λ��
						//����Ǳ�����������������
						if (pCom->IsDeformable())
						{
							//���ӷ���
							auto itr_fb = mapFramebPos.find(sComFullName);
							assert(itr_fb != mapFramebPos.end());
							vector<gp_Pnt> vFBP0 = itr_fb->second;
							gp_Pnt frameBp0 = vFBP0[i];

							//frame_a��λ��
							gp_Pnt p0FA = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
							if (i == 0) //��ʼ����
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
						//��ת
						auto itr_R = mapCompRotation.find(sComFullName);
						if (itr_R != mapCompRotation.end())
						{//��ת�˶�
							double rotRad = itr_R->second[i];
							//�õ����ӵĸ���
							string sLinkRigid = m_pModel->GetLinkRigid(sComFullName.c_str());
							if (!sLinkRigid.empty())
							{
								//�������һ���˶�
								auto itr_i = mapCompPlacement.find(sLinkRigid);
								if (itr_i != mapCompPlacement.end())
								{//�����˶�
									vector<gp_Trsf> vTrsf = itr_i->second;
									trsf = vTrsf[i]; //frame_a������λ��
									DisplaceRComponent(m_pModel, pCom, sLinkRigid.c_str(), trsf, rotRad, i, k_vec++, vecDeltaP0);
								}
							}
							else {
								//�Լ�ת��
								double rotRad = itr_R->second[i];
								RotateComponent(m_pModel, pCom, rotRad);
							}
						}

						else {
							//ƽ��
							auto itr_T = mapCompTranslation.find(sComFullName);
							if (itr_T != mapCompTranslation.end())
							{//�˶�
								assert(false);
							}
						}
					}
				}
			}
			//�ſ�һ��
			//if (i % 2 == 0)
			{
				pauseLock.lock();
				emit rep();
				pauseLock.unlock();
			}

			
			if (m_AniWid->haveCreatedInfoSendDlg)
			{
				//ֻ��issendingΪtrueʱ�ŷ�������
				if (InfoSendDlg->issending == true)
				{
					//��AnimationWidget��������
					//��ʽһ������ʱ�Ŵ�CSV�ļ����ȡ����
					//QStringList DataRows = FileParse::GetDataByTime(m_sCSVFilePath, timeSeries[i]);
					////���AllDatas
					//AllDatas.clear();
					//for (int col : sendVarsCols) {
					//	string data = DataRows[col].toStdString();
					//	AllDatas.push_back(data);
					//}

					//��ʽ�����ļ�Ԥ��
					AllDatas = timeToDatas[timeSeries[i]];

					datatime = timeSeries[i];
					emit SendSimulateMsg();
					//InfoSendDlg->SendSimulateData(timeSeries[i], AllDatas);
				}
			}
			
			
			
			//ִ��һ�����ʱ��
			QTime t1 = QTime::currentTime();
			//������ʱ�䳤�ȶ��� ʱ�����п��*speedK������ͣһ�²�ֵ
			//ʱ���Ժ���ƣ�
			qint64 intervalTimeMS = t0.msecsTo(t1);
			
			//�ɰ�����ٶȵķ���
			////m_speedK = 1;
			//double m_speedK_calcul = m_speedK < 1 ? m_speedK : 1;//m_speedK<1ʱ��msleep�����ٶȣ�>1ʱ��ȡ���ݿ����ٶ�
			//double TimeNeed = (double)10 / (double)m_speedK_calcul;
			//if (intervalTimeMS < TimeNeed)
			//{
			//	double TimeDifference = TimeNeed - intervalTimeMS;
			//	TimeDifferences[i] = TimeDifference;
			//	QThread::msleep(TimeDifference);
			//}

			//if (m_speedK > 1 && m_speedK <= 2)
			//{
			//	double extract = (m_speedK * 10) / (m_speedK * 10 - 10);//ÿextract�����ݳ�ȡ1������
			//	// //����ȡ��
			//	//cout << (int)a << endl;
			//	////����ȡ��
			//	//cout << (a > (int)a ? (int)a + 1 : (int)a) << endl;
			//	////��������
			//	//cout << (int)(a + 0.5) << endl;
			//	int i_extract = (int)(extract + 0.5);
			//	if (i % i_extract == 0)
			//	{
			//		i++;
			//	}
			//}
			//
			////����2.0���ϣ�����ÿn�����ݱ���һ������
			//if (m_speedK > 2)
			//{
			//	int i_extract = m_speedK > (int)m_speedK ? (int)m_speedK + 1 : (int)m_speedK;//ÿi_extarct�����ݱ���һ������
			//	if (i % i_extract == 0)
			//	{
			//		i = i + (i_extract - 1);
			//	}
			//}

			//�����ݷ��ͷ��ڼ�ʱ���棬����Ч���ǳ���������Ų��ǰ��ȥ����
			//if (InfoSendDlg != nullptr)
			//{
			//	//��AnimationWidget��������
			//	QStringList DataRows = FileParse::GetDataByTime(m_sCSVFilePath, timeSeries[i]);
			//	//���AllDatas
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

			
			//�°��ٶȿ��ƣ�ÿһ������������ʱ�䣬ͨ��������ʱ��ķ�����̬������ǰ���������
			double CurrentStep_time = timeSeries[i];
			double TimeNeed = (double)1000 * (CurrentStep_time - LastStep_time) / (double)m_speedK;   //*1000����Ϊ��->����ĵ�λת��
			//�����ǰ����ʱ��С������ʱ����Ŀǰ����Ϊ1����ô����Ϊ1���ֲ���
			if (intervalTimeMS < TimeNeed && CurrentStepInterval == 1)
			{
				double TimeDifference = TimeNeed - intervalTimeMS;
				QThread::msleep(TimeDifference);
				i = i + CurrentStepInterval;
			}
			//�����ǰ����ʱ��С������ʱ����Ŀǰ��������1��������жϲ����Ƿ���Ҫ��С
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
			//�������ʱ���������ʱ�䣬��ô���Ӳ���
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
	//ʹ�˶�ֹͣ
	this->pauseLock.tryLock();

	//��������Ϊ��ʼλ��
	auto itr2 = m_vComponents.begin();
	for (; itr2 != m_vComponents.end(); ++itr2)
	{
		Component* pCom = *itr2;
		gp_Trsf trsf2;
		m_pModel->SetCompLocation(pCom, trsf2);
	}
}

