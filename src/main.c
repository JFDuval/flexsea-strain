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
				//Case 0: I2C_0
				case 0:
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
												
					break;
				
				//Case 1: I2C_1
				case 1:
										
					break;
				
				//Case 2:	
				case 2:
					break;
				
				//Case 3: 
				case 3:		
					
					//ToDo do this when new data, not randomly
					strain_filter();
					strain_to_ezi2c();
					
					break;
				
				//Case 4: User Interface	
				case 4:
					
					//Alive LED
					//alive_led();	//Done in ISR
					
					//UI RGB LED:
					// ToDo...
					
					break;
				
				//Case 5:
				case 5:
							
					break;
				
				//Case 6: 
				case 6:
										
					break;
				
				case 7:
					
					break;
				
				//Case 8:
				case 8:
					
					break;
				
				//Case 9: 	
				case 9:
								
					//1s timebase:
					if(timebase_1s())
					{
						//Insert code that needs to run every second here
						//...
					}
					
					break;
				
				default:
					break;
			}
			
			//The code below is executed every 100us, after the previous slot. 
			//Keep it short!
			
			//BEGIN - 10kHz Refresh
				
			//USB Byte Input
			#ifdef USE_USB			
		
			get_usb_data();
			
			if(data_ready_usb)
			{
				data_ready_usb = 0;
				//Got new data in, try to decode
				cmd_ready_usb = unpack_payload_usb();
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
			
			//END - 10kHz Refresh
		}
		else
		{
			//Asynchronous code goes here.
			//...
		}
	}
}
