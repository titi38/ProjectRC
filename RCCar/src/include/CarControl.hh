//****************************************************************************
/**
 * @file  CarControl.hh
 *
 * @brief The Car Control interface through I2C
 *
 * @author T.Descombes (descombes@lpsc.in2p3.fr)
 *
 * @version 1
 * @date 3/7/17
 */
//****************************************************************************

#ifndef CARCONTROL_HH_
#define CARCONTROL_HH_

#include "PCA9685.hh"

#define RC_REVERSE_MINVALUE 480
#define RC_REVERSE_MAXVALUE 560

#define RC_FORWARD_MINVALUE 580
#define RC_FORWARD_MAXVALUE 640

#define RC_WHEELS_FULLRIGHT 550
#define RC_WHEELS_FULLLEFT  650
#define RC_WHEELS_MIDDLEPOS 600

class CarControl 
{
    PCA9685 *pca9685;
    uint8_t dirChan;
    uint8_t throttleChan;
    bool inReverse;

  public:

    CarControl (int bus = 1, int address = 0x40, int freq = 85, uint8_t dirChan = 2, uint8_t throttleChan = 3)
    {
      pca9685 = new PCA9685(bus, address);
      pca9685->reset();
      pca9685->setPWMFreq(freq);
      this->dirChan = dirChan;
      this->throttleChan = throttleChan;
      inReverse=false;
    };

    virtual ~CarControl()
    {
      pca9685->setPWM(dirChan,0);
      pca9685->setPWM(throttleChan,0);
      delete pca9685;
    };

    void turnRight(unsigned short percent)
    {
      int value = RC_WHEELS_MIDDLEPOS + ( percent * (RC_WHEELS_FULLRIGHT - RC_WHEELS_MIDDLEPOS) ) / 100;

      pca9685->setPWM(dirChan,value);
    };

    void turnLeft(unsigned short percent)
    {
      int value = RC_WHEELS_MIDDLEPOS + ( percent * (RC_WHEELS_FULLLEFT - RC_WHEELS_MIDDLEPOS) ) / 100;

      pca9685->setPWM(dirChan,value);
    };

    void runReverse(unsigned short percent)
    {
      int value = RC_REVERSE_MINVALUE + ( percent * (RC_REVERSE_MAXVALUE - RC_REVERSE_MINVALUE) ) / 100;

      pca9685->setPWM(throttleChan,value);
      if (!inReverse)
      {
        usleep(500000);
        pca9685->setPWM(throttleChan,value);
        inReverse = true;
      }
    };

    void runForward(unsigned short percent)
    {
      int value = RC_FORWARD_MINVALUE + ( percent * (RC_FORWARD_MINVALUE - RC_FORWARD_MINVALUE) ) / 100;

      pca9685->setPWM(throttleChan,value);
      inReverse = false;
    };


};


#endif //CARCONTROL_HH_

