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

#ifdef __VBCC__
# include <tos.h>
#else
# include <mint/osbind.h>
# include <mint/linea.h>
# include "libc/snprintf/snprintf.h"
#endif

#include <time.h>
/* #include <vt52.h> */
#include <stdarg.h>


#include "graphx/data_bmp_hxc2001logo_bmp.h"
#include "graphx/data_bmp_font_bmp.h"
#include "graphx/data_bmp_font8x8_bmp.h"
#include "graphx/data_bmp_sdhxcfelogo_bmp.h"

#include "gui_utils.h"
#include "atari_hw.h"

#include "conf.h"

unsigned char * screen_addr;
unsigned char * screen_buffer_backup;
unsigned char screen_backup_isUsed;
unsigned char color;
unsigned char highresmode;

unsigned char  NUMBER_OF_FILE_ON_DISPLAY;/* 19-5 //19 -240 */

unsigned short SCREEN_YRESOL;				/* screen X resolution (pixels) */
unsigned short SCREEN_XRESOL;               /* screen Y resolution (pixels) */
unsigned short LINE_BYTES;                  /* number of bytes per line     */
unsigned short LINE_WORDS;                  /* number of words per line     */
unsigned short LINE_CHARS;                  /* number of 8x8 chars per line */
unsigned short NB_PLANES;                   /* number of planes (1:2 colors */
                                            /*  4:16 colors, 8: 256 colors) */
__LINEA *__aline;
__FONT  **__fonts;
short  (**__funcs) (void);


/*
-------+-----+-----------------------------------------------------+----------
       |     |                                BIT 11111198 76543210|
       |     |                                    543210           |
       |     |                     ST color value .....RRr .GGr.BBb|
       |     |                    STe color value ....rRRR gGGGbBBB|
$FF8240|word |Video palette register 0              Lowercase = LSB|R/W
    :  |  :  |  :      :       :     :                             | :
$FF825E|word |Video palette register 15                            |R/W
-------+-----+-----------------------------------------------------+----------
*/

static unsigned long colortable[] = {
								0x002, 0xFFF, 0x567, 0x226, // ok
								0x000, 0xFFF, 0x3f3, 0x00f,
								0xFFF, 0x000, 0x5f5, 0x00f, // ok
								0x030, 0xFFF, 0x0f0, 0x00f,
								0x300, 0xFFF, 0xff4, 0x00f, // ok
								0x303, 0xFFF, 0xee4, 0x00f, // ok
								0x999, 0x000, 0x333, 0x999, // ok
								0xFFF, 0x343, 0x0f0, 0x00f, // ok
								0xF33, 0xFFF, 0xe11, 0x777, // ok
								0xF0F, 0xFFF, 0x44f, 0x000, // ok
								0xFFF, 0x0F0, 0x0f0, 0x4c4, // ok
								0x330, 0xFFF, 0xcc0, 0x77f, // ok caca d'oie
								0x000, 0xF00, 0xd00, 0x003, // ok rouge sur noir
								0x000, 0x0F0, 0x0f0, 0x00f, // ok vert sur noir
								0x000, 0x00F, 0xdd1, 0x222, // ok bleu sur noir
								0x004, 0xFFF, 0x5f5, 0x00e, // ok

								0x036, 0xFFF, 0x0f0, 0x00f,
								0x444, 0x037, 0x0f0, 0x00f,
								0x000, 0xFF0, 0x0f0, 0x00f,
								0x404, 0x743, 0x0f0, 0x00f,
								0xFFF, 0x700, 0x0f0, 0x00f,
								0x000, 0x222, 0x0f0, 0x00f,
								0x000, 0x333, 0x0f0, 0x00f,
								0x000, 0x444, 0x0f0, 0x00f,
								0x000, 0x555, 0x0f0, 0x00f,
								0x000, 0x666, 0x0f0, 0x00f,
								0x000, 0x777, 0x0f0, 0x00f,
								0x222, 0x000, 0x0f0, 0x00f,
								0x333, 0x000, 0x0f0, 0x00f,
								0x444, 0x000, 0x0f0, 0x00f,
								0x555, 0x000, 0x0f0, 0x00f,
								0x666, 0x000, 0x0f0, 0x00f,

};


