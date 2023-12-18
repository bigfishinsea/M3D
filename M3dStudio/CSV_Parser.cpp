#include "DocumentModel.h"
#include "M3d.h"
#include "CSV_Parser.h"
#include <QFile>
#include <QTextStream>
#include "global.h"
//#include "DocumentModel.h"
#include "DocumentComponent.h"
#include "LibPathsSetDlg.h"
#include "omp.h"
#include <iostream>

using namespace CSV_Parser;

//解析规则为：
//1、若逗号间无数据，仍解释为空数据
//2、若字段中含有逗号则用"将字段包含起来
//3、若数据中存在 " ，则将其替换为 """"

Sheet FileParse::parseCSV(const QString& fileName)
{
    CSV_Parser::Sheet sheet;

    int nameStartIndex = fileName.lastIndexOf('/') + 1;
    if (nameStartIndex < 1)
    {
        nameStartIndex = fileName.lastIndexOf('\\') + 1;
    }
    int nameEndIndex = fileName.lastIndexOf('.');
    sheet.name = fileName.mid(nameStartIndex, nameEndIndex - nameStartIndex);

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return sheet;
    }

    QTextStream inStream(&file);

    for (QString lineStr; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }

        sheet.data.append(splitCSVLine(lineStr));
    }
    file.close();

    return qMove(sheet);
}

QStringList FileParse::splitCSVLine(const QString& lineStr)
{
    QStringList strList;
    QString str;

    int length = lineStr.length();
    int quoteCount = 0;
    int repeatQuoteCount = 0;

    for (int i = 0; i < length; ++i)
    {
        if (lineStr[i] != '\"')
        {
            repeatQuoteCount = 0;
            if (lineStr[i] != ',')
            {
                str.append(lineStr[i]);
            }
            else
            {
                // 字段里面可能有，
                if (quoteCount % 2)
                {
                    str.append(',');
                }
                else
                {
                    strList.append(str);
                    quoteCount = 0;
                    str.clear();
                }
            }
        }
        else
        {
            ++quoteCount;
            ++repeatQuoteCount;
            if (repeatQuoteCount == 4)
            {
                str.append('\"');
                repeatQuoteCount = 0;
                quoteCount -= 4;
            }
        }
    }
    strList.append(str);

    return qMove(strList);
}

bool FileParse::generateCSV(const QString& fileName, const CSV_Parser::Sheet& sheet)
{
    QFile file(fileName);
    bool openOk = file.open(QIODevice::WriteOnly);
    if (!openOk)
    {
        return false;
    }
    QTextStream outStream(&file);

    int strCount = sheet.data.count();
    for (int i = 0; i < strCount; ++i)
    {
        outStream << joinCSVStrs(sheet.data.at(i));
        outStream << '\n';
    }
    file.close();

    return true;
}

QString FileParse::joinCSVStrs(const QStringList& strList)
{
    QString lineStr;

    int strCount = strList.count();
    int lastStrIndex = strCount - 1;

    for (int k = 0; k < strCount; ++k)
    {
        QString tarStr;
        bool commaFlag = false;

        const QString& oriStr = strList.at(k);
        int length = oriStr.length();
        for (int i = 0; i < length; ++i)
        {
            if (oriStr[i] == ',')
            {
                tarStr.append(oriStr[i]);
                commaFlag = true;
            }
            else if (oriStr[i] == '\"')
            {
                tarStr.append("\"\"\"\"");
            }
            else
            {
                tarStr.append(oriStr[i]);
            }
        }
        if (commaFlag)
        {
            tarStr.push_front('\"');
            tarStr.push_back('\"');
        }
        if (k != lastStrIndex)
        {
            tarStr.append(',');
        }

        lineStr.append(tarStr);
    }

    return qMove(lineStr);
}

