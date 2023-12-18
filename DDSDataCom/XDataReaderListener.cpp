// -*- C++ -*-
//
#include "XDataReaderListener.h"
#include "XDataTypeSupportC.h"
#include "XDataTypeSupportImpl.h"
#include <dds/DCPS/Service_Participant.h>
#include <ace/streams.h>

#include "atlbase.h"
#include "atlstr.h"

using namespace XDataIdlModule;
using namespace std;

XDataReaderListener::XDataReaderListener()
    : num_reads_(0)
{
}

XDataReaderListener::~XDataReaderListener()
{
}

set<string> XDataReaderListener::parseStrBySeparate(string str, char separate)
{
    set<string> ans;
    string nowstr = "";
    for (int i = 0; i < str.length(); i++) {
        if (str.at(i) == separate) {
            ans.insert(nowstr);
            nowstr = "";
        }
        else {
            nowstr.push_back(str.at(i));
        }
    }
    if (nowstr != "") {
        ans.insert(nowstr);
    }
    return ans;
}

void XDataReaderListener::on_data_available(DDS::DataReader_ptr reader)
{
    ++num_reads_;
    try {
        XDataTopic1DataReader_var message_dr = XDataTopic1DataReader::_narrow(reader);
        if (CORBA::is_nil(message_dr.in())) {
            cerr << "read: _narrow failed." << endl;
            OutputDebugPrintf("read: _narrow failed.");
            exit(1);
        }

        XDataTopic1 message;
        DDS::SampleInfo si;
        DDS::ReturnCode_t status = message_dr->take_next_sample(message, si);

        if (status == DDS::RETCODE_OK) {
            cout << "       SUB-message-id        = " << message.id << endl;
            cout << "   SUB-message-destination   = " << message.destination << endl;
            cout << "      SUB-message-time:      = " << message.tTimer << endl;
            cout << "      SUB-message-vData      = " << message.vData << endl;

            cout << "SampleInfo.sample_rank = " << si.sample_rank << endl;

            //让输出可以显示
            TAO::String_Manager tempid = message.id;
            string strid = tempid._retn();
            string out1 = "SUB-RETT-message!!!(id): " + strid + "\n";

            TAO::String_Manager tempdes = message.destination;
            string strdes = tempdes._retn();
            set<string> destinations = parseStrBySeparate(strdes, ';');
            string out2 = "SUB-message-destination: " + strdes + "\n";

            string strtime = std::to_string(message.tTimer);
            string out3 = "    SUB-message-time:    " + strtime + "\n";

            TAO::String_Manager tempdata = message.vData;
            string strdata = tempdata._retn();
            string out4 = "    SUB-message-vData:   " + strdata + "\n";

            OutputDebugPrintf(out1.c_str());
            OutputDebugPrintf(out2.c_str());
            OutputDebugPrintf(out3.c_str());
            OutputDebugPrintf(out4.c_str());

            //过滤收到的信息，将其存储在m_StoredData中
            //用本机Id过滤
            if (homeID != "" && !destinations.empty() && !destinations.count(homeID))
            {
                return;
            }
            //用消息Id过滤
            set<string> messageids = parseStrBySeparate(this->msgIds, ';');
            if (strid != "" && !messageids.empty() && !messageids.count(strid))
            {
                return;
            }

            //利用XData的转换函数将字符串类型的数据转换为vector类型的数据
            XData data;
            vector<string> vec_data = data.StrtoVec(strdata);
            
            if (message.tTimer == -1)
            {
                m_StoredData->SetParameterNames(vec_data);
            }
            else
            {
                m_StoredData->AddRowData(vec_data);
                if (m_StoredData->isreceiving == false)
                {
                    //将开关变量置为true
                    m_StoredData->isreceiving = true;
                }
            }
        }
        else if (status == DDS::RETCODE_NO_DATA) {
            cerr << "ERROR: reader received DDS::RETCODE_NO_DATA!" << endl;
            OutputDebugPrintf("ERROR: reader received DDS::RETCODE_NO_DATA!");
        }
        else {
            cerr << "ERROR: read Message: Error: " << status << endl;
            OutputDebugPrintf("ERROR: read Message: Error: ");
        }
    }
    catch (CORBA::Exception& e) {
        cerr << "Exception caught in read:" << endl << e << endl;
        OutputDebugPrintf("Exception caught in read:");
        exit(1);
    }
    return;
    
    
    //++num_reads_;
    ////0. 文件
    //FILE* fp;
    ////char c[] = "The counter is: 5";
    //char c[50] = "2";
    //char buffer[50] = { 0 };
    //long _counter = 0;
    ///* 打开文件用于读写 */
    //if (fp = fopen("F:\\J18-ZaiJian\\BBB.txt", "w+"))
    //    cout << "RETT-Open file success--- ";



    //try {
    //    XDataTopic1DataReader_var message_dr = XDataTopic1DataReader::_narrow(reader);
    //    if (CORBA::is_nil(message_dr.in())) {
    //        cerr << "read: _narrow failed." << endl;
    //        exit(1);
    //    }

    //    XDataTopic1 message;
    //    DDS::SampleInfo si;
    //    DDS::ReturnCode_t status = message_dr->take_next_sample(message, si);

    //    if (status == DDS::RETCODE_OK) {
    //        cout << "PP-Message: id    = " << message.id << endl
    //            << "         PP-DemoTopic1_destination = " << message.destination << endl
    //            << "         PP-DemoTopic1_vData = " << message.vData << endl;

    //        cout << "SampleInfo.sample_rank = " << si.sample_rank << endl;
    //        //_counter = message.counter;
    //    }
    //    else if (status == DDS::RETCODE_NO_DATA) {
    //        cerr << "ERROR: reader received DDS::RETCODE_NO_DATA!" << endl;
    //    }
    //    else {
    //        cerr << "ERROR: read Message: Error: " << status << endl;
    //    }
    //}
    //catch (CORBA::Exception& e) {
    //    cerr << "Exception caught in read:" << endl << e << endl;
    //    exit(1);
    //}

    //ltoa(_counter, c, 10);

    ///* 写入数据到文件 */
    //fwrite(c, strlen(c) + 1, 1, fp);

    ///* 查找文件的开头 */
    //fseek(fp, 0, SEEK_SET);
    ///* 读取并显示数据 */
    //fread(buffer, strlen(c) + 1, 1, fp);
    //cout << buffer << endl;
    //fclose(fp);
}