void display_sprite(unsigned char * membuffer, bmaptype * sprite,unsigned short x, unsigned short y)
{
	unsigned short i,j,k,x_offset,p;
	unsigned short *ptr_src;
	unsigned short *ptr_dst;
	ULONG          base_offset, l;

	ptr_dst=(unsigned short*)membuffer;
	ptr_src=(unsigned short*)&sprite->data[0];

	k=0;
	l=0;

	if(highresmode)
	{
		base_offset=(((ULONG) y*LINE_BYTES)+ (((x>>3)&(~0x1))))/2;
		for(j=0;j<(sprite->Ysize);j++)
		{
			l=(ULONG) base_offset +((ULONG) LINE_WORDS*j);
			for(i=0;i<(sprite->Xsize/16);i++)
			{
				ptr_dst[(ULONG) l]=ptr_src[k];
				l++;
				k++;
			}
		}
	}
	else
	{
		base_offset=(((ULONG) y*LINE_BYTES)+ (((x>>2)&(~0x3))))/2;
		for(j=0;j<(sprite->Ysize);j++)
		{
			l=base_offset +(LINE_WORDS*j);
			for(i=0;i<(sprite->Xsize/16);i++)
			{
				ptr_dst[l]=ptr_src[k];
				l++;
				ptr_dst[l]=ptr_src[k];
				l++;
				k++;
			}
		}
	}

}

void print_char(unsigned char * membuffer, bmaptype * font,unsigned short x, unsigned short y,unsigned char c)
{
	unsigned short j,k,c1;
	unsigned short *ptr_src;
	unsigned short *ptr_dst;
	ULONG l;

	ptr_dst=(unsigned short*)membuffer;
	ptr_src=(unsigned short*)&font->data[0];

	if(highresmode)
	{
		x=(x>>3) & (~0x1);

		l=((ULONG) y*LINE_WORDS)+ x;
		k=((c>>4)*(16*16))+(c&0xF);
		for(j=0;j<16;j++)
		{
			ptr_dst[l]  =ptr_src[k];
			k=k+(16);
			l=l+((ULONG) LINE_WORDS);
		}

	}
	else
	{
		x=(x>>3) & (~0x1);

		l=((ULONG) y*LINE_WORDS)+ x;
		k=((c>>4)*(16*16))+(c&0xF);
		for(j=0;j<16;j++)
		{
			ptr_dst[l]  =ptr_src[k];
			ptr_dst[l+1]=ptr_src[k];
			k=k+(16);
			l=l+((ULONG) LINE_WORDS);
		}
	}

}

void print_char8x8_mr(unsigned char * membuffer, bmaptype * font,unsigned short x, unsigned short y,unsigned char c)
{
	unsigned short j,k,c1;
	unsigned char *ptr_src;
	unsigned char *ptr_dst;
	ULONG l;

	ptr_dst=(unsigned char*)membuffer;
	ptr_src=(unsigned char*)&font->data[0];

	x=x>>3;
	x=((x&(~0x1))<<1)+(x&1);/*  0 1   2 3 */
	l=((ULONG) y*LINE_BYTES)+ (x);
	k=((c>>4)*(8*8*2))+(c&0xF);
	for(j=0;j<8;j++)
	{
		ptr_dst[l]  =ptr_src[k];
		ptr_dst[l+2]=ptr_src[k];
		k=k+(16);
		l=l+(LINE_BYTES);
	}

}

