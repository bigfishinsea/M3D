/* Generated by D:\opendds\OpenDDS-3.14\bin\opendds_idl version 3.14 (ACE version 6.5.12) running on input file XData.idl */
#include "XDataTypeSupportImpl.h"

#include <cstring>
#include <stdexcept>
#include "dds/DCPS/BuiltInTopicUtils.h"
#include "dds/DCPS/ContentFilteredTopicImpl.h"
#include "dds/DCPS/DataReaderImpl_T.h"
#include "dds/DCPS/DataWriterImpl_T.h"
#include "dds/DCPS/FilterEvaluator.h"
#include "dds/DCPS/MultiTopicDataReader_T.h"
#include "dds/DCPS/PoolAllocator.h"
#include "dds/DCPS/PublicationInstance.h"
#include "dds/DCPS/PublisherImpl.h"
#include "dds/DCPS/Qos_Helper.h"
#include "dds/DCPS/RakeData.h"
#include "dds/DCPS/RakeResults_T.h"
#include "dds/DCPS/ReceivedDataElementList.h"
#include "dds/DCPS/Registered_Data_Types.h"
#include "dds/DCPS/Service_Participant.h"
#include "dds/DCPS/SubscriberImpl.h"
#include "dds/DCPS/Util.h"
#include "dds/DCPS/debug.h"
#include "dds/DdsDcpsDomainC.h"


/* Begin MODULE: CORBA */


/* End MODULE: CORBA */


/* Begin MODULE: XDataIdlModule */



/* Begin STRUCT: XDataTopic1 */

OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL
namespace OpenDDS { namespace DCPS {

void gen_find_size(const XDataIdlModule::XDataTopic1& stru, size_t& size, size_t& padding)
{
  ACE_UNUSED_ARG(stru);
  ACE_UNUSED_ARG(size);
  ACE_UNUSED_ARG(padding);
  find_size_ulong(size, padding);
  size += ACE_OS::strlen(stru.id.in()) + 1;
  find_size_ulong(size, padding);
  size += ACE_OS::strlen(stru.destination.in()) + 1;
  if ((size + padding) % 8) {
    padding += 8 - ((size + padding) % 8);
  }
  size += gen_max_marshaled_size(stru.tTimer);
  find_size_ulong(size, padding);
  size += ACE_OS::strlen(stru.vData.in()) + 1;
}

bool operator<<(Serializer& strm, const XDataIdlModule::XDataTopic1& stru)
{
  ACE_UNUSED_ARG(strm);
  ACE_UNUSED_ARG(stru);
  return (strm << stru.id.in())
    && (strm << stru.destination.in())
    && (strm << stru.tTimer)
    && (strm << stru.vData.in());
}

bool operator>>(Serializer& strm, XDataIdlModule::XDataTopic1& stru)
{
  ACE_UNUSED_ARG(strm);
  ACE_UNUSED_ARG(stru);
  return (strm >> stru.id.out())
    && (strm >> stru.destination.out())
    && (strm >> stru.tTimer)
    && (strm >> stru.vData.out());
}

size_t gen_max_marshaled_size(const XDataIdlModule::XDataTopic1& stru, bool align)
{
  ACE_UNUSED_ARG(stru);
  ACE_UNUSED_ARG(align);
  return 0;
}

size_t gen_max_marshaled_size(KeyOnly<const XDataIdlModule::XDataTopic1> stru, bool align)
{
  ACE_UNUSED_ARG(stru);
  ACE_UNUSED_ARG(align);
  return 0;
}

void gen_find_size(KeyOnly<const XDataIdlModule::XDataTopic1> stru, size_t& size, size_t& padding)
{
  ACE_UNUSED_ARG(stru);
  ACE_UNUSED_ARG(size);
  ACE_UNUSED_ARG(padding);
  find_size_ulong(size, padding);
  size += ACE_OS::strlen(stru.t.id.in()) + 1;
}

bool operator<<(Serializer& strm, KeyOnly<const XDataIdlModule::XDataTopic1> stru)
{
  ACE_UNUSED_ARG(strm);
  ACE_UNUSED_ARG(stru);
  return (strm << stru.t.id.in());
}

bool operator>>(Serializer& strm, KeyOnly<XDataIdlModule::XDataTopic1> stru)
{
  ACE_UNUSED_ARG(strm);
  ACE_UNUSED_ARG(stru);
  return (strm >> stru.t.id.out());
}

}  }
OPENDDS_END_VERSIONED_NAMESPACE_DECL


