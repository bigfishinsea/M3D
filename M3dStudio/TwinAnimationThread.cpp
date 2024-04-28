#include "DigitaltwinsWidget.h"
#include "DocumentModel.h"
#include "TwinAnimationThread.h"
//#include "DigitaltwinsWidget.h"
#include "Component.h"
#include "DocumentComponent.h"
#include "global.h"


TwinAnimationThread::TwinAnimationThread(QObject* pParent)
	: QThread(pParent)
{

}

void TwinAnimationThread::SetComponents(vector<Component*>& vComps)
{
	m_vComponents = vComps;
}


void TwinAnimationThread::initNeuralNetwork(vector<pair<NeuralNetworkInfo, bool>>& vecNetworkInfos, vector<string>& parameternames, \
	map<int, vector<int>>& transferTIdx, map<int, vector<int>>& netInputIdx) {
	// 步骤一：对神经网络输入的处理
	unordered_map<string, int> paraNameToIdx;
	for (int i = 0; i < parameternames.size(); i++) {
		paraNameToIdx[parameternames[i]] = i;
	}
	for (int i = 0; i < vecNetworkInfos.size(); i++) {
		if (vecNetworkInfos[i].second == false) {
			continue;
		}
		vector<string> inputVarNames = vecNetworkInfos[i].first.inputVarNames;
		for (string inputvarname : inputVarNames) {
			netInputIdx[i].push_back(paraNameToIdx[inputvarname]);
		}
	}
		
	
	// 步骤二：对神经网络输出的处理
	// 保存所有跑神经网络的输出值
	vector<string> allOutputNames;
	// 保存欧拉角转换后的输出名称
	vector<string> transferNames;
	for (int i = 0; i < vecNetworkInfos.size(); i++) {
		if (vecNetworkInfos[i].second == false) {
			continue;
		}
		// 模型加载
		vecNetworkInfos[i].first.initNetModule();

		// 填充三个数组
		vector<string> outputVarNames = vecNetworkInfos[i].first.outputVarNames;
		vector<int> idxes(3, -1);
		for (int j = 0; j < outputVarNames.size(); j++) {
			allOutputNames.push_back(outputVarNames[j]);
			int idxAlpha = outputVarNames[j].find(".Alpha");
			if (idxAlpha != string::npos) {
				// 判断是否末尾是".Alpha"
				string sRight = ".Alpha";
				string sNowFullName = outputVarNames[j];
				//if (sNowFullName.substr(sNowFullName.length() - sRight.length()) == sRight) 
				{
					string sCompFullName = sNowFullName.substr(0, sNowFullName.length() - sRight.length());
					// 更新索引
					idxes[0] = j;
					// 保存欧拉角转换后的输出名称
					transferNames.push_back(sCompFullName + ".frame_a.R.T[1, 1]");
					transferNames.push_back(sCompFullName + ".frame_a.R.T[1, 2]");
					transferNames.push_back(sCompFullName + ".frame_a.R.T[1, 3]");
					transferNames.push_back(sCompFullName + ".frame_a.R.T[2, 1]");
					transferNames.push_back(sCompFullName + ".frame_a.R.T[2, 2]");
					transferNames.push_back(sCompFullName + ".frame_a.R.T[2, 3]");
					transferNames.push_back(sCompFullName + ".frame_a.R.T[3, 1]");
					transferNames.push_back(sCompFullName + ".frame_a.R.T[3, 2]");
					transferNames.push_back(sCompFullName + ".frame_a.R.T[3, 3]");
				}

			}
			int idxBeta = outputVarNames[j].find(".Beta");
			if (idxBeta != string::npos) {
				idxes[1] = j;
			}
			int idxGamma = outputVarNames[j].find(".Gamma");
			if (idxGamma != string::npos) {
				idxes[2] = j;
			}
		}

		
		if (idxes[0] != -1) {
			transferTIdx[i] = idxes;
		}
	}

	// 填充到parameternames中
	for (string outputname : allOutputNames) {
		parameternames.push_back(outputname);
	}

	for (string transfername : transferNames) {
		parameternames.push_back(transfername);
	}
}


