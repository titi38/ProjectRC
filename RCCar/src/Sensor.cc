//------------------------------------------------------------------------------
//
// Sensor.cc : the sensor's process of the EventBuilder
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

#ifdef __OMNIORB4__
#include "omniORB4/CORBA.h"
#else
#ifdef __OMNIORB3__
#include "omniORB3/CORBA.h"
#endif
#endif

extern "C"
{
int ftime ( struct timeb *tp );
}

#include <time.h>
#include <sys/timeb.h>
#include <sys/time.h>
#include <errno.h>
#include <iostream>

#include "Sensor.hh"
#include "LogType.hh"

#define MAX_SEQ_NUMBER 10000


#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


using namespace std;

Diaphane::Sensor *s;

bool Diaphane::Sensor::connectedToDaq = false;
Diaphane::Sensor::AcquisitionParametersType Diaphane::Sensor::acquisitionParameters = { UNINITIALIZED, 0, 0 };
Diaphane::coord Diaphane::Sensor::coordSensor = { 0, 0, 0 };


/************************************************************************/

void printHelp ( const char *execname )
{
  printf ("Usage:\n");
  printf ("  %s [OPTION...] X Y Z\n\n", execname);
  printf ("Options:\n");
  printf ("  -h \t\t Show help options\n");
  printf ("  -s \t\t quiet mode\n");
//    printf("  -f \t\t no hardware mode - for test purpose only\n");
  printf ("  -C FILE\t Use given configuration file\n");
  printf ("  -c VALUE\t set a coordStatus\n");
  printf ("  -i VALUE\t set a fpgaInitStatus\n\n");
  printf ("Parameters:\n");
  printf ("  X \tcoord_X\n");
  printf ("  Y \tcoord_Y\n");
  printf ("  Z \tcoord_Z\n\n");

  fflush (NULL);
}

int main ( int argc, char **argv )
{
  int c;

  Diaphane::coord coordSensor;
  Diaphane::coordStatusType coordStatus = Diaphane::OK;
  unsigned char fpgaInitStatus = 0;
//  bool quietMode=false;
//  bool fakeHwMode=false;
  char configFile[300] = "/var/tmp/mnt/TT/sensor.conf";

  char title[40], decotitle[40];
  snprintf (title, 40, "Data Acquisition Software v%s", SENSOR_SOFTWARE_VERSION);
  memset (decotitle, '*', strlen (title) + 4);
  decotitle[ strlen (title) + 4 ] = '\0';
  printf ("\n%s\n* %s *\n%s\n", decotitle, title, decotitle);
  printf ("Copyright 2017 by T.Descombes <thierry.descombes@gmail.com>\n\n");

  if ( argc < 4 )
  {
    printHelp (argv[ 0 ]);
    exit (0);
  }
  char *endptr, *str;

  str = *( argv + argc - 3 );
  coordSensor.x = (CORBA::Octet) strtol( str, &endptr, 10);
  if ( errno  || str == endptr
     || ( coordSensor.x > (CORBA::Octet) Diaphane::SENSOR_X_MAX )
     || ( coordSensor.x < (CORBA::Octet) Diaphane::SENSOR_X_MIN ) )
  {
    fprintf (stderr, "ERROR: Invalid x coordinates\n");
    exit (EXIT_FAILURE);
  }

  str = *( argv + argc - 2 );
  coordSensor.y = (CORBA::Octet) strtol( str, &endptr, 10);
  if ( errno  || str == endptr
         || ( coordSensor.y > (CORBA::Octet) Diaphane::SENSOR_Y_MAX )
         || ( coordSensor.y < (CORBA::Octet) Diaphane::SENSOR_Y_MIN ) )
  {
    fprintf (stderr, "ERROR: Invalid y coordinates\n");
    exit (EXIT_FAILURE);
  }

  str = *( argv + argc - 1 );
  coordSensor.nPlan = (Diaphane::NumPlan)strtol( str, &endptr, 10);
  if ( errno  || str == endptr
       || ( coordSensor.nPlan > (CORBA::Octet) Diaphane::SENSOR_NUM_PLAN_MAX )
       || ( coordSensor.nPlan < (Diaphane::NumPlan) Diaphane::SENSOR_NUM_PLAN_MIN ) )
  {
    fprintf (stderr, "ERROR: Invalid numPlan coordinates\n");
    exit (EXIT_FAILURE);
  }

  while (( c = getopt (argc - 3, argv, "hicC:")) != -1 )
  {
    switch ( c )
    {
      case 'h':
        printHelp (argv[ 0 ]);
        exit (0);
/*
      case 's':
        quietMode=true;
        break;
*/
      case 'C':
        snprintf (configFile, sizeof configFile, "%s", optarg);
        break;

/*    case 'f':
        fakeHwMode=true;
        break;
*/
      case 'c':
        if ( strcmp (optarg, "ERROR") == 0 ) coordStatus = Diaphane::BAD;
        if ( strcmp (optarg, "OUT_OF_RANGE") == 0 ) coordStatus = Diaphane::OUT_OF_RANGE;
        if ( coordStatus != Diaphane::OK && strcmp (optarg, "OK") != 0 )
        {
          fprintf (stderr, "Invalid coordStatus value '%s' (should be ERROR, OUT_OF_RANGE, or OK)\n", optarg);
          exit(EXIT_FAILURE);
        }
        break;

      case 'i':
        fpgaInitStatus = atoi (optarg);
        break;

      default:
        fprintf (stderr, "Invalid option: run '%s -h' to see a full list of available command line options\n", argv[ 0 ]);
        exit(EXIT_FAILURE);
    }
  }


    Diaphane::Sensor (coordSensor, configFile, coordStatus, fpgaInitStatus /*, quietMode, fakeHwMode*/);

  return 0;
}

/************************************************************************/

namespace Diaphane
{
  SensorHardwareParamType Sensor::sensorHardwareParam = { };

  DaqToSensorItf_ptr Sensor::daqToSensorObjRef = NULL;
  SensorToDaqItf_ptr Sensor::sensorToDaqItfRef = NULL;

  // Reduction des log par une "barre de progression" pour la connexion CORBA

  Sensor::~Sensor ()
  {
    delete calibrationProcessing;
    delete hardwareDataFifo;
  }