namespace XDataIdlModule {
::DDS::DataWriter_ptr XDataTopic1TypeSupportImpl::create_datawriter()
{
  typedef OpenDDS::DCPS::DataWriterImpl_T<XDataTopic1> DataWriterImplType;
  ::DDS::DataWriter_ptr writer_impl = ::DDS::DataWriter::_nil();
  ACE_NEW_NORETURN(writer_impl,
                   DataWriterImplType());
  return writer_impl;
}

::DDS::DataReader_ptr XDataTopic1TypeSupportImpl::create_datareader()
{
  typedef OpenDDS::DCPS::DataReaderImpl_T<XDataTopic1> DataReaderImplType;
  ::DDS::DataReader_ptr reader_impl = ::DDS::DataReader::_nil();
  ACE_NEW_NORETURN(reader_impl,
                   DataReaderImplType());
  return reader_impl;
}

#ifndef OPENDDS_NO_MULTI_TOPIC
::DDS::DataReader_ptr XDataTopic1TypeSupportImpl::create_multitopic_datareader()
{
  typedef OpenDDS::DCPS::DataReaderImpl_T<XDataTopic1> DataReaderImplType;
  typedef OpenDDS::DCPS::MultiTopicDataReader_T<XDataTopic1, DataReaderImplType> MultiTopicDataReaderImplType;
  ::DDS::DataReader_ptr multitopic_reader_impl = ::DDS::DataReader::_nil();
  ACE_NEW_NORETURN(multitopic_reader_impl,
                   MultiTopicDataReaderImplType());
  return multitopic_reader_impl;
}
#endif /* !OPENDDS_NO_MULTI_TOPIC */

#ifndef OPENDDS_NO_CONTENT_SUBSCRIPTION_PROFILE
const OpenDDS::DCPS::MetaStruct& XDataTopic1TypeSupportImpl::getMetaStructForType()
{
  return OpenDDS::DCPS::getMetaStruct<XDataTopic1>();
}
#endif /* !OPENDDS_NO_CONTENT_SUBSCRIPTION_PROFILE */

bool XDataTopic1TypeSupportImpl::has_dcps_key()
{
  return TraitsType::gen_has_key ();
}

const char* XDataTopic1TypeSupportImpl::default_type_name() const
{
  return TraitsType::type_name();
}

XDataTopic1TypeSupport::_ptr_type XDataTopic1TypeSupportImpl::_narrow(CORBA::Object_ptr obj)
{
  return TypeSupportType::_narrow(obj);
}
}

#ifndef OPENDDS_NO_CONTENT_SUBSCRIPTION_PROFILE
OPENDDS_BEGIN_VERSIONED_NAMESPACE_DECL
namespace OpenDDS { namespace DCPS {

template<>
struct MetaStructImpl<XDataIdlModule::XDataTopic1> : MetaStruct {
  typedef XDataIdlModule::XDataTopic1 T;

#ifndef OPENDDS_NO_MULTI_TOPIC
  void* allocate() const { return new T; }

  void deallocate(void* stru) const { delete static_cast<T*>(stru); }

  size_t numDcpsKeys() const { return 1; }

#endif /* OPENDDS_NO_MULTI_TOPIC */

  bool isDcpsKey(const char* field) const
  {
    if (!ACE_OS::strcmp(field, "id")) {
      return true;
    }
    return false;
  }

