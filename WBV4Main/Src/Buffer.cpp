#include "Buffer.hpp"

TIM_HandleTypeDef* Buffer::htim;
bool Buffer::lastOrderEnabled[MAX_ROBOT];
unsigned int Buffer::lastOrder[MAX_ROBOT][ORDER_LENGTH];
unsigned int Buffer::lastOrderUpdate[MAX_ROBOT];
unsigned int Buffer::lastResponseUpdate[MAX_ROBOT];
unsigned char Buffer::response[MAX_ROBOT][RESPONSE_LENGTH];
char Buffer::order[MAX_ROBOT][ORDER_LENGTH];

void Buffer::init(TIM_HandleTypeDef* htim)
{
	Buffer::htim = htim;
	for (unsigned char i = 0; i < MAX_ROBOT ; i++)
	{
		freeWheel(i);
		clearResponse(i);
		lastOrderEnabled[i] = false;
		lastOrderUpdate[i] = 0;
		lastResponseUpdate[i] = 0;
		for (unsigned char j = 0 ; j < ORDER_LENGTH ; j++)
			lastOrder[i][j] = 0;
	}
	HAL_TIM_Base_Start_IT(htim);
}
void Buffer::freeWheel(unsigned char ROBOID)
{
	order[ROBOID][0] = ROBOID;
	for (unsigned char i = 1 ; i < ORDER_LENGTH ; i++)
		order[ROBOID][i] = 0;
	order[ROBOID][1] = 1;
	order[ROBOID][2] = 2;
	order[ROBOID][3] = 3;
	order[ROBOID][4] = 4;
}
void Buffer::clearResponse(unsigned char ROBOID)
{
	for (unsigned char i = 0 ; i < RESPONSE_LENGTH ; i++)
		response[ROBOID][i] = 0;
}
void Buffer::writeResponse(unsigned char ROBOID,char str[RESPONSE_LENGTH])
{
	lastResponseUpdate[ROBOID] = 0;
	for (unsigned char i = 0; i < RESPONSE_LENGTH ; i++)
		response[ROBOID][i] = str[i];
///*
	if (joystic.isValid && ROBOID == joystic.order[0])
		for (unsigned char i = 0; i < RESPONSE_LENGTH ; i++)
			joystic.response[i] = str[i];
//*/
}
void Buffer::writeOrder(unsigned char ROBOID,char str[ORDER_LENGTH])
{
	//char string[100];
	//unsigned char leng = sprintf(string,"Buffer::writeOrder();\r\n");
	//HAL_UART_Transmit(&huart2,(unsigned char *)string,leng,200);
	lastOrderUpdate[ROBOID] = 0;
	if (lastOrderEnabled[ROBOID])
	{
		bool flag = true;
		for (unsigned char i =0 ; i < ORDER_LENGTH; i ++)
		{
			if (lastOrder[ROBOID][i] != str[i])
			{
				flag = false;
				break;
			}
		}
		if (flag)
			return;
	}
	if (!lastOrderEnabled[ROBOID])
		lastOrderEnabled[ROBOID] = true;
	for (unsigned char i = 0 ; i < ORDER_LENGTH; i ++)
		lastOrder[ROBOID][i] = str[i];
	for (unsigned char i = 0 ; i < ORDER_LENGTH; i ++)
		order[ROBOID][i] = str[i];
///*
	if (joystic.isValid && ROBOID == joystic.order[0])
	{
		for (unsigned char i = 0;i < 7;i++)
		{
			lastOrder[ROBOID][i] = joystic.order[i];
			order[ROBOID][i] = joystic.order[i];
		}
		lastOrder[ROBOID][15] = joystic.order[15];
		order[ROBOID][15] = joystic.order[15];
	}
//*/
}
void Buffer::timePassed(TIM_HandleTypeDef* htim)
{
	if (htim != Buffer::htim)
		return;
	for (unsigned char i = 0; i< MAX_ROBOT;i++)
	{
		if (lastOrderUpdate[i] < DELAY_BEFORE_FREEWHEEL + 10)
			lastOrderUpdate[i]++;
		if (lastResponseUpdate[i] < DELAY_BEFORE_DISCONNECT + 10)
			lastResponseUpdate[i]++;
		if (lastOrderUpdate[i] == DELAY_BEFORE_FREEWHEEL)
			freeWheel(i);
		if (lastResponseUpdate[i] == DELAY_BEFORE_DISCONNECT)
			clearResponse(i);
	}
}


