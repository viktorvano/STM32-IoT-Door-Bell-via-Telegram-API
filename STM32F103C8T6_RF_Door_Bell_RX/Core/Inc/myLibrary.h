/*
 * myLibrary.h
 *
 *  Created on: Jan 16, 2022
 *      Author: vikto
 */

#ifndef INC_MYLIBRARY_H_
#define INC_MYLIBRARY_H_

#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Change your WiFi credentials
#define WiFi_Credentials	"AT+CWJAP=\"WiFiSSID\",\"WiFiPASSWORD\"\r\n"
#define ESP_Post			"AT+CIPSTART=\"TCP\",\"192.168.1.25\",8765\r\n"

#define ESP_MESSAGE			"Ding, dong. From the door."

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

extern uint8_t buffer_ESP[2000], buffer_RF[3000];
extern uint16_t buffer_index_ESP, timeout_ESP, messageHandlerFlag_ESP;
extern uint16_t buffer_index_RF, timeout_RF, messageHandlerFlag_RF;
extern uint32_t seconds;

void ESP_RESET();
void ESP_Init();
void ESP_Clear_Buffer();
uint8_t string_compare(char array1[], char array2[], uint16_t length);
int string_contains(char bufferArray[], char searchedString[], uint16_t length);
void messageHandler_ESP();
void RF_Clear_Buffer();
void messageHandler_RF();

#endif /* INC_MYLIBRARY_H_ */
