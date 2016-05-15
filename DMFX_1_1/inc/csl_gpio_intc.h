/*
 * csl_gpio_intc.h
 *
 *  Created on: Nov 29, 2015
 *      Author: Diego de la Cruz
 */

#ifndef CSL_GPIO_INTC_H_
#define CSL_GPIO_INTC_H_

#define DOWN_BUTTON		0x1
#define LEFT_BUTTON		0x2
#define CENTER_BUTTON	0x4
#define RIGHT_BUTTON	0x8
#define UP_BUTTON		0x10

/*---------Function prototypes---------*/
void gpioISR(void);
void gpio_int_test(void);
void gpio_int_init(void);
void gpio_int_deinit(void);

#endif /* CSL_GPIO_INTC_H_ */
