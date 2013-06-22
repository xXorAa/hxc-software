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
// File : daccess.c
// Contains: 
//
// Written by:	DEL NERO Jean Francois
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

#include	<stdio.h>
#include	<io.h>
#include	<stdlib.h>
#include	<string.h>

#include	<bios.h>
#include	<dos.h>

#include	"daccess.h"
#include	"testhxcfe.h"

#include	"print_output.h"
#include	"floppyio.h"


int setlbabase(unsigned long lba)
{
	unsigned char  sector[512];
	unsigned short status;

	direct_access_cmd_sector    * dacs;
	direct_access_status_sector * dass;

	dass=(direct_access_status_sector *)sector;
	dacs=(direct_access_cmd_sector *)sector;

	memset(&sector,0,512);

	sprintf(dacs->DAHEADERSIGNATURE,"HxCFEDA");
	dacs->cmd_code=1;
	dacs->parameter_0=(lba)&0xFF;
	dacs->parameter_1=(lba>>8)&0xFF;
	dacs->parameter_2=(lba>>16)&0xFF;
	dacs->parameter_3=(lba>>24)&0xFF;

	status=chs_biosdisk(_DISK_WRITE,0,	0,	255,0,	1,	&sector);
	if(!status)
	{
		status=chs_biosdisk(_DISK_READ,0,	0,	255,0,	1,	&sector);
		if(!status)	hxc_printf(0,"new lba adr : %d\n",dass->lba_base);
	}

	if(status)	hxc_printf(0,"write Access error !!! \n");

	hxc_printf(0,"\n");
	return	0;
}
