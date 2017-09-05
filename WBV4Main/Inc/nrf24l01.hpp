/*
 * @author
 * Copyright (C) 2012 Luis R. Hilario http://www.luisdigital.com
 *
 */
#ifndef NRF24L01_H_
#define NRF24L01_H_
//#include <asf.h>
//#include <conf_board.h>
#include "stm32f4xx_hal.h"
#include "spi.h"
#include "gpio.h"

#define	_CH_0 50			// Channel 0..125
#define	_CH_1 1
#define	_CH_2 125
#define	_CH_3 100
#define JOYSTIC_CHANNEL 85
#define	_Address_Width	5	// 3..5
#define _Buffer_Size 32 // 1..32

/************************************************************************/
/* Board Initialize                                                     */
/************************************************************************/


// # define NRF24L01_CLK1_LINE  PIN0_bm
// # define NRF24L01_DATA1_LINE PIN1_bm
// # define NRF24L01_DR2_LINE
// # define NRF24L01_CLK2_LINE
// # define NRF24L01_DATA2_LINE

/* IO direction control definitions */

// # define NRF24L01_set_ce_output()    (NRF24L01_PORTDIRSET = NRF24L01_CE_LINE)
// # define NRF24L01_set_cs_output()    (NRF24L01_PORTDIRSET = NRF24L01_CS_LINE)
// # define NRF24L01_set_clk1_output()  (NRF24L01_PORTDIRSET = NRF24L01_CLK1_LINE)
// # define NRF24L01_set_data1_output() (NRF24L01_PORTDIRSET = NRF24L01_DATA1_LINE)
// # define NRF24L01_set_data1_input()  (NRF24L01_PORTDIRCLR = NRF24L01_DATA1_LINE)
// # define NRF24L01_set_IRQ_input()    (NRF24L01_PORTDIRCLR = NRF24L01_IRQ_LINE)
// # define NRF24L01_set_clk2_output()  (NRF24L01_PORTDIRSET = NRF24L01_CLK2_LINE)
// # define NRF24L01_set_data2_output() (NRF24L01_PORTDIRSET = NRF24L01_DATA2_LINE)
// # define NRF24L01_set_data2_input()  (NRF24L01_PORTDIRCLR = NRF24L01_DATA2_LINE)
// # define NRF24L01_set_dr2_input()    (NRF24L01_PORTDIRCLR = NRF24L01_DR2_LINE)

/* IO level sense & control definitions */
//# define bit_is_set_(_PORT_,_PIN_)  (_PORT_&_PIN_)
// # define NRF24L01_data1_is_high()  (NRF24L01_PORT_IN & NRF24L01_DATA1_LINE)
// # define NRF24L01_data2_is_high()  (NRF24L01_PORT_IN & NRF24L01_DATA2_LINE)
// # define NRF24L01_set_ce_high()    (NRF24L01_PORTOUTSET = NRF24L01_CE_LINE)
// # define NRF24L01_set_ce_low()     (NRF24L01_PORTOUTCLR = NRF24L01_CE_LINE)
// # define NRF24L01_set_cs_high()    (NRF24L01_PORTOUTSET = NRF24L01_CS_LINE)
// # define NRF24L01_set_cs_low()     (NRF24L01_PORTOUTCLR = NRF24L01_CS_LINE)
// # define NRF24L01_set_data1_high() (NRF24L01_PORTOUTSET = NRF24L01_DATA1_LINE)
// # define NRF24L01_set_data1_low()  (NRF24L01_PORTOUTCLR = NRF24L01_DATA1_LINE)
// # define NRF24L01_set_clk1_high()  (NRF24L01_PORTOUTSET = NRF24L01_CLK1_LINE)
// # define NRF24L01_set_clk1_low()   (NRF24L01_PORTOUTCLR = NRF24L01_CLK1_LINE)
// # define NRF24L01_dr1_is_high()    (NRF24L01_PORT_IN & NRF24L01_IRQ_LINE)
// # define NRF24L01_set_data2_high() (NRF24L01_PORTOUTSET = NRF24L01_DATA2_LINE)
// # define NRF24L01_set_data2_low()  (NRF24L01_PORTOUTSET = NRF24L01_DATA2_LINE)
// # define NRF24L01_set_clk2_high()  (NRF24L01_PORTOUTSET = NRF24L01_CLK2_LINE)
// # define NRF24L01_set_clk2_low()   (NRF24L01_PORTOUTCLR = NRF24L01_CLK2_LINE)
// # define NRF24L01_dr2_is_high()    (NRF24L01_PORT_IN & NRF24L01_DR2_LINE)


/* Port 0, bit 17 is set to GPIO output (NRF24L01 CE) */
//#define NRF24L01_CE_OUT		NRF24L01_set_ce_output()


// Bits
#define TX_REUSE (1<<6)
#define TX_FULL  (1<<5)
#define TX_EMPTY (1<<4)
#define RX_FULL  (1<<1)
#define RX_EMPTY (1<<0)

/**
 * Data Ready RX FIFO interrupt
 */
#define _RX_DR (1<<6)

/**
 * Data Sent TX FIFO interrupt
 */
#define _TX_DS (1<<5)

