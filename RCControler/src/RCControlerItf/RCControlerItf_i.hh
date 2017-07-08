//------------------------------------------------------------------------------
//
// RCControlerItf_i.hh : Implementation of the interface from Sensor to Daq
//
//------------------------------------------------------------------------------
//
// AUTHOR      : T.DESCOMBES (descombt@ipnl.in2p3.fr)
// PROJECT     : EventBuilder
//
//------------------------------------------------------------------------------
//           Versions and Editions  historic
//------------------------------------------------------------------------------
// Ver      | Ed  |   date   | resp.       | comment
//----------+-----+----------+-------------+------------------------------------
// 1        | 1   | 10/12/02 | T.DESCOMBES | source creation
//----------+-----+----------+-------------+------------------------------------
//
//           Detailed modification operations on this source
//------------------------------------------------------------------------------
//  Date  :                            Author :
//  REL   :
//  Description :
//
//------------------------------------------------------------------------------

#ifndef RCCONTROLERITF_I_HH_
#define RCCONTROLERITF_I_HH_

#include "RCControlerItf.hh"
#include "RCControler.hh"

namespace ProjectRC
{
class RCControler;

class RCControlerItf_i : public POA_ProjectRC::SensorToDaqItf,
			 public PortableServer::RefCountServantBase
{
  RCControler* controler;
  
public:
  RCControlerItf_i(RCControler*);
  virtual void connect(RCCarItf_ptr) throw (CORBA::SystemException);
  virtual void logReport( const LogMessage& logmsg ) throw (CORBA::SystemException);
  virtual void ping() throw (CORBA::SystemException);
};
};

#endif
