/** @mainpage
 @version 0.9
 @author
  Copyright (C) 2012 Luis R. Hilario http://www.luisdigital.com

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

 @section intro Introduction
 This library is intended to be universal and easy to use.

 Download source code here: http://www.luisdigital.com/proyectos/nrf24l01/nrf24l01.zip

 The files cpu_*.* contain code for a specific processor.

 Examples included for the following processors:

 NXP LPC11xx, LPC13xx "cpu_lpc1000.h"

 NXP LPC17xx "cpu_lpc1700.h"

 Parallax Propeller 1 "cpu_p8x32a.h"

 You can also provide examples for other micros.

 Please report any bug and / or solution you find.
*/



#include "NRF24L01.hpp"
/**
 Read a register

 @param Reg Register to read

 @return Registry Value
*/
char NRF::readReg(char Reg) {
	char Result;

	//NRF24L01_CS_LOW;
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_RESET);
	SPI_L(Reg);
	Result = SPI_L(NOP);
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_SET);
	//NRF24L01_CS_HIGH;

	return Result;
}

/**
 Returns the STATUS register and then modify a register

 @param Reg Register to change
 @param Value New value

 @return STATUS Register
*/
char NRF::writeReg(char Reg, char Value) 
{
	char Result;

	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_RESET);
	//NRF24L01_CS_LOW;
	Result = SPI_L(Reg);
	
	SPI_L(Value);
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_SET);
	//NRF24L01_CS_HIGH;

	return Result;
}

/**
 Returns the STATUS register and then read "n" registers

 @param Reg Register to read
 @param Buf Pointer to a buffer
 @param Size Buffer Size

 @return STATUS Register
*/
char NRF::readRegBuf(char Reg, char *Buf, int Size) 
{
	int i;
	char Result;
	
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_RESET);
	//NRF24L01_CS_LOW;
	Result = SPI_L(Reg);

	for (i = 0; i < Size; i++) {
		Buf[i] = SPI_L(NOP);
	}

	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_SET);
	//NRF24L01_CS_HIGH;

	return Result;
}

/**
 Returns the STATUS register and then write "n" registers

 @param Reg Registers to change
 @param Buf Pointer to a buffer
 @param Size Buffer Size

 @return STATUS Register
*/
char NRF::writeRegBuf(char Reg, char *Buf, int Size) 
{
	int i;
	char Result;

	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_RESET);
	//NRF24L01_CS_LOW;
	Result = SPI_L(Reg);

	for (i = 0; i < Size; i++) {
		SPI_L(Buf[i]);
	}
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_SET);
	//NRF24L01_CS_HIGH;

	return Result;
}

/**
 Returns the STATUS register

 @return STATUS Register
*/
char NRF::get_Status() 
{
	char Result;

	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_RESET);
	//NRF24L01_CS_LOW;
	SPI_L(0x07);     
	Result = SPI_L(NOP);
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_SET);
	//NRF24L01_CS_HIGH;

	return Result;
}

/**
 Returns the carrier signal in RX mode (high when detected)

 @return CD
*/
char NRF::get_CD()
{
	return (readReg(CD) & 1);
}

/**
 Select power mode

 @param Mode = _POWER_DOWN, _POWER_UP

 @see _POWER_DOWN
 @see _POWER_UP

*/
void NRF::set_Power(char Mode)
{
	char Result;
	/* Read Conf. Reg. AND Clear bit 1 (PWR_UP) and 7 (Reserved);*/
	Result = readReg(CONFIG);
	Result &= 0x7D;
	//Result &= 0b01111101;
	writeReg(W_REGISTER | CONFIG, Result | Mode);
	return;
}

/**
 Select the radio channel

 @param CH = 0..125

*/
void NRF::set_CH(char CH) 
{
	writeReg(W_REGISTER | RF_CH, CH & 0x7F); // Clear bit 8
}

/**
 Select Enhanced ShockBurst ON/OFF

 Disable this functionality to be compatible with NRF24L01

 @param Mode = _ShockBurst_ON, _ShockBurst_OFF

 @see _ShockBurst_ON
 @see _ShockBurst_OFF

*/

