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
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_codegen.cpp:366


#include "XDataTypeSupportC.h"
#include "tao/CDR.h"
#include "ace/OS_NS_string.h"

#if !defined (__ACE_INLINE__)
#include "XDataTypeSupportC.inl"
#endif /* !defined INLINE */

// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_cs.cpp:48
TAO_BEGIN_VERSIONED_NAMESPACE_DECL

// Traits specializations for XDataIdlModule::XDataTopic1TypeSupport.

XDataIdlModule::XDataTopic1TypeSupport_ptr
TAO::Objref_Traits<XDataIdlModule::XDataTopic1TypeSupport>::duplicate (
    XDataIdlModule::XDataTopic1TypeSupport_ptr p)
{
  return XDataIdlModule::XDataTopic1TypeSupport::_duplicate (p);
}

void
TAO::Objref_Traits<XDataIdlModule::XDataTopic1TypeSupport>::release (
    XDataIdlModule::XDataTopic1TypeSupport_ptr p)
{
  ::CORBA::release (p);
}

XDataIdlModule::XDataTopic1TypeSupport_ptr
TAO::Objref_Traits<XDataIdlModule::XDataTopic1TypeSupport>::nil (void)
{
  return XDataIdlModule::XDataTopic1TypeSupport::_nil ();
}

::CORBA::Boolean
TAO::Objref_Traits<XDataIdlModule::XDataTopic1TypeSupport>::marshal (
    const XDataIdlModule::XDataTopic1TypeSupport_ptr p,
    TAO_OutputCDR & cdr)
{
  return ::CORBA::Object::marshal (p, cdr);
}

TAO_END_VERSIONED_NAMESPACE_DECL



XDataIdlModule::XDataTopic1TypeSupport::XDataTopic1TypeSupport (void)
{}

XDataIdlModule::XDataTopic1TypeSupport::~XDataTopic1TypeSupport (void)
{
}

XDataIdlModule::XDataTopic1TypeSupport_ptr
XDataIdlModule::XDataTopic1TypeSupport::_narrow (
    ::CORBA::Object_ptr _tao_objref)
{
  return XDataTopic1TypeSupport::_duplicate (
      dynamic_cast<XDataTopic1TypeSupport_ptr> (_tao_objref));
}

XDataIdlModule::XDataTopic1TypeSupport_ptr
XDataIdlModule::XDataTopic1TypeSupport::_unchecked_narrow (
    ::CORBA::Object_ptr _tao_objref)
{
  return XDataTopic1TypeSupport::_duplicate (
      dynamic_cast<XDataTopic1TypeSupport_ptr> (_tao_objref));
}

XDataIdlModule::XDataTopic1TypeSupport_ptr
XDataIdlModule::XDataTopic1TypeSupport::_nil (void)
{
  return 0;
}

XDataIdlModule::XDataTopic1TypeSupport_ptr
XDataIdlModule::XDataTopic1TypeSupport::_duplicate (XDataTopic1TypeSupport_ptr obj)
{
  if (! ::CORBA::is_nil (obj))
    {
      obj->_add_ref ();
    }
  return obj;
}

void
XDataIdlModule::XDataTopic1TypeSupport::_tao_release (XDataTopic1TypeSupport_ptr obj)
{
  ::CORBA::release (obj);
}

::CORBA::Boolean
XDataIdlModule::XDataTopic1TypeSupport::_is_a (const char *value)
{
  if (
      ACE_OS::strcmp (
          value,
          "IDL:DDS/TypeSupport:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:OpenDDS/DCPS/TypeSupport:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:XDataIdlModule/XDataTopic1TypeSupport:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:omg.org/CORBA/LocalObject:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:omg.org/CORBA/Object:1.0"
        ) == 0
    )
    {
      return true; // success using local knowledge
    }
  else
    {
      return false;
    }
}

const char* XDataIdlModule::XDataTopic1TypeSupport::_interface_repository_id (void) const
{
  return "IDL:XDataIdlModule/XDataTopic1TypeSupport:1.0";
}

