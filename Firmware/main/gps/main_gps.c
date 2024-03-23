/*
 * main_gps.c
 *
 *  Created on: Dec 25, 2023
 *      Author: odemki
 */



#include "main_gps.h"

/////////////////////////////////////////////////////////////////////////////////////////

#define TIME_ZONE (+2)   //Beijing Time
#define YEAR_BASE (2000) //date in GPS starts from 2000

extern QueueHandle_t gps_data_log_queue;
extern QueueHandle_t gps_data_json_queue;
//extern QueueHandle_t spiffs_size_queue;

extern bool action_1_state;


//nmea_parser_config_t config;
//nmea_parser_handle_t nmea_hdl;

/////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief GPS Event Handler
 *
 * @param event_handler_arg handler specific arguments
 * @param event_base event base, here is fixed to ESP_NMEA_EVENT
 * @param event_id event id
 * @param event_data event specific arguments
 */
static void gps_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	static const char *TAG = "GPS: ";

	qLogGPSData qLogGPSData_t;

    gps_t *gps = NULL;

    switch (event_id) {
    case GPS_UPDATE:
        gps = (gps_t *)event_data;
        /* print information parsed from GPS statements */
//        ESP_LOGI(TAG, "%d/%d/%d %d:%d:%d => \r\n"
//                 "\t\t\t\t\t\tlatitude   = %.05f°N\r\n"
//                 "\t\t\t\t\t\tlongitude = %.05f°E\r\n"
//                 "\t\t\t\t\t\taltitude   = %.02fm\r\n"
//                 "\t\t\t\t\t\tspeed      = %fm/s",
//                 gps->date.year + YEAR_BASE, gps->date.month, gps->date.day,
//                 gps->tim.hour + TIME_ZONE, gps->tim.minute, gps->tim.second,
//				 gps->latitude, gps->longitude, gps->altitude, gps->speed);

        		// Fill in queue

        	qLogGPSData_t.latitude = gps->latitude;
        	qLogGPSData_t.longitude = gps->longitude;
        	qLogGPSData_t.altitude = gps->altitude;
        	qLogGPSData_t.speed = gps->speed;
        	qLogGPSData_t.sats_in_view = gps->sats_in_view;

        	qLogGPSData_t.time.hour = gps->tim.hour+TIME_ZONE;
        	qLogGPSData_t.time.minute = gps->tim.minute;
        	qLogGPSData_t.time.second = gps->tim.second;

        	qLogGPSData_t.date.year = gps->date.year + YEAR_BASE;
        	qLogGPSData_t.date.month = gps->date.month;
        	qLogGPSData_t.date.day = gps->date.day;

        	// xQueueSendToBack(gps_data_html_queue, &qLogGPSData_t, 0);
        	xQueueSendToBack(gps_data_log_queue, &qLogGPSData_t, 0);
        	xQueueSendToBack(gps_data_json_queue, &qLogGPSData_t, 0);
        break;
    case GPS_UNKNOWN:
        /* print unknown statements */
        ESP_LOGW(TAG, "Unknown statement:%s", (char *)event_data);
        break;
    default:
        break;
    }
}


// --------------------------------------------------------------------------------------------------
void turn_on_gps(uint8_t status)
{
	static const char *TAG = "GPS: ";

	static nmea_parser_handle_t nmea_hdl;
	static uint8_t prev_status = 0;

	//if(status > 1)	{status = 0;}

	if(prev_status != status)
	{
		if(status == 1)
		{
			action_1_state = 1;
			ESP_LOGI(TAG, "ON");
			turn_on_gps_module();
			vTaskDelay(100 / portTICK_PERIOD_MS);
			nmea_parser_config_t config = NMEA_PARSER_CONFIG_DEFAULT();	    /* NMEA parser configuration */
			nmea_hdl = nmea_parser_init(&config);		/* init NMEA parser library */
			nmea_parser_add_handler(nmea_hdl, gps_event_handler, NULL);		/* register event handler for NMEA parser library */
		}
		if(status == 0)
		{
			action_1_state = 0;
			ESP_LOGI(TAG, "OFF");
			nmea_parser_remove_handler(nmea_hdl, gps_event_handler);		/* unregister event handler */
			nmea_parser_deinit(nmea_hdl);									//    /* deinit NMEA parser library */
			turn_off_gps_module();
		}
		prev_status = status;
	}
}
// --------------------------------------------------------------------------------------------------



