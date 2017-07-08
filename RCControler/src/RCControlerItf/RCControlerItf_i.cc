//------------------------------------------------------------------------------
//
// RCControlerItf_i.cc : Implementation of the interface from Sensor to Daq
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

#include "RCControlerItf_i.hh"


namespace ProjectRC
{

  RCControlerItf_i::RCControlerItf_i(RCControler* c)
  {
    controler=c;
  }

  /***********************************************************************/

  void RCControlerItf_i::connect ( RCCarItf_ptr carItf ) throw (CORBA::SystemException)
  {
     controler->connectCar ( carItf );
  }

  /***********************************************************************/
  
  void RCControlerItf_i::logReport( const LogMessage& logmsg ) throw (CORBA::SystemException)
  {
    controler->logReport ( logmsg );
  }

  /***********************************************************************/

  void RCControlerItf_i::ping() throw (CORBA::SystemException)
  {
  }
  
};