void TwinAnimationThread::runNeuralNetwork(vector<pair<NeuralNetworkInfo, bool>>& vecNetworkInfos, vector<string>& datas, \
	map<int, vector<int>>& transferTIdx, map<int, vector<int>>& netInputIdx) {
	vector<float> vecFloDatas;
	for (string data : datas) {
		vecFloDatas.push_back(stof(data));
	}

	// 输出
	vector<float> outputDatas;
	// 转换矩阵T
	vector<float> transferTDatas;

	for (int i = 0; i < vecNetworkInfos.size(); i++) {
		if (vecNetworkInfos[i].second == false) {
			continue;
		}
		vector<int> inputidx = netInputIdx[i];
		vector<float> inputvec;
		for (int idx : inputidx) {
			inputvec.push_back(vecFloDatas[idx]);
		}

		// Creat a vector of inputs
		std::vector<torch::jit::IValue> inputs;
		inputs.push_back(torch::from_blob(inputvec.data(), inputvec.size(), torch::kFloat).to(at::kCUDA));

		// Execute the model and turn its output into a tensor.
		auto o = vecNetworkInfos[i].first.getNetModule()->module_.forward(inputs);
		at::Tensor output = o.toTensor();

		// Tensor张量转vector数组
		// 要转到CPU上，不转就报错
		at::Tensor t = output.toType(torch::kFloat).clone().to(at::kCPU);
		std::vector<float> outputvec(t.data_ptr<float>(), t.data_ptr<float>() + t.numel());

		for (double outputdata : outputvec) {
			outputDatas.push_back(outputdata);
		}

		// 如果需要矩阵转换，进行矩阵转换
		if (transferTIdx.count(i)) {
			vector<int> transferidx = transferTIdx[i];
			vector<float> eulerAngles;
			for (int idx : transferidx) {
				eulerAngles.push_back(outputvec[idx]);
			}
			vector<float> TDatas = eulerToMatrixT(eulerAngles);
			for (double tdata : TDatas) {
				transferTDatas.push_back(tdata);
			}
		}
	}

	for (float outputdata : outputDatas) {
		vecFloDatas.push_back(outputdata);
	}

	for (float tdata : transferTDatas) {
		vecFloDatas.push_back(tdata);
	}

	vector<string> ret;
	for (float flodata : vecFloDatas) {
		ret.push_back(std::to_string(flodata));
	}
	datas = ret;
}

vector<float> TwinAnimationThread::eulerToMatrixT(vector<float>& eulerAngles) {
	double Alpha = eulerAngles[0];
	double Beta = eulerAngles[1];
	double Gamma = eulerAngles[2];

	// 弧度值
	double r11 = cos(Alpha) * cos(Beta);
	double r12 = cos(Alpha) * sin(Beta) * sin(Gamma) - sin(Alpha) * cos(Gamma);
	double r13 = cos(Alpha) * sin(Beta) * cos(Gamma) + sin(Alpha) * sin(Gamma);
	double r21 = sin(Alpha) * cos(Beta);
	double r22 = sin(Alpha) * sin(Beta) * sin(Gamma) + cos(Alpha) * cos(Gamma);
	double r23 = sin(Alpha) * sin(Beta) * cos(Gamma) - cos(Alpha) * sin(Gamma);
	double r31 = (-1.0) * sin(Beta);
	double r32 = cos(Beta) * sin(Gamma);
	double r33 = cos(Beta) * cos(Gamma);

	vector<float> ret;
	ret.push_back(r11);
	ret.push_back(r12);
	ret.push_back(r13);
	ret.push_back(r21);
	ret.push_back(r22);
	ret.push_back(r23);
	ret.push_back(r31);
	ret.push_back(r32);
	ret.push_back(r33);
	return ret;
}