  Value getValue(const void* stru, const char* field) const
  {
    const XDataIdlModule::XDataTopic1& typed = *static_cast<const XDataIdlModule::XDataTopic1*>(stru);
    ACE_UNUSED_ARG(typed);
    if (std::strcmp(field, "id") == 0) {
      return typed.id.in();
    }
    if (std::strcmp(field, "destination") == 0) {
      return typed.destination.in();
    }
    if (std::strcmp(field, "tTimer") == 0) {
      return typed.tTimer;
    }
    if (std::strcmp(field, "vData") == 0) {
      return typed.vData.in();
    }
    throw std::runtime_error("Field " + OPENDDS_STRING(field) + " not found or its type is not supported (in struct XDataIdlModule::XDataTopic1)");
  }

  Value getValue(Serializer& ser, const char* field) const
  {
    if (std::strcmp(field, "id") == 0) {
      TAO::String_Manager val;
      if (!(ser >> val.out())) {
        throw std::runtime_error("Field 'id' could not be deserialized");
      }
      return val;
    } else {
      ACE_CDR::ULong len;
      if (!(ser >> len)) {
        throw std::runtime_error("String 'id' length could not be deserialized");
      }
      if (!ser.skip(static_cast<ACE_UINT16>(len))) {
        throw std::runtime_error("String 'id' contents could not be skipped");
      }
    }
    if (std::strcmp(field, "destination") == 0) {
      TAO::String_Manager val;
      if (!(ser >> val.out())) {
        throw std::runtime_error("Field 'destination' could not be deserialized");
      }
      return val;
    } else {
      ACE_CDR::ULong len;
      if (!(ser >> len)) {
        throw std::runtime_error("String 'destination' length could not be deserialized");
      }
      if (!ser.skip(static_cast<ACE_UINT16>(len))) {
        throw std::runtime_error("String 'destination' contents could not be skipped");
      }
    }
    if (std::strcmp(field, "tTimer") == 0) {
      ACE_CDR::Double val;
      if (!(ser >> val)) {
        throw std::runtime_error("Field 'tTimer' could not be deserialized");
      }
      return val;
    } else {
      if (!ser.skip(1, 8)) {
        throw std::runtime_error("Field 'tTimer' could not be skipped");
      }
    }
    if (std::strcmp(field, "vData") == 0) {
      TAO::String_Manager val;
      if (!(ser >> val.out())) {
        throw std::runtime_error("Field 'vData' could not be deserialized");
      }
      return val;
    } else {
      ACE_CDR::ULong len;
      if (!(ser >> len)) {
        throw std::runtime_error("String 'vData' length could not be deserialized");
      }
      if (!ser.skip(static_cast<ACE_UINT16>(len))) {
        throw std::runtime_error("String 'vData' contents could not be skipped");
      }
    }
    if (!field[0]) {
      return 0;
    }
    throw std::runtime_error("Field " + OPENDDS_STRING(field) + " not valid for struct XDataIdlModule::XDataTopic1");
  }

  ComparatorBase::Ptr create_qc_comparator(const char* field, ComparatorBase::Ptr next) const
  {
    ACE_UNUSED_ARG(next);
    if (std::strcmp(field, "id") == 0) {
      return make_field_cmp(&T::id, next);
    }
    if (std::strcmp(field, "destination") == 0) {
      return make_field_cmp(&T::destination, next);
    }
    if (std::strcmp(field, "tTimer") == 0) {
      return make_field_cmp(&T::tTimer, next);
    }
    if (std::strcmp(field, "vData") == 0) {
      return make_field_cmp(&T::vData, next);
    }
    throw std::runtime_error("Field " + OPENDDS_STRING(field) + " not found or its type is not supported (in struct XDataIdlModule::XDataTopic1)");
  }

#ifndef OPENDDS_NO_MULTI_TOPIC
  const char** getFieldNames() const
  {
    static const char* names[] = {"id", "destination", "tTimer", "vData", 0};
    return names;
  }

