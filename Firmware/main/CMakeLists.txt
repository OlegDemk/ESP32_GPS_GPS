idf_component_register(SRCS "main.c" "wifi/file_server.c" "microsd/mount.c" "gpio/gpio.c" "bme280/bme280.c" "bme280/main_bme280.c" "gps/main_gps.c" "gps/nmea_parser.c" "gsm/gsm_sim800l.c" 
                    INCLUDE_DIRS "."
                    EMBED_FILES "favicon.ico" "upload_script.html")
