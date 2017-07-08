//------------------------------------------------------------------------------
//
// RCControler.cc : the RCControler's process
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

#include "RCControler.hh"


#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"


using namespace std;

ProjectRC::RCControler *rcControler;


/************************************************************************/

void printHelp ( const char *execname )
{
  printf ("Usage:\n");
  printf ("  %s [OPTION...] X Y Z\n\n", execname);
  printf ("Options:\n");
  printf ("  -h \t\t Show help options\n");
  printf ("  -C FILE\t Use given configuration file\n");

  fflush (NULL);
}

/************************************************************************/

int main ( int argc, char **argv )
{
  int c;

  char configFile[300] = "rccontrol.conf";

  char title[40], decotitle[40];
  snprintf (title, 40, "RCControler Software v%s", RCCAR_VERSION);
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

  // connect various signals to our clean-up function
  signal( SIGTERM, forceCleanExit );
  signal( SIGINT, forceCleanExit );
  signal( SIGQUIT, forceCleanExit );

  ProjectRC::RCControler controler ( configFile );

  return 0;
}


/***********************************************************************/

void forceCleanExit ( int dummy )
{
  DaqApplication::AcquisitionORB::getInstance()->orbShutdown();
}


  /***********************************************************************/

 

namespace ProjectRC
{


  // Reduction des log par une "barre de progression" pour la connexion CORBA

  RCControler::~RCControler ()
  {
  }

  /***********************************************************************/
  /* RCControler(): Le constructeur de la classe                              */
  /***********************************************************************/
  RCControler::RCControler ( const char *configFile )
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
        bool isARCControlerOption = false;
        bool isACorbaOption = acquisitionOrb->readCorbaOption (configLine);

        if ( !isACorbaOption )
        {
          /************************/
          /*   RCControler options   */
          /************************/

          char *value;

          /* looks for the line containing "refNameRCControler" */
          if (( value = strstr (configLine, "refNameRCControler=\"")) != NULL )
          {
            int len = rindex (value, '\"') - index (value, '\"') - 1;
            if ( len == -1 )
            {
              fprintf (stderr, "\nERROR: refNameRCControler => interminated string in rccar.conf\n");
              fflush (NULL);
              exit (EXIT_FAILURE);
            }

            refNameRCControlerServant = (char *) malloc (( len + 1 ) * sizeof (char));
            strncpy (refNameRCControlerServant, value + strlen ("refNameRCControler=\""), len);
            *( refNameRCControlerServant + len ) = '\0';

            printf ("* Set refNameRCControler to \'%s\'\n", refNameRCControler);
            isARCControlerOption = true;
          }

        }