vector<int> GetAllCompCols3(const QString& sCompFullName, const QStringList& colNames)
{
    vector<int> vCols;

    QString strCol[3];
    strCol[0] = sCompFullName + ".frame_b.r_0[1]";
    strCol[1] = sCompFullName + ".frame_b.r_0[2]";
    strCol[2] = sCompFullName + ".frame_b.r_0[3]";

    for (int j = 0; j < 3; j++)
    {
        QString sVarName = strCol[j];
        for (int i = 0; i < colNames.size(); i++)
        {
            if (sVarName == colNames[i])
            {
                vCols.push_back(i);
                break;
            }
        }
    }

    return vCols;
}

vector<int> GetAllCompCols(const QString& sCompFullName, const QStringList& colNames)
{
    vector<int> vCols;

    QString strCol[12];
    string SolverName = LibPathsSetDlg::GetSolverName();
    if (SolverName == "openmodelica")
    {
        strCol[3] = sCompFullName + ".frame_a.r_0[1]";
        strCol[7] = sCompFullName + ".frame_a.r_0[2]";
        strCol[11] = sCompFullName + ".frame_a.r_0[3]";
        strCol[0] = sCompFullName + ".frame_a.R.T[1,1]";
        strCol[1] = sCompFullName + ".frame_a.R.T[1,2]";
        strCol[2] = sCompFullName + ".frame_a.R.T[1,3]";
        strCol[4] = sCompFullName + ".frame_a.R.T[2,1]";
        strCol[5] = sCompFullName + ".frame_a.R.T[2,2]";
        strCol[6] = sCompFullName + ".frame_a.R.T[2,3]";
        strCol[8] = sCompFullName + ".frame_a.R.T[3,1]";
        strCol[9] = sCompFullName + ".frame_a.R.T[3,2]";
        strCol[10] = sCompFullName + ".frame_a.R.T[3,3]";
    }
    else if (SolverName == "MWorks")
    {
        strCol[3] = sCompFullName + ".frame_a.r_0[1]";
        strCol[7] = sCompFullName + ".frame_a.r_0[2]";
        strCol[11] = sCompFullName + ".frame_a.r_0[3]";
        strCol[0] = sCompFullName + ".frame_a.R.T[1, 1]";
        strCol[1] = sCompFullName + ".frame_a.R.T[1, 2]";
        strCol[2] = sCompFullName + ".frame_a.R.T[1, 3]";
        strCol[4] = sCompFullName + ".frame_a.R.T[2, 1]";
        strCol[5] = sCompFullName + ".frame_a.R.T[2, 2]";
        strCol[6] = sCompFullName + ".frame_a.R.T[2, 3]";
        strCol[8] = sCompFullName + ".frame_a.R.T[3, 1]";
        strCol[9] = sCompFullName + ".frame_a.R.T[3, 2]";
        strCol[10] = sCompFullName + ".frame_a.R.T[3, 3]";
    }
    
    for (int j = 0; j < 12; j++)
    {
        QString sVarName = strCol[j];
        for (int i = 0; i < colNames.size(); i++)
        {
            if (sVarName == colNames[i])
            {
                vCols.push_back(i);
                break;
            }
        }
    }

    return vCols;
}

vector<double> GetPlacementValues(const QStringList& colValues, const vector<int>& vCols)
{
    vector<double> v12;

    for (int i = 0; i < vCols.size(); i++)
    {
        QString sVal = colValues.at(vCols[i]);
        v12.push_back(sVal.toDouble());
    }

    return v12;
}

