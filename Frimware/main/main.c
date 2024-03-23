

#include "main.h"

// Task handlers
TaskHandle_t task_bme280_handlr;
TaskHandle_t task_log_handler;
TaskHandle_t task_tx_gsm_handler;
TaskHandle_t task_rx_gsm_handler;

// Queues
QueueHandle_t bme280_data_log_queue = NULL;
QueueHandle_t bme280_data_json_queue = NULL;
QueueHandle_t gps_data_log_queue = NULL;
QueueHandle_t gps_data_json_queue = NULL;
QueueHandle_t gsm_data_queue = NULL;

static const char *TAG = "example";


// --------------------------------------------------------------------------------------------------------------------
void task_bme280(void *ignore)
{
	qthp_bme280 qthp_bme280_t;
	static const char *TAG_BME280 = "BME280";

	i2c_master_init_BME280();

	struct bme280_t bme280 = {
		.bus_write = BME280_I2C_bus_write,
		.bus_read = BME280_I2C_bus_read,
		.dev_addr = BME280_I2C_ADDRESS2,
		.delay_msec = BME280_delay_msek
	};

	s32 com_rslt;
	s32 v_uncomp_pressure_s32;
	s32 v_uncomp_temperature_s32;
	s32 v_uncomp_humidity_s32;

	com_rslt = bme280_init(&bme280);

	com_rslt += bme280_set_oversamp_pressure(BME280_OVERSAMP_16X);
	com_rslt += bme280_set_oversamp_temperature(BME280_OVERSAMP_2X);
	com_rslt += bme280_set_oversamp_humidity(BME280_OVERSAMP_1X);
	com_rslt += bme280_set_standby_durn(BME280_STANDBY_TIME_1_MS);
	com_rslt += bme280_set_filter(BME280_FILTER_COEFF_16);

	com_rslt += bme280_set_power_mode(BME280_NORMAL_MODE);

	if (com_rslt == SUCCESS)
	{
		while(true)
		{
			com_rslt = bme280_read_uncomp_pressure_temperature_humidity(
				&v_uncomp_pressure_s32, &v_uncomp_temperature_s32, &v_uncomp_humidity_s32);

			if (com_rslt == SUCCESS)
			{

				double t = bme280_compensate_temperature_double(v_uncomp_temperature_s32);
				double p = bme280_compensate_pressure_double(v_uncomp_pressure_s32)/100;
				double h = bme280_compensate_humidity_double(v_uncomp_humidity_s32);

//				ESP_LOGI(TAG_BME280, "%.2f degC / %.3f hPa / %.3f %%", t, p, h);

				qthp_bme280_t.temperature = t;
				qthp_bme280_t.humidity = h;
				qthp_bme280_t.preasure = p;
				xQueueSendToBack(bme280_data_log_queue, &qthp_bme280_t, 0);
				xQueueSendToBack(bme280_data_json_queue, &qthp_bme280_t, 0);
			}
			else
			{
				ESP_LOGE(TAG_BME280, "measure error. code: %d", com_rslt);
			}

			// Resurse monitor
//			size_t high_water_mark = uxTaskGetStackHighWaterMark(NULL);
//			ESP_LOGI(TAG, ">>> task_bme280_normal_mode: High water mark = %u words\n", high_water_mark);
			vTaskDelay(1000/portTICK_PERIOD_MS);
		}
	}
	else
	{
		ESP_LOGE(TAG_BME280, "init or setting error. code: %d", com_rslt);
	}

	vTaskDelete(NULL);
}
// --------------------------------------------------------------------------------------------------------------------
void task_log(void *ignore)
{
#define LOG_INTO_COMPORT ON
	static const char *TAG_LOG = "LOG";

	BaseType_t qStatus = false;
	qLogGPSData qLogGPSData_t;
	qthp_bme280 qthp_bme280_t;

	static uint8_t create_new_file = 1;
	int log_counter = 0;
	uint8_t log_status = 1; // Loging on



//	ESP_LOGI(TAG_LOG, "Initializing SPIFFS");
//	if(inti_spi_flash_card() != 1)
//	{
//		ESP_LOGE(TAG_LOG, "Failed init flash card");
//	}

	while(1)
	{
		if(log_status == 1)	// Log mode is ON
		{
			if(LOG_INTO_COMPORT == ON)
			{
				ESP_LOGI(TAG_LOG, "LOG ON");
			}

			//	Get unique name for file
//			int time_now = esp_timer_get_time();
//			char file_name[10] = "f";
//			char file_number[10] = {0,};
//			sprintf(file_number, "%d" ,time_now/1000000);
//			ESP_LOGI(TAG_LOG, "Size of file: %d", sizeof(file_number));
//			strcat(file_name, file_number);
//			strcat(file_name, ".txt");
//			ESP_LOGI(TAG_LOG, "FILE NAME: %s", file_name);
			//////////////////////////////////////////////////////////////////

//			char mount_point_buf[30]= {0,};
//			memset(mount_point_buf, 0, sizeof(mount_point_buf));
//			strcat(mount_point_buf, MOUNT_POINT);
//			ESP_LOGI(TAG_LOG, "mount_point_buf:> %s", mount_point_buf);
//			strcat(mount_point_buf, "/");
//			strcat(mount_point_buf, "12345679.txt");		// !!! name cant be more than 8 characters
//
//			ESP_LOGI(TAG_LOG, "all mount_point_buf:> %s", mount_point_buf);



			// Якщо дані прийшли від BME280
			qStatus = xQueueReceive(bme280_data_log_queue, &qthp_bme280_t, 1000 / portTICK_PERIOD_MS);
			if(qStatus == pdPASS)
			{
				char bme280_buf_data[100] = {0, };
				// Prepare data to save into file
				snprintf(bme280_buf_data, sizeof(bme280_buf_data),
						"Counter:%d  T:%.1fC, H:%.1f %%, P:%.1f hPa",
						log_counter++, qthp_bme280_t.temperature, qthp_bme280_t.humidity, qthp_bme280_t.preasure
				);

				if(LOG_INTO_COMPORT == ON)
				{
					ESP_LOGI(TAG_LOG, "---------- BME280 data ----------");
					ESP_LOGI(TAG_LOG, "T:%.1fC, H:%.1f %%, P:%.1f hPa",
							qthp_bme280_t.temperature, qthp_bme280_t.humidity, qthp_bme280_t.preasure);
					ESP_LOGI(TAG_LOG, "---------------------------------");
				}
				log_data("/data", "test1234.txt", bme280_buf_data,  "writing BME280 data");
			}

			// Якщо дані прийшли від GPS
			qStatus = xQueueReceive(gps_data_log_queue, &qLogGPSData_t, 1000/portTICK_PERIOD_MS);
			if(qStatus == pdPASS)
			{
				if(LOG_INTO_COMPORT == ON)
				{
					ESP_LOGI(TAG_LOG, "---------- GPS data ----------");
					ESP_LOGI(TAG_LOG, "latitude: %.05f°N", qLogGPSData_t.latitude);
					ESP_LOGI(TAG_LOG, "longitude: %.05f°E", qLogGPSData_t.longitude);
					ESP_LOGI(TAG_LOG, "altitude: %.02fm", qLogGPSData_t.altitude);
					ESP_LOGI(TAG_LOG, "speed: %.02fm", qLogGPSData_t.speed);

					ESP_LOGI(TAG_LOG, "second: %d", qLogGPSData_t.time.second);
					ESP_LOGI(TAG_LOG, "minute: %d", qLogGPSData_t.time.minute);
					ESP_LOGI(TAG_LOG, "hour: %d", qLogGPSData_t.time.hour);

					ESP_LOGI(TAG_LOG, "day: %d", qLogGPSData_t.date.day);
					ESP_LOGI(TAG_LOG, "month: %d", qLogGPSData_t.date.month);
					ESP_LOGI(TAG_LOG, "year: %d", qLogGPSData_t.date.year);
					ESP_LOGI(TAG_LOG, "sats -in view: %d", qLogGPSData_t.sats_in_view);
					ESP_LOGI(TAG_LOG, "---------------------------------");
				}

				char gps_buf_data[100] = {0,};
				memset(gps_buf_data, 0, sizeof(gps_buf_data));

				snprintf(gps_buf_data, sizeof(gps_buf_data),
						"%d.%d.%d %d:%d:%d la:%.05f°N lo:%.05f°E al:%.01fm speed:%.01fm sat:%d\n"
						,
						qLogGPSData_t.date.year, qLogGPSData_t.date.month, qLogGPSData_t.date.day,
						qLogGPSData_t.time.hour, qLogGPSData_t.time.minute, qLogGPSData_t.time.second,
						qLogGPSData_t.latitude,qLogGPSData_t.longitude,qLogGPSData_t.altitude, qLogGPSData_t.speed,
						qLogGPSData_t.sats_in_view
				);

				log_data("/data", "test1234.txt", gps_buf_data,  "writing GPS data");

			}

//			show_files_on_flash_card();
		}
		else					// Log mode is OFF
		{
			if(LOG_INTO_COMPORT == ON)
			{
				ESP_LOGI(TAG_LOG, "LOG OFF");
			}
			vTaskDelay(1000/portTICK_PERIOD_MS);
		}
	}
}
// -------------------------------------------------------------------------------------
void task_tx_gsm(void *ignore)
{
	 static const char *TX_TASK_TAG = "TX_TASK";
	 esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
	 vTaskDelay(5000 / portTICK_PERIOD_MS);

	 init_gsm();

	 while (1)
	 {
		 //sendData(TX_TASK_TAG, "Hello world\n\r");
		 //sendData(TX_TASK_TAG, "ATD+380931482354;\n\r");

//		 init_gsm();




	     vTaskDelay(15000 / portTICK_PERIOD_MS);
	 }
}
// ----------------------------------------------------------------------------------------------
void task_rx_gsm(void *ignore)
{
	BaseType_t qStatus = false;
	gsm_ansver_buf gsm_ansver_t;

	static const char *RX_TASK_TAG = "RX_TASK";
	esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
	uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);

	while (1)
	{
		const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 10 / portTICK_PERIOD_MS);
		if (rxBytes > 0)
		{
			data[rxBytes] = 0;
			ESP_LOGI(RX_TASK_TAG, "------>Read %d bytes: '%s'", rxBytes, data);
//			ESP_LOG_BUFFER_HEXDUMP(RX_TASK_TAG, data, rxBytes, ESP_LOG_INFO);

			// Якщо прийшли дані в чергу, я

			qStatus = xQueueReceive(gsm_data_queue, &gsm_ansver_t, 1000/portTICK_PERIOD_MS);
			if(qStatus == pdPASS)
			{
				ESP_LOGI(RX_TASK_TAG, "RESEIVED ANSVER: %s", gsm_ansver_t.gsm_buf);

//				char buf_1[30] = {0,};
//				char buf_2[30] = {0,};
//				strcpy(buf_1, (char*)data);
//				strcpy(buf_2, gsm_ansver_t.gsm_buf);
//				ESP_LOGI(RX_TASK_TAG, "buf_1: %s", buf_1);
//				ESP_LOGI(RX_TASK_TAG, "buf_2: %s", buf_2);



				char *result = strstr((char*)data, (char*)gsm_ansver_t.gsm_buf);
//				char *result = strstr(buf_1, buf_2);
				if(result != NULL)
				{
					ESP_LOGI(RX_TASK_TAG, "OKOKOKOKOPKOKOKO");
				}
				else
				{
					ESP_LOGE(RX_TASK_TAG, "WRONG ANSVER!!!!!!!!!");
				}


			}

			// Input call
			char buf_ring[] = "+CLIP: \"+380931482354\"";
			char *result = strstr((char*)data, (char*)buf_ring);
			//				char *result = strstr(buf_1, buf_2);
			if(result != NULL)
			{
				sendData(RX_TASK_TAG, "ATA\n\r");
			}



	    }
	}
	free(data);
}
// ----------------------------------------------------------------------------------------------
void gsm(uint8_t status)
{
	static const char *GSM_LOG = "GSM";

	static uint8_t prev_status = 0;

	if(prev_status != status)
	{
		prev_status = status;

		if(status == 0)
		{
			ESP_LOGI(GSM_LOG, "GSM module OFF");

			vQueueDelete(gsm_data_queue);
			vTaskDelete(task_tx_gsm_handler);
			vTaskDelete(task_rx_gsm_handler);
			deinit_uart_for_gsm();
			turn_off_gsm_module();
		}
		else
		{
			ESP_LOGI(GSM_LOG, "GSM module ON");

			turn_on_gsm_module();

			gsm_data_queue = xQueueCreate(5, sizeof(gsm_ansver_buf));
			init_uart_for_gsm();
			xTaskCreate(task_tx_gsm, "task_tx_gsm", 2048, NULL, configMAX_PRIORITIES - 1, &task_tx_gsm_handler);
			xTaskCreate(task_rx_gsm, "task_rx_gsm", 2048, NULL, configMAX_PRIORITIES - 1, &task_rx_gsm_handler);
		}
	}

}
// ----------------------------------------------------------------------------------------------

