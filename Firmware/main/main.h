/*
 * main.h
 *
 *  Created on: Jan 30, 2024
 *      Author: odemki
 */

#ifndef MAIN_MAIN_H_
#define MAIN_MAIN_H_

#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "protocol_examples_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#include "driver/gpio.h"
#include "gpio/gpio.h"
#include "microsd/mount.h"

#include "wifi/file_server.h"

#include "driver/i2c.h"
#include "bme280/bme280.h"
#include "bme280/main_bme280.h"

#include "gps/main_gps.h"
#include "gps/nmea_parser.h"

#include "gsm/gsm_sim800l.h"


#define ON 1
#define OFF 0

void gsm(uint8_t status);
void deinit_uart_for_gsm(void);

// -------------------------------------------------------------------------------
typedef struct{
	double temperature;
	double humidity;
	double preasure;
}qthp_bme280;
// -------------------------------------------------------------------------------
// -------------------------------------------------------------------------------
typedef struct {
	uint8_t hour;      /*!< Hour */
	uint8_t minute;    /*!< Minute */
	uint8_t second;    /*!< Second */
	uint16_t thousand; /*!< Thousand */
} my_gps_time_t;

typedef struct {
    uint8_t day;   /*!< Day (start from 1) */
    uint8_t month; /*!< Month (start from 1) */
    uint16_t year; /*!< Year (start from 2000) */
} my_gps_date_t;

typedef struct
{
	float latitude;
	float longitude;
	float altitude;
	float speed;
	my_gps_time_t time;
	my_gps_date_t date;
	uint8_t sats_in_view;

}qLogGPSData;

typedef struct
{
	char gsm_buf[25];
}gsm_ansver_buf;



#endif /* MAIN_MAIN_H_ */
