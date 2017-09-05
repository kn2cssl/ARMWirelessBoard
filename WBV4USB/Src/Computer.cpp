#include "Computer.hpp"
#include "usbd_cdc_if.h"
#include "main.h"
#include <string.h>

unsigned char Computer::header[HEADER_LENGTH] = { 0 };
unsigned char Computer::data[MAX_NRF_DATA_LENGTH + 3] = { 0 };
unsigned char Computer::response[MAX_ROBOT][RESPONSE_LENGTH] = { 0 };
unsigned char Computer::order[MAX_ROBOT][ORDER_LENGTH] = { 0 };
bool Computer::writeOrderRequest[MAX_ROBOT] = {false};

void Computer::init(void)
{
	for (unsigned char i = 0 ; i < MAX_ROBOT ; i++)
	{
		freeWheel(i);
		responceClear(i);
		writeOrderRequest[i] = false;
	}
}
void Computer::freeWheel(unsigned char ROBOID)
{
	order[ROBOID][0] = ROBOID;
	for (unsigned char i = 1 ; i < ORDER_LENGTH ; i++)
		order[ROBOID][i] = 0;
	order[ROBOID][1] = 1;
	order[ROBOID][2] = 2;
	order[ROBOID][3] = 3;
	order[ROBOID][4] = 4;
}
void Computer::responceClear(unsigned char ROBOID)
{
	for (unsigned char i = 0 ; i < RESPONSE_LENGTH ; i++)
		response[ROBOID][i] = 0;
}
void Computer::clearData(void)
{
	for (unsigned char i = 0 ; i < HEADER_LENGTH ; i++)
		header[i] = 0;
	for (unsigned char i = 0 ; i < MAX_NRF_DATA_LENGTH + 3 ; i++)
		data[i] = 0;
}
void Computer::parseHeader(void)
{
	unsigned char str[1] = { 0 };
	str[0] = NACK;
	if (header[0] == START_BYTE && header[3] == STOP_BYTE)
	{
		if (header[1] == WRITE_REQUEST || header[1] == READ_REQUEST)
		{
			if (header[2] < MAX_ROBOT)
			{
				str[0] = ACK;
				if (header[1] == READ_REQUEST)
				{
					readResponse(header[2]);
					return;
				}
				CDC_Transmit_FS(str,1);
				return;
			}
		}
	}
	CDC_Transmit_FS(str,1);
	clearData();
}
void Computer::listen(unsigned char* buffer, unsigned int* length)
{
	if (*length == HEADER_LENGTH + ORDER_LENGTH + 3 && header[0] == 0)
	{
		for (unsigned char i = 0 ; i < HEADER_LENGTH ; i++)
			header[i] = buffer[i];
		for (unsigned char i = HEADER_LENGTH ; i < HEADER_LENGTH + ORDER_LENGTH + 3 ; i++)
			data[i - HEADER_LENGTH] = buffer[i];
		if (header[0] == START_BYTE && header[1] == WRITE_REQUEST && header[2] <= MAX_ROBOT && header[3] == STOP_BYTE)
			writeOrder(header[2]);
	}
	/*else if (*length == ORDER_LENGTH + 3 && header[1] == WRITE_REQUEST)
	{
		for (unsigned char i = 0 ; i < ORDER_LENGTH + 3 ; i++)
			data[i] = buffer[i];
		writeOrder(header[2]);
	}*/
	else
	{
		unsigned char str[1] = { 0 };
		str[0] = NACK;
		CDC_Transmit_FS(str,1);
		clearData();
	}
}
void Computer::readResponse(unsigned char ROBOID)
{
	unsigned char str[RESPONSE_LENGTH + 4] = { 0 };
	str[0] = ACK;
	str[1] = START_BYTE;
	str[2] = DATA_REQUEST;
	for (unsigned char i = 0 ; i < RESPONSE_LENGTH ; i++)
		str[i + 3] = response[ROBOID][i];
	str[RESPONSE_LENGTH + 3] = STOP_BYTE;
	CDC_Transmit_FS(str,(RESPONSE_LENGTH + 4) * 2);
	clearData();
}
void Computer::writeOrder(unsigned char ROBOID)
{
	unsigned char str[1] = { 0 };
	if (data[0] == START_BYTE && data[1] == DATA_REQUEST && data[ORDER_LENGTH + 2] == STOP_BYTE)
	{
		for (unsigned char i = 0 ; i < ORDER_LENGTH ; i++)
			order[ROBOID][i] = data[i + 2];
		writeOrderRequest[ROBOID] = true;
		str[0] = ACK;
	}
	else 
		str[0] = NACK;
	CDC_Transmit_FS(str,1);
	clearData();
}