  /***********************************************************************/
  /* Sensor(): Le constructeur de la classe                              */
  /***********************************************************************/
  Sensor::Sensor ( const Diaphane::coord &c, const char *configFile, coordStatusType cStatus,
                   unsigned char fStatus /*, const bool quiet, const bool fake*/) :
    coordStatus (cStatus), fpgaInitStatus (fStatus),  /*fakeHwMode(fake), quietMode(quiet),*/
    sensorMaxSeqNumber (MAX_SEQ_NUMBER)
  {
    coordSensor = c;
    numSuperModule = 0;

    // acquisitionOrb est l'instance unique de la classe d'accès à l'ORB
    acquisitionOrb = DaqApplication::AcquisitionORB::getInstance ();

    //AandBModeEnb = false;
    //BusyModeEnb = false;


    /********************************************************************/
    // Lecture du fichier de conf
    /********************************************************************/

    FILE *configfile;
    char configLine[120];
    int ligne = 0;

    refNameSensorToDaqServant = NULL;

    if (( configfile = fopen (configFile, "r")) == NULL )
    {
      fprintf (stderr, "\nERROR: could not read configuration file '%s'\n", configFile);
      fflush (NULL);
      exit (EXIT_FAILURE);
    }

    printf ("Reading configuration\n");
    printf ("---------------------\n");

    while (( fgets (configLine, 120, configfile)) != NULL )
    {
      ligne++;

      // Is-it a comment ?
      int i = 0;
      while ( configLine[ i++ ] == ' ' ) { }
      if ( !( configLine[ i - 1 ] == '\n' || ( configLine[ i - 1 ] == '/' && configLine[ i ] == '/' )))
      {
        bool isASensorOption = false;
        bool isACorbaOption = acquisitionOrb->readCorbaOption (configLine);

        if ( !isACorbaOption )
        {
          /************************/
          /*   Sensor options   */
          /************************/

          char *value;

          /* looks for the line containing "refNameSensorToDaqServant" */
          if (( value = strstr (configLine, "refNameSensorToDaqServant=\"")) != NULL )
          {
            int len = rindex (value, '\"') - index (value, '\"') - 1;
            if ( len == -1 )
            {
              fprintf (stderr, "\nERROR: refNameSensorToDaqServant => interminated string in Sensor.conf\n");
              fflush (NULL);
              exit (EXIT_FAILURE);
            }

            refNameSensorToDaqServant = (char *) malloc (( len + 1 ) * sizeof (char));
            strncpy (refNameSensorToDaqServant, value + strlen ("refNameSensorToDaqServant=\""), len);
            *( refNameSensorToDaqServant + len ) = '\0';

            printf ("* Set refNameSensorToDaqServant to \'%s\'\n", refNameSensorToDaqServant);
            isASensorOption = true;
          }

          /* looks for the line containing "SensorMaxSeqNumber" */
          if (( value = strstr (configLine, "SensorMaxSeqNumber=")) != NULL )
          {
            sensorMaxSeqNumber = atoi (value + strlen ("SensorMaxSeqNumber="));

            if ( sensorMaxSeqNumber <= 0 )
            {
              fprintf (stderr, "\nERROR: Bad Value of SensorMaxSeqNumber\n");
              fflush (NULL);
              exit (EXIT_FAILURE);
            }

            printf ("* Set SensorMaxSeqNumber to %d\n", sensorMaxSeqNumber);
            isASensorOption = true;
          }


          /* looks for the line containing "SensorFifoLimit" */
          if (( value = strstr (configLine, "SensorFifoLimit=")) != NULL )
          {
            sensorFifoLimit = atoi (value + strlen ("SensorFifoLimit="));

            if ( sensorFifoLimit <= 0 )
            {
              fprintf (stderr, "\nERROR: Bad Value of SensorFifoLimit");
              fflush (NULL);
              exit (EXIT_FAILURE);
            }

            printf ("* Set SensorFifoLimit to %d\n", sensorFifoLimit);
            isASensorOption = true;
          }

        }

        if ( !isASensorOption && !isACorbaOption )
        {
          fprintf (stderr, "\nERROR: Line %d, unknown option %s\n", ligne, configLine);
          fflush (NULL);
          exit (EXIT_FAILURE);
        }
      }
    }

    fclose (configfile);

    /************************** Fin lecture *****************************/

    if ( refNameSensorToDaqServant == NULL )
    {
      fprintf (stderr, "\nERROR: refNameSensorToDaqServant undefined in Sensor.conf\n");
      fflush (NULL);
      exit (EXIT_FAILURE);
    }

    printf ("\n\n");
    fflush (NULL);

    /********************************************************************/

    pthread_mutex_init (&p_mut_start_stop_acquiring, NULL);
    pthread_mutex_lock (&p_mut_start_stop_acquiring);
    isProcessAcquisitionRunning = false;

    /********************************************************************/

    //fpgaRegisters est l'instance unique de la classe d'accès au FrontEnd  
    fpgaRegisters = FpgaRegisters::getInstance ();


    /*   if( fpgaRegisters->getNodeCoord(coordSensor, numSuperModule ) )
       {
          printf("Out of range Node coordinates \n");
          exit(EXIT_FAILURE);
       }
   */

    snprintf (readoutParamFilename, 128, "readoutParam_%d_%d_%d.conf", coordSensor.x, coordSensor.y, coordSensor.nPlan);
    snprintf (frontEndConfigFilename, 128, "frontEndConfig_%d_%d_%d.conf", coordSensor.x, coordSensor.y,
              coordSensor.nPlan);
    snprintf (pmtGainFilename, 128, "corrchip_%d_%d_%d.conf", coordSensor.x, coordSensor.y, coordSensor.nPlan);

    printf ("Starting Sensor on Node : X=%d Y=%d PLAN=%d SMODULE=%d\n",
            coordSensor.x,
            coordSensor.y,
            coordSensor.nPlan,
            numSuperModule);


    printf ("Version: %s\n", fpgaRegisters->getFirmwareVersion ());
    fflush (NULL);

    /********************************************************************/

    calibrationProcessing = new CalibrationProcessing (this);

    /********************************************************************/

    hardwareDataFifo = new HardwareDataFifo (sensorMaxSeqNumber);

    /********************************************************************/

    sensorIsShutingdown = false;

    pthread_mutex_init (&p_mut_acquiring, NULL);
    pthread_cond_init (&p_cond_acquiring, NULL);
    create_thread (&( this->thread_acquisition ), Sensor::startingThreadAcquisition, static_cast<void *>(this));


    htChangeStatus = NONE;
    pthread_mutex_init (&p_mut_HTManager, NULL);
    pthread_cond_init (&p_cond_HTManager, NULL);
    create_thread (&( this->thread_HTManager ), Sensor::startingThreadHTManager, static_cast<void *>(this));

    connectedToDaq = false;
    pthread_mutex_init (&p_mut_connectManager, NULL);
    pthread_cond_init (&p_cond_connectManager, NULL);
    create_thread (&( this->thread_connectManager ), Sensor::startingThreadConnectionManager,
                   static_cast<void *>(this));


    /*******************************************************************/

    if ( initialize ()) exit (0);

    acquisitionOrb->orbRun ();
  }

  /***********************************************************************/

  void Sensor::reportToDaq ( const DaqApplication::LogKind kind, const char *msg )
  {
    DaqApplication::LogMessage lmsg;

    switch ( kind )
    {
      case DaqApplication::INFO:
        fprintf (stderr, "INFO: ");
        break;
/*      case DaqApplication::RUN:
        fprintf(stderr, "RUN: ");
	break;*/
      case DaqApplication::WARNING:
        fprintf (stderr, "WARNING: ");
        break;
      case DaqApplication::ERROR:
        fprintf (stderr, "ERROR: ");
        break;
      case DaqApplication::FATAL:
        fprintf (stderr, "FATAL: ");
        break;
    }

    fprintf (stderr, "%s\n", msg);

    lmsg.kind = kind;
    lmsg.message = CORBA::string_dup (msg);
    if ( connectedToDaq )
      try { sensorToDaqItfRef->sensorReportMessage (coordSensor, lmsg); }
      catch ( ... ) { fprintf (stderr, "An exception occurs when invoking the distributed method reportToDaq()\n"); }
  }

