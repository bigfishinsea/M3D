// -*- C++ -*-
/**
 * Code generated by the The ACE ORB (TAO) IDL Compiler v2.5.12
 * TAO and the TAO IDL Compiler have been developed by:
 *       Center for Distributed Object Computing
 *       Washington University
 *       St. Louis, MO
 *       USA
 * and
 *       Distributed Object Computing Laboratory
 *       University of California at Irvine
 *       Irvine, CA
 *       USA
 * and
 *       Institute for Software Integrated Systems
 *       Vanderbilt University
 *       Nashville, TN
 *       USA
 *       https://www.isis.vanderbilt.edu/
 *
 * Information about TAO is available at:
 *     https://www.dre.vanderbilt.edu/~schmidt/TAO.html
 **/

// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_codegen.cpp:149

#ifndef _TAO_IDL_XDATATYPESUPPORTC_M2VQZ1_H_
#define _TAO_IDL_XDATATYPESUPPORTC_M2VQZ1_H_

#include /**/ "ace/pre.h"


#include /**/ "ace/config-all.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */


#include "tao/ORB.h"
#include "tao/SystemException.h"
#include "tao/Basic_Types.h"
#include "tao/ORB_Constants.h"
#include "dds/DCPS/ZeroCopyInfoSeq_T.h"
#include "dds/DCPS/ZeroCopySeq_T.h"
#include "tao/Object.h"
#include "tao/String_Manager_T.h"
#include "tao/Sequence_T.h"
#include "tao/Objref_VarOut_T.h"
#include "tao/Seq_Var_T.h"
#include "tao/Seq_Out_T.h"
#include "tao/Arg_Traits_T.h"
#include "tao/Basic_Arguments.h"
#include "tao/Special_Basic_Arguments.h"
#include "tao/Any_Insert_Policy_T.h"
#include "tao/Fixed_Size_Argument_T.h"
#include "tao/Var_Size_Argument_T.h"
#include /**/ "tao/Version.h"
#include /**/ "tao/Versioned_Namespace.h"

#include "XDataC.h"
#include "dds/DdsDcpsInfrastructureC.h"
#include "dds/DdsDcpsPublicationC.h"
#include "dds/DdsDcpsSubscriptionExtC.h"
#include "dds/DdsDcpsTopicC.h"
#include "dds/DdsDcpsTypeSupportExtC.h"

#if TAO_MAJOR_VERSION != 2 || TAO_MINOR_VERSION != 5 || TAO_MICRO_VERSION != 12
#error This file should be regenerated with TAO_IDL
#endif

// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_module\module_ch.cpp:35

namespace XDataIdlModule
{

  // TAO_IDL - Generated from
  // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_sequence\sequence_ch.cpp:99

  typedef ::TAO::DCPS::ZeroCopyDataSeq< XDataIdlModule::XDataTopic1, DCPS_ZERO_COPY_SEQ_DEFAULT_SIZE> XDataTopic1Seq;
  

  // TAO_IDL - Generated from
  // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_interface.cpp:748

#if !defined (_XDATAIDLMODULE_XDATATOPIC1TYPESUPPORT__VAR_OUT_CH_)
#define _XDATAIDLMODULE_XDATATOPIC1TYPESUPPORT__VAR_OUT_CH_

  class XDataTopic1TypeSupport;
  typedef XDataTopic1TypeSupport *XDataTopic1TypeSupport_ptr;
  typedef TAO_Objref_Var_T<XDataTopic1TypeSupport> XDataTopic1TypeSupport_var;
  typedef TAO_Objref_Out_T<XDataTopic1TypeSupport> XDataTopic1TypeSupport_out;
  

#endif /* end #if !defined */

  // TAO_IDL - Generated from
  // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_ch.cpp:40

  class  XDataTopic1TypeSupport
    : public virtual ::OpenDDS::DCPS::TypeSupport
  
