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
	[This file] peripherals: code for the general peripheral modules
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-26 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//Note:  many init_x() functions are included in other files to keep a logical
// organization. Place them here if they are not specific to one file.

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "peripherals.h"
#include "usb.h"
#include "ui.h"
#include "strain.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8 ezI2Cbuf[EZI2C_BUF_SIZE];

//****************************************************************************
// Function(s)
//****************************************************************************

//Initialize and enables all the peripherals
void init_peripherals(void)
{
	//Timebases:
	init_tb_timers();
	
	//Enable Global Interrupts
    CyGlobalIntEnable; 
	
	//I2C0 - 3V3/5V, External world
	#ifdef USE_I2C_0	
		
		init_i2c_0();
	
	#endif	//USE_I2C_0
	
	//I2C1 - 5V, strain gauge pots
	#ifdef USE_I2C_1	
		
		//I2C1 peripheral:
		init_i2c_1();
		
		//Strain amplifier:
		#ifdef USE_STRAIN
			
		init_strain();
		
		#endif	//USE_STRAIN
		
	#endif	//USE_I2C_1	
	
	//Special color when waiting for USB (Yellow):
	set_led_rgb(1, 1, 0);
	
	//USB CDC
	#ifdef USE_USB	
	init_usb();
	#endif	//USE_USB
	
	//And now RGB OFF:
	set_led_rgb(0, 0, 0);
}

//Timebase timers init:
void init_tb_timers(void)
{
	//Timer 1: 1ms (LEDs, PID)
	Timer_1_Init();
	Timer_1_Start();
	isr_t1_Start();
}

//I2C0 - 3V3/5V, External.
void init_i2c_0(void)
{
	//I2C_0_EnableInt();
	//I2C_0_Start();	
	
	//We use EZI2C - Shared memory
	EZI2C_0_SetBuffer1(EZI2C_BUF_SIZE, EZI2C_WBUF_SIZE, ezI2Cbuf);
	EZI2C_0_Start();
}

//I2C1 - 5V, Strain gauge pots
void init_i2c_1(void)
{
	I2C_1_EnableInt();
	I2C_1_Start();
}
