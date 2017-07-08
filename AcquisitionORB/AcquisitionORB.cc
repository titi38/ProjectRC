//------------------------------------------------------------------------------
//
// AcquisitionORB.cc: generic class implementing CORBA calls for all entities of
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

#include <stdio.h>
#include <stdlib.h>

#include "AcquisitionORB.hh"
#include "configORB.hh"


namespace DaqApplication
{
/**
* theUniqueAcquisitionORB: unique AcquisitionORB Object of the EventBuilder 
*/
AcquisitionORB * AcquisitionORB::theUniqueAcquisitionORB = NULL;

/*******************************************************************************/
/**
* getInstance: return theUniqueAcquisitionORB, create it if NULL
* \return AcquisitionORB - AcquisitionORB Object of the EventBuilder
*/
AcquisitionORB * AcquisitionORB::getInstance()
{
	if (theUniqueAcquisitionORB == NULL)
		theUniqueAcquisitionORB = new AcquisitionORB();
		
	return (theUniqueAcquisitionORB);
}

/*******************************************************************************/
/**
* freeInstance: free theUniqueAcquisitionORB
*/
void AcquisitionORB::freeInstance()
{
	if (theUniqueAcquisitionORB != NULL)
		delete theUniqueAcquisitionORB;
	theUniqueAcquisitionORB=NULL;	
}

/*******************************************************************************/
/**
* AcquisitionORB: base constructor, initialisation of all options to NULL
*/
AcquisitionORB::AcquisitionORB()
{
	urlNameservice=NULL;
	
	debugTraceLevel=0;
	
	//Client side options:
	clientCallTimeOutPeriod=NULL;
	outConScanPeriod=NULL;
	maxGIOPConnectionPerServer=NULL;
	oneCallPerConnection=NULL;
	offerBiDirectionalGIOP=NULL;
	
	//Server side options:	
	urlORBendPoint=NULL;
	serverCallTimeOutPeriod=NULL;
	inConScanPeriod=NULL;
	threadPerConnectionPolicy=NULL;
	maxServerThreadPerConnection=NULL;
	maxServerThreadPoolSize=NULL;
	threadPerConnectionUpperLimit=NULL;
	threadPerConnectionLowerLimit=NULL;
	threadPoolWatchConnection=NULL;
	acceptBiDirectionalGIOP=NULL;
	poaHoldRequestTimeout=NULL;
	
	orb=CORBA::ORB::_nil();
	rootContext=CosNaming::NamingContext::_nil();
}
/*******************************************************************************/
/**
* ~AcquisitionORB: base destructor, free all variables
*/
AcquisitionORB::~AcquisitionORB()
{
	if (urlNameservice!=NULL)
	  delete urlNameservice;

	if (clientCallTimeOutPeriod!=NULL)
	  delete clientCallTimeOutPeriod;
	if (outConScanPeriod!=NULL)
	  delete outConScanPeriod;
	if (maxGIOPConnectionPerServer!=NULL)
	  delete maxGIOPConnectionPerServer;
	if (oneCallPerConnection!=NULL)
	  delete oneCallPerConnection;
	if (offerBiDirectionalGIOP!=NULL)
	  delete offerBiDirectionalGIOP;

	if (serverCallTimeOutPeriod!=NULL)
	  delete serverCallTimeOutPeriod;
	if (inConScanPeriod!=NULL)
	  delete inConScanPeriod;
	if (threadPerConnectionPolicy!=NULL)
	  delete threadPerConnectionPolicy;
	if (maxServerThreadPerConnection!=NULL)
	  delete maxServerThreadPerConnection;
	if (maxServerThreadPoolSize!=NULL)
	  delete maxServerThreadPoolSize;
	if (threadPerConnectionUpperLimit!=NULL)
	  delete threadPerConnectionUpperLimit;
	if (threadPerConnectionLowerLimit!=NULL)
	  delete threadPerConnectionLowerLimit;
	if (threadPoolWatchConnection!=NULL)
	  delete threadPoolWatchConnection;
	if (acceptBiDirectionalGIOP!=NULL)
	  delete acceptBiDirectionalGIOP;
	if (poaHoldRequestTimeout!=NULL)
	  delete poaHoldRequestTimeout;
	if (urlORBendPoint!=NULL)
	  delete urlORBendPoint;

}

/*******************************************************************************/
/**
* setTraceLevel: set the Trace level to the given int value
* \param level - trace level
*/
void AcquisitionORB::setTraceLevel(int level)
{
	debugTraceLevel=level;
}

/*******************************************************************************/
/**
* setUrlNameService: set the Url of the CORBA NameService to the given string
* \param url - nameservice url
*/
void AcquisitionORB::setUrlNameservice(char *url)
{
	if (urlNameservice!=NULL)
	  delete urlNameservice;
	urlNameservice=(char*)malloc((strlen(url)+1)*sizeof(char));
	strcpy(urlNameservice,url);
}

/*******************************************************************************/
/**
* setClientCallTimeOutPeriod: set the option to the given int value
* \param ms - ClientCallTimeOutPeriod
*/
void AcquisitionORB::setClientCallTimeOutPeriod(unsigned ms)
{
	if (clientCallTimeOutPeriod==NULL)
	  clientCallTimeOutPeriod=new unsigned;
	  
	*clientCallTimeOutPeriod=ms;
}
/**
* setOutConScanPeriod: set the option to the given int value
* \param s - OutConScanPeriod
*/
void AcquisitionORB::setOutConScanPeriod(unsigned s)
{
	if (outConScanPeriod==NULL)
	  outConScanPeriod=new unsigned;
	  
	*outConScanPeriod=s;
}
/**
* setMaxGIOPConnectionPerServer: set the option to the given int value
* \param nb - MaxGIOPConnectionPerServer
*/
void AcquisitionORB::setMaxGIOPConnectionPerServer(unsigned nb)
{
	if (maxGIOPConnectionPerServer==NULL)
	  maxGIOPConnectionPerServer=new unsigned;
	  
	*maxGIOPConnectionPerServer=nb;
}

/**
* setOneCallPerConnection: set the option to the given bool value
* \param b - OneCallPerConnection
*/
void AcquisitionORB::setOneCallPerConnection(bool b)
{
	if (oneCallPerConnection==NULL)
	  oneCallPerConnection=new bool;

	*oneCallPerConnection=b;
}

/**
* setOfferBiDirectionalGIOP: set the option to the given bool value
* \param b - OfferBiDirectionalGIOP
*/
void AcquisitionORB::setOfferBiDirectionalGIOP(bool b)
{
	if (offerBiDirectionalGIOP==NULL)
	  offerBiDirectionalGIOP=new bool;

	*offerBiDirectionalGIOP=b;
}

/*******************************************************************************/
/**
* setServerCallTimeOutPeriod: set the option to the given int value
* \param ms - ServerCallTimeOutPeriod
*/
void AcquisitionORB::setServerCallTimeOutPeriod(unsigned ms)
{
	if (serverCallTimeOutPeriod==NULL)
	  serverCallTimeOutPeriod=new unsigned;
	  
	*serverCallTimeOutPeriod=ms;
}

/**
* setInConScanPeriod: set the option to the given int value
* \param s - InConScanPeriod
*/
void AcquisitionORB::setInConScanPeriod(unsigned s)
{
	if (inConScanPeriod==NULL)
	  inConScanPeriod=new unsigned;
	  
	*inConScanPeriod=s;
}

/**
* setThreadPerConnectionPolicy: set the option to the given bool value
* \param b - ThreadPerConnectionPolicy
*/
void AcquisitionORB::setThreadPerConnectionPolicy(bool b)
{
	if (threadPerConnectionPolicy==NULL)
	  threadPerConnectionPolicy=new bool;

	*threadPerConnectionPolicy=b;
}

/**
* setMaxServerThreadPerConnection: set the option to the given int value
* \param nb - MaxServerThreadPerConnection
*/
void AcquisitionORB::setMaxServerThreadPerConnection(unsigned nb)
{
	if (maxServerThreadPerConnection==NULL)
	  maxServerThreadPerConnection=new unsigned;
	  
	*maxServerThreadPerConnection=nb;
}

/**
* setMaxServerThreadPoolSize: set the option to the given int value
* \param nb - MaxServerThreadPoolSize
*/
void AcquisitionORB::setMaxServerThreadPoolSize(unsigned nb)
{
	if (maxServerThreadPoolSize==NULL)
	  maxServerThreadPoolSize=new unsigned;
	  
	*maxServerThreadPoolSize=nb;
}

/**
* setThreadPerConnectionUpperLimit: set the option to the given int value
* \param nb - ThreadPerConnectionUpperLimit
*/
void AcquisitionORB::setThreadPerConnectionUpperLimit(unsigned nb)
{
	if (threadPerConnectionUpperLimit==NULL)
	  threadPerConnectionUpperLimit=new unsigned;
	  
	*threadPerConnectionUpperLimit=nb;
}

/**
* setThreadPerConnectionLowerLimit: set the option to the given int value
* \param nb - ThreadPerConnectionLowerLimit
*/
void AcquisitionORB::setThreadPerConnectionLowerLimit(unsigned nb)
{
	if (threadPerConnectionLowerLimit==NULL)
	  threadPerConnectionLowerLimit=new unsigned;
	  
	*threadPerConnectionLowerLimit=nb;
}

/**
* setThreadPoolWatchConnection: set the option to the given bool value
* \param b - ThreadPoolWatchConnection
*/
void AcquisitionORB::setThreadPoolWatchConnection(bool b)
{
	if (threadPoolWatchConnection==NULL)
	  threadPoolWatchConnection=new bool;

	*threadPoolWatchConnection=b;
}
/**
* setThreadPoolWatchConnection: set the option to the given bool value
* \param b - ThreadPoolWatchConnection
*/
void AcquisitionORB::setAcceptBiDirectionalGIOP(bool b)
{
	if (acceptBiDirectionalGIOP==NULL)
	  acceptBiDirectionalGIOP=new bool;

	*acceptBiDirectionalGIOP=b;
}

/**
* setPoaHoldRequestTimeout: set the option to the given int value
* \param ms - PoaHoldRequestTimeout
*/
void AcquisitionORB::setPoaHoldRequestTimeout(unsigned ms)
{
	if (poaHoldRequestTimeout==NULL)
	  poaHoldRequestTimeout=new unsigned;
	  
	*poaHoldRequestTimeout=ms;
}

/**
* setORBendPoint: set the option to the given string
* \param url - ORBendPoint
*/
void AcquisitionORB::setORBendPoint(char *url)
{
	if (urlORBendPoint!=NULL)
	  delete urlORBendPoint;
	urlORBendPoint=(char*)malloc((strlen(url)+1)*sizeof(char));
	strcpy(urlORBendPoint,url);
}

/*******************************************************************************/
/**
* initORB: initialisation of the CORBA orb with the given options
* \return bool - success/failure
*/
CORBA::Boolean AcquisitionORB::initORB()
{
  try
  {
    if (urlNameservice==NULL)
    {
      printf("\nERROR: the URL of the Nameservice is undefined\n");
      exit(EXIT_FAILURE);
    }

    static int nbopt=0;
    
    #ifdef __OMNIORB4__
      
    	char *options[25][2]; // 50 options maxi...
	
	for (int i=0; i<25; i++)
	  for (int j=0; j<2; j++)
    		options[i][j]=(char*)malloc(81*sizeof(char));

    	if (debugTraceLevel!=0)
	{
	  snprintf(options[nbopt][0] ,80, "traceLevel");
    	  snprintf(options[nbopt][1] ,80, "%d",debugTraceLevel);
	  nbopt++;
	}
	
    	if (urlNameservice!=NULL)
	{
	  snprintf(options[nbopt][0] ,80, "InitRef");
    	  snprintf(options[nbopt][1] ,80, "%s",urlNameservice);
	  nbopt++;
	}

	// Client Options (10)
	if (clientCallTimeOutPeriod!=NULL)
	{
    	  snprintf(options[nbopt][0] ,80, "clientCallTimeOutPeriod");
    	  snprintf(options[nbopt][1] ,80, "%u",*clientCallTimeOutPeriod);
	  nbopt++;
	}
	
	if (outConScanPeriod!=NULL)
      	{
	  snprintf(options[nbopt][0] ,80, "outConScanPeriod");
     	  snprintf(options[nbopt][1] ,80, "%u",*outConScanPeriod);
	  nbopt++;
	}
	
	if (maxGIOPConnectionPerServer!=NULL)
	{
     	  snprintf(options[nbopt][0] ,80, "maxGIOPConnectionPerServer");
    	  snprintf(options[nbopt][1] ,80, "%u",*maxGIOPConnectionPerServer);
	  nbopt++;
	}
	
	if (oneCallPerConnection!=NULL)
	{
     	  snprintf(options[nbopt][0] ,80, "oneCallPerConnection");
    	  snprintf(options[nbopt][1] ,80, "%u",*oneCallPerConnection);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "offerBiDirectionalGIOP");
    	  snprintf(options[nbopt][1] ,80, "%u",*offerBiDirectionalGIOP);
	  nbopt++;
	}

    	// Server Options (20)
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "serverCallTimeOutPeriod");
          snprintf(options[nbopt][1] ,80, "%u",*serverCallTimeOutPeriod);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "inConScanPeriod");
          snprintf(options[nbopt][1] ,80, "%u",*inConScanPeriod);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "threadPerConnectionPo{licy");
          snprintf(options[nbopt][1] ,80, "%u",*threadPerConnectionPolicy);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "maxServerThreadPerConnection");
          snprintf(options[nbopt][1] ,80, "%u",*maxServerThreadPerConnection);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "maxServerThreadPoolSize");
          snprintf(options[nbopt][1] ,80, "%u",*maxServerThreadPoolSize);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "threadPerConnectionUpperLimit");
          snprintf(options[nbopt][1] ,80, "%u",*threadPerConnectionUpperLimit);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "threadPerConnectionLowerLimit");
          snprintf(options[nbopt][1] ,80, "%u",*threadPerConnectionLowerLimit);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "threadPoolWatchConnection");
          snprintf(options[nbopt][1] ,80, "%u",*threadPoolWatchConnection);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "acceptBiDirectionalGIOP");
          snprintf(options[nbopt][1] ,80, "%u",*acceptBiDirectionalGIOP);
	  nbopt++;
	}
	
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt][0] ,80, "poaHoldRequestTimeout");
          snprintf(options[nbopt][1] ,80, "%u",*poaHoldRequestTimeout);
	  nbopt++;
	}
	
	if (urlORBendPoint!=NULL)
	{
	  snprintf(options[nbopt][0] ,80,"endPoint");
	  snprintf(options[nbopt][1] ,80,"%s",urlORBendPoint);
	  nbopt++;
	}

    	options[nbopt][0]=0;
    	options[nbopt][1]=0;  
    
    	int argc=0;
    	orb = CORBA::ORB_init(argc, NULL, ORB_NAME, (const char *(*)[2])options);

  #else
    
    #ifdef __OMNIORB3__
	const int maxNbOptions=33;
	char *options[maxNbOptions]; // 33 OPTIONS MAXI
		
	for (int i=0; i<=maxNbOptions-1; i++)
    		options[i]=(char*)malloc(81*sizeof(char));
	
	//snprintf(options[nbopt++] ,80*sizeof(char),"");
	options[nbopt++][0]='\0';
		
	if (urlNameservice!=NULL)
	{    	
    	  snprintf(options[nbopt++],80,"-ORBInitRef");
    	  snprintf(options[nbopt++],80,"%s",urlNameservice);
	}

	if (outConScanPeriod!=NULL)
      	{
	  snprintf(options[nbopt++] ,80, "-ORBoutConScanPeriod");
     	  snprintf(options[nbopt++] ,80, "%u",*outConScanPeriod);
	}
	if (offerBiDirectionalGIOP!=NULL)
	{
          snprintf(options[nbopt++] ,80, "-ORBinConScanPeriod");
          snprintf(options[nbopt++] ,80, "%u",*inConScanPeriod);
	}
    	if (debugTraceLevel!=0)
	{
	  snprintf(options[nbopt++] ,80, "-ORBtraceLevel");
    	  snprintf(options[nbopt++] ,80, "%d",debugTraceLevel);
	}
	if (clientCallTimeOutPeriod!=NULL)
	{	
    	  snprintf(options[nbopt++],80,"-ORBclientCallTimeOutPeriod");
    	  snprintf(options[nbopt++],80,"%u",*clientCallTimeOutPeriod/1000);
	}
	if (offerBiDirectionalGIOP!=NULL)
	{
    	  snprintf(options[nbopt++],80,"-ORBserverCallTimeOutPeriod");
    	  snprintf(options[nbopt++],80,"%u",*serverCallTimeOutPeriod);
	}
      	orb = CORBA::ORB_init(nbopt, (char **)options, ORB_NAME );
    #endif
  #endif   
  }
  catch(CORBA::COMM_FAILURE& ex)
  {
    printf("initORB: Caught system exception COMM_FAILURE -- unable to contact the naming service.\n");
    return 1;
  }
  catch(CORBA::SystemException&)
  {
    printf("initORB: Caught a CORBA::SystemException while using the naming service.\n");
    return 1;
  }

  return 0;
}

