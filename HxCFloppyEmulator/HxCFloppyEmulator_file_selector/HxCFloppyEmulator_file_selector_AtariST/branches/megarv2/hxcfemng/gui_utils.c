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
#include "graphx/data_bmp_font8x8_bmp.h"
#include "graphx/data_bmp_sdhxcfelogo_bmp.h"

#include "gui_utils.h"
#include "atari_hw.h"
#include "assembly.h"

#include "conf.h"

unsigned char * screen_addr;
unsigned char * screen_buffer_backup;
unsigned char screen_backup_isUsed;
unsigned char color;
unsigned char highresmode;
static short  _oldrez = 0xffff;
static UWORD  _business = 0;

unsigned char  NUMBER_OF_FILE_ON_DISPLAY;/* 19-5 //19 -240 */

unsigned short SCREEN_YRESOL;				/* screen X resolution (pixels) */
unsigned short SCREEN_XRESOL;               /* screen Y resolution (pixels) */
unsigned short LINE_BYTES;                  /* number of bytes per line     */
unsigned short LINE_WORDS;                  /* number of words per line     */
unsigned short LINE_CHARS;                  /* number of 8x8 chars per line */
unsigned short NB_PLANES;                   /* number of planes (1:2 colors */
                                            /*  4:16 colors, 8: 256 colors) */
unsigned short CHUNK_WORDS;                 /* number of words for a 16-    */
                                            /* pixel chunk =2*NB_PLANES     */
unsigned short PLANES_ALIGNDEC;             /* number of left shifts to
                                               transform nbChucks to Bytes  */
unsigned short STATUSL_YPOS;                /* status line y position */

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

static unsigned short colortable[] = {
								0x300, 0xEEE, 0x00f, 0xee4, // b ok blanc sur rouge foncé (nice)
								0x777, 0x300, 0x00f, 0x5f5, // w noir sur blanc, select vert (nice)
								0x002, 0xeee, 0x226, 0x567, // b ok blanc sur bleu nuit (nice)
								0xFFF, 0x343, 0x00f, 0x0f0, // w ok vert sombre sur blanc, select vert
								0x000, 0x00F, 0x222, 0xdd1, // b ok bleu sur noir
								0x000, 0xFFF, 0x00f, 0x3f3, // b ok blanc sur noir, select vert
								0x303, 0xFFF, 0x00f, 0xee4, // w ok blanc sur mauve
								0x030, 0xFFF, 0x00f, 0x0f0, // b ok vert
								0x999, 0x000, 0x999, 0x333, // w ok gris sombre
								0x330, 0xFFF, 0x77f, 0xcc0, // b ok caca d'oie
								0xF33, 0xFFF, 0x777, 0xe11, // w ok blanc sur rose et rouge
								0x000, 0xF00, 0x003, 0xd00, // b ok rouge sur noir
								0xF0F, 0xFFF, 0x000, 0x44f, // w ok violet vif
								0x000, 0x0E0, 0x00f, 0x0f0, // b ok vert sur noir
								0xFFF, 0x0F0, 0x4c4, 0x0f0, // w ok vert sur blanc
								0x004, 0xFFF, 0x00e, 0x5f5, // b ok blanc sur bleu marine

								0x036, 0xFFF, 0x00f, 0x0f0, // b
								0x444, 0x037, 0x00f, 0x0f0, // b
								0x000, 0xFF0, 0x00f, 0x0f0, // b
								0x404, 0x743, 0x00f, 0x0f0, // b
								0xFFF, 0x700, 0x00f, 0x0f0, // w
								0x000, 0x222, 0x00f, 0x0c0, // b
								0x000, 0x333, 0x00f, 0x0d0, // b
								0x000, 0x444, 0x00f, 0x0e0, // b
								0x000, 0x555, 0x00f, 0x0f0, // b
								0x000, 0x666, 0x00f, 0x0f0, // b
								0x000, 0x777, 0x00f, 0x0f0, // b
								0x222, 0x000, 0x00f, 0x0c0, // b
								0x333, 0x000, 0x00f, 0x0d0, // w
								0x444, 0x000, 0x00f, 0x0e0, // b
								0x555, 0x000, 0x00f, 0x0f0, // w
								0x666, 0x000, 0x00f, 0x0f0, // b

};


void display_sprite(unsigned char * membuffer, bmaptype * sprite,unsigned short x, unsigned short y)
{
	unsigned short i,j,k;
	unsigned short *ptr_src;
	unsigned short *ptr_dst;
	ULONG          base_offset, l;

	ptr_dst=(unsigned short*)membuffer;
	ptr_src=(unsigned short*)&sprite->data[0];

	k=0;

	base_offset=( ((ULONG) y*LINE_BYTES) + ((x>>4)<<PLANES_ALIGNDEC) )/2;
	for(j=0;j<(sprite->Ysize);j++)
	{
		l = base_offset;
		for (i=0; i<(sprite->Xsize>>4); i++)
		{
			ptr_dst[l]=ptr_src[k];
			l += NB_PLANES;
			k++;
		}
		base_offset += LINE_WORDS;
	}
}


