//------------------------------------------------------------------------------
//
// RCCar.cc : the RCCar's process
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
#include <unistd.h>
#include <stdio.h>

#include "libnavajo/nvjThread.h"

#include "RCCar.hh"


#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


using namespace std;

ProjectRC::RCCar *rcCar;

bool ProjectRC::RCCar::connectedToDaq = false;
bool ProjectRC::RCCar::rcCarIsShutingdown = false;

/************************************************************************/

void printHelp ( const char *execname )
{
  printf ("Usage:\n");
  printf ("  %s [OPTION...] X Y Z\n\n", execname);
  printf ("Options:\n");
  printf ("  -h \t\t Show help options\n");
//    printf("  -f \t\t no hardware mode - for test purpose only\n");
  printf ("  -C FILE\t Use given configuration file\n");

  fflush (NULL);
}


int main ( int argc, char **argv )
{
  int c;

  char configFile[300] = "rccar.conf";

  char title[40], decotitle[40];
  snprintf (title, 40, "RCCar Software v%s", RCCAR_VERSION);
  memset (decotitle, '*', strlen (title) + 4);
  decotitle[ strlen (title) + 4 ] = '\0';
  printf ("\n%s\n* %s *\n%s\n", decotitle, title, decotitle);
  printf ("Copyright 2017 by T.Descombes <thierry.descombes@gmail.com>\n\n");


  while (( c = getopt (argc - 3, argv, "hicC:")) != -1 )
  {
    switch ( c )
    {
      case 'h':
        printHelp (argv[ 0 ]);
        exit (0);

      case 'C':
        snprintf (configFile, sizeof configFile, "%s", optarg);
        break;

      default:
        fprintf (stderr, "Invalid option: run '%s -h' to see a full list of available command line options\n", argv[ 0 ]);
        exit(EXIT_FAILURE);
    }
  }

  ProjectRC::RCCar car ( configFile );

  return 0;
}

/************************************************************************/

namespace ProjectRC
{

  RCCarItf_ptr RCCar::rcCarItfRef = NULL;
  RCControlerItf_ptr RCCar::rcControlerItfRef = NULL;

  // Reduction des log par une "barre de progression" pour la connexion CORBA

  RCCar::~RCCar ()
  {
  }

