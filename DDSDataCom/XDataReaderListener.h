// 数据读者监听者实现
#ifndef DATAREADER_LISTENER_IMPL
#define DATAREADER_LISTENER_IMPL

#include <dds/DdsDcpsSubscriptionExtC.h>
#include <dds/DCPS/LocalObject.h>
#include "DDSData.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

class XDataReaderListener
    : public virtual OpenDDS::DCPS::LocalObject<OpenDDS::DCPS::DataReaderListener>
{
public:
    XDataReaderListener();

    virtual ~XDataReaderListener(void);

    virtual void on_requested_deadline_missed(
        DDS::DataReader_ptr reader,
        const DDS::RequestedDeadlineMissedStatus& status);

    virtual void on_requested_incompatible_qos(
        DDS::DataReader_ptr reader,
        const DDS::RequestedIncompatibleQosStatus& status);

    virtual void on_liveliness_changed(
        DDS::DataReader_ptr reader,
        const DDS::LivelinessChangedStatus& status);

    virtual void on_subscription_matched(
        DDS::DataReader_ptr reader,
        const DDS::SubscriptionMatchedStatus& status);

    virtual void on_sample_rejected(
        DDS::DataReader_ptr reader,
        const DDS::SampleRejectedStatus& status);

    virtual void on_data_available(
        DDS::DataReader_ptr reader);

    virtual void on_sample_lost(
        DDS::DataReader_ptr reader,
        const DDS::SampleLostStatus& status);

    virtual void on_subscription_disconnected(
        DDS::DataReader_ptr reader,
        const ::OpenDDS::DCPS::SubscriptionDisconnectedStatus& status);

    virtual void on_subscription_reconnected(
        DDS::DataReader_ptr reader,
        const ::OpenDDS::DCPS::SubscriptionReconnectedStatus& status);

    virtual void on_subscription_lost(
        DDS::DataReader_ptr reader,
        const ::OpenDDS::DCPS::SubscriptionLostStatus& status);

    virtual void on_budget_exceeded(
        DDS::DataReader_ptr reader,
        const ::OpenDDS::DCPS::BudgetExceededStatus& status);

    long num_reads() const {
        return num_reads_;
    }

    void SetStoredData(StoredData* ptr)
    {
        m_StoredData = ptr;
    }

    StoredData* GetStoredData()
    {
        return m_StoredData;
    }

    void SethomeID(string homeid)
    {
        homeID = homeid;
    }

    void SetRecvMsgId(string msgids)
    {
        msgIds = msgids;
    }

private:

    DDS::DataReader_var reader_;
    long                  num_reads_;

    set<string> parseStrBySeparate(string str, char separate);     // 自定义的字符串拆分函数
    void OutputDebugPrintf(const char* strOutputString, ...);      // 自定义的输出字符串转换函数
    StoredData* m_StoredData;                                      // 存储接收到的数据
    string homeID;                                                 // 本机id，用于过滤接收到的数据
    string msgIds;                                                 // 接收消息id，用于过滤接收到的数据（可能有多个）
};

#endif /* DATAREADER_LISTENER_IMPL  */
