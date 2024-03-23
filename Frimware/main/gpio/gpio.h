/*
 * gpio.h
 *
 *  Created on: Dec 28, 2023
 *      Author: odemki
 */

#ifndef MAIN_GPIO_GPIO_H_
#define MAIN_GPIO_GPIO_H_

#include "../main.h"

void init_output_gpio(void);
void turn_on_gps_module(void);
void turn_off_gps_module(void);
void turn_on_gsm_module(void);
void turn_off_gsm_module(void);


#endif /* MAIN_GPIO_GPIO_H_ */
