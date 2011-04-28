/*
//
// Copyright (C) 2009, 2010, 2011 Jean-François DEL NERO
//
// This file is part of the HxCFloppyEmulator file selector.
//
// HxCFloppyEmulator file selector may be used and distributed without restriction
// provided that this copyright statement is not removed from the file and that any
// derivative work contains the original copyright notice and the associated
// disclaimer.
//
// HxCFloppyEmulator file selector is free software; you can redistribute it
// and/or modify  it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// HxCFloppyEmulator file selector is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with HxCFloppyEmulator file selector; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
*/

typedef struct keyboard_funct_mapper_
{
	unsigned char function_code;
	unsigned char keyboard_code;
}keyboard_funct_mapper;

keyboard_funct_mapper keysmap[]=
{
	{FCT_UP_KEY,72},
	{FCT_DOWN_KEY,80},
	{FCT_LEFT_KEY,75},
	{FCT_RIGHT_KEY,77},
	{FCT_SELECT_FILE_DRIVEA,82},
	{FCT_SELECT_FILE_DRIVEA,28},	
	{FCT_SELECT_FILE_DRIVEB,71},
	{FCT_SELECT_FILE_DRIVEA_AND_NEXTSLOT,43},
	{FCT_NEXTSLOT,97},
	{FCT_SAVE,67},
	{FCT_REBOOT,66},
	{FCT_CLEARSLOT,14},
	{FCT_CLEARSLOT_AND_NEXTSLOT,83},
	{FCT_HELP,98},
	{FCT_SAVEREBOOT,68},
	{FCT_SELECTSAVEREBOOT,65},
	{FCT_OK,57},
	{FCT_SHOWSLOTS,15},
	{FCT_SEARCH,59},
	{FCT_TOP,53},
	{FCT_CHGCOLOR,60},
	{FCT_EMUCFG,61},
	{FCT_NO_FUNCTION,0x00}
};

typedef struct keyboard_scancode_mapper_
{
	unsigned char char_code;
	unsigned char keyboard_code;
}keyboard_scancode_mapper;

keyboard_funct_mapper char_keysmap[]=
{
	{'a',0x10},
	{'b',0x30},
	{'c',0x2E},
	{'d',0x20},
	{'e',0x12},
	{'f',0x21},
	{'g',0x22},
	{'h',0x23},
	{'i',0x17},
	{'j',0x24},
	{'k',0x25},
	{'l',0x26},
	{'m',0x27},
	{'n',0x31},
	{'o',0x18},
	{'p',0x19},
	{'q',0x1E},
	{'r',0x13},
	{'s',0x1F},
	{'t',0x14},
	{'u',0x16},
	{'v',0x2F},
	{'w',0x2C},
	{'x',0x2D},
	{'y',0x15},
	{'z',0x11},

	{'0',0x0B},
	{'1',0x02},
	{'2',0x03},
	{'3',0x04},
	{'4',0x05},
	{'5',0x06},
	{'6',0x07},
	{'7',0x08},
	{'8',0x09},
	{'9',0x0A},

	//{'0',0x00},
	{'1',0x6D},
	{'2',0x6E},
	{'3',0x6F},
	{'4',0x6A},
	{'5',0x6B},
	{'6',0x6C},
	{'7',0x67},
	{'8',0x68},
	{'9',0x69},
	{' ',0x39},
	{'.',0x71},
	{'\n',0x1C},
	
	{0xFF,0x00},
};