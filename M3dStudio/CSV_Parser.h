#pragma once
#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <QString>
#include <QStringList>
#include <gp_Trsf.hxx>

class DocumentModel; //不好处理，先这样吧

namespace CSV_Parser
{
    class Sheet
    {
        enum FieldType { STRING, INT, DOUBLE, BOOL };
    public:
        Sheet() {}
        //Sheet(Sheet&& rhs);
        //Sheet& operator =(Sheet&& rhs);
    public:
        QString name;
        QList<QStringList> data;
        QList<FieldType> fieldTypes;
    };

    class FileParse
    {
    private:
        static QStringList splitCSVLine(const QString& lineStr);
        static QString     joinCSVStrs(const QStringList& strList);

    public:
        static bool GetAllMultibodyPlacement(DocumentModel* pMdl,const QString& fileName, vector<double>& timeSeries,map<string, vector<gp_Trsf>>& mapCompPlacement,
            map<string, vector<double>>& mapCompRotation, map<string, vector<double>>& mapCompTranslation, map<string, vector<gp_Pnt>>& mapFramebPos);
        static Sheet parseCSV(const QString& fileName);
        static bool  generateCSV(const QString& fileName, const Sheet& sheet);
        //获取一列数据
        static QStringList GetDataSeries(const QString& fileName, const char* sColName);
        static QStringList GetAllVariablesNames(const QString& fileName);                   //获取所有变量名称
        static QStringList GetDataByTime(const QString& fileName, double time);             //根据仿真时间获取一行仿真数据
        static vector<QStringList> GetDataByNums(const QString& fileName, int nums);        //在仿真文件中均匀取nums行数据
        static void GetDDSDatasFromCols(const QString& fileName, vector<int>& sendVarsCols, map<double, vector<string>>& timeToDatas);       //预读用于数据通信的DDS发送数据
    };
};

#endif //CSV_PARSER_H