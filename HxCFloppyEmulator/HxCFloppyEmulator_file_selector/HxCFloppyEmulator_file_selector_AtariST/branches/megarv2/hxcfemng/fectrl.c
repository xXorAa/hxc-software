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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __VBCC__
#include <tos.h>
#else
#include <mint/osbind.h>
#endif

#include <time.h>
/* #include <vt52.h>
 */

#include "keysfunc_defs.h"

#include "gui_utils.h"
#include "cfg_file.h"
#include "hxcfeda.h"
#include "dir.h"
#include "filelist.h"


#include "atari_hw.h"
/* #include "atari_regs.h" */

#include "fat_opts.h"
#include "fat_misc.h"
#include "fat_defs.h"
#include "fat_filelib.h"

#include "conf.h"


static unsigned long indexptr;
static unsigned short y_pos;
static unsigned long last_setlbabase;
static unsigned char sector[512];
static unsigned char cfgfile_header[512];

static unsigned char currentPath[4*256] = {"\\"};

static unsigned char sdfecfg_file[2048];
static char filter[17];


static char selectorpos;
static unsigned char fRedraw_files, fRepaginate_files, fRedraw_status;
static UWORD page_number;

static disk_in_drive disks_slot_a[NUMBER_OF_SLOT];
static disk_in_drive disks_slot_b[NUMBER_OF_SLOT];
static UWORD FilelistCurrentPage_tab[120];

static struct fs_dir_list_status file_list_status;
static struct fat_dir_entry sfEntry;
static struct fs_dir_ent dir_entry;
extern  struct fatfs _fs;

extern unsigned short SCREEN_XRESOL;
extern unsigned short SCREEN_YRESOL;
extern unsigned char  NUMBER_OF_FILE_ON_DISPLAY;


void lockup()
{
	while(1) {
		get_char();
	}
}


int setlbabase(unsigned long lba)
{
	int ret;
	unsigned char cmd_cnt;
	unsigned long lbatemp;
	direct_access_cmd_sector * dacs;
	direct_access_status_sector * dass;

	dass=(direct_access_status_sector *)sector;
	dacs=(direct_access_cmd_sector  *)sector;

	memset(&sector,0,512);

	sprintf(dacs->DAHEADERSIGNATURE,"HxCFEDA");
	dacs->cmd_code=1;
	dacs->parameter_0=(lba>>0)&0xFF;
	dacs->parameter_1=(lba>>8)&0xFF;
	dacs->parameter_2=(lba>>16)&0xFF;
	dacs->parameter_3=(lba>>24)&0xFF;
	dacs->parameter_4=0xA5;

	ret=writesector( 0,(unsigned char *)&sector);
	if(!ret)
	{
		hxc_printf_box(0,"FATAL ERROR: Write CTRL ERROR !");
		lockup();
	}

	return 0;
}


/**
 * Init the hardware
 * Display Firmware version
 * @return 0 on failure, 1 on success
 */
int hxc_media_init()
{
	unsigned char ret;
	unsigned char sector[512];
	int i;


	direct_access_status_sector * dass;

	last_setlbabase=0xFFFFF000;
	ret=readsector(0,(unsigned char*)&sector,1);

	if(ret)
	{
		dass=(direct_access_status_sector *)sector;
		if(!strcmp(dass->DAHEADERSIGNATURE,"HxCFEDA"))
		{
			hxc_printf(0,0,SCREEN_YRESOL-30,"Firmware %s" ,dass->FIRMWAREVERSION);

			dass= (direct_access_status_sector *)sector;
			last_setlbabase=0;
			setlbabase(last_setlbabase);

			return 1;
		}

		hxc_printf_box(0,"FATAL ERROR: HxC Floppy Emulator not found!");

		return 0;
	}
	hxc_printf_box(0,"FATAL ERROR: Floppy Access error!  [%d]",ret);

	return 0;
}