        if ( !isARCControlerOption && !isACorbaOption )
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
      fprintf (stderr, "\nERROR: refNameRCControlerServant undefined in rccontrol.conf\n");
      exit (EXIT_FAILURE);
    }

    printf ("\n\n");
    fflush (NULL);


    if ( !initJoystick() )
    {
      fprintf (stderr, "\nERROR: Joystick not found\n");
      exit (EXIT_FAILURE);
    }

    // ************************
    // * Initialisation CORBA *
    // ************************

    // acquisition ORB et l'instance unique de la classe d'acc�s � l'ORB
    acquisitionOrb=DaqApplication::AcquisitionORB::getInstance();

    if ( ! corbaInit() )
    {
      fprintf (stderr, "\nERROR: ORB initialisation failed\n");
      exit (EXIT_FAILURE);
    }

    if ( !publishCorbaItf() )    
      exit (EXIT_FAILURE);

    if ( !initJoystick() ) 
    {
      fprintf (stderr, "\nERROR: Joystick not found\n");
      exit (EXIT_FAILURE);
    }

    pthread_t thread_joystick;
    create_thread (&( this->thread_joystick ), RCCar::startingThreadJoystick,
                   static_cast<void *>(this));

    acquisitionProcessMgr->launchThread();

    acquisitionOrb->orbRun();
    exiting = true;

    delete RCControlerServant;
    shutdown();

    printf( "The ORB has been shuted down....\n" );
    fflush( NULL );

    delete joystick;

  }

  /***********************************************************************/

  bool RCControler::publishCorbaItf ( void )
  {
    PortableServer::POA_var poa = acquisitionOrb->getPOA();
    if ( CORBA::is_nil( poa ) ) exit( 1 );
    if ( acquisitionOrb->activatePOAMgr( poa ) ) exit( 1 );

    // Creation des instances d'interfaces
    RCControlerServant = new RCControlerItf_i( this );

    // On publie les objets distribues sur l'ORB
    PortableServer::ObjectId_var RCControlerId = poa->activate_object( RCControlerServant );
    RCControlerObjRef = RCControlerServant->_this();
    RCControlerServant->_remove_ref();

    if ( registerObjRefToNameservice() )
    {
      LOG->append( NVJ_FATAL, "Problem using the Nameservice:\nCan't bind Object to Name" );
      return false;
    }

    return true;
  }



  /***********************************************************************/

  void RCControler::corbaInit()
  {
    return ! acquisitionOrb->initORB();
  }

  /***********************************************************************/

  int RCControler::registerObjRefToNameservice(void)
  {
    // On enregistre dans le NameService la References de ttsensorToTTdaqServant

    printf("Record RCControlerServant as \'%s\' into the Corba's NameService\n", refNameRCControlerServant);

    return acquisitionOrb->bindObjectToName(RCControlerObjRef, refNameRCControlerServant);
  }

  /***********************************************************************/

  void RCControler::unregisterObjRefsToNameservice(void)
  {
    acquisitionOrb->unbindRefName( refNameRCControlerServant );
  }

  /***********************************************************************/

  void RCControler::shutdown()
  {

    unregisterObjRefsToNameservice();

    disconnectCar();


    PortableServer::POA_var poa=acquisitionOrb->getPOA();

    if (acquisitionOrb->deactivatePOAMgr(poa)) 
    {
      fprintf(stderr,"can't desactivate the POA....\n");
      fflush(NULL);
    }
    
    acquisitionOrb->orbShutdown();
  }

  /***********************************************************************/

  bool RCControler::initJoystick()
  {
    // Create an instance of Joystick
    joystick=new Joystick("/dev/input/js0");

    // Ensure that it was found and that we can use it
    return joystick->isFound();
  }

  /***********************************************************************/

  void RCControler::runJoystick()
  {
    while (!exiting)
    {
      // Restrict rate
      usleep(1000);

      // Attempt to sample an event from the joystick
      JoystickEvent event;
      if (connectedToRCCar && joystick->sample(&event))
      {
        if (event.isButton())
        {
/*        printf("Button %u is %s\n",
            event.number,
            event.value == 0 ? "up" : "down"); 
*/
        }
        else if (event.isAxis())
        {
          try
          {
            switch (event.number)
            {
              case 0:
                if (event.value>0)
                {
                  rcCarItf->turnRight(100 * event.value / 32767);
                  printf("Turn Right : %d\n", 100 * event.value / 32767);
                }
                if (event.value<0)
                {
                  rcCarItf->turnLeft(-100 * event.value / 32767);
                  printf("Turn Left : %d\n", -100 * event.value / 32767);
                }
                break;

              case 1:
                rcCarItf->runForward( 100-100* (event.value + 32768) / 65535 );
                printf("Accelerate : %d\n", 100-100* (event.value + 32768) / 65535);
                break;

              case 2:
                rcCarItf->runReverse( 100-100* (event.value + 32768) / 65535 );
                printf("Brake : %d\n", 100-100* (event.value + 32768) / 65535);
                break;

              default:
      //        printf("Axis %u is at position %d\n", event.number, event.value);
            }
          }
          catch ( CORBA::COMM_FAILURE &ex )
          {
            printf ("An exception CORBA::COMM_FAILURE has been raised\n");
            fflush (NULL);
          }
          catch ( CORBA::SystemException &ex )
          {
            printf ("An exception CORBA::SystemException has been raised\n");
            fflush (NULL);
          }
        }
      }
    }
  }

  /***********************************************************************/

  void *RCControler::startingThreadJoystick ( void *ctrl )
  {
    static_cast<RCControler *>(ctrl)->runJoystick ();
    pthread_exit (0);
    return NULL;
  }

  /***********************************************************************/

  void RCControler::connect(RCCarItf_ptr dsi)
  {
   if ( connectedToRCCar )
    {
      CORBA::release (rcCarItf);
      rcCarItf = DaqToSensorItf::_narrow (dsi);
      //throw AlreadyConnectedException ();
    }
    else
    {
      rcCarItf = DaqToSensorItf::_narrow (dsi);
      connectedToRCCar = true;
  }

  /***********************************************************************/

  void RCControler::logReport( const LogMessage& logmsg )
  {
  }

};