/*******************************************************************************/
/**
* getPOA: resolve release and return a CORBA Portable Object Adapter 
* \return POA_ptr - CORBA Portable Object Adapter, NULL if failure
*/
PortableServer::POA_ptr AcquisitionORB::getPOA()
{
  PortableServer::POA_ptr poa;
  
  try
  {
    CORBA::Object_ptr obj = orb->resolve_initial_references("RootPOA");
    poa = PortableServer::POA::_narrow(obj);
    CORBA::release(obj);
  }
  catch(CORBA::COMM_FAILURE& ex) 
  {
    printf("initPOA: Caught system exception COMM_FAILURE -- unable to get the POA.\n");
    return PortableServer::POA::_nil();
  }
  catch(CORBA::SystemException&)
  {
    printf("initPOA: Caught a CORBA::SystemException -- unable to get the POA.\n");
    return PortableServer::POA::_nil();
  }
  return poa;
}

/*******************************************************************************/
/**
* activatePOAMgr: activate the POA manager of the given poa
* \param  poa - POA pointer 
* \return bool - success/failure
*/
CORBA::Boolean AcquisitionORB::activatePOAMgr(PortableServer::POA_ptr poa)
{
  try
  {
    //Activate the POA Manager
    PortableServer::POAManager_ptr pman = poa->the_POAManager();
    pman->activate();
  }
  catch(CORBA::COMM_FAILURE& ex) 
  {
    printf("activatePOAMgr: Caught system exception COMM_FAILURE.\n");
    return 1;
  }
  catch(CORBA::SystemException&)
  {
    printf("activatePOAMgr: Caught a CORBA::SystemException.\n");
    return 1;
  }

  return 0;
}