void NRF::set_ShockBurst(char Mode) {
	
	writeReg(W_REGISTER | SETUP_RETR, Mode);
	
	writeReg(W_REGISTER | EN_AA, Mode);
}

/**
 Select the address width

 @param Width = 3..5
*/
void NRF::set_Address_Width(char Width) {
	writeReg(W_REGISTER | SETUP_AW, 0x03/*(Width & 3) -2*/);
}

/**
 Select mode receiver or transmitter

 @param Device_Mode = _TX_MODE, _RX_MODE

 @see _TX_MODE
 @see _RX_MODE
*/
void NRF::set_Device_Mode(char Device_Mode) {
	char Result;

	Result = (readReg(CONFIG) & 0x7E); // Read Conf. Reg. AND Clear bit 0 (PRIM_RX) and 7 (Reserved)
	writeReg(W_REGISTER | CONFIG, Result | Device_Mode);
}

/**
 Enables and configures the pipe receiving the data

 @param PipeNum Number of pipe
 @param Address Address
 @param AddressSize Address size
 @param PayloadSize Buffer size, data receiver

*/
void NRF::set_RX_Pipe(char PipeNum, char *Address, int AddressSize, char PayloadSize) {
	char Result;

	Result = readReg(EN_RXADDR);
	writeReg(W_REGISTER | EN_RXADDR, Result | (1 << PipeNum));

	writeReg(W_REGISTER | (RX_PW_P0 + PipeNum), PayloadSize);
	
	writeRegBuf(W_REGISTER | (RX_ADDR_P0 + PipeNum), Address, AddressSize);
}

/**
 Disable all pipes
*/
void NRF::disable_All_Pipes() {
	writeReg(W_REGISTER | EN_RXADDR, 0);
}


/** Returns the STATUS register and then clear all interrupts
 *
 * @return STATUS Register
 */
char NRF::clear_Interrupts() {
	return writeReg(W_REGISTER | STATUSe, _RX_DR | _TX_DS | _MAX_RT);
}

/**
 Sets the direction of transmission

 @param Address Address
 @param Size Address size 3..5

*/
void NRF::set_TX_Address(char *Address, int Size) {
	writeRegBuf(W_REGISTER | TX_ADDR, Address, Size);
}

/**
 Empty the transmit buffer

*/
void NRF::flush_TX() {
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_RESET);
	//NRF24L01_CS_LOW;
	SPI_L(FLUSH_TX);
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_SET);
	//NRF24L01_CS_HIGH;
}

/**
 Empty the receive buffer
*/
void NRF::flush_RX() {
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_RESET);
	//NRF24L01_CS_LOW;
	SPI_L(FLUSH_RX);
	HAL_GPIO_WritePin(CSPort,CSPin,GPIO_PIN_SET);
	//NRF24L01_CS_HIGH;
}

