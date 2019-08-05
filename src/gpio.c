#include <errno.h>
#include <stdio.h>

#include "common.h"
#include "file_util.h"
#include "gpio.h"
#if defined(BCMARM)
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <bcmendian.h>
#include <bcmnvram.h>
#include "utils.h"
#include "shutils.h"
#include "shared.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux_gpio.h>
#include <time.h>
static int bcm_gpio_ioctl(int f, int gpioreg, unsigned int mask, unsigned int val)
{
	struct gpio_ioctl gpio;

	gpio.val = val;
	gpio.mask = mask;

	if (ioctl(f, gpioreg, &gpio) < 0) {
		_dprintf("Invalid gpioreg %d\n", gpioreg);
		return -1;
	}
	return (gpio.val);
}

static int _bcm_gpio_open()
{
	int f = open("/dev/gpio", O_RDWR);
	if (f < 0)
		_dprintf ("Failed to open /dev/gpio\n");
	return f;
}

int bcm_gpio_open(uint32_t mask)
{
	uint32_t bit;
	int i;
	int f = _bcm_gpio_open();

	if ((f >= 0) && mask) {
		for (i = 0; i <= 15; i++) {
			bit = 1 << i;
			if ((mask & bit) == bit) {
				bcm_gpio_ioctl(f, GPIO_IOC_RESERVE, bit, bit);
				bcm_gpio_ioctl(f, GPIO_IOC_OUTEN, bit, 0);
			}
		}
		close(f);
		f = _bcm_gpio_open();
	}

	return f;
}

void bcm_gpio_write(uint32_t bit, int en)
{
	int f;

	if ((f = bcm_gpio_open(0)) < 0) return;

	bcm_gpio_ioctl(f, GPIO_IOC_RESERVE, bit, bit);
	bcm_gpio_ioctl(f, GPIO_IOC_OUTEN, bit, bit);
	bcm_gpio_ioctl(f, GPIO_IOC_OUT, bit, en ? bit : 0);
	close(f);
}
uint32_t bcm_set_gpio(uint32_t gpio, uint32_t value)
{
	bcm_gpio_write(gpio, value);
	return 0;
}
#else
int gpio_export(int gpio) {
    char path_buf[50];

    sprintf(path_buf, "/sys/class/gpio/gpio%d", gpio);

    return path_exists(path_buf) == SUCCESS ||
           write_file_int("/sys/class/gpio/export", gpio);
}

int gpio_unexport(int gpio) {
    char path_buf[50];

    sprintf(path_buf, "/sys/class/gpio/gpio%d", gpio);

    return path_exists(path_buf) == FAILURE ||
           write_file_int("/sys/class/gpio/unexport", gpio);
}

int gpio_set_direction(int gpio, GPIO_DIRECTION dir) {
    char path_buf[50];
    char *dir_str;

    sprintf(path_buf, "/sys/class/gpio/gpio%d/direction", gpio);

    switch (dir) {
    case GPIO_IN:
        dir_str = "in";
        break;
    case GPIO_OUT:
        dir_str = "out";
        break;
    }

    return write_file_str(path_buf, dir_str);
}

int gpio_set_value(int gpio, int value) {
    char path_buf[50];

    sprintf(path_buf, "/sys/class/gpio/gpio%d/value", gpio);

    return write_file_int(path_buf, value);
}
#endif