/*******************************************************************************/
/**
* activatePOAMgr: activate the POA manager of the given poa
* \param  poa - POA pointer 
* \return bool - success/failure
*/
CORBA::Boolean AcquisitionORB::deactivatePOAMgr(PortableServer::POA_ptr poa)
{
  try
  {
    //Activate the POA Manager
    PortableServer::POAManager_ptr pman = poa->the_POAManager();
    pman->deactivate(false,false);
  }
  catch(CORBA::COMM_FAILURE& ex) 
  {
    printf("deactivatePOAMgr: Caught system exception COMM_FAILURE.\n");
    return 1;
  }
  catch(CORBA::SystemException&)
  {
    printf("deactivatePOAMgr: Caught a CORBA::SystemException.\n");
    return 1;
  }

  return 0;
}

/*******************************************************************************/
/**
* getRootContext: activate the POA manager of the given poa 
* \return NamingContext_ptr - NameService root context, NULL if failure
*/
CosNaming::NamingContext_ptr AcquisitionORB::getRootContext()
{
  CosNaming::NamingContext_ptr rootContext;
  
  try
  {
    // Obtain a reference to the root context of the Name service:
    CORBA::Object_ptr obj;
    obj = orb->resolve_initial_references("NameService");

    // Narrow the reference returned.
    rootContext = CosNaming::NamingContext::_narrow(obj);
    CORBA::release(obj);

    if( CORBA::is_nil(rootContext) ) 
    {
      printf("Failed to narrow the root naming context.\n");
      return CosNaming::NamingContext::_nil();
    }
  }
  catch(CORBA::ORB::InvalidName& ex)
  {
    // This should not happen!
    printf("getRootContext: Service required is invalid [does not exist].\n");
    return CosNaming::NamingContext::_nil();
  }
    catch(CORBA::COMM_FAILURE& ex) 
  {
    printf("getRootContext: Caught system exception COMM_FAILURE -- unable to contact the naming service.\n");
    return CosNaming::NamingContext::_nil();
  }
  catch(CORBA::SystemException&)
  {
    printf("getRootContext: Caught a CORBA::SystemException while using the naming service.\n");
    return CosNaming::NamingContext::_nil();
  }
  catch (...)
  {
    printf("getRootContext: Caught a unknown exception while using the naming service.\n");
    return CosNaming::NamingContext::_nil();
  }   
  
  return (rootContext);
}

