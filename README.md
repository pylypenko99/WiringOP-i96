   WiringPi for OrangePi i96 is not included [official OrangePi Repo](https://github.com/OrangePiLibra/WiringPi) at the moment. I committed [the code](https://github.com/JYKeith123/WiringPi/commit/8200c4bc32b7dd52496b5d33df1b25311b771924) for OrangePi i96 from [official debian image](http://www.orangepi.org/downloadresources/orangepii96/orangepii96_75fdb65e681d4439f14e4531.html). but still didn't work.

   The purpose of this Repo is for me to make basic GPIO functionality works at least. I'm very new to using WringPi and am not guarantee code stability. Use at your own risk.

### How to build and install

```sh
$ ./build clean
$ ./build OrangePi_i96
$ ./build OrangePi_i96 install
```

### Resouces and facts
1. pin numbering
there are 3 pin numbering data in WiringPi/OrangePi.c.

| variable name | what I understand |
| ------ | ------ |
| physToGPIO- | physical pin number from board to SoC GPIO numbering (this is called BCM for raspberypi. this could be changed by manufacturer). You can access GPIO from filesystem /sys/class/gpio/ with this pin number. I tested [physToGpioOrangePi](https://github.com/JYKeith123/WiringPi/commit/8200c4bc32b7dd52496b5d33df1b25311b771924) for i96 from debian image seems correct.|
| pinToGPIO- | WiringPi numbering to SoC GPIO numbering |
| physToPin | Physical pin number to WiringPi numbering. you can change this. Gorden who made WiringPi explained what he intended about numbering for raspberry pi in [this link](http://wiringpi.com/pins/)  |


2. I have only one i96 and tested GPIO with Soc GPIO number via shell like below.
```sh
$ echo "out" > /sys/class/gpio/gpio3/direction
$ echo "0" > /sys/class/gpio/gpio3/value
$ echo "1" > /sys/class/gpio/gpio3/value
```
I tested simple blinking LED with GPIO and found working GPIOs. I'm not sure if all i96s are same with my one i96.
![Alt text](http://drive.google.com/uc?export=view&id=1hhYOHaDVrTkZ2CvuBzIEt1kpB1yy0er5)
green colored is working GPIO.
red colored is always High.
light red colored is also always High but week LED light.
(I don't know why this happen. I checked voltage and it's equivalant voltage with red colored (output volatage of GPIO : 2.9v, resistor : 270¥Ø))
No color is GPIO not working except power source and ground.

3. On 90 page of [this chip datasheet](https://www.raspberrypi.org/documentation/hardware/raspberrypi/bcm2835/BCM2835-ARM-Peripherals.pdf) for raspberry pi, you can see physical addresses to access GPIO. And 45 page of [RDA8810PL datasheet](https://www.google.co.kr/url?sa=t&rct=j&q=&esrc=s&source=web&cd=3&ved=0ahUKEwitiPriuoLaAhXClpQKHcHTBhcQFgg3MAI&url=https%3A%2F%2Fgithub.com%2Faib%2Fopi2g-utils%2Ffiles%2F1232725%2F386cd004-3df5-48aa-9567-2f111779377e.pdf&usg=AOvVaw2hhhEKrpX5m27F4evme8kS) is for 2G IoT and i96 which is quite different with raspberry pi IOC.
So specific codes for OrangePi are written in wringPi/OrangePi.c.
I found some information from [the uncommited code before](https://github.com/JYKeith123/WiringPi/commit/8200c4bc32b7dd52496b5d33df1b25311b771924). 
- 2G IoT and i96 has 4 Group of GPIO. A, B, C, D.
- Group C is special (differenct memory address and access point). I think this is for alternative functionality because RDA8810 seems doesn't support ALT mode for GPIO basically unlike raspberry pi.
Group C has differenct access point from path /sys/devices/platform/rda-gpioc/, but it doesn't work.
- each group has 32 GPIO.

| group | Soc GPIO number | memory address (see 45 page of [this](https://www.google.co.kr/url?sa=t&rct=j&q=&esrc=s&source=web&cd=3&ved=0ahUKEwitiPriuoLaAhXClpQKHcHTBhcQFgg3MAI&url=https%3A%2F%2Fgithub.com%2Faib%2Fopi2g-utils%2Ffiles%2F1232725%2F386cd004-3df5-48aa-9567-2f111779377e.pdf&usg=AOvVaw2hhhEKrpX5m27F4evme8kS))|
| ------ | ------ | ------ |
|A | 0~31 | 0x20930000 |
|B | 32~63 | 0x20931000 |
|C | 64~95 | 0x11A08000 |
|D | 96~127 | 0x20932000 |

- You can control GPIO of each group via memory address above.
- It seems that pwm, clk, pad are not supported.



wiringPi README
===============

Please note that the official way to get wiringPi is via git from
git.drogon.net and not GitHub.

ie.

  git clone git://git.drogon.net/wiringPi

The version of wiringPi held on GitHub by "Gadgetoid" is used to build the
wiringPython, Ruby, Perl, etc. wrappers for these other languages. This
version may lag the official Drogon release.  Pull requests may not be
accepted to Github....

Please see

  http://wiringpi.com/

for the official documentation, etc. and the best way to submit bug reports, etc.
is by sending an email to projects@drogon.net

Thanks!

  -Gordon