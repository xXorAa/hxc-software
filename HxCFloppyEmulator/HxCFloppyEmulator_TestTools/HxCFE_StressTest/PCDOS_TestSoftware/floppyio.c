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
// File : floppyio.c
// Contains: upd765 Floppy IO functions.
//
// Written by:  DEL NERO Jean Francois
//
// Change History (most recent first):
///////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <string.h>

#include <bios.h>
#include <dos.h>
#include <conio.h>
#include <i86.h>

#include "upd765.h"
#include "dma.h"
#include "print_output.h"
#include "floppyio.h"

unsigned char fdc_track;
unsigned char sr0;
unsigned char status[8];

unsigned char intflag;

unsigned short ticktimer;

unsigned char * dmabuffer;
unsigned short dmabuf_off;
unsigned dmabuf_seg_rd,dmabuf_seg_wr;

unsigned char *bufrd;
unsigned char *bufwr;

void (__interrupt __far *prev_floppy_int)( void );
void (__interrupt __far *prev_timer_int)( void );

// Digital Output Register - Drive select (0-3) values
unsigned char DORsel[] = { 0x14, 0x25, 0x46, 0x87 };
unsigned int  ratecode[]={500,300,250,1000,0};

unsigned char sector_buf[512];

unsigned int sizecode[]={128,256,512,1024,2048,4096,8192,0};

void _interrupt	timer_interrupt_handler(void)
{
	ticktimer++;
	outp(0x20,0x20);
}

void _interrupt floppy_interrupt_handler(void)
{
	//printf("IRQ   6\n");
	intflag=0xFF;
	outp(0x20,0x20);
	//outp(0x3F2,0x14);
}

void init_floppyio(void)
{
	int status;

	ticktimer = 0x00;
	intflag = 0x00;

	memset(&sector_buf,0,512);

	status	= chs_biosdisk(_DISK_RESET,0, 0, 255,0, 1, &sector_buf);
	if(status)
		hxc_printf(0,"FDC Reset error : Return value %d\n",status);

	prev_floppy_int	= _dos_getvect( 8+0x6 );
	_dos_setvect( 8+0x6, floppy_interrupt_handler );

	prev_timer_int = _dos_getvect( 8+0x0 );
	_dos_setvect( 8+0x0, timer_interrupt_handler	);

	if(_dos_allocmem( 32768/16, &dmabuf_seg_rd ))
	{
		hxc_printf(0,"Alloc Error !\n");
		for(;;);
	}
	bufrd = (unsigned char *)MK_FP(dmabuf_seg_rd, 0);

	if(_dos_allocmem( 32768/16, &dmabuf_seg_wr ))
	{
		hxc_printf(0,"Alloc Error !\n");
		for(;;);
	}
	bufwr = (unsigned char *)MK_FP(dmabuf_seg_wr, 0);

	reset_drive(0);
}

int	getsizecode(int size)
{
	int	i;

	i=0;
	while(sizecode[i] && (sizecode[i]!=size))
	{
		i++;
	}

	return i;
}


/*
 * Read byte from FDC
 */
unsigned short rdfdc(void)
{
	int tmo;

	for(tmo=0;tmo<128;tmo++)
	{
		if( ( inp( FDC_MSR ) & 0xD0 ) == 0xD0 )
		{
			return inp( FDC_DATA );
		}

		inp( 0x80 );
	}

	return 0xFFFF;
};

/*
 * Write byte to the FDC
 */
void wrfdc(unsigned char cmd)
{
	int tmo;

	for (tmo=0;tmo<128;tmo++)
	{
		if( ( inp(FDC_MSR) & 0xC0 ) == 0x80 )
		{
			outp( FDC_DATA, cmd );
			return;
		}

		inp( 0x80 );
	}
}


int getratecode(int	bitrate)
{
	int i;

	i=0;
	while(ratecode[i] && (ratecode[i]!=bitrate))
	{
		i++;
	}

	return i;
}

int fd_result(int sensei)
{
	int n;
	int data;

	// Read in command result bytes
	n   =   0;
	while (n < 7 && ( inp(FDC_MSR) & (1 << 4)) != 0)
	{
		data = rdfdc();
		status[n++] = data;
		n++;
	}

	if (sensei)
	{
		// Send a "sense interrupt status" command
		wrfdc(FDC_CMD_SENSEI);
		sr0 = rdfdc();
		fdc_track = rdfdc();
	}

	return 0;
}

/*
 *   Wait for interrupt from the FDC
 *   Last byte of command is sent here so that we can enable
 *   interrupt before commands begins.
 */
unsigned char waitirq()
{
	ticktimer = 0;
	do
	{
	}while( (!intflag) && (ticktimer<(18*3)) );

	if(ticktimer>=(18*3))
	{
		hxc_printf(0,"Waitirq Timeout\n");
		return  0;
	}

	return  1;
}

/*
 * Initialize DMA channel
 */
void initdma(unsigned char mode, unsigned count)
{
	unsigned dmabuf_seg;

	// Set up DMA
	outp(DMA1_CHAN, 0x06);

	if(mode   ==  FD_MODE_READ)
	{
		dmabuf_seg  =  dmabuf_seg_rd;
		outp(DMA1_RESET, DMA1_CHAN2_READ);
		outp(DMA1_MODE, DMA1_CHAN2_READ);
	}
	else
	{
		dmabuf_seg = dmabuf_seg_wr;
		outp(DMA1_RESET, DMA1_CHAN2_WRITE);
		outp(DMA1_MODE,  DMA1_CHAN2_WRITE);
	}

	//  Setup   DMA transfer
	outp(DMA1_CHAN2_ADDR, (dmabuf_seg << 4)  &   255 );
	outp(DMA1_CHAN2_ADDR, (dmabuf_seg >> 4)  &   255 );
	outp(DMA1_CHAN2_PAGE,  dmabuf_seg >> 12);
	outp(DMA1_CHAN2_COUNT, ((count - 1) &  0xFF));
	outp(DMA1_CHAN2_COUNT, ((count - 1) >> 8));
	outp(DMA1_CHAN, 0x02);
}

void calibratedrive(unsigned char drive)
{
	intflag=0;

#ifdef DBGMODE
	hxc_printf(0,"Calibrate drive %d ... ",drive);
#endif
	outp(FDC_DOR, DORsel[drive&3] | 0xC );

	wrfdc(FDC_CMD_RECAL);
	wrfdc(drive&3);

	waitirq();
	fd_result(1);

#ifdef DBGMODE
	hxc_printf(0,"done\n");
#endif

}

void trackseek(unsigned char drive,unsigned char track,unsigned char head)
{
	unsigned char byte,ret;

	do
	{
		intflag=0;

#ifdef  DBGMODE
		hxc_printf(0,"Seek drive %d to track %d head %d... ",drive,track,head);
#endif

		outp(FDC_DOR, DORsel[drive&3] | 0xC);

		wrfdc(FDC_CMD_SEEK);

		byte = drive & 3;
		if(head)
		{
			byte = byte | 0x4;
		}

		wrfdc(byte);
		wrfdc(track);
		ret = waitirq();
		fd_result(1);

	}while(!ret);

#ifdef  DBGMODE
	hxc_printf(0,"done\n");
#endif

}

/*
 * Read a sector from the disk
 */
int read_sector(unsigned char deleted,unsigned index,unsigned char drive,unsigned char head,unsigned char track,unsigned char sector,unsigned char nbsector,unsigned int size,unsigned char density,int rate,int gap3)
{
	unsigned char byte,ret;

#ifdef  DBGMODE
	hxc_printf(0,"Read Sector %d track %d head %d drive %d...\n",sector,track,head,drive);
#endif
	intflag=0;

	// Program data rate 500K/s)
	outp(FDC_DRS,getratecode(rate));
	outp(FDC_CCR,getratecode(rate));
	outp(FDC_DOR, DORsel[drive&3] | 0xC);

	initdma(FD_MODE_READ, size * nbsector);

	if(deleted)
		byte = FDC_CMD_READDELETEDDATA;
	else
		byte = FDC_CMD_READ;

	byte = byte | 0xA0;
	if(density)
		byte = byte | 0x40;
	wrfdc(byte);

	byte = drive & 3;
	if( head )
		byte = byte | 0x4;
	wrfdc(byte);

	wrfdc(track);                               //  Cylinder
	wrfdc(head);                                //  Head
	wrfdc(index);                               //  Sector
	wrfdc(getsizecode(size));                   //  N
	wrfdc(index + (nbsector-1));                //  EOT
	wrfdc(gap3);                                //  GL
	wrfdc(0xFF);

	ret =   waitirq();
	//if(ret)

	fd_result(1);

	if((status[0] & 0xC0) || !ret)
	{
		hxc_printf(0,"Read failed : ST0:0x%.2x, ST1:0x%.2x, ST2:0x%.2x\n",status[0],status[1],status[2]);
		return  1;
	}

#ifdef  DBGMODE
	hxc_printf(0,"done\n");
#endif

	return 0;
}

/*
 * Write a sector to the disk
 */
int write_sector(unsigned char deleted,unsigned index,unsigned char drive,unsigned char head, unsigned char track,unsigned char sector,unsigned char nbsector,unsigned int size,unsigned char density,unsigned char precomp,int rate,int gap3)
{
	unsigned char byte,ret;

#ifdef DBGMODE
	hxc_printf(0,"Write Sector %d track %d head %d drive %d ...\n",sector,track,head,drive);
#endif

	intflag=0;

	if(((precomp&7)==5) &&  rate==500)
		precomp=6;

	outp(FDC_DRS,((precomp&7)<<2) | getratecode(rate));

	// Program data rate
	outp(FDC_CCR, getratecode(rate));
	outp(FDC_DOR, DORsel[drive&3] | 0xC);

	initdma(FD_MODE_WRITE, size * nbsector);

	if(deleted)
		byte = FDC_CMD_WRITEDELETEDDATA;
	else
		byte = FDC_CMD_WRITE;

	byte = byte | 0xA0;
	if(density)
		byte = byte | 0x40;
	wrfdc(byte);

	byte = drive & 3;

	if( head )
		byte = byte | 0x4;
	wrfdc(byte);

	wrfdc(track);                               //  Cylinder
	wrfdc(head);                                //  Head
	wrfdc(index);                               //  Sector
	wrfdc(getsizecode(size));                   //  N
	wrfdc(index + (nbsector-1));                //  EOT
	wrfdc(gap3);                                //  GL
	wrfdc(0xFF);

	ret =   waitirq();
	//if(ret)
	fd_result(1);

	if((status[0]   &   0xC0)   ||  !ret)
	{

		hxc_printf(0,"Write failed : ST0:0x%.2x, ST1:0x%.2x, ST2:0x%.2x\n",status[0],status[1],status[2]);
		return  1;
	}


#ifdef  DBGMODE
	hxc_printf(0,"done\n");
#endif

	return  0;
}

void reset_drive(unsigned char drive)
{
	outp(FDC_DOR,0);

	outp(FDC_DRS,0);

	ticktimer=0;
	do
	{
	}while(ticktimer<18);

	outp(FDC_DOR,DORsel[drive&3] | 0xC);

	waitirq();
	fd_result(1);

	wrfdc(FDC_CMD_SPECIFY);
	wrfdc(0xDF);
	wrfdc(0x02);

	trackseek(0,1,0);
	calibratedrive(0);
}

void reset2(unsigned char drive)
{
	outp(FDC_DOR,0);

	outp(FDC_DRS,0);

	ticktimer=0;
	do
	{
	}while(ticktimer<1);
	outp(FDC_DOR,DORsel[drive&3] | 0xC);

	waitirq();
	fd_result(1);

	wrfdc(FDC_CMD_SPECIFY);
	wrfdc(0xDF);
	wrfdc(0x02);

	trackseek(0,1,0);
	calibratedrive(0);
}

void fdc_specify(unsigned char t)
{
	wrfdc(FDC_CMD_SPECIFY);
	wrfdc(0x5F);
	wrfdc(0x02);
}

int format_track(int drive,unsigned char density,int track,int head,int nbsector,int sectorsize,int interleave,unsigned char formatvalue,unsigned char precomp,int rate,int gap3)
{
	unsigned char byte,ret,i;

#ifdef DBGMODE
	hxc_printf(0,"Format track %d head %d drive %d ...\n",track);
#endif

	intflag=0;

	if(((precomp&7)==5) &&  rate==500)
		precomp=6;

	for( i = 0 ; i < nbsector ; i++ )
	{
		bufwr[(i*4) + 0] = track;
		bufwr[(i*4) + 1] = head;
		bufwr[(i*4) + 2] = i+1;
		bufwr[(i*4) + 3] = getsizecode(sectorsize);
	}

	outp(FDC_DRS,((precomp&7)<<2) | getratecode(rate));

	// Program data rate
	outp(FDC_CCR, getratecode(rate));
	outp(FDC_DOR, DORsel[drive&3] | 0xC);

	initdma(FD_MODE_WRITE, nbsector * 4);

	byte = FDC_CMD_FORMAT;
	if(density)
		byte = byte | 0x40;
	wrfdc(byte);                                //  CMD
	byte = drive & 3;
	if( head )
		byte = byte | 0x4;
	wrfdc(byte);                                //  Drive
	wrfdc(getsizecode(sectorsize));             //  N
	wrfdc(nbsector);                            //  Nbsector
	wrfdc(gap3);                                //  gap3
	wrfdc(formatvalue);                         //  Format value

	ret =   waitirq();
	//if(ret)
	fd_result(1);

	if((status[0]   &   0xC0)   ||  !ret)
	{
		hxc_printf(0,"Format failed : ST0:0x%.2x, ST1:0x%.2x, ST2:0x%.2x\n",status[0],status[1],status[2]);
		return  1;
	}

#ifdef  DBGMODE
	hxc_printf(0,"done\n");
#endif

	return  0;
}

int chs_biosdisk(int cmd, int drive, int head, int track,int sector, int nsects, void *buf)
{
	unsigned tries, err;
	struct SREGS sregs;
	union  REGS  regs;

	/* set up registers for INT 13h AH=02/03h */
	sregs.es  = FP_SEG(buf);
	regs.h.dh = head;
	regs.h.dl = drive;
	regs.h.ch = track;
	regs.h.cl = ((track >> 2) & 0xC0) | sector;
	regs.x.bx = FP_OFF(buf);

	/* make 3 attempts */
	for(tries = 1; tries != 0; tries--)
	{
		if(cmd == _DISK_RESET)
			regs.h.ah = 0x00;
		else if(cmd == _DISK_STATUS)
			regs.h.ah = 0x01;
		else if(cmd == _DISK_READ)
			regs.h.ah = 0x02;
		else if(cmd == _DISK_WRITE)
			regs.h.ah = 0x03;
		else if(cmd == _DISK_VERIFY)
			regs.h.ah = 0x04;
		else if(cmd == _DISK_FORMAT)
			regs.h.ah = 0x05;
		else
			return 1; /* invalid command */

		regs.h.al = nsects;
		int86x(0x13, &regs, &regs, &sregs);
		err = regs.h.ah;
		if(err == 0)
			return 0;

		/* reset disk */
		regs.h.ah = 0;
		int86x(0x13,&regs,&regs,&sregs);
	}

	hxc_printf(0,"chs_biosdisk: error 0x%02X\n", err);

	return err;
}