int GetComponentConnectType(DocumentModel* pMdl,const QString& sColName, QString& sCompFullName)
{
    int nType; //1-frame_a.r_0, 2-flange_a.phi,3-flange_a.s

    //int pos = sColName.indexOf(".r_0[1]");
    QString sRight = ".r_0[1]";
    if (sColName.right(sRight.length()) == sRight)
    {
        sCompFullName = sColName.left(sColName.length() - sRight.length());
        int pos = sCompFullName.lastIndexOf(".");
        QString sConnectorName = sCompFullName.right(sCompFullName.length() - pos - 1);
        sCompFullName = sCompFullName.left(pos);

        Component* pCom = pMdl->GetComponent()->GetChild(sCompFullName.toStdString().c_str());
        if (!pCom)
            return 0;
        Connector* pConn = ((DocumentComponent*)pCom->GetDocument())->GetConnector(sConnectorName.toStdString().c_str());
        if (pConn && pConn->GetConnType() == "Modelica.Mechanics.MultiBody.Interfaces.Frame_a")
        {
            return 1;
        }
        else {
            return 0;
        }
    }

    sRight = ".phi";
    if (sColName.right(sRight.length()) == sRight)
    {
        sCompFullName = sColName.left(sColName.length() - sRight.length());
        int pos = sCompFullName.lastIndexOf(".");
        QString sConnectorName = sCompFullName.right(sCompFullName.length() - pos - 1);
        sCompFullName = sCompFullName.left(pos);

        Component* pCom = pMdl->GetComponent()->GetChild(sCompFullName.toStdString().c_str());
        if (!pCom)
            return 0; 
        Connector* pConn = ((DocumentComponent*)pCom->GetDocument())->GetConnector(sConnectorName.toStdString().c_str());
        if (pConn && (pConn->GetConnType() == "Modelica.Mechanics.Rotational.Interfaces.Flange_a" \
               || pConn->GetConnType() == "Modelica.Mechanics.Rotational.Interfaces.Flange_b"))
        {
            return 2;
        }
        else {
            return 0;
        }
    }

    sRight = ".s";
    if (sColName.right(sRight.length()) == sRight)
    {
        sCompFullName = sColName.left(sColName.length() - sRight.length());
        int pos = sCompFullName.lastIndexOf(".");
        QString sConnectorName = sCompFullName.right(sCompFullName.length() - pos - 1);
        sCompFullName = sCompFullName.left(pos);

        Component* pCom = pMdl->GetComponent()->GetChild(sCompFullName.toStdString().c_str());
        if (!pCom)
            return 0; 
        Connector* pConn = ((DocumentComponent*)pCom->GetDocument())->GetConnector(sConnectorName.toStdString().c_str());
        if (pConn && (pConn->GetConnType() == "Modelica.Mechanics.Translational.Interfaces.Flange_a" \
            || pConn->GetConnType() == "Modelica.Mechanics.Translational.Interfaces.Flange_b"))
        {
            return 3;
        }
        else {
            return 0;
        }
    }
    return 5;
}