void app_main(void)
{
	init_output_gpio();


    /* Initialize file storage */
    const char* base_path = "/data";
    ESP_ERROR_CHECK(example_mount_storage(base_path));

    bme280_data_json_queue = xQueueCreate(5, sizeof(qthp_bme280));
    bme280_data_log_queue = xQueueCreate(5, sizeof(qthp_bme280));
	xTaskCreate(task_bme280, "task_bme280", 2048, NULL, configMAX_PRIORITIES - 1, &task_bme280_handlr);

	gps_data_log_queue = xQueueCreate(5, sizeof(qLogGPSData));
	gps_data_json_queue = xQueueCreate(5, sizeof(qLogGPSData));
	turn_on_gps(1);



	////////////////////////////////////////////////////////////////////////////////////
    ESP_LOGI(TAG, "Starting example");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());



    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
     * Read "Establishing Wi-Fi or Ethernet Connection" section in
     * examples/protocols/README.md for more information about this function.
     */
    ESP_ERROR_CHECK(example_connect());

    /* Start the file server */
    ESP_ERROR_CHECK(example_start_file_server(base_path));
    ESP_LOGI(TAG, "File server started");

    xTaskCreate(task_log, "task_log", 4096, NULL, configMAX_PRIORITIES - 1, &task_log_handler);


    gsm(1);			// turn on GSM module by defoult





}
// ----------------------------------------------------------------------------------------------

