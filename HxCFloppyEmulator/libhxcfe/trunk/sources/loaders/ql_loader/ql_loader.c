/*
//
// Copyright (C) 2006-2014 Jean-François DEL NERO
// Copyright (C) 2014-2015 Graeme Gregory <gg@slimlogic.co.uk>
//
// This file is part of the HxCFloppyEmulator library
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
//----------------------------------------------------- http://hxc2001.free.fr --//
///////////////////////////////////////////////////////////////////////////////////
// File : qlraw_loader.c
// Contains: RAW floppy image loader
//
// Written by:	DEL NERO Jean Francois
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "types.h"

#include "internal_libhxcfe.h"
#include "tracks/track_generator.h"
#include "libhxcfe.h"

#include "floppy_loader.h"
#include "floppy_utils.h"

#include "ql_loader.h"

#include "libhxcadaptor.h"

#include "tracks/sector_extractor.h"

struct ql_block0
{
	int8_t q5a_id[4];
	uint8_t q5a_mnam[10];
	uint16_t q5a_rand;
	uint32_t q5a_mupd;
	uint16_t q5a_free;
	uint16_t q5a_good;
	uint16_t q5a_totl;
	uint16_t q5a_strk;
	uint16_t q5a_scyl;
	uint16_t q5a_trak;
	uint16_t q5a_allc;
	uint16_t q5a_eodbl;
	uint16_t q5a_eodby;
	uint16_t q5a_soff;
	uint8_t q5a_lgph[18];
	uint8_t q5a_phlg[18];
	uint8_t q5a_spr0[20];
	uint8_t map[1];
};

#define QL_SSIZE 512

int QL_libIsValidDiskFile(HXCFE_IMGLDR* imgldr_ctx,char * imgfile)
{
	FILE *f;
	struct ql_block0 b0;
	int sectors;

	imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"IMG_libIsValidDiskFile");

	if(hxc_checkfileext(imgfile,"img"))
	{
		imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"QL_libIsValidDiskFile : %s is an QL file !",imgfile);

		f=hxc_fopen(imgfile,"rb");
		if(!f)
		{
			imgldr_ctx->hxcfe->hxc_printf(MSG_ERROR,"IMG_libIsValidDiskFile : Cannot open %s !",imgfile);
			return HXCFE_ACCESSERROR;
		}

		fread(&b0,sizeof(b0),1,f);
		hxc_fclose(f);

		if(strncmp("QL5",b0.q5a_id,3)!=0)
		{
			imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"QL_libIsValidDiskFile : non IMG file - invalid header !");
			return HXCFE_BADFILE;
		}

		sectors=htons(b0.q5a_totl);
		if ((sectors!=1440)&&(sectors!=2880)) {
			imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"QL_libIsValidDiskFile : unknown disk size %d\n",sectors);
			return HXCFE_BADFILE;
		}

		return HXCFE_VALIDFILE;
	}
	else
	{
		imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"QL_libIsValidDiskFile : not .img file !");
		return HXCFE_BADFILE;
	}

	return HXCFE_BADPARAMETER;
}

int QL_libLoad_DiskFile(HXCFE_IMGLDR* imgldr_ctx,HXCFE_FLOPPY * floppydisk,char * imgfile,void * parameters)
{
	FILE * f;
	unsigned int filesize;
	unsigned int i,j;
	unsigned short rpm;
	unsigned int file_offset;
	unsigned char* trackdata;
	unsigned char gap3len,trackformat,interleave;
	unsigned short sectorsize;
	HXCFE_CYLINDER* currentcylinder;
	struct ql_block0 b0;
	int sectors;

	imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"QL_libLoad_DiskFile %s",imgfile);

	f=hxc_fopen(imgfile,"rb");
	if(f==NULL)
	{
		imgldr_ctx->hxcfe->hxc_printf(MSG_ERROR,"Cannot open %s !",imgfile);
		return HXCFE_ACCESSERROR;
	}

	fread(&b0,sizeof(b0),1,f);

	sectors=htons(b0.q5a_totl);
	switch(sectors) {
	case 1440:
		floppydisk->floppySectorPerTrack=9;
		floppydisk->floppyBitRate=DEFAULT_DD_BITRATE;
		floppydisk->floppyiftype=IBMPC_DD_FLOPPYMODE;
		break;
	case 2880:
		floppydisk->floppySectorPerTrack=18;
		floppydisk->floppyBitRate=DEFAULT_HD_BITRATE;
		floppydisk->floppyiftype=IBMPC_HD_FLOPPYMODE;
		break;
	default:
		hxc_fclose(f);
		return HXCFE_BADFILE;
	}

	rpm=DEFAULT_DD_RPM;

	floppydisk->floppyNumberOfSide=2;
	floppydisk->floppyNumberOfTrack=80;
	floppydisk->tracks=(HXCFE_CYLINDER**)malloc(sizeof(HXCFE_CYLINDER*)*floppydisk->floppyNumberOfTrack);

	sectorsize=QL_SSIZE;
	interleave=1;
	gap3len=84;
	trackformat=IBMFORMAT_DD;

	trackdata = (unsigned char*)malloc(sectorsize*floppydisk->floppySectorPerTrack);

	for(j=0;j<floppydisk->floppyNumberOfTrack;j++)
	{

		floppydisk->tracks[j]=allocCylinderEntry(rpm,floppydisk->floppyNumberOfSide);
		currentcylinder=floppydisk->tracks[j];

		for(i=0;i<floppydisk->floppyNumberOfSide;i++)
		{
			file_offset=(sectorsize*(j*floppydisk->floppySectorPerTrack*floppydisk->floppyNumberOfSide))+
					(sectorsize*(floppydisk->floppySectorPerTrack)*i);
			fseek (f , file_offset , SEEK_SET);
			fread(trackdata,sectorsize*floppydisk->floppySectorPerTrack,1,f);

			currentcylinder->sides[i]=tg_generateTrack(trackdata,sectorsize,floppydisk->floppySectorPerTrack,(unsigned char)j,(unsigned char)i,1,interleave,(unsigned char)0,floppydisk->floppyBitRate,currentcylinder->floppyRPM,trackformat,gap3len,0,2500|REVERTED_INDEX,-2500);
		}
	}

	free(trackdata);

	imgldr_ctx->hxcfe->hxc_printf(MSG_INFO_1,"track file successfully loaded and encoded!");
	hxc_fclose(f);
	return HXCFE_NOERROR;
}

int QL_libWrite_DiskFile(HXCFE_IMGLDR* imgldr_ctx,HXCFE_FLOPPY * floppy,char * filename)
{
	int i,j,k,s;
	FILE * rawfile;
	unsigned char blankblock[512];
	int sectorsize,track_type_id;
	int systblockfound;
	struct ql_block0 b0;
	int sectors,sec_track;

	HXCFE_SECTORACCESS* ss;
	HXCFE_SECTCFG* sc;

	imgldr_ctx->hxcfe->hxc_printf(MSG_INFO_1,"Write QL_IMG file %s...",filename);

	memset(blankblock,0x00,sizeof(blankblock));

	if((floppy->floppyNumberOfTrack < 80) || (floppy->floppyNumberOfSide != 2) )
	{
		return HXCFE_BADPARAMETER;
	}

	rawfile=hxc_fopen(filename,"wb");
	if(rawfile)
	{
		ss=hxcfe_initSectorAccess(imgldr_ctx->hxcfe,floppy);

		/* Get header block to check size */
		sc=hxcfe_searchSector(ss,0,0,1,ISOIBM_MFM_ENCODING);
		if (sc->sectorsize == QL_SSIZE)
		{
			memcpy(&b0,sc->input_data,sizeof(b0));

			if(strncmp("QL5",b0.q5a_id,3)!=0)
			{
				imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"QL_libWrite_DiskFile : QL5 header missing\n");
				goto error;
			}
		}
		else
		{
			imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"QL_libWrite_DiskFile : Cannot find root sector\n");
			goto error;
		}

		sectors=htons(b0.q5a_totl);
		switch(sectors)
		{
			case 1440:
				sec_track=9;
				break;
			case 2880:
				sec_track=18;
				break;
			default:
				imgldr_ctx->hxcfe->hxc_printf(MSG_DEBUG,"QL_libWrite_DiskFile : unknown number of sectors %d\n", sectors);
				goto error;
		}

		if(ss)
		{
			for(j=0;j<80;j++)
			{
				for(i=0;i<2;i++)
				{
					for(s=1;s<=sec_track;s++)
					{
						sc = hxcfe_searchSector (ss,j,i,s,ISOIBM_MFM_ENCODING);

						if(sc)
						{
							sectorsize = sc->sectorsize;
							if(sectorsize == QL_SSIZE)
							{
								fwrite(sc->input_data,sectorsize,1,rawfile);
							}
							else
							{
								memset(blankblock,0x00,sizeof(blankblock));
								for(k=0;k<32;k++)
									strcat((char*)blankblock,">MISSING BLOCK<!");
								fwrite(blankblock,sizeof(blankblock),1,rawfile);
							}

							if(sc->input_data)
								free(sc->input_data);

							free(sc);
						}
						else
						{
							imgldr_ctx->hxcfe->hxc_printf(MSG_WARNING,"T%.2dH%dS%d : QL Sector not found !?!...",j,i,s);
							// Sector Not found ?!?
							// Put a blank data sector instead...
							memset(blankblock,0x00,sizeof(blankblock));
							for(k=0;k<31;k++)
								strcat((char*)blankblock,">MISSING BLOCK<!");
							fwrite(blankblock,sizeof(blankblock),1,rawfile);
						}
					}
				}
			}
			hxcfe_deinitSectorAccess(ss);
		}
		hxc_fclose(rawfile);
	}

	return 0;

error:
	hxcfe_deinitSectorAccess(ss);
	hxc_fclose(rawfile);
	return HXCFE_BADFILE;
}

int QL_libGetPluginInfo(HXCFE_IMGLDR* imgldr_ctx,uint32_t infotype,void * returnvalue)
{

	static const char plug_id[]="QL_IMG";
	static const char plug_desc[]="Sinclair QL floppy image loader";
	static const char plug_ext[]="img";

	plugins_ptr plug_funcs=
	{
		(ISVALIDDISKFILE)	QL_libIsValidDiskFile,
		(LOADDISKFILE)		QL_libLoad_DiskFile,
		(WRITEDISKFILE)		QL_libWrite_DiskFile,
		(GETPLUGININFOS)	QL_libGetPluginInfo
	};

	return libGetPluginInfo(
			imgldr_ctx,
			infotype,
			returnvalue,
			plug_id,
			plug_desc,
			&plug_funcs,
			plug_ext
			);
}