  /***********************************************************************/
  /* initialize():  Appelé par le constructeur de la classe              */
  /* Instantie et publie sur l'ORB l'objet distribué contenant           */
  /* l'implémentation de l'interface Corba DaqToSensor               */
  /***********************************************************************/

  bool Sensor::initialize ( void )
  {
    if ( acquisitionOrb->initORB ()) return 1;
    PortableServer::POA_ptr poa = acquisitionOrb->getPOA ();
    if ( CORBA::is_nil (poa)) return 1;
    if ( acquisitionOrb->activatePOAMgr (poa)) return 1;

    // Création des instances d'interfaces
    daqToSensorServant = new DaqToSensorItf_i (this);

    // On publie les objets distribués sur l'ORB

    PortableServer::ObjectId_var oid = poa->activate_object (daqToSensorServant);
    daqToSensorObjRef = daqToSensorServant->_this ();
    daqToSensorServant->_remove_ref ();

    return 0;
  }

  /***********************************************************************/
  /* getDaqObjRefFromNS(): Appelé par le constructeur de la classe       */
  /* Permet de récupérer dans le NameService CORBA, la référence de      */
  /* de l'interface SensorToDaq grâce à son nom:                     */
  /*      "refNameSensorToDaqServant" suivit du numéro de supermodule  */
  /***********************************************************************/

  void Sensor::getDaqObjRefFromNS ()
  {
    CORBA::Object_ptr objref;

    char refname[30];
    //char numSM='0'+numSuperModule;
    //if (snprintf(refname, 30, "%s%c", refNameSensorToDaqServant, numSM) == -1)
    if ( snprintf (refname, 30, "%s", refNameSensorToDaqServant) == -1 )
    {
      printf ("refNameSensorToDaqServant is too long !\n");
      fflush (NULL);
      exit (EXIT_FAILURE);
    }

    while ( CORBA::is_nil (( objref = acquisitionOrb->getObjectReference (refname))))
    {
      printf ("No reference with the name \"%s\" was found into the NameService => Wait 2 seconds before retrying ",
              refname);
      fflush (NULL);
      sleep (2);
    }

    sensorToDaqItfRef = SensorToDaqItf::_narrow (objref);
    CORBA::release (objref);

    printf ("\nAn object Reference recorded as %s into the NameService has been found.\n", refname);
    fflush (NULL);
  }

  /***********************************************************************/
  /* connectTheDaq(): Appeler par le constructeur de la classe            */
  /* Invoque la méthode connectSensor() du Daq, qui permet d'enregister  */
  /* un nouveau capteur.                                                 */
  /***********************************************************************/

  void Sensor::connectTheDaq ()
  {
    bool connected = false;

    do
    {
      getDaqObjRefFromNS ();

      try
      {
        sensorToDaqItfRef->connectSensor (daqToSensorObjRef, coordSensor, sensorHardwareParam);
        connected = true;
      }
      catch ( CORBA::COMM_FAILURE &ex )
      {
        printf ("An exception CORBA::COMM_FAILURE has been raised when trying to connect the Daq\n");
        fflush (NULL);
      }
      catch ( CORBA::SystemException &ex )
      {
        printf ("An exception CORBA::SystemException has been raised when trying to connect the Daq\n");
        fflush (NULL);
      }

      if ( !connected )
      {
        CORBA::release (sensorToDaqItfRef);
        printf ("Failed to connect the Daq: Invalid CORBA reference => Wait 2 seconds before retrying ");
        fflush (NULL);
        sleep (2);
      }
    }
    while ( !connected );
    printf ("\nConnected to daq!\n");
    printf ("\n");

    pthread_mutex_lock (&p_mut_connectManager);
    connectedToDaq = true;
    pthread_mutex_unlock (&p_mut_connectManager);
  }

  /***********************************************************************/

  void Sensor::updateHardwareParam ( const SensorHardwareParamType &hwParam ) throw (InAcquisitionException)
  {
    pthread_mutex_lock (&p_mut_acquiring);

    if ( isProcessAcquisitionRunning )
      throw InAcquisitionException ();

    sensorHardwareParam = hwParam;


    // Si le voltage a changé, on le met à jour !

    if (( hwParam.highVoltageMode != fpgaRegisters->getHighVoltageMode ())
        || ( hwParam.feParams.highVoltageAmplitude != fpgaRegisters->getHighVoltageAmplitude ()))
    {
      fpgaRegisters->setHighVoltageMode (sensorHardwareParam.highVoltageMode);
      fpgaRegisters->setHighVoltageAmplitude (sensorHardwareParam.feParams.highVoltageAmplitude);

      pthread_mutex_lock (&p_mut_HTManager);
      switch ( hwParam.highVoltageMode )
      {
        case HtOff:
          htChangeStatus = REMOVE_HT;
          break;
        case HtAutoSetup:
          htChangeStatus = PUT_HT;
          break;
        default:
          break;
      }

      pthread_cond_broadcast (&p_cond_HTManager);
      pthread_mutex_unlock (&p_mut_HTManager);
    }

    pthread_mutex_unlock (&p_mut_acquiring);

  }

  /***********************************************************************/

  void *Sensor::startingThreadConnectionManager ( void *s )
  {
    static_cast<Sensor *>(s)->processConnectionManagerThread ();
    pthread_exit (0);
    return NULL;
  }

  /***********************************************************************/

  void Sensor::reportInitErrors ()
  {
    // Report initialization errors

    if ( fpgaInitStatus )
    {

      bool errorGPIO = fpgaInitStatus & 1;
      bool errorPLL2 = fpgaInitStatus & 2;
      bool errorPLL4 = fpgaInitStatus & 4;

      std::string fpgaMessage ("BAD FPGA INIT STATUS ! => ");

      if ( errorGPIO )
      {
        fpgaMessage += "Error etrax GPIOA initialization";
        if ( errorPLL2 || errorPLL4 )
          fpgaMessage = fpgaMessage + ", ";
      }

      if ( errorPLL2 )
      {
        fpgaMessage += "Error PLL(2) FPGA";
        if ( errorPLL4 )
          fpgaMessage = fpgaMessage + ", ";
      }

      if ( errorPLL4 )
        fpgaMessage += "Error PLL(4) FPGA";

      reportToDaq (DaqApplication::ERROR, fpgaMessage.c_str ());
    }

    if ( coordStatus != OK )
    {
      char coordMessage[100];
      if ( coordStatus == BAD )
        snprintf (coordMessage, 100, "Error initializing coordinates !");
      else //coordStatus == OUT_OF_RANGE
        snprintf (coordMessage, 100, "Error initializing coordinates : Out Of Range !");

      reportToDaq (DaqApplication::ERROR, coordMessage);
    }
  }

  /***********************************************************************/

  void Sensor::initHT ()
  {
    // Lance le montage de la HT

    fpgaRegisters->setHighVoltageMode (sensorHardwareParam.highVoltageMode);
    fpgaRegisters->setHighVoltageAmplitude (sensorHardwareParam.feParams.highVoltageAmplitude);

    if ( sensorHardwareParam.highVoltageMode == HtAutoSetup )
    {
      pthread_mutex_lock (&p_mut_HTManager);
      htChangeStatus = PUT_HT;
      pthread_cond_broadcast (&p_cond_HTManager);
      pthread_mutex_unlock (&p_mut_HTManager);
    }
  }

  /***********************************************************************/