int hxc_media_read(unsigned long sector, unsigned char *buffer)
{
	int ret,retry;
	direct_access_status_sector * dass;

	dass= (direct_access_status_sector *)buffer;

	hxc_printf(0,8*79,0,"%c",23);

	ret=0;

	do
	{
		if((sector-last_setlbabase)>=8)
		{
			setlbabase(sector);
		}

		if(!readsector(0,buffer,0))
		{
			hxc_printf_box(0,"ERROR: Read ERROR ! fsector %d",(sector-last_setlbabase)+1);
			get_char_restore_box();
		}
		last_setlbabase=L_INDIAN(dass->lba_base);

		/* hxc_printf(0,0,0,"BA: %08X %08X" ,L_INDIAN(dass->lba_base),sector);*/
	}while((sector-L_INDIAN(dass->lba_base))>=8);

	if(!readsector((sector-last_setlbabase)+1,buffer,0))
	{
		hxc_printf_box(0,"FATAL ERROR: Read ERROR ! fsector %d",(sector-last_setlbabase)+1);
		lockup();
	}

	hxc_printf(0,8*79,0," ");

	return 1;
}

int hxc_media_write(unsigned long sector, unsigned char *buffer)
{
	int ret,retry;
	direct_access_status_sector * dass;

	hxc_printf(0,8*79,0,"%c",23);

	if((sector-last_setlbabase)>=8)
	{
		last_setlbabase=sector;
		setlbabase(sector);
	}

	if(!writesector((sector-last_setlbabase)+1,buffer))
	{
		hxc_printf_box(0,"FATAL ERROR: Write sector ERROR !");
		lockup();
	}

	hxc_printf(0,8*79,0," ");

	return 1;
}


int bios_media_read(unsigned long sector, unsigned char *buffer)
{
	Rwabs(0, buffer, 1, sector, 0);
	return 1;
}

int bios_media_write(unsigned long sector, unsigned char *buffer)
{
	Rwabs(1, buffer, 1, sector, 0);
	return 1;
}






char read_cfg_file(unsigned char * sdfecfg_file)
{
	char ret;
	unsigned char number_of_slot;
	unsigned short i;
	cfgfile * cfgfile_ptr;
	FL_FILE *file;

	memset((void*)&disks_slot_a,0,sizeof(disk_in_drive)*NUMBER_OF_SLOT);
	memset((void*)&disks_slot_b,0,sizeof(disk_in_drive)*NUMBER_OF_SLOT);

	ret=0;
	file = fl_fopen("/HXCSDFE.CFG", "r");
	if (file)
	{
		cfgfile_ptr=(cfgfile * )cfgfile_header;

		fl_fread(cfgfile_header, 1, 512 , file);
		number_of_slot=cfgfile_ptr->number_of_slot;

		fl_fseek(file , 1024 , SEEK_SET);

		fl_fread(sdfecfg_file, 1, 512 , file);
		i=1;
		do
		{
			if(!(i&3))
			{
				fl_fread(sdfecfg_file, 1, 512 , file);
			}

			memcpy(&disks_slot_a[i],&sdfecfg_file[(i&3)*128],sizeof(disk_in_drive));
			memcpy(&disks_slot_b[i],&sdfecfg_file[((i&3)*128)+64],sizeof(disk_in_drive));

			i++;
		}while(i<number_of_slot);

		fl_fclose(file);
	}
	else
	{
		ret=1;
	}

	if(ret)
	{
		hxc_printf_box(0,"ERROR: Access HXCSDFE.CFG file failed! [%d]",ret);
		get_char_restore_box();
	}

	return ret;
}

