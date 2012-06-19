#include "compiler_helper.h"

#ifndef BMAPTYPEDEF
#define BMAPTYPEDEF


STRUCT_ALIGN2_BEGIN
typedef  struct STRUCT_ALIGN2 _bmaptype
{
   int type;
   int Xsize;
   int Ysize;
   int size;
   int csize;
   unsigned char * data;
} STRUCT_ALIGN2_PREEND bmaptype;
STRUCT_ALIGN2_END
#endif