  void Sensor::processConnectionManagerThread ()
  {
    int retcode;
    struct timeval now;
    struct timespec timeout;
    int in = 0;
    bool isHTmutexLocked = false;

    do
    {
      pthread_mutex_lock (&p_mut_connectManager);

      while ( connectedToDaq && !sensorIsShutingdown )
      {
        gettimeofday (&now, NULL);
        timeout.tv_sec = now.tv_sec + 5; // Toutes les 5s on ping !
        timeout.tv_nsec = now.tv_usec * 1000;

        retcode = pthread_cond_timedwait (&p_cond_connectManager, &p_mut_connectManager, &timeout);
        if ( retcode == ETIMEDOUT )
          try
          {
            // Mieux que le ping... ou comment joindre l'utile à l'agréable ;-)
            if (( in++ ) % 3600 == 0 ) // toutes les heures: on envoie la temp!
            {
              pthread_mutex_lock (&p_mut_HTManager);
              isHTmutexLocked = true;
              if ( htChangeStatus == NONE )
                sensorToDaqItfRef->sendTemperature (coordSensor, (CORBA::UShort) fpgaRegisters->readAdcTemp ());
              pthread_mutex_unlock (&p_mut_HTManager);
              isHTmutexLocked = false;
            }
            else
              sensorToDaqItfRef->pingDaq ();
          }
          catch ( ... )
          {
            connectedToDaq = false;
            CORBA::release (sensorToDaqItfRef);
            if ( isHTmutexLocked ) pthread_mutex_unlock (&p_mut_HTManager);
          }
      };

      pthread_mutex_unlock (&p_mut_connectManager);

      if ( sensorIsShutingdown ) break;

      sleep (1);

      connectTheDaq ();

      reportInitErrors ();

      initHT ();

    }
    while ( !sensorIsShutingdown );

    printf ("processConnectionManager has finished.\n");
    pthread_exit (NULL);
  }

  /***********************************************************************/

  void *Sensor::startingThreadHTManager ( void *s )
  {
    static_cast<Sensor *>(s)->processHTManagerThread ();
    pthread_exit (0);
    return NULL;
  }

  /***********************************************************************/

  void Sensor::processHTManagerThread ()
  {
    bool hasChangeStatusDuringProcess = false;

    do
    {
      if ( !hasChangeStatusDuringProcess )
      {
        pthread_mutex_lock (&p_mut_HTManager);
        htChangeStatus = NONE;

        while ( htChangeStatus == NONE && !sensorIsShutingdown )
        {
          printf ("PROCESSHTMANAGER IS SLEEPING\n");
          fflush (NULL);
          pthread_cond_wait (&p_cond_HTManager, &p_mut_HTManager);
        };
        pthread_mutex_unlock (&p_mut_HTManager);
      }

      if ( sensorIsShutingdown ) break;

      switch ( htChangeStatus )
      {
        case PUT_HT:
          printf ("Starting Put High Voltage Process...\n");
          fflush (NULL);
          try
          {
            fpgaRegisters->setHT (fpgaRegisters->getHighVoltageAmplitude ());
          }
          catch ( HTException )
          {
            reportToDaq (DaqApplication::ERROR, "Putting HT failed in Sensor::processHTManagerThread - HTException");
            pthread_mutex_lock (&p_mut_HTManager);
            htChangeStatus = REMOVE_HT;
            pthread_mutex_unlock (&p_mut_HTManager);
            hasChangeStatusDuringProcess = true;
            break;
          }
          printf ("High Voltage sucessfully put...\n");
          fflush (NULL);
          hasChangeStatusDuringProcess = ( htChangeStatus != PUT_HT );
          break;

        case REMOVE_HT:
          printf ("Removing High Voltage Process...\n");
          fflush (NULL);

          try
          {
            fpgaRegisters->setHT (0);
            fpgaRegisters->killHT ();
          }
          catch ( HTException )
          {
            reportToDaq (DaqApplication::ERROR, "Removing HT failed in Sensor::processHTManagerThread - HTException");
            break;
          }
          printf ("High Voltage sucessfully removed.\n");
          hasChangeStatusDuringProcess = ( htChangeStatus != REMOVE_HT );
          break;

        default:
          fprintf (stderr, "ERROR: htChangeStatus = NONE in Sensor::processHTManagerThread() !\n");
          reportToDaq (DaqApplication::ERROR, "htChangeStatus = NONE in Sensor::processHTManagerThread()");
          break;
      }
    }
    while ( !sensorIsShutingdown );

    printf ("processHTManager has finished.\n");
    pthread_exit (NULL);
  }

  /***********************************************************************/
  /* getCurrentCycle() : Invoquée par le Daq                             */
  /* permet de connaitre le numéro du cycle courrant                     */
  /* (util si le Daq n'est pas pourvu de l'horloge GPS).                 */
  /***********************************************************************/

  DaqApplication::NumCycle Sensor::getCurrentCycle ()
  {
    return fpgaRegisters->getCurrentCycle ();
  }

  /***********************************************************************/

  void *Sensor::startingThreadAcquisition ( void *s )
  {
    static_cast<Sensor *>(s)->processAcquisition ();
    pthread_exit (0);
    return NULL;
  }

  /***********************************************************************/
  /* processAcquisition : thread crée par le constructeur de la classe.  */
  /* Il est "réveiller" par un appel à startDataAcquiring() et endormit  */
  /* par un appel à stopDataAcquiring().                                 */
  /* Il effectue la lecture des ADCs d'un cycle sur le front-end et      */
  /* place ces données dans une FIFO de profondeur FIFO_SIZE             */
  /***********************************************************************/