  /***********************************************************************/
  /* RCCar(): Le constructeur de la classe                              */
  /***********************************************************************/
  RCCar::RCCar ( const char *configFile )
  {
    // acquisitionOrb est l'instance unique de la classe d'accès à l'ORB
    acquisitionOrb = DaqApplication::AcquisitionORB::getInstance ();


    /********************************************************************/
    // Lecture du fichier de conf
    /********************************************************************/

    FILE *configfile;
    char configLine[120];
    int ligne = 0;

    refNameRCControlerServant = NULL;

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
        bool isARCCarOption = false;
        bool isACorbaOption = acquisitionOrb->readCorbaOption (configLine);

        if ( !isACorbaOption )
        {
          /************************/
          /*   RCCar options   */
          /************************/

          char *value;

          /* looks for the line containing "refNameRCControlerServant" */
          if (( value = strstr (configLine, "refNameRCControlerServant=\"")) != NULL )
          {
            int len = rindex (value, '\"') - index (value, '\"') - 1;
            if ( len == -1 )
            {
              fprintf (stderr, "\nERROR: refNameRCControlerServant => interminated string in rccar.conf\n");
              fflush (NULL);
              exit (EXIT_FAILURE);
            }

            refNameRCControlerServant = (char *) malloc (( len + 1 ) * sizeof (char));
            strncpy (refNameRCControlerServant, value + strlen ("refNameRCControlerServant=\""), len);
            *( refNameRCControlerServant + len ) = '\0';

            printf ("* Set refNameRCControlerServant to \'%s\'\n", refNameRCControlerServant);
            isARCCarOption = true;
          }

        }

        if ( !isARCCarOption && !isACorbaOption )
        {
          fprintf (stderr, "\nERROR: Line %d, unknown option %s\n", ligne, configLine);
          fflush (NULL);
          exit (EXIT_FAILURE);
        }
      }
    }

    fclose (configfile);

    /************************** Fin lecture *****************************/

    if ( refNameRCControlerServant == NULL )
    {
      fprintf (stderr, "\nERROR: refNameRCControlerServant undefined in rccar.conf\n");
      fflush (NULL);
      exit (EXIT_FAILURE);
    }

    printf ("\n\n");
    fflush (NULL);

    /********************************************************************/


    rcCarIsShutingdown = false;

    connectedToDaq = false;
    pthread_mutex_init (&p_mut_connectManager, NULL);
    pthread_cond_init (&p_cond_connectManager, NULL);
    create_thread (&( this->thread_connectManager ), RCCar::startingThreadConnectionManager,
                   static_cast<void *>(this));


    /*******************************************************************/

    if ( initialize ()) exit (0);

    acquisitionOrb->orbRun ();
  }

  /***********************************************************************/

  void RCCar::reportToDaq ( const LogKind kind, const char *msg )
  {
    LogMessage lmsg;

    switch ( kind )
    {
      case INFO:
        fprintf (stderr, "INFO: ");
        break;
      case WARNING:
        fprintf (stderr, "WARNING: ");
        break;
      case ERROR:
        fprintf (stderr, "ERROR: ");
        break;
      case FATAL:
        fprintf (stderr, "FATAL: ");
        break;
    }

    fprintf (stderr, "%s\n", msg);

    lmsg.kind = kind;
    lmsg.message = CORBA::string_dup (msg);
    if ( connectedToDaq )
      try { rcControlerItfRef->logReport ( lmsg ); }
      catch ( ... ) { fprintf (stderr, "An exception occurs when invoking the distributed method reportToDaq()\n"); }
  }

  /***********************************************************************/
  /* initialize():  Appelé par le constructeur de la classe              */
  /* Instantie et publie sur l'ORB l'objet distribué contenant           */
  /* l'implémentation de l'interface Corba rcCarItf                      */
  /***********************************************************************/

  bool RCCar::initialize ( void )
  {
    if ( acquisitionOrb->initORB ()) return 1;
    PortableServer::POA_ptr poa = acquisitionOrb->getPOA ();
    if ( CORBA::is_nil (poa)) return 1;
    if ( acquisitionOrb->activatePOAMgr (poa)) return 1;

    // Création des instances d'interfaces
    rcCarItfServant = new RCCarItf_i (this);

    // On publie les objets distribués sur l'ORB

    PortableServer::ObjectId_var oid = poa->activate_object (rcCarItfServant);
    rcCarItfRef = rcCarItfServant->_this ();
    rcCarItfServant->_remove_ref ();

    return 0;
  }

  /***********************************************************************/
  /* getDaqObjRefFromNS(): Appelé par le constructeur de la classe       */
  /* Permet de récupérer dans le NameService CORBA, la référence de      */
  /* de l'interface SensorToDaq grâce à son nom:                         */
  /*      "refNameRCControlerServant" suivit du numéro de supermodule    */
  /***********************************************************************/

  void RCCar::getDaqObjRefFromNS ()
  {
    CORBA::Object_ptr objref;

    char refname[30];
    if ( snprintf (refname, 30, "%s", refNameRCControlerServant) == -1 )
    {
      printf ("refNameRCControlerServant is too long !\n");
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

    rcControlerItfRef = RCControlerItf::_narrow (objref);
    CORBA::release (objref);

    printf ("\nAn object Reference recorded as %s into the NameService has been found.\n", refname);
    fflush (NULL);
  }

  /***********************************************************************/
  /* connectTheDaq(): Appeler par le constructeur de la classe           */
  /* Invoque la méthode connectSensor() du Daq, qui permet d'enregister  */
  /* un nouveau capteur.                                                 */
  /***********************************************************************/

  void RCCar::connectTheDaq ()
  {
    bool connected = false;

    do
    {
      getDaqObjRefFromNS ();

      try
      {
        rcControlerItfRef->connect ( rcCarItfRef );
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
        CORBA::release (rcControlerItfRef);
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

  void *RCCar::startingThreadConnectionManager ( void *car )
  {
    static_cast<RCCar *>(car)->processConnectionManagerThread ();
    pthread_exit (0);
    return NULL;
  }

 /***********************************************************************/

  void RCCar::processConnectionManagerThread ()
  {
    int retcode;
    struct timeval now;
    struct timespec timeout;
    int in = 0;
    bool isHTmutexLocked = false;

    do
    {
      pthread_mutex_lock (&p_mut_connectManager);

      while ( connectedToDaq && !rcCarIsShutingdown )
      {
        gettimeofday (&now, NULL);
        timeout.tv_sec = now.tv_sec + 5; // Toutes les 5s on ping !
        timeout.tv_nsec = now.tv_usec * 1000;

        retcode = pthread_cond_timedwait (&p_cond_connectManager, &p_mut_connectManager, &timeout);
        if ( retcode == ETIMEDOUT )
          try
          {
            rcControlerItfRef->ping ();
          }
          catch ( ... )
          {
            connectedToDaq = false;
            CORBA::release (rcControlerItfRef);
          }
      };

      pthread_mutex_unlock (&p_mut_connectManager);

      if ( rcCarIsShutingdown ) break;

      sleep (1);

      connectTheDaq ();

    }
    while ( !rcCarIsShutingdown );

    printf ("processConnectionManager has finished.\n");
    pthread_exit (NULL);
  }


  /***********************************************************************/

  void RCCar::disconnect ()
  {
    printf ("RCCar was disconnected by its Controler\n");
    fflush (NULL);

    pthread_mutex_lock (&p_mut_connectManager);
    CORBA::release (rcControlerItfRef);

    connectedToDaq = false;

    pthread_cond_broadcast (&p_cond_connectManager);

    pthread_mutex_unlock (&p_mut_connectManager);
  }


  /***********************************************************************/

  void RCCar::shutdown ()
  {
    acquisitionOrb->orbShutdown ();
  }

  /***********************************************************************/

  void RCCar::turnRight(::CORBA::UShort percent) throw (FailException)
  {
    carControl.turnRight(percent);
  }

  /***********************************************************************/

  void RCCar::turnLeft(::CORBA::UShort percent) throw (FailException)
  {
    carControl.turnLeft(percent);
  }

  /***********************************************************************/

  void RCCar::runReverse(::CORBA::UShort percent) throw (FailException)
  {
    carControl.runReverse(percent);
  }

  /***********************************************************************/

  void RCCar::runForward(::CORBA::UShort percent) throw (FailException)
  {
    carControl.runForward(percent);
  }

  /***********************************************************************/

};


/*







int main()
{
  CarControl cc;

  
  // TEST DIRECTION
  printf("TEST DIRECTION\n");
  printf("--------------\n\n"); fflush(NULL);

  printf("Turn Left: 50%%\n"); fflush(NULL);
  cc.turnLeft(50);
  sleep(1);

  printf("Turn Left: 100%%\n"); fflush(NULL);
  cc.turnLeft(100);
  sleep(1);

  printf("Center\n"); fflush(NULL);
  cc.turnLeft(0);
  sleep(5);

  for (int i=100; i>=0; i--)
  {
    cc.turnLeft(i);
    usleep(100000);
  }
  sleep(1);

  printf("Turn Right: 50%%\n"); fflush(NULL);
  cc.turnRight(50);
  sleep(1);

  printf("Turn Right: 100%%\n"); fflush(NULL);
  cc.turnRight(100);
  sleep(1);

  printf("Center\n"); fflush(NULL);
  cc.turnRight(0);
  sleep(5);

  for (int i=100; i>=0; i--)
  {
    cc.turnRight(i);
    usleep(100000);
  }
  sleep(1);
  
  // TEST THROTTLE
  printf("TEST THROTTLE\n");
  printf("--------------\n\n"); fflush(NULL);

  printf("Forward: 50%%\n"); fflush(NULL);
  cc.runForward(50);
  sleep(1);

  printf("Forward: 100%%\n"); fflush(NULL);
  cc.runForward(100);
  sleep(1);

  printf("Stopped\n"); fflush(NULL);
  cc.runForward(0);
  sleep(5);

  for (int i=90; i>=0; i--)
  {
    cc.runForward(i);
    usleep(100000);
  }
  sleep(1);


  printf("Backward: 50%%\n"); fflush(NULL);
  cc.runReverse(50);
  sleep(1);

  printf("Backward: 100%%\n"); fflush(NULL);
  cc.runReverse(100);
  sleep(1);

  printf("Stopped\n"); fflush(NULL);
  cc.runReverse(0);
  sleep(5);

  for (int i=90; i>=0; i--)
  {
    cc.runReverse(i);
    usleep(100000);
  }
  sleep(1);



  return 0;
}*/
