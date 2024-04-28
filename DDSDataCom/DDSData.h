#pragma once
#ifdef DDSDATACOM
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif
#include "DemoTypeSupportImpl.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace DemoIdlModule;
using namespace std;

class DLL XData {
public:
	double time;
	vector<string> vec_Data;
	
	string VectoStr(vector<string> vec_Data);
	vector<string> StrtoVec(string data);
};

class DLL StoredData {
public:
	StoredData();
	~StoredData();

	//���ر���
	bool isreceiving = false;
	
	//���ò�������
	void SetParameterNames(vector<string> parameternames) {
		ParameterNames = parameternames;
	};

	vector<string> GetParameterNmaes() {
		return ParameterNames;
	}

	//����������
	void AddRowData(vector<string> rowdata)
	{
		if (AllDatas.size() > 10000) {
			vector<string> originData = AllDatas[0];
			// ���浽�ļ�
			string filePath = "output" + std::to_string(nextSavedFile) + ".txt";
			nextSavedFile++;
			nextSavedFile %= maxSavedFiles;
			SaveToFile(AllDatas, filePath);
			AllDatas = vector<vector<string>>(1);
			AllDatas[0] = originData;
		}
		AllDatas.push_back(rowdata);
	};

	pair<bool, vector<string>> GetRowData(int row) {
		if (row < AllDatas.size()) {
			return make_pair(true, AllDatas[row]);
		}
		else {
			return make_pair(false, vector<string>());
		}
	}

	vector<vector<string>> GetAllDatas() {
		return AllDatas;
	}
private:
	vector<string> ParameterNames;
	
	void SaveToFile(const std::vector<std::vector<std::string>>& data, const std::string& filename) {
		std::ofstream outputFile(filename, std::ofstream::trunc);

		if (!outputFile.is_open()) {
			return;
		}

		// ������д���ļ�
		for (const auto& row : data) {
			for (const auto& value : row) {
				outputFile << value << ";";
			}
			outputFile << "\n"; // ��ÿһ�н���ʱ��ӻ��з�
		}

		outputFile.close();
	}

	vector<vector<string>> AllDatas;
	int maxSavedFiles = 10;
	int nextSavedFile = 0;
};


//�⼸���ǲ����õĺ�������Ϊ���ӣ�ʵ�����ò���
DLL int ACE_TMAIN_Sub(int argc, ACE_TCHAR* argv[]);
DLL int ACE_TMAIN_Pub(int argc, ACE_TCHAR* argv[]);
DLL int testfunction_sub(int data);
DLL int testfunction_pub(int data);
DLL int ACE_TMAIN_XDataPub(int argc, ACE_TCHAR* argv[]);

//M3dStudio����Ҫ���õĺ���
DLL int GenXDataMembers(int discoveryMehodID);                           //��ʼ��XData�����ݳ�Ա����ͬ����ֵ����ͬ���󣬷���0��ʾ���ɳɹ�
DLL int SendXData(string messageId, string destinationId, XData data);   //����XData��Ϣ
DLL string GetSendedMsg();                                               //��ȡ������Ϣ��������GUI��������ʾ
DLL int ClearXDataMembers();                                             //�������������XData�ĳ�Ա

DLL int InitSubscribe(StoredData* m_SortedData, int discoveryMehodID);   //��ʼ������
DLL void SetSubhomeID(string homeid);                                    //���ö����ߵı���id
DLL void StartSubscribe();                                               //��ʼ����
DLL int EndSubscribe();                                                  //��������
DLL void SetRecvMsgId(string msgids);                                    //���ö����ߵĽ�����Ϣid��������Ϣ���ˣ