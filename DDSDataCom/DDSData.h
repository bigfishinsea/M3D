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

	//开关变量
	bool isreceiving = false;
	
	//设置参数名称
	void SetParameterNames(vector<string> parameternames) {
		ParameterNames = parameternames;
	};

	vector<string> GetParameterNmaes() {
		return ParameterNames;
	}

	//增加行数据
	void AddRowData(vector<string> rowdata)
	{
		if (AllDatas.size() > 10000) {
			vector<string> originData = AllDatas[0];
			// 保存到文件
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

		// 将数据写入文件
		for (const auto& row : data) {
			for (const auto& value : row) {
				outputFile << value << ";";
			}
			outputFile << "\n"; // 在每一行结束时添加换行符
		}

		outputFile.close();
	}

	vector<vector<string>> AllDatas;
	int maxSavedFiles = 10;
	int nextSavedFile = 0;
};


//这几个是测试用的函数，作为例子，实际上用不到
DLL int ACE_TMAIN_Sub(int argc, ACE_TCHAR* argv[]);
DLL int ACE_TMAIN_Pub(int argc, ACE_TCHAR* argv[]);
DLL int testfunction_sub(int data);
DLL int testfunction_pub(int data);
DLL int ACE_TMAIN_XDataPub(int argc, ACE_TCHAR* argv[]);

//M3dStudio中需要调用的函数
DLL int GenXDataMembers(int discoveryMehodID);                           //初始化XData的数据成员，不同返回值代表不同错误，返回0表示生成成功
DLL int SendXData(string messageId, string destinationId, XData data);   //发送XData信息
DLL string GetSendedMsg();                                               //获取发送信息，便于在GUI界面上显示
DLL int ClearXDataMembers();                                             //传输结束，清理XData的成员

DLL int InitSubscribe(StoredData* m_SortedData, int discoveryMehodID);   //初始化订阅
DLL void SetSubhomeID(string homeid);                                    //设置订阅者的本机id
DLL void StartSubscribe();                                               //开始订阅
DLL int EndSubscribe();                                                  //结束订阅
DLL void SetRecvMsgId(string msgids);                                    //设置订阅者的接收消息id（用于消息过滤）