//------------------------------------------------------------------------------
//
// RCControler.hh : the sensor's process of the EventBuilder
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

#ifndef RCControler_HH_
#define RCControler_HH_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // for sleep()

#include "AcquisitionORB/AcquisitionORB.hh"
#include "AcquisitionORB/configORB.hh"

#include "RCControlerItf.hh"
#include "RCControlerItf_i.hh"

#include "joystick.hh"


namespace ProjectRC
{
  class RCControlerItf_i;

  class RCControler
  {


    public:
      RCControler(const char *configFile);
      ~RCControler();

     void connectCar(RCCarItf_ptr);
     void logReport( const LogMessage& logmsg );

    private:
      volatile bool exiting;
      volatile RCCarItf_ptr rcCarItf;

      Joystick *joystick;

      bool publishCorbaItf ( void );
      bool corbaInit();
      int registerObjRefToNameservice(void);
      void unregisterObjRefsToNameservice(void);
      void shutdown();
      bool initJoystick();
      void runJoystick();

      void disconnectCar();

      char *refNameRCControlerServant;

      static RCControlerItf_ptr rcControlerItfRef;
      DaqApplication::AcquisitionORB *acquisitionOrb;

      static RCControlerItf_ptr RCControlerItfRef;
      RCControlerItf_i* rcControlerServant;
      CORBA::Object_ptr RCControlerObjRef;

      static void* startingThreadJoystick(void *s);


  };
};

#endif