/*******************************************************************************/
/**
* bindObjectToName: bind a given object to a given name in the root context of the NameService 
* \param objref - object reference to bind
* \param refName - associated name
* \return bool - success/failure
*/
CORBA::Boolean AcquisitionORB::bindObjectToName(CORBA::Object_ptr objref, const char* refName)
{
  if ( CORBA::is_nil(rootContext) ) 
  	rootContext=getRootContext();

  if ( CORBA::is_nil(rootContext) )
  	return 1;

  // Bind objref with name Echo to the testContext:
  CosNaming::Name objectName;
  objectName.length(1);
  objectName[0].id   = refName;
  objectName[0].kind = (const char*) "Object"; // string copied

  try 
  {
    rootContext->bind(objectName, objref);
  }
  catch(CosNaming::NamingContext::AlreadyBound& ex) 
  {
    rootContext->rebind(objectName, objref);
  }
    // Note: Using rebind() will overwrite any Object previously bound
    //       Alternatively, bind() can be used, which will raise a
    //       CosNaming::NamingContext::AlreadyBound exception if the name
    //       supplied is already bound to an object.

    // Amendment: When using OrbixNames, it is necessary to first try bind
    // and then rebind, as rebind on it's own will throw a NotFoundexception if
    // the Name has not already been bound. [This is incorrect behaviour -
    // it should just bind].

  catch(CORBA::COMM_FAILURE& ex) 
  {
    printf("bindObjectToName: Caught system exception COMM_FAILURE -- unable to contact the naming service.\n");
    return 1;
  }
  catch(CORBA::SystemException&)
  {
    printf("bindObjectToName: Caught a CORBA::SystemException while using the naming service.\n");
    return 1;
  }

  return 0;
}

