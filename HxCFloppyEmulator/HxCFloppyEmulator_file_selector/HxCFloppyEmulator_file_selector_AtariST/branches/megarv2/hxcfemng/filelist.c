#include "fat_access.h"
#include "atari_hw.h"
#include "hxcfeda.h"
#include "quickersort.h"
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


// A enlever:
#include "gui_utils.h"


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
    // remove trailing spaces
    while(dir_entry->filename[len-1] == ' ') {
        len--;
    }
    if (! (1 == len && '.' == dir_entry->filename[0]) )
    {   // don't push "."
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
        *(newAdr+8) = 2-(dir_entry->is_dir);   // copy (is_dir+1), so it is either 1(dir) or 2(file) (for sorting)
        memcpy(newAdr+9, &dir_entry->filename, len);
        *(newAdr+9 + len) = (unsigned char) 0;

        // add pointer to the block
        UBYTE **ptr;
        ptr = (UBYTE **) (_base + _nbEntries*4);
        *ptr = newAdr;

        _nbEntries++;
    }
    return TRUE;
}


#if(0)
UWORD fli_next(UWORD current)
{
	if ( (current+1) < _nbEntries ) {
		return current+1;
	}
	return 0xffff;
}
#endif

/**
 * Fill the provided fs_dir_ent struct for the specified file index
 * This is the Big Endian struct defined in fat_access.h:
 *   char                    filename[FATFS_MAX_LONG_FILENAME];
 *   unsigned char           is_dir;
 *   UINT32                  cluster;
 *   UINT32                  size;
 *
 * @returns boolean success
 */
int fli_getDirEntryMSB(UWORD number, struct fs_dir_ent * dir_entry)
{
    UBYTE **ptr;
    UBYTE *newAdr;

    if (number >= _nbEntries) {
        return FALSE;
    }
    ptr = (UBYTE **) (_base + number*4);
    newAdr = *ptr;

    memcpy(&dir_entry->cluster, newAdr, 8);                // copy cluster, size
    dir_entry->is_dir = 2-(*(newAdr+8));                   // copy is_dir
    strcpy(dir_entry->filename, (char *) newAdr+9);        // copy filename

    return TRUE;
}

/**
 * Fill the provided DirectoryEntry struct for the specified file index
 * This is the Little Endian struct defined in hxcfeda.h:
 *   unsigned char name[12];
 *   unsigned char attributes;
 *   unsigned long firstCluster;
 *   unsigned long size;
 *   unsigned char longName[LFN_MAX_SIZE];
 *
 * @returns boolean success
 */
int fli_getDirEntryLSB(UWORD number, DirectoryEntry * dir_entry)
{
    UBYTE **ptr;
    UBYTE *newAdr;

    if (number >= _nbEntries) {
        return FALSE;
    }
    ptr = (UBYTE **) (_base + number*4);
    newAdr = *ptr;

    //copy cluster, and size little endian
    dir_entry->firstCluster_b1 = *(newAdr+3);
    dir_entry->firstCluster_b2 = *(newAdr+2);
    dir_entry->firstCluster_b3 = *(newAdr+1);
    dir_entry->firstCluster_b4 = *(newAdr+0);
    dir_entry->size_b1 = *(newAdr+7);
    dir_entry->size_b2 = *(newAdr+6);
    dir_entry->size_b3 = *(newAdr+5);
    dir_entry->size_b4 = *(newAdr+4);

    // copy cluster, size
    UBYTE attr = 0;
    if (1 == *(newAdr+8)) {
        attr = 0x10;
    }
    dir_entry->attributes = attr;                         // copy attribute
    memcpy(dir_entry->name,     newAdr+9, 12);              // copy name
    memcpy(dir_entry->longName, newAdr+9, LFN_MAX_SIZE);    // copy longname
    dir_entry->longName[LFN_MAX_SIZE-1] = 0;
    return TRUE;
}


void fli_sort(void)
{
    extern void quickersort(UWORD nbEntries, UBYTE offset, UBYTE *adr);
    //unsigned long time = get_hz200();
    quickersort_call(quickersort, (UWORD) _nbEntries, (UBYTE) 8, _base);
    //time = get_hz200() - time;
    //time = time / 2;
    //hxc_printf(0,0,0,"fli_sort(): %ld seconds/100 ", time);

}
