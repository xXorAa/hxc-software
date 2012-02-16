/*! \file	Win32/adf_nativ.h
 *  \brief	Win32 specific drive-access routines header.
 *
 * Win32 specific drive access routines for ADFLib
 * Copyright 1999 by Dan Sutherland <dan@chromerhino.demon.co.uk>
 */

#ifndef ADF_NATIV_H
#define ADF_NATIV_H

#include "../adf_str.h"

#define NATIVE_FILE  8001								/*!< Native file.	*/

#ifndef BOOL
#define BOOL int										/*!< Boolean.		*/
#endif

#ifndef RETCODE
#define RETCODE long									/*!< Return Code.	*/
#endif

/*! \brief Native Device Struct */
struct nativeDevice{
	FILE *fd;		/*!< A file descriptor. Needed by adf_dump.c.			*/
	void *hDrv;		/*!< A handle to a drive opened under NT4, 2k or XP.	*/
};

/*! \brief Native Device Functions Struct */
struct nativeFunctions{
	RETCODE (*adfInitDevice)(struct Device*, char*, BOOL);						/*!< Initialize a native device.	*/
	RETCODE (*adfNativeReadSector)(struct Device*, long, int, unsigned char*);	/*!< Read a native device sector.	*/
	RETCODE (*adfNativeWriteSector)(struct Device*, long, int, unsigned char*);	/*!< Write a native device sector.	*/
	BOOL (*adfIsDevNative)(char*);												/*!< Check if a device is native.	*/
	RETCODE (*adfReleaseDevice)();												/*!< Release a native device.		*/
};

void adfInitNativeFct();

RETCODE Win32ReadSector(struct Device *dev, long n, int size, unsigned char* buf);
RETCODE Win32WriteSector(struct Device *dev, long n, int size, unsigned char* buf);
RETCODE Win32InitDevice(struct Device *dev, char* name, BOOL ro);
RETCODE Win32ReleaseDevice(struct Device *dev);
BOOL Win32IsDevNative(char*);

#endif /* ndef ADF_NATIV_H */
