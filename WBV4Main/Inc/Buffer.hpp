#ifndef __BUFFER_HPP__
#define __BUFFER_HPP__

#include "stm32f4xx_hal.h"
#include "tim.h"
#include <stdbool.h>
#include "main.h"
#include "usart.h"
#include <stdio.h>
#include "Joystic.hpp"

#ifndef __SETTINGS__
#define __SETTINGS__

	#define HEADER_LENGTH 4 
	#define ORDER_LENGTH 18 // old length 11 //@mparchin: last definition was DATA_LENGTH
	#define RESPONSE_LENGTH 32 //@mparchin: data lenght for returned data from robots to computer
	#define MAX_ROBOT 12 //@mparchin: we must change this shit either to 16(maximum robot support) or 8(maximum speed) 
	#define MAX_NRF_DATA_LENGTH 32
	#define START_BYTE 0xAA
	#define STOP_BYTE 0x55
	#define WRITE_REQUEST 'W'
	#define READ_REQUEST 'R'
	#define DATA_REQUEST 'D'
	#define ACK 0x99
	#define NACK 0x66
	#define DELAY_BEFORE_FREEWHEEL 1000 //100 = 1s
	#define DELAY_BEFORE_DISCONNECT 1000
	#ifndef JOYSTIC
		#define JOYSTIC		//@mparchin:comment this line for disableing Joystic
	#endif
#endif

//copy to main file 
/*
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
{
	buffer.timePassed(htim);
}
*/

static class Buffer
{
	private:
		static TIM_HandleTypeDef* htim;
		static bool lastOrderEnabled[MAX_ROBOT];
		static unsigned int lastOrder[MAX_ROBOT][ORDER_LENGTH];
		static unsigned int lastOrderUpdate[MAX_ROBOT];
		static unsigned int lastResponseUpdate[MAX_ROBOT];
		void freeWheel(unsigned char ROBOID);
		void clearResponse(unsigned char ROBOID);
	public:
		static unsigned char response[MAX_ROBOT][RESPONSE_LENGTH];
		static char order[MAX_ROBOT][ORDER_LENGTH];
		void init(TIM_HandleTypeDef* htim);
		void writeResponse(unsigned char ROBOID,char str[RESPONSE_LENGTH]);
		void writeOrder(unsigned char ROBOID,char str[ORDER_LENGTH]);
		void timePassed(TIM_HandleTypeDef* htim);
}buffer;

#endif
