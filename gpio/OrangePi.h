#ifndef _ORANGEPI_H_
#define _ORANGEPI_H_

#define GPIO_PHY_NUM	40 // total count of physical gpio

extern char *physNames[64];
extern int physToWpi[64];

extern void OrangePiReadAll(void);
extern void readallPhys(int physPin);

#endif