void XDataReaderListener::OutputDebugPrintf(const char* strOutputString, ...)
{
    char strBuffer[4096] = { 0 };
    va_list vlArgs;
    va_start(vlArgs, strOutputString);
    _vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
    //vsprintf(strBuffer, strOutputString, vlArgs);
    va_end(vlArgs);
    OutputDebugString(CA2W(strBuffer));
}

void XDataReaderListener::on_requested_deadline_missed(
    DDS::DataReader_ptr,
    const DDS::RequestedDeadlineMissedStatus&)
{
    cerr << "DataReaderListener::on_requested_deadline_missed" << endl;
}

void XDataReaderListener::on_requested_incompatible_qos(
    DDS::DataReader_ptr,
    const DDS::RequestedIncompatibleQosStatus&)
{
    cerr << "DataReaderListener::on_requested_incompatible_qos" << endl;
}

void XDataReaderListener::on_liveliness_changed(
    DDS::DataReader_ptr,
    const DDS::LivelinessChangedStatus&)
{
    cerr << "DataReaderListener::on_liveliness_changed" << endl;
}

void XDataReaderListener::on_subscription_matched(
    DDS::DataReader_ptr,
    const DDS::SubscriptionMatchedStatus&)
{
    cerr << "DataReaderListener::on_subscription_matched" << endl;
}

void XDataReaderListener::on_sample_rejected(
    DDS::DataReader_ptr,
    const DDS::SampleRejectedStatus&)
{
    cerr << "DataReaderListener::on_sample_rejected" << endl;
}

void XDataReaderListener::on_sample_lost(
    DDS::DataReader_ptr,
    const DDS::SampleLostStatus&)
{
    cerr << "DataReaderListener::on_sample_lost" << endl;
}

void XDataReaderListener::on_subscription_disconnected(
    DDS::DataReader_ptr,
    const ::OpenDDS::DCPS::SubscriptionDisconnectedStatus&)
{
    cerr << "DataReaderListener::on_subscription_disconnected" << endl;
}

void XDataReaderListener::on_subscription_reconnected(
    DDS::DataReader_ptr,
    const ::OpenDDS::DCPS::SubscriptionReconnectedStatus&)
{
    cerr << "DataReaderListener::on_subscription_reconnected" << endl;
}

void XDataReaderListener::on_subscription_lost(
    DDS::DataReader_ptr,
    const ::OpenDDS::DCPS::SubscriptionLostStatus&)
{
    cerr << "DataReaderListener::on_subscription_lost" << endl;
}

void XDataReaderListener::on_budget_exceeded(
    DDS::DataReader_ptr,
    const ::OpenDDS::DCPS::BudgetExceededStatus&)
{
    cerr << "DataReaderListener::on_budget_exceeded" << endl;
}
