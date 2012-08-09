/*
//
// Copyright (C) 2009, 2010, 2011, 2012 Jean-François DEL NERO
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


//#define TV_DEBUG



#include <string.h>

#include "atari_hw.h"
#include "hxcfeda.h"
#include "gui_utils.h"
#include "fat32/fat_filelib.h"
#include "libc/snprintf/snprintf.h"

#ifdef TV_DEBUG
	static int debug_line=0;
#endif


#include "conf.h"



//
// Externs
//
extern unsigned char NUMBER_OF_FILE_ON_DISPLAY;
extern unsigned short SCREEN_XRESOL;
extern unsigned short SCREEN_YRESOL;



void viewer(char *currentPath, DirectoryEntry *gfl_dirEntLSB_ptr, char fHex)
{
	char buffer[512];
	int bufUsed   = 0;
	int bufOffset = 0;
	FL_FILE *file;
	char filename[2*LFN_MAX_SIZE+2];

	strcpy(filename, currentPath);
	filename[strlen(filename)+1] = '\0';
	filename[strlen(filename)]   = '/';
	strcat(filename, (const char *)gfl_dirEntLSB_ptr->longName);

	file = fl_fopen(filename, "r");
	if (!file) {
		get_char();
		return;
	}


	UWORD curX, curY, i;
	unsigned char curChar;
	unsigned char c;
	int bytesRead;

	// clear the screen
	clear_list(5);
	curX = 0; curY = 0;

	if (fHex) {
		// hex viewer

		char *lineStart = buffer + 18;
		char *lineHex   = buffer + 18 + 11;
		char *lineAscii = buffer + 18 + 63;
		unsigned long offset = 0;

		do {
			// fill the buffer
			bytesRead = fl_fread(buffer, 1, 16 , file);
			memset(lineHex, ' ', 79-11);

			if (bytesRead>0) {
				plp_snprintf(lineStart, 80, "%08lx : ", (unsigned long)offset);
// "12345678 : 00 11 22 33 44 55 66 77 88 99 aa bb cc dd ee ff     0123456789abcdef"

				for (curX=0,i=0; curX<16 && curX<bytesRead; curX++, i++) {
					curChar = buffer[curX];
					lineAscii[curX] = (curChar) ? (curChar) : (' ');
					c = curChar>>4;
					c += (c<10) ? ('0') : ('a'-10);
					lineHex[i++] = c;

					c = curChar&0xf;
					c += (c<10) ? ('0') : ('a'-10);
					lineHex[i++] = c;
				}
				lineAscii[curX] = '\0';

				print_str(0, lineStart , 0, curY + VIEWER_Y_POS);
				curY += 8;

				if ( curY >= ((NUMBER_OF_FILE_ON_DISPLAY+5)<<3) ) {
					get_char();
					clear_list(5);
					curX = 0;
					curY = 0;
				}
			}
			offset += 16;
		} while (bytesRead>0);

	} else {
		// text view
		do
		{
			// discard the already shown bytes
			memcpy(buffer, buffer + bufOffset, bufUsed - bufOffset);
			bufUsed -= bufOffset;
			bufOffset = 0;

			// fill the buffer
			bytesRead = fl_fread(buffer + bufUsed, 1, 512 - bufUsed , file);

			if (bytesRead>=0) {
				bufUsed += bytesRead;

				do {
					curChar = buffer[bufOffset];

					if (10 == curChar) {
						curX = 0xffff;
						if (13 == buffer[bufOffset+1]) {
							// LFCR
							bufOffset++;
						}
					} else if (13 == curChar) {
						curX = 0xffff;
						if (10 == buffer[bufOffset+1]) {
							// CRLF
							bufOffset++;
						}
					} else {
						print_char8x8(0, curX, VIEWER_Y_POS + curY, curChar);
						curX +=8;
					}

					if (curX >= SCREEN_XRESOL) {
						curX = 0;
						curY += 8;
						if ( curY >= ((NUMBER_OF_FILE_ON_DISPLAY+5)<<3) ) {
							get_char();
							clear_list(5);
							curX = 0;
							curY = 0;
						}
					}
					bufOffset++;
				} while (    (bytesRead>0 && bufOffset<(bufUsed-1))         // reserve 1 char for CRLF handling
				          || ((0xffff == bytesRead) && bufOffset<(bufUsed))	// last chunk
				        );	// -1 for CRLF handling
			}
		} while (bytesRead>0);
	} // text viewer

	get_char();
	fl_fclose(file);
}


