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
	[This file] flexsea_board: configuration and functions for this 
	particular board
*****************************************************************************
	[Change log] (Convention: YYYY-MM-DD | author | comment)
	* 2016-09-26 | jfduval | Initial GPL-3.0 release
	*
****************************************************************************/

#ifndef INC_FLEXSEA_BOARD_H
#define INC_FLEXSEA_BOARD_H

//****************************************************************************
// Include(s)
//****************************************************************************

#include <stdint.h>
#include <flexsea.h>
#include <flexsea_comm.h>

//****************************************************************************
// Prototype(s):
//****************************************************************************

void flexsea_send_serial_slave(PacketWrapper* p);
void flexsea_send_serial_master(PacketWrapper* p);
void flexsea_receive_from_master(void);
uint8_t getBoardID(void);
void setBoardID(uint8_t bid);
uint8_t getBoardUpID(void);
uint8_t getBoardSubID(uint8_t sub, uint8_t idx);
uint8_t getSlaveCnt(uint8_t sub);

//****************************************************************************
// Definition(s):
//****************************************************************************

//<FlexSEA User>
//==============

//How many slave busses?
#define COMM_SLAVE_BUS				2

//How many slaves on this bus?
#define SLAVE_BUS_1_CNT				0
#define SLAVE_BUS_2_CNT				0
//Note: only Manage can have a value different than 0 or 1

//How many possible masters?
#define COMM_MASTERS				3

//Slave Read Buffer Size:
#define SLAVE_READ_BUFFER_LEN		32	//ToDo TBD

//How many slaves on this bus?
#define SLAVE_BUS_1_CNT				0
#define SLAVE_BUS_2_CNT				0
//Note: only Manage can have a value different than 0 or 1

//Slave Read Buffer Size:
#define SLAVE_READ_BUFFER_LEN		32	//ToDo TBD

//Enabled the required FlexSEA Buffers for this board:
#define ENABLE_FLEXSEA_BUF_1	//RS-485
#define ENABLE_FLEXSEA_BUF_2	//USB
//#define ENABLE_FLEXSEA_BUF_3	//In the future: radio-module
//#define ENABLE_FLEXSEA_BUF_4	//In the future: battery board?

//Overload buffer & function names (for user convenience):

#define comm_str_485_1				comm_str_1
#define unpack_payload_485_1		unpack_payload_1
#define rx_command_485_1			rx_command_1
#define update_rx_buf_byte_485_1	update_rx_buf_byte_1
#define update_rx_buf_array_485_1	update_rx_buf_array_1

#define comm_str_usb				comm_str_2
#define unpack_payload_usb			unpack_payload_2
#define rx_command_usb				rx_command_2
//#define update_rx_buf_byte_usb		update_rx_buf_byte_2
//#define update_rx_buf_array_usb		update_rx_buf_array_2
#define update_rx_buf_usb(x, y)		circ_buff_write(commPeriph[PORT_USB].rx.circularBuff, (x), (y))

//===============
//</FlexSEA User>

//****************************************************************************
// Structure(s):
//****************************************************************************

//****************************************************************************
// Shared variable(s)
//****************************************************************************

#endif	//INC_FLEXSEA_BOARD_H
