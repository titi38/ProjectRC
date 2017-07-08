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
 * Name        : I2C.h
 * Author      : Georgi Todorov
 * Version     :
 * Created on  : Dec 30, 2012
 *
 * Copyright © 2012 Georgi Todorov  <terahz@geodar.com>
 */

#ifndef I2C_H_
#define I2C_H_
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>      /* Standard I/O functions */
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <syslog.h>		/* Syslog functionality */
#include <inttypes.h>

#define BUFFER_SIZE 0x01  //1 byte buffer


class I2C {
public:
	I2C(int bus, int address) {
		_i2cbus = bus;
		_i2caddr = address;
		snprintf(busfile, sizeof(busfile), "/dev/i2c-%d", bus);
		openfd();
	};

	virtual ~I2C() {
		close(fd);
	};

	uint8_t dataBuffer[BUFFER_SIZE];

	//! Read a single byte from I2C Bus
	/*!
	 \param address register address to read from
	 */
	uint8_t read_byte(uint8_t address) {
		if (fd != -1) {
			uint8_t buff[BUFFER_SIZE];
			buff[0] = address;
			if (write(fd, buff, BUFFER_SIZE) != BUFFER_SIZE) {
				syslog(LOG_ERR, "I2C slave 0x%x failed to go to register 0x%x [read_byte():write %d]",
						_i2caddr, address, errno);
				return (-1);
			} else {
				if (read(fd, dataBuffer, BUFFER_SIZE) != BUFFER_SIZE) {
					syslog(LOG_ERR, "Could not read from I2C slave 0x%x, register 0x%x [read_byte():read %d]",
							_i2caddr, address, errno);
					return (-1);
				} else {
					return dataBuffer[0];
				}
			}
		} else {
			syslog(LOG_ERR, "Device File not available. Aborting read");
			return (-1);
		}

	};

	//! Write a single byte from a I2C Device
	/*!
	 \param address register address to write to
	 \param data 8 bit data to write
	 */
	uint8_t write_byte(uint8_t address, uint8_t data) {
		if (fd != -1) {
			uint8_t buff[2];
			buff[0] = address;
			buff[1] = data;
			if (write(fd, buff, sizeof(buff)) != 2) {
				syslog(LOG_ERR,
						"Failed to write to I2C Slave 0x%x @ register 0x%x [write_byte():write %d]",
						_i2caddr, address, errno);
				return (-1);
			} else {
				syslog(LOG_INFO, "Wrote to I2C Slave 0x%x @ register 0x%x [0x%x]",
						_i2caddr, address, data);
				return (-1);
			}
		} else {
			syslog(LOG_INFO, "Device File not available. Aborting write");
			return (-1);
		}
		return 0;
	};

private:
	int _i2caddr;
	int _i2cbus;

	//! Open device file for I2C Device
	void openfd() {
		if ((fd = open(busfile, O_RDWR)) < 0) {
			syslog(LOG_ERR, "Couldn't open I2C Bus %d [openfd():open %d]", _i2cbus,
					errno);
		}
		if (ioctl(fd, I2C_SLAVE, _i2caddr) < 0) {
			syslog(LOG_ERR, "I2C slave %d failed [openfd():ioctl %d]", _i2caddr,
					errno);
		}
	};

	char busfile[64];
	int fd;
};

#endif /* I2C_H_ */
