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
	[This file] main_fsm: Contains all the case() code for the main FSM
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-10-05 | jfduval | Copied Execute & Manage
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "main_fsm.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8 eL0 = 0, eL1 = 0, eL2 = 0;
uint8 i = 0;
unsigned char result = 0;
uint8 cmd_ready_usb = 0;
static uint8 new_cmd_led = 0;
uint8_t tmp_rx_command_485[PAYLOAD_BUF_LEN];
uint8_t tmp_rx_command_usb[PAYLOAD_BUF_LEN];
uint8 i2c_time_share = 0;

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************	

//****************************************************************************
// Public Function(s)
//****************************************************************************

//1kHz time slots:
//================

//Case 0: I2C_0
void main_fsm_case_0(void)
{
	//TODO: Move to a function
	
	i2c_time_share++;
	i2c_time_share %= 4;

	#ifdef USE_I2C_0

	//Subdivided in 4 slots.
	switch(i2c_time_share)
	{
		//Case 0.0: 
		case 0:
		
			break;
		
		//Case 0.1: 
		case 1:
		
			break;
		
		//Case 0.2:
		case 2:
			//...
			break;
		
		//Case 0.3: 
		case 3:
			
			break;
		
		default:
			break;
	}
	
	#endif //USE_I2C_INT
}

//Case 1:
void main_fsm_case_1(void)
{

}

//Case 2: 
void main_fsm_case_2(void)
{

}

//Case 3: 
void main_fsm_case_3(void)
{
	//ToDo do this when new data, not randomly
	strain_filter();
	strain_to_ezi2c();
}

//Case 4: User Interface
void main_fsm_case_4(void)
{
	//UI RGB LED:	
	rgbLedRefreshFade();
	
	rgb_led_ui(eL0, eL1, eL2, new_cmd_led);		//ToDo add more error codes
	if(new_cmd_led)
	{
		new_cmd_led = 0;
	}
}

//Case 5: 
void main_fsm_case_5(void)
{

}

//Case 6: 
void main_fsm_case_6(void)
{

}

//Case 7:
void main_fsm_case_7(void)
{
	//...
}

//Case 8:
void main_fsm_case_8(void)
{

}

//Case 9: User functions & 1s timebase	
void main_fsm_case_9(void)
{
	//1s timebase:
	if(timebase_1s())
	{
		//Insert code that needs to run every second here
		//...
	} 
}

//10kHz time slot:
//================

void main_fsm_10kHz(void)
{    
	//USB Byte Input
	#ifdef USE_USB

	get_usb_data();
	
	if(data_ready_usb)
	{
		data_ready_usb = 0;
		//Got new data in, try to decode
		cmd_ready_usb = unpack_payload_usb();
		
		//eL1 = 1;
	}

	#endif	//USE_USB
	
	//FlexSEA Network Communication
	#ifdef USE_COMM
	
	//Valid communication from USB?
	if(cmd_ready_usb != 0)
	{
		cmd_ready_usb = 0;
		
		//Cheap trick to get first line	//ToDo: support more than 1
		for(i = 0; i < PAYLOAD_BUF_LEN; i++)
		{
			tmp_rx_command_usb[i] = rx_command_usb[0][i];
		}
		
		//payload_parse_str() calls the functions (if valid)
		result = payload_parse_str(tmp_rx_command_usb);
		
		//LED:
		if(result == PARSE_SUCCESSFUL)
		{
			//Green LED only if the ID matched and the command was known
			new_cmd_led = 1;
		}
	}
	
	#endif	//USE_COMM
	
	//RGB LED:
	rgbLedRefresh();
}

//Asynchronous time slots:
//========================

void main_fsm_asynchronous(void)
{

}

//****************************************************************************
// Private Function(s)
//****************************************************************************
