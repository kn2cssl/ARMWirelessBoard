#include "buffer.hpp"
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "Robot.hpp"
#include "main.h"

void Robot::init(NRF* hnrf1,NRF* hnrf2)
{
	Robot::hnrf1 = hnrf1;
	Robot::hnrf2 = hnrf2;
	Robot::nrfInit(hnrf1);
	Robot::nrfInit(hnrf2);
	LEDs[0].port = RID0_GPIO_Port;
	LEDs[0].pin = RID0_Pin;
	LEDs[1].port = RID1_GPIO_Port;
	LEDs[1].pin = RID1_Pin;
	LEDs[2].port = RID2_GPIO_Port;
	LEDs[2].pin = RID2_Pin;
	LEDs[3].port = RID3_GPIO_Port;
	LEDs[3].pin = RID3_Pin;
	LEDs[4].port = RID4_GPIO_Port;
	LEDs[4].pin = RID4_Pin;
	LEDs[5].port = RID5_GPIO_Port;
	LEDs[5].pin = RID5_Pin;
	LEDs[6].port = RID6_GPIO_Port;
	LEDs[6].pin = RID6_Pin;
	LEDs[7].port = RID7_GPIO_Port;
	LEDs[7].pin = RID7_Pin;
	LEDs[8].port = RID8_GPIO_Port;
	LEDs[8].pin = RID8_Pin;
	LEDs[9].port = RID9_GPIO_Port;
	LEDs[9].pin = RID9_Pin;
	LEDs[10].port = RIDA_GPIO_Port;
	LEDs[10].pin = RIDA_Pin;
	LEDs[11].port = RIDB_GPIO_Port;
	LEDs[11].pin = RIDB_Pin;
	LEDs[12].port = RIDC_GPIO_Port;
	LEDs[12].pin = RIDC_Pin;
	LEDs[13].port = RIDD_GPIO_Port;
	LEDs[13].pin = RIDD_Pin;
	LEDs[14].port = RIDE_GPIO_Port;
	LEDs[14].pin = RIDE_Pin;
	LEDs[15].port = RIDF_GPIO_Port;
	LEDs[15].pin = RIDF_Pin;
	LEDs[0].on();
	HAL_Delay(25);
	for (unsigned char  i = 1;i < 16;i++)
	{
		LEDs[i-1].off();
		LEDs[i].on();
		HAL_Delay(25);
	}
	LEDs[15].off();
}
void Robot::nrfInit(NRF* hnrf)
{
	char Address[5] = { 0x11,0x22,0x33,0x44,0x11};
	hnrf -> clear_Interrupts();
	hnrf -> flush_TX();
	hnrf -> flush_RX();
	hnrf -> init_milad(_TX_MODE,_CH_1,_2Mbps,Address,_Address_Width,_Buffer_Size,RF_PWR_MAX);
	hnrf -> writeReg(W_REGISTER | DYNPD,0x01);
	hnrf -> writeReg(W_REGISTER | FEATURE,0x06);
	hnrf -> clear_All();
	HAL_Delay(2);
}
void Robot::changeRobot(NRF* hnrf,unsigned char robotID)
{
	char Address[5] = { 0x11,0x22,0x33,0x44,0x11};
	Address[4] = (robotID << 4) | robotID;
	hnrf -> set_RX_Pipe(0,Address,5,32);
	hnrf -> set_TX_Address(Address,5);
	if (robotID < MAX_ROBOT/2 )
		hnrf -> set_CH(_CH_1);
	else
		hnrf -> set_CH(_CH_0);
}
void Robot::writeBuffer(NRF* hnrf,char str[32])
{
	hnrf -> clear_All();
	hnrf -> write_TX_Buf(str,32);
}
void Robot::readBuffer(NRF* hnrf,char str[32])
{
	hnrf -> read_RX_Buf(str,32);
	hnrf -> clear_All();
}
void Robot::transmit()
{
	HAL_GPIO_WritePin(hnrf1 -> CEPort,hnrf1 -> CEPin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(hnrf2 -> CEPort,hnrf2 -> CEPin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(hnrf1 -> CEPort,hnrf1 -> CEPin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(hnrf2 -> CEPort,hnrf2 -> CEPin,GPIO_PIN_SET);
	for (unsigned long int j = 0; j < 30000 ; j++);
	//HAL_Delay(1);
	HAL_GPIO_WritePin(hnrf1 -> CEPort,hnrf1 -> CEPin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(hnrf2 -> CEPort,hnrf2 -> CEPin,GPIO_PIN_RESET);
	//for (unsigned long int j = 0; j < 60000 ; j++);
	//HAL_Delay(1);
}
bool Robot::dataReady(NRF* hnrf)
{
	unsigned char status = hnrf -> get_Status();
	//LEDs[14].on();
	//char stringToUart[100];
	//unsigned char lengthToUart = sprintf(stringToUart,"status = %x\r\n",status);
	//HAL_UART_Transmit(&huart2,(unsigned char *)stringToUart,lengthToUart,200);
	if ((status & 0x40) == 0x40 )
		return true;
	return false;
}
void Robot::update()
{
/*	
	if (joystic.isValid)
	{
		for (unsigned char i = 0;i < 7;i++)
			buffer.order[joystic.order[0]][i] = joystic.order[i];
		for (unsigned char i = 0;i < RESPONSE_LENGTH;i++)
			joystic.response[i] = buffer.response[joystic.order[0]][i];
	}
	*/
	for (unsigned char i = 0 ; i < MAX_ROBOT ; i++)
	{
		if (buffer.response[i][15] > 15 && buffer.response[i][15] < 60)
			LEDs[i].on();
		else 
			LEDs[i].off();
	}
	char stringToUart[100];
	unsigned char lengthToUart = sprintf(stringToUart,"ss = %d \t Vx = %d \t Vy = %d \t Vw = %d \t Shoot = %d \r\n",buffer.response[0x08][15],(signed int)(buffer.order[0x08][1] * 256 + buffer.order[0x08][2]),(signed int)(buffer.order[0x08][3] * 256 + buffer.order[0x08][4]),(signed int)(buffer.order[0x08][5] * 256 + buffer.order[0x08][6]),buffer.order[0x08][15]);
	HAL_UART_Transmit_IT(&huart2,(unsigned char *)stringToUart,lengthToUart);
	for (unsigned char i = 0 ; i < MAX_ROBOT / 2 ; i++)
	{
		unsigned char index1 = i,index3 = i + MAX_ROBOT / 2;
		changeRobot(hnrf1,index1);
		changeRobot(hnrf2,index3);
		writeBuffer(hnrf1,buffer.order[index1]);
		writeBuffer(hnrf2,buffer.order[index3]);
		transmit();
		//transmit();
		//transmit();
		//transmit();
		//LEDs[15].on();
		if (dataReady(hnrf1) == true)
		{
			char str[32] = {0};
			readBuffer(hnrf1,str);
			buffer.writeResponse(index1,str);
		}
		if (dataReady(hnrf2) == true)
		{
			char str[32] = {0};
			readBuffer(hnrf2,str);
			buffer.writeResponse(index3,str);
		}
	}
	if (MAX_ROBOT % 2 == 1)
	{
		unsigned char index = MAX_ROBOT - 1;
		changeRobot(hnrf1,index);
		writeBuffer(hnrf1,buffer.order[index]);
		transmit();
		if (dataReady(hnrf1) == true)
		{
			char str[32] = {0};
			readBuffer(hnrf1,str);
			buffer.writeResponse(index,str);
		}
	}
}
void LED::on(void)
{
	HAL_GPIO_WritePin(LED::port,LED::pin,GPIO_PIN_SET);
}
void LED::off(void)
{
	HAL_GPIO_WritePin(LED::port,LED::pin,GPIO_PIN_RESET);
}


