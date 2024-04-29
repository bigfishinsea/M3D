#include "DigitaltwinsWidget.h"
#include "ErrorMonitorThread.h"
#include <chrono>
#include "CSV_Parser.h"
#include <cstdlib>

using namespace std;
using namespace CSV_Parser;


ErrorMonitorThread::ErrorMonitorThread(QObject* pParent)
	: QThread(pParent)
{

}

void ErrorMonitorThread::run() {
	//匹配DigitaltwinsWidget,便于之后向DigitaltwinsWidget同步发送数据
	MainWindow* pWnd = (MainWindow*)m_pModel->parent();
	DigitaltwinsWidget* m_DigWid = pWnd->GetDigitaltwinsWidget();
	ErrorMonitorDlg* errorDlg = m_DigWid->getErrorMonitorDlg();
	connect(this, SIGNAL(paraNameError()), errorDlg, SLOT(paraNameErrorMsg()), Qt::BlockingQueuedConnection);
	connect(this, SIGNAL(classifierPathError()), errorDlg, SLOT(classifierPathErrorMsg()), Qt::BlockingQueuedConnection);
	connect(this, SIGNAL(errorAlarm()), errorDlg, SLOT(errorAlarmMsg()), Qt::BlockingQueuedConnection);


	//先接收参数信息
	while ((m_pStoredData->GetParameterNmaes().size()) == 0)
	{
		Sleep(500);
	}

	vector<string> parameternames = m_pStoredData->GetParameterNmaes();
	int paraIdx = -1;
	for (int i = 0; i < parameternames.size(); i++) {
		if (parameternames[i] == paraName) {
			paraIdx = i;
			break;
		}
	}
	if (paraIdx == -1) {
		emit paraNameError();
		return;
	}
	
	std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::seconds>(
		std::chrono::system_clock::now().time_since_epoch()
		);

	int nowreaded = 0;                        //目前已经读到的行数
	double lastDetectionTime = ms.count();    //上一次监测的时间
	vector<double> windowData;                //该时间窗口内监测的数据
	while (1) {
		pair<bool, vector<string>> nowPair = m_pStoredData->GetRowData(nowreaded);
		if (nowPair.first == false) {
			if (nowreaded > 1 && m_pStoredData->GetRowData(nowreaded - 1).first == false) {
				nowreaded = 1;
			}
		}
		else {
			vector<string> nowdatas = nowPair.second;
			double nowData = stod(nowdatas[paraIdx]);
			windowData.push_back(nowData);
			
			double nowTime = ms.count();    //上一次监测的时间
			if (nowTime - lastDetectionTime >= windowLength) {
				lastDetectionTime = nowTime;
				// 1.将数据打包为csv文件
				CSV_Parser::Sheet sheet;
				// 设置表名称
				sheet.name = "paraName";

				// 设置第一行参数名称
				QStringList strListNum;
				strListNum.append(QString::fromStdString(paraName));
				sheet.data.append(strListNum);


				// 设置剩余行数据
				for (int i = 0; i < windowData.size(); i++) {
					QStringList strList;
					strList.append(QString::fromStdString(std::to_string(windowData[i])));
					sheet.data.append(strList);
				}

				// 生成神经网络结构文件
				QString netFrmPath = "./getFeatures/getFeaturesFromData/for_redistribution_files_only/dataFile.csv";
				bool isnowsuccess = FileParse::generateCSV(netFrmPath, sheet);
				windowData = vector<double>();

				
				// 2.外调exe文件提取特征
				// 调用相对路径下的可执行文件
				system("./getFeatures/getFeaturesFromData/for_redistribution_files_only/getFeaturesFromData.exe");
				// 等待5秒钟
				QThread::sleep(5);

				// 3.获得分类器分类结果
				bool isError = false;
				/*
				* 这里可以使用多种分类器，在此介绍使用随机森林分类器实现的步骤
				* #include <scikit-learn.h>
				* auto model = sl::load_model("random_forest_model.joblib"); // 加载模型
				* 测试数据从该路径下获得：./getFeatures/getFeaturesFromData/for_redistribution_files_only/featureFile.csv
				* std::vector<float> data = {5.1, 3.5, 1.4, 0.2}; // 准备测试数据
				* auto prediction = model.predict(data); // 进行预测						
				*/
				
				if (isError) {
					emit errorAlarm();
				}

				// 4.记录日志信息
				 // 打开日志文件
				std::ofstream logfile("logfile.txt", std::ios::app); // ios::app 表示以追加模式打开文件

				if (!logfile.is_open()) {
					continue;
				}

				// 获取当前时间
				auto now = std::chrono::system_clock::now();
				auto now_time = std::chrono::system_clock::to_time_t(now);

				// 将当前时间写入日志文件
				logfile << "Current time: " << std::ctime(&now_time);

				// 写入其他信息
				if (isError) {
					logfile << "This is an error." << std::endl;
				}
				else {
					logfile << "No error." << std::endl;
				}
				

				// 关闭日志文件
				logfile.close();

			}
		}
		nowreaded++;
		nowreaded %= 10000;
	}
}