/*******************************************************************************/
/**
* getObjectReference: return an object reference given its name  
* \param refName - object name to resolve
* \return Object_ptr - associated object in the root context of the NameService, NULL if failure
*/
CORBA::Object_ptr AcquisitionORB::getObjectReference(const char* refName, const char* type)
{
     
  if ( CORBA::is_nil(rootContext) )
        rootContext=getRootContext();

  if ( CORBA::is_nil(rootContext) ) return CORBA::Object::_nil();

  // Create a name object, containing the name test/context:
  CosNaming::Name name;
  name.length(1);

  name[0].id   = CORBA::string_dup(refName);
  name[0].kind = CORBA::string_dup(type);

  // Note on kind: The kind field is used to indicate the type
  // of the object. This is to avoid conventions such as that used
  // by files (name.type -- e.g. test.ps = postscript etc.)

  CORBA::Object_ptr objRefPtr;
  
  try
  {
    //Resolve the name to an object reference.
    objRefPtr = rootContext->resolve(name);
  }
  catch(CosNaming::NamingContext::NotFound& ex)
  {
    // This exception is thrown if any of the components of the
    // path [contexts or the object] aren't found:
    //printf("Context not found.\n");
    //fflush(NULL);
    objRefPtr = CORBA::Object::_nil();
  }
  catch(CORBA::COMM_FAILURE& ex)
  {
    printf("getObjectReference: Caught system exception COMM_FAILURE -- unable to contact the naming service.\n");
    fflush(NULL);
    objRefPtr = CORBA::Object::_nil();
  }
  catch(CORBA::SystemException&)
  {
    printf("getObjectReference: Caught a CORBA::SystemException while using the naming service.\n");
    fflush(NULL);
    objRefPtr = CORBA::Object::_nil();
  }
  catch(...)
  {
    printf("getObjectReference: Caught a Unknown Exception while using the naming service.\n");
    fflush(NULL);
    objRefPtr = CORBA::Object::_nil();
  }

    return objRefPtr;
}

