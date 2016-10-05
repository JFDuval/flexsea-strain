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
	[Lead developper] Jean-Francois Duval, jfduval at dephy dot com.
	[Origin] Based on Jean-Francois Duval's work at the MIT Media Lab 
	Biomechatronics research group <http://biomech.media.mit.edu/>
	[Contributors]
*****************************************************************************
	[This file] RGB LED: Onboard LED Driver that supports fading
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-10-05 | jfduval | Copied Execute & Manage
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "rgb_led.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

uint8 rgbFade = 0;

//RGB LED:
uint8 rgbPeriodR = 0, rgbPeriodG = 0, rgbPeriodB = 0;

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************


//****************************************************************************
// Public Function(s)
//****************************************************************************

//Use this to set a new value
void rgbLedSet(uint8 r, uint8 g, uint8 b)
{
	rgbPeriodR = r;
	rgbPeriodG = g;
	rgbPeriodB = b;
}

//Timer-based RGB driver - w/ fading.
//Call this function at 10kHz
void rgbLedRefresh(void)
{
	static uint8 cnt = 0;
	static uint8 rON = 0, gON = 0, bON = 0;
	
	//New cycle?
	if(!cnt)
	{
		//All ON
		LED_R_Write(0);
		LED_G_Write(0);
		LED_B_Write(0);
		rON = 1;
		gON = 1;
		bON = 1;
	}
	
	//Ready to turn OFF?
	
	if(rON && cnt >= rgbPeriodR)
	{
		LED_R_Write(1);
		rON = 0;
	}
	
	if(gON && cnt >= rgbPeriodG)
	{
		LED_G_Write(1);
		gON = 0;
	}
	
	if(bON && cnt >= rgbPeriodB)
	{
		LED_B_Write(1);
		bON = 0;
	}
	
	//Increment counter. It will eventually roll over.
	cnt += 2;
}

//Accessor
uint8 rgbLedGetFade(void)
{
	return rgbFade;
}

//Call this function every ms. It will update the rgbFade variable.
void rgbLedRefreshFade(void)
{
	static uint16 fade = 0, val = 0;

	val++;
	val %= FADE_PERIOD_MS;
	
	if(val > FADE_MIDPOINT-2)
		fade = FADE_PERIOD_MS - val;
	else
		fade = val;
	
	rgbFade = (uint8) (fade>>1 & 0xFF);
}

//Test code
void rgbLedRefresh_testcode_blocking(void)
{
	uint8 div = 0;
	
	while(1)
	{		
		rgbLedSet(0, rgbFade, 0);
		CyDelayUs(100);
		
		div++;
		div %= 10;
		if(!div)
		{
			//1ms
			rgbLedRefreshFade();
		}
	}
}

//****************************************************************************
// Private Function(s)
//****************************************************************************