char save_cfg_file(unsigned char * sdfecfg_file)
{
	unsigned char number_of_slot,slot_index;
	unsigned char i,sect_nb,ret;
	cfgfile * cfgfile_ptr;
	unsigned short  floppyselectorindex;
	FL_FILE *file;

	ret=0;
	file = fl_fopen("/HXCSDFE.CFG", "r");
	if (file)
	{
		number_of_slot=1;
		slot_index=1;
		i=1;

		floppyselectorindex=128;                      /* First slot offset */
		memset( sdfecfg_file,0,512);                  /* Clear the sector */
		sect_nb=2;                                    /* Slots Sector offset */

		do
		{
			if( read_long_odd(&(disks_slot_a[i].DirEnt.size_b1)))            /* Valid slot found */
			{
				// Copy it to the actual file sector
				memcpy(&sdfecfg_file[floppyselectorindex],&disks_slot_a[i],sizeof(disk_in_drive));
				memcpy(&sdfecfg_file[floppyselectorindex+64],&disks_slot_b[i],sizeof(disk_in_drive));

				//Next slot...
				number_of_slot++;
				floppyselectorindex=(floppyselectorindex+128)&0x1FF;

				if(!(number_of_slot&0x3))                /* Need to change to the next sector */
				{
					/* Save the sector */
					if (fl_fswrite((unsigned char*)sdfecfg_file, 1,sect_nb, file) != 1)
					{
						hxc_printf_box(0,"ERROR: Write file failed!");
						get_char_restore_box();
						ret=1;
					}
					/* Next sector */
					sect_nb++;
					memset( sdfecfg_file,0,512);                  /* Clear the next sector */
				}
			}

			i++;
		}while(i<NUMBER_OF_SLOT);

		if(number_of_slot&0x3)
		{
			if (fl_fswrite((unsigned char*)sdfecfg_file, 1,sect_nb, file) != 1)
			{
				hxc_printf_box(0,"ERROR: Write file failed!");
				get_char_restore_box();
				ret=1;
			}
		}

		if(slot_index>=number_of_slot)
		{
			slot_index=number_of_slot-1;
		}

		fl_fseek(file , 0 , SEEK_SET);

		/* Update the file header */
		/* fl_fread(sdfecfg_file, 1, 512 , file); */

		cfgfile_ptr=(cfgfile * )cfgfile_header;/* sdfecfg_file; */
		cfgfile_ptr->number_of_slot=number_of_slot;
		cfgfile_ptr->slot_index=slot_index;

		if (fl_fswrite((unsigned char*)cfgfile_header, 1,0, file) != 1)
		{
			hxc_printf_box(0,"ERROR: Write file failed!");
			get_char_restore_box();
			ret=1;
		}

	}
	else
	{
		hxc_printf_box(0,"ERROR: Create file failed!");
		get_char_restore_box();
		ret=1;
	}
	/* Close file */
	fl_fclose(file);

	return ret;
}


/**
 * Clear the screen
 * @param integer add number of additional lines to clear. Allow to clear the status bar
 */
void clear_list(unsigned char add)
{
	unsigned short y_pos,i;

	y_pos=FILELIST_Y_POS;
	for(i=0;i<NUMBER_OF_FILE_ON_DISPLAY+add;i++)
	{
		clear_line(y_pos,0);
		y_pos=y_pos+8;
	}
}






void _printslotstatus(unsigned char slotnumber,  disk_in_drive * disks_a,  disk_in_drive * disks_b)
{
	char tmp_str[17];

	hxc_printf(0,0,SLOT_Y_POS,"Slot %02d:", slotnumber);

	/* clear_line(SLOT_Y_POS+8,0); */
	hxc_printf(0,0,SLOT_Y_POS+8,"Drive A:                 ");
	if( read_long_odd(&(disks_a->DirEnt.size_b1)))
	{
		memcpy(tmp_str,disks_a->DirEnt.longName,16);
		tmp_str[16]=0;
		hxc_printf(0,0,SLOT_Y_POS+8,"Drive A: %s", tmp_str);
	}

	/* clear_line(SLOT_Y_POS+16,0); */
	hxc_printf(0,0,SLOT_Y_POS+16,"Drive B:                 ");

	if( read_long_odd(&(disks_b->DirEnt.size_b1)))
	{
		memcpy(tmp_str,disks_b->DirEnt.longName,16);
		tmp_str[16]=0;
		hxc_printf(0,0,SLOT_Y_POS+16,"Drive B: %s", tmp_str);
	}
}

void display_slot(unsigned char slotnumber)
{
	_printslotstatus(slotnumber, (disk_in_drive *) &disks_slot_a[slotnumber], (disk_in_drive *) &disks_slot_b[slotnumber]) ;
}


unsigned char next_slot(unsigned char slotnumber)
{
	slotnumber++;
	if(slotnumber>(NUMBER_OF_SLOT-1))
	{	// slot 0 is reserved for autoboot
		slotnumber=1;
	}
	display_slot(slotnumber);
	return slotnumber;
}

