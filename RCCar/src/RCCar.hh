//------------------------------------------------------------------------------
//
// RCCar.hh : the sensor's process of the EventBuilder
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

#ifndef RCCAR_HH_
#define RCCAR_HH_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for sleep()

#include "AcquisitionORB/AcquisitionORB.hh"
#include "AcquisitionORB/configORB.hh"

#include "RCControlerItf.hh"
#include "RCCarItf_i.hh"

#include "CarControl.hh"



namespace ProjectRC
{
  class RCCarItf_i;

  class RCCar
  {
      CarControl carControl;


    public:
      RCCar(const char *configFile);
      ~RCCar();

      void disconnect();
      void shutdown();
      void turnRight(::CORBA::UShort percent) throw (FailException);
      void turnLeft(::CORBA::UShort percent) throw (FailException);
      void runReverse(::CORBA::UShort percent) throw (FailException);
      void runForward(::CORBA::UShort percent) throw (FailException);

      static void reportToDaq( const LogKind kind, const char *msg );


    private:

      bool initialize();
      void getDaqObjRefFromNS();
      void connectTheDaq();
      static bool connectedToDaq;
      static bool rcCarIsShutingdown;
      char *refNameRCControlerServant;
      static RCControlerItf_ptr rcControlerItfRef;
      DaqApplication::AcquisitionORB *acquisitionOrb;

      static RCCarItf_ptr rcCarItfRef;
      RCCarItf_i* rcCarItfServant;

      static void* startingThreadConnectionManager(void *s);
      void processConnectionManagerThread ();
      pthread_t thread_connectManager;
      pthread_mutex_t p_mut_connectManager;
      pthread_cond_t p_cond_connectManager;


  };
};

#endif
