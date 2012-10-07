#ifndef __FILELIST_H__
#define __FILELIST_H__

#include "atari_hw.h"
#include "fat_access.h"
#include "hxcfeda.h"

void fli_clear(void);
int fli_init(void * base, LONG length);
int fli_push(fl_dirent * dir_entry);
UWORD fli_next(UWORD current);
UWORD fli_getNbEntries(void);
UWORD fli_getFirstFile(void);
int fli_getDirEntryMSB(UWORD number, fl_dirent * dir_entry);
int fli_getDirEntryLSB(UWORD number, DirectoryEntry * dir_entry);
void fli_sort(void);

#endif
