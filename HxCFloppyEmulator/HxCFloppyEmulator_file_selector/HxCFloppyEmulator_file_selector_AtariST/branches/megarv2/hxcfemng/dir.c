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

#include "filelist.h"
#include "atari_hw.h"
#include "fat_opts.h"
#include "fat_access.h"
#include "conf.h"

static struct fs_dir_list_status _file_list_status;
static UWORD _FilelistPages_tab[512];
static struct fs_dir_ent _dir_entry;
static char * _filter=0;
static UWORD _nbPages = 0xffff;

extern unsigned short SCREEN_YRESOL;
extern unsigned char  NUMBER_OF_FILE_ON_DISPLAY;


void mystrlwr(char *string)
{
	while(*string)
	{
		if ( *string >= 'A' && *string <= 'Z' )
		{
			 *string = *string + 32;
		}
		string++;
	}
}

UWORD dir_getFirstFileForPage(UWORD page)
{
	return _FilelistPages_tab[page];
}

#if(0)
int dir_getFilesForPage(UWORD page, UWORD *FilelistCurrentPage_tab)
{
	UWORD i, currentFile;

	currentFile = _FilelistPages_tab[page & 0x1ff];
	if (0xffff == currentFile) {
		return FALSE;
	}

	// clear the page
	for(i=0; i<32; i++) {
		FilelistCurrentPage_tab[i] = 0xffff;
	}


	// beginning with currentFile, get the filename, and apply the filter
	for(i=0; i<NUMBER_OF_FILE_ON_DISPLAY; currentFile++) {
		if ( TRUE != fli_getDirEntryMSB(currentFile, &_dir_entry) ) {
			break;
		}
		if (dir_filter(&_dir_entry)) {
			// file will be shown
			FilelistCurrentPage_tab[i] = currentFile;
			i++;
		}
	}

	return TRUE;
}
#endif


/**
 * @returns integer number of pages
 */
UWORD dir_getNbPages()
{
	return _nbPages;
}


/**
 * Filter the files and fill an array with the first file index
 * for each page.
 */
void dir_paginate()
{
	UWORD i;
	UWORD currentPage;
	UWORD currentFile;
	UBYTE currentFileInPage;

	// erase all pages
	for(i=0;i<512;i++)
	{
		_FilelistPages_tab[i] = 0xffff;
	}

	currentPage       = 0xffff;
	currentFile       = 0;
	currentFileInPage = NUMBER_OF_FILE_ON_DISPLAY;

	while(fli_getDirEntryMSB(currentFile, &_dir_entry)) {
		if (dir_filter(&_dir_entry)) {
			// file will be shown
			if (currentFileInPage >= NUMBER_OF_FILE_ON_DISPLAY) {
				// add a page
				currentPage++;
				currentFileInPage = 0;
				// store the first file of the page
				_FilelistPages_tab[currentPage] = currentFile;
			}
			currentFileInPage++;
		}

		currentFile++;
	}

	_nbPages = currentPage+1;
}


char * dir_getFilter()
{
	return _filter;
}

void dir_setFilter(char *filter)
{
	if (filter != 0 && strlen(filter)) {
		_filter = filter;
	} else {
		_filter = 0;
	}
}



int dir_filter(struct fs_dir_ent *dir_entry)
{
	unsigned char tmp[FATFS_MAX_LONG_FILENAME];
	if (0 != _filter && !(dir_entry->is_dir)) {
		// only filter when filter is set. Don't filter dirs
		strcpy(tmp, dir_entry->filename);
		mystrlwr(tmp);

		if(!strstr(tmp, _filter))
		{
			return FALSE;
		}
	}
	return TRUE;
}


int dir_scan(char *path)
{
	UWORD nbFiles;

	// reset the file list
	fli_clear();

	// get all the files in the dir
	nbFiles = 0;
	fl_list_opendir(path, &_file_list_status);
	while( fl_list_readdir(&_file_list_status, &_dir_entry) ) {
		fli_push(&_dir_entry);
		nbFiles++;
	}

	return nbFiles;
}



