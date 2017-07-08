//------------------------------------------------------------------------------
//
// RCCarItf_i.cc : Implementation of the interface
//
//------------------------------------------------------------------------------
//
// AUTHOR      : T.DESCOMBES (descombt@ipnl.in2p3.fr)
// PROJECT     : ProjectRC
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

#include "RCCarItf_i.hh"


namespace ProjectRC
{
	
  RCCarItf_i::RCCarItf_i( RCCar* s )
  {
    rcCar=s;
  }

  /***********************************************************************/

  void RCCarItf_i::disconnect() throw (CORBA::SystemException)
  {
    rcCar->disconnect();
  }

  /***********************************************************************/

  void RCCarItf_i::turnRight(::CORBA::UShort percent) throw (CORBA::SystemException, FailException)
  {
    rcCar->turnRight(percent);
  }

  /***********************************************************************/

  void RCCarItf_i::turnLeft(::CORBA::UShort percent) throw (CORBA::SystemException, FailException)
  {
    rcCar->turnLeft(percent);
  }

  /***********************************************************************/

  void RCCarItf_i::runReverse(::CORBA::UShort percent) throw (CORBA::SystemException, FailException)
  {
    rcCar->runReverse(percent);
  }

  /***********************************************************************/

  void RCCarItf_i::runForward(::CORBA::UShort percent) throw (CORBA::SystemException, FailException)
  {
    rcCar->runForward(percent);
  }

  /***********************************************************************/

};
