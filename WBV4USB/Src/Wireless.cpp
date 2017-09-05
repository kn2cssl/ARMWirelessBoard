#include "Wireless.hpp"
#include <string.h>
#include "usbd_cdc_if.h"
UART_HandleTypeDef  *Wireless::order,*Wireless::response;
bool Wireless::transmitting = false;
unsigned char Wireless::counter,Wireless::responseData[RESPONSE_LENGTH + 3];

void Wireless::init(UART_HandleTypeDef *order,UART_HandleTypeDef *responce)
{
	Wireless::order = order;
	Wireless::response = responce;
	counter = 0;
}
void Wireless::send(unsigned char ROBOID)
{
	static unsigned char data[ORDER_LENGTH + 3];
	if (!computer.writeOrderRequest[ROBOID])
		return;
	data[0] = START_BYTE;
	data[1] = ROBOID;
	for (unsigned char i = 0; i < ORDER_LENGTH; i++)
		data[i + 2] = computer.order[ROBOID][i]; 
	data[ORDER_LENGTH + 2] = STOP_BYTE;
	if (!transmitting)
	{
		//HAL_Delay(200);
		HAL_UART_Transmit_IT(order,data,ORDER_LENGTH + 3);
		//CDC_Transmit_FS(data,42);
		transmitting = true;
		computer.writeOrderRequest[ROBOID] = false;
	}
	/*if (!transmitting)
	{
		HAL_UART_Transmit_IT(order,computer.order[0],18);
		transmitting = true;
	}*/
	
}
void Wireless::transmit(UART_HandleTypeDef *order)
{
	if (order == Wireless::order)
	{
		HAL_UART_Receive_IT(response,responseData,RESPONSE_LENGTH + 3);
		transmitting = false;
	}
}
void Wireless::receive(UART_HandleTypeDef *response)
{
	if (response == Wireless::response)
		get();
}
void Wireless::get()
{
	if (responseData[0] == START_BYTE && responseData[RESPONSE_LENGTH + 2] == STOP_BYTE)
		for (unsigned char i = 0;i < RESPONSE_LENGTH;i++)
			computer.response[responseData[1]][i] = responseData[i + 2];
	counter++;
	if (counter == MAX_ROBOT)
		counter = 0;
	send(counter);
}
void Wireless::updateBuffers()
{
	for (unsigned char i = 0; i < MAX_ROBOT; i++ )
	{
		if (computer.writeOrderRequest[i])
			send(i);		
	}
}