/**
 Initializes the device
 @param Device_Mode = _TX_MODE, _RX_MODE
 @param CH = 0..125
 @param DataRate = _1Mbps, _2Mbps
 @param Address Address
 @param Address_Width Width direction: 3..5
 @param Size_Payload Data buffer size

 @see _TX_MODE
 @see _RX_MODE
 @see _1Mbps
 @see _2Mbps
*/
void NRF::init(char Device_Mode, char CH, char DataRate,char *Address, char Address_Width, char Size_Payload, char Tx_Power) {

	//NRF24L01_CE_OUT; // Set Port DIR out

	// Disable Enhanced ShockBurst
	set_ShockBurst(_ShockBurst_OFF);
	
	// RF output power in TX mode = 0dBm (Max.)
	// Set LNA gain
	writeReg(W_REGISTER | RF_SETUP, 0x01 | Tx_Power | DataRate);////////////

	set_Address_Width(Address_Width);//////////////////////

	set_RX_Pipe(0, Address, Address_Width, Size_Payload);

	set_CH(CH);

	set_TX_Address(Address, Address_Width); // Set Transmit address

	// Bits 4..6: Reflect interrupts as active low on the IRQ pin
	// Bit 3: Enable CRC
	// Bit 2: CRC 1 Byte
	// Bit 1: Power Up
	writeReg(W_REGISTER | CONFIG, 0x0A | Device_Mode);

	//delay_us(1500);
	HAL_Delay(2);
}
void NRF::init_milad(char Device_Mode, char CH, char DataRate,char *Address, char Address_Width, char Size_Payload, char Tx_Power) 
{

	//NRF24L01_CE_OUT; // Set Port DIR out

	// Enable Enhanced ShockBurst
	set_ShockBurst(_ShockBurst_OFF);
	writeReg(W_REGISTER | EN_AA, 0x01);//01
	writeReg(W_REGISTER | SETUP_RETR, 0x2F);//2f 
	//NRF24L01_WriteReg(W_REGISTER | FEATURE, 0x02);  //
	
	// RF output power in TX mode = 0dBm (Max.)
	// Set LNA gain
	writeReg(W_REGISTER | RF_SETUP, /*0b00000001 |*/ Tx_Power | DataRate);////////////

	set_Address_Width(Address_Width);//////////////////////

	set_RX_Pipe(0, Address, Address_Width, Size_Payload);

	set_CH(CH);

	set_TX_Address(Address, Address_Width); // Set Transmit address

	// Bits 4..6: Reflect interrupts as active low on the IRQ pin
	// Bit 3: Enable CRC
	// Bit 2: CRC 1 Byte
	// Bit 1: Power Up
	writeReg(W_REGISTER | CONFIG, 0x0E | Device_Mode);

	//delay_us(1500);
	HAL_Delay(2);
}

/**
 Turn on transmitter, and transmits the data loaded into the buffer
*/
void NRF::RF_TX() {
	HAL_GPIO_WritePin(CEPort,CEPin,GPIO_PIN_RESET);
	//NRF24L01_CE_LOW;
	HAL_GPIO_WritePin(CEPort,CEPin,GPIO_PIN_SET);
	//NRF24L01_CE_HIGH;
	//delay_us(10);
	for(unsigned int i = 0; i < 20000; i++);
	HAL_GPIO_WritePin(CEPort,CEPin,GPIO_PIN_RESET);
	//NRF24L01_CE_LOW;
}

/**
 Writes the buffer of data transmission

 @param Buf Buffer with data to send
 @param Size Buffer size

*/
void NRF::write_TX_Buf(char *Buf, int Size) {
	writeRegBuf(W_REGISTER | W_TX_PAYLOAD, Buf, Size);
}

/**
 Read the data reception buffer

 @param Buf Buffer with data received
 @param Size Buffer size

*/
void NRF::read_RX_Buf(char *Buf, int Size) {
	readRegBuf(R_RX_PAYLOAD, Buf, Size);
}


void NRF::clear_All()
{
	clear_Interrupts();
	flush_TX();
	flush_RX();
	writeReg(STATUSe,0x7E);
}

void NRF::receive(char Buf[_Buffer_Size]) {
	HAL_GPIO_WritePin(CEPort,CEPin,GPIO_PIN_SET);
	//NRF24L01_CE_HIGH;
	for(unsigned long int i = 0; i < 2600000; i++);
	//delay_us(130);

	while ((get_Status() & _RX_DR) != _RX_DR);

	HAL_GPIO_WritePin(CEPort,CEPin,GPIO_PIN_RESET);
	//NRF24L01_CE_LOW;

	read_RX_Buf(Buf, _Buffer_Size);
	clear_Interrupts();
}

void NRF::send(char Buf[_Buffer_Size]) {
	write_TX_Buf(Buf, _Buffer_Size);
	if((readReg(FIFO_STATUS) & TX_EMPTY) == TX_EMPTY )
		RF_TX();
	while ((get_Status() & _TX_DS) != _TX_DS)
	{
		
	}		
	clear_Interrupts();

}


char NRF::SPI_L(char TX_Data) 
{	
	unsigned char result;
	unsigned char data = TX_Data;
	HAL_SPI_TransmitReceive(module , &data , &result , 1 , 10);
	return (char)(result);
}
