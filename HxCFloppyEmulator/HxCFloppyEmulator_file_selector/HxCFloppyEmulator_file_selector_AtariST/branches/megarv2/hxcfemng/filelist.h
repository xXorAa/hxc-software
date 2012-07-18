#ifndef __FILELIST_H__
#define __FILELIST_H__

#include "atari_hw.h"
#include "fat_access.h"
#include "cfg_file.h"

void fli_clear(void);
int fli_init(void * base, LONG length);
int fli_push(struct fs_dir_ent * dir_entry);
int fli_getDirEntry(UWORD number, struct fs_dir_ent * dir_entry);
int fli_getDiskInDrive(UWORD number, disk_in_drive * disk_ptr);

#endif
