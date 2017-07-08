//------------------------------------------------------------------------------
//
// AcquisitionORB.hh: generic class implementing CORBA calls for all entities of
//                                                             the EventDisplay.
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


#ifndef ACQUISITIONORB_HH_
#define ACQUISITIONORB_HH_

#ifdef __OMNIORB4__
  #include "omniORB4/CORBA.h"
#else
  #ifdef __OMNIORB3__
    #include "omniORB3/CORBA.h"
  #endif
#endif

namespace DaqApplication
{
 /**
 * AcquisitionORB: generic class managing CORBA calls for all entities of the EvenBuilder 
 */
 class AcquisitionORB
 {
  public:
	static AcquisitionORB *getInstance();
	static void freeInstance();
	~AcquisitionORB();
	bool readCorbaOption(char *configLine);
	void setUrlNameservice(char *url);
	void setClientCallTimeOutPeriod(unsigned ms);
	void setOutConScanPeriod(unsigned s);
	void setMaxGIOPConnectionPerServer(unsigned nb);
	void setOneCallPerConnection(bool b);
	void setOfferBiDirectionalGIOP(bool b);
	void setORBendPoint(char *);
	void setServerCallTimeOutPeriod(unsigned ms);
	void setInConScanPeriod(unsigned s);
	void setThreadPerConnectionPolicy(bool b);
	void setMaxServerThreadPerConnection(unsigned nb);
	void setMaxServerThreadPoolSize(unsigned nb);
	void setThreadPerConnectionUpperLimit(unsigned nb);
	void setThreadPerConnectionLowerLimit(unsigned nb);
	void setThreadPoolWatchConnection(bool b);
	void setAcceptBiDirectionalGIOP(bool b);
	void setPoaHoldRequestTimeout(unsigned ms);
	void setTraceLevel(int level);
	
 	CORBA::Boolean initORB();
 	PortableServer::POA_ptr getPOA();
 	CosNaming::NamingContext_ptr getRootContext();
	CORBA::Boolean bindObjectToName(CORBA::Object_ptr objref, const char* refName);
	CORBA::Boolean unbindRefName(const char* refName);
	CORBA::Object_ptr getObjectReference(const char* refName, const char* type = "Object");
 	CORBA::Boolean activatePOAMgr(PortableServer::POA_ptr);
	CORBA::Boolean deactivatePOAMgr(PortableServer::POA_ptr);
	//CORBA::Object_ptr publishObject(PortableServer::ServantBase*,PortableServer::POA_ptr);
 	void orbRun();
 	void orbShutdown();
  
  protected:
	AcquisitionORB();
	
  private:
  	static AcquisitionORB *theUniqueAcquisitionORB;
        CosNaming::NamingContext_ptr rootContext;
	CORBA::ORB_ptr orb;
	int debugTraceLevel;
	char *urlNameservice;
	char *urlORBendPoint;
	unsigned *clientCallTimeOutPeriod;
	unsigned *outConScanPeriod;
	unsigned *maxGIOPConnectionPerServer;
	bool *oneCallPerConnection;
	bool *offerBiDirectionalGIOP;

	unsigned *serverCallTimeOutPeriod;
	unsigned *inConScanPeriod;
	bool *threadPerConnectionPolicy;
	unsigned *maxServerThreadPerConnection;
	unsigned *maxServerThreadPoolSize;
	unsigned *threadPerConnectionUpperLimit;
	unsigned *threadPerConnectionLowerLimit;
	bool *threadPoolWatchConnection;
	bool *acceptBiDirectionalGIOP;
	unsigned *poaHoldRequestTimeout;
 };
};
#endif
