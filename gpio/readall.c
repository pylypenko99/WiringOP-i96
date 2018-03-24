/*
 * readall.c:
 *	The readall functions - getting a bit big, so split them out.
 *	Copyright (c) 2012-2013 Gordon Henderson
 ***********************************************************************
 * This file is part of wiringPi:
 *	https://projects.drogon.net/raspberry-pi/wiringpi/
 *
 *    wiringPi is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Lesser General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    wiringPi is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Lesser General Public License for more details.
 *
 *    You should have received a copy of the GNU Lesser General Public License
 *    along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 ***********************************************************************
 */


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <wiringPi.h>

#ifdef CONFIG_ORANGEPI
#include "OrangePi.h"
#endif

extern int wpMode;

#ifndef TRUE
#define       TRUE    (1==1)
#define       FALSE   (1==2)
#endif

#ifndef CONFIG_ORANGEPI

static char *physNames [64] =
{
  NULL,

  "      0v", "0v      ",
  "    CTS2", "PWRKEY  ",
  "    TxD2", "RST     ",
  "    RxD2", "SPI2CLK ",
  "    RTS2", "SPI2DI  ",
  "    TxD1", "SPI2CS1 ",
  "    RxD1", "SPI2DIO ",
  "    SCL2", "I2SLRCK ",
  "    SDA2", "I2SBCK  ",
  "    SCL3", "I2S_DO  ",
  "    SDA3", "I2S_DI_0",
  "  GPIO.1", "GPIO.2  ",
  "  GPIO.3", "GPIO.4  ",
  "    CTS3", "RTS3    ",
  "  GPIO.5", "GPIO.6  ",
  "  GPIO.7", "GPIO.8  ",
  "  GPIO.9", "GPIO.10 ",
  "    2.8v", "NC      ",
  "      5v", "NC      ",
  "      0v", "0v      ",
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
       NULL, NULL,
	   NULL, NULL,
	   NULL, NULL,
   NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,
};
#endif

#define ALT_COUNT 8
static char *alts[] =
{
  "IN", "OUT", "ALT5", "ALT4", "ALT0", "ALT1", "ALT2", "ALT3"
};

/*
 * doReadallExternal:
 *	A relatively crude way to read the pins on an external device.
 *	We don't know the input/output mode of pins, but we can tell
 *	if it's an analog pin or a digital one...
 */

static void doReadallExternal(void)
{
	int pin;

	printf("+------+---------+--------+\n");
	printf("|  Pin | Digital | Analog |\n");
	printf("+------+---------+--------+\n");

	for (pin = wiringPiNodes->pinBase; pin <= wiringPiNodes->pinMax; ++pin)
		printf ("| %4d |  %4d   |  %4d  |\n", pin, digitalRead (pin), analogRead (pin));

	printf ("+------+---------+--------+\n");
}

/*
 * readallPhys:
 *	Given a physical pin output the data on it and the next pin:
 * | BCM | wPi |   Name  | Mode | Val| Physical |Val | Mode | Name    | wPi | BCM |
 */

void readallPhys(int physPin)
{
#ifdef CONFIG_ORANGEPI
	if (physPin < 0 || physPin > GPIO_PHY_NUM)
		return;
#endif

	int pin;
	int val;

	(physPinToGpio(physPin) >= 0) ? printf(" | %3d ", physPinToGpio(physPin)) : printf(" |     ");
	(physPinToPin(physPin) >= 0) ? printf(" | %3d ", physPinToPin(physPin)) : printf(" |     ");
	
	printf(" | %s", physNames[physPin]);

	if (physPinToPin(physPin) >= 0)
	{
		if (wpMode == WPI_MODE_GPIO)
			pin = physPinToGpio(physPin);
		else if (wpMode == WPI_MODE_PHYS)
			pin = physPin;
		else if (wpMode == WPI_MODE_PINS)
			pin = physPinToPin(physPin);
		else
			pin = -1;

		int alt = getAlt(pin);
		(alt >= 0 && alt < ALT_COUNT) ? printf(" | %4s", alts[alt]) : printf(" | INVALID");
		
		val = digitalRead(pin);
		if (val == -1)
			printf(" | 0");
		else
			printf(" | %d", val);
	}
	else
	{
		printf(" |      |  ");
	}
		
	
	/* Pin numbers: */
	printf(" | %2d", physPin);
	++physPin;
	printf(" || %-2d", physPin);

	/* Same, reversed */

	if (physPinToPin(physPin) >= 0)
	{
		if (wpMode == WPI_MODE_GPIO)
			pin = physPinToGpio(physPin);
		else if (wpMode == WPI_MODE_PHYS)
			pin = physPin;
		else if (wpMode == WPI_MODE_PINS)
			pin = physPinToPin(physPin);
		else
			pin = -1;
				

		val = digitalRead(pin);
		if (val == -1)
			printf(" | 0");
		else
			printf(" | %d", val);

		int alt = getAlt(pin);
		(alt >= 0 && alt < ALT_COUNT) ? printf(" | %-4s", alts[alt]) : printf(" | INVALID");
	}
	else
	{
		printf(" |   |     ");
	}

	printf (" | %-5s", physNames[physPin]);

	(physPinToPin(physPin) >= 0) ? printf(" | %-3d ", physPinToPin(physPin)) : printf(" |     ");
	(physPinToGpio(physPin) >= 0) ? printf(" | %-3d ", physPinToGpio(physPin)) : printf(" |     ");
	
	printf(" |\n");
}