  inline void Sensor::processAcquisition ()
  {
    bool acquisitionFailure = false;
    bool isCycleLengthAcquistion = false;

    fifo.init ();

    fpgaRegisters->softHalt ();
    fpgaRegisters->stopSequencer ();


    do
    {
      // Conditions d'arret d'acquisition : 


      if ( acquisitionFailure || !isProcessAcquisitionRunning ||
           ( isCycleLengthAcquistion && acquisitionParameters.numberOfCycleToAcquire <= 0 ) || !connectedToDaq ||
           sensorIsShutingdown )
      {
        printf ("ACQUISITION PROCESS IS STOPPING\n");
        fflush (NULL);

        // Procédure d'arret de l'acquisition :

        if ( fpgaRegisters->isSequencerRunning ())
          fpgaRegisters->stopSequencer ();

        // On vide la FIFO
        fifo.clear ();
        calibrationProcessing->clear ();

        isProcessAcquisitionRunning = false;

        if ( sensorIsShutingdown ) pthread_exit (NULL);

        pthread_mutex_unlock (&p_mut_start_stop_acquiring);
        pthread_mutex_lock (&p_mut_acquiring);

        while ( !isProcessAcquisitionRunning && !sensorIsShutingdown )
        {
          pthread_cond_wait (&p_cond_acquiring, &p_mut_acquiring);
        };


        // Procédure de redemarage de l'acquisition

        acquisitionFailure = false;
        pthread_mutex_unlock (&p_mut_acquiring);
        pthread_mutex_unlock (&p_mut_start_stop_acquiring);


        printf ("ACQUISITION PROCESS IS STARTING\n");
        fflush (NULL);

        hardwareDataFifo->clearDataFifo ();

        isCycleLengthAcquistion = ( acquisitionParameters.numberOfCycleToAcquire != 0 );

        // Allocate Calibration Data Type

        /* Modif JM 25/07/2006: inhibition de l'envoi des histos de bruit */
        if ( acquisitionParameters.type != NORMAL_ACQUISITION )
          calibrationProcessing->initialize ();


        if ( fpgaRegisters->initializeAcquisitionStartupParameters ())
        {
          reportToDaq (DaqApplication::ERROR,
                       "Problem initializing Fpga parameters in Sensor::processAcquisition() when calling FpgaRegisters::initializeAcquisitionStartupParameters()");
          acquisitionFailure = true;
        }

        acquisitionParameters.acquisitionStartCycle = fpgaRegisters->getCurrentCycle ();
      }

      /************************************/
      /*       BOUCLE D'ACQUISITION       */
      /************************************/

      if ( !acquisitionFailure )
      {
        DaqApplication::NumCycle nc;
        SeqAdcEventData *seqADC = NULL;
        struct timeb t1, t2; //t0, t3;

//	ftime(&t0);
        seqADC = new SeqAdcEventData (sensorMaxSeqNumber);


        try
        {
          ftime (&t1);

          hardwareDataFifo->readSequence (seqADC, nc);

          ftime (&t2);

          if ( nc == 0 )
          {
            reportToDaq (DaqApplication::ERROR,
                         "Bad cycle number - last read cycle was cycle number 0 in HardwareDataFifo::readSequence()");
            acquisitionFailure = true;
          }
          else
            try
            {
              processCycleSeq (seqADC, nc);

/*	      ftime(&t3);

	      printf("tps_alloc=%d ms\ttps_decodeCycle=%d ms\ttps_process=%d ms\t", (int)((t1.time*1000+t1.millitm) - (t0.time*1000+t0.millitm)), (int)((t2.time*1000+t2.millitm) - (t1.time*1000+t1.millitm)), (int)((t3.time*1000+t3.millitm) - (t2.time*1000+t2.millitm))); */
              // Reduce logs messages J.M. 21 Aug '06
              // printf("%d\t %lu\n", (int)((t2.time*1000+t2.millitm) - (t1.time*1000+t1.millitm)), (unsigned long) seqADC->length());
              fflush (NULL);

            }
            catch ( FifoFullException )
            {
              char message[100];
              snprintf (message, 100, "Acquistion failed at cycle n° %u - FifoFullException", nc);
              printf ("%s\n", message);

              reportToDaq (DaqApplication::ERROR, message);

              acquisitionFailure = true;
            }

        }
        catch ( ReadFailureException )
        {
          reportToDaq (DaqApplication::ERROR, "HardwareDataFifo::readSequence() raised a ReadFailureException");
          acquisitionFailure = true;
        }

        if ( isCycleLengthAcquistion && !acquisitionFailure )
          acquisitionParameters.numberOfCycleToAcquire--;

      };
    }
    while ( !sensorIsShutingdown );

    printf ("processAcquisition has finished.\n");
    pthread_exit (NULL);
  }

  /***********************************************************************/

  inline void
  Sensor::processCycleSeq ( SeqAdcEventData *seqADC, DaqApplication::NumCycle &nc ) throw (FifoFullException)
  {
    // Dans tous les modes sauf debug, on fait de la calibration
    /* Modif JM 25/07/2006: inhibition de l'envoi des histos de bruit */
    if ( acquisitionParameters.type != DEBUG_ACQUISITION && acquisitionParameters.type != NORMAL_ACQUISITION )
      calibrationProcessing->updateWithCycleSeqADC (seqADC);

    // Si on est en debug ou en acquisition, on stocke les données dans la fifo soft.
    if ( acquisitionParameters.type == DEBUG_ACQUISITION || acquisitionParameters.type == NORMAL_ACQUISITION )
    {
      try
      {
        fifo.add (nc, seqADC);
      }
      catch ( FifoFullException )
      {
        throw;
      }
    }
    else
      // Si on fait une calibration, on ne stocke pas les données.
    {
      delete seqADC;

      // Fin de calibration ?
      if ( acquisitionParameters.numberOfCycleToAcquire == 1 )
        calibrationProcessing->setCalibrationIsFinished ();
    }
  }


  /***********************************************************************/
  /* getTargetSensorCycleDatas(nc): Invoquée par le Daq                  */
  /* Permet de récupérer la Séquence d'ADC du cycle nc                   */
  /***********************************************************************/

  SeqAdcEventData *Sensor::getTargetSensorCycleDatas (
    DaqApplication::NumCycle nc ) throw (CycleNotFoundException, CycleNotYetAcquiredException, NotInAcquisitionException)
  {
    if ( !isProcessAcquisitionRunning )
      throw NotInAcquisitionException ();

    try
    {
      return fifo.getAndCleanOlder (nc);
    }
    catch ( ... )
    {
      throw;
    }
  }

  /***********************************************************************/