/*******************************************************************************/
/**
* unbindRefName: remove the given reference from the NameService root context 
* \param refName - reference to be removed
* \return bool - success/failure
*/
CORBA::Boolean AcquisitionORB::unbindRefName(const char* refName)
{
  if ( CORBA::is_nil(rootContext) ) 
  	rootContext=getRootContext();

  if ( CORBA::is_nil(rootContext) )
  	return 1;

  // Unbind objref with name Echo to the testContext:
  CosNaming::Name objectName;
  objectName.length(1);
  objectName[0].id   = refName;
  objectName[0].kind = (const char*) "Object"; // string copied

  try 
  {
    rootContext->unbind(objectName);
   
  }
  catch(CosNaming::NamingContext::NotFound& ex)
  {
    // This exception is thrown if any of the components of the
    // path [contexts or the object] aren't found:
    printf("unbindRefName: Caught a CosNaming::NamingContext::NotFound -- Context not found.\n");
    fflush(NULL);
    return 1;
  }
  catch(CORBA::COMM_FAILURE& ex) 
  {
    printf("unbindRefName: Caught system exception COMM_FAILURE -- unable to contact the naming service.\n");
    fflush(NULL);
    return 1;
  }
  catch(CORBA::SystemException&)
  {
    printf("unbindRefName: Caught a CORBA::SystemException while using the naming service.\n");
    fflush(NULL);
    return 1;
  }
  catch(...)
  {
    printf("unbindRefName: Caught an unknown exception.\n");
    fflush(NULL);
    return 1;
  }

  return 0;
}

/*******************************************************************************/
/**
* publishObject: publish a servant with the given poa 
* \param servant - object to be published
* \param poa - Portable Object Adapter
* \return Object_ptr - associated object pointer
*/
/*CORBA::Object_ptr AcquisitionORB::publishObject(PortableServer::ServantBase* servant,PortableServer::POA_ptr poa)
{
  PortableServer::ObjectId_ptr Id = poa->activate_object(servant);
  CORBA::Object_ptr objRef =  servant->_this();
  servant->_remove_ref();
  return objRef;
}*/

/*******************************************************************************/
/**
* orbRun: start the orb
*/
void AcquisitionORB::orbRun()
{
	orb->run();
}

/*******************************************************************************/
/**
* orbRun: shutdown the orb
*/
void AcquisitionORB::orbShutdown()
{
	orb->shutdown(false);
}

