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
	[This file] UI: User Interface related 
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-26 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

//****************************************************************************
// Include(s)
//****************************************************************************

#include "main.h"
#include "ui.h"

//****************************************************************************
// Variable(s)
//****************************************************************************

//MinM RGB:
uint8 minm_rgb_color = 0, last_minm_rgb_color = 0;

//****************************************************************************
// Private Function Prototype(s):
//****************************************************************************


//****************************************************************************
// Public Function(s)
//****************************************************************************

void alive_led(void)
{
	static uint8 toggle0 = 1, count0 = 0;
	
	//Fast blinking - Alive LED
	count0++;
	if(count0 >= ALIVE_HALF_T)
	{
		count0 = 0;
		toggle0^=1;
		LED_G_Write(toggle0);
	}
}

//Power On Delay with LEDs
void power_on(void)
{
	set_led_rgb(1,0,0);
	CyDelay(250);
	set_led_rgb(0,1,0);
	CyDelay(250);
	set_led_rgb(0,0,1);
	CyDelay(250);
	set_led_rgb(0,0,0);
	CyDelay(250);
}

void set_led_rgb(unsigned char r, unsigned char g, unsigned char b)
{
	//No fading, we use 1 or 0 for now. Flipping the sign so x = 1 means ON

	LED_R_Write((r & 0x01)^1);
	LED_G_Write((g & 0x01)^1);
	LED_B_Write((b & 0x01)^1);
}

//Call this function every ms in main while()
void rgb_led_ui(uint8_t err_l0, uint8_t err_l1, uint8_t err_l2, uint8_t new_comm)
{
	static uint32_t cnt_comm = UI_COMM_TIMEOUT, cnt_err_l0 = 0, cnt_err_l1 = 0, cnt_flash = 0;
	static uint8_t latch_err_l2 = 0, flash_red = 0, comm_blue = 0;
	uint8_t r = 0, g = 0, b = 0;
	int8 rgbStatus = 0;

	//Set variable for the flashing red light:
	if(cnt_flash < UI_RED_FLASH_ON)
	{
		flash_red = 1;
	}
	else
	{
		flash_red = 0;
	}

	cnt_flash++;
	if(cnt_flash >= UI_RED_FLASH_PERIOD)
	{
		cnt_flash = 0;
	}

	//New communication green/blue:
	if(new_comm)
	{
		//Received a new valid packet, resets the counter
		cnt_comm = UI_COMM_TIMEOUT;
	}

	if(cnt_comm > 0)
		cnt_comm--;

	if(!cnt_comm)
	{
		comm_blue = 1;
	}
	else
	{
		comm_blue = 0;
	}

	//From the highest priority to the lowest:
	//=======================================
	
	rgbStatus = -1;

	if((err_l2 == 1) || (latch_err_l2 == 1))
	{
		//Major error => flashing red

		latch_err_l2 = 1;	//Latching it, will require a reset to be normal again
		r = flash_red;
		g = 0;
		b = 0;
	}
	else
	{
		if(err_l1 == 1)
		{
			//Intermediate error => Steady Red
			r = 1;
			g = 0;
			b = 0;
		}
		else
		{
			if(err_l0 == 1)
			{
				//Software error => Steady Yellow
				r = 1;
				g = 1;
				b = 0;
			}
			else
			{
				if(comm_blue == 1)
				{
					//Communication timeout, blue
					r = 0;
					g = 0;
					b = 1;
					rgbStatus = 0;
				}
				else
				{
					//Normal, green
					r = 0;
					g = 1;
					b = 0;
					rgbStatus = 1;
				}
			}
		}
	}

	//Assign the color to the RGB LED:
	
	//Use the Fading code. 
	//TODO this isn't the cleanest integration...
	if(rgbStatus == 0)
	{		
		rgbLedSet(0,0,rgbLedGetFade());
	}
	else if(rgbStatus == 1)
	{
		rgbLedSet(0,rgbLedGetFade(),0);
	}
	else
	{
		//Legacy code, used for all the errors
		rgbLedSet(255*r, 255*b, 255*g);
	}
}

//****************************************************************************
// Private Function(s)
//****************************************************************************