  void Sensor::configAcquisitionOperatingModeFpgaRegisters (
    const AcquisitionOperatingModeParametersType &aomp ) throw (InAcquisitionException)
  {
    pthread_mutex_lock (&p_mut_acquiring);

    printf (GRN "** configAcquisitionOperatingModeFpgaRegisters - In progress\n" RESET);
    fflush (NULL);

    if ( isProcessAcquisitionRunning )
      throw InAcquisitionException ();

    bool highVoltageHasChangedStatus = false;

    acquisitionParameters.type = aomp._d ();

//?    fpgaRegisters->setAandBMode(sensorHardwareParam.aAndBMode);
    fpgaRegisters->setBusyMode (sensorHardwareParam.busyMode);
    fpgaRegisters->setValidMode (false);
    fpgaRegisters->setTrigOutMode (false);

    if ( aomp._d () == NORMAL_ACQUISITION || aomp._d () == TRIGRATE_CALIBRATION )
    {
      printf ("loading NORMAL_ACQUISITION parameters\n");
      fflush (NULL);
      fpgaRegisters->setTrigSrc (NormTrig);
      fpgaRegisters->setCalibrationMode (RunOnlyMode);

      fpgaRegisters->setZeroSuppEnable (true);
      fpgaRegisters->setHoldDelayA (sensorHardwareParam.holdDelayNormTrigA);
      fpgaRegisters->setHoldDelayB (sensorHardwareParam.holdDelayNormTrigB);
      fpgaRegisters->setThresholdA (sensorHardwareParam.thresholdA);
      fpgaRegisters->setThresholdB (sensorHardwareParam.thresholdB);

      fpgaRegisters->setFrontEndMasks (sensorHardwareParam.feParams.masksTable);
      fpgaRegisters->setFrontEndGains (sensorHardwareParam.feParams.gainsTable);
      fpgaRegisters->setNbCycleIrq (sensorHardwareParam.nbCycleIrq);
      fpgaRegisters->setMonoChannelEnable (false);
      fpgaRegisters->setDelayCycle (sensorHardwareParam.delayCycle);
      fpgaRegisters->setZeroSuppTable (sensorHardwareParam.feParams.zeroSuppressTable);
      fpgaRegisters->setPedestals (sensorHardwareParam.feParams.pedestalsTable);

      fpgaRegisters->setValidMode (sensorHardwareParam.externalValidMode, sensorHardwareParam.externalValidWindowWidth);

      highVoltageHasChangedStatus = ( sensorHardwareParam.highVoltageMode != fpgaRegisters->getHighVoltageMode ())
                                    || ( sensorHardwareParam.feParams.highVoltageAmplitude !=
                                         fpgaRegisters->getHighVoltageAmplitude ());
      fpgaRegisters->setHighVoltageMode (sensorHardwareParam.highVoltageMode);
      fpgaRegisters->setHighVoltageAmplitude (sensorHardwareParam.feParams.highVoltageAmplitude);
      fpgaRegisters->setFifoFlagsOffsetParams (sensorHardwareParam.fifoFlagsOffsetParams);
    }

    if ( aomp._d () == PEDESTALS_CALIBRATION )
    {
      printf ("loading PEDESTALS_CALIBRATION parameters\n");
      fflush (NULL);
      fpgaRegisters->setTrigSrc (LedPulseTrig);
      fpgaRegisters->setCalibrationMode (LedPulseMode);
      fpgaRegisters->setZeroSuppEnable (false);
      fpgaRegisters->setLedPulsePeriod (sensorHardwareParam.ledPulsePeriod);
      fpgaRegisters->setHoldDelayA (sensorHardwareParam.holdDelayLedTrigA);
      fpgaRegisters->setHoldDelayB (sensorHardwareParam.holdDelayLedTrigB);
      fpgaRegisters->setThresholdA (sensorHardwareParam.thresholdA);
      fpgaRegisters->setThresholdB (sensorHardwareParam.thresholdB);
      fpgaRegisters->setLedPulseAmplitude (0);
      fpgaRegisters->setFrontEndMasks (sensorHardwareParam.feParams.masksTable);
      fpgaRegisters->setFrontEndGains (sensorHardwareParam.feParams.gainsTable);
      fpgaRegisters->setNbCycleIrq (sensorHardwareParam.nbCycleIrq);
      fpgaRegisters->setMonoChannelEnable (false);
      fpgaRegisters->setDelayCycle (sensorHardwareParam.delayCycle);
      fpgaRegisters->setZeroSuppTable (sensorHardwareParam.feParams.zeroSuppressTable);
      fpgaRegisters->setPedestals (sensorHardwareParam.feParams.pedestalsTable);
      fpgaRegisters->setValidMode (false);
      fpgaRegisters->setTrigOutMode (false);

      fpgaRegisters->setAandBMode (false);

      /* reSwap to pedestals measurements with HT OFF ;-) */
      highVoltageHasChangedStatus = fpgaRegisters->getHighVoltageMode () != HtOff;
      fpgaRegisters->setHighVoltageMode (HtOff);

      fpgaRegisters->setFifoFlagsOffsetParams (sensorHardwareParam.fifoFlagsOffsetParams);
      fpgaRegisters->setBusyMode (false);
    }

    if ( aomp._d () == GAIN_CALIBRATION )
    {
      printf ("loading GAIN_CALIBRATION parameters\n");
      fflush (NULL);
      fpgaRegisters->setTrigSrc (LedPulseTrig);
      fpgaRegisters->setCalibrationMode (LedPulseMode);
      fpgaRegisters->setZeroSuppEnable (false);
      fpgaRegisters->setLedPulsePeriod (sensorHardwareParam.ledPulsePeriod);
      fpgaRegisters->setHoldDelayA (sensorHardwareParam.holdDelayLedTrigA);
      fpgaRegisters->setHoldDelayB (sensorHardwareParam.holdDelayLedTrigB);
      fpgaRegisters->setThresholdA (sensorHardwareParam.thresholdA);
      fpgaRegisters->setThresholdB (sensorHardwareParam.thresholdB);
      fpgaRegisters->setLedPulseAmplitude (sensorHardwareParam.ledPulseAmplitudeLow);
      fpgaRegisters->setFrontEndMasks (sensorHardwareParam.feParams.masksTable);
      fpgaRegisters->setFrontEndGains (sensorHardwareParam.feParams.gainsTable);
      fpgaRegisters->setNbCycleIrq (sensorHardwareParam.nbCycleIrq);
      fpgaRegisters->setMonoChannelEnable (false);
      fpgaRegisters->setDelayCycle (sensorHardwareParam.delayCycle);
      fpgaRegisters->setZeroSuppTable (sensorHardwareParam.feParams.zeroSuppressTable);
      fpgaRegisters->setPedestals (sensorHardwareParam.feParams.pedestalsTable);
      fpgaRegisters->setValidMode (false);
      fpgaRegisters->setTrigOutMode (false);
      highVoltageHasChangedStatus = ( sensorHardwareParam.highVoltageMode != fpgaRegisters->getHighVoltageMode ())
                                    || ( sensorHardwareParam.feParams.highVoltageAmplitude !=
                                         fpgaRegisters->getHighVoltageAmplitude ());
      fpgaRegisters->setHighVoltageMode (sensorHardwareParam.highVoltageMode);
      fpgaRegisters->setHighVoltageAmplitude (sensorHardwareParam.feParams.highVoltageAmplitude);
      fpgaRegisters->setFifoFlagsOffsetParams (sensorHardwareParam.fifoFlagsOffsetParams);
      fpgaRegisters->setBusyMode (false);
    }

    if ( aomp._d () == LED_CALIBRATION )
    {
      printf ("loading LED_CALIBRATION parameters\n");

      fflush (NULL);
      fpgaRegisters->setTrigSrc (LedPulseTrig);
      fpgaRegisters->setCalibrationMode (LedPulseMode);
      fpgaRegisters->setZeroSuppEnable (true);
      fpgaRegisters->setLedPulsePeriod (sensorHardwareParam.ledPulsePeriod);
      fpgaRegisters->setHoldDelayA (sensorHardwareParam.holdDelayLedTrigA);
      fpgaRegisters->setHoldDelayB (sensorHardwareParam.holdDelayLedTrigB);
      fpgaRegisters->setThresholdA (sensorHardwareParam.thresholdA);
      fpgaRegisters->setThresholdB (sensorHardwareParam.thresholdB);
      fpgaRegisters->setLedPulseAmplitude (sensorHardwareParam.ledPulseAmplitudeHigh);

      fpgaRegisters->setFrontEndMasks (sensorHardwareParam.feParams.masksTable);
      fpgaRegisters->setFrontEndGains (sensorHardwareParam.feParams.gainsTable);
      fpgaRegisters->setNbCycleIrq (sensorHardwareParam.nbCycleIrq);
      fpgaRegisters->setMonoChannelEnable (false);
      fpgaRegisters->setDelayCycle (sensorHardwareParam.delayCycle);
      fpgaRegisters->setZeroSuppTable (sensorHardwareParam.feParams.zeroSuppressTable);
      fpgaRegisters->setPedestals (sensorHardwareParam.feParams.pedestalsTable);
      fpgaRegisters->setValidMode (false);
      fpgaRegisters->setTrigOutMode (false);
      highVoltageHasChangedStatus = ( sensorHardwareParam.highVoltageMode != fpgaRegisters->getHighVoltageMode ())
                                    || ( sensorHardwareParam.feParams.highVoltageAmplitude !=
                                         fpgaRegisters->getHighVoltageAmplitude ());
      fpgaRegisters->setHighVoltageMode (sensorHardwareParam.highVoltageMode);
      fpgaRegisters->setHighVoltageAmplitude (sensorHardwareParam.feParams.highVoltageAmplitude);
      fpgaRegisters->setFifoFlagsOffsetParams (sensorHardwareParam.fifoFlagsOffsetParams);
      fpgaRegisters->setBusyMode (false);
    }

    if ( aomp._d () == OPPOSITE_SIDE_LED_CALIBRATION )
    {
      printf ("loading OPPOSITE_SIDE_LED_CALIBRATION parameters\n");
      fflush (NULL);
      fpgaRegisters->setTrigSrc (LedPulseTrig);
      fpgaRegisters->setCalibrationMode (ExtLedPulseMode);
      fpgaRegisters->setZeroSuppEnable (true);
      fpgaRegisters->setLedPulsePeriod (sensorHardwareParam.ledPulsePeriod);
      fpgaRegisters->setHoldDelayA (sensorHardwareParam.holdDelayLedTrigA);
      fpgaRegisters->setHoldDelayB (sensorHardwareParam.holdDelayLedTrigB);
      fpgaRegisters->setThresholdA (sensorHardwareParam.thresholdA);
      fpgaRegisters->setThresholdB (sensorHardwareParam.thresholdB);
      fpgaRegisters->setLedPulseAmplitude (sensorHardwareParam.ledPulseAmplitudeHigh);

      fpgaRegisters->setFrontEndMasks (sensorHardwareParam.feParams.masksTable);
      fpgaRegisters->setFrontEndGains (sensorHardwareParam.feParams.gainsTable);
      fpgaRegisters->setNbCycleIrq (sensorHardwareParam.nbCycleIrq);
      fpgaRegisters->setMonoChannelEnable (false);
      fpgaRegisters->setDelayCycle (sensorHardwareParam.delayCycle);
      fpgaRegisters->setZeroSuppTable (sensorHardwareParam.feParams.zeroSuppressTable);
      fpgaRegisters->setPedestals (sensorHardwareParam.feParams.pedestalsTable);
      fpgaRegisters->setValidMode (false);
      fpgaRegisters->setTrigOutMode (false);
      highVoltageHasChangedStatus = ( sensorHardwareParam.highVoltageMode != fpgaRegisters->getHighVoltageMode ())
                                    || ( sensorHardwareParam.feParams.highVoltageAmplitude !=
                                         fpgaRegisters->getHighVoltageAmplitude ());
      fpgaRegisters->setHighVoltageMode (sensorHardwareParam.highVoltageMode);
      fpgaRegisters->setHighVoltageAmplitude (sensorHardwareParam.feParams.highVoltageAmplitude);
      fpgaRegisters->setFifoFlagsOffsetParams (sensorHardwareParam.fifoFlagsOffsetParams);
      fpgaRegisters->setBusyMode (false);
    }

    if ( aomp._d () == TEST1_PULSE_CALIBRATION )
    {
      printf ("loading TEST1PULSE_CALIBRATION parameters\n");
      fflush (NULL);
      fpgaRegisters->setTrigSrc (TestPulseTrig);
      fpgaRegisters->setCalibrationMode (Test1PulseMode);
      fpgaRegisters->setTestPulseAmp (sensorHardwareParam.testPulseAmplitude);
      fpgaRegisters->setZeroSuppEnable (true);
      fpgaRegisters->setTestPulsePeriod (sensorHardwareParam.testPulsePeriod);
      fpgaRegisters->setHoldDelayA (sensorHardwareParam.holdDelayTestTrigA);
      fpgaRegisters->setHoldDelayB (sensorHardwareParam.holdDelayTestTrigB);
      fpgaRegisters->setThresholdA (sensorHardwareParam.thresholdA);
      fpgaRegisters->setThresholdB (sensorHardwareParam.thresholdB);

      fpgaRegisters->setFrontEndMasks (sensorHardwareParam.feParams.masksTable);
      fpgaRegisters->setFrontEndGains (sensorHardwareParam.feParams.gainsTable);
      fpgaRegisters->setNbCycleIrq (sensorHardwareParam.nbCycleIrq);
      fpgaRegisters->setMonoChannelEnable (false);
      fpgaRegisters->setDelayCycle (sensorHardwareParam.delayCycle);
      fpgaRegisters->setZeroSuppTable (sensorHardwareParam.feParams.zeroSuppressTable);
      fpgaRegisters->setPedestals (sensorHardwareParam.feParams.pedestalsTable);
      fpgaRegisters->setValidMode (false);
      fpgaRegisters->setTrigOutMode (false);
      highVoltageHasChangedStatus = fpgaRegisters->getHighVoltageMode () != HtOff;
      fpgaRegisters->setHighVoltageMode (HtOff);
      fpgaRegisters->setFifoFlagsOffsetParams (sensorHardwareParam.fifoFlagsOffsetParams);
      fpgaRegisters->setBusyMode (false);
    }

    if ( aomp._d () == TEST2_PULSE_CALIBRATION )
    {
      printf ("loading TEST2PULSE_CALIBRATION parameters\n");
      fflush (NULL);
      fpgaRegisters->setTrigSrc (TestPulseTrig);
      fpgaRegisters->setCalibrationMode (Test2PulseMode);
      fpgaRegisters->setTestPulseAmp (sensorHardwareParam.testPulseAmplitude);
      fpgaRegisters->setZeroSuppEnable (true);
      fpgaRegisters->setTestPulsePeriod (sensorHardwareParam.testPulsePeriod);
      fpgaRegisters->setHoldDelayA (sensorHardwareParam.holdDelayTestTrigA);
      fpgaRegisters->setHoldDelayB (sensorHardwareParam.holdDelayTestTrigB);
      fpgaRegisters->setThresholdA (sensorHardwareParam.thresholdA);
      fpgaRegisters->setThresholdB (sensorHardwareParam.thresholdB);

      fpgaRegisters->setFrontEndMasks (sensorHardwareParam.feParams.masksTable);
      fpgaRegisters->setFrontEndGains (sensorHardwareParam.feParams.gainsTable);
      fpgaRegisters->setNbCycleIrq (sensorHardwareParam.nbCycleIrq);
      fpgaRegisters->setMonoChannelEnable (false);
      fpgaRegisters->setDelayCycle (sensorHardwareParam.delayCycle);
      fpgaRegisters->setZeroSuppTable (sensorHardwareParam.feParams.zeroSuppressTable);
      fpgaRegisters->setPedestals (sensorHardwareParam.feParams.pedestalsTable);
      fpgaRegisters->setValidMode (false);
      fpgaRegisters->setTrigOutMode (false);
      highVoltageHasChangedStatus = fpgaRegisters->getHighVoltageMode () != HtOff;
      fpgaRegisters->setHighVoltageMode (HtOff);
      fpgaRegisters->setFifoFlagsOffsetParams (sensorHardwareParam.fifoFlagsOffsetParams);
      fpgaRegisters->setBusyMode (false);
    }

    if ( aomp._d () == DEBUG_ACQUISITION )
    {
      printf ("loading DEBUG_ACQUISITION parameters\n");
      fflush (NULL);
      HighVoltageModeType hvm = fpgaRegisters->getHighVoltageMode ();
      printf("HighVoltageModeType= %lu\n",hvm);
      CORBA::UShort hva = fpgaRegisters->getHighVoltageAmplitude ();
      printf("HighVoltageAmplitude= %lu\n",hva);
      fpgaRegisters->setFpgaRegisters (aomp.fpgaConfig ());
      highVoltageHasChangedStatus = ( hvm != fpgaRegisters->getHighVoltageMode ())
                                    || ( hva != fpgaRegisters->getHighVoltageAmplitude ());
    }

    fpgaRegisters->setFifoLimit (sensorFifoLimit);

    fpgaRegisters->setAandBMode (sensorHardwareParam.aAndBMode);

    // Si le voltage a changé, on le met à jour !

    if ( highVoltageHasChangedStatus )
    {
      pthread_mutex_lock (&p_mut_HTManager);
      switch ( fpgaRegisters->getHighVoltageMode ())
      {
        case HtOff:
          htChangeStatus = REMOVE_HT;
          break;
        case HtAutoSetup:
          htChangeStatus = PUT_HT;
          break;
        default:
          break;
      }
      pthread_cond_broadcast (&p_cond_HTManager);
      pthread_mutex_unlock (&p_mut_HTManager);
    }
    printf (GRN "** configAcquisitionOperatingModeFpgaRegisters - Done\n" RESET);
    fflush (NULL);
    pthread_mutex_unlock (&p_mut_acquiring);
  }