/*
 * doReadall:
 *	Read all the GPIO pins
 *	We also want to use this to read the state of pins on an externally
 *	connected device, so we need to do some fiddling with the internal
 *	wiringPi node structures - since the gpio command can only use
 *	one external device at a time, we'll use that to our advantage...
 *********************************************************************************
 */
void cmReadall (void)
{
	int pin ;

	printf("+-----+------+-------+      +-----+------+-------+\n");
	printf("| Pin | Mode | Value |      | Pin | Mode | Value |\n");
	printf("+-----+------+-------+      +-----+------+-------+\n");

	for (pin = 0; pin < 28; ++pin) {
		printf("| %3d ", pin);
		printf("| %-4s ", alts[getAlt(pin)]);	
		printf("| %s  ", digitalRead(pin) == HIGH ? "High" : "Low ");
		printf("|      ");
		printf("| %3d ", pin + 28);
		printf("| %-4s ", alts[getAlt(pin + 28)]);
		printf("| %s  ", digitalRead(pin + 28) == HIGH ? "High" : "Low ");
		printf("|\n");
	}
	printf("+-----+------+-------+      +-----+------+-------+\n");
}


/*
 * abReadall:
 *	Read all the pins on the model A or B.
 */

void abReadall (int model, int rev)
{
	int pin;
	char *type;

	if (model == PI_MODEL_A)
		type = " A";
	else
		if (rev == PI_VERSION_2)
			type = "B2";
		else
			type = "B1";

	printf(" +-----+-----+---------+------+---+-Model %s-+---+------+---------+-----+-----+\n", type);
	printf(" | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |\n");
	printf(" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n");

	for (pin = 1; pin <= 26; pin += 2)
		readallPhys(pin);

	if (rev == PI_VERSION_2) {
		printf(" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n");
		for (pin = 51; pin <= 54; pin += 2)
		readallPhys(pin);
	}

	printf(" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n");
	printf(" | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |\n");
	printf(" +-----+-----+---------+------+---+-Model %s-+---+------+---------+-----+-----+\n", type);
}


/*
 * bPlusReadall:
 *	Read all the pins on the model B+
 */

void bPlusReadall (void)
{
	int pin;

	printf(" +-----+-----+---------+------+---+--B Plus--+---+------+---------+-----+-----+\n");
	printf(" | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |\n");
	printf(" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n");
  
	for (pin = 1; pin <= 40; pin += 2)
		readallPhys (pin);
	
	printf(" +-----+-----+---------+------+---+----++----+---+------+---------+-----+-----+\n");
	printf(" | BCM | wPi |   Name  | Mode | V | Physical | V | Mode | Name    | wPi | BCM |\n");
	printf(" +-----+-----+---------+------+---+--B Plus--+---+------+---------+-----+-----+\n");
}


/*
 * Display WiringPi layer.
 */
void doReadall (void)
{
	int model, rev, mem, maker, overVolted;

	if (wiringPiNodes != NULL) {
		doReadallExternal();
		return;
	}

	piBoardId(&model, &rev, &mem, &maker, &overVolted);

	if ((model == PI_MODEL_A) || (model == PI_MODEL_B))
		abReadall(model, rev);
	else if (model == PI_MODEL_CM)
		cmReadall();
	else if (model == PI_MODEL_ORANGEPI)
		OrangePiReadAll();
	else
		printf ("Oops - unable to determine board type... model: %d\n", model);
}
