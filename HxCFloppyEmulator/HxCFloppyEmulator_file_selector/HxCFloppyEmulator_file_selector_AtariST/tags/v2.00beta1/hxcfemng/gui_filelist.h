#ifndef __GUI_FILELIST_H__
#define __GUI_FILELIST_H__

UWORD gfl_getSelectorPos();
UWORD gfl_getSelectorEntry();
void gfl_setCachedPage(UWORD newpage);
void gfl_jumpToFile(UWORD askedFile);
void gfl_showFilesForPage(UBYTE fRepaginate, UBYTE fForceRedraw);
long gfl_mainloop();

#endif
