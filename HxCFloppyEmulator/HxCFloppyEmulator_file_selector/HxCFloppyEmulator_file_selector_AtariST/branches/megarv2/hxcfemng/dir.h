#ifndef __DIR_H__
#define __DIR_H__

void mystrlwr(char *string);
int dir_paginate();
void dir_setFilter(char *filter);
int dir_filter(char *filename);
int dir_scan(char *path);

#endif