void clear_slot(int slotnumber)
{
	memset((void*)&disks_slot_a[slotnumber],0,sizeof(disk_in_drive));
	memset((void*)&disks_slot_b[slotnumber],0,sizeof(disk_in_drive));
}

void insert_in_slot(DirectoryEntry *dirEntLSB_ptr, unsigned char slotnumber, unsigned char drive)
{
	if (0 == drive)
	{
		memset((void*)&disks_slot_a[slotnumber], 0, sizeof(disk_in_drive));
		memcpy((void*)&disks_slot_a[slotnumber], dirEntLSB_ptr, sizeof(struct ShortDirectoryEntry));
	}
	else
	{
		memset((void*)&disks_slot_b[slotnumber], 0, sizeof(disk_in_drive));
		memcpy((void*)&disks_slot_b[slotnumber], dirEntLSB_ptr, sizeof(struct ShortDirectoryEntry));
	}
	display_slot(slotnumber);
}




/**
 * Display the current folder
 */
void displayFolder()
{
	int i;
	UWORD curdir_len;

	curdir_len = (SCREEN_XRESOL - CURDIR_X_POS)/8;
	hxc_printf(0, CURDIR_X_POS, CURDIR_Y_POS, "Current directory:");

	for(i=CURDIR_X_POS; i<SCREEN_XRESOL; i=i+8) {
		hxc_printf(0, i, CURDIR_Y_POS+8, " ");
	}

	if(strlen(currentPath) < curdir_len)
		hxc_printf(0, CURDIR_X_POS, CURDIR_Y_POS+8, "%s", currentPath);
	else
		hxc_printf(0, CURDIR_X_POS, CURDIR_Y_POS+8, "...%s", &currentPath[strlen(currentPath)-curdir_len]+3);
}



void enter_sub_dir(DirectoryEntry *dirEntLSB_ptr)
{
	int currentPathLength;
	unsigned char folder[LFN_MAX_SIZE];
	unsigned char c;
	int i;
	int old_index;

	old_index=strlen( currentPath );

	if ( (dirEntLSB_ptr->longName[0] == (unsigned char)'.') && (dirEntLSB_ptr->longName[1] == (unsigned char)'.') )
	{
		currentPathLength = strlen( currentPath ) - 1;
		do
		{
			currentPath[ currentPathLength ] = 0;
			currentPathLength--;
		}
		while ( currentPath[ currentPathLength ] != (unsigned char)'/' );

		/*if ( currentPath[ currentPathLength-1 ] != (unsigned char)':' )
		{
			currentPath[ currentPathLength ] = 0;
		}*/
	}
	else
	{
		if((dirEntLSB_ptr->longName[0] == (unsigned char)'.'))
		{
		}
		else
		{
			for (i=0; i < LFN_MAX_SIZE; i++ )
			{
				c = dirEntLSB_ptr->longName[i];
				if ( ( c >= (32+0) ) && (c <= 127) )
				{
					folder[i] = c;
				}
				else
				{
					folder[i] = 0;
					i = LFN_MAX_SIZE;
				}
			}
			folder[LFN_MAX_SIZE-1] = 0;

			currentPathLength = strlen( currentPath );
			/*if ( currentPath[ currentPathLength-1-1 ] != (unsigned char)':' )
			{
				strcat( currentPath, "/" );
			}*/

			if( currentPath[ currentPathLength-1] != '/')
			strcat( currentPath, "/" );

			strcat( currentPath, folder );
		}

		/* strcat( currentPath, "/" ); */
	}

	displayFolder();

	if(!fl_list_opendir(currentPath, &file_list_status))
	{
		currentPath[old_index]=0;
		displayFolder();
	}
	dir_scan(currentPath);

	fRepaginate_files = 1;
}