/*******************************************************************************/
/**
* readCorbaOption: read CORBA options from the given string 
* \param configLine - option string
* \return bool - sucess/failure(not a known option)
*/
bool AcquisitionORB::readCorbaOption(char *configLine)
{
	bool result=false;
	char *value;
        
     	/* looks for the line containing "NameService=" */
	if ( (value = strstr(configLine,"NameService=")) != NULL )
	{
	  
	  int len=strlen(value);
	  
      	  while (value[len-1]=='\n' || value[len-1]==' ')
	  {
	    len--;
	  }

	  char *urlNameservice;
	  urlNameservice=(char*)malloc((len+1)*sizeof(char));	  
  
	  strncpy(urlNameservice,value,len );
	  *(urlNameservice+len)='\0';
	  
	  printf("* Set %s\n",urlNameservice);
	  setUrlNameservice(urlNameservice);
	  free(urlNameservice);
	  result=true;
	}

	// CORBA DEBUG OPTION
	
         /* looks for the line containing "ORBofferBiDirectionalGIOP=" */
         if ( (value = strstr(configLine,"ORBtraceLevel=")) != NULL )
         {
           int traceLevel = atoi(value+strlen("ORBtraceLevel="));
	   
	   if (traceLevel >= 0 || traceLevel <= 25)
	   {
             printf("* Set ORBtraceLevel to %d\n",traceLevel);
             setTraceLevel(traceLevel);
           }
           else
           {
             printf("\nERROR: Bad Value of traceLevel\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }


	 /************************/
      	 /* CORBA Client Options */
      	 /************************/
      	 
         /* looks for the line containing "ORBclientCallTimeOutPeriod=" */
         if ( (value = strstr(configLine,"ORBclientCallTimeOutPeriod=")) != NULL )
         {
           int clientCallTimeOutPeriod = atoi(value+strlen("ORBclientCallTimeOutPeriod="));
	   	   
	   if (clientCallTimeOutPeriod>=0)
	   {
             printf("* Set ORBclientCallTimeOutPeriod to %ds\n",clientCallTimeOutPeriod);
             setClientCallTimeOutPeriod(clientCallTimeOutPeriod);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBclientCallTimeOutPeriod\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }

         /* looks for the line containing "ORBoutConScanPeriod=" */
         if ( (value = strstr(configLine,"ORBoutConScanPeriod=")) != NULL )
         {
           int outConScanPeriod = atoi(value+strlen("ORBoutConScanPeriod="));
	   	   
	   if (outConScanPeriod>=0)
	   {
             printf("* Set ORBoutConScanPeriod to %dms\n",outConScanPeriod);
             setOutConScanPeriod(outConScanPeriod);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBoutConScanPeriod\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;  
         }         
                  
         /* looks for the line containing "ORBmaxGIOPConnectionPerServer=" */
         if ( (value = strstr(configLine,"ORBmaxGIOPConnectionPerServer=")) != NULL )
         {
           int maxGIOPConnectionPerServer = atoi(value+strlen("ORBmaxGIOPConnectionPerServer="));
	   	   
	   if (maxGIOPConnectionPerServer > 0)
	   {
             printf("* Set ORBmaxGIOPConnectionPerServer to %d\n",maxGIOPConnectionPerServer);
             setMaxGIOPConnectionPerServer(maxGIOPConnectionPerServer);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBmaxGIOPConnectionPerServer\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }
         
         /* looks for the line containing "ORBoneCallPerConnection=" */
         if ( (value = strstr(configLine,"ORBoneCallPerConnection=")) != NULL )
         {
           int oneCallPerConnection = atoi(value+strlen("ORBoneCallPerConnection="));
	   
	   if (oneCallPerConnection == 0 || oneCallPerConnection == 1)
	   {
	     bool bparam=(oneCallPerConnection==1);
             printf("* Set ORBoneCallPerConnection to %d\n",oneCallPerConnection);
             setOneCallPerConnection(bparam);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBoneCallPerConnection\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }
         
         /* looks for the line containing "ORBofferBiDirectionalGIOP=" */
         if ( (value = strstr(configLine,"ORBofferBiDirectionalGIOP=")) != NULL )
         {
           int offerBiDirectionalGIOP = atoi(value+strlen("ORBofferBiDirectionalGIOP="));
	   
	   if (offerBiDirectionalGIOP == 0 || offerBiDirectionalGIOP == 1)
	   {
	     bool bparam=(offerBiDirectionalGIOP==1);
             printf("* Set ORBofferBiDirectionalGIOP to %d\n",offerBiDirectionalGIOP);
             setOfferBiDirectionalGIOP(bparam);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBofferBiDirectionalGIOP\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }

	 /************************/
      	 /* CORBA Server Options */
      	 /************************/

     	/* looks for the line containing "ORBendPoint=" */
	if ( (value = strstr(configLine,"ORBendPoint=")) != NULL )
	{
	  char *new_value;
	  new_value=value+strlen("ORBendPoint=");

	  int len=strlen(new_value);
      	  while (new_value[len-1]=='\n' || new_value[len-1]==' ')
	  {
	    len--;
	  }
	 
	  char *urlORBendPoint;
	  urlORBendPoint=(char*)malloc((len+1)*sizeof(char));	  
  
	  strncpy(urlORBendPoint,new_value,len );
	  *(urlORBendPoint+len)='\0';
	  
	  printf("* Set ORBendPoint=%s\n",urlORBendPoint);
	  setORBendPoint(urlORBendPoint);
	  free(urlORBendPoint);
	  result=true;
	}

         
         /* looks for the line containing "ORBserverCallTimeOutPeriod=" */
         if ( (value = strstr(configLine,"ORBserverCallTimeOutPeriod=")) != NULL )
         {

           int serverCallTimeOutPeriod = atoi(value+strlen("ORBserverCallTimeOutPeriod="));
	   	   
	   if (serverCallTimeOutPeriod>=0)
	   {
             printf("* Set ORBserverCallTimeOutPeriod to %dms\n",serverCallTimeOutPeriod);
             setServerCallTimeOutPeriod(serverCallTimeOutPeriod);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBserverCallTimeOutPeriod\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }

         /* looks for the line containing "ORBinConScanPeriod=" */
         if ( (value = strstr(configLine,"ORBinConScanPeriod=")) != NULL )
         {

           int inConScanPeriod = atoi(value+strlen("ORBinConScanPeriod="));
	   	   
	   if (inConScanPeriod>=0)
	   {
             printf("* Set ORBinConScanPeriod to %ds\n",inConScanPeriod);
             setInConScanPeriod(inConScanPeriod);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBinConScanPeriod\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }
         
         /* looks for the line containing "ORBthreadPerConnectionPolicy=" */
         if ( (value = strstr(configLine,"ORBthreadPerConnectionPolicy=")) != NULL )
         {
           int threadPerConnectionPolicy = atoi(value+strlen("ORBthreadPerConnectionPolicy="));
	   
	   if (threadPerConnectionPolicy == 0 || threadPerConnectionPolicy == 1)
	   {
	     bool bparam=(threadPerConnectionPolicy==1);
             printf("* Set ORBthreadPerConnectionPolicy to %d\n",threadPerConnectionPolicy);
             setThreadPerConnectionPolicy(bparam);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBthreadPerConnectionPolicy\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }
         
         /* looks for the line containing "ORBmaxServerThreadPerConnection=" */
         if ( (value = strstr(configLine,"ORBmaxServerThreadPerConnection=")) != NULL )
         {

           int maxServerThreadPerConnection = atoi(value+strlen("ORBmaxServerThreadPerConnection="));
	   	   
	   if (maxServerThreadPerConnection>=0)
	   {
             printf("* Set ORBmaxServerThreadPerConnection to %d\n",maxServerThreadPerConnection);
             setMaxServerThreadPerConnection(maxServerThreadPerConnection);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBmaxServerThreadPerConnection\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }
         
         /* looks for the line containing "ORBmaxServerThreadPoolSize=" */
         if ( (value = strstr(configLine,"ORBmaxServerThreadPoolSize=")) != NULL )
         {

           int maxServerThreadPoolSize = atoi(value+strlen("ORBmaxServerThreadPoolSize="));
	   	   
	   if (maxServerThreadPoolSize>=0)
	   {
             printf("* Set ORBmaxServerThreadPoolSize to %d\n",maxServerThreadPoolSize);
             setMaxServerThreadPoolSize(maxServerThreadPoolSize);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBmaxServerThreadPoolSize\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }

         /* looks for the line containing "ORBthreadPerConnectionUpperLimit=" */
         if ( (value = strstr(configLine,"ORBthreadPerConnectionUpperLimit=")) != NULL )
         {

           int threadPerConnectionUpperLimit = atoi(value+strlen("ORBthreadPerConnectionUpperLimit="));
	   	   
	   if (threadPerConnectionUpperLimit>=0)
	   {
             printf("* Set ORBthreadPerConnectionUpperLimit to %d\n",threadPerConnectionUpperLimit);
             setThreadPerConnectionUpperLimit(threadPerConnectionUpperLimit);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBthreadPerConnectionUpperLimit\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }
         
         /* looks for the line containing "ORBthreadPerConnectionLowerLimit=" */
         if ( (value = strstr(configLine,"ORBthreadPerConnectionLowerLimit=")) != NULL )
         {

           int threadPerConnectionLowerLimit = atoi(value+strlen("ORBthreadPerConnectionLowerLimit="));
	   	   
	   if (threadPerConnectionLowerLimit>=0)
	   {
             printf("* Set ORBthreadPerConnectionLowerLimit to %d\n",threadPerConnectionLowerLimit);
             setThreadPerConnectionLowerLimit(threadPerConnectionLowerLimit);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBthreadPerConnectionLowerLimit\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }

         /* looks for the line containing "ORBthreadPoolWatchConnection=" */
         if ( (value = strstr(configLine,"ORBthreadPoolWatchConnection=")) != NULL )
         {
           int threadPoolWatchConnection = atoi(value+strlen("ORBthreadPoolWatchConnection="));
	   
	   if (threadPoolWatchConnection == 0 || threadPoolWatchConnection == 1)
	   {
	     bool bparam=(threadPoolWatchConnection==1);
             printf("* Set ORBthreadPoolWatchConnection to %d\n",threadPoolWatchConnection);
             setThreadPoolWatchConnection(bparam);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBthreadPoolWatchConnection\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }

         /* looks for the line containing "ORBacceptBiDirectionalGIOP=" */
         if ( (value = strstr(configLine,"ORBacceptBiDirectionalGIOP=")) != NULL )
         {
           int acceptBiDirectionalGIOP = atoi(value+strlen("ORBacceptBiDirectionalGIOP="));
	   
	   if (acceptBiDirectionalGIOP == 0 || acceptBiDirectionalGIOP == 1)
	   {
	     bool bparam=(acceptBiDirectionalGIOP==1);
             printf("* Set ORBacceptBiDirectionalGIOP to %d\n",acceptBiDirectionalGIOP);
             setAcceptBiDirectionalGIOP(bparam);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBacceptBiDirectionalGIOP\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }

         /* looks for the line containing "ORBpoaHoldRequestTimeout=" */
         if ( (value = strstr(configLine,"ORBpoaHoldRequestTimeout=")) != NULL )
         {

           int poaHoldRequestTimeout = atoi(value+strlen("ORBpoaHoldRequestTimeout="));
	   	   
	   if (poaHoldRequestTimeout>=0)
	   {
             printf("* Set ORBpoaHoldRequestTimeout to %dms\n",poaHoldRequestTimeout);
             setPoaHoldRequestTimeout(poaHoldRequestTimeout);
           }
           else
           {
             printf("\nERROR: Bad Value of ORBpoaHoldRequestTimeout\n");
             exit(EXIT_FAILURE);
	   }
	   result=true;
         }
         return(result);
}

};
