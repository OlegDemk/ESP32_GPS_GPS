/*
 * gsm_sim800l.h
 *
 *  Created on: Jan 17, 2024
 *      Author: odemki
 */

#ifndef MAIN_GSM_GSM_SIM800L_H_
#define MAIN_GSM_GSM_SIM800L_H_

#include "../main.h"

static const int RX_BUF_SIZE = 1024;

void init_uart_for_gsm(void);
int sendData(const char* logName, const char* data);
int send_command_to_gsm(char* command, char* ansver);
int init_gsm(void);

#endif /* MAIN_GSM_GSM_SIM800L_H_ */
