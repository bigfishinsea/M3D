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
	//ƥ��DigitaltwinsWidget,����֮����DigitaltwinsWidgetͬ����������
	MainWindow* pWnd = (MainWindow*)m_pModel->parent();
	DigitaltwinsWidget* m_DigWid = pWnd->GetDigitaltwinsWidget();
	ErrorMonitorDlg* errorDlg = m_DigWid->getErrorMonitorDlg();
	connect(this, SIGNAL(paraNameError()), errorDlg, SLOT(paraNameErrorMsg()), Qt::BlockingQueuedConnection);
	connect(this, SIGNAL(classifierPathError()), errorDlg, SLOT(classifierPathErrorMsg()), Qt::BlockingQueuedConnection);
	connect(this, SIGNAL(errorAlarm()), errorDlg, SLOT(errorAlarmMsg()), Qt::BlockingQueuedConnection);


	//�Ƚ��ղ�����Ϣ
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

	int nowreaded = 0;                        //Ŀǰ�Ѿ�����������
	double lastDetectionTime = ms.count();    //��һ�μ���ʱ��
	vector<double> windowData;                //��ʱ�䴰���ڼ�������
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
			
			double nowTime = ms.count();    //��һ�μ���ʱ��
			if (nowTime - lastDetectionTime >= windowLength) {
				lastDetectionTime = nowTime;
				// 1.�����ݴ��Ϊcsv�ļ�
				CSV_Parser::Sheet sheet;
				// ���ñ�����
				sheet.name = "paraName";

				// ���õ�һ�в�������
				QStringList strListNum;
				strListNum.append(QString::fromStdString(paraName));
				sheet.data.append(strListNum);


				// ����ʣ��������
				for (int i = 0; i < windowData.size(); i++) {
					QStringList strList;
					strList.append(QString::fromStdString(std::to_string(windowData[i])));
					sheet.data.append(strList);
				}

				// ����������ṹ�ļ�
				QString netFrmPath = "./getFeatures/getFeaturesFromData/for_redistribution_files_only/dataFile.csv";
				bool isnowsuccess = FileParse::generateCSV(netFrmPath, sheet);
				windowData = vector<double>();

				
				// 2.���exe�ļ���ȡ����
				// �������·���µĿ�ִ���ļ�
				system("./getFeatures/getFeaturesFromData/for_redistribution_files_only/getFeaturesFromData.exe");
				// �ȴ�5����
				QThread::sleep(5);

				// 3.��÷�����������
				bool isError = false;
				/*
				* �������ʹ�ö��ַ��������ڴ˽���ʹ�����ɭ�ַ�����ʵ�ֵĲ���
				* #include <scikit-learn.h>
				* auto model = sl::load_model("random_forest_model.joblib"); // ����ģ��
				* �������ݴӸ�·���»�ã�./getFeatures/getFeaturesFromData/for_redistribution_files_only/featureFile.csv
				* std::vector<float> data = {5.1, 3.5, 1.4, 0.2}; // ׼����������
				* auto prediction = model.predict(data); // ����Ԥ��						
				*/
				
				if (isError) {
					emit errorAlarm();
				}

				// 4.��¼��־��Ϣ
				 // ����־�ļ�
				std::ofstream logfile("logfile.txt", std::ios::app); // ios::app ��ʾ��׷��ģʽ���ļ�

				if (!logfile.is_open()) {
					continue;
				}

				// ��ȡ��ǰʱ��
				auto now = std::chrono::system_clock::now();
				auto now_time = std::chrono::system_clock::to_time_t(now);

				// ����ǰʱ��д����־�ļ�
				logfile << "Current time: " << std::ctime(&now_time);

				// д��������Ϣ
				if (isError) {
					logfile << "This is an error." << std::endl;
				}
				else {
					logfile << "No error." << std::endl;
				}
				

				// �ر���־�ļ�
				logfile.close();

			}
		}
		nowreaded++;
		nowreaded %= 10000;
	}
}