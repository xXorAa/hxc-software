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

/*
 * source: compendium "IKBD Scan Codes - F3.3"
 */
keyboard_funct_mapper keysmap[]=
{
	{FCT_ESC,1},
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
	{FCT_SORT,0x1f},
	{FCT_NO_FUNCTION,0x00}	// must be last
};
