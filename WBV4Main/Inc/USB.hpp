#ifndef __USB_HPP__
#define __USB_HPP__

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "Buffer.hpp"

//copy to main file
/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	USB.receive(huart);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
	USB.transmit(huart);
}
*/

static class USB
{
		static UART_HandleTypeDef *huart;
		static unsigned char orderData[ORDER_LENGTH + 3];
		void send(unsigned char ROBOID);
		void get();
	public:
		void init(UART_HandleTypeDef *huart);
		///<summery>
		/// transmit is called in interrupt and sends response Data to Computer
		///</summery>
		void transmit(UART_HandleTypeDef *huart);
		///<summery>
		/// receive is called in interrupt and receives order Data from Computer
		///</summery>
		void receive(UART_HandleTypeDef *huart);
}USB;

#endif