  /***********************************************************************/

  DaqApplication::NumCycle Sensor::getAcquisitionStartCycle () throw (NotInAcquisitionException)
  {
    DaqApplication::NumCycle cycle;

    //pthread_mutex_lock(&p_mut_acquiring);

    if ( !isProcessAcquisitionRunning || !acquisitionParameters.acquisitionStartCycle )
    {
      //pthread_mutex_unlock(&p_mut_acquiring);
      throw NotInAcquisitionException ();
    }
    cycle = acquisitionParameters.acquisitionStartCycle;

    //pthread_mutex_unlock(&p_mut_acquiring);  

    return cycle;
  };

  /***********************************************************************/
  /* startDataAcquiring(): Invoquée par le Daq                           */
  /* Permet de démarrer l'acquisition sur le capteur                     */
  /***********************************************************************/

  void Sensor::startDataAcquiring (
    short unsigned int nc ) throw (AcquisitionIsAlreadyRunningException, HighVoltageInitializationInProgressException)
  {
    pthread_mutex_lock (&p_mut_start_stop_acquiring);
    pthread_mutex_lock (&p_mut_acquiring);

    if ( isProcessAcquisitionRunning )
    {
      printf ("Sensor x=%u y=%u, nplan=%u \tstartDataAcquiring => AcquisitionIsAlreadyRunning\n", coordSensor.x,
              coordSensor.y, coordSensor.nPlan);
      fflush (NULL);
      pthread_mutex_unlock (&p_mut_acquiring);
      pthread_mutex_unlock (&p_mut_start_stop_acquiring);
      throw AcquisitionIsAlreadyRunningException ();
    }

    pthread_mutex_lock (&p_mut_HTManager);

    if ( htChangeStatus != NONE )
    {
      printf ("Sensor x=%u y=%u, nplan=%u \tstartDataAcquiring => HighVoltageInitializationInProgress\n", coordSensor.x,
              coordSensor.y, coordSensor.nPlan);
      fflush (NULL);
      pthread_mutex_unlock (&p_mut_HTManager);
      pthread_mutex_unlock (&p_mut_acquiring);
      pthread_mutex_unlock (&p_mut_start_stop_acquiring);
      throw HighVoltageInitializationInProgressException ();
    }

    pthread_mutex_unlock (&p_mut_HTManager);

    acquisitionParameters.numberOfCycleToAcquire = nc;
    acquisitionParameters.acquisitionStartCycle = 0;

    isProcessAcquisitionRunning = true;

    pthread_cond_broadcast (&p_cond_acquiring);
    pthread_mutex_unlock (&p_mut_acquiring);

    printf ("Sensor x=%u y=%u, nplan=%u \tstartDataAcquiring\n", coordSensor.x, coordSensor.y, coordSensor.nPlan);
    fflush (NULL);
  }

