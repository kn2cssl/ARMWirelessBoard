#ifndef __COMPUTER_HPP__
#define __COMPUTER_HPP__

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "NRF24L01.hpp"

#define HEADER_LENGTH 4
#define STOP_BYTE_SIZE 1
#define DATA_LENGTH 18 // old length 11
#define MAX_ROBOT 12
#define START_BYTE0 0xA5
#define START_BYTE1 0x5A
#define STOP_BYTE 0x80
#define DELAY_BEFORE_FREEWHEEL 100 //100 = 1s
#define DELAY_BEFORE_DISCONNECT 100

//115200 8 n 1 
//frequency 1K period 10

//copy to main file
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	computer.listen(huart);
}
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	computer.timePassed(htim);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
	computer.transmit(huart);
}
*/

static class ComputerConnection
{
	private:
		static unsigned char state;
		static unsigned char header[HEADER_LENGTH];
		static unsigned char dataBody[DATA_LENGTH * MAX_ROBOT];
		static UART_HandleTypeDef* huartTransmit;
		static UART_HandleTypeDef* huartReceive;
		static TIM_HandleTypeDef* htim;
		static unsigned int lastUpdate[MAX_ROBOT];
		static unsigned int lastACKUpdate[MAX_ROBOT];
		static unsigned char index;
		static bool transmitting;
		void updateBuffer(void);
		void clearACK(unsigned char roboID);
		void freeWheel(unsigned char roboID);
		void initBuffer(void);
		void getHeader(void);
		void clearData(void);
		bool checkSum(void);
	public:
		static unsigned char ack[MAX_ROBOT][34];
		static char buffer[MAX_ROBOT][32];
		void init(UART_HandleTypeDef* huartReceive,UART_HandleTypeDef* huartTransmit,TIM_HandleTypeDef* htim);
		void listen(UART_HandleTypeDef* huartReceive);
		void timePassed(TIM_HandleTypeDef* htim);
		void transmit(UART_HandleTypeDef* huartTransmit);
		void ACK(unsigned char robotID,char ACKbuffer[32]);
}computer;

#endif
