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
// File : floppy_utils.c
// Contains: HxC Floppy Emulator Endurance / Stress test software.
//
// Written by: DEL NERO Jean Francois
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <math.h>
#include <bios.h>
#include <dos.h>
#include <conio.h>
#include <i86.h>

#include "upd765.h"
#include "dma.h"
#include "daccess.h"
#include "testhxcfe.h"

#include "print_output.h"
#include "floppyio.h"

//#define	DBGMODE	1

unsigned char tracksectors[32][1024];

extern unsigned char *bufrd;
extern unsigned char *bufwr;

extern unsigned short ticktimer;


unsigned int sector_size_table_mfm_500_26s[]={256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,256,0};

unsigned int sector_size_table_mfm_500[]={256,1024,256,512,256,512,256,1024,512,256,1024,256,256,512,512,512,1024,1024,0};
unsigned int sector_size_table_fm_500[]={256,1024,128,256,128,512,128,128,128,256,512,512,256,1024,128,0};

unsigned int sector_size_table_mfm_300[]={256,1024,256,512,512,256,256,512,512,256,1024,1024,0};
unsigned int sector_size_table_fm_300[]={256,1024,128,256,128,128,512,512,128,128,0};

unsigned int sector_size_table_mfm_250[]={256,1024,256,512,512,256,256,512,256,1024,0};
unsigned int sector_size_table_fm_250[]={256,1024,128,256,128,128,128,512,128,0};

extern unsigned long time_tick_count;

typedef struct _track_list{
	unsigned int nbsector;
	unsigned int gap3;
	unsigned int sectorsize;
	int density;
}track_list;

typedef struct _filelist{
	unsigned int index;
	unsigned int rpm;
	unsigned int bitrate;
	unsigned int density;
	unsigned int density_T0S0;
	unsigned int density_T0S1;

	unsigned int disable;
	unsigned int cycle;
}filelist;

typedef struct _floppystat{
	unsigned long cur_test_number;

	unsigned long total_nb_read_sector;
	unsigned long total_nb_write_sector;
	unsigned long total_nb_format;

	unsigned long total_size_read_sector;
	unsigned long total_size_write_sector;

	unsigned long total_nb_read_error;
	unsigned long total_nb_read_retry;
	unsigned long total_nb_write_error;
	unsigned long total_nb_format_error;
	unsigned long total_nb_data_error;

	unsigned long last_index_error;
	unsigned long last_track_error;
	unsigned long last_hide_error;
}floppystat;

floppystat test_stat;

track_list tlist[180];

unsigned long timetrack[180];

filelist file_list[]=
{
	// MFM
	{1,300,250,1,1,1,0,0},
	{2,300,500,1,1,1,0,0},
	{3,300,300,1,1,1,0,0},
	{4,360,250,1,1,1,0,0},
	{5,360,500,1,1,1,0,0},
	{6,360,300,1,1,1,0,0},
	// FM
	{7,300,250,0,0,0,0,0},
	{8,300,500,0,0,0,0,0},
	{9,300,300,0,0,0,0,0},
	{10,360,250,0,0,0,0,0},
	{11,360,500,0,0,0,0,0},
	{12,360,300,0,0,0,0,0},

	//---------------//
	// MFM (MIXED)
	{13,300,250,1,0,0,0,0},
	{14,300,500,1,0,0,0,0},
	{15,300,300,1,0,0,0,0},
	{16,360,250,1,0,0,0,0},
	{17,360,500,1,0,0,0,0},
	{18,360,300,1,0,0,0,0},
	// FM (MIXED)
	{19,300,250,0,1,1,0,0},
	{20,300,500,0,1,1,0,0},
	{21,300,300,0,1,1,0,0},
	{22,360,250,0,1,1,0,0},
	{23,360,500,0,1,1,0,0},
	{24,360,300,0,1,1,0,0},

	//---------------//
	// MFM (MIXED)
	{25,300,250,1,0,1,0,0},
	{26,300,500,1,0,1,0,0},
	{27,300,300,1,0,1,0,0},
	{28,360,250,1,0,1,0,0},
	{29,360,500,1,0,1,0,0},
	{30,360,300,1,0,1,0,0},
	// FM (MIXED)
	{31,300,250,0,1,0,0,0},
	{32,300,500,0,1,0,0,0},
	{33,300,300,0,1,0,0,0},
	{34,360,250,0,1,0,0,0},
	{35,360,500,0,1,0,0,0},
	{36,360,300,0,1,0,0,0},

	//---------------//
	// MFM (MIXED)
	{37,300,250,1,1,0,0,0},
	{38,300,500,1,1,0,0,0},
	{39,300,300,1,1,0,0,0},
	{40,360,250,1,1,0,0,0},
	{41,360,500,1,1,0,0,0},
	{42,360,300,1,1,0,0,0},
	// FM (MIXED)
	{43,300,250,0,0,1,0,0},
	{44,300,500,0,0,1,0,0},
	{45,300,300,0,0,1,0,0},
	{46,360,250,0,0,1,0,0},
	{47,360,500,0,0,1,0,0},
	{48,360,300,0,0,1,0,0},

	// Special
	{49,150,250,1,1,1,0,0},
	{50,150,300,1,1,1,0,0},
	{51,600,250,1,1,1,0,0},
	{52,600,500,1,1,1,0,0},
	{53,600,300,1,1,1,0,0},

	{54,150,250,0,0,0,0,0},
	{55,150,300,0,0,0,0,0},
	{56,600,250,0,0,0,0,0},
	{57,600,500,0,0,0,0,0},
	{58,600,300,0,0,0,0,0},

	{0,0,0,0,0,0,0,0}
};