  {
  public:

    // TAO_IDL - Generated from
    // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_type.cpp:304

    typedef XDataTopic1TypeSupport_ptr _ptr_type;
    typedef XDataTopic1TypeSupport_var _var_type;
    typedef XDataTopic1TypeSupport_out _out_type;

    // The static operations.
    static XDataTopic1TypeSupport_ptr _duplicate (XDataTopic1TypeSupport_ptr obj);

    static void _tao_release (XDataTopic1TypeSupport_ptr obj);

    static XDataTopic1TypeSupport_ptr _narrow (::CORBA::Object_ptr obj);
    static XDataTopic1TypeSupport_ptr _unchecked_narrow (::CORBA::Object_ptr obj);
    static XDataTopic1TypeSupport_ptr _nil (void);

    // TAO_IDL - Generated from
    // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_ch.cpp:137

    virtual ::CORBA::Boolean _is_a (const char *type_id);
    virtual const char* _interface_repository_id (void) const;
    virtual ::CORBA::Boolean marshal (TAO_OutputCDR &cdr);
  
  protected:
    // Abstract or local interface only.
    XDataTopic1TypeSupport (void);

    

    virtual ~XDataTopic1TypeSupport (void);
  
  private:
    // Private and unimplemented for concrete interfaces.
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1TypeSupport (const XDataTopic1TypeSupport &))
#if defined (ACE_HAS_CPP11)
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1TypeSupport (XDataTopic1TypeSupport &&))
#endif /* ACE_HAS_CPP11 */
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1TypeSupport &operator= (const XDataTopic1TypeSupport &))
#if defined (ACE_HAS_CPP11)
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1TypeSupport &operator= (XDataTopic1TypeSupport &&))
#endif /* ACE_HAS_CPP11 */
  };

  // TAO_IDL - Generated from
  // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_interface.cpp:748

#if !defined (_XDATAIDLMODULE_XDATATOPIC1DATAWRITER__VAR_OUT_CH_)
#define _XDATAIDLMODULE_XDATATOPIC1DATAWRITER__VAR_OUT_CH_

  class XDataTopic1DataWriter;
  typedef XDataTopic1DataWriter *XDataTopic1DataWriter_ptr;
  typedef TAO_Objref_Var_T<XDataTopic1DataWriter> XDataTopic1DataWriter_var;
  typedef TAO_Objref_Out_T<XDataTopic1DataWriter> XDataTopic1DataWriter_out;
  

#endif /* end #if !defined */

  // TAO_IDL - Generated from
  // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_ch.cpp:40

  class  XDataTopic1DataWriter
    : public virtual ::DDS::DataWriter
  
  {
  public:

    // TAO_IDL - Generated from
    // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_type.cpp:304

    typedef XDataTopic1DataWriter_ptr _ptr_type;
    typedef XDataTopic1DataWriter_var _var_type;
    typedef XDataTopic1DataWriter_out _out_type;

    // The static operations.
    static XDataTopic1DataWriter_ptr _duplicate (XDataTopic1DataWriter_ptr obj);

    static void _tao_release (XDataTopic1DataWriter_ptr obj);

    static XDataTopic1DataWriter_ptr _narrow (::CORBA::Object_ptr obj);
    static XDataTopic1DataWriter_ptr _unchecked_narrow (::CORBA::Object_ptr obj);
    static XDataTopic1DataWriter_ptr _nil (void);

    virtual ::DDS::InstanceHandle_t register_instance (
      const ::XDataIdlModule::XDataTopic1 & instance) = 0;

    virtual ::DDS::InstanceHandle_t register_instance_w_timestamp (
      const ::XDataIdlModule::XDataTopic1 & instance,
      const ::DDS::Time_t & timestamp) = 0;

    virtual ::DDS::ReturnCode_t unregister_instance (
      const ::XDataIdlModule::XDataTopic1 & instance,
      ::DDS::InstanceHandle_t handle) = 0;

    virtual ::DDS::ReturnCode_t unregister_instance_w_timestamp (
      const ::XDataIdlModule::XDataTopic1 & instance,
      ::DDS::InstanceHandle_t handle,
      const ::DDS::Time_t & timestamp) = 0;

    virtual ::DDS::ReturnCode_t write (
      const ::XDataIdlModule::XDataTopic1 & instance_data,
      ::DDS::InstanceHandle_t handle) = 0;

    virtual ::DDS::ReturnCode_t write_w_timestamp (
      const ::XDataIdlModule::XDataTopic1 & instance_data,
      ::DDS::InstanceHandle_t handle,
      const ::DDS::Time_t & source_timestamp) = 0;

    virtual ::DDS::ReturnCode_t dispose (
      const ::XDataIdlModule::XDataTopic1 & instance_data,
      ::DDS::InstanceHandle_t instance_handle) = 0;

    virtual ::DDS::ReturnCode_t dispose_w_timestamp (
      const ::XDataIdlModule::XDataTopic1 & instance_data,
      ::DDS::InstanceHandle_t instance_handle,
      const ::DDS::Time_t & source_timestamp) = 0;

    virtual ::DDS::ReturnCode_t get_key_value (
      ::XDataIdlModule::XDataTopic1 & key_holder,
      ::DDS::InstanceHandle_t handle) = 0;

    virtual ::DDS::InstanceHandle_t lookup_instance (
      const ::XDataIdlModule::XDataTopic1 & instance_data) = 0;

    // TAO_IDL - Generated from
    // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_ch.cpp:137

    virtual ::CORBA::Boolean _is_a (const char *type_id);
    virtual const char* _interface_repository_id (void) const;
    virtual ::CORBA::Boolean marshal (TAO_OutputCDR &cdr);
  
  protected:
    // Abstract or local interface only.
    XDataTopic1DataWriter (void);

    

    virtual ~XDataTopic1DataWriter (void);
  
  private:
    // Private and unimplemented for concrete interfaces.
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1DataWriter (const XDataTopic1DataWriter &))
#if defined (ACE_HAS_CPP11)
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1DataWriter (XDataTopic1DataWriter &&))
#endif /* ACE_HAS_CPP11 */
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1DataWriter &operator= (const XDataTopic1DataWriter &))
#if defined (ACE_HAS_CPP11)
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1DataWriter &operator= (XDataTopic1DataWriter &&))
#endif /* ACE_HAS_CPP11 */
  };

  // TAO_IDL - Generated from
  // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_interface.cpp:748

