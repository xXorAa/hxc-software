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

static struct fs_dir_list_status file_list_status;
static UWORD FilelistPages_tab[512];
static struct fs_dir_ent dir_entry;
static char * _filter=0;

extern unsigned short SCREEN_YRESOL;
extern unsigned char  NUMBER_OF_FILE_ON_DISPLAY;

int dir_getFilesForPage(UWORD page, UWORD *FilelistCurrentPage_tab)
{
	UWORD i, currentFile;

	currentFile = FilelistPages_tab[page & 0x1ff];
	if (0xffff == currentFile) {
		return FALSE;
	}

	// clear the page
	for(i=0; i<32; i++) {
		FilelistCurrentPage_tab[i] = 0xffff;
	}


	// beginning with currentFile, get the filename, and apply the filter
	for(i=0; i<NUMBER_OF_FILE_ON_DISPLAY; currentFile++) {
		if ( TRUE != fli_getDirEntry(currentFile, &dir_entry) ) {
			break;
		}
		if (dir_filter(dir_entry.filename)) {
			// file will be shown
			FilelistCurrentPage_tab[i] = currentFile;
			i++;
		}
	}


}

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

/*
void enter_sub_dir(disk_in_drive *disk_ptr)
{
	int currentPathLength;
	unsigned char folder[128+1];
	unsigned char c;
	int i;
	int old_index;

	old_index=strlen( currentPath );
	
	if ( (disk_ptr->DirEnt.longName[0] == (unsigned char)'.') && (disk_ptr->DirEnt.longName[1] == (unsigned char)'.') )
	{
		currentPathLength = strlen( currentPath ) - 1;
		do
		{
			currentPath[ currentPathLength ] = 0;
			currentPathLength--;
		}
		while ( currentPath[ currentPathLength ] != (unsigned char)'/' );

	}
	else
	{
		if((disk_ptr->DirEnt.longName[0] == (unsigned char)'.'))
		{
		}
		else
		{
			for (i=0; i < 128; i++ )
			{
				c = disk_ptr->DirEnt.longName[i];
				if ( ( c >= (32+0) ) && (c <= 127) )
				{
					folder[i] = c;
				}
				else
				{
					folder[i] = 0;
					i = 128;
				}
			}

			currentPathLength = strlen( currentPath );

			if( currentPath[ currentPathLength-1] != '/')
			strcat( currentPath, "/" );

			strcat( currentPath, folder );
		}

	}

	displayFolder();

	selectorpos=0;


	if(!fl_list_opendir(currentPath, &file_list_status))
	{
		currentPath[old_index]=0;
		fl_list_opendir(currentPath, &file_list_status);
		displayFolder();
	}
	for(i=0;i<512;i++)
	{
		FilelistPages_tab[i] = 0xffff;
	}
	clear_list(0);
	read_entry=1;
}
*/


int dir_paginate()
{
	int i;
	UWORD currentPage;
	UWORD currentFile;
	UBYTE currentFileInPage;

	// erase all pages
	for(i=0;i<512;i++)
	{
		FilelistPages_tab[i] = 0xffff;
	}

	currentPage       = 0xffff;
	currentFile       = 0;
	currentFileInPage = NUMBER_OF_FILE_ON_DISPLAY;

	while(fli_getDirEntry(currentFile, &dir_entry)) {
		if (dir_filter(dir_entry.filename)) {
			// file will be shown
			if (currentFileInPage >= NUMBER_OF_FILE_ON_DISPLAY) {
				// add a page
				currentPage++;
				currentFileInPage = 0;
				// store the first file of the page
				FilelistPages_tab[currentPage] = currentFile;
			}
			currentFileInPage++;
		}

		currentFile++;
	}

	return currentPage+1;
}



void dir_setFilter(unsigned char *filter)
{
	_filter = filter;
}


int dir_filter(char *filename)
{
	unsigned char tmp[FATFS_MAX_LONG_FILENAME];
	if (0 != _filter) {
		strcpy(tmp, filename);
		mystrlwr(tmp);

		if(!strstr(filename, _filter))
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
	fl_list_opendir(path, &file_list_status);
	while( fl_list_readdir(&file_list_status, &dir_entry) ) {
		fli_push(&dir_entry);
		nbFiles++;
	}

	return nbFiles;
}