void wait(unsigned char second)
{
	ticktimer = 0;
	do
	{

	}while(ticktimer < (second*18) );
}

void waittick(unsigned short tick)
{
	ticktimer = 0;
	do
	{

	}while(ticktimer < tick );
}

int	randomaccess(unsigned long nbsect)
{
	unsigned long i;
	int	track,head,sector;
	unsigned char sectorbuf[512];
	unsigned short status,errorcnt;

	errorcnt=0;

	hxc_printf(0,"Random Access test...\n");
	for(i=0;i<nbsect;i++)
	{
		track=rand()%80;
		head=rand()%2;
		sector=rand()%19;
		if(!sector)	sector=1;

		hxc_printf(0,"%u : ",i);
		hxc_printf(0,"Track:%d Head:%d Sector:%d (E:%d)\n",track,head,sector,errorcnt);
		status=chs_biosdisk(_DISK_READ,0, head, track,sector, 1, &sectorbuf);
		if(status)
		{
			hxc_printf(0,"------------Failed!!!!---------\n");
			status = chs_biosdisk(_DISK_RESET,0, 0, 255,0, 1, &sector);
			errorcnt++;
//			for(;;);
		}
	}

	hxc_printf(0,"Ok!!!!\n");
	return	0;
}

void test_track00_()
{
	int i;
	
	i=0;
	for(;;)
	{
		selectdrive(0);
		if(!(i&3))
			waittick(1);
		i++;
		
		if((i&7))
			selectdrive(4);
		if(!(i&3))
			waittick(1);
		i++;	
		
		
		selectdrive(1);
		if(!(i&3))
			waittick(1);
		i++;
		
		if((i&7))
			selectdrive(4);
		
		if(!(i&3))
			waittick(1);
		i++;		
		
		i++;
/*		calibratedrive(0);
	
		calibratedrive(1);
		
		calibratedrive(2);
		
		calibratedrive(3);*/
		
//		waittick(1);
	}

}

void swait()
{
	unsigned short w;
	
	w = 0;
	
	do
	{
		w--;
	}while(w);
}

void test_track00()
{
	int i;
	
	i=0;
	for(;;)
	{
		selectdrive(0);
		swait();
		
		selectdrive(4);
		swait();

		selectdrive(0);
		swait();
		
		selectdrive(4);
		swait();

		selectdrive(0);
		swait();
		
		selectdrive(4);
		swait();

		selectdrive(0);
		swait();
		
		selectdrive(4);
		swait();
		
		selectdrive(1);
		swait();
		
		selectdrive(4);
		swait();
////////////////////////////
		selectdrive(1);
		swait();
		
		selectdrive(4);
		swait();		

		selectdrive(1);
		swait();
		
		selectdrive(4);
		swait();		
		
		selectdrive(1);
		swait();
		
		selectdrive(4);
		swait();		
		
		selectdrive(1);
		swait();
		
		selectdrive(4);
		swait();		
		
		
	}
}

