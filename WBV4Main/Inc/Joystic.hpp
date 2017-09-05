#ifndef __JOYSTIC_HPP__
#define __JOYSTIC_HPP__

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "NRF24L01.hpp"
#include "buffer.hpp"
#include "Robot.hpp"
#include "main.h"

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
	#define DELAY_BEFORE_FREEWHEEL 100 //100 = 1s
	#define DELAY_BEFORE_DISCONNECT 100

#endif 
static class Joystic
{
	private:
		static NRF* hnrf;
		static TIM_HandleTypeDef* htim;
		static unsigned int roboLastUpdate;
		void NRF_Init();
		void Clear_Data();
		bool Data_Ready();
		void Read_Data();
		void Write_Data();
		void Receive(bool state);
	public:
		static bool isValid;
		static unsigned char order[ORDER_LENGTH];
		static unsigned char response[RESPONSE_LENGTH];
		void Init(NRF* hnrf,TIM_HandleTypeDef* htim);
		void Update();
		void Time_Passed(TIM_HandleTypeDef* htim);
}joystic;

#endif 
