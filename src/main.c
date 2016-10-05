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
	[This file] main: FlexSEA-Strain (6-channel amplifier)
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-26 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "cyapicallbacks.h"

#if defined (__GNUC__)
    /* Add an explicit reference to the floating point printf library */
    /* to allow the usage of floating point conversion specifiers. */
    /* This is not linked in by default with the newlib-nano library. */
    asm (".global _printf_float");
#endif

//****************************************************************************
// Variable(s)
//****************************************************************************

//Misc. variables, used for debugging only
uint8 last_byte = 0;
uint8_t tmp_rx_command_usb[PAYLOAD_BUF_LEN];

//****************************************************************************
// Function(s)
//****************************************************************************

int main(void)
{
	//Local variables:
	uint8 i = 0;
	unsigned char result = 0;
	uint8 cmd_ready_usb = 0;
	static uint8 new_cmd_led = 0;
	uint16 safety_delay = 0;
	uint8 i2c_time_share = 0;
	//uint8 toggle_led = 0;
	
	//Power on delay with LEDs
	power_on();
	
	//Prepare FlexSEA stack:
	init_flexsea_payload_ptr();

	//Initialize all the peripherals
	init_peripherals();
	
	//Start with an empty buffer
	flexsea_clear_slave_read_buffer();
	
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
	//Blocking Test code - enable one and only one for special 
	//debugging. Normal code WILL NOT EXECUTE when this is enabled!
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//strain_test_blocking();
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//Non-Blocking Test code
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
	//...
	//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=	

	//Main loop
	while(1)
	{
		if(t1_new_value == 1)
		{
			//If the time share slot changed we run the timing FSM. Refer to
			//timing.xlsx for more details. 't1_new_value' updates at 10kHz,
			//each slot at 1kHz.			
            
            t1_new_value = 0;            
			
			//Timing FSM:
			switch(t1_time_share)
			{
				case 0:                    
					main_fsm_case_0();	
					break;				
				case 1:       
					main_fsm_case_1();	
					break;				
				case 2:
					main_fsm_case_2();
					break;
				case 3:				
					main_fsm_case_3();					
					break;					
				case 4:
					main_fsm_case_4();			
					break;				
				case 5:
					main_fsm_case_5();			
					break;					
				case 6:
					main_fsm_case_6();						
					break;				
				case 7:					
					main_fsm_case_7();	
					break;				
				case 8:
					main_fsm_case_8();					
					break;
				case 9:
					main_fsm_case_9();	
					break;				
				default:
					break;
			}
			
			//Increment value, limits to 0-9
        	t1_time_share++;
	        t1_time_share %= 10;
			
			//The code below is executed every 100us, after the previous slot. 
			//Keep it short! (<10us if possible)
			main_fsm_10kHz();         
		}
		else
		{
			//Asynchronous code goes here.
			main_fsm_asynchronous();			
		}
	}
}
