#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

#define	BLOCK_SIZE	(4*1024)
//#define GPIO_BASE	0x20930000
#define GPIOA_BASE                         0x20930000
#define GPIOB_BASE                         0x20931000
#define GPIOC_BASE                         0x11A08000
#define GPIOD_BASE                         0x20932000
#define GPIOE_BASE                         0x20933000
int main()
{
	int fd = -1;
	if ((fd = open("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC)) < 0)
	{
		printf("can't open /dev/mem.");
		return -1;
	}

	int block_count = 3;
	uint32_t *gpio = (uint32_t *)mmap(0, BLOCK_SIZE * block_count, PROT_READ | PROT_WRITE, MAP_SHARED, fd, GPIOA_BASE);
	if ((int32_t)(unsigned long)gpio == -1)
	{
		printf("mmap failed.");
		return -2;
	}

	for (int i = 0; i < block_count; ++i)
	{
		uint32_t *gpio_block = gpio + (i * BLOCK_SIZE / 4);
		printf("\n=============================== block : %d ============================\n"
			"------------------------\n"
			"gpio_oen_val : 0x%x\n"
			"31:0 bit | direction of GPIO n | 0 = output, 1 = input\n"
			"------------------------\n"
			"gpio_oen_set_out : 0x%x\n"
			"31:0 bit | Write '1' sets the corresponding GPIO pin as output.\n"
			"------------------------\n"
			"gpio_oen_set_in : 0x%x\n"
			"31:0 bit | Write '1' sets the corresponding GPIO pin as input.\n"
			"------------------------\n"
			"gpio_val : 0x%x\n"
			"31:0 bit | When write, update the output value. When read, get the input \n"
			"------------------------\n"
			"gpio_set : 0x%x\n"
			"31:0 bit | Write '1' will set GPIO output value. When read, get the GPIO\n"
			"------------------------\n"
			"gpio_clr : 0x%x\n"
			"31:0 bit | Write '1' clears corresponding GPIO output value. When read, get the GPIO\n"
			"------------------------\n"
			"gpint_ctrl_set : 0x%x\n"
			"0 bit		| Write '1' will set GPIO interrupt mask for rising edge and level high. When read, get the GPIO interrupt mask for rising edge and level high.\n"
			"15:8 bit	| Write '1' will set GPIO interrupt mask for falling edge and level low. When read, get the GPIO interrupt mask for falling edge and level low.\n"
			"23:16 bit	| Write '1' will enable debounce mechanism.\n"
			"31:24 bit	| Write '1' will set interruption mode to level.\n"
			"------------------------\n"
			"gpint_ctrl_clr : 0x%x\n"
			"0 bit		| Write '1' will clear GPIO interrupt mask for rising edge and level high.\n"
			"15:8 bit	| Write '1' will clear GPIO interrupt mask for falling edge and level low.\n"
			"23:16 bit	| Write '1' will disable debounce mechanism.\n"
			"31:24 bit	| Write '1' will set interruption mode to edge triggered.\n"
			"------------------------\n"
			"int_clr : 0x%x\n"
			"7:0 bit | Write '1' will clear GPIO interrupt.\n"
			"------------------------\n"
			"int_status : 0x%x\n"
			"7:0 bit | Each bit represents if there is a GPIO interrupt\n"
			"------------------------\n"
			"chg_ctrl : 0x%x\n"
			"0 bit		| time for which GPIO0 is set to output mode, after a start read DCON command is issued. The output time = (OUT_TIME + 1)*30.5us.\n"
			"9:4 bit		| time for which GPIO0 should wait before reading DC_ON, after a start read DCON command is issued. The wait time = (WAIT_TIME + 1)*30.5us. NOTE: wait_time must be strictly greater than out_time; \n"
			"17:16 bit	| interruption mode of GPIO0 in mode DC_ON detection. \n"
			"------------------------\n"
			"=================================================================\n\n"
			, i
			, *gpio_block
			, *(gpio_block + 1)
			, *(gpio_block + 2)
			, *(gpio_block + 3)
			, *(gpio_block + 4)
			, *(gpio_block + 5)
			, *(gpio_block + 6)
			, *(gpio_block + 7)
			, *(gpio_block + 8)
			, *(gpio_block + 9)
			, *(gpio_block + 10));
	}	

	return 0;
}