void test_step()
{
	int track;
	int side;

	side =0;
	do
	{
		for(track=0;track<80;track++)
		{
			hxc_printf(0,"Track: %d, Side: %d\n",track,side);
			trackseek(0,track,side);
			wait(1);
		}
	}while(1);
}

int forceaccess()
{
	int i,j;
	int track;
	int side;
	unsigned char c;

	fdc_specify(2);

	hxc_printf(0,"+ : inc track, - dec track, s: change side, 0-9, r, i\n");

	track = 0;
	side = 0;

	do
	{
		hxc_printf(0,"Track: %d, Side: %d\n",track,side);
		trackseek(0,track,side);
		c=getch();
		switch(c)
		{
			case '+':
				if(track<256) track++;
			break;
			case '-':
				if(track) track--;
			break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				track = (c - '0') * 10;
			break;
			case 'r':
				for(i=0;i<100;i++)
				{
					track = rand()%80;
					side = rand()%1;
					hxc_printf(0,"Track: %d, Side: %d\n",track,side);
					trackseek(0,track,side);
				}
			break;
			case 'i':
				for(;;)
				{
					for(j=0;j<2;j++)
					{
						for(i=0;i<80;i++)
						{
							hxc_printf(0,"Track: %d, Side: %d\n",i,j);
							trackseek(0,i,j);
						}
					}
				}
			break;
			case 's':
				if(side)
					side = 0;
				else
					side = 1;
			break;
		}

	}while(1);

}

int seektest(int drive)
{
	int testcnt;

	hxc_printf(0,"Seek test...\n");
	for(testcnt=0;testcnt<10;testcnt++)
	{	
		trackseek(drive,10,0);
		trackseek(drive,20,0);
		
		trackseek(drive,15,0);
		trackseek(drive,30,0);
		
		trackseek(drive,20,0);
		trackseek(drive,40,0);
		
		trackseek(drive,25,0);
		trackseek(drive,50,0);
		
		trackseek(drive,30,0);
		trackseek(drive,60,0);
		
		trackseek(drive,35,0);
		trackseek(drive,70,0);
		
		trackseek(drive,70,0);
		trackseek(drive,75,0);	
	}

	for(testcnt=0;testcnt<60;testcnt++)
	{	
		trackseek(drive,70,0);
		trackseek(drive,72,0);
	}

	trackseek(drive,40,0);

	for(testcnt=0;testcnt<10;testcnt++)
	{	
		trackseek(drive,50,0);
		waittick(testcnt);
		trackseek(drive,52,0);
		waittick(testcnt);
	}

	for(testcnt=10;testcnt<80;testcnt++)
	{	
		trackseek(drive,testcnt,0);
		waittick(3);
	}

	for(testcnt=80;testcnt>10;testcnt--)
	{	
		trackseek(drive,testcnt,0);
		waittick(3);
	}
	
	trackseek(drive,40,0);
	
	wait(1);

	return 0;
}

