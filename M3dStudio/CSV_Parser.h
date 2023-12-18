#pragma once
#ifndef CSV_PARSER_H
#define CSV_PARSER_H

#include <QString>
#include <QStringList>
#include <gp_Trsf.hxx>

class DocumentModel; //���ô�����������

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
        //��ȡһ������
        static QStringList GetDataSeries(const QString& fileName, const char* sColName);
        static QStringList GetAllVariablesNames(const QString& fileName);                   //��ȡ���б�������
        static QStringList GetDataByTime(const QString& fileName, double time);             //���ݷ���ʱ���ȡһ�з�������
        static vector<QStringList> GetDataByNums(const QString& fileName, int nums);        //�ڷ����ļ��о���ȡnums������
        static void GetDDSDatasFromCols(const QString& fileName, vector<int>& sendVarsCols, map<double, vector<string>>& timeToDatas);       //Ԥ����������ͨ�ŵ�DDS��������
    };
};

#endif //CSV_PARSER_H