  /***********************************************************************/

  void Sensor::startCalibration (
    short unsigned int nc ) throw (AcquisitionIsAlreadyRunningException, HighVoltageInitializationInProgressException)
  {
    // On démarre l'acquisition

    try
    {
      startDataAcquiring (nc);
    }
    catch ( ... )
    {
      throw;
    }

    printf ("Sensor x=%u y=%u, nplan=%u startCalibration(%d)\n", coordSensor.x, coordSensor.y, coordSensor.nPlan, nc);
    fflush (NULL);
  }

  /***********************************************************************/
  /* stopDataAcquiring(): Invoquée par le Daq                            */
  /* Permet de suspendre l'acquisition sur le capteur                    */
  /***********************************************************************/

  void Sensor::stopDataAcquiring () throw (NotInAcquisitionException)
  {
    pthread_mutex_lock (&p_mut_start_stop_acquiring);
    pthread_mutex_lock (&p_mut_acquiring);
    if ( !isProcessAcquisitionRunning )
    {
      printf ("Sensor x=%u y=%u, nplan=%u \tstopDataAcquiring => AlreadyDoneException\n", coordSensor.x, coordSensor.y,
              coordSensor.nPlan);
      fflush (NULL);
      pthread_mutex_unlock (&p_mut_acquiring);
      throw NotInAcquisitionException ();
    }

    isProcessAcquisitionRunning = false;
    acquisitionParameters.acquisitionStartCycle = 0;

    pthread_mutex_unlock (&p_mut_acquiring);

    printf ("Sensor x=%u y=%u, nplan=%u \tstopDataAcquiring\n", coordSensor.x, coordSensor.y, coordSensor.nPlan);
    fflush (NULL);
  }

  /***********************************************************************/

  void Sensor::disconnect ()
  {
    printf ("Sensor x=%u y=%u, nplan=%u was disconnected by Daq\n", coordSensor.x, coordSensor.y, coordSensor.nPlan);
    fflush (NULL);

    pthread_mutex_lock (&p_mut_connectManager);
    CORBA::release (sensorToDaqItfRef);

    connectedToDaq = false;

    pthread_cond_broadcast (&p_cond_connectManager);

    pthread_mutex_unlock (&p_mut_connectManager);
  }

  /***********************************************************************/

  void Sensor::removeHighVoltage ()
  {
    pthread_mutex_lock (&p_mut_HTManager);
    htChangeStatus = REMOVE_HT;
    pthread_cond_broadcast (&p_cond_HTManager);
    pthread_mutex_unlock (&p_mut_HTManager);
  }

  /***********************************************************************/

  void Sensor::shutdown ()
  {
    // On enlève la HT
    removeHighVoltage ();
    while ( htChangeStatus == NONE )
    {
      sleep (1);
    }

    // On termine tous les threads
    sensorIsShutingdown = true;
    pthread_cond_broadcast (&p_cond_acquiring);
    pthread_join (thread_acquisition, NULL);
    pthread_cond_broadcast (&p_cond_HTManager);
    pthread_join (thread_HTManager, NULL);
    pthread_cond_broadcast (&p_cond_connectManager);
    pthread_join (thread_connectManager, NULL);

    acquisitionOrb->orbShutdown ();
  }

};
