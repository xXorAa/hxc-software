#ifndef __HXCFEDA_H__
#define __HXCFEDA_H__
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

STRUCT_PACK_BEGIN
typedef struct STRUCT_PACK direct_access_status_sector_
{
	char DAHEADERSIGNATURE[8];
	char FIRMWAREVERSION[12];
	unsigned long lba_base;
	unsigned char cmd_cnt;
	unsigned char read_cnt;
	unsigned char write_cnt;
	unsigned char last_cmd_status;
} STRUCT_PACK_PREEND direct_access_status_sector;
STRUCT_PACK_END


STRUCT_PACK_BEGIN
typedef struct STRUCT_PACK direct_access_cmd_sector_
{
	char DAHEADERSIGNATURE[8];
	unsigned char cmd_code;
	unsigned char parameter_0;
	unsigned char parameter_1;
	unsigned char parameter_2;
	unsigned char parameter_3;
	unsigned char parameter_4;
	unsigned char parameter_5;
	unsigned char parameter_6;
	unsigned char parameter_7;
	unsigned char cmd_checksum;
} STRUCT_PACK_PREEND direct_access_cmd_sector;
STRUCT_PACK_END

#define LFN_MAX_SIZE 128

STRUCT_PACK_BEGIN
typedef struct STRUCT_PACK DirectoryEntry_ {
	unsigned char name[12];
	unsigned char attributes;
/*	unsigned long firstCluster;*/
	unsigned char firstCluster_b1;
	unsigned char firstCluster_b2;
	unsigned char firstCluster_b3;
	unsigned char firstCluster_b4;
/*	unsigned long size;*/
	unsigned char size_b1;
	unsigned char size_b2;
	unsigned char size_b3;
	unsigned char size_b4;
	unsigned char longName[LFN_MAX_SIZE];	/* boolean */
} STRUCT_PACK_PREEND DirectoryEntry;
STRUCT_PACK_END
#endif
