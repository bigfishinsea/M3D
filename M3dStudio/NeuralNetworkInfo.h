#pragma once
#include <torch/torch.h>
#include <torch/script.h>
#include <vector>


using namespace std;

class LoadModule {
public:
	LoadModule(const std::string& model_path, bool use_gpu = true);
	torch::jit::script::Module module_;
	torch::Device device_;
};

class NeuralNetworkInfo {
public:
	NeuralNetworkInfo();
	~NeuralNetworkInfo();

	void initNetModule();            //��ʼ������ģ��
	LoadModule* getNetModule() {
		return netModule;
	}

	string sNetworkFilePath;         //�������ļ�·����*.pt��
	string sConfigFilePath;          //�����ļ�·����*.csv��
	int inputNums;                   //�������������
	int outputNums;                  //�������������
	vector<string> inputVarNames;    //�����������������
	vector<string> outputVarNames;   //�����������������
private:
	LoadModule* netModule = nullptr;
};