#if !defined (_XDATAIDLMODULE_XDATATOPIC1DATAREADER__VAR_OUT_CH_)
#define _XDATAIDLMODULE_XDATATOPIC1DATAREADER__VAR_OUT_CH_

  class XDataTopic1DataReader;
  typedef XDataTopic1DataReader *XDataTopic1DataReader_ptr;
  typedef TAO_Objref_Var_T<XDataTopic1DataReader> XDataTopic1DataReader_var;
  typedef TAO_Objref_Out_T<XDataTopic1DataReader> XDataTopic1DataReader_out;
  

#endif /* end #if !defined */

  // TAO_IDL - Generated from
  // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_ch.cpp:40

  class  XDataTopic1DataReader
    : public virtual ::OpenDDS::DCPS::DataReaderEx
  
  {
  public:

    // TAO_IDL - Generated from
    // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_type.cpp:304

    typedef XDataTopic1DataReader_ptr _ptr_type;
    typedef XDataTopic1DataReader_var _var_type;
    typedef XDataTopic1DataReader_out _out_type;

    // The static operations.
    static XDataTopic1DataReader_ptr _duplicate (XDataTopic1DataReader_ptr obj);

    static void _tao_release (XDataTopic1DataReader_ptr obj);

    static XDataTopic1DataReader_ptr _narrow (::CORBA::Object_ptr obj);
    static XDataTopic1DataReader_ptr _unchecked_narrow (::CORBA::Object_ptr obj);
    static XDataTopic1DataReader_ptr _nil (void);

    virtual ::DDS::ReturnCode_t read (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & info_seq,
      ::CORBA::Long max_samples,
      ::DDS::SampleStateMask sample_states,
      ::DDS::ViewStateMask view_states,
      ::DDS::InstanceStateMask instance_states) = 0;

    virtual ::DDS::ReturnCode_t take (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & info_seq,
      ::CORBA::Long max_samples,
      ::DDS::SampleStateMask sample_states,
      ::DDS::ViewStateMask view_states,
      ::DDS::InstanceStateMask instance_states) = 0;

    virtual ::DDS::ReturnCode_t read_w_condition (
      ::XDataIdlModule::XDataTopic1Seq & data_values,
      ::DDS::SampleInfoSeq & sample_infos,
      ::CORBA::Long max_samples,
      ::DDS::ReadCondition_ptr a_condition) = 0;

    virtual ::DDS::ReturnCode_t take_w_condition (
      ::XDataIdlModule::XDataTopic1Seq & data_values,
      ::DDS::SampleInfoSeq & sample_infos,
      ::CORBA::Long max_samples,
      ::DDS::ReadCondition_ptr a_condition) = 0;

    virtual ::DDS::ReturnCode_t read_next_sample (
      ::XDataIdlModule::XDataTopic1 & received_data,
      ::DDS::SampleInfo & sample_info) = 0;

    virtual ::DDS::ReturnCode_t take_next_sample (
      ::XDataIdlModule::XDataTopic1 & received_data,
      ::DDS::SampleInfo & sample_info) = 0;

    virtual ::DDS::ReturnCode_t read_instance (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & info_seq,
      ::CORBA::Long max_samples,
      ::DDS::InstanceHandle_t a_handle,
      ::DDS::SampleStateMask sample_states,
      ::DDS::ViewStateMask view_states,
      ::DDS::InstanceStateMask instance_states) = 0;

    virtual ::DDS::ReturnCode_t take_instance (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & info_seq,
      ::CORBA::Long max_samples,
      ::DDS::InstanceHandle_t a_handle,
      ::DDS::SampleStateMask sample_states,
      ::DDS::ViewStateMask view_states,
      ::DDS::InstanceStateMask instance_states) = 0;

    virtual ::DDS::ReturnCode_t read_instance_w_condition (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & sample_infos,
      ::CORBA::Long max_samples,
      ::DDS::InstanceHandle_t a_handle,
      ::DDS::ReadCondition_ptr a_condition) = 0;

    virtual ::DDS::ReturnCode_t take_instance_w_condition (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & sample_infos,
      ::CORBA::Long max_samples,
      ::DDS::InstanceHandle_t a_handle,
      ::DDS::ReadCondition_ptr a_condition) = 0;

    virtual ::DDS::ReturnCode_t read_next_instance (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & info_seq,
      ::CORBA::Long max_samples,
      ::DDS::InstanceHandle_t a_handle,
      ::DDS::SampleStateMask sample_states,
      ::DDS::ViewStateMask view_states,
      ::DDS::InstanceStateMask instance_states) = 0;

    virtual ::DDS::ReturnCode_t take_next_instance (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & info_seq,
      ::CORBA::Long max_samples,
      ::DDS::InstanceHandle_t a_handle,
      ::DDS::SampleStateMask sample_states,
      ::DDS::ViewStateMask view_states,
      ::DDS::InstanceStateMask instance_states) = 0;

    virtual ::DDS::ReturnCode_t read_next_instance_w_condition (
      ::XDataIdlModule::XDataTopic1Seq & data_values,
      ::DDS::SampleInfoSeq & sample_infos,
      ::CORBA::Long max_samples,
      ::DDS::InstanceHandle_t previous_handle,
      ::DDS::ReadCondition_ptr a_condition) = 0;

    virtual ::DDS::ReturnCode_t take_next_instance_w_condition (
      ::XDataIdlModule::XDataTopic1Seq & data_values,
      ::DDS::SampleInfoSeq & sample_infos,
      ::CORBA::Long max_samples,
      ::DDS::InstanceHandle_t previous_handle,
      ::DDS::ReadCondition_ptr a_condition) = 0;

    virtual ::DDS::ReturnCode_t return_loan (
      ::XDataIdlModule::XDataTopic1Seq & received_data,
      ::DDS::SampleInfoSeq & info_seq) = 0;

    virtual ::DDS::ReturnCode_t get_key_value (
      ::XDataIdlModule::XDataTopic1 & key_holder,
      ::DDS::InstanceHandle_t handle) = 0;

    virtual ::DDS::InstanceHandle_t lookup_instance (
      const ::XDataIdlModule::XDataTopic1 & instance_data) = 0;

    // TAO_IDL - Generated from
    // D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_ch.cpp:137

    virtual ::CORBA::Boolean _is_a (const char *type_id);
    virtual const char* _interface_repository_id (void) const;
    virtual ::CORBA::Boolean marshal (TAO_OutputCDR &cdr);
  
  protected:
    // Abstract or local interface only.
    XDataTopic1DataReader (void);

    

    virtual ~XDataTopic1DataReader (void);
  
  private:
    // Private and unimplemented for concrete interfaces.
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1DataReader (const XDataTopic1DataReader &))
#if defined (ACE_HAS_CPP11)
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1DataReader (XDataTopic1DataReader &&))
#endif /* ACE_HAS_CPP11 */
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1DataReader &operator= (const XDataTopic1DataReader &))
#if defined (ACE_HAS_CPP11)
    ACE_UNIMPLEMENTED_FUNC (XDataTopic1DataReader &operator= (XDataTopic1DataReader &&))