void print_char8x8_hr(unsigned char * membuffer, bmaptype * font,unsigned short x, unsigned short y,unsigned char c)
{
	unsigned short j,k,c1;
	unsigned char *ptr_src;
	unsigned char *ptr_dst;
	ULONG l;

	ptr_dst=(unsigned char*)membuffer;
	ptr_src=(unsigned char*)&font->data[0];

	x=x>>3;
	x=((x&(~0x1))<<0)+(x&1);/*  0 1   2 3 */
	l=((ULONG) y*LINE_BYTES)+ (x);
	k=((c>>4)*(8*8*2))+(c&0xF);
	for(j=0;j<8;j++)
	{
		ptr_dst[l] = ptr_src[k];
		k=k+(16);
		l=l+(LINE_BYTES);
	}
}

void print_char8x8(unsigned char * membuffer, bmaptype * font,unsigned short x, unsigned short y,unsigned char c)
{
	if(highresmode)
		print_char8x8_hr(membuffer,font,x,y,c);
	else
		print_char8x8_mr(membuffer,font,x,y,c);
}

void print_str(unsigned char * membuffer,char * buf,unsigned short x_pos,unsigned short y_pos,unsigned char font)
{
	unsigned short i;
	i=0;

	switch(font)
	{
	case 8:
		while(buf[i])
		{
			if(x_pos<=(SCREEN_XRESOL-8))
			{
				print_char8x8(membuffer,bitmap_font8x8_bmp,x_pos,y_pos,buf[i]);
				x_pos=x_pos+8;
			}
			i++;
		}
	break;
	case 16:
		while(buf[i])
		{
			if(x_pos<=(SCREEN_XRESOL-16))
			{
				print_char(membuffer,bitmap_font_bmp,x_pos,y_pos,buf[i]);
				x_pos=x_pos+16;
			}
			i++;
		}
	break;
	}
}

/**
 * printf a string. Add 4 to mode to use the 16x16 font
 * @param int    mode  0:normal(use x,y)  1:align=center (use y)  2:align=right (use y)
 * @param int    x_pos  (in pixel)
 * @param int    y_pos  (in pixel) of the top of the char
 * @param string chaine
 * @param ...
 */
void hxc_printf(unsigned char mode,unsigned short x_pos,unsigned short y_pos,char * chaine, ...)
{
	char temp_buffer[1024];

	va_list marker;
	va_start( marker, chaine );

	vsnprintf(temp_buffer,1024,chaine,marker);
	switch(mode)
	{
		case 0:
			print_str(screen_addr,temp_buffer,x_pos,y_pos,8);
		break;
		case 1:
			print_str(screen_addr,temp_buffer,(SCREEN_XRESOL-(strlen(temp_buffer)*8))/2,y_pos,8);
		break;
		case 2:
			print_str(screen_addr,temp_buffer,(SCREEN_XRESOL-(strlen(temp_buffer)*8)),y_pos,8);
		break;
		case 4:
			print_str(screen_addr,temp_buffer,x_pos,y_pos,16);
		break;
		case 5:
			print_str(screen_addr,temp_buffer,(SCREEN_XRESOL-(strlen(temp_buffer)*16))/2,y_pos,16);
		break;
		case 6:
			print_str(screen_addr,temp_buffer,(SCREEN_XRESOL-(strlen(temp_buffer)*16)),y_pos,16);
		break;
	}

	va_end( marker );
}


void h_line(unsigned short y_pos,unsigned char val)
{
	UBYTE * ptr_dst;
	unsigned short i,s;
	unsigned long ptroffset;

	s=LINE_BYTES;

	ptr_dst=(UBYTE *) screen_addr;
	ptr_dst += (ULONG) s * y_pos;

	memset(ptr_dst, val, s);
}

