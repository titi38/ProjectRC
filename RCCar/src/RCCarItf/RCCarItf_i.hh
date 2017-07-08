//------------------------------------------------------------------------------
//
// RCCarItf_i.hh : Implementation of the interface
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


#ifndef RCCARITF_I_HH_
#define RCCARITF_I_HH_

#include "RCCarItf.hh"
#include "RCCar.hh"

namespace ProjectRC
{
  class RCCar;

  class RCCarItf_i : 	public POA_ProjectRC::RCCarItf,
			public PortableServer::RefCountServantBase
  {
    private:
      RCCar *rcCar;

    public:
      RCCarItf_i( RCCar* s );
      virtual ~RCCarItf_i() { };

      virtual void disconnect() throw (CORBA::SystemException);
      virtual void turnRight(::CORBA::UShort percent) throw (CORBA::SystemException, FailException);
      virtual void turnLeft(::CORBA::UShort percent) throw (CORBA::SystemException, FailException);
      virtual void runReverse(::CORBA::UShort percent) throw (CORBA::SystemException, FailException);
      virtual void runForward(::CORBA::UShort percent) throw (CORBA::SystemException, FailException);

  };
};

#endif