#endif /* ACE_HAS_CPP11 */
  };

// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_module\module_ch.cpp:64

} // module XDataIdlModule

// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_arg_traits.cpp:66

TAO_BEGIN_VERSIONED_NAMESPACE_DECL


// Arg traits specializations.
namespace TAO
{
}

TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_traits.cpp:60

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

// Traits specializations.
namespace TAO
{

#if !defined (_XDATAIDLMODULE_XDATATOPIC1TYPESUPPORT__TRAITS_)
#define _XDATAIDLMODULE_XDATATOPIC1TYPESUPPORT__TRAITS_

  template<>
  struct  Objref_Traits< ::XDataIdlModule::XDataTopic1TypeSupport>
  {
    static ::XDataIdlModule::XDataTopic1TypeSupport_ptr duplicate (
        ::XDataIdlModule::XDataTopic1TypeSupport_ptr p);
    static void release (
        ::XDataIdlModule::XDataTopic1TypeSupport_ptr p);
    static ::XDataIdlModule::XDataTopic1TypeSupport_ptr nil (void);
    static ::CORBA::Boolean marshal (
        const ::XDataIdlModule::XDataTopic1TypeSupport_ptr p,
        TAO_OutputCDR & cdr);
  };

#endif /* end #if !defined */

#if !defined (_XDATAIDLMODULE_XDATATOPIC1DATAWRITER__TRAITS_)
#define _XDATAIDLMODULE_XDATATOPIC1DATAWRITER__TRAITS_

