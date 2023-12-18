#include "libLoadThread.h"
#include "ModelicaModel.h"

//此处定义
vector<ModelicaModel*> g_vModelicaModels;

LibLoadThread::LibLoadThread(QObject* pParent)
	: QThread(pParent)
{

}

void LibLoadThread::AddLibPath(string sMoLibPath)
{
	m_vLibPaths.push_back(sMoLibPath);
}

//对每个路径进行加载
void LibLoadThread::run()
{
	for (int i = 0; i < m_vLibPaths.size(); i++)
	{
		ModelicaModel* pMdl = ModelicaModel::LoadMoLibrary(m_vLibPaths[i].c_str());
		if(pMdl)
			g_vModelicaModels.push_back(pMdl);
	}
}