void handle_show_all_slots(void)
{
	#define ALLSLOTS_Y_POS 16
	char tmp_str[17];
	int i;

	clear_list(5);

	for ( i = 1; i < NUMBER_OF_SLOT; i++ )
	{
		if( read_long_odd(&(disks_slot_a[i].DirEnt.size_b1)) || read_long_odd(&(disks_slot_b[i].DirEnt.size_b1)))
		{
			memcpy(tmp_str,&disks_slot_a[i].DirEnt.longName,16);
			tmp_str[16]=0;
			hxc_printf(0,0,ALLSLOTS_Y_POS + (i*8),"Slot %02d - A : %s", i, tmp_str);

			memcpy(tmp_str,&disks_slot_b[i].DirEnt.longName,16);
			tmp_str[16]=0;
			hxc_printf(0,40*8,ALLSLOTS_Y_POS + (i*8),"B : %s", tmp_str);

		}
		else
		{
			hxc_printf(0,0,ALLSLOTS_Y_POS + (i*8),"Slot %02d - A :", i);
			hxc_printf(0,40*8,ALLSLOTS_Y_POS + (i*8),"B :", i);
		}
	}

	hxc_printf(1,0,ALLSLOTS_Y_POS + NUMBER_OF_SLOT*8 + 1,"---Press any key---");
	wait_function_key();
}





void handle_help()
{
	int i;
	clear_list(5);

	i=0;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Function Keys (1/2):");

	i=2;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Up/Down/Right/Left: Browse the SDCard files");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Caps Lock         : Go back to the top of the folder");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Insert            : Insert the selected file in the current slot to A:");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "                    Enter a subfolder");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Clr/Home          : Insert the selected file in the current slot to B:");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Pipe              : Insert the selected file in the current slot to A: and ");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "                    select the next slot");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "F7                : Insert the selected file in the slot to 1 and restart the");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "                    computer with this disk.");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Undo              : Select the the next slot");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Backspace         : Clear the current slot");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Delete            : Clear the current slot and Select the the next slot");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "TAB               : Show all slots selections");

	i=i+2;

	hxc_printf(1,0,HELP_Y_POS+(i*8), "---Any key to continue---");

	wait_function_key();

	clear_list(5);

	i=0;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Function Keys (2/2):");

	i=2;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "F1                : Search files in the current folder");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "                    Type the word to search then enter");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "                    Enter blank to abort the search");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "F2                : Change color");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "F3                : Settings menu");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "F8                : Reboot");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "F9                : Save");
	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "F10               : Save and Reboot");

	i=i+2;
	i = display_credits(i);

	wait_function_key();
}