#if(0)
void box(unsigned short x_p1,unsigned short y_p1,unsigned short x_p2,unsigned short y_p2,unsigned short fillval,unsigned char fill)
{
	unsigned short *ptr_dst;
	unsigned short i,j,ptroffset,x_size;


	ptr_dst=(unsigned short*)screen_addr;

	if(highresmode)
	{
		x_size=((x_p2-x_p1)/16);

		for(j=0;j<(y_p2-y_p1);j++)
		{
			for(i=0;i<x_size;i++)
			{
				ptr_dst[ptroffset+i]=fillval;
			}
			ptroffset=LINE_WORDS* (y_p1+j);
		}

	}
	else
	{
		x_size=((x_p2-x_p1)/16)*2;

		for(j=0;j<(y_p2-y_p1);j++)
		{
			for(i=0;i<x_size;i++)
			{
				ptr_dst[ptroffset+i]=fillval;
			}
			ptroffset=LINE_WORDS* (y_p1+j);
		}
	}
}
#endif

void clear_line(unsigned short y_pos, unsigned short val)
{
	unsigned short i;
	for(i=0; i<8; i++)
	{
		h_line(y_pos+i, val);
	}
}

void invert_line(unsigned short y_pos)
{
	unsigned char i,j;
	unsigned short *ptr_dst;
	unsigned short ptroffset;

	ptr_dst=(unsigned short*)screen_addr;

	if(highresmode)
	{
		for(j=0;j<8;j++)
		{
			ptroffset=LINE_WORDS* (y_pos+j);

			for(i=0;i<LINE_WORDS;i++)
			{
				ptr_dst[ptroffset+i]=ptr_dst[ptroffset+i]^0xFFFF;
			}
		}
	}
	else
	{
		for(j=0;j<8;j++)
		{
			ptroffset=LINE_WORDS* (y_pos+j);

			for(i=0;i<LINE_WORDS;i+=2)
			{
				ptr_dst[ptroffset+i]=ptr_dst[ptroffset+i]^0xFFFF;
			}
		}
	}
}

void get_char_restore_box()
{
	get_char();
	restore_box();
}

void restore_box()
{
	if (screen_backup_isUsed) {
		if (1 == screen_backup_isUsed) {
			memcpy(&screen_addr[LINE_BYTES*70], screen_buffer_backup, 8000L);
		}
		screen_backup_isUsed--;
	}
}

void hxc_printf_box(unsigned char mode,char * chaine, ...)
{
	char temp_buffer[1024];
	int str_size;
	unsigned short i;

	if (!screen_backup_isUsed) {
		memcpy(screen_buffer_backup,&screen_addr[LINE_BYTES*70], 8000L);
	}
	screen_backup_isUsed++;

	va_list marker;
	va_start( marker, chaine );

	vsnprintf(temp_buffer,1024,chaine,marker);

	str_size=strlen(temp_buffer) * 8;
	str_size=str_size+(4*8);

	for(i=0;i< str_size;i=i+8)
	{
		print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2)+i,LINE_CHARS-8,8);
	}
	print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2)+(i-8),LINE_CHARS-8,3);
	print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2),LINE_CHARS-8,2);

	for(i=0;i< str_size;i=i+8)
	{
		print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2)+i,LINE_CHARS,' ');
	}

	print_str(screen_addr,temp_buffer,((SCREEN_XRESOL-str_size)/2)+(2*8),LINE_CHARS,8);
	print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2)+(i-8),LINE_CHARS,7);
	print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2),LINE_CHARS,6);

	for(i=0;i< str_size;i=i+8)
	{
		print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2)+i,LINE_CHARS+8,9);
	}
	print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2)+(i-8),LINE_CHARS+8,5);
	print_char8x8(screen_addr,bitmap_font8x8_bmp,((SCREEN_XRESOL-str_size)/2),LINE_CHARS+8,4);

	va_end( marker );
}


void display_welcome()
{
	int i;

	hxc_printf(1,0,0,"SDCard HxC Floppy Emulator Manager for Atari ST");
	h_line(8,0xFF) ;

	i=0;
	i = display_credits(i);

	display_status();

	// line just above the logos
	h_line(SCREEN_YRESOL-34,0xFF) ;

	hxc_printf(0,0,SCREEN_YRESOL-(8*1),"Ver %s",VERSIONCODE);
	display_sprite(screen_addr, bitmap_sdhxcfelogo_bmp,(SCREEN_XRESOL-bitmap_sdhxcfelogo_bmp->Xsize)/2, (SCREEN_YRESOL-bitmap_sdhxcfelogo_bmp->Ysize));
	display_sprite(screen_addr, bitmap_hxc2001logo_bmp,(SCREEN_XRESOL-bitmap_hxc2001logo_bmp->Xsize), (SCREEN_YRESOL-bitmap_hxc2001logo_bmp->Ysize));

}

