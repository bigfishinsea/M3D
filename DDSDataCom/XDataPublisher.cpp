#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/transport/tcp/TcpInst.h>
#include "dds/DCPS/StaticIncludes.h"

#include <ace/streams.h>

#include "XDataTypeSupportImpl.h"
#include "DDSData.h"
#include <ace/Init_ACE.h>

#include "atlbase.h"
#include "atlstr.h"


using namespace XDataIdlModule;
using namespace std;

DDS::DomainParticipantFactory_var dpf;
DDS::DomainParticipant_var participant;
XDataTopic1DataWriter_var XDataTopic1_dw;
string LastSendedMsg;                                                //上一条发送的消息

string XData::VectoStr(vector<string> vec_Data) {
    string ans;
    for (int i = 0; i < vec_Data.size(); i++)
    {
        ans += vec_Data[i];
        ans += ";";
    }
    return ans;
}

vector<string> XData::StrtoVec(string data) {
    vector<string> ans;
    string nowstr = "";
    for (int i = 0; i < data.length(); i++)
    {
        if (data.at(i) != ';')
        {
            nowstr.push_back(data.at(i));
        }
        else
        {
            ans.push_back(nowstr);
            nowstr = "";
        }
    }
    if (nowstr != "")
    {
        ans.push_back(nowstr);
    }
    return ans;
}