void TwinAnimationThread::run()
{
	//匹配DigitaltwinsWidget,便于之后向DigitaltwinsWidget同步发送数据
	MainWindow* pWnd = (MainWindow*)m_pModel->parent();
	DigitaltwinsWidget* m_DigWid = pWnd->GetDigitaltwinsWidget();
	connect(this, SIGNAL(SendReceivedMsg()), m_DigWid, SLOT(ShowReceivedMsg()), Qt::BlockingQueuedConnection);
	
	//先接收参数信息
	while ((m_pStoredData->GetParameterNmaes().size()) == 0)
	{
		Sleep(500);
	}

	vector<string> parameternames = m_pStoredData->GetParameterNmaes();
	string str_paranames;
	str_paranames += "参数名称：";
	for (int i = 0; i < parameternames.size(); i++)
	{
		str_paranames += parameternames[i];
		str_paranames += "  ";
	}
	LastReceivedMsg = str_paranames;
	emit SendReceivedMsg();

	// 跳出while循环，而后加载module_
	while (m_DigWid->getCanLoadNet() == false)
	{
		Sleep(500);
	}

	
	// 加载神经网络
	vector<pair<NeuralNetworkInfo, bool>> vecNetworkInfos = m_DigWid->getNetworkInfos();
	// 保存需要进行欧拉角转矩阵的下标（下标针对的是不同组件的outputVarNames，按照".Alpha" ".Beta" ".Gamma"顺序）
	map<int, vector<int>> transferTIdx;
	// 保存不同神经网络的输入元素索引
	map<int, vector<int>> netInputIdx;
	if (m_DigWid->getIsUseNet()) {
		initNeuralNetwork(vecNetworkInfos, parameternames, transferTIdx, netInputIdx);
	}

	
	//读取组件位姿所在列
	map<string, vector<int>> mapCompnameCols; //【组件全名--位姿所在列表】
	map<string, vector<int>> mapCompnameFrameBPosCols;
	map<string, int> mapCompnameRotateCol;
	map<string, int> mapCompnameTransCol;
	bool b0 = GetAllMultibodyColsData(m_pModel, parameternames, mapCompnameCols, mapCompnameRotateCol, mapCompnameTransCol, mapCompnameFrameBPosCols);
	if (!b0)
	{
		//提示错误;
		assert(false);
		return;
	}


	//如果目前还没开始接收，先不读取数据
	while (m_pStoredData->isreceiving == false)
	{
		Sleep(500);
	}

	vector<gp_Vec> vecDeltaP0;                //记录frame_a的偏移
	map<string, double> mapCompRab;           //记录frame_a到frame_b的r的初始长度
	int nowreaded = 0;                        //目前已经读到的行数
	while (1) {
		pair<bool, vector<string>> nowPair = m_pStoredData->GetRowData(nowreaded);
		if (nowPair.first == false) {
			if (nowreaded > 1 && m_pStoredData->GetRowData(nowreaded - 1).first == false) {
				nowreaded = 1;
			}
		}else{
			vector<string> nowdata = nowPair.second;
			string now_str_data;
			now_str_data += "收到数据：";
			for (int i = 0; i < nowdata.size(); i++)
			{
				now_str_data += nowdata[i];
				now_str_data += "  ";
			}
			LastReceivedMsg = now_str_data;
			// FIXME 注释掉这一句，看看能否提高效率
			emit SendReceivedMsg();

			// 对接收到的数据用神经网络进行处理
			if (m_DigWid->getIsUseNet()) {
				runNeuralNetwork(vecNetworkInfos, nowdata, transferTIdx, netInputIdx);
			}

			//利用接收到的数据驱动组件运动
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
					auto itr_i = mapCompnameCols.find(sComFullName);
					if (itr_i != mapCompnameCols.end())
					{//多体运动
						vector<int> CompnameCols = itr_i->second;
						trsf = GetTrsfFromCols(nowdata, CompnameCols);
						//如果是变形零件，则放缩它！
						if (pCom->IsDeformable())
						{
							//增加放缩
							auto itr_fb = mapCompnameFrameBPosCols.find(sComFullName);
							assert(itr_fb != mapCompnameFrameBPosCols.end());
							vector<int> CompnameFrameBPosCols = itr_fb->second;
							gp_Pnt frameBp0 = GetPntFromCols(nowdata, CompnameFrameBPosCols);

							//frame_a的位置
							gp_Pnt p0FA = gp_Pnt(trsf.Value(1, 4), trsf.Value(2, 4), trsf.Value(3, 4));
							if (nowreaded == 0) //初始长度
							{
								gp_Vec rab = gp_Vec(p0FA, frameBp0);
								double r_ab = rab.Magnitude();
								mapCompRab.insert(make_pair(pCom->GetCompName(), r_ab));
							}

							DisplaceScaleComponent(m_pModel, pCom, trsf, nowreaded, k_vec++, vecDeltaP0, mapCompRab, frameBp0);
						}
						else
						{
							DisplaceComponent_old(m_pModel, pCom, trsf, nowreaded, k_vec++, vecDeltaP0);
						}
					}
					else {
						//旋转
						auto itr_R = mapCompnameRotateCol.find(sComFullName);
						if (itr_R != mapCompnameRotateCol.end())
						{//旋转运动
							vector<int> vCols;
							vCols.push_back(itr_R->second);
							vector<double> v1 = GetPlacementValues(nowdata, vCols);							
							double rotRad = v1[0];
							//得到连接的刚体
							string sLinkRigid = m_pModel->GetLinkRigid(sComFullName.c_str());
							if (!sLinkRigid.empty())
							{
								//跟随刚体一起运动
								auto itr_i = mapCompnameCols.find(sLinkRigid);
								if (itr_i != mapCompnameCols.end())
								{//刚体运动
									vector<int> CompnameCols = itr_i->second;
									trsf = GetTrsfFromCols(nowdata, CompnameCols);
									DisplaceRComponent(m_pModel, pCom, sLinkRigid.c_str(), trsf, rotRad, nowreaded, k_vec++, vecDeltaP0);
								}
							}
							else {
								//自己转动
								RotateComponent(m_pModel, pCom, rotRad);
							}
						}
						else {
							//平移
							auto itr_T = mapCompnameTransCol.find(sComFullName);
							if (itr_T != mapCompnameTransCol.end())
							{//运动
								assert(false);
							}
						}
					}
				}
			}
			pauseLock.lock();
			emit rep();
			pauseLock.unlock();
			nowreaded++;
			nowreaded %= 10000;
		}
	}
}

