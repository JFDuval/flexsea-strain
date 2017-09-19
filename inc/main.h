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

#ifndef MAIN_H_
#define MAIN_H_

//****************************************************************************
// Include(s)
//****************************************************************************

#include <project.h>
#include <math.h>
#include "stdio.h"
//#include "main_fsm.h"
//#include "misc.h"
//#include "ui.h"
//#include "rgb_led.h"
//#include "usb.h"
//#include "strain.h"
//#include "peripherals.h"
//#include "flexsea_board.h"
//#include "../flexsea-system/inc/flexsea_system.h"
//#include "../flexsea-comm/inc/flexsea.h"

//****************************************************************************
// Shared variable(s)
//****************************************************************************
	
extern int steps;

//****************************************************************************
// Prototype(s):
//****************************************************************************

int main(void);

//****************************************************************************
// Definition(s):
//****************************************************************************
	
//Enable/Disable sub-modules:
#define USE_USB
#define USE_COMM			//Requires USE_RS485 and/or USE_USB
#define USE_I2C_0			//3V3/5V, External
#define USE_I2C_1			//5V, Strain gauge pots.
#define USE_STRAIN			//Requires USE_I2C_1

#endif // MAIN_H_
