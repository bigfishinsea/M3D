#pragma once
#ifdef DDSDATACOM
#define DLL __declspec(dllexport)
#else
#define DLL __declspec(dllimport)
#endif
#include "DemoTypeSupportImpl.h"
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

	//����������
	void AddRowData(vector<string> rowdata)
	{
		AllDatas.push_back(rowdata);
	};

	vector<string> GetParameterNmaes() {
		return ParameterNames;
	}

private:
	vector<string> ParameterNames;

public:
	vector<vector<string>> AllDatas;
	vector<double> time;
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