//读取全部多体组件的位姿数据
bool FileParse::GetAllMultibodyPlacement(DocumentModel* pMdl,const QString& fileName, vector<double>& timeSeries, map<string, vector<gp_Trsf>>& mapCompPlacement,\
    map<string, vector<double>>& mapCompRotation,map<string, vector<double>>& mapCompTranslation, map<string, vector<gp_Pnt>>& mapFramebPos)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return false;
    }

    QTextStream inStream(&file);
    QString lineStr;
    for (; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }
        else {
            break;
        }
    }

    //从所有的列名分离出time，和各个组件名
    QStringList colNames = splitCSVLine(lineStr);
    int timeCol = 0; //时间列
    map<string,vector<int>> mapCompnameCols; //【组件全名--位姿所在列表】
    map<string, vector<int>> mapCompnameFrameBPosCols;
    map<string, int> mapCompnameRotateCol;
    map<string, int> mapCompnameTransCol;
    for (int i = 0; i < colNames.size(); i++)
    {
        QString sColName = colNames[i];
        if (sColName == "time" || sColName == "Time")
        {
            timeCol = i;
        }
        else
        {
            //int posDot = sColName.indexOf(".frame_a.r_0"); //刚体
            QString sCompFullName;// = sColName.left(posDot);
            int nType = GetComponentConnectType(pMdl,sColName, sCompFullName);
            if (nType == 1) //posDot > 0)
            {
                //是多体组件的位姿变量
                //是否已经搜索
                if (mapCompnameCols.find(sCompFullName.toStdString()) == mapCompnameCols.end())
                {
                    vector<int> vCols = GetAllCompCols(sCompFullName,colNames);
                    if (vCols.size() == 12)
                    {//是的，位姿有r_0[]3个和R.T[3,3]的9个，一个12列
                        mapCompnameCols.insert(make_pair(sCompFullName.toStdString(), vCols));
                    }
                }
                //Frame_b的位置列
                if (mapCompnameFrameBPosCols.find(sCompFullName.toStdString()) == mapCompnameFrameBPosCols.end())
                {
                    vector<int> vCols = GetAllCompCols3(sCompFullName, colNames);
                    if (vCols.size() == 3)
                    {//是的，frame_b的位置有r_0[]3个
                        mapCompnameFrameBPosCols.insert(make_pair(sCompFullName.toStdString(), vCols));
                    }
                }
            }
            //posDot = sColName.indexOf(".frame_a.phi"); //旋转
            else if (nType == 2) //posDot > 0)
            {
                mapCompnameRotateCol.insert(make_pair(sCompFullName.toStdString(), i));
            }
            //posDot = sColName.indexOf(".frame_a.s"); //平移
            else if (nType == 3)//if (posDot > 0)
            {
                mapCompnameTransCol.insert(make_pair(sCompFullName.toStdString(), i));
            }
            else{
                ;//暂不去管他
            }
        }
    }

    map<string, vector<int>>::iterator iter = mapCompnameCols.begin();
    vector<map<string, vector<int>>::iterator> vecitrs_CompnameCols;
    for (; iter != mapCompnameCols.end(); ++iter)
    {
        vecitrs_CompnameCols.push_back(iter);
    }

    //正式读时间序列和可视化组件数据
    for (; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }

        QStringList colValues = splitCSVLine(lineStr);
        //时间序列
        vector<int> vTime;
        vTime.push_back(timeCol);
        vector<double> vTimeVals = GetPlacementValues(colValues, vTime);
        assert(vTimeVals.size() == 1);
        timeSeries.push_back(vTimeVals[0]);

        ////组件的位姿
        //double start1 = omp_get_wtime();//获取起始时间
        //auto itr = mapCompnameCols.begin();
        //for (; itr != mapCompnameCols.end(); ++itr)
        //{
        //    string sCompName = itr->first;
        //    vector<int> vCols = itr->second;

        //    vector<double> v12 = GetPlacementValues(colValues, vCols);
        //    gp_Trsf trsf = MakeTransFromV12(v12);
        //    //把这个变换矩阵加到map
        //    auto itr1 = mapCompPlacement.find(sCompName);
        //    if (itr1 == mapCompPlacement.end())
        //    {
        //        //如果没有
        //        vector<gp_Trsf> vTrsfs;
        //        vTrsfs.push_back(trsf);
        //        mapCompPlacement.insert(make_pair(sCompName, vTrsfs));
        //    }
        //    else {
        //        //已经有了
        //        vector<gp_Trsf> vTrsfs = itr1->second;
        //        vTrsfs.push_back(trsf);
        //        itr1->second = vTrsfs;
        //    }
        //}
        //double end1 = omp_get_wtime();
        //double runtime1 = end1 - start1;
        //int testbreak = 0;

        //组件的位姿-并行for写法(效率大约提升两倍)
        double start2 = omp_get_wtime();//获取起始时间
        omp_set_num_threads(4);