  const void* getRawField(const void* stru, const char* field) const
  {
    if (std::strcmp(field, "id") == 0) {
      return &static_cast<const T*>(stru)->id;
    }
    if (std::strcmp(field, "destination") == 0) {
      return &static_cast<const T*>(stru)->destination;
    }
    if (std::strcmp(field, "tTimer") == 0) {
      return &static_cast<const T*>(stru)->tTimer;
    }
    if (std::strcmp(field, "vData") == 0) {
      return &static_cast<const T*>(stru)->vData;
    }
    throw std::runtime_error("Field " + OPENDDS_STRING(field) + " not found or its type is not supported (in struct XDataIdlModule::XDataTopic1)");
  }

  void assign(void* lhs, const char* field, const void* rhs,
    const char* rhsFieldSpec, const MetaStruct& rhsMeta) const
  {
    ACE_UNUSED_ARG(lhs);
    ACE_UNUSED_ARG(field);
    ACE_UNUSED_ARG(rhs);
    ACE_UNUSED_ARG(rhsFieldSpec);
    ACE_UNUSED_ARG(rhsMeta);
    if (std::strcmp(field, "id") == 0) {
      static_cast<T*>(lhs)->id = *static_cast<const TAO::String_Manager*>(rhsMeta.getRawField(rhs, rhsFieldSpec));
      return;
    }
    if (std::strcmp(field, "destination") == 0) {
      static_cast<T*>(lhs)->destination = *static_cast<const TAO::String_Manager*>(rhsMeta.getRawField(rhs, rhsFieldSpec));
      return;
    }
    if (std::strcmp(field, "tTimer") == 0) {
      static_cast<T*>(lhs)->tTimer = *static_cast<const CORBA::Double*>(rhsMeta.getRawField(rhs, rhsFieldSpec));
      return;
    }
    if (std::strcmp(field, "vData") == 0) {
      static_cast<T*>(lhs)->vData = *static_cast<const TAO::String_Manager*>(rhsMeta.getRawField(rhs, rhsFieldSpec));
      return;
    }
    throw std::runtime_error("Field " + OPENDDS_STRING(field) + " not found or its type is not supported (in struct XDataIdlModule::XDataTopic1)");
  }
#endif /* OPENDDS_NO_MULTI_TOPIC */

  bool compare(const void* lhs, const void* rhs, const char* field) const
  {
    ACE_UNUSED_ARG(lhs);
    ACE_UNUSED_ARG(field);
    ACE_UNUSED_ARG(rhs);
    if (std::strcmp(field, "id") == 0) {
      return 0 == ACE_OS::strcmp(static_cast<const T*>(lhs)->id.in(), static_cast<const T*>(rhs)->id.in());
    }
    if (std::strcmp(field, "destination") == 0) {
      return 0 == ACE_OS::strcmp(static_cast<const T*>(lhs)->destination.in(), static_cast<const T*>(rhs)->destination.in());
    }
    if (std::strcmp(field, "tTimer") == 0) {
      return static_cast<const T*>(lhs)->tTimer == static_cast<const T*>(rhs)->tTimer;
    }
    if (std::strcmp(field, "vData") == 0) {
      return 0 == ACE_OS::strcmp(static_cast<const T*>(lhs)->vData.in(), static_cast<const T*>(rhs)->vData.in());
    }
    throw std::runtime_error("Field " + OPENDDS_STRING(field) + " not found or its type is not supported (in struct XDataIdlModule::XDataTopic1)");
  }
};

template<>
const MetaStruct& getMetaStruct<XDataIdlModule::XDataTopic1>()
{
  static MetaStructImpl<XDataIdlModule::XDataTopic1> msi;
  return msi;
}

bool gen_skip_over(Serializer& ser, XDataIdlModule::XDataTopic1*)
{
  ACE_UNUSED_ARG(ser);
  MetaStructImpl<XDataIdlModule::XDataTopic1>().getValue(ser, "");
  return true;
}

}  }
OPENDDS_END_VERSIONED_NAMESPACE_DECL

#endif

/* End STRUCT: XDataTopic1 */

/* End MODULE: XDataIdlModule */