void print_char8x8(unsigned char * membuffer, unsigned short x, unsigned short y,unsigned char c)
{
	bmaptype * font;
	unsigned short j,k;
	unsigned char *ptr_src;
	unsigned char *ptr_dst;
	ULONG base_offset;

	font = bitmap_font8x8_bmp;

	if (0 == membuffer) {
		ptr_dst = screen_addr;
	} else {
		ptr_dst=(unsigned char*)membuffer;
	}

	ptr_src=(unsigned char*)&font->data[0];

	k=((c>>4)*(8*8*2))+(c&0xF);

	base_offset=((ULONG) y*LINE_BYTES) + ((x>>4)<<PLANES_ALIGNDEC) + ((x&8)==8);
	// in a 16-pixel chunk, there are 2 8-pixel chars, hence the x&8==8

	for(j=0;j<8;j++)
	{
		ptr_dst[base_offset] = ptr_src[k];
		k=k+(16);
		base_offset += LINE_BYTES;
	}
}


void print_str(unsigned char * membuffer,char * buf,unsigned short x_pos,unsigned short y_pos)
{
	unsigned short i;
	i=0;

	while(buf[i])
	{
		if(x_pos<=(SCREEN_XRESOL-8))
		{
			print_char8x8(membuffer, x_pos, y_pos, buf[i]);
			x_pos=x_pos+8;
		}
		i++;
	}
}

/**
 * printf a string.
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
			print_str(screen_addr,temp_buffer,x_pos,y_pos);
		break;
		case 1:
			print_str(screen_addr,temp_buffer,(SCREEN_XRESOL-(strlen(temp_buffer)*8))/2,y_pos);
		break;
		case 2:
			print_str(screen_addr,temp_buffer,(SCREEN_XRESOL-(strlen(temp_buffer)*8)),y_pos);
		break;
	}

	va_end( marker );
}


/**
 * Draw / remove a horizontal line
 * (only the first bitplane)
 */
void h_line(unsigned short y_pos, unsigned short val)
{
	UWORD * ptr_dst;
	UWORD i;

	ptr_dst=(UWORD *) screen_addr;
	ptr_dst += (ULONG) LINE_WORDS * y_pos;

	for(i=0; i<LINE_WORDS; i+=NB_PLANES)
	{
		*(ptr_dst) = val;
		ptr_dst += NB_PLANES;
	}

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




/**
 * Clear the screen (all bitplanes)
 * @param integer add number of additional lines to clear. Allow to clear the status bar
 */
void clear_list(unsigned char add)
{
	unsigned char * ptr_dst;

	ptr_dst =  (UBYTE *)screen_addr;
	ptr_dst += (ULONG) LINE_BYTES * FILELIST_Y_POS;
	//memset(ptr_dst, 0, (ULONG) ((UWORD)(LINE_BYTES)*(UWORD)((NUMBER_OF_FILE_ON_DISPLAY+add)<<3)));
	memsetword(ptr_dst, 0, (ULONG) (UWORD)(LINE_BYTES)*(UWORD)((NUMBER_OF_FILE_ON_DISPLAY+add)<<2));

}



/**
 * invert all planes on a line of text of the file selector
 * @param integer linenumber number of the line of text to invert
 */
void invert_line(unsigned short linenumber)
{
	unsigned short i;
	unsigned char j;
	unsigned char  *ptr_dst;
	unsigned short *ptr_dst2;

	ptr_dst   = screen_addr;
	ptr_dst  += (ULONG) LINE_BYTES * (FILELIST_Y_POS + (linenumber<<3));

	ptr_dst2 = (unsigned short *)ptr_dst;

	for(j=0;j<8;j++)
	{
		for(i=0; i<LINE_WORDS; i+=1)
		{
			//*ptr_dst = (*ptr_dst ^ 0xFFFF);
			*ptr_dst2 = (*ptr_dst2 ^ 0xFFFF);
			ptr_dst2++;
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
		print_char8x8(screen_addr, ((SCREEN_XRESOL-str_size)/2)+i,     LINE_CHARS-8, 8);
	}
	print_char8x8(screen_addr,     ((SCREEN_XRESOL-str_size)/2)+(i-8), LINE_CHARS-8, 3);
	print_char8x8(screen_addr,     ((SCREEN_XRESOL-str_size)/2),       LINE_CHARS-8, 2);

	for(i=0;i< str_size;i=i+8)
	{
		print_char8x8(screen_addr, ((SCREEN_XRESOL-str_size)/2)+i,     LINE_CHARS, ' ');
	}

	print_str(screen_addr,temp_buffer,((SCREEN_XRESOL-str_size)/2)+(2*8),LINE_CHARS);
	print_char8x8(screen_addr,     ((SCREEN_XRESOL-str_size)/2)+(i-8), LINE_CHARS,   7);
	print_char8x8(screen_addr,     ((SCREEN_XRESOL-str_size)/2),       LINE_CHARS,   6);

	for(i=0;i< str_size;i=i+8)
	{
		print_char8x8(screen_addr, ((SCREEN_XRESOL-str_size)/2)+i,     LINE_CHARS+8, 9);
	}
	print_char8x8(screen_addr, ((SCREEN_XRESOL-str_size)/2)+(i-8),     LINE_CHARS+8, 5);
	print_char8x8(screen_addr, ((SCREEN_XRESOL-str_size)/2),           LINE_CHARS+8, 4);

	va_end( marker );
}


void display_welcome()
{
	int i;

	hxc_printf(1,0,0,"SDCard HxC Floppy Emulator Manager for Atari ST");
	h_line(8,0xFFFF) ;

	i=0;
	i = display_credits(i);

	redraw_statusl();

	// line just above the logos
	h_line(SCREEN_YRESOL-34,0xFFFF) ;

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
	hxc_printf(1,0,HELP_Y_POS+(i*8), "and Megar / Gilles Bouthenot");
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


void display_statusl(unsigned char mode, unsigned char clear, char * text, ...)
{
	char temp_buffer[256];

	va_list marker;
	va_start( marker, text );

	if (clear) {
		memsetword(screen_addr + STATUSL_YPOS*(LINE_WORDS<<1), 0, LINE_WORDS<<3);
	}

	vsnprintf(temp_buffer,256,text,marker);
	hxc_printf(mode, 0, STATUSL_YPOS, temp_buffer);
	va_end(marker);
}

void redraw_statusl()
{
	// line just above the statusbar
	h_line(SCREEN_YRESOL-48-24-2,0xFFFF) ;

	// line just under the statusbar
	h_line(SCREEN_YRESOL-48+2,0xFFFF) ;

	display_statusl(1, 0, ">>>Press HELP key for the function key list<<<");
}


void more_busy()
{
	_business++;
	hxc_printf(0,8*(LINE_CHARS-1),0,"%c",23);
}
void less_busy()
{
	if (_business) {
		_business--;
		if (!_business) {
			hxc_printf(0,8*(LINE_CHARS-1),0," ",23);
		}
	}
}


/**
 * Set the palette
 * @param int colorm the number of the palette, -1 to cycle, -2 to restore
 * @return int the new palette number
 */
unsigned char set_color_scheme(unsigned char colorm)
{
	unsigned short * palette;
	short tmpcolor;
	int i,j;
	int nbcols;
	static UWORD initialpalette[4] = {0xffff, 0xffff, 0xffff, 0xffff};

	if (0xff == colorm) {
		// cycle
		colorm = color+1;
		if ( colorm >= (sizeof(colortable))>>3 ) {
			// reset to first
			colorm = 0;
		}
	}
	if (0xfe == colorm) {
		// restore
		palette = initialpalette;
	} else {
		color = colorm;
		palette = &colortable[color<<2];
	}
	nbcols = 2<<(NB_PLANES-1);

	for (i=0; i<4 && i<nbcols; i++) {
		j = i;
		if (i>=2) {
			// the first two colors are always pal[0] and pal[1]
			// the last two colors may be pal[2] and pal[3] in 2 planes, or pal[14] and pal[15] in 4 planes
			j = nbcols - 4 + i;
		}
		tmpcolor = Setcolor(j, palette[i]);
		if (0xffff == initialpalette[i]) {
			initialpalette[i] = tmpcolor;
		}
	}

	return color;
}

void restore_display()
{
	set_color_scheme(0xfe);

	// Line-A : Showmouse
	__asm__("dc.w 0xa009");

	if (0xffff != _oldrez) {
		Setscreen((unsigned char *) -1, (unsigned char *) -1, _oldrez );
	}
}

void init_display()
{
	unsigned short k,i;

	screen_backup_isUsed = 0;

	highresmode=get_vid_mode();

	linea0();

	// Line-A : Hidemouse
	__asm__("dc.w 0xa00a");

	screen_addr = (unsigned char *) Physbase();

	screen_buffer_backup=(unsigned char*)malloc(8000L);

	if (V_X_MAX < 640) {
		/*Blitmode(1) */;
		_oldrez = Getrez();

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
	STATUSL_YPOS  = SCREEN_YRESOL-(48+20)+24;
	LINE_BYTES    = V_BYTES_LIN;
	LINE_WORDS    = V_BYTES_LIN/2;
	LINE_CHARS    = SCREEN_XRESOL/8;
	NB_PLANES     = __aline->_VPLANES;
	CHUNK_WORDS   = NB_PLANES<<1;

	NUMBER_OF_FILE_ON_DISPLAY = ((SCREEN_YRESOL-48-24-2) - (FILELIST_Y_POS+2)) / 8;

	for (i=NB_PLANES, k=0; i!=0; i>>=1, k++);
	PLANES_ALIGNDEC = k;

	if (1 == NB_PLANES) {
		highresmode = 1;
	} else {
		highresmode = 0;
	}

	// clear the screen
	memsetword(screen_addr, 0, (ULONG) SCREEN_YRESOL * LINE_WORDS);

	set_color_scheme(0);

	display_welcome();
}