  template<>
  struct  Objref_Traits< ::XDataIdlModule::XDataTopic1DataWriter>
  {
    static ::XDataIdlModule::XDataTopic1DataWriter_ptr duplicate (
        ::XDataIdlModule::XDataTopic1DataWriter_ptr p);
    static void release (
        ::XDataIdlModule::XDataTopic1DataWriter_ptr p);
    static ::XDataIdlModule::XDataTopic1DataWriter_ptr nil (void);
    static ::CORBA::Boolean marshal (
        const ::XDataIdlModule::XDataTopic1DataWriter_ptr p,
        TAO_OutputCDR & cdr);
  };

#endif /* end #if !defined */

#if !defined (_XDATAIDLMODULE_XDATATOPIC1DATAREADER__TRAITS_)
#define _XDATAIDLMODULE_XDATATOPIC1DATAREADER__TRAITS_

  template<>
  struct  Objref_Traits< ::XDataIdlModule::XDataTopic1DataReader>
  {
    static ::XDataIdlModule::XDataTopic1DataReader_ptr duplicate (
        ::XDataIdlModule::XDataTopic1DataReader_ptr p);
    static void release (
        ::XDataIdlModule::XDataTopic1DataReader_ptr p);
    static ::XDataIdlModule::XDataTopic1DataReader_ptr nil (void);
    static ::CORBA::Boolean marshal (
        const ::XDataIdlModule::XDataTopic1DataReader_ptr p,
        TAO_OutputCDR & cdr);
  };

#endif /* end #if !defined */
}
TAO_END_VERSIONED_NAMESPACE_DECL



// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_codegen.cpp:1686
#if defined (__ACE_INLINE__)
#include "XDataTypeSupportC.inl"
#endif /* defined INLINE */

#include /**/ "ace/post.h"

#endif /* ifndef */