void TwinAnimationThread::closeThread()
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

//读取组件位姿所在列信息
bool TwinAnimationThread::GetAllMultibodyColsData(DocumentModel* pMdl, vector<string>& parameternames, map<string, vector<int>>& mapCompnameCols, \
	map<string, int>& mapCompnameRotateCol, map<string, int>& mapCompnameTransCol, map<string, vector<int>>& mapCompnameFrameBPosCols) {
	for (int i = 0; i < parameternames.size(); i++)
	{
		string sColName = parameternames[i];
		if (sColName == "time" || sColName == "Time")
		{
			//时间列，先暂时不做处理
		}
		else
		{
            //刚体
			string sCompFullName;
			int nType = GetComponentConnectType(pMdl, sColName, sCompFullName);
			if (nType == 1)
			{
				//是多体组件的位姿变量
				//是否已经搜索
				if (mapCompnameCols.find(sCompFullName) == mapCompnameCols.end())
				{
					vector<int> vCols = GetAllCompCols(sCompFullName, parameternames);
					if (vCols.size() == 12)
					{//是的，位姿有r_0[]3个和R.T[3,3]的9个，一个12列
						mapCompnameCols.insert(make_pair(sCompFullName, vCols));
					}
				}
				//Frame_b的位置列
				if (mapCompnameFrameBPosCols.find(sCompFullName) == mapCompnameFrameBPosCols.end())
				{
					vector<int> vCols = GetAllCompCols3(sCompFullName, parameternames);
					if (vCols.size() == 3)
					{//是的，frame_b的位置有r_0[]3个
						mapCompnameFrameBPosCols.insert(make_pair(sCompFullName, vCols));
					}
				}
			}
			//旋转
			else if (nType == 2)
			{
				mapCompnameRotateCol.insert(make_pair(sCompFullName, i));
			}
			//平移
			else if (nType == 3)
			{
				mapCompnameTransCol.insert(make_pair(sCompFullName, i));
			}
			else {
				;//暂不去管他
			}
		}
	}
	return true;
}