#pragma omp parallel for
        for (int i = 0; i < vecitrs_CompnameCols.size(); ++i)
        {
            string sCompName = vecitrs_CompnameCols[i]->first;
            vector<int> vCols = vecitrs_CompnameCols[i]->second;

            vector<double> v12 = GetPlacementValues(colValues, vCols);
            gp_Trsf trsf = MakeTransFromV12(v12);
            //把这个变换矩阵加到map
            auto itr1 = mapCompPlacement.find(sCompName);
            if (itr1 == mapCompPlacement.end())
            {
                //如果没有
                vector<gp_Trsf> vTrsfs;
                vTrsfs.push_back(trsf);
                mapCompPlacement.insert(make_pair(sCompName, vTrsfs));
            }
            else {
                //已经有了
                vector<gp_Trsf> vTrsfs = itr1->second;
                vTrsfs.push_back(trsf);
                itr1->second = vTrsfs;
            }
        }
        double end2 = omp_get_wtime();
        double runtime2 = end2 - start2;
        int testbreak2 = 0;

        //组件frame_b的位置
        auto itr_b = mapCompnameFrameBPosCols.begin();
        for (; itr_b != mapCompnameFrameBPosCols.end(); ++itr_b)
        {
            string sCompName = itr_b->first;
            vector<int> vCols = itr_b->second;

            vector<double> v3 = GetPlacementValues(colValues, vCols);
            gp_Pnt pnt(v3[0], v3[1], v3[2]);
            //把这个变换矩阵加到map
            auto itr1 = mapFramebPos.find(sCompName);
            if (itr1 == mapFramebPos.end())
            {
                //如果没有
                vector<gp_Pnt> vPnts;
                vPnts.push_back(pnt);
                mapFramebPos.insert(make_pair(sCompName, vPnts));
            }
            else {
                //已经有了
                vector<gp_Pnt> vPnts = itr1->second;
                vPnts.push_back(pnt);
                itr1->second = vPnts;
            }
        }

        //旋转组件的数据，转角
        auto itr1 = mapCompnameRotateCol.begin();
        for (; itr1 != mapCompnameRotateCol.end(); ++itr1)
        {
            string sCompName = itr1->first;
            int nCol = itr1->second;
            vector<int> vCols;
            vCols.push_back(nCol);

            vector<double> v1 = GetPlacementValues(colValues, vCols);
            auto itr2 = mapCompRotation.find(sCompName);
            if (itr2 == mapCompRotation.end())
            {
                //如果没有
                vector<double> val;
                val.push_back(v1[0]);
                mapCompRotation.insert(make_pair(sCompName, val));
            }
            else {
                //已经有了
                vector<double> vals = itr2->second;
                vals.push_back(v1[0]);
                itr2->second = vals;
            }
        }

        //平移组件的数据，位移
        auto itr3 = mapCompnameTransCol.begin();
        for (; itr3 != mapCompnameTransCol.end(); ++itr3)
        {
            string sCompName = itr3->first;
            int nCol = itr3->second;
            vector<int> vCols;
            vCols.push_back(nCol);

            vector<double> v1 = GetPlacementValues(colValues, vCols);
            auto itr2 = mapCompTranslation.find(sCompName);
            if (itr2 == mapCompTranslation.end())
            {
                //如果没有
                vector<double> val;
                val.push_back(v1[0]);
                mapCompTranslation.insert(make_pair(sCompName, val));
            }
            else {
                //已经有了
                vector<double> vals = itr2->second;
                vals.push_back(v1[0]);
                itr2->second = vals;
            }
        }
    }
    file.close();

    return true;
}

//获得某一列的数据
//认为第一非空白行是列名排列！！！
QStringList FileParse::GetDataSeries(const QString& fileName, const char* sColName)
{
    QStringList strLstRtn;

    //先得到列名对应的第i列的序号
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return strLstRtn;
    }

    QTextStream inStream(&file);
    QString lineStr;
    for ( ; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }
        else {
            break;
        }
    }

    QStringList colNames = splitCSVLine(lineStr);
    int iCol = 0; //从0开始
    for (int i = 0; i < colNames.size(); i++)
    {
        if (colNames[i] == sColName)
        {
            iCol = i;
            break;
        }
    }

    //继续读
    //得到第i列的数据
    for (; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }

        QStringList colValues = splitCSVLine(lineStr);
        QString sColVal = colValues.at(iCol);
        strLstRtn.append(sColVal);
    }
    file.close();

    return strLstRtn;
}

