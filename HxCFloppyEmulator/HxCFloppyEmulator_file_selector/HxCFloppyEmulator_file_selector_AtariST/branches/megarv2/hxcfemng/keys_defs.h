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
	{FCT_SELECT_FILE_DRIVEA,82},					// INSERT
	{FCT_SELECT_FILE_DRIVEA,28},					// RETURN
	{FCT_SELECT_FILE_DRIVEB,71},					// CLR HOME
	{FCT_SELECT_FILE_DRIVEA_AND_NEXTSLOT,43},		// pipe / backslash ?
	{FCT_NEXTSLOT,97},								// UNDO
	{FCT_SAVE,67},									// F9
	{FCT_REBOOT,66},								// F8
	{FCT_CLEARSLOT,14},								// BACKSPACE
	{FCT_CLEARSLOT_AND_NEXTSLOT,83},				// DELETE
	{FCT_HELP,98},									// HELP
	{FCT_SAVEREBOOT,68},							// F10
	{FCT_SELECTSAVEREBOOT,65},						// F7
	{FCT_OK,57},									// SPACE
	{FCT_SHOWSLOTS,15},								// TAB
	{FCT_FILTER,59},								// F1
	{FCT_TOP,53},									// CAPS LOCK (not working)
	{FCT_CHGCOLOR,60},								// F2
	{FCT_EMUCFG,61},								// F3
	{FCT_SORT,0x1f},								// s
	{FCT_NO_FUNCTION,0x00}	// must be last
};
