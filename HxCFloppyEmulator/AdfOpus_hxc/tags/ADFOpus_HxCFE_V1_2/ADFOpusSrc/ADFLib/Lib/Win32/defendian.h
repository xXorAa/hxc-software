/*
 *  ADF Library. (C) 1997-2002 Laurent Clevy
 */
/*! \file	defendian.h
 *  \brief	Endian-dependent definitions header.
 */

#ifndef DEFENDIAN_H
#define DEFENDIAN_H
#define LITT_ENDIAN   1

#ifdef _MSC_VER

#undef printf
#define printf

#undef putchar
#define putchar

#endif

#endif /* DEFENDIAN_H */