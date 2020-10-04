/* 
 *  Arnold emulator (c) Copyright, Kevin Thacker 1995-2001
 *  
 *  This file is part of the Arnold emulator source code distribution.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#pragma once
#ifndef __Z8536_HEADER_INCLUDED__
#define __Z8536_HEADER_INCLUDED__

/*
enum
{
	Z8536_RESET_STATE,
	Z8536_STATE_0,
	Z8536_STATE_1
} Z8536_STATES;
*/

#define Z8536_RESET_STATE 0
#define Z8536_STATE_0 1
#define Z8536_STATE_1 2

typedef struct
{
	unsigned char State;
	unsigned char PointerRegister;
	unsigned char Registers[64];
	/* outputs as seen by hardware connected to z8536 */
	unsigned char final_output[3];

	/* previous input data provided by hardware connected to z8536 */
	unsigned char previous_inputs[3];
	/* input data provided by hardware connected to z8536 */
	unsigned char inputs[3];

	/* the counters can be linked. This allows them to be linked together */
	/* inputs to each counter */
	unsigned char counter_inputs[3];
	/* previous inputs to counter */
	unsigned char counter_previous_inputs[3];
	/* counter outputs */
	unsigned char counter_outputs[3];

} Z8536;	

#define Z8536_REGISTER_MASTER_INTERRUPT_CONTROL 0
#define Z8536_REGISTER_CONFIGURATION_CONTROL 1
#define Z8536_REGISTER_PORT_A_INTERRUPT_VECTOR 2
#define Z8536_REGISTER_PORT_B_INTERRUPT_VECTOR 3
#define Z8536_REGISTER_COUNTER_TIMER_INTERRUPT_VECTOR 4
#define Z8536_REGISTER_PORT_C_DATA_PATH_POLARITY 5
#define Z8536_REGISTER_PORT_C_DATA_DIRECTION 6
#define Z8536_REGISTER_PORT_C_SPECIAL_IO_CONTROL 7
#define Z8536_REGISTER_PORT_A_COMMAND_AND_STATUS 8
#define Z8536_REGISTER_PORT_B_COMMAND_AND_STATUS 9
#define Z8536_REGISTER_COUNTER_TIMER_1_COMMAND_AND_STATUS 10
#define Z8536_REGISTER_COUNTER_TIMER_2_COMMAND_AND_STATUS 11
#define Z8536_REGISTER_COUNTER_TIMER_3_COMMAND_AND_STATUS 12
#define Z8536_REGISTER_PORT_A_DATA 13
#define Z8536_REGISTER_PORT_B_DATA 14
#define Z8536_REGISTER_PORT_C_DATA 15
#define Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_MSB 16
#define Z8536_REGISTER_COUNTER_TIMER_1_CURRENT_COUNT_LSB 17
#define Z8536_REGISTER_COUNTER_TIMER_2_CURRENT_COUNT_MSB 18
#define Z8536_REGISTER_COUNTER_TIMER_2_CURRENT_COUNT_LSB 19
#define Z8536_REGISTER_COUNTER_TIMER_3_CURRENT_COUNT_MSB 20
#define Z8536_REGISTER_COUNTER_TIMER_3_CURRENT_COUNT_LSB 21
#define Z8536_REGISTER_COUNTER_TIMER_1_TIME_CONSTANT_MSB 22
#define Z8536_REGISTER_COUNTER_TIMER_1_TIME_CONSTANT_LSB 23
#define Z8536_REGISTER_COUNTER_TIMER_2_TIME_CONSTANT_MSB 24
#define Z8536_REGISTER_COUNTER_TIMER_2_TIME_CONSTANT_LSB 25
#define Z8536_REGISTER_COUNTER_TIMER_3_TIME_CONSTANT_MSB 26
#define Z8536_REGISTER_COUNTER_TIMER_3_TIME_CONSTANT_LSB 27
#define Z8536_REGISTER_COUNTER_TIMER_1_MODE_SPECIFICATION 28
#define Z8536_REGISTER_COUNTER_TIMER_2_MODE_SPECIFICATION 29
#define Z8536_REGISTER_COUNTER_TIMER_3_MODE_SPECIFICATION 30
#define Z8536_REGISTER_CURRENT_VECTOR 30
#define Z8536_REGISTER_PORT_A_MODE_SPECIFICATION 31
#define Z8536_REGISTER_PORT_A_HANDSHAKE_SPECIFICATION 32
#define Z8536_REGISTER_PORT_A_DATA_PATH_POLARITY 33
#define Z8536_REGISTER_PORT_A_DATA_DIRECTION 34
#define Z8536_REGISTER_PORT_A_SPECIAL_IO_CONTROL 35
#define Z8536_REGISTER_PORT_A_PATTERN_POLARITY 36
#define Z8536_REGISTER_PORT_A_PATTERN_TRANSITION 37
#define Z8536_REGISTER_PORT_A_PATTERN_MASK 38
#define Z8536_REGISTER_PORT_B_MODE_SPECIFICATION 39
#define Z8536_REGISTER_PORT_B_HANDSHAKE_SPECIFICATION 40
#define Z8536_REGISTER_PORT_B_DATA_PATH_POLARITY 41
#define Z8536_REGISTER_PORT_B_DATA_DIRECTION 42
#define Z8536_REGISTER_PORT_B_SPECIAL_IO_CONTROL 43
#define Z8536_REGISTER_PORT_B_PATTERN_POLARITY 44
#define Z8536_REGISTER_PORT_B_PATTERN_TRANSITION 45
#define Z8536_REGISTER_PORT_B_PATTERN_MASK 46


void Z8536_WriteData(unsigned char Address, unsigned char Data);
unsigned char Z8536_ReadData(unsigned char Address);
void	Z8536_Update(void);

#endif

