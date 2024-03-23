/*
 * gpio.c
 *
 *  Created on: Dec 28, 2023
 *      Author: odemki
 */

#include "gpio.h"


// -----------------------------------------------------------------
#define GPIO_OUTPUT_PIN_SEL (1ULL<<CONFIG_BLINK_GPIO)
#define GPIO_OUTPUT_PIN_SEL_RED (1ULL<<CONFIG_RED_GPIO)
#define GPIO_OUTPUT_PIN_SEL_GREEN (1ULL<<CONFIG_GREEN_GPIO)
#define GPIO_OUTPUT_PIN_SEL_BLUE (1ULL<<CONFIG_BLUE_GPIO)
#define GPIO_OUTPUT_PIN_SEL_BLUE (1ULL<<CONFIG_BLUE_GPIO)
#define GPIO_POWER_GPS (1ULL<<CONFIG_POWER_GPS_GPIO)
#define GPIO_POWER_GSM (1ULL<<CONFIG_POWER_GSM_GPIO)

// -----------------------------------------------------------------
void init_output_gpio(void)
{
	gpio_config_t io_conf = {};		// structure for configure GPIOs
	// Configure output PIN
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_OUTPUT;
//	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL | GPIO_OUTPUT_PIN_SEL_RED | GPIO_OUTPUT_PIN_SEL_GREEN | GPIO_OUTPUT_PIN_SEL_BLUE;
//	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL | GPIO_OUTPUT_PIN_SEL_RED | GPIO_OUTPUT_PIN_SEL_GREEN | GPIO_OUTPUT_PIN_SEL_BLUE | GPIO_POWER_GPS | GPIO_POWER_GSM;
	io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL | GPIO_OUTPUT_PIN_SEL_RED | GPIO_OUTPUT_PIN_SEL_GREEN | GPIO_OUTPUT_PIN_SEL_BLUE | GPIO_POWER_GPS | GPIO_POWER_GSM;
	io_conf.pull_down_en = 0;
	io_conf.pull_up_en = 0;
	gpio_config(&io_conf);

	for(int i =0; i < 10; i++)
	{
		gpio_set_level(CONFIG_RED_GPIO, 1);
		gpio_set_level(CONFIG_GREEN_GPIO, 1);
		gpio_set_level(CONFIG_BLUE_GPIO, 1);

		vTaskDelay(50 / portTICK_PERIOD_MS);

		gpio_set_level(CONFIG_RED_GPIO, 0);
		gpio_set_level(CONFIG_GREEN_GPIO, 0);
		gpio_set_level(CONFIG_BLUE_GPIO, 0);

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}

}
// -----------------------------------------------------------------
void turn_on_gps_module(void)
{
	gpio_set_level(CONFIG_POWER_GPS_GPIO, 0);
}
// -----------------------------------------------------------------
void turn_off_gps_module(void)
{
	gpio_set_level(CONFIG_POWER_GPS_GPIO, 1);
}
// -----------------------------------------------------------------
void turn_on_gsm_module(void)
{
	gpio_set_level(CONFIG_POWER_GSM_GPIO, 0);
}
// -----------------------------------------------------------------
void turn_off_gsm_module(void)
{
	gpio_set_level(CONFIG_POWER_GSM_GPIO, 1);
}
//// -----------------------------------------------------------------

