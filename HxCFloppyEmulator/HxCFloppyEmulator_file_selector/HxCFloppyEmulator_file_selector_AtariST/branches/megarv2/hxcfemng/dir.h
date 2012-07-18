#ifndef __DIR_H__
#define __DIR_H__

#include "fat_access.h"

void mystrlwr(char *string);
int dir_paginate();
void dir_setFilter(char *filter);
int dir_filter(struct fs_dir_ent *dir_entry);
int dir_scan(char *path);

#endif
