/*
 * file_server.h
 *
 *  Created on: Jan 30, 2024
 *      Author: odemki
 */

#ifndef MAIN_WIFI_FILE_SERVER_H_
#define MAIN_WIFI_FILE_SERVER_H_

#include "main.h"

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#include "esp_err.h"
#include "esp_log.h"

#include "esp_vfs.h"
#include "esp_spiffs.h"
#include "esp_http_server.h"

#include "../gps/main_gps.h"
#include "../gpio/gpio.h"

esp_err_t example_start_file_server(const char *base_path);



#endif /* MAIN_WIFI_FILE_SERVER_H_ */
