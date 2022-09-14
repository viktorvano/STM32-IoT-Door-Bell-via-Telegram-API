/*
 * myLibrary.c
 *
 *  Created on: Jan 16, 2022
 *      Author: vikto
 */


#include "myLibrary.h"

uint8_t buffer_ESP[2000], buffer_RF[3000];
uint16_t buffer_index_ESP, timeout_ESP, messageHandlerFlag_ESP;
uint16_t buffer_index_RF, timeout_RF, messageHandlerFlag_RF;
uint32_t seconds;

void ESP_RESET()
{
	HAL_GPIO_WritePin(ESP_ENABLE_GPIO_Port, ESP_ENABLE_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port, ESP_RESET_Pin, GPIO_PIN_RESET);
	HAL_Delay(30);
	HAL_GPIO_WritePin(ESP_ENABLE_GPIO_Port, ESP_ENABLE_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ESP_RESET_GPIO_Port, ESP_RESET_Pin, GPIO_PIN_SET);
}

void ESP_Init()
{
	ESP_RESET();
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+RST\r\n", strlen("AT+RST\r\n"), 100);
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CWMODE=1\r\n", strlen("AT+CWMODE=1\r\n"), 100);
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPMUX=0\r\n", strlen("AT+CIPMUX=1\r\n"), 100);
	HAL_Delay(200);
	ESP_Clear_Buffer();

	HAL_UART_Transmit(&huart1, (uint8_t*)WiFi_Credentials, strlen(WiFi_Credentials), 100);
}

void ESP_Clear_Buffer()
{
	memset(buffer_ESP, 0, 2000);
	buffer_index_ESP = 0;
}

uint8_t string_compare(char array1[], char array2[], uint16_t length)
{
	 uint16_t comVAR=0, i;
	 for(i=0;i<length;i++)
	   	{
	   		  if(array1[i]==array2[i])
	   	  		  comVAR++;
	   	  	  else comVAR=0;
	   	}
	 if (comVAR==length)
		 	return 1;
	 else 	return 0;
}

int string_contains(char bufferArray[], char searchedString[], uint16_t length)
{
	uint8_t result=0;
	for(uint16_t i=0; i<length; i++)
	{
		result = string_compare(&bufferArray[i], &searchedString[0], strlen(searchedString));
		if(result == 1)
			return i;
	}
	return -1;
}

void messageHandler_ESP()
{
	__HAL_UART_DISABLE_IT(&huart1, UART_IT_RXNE);
	if(string_contains((char*)buffer_ESP, "CWJAP", buffer_index_ESP) != -1
	&& (string_contains((char*)buffer_ESP, "FAIL", buffer_index_ESP) != -1
	|| string_contains((char*)buffer_ESP, "WIFI DISCONNECT", buffer_index_ESP) != -1)
	&& !(string_contains((char*)buffer_ESP, "WIFI CONNECTED", buffer_index_ESP) != -1))
	{
		HAL_UART_Transmit(&huart1, (uint8_t*)WiFi_Credentials, strlen(WiFi_Credentials), 100);
	}
	ESP_Clear_Buffer();
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);
}

void RF_Clear_Buffer()
{
	memset(buffer_RF, 0, 3000);
	buffer_index_RF = 0;
}

void messageHandler_RF()
{
	__HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
	int position = 0;
	if((position = string_contains((char*)buffer_RF, "Glory_Code\n", buffer_index_RF)) != -1)
	{
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
		HAL_UART_Transmit(&huart1, (uint8_t*)ESP_Post, strlen(ESP_Post), 100);
		HAL_Delay(80);
		char cipsend[30];

#ifdef USE_Speech_Recognition_AI
		//send TOKEN
		memset(cipsend, 0, 30);
		sprintf(cipsend, "AT+CIPSEND=%i\r\n", strlen(AI_Token)+2);
		HAL_UART_Transmit(&huart1, (uint8_t*)cipsend, strlen(cipsend), 100);
		HAL_Delay(80);
		uint8_t length_token[2] = {0, strlen(AI_Token)};
		HAL_UART_Transmit(&huart1, length_token, 2, 100);
		HAL_UART_Transmit(&huart1, (uint8_t*)AI_Token, strlen(AI_Token), 100);
		HAL_Delay(80);
#endif
		//send Message
		memset(cipsend, 0, 30);
		sprintf(cipsend, "AT+CIPSEND=%i\r\n", strlen(ESP_MESSAGE)+2);
		HAL_UART_Transmit(&huart1, (uint8_t*)cipsend, strlen(cipsend), 100);
		HAL_Delay(80);
		uint8_t length[2] = {0, strlen(ESP_MESSAGE)};
		HAL_UART_Transmit(&huart1, length, 2, 100);
		HAL_UART_Transmit(&huart1, (uint8_t*)ESP_MESSAGE, strlen(ESP_MESSAGE), 100);

#ifdef USE_Speech_Recognition_AI
		HAL_Delay(250);//a delay for Speech Recognition AI response message
#else
		HAL_Delay(80);
#endif
		HAL_UART_Transmit(&huart1, (uint8_t*)"AT+CIPCLOSE\r\n", strlen("AT+CIPCLOSE\r\n"), 100);
		HAL_Delay(80);
		HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
	}
	RF_Clear_Buffer();
	__HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}