//获取所有变量名称
QStringList FileParse::GetAllVariablesNames(const QString& fileName)
{
    QStringList strLstNames;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return strLstNames;
    }

    //获得第一行非空白行
    QTextStream inStream(&file);
    QString lineStr;
    for (; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }
        else {
            break;
        }
    }

    QStringList colNames = splitCSVLine(lineStr);
    return colNames;

}

//根据仿真时间获取一行仿真数据
//最好别用这个函数，读到后面时间的时候，需要读几乎整个文件，效率太低，使用文件预读比较好
QStringList FileParse::GetDataByTime(const QString& fileName, double time)
{
    QStringList strLstNames;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return strLstNames;
    }

    //获得第一行非空白行
    QTextStream inStream(&file);
    QString lineStr;
    for (; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }
        else {
            QStringList colDatas = splitCSVLine(lineStr);
            string strtime = colDatas.at(0).toStdString();
            if (strtime == "time" || strtime == "Time")
            {
                continue;
            }
            double dtime = std::stod(strtime.c_str());
            if (dtime == time)
            {
                return colDatas;
            }
        }
    }
    
    //照理说不会执行到这一步
    return strLstNames;
}

//在仿真文件中均匀取nums行数据
vector<QStringList> FileParse::GetDataByNums(const QString& fileName, int nums)
{
    //先得到该文件的行数
    QStringList time = GetDataSeries(fileName, "time");
    QStringList Time = GetDataSeries(fileName, "Time");

    int length = max(time.length(), Time.length());

    int interval = length / nums;  //每intervlas行取一行
    vector<QStringList> AllRows;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return AllRows;
    }

    //获得第一行非空白行
    int index = 0; // 行的索引
    QTextStream inStream(&file);
    QString lineStr;
    for (; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }
        else {
            QStringList colDatas = splitCSVLine(lineStr);
            string strtime = colDatas.at(0).toStdString();
            if (strtime == "time" || strtime == "Time")
            {
                continue;
            }
            
            if (index % interval == 0)
            {
                AllRows.push_back(colDatas);
            }
            index++;
        }
    }

    //如果最后数目取多了，要减去
    while (AllRows.size() > nums)
    {
        AllRows.pop_back();
    }
    return AllRows;
}


//预读用于数据通信的DDS发送数据
void FileParse::GetDDSDatasFromCols(const QString& fileName, vector<int>& sendVarsCols, map<double, vector<string>>& timeToDatas)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream inStream(&file);
    QString lineStr;
    for (; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }
        else {
            break;
        }
    }
    
    int timeCol = 0;
    QStringList colNames = splitCSVLine(lineStr);
    for (int i = 0; i < colNames.size(); i++)
    {
        QString sColName = colNames[i];
        if (sColName == "time" || sColName == "Time")
        {
            timeCol = i;
        }
    }

    //正式读时间序列和可视化组件数据
    for (; !inStream.atEnd(); )
    {
        lineStr = inStream.readLine();
        if (lineStr.isEmpty())
        {
            continue;
        }

        QStringList colValues = splitCSVLine(lineStr);
        //时间序列
        vector<int> vTime;
        vTime.push_back(timeCol);
        vector<double> vTimeVals = GetPlacementValues(colValues, vTime);
        assert(vTimeVals.size() == 1);
        double nowtime = vTimeVals[0];

        //填充发送数据
        for (int i = 0; i < sendVarsCols.size(); i++)
        {
            QString sVal = colValues.at(sendVarsCols[i]);
            timeToDatas[nowtime].push_back(sVal.toStdString());
        }
    }

    return;
}