int	testdrive(int index,int drive,unsigned int * secttable, int trackformat,unsigned int * secttableT0S0, int trackformatT0S0,unsigned int * secttableT0S1, int trackformatT0S1,int bitrate,int readonly)
{
	unsigned int i,j,ret,c,testcnt;
	unsigned char track,head,precomp;

	unsigned int * sectortable;
	unsigned char deleted;
	int	density,fail,failcnt,okcnt,gap3;

	deleted = 0x00;
	precomp = 0;
	fail    = 0;
	failcnt = 0;
	okcnt   = 0;
	gap3 = 30;

	SetIndex(index);
	wait(4);

	seektest(drive);

	for(testcnt=0;testcnt<30;testcnt++)
	{
		track=(rand()%80);
		head=rand()%2;

		if(!track) track =1;

		if(track)
		{
			sectortable = secttable;
			density = trackformat;
		}
		else
		{
			if(head)
			{
				sectortable = secttableT0S1;
				density = trackformatT0S1;
			}
			else
			{
				sectortable = secttableT0S0;
				density = trackformatT0S0;
			}
		}

		hxc_printf(0,"Track:%d, Side:%d, density:%d, Precomp: %d, deleted : 0x%.2x Rate:%d\n",track,head,density,precomp&7,deleted,bitrate);

		trackseek(drive,track,head);

		i=0;
		while(sectortable[i])
		{
			for(j=0;j<sectortable[i];j++)
			{
				tracksectors[i][j]=rand();
			}
			//tracksectors[i][0]=0xAA;
			//tracksectors[i][sectortable[i]-1]=0x55;
			i++;
		}

		if(!readonly)
		{
			i=0;
			while(sectortable[i])
			{
				memcpy(bufwr,&tracksectors[i],sectortable[i]);
				ret = write_sector(deleted,1+i,drive,head,track,1,1,sectortable[i],density,precomp,bitrate,gap3);
				fd_result(1);
				if(ret)
				{
					test_stat.total_nb_write_error++;
					test_stat.last_index_error = index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = head;
				}

				test_stat.total_nb_write_sector++;
				test_stat.total_size_write_sector = test_stat.total_size_write_sector + sectortable[i];

				i++;				
			}
		}

		i=0;
		while(sectortable[i])
		{

			c=0;
			do
			{
				memset(bufrd,0,sectortable[i]);
				ret = read_sector(deleted,1+i,drive,head,track,1,1,sectortable[i],density,bitrate,gap3);
				fd_result(1);
				if(ret)
				{					
					hxc_printf(0,"Read Error Track %d Side %d Sector %d Deleted:%d Size :%d Retry...\n",track,head,1+i,deleted,sectortable[i]);

					test_stat.last_index_error = index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = head;					
				}
				c++;
			}while(ret &&  c < 3);

			if(ret)
			{
				test_stat.total_nb_read_error++;
			}

			test_stat.total_nb_read_retry = test_stat.total_nb_read_retry + (c - 1);
			test_stat.total_nb_read_sector++;
			test_stat.total_size_read_sector = test_stat.total_size_read_sector + sectortable[i];

			if(!readonly)
			{
				if(memcmp(bufrd,&tracksectors[i],sectortable[i]))
				{
					test_stat.total_nb_data_error++;
					hxc_printf(0,"-!-!-!-!-!-!-Write diff-!-!-!-!-!-!- : Sector %d Size:%d\n",i+1,sectortable[i]);
					fail = 1;

					test_stat.last_index_error = index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = head;
				}
			}

			i++;
		}

		if(!fail)
		{
			okcnt++;
			if(readonly)
				hxc_printf(0,"---------Read  ok--------- ok: %d fail: %d rderr %d (retry %d) wrerr %d\n",okcnt,failcnt,test_stat.total_nb_read_error,test_stat.total_nb_read_retry,test_stat.total_nb_write_error);
			else
				hxc_printf(0,"---------Write ok--------- ok: %d fail: %d rderr %d (retry %d) wrerr %d\n",okcnt,failcnt,test_stat.total_nb_read_error,test_stat.total_nb_read_retry,test_stat.total_nb_write_error);
		}
		else
		{
			failcnt++;
			hxc_printf(0,"---!--->>Write Failed<<---!--- ok: %d fail: %d rderr %d (retry %d) wrerr %d\n",okcnt,failcnt,test_stat.total_nb_read_error,test_stat.total_nb_write_error,test_stat.total_nb_read_retry,test_stat.total_nb_write_error);

			trackseek(0,1,0);
			calibratedrive(0);
			fail = 0;
		}

		if(!readonly)
			deleted=~deleted;

		precomp++;
	}

	return 0;
}

int getmaxsector(int sectorsize,int gap3,unsigned int bitrate,unsigned int rpm, int density)
{
	int tracksize;
	long ssize,nbsect;

	nbsect =0;

	if(density)
	{

		tracksize = (unsigned long)((float)(bitrate)*( ((float)(60*125)/(float)rpm)));
		ssize = 60 + sectorsize + 2 + gap3 + 4;
		
		tracksize = tracksize - 80;
		do
		{
			tracksize = tracksize - ssize;
			if(tracksize>0)
			{
				nbsect++;
			}
		}while(tracksize>0);
	}
	else
	{
		tracksize = (unsigned long)((float)(bitrate/2)*( ((float)(60*125)/(float)rpm)));
		ssize = 31 + sectorsize + 2 + gap3 + 4;

		tracksize = tracksize - 38;
		do
		{
			tracksize = tracksize - ssize;
			if(tracksize>0)
			{
				nbsect++;
			}
		}while(tracksize>0);
	}

	return nbsect;
}

void printsize(unsigned long size)
{
	if(size < 1024)
	{
		hxc_printf(0,"%u B ",size);
	}
	else
	{
		if(size < 1048576)
		{
			hxc_printf(0,"%u.%.2d KB ",(size/1024),(((size-((size/1024)*1024))*100)/1024));
		}
		else
		{
			hxc_printf(0,"%u.",(size/(unsigned long)1048576));
			hxc_printf(0,"%.3u MB ",((size-((size/1048576)*1048576))*1000)/1048576);
		}
	}
}

void print_stat(floppystat * floppystat)
{
	hxc_printf(0,"\n-------------------------------------------------------------------------------\n");
	hxc_printf(0,"Test Duration : %lu s\n",(time_tick_count/91)*5);
	hxc_printf(0,"Current Test Number : %lu\n",floppystat->cur_test_number);
	hxc_printf(0,"\n");

	hxc_printf(0,"Total number of Track Format : %lu\n",floppystat->total_nb_format);
	hxc_printf(0,"Total number of Format Error: >>>> %lu <<<<<\n",floppystat->total_nb_format_error);
	hxc_printf(0,"\n");

	hxc_printf(0,"Total number of Write : %lu Sectors\n",floppystat->total_nb_write_sector);
	hxc_printf(0,"Total Write Size : ");
	printsize(floppystat->total_size_write_sector);
	hxc_printf(0,"\n");
	hxc_printf(0,"Total number of Write Error: >>>> %lu <<<<<\n",floppystat->total_nb_write_error);
	hxc_printf(0,"\n");

	hxc_printf(0,"Total number of Read : %lu Sectors\n",floppystat->total_nb_read_sector);
	hxc_printf(0,"Total Read Size : ");
	printsize(floppystat->total_size_read_sector);
	hxc_printf(0,"\n");
	hxc_printf(0,"Total number of Read Retry: >>>> %lu <<<<<\n",floppystat->total_nb_read_retry);
	hxc_printf(0,"\n");
	hxc_printf(0,"Total number of Read Error: >>>> %lu <<<<<\n",floppystat->total_nb_read_error);
	hxc_printf(0,"\n");
	hxc_printf(0,"Total number of Data Error: >>>> %lu <<<<<\n",floppystat->total_nb_data_error);


	hxc_printf(0,"\nLast Error Image %lu,Track %lu,Side %lu\n",test_stat.last_index_error,test_stat.last_track_error,test_stat.last_hide_error);

	hxc_printf(0,"-------------------------------------------------------------------------------\n");
}

void write_read_test(int index)
{
	switch(index)
	{
		case 60:
			hxc_printf(0,"1 - MFM500K.HFE\n");
			testdrive(index,0,sector_size_table_mfm_500,1,sector_size_table_fm_500, 0,sector_size_table_fm_500,	0,500,0);
		break;

		case 61:
			hxc_printf(0,"2 - MFM300K.HFE\n");
			testdrive(index,0,sector_size_table_mfm_300,1,sector_size_table_fm_300, 0,sector_size_table_fm_300,	0,300,0);
		break;

		case 62:
			hxc_printf(0,"3 - MFM250K.HFE\n");
			testdrive(index,0,sector_size_table_mfm_250,1,sector_size_table_fm_250, 0,sector_size_table_fm_250, 0,250,0);
		break;

		case 63:
			hxc_printf(0,"4 - FM500K.HFE\n");
			testdrive(index,0,sector_size_table_fm_500,0,sector_size_table_mfm_500, 1,sector_size_table_mfm_500, 1,500,0);
		break;

		case 64:
			hxc_printf(0,"5 - FM300K.HFE\n");
			testdrive(index,0,sector_size_table_fm_300,0,sector_size_table_mfm_300, 1,sector_size_table_mfm_300, 1,300,0);
		break;

		case 65:			
			hxc_printf(0,"6 - FM250K.HFE\n");			
			testdrive(index,0,sector_size_table_fm_250,0,sector_size_table_mfm_250, 1,sector_size_table_mfm_250, 1,250,0);
		break;		
	}
	
	print_stat(&test_stat);
}

