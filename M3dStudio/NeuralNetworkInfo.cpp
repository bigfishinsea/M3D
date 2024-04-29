#include "NeuralNetworkInfo.h"

//------------------------------------------------------------
//MyLoadModule
MyLoadModule::MyLoadModule(const std::string& model_path, bool use_gpu) :device_(torch::kCPU)
{
	if (torch::cuda::is_available() && use_gpu)
	{
		device_ = torch::kCUDA;
	}
	else
	{
		device_ = torch::kCPU;
	}

	try {
		torch::Device device(device_);
		module_ = torch::jit::load(model_path, device);
	}
	catch (const c10::Error& e) {
		std::cerr << "Error loading the model!\n";
		std::exit(EXIT_FAILURE);
	}

	module_.to(device_);

	module_.eval();
}

//------------------------------------------------------------
//NeuralNetworkInfo

NeuralNetworkInfo::NeuralNetworkInfo() {
	netModule = nullptr;
}

NeuralNetworkInfo::~NeuralNetworkInfo() {
	if (netModule != nullptr) {
		delete netModule;
	}
}

void NeuralNetworkInfo::initNetModule() {
	if (sNetworkFilePath == "") {
		return;
	}
	netModule = new MyLoadModule(sNetworkFilePath);
	return;
}