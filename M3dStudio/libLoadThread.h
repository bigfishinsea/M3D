#pragma once
#include "M3d.h"
#include <QThread>

class LibLoadThread : public QThread
{
    Q_OBJECT
public:
    LibLoadThread(QObject* parent = 0);

    void AddLibPath(string sMoLibPath);
    void SetLibPaths(vector<string> vPaths) { m_vLibPaths = vPaths; }

protected:
    void run(); // 新线程入口

private:
    vector<string> m_vLibPaths;
};