int TwinAnimationThread::GetComponentConnectType(DocumentModel* pMdl, const string& sColName, string& sCompFullName)
{
	int nType; //1-frame_a.r_0, 2-flange_a.phi,3-flange_a.s

	//int pos = sColName.indexOf(".r_0[1]");
	string sRight = ".r_0[1]";
	if (sColName.substr(sColName.length() - sRight.length()) == sRight)
	{
		//例如：sColName---Revolute1.frame_a.r_0[1]
		//      sConnectorName---frame_a
		//      sCompFullName---Revolute1
		sCompFullName = sColName.substr(0, sColName.length() - sRight.length());
		int pos = sCompFullName.find_last_of(".");
		string sConnectorName = sCompFullName.substr(pos + 1);
		sCompFullName = sCompFullName.substr(0, pos);

		Component* pCom = pMdl->GetComponent()->GetChild(sCompFullName.c_str());
		if (!pCom)
			return 0;
		Connector* pConn = ((DocumentComponent*)pCom->GetDocument())->GetConnector(sConnectorName.c_str());
		if (pConn && pConn->GetConnType() == "Modelica.Mechanics.MultiBody.Interfaces.Frame_a")
		{
			return 1;
		}
		else {
			return 0;
		}
	}

	sRight = ".phi";
	if (sColName.substr(sColName.length() - sRight.length()) == sRight)
	{
		sCompFullName = sColName.substr(0, sColName.length() - sRight.length());
		int pos = sCompFullName.find_last_of(".");
		string sConnectorName = sCompFullName.substr(pos + 1);
		sCompFullName = sCompFullName.substr(0, pos);

		Component* pCom = pMdl->GetComponent()->GetChild(sCompFullName.c_str());
		if (!pCom)
			return 0;
		Connector* pConn = ((DocumentComponent*)pCom->GetDocument())->GetConnector(sConnectorName.c_str());
		if (pConn && (pConn->GetConnType() == "Modelica.Mechanics.Rotational.Interfaces.Flange_a" \
			|| pConn->GetConnType() == "Modelica.Mechanics.Rotational.Interfaces.Flange_b"))
		{
			return 2;
		}
		else {
			return 0;
		}
	}

	sRight = ".s";
	if (sColName.substr(sColName.length() - sRight.length()) == sRight)
	{
		sCompFullName = sColName.substr(0, sColName.length() - sRight.length());
		int pos = sCompFullName.find_last_of(".");
		string sConnectorName = sCompFullName.substr(pos + 1);
		sCompFullName = sCompFullName.substr(0, pos);

		Component* pCom = pMdl->GetComponent()->GetChild(sCompFullName.c_str());
		if (!pCom)
			return 0;
		Connector* pConn = ((DocumentComponent*)pCom->GetDocument())->GetConnector(sConnectorName.c_str());
		if (pConn && (pConn->GetConnType() == "Modelica.Mechanics.Translational.Interfaces.Flange_a" \
			|| pConn->GetConnType() == "Modelica.Mechanics.Translational.Interfaces.Flange_b"))
		{
			return 3;
		}
		else {
			return 0;
		}
	}
	return 5;
}

//获取组件名称+frame_a所在列
vector<int> TwinAnimationThread::GetAllCompCols(const string& sCompFullName, const vector<string>& parameternames)
{
	vector<int> vCols;

	string strCol1[12];//如果传值过来的求解器是openmodelica
	string strCol2[12];//如果传值过来的求解器是MWorks
	//if (SolverName == "openmodelica")
	{
		strCol1[3] = sCompFullName + ".frame_a.r_0[1]";
		strCol1[7] = sCompFullName + ".frame_a.r_0[2]";
		strCol1[11] = sCompFullName + ".frame_a.r_0[3]";
		strCol1[0] = sCompFullName + ".frame_a.R.T[1,1]";
		strCol1[1] = sCompFullName + ".frame_a.R.T[1,2]";
		strCol1[2] = sCompFullName + ".frame_a.R.T[1,3]";
		strCol1[4] = sCompFullName + ".frame_a.R.T[2,1]";
		strCol1[5] = sCompFullName + ".frame_a.R.T[2,2]";
		strCol1[6] = sCompFullName + ".frame_a.R.T[2,3]";
		strCol1[8] = sCompFullName + ".frame_a.R.T[3,1]";
		strCol1[9] = sCompFullName + ".frame_a.R.T[3,2]";
		strCol1[10] = sCompFullName + ".frame_a.R.T[3,3]";
	}
	//else if (SolverName == "MWorks")
	{
		strCol2[3] = sCompFullName + ".frame_a.r_0[1]";
		strCol2[7] = sCompFullName + ".frame_a.r_0[2]";
		strCol2[11] = sCompFullName + ".frame_a.r_0[3]";
		strCol2[0] = sCompFullName + ".frame_a.R.T[1, 1]";
		strCol2[1] = sCompFullName + ".frame_a.R.T[1, 2]";
		strCol2[2] = sCompFullName + ".frame_a.R.T[1, 3]";
		strCol2[4] = sCompFullName + ".frame_a.R.T[2, 1]";
		strCol2[5] = sCompFullName + ".frame_a.R.T[2, 2]";
		strCol2[6] = sCompFullName + ".frame_a.R.T[2, 3]";
		strCol2[8] = sCompFullName + ".frame_a.R.T[3, 1]";
		strCol2[9] = sCompFullName + ".frame_a.R.T[3, 2]";
		strCol2[10] = sCompFullName + ".frame_a.R.T[3, 3]";
	}

	for (int j = 0; j < 12; j++)
	{
		string sVarName1 = strCol1[j];
		string sVarName2 = strCol2[j];
		for (int i = 0; i < parameternames.size(); i++)
		{
			if (sVarName1 == parameternames[i] || sVarName2 == parameternames[i])
			{
				vCols.push_back(i);
				break;
			}
		}
	}

	return vCols;
}