::CORBA::Boolean
XDataIdlModule::XDataTopic1TypeSupport::marshal (TAO_OutputCDR & /* cdr */)
{
  return false;
}

// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_cs.cpp:48
TAO_BEGIN_VERSIONED_NAMESPACE_DECL

// Traits specializations for XDataIdlModule::XDataTopic1DataWriter.

XDataIdlModule::XDataTopic1DataWriter_ptr
TAO::Objref_Traits<XDataIdlModule::XDataTopic1DataWriter>::duplicate (
    XDataIdlModule::XDataTopic1DataWriter_ptr p)
{
  return XDataIdlModule::XDataTopic1DataWriter::_duplicate (p);
}

void
TAO::Objref_Traits<XDataIdlModule::XDataTopic1DataWriter>::release (
    XDataIdlModule::XDataTopic1DataWriter_ptr p)
{
  ::CORBA::release (p);
}

XDataIdlModule::XDataTopic1DataWriter_ptr
TAO::Objref_Traits<XDataIdlModule::XDataTopic1DataWriter>::nil (void)
{
  return XDataIdlModule::XDataTopic1DataWriter::_nil ();
}

::CORBA::Boolean
TAO::Objref_Traits<XDataIdlModule::XDataTopic1DataWriter>::marshal (
    const XDataIdlModule::XDataTopic1DataWriter_ptr p,
    TAO_OutputCDR & cdr)
{
  return ::CORBA::Object::marshal (p, cdr);
}

TAO_END_VERSIONED_NAMESPACE_DECL



XDataIdlModule::XDataTopic1DataWriter::XDataTopic1DataWriter (void)
{}

XDataIdlModule::XDataTopic1DataWriter::~XDataTopic1DataWriter (void)
{
}

XDataIdlModule::XDataTopic1DataWriter_ptr
XDataIdlModule::XDataTopic1DataWriter::_narrow (
    ::CORBA::Object_ptr _tao_objref)
{
  return XDataTopic1DataWriter::_duplicate (
      dynamic_cast<XDataTopic1DataWriter_ptr> (_tao_objref));
}

XDataIdlModule::XDataTopic1DataWriter_ptr
XDataIdlModule::XDataTopic1DataWriter::_unchecked_narrow (
    ::CORBA::Object_ptr _tao_objref)
{
  return XDataTopic1DataWriter::_duplicate (
      dynamic_cast<XDataTopic1DataWriter_ptr> (_tao_objref));
}

XDataIdlModule::XDataTopic1DataWriter_ptr
XDataIdlModule::XDataTopic1DataWriter::_nil (void)
{
  return 0;
}

XDataIdlModule::XDataTopic1DataWriter_ptr
XDataIdlModule::XDataTopic1DataWriter::_duplicate (XDataTopic1DataWriter_ptr obj)
{
  if (! ::CORBA::is_nil (obj))
    {
      obj->_add_ref ();
    }
  return obj;
}

void
XDataIdlModule::XDataTopic1DataWriter::_tao_release (XDataTopic1DataWriter_ptr obj)
{
  ::CORBA::release (obj);
}

::CORBA::Boolean
XDataIdlModule::XDataTopic1DataWriter::_is_a (const char *value)
{
  if (
      ACE_OS::strcmp (
          value,
          "IDL:DDS/Entity:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:DDS/DataWriter:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:XDataIdlModule/XDataTopic1DataWriter:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:omg.org/CORBA/LocalObject:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:omg.org/CORBA/Object:1.0"
        ) == 0
    )
    {
      return true; // success using local knowledge
    }
  else
    {
      return false;
    }
}

const char* XDataIdlModule::XDataTopic1DataWriter::_interface_repository_id (void) const
{
  return "IDL:XDataIdlModule/XDataTopic1DataWriter:1.0";
}

::CORBA::Boolean
XDataIdlModule::XDataTopic1DataWriter::marshal (TAO_OutputCDR & /* cdr */)
{
  return false;
}

