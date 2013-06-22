/*
//
// Copyright (C) 2006-2013 Jean-François DEL NERO
//
// This file is part of HxCFloppyEmulator.
//
// HxCFloppyEmulator may be used and distributed without restriction provided
// that this copyright statement is not removed from the file and that any
// derivative work contains the original copyright notice and the associated
// disclaimer.
//
// HxCFloppyEmulator is free software; you can redistribute it
// and/or modify  it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// HxCFloppyEmulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with HxCFloppyEmulator; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//
*/
///////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------//
//-------------------------------------------------------------------------------//
//-----------H----H--X----X-----CCCCC----22222----0000-----0000------11----------//
//----------H----H----X-X-----C--------------2---0----0---0----0--1--1-----------//
//---------HHHHHH-----X------C----------22222---0----0---0----0-----1------------//
//--------H----H----X--X----C----------2-------0----0---0----0-----1-------------//
//-------H----H---X-----X---CCCCC-----222222----0000-----0000----1111------------//
//-------------------------------------------------------------------------------//
//----------------------------------------------------- http://hxc2001.com ------//
///////////////////////////////////////////////////////////////////////////////////
// File : print_output.c
// Contains:
//
// Written by:  DEL NERO Jean Francois
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

#include	<stdio.h>
#include	<io.h>
#include	<stdlib.h>
#include	<string.h>
#include    <stdarg.h>

#include	<bios.h>
#include	<dos.h>
#include	<conio.h>
#include	<i86.h>

#include "print_output.h"

char stroutbuffer[512];
unsigned long indexptr;

unsigned com1_init;

void serialprint(char * str)
{
	unsigned int i;

	i=0;
	while(str[i])
	{
		_bios_serialcom(_COM_SEND, 0,str[i]);
		i++;
	}

	_bios_serialcom(_COM_SEND, 0,'\r');
	_bios_serialcom(_COM_SEND, 0,'\n');
}

int hxc_printf(int MSGTYPE,char * chaine, ...)
{
	va_list marker;
	va_start( marker, chaine );     

	vsprintf(stroutbuffer,chaine,marker);

	printf(stroutbuffer);
	printf("\n");
	//serialprint(stroutbuffer);    
	
	va_end( marker );

    return 0;
}

void print_hex(unsigned char * buffer, int size)
{
	int	c,i;

	c=0;
	for(i=0;i<size;i++)
	{
		hxc_printf(0,"%.2X ",buffer[i]);
		c++;
		if(!(c&0xF))
		{
			hxc_printf(0,"\n%.3X: ",indexptr);
			indexptr=indexptr+16;
		}
	}
}

void init_outputtxt()
{

	indexptr = 0;
	com1_init = _bios_serialcom(_COM_INIT, 0,_COM_CHR8 | _COM_NOPARITY | _COM_STOP1 | _COM_9600);

}