//获取组件名称+frame_b所在列
vector<int> TwinAnimationThread::GetAllCompCols3(const string& sCompFullName, const vector<string>& parameternames)
{
	vector<int> vCols;

	string strCol[3];
	strCol[0] = sCompFullName + ".frame_b.r_0[1]";
	strCol[1] = sCompFullName + ".frame_b.r_0[2]";
	strCol[2] = sCompFullName + ".frame_b.r_0[3]";

	for (int j = 0; j < 3; j++)
	{
		string sVarName = strCol[j];
		for (int i = 0; i < parameternames.size(); i++)
		{
			if (sVarName == parameternames[i])
			{
				vCols.push_back(i);
				break;
			}
		}
	}

	return vCols;
}

//从当前数据列中获取gp_Trsf
gp_Trsf TwinAnimationThread::GetTrsfFromCols(const vector<string>& nowdata, const vector<int>& CompnameCols) {
	vector<double> v12 = GetPlacementValues(nowdata, CompnameCols);
	gp_Trsf trsf = MakeTransFromV12(v12);
	return trsf;
}

//从当前数据列中获取gp_Pnt
gp_Pnt TwinAnimationThread::GetPntFromCols(const vector<string>& nowdata, const vector<int>& CompnameFrameBPosCols) {
	vector<double> v3 = GetPlacementValues(nowdata, CompnameFrameBPosCols);
	gp_Pnt pnt(v3[0], v3[1], v3[2]);
	return pnt;
}

vector<double> TwinAnimationThread::GetPlacementValues(const vector<string>& nowdata, const vector<int>& vCols)
{
	vector<double> v12;

	for (int i = 0; i < vCols.size(); i++)
	{
		string sVal = nowdata[vCols[i]];
		v12.push_back(std::stod(sVal));
	}

	return v12;
}

//实现组件的放缩和位姿摆放
void TwinAnimationThread::DisplaceScaleComponent(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
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
	pModel->SetCompLocation(pCom, trsfScale * trsfFrom_TO);
}

//刚体运动部件,旧版
void TwinAnimationThread::DisplaceComponent_old(DocumentModel* pModel, Component* pCom, gp_Trsf& trsf, \
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

	//再选转 trsf
	trsf.Invert();
	//gp_Trsf trsfRot;

	//再平移到计算的位置
	gp_Trsf trsfTr1; //从当前点，移动到原点
	trsfTr1.SetTranslation(Po, p0To);

	pModel->SetCompLocation(pCom, trsfTr1 * trsf * trsfTr0);
}

//先刚体运动，再自转
void TwinAnimationThread::DisplaceRComponent(DocumentModel* pModel, Component* pCom, const char* sRigid, gp_Trsf& trsf, double rotRad, \
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

//旋转部件
void TwinAnimationThread::RotateComponent(DocumentModel* pModel, Component* pCom, double rotRad)
{
	//绕着frame_a的Z轴旋转
	gp_Ax3 ax31 = pModel->GetCompConnGlobalAx3(pCom->GetCompName().c_str(), "frame_a");

	gp_Ax1 ax1 = ax31.Axis();
	gp_Trsf trsf;
	trsf.SetRotation(ax1, rotRad);

	pModel->SetCompLocation(pCom, trsf);
}