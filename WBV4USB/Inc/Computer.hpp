#ifndef __COMPUTER_HPP__
#define __COMPUTER_HPP__

#include <stdbool.h>

#ifndef __SETTINGS__
#define __SETTINGS__

#define HEADER_LENGTH 4 
#define ORDER_LENGTH 18 // old length 11 //@mparchin: last definition was DATA_LENGTH
#define RESPONSE_LENGTH 32 //@mparchin: data lenght for returned data from robots to computer
#define MAX_ROBOT 12 //@mparchin: we must change this shit either to 16(maximum robot support) or 8(maximum speed) 
#define MAX_NRF_DATA_LENGTH 32
#define START_BYTE 0xAA
#define STOP_BYTE 0x55
#define WRITE_REQUEST 'W'
#define READ_REQUEST 'R'
#define DATA_REQUEST 'D'
#define ACK 0x99
#define NACK 0x66

#endif

//@mparchin: copy to usbd_cdc_if.c and replace the definition of the function
/*
static int8_t CDC_Receive_FS (uint8_t* Buf, uint32_t *Len)
{
  computer.listen(Buf,Len);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  return (USBD_OK);
}
*/

/*
//write 
//computer side
START
WRITE_ORDER
ID
STOP
START
DATA_ORDER
DATA[18]
STOP
//response
//ACK or NACK



//read
//computer side
START
READ_ORDER
ID
STOP
//response
//DATA[32]


*/

static class Computer
{
	private:
		static unsigned char header[HEADER_LENGTH];
		static unsigned char data[MAX_NRF_DATA_LENGTH + 3];
		void freeWheel(unsigned char ROBOID);
		void parseHeader(void);
		void clearData(void);
		void writeOrder(unsigned char ROBOID);
		void readResponse(unsigned char ROBOID);
		void responceClear(unsigned char ROBOID);
	public:
		static bool writeOrderRequest[MAX_ROBOT];
		static unsigned char response[MAX_ROBOT][RESPONSE_LENGTH];
		static unsigned char order[MAX_ROBOT][ORDER_LENGTH];
		void init(void);
		void listen(unsigned char* buffer, unsigned int* length);
}computer;

#endif 
