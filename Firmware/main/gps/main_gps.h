/*
 * main_gps.h
 *
 *  Created on: Dec 25, 2023
 *      Author: odemki
 */

#ifndef MAIN_GPS_MAIN_GPS_H_
#define MAIN_GPS_MAIN_GPS_H_

#include "../main.h"

//typedef struct
//{
//	uint8_t hour;      /*!< Hour */
//	uint8_t minute;    /*!< Minute */
//	uint8_t second;    /*!< Second */
//	uint16_t thousand; /*!< Thousand */
//} my_gps_time_t;
//
//typedef struct {
//    uint8_t day;   /*!< Day (start from 1) */
//    uint8_t month; /*!< Month (start from 1) */
//    uint16_t year; /*!< Year (start from 2000) */
//} my_gps_date_t;
//
//typedef struct
//{
//	float latitude;
//	float longitude;
//	float altitude;
//	float speed;
//	my_gps_time_t time;
//	my_gps_date_t date;
//	uint8_t sats_in_view;
//
//}qLogGPSData;

//QueueHandle_t gps_data_queue = NULL;

void turn_on_gps(uint8_t status);





#endif /* MAIN_GPS_MAIN_GPS_H_ */
