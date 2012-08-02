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

#include <string.h>


#include "gui_utils.h"
#include "cfg_file.h"
#include "hxcfeda.h"
#include "fat_access.h"
#include "dir.h"
#include "filelist.h"


#include "atari_hw.h"

#include "conf.h"


//
// Externs
//
extern unsigned char NUMBER_OF_FILE_ON_DISPLAY;
extern unsigned short SCREEN_XRESOL;
extern unsigned short SCREEN_YRESOL;



//
// Static variables
//
static UWORD _currentPage = 0xffff;
static UWORD _selectorPos;
static UWORD _selectorToEntry = 0xffff;
static signed short _invertedLine;
static UBYTE _isLastPage;
static UWORD _nbPages;


// exported variables:
DirectoryEntry   _dirEntLSB;
DirectoryEntry * gfl_dirEntLSB_ptr = &_dirEntLSB;
UWORD gfl_filelistCurrentPage_tab[MAXFILESPERPAGE];






void gfl_jumpToFile(UWORD askedFile)
{
	_currentPage = dir_getPageForEntry(askedFile);
	_selectorToEntry = askedFile;
}




void gfl_showFilesForPage(UBYTE fRepaginate, UBYTE fForceRedrawAll)
{
	static UWORD _oldPage = 0xffff;
	UWORD curFile;
	UWORD i;
	UWORD y_pos;
	UBYTE fForceRedraw=0;

	if (fRepaginate) {
		dir_paginateAndPrefillCurrentPage();
		_nbPages = dir_getNbPages();
		_selectorPos = 0;
		_currentPage = 0;
		fForceRedraw = 1;
	}

	if ( (_oldPage != _currentPage) || fForceRedraw || fForceRedrawAll )
	{
		unsigned char fCached;
		struct fs_dir_ent dir_entry;

		_oldPage = _currentPage;

		if (fForceRedrawAll) {
			clear_list(5);
		} else {
			clear_list(0);
		}

		_invertedLine = -1;	// the screen has been clear: no need to de-invert the line

		// page number
		hxc_printf(0,PAGE_X_POS, PAGE_Y_POS, "Page %d of %d      ", _currentPage+1, _nbPages);

		// filter
		hxc_printf(0, FILTER_X_POS, FILTER_Y_POS,"Filter (F1): [%s]", dir_getFilter());

		y_pos=FILELIST_Y_POS;

		// start at the first file of the directory
		curFile = dir_getFirstFileForPage(_currentPage);
		if (curFile == gfl_filelistCurrentPage_tab[0])
		{
			fCached = 1;
		}
		else
		{
			// reset the files for this page
			memset(&gfl_filelistCurrentPage_tab[0], 0xff, 2*MAXFILESPERPAGE);
			fCached = 0;
		}

		fli_getDirEntryMSB(curFile, &dir_entry);

		for (i=0; i<NUMBER_OF_FILE_ON_DISPLAY; )
		{
			hxc_printf(0,0,y_pos," %c%s", (dir_entry.is_dir)?(10):(' '), dir_entry.filename);
			y_pos=y_pos+8;

			gfl_filelistCurrentPage_tab[i] = curFile;
			i++;

			if ( fCached )
			{
				// filelist has already been processed
				curFile = gfl_filelistCurrentPage_tab[i];
				if (!fli_getDirEntryMSB(curFile, &dir_entry)) {
					i = NUMBER_OF_FILE_ON_DISPLAY;
					break;
				}
			}
			else
			{
				// find the next file to display
				do {
					curFile++;
					if (!fli_getDirEntryMSB(curFile, &dir_entry)) {
						i = NUMBER_OF_FILE_ON_DISPLAY;
						break;
					}
				} while (!dir_filter(&dir_entry));
			}
		}

		_invertedLine = -1;
		_isLastPage = ((_currentPage+1) == _nbPages);
	} // if ( (newPage != _currentPage) || (0xffff == _currentPage) )



	if (0xffff != _selectorToEntry)
	{
		// the page just has been shown, so it is now cached
		for (i=0; i<NUMBER_OF_FILE_ON_DISPLAY; i++)
		{
			curFile = gfl_filelistCurrentPage_tab[i];
			if (curFile >= _selectorToEntry) {
				// this is the asked entry (or >, to handle filter)
				_selectorPos = i;
				break;
			}
		}
		_selectorToEntry = 0xffff;
	}


	if (_invertedLine != _selectorPos) {
		// reset the inverted line
		if (_invertedLine>=0) {
			invert_line(_invertedLine);
			hxc_printf(0, 0, FILELIST_Y_POS+(_invertedLine*8), " ");
		}

		// set the inverted line (only if it has changed)
		_invertedLine = _selectorPos;
		hxc_printf(0, 0, FILELIST_Y_POS+(_selectorPos*8), ">");
		invert_line(_selectorPos);
	}

	fli_getDirEntryLSB(gfl_filelistCurrentPage_tab[_selectorPos], gfl_dirEntLSB_ptr);
}




long gfl_mainloop()
{
	long key, key_ret;
	key_ret = 0;
	key = wait_function_key();

	switch((UWORD) (key>>16))
	{
		case 0x48: /* UP */
			if (_selectorPos > 0) {
				_selectorPos--;
				break;
			}

			// top line
			if (0==_currentPage) {
				// first page: stuck
				break;
			}

			// top line not on the first page
			_selectorPos=NUMBER_OF_FILE_ON_DISPLAY-1;
			_currentPage--;
			break;

		case 0x50: /* Down */
			if ( (_selectorPos+1)==NUMBER_OF_FILE_ON_DISPLAY ) {
				// last line of display
				if (!_isLastPage) {
					_currentPage++;
					_selectorPos = 0;
				}
				break;
			} else if (0xffff != gfl_filelistCurrentPage_tab[_selectorPos+1]) {
				// next file exist: allow down
				_selectorPos++;
			}
			break;

		case 0x0150: /* Shift Down : go next page */
		case 0x0250:
			/* TODO: better handling */
			if ( (_nbPages > 1) && (!_isLastPage) ) {
				_currentPage++;
				_selectorPos=0;
			}
			break;

		case 0x148: /* Shift Up: go previous page */
		case 0x248:
			if (_currentPage) {
				_currentPage--;
			}
			_selectorPos=0;
			break;

		case 0x448:	/* Ctrl Up : go to first page, first file */
			_currentPage=0;
			_selectorPos=0;
			break;

		case 0x450: /* Ctrl Down: go to last file */
			/* TODO : currently it only go to the first file of the last page ! */
			_currentPage = _nbPages - 1;
			_selectorPos=0;
			break;


		default:
			//key not processed: return it (otherwise, return 0)
			key_ret = key;
	}

	return key_ret;
}