void handle_emucfg(void)
{
	cfgfile * cfgfile_ptr;
	int i;
	unsigned char c;

	clear_list(5);
	cfgfile_ptr=(cfgfile * )cfgfile_header;

	i=0;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "SD HxC Floppy Emulator settings:");

	i=2;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "Track step sound :");
	hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%s ",cfgfile_ptr->step_sound?"on":"off");

	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "User interface sound:");
	hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d  ",cfgfile_ptr->buzzer_duty_cycle);

	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "LCD Backlight standby:");
	hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d s",cfgfile_ptr->back_light_tmr);

	i++;
	hxc_printf(0,0,HELP_Y_POS+(i*8), "SDCard Standby:");
	hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d s",cfgfile_ptr->standby_tmr);

	i=i+2;
	hxc_printf(1,0,HELP_Y_POS+(i*8), "---Press Esc to exit---");

	i=2;
	invert_line(HELP_Y_POS+(i*8));
	do
	{
		c=wait_function_key();
		switch(c)
		{
			case FCT_UP_KEY:
				invert_line(HELP_Y_POS+(i*8));
				if(i>2) i--;
				invert_line(HELP_Y_POS+(i*8));
			break;
			case FCT_DOWN_KEY:
				invert_line(HELP_Y_POS+(i*8));
				if(i<5) i++;
				invert_line(HELP_Y_POS+(i*8));
			break;
			case FCT_LEFT_KEY:
				invert_line(HELP_Y_POS+(i*8));
				switch(i)
				{
				case 2:
					cfgfile_ptr->step_sound=~cfgfile_ptr->step_sound;
					hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%s ",cfgfile_ptr->step_sound?"on":"off");
				break;
				case 3:
					if(cfgfile_ptr->buzzer_duty_cycle) cfgfile_ptr->buzzer_duty_cycle--;
					hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d  ",cfgfile_ptr->buzzer_duty_cycle);
					if(!cfgfile_ptr->buzzer_duty_cycle) cfgfile_ptr->ihm_sound=0x00;
					break;
				case 4:
					if(cfgfile_ptr->back_light_tmr) cfgfile_ptr->back_light_tmr--;
					hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d s ",cfgfile_ptr->back_light_tmr);
				break;

				case 5:
					if(cfgfile_ptr->standby_tmr) cfgfile_ptr->standby_tmr--;
					hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d s ",cfgfile_ptr->standby_tmr);
				break;
				}
				invert_line(HELP_Y_POS+(i*8));

			break;
			case FCT_RIGHT_KEY:
				invert_line(HELP_Y_POS+(i*8));
				switch(i)
				{
				case 2:
					cfgfile_ptr->step_sound=~cfgfile_ptr->step_sound;
					hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%s ",cfgfile_ptr->step_sound?"on":"off");
					break;
				case 3:
					if(cfgfile_ptr->buzzer_duty_cycle<0x80) cfgfile_ptr->buzzer_duty_cycle++;
					hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d  ",cfgfile_ptr->buzzer_duty_cycle);
					cfgfile_ptr->ihm_sound=0xFF;
				break;
				case 4:
					if(cfgfile_ptr->back_light_tmr<0xFF) cfgfile_ptr->back_light_tmr++;
					hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d s ",cfgfile_ptr->back_light_tmr);

				break;
				case 5:
					if(cfgfile_ptr->standby_tmr<0xFF) cfgfile_ptr->standby_tmr++;
					hxc_printf(0,SCREEN_XRESOL/2,HELP_Y_POS+(i*8), "%d s ",cfgfile_ptr->standby_tmr);
				break;
				}
				invert_line(HELP_Y_POS+(i*8));
			break;

		}
	}while(c!=FCT_ESC);

}