int display_credits(int i)
{
	hxc_printf(1,0,HELP_Y_POS+(i*8), "SDCard HxC Floppy Emulator file selector for Atari ST");
	i++;
	hxc_printf(1,0,HELP_Y_POS+(i*8), "(c) 2006-2012 HxC2001 / Jean-Francois DEL NERO");
	i++;
	hxc_printf(1,0,HELP_Y_POS+(i*8), "Fast Loader by Gilles Bouthenot");
	i++;
	hxc_printf(1,0,HELP_Y_POS+(i*8), "Check for updates on :");
	i++;
	hxc_printf(1,0,HELP_Y_POS+(i*8), "http://www.hxc2001.com/");
	i++;
	hxc_printf(1,0,HELP_Y_POS+(i*8), "Email : hxc2001@hxc2001.com");
	i++;
	hxc_printf(1,0,HELP_Y_POS+(i*8), "V%s - %s",VERSIONCODE,DATECODE);
	i++;

	return i;
}

void display_status()
{
	// line just above thestatusbar
	h_line(SCREEN_YRESOL-(48+20)+24-2,0xFF) ;

	// line just under the statusbar
	h_line(SCREEN_YRESOL-((48+(3*8))+2),0xFF) ;

	hxc_printf(1,0,SCREEN_YRESOL-(48+20)+24,">>>Press HELP key for the function key list<<<");
}

void initpal()
{
	volatile unsigned short * ptr;

	ptr=(unsigned short *)0xFF8240;
	*ptr=colortable[((color&0x1F)*4)+0];
	ptr=(unsigned short *)0xFF8242;
	*ptr=colortable[((color&0x1F)*4)+2];
	ptr=(unsigned short *)0xFF8244;
	*ptr=colortable[((color&0x1F)*4)+3];
	ptr=(unsigned short *)0xFF8246;
	*ptr=colortable[((color&0x1F)*4)+1];

}

void set_color_scheme(unsigned char colorm)
{
	color=colorm;
	my_Supexec((LONG *) initpal);
}


void init_display()
{
	unsigned short loop,yr;
	unsigned short k,i;

	NUMBER_OF_FILE_ON_DISPLAY=19-5;/* 19-5 //19 -240 */
	screen_backup_isUsed = 0;

	highresmode=get_vid_mode();

	linea0();

	// Line-A : Hidemouse
	__asm__("dc.w 0xa00a");

	screen_addr = (unsigned char *) Physbase();

	screen_buffer_backup=(unsigned char*)malloc(8000L);

	if (V_X_MAX < 640) {
		/*Blitmode(1) */;
		if(highresmode)
		{
			Setscreen((unsigned char *) -1, (unsigned char *) -1, 2 );
		}
		else
		{
			Setscreen((unsigned char *) -1, (unsigned char *) -1, 1 );
		}
	}

	SCREEN_XRESOL = V_X_MAX;
	SCREEN_YRESOL = V_Y_MAX;
	LINE_BYTES    = V_BYTES_LIN;
	LINE_WORDS    = V_BYTES_LIN/2;
	LINE_CHARS    = SCREEN_XRESOL/8;
	NB_PLANES     = __aline->_VPLANES;

	if (1 == NB_PLANES) {
		highresmode = 1;
	} else {
		highresmode = 0;
	}

	// clear the screen
	memset(screen_addr, 0, (ULONG) SCREEN_YRESOL * LINE_BYTES);


	color=0;
	my_Supexec((LONG *) initpal);

	display_welcome();
}