int GenXDataMembers(int discoveryMehodID) {
    //初始化参与者
    ACE::init();
    //int argc = 3;
    //ACE_TCHAR* argv[3];
    //
    //char buff[250];
    //_getcwd(buff, 250);
    //std::string current_working_directory(buff);

    //string str = "D:\\opendds\\XData-x64\\publisher.exe";
    //argv[0] = new ACE_TCHAR(str.length() + 1);
    //strcpy(argv[0], str.c_str());
    //string str2 = "-DCPSConfigFile";
    //argv[1] = new ACE_TCHAR(str2.length() + 1);
    //strcpy(argv[1], str2.c_str());
    //string str3 = "config_Pub.ini";
    //argv[2] = new ACE_TCHAR(str3.length() + 1);
    //strcpy(argv[2], str3.c_str());

    //argv[1] = "-DCPSConfigFile";
    //argv[2] = "config_Pub.ini";

    const int nNUMBER_OF_ARGUEMENTS = 2;
    int argc = nNUMBER_OF_ARGUEMENTS;
    ACE_TCHAR* argv[nNUMBER_OF_ARGUEMENTS] = { ACE_TEXT("-DCPSConfigFile"), ACE_TEXT("config_Pub.ini") };
    
    if (discoveryMehodID == 1)  // DCPSInfoRepo的集中式发现
    {
        argv[1] = ACE_TEXT("config_DCPSInfoRepo_Pub.ini");
    }
    
    //1. 初始化参与者
    //DDS::DomainParticipantFactory_var dpf =
    dpf = 
        TheParticipantFactoryWithArgs(argc, argv);

    //DDS::DomainParticipant_var participant =
    participant =
        dpf->create_participant(111,
            PARTICIPANT_QOS_DEFAULT,
            DDS::DomainParticipantListener::_nil(),
            ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (CORBA::is_nil(participant.in())) {
        cerr << "create_participant failed." << endl;
        return 1;
    }

    // 2. 注册数据类型
    //这里是Topic而不是Topics，意义不同，体现在idl文件里。
    XDataTopic1TypeSupportImpl* servant = new  XDataTopic1TypeSupportImpl();//这句是要根据XXXXTypeSupportImpl中的前缀与idl文件中的Topic key名对应，在这里即"DemoTopic1"
    OpenDDS::DCPS::LocalObject_var safe_servant = servant;

    if (DDS::RETCODE_OK != servant->register_type(participant.in(), "")) {
        cerr << "register_type failed." << endl;
        return 2;
        exit(1);
    }

    // 3. 创建主题
    CORBA::String_var type_name = servant->get_type_name();

    DDS::TopicQos topic_qos;
    participant->get_default_topic_qos(topic_qos);
    DDS::Topic_var topic =
        participant->create_topic("Movie Discussion List",
            type_name.in(),
            topic_qos,
            DDS::TopicListener::_nil(),
            ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (CORBA::is_nil(topic.in())) {
        cerr << "create_topic failed." << endl;
        return 3;
        exit(1);
    }

    // 4. 创建公布者
    DDS::Publisher_var pub =
        participant->create_publisher(PUBLISHER_QOS_DEFAULT,
            DDS::PublisherListener::_nil(),
            ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (CORBA::is_nil(pub.in())) {
        cerr << "create_publisher failed." << endl;
        return 4;
        exit(1);
    }

    // 5. 创建数据写者
    DDS::DataWriterQos dw_qos;
    pub->get_default_datawriter_qos(dw_qos);
    DDS::DataWriter_var dw =
        pub->create_datawriter(topic.in(),
            dw_qos,
            DDS::DataWriterListener::_nil(),
            ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
    if (CORBA::is_nil(dw.in())) {
        cerr << "create_datawriter failed." << endl;
        return 5;
        exit(1);
    }
    XDataTopic1_dw
    //XDataTopic1DataWriter_var message_dw //这句是要根据XXXXDataWriter_var，XXXXDataWriter中的前缀与idl文件中的Topic key名对应，在这里即"DemoTopic1"
        = XDataTopic1DataWriter::_narrow(dw.in());


    // Get default Publisher QoS from a DomainParticipant:
    DDS::PublisherQos pub_qos;
    DDS::ReturnCode_t ret;
    ret = participant->get_default_publisher_qos(pub_qos);

    if (DDS::RETCODE_OK != ret) {
        std::cerr << "Could not get default publisher QoS" << std::endl;
        return 6;
    }

    // Get default Subscriber QoS from a DomainParticipant:
    DDS::SubscriberQos sub_qos;
    ret = participant->get_default_subscriber_qos(sub_qos);
    if (DDS::RETCODE_OK != ret) {
        std::cerr << "Could not get default subscriber QoS" << std::endl;
        return 7;
    }

    // Get default Topic QoS from a DomainParticipant:
    DDS::TopicQos topic_qos2;
    ret = participant->get_default_topic_qos(topic_qos2);
    if (DDS::RETCODE_OK != ret) {
        std::cerr << "Could not get default topic QoS" << std::endl;
        return 8;
    }

    // Get default DomainParticipant QoS from a DomainParticipantFactory:
    DDS::DomainParticipantQos dp_qos;
    ret = dpf->get_default_participant_qos(dp_qos);
    if (DDS::RETCODE_OK != ret) {
        std::cerr << "Could not get default participant QoS" << std::endl;
        return 9;
    }

    // Get default DataWriter QoS from a Publisher:
    DDS::DataWriterQos dw_qos2;
    ret = pub->get_default_datawriter_qos(dw_qos2);
    if (DDS::RETCODE_OK != ret) {
        std::cerr << "Could not get default data writer QoS" << std::endl;
        return 10;
    }
    
    return 0;//返回0即为初始化成功
}

void OutputDebugPrintf(const char* strOutputString, ...)
{
    char strBuffer[4096] = { 0 };
    va_list vlArgs;
    va_start(vlArgs, strOutputString);
    _vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
    //vsprintf(strBuffer, strOutputString, vlArgs);
    va_end(vlArgs);
    OutputDebugString(CA2W(strBuffer));
}


int SendXData(string messageId, string destinationId, XData data) {
    // 6. 公布数据
    XDataTopic1 message;//这句是要根据idl文件中的Topic key名对应，在这里即"DemoTopic1"
    message.id = CORBA::string_dup(messageId.c_str());//消息id
    ::DDS::InstanceHandle_t handle = XDataTopic1_dw->register_instance(message);
    //   message.counter = 0;
    message.destination = CORBA::string_dup(destinationId.c_str());
    message.tTimer = data.time;
    message.vData = CORBA::string_dup(data.XData::VectoStr(data.vec_Data).c_str());
    cout << "RETT-message!!!(id): " << message.id << endl;
    cout << "message-destination: " << message.destination << endl;
    cout << "    message-time:    " << message.tTimer << endl;
    cout << "    message-vData:   " << message.vData << endl;
    
    //让输出可以显示
    TAO::String_Manager tempid = message.id;
    string strid = tempid._retn();
    string out1 = "RETT-message!!!(id): " + strid + "\n";
    
    TAO::String_Manager tempdes= message.destination;
    string strdes = tempdes._retn();
    string out2 = "message-destination: " + strdes + "\n";
    
    string strtime = std::to_string(message.tTimer);
    string out3 = "    message-time:    " + strtime + "\n";
    
    TAO::String_Manager tempdata = message.vData;
    string strdata = tempdata._retn();
    string out4 = "    message-vData:   " + strdata + "\n";
  
    OutputDebugPrintf(out1.c_str());
    OutputDebugPrintf(out2.c_str());
    OutputDebugPrintf(out3.c_str());
    OutputDebugPrintf(out4.c_str());

    XDataTopic1_dw->write(message, handle); 

    LastSendedMsg = out1 + out2 + out3 + out4;
    
    return 0;
}

string GetSendedMsg() {
    return LastSendedMsg;
}

int ClearXDataMembers() {
    // 7. 实体清理
    //先检查有没有被清理
    //if (participant != nullptr)
    //{
    //    participant->delete_contained_entities();
    //    dpf->delete_participant(participant);
    //    TheServiceParticipant->shutdown();
    //    dpf = nullptr;;
    //    participant = nullptr;
    //    XDataTopic1_dw = nullptr;
    //    ACE::fini();
    //    return 0;
    //}
    //return 0;

    if (participant != nullptr)
    {
        participant->delete_contained_entities();
    }
    if (dpf != nullptr && participant != nullptr)
    {
        dpf->delete_participant(participant);
    }
    TheServiceParticipant->shutdown();
    dpf = nullptr;;
    participant = nullptr;
    XDataTopic1_dw = nullptr;
    ACE::fini();
    return 0;
}


int ACE_TMAIN_XDataPub(int argc, ACE_TCHAR* argv[]) {
    try {
        // 初始化参与者
        argv[1] = "-DCPSConfigFile";
        argv[2] = "config_Pub.ini";
        argc = 3;
#if 1   //0. 文件 //将参数从文件读出，然后作为DDS传输的主题数据。
        FILE* fp;
        //char c[] = "The counter is: 5";
        char c[] = "2";
        char buffer[50] = { 0 };
        //    char* buffer = { 0 };
        //    int bufsize = 0;
        long _counter = 0; //用来交给6.公布数据的 message.counter

        char fname[80] = "F:\\J18-ZaiJian\\AAA.txt";
        int charCount = 0;/** 保存文件的字符个数 **/
        /* 打开文件用于读 */
        if ((fp = fopen(fname, "r")) == NULL)
        {
            printf("Open file failed!!\n");
            exit(1);
        }
        while (fgetc(fp) != EOF) /** 统计字符个数 **/
            charCount++;
        /* 查找文件的开头 */
        fseek(fp, 0, SEEK_SET);

        /* 读取并显示数据 */
     //  fread(buffer, strlen(buffer) + 1, 1, fp);
        fread(buffer, charCount + 1, 1, fp);
        cout << buffer << endl;
        fclose(fp);

        /* 字符串转换long */
        _counter = strtol(buffer, NULL, 10);
        cout << "Begin!!! _counter: " << _counter << endl;
#endif  
        // 1. 初始化参与者
        DDS::DomainParticipantFactory_var dpf =
            TheParticipantFactoryWithArgs(argc, argv);

        DDS::DomainParticipant_var participant =
            dpf->create_participant(111,
                PARTICIPANT_QOS_DEFAULT,
                DDS::DomainParticipantListener::_nil(),
                ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(participant.in())) {
            cerr << "create_participant failed." << endl;
            return 1;
        }

        // 2. 注册数据类型
        //这里是Topic而不是Topics，意义不同，体现在idl文件里。
        XDataTopic1TypeSupportImpl* servant = new  XDataTopic1TypeSupportImpl();//这句是要根据XXXXTypeSupportImpl中的前缀与idl文件中的Topic key名对应，在这里即"DemoTopic1"
        OpenDDS::DCPS::LocalObject_var safe_servant = servant;

        if (DDS::RETCODE_OK != servant->register_type(participant.in(), "")) {
            cerr << "register_type failed." << endl;
            exit(1);
        }

        // 3. 创建主题
        CORBA::String_var type_name = servant->get_type_name();

        DDS::TopicQos topic_qos;
        participant->get_default_topic_qos(topic_qos);
        DDS::Topic_var topic =
            participant->create_topic("Movie Discussion List",
                type_name.in(),
                topic_qos,
                DDS::TopicListener::_nil(),
                ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(topic.in())) {
            cerr << "create_topic failed." << endl;
            exit(1);
        }

        // 4. 创建公布者
        DDS::Publisher_var pub =
            participant->create_publisher(PUBLISHER_QOS_DEFAULT,
                DDS::PublisherListener::_nil(),
                ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(pub.in())) {
            cerr << "create_publisher failed." << endl;
            exit(1);
        }

        // 5. 创建数据写者
        DDS::DataWriterQos dw_qos;
        pub->get_default_datawriter_qos(dw_qos);
        DDS::DataWriter_var dw =
            pub->create_datawriter(topic.in(),
                dw_qos,
                DDS::DataWriterListener::_nil(),
                ::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
        if (CORBA::is_nil(dw.in())) {
            cerr << "create_datawriter failed." << endl;
            exit(1);
        }
        XDataTopic1DataWriter_var message_dw //这句是要根据XXXXDataWriter_var，XXXXDataWriter中的前缀与idl文件中的Topic key名对应，在这里即"DemoTopic1"
            = XDataTopic1DataWriter::_narrow(dw.in());

        //
        // Get default Publisher QoS from a DomainParticipant:
        DDS::PublisherQos pub_qos;
        DDS::ReturnCode_t ret;
        ret = participant->get_default_publisher_qos(pub_qos);

        if (DDS::RETCODE_OK != ret) {
            std::cerr << "Could not get default publisher QoS" << std::endl;
        }

        // Get default Subscriber QoS from a DomainParticipant:
        DDS::SubscriberQos sub_qos;
        ret = participant->get_default_subscriber_qos(sub_qos);
        if (DDS::RETCODE_OK != ret) {
            std::cerr << "Could not get default subscriber QoS" << std::endl;
        }

        // Get default Topic QoS from a DomainParticipant:
        DDS::TopicQos topic_qos2;
        ret = participant->get_default_topic_qos(topic_qos2);
        if (DDS::RETCODE_OK != ret) {
            std::cerr << "Could not get default topic QoS" << std::endl;
        }

        // Get default DomainParticipant QoS from a DomainParticipantFactory:
        DDS::DomainParticipantQos dp_qos;
        ret = dpf->get_default_participant_qos(dp_qos);
        if (DDS::RETCODE_OK != ret) {
            std::cerr << "Could not get default participant QoS" << std::endl;
        }

        // Get default DataWriter QoS from a Publisher:
        DDS::DataWriterQos dw_qos2;
        ret = pub->get_default_datawriter_qos(dw_qos2);
        if (DDS::RETCODE_OK != ret) {
            std::cerr << "Could not get default data writer QoS" << std::endl;
        }


        // 6. 公布数据
        XDataTopic1 message;//这句是要根据idl文件中的Topic key名对应，在这里即"DemoTopic1"
        message.id = "10.16.226.141";
        ::DDS::InstanceHandle_t handle = message_dw->register_instance(message);
        //   message.counter = 0;
#if 1
        message.destination = "10.16.226.141";
        message.tTimer = -2;
        message.vData = "A.alpha  A.beta  A.gama";
        cout << "			RETT-message!!!(id): " << message.id << endl;
#endif

        char tMsg[50] = { 0 };
        while (1)
        {
            message.tTimer++;
            memset(tMsg, 0, 50);
            sprintf(tMsg, "RETT-Msg Destination : %s", message.destination);
            //message.vData = ::TAO::String_Manager(tMsg);
            message_dw->write(message, handle);
            ACE_OS::sleep(1);
            cout << "RETT..." << endl;
            cout << "RETT-halo eyeryone!!!" << message.id << endl;
        }


        // 7. 实体清理
        participant->delete_contained_entities();
        dpf->delete_participant(participant);
        TheServiceParticipant->shutdown();
    }
    catch (CORBA::Exception& e)
    {
        cerr << "PUB: Exception caught in main.cpp:" << endl
            << e << endl;
        exit(1);
    }

    return 0;
}