/*
	TODO:
		1. Робити унікальне імя файлу для логування від 1 до ... використовуючи зберігання змінної назви в флеш

		2. Зробити кнопку вулючення виключення GPS (додати колір статусу в кнопці)
		3. Зробити кнопку вулючення виключення GSM (додати колір статусу в кнопці)
		4. Зробити кнопку вулючення виключення ЛОГУВАННЯ ДАНИХ (додати колір статусу в кнопці)
		5. Зробити кнопку вулючення виключення ВСЬОГО функціоналу (додати колір статусу в кнопці)
		6. Зробити кнопку ресет всього дівайсу
		7. Зробити кнопку заходження в сон всього дівайсу (можливо вимикати ве, і включати тільки адвертайзинг блютусу.

		8. Отримувати загальний час роботи дівайсу і записувати в табличку (виводити в таблиці дані в форматі дні, години, хвилини, секунди).
		9. Отримувати загальну память на флешці, використану память і записувати в табличку.
		10. Виводити поточний час і дату з даних GPS в іншому вигляді а не у вигляді таблиці

		11. Логувати дані для google Earth

		12. зробити так, щоб код працював якщо не працює BME280
		13. GSM

		дівайс після декількох невдалих спроб підключення до WiFi перезавантажується

	DONE:


 */













