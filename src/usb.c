/****************************************************************************
	[Project] FlexSEA: Flexible & Scalable Electronics Architecture
	[Sub-project] 'flexsea-strain' 6-ch Strain Gauge Amplifier
	Copyright (C) 2016 Dephy, Inc. <http://dephy.com/>

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************
	[Lead developper] Jean-Francois (JF) Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab 
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors] 
*****************************************************************************
	[This file] usb: USB CDC
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-26 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "usb.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8 buffer[RX_BUF_LEN];

//****************************************************************************
// Function(s)
//****************************************************************************

//Initialize the USB peripheral
//Returns 0 is success, 1 if timeout (happens when the cable is unplugged)
uint8 init_usb(void)
{
	uint16 cnt = 0, flag = 0;
	
	//Start USBFS Operation with 3V operation
    USBUART_1_Start(0u, USBUART_1_5V_OPERATION);
	
	//Wait for Device to enumerate */
    //while(!USBUART_1_GetConfiguration());
	for(cnt = 0; cnt < USB_ENUM_TIMEOUT; cnt++)
	{
		if(USBUART_1_GetConfiguration())
		{
			flag = 1;
			break;
		}
		CyDelay(1);
	}

	if(flag)	
	{
	    //Enumeration is done, enable OUT endpoint for receive data from Host
	    USBUART_1_CDC_Init();	
		return 1;		//Success
	}
	
	return 0;	//Timeout	
}

void get_usb_data(void)
{
	static 	int16 count = 0;
	
	//USB Data
	if(USBUART_1_DataIsReady() != 0u)               	//Check for input data from PC
	{   
		count = USBUART_1_GetAll(buffer);           	//Read received data and re-enable OUT endpoint
		if(count != 0u)
		{
			//Store all bytes in rx buf:			
			update_rx_buf_array_usb(buffer, count+1);
		
			data_ready_usb++;
		}
    } 	
}

//1 byte through USB - ho headers, raw data
void send_usb_int8(char payload)
{
	char8 tmp[16]; 
	tmp[0] = (payload & 0xFF);
	
	//Make sure that the peripheral is ready
	while(USBUART_1_CDCIsReady() == 0u);	//ToDo Add timeout!
	USBUART_1_PutData((const uint8*)tmp, 1);

	return;
}

//1 byte through USB - ho headers, raw data
void send_usb_uint8(uint8 payload)
{
	uint8 tmp[16]; 
	tmp[0] = (payload & 0xFF);
	
	//Make sure that the peripheral is ready
	while(USBUART_1_CDCIsReady() == 0u);	//ToDo Add timeout!
	USBUART_1_PutData((const uint8*)tmp, 1);

	return;
}

//Chops a int16 in 2 bytes and send them through USB - ho headers, raw data
//Works with Matlab
void send_usb_int16(int16 payload)
{
	int tmp = 0;
	char8 packet[4];
	
	//MSB first
	tmp = (payload >> 8) & 0xFF;
	packet[1] = (uint8) tmp;
	tmp = (payload) & 0xFF;
	packet[0] = (uint8) tmp;
	
	//Make sure that the peripheral is ready
	while(USBUART_1_CDCIsReady() == 0u);	//ToDo Add timeout!
	USBUART_1_PutData((const uint8*)packet, 2);

	return;
}

//Chops a int32 in 4 bytes and send them through USB - ho headers, raw data
void send_usb_int32(int payload)
{
	int tmp = 0;
	char8 packet[4];
	
	//MSB first
	tmp = (payload >> 24) & 0xFF;
	packet[0] = (uint8) tmp;
	tmp = (payload >> 16) & 0xFF;
	packet[1] = (uint8) tmp;
	tmp = (payload >> 8) & 0xFF;
	packet[2] = (uint8) tmp;
	tmp = (payload) & 0xFF;
	packet[3] = (uint8) tmp;
	
	//Make sure that the peripheral is ready
	while(USBUART_1_CDCIsReady() == 0u);	//ToDo Add timeout!
	USBUART_1_PutData((const uint8*)packet, 4);

	return;
}

void usb_puts(uint8 *buf, uint32 len)
{
	if(USBUART_1_CDCIsReady() == 1)
		USBUART_1_PutData(( const uint8*)buf, len);
}

//****************************************************************************
// Test Function(s) - Use with care!
//****************************************************************************

//Returns chars to PC. ToDo: make it non-blocking!
uint8 usb_echo_blocking(void)
{
	static 	int16 count = 0;
	uint16 i = 0;
	
	//USB Data
	if(USBUART_1_DataIsReady() != 0u)               	//Check for input data from PC
	{   
		count = USBUART_1_GetAll(buffer);           	//Read received data and re-enable OUT endpoint
		if(count != 0u)
		{
			while(USBUART_1_CDCIsReady() == 0u);    	//Wait till component is ready to send more data to the PC
			USBUART_1_PutData(buffer, count);   		//Send data back to PC

			//Store all bytes in rx buf:			
			for(i = 0; i < count; i++)
		    {
		        update_rx_buf_byte_usb(buffer[i]);
		    }	
			//ToDo should use array update
			
			return 1;	//Got byte(s)
		}
    } 
	
	return 0;	//No byte
}

//****************************************************************************
// Deprecated Function(s)
//****************************************************************************

//Send 4 uint16 to the terminal
int16 send_usb_packet(uint16 word1, uint16 word2, uint16 word3, uint16 word4)
{
	static uint8 cnt = 0;
	char8 packet[18];

	cnt++;
	
	packet[0] = 0xAA;	//Start byte	

	packet[2] = ((word1 & 0xFF00) >> 8);
	packet[3] = ((word1 & 0x00FF));
	
	packet[4] = ((word2 & 0xFF00) >> 8);
	packet[5] = ((word2 & 0x00FF));
	
	packet[6] = ((word3 & 0xFF00) >> 8);
	packet[7] = ((word3 & 0x00FF));
	
	packet[8] = ((word4 & 0xFF00) >> 8);
	packet[9] = ((word4 & 0x00FF));
	

	packet[15] = 0xCC;	//End byte
	packet[16] = '\n';
	packet[17] = '\0';
	
	//Make sure that the peripheral is ready
	while(USBUART_1_CDCIsReady() == 0u);	//ToDo Add timeout!
	USBUART_1_PutData((const uint8*)packet, 18);

	return 0;
}
