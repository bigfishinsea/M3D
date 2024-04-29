#pragma once
#include "DDSData.h"
#include "DocumentModel.h"
#include <QThread>

class DocumentModel;


class ErrorMonitorThread : public QThread
{
    Q_OBJECT
public:
    ErrorMonitorThread(QObject* parent = 0);
    StoredData* m_pStoredData = nullptr;
    
    void setParaName(string paraname) { this->paraName = paraname; };
    void setClassifierPath(string classifierpath) { this->classifierPath = classifierpath; };
    void setWindowLength(double windowlength) { this->windowLength = windowlength; };
    void SetDocumentModel(DocumentModel* pMdl) { m_pModel = pMdl; }

private:
    string paraName;
    string classifierPath;
    double windowLength;
    DocumentModel* m_pModel;


public:

signals:
    void paraNameError();
    void classifierPathError();
    void errorAlarm();



protected:
    void run(); // 新线程入口

};
