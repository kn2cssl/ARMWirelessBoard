#ifndef __WIRELESS_HPP__
#define __WIRELESS_HPP__

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "Computer.hpp"

//copy to main file
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	wireless.receive(huart);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
	wireless.transmit(huart);
}
*/

static class Wireless
{
	private:
		static UART_HandleTypeDef *order,*response;
		static bool transmitting;
		static unsigned char counter;
		static unsigned char responseData[RESPONSE_LENGTH + 3];
		void send(unsigned char ROBOID);
		void get();
	public:
		void init(UART_HandleTypeDef *order,UART_HandleTypeDef *responce);
		///<summery>
		/// transmit is called in interrupt and sends order Data to robots
		///</summery>
		void transmit(UART_HandleTypeDef *order);
		///<summery>
		/// receive is called in interrupt and receives response Data from robots
		///</summery>
		void receive(UART_HandleTypeDef *response);
		void updateBuffers(void);
}wireless;

#endif
