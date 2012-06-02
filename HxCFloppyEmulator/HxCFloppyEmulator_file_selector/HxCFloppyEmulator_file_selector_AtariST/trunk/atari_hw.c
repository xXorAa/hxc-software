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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mint/osbind.h>
#include <time.h>
#include <vt52.h>

#include "keysfunc_defs.h"
#include "keys_defs.h"
#include "atari_hw.h"
#include "atari_regs.h"

static unsigned char floppydrive;
static unsigned char datacache[512*9];
static unsigned char valid_cache;

#define CONTERM *((unsigned char *) 0x484)

static void keysound_off()
{
	CONTERM &= 0xFE;
	CONTERM &= 0xFB;
}

void initsound()
{
	Supexec(keysound_off);
}
void jumptotrack(unsigned char t)
{
	unsigned short i,j;
	unsigned char data[512];

	Floprd( &data, 0, floppydrive, 1, t, 0, 1 );

};

unsigned char writesector(unsigned char sectornum,unsigned char * data)
{
	int ret,retry;
	valid_cache =0;
	retry=3;

	ret=1;
	while(retry && ret)
	{	
		ret=Flopwr( data, 0, floppydrive, sectornum, 255, 0, 1 );
	retry--;
	}

	if(!ret)
		return 1;
	else
		return 0;
}

unsigned char readsector(unsigned char sectornum,unsigned char * data,unsigned char invalidate_cache)
{
	int ret,retry;

	retry=3;
	ret=0;
	if(!valid_cache || invalidate_cache)
	{
		if(sectornum<10)
		{
			ret=1;
			while(retry && ret)
			{
				ret=Floprd( datacache, 0, floppydrive, 0, 255, 0, 9 );
				retry--;
			}
			memcpy((void*)data,&datacache[sectornum*512],512);
			valid_cache=0xFF;
		}
	}
	else
	{
		memcpy((void*)data,&datacache[sectornum*512],512);
	}
	if(!ret)
		return 1;
	else
		return 0;

}


void init_atari_fdc(unsigned char drive)
{
	unsigned short i,ret;

	valid_cache=0;
	floppydrive=drive;
	Floprate( floppydrive, 2);
	ret=Floprd( &datacache, 0, floppydrive, 0, 255, 0, 1 );
}

unsigned char Keyboard()
{
	return 0;
}

int kbhit()
{
}

void flush_char()
{
}

unsigned char get_char()
{
	unsigned char buffer;
	unsigned char key,i,c;
	unsigned char function_code,key_code;

	function_code=FCT_NO_FUNCTION;	
	do
	{

		key=Cconin()>>16;

//		hxc_printf(0,0,0,"%.8X",key);

		i=0;
		do
		{
			function_code=char_keysmap[i].function_code;
			key_code=char_keysmap[i].keyboard_code;
			i++;
		}while((key_code!=key) && (function_code!=FCT_NO_FUNCTION) );

	}while(function_code==FCT_NO_FUNCTION);

	return function_code;		
}


unsigned char wait_function_key()
{
	unsigned char key,i,c;
	unsigned char function_code,key_code;

	function_code=FCT_NO_FUNCTION;
	do
	{
		
		key=Cconin()>>16;
		//		hxc_printf(0,0,0,"%.8X",key);

		i=0;
		do
		{
			function_code=keysmap[i].function_code;
			key_code=keysmap[i].keyboard_code;
			i++;
		}while((key_code!=key) && (function_code!=FCT_NO_FUNCTION) );

	}while(function_code==FCT_NO_FUNCTION);

	return function_code;
}



unsigned short get_vid_mode()
{
	return 0;
}

void su_reboot()
{
	asm("move.l #4,A6");
	asm("move.l (A6),A0");
	asm("move.l A0,-(SP)");
	asm("rts");
}

void reboot()
{
	Supexec(su_reboot);
}

