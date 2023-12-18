#include "XDataTypeSupportImpl.h"
#include <dds/DCPS/Service_Participant.h>
#include <dds/DCPS/Marked_Default_Qos.h>
#include <dds/DCPS/PublisherImpl.h>
#include <dds/DCPS/transport/tcp/TcpInst.h>

#include "dds/DCPS/StaticIncludes.h"
#include <ace/streams.h>

#include "XDataReaderListener.h"

#include "DDSData.h"
#include <ace/Init_ACE.h>

DDS::DomainParticipantFactory_var dpf_sub;
DDS::DomainParticipant_var participant_sub;
XDataReaderListener* listener_servant;

using namespace XDataIdlModule;

StoredData::StoredData() {

}

StoredData::~StoredData() {

}

//初始化订阅
int InitSubscribe(StoredData* m_pStoredData, int discoveryMehodID) {
	
	//初始化ACE
	ACE::init();
	const int nNUMBER_OF_ARGUEMENTS = 2;
	int argc = nNUMBER_OF_ARGUEMENTS;
	ACE_TCHAR* argv[nNUMBER_OF_ARGUEMENTS] = { ACE_TEXT("-DCPSConfigFile"), ACE_TEXT("config_Sub.ini") };

	if (discoveryMehodID == 1)  // DCPSInfoRepo的集中式发现
	{
		argv[1] = ACE_TEXT("config_DCPSInfoRepo_Sub.ini");
	}

	// 1. 初始化参与者
	//DDS::DomainParticipantFactory_var dpf =
	dpf_sub = 
		TheParticipantFactoryWithArgs(argc, argv);

	//DDS::DomainParticipant_var participant =
	participant_sub = 
		dpf_sub->create_participant(111,
			PARTICIPANT_QOS_DEFAULT,
			DDS::DomainParticipantListener::_nil(),
			::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
	if (CORBA::is_nil(participant_sub.in())) {
		cerr << "create_participant failed." << endl;
		return 1;
	}

	// 2. 注册数据类型并创建主题
	XDataTopic1TypeSupportImpl* servant = new	XDataTopic1TypeSupportImpl();
	OpenDDS::DCPS::LocalObject_var safe_servant = servant;

	if (DDS::RETCODE_OK != servant->register_type(participant_sub.in(), "")) {
		cerr << "register_type failed." << endl;
		return 2;
		exit(1);
	}

	CORBA::String_var type_name = servant->get_type_name();

	DDS::TopicQos topic_qos;
	participant_sub->get_default_topic_qos(topic_qos);
	DDS::Topic_var topic =
		participant_sub->create_topic("Movie Discussion List",
			type_name.in(),
			topic_qos,
			DDS::TopicListener::_nil(),
			::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
	if (CORBA::is_nil(topic.in())) {
		cerr << "create_topic failed." << endl;
		return 3;
		exit(1);
	}

	// 3. 创建订阅者
	DDS::Subscriber_var sub =
		participant_sub->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
			DDS::SubscriberListener::_nil(),
			::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
	if (CORBA::is_nil(sub.in())) {
		cerr << "Failed to create_subscriber." << endl;
		return 4;
		exit(1);
	}

	// 4. 创建监听者
	//激活监听者
	DDS::DataReaderListener_var listener(new XDataReaderListener);
	//XDataReaderListener* listener_servant =
	listener_servant =
		dynamic_cast<XDataReaderListener*>(listener.in());
	//将StoredData的指针传入
	listener_servant->SetStoredData(m_pStoredData);


	if (CORBA::is_nil(listener.in())) {
		cerr << "listener is nil." << endl;
		return 5;
		exit(1);
	}

	if (!listener_servant) {
		ACE_ERROR_RETURN((LM_ERROR,
			ACE_TEXT("%N:%l main()")
			ACE_TEXT(" ERROR: listener_servant is nil (dynamic_cast failed)!\n")), -1);
		return 6;
	}

	// 5. 创建数据读者
	DDS::DataReaderQos dr_qos;
	sub->get_default_datareader_qos(dr_qos);
	DDS::DataReader_var dr = sub->create_datareader(topic.in(),
		dr_qos,
		listener.in(),
		::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
	if (CORBA::is_nil(dr.in())) {
		cerr << "create_datareader failed." << endl;
		return 7;
		exit(1);
	}

	//
	// Get default Publisher QoS from a DomainParticipant:
	DDS::PublisherQos pub_qos;
	DDS::ReturnCode_t ret;
	ret = participant_sub->get_default_publisher_qos(pub_qos);

	if (DDS::RETCODE_OK != ret) {
		std::cerr << "Could not get default publisher QoS" << std::endl;
		return 8;
	}

	// Get default Subscriber QoS from a DomainParticipant:
	DDS::SubscriberQos sub_qos;
	ret = participant_sub->get_default_subscriber_qos(sub_qos);
	if (DDS::RETCODE_OK != ret) {
		std::cerr << "Could not get default subscriber QoS" << std::endl;
		return 9;
	}

	// Get default Topic QoS from a DomainParticipant:
	DDS::TopicQos topic_qos2;
	ret = participant_sub->get_default_topic_qos(topic_qos2);
	if (DDS::RETCODE_OK != ret) {
		std::cerr << "Could not get default topic QoS" << std::endl;
		return 10;
	}

	// Get default DomainParticipant QoS from a DomainParticipantFactory:
	DDS::DomainParticipantQos dp_qos;
	ret = dpf_sub->get_default_participant_qos(dp_qos);
	if (DDS::RETCODE_OK != ret) {
		std::cerr << "Could not get default participant QoS" << std::endl;
		return 11;
	}

	// Get default DataReader QoS from a Subscriber:
	DDS::DataReaderQos dr_qos2;
	ret = sub->get_default_datareader_qos(dr_qos2);
	if (DDS::RETCODE_OK != ret) {
		std::cerr << "Could not get default data reader QoS" << std::endl;
		return 12;
	}

	return 0;
}

//开始订阅
void StartSubscribe() {
	while (1) {
		ACE_OS::sleep(1);
	}
}

//设置订阅者的本机id
void SetSubhomeID(string homeid)
{
	listener_servant->SethomeID(homeid);
}

//设置订阅者的接收消息id（用于消息过滤）
void SetRecvMsgId(string msgids)
{
	listener_servant->SetRecvMsgId(msgids);
}

//结束订阅
int EndSubscribe() {
	// 6. 清理与OpenDDS相关联的资源
	//先检查有没有被清理
	//if (participant_sub != nullptr)
	//{
	//	participant_sub->delete_contained_entities();
	//	dpf_sub->delete_participant(participant_sub);
	//	TheServiceParticipant->shutdown();
	//	participant_sub = nullptr;
	//	dpf_sub = nullptr;
	//	listener_servant = nullptr;
	//	ACE::fini();
	//	return 0;
	//}
	//return 0;

	if (participant_sub != nullptr)
	{
		participant_sub->delete_contained_entities();
	}
	if (dpf_sub != nullptr && participant_sub != nullptr)
	{
		dpf_sub->delete_participant(participant_sub);
	}
	TheServiceParticipant->shutdown();
	participant_sub = nullptr;
	dpf_sub = nullptr;
	listener_servant = nullptr;
	ACE::fini();
	return 0;
}

int ACE_TMAIN_XDataSub(int argc, ACE_TCHAR* argv[]) {
	try
	{
		// 初始化参与者
		argv[1] = "-DCPSConfigFile";
		argv[2] = "config_Sub.ini";
		argc = 3;

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

		// 2. 注册数据类型并创建主题
		XDataTopic1TypeSupportImpl* servant = new	XDataTopic1TypeSupportImpl();
		OpenDDS::DCPS::LocalObject_var safe_servant = servant;

		if (DDS::RETCODE_OK != servant->register_type(participant.in(), "")) {
			cerr << "register_type failed." << endl;
			exit(1);
		}

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

		// 3. 创建订阅者
		DDS::Subscriber_var sub =
			participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT,
				DDS::SubscriberListener::_nil(),
				::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		if (CORBA::is_nil(sub.in())) {
			cerr << "Failed to create_subscriber." << endl;
			exit(1);
		}

		// 4. 创建监听者
		//激活监听者
		DDS::DataReaderListener_var listener(new XDataReaderListener);
		XDataReaderListener* listener_servant =
			dynamic_cast<XDataReaderListener*>(listener.in());

		if (CORBA::is_nil(listener.in())) {
			cerr << "listener is nil." << endl;
			exit(1);
		}

		if (!listener_servant) {
			ACE_ERROR_RETURN((LM_ERROR,
				ACE_TEXT("%N:%l main()")
				ACE_TEXT(" ERROR: listener_servant is nil (dynamic_cast failed)!\n")), -1);
		}

		// 5. 创建数据读者
		DDS::DataReaderQos dr_qos;
		sub->get_default_datareader_qos(dr_qos);
		DDS::DataReader_var dr = sub->create_datareader(topic.in(),
			dr_qos,
			listener.in(),
			::OpenDDS::DCPS::DEFAULT_STATUS_MASK);
		if (CORBA::is_nil(dr.in())) {
			cerr << "create_datareader failed." << endl;
			exit(1);
		}

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

		// Get default DataReader QoS from a Subscriber:
		DDS::DataReaderQos dr_qos2;
		ret = sub->get_default_datareader_qos(dr_qos2);
		if (DDS::RETCODE_OK != ret) {
			std::cerr << "Could not get default data reader QoS" << std::endl;
		}


		while (1) {
			ACE_OS::sleep(1);
		}

		// 6. 清理与OpenDDS相关联的资源
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
