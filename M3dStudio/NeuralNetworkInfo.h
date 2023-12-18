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

	void initNetModule();            //初始化网络模型
	LoadModule* getNetModule() {
		return netModule;
	}

	string sNetworkFilePath;         //神经网络文件路径（*.pt）
	string sConfigFilePath;          //配置文件路径（*.csv）
	int inputNums;                   //神经网络输入个数
	int outputNums;                  //神经网络输出个数
	vector<string> inputVarNames;    //神经网络输入参数名称
	vector<string> outputVarNames;   //神经网络输出参数名称
private:
	LoadModule* netModule = nullptr;
};
