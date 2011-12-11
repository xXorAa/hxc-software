/*
//
// Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Jean-François DEL NERO
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

////////////////////////////////////////////
// Output Message  level
#define MSG_INFO_0 0
#define MSG_INFO_1 1
#define MSG_WARNING 2
#define MSG_ERROR 3
#define MSG_DEBUG 4

////////////////////////////////////////////
// Output functions 
typedef int (*HXCPRINTF_FUNCTION)(int MSGTYPE,char * string, ...);
typedef int (*DISPLAYTRACKPOS_FUNCTION)(unsigned int current,unsigned int total);

//////////////////////////////////////////////////////////
// Error codes

#define HXCFE_VALIDFILE			1
#define HXCFE_NOERROR			0 
#define HXCFE_ACCESSERROR		-1
#define HXCFE_BADFILE			-2
#define HXCFE_FILECORRUPTED		-3 
#define HXCFE_BADPARAMETER		-4
#define HXCFE_INTERNALERROR		-5
#define HXCFE_UNSUPPORTEDFILE	-6

////////////////////////////////////////////
// hxcfe types 
typedef void HXCFLOPPYEMULATOR;
typedef void FLOPPY;
typedef void FBuilder; 
typedef void SECTORSEARCH;
typedef void SECTORCONFIG;

////////////////////////////////////////////
// Init Function

HXCFLOPPYEMULATOR* hxcfe_init(void);
int hxcfe_getVersion(HXCFLOPPYEMULATOR* floppycontext,char * version,unsigned int * size1,char *copyright,unsigned int * size2);
int hxcfe_setOutputFunc(HXCFLOPPYEMULATOR* floppycontext,HXCPRINTF_FUNCTION hxc_printf);
void hxcfe_deinit(HXCFLOPPYEMULATOR* hxcfe);

////////////////////////////////////////////
// File image functions

int hxcfe_getContainerId(HXCFLOPPYEMULATOR* floppycontext,int index,char * id,char * desc);
int hxcfe_selectContainer(HXCFLOPPYEMULATOR* floppycontext,char * container);

FLOPPY * hxcfe_floppyLoad(HXCFLOPPYEMULATOR* floppycontext,char* imgname,int * err_ret);
int hxcfe_floppyUnload(HXCFLOPPYEMULATOR* floppycontext,FLOPPY * floppydisk);
int hxcfe_floppyExport(HXCFLOPPYEMULATOR* floppycontext,FLOPPY * newfloppy,char* imgname);

int hxcfe_getFloppySize(HXCFLOPPYEMULATOR* floppycontext,FLOPPY *fp,int * nbsector);

int hxcfe_getNumberOfTrack(HXCFLOPPYEMULATOR* floppycontext,FLOPPY *fp);
int hxcfe_getNumberOfSide(HXCFLOPPYEMULATOR* floppycontext,FLOPPY *fp);


////////////////////////////////////////////
// Custom Image/floppy generation functions

// Track / sector encoding type
#define IBMFORMAT_SD 0x1
#define IBMFORMAT_DD 0x2
#define ISOFORMAT_SD 0x3
#define ISOFORMAT_DD 0x4
#define ISOFORMAT_DD11S 0x5
#define AMIGAFORMAT_DD  0x6

FBuilder* hxcfe_initFloppy(HXCFLOPPYEMULATOR* floppycontext,int nb_of_track,int nb_of_side);

int	hxcfe_pushTrack (FBuilder*,unsigned int rpm,int number,int side,int type);
int hxcfe_setTrackInterleave(FBuilder*,int interleave);
int hxcfe_setTrackSkew(FBuilder*,int skew);

int hxcfe_setIndexPosition(FBuilder*,int position,int allowsector);
int hxcfe_setIndexLength(FBuilder*,int Length);

int hxcfe_setTrackBitrate(FBuilder*,int bitrate);

int hxcfe_addSector(FBuilder* fb,int sectornumber,int side,int track,unsigned char * buffer,int size);

int hxcfe_setSectorBitrate(FBuilder* fb,int bitrate);

int hxcfe_setSectorGap3(FBuilder* fb,unsigned char Gap3);
int hxcfe_setSectorSizeID(FBuilder* fb,unsigned char sectorsizeid);
int hxcfe_setSectorFill(FBuilder*,unsigned char fill);

int hxcfe_setSectorEncoding(FBuilder*,int encoding);

int hxcfe_setSectorDataCRC(FBuilder*,unsigned short crc);
int hxcfe_setSectorHeaderCRC(FBuilder*,unsigned short crc);

int hxcfe_setSectorDataMark(FBuilder*,unsigned char datamark);

int hxcfe_popTrack (FBuilder* fb);

////////////////////////////////////////////
// Read Sector functions

FLOPPY* hxcfe_getFloppy(FBuilder* fb);
SECTORSEARCH* hxcfe_initSectorSearch(HXCFLOPPYEMULATOR* floppycontext,FLOPPY *fp);
SECTORCONFIG* hxcfe_getNextSector(SECTORSEARCH* ss,int track,int side);
SECTORCONFIG* hxcfe_searchSector(SECTORSEARCH* ss,int track,int side,int id);
int hxcfe_getSectorSize(SECTORSEARCH* ss,SECTORCONFIG* sc);
unsigned char * hxcfe_getSectorData(SECTORSEARCH* ss,SECTORCONFIG* sc);
int hxcfe_readSectorData(SECTORSEARCH* ss,int track,int side,int sector,int numberofsector,int sectorsize,unsigned char * buffer);
void hxcfe_freeSectorConfig(SECTORSEARCH* ss,SECTORCONFIG* sc);
void hxcfe_deinitSectorSearch(SECTORSEARCH* ss);


////////////////////////////////////////////
// Floppy interfaces setting functions.

// Floppy interface mode
#define IBMPC_DD_FLOPPYMODE				0x00
#define IBMPC_HD_FLOPPYMODE				0x01
#define ATARIST_DD_FLOPPYMODE			0x02
#define ATARIST_HD_FLOPPYMODE			0x03
#define AMIGA_DD_FLOPPYMODE				0x04
#define AMIGA_HD_FLOPPYMODE				0x05
#define CPC_DD_FLOPPYMODE				0x06
#define GENERIC_SHUGART_DD_FLOPPYMODE	0x07
#define IBMPC_ED_FLOPPYMODE				0x08
#define MSX2_DD_FLOPPYMODE				0x09
#define C64_DD_FLOPPYMODE				0x0A
#define EMU_SHUGART_FLOPPYMODE			0x0B

enum {
	DOUBLESTEP = 1,
	INTERFACEMODE = 2
};

enum {
	SET = 0,
	GET = 1
};

int hxcfe_floppyGetSetParams(HXCFLOPPYEMULATOR* floppycontext,FLOPPY * newfloppy,unsigned char dir,unsigned short param,void * value);
