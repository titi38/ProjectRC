/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Name        : PCA9685.h
 * Author      : Georgi Todorov
 * Version     :
 * Created on  : Dec 9, 2012
 *
 * Copyright © 2012 Georgi Todorov  <terahz@geodar.com>
 */

#ifndef _PCA9685_HH
#define _PCA9685_HH
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdio.h>      /* Standard I/O functions */
#include <fcntl.h>
#include <syslog.h>		/* Syslog functionallity */
#include <inttypes.h>
#include <errno.h>
#include <math.h>
#include <inttypes.h>
#include "I2C.hh"

// Register Definitions

#define MODE1 0x00			//Mode  register  1
#define MODE2 0x01			//Mode  register  2
#define SUBADR1 0x02		//I2C-bus subaddress 1
#define SUBADR2 0x03		//I2C-bus subaddress 2
#define SUBADR3 0x04		//I2C-bus subaddress 3
#define ALLCALLADR 0x05     //LED All Call I2C-bus address
#define LED0 0x6			//LED0 start register
#define LED0_ON_L 0x6		//LED0 output and brightness control byte 0
#define LED0_ON_H 0x7		//LED0 output and brightness control byte 1
#define LED0_OFF_L 0x8		//LED0 output and brightness control byte 2
#define LED0_OFF_H 0x9		//LED0 output and brightness control byte 3
#define LED_MULTIPLYER 4	// For the other 15 channels
#define ALLLED_ON_L 0xFA    //load all the LEDn_ON registers, byte 0 (turn 0-7 channels on)
#define ALLLED_ON_H 0xFB	//load all the LEDn_ON registers, byte 1 (turn 8-15 channels on)
#define ALLLED_OFF_L 0xFC	//load all the LEDn_OFF registers, byte 0 (turn 0-7 channels off)
#define ALLLED_OFF_H 0xFD	//load all the LEDn_OFF registers, byte 1 (turn 8-15 channels off)
#define PRE_SCALE 0xFE		//prescaler for output frequency
#define CLOCK_FREQ 25000000.0 //25MHz default osc clock

//! class features for PCA9685 chip
class PCA9685 {
public:
	PCA9685(int bus, int address) {
		i2c = new I2C(bus,address);
	};

	virtual ~PCA9685() {
		delete i2c;
	};

	//! Set the frequency of PWM
	/*!
	 \param freq desired frequency. 40Hz to 1000Hz using internal 25MHz oscillator.
	 */
	void setPWMFreq(int freq) {

		uint8_t prescale_val = (CLOCK_FREQ / 4096 / freq)  - 1;
		i2c->write_byte(MODE1, 0x10); //sleep
		i2c->write_byte(PRE_SCALE, prescale_val); // multiplyer for PWM frequency
		i2c->write_byte(MODE1, 0x80); //restart
		i2c->write_byte(MODE2, 0x04); //totem pole (default)
	}

	//! PWM a single channel
	/*!
	 \param led channel (1-16) to set PWM value for
	 \param value 0-4095 value for PWM
	 */
	void setPWM(uint8_t led, int value) {
		setPWM(led, 0, value);
	}
	//! PWM a single channel with custom on time
	/*!
	 \param led channel (1-16) to set PWM value for
	 \param on_value 0-4095 value to turn on the pulse
	 \param off_value 0-4095 value to turn off the pulse
	 */
	void setPWM(uint8_t led, int on_value, int off_value) {
		i2c->write_byte(LED0_ON_L + LED_MULTIPLYER * (led - 1), on_value & 0xFF);
		i2c->write_byte(LED0_ON_H + LED_MULTIPLYER * (led - 1), on_value >> 8);
		i2c->write_byte(LED0_OFF_L + LED_MULTIPLYER * (led - 1), off_value & 0xFF);
		i2c->write_byte(LED0_OFF_H + LED_MULTIPLYER * (led - 1), off_value >> 8);
	}

	int getPWM(uint8_t led){
		int ledval = 0;
		ledval = i2c->read_byte(LED0_OFF_H + LED_MULTIPLYER * (led-1));
		ledval = ledval & 0xf;
		ledval <<= 8;
		ledval += i2c->read_byte(LED0_OFF_L + LED_MULTIPLYER * (led-1));
		return ledval;
	}
        void reset() {
		i2c->write_byte(MODE1, 0x00); //Normal mode
		i2c->write_byte(MODE2, 0x04); //totem pole

	}

private:
	I2C *i2c;
};
#endif

