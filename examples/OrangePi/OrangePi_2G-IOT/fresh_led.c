/*
 * button.c:
 *   This tests the hardware button.
 *   
 *   Copyright (c) 2017 OrangePi
 *
 * wiringPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *           
 * wiringPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *                
 * You should have received a copy of the GNU Lesser General Public License
 * along with wiringPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <wiringPi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define GPIO1         1
#define BUTTON        GPIO1

/*
 * LED Fresh
 */
void led_fresh(int value)
{
	int fd;
	char buf[20];

	fd = open("/sys/class/leds/red-flash/trigger", O_RDWR);
	if (fd < 0) {
		printf("ERROR: can't open LED\n");
		return;
	}

	memset(buf, 0, sizeof(buf));
	if (value == 1)
		strcpy(buf, "rfkill0");
	else
		strcpy(buf, "rfkill1");

	write(fd, buf, strlen(buf));

	close(fd);
}


int main()
{
	int val;

	for (;;) {
		sleep(1);
		led_fresh(1);
		sleep(1);
		led_fresh(0);
		sleep(1);
	}

	return 0;
}