int main(int argc, char* argv[])
{
	unsigned short i;
	unsigned char key, entrytype,bootdev,j;
	unsigned char isLastPage,c;
	DirectoryEntry   dirEntLSB;
	DirectoryEntry * dirEntLSB_ptr = &dirEntLSB;
	unsigned char colormode;
	unsigned char fSelectorValid;
	unsigned char slotnumber;

	FILE *f;

	UBYTE * bigmem_adr;
	LONG    bigmem_len;

	UWORD	nbPages;

	init_display();

//
	bigmem_len = (long)    malloc(-1L);
	bigmem_adr = (UBYTE *) malloc(bigmem_len);
	fli_init(bigmem_adr, bigmem_len);
//

	bootdev=0;/* argv[1][0]-'0'; */

	/* Initialise File IO Library */
	fl_init();

	init_atari_hw();

	fn_diskio_read media_read_callback;
	fn_diskio_write media_write_callback;

	if (emulatordetect())
	{
		media_read_callback = bios_media_read;
		media_write_callback = bios_media_write;
	}
	else
	{
		init_atari_fdc(bootdev);

		if(!hxc_media_init()) {
			lockup();
		}

		media_read_callback = hxc_media_read;
		media_write_callback = hxc_media_write;
	}


	/* Attach media access functions to library*/
	if (fl_attach_media(media_read_callback, media_write_callback) != FAT_INIT_OK)
	{
		hxc_printf_box(0,"FATAL ERROR: Media attach failed !");
		lockup();
	}

	hxc_printf_box(0,"Reading HXCSDFE.CFG ...");
	read_cfg_file(sdfecfg_file);
	restore_box();

	if(cfgfile_header[256+128]!=0xFF)
		set_color_scheme(cfgfile_header[256+128]);

	strcpy( currentPath, "/" );

	slotnumber=1;

	colormode=0;
	selectorpos=0;
	page_number=0;

	// get all the files in the dir
	dir_scan(currentPath);
	dir_setFilter(0);
	fRepaginate_files = 1;
	fRedraw_status = 1;



	int inverted_line;
	for(;;)
	{
		y_pos=FILELIST_Y_POS;

		if (fRepaginate_files) {
			nbPages = dir_paginate();
			selectorpos=0;
			page_number=0;
			fRedraw_files=1;
			fRepaginate_files=0;
		} // if (fRepaginate_files)

		if (fRedraw_status) {
			clear_list(5);
			display_status();
			display_slot(slotnumber);
			displayFolder();
			fRedraw_files=1;
			fRedraw_status=0;
		}

		if (fRedraw_files) {
			// start at the first file of the directory
//			dir_getFilesForPage(page_number, FilelistCurrentPage_tab);
			clear_list(0);

			// clear the page
			for (i=1; i<32; i++) {
				FilelistCurrentPage_tab[i] = 0xffff;
			}

			// page number
			hxc_printf(0,PAGE_X_POS, PAGE_Y_POS, "Page %d of %d      ", page_number+1, nbPages);

			// search
			hxc_printf(0, SEARCH_X_POS, SEARCH_Y_POS,"Search (F1): [%s]", filter);

			y_pos=FILELIST_Y_POS;


			UWORD curFile;
			curFile = dir_getFirstFileForPage(page_number);
			fli_getDirEntryMSB(curFile, &dir_entry);

			for (i=0; i<NUMBER_OF_FILE_ON_DISPLAY; i++)
			{
				hxc_printf(0,0,y_pos," %c%s", (dir_entry.is_dir)?(10):(' '), dir_entry.filename);
				y_pos=y_pos+8;

				FilelistCurrentPage_tab[i] = curFile;
				do {
					curFile++;
					if (!fli_getDirEntryMSB(curFile, &dir_entry)) {
						i = NUMBER_OF_FILE_ON_DISPLAY;
						break;
					}
					;
				} while (!dir_filter(&dir_entry));

			}

			fRedraw_files = 0;
			inverted_line = -1;
			isLastPage = ((page_number+1) == nbPages);
		} // if (fRedraw_files)

		// reset the inverted line
		if (inverted_line>=0) {
			invert_line(inverted_line);
			hxc_printf(0,0,inverted_line," ");
		}

		// set the inverted line
		inverted_line = FILELIST_Y_POS+(selectorpos*8);
		hxc_printf(0,0,inverted_line,">");
		invert_line(inverted_line);

		fSelectorValid = fli_getDirEntryLSB(FilelistCurrentPage_tab[selectorpos], dirEntLSB_ptr);


		// debug:
		//hxc_printf(0,0,0,"pagenumber:%d isLastPage:%d selectorpos:%d nbPages:%d    ", page_number, isLastPage, selectorpos, nbPages);


		key=wait_function_key();
		switch(key)
		{
		case FCT_UP_KEY: /* UP */
			if (selectorpos > 0) {
				selectorpos--;
				break;
			}

			// top line
			if (0==page_number) {
				// first page: stuck
				break;
			}

			// top line not on the first page
			selectorpos=NUMBER_OF_FILE_ON_DISPLAY-1;
			page_number--;
			fRedraw_files=1;
			break;

		case FCT_DOWN_KEY: /* Down */
			if ( (selectorpos+1)==NUMBER_OF_FILE_ON_DISPLAY ) {
				// last line of display
				if (!isLastPage) {
					page_number++;
					fRedraw_files = 1;
					selectorpos = 0;
				}
				break;
			} else if (0xffff != FilelistCurrentPage_tab[selectorpos+1]) {
				// next file exist: allow down
				selectorpos++;
			}
			break;

		case FCT_RIGHT_KEY: /* Right */
			if (nbPages > 1) {
				if (isLastPage) {
					page_number = 0;
				} else {
					page_number++;
				}
				fRedraw_files=1;
				selectorpos=0;
			}
			break;

		case FCT_LEFT_KEY:
			if (nbPages > 1) {
				if(0 == page_number) {
					page_number = nbPages - 1;
				} else {
					page_number--;
				}
				selectorpos=0;
				fRedraw_files=1;
			}
			break;

		case FCT_NEXTSLOT:
			slotnumber = next_slot(slotnumber);
			break;

		case FCT_SAVE:
			hxc_printf_box(0,"Saving selection...");
			save_cfg_file(sdfecfg_file);
			restore_box();
			break;

		case FCT_SELECT_FILE_DRIVEA:
			if (fSelectorValid)
			{
				if(dirEntLSB_ptr->attributes&0x10)
				{
					enter_sub_dir(dirEntLSB_ptr);
				}
				else
				{
					insert_in_slot(dirEntLSB_ptr, slotnumber, 0);
				}
			}
			break;

		case FCT_SELECT_FILE_DRIVEB:
			if (fSelectorValid)
			{
				if(dirEntLSB_ptr->attributes&0x10)
				{
					enter_sub_dir(dirEntLSB_ptr);
				}
				else
				{
					insert_in_slot(dirEntLSB_ptr, slotnumber, 1);
				}
			}
			break;

		case FCT_SELECT_FILE_DRIVEA_AND_NEXTSLOT:
			if (fSelectorValid)
			{
				if(dirEntLSB_ptr->attributes&0x10)
				{
					enter_sub_dir(dirEntLSB_ptr);
				}
				else
				{
					insert_in_slot(dirEntLSB_ptr, slotnumber, 0);
					slotnumber = next_slot(slotnumber);
				}
			}
			break;

		case FCT_SELECTSAVEREBOOT:
			if (fSelectorValid)
			{
				if(dirEntLSB_ptr->attributes&0x10)
				{
					enter_sub_dir(dirEntLSB_ptr);
				}
				else
				{
					insert_in_slot(dirEntLSB_ptr, 1, 0);
					hxc_printf_box(0,"Saving selection and restart...");
					save_cfg_file(sdfecfg_file);
					restore_box();
					hxc_printf_box(0,">>>>>Rebooting...<<<<<");
					/* sleep(1); */
					jumptotrack0();
					reboot();
				}
			}
			break;

		case FCT_HELP:
			handle_help();

			fRedraw_status = 1;
			break;

		case FCT_EMUCFG:
			handle_emucfg();

			fRedraw_status = 1;
			break; // case FCT_EMUCFG:

		case FCT_SHOWSLOTS:
			handle_show_all_slots();

			fRedraw_status = 1;
			break;

		case FCT_CLEARSLOT:
			clear_slot(slotnumber);
			display_slot(slotnumber);
			break;

		case FCT_CLEARSLOT_AND_NEXTSLOT:
			clear_slot(slotnumber);
			slotnumber = next_slot(slotnumber);
			break;

		case FCT_SAVEREBOOT:
			hxc_printf_box(0,"Saving selection and restart...");
			save_cfg_file(sdfecfg_file);
			restore_box();
			hxc_printf_box(0,">>>>>Rebooting...<<<<<");
			/* sleep(1); */
			jumptotrack0();
			reboot();
			break;

		case FCT_REBOOT:
			hxc_printf_box(0,">>>>>Rebooting...<<<<<");
			/* sleep(1); */
			jumptotrack0();
			reboot();
			break;

		case FCT_CHGCOLOR:
			colormode++;
			set_color_scheme(colormode);
			cfgfile_header[256+128]=colormode;
			break;

		case FCT_TOP:
			page_number=0;
			selectorpos=0;
			fRedraw_files=1;
			break;

		case FCT_SEARCH:
			for(i=SEARCH_X_POS+13*8; i<SCREEN_XRESOL; i=i+8) {
				hxc_printf(0, i, SEARCH_Y_POS, " ");
			}
			flush_char();
			i=0;
			do
			{
				filter[i]=0;
				c=get_char();
				if(c!='\n')
				{
					filter[i]=c;
					hxc_printf(0, SEARCH_X_POS+13*8+(8*i), SEARCH_Y_POS, "%c", c);
				}
				i++;
			}while(c!='\n' && i<16);
			filter[i]=0;

			/* get_str(&filter); */
			mystrlwr(filter);

			dir_setFilter(filter);
			fRepaginate_files=1;
			break;

		case FCT_SORT:
			fli_sort();
			page_number=0;
			selectorpos=0;
			fRepaginate_files=1;
			break;

		default:
			/* printf("err %d!\n",key); */
			break;
		}
	} // for(;;)

	return 0;
}

