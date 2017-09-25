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
	[This file] misc: when it doesn't belong in any another file, it ends up 
	here...
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-26 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//Note: It's a sign that code is misplaced when this file gets long.

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "misc.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8 i2c_last_request = 0;

//Timers:
volatile uint8 t1_100us_flag = 0;
volatile uint8 t1_time_share = 0, t1_new_value = 0;

//ADC:
volatile uint8 adc_delsig_flag = 0, adc_delsig_lastCh = 0;

//USB:
volatile uint8 data_ready_usb = 0;

//LED:
uint16 led_period = LED_PERIOD_NORM;
uint8 new_cmd_led = 0;

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************


//****************************************************************************
// Public Function(s)
//****************************************************************************

//Call this function in the 1kHz FSM. It will return 1 every second.
uint8 timebase_1s(void)
{
	static uint16 time = 0;
	
	time++;
	if(time >= 999)
	{
		time = 0;
		return 1;
	}
	
	return 0;
}

//****************************************************************************
// Private Function(s)
//****************************************************************************

//****************************************************************************
// Test Function(s) - Use with care!
//****************************************************************************
