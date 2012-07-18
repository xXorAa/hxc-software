#ifndef __FILELIST_H__
#define __FILELIST_H__

#include "atari_hw.h"
#include "fat_access.h"

void fli_clear(void);
int fli_init(void * base, LONG length);
int fli_push(struct fs_dir_ent * dir_entry);
int fli_get(UWORD number, struct fs_dir_ent * dir_entry);

#endif
