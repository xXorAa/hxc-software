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

/* //#define NUMBER_OF_FILE_ON_DISPLAY 19-5 //19 -240 */
#define NUMBER_OF_SLOT 16

#define SCREEN_XRESOL 640
/* #define SCREEN_YRESOL 200 */

#define FILELIST_Y_POS 12
#define HELP_Y_POS 12

#define SLOT_Y_POS    SCREEN_YRESOL-(48+(3*8))
#define CURDIR_X_POS  SCREEN_XRESOL/2
#define CURDIR_Y_POS  SCREEN_YRESOL-(48+(3*8))
#define PAGE_X_POS    CURDIR_X_POS+21*8
#define PAGE_Y_POS    CURDIR_Y_POS
