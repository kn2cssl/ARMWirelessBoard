#include "USB.hpp"
#include "Robot.hpp"
#include <string.h>
UART_HandleTypeDef *USB::huart;
unsigned char USB::orderData[ORDER_LENGTH + 3];

void USB::init(UART_HandleTypeDef *huart)
{
	USB::huart = huart;
	HAL_UART_Receive_IT(huart,orderData,ORDER_LENGTH + 3);
	//char str[100];
	//unsigned char leng = sprintf(str,"USB Init ended\r\n");
	//HAL_UART_Transmit(&huart2,(unsigned char *)str,leng,200);
}
void USB::send(unsigned char ROBOID)
{
	static unsigned char data[RESPONSE_LENGTH + 3];
	data[0] = START_BYTE;
	data[1] = ROBOID;
	for (unsigned char i = 0; i < RESPONSE_LENGTH; i++)
		data[i + 2] = buffer.response[ROBOID][i]; 
	data[RESPONSE_LENGTH + 2] = STOP_BYTE;
	HAL_UART_Transmit_IT(huart,data,RESPONSE_LENGTH + 3);
}
void USB::transmit(UART_HandleTypeDef *huart)
{
	if (huart == USB::huart)
		HAL_UART_Receive_IT(huart,orderData,ORDER_LENGTH + 3);
}
void USB::receive(UART_HandleTypeDef *huart)
{
	if (huart == USB::huart)
		get();
}
void USB::get()
{
	static char str[ORDER_LENGTH];
	//char string[100],kuft[100];
	//unsigned char leng = sprintf(string,"USB::get();\r\n");
	//HAL_UART_Transmit(&huart2,(unsigned char *)string,leng,200);
	//for (unsigned char i = 0 ; i < ORDER_LENGTH + 3 ; i++)
	//	sprintf(kuft,"%s\r\n%d",kuft,orderData[i]);
	//leng = sprintf(string,"Data = %s\r\n",kuft);
	//HAL_UART_Transmit(&huart2,(unsigned char *)string,leng,200);
	if (orderData[0] == START_BYTE && orderData[ORDER_LENGTH + 2] == STOP_BYTE)
	{
		//char string[100];
		//unsigned char leng = sprintf(string,"Data Correct\r\n");
		//HAL_UART_Transmit(&huart2,(unsigned char *)string,leng,200);
		for (unsigned char i = 0;i < ORDER_LENGTH;i++)
			str[i] = orderData[i + 2];
		buffer.writeOrder(orderData[1],str);
	}
	send(orderData[1]);
}