void format_write_read(int drive)
{
	int track,ret,i;
	int side,nbsector,sector;
	int sectorsize,precomp,sprecomp;
	unsigned char formatvalue,gap3,fvalue;
	int t,sectshift;
	unsigned int cycle,current_index,current_index2,base_index;
	unsigned int filei, gapindex;
	int bitrate;
	int density,retry;

	precomp = 0;
	sprecomp = 0;
	gap3 = 30;
	formatvalue = 0x00;

	cycle = 0;
	sectshift = 0;
	filei = 0;

	gapindex = 0;

	current_index2 = 60;

	base_index = GetCurrentIndex();
	hxc_printf(0,"Current index : %d\n",base_index);

	do{

		write_read_test(current_index2);

		current_index2++;
		if(current_index2>65) current_index2 = 60;

		do
		{
			if(!file_list[filei].index)
			{
				filei = 0;
			}

			if(file_list[filei].disable)
				filei++;

		}while(file_list[filei].disable || !file_list[filei].index);

		current_index = file_list[filei].index + base_index;

		bitrate = file_list[filei].bitrate;
		do
		{
			t=0;
			density = file_list[filei].density_T0S0;
			tlist[t].sectorsize = 128<<(((sectshift%5)+density));
			tlist[t].gap3 = 30 + (gapindex&0x3F);
			tlist[t].density = density;
			sectshift++;
			tlist[t].nbsector = getmaxsector(tlist[t].sectorsize,tlist[t].gap3,bitrate,file_list[filei].rpm, density);
		}while(tlist[t].nbsector<=0);
		gapindex++;

		do
		{
			t=1;
			density = file_list[filei].density_T0S1;
			tlist[t].sectorsize = 128<<(((sectshift%5)+density));
			tlist[t].gap3 = 30 + (gapindex&0x3F);
			tlist[t].density = density;
			sectshift++;
			tlist[t].nbsector = getmaxsector(tlist[t].sectorsize,tlist[t].gap3,bitrate,file_list[filei].rpm, density);
		}while(tlist[t].nbsector<=0);
		gapindex++;

		for(t=2;t<160;t++)
		{
			do
			{
				density = file_list[filei].density;
				tlist[t].gap3 = 30 + (gapindex&0x3F);
				tlist[t].sectorsize = 128<<(((sectshift%5)+density));
				tlist[t].density = density;
				tlist[t].nbsector = getmaxsector(tlist[t].sectorsize,tlist[t].gap3,bitrate,file_list[filei].rpm, density);
				sectshift++;

			}while(tlist[t].nbsector<=0);

			gapindex++;
		}
		sectshift++;
		gapindex++;

		hxc_printf(0,">>>>>>>>Change image : %d<<<<<<<<<<<<\n",current_index);
		SetIndex(current_index);
		wait(4);

		fvalue = formatvalue;
		sprecomp = precomp;
		for(track=0;track<80;track++)
		{
			trackseek(drive,track,side);
			for(side=0;side<=1;side++)
			{
				sectorsize = tlist[(track<<1) | side].sectorsize;
				nbsector = tlist[(track<<1) | side].nbsector;
				density = tlist[(track<<1) | side].density;
				gap3 = tlist[(track<<1) | side].gap3;

				hxc_printf(0,"Format:%dx%d, T:%d, S:%d, Dens:%d, WComp:%d, Rate:%d, Gap:%d\n",sectorsize,nbsector,track,side,density,precomp&7,bitrate,gap3);
				ret = format_track(drive,density,track,side,nbsector,sectorsize,1,formatvalue,precomp,bitrate,gap3);
				fd_result(1);
				if(ret)
				{
					test_stat.total_nb_format_error++;
					hxc_printf(0,"Format Error Track %d Side %d...\n",track,side);

					test_stat.last_index_error = current_index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = side;

					file_list[filei].disable = 1;
				}

				formatvalue++;
				precomp++;

				test_stat.total_nb_format++;
				test_stat.total_size_write_sector = test_stat.total_size_write_sector + (unsigned long)(nbsector * sectorsize);
			}
		}

		print_stat(&test_stat);

		precomp = sprecomp;
		formatvalue = fvalue;
		for(track=0;track<80;track++)
		{
			trackseek(drive,track,side);
			for(side=0;side<=1;side++)
			{
				sectorsize = tlist[(track<<1) | side].sectorsize;
				nbsector = tlist[(track<<1) | side].nbsector;
				density = tlist[(track<<1) | side].density;
				gap3 = tlist[(track<<1) | side].gap3;

				hxc_printf(0,"Read:%dx%d, T:%d, S:%d, Dens:%d, WComp:%d, Rate:%d, Gap:%d\n",sectorsize,nbsector,track,side,density,precomp&7,bitrate,gap3);
				bufrd[0]=0xAA;
				bufrd[1]=43;

				sector = 0;
				retry = 0;
				do
				{
					ret = read_sector(0,1+sector,drive,side,track,1,nbsector,sectorsize,density,bitrate,gap3);
					fd_result(1);
					retry++;

					if(ret && (retry < 3))
						hxc_printf(0,"Read Retry Track %d Side %d Sector %d Size :%d Retry...\n",track,side,1+sector,sectorsize);

				}while(ret && (retry < 3) );

				test_stat.total_nb_read_retry = test_stat.total_nb_read_retry + (retry-1);

				if(ret)
				{
					test_stat.total_nb_read_error++;
					hxc_printf(0,"Read Error Track %d Side %d Sector %d Size :%d Retry...\n",track,side,1+sector,sectorsize);

					test_stat.last_index_error = current_index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = side;

					file_list[filei].disable = 1;
				}

				memset(bufwr,formatvalue,nbsector*sectorsize);
				if(memcmp(bufwr,bufrd,nbsector*sectorsize))
				{
					hxc_printf(0,"Data Error Track %d Side %d Sector %d Size :%d\n",track,side,1+sector,sectorsize);
					test_stat.total_nb_data_error++;
					test_stat.total_nb_format_error++;

					test_stat.last_index_error = current_index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = side;

					file_list[filei].disable = 1;
				}

				formatvalue++;
				precomp++;

				test_stat.total_nb_read_sector = test_stat.total_nb_read_sector + (unsigned long)nbsector;
				test_stat.total_size_read_sector = test_stat.total_size_read_sector + (unsigned long)(nbsector * sectorsize);
			}
		}

		print_stat(&test_stat);

		srand(fvalue);

		precomp = sprecomp;
		for(track=0;track<80;track++)
		{
			trackseek(drive,track,side);
			for(side=0;side<=1;side++)
			{
				sectorsize = tlist[(track<<1) | side].sectorsize;
				nbsector = tlist[(track<<1) | side].nbsector;
				density = tlist[(track<<1) | side].density;
				gap3 = tlist[(track<<1) | side].gap3;

				hxc_printf(0,"Write:%dx%d, T:%d, S:%d, Dens:%d, WComp:%d, Rate:%d, Gap:%d\n",sectorsize,nbsector,track,side,density,precomp&7,bitrate,gap3);

				for(i=0;i<(nbsector*sectorsize);i++)
				{
					bufwr[i] = rand();
				}

				timetrack[(track<<1) | side] = time_tick_count;

				for(i=0;i<(nbsector);i++)
				{
					sprintf(&bufwr[i*sectorsize],"--- T%.2dH%dS%.2d,%.4dx%.2d,Dens:%d,Rate:%dkbs,Gap:%.2d,PComp:%d,filei:%d,Cycle:%.3d,fCycle:%.3d,Tim:%.8lu,Curtest:%.4lu ---",track,side,i+1,sectorsize,nbsector,density,bitrate,gap3,precomp&7,filei,cycle,file_list[filei].cycle,timetrack[(track<<1) | side],test_stat.cur_test_number);
				}

				memset(bufrd,0,nbsector*sectorsize);

				sector = 0;
				ret = write_sector(0,1+sector,drive,side,track,1,nbsector,sectorsize,density,0,bitrate,gap3);
				fd_result(1);
				if(ret)
				{
					test_stat.total_nb_write_error++;
					hxc_printf(0,"Write Error Track %d Side %d Sector %d Size :%d Retry...\n",track,side,1+sector,sectorsize);

					test_stat.last_index_error = current_index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = side;

					file_list[filei].disable = 1;
				}

				precomp++;

				test_stat.total_nb_write_sector = test_stat.total_nb_write_sector + (unsigned long)nbsector;
				test_stat.total_size_write_sector = test_stat.total_size_write_sector + (unsigned long)(nbsector * sectorsize);
			}
		}

		print_stat(&test_stat);

		precomp = sprecomp;
		srand(fvalue);
		for(track=0;track<80;track++)
		{
			trackseek(drive,track,side);
			for(side=0;side<=1;side++)
			{
				sectorsize = tlist[(track<<1) | side].sectorsize;
				nbsector = tlist[(track<<1) | side].nbsector;
				density = tlist[(track<<1) | side].density;
				gap3 = tlist[(track<<1) | side].gap3;

				hxc_printf(0,"(W) Read:%dx%d, T:%d, S:%d, Dens:%d, WComp:%d, Rate:%d, Gap:%d\n",sectorsize,nbsector,track,side,density,precomp&7,bitrate,gap3);

				for(i=0;i<(nbsector*sectorsize);i++)
				{
					bufwr[i] = rand();
				}
				for(i=0;i<(nbsector);i++)
				{
					sprintf(&bufwr[i*sectorsize],"--- T%.2dH%dS%.2d,%.4dx%.2d,Dens:%d,Rate:%dkbs,Gap:%.2d,PComp:%d,filei:%d,Cycle:%.3d,fCycle:%.3d,Tim:%.8lu,Curtest:%.4lu ---",track,side,i+1,sectorsize,nbsector,density,bitrate,gap3,precomp&7,filei,cycle,file_list[filei].cycle,timetrack[(track<<1) | side],test_stat.cur_test_number);
				}

				sector = 0;
				retry = 0;
				do
				{
					ret = read_sector(0,1+sector,drive,side,track,1,nbsector,sectorsize,density,bitrate,gap3);
					fd_result(1);
					retry++;

					if(ret && (retry < 3))
						hxc_printf(0,"Read Retry Track %d Side %d Sector %d Size :%d Retry...\n",track,side,1+sector,sectorsize);

				}while(ret && (retry < 3) );

				test_stat.total_nb_read_retry = test_stat.total_nb_read_retry + (retry-1);

				if(ret)
				{
					test_stat.total_nb_read_error++;
					hxc_printf(0,"Read Error Track %d Side %d Sector %d Size :%d Retry...\n",track,side,1+sector,sectorsize);

					test_stat.last_index_error = current_index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = side;

					file_list[filei].disable = 1;
				}

				if(memcmp(bufwr,bufrd,nbsector*sectorsize))
				{
					hxc_printf(0,"Write Data Error Track %d Side %d Sector %d Size :%d\n",track,side,1+sector,sectorsize);
					test_stat.total_nb_write_error++;
					test_stat.total_nb_data_error++;

					test_stat.last_index_error = current_index;
					test_stat.last_track_error = track;
					test_stat.last_hide_error = side;

					file_list[filei].disable = 1;
				}

				precomp++;

				test_stat.total_nb_read_sector   = test_stat.total_nb_read_sector   + (unsigned long)nbsector;
				test_stat.total_size_read_sector = test_stat.total_size_read_sector + (unsigned long)(nbsector * sectorsize);
			}
		}

		file_list[filei].cycle++;

		print_stat(&test_stat);

		formatvalue++;

		precomp = sprecomp;

		cycle++;
		precomp++;
		filei++;

		test_stat.cur_test_number++;
	}while(1);
}

int	main(int argc, char* argv[])
{
	direct_access_cmd_sector dacs;

	init_outputtxt();

	hxc_printf(0,"--------------------------------------------------------\n");
	hxc_printf(0,"-- HxC Floppy Emulator Endurance/Stress Test Software --\n");
	hxc_printf(0,"--------------------------------------------------------\n");

	memset(&test_stat,0,sizeof(floppystat));

	memset(&dacs,0,sizeof(direct_access_cmd_sector));
	sprintf(dacs.DAHEADERSIGNATURE,"HxCFEDA");
	
	init_floppyio();

	GetHxCVer(0);

	format_write_read(0);

	//forceaccess();
/*	for(;;)
	{
	
	test_track00();
	}*/

	return 0;
}