/**
 * Maximum number of TX retransmits interrupt
 */
#define _MAX_RT (1<<4)

/** Power Down mode
 *
 * Minimal current consumption, SPI can be activated
 *
 * @see NRF24L01_Set_Power(char Mode)
 */
#define _POWER_DOWN	0

/** Power Up mode
 *
 * Standby-I mode
 *
 * @see NRF24L01_Set_Power(char Mode)
 */
#define _POWER_UP	(1<<1)

/** Mode radio transmitter
 *
 * @see NRF24L01_Set_Device_Mode(char Device_Mode)
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define _TX_MODE	0

/** Mode radio receiver
 *
 * @see NRF24L01_Set_Device_Mode(char Device_Mode)
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define _RX_MODE	1

/**  Air data rate = 1 Mbps
 *
 *
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define	_1Mbps	0

/** Air data rate = 2 Mbps
 *
 * @see NRF24L01_Init(char Device_Mode, char CH, char DataRate,
		char *Address, char Address_Width, char Size_Payload)
 */
#define	_2Mbps	(1<<3)

/** Enable ShockBurst

 Automatic Retransmission (Up to 1 Re-Transmit on fail of AA)

 Auto Acknowledgment (data pipe 0)

 @see NRF24L01_Set_ShockBurst(char Mode)
 */
#define _ShockBurst_ON 1

/** Disable ShockBurst
 *
 @see NRF24L01_Set_ShockBurst(char Mode)
 */
#define _ShockBurst_OFF 0

/************************************************************************/
/* Transmit Power                                                                     */
/************************************************************************/

#define RF_PWR_LOW 0x00<<1 //-18dBm
#define RF_PWR_MED 0x01<<1 //-12dBm
#define RF_PWR_HI  0x02<<1 //-6dBm
#define RF_PWR_MAX 0x03<<1 //0dBm

// REGISTERS
#define CONFIG		0x00
#define EN_AA		0x01
#define EN_RXADDR	0x02
#define SETUP_AW	0x03
#define SETUP_RETR	0x04
#define RF_CH		0x05
#define RF_SETUP	0x06
#define STATUSe		0x07
#define OBSERVE_TX	0x08
#define CD			0x09
#define RX_ADDR_P0	0x0A
#define RX_ADDR_P1	0x0B
#define RX_ADDR_P2	0x0C
#define RX_ADDR_P3	0x0D
#define RX_ADDR_P4	0x0E
#define RX_ADDR_P5	0x0F
#define TX_ADDR		0x10
#define RX_PW_P0	0x11
#define RX_PW_P1	0x12
#define RX_PW_P2	0x13
#define RX_PW_P3	0x14
#define RX_PW_P4	0x15
#define RX_PW_P5	0x16
#define FIFO_STATUS	0x17
#define DYNPD		0x1C
#define FEATURE		0x1D

// COMMANDS
#define R_REGISTER			0x00
#define W_REGISTER			0x20
#define R_RX_PAYLOAD        0x61
#define W_TX_PAYLOAD        0xA0
#define FLUSH_TX      		0xE1
#define FLUSH_RX	 		0xE2
#define REUSE_TX_PL  		0xE3
#define ACTIVATE            0x50
#define R_RX_PL_WID         0x60
#define W_ACK_PAYLOAD		0xA8
#define W_TX_PAYLOAD_NOACK	0x58
#define NOP                 0xFF

class NRF
{
	public:
	//SPI module represented by HAL SPI
	SPI_HandleTypeDef* module;
	//Transmit bit on module PORT and pin in HAL GPIO
	GPIO_TypeDef* CEPort;
	unsigned int CEPin;
	//SPI slect pin and port in HAL GPIO
	GPIO_TypeDef* CSPort;
	unsigned int CSPin;
	/*
	* SPI functions for NRF24L01
	*/
	char SPI_L(char TX_Data);
	char readReg(char Reg);
	char writeReg(char Reg, char Value);
	char readRegBuf(char Reg, char *Buf, int Size);
	char writeRegBuf(char Reg, char *Buf, int Size);
	/*
	* NRF24L01 functions
	*/
	char get_Status(void);
	char get_CD(void);
	void set_Power(char Mode);
	void set_CH(char CH);
	void set_ShockBurst(char Mode);
	void set_Address_Width(char Width);
	void set_Device_Mode(char Device_Mode);
	void set_RX_Pipe(char PipeNum, char *Address, int AddressSize, char PayloadSize);
	void disable_All_Pipes(void);
	char clear_Interrupts(void);
	void set_TX_Address(char *Address, int Size);
	void flush_TX(void);
	void flush_RX(void);
	void init(char Device_Mode, char CH, char DataRate,char *Address, char Address_Width, char Size_Payload, char Tx_Power);
	void init_milad(char Device_Mode, char CH, char DataRate,char *Address, char Address_Width, char Size_Payload, char Tx_Power);
	void RF_TX(void);
	void write_TX_Buf(char *Buf, int Size);
	void read_RX_Buf(char *Buf, int Size);
	void clear_All(void);
	void receive(char Buf[_Buffer_Size]);
	void send(char Buf[_Buffer_Size]);
};


#endif /*NRF24L01_H_*/
