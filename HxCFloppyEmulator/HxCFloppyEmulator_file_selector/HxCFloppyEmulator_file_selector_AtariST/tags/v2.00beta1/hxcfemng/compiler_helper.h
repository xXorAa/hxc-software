#ifndef __COMPILER_HELPER_H__
#define __COMPILER_HELPER_H__

#ifdef __VBCC__
	#define STRUCT_PACK_BEGIN    #pragma pack(1)
	#define STRUCT_PACK
	#define STRUCT_PACK_PREEND   #pragma pack()
	#define STRUCT_PACK_END

	#define STRUCT_ALIGN2_BEGIN  #pragma amiga-align
	#define STRUCT_ALIGN2
	#define STRUCT_ALIGN2_PREEND
	#define STRUCT_ALIGN2_END    #pragma default-align

	#define ALIGN2_BEGIN         #pragma amiga-align
	#define ALIGN2
	#define ALIGN2_PREEND
	#define ALIGN2_END           #pragma default-align

#else

	#define STRUCT_PACK_BEGIN
	#define STRUCT_PACK
	#define STRUCT_PACK_PREEND   __attribute__ ((__packed__))
	#define STRUCT_PACK_END

	#define STRUCT_ALIGN2_BEGIN
	#define STRUCT_ALIGN2        __attribute__ ((aligned (2)))
	#define STRUCT_ALIGN2_PREEND
	#define STRUCT_ALIGN2_END

	#define ALIGN2_BEGIN
	#define ALIGN2               __attribute__ ((aligned (2)))
	#define ALIGN2_PREEND
	#define ALIGN2_END
#endif
#endif
