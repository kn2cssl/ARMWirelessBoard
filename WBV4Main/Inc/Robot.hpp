#ifndef __ROBOT_HPP__
#define __ROBOT_HPP__
#include "buffer.hpp"
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "NRF24L01.hpp"
#include "Joystic.hpp"
#include "main.h"

static class Robot
{
	private:
		NRF* hnrf1;
		NRF* hnrf2;
		void nrfInit(NRF* hnrf);
		void changeRobot(NRF* hnrf,unsigned char robotID);
		void writeBuffer(NRF* hnrf,char str[32]);
		void readBuffer(NRF* hnrf,char str[32]);
		void transmit();
		bool dataReady(NRF* hnrf);
	public:
		void init(NRF* hnrf1,NRF* hnrf2);
		void update();
}robot;

static class LED
{
	public:
		GPIO_TypeDef* port;
		unsigned int pin;
		void on(void);
		void off(void);
}LEDs[16];

#endif
