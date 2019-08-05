#ifndef _GPIO_H
#define _GPIO_H

#if defined(BCMARM)
#include <stdint.h>
extern uint32_t bcm_set_gpio(uint32_t gpio, uint32_t value);
#else
typedef enum _gpio_dir { GPIO_IN, GPIO_OUT } GPIO_DIRECTION;
int gpio_export(int gpio);
int gpio_unexport(int gpio);
int gpio_set_direction(int gpio, GPIO_DIRECTION dir);
int gpio_set_value(int gpio, int value);
#endif
#endif
