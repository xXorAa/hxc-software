#include "fat_access.h"
#include "atari_hw.h"
#include <string.h>

UBYTE * _base;
LONG    _length;
UWORD   _nbEntries;
UBYTE * _endAdr;

/*
nouveau map:
firstCluster    4
size            4
is_dir          1
longName      VAR (max 128: 127+1)

associé à un tableau de pointeurs: 4

en mémoire:
d'abord les pointeurs, au début du bloc
puis les direntryVar, qui commencent à la fin du bloc.
*/



void fli_clear(void)
{
    _nbEntries = 0;
    _endAdr      = _base + _length;
}

int fli_init(void * base, LONG length)
{
    _base   = base;
    _length = length;

    fli_clear();
    return TRUE;
}

int fli_push(struct fs_dir_ent * dir_entry) {
    UWORD len;
    UWORD totalLen;
    UBYTE *newAdr;

    // compute lenght of filename
    len = (UWORD) strlen(dir_entry->filename);
    if (len > (UWORD) 127) {
        // 127 char max + 0x00 = 128 chars
        len = 127;
    }

    // compute total length of the struct
    totalLen = (len+1 + 4+4+1 + 1) & 0xfffe;  // filename, 0x00 (+1), size(+4), cluster(+4), is_dir(+1). Add one byte and align.

    // reserve space at the end of the block
    newAdr  = _endAdr - totalLen;
    _endAdr = newAdr;

    if (newAdr <= (_base + (_nbEntries+1)*4)) {
        // no more space
        return FALSE;
    }

    // copy data into new allocated block
    memcpy(newAdr, &dir_entry->cluster, 8);  // copy cluster, size
    *(newAdr+8) = dir_entry->is_dir;         // copy is_dir
    memcpy(newAdr+9, &dir_entry->filename, len);
    *(newAdr+9 + len) = (unsigned char) 0;

    // add pointer to the block
    UBYTE **ptr;
    ptr = (UBYTE **) (_base + _nbEntries*4);
    *ptr = newAdr;

    _nbEntries++;
    return TRUE;
}


int fli_get(UWORD number, struct fs_dir_ent * dir_entry)
{
    UBYTE **ptr;
    UBYTE *newAdr;

    ptr = (UBYTE **) (_base + number*4);
    newAdr = *ptr;

    memcpy(&dir_entry->cluster, newAdr, 8);       // copy cluster, size
    dir_entry->is_dir = *(newAdr+8);              // copy is_dir
    strcpy(dir_entry->filename, newAdr+9);        // copy filename

    return TRUE;
}

