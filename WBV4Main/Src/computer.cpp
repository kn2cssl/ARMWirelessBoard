#include "computer.hpp"

//bitte don't change
//unsigned char ComputerConnection::firstByte;
unsigned char ComputerConnection::state;
unsigned char ComputerConnection::header[4];
unsigned char ComputerConnection::dataBody[DATA_LENGTH * MAX_ROBOT];
UART_HandleTypeDef* ComputerConnection::huartReceive;
UART_HandleTypeDef* ComputerConnection::huartTransmit;
TIM_HandleTypeDef* ComputerConnection::htim;
unsigned int ComputerConnection::lastUpdate[MAX_ROBOT] = {0};
unsigned char ComputerConnection::index = 0;
bool ComputerConnection::transmitting = false;
unsigned char ComputerConnection::ack[MAX_ROBOT][34] = {0};
unsigned int ComputerConnection::lastACKUpdate[MAX_ROBOT] = {0};
//public and you have access read only
char ComputerConnection::buffer[MAX_ROBOT][32] = {0};


//public
void ComputerConnection::init(UART_HandleTypeDef* huartReceive,UART_HandleTypeDef* huartTransmit,TIM_HandleTypeDef* htim)
{
	ComputerConnection::huartTransmit = huartTransmit;
	ComputerConnection::huartReceive = huartReceive;
	ComputerConnection::htim = htim;
	ComputerConnection::initBuffer();
	ComputerConnection::getHeader();
	HAL_TIM_Base_Start_IT(ComputerConnection::htim);
	char buffer[32] = {0};
	ComputerConnection::ACK(0,buffer);
}
void ComputerConnection::listen(UART_HandleTypeDef* huartReceive)
{
	if (huartReceive != ComputerConnection::huartReceive)
		return;
	switch (ComputerConnection::state)
	{
		case 0:
			if (ComputerConnection::header[0] == START_BYTE0 && ComputerConnection::header[1] == START_BYTE1)
			{
				ComputerConnection::state = 1;
				unsigned char remaningDataLen = (ComputerConnection::header[2] -  HEADER_LENGTH );
				HAL_UART_Receive_IT(ComputerConnection::huartReceive,ComputerConnection::dataBody,remaningDataLen);
			}
			else
				ComputerConnection::getHeader();
		break;
		case 1:
			unsigned char remaningDataLen = (ComputerConnection::header[2] -  HEADER_LENGTH );
		if (ComputerConnection::dataBody[remaningDataLen - 1] == STOP_BYTE && ComputerConnection::checkSum() == true)
			{
				ComputerConnection::updateBuffer();
			}
			ComputerConnection::getHeader();
		break;
	};
}
void ComputerConnection::updateBuffer(void)
{
	unsigned char numberOfRobots = (ComputerConnection::header[2] -  HEADER_LENGTH - STOP_BYTE_SIZE)/DATA_LENGTH;
	for (unsigned char i = 0;i < numberOfRobots ; i++)
	{
		if (ComputerConnection::dataBody[i * DATA_LENGTH] < MAX_ROBOT)
		{
			unsigned char robotID = ComputerConnection::dataBody[i * DATA_LENGTH];
			for (unsigned char j = 0; j < DATA_LENGTH;j++)
				ComputerConnection::buffer[robotID][j] = ComputerConnection::dataBody[i * DATA_LENGTH + j];
		}
	}
}
bool ComputerConnection::checkSum(void)
{
	unsigned char remaningDataLen = (ComputerConnection::header[2] -  HEADER_LENGTH ) - 1;
	unsigned char temp = ComputerConnection::header[3];
	for (unsigned long int i = 0 ; i < remaningDataLen ; i++)
		temp -= ComputerConnection::dataBody[i];
	if (temp == 0)
		return true;
	return false;
}
void ComputerConnection::getHeader()
{
	ComputerConnection::state = 0;
	ComputerConnection::clearData();
	HAL_UART_Receive_IT(ComputerConnection::huartReceive,ComputerConnection::header,HEADER_LENGTH);
}
void ComputerConnection::initBuffer(void)
{
	for (int i = 0; i < MAX_ROBOT; i++)
	{
		ComputerConnection::buffer[i][0] = i;
		ComputerConnection::freeWheel(i);
		ComputerConnection::ack[i][0] = 'a' + i;
		for (int j = 0 ; j < 32; j ++)
			ComputerConnection::ack[i][1 + j] = 0;
		ComputerConnection::ack[i][33] = 'z';
	}
}
void ComputerConnection::clearACK(unsigned char roboID)
{
	for (unsigned char i = 1 ; i < 33 ; i++)
		ComputerConnection::ack[roboID][i] = 0;
}
void ComputerConnection::freeWheel(unsigned char roboID)
{
	for (int i = 1;i < MAX_ROBOT;i++)
		ComputerConnection::buffer[roboID][i] = 0;
	ComputerConnection::buffer[roboID][1] = 1;
	ComputerConnection::buffer[roboID][2] = 2;
	ComputerConnection::buffer[roboID][3] = 3;
	ComputerConnection::buffer[roboID][4] = 4;
}
void ComputerConnection::clearData()
{
	for (unsigned char i = 0 ; i < HEADER_LENGTH; i++ )
		ComputerConnection::header[i] = 0;
	for (unsigned long int i = 0 ; i < DATA_LENGTH * MAX_ROBOT ; i++)
		ComputerConnection::dataBody[i] = 0;
}
void ComputerConnection::timePassed(TIM_HandleTypeDef* htim)
{
	if (htim != ComputerConnection::htim)
		return;
	for (unsigned char i = 0;i< MAX_ROBOT; i++)
	{
		ComputerConnection::lastUpdate[i]++;
		ComputerConnection::lastACKUpdate[i]++;
		if (ComputerConnection::lastUpdate[i] >= DELAY_BEFORE_FREEWHEEL)
		{
			ComputerConnection::lastUpdate[i] = 0;
			ComputerConnection::freeWheel(i);
		}
		if (ComputerConnection::lastACKUpdate[i] >= DELAY_BEFORE_DISCONNECT)
		{
			ComputerConnection::lastACKUpdate[i] = 0;
			ComputerConnection::clearACK(i);
		}
	}
}
void ComputerConnection::ACK(unsigned char robotID,char ACKbuffer[32])
{
	ComputerConnection::lastACKUpdate[robotID] = 0;
	for (unsigned char i = 0 ; i < 32; i++)
	{
		ComputerConnection::ack[robotID][i + 1] = ACKbuffer[i];
	}
	if (ComputerConnection::transmitting == false)
	{
		ComputerConnection::transmitting = true;
		//ComputerConnection::index = robotID;
		HAL_UART_Transmit_IT(ComputerConnection::huartTransmit,ComputerConnection::ack[ComputerConnection::index],34);
		//HAL_Delay(1);
	}
}
void ComputerConnection::transmit(UART_HandleTypeDef* huartTransmit)
{
	//return;
	ComputerConnection::index ++;
	if (ComputerConnection::index == MAX_ROBOT)
		ComputerConnection::index = 0;
	HAL_UART_Transmit_IT(ComputerConnection::huartTransmit,ComputerConnection::ack[ComputerConnection::index],34);	
}

		