// TAO_IDL - Generated from
// D:\opendds\ACE_wrappers\TAO\TAO_IDL\be\be_visitor_interface\interface_cs.cpp:48
TAO_BEGIN_VERSIONED_NAMESPACE_DECL

// Traits specializations for XDataIdlModule::XDataTopic1DataReader.

XDataIdlModule::XDataTopic1DataReader_ptr
TAO::Objref_Traits<XDataIdlModule::XDataTopic1DataReader>::duplicate (
    XDataIdlModule::XDataTopic1DataReader_ptr p)
{
  return XDataIdlModule::XDataTopic1DataReader::_duplicate (p);
}

void
TAO::Objref_Traits<XDataIdlModule::XDataTopic1DataReader>::release (
    XDataIdlModule::XDataTopic1DataReader_ptr p)
{
  ::CORBA::release (p);
}

XDataIdlModule::XDataTopic1DataReader_ptr
TAO::Objref_Traits<XDataIdlModule::XDataTopic1DataReader>::nil (void)
{
  return XDataIdlModule::XDataTopic1DataReader::_nil ();
}

::CORBA::Boolean
TAO::Objref_Traits<XDataIdlModule::XDataTopic1DataReader>::marshal (
    const XDataIdlModule::XDataTopic1DataReader_ptr p,
    TAO_OutputCDR & cdr)
{
  return ::CORBA::Object::marshal (p, cdr);
}

TAO_END_VERSIONED_NAMESPACE_DECL



XDataIdlModule::XDataTopic1DataReader::XDataTopic1DataReader (void)
{}

XDataIdlModule::XDataTopic1DataReader::~XDataTopic1DataReader (void)
{
}

XDataIdlModule::XDataTopic1DataReader_ptr
XDataIdlModule::XDataTopic1DataReader::_narrow (
    ::CORBA::Object_ptr _tao_objref)
{
  return XDataTopic1DataReader::_duplicate (
      dynamic_cast<XDataTopic1DataReader_ptr> (_tao_objref));
}

XDataIdlModule::XDataTopic1DataReader_ptr
XDataIdlModule::XDataTopic1DataReader::_unchecked_narrow (
    ::CORBA::Object_ptr _tao_objref)
{
  return XDataTopic1DataReader::_duplicate (
      dynamic_cast<XDataTopic1DataReader_ptr> (_tao_objref));
}

XDataIdlModule::XDataTopic1DataReader_ptr
XDataIdlModule::XDataTopic1DataReader::_nil (void)
{
  return 0;
}

XDataIdlModule::XDataTopic1DataReader_ptr
XDataIdlModule::XDataTopic1DataReader::_duplicate (XDataTopic1DataReader_ptr obj)
{
  if (! ::CORBA::is_nil (obj))
    {
      obj->_add_ref ();
    }
  return obj;
}

void
XDataIdlModule::XDataTopic1DataReader::_tao_release (XDataTopic1DataReader_ptr obj)
{
  ::CORBA::release (obj);
}

::CORBA::Boolean
XDataIdlModule::XDataTopic1DataReader::_is_a (const char *value)
{
  if (
      ACE_OS::strcmp (
          value,
          "IDL:DDS/Entity:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:DDS/DataReader:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:OpenDDS/DCPS/DataReaderEx:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:XDataIdlModule/XDataTopic1DataReader:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:omg.org/CORBA/LocalObject:1.0"
        ) == 0 ||
      ACE_OS::strcmp (
          value,
          "IDL:omg.org/CORBA/Object:1.0"
        ) == 0
    )
    {
      return true; // success using local knowledge
    }
  else
    {
      return false;
    }
}

const char* XDataIdlModule::XDataTopic1DataReader::_interface_repository_id (void) const
{
  return "IDL:XDataIdlModule/XDataTopic1DataReader:1.0";
}

::CORBA::Boolean
XDataIdlModule::XDataTopic1DataReader::marshal (TAO_OutputCDR & /* cdr */)
{
  return false;
}
