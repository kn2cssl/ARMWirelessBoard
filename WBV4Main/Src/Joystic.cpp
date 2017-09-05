#include "Joystic.hpp"

bool Joystic::isValid;
unsigned char Joystic::order[ORDER_LENGTH];
unsigned char Joystic::response[RESPONSE_LENGTH];
NRF* Joystic::hnrf;
TIM_HandleTypeDef* Joystic::htim;
unsigned int Joystic::roboLastUpdate;
				
void Joystic::NRF_Init()
{
	char Address[5] = { 0x11,0x22,0x33,0x44,0x00};
	hnrf -> clear_Interrupts();
	hnrf -> flush_TX();
	hnrf -> flush_RX();
	hnrf -> init_milad(_RX_MODE,JOYSTIC_CHANNEL,_2Mbps,Address,_Address_Width,_Buffer_Size,RF_PWR_MAX);
	hnrf -> set_RX_Pipe(0,Address,5,32);
	hnrf -> set_TX_Address(Address,5);
	hnrf -> writeReg(W_REGISTER | DYNPD,0x01);
	hnrf -> writeReg(W_REGISTER | FEATURE,0x06);
	hnrf -> clear_All();
	HAL_Delay(2);
	Receive(true);
}
bool Joystic::Data_Ready()
{
	Receive(false);
	unsigned char status = hnrf -> get_Status();
	Receive(true);
	if (((status & 0x40) == 0x40) && (status != 0xFF))
		return true;
	return false;
}
void Joystic::Clear_Data()
{
	for (unsigned char i = 0;i < ORDER_LENGTH;i++)
		order[i] = 0;
	for (unsigned char i = 0;i < RESPONSE_LENGTH;i++)
		response[i] = 0;
	isValid = false;
}
void Joystic::Init(NRF* hnrf,TIM_HandleTypeDef* htim)
{
	Joystic::hnrf = hnrf;
	Joystic::htim = htim;
	roboLastUpdate = 0;
	Clear_Data();
	Receive(false);
	NRF_Init();
}
void Joystic::Receive(bool state)
{
	if (state)
		HAL_GPIO_WritePin(hnrf -> CEPort,hnrf -> CEPin,GPIO_PIN_SET);
	else 
		HAL_GPIO_WritePin(hnrf -> CEPort,hnrf -> CEPin,GPIO_PIN_RESET);
	for (unsigned long int j = 0; j < 30000 ; j++);
}
void Joystic::Read_Data()
{
	char str[32];
	hnrf -> read_RX_Buf(str,32);
	hnrf -> clear_All();
	for (unsigned char i = 0; i < ORDER_LENGTH ; i++)
		order[i] = str[i];
}
void Joystic::Write_Data()
{
	char str[32];
	for (unsigned char i = 0; i < RESPONSE_LENGTH ; i++)
		str[i] = response[i];
	hnrf -> clear_All();
	hnrf -> write_TX_Buf(str,32);
}
void Joystic::Update()
{
	Receive(false);
	if (Data_Ready())
	{
		Read_Data();
		Write_Data();
		isValid = true;
		roboLastUpdate = 0;
	}
	Receive(true);
}
void Joystic::Time_Passed(TIM_HandleTypeDef* htim)
{
	if (htim != Joystic::htim)
		return;
	if (roboLastUpdate < DELAY_BEFORE_DISCONNECT + 10)
		roboLastUpdate++;
	if (roboLastUpdate == DELAY_BEFORE_DISCONNECT)
		Clear_Data();
}
