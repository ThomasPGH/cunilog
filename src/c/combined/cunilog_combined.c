/****************************************************************************************

	File:		head.c
	Why:		Provides the head of the combined cunilog.c code file.
	OS:			C99.
	Author:		Thomas
	Created:	2024-08-01

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-08-01	Thomas			Created.

****************************************************************************************/

#ifndef CUNILOG_USE_LIBCUNILOG

#include "./cunilog_combined.h"
/****************************************************************************************

	File		membuf.c
	Why:		Helpers for memory buffers
	OS:			C99
	Created:	2022-05-11

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2022-05-11	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./membuf.h"
	#include "./ubfmem.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
	#else
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

void *setToSizeSMEMBUF (SMEMBUF *pb, size_t siz)
{
	ubf_assert_non_NULL (pb);

	if (siz != pb->size)
	{
		unsigned char *p;
		
		if (pb->buf.puc)
		{
			if (siz < pb->size)
			{
				p = ubf_realloc (pb->buf.puc, siz);
				if (NULL == p)
				{
					ubf_free (pb->buf.puc);
					pb->buf.puc = NULL;
					p = ubf_malloc (siz);
				}
			} else
			{
				ubf_free (pb->buf.puc);
				pb->buf.puc = NULL;
				p = ubf_malloc (siz);
			}
		} else
		{
			p = ubf_malloc (siz);
		}
		if (p)
		{
			pb->buf.puc	= p;
			pb->size	= siz;
			return p;
		} else
		{
			doneSMEMBUF (pb);
		}
	}
	return pb->buf.pvoid;
}

void *growToSizeSMEMBUF (SMEMBUF *pb, size_t siz)
{
	ubf_assert_non_NULL (pb);

	if (siz > pb->size)
	{
		setToSizeSMEMBUF (pb, siz);
	}
	return pb->buf.pvoid;
}

void *growToSizeSMEMBUFreserve (SMEMBUF *pb, size_t siz, size_t res)
{
	ubf_assert_non_NULL (pb);

	if (siz > pb->size)
	{
		setToSizeSMEMBUF (pb, siz + res);
	}
	return pb->buf.pvoid;
}

void *growToSizeSMEMBUF64aligned (SMEMBUF *pb, size_t siz)
{
	ubf_assert_non_NULL (pb);

	siz = ALIGN_SIZE (siz, 64);
	if (siz > pb->size)
	{
		setToSizeSMEMBUF (pb, siz);
	}
	return pb->buf.pvoid;
}

void *growToSizeRetainSMEMBUF (SMEMBUF *pb, size_t siz)
{
	ubf_assert_non_NULL (pb);

	if (siz > pb->size)
	{
		void *n = ubf_realloc (pb->buf.pvoid, siz);
		if (n)
		{
			pb->buf.pvoid	= n;
			pb->size		= siz;
		} else
		{
			n = ubf_malloc (siz);
			if (n)
			{
				memcpy (n, pb->buf.pvoid, pb->size);
				ubf_free (pb->buf.pvoid);
				pb->size = siz;
			} else
			{
				doneSMEMBUF (pb);
			}
		}
	}
	return pb->buf.pvoid;
}

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void freeSMEMBUF (SMEMBUF *pb)
	{
		ubf_assert_non_NULL (pb);
		ubf_assert_non_NULL (pb->buf.pvoid);

		if (pb->buf.pvoid)
		{
			ubf_assert (0 < pb->size);

			ubf_free (pb->buf.pvoid);
		}
		#ifdef DEBUG
			// Anything is Ok apart from 'A'.
			pb->initComplete = '#';
		#endif
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void doneSMEMBUF (SMEMBUF *pb)
	{
		ubf_assert_non_NULL (pb);

		if (pb->buf.pvoid)
		{
			ubf_assert (0 < pb->size);

			ubf_free (pb->buf.pvoid);
			initSMEMBUF (pb);
		}
	}
#endif

void copySMEMBUF (SMEMBUF *cunilog_restrict dst, SMEMBUF *cunilog_restrict src)
{
	ubf_assert_non_NULL	(dst);
	ubf_assert_non_NULL	(src);
	ubf_assert			(isInitialisedSMEMBUF (dst));
	ubf_assert			(isInitialisedSMEMBUF (src));
	ubf_assert			(dst != src);

	growToSizeSMEMBUF (dst, src->size);
	if (isUsableSMEMBUF (dst))
	{
		memcpy (dst->buf.pch, src->buf.pch, src->size);
	}
}

void copySMEMBUFsiz (SMEMBUF *cunilog_restrict dst, SMEMBUF *cunilog_restrict src, size_t siz)
{
	ubf_assert_non_NULL	(dst);
	ubf_assert_non_NULL	(src);
	ubf_assert			(isInitialisedSMEMBUF (dst));
	ubf_assert			(isInitialisedSMEMBUF (src));
	ubf_assert			(dst != src);
	ubf_assert			(siz <= src->size);

	growToSizeSMEMBUF (dst, siz);
	if (isUsableSMEMBUF (dst))
	{
		memcpy (dst->buf.pch, src->buf.pch, siz);
	}
}

void copySMEMBUFreserve	(
		SMEMBUF		*cunilog_restrict dst,
		SMEMBUF		*cunilog_restrict src,
		size_t		reserve
						)
{
	ubf_assert_non_NULL	(dst);
	ubf_assert_non_NULL	(src);
	ubf_assert			(isInitialisedSMEMBUF (dst));
	ubf_assert			(isInitialisedSMEMBUF (src));
	ubf_assert			(dst != src);

	growToSizeSMEMBUF (dst, src->size + reserve);
	if (isUsableSMEMBUF (dst))
	{
		memcpy (dst->buf.pch, src->buf.pch, src->size);
	}
}
/****************************************************************************************

	File		WinAPI_U8.c
	Why:		UTF-8 versions of many Windows APIs.
	OS:			Windows
	Created:	2020-12-13

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2020-12-13	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifdef _WIN32

#include <Windows.h>
#include <limits.h>
#include <malloc.h>

#include <io.h>
#include <fcntl.h>

#ifdef _MSC_VER
	#include <crtdbg.h>
#endif

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./WinAPI_U8.h"

	#ifdef HAVE_STRWILDCARDS
		#if defined (UBF_STRWILDCARDS_USE_CURR_DIR) || defined (UBF_USE_FLAT_FOLDER_STRUCTURE)
			#include "./strwildcards.h"
		#else
			#include "./../../string/strwildcards.h"
		#endif
		#ifdef WINAPIU8_CONVERT_FORWARD_TO_BACKSLASH
			#include "./../../string/strslashes.h"
		#endif
	#endif

#endif

WCHAR		wcLongFileNamePrefix []	= L"\\\\?\\";				// "\\?\" prefix for very
																//	long filenames.
const char	ccLongFileNamePrefix [] = "\\\\?\\";				// The ASCII/UTF-8 version.
// The length of the long filename prefix in characters, exlcuding the NUL-terminator.
#define LENOF_LONGFILENAMEPREFIX						\
	((sizeof (ccLongFileNamePrefix)) - 1)

#ifdef WINAPIU8_CONVERT_FORWARD_TO_BACKSLASH
	// Uses the functions in strslashes.h.
#else
	// Dummy macros.
	#define forward_to_backslashU8(s,l)
	#define forward_to_backslashW(w,l)
#endif

#ifndef UNICODE
	/*
		MSVC might be set to "Use Multi-Byte Character Set" or "Not set".
		This is not what we want.
		Go to the project properties, General, Character Set, and change it to
		"Use Unicode Character Set" for all configurations.
	*/
	#error This module can only be used in Windows applications compiled for unicode (Use Unicode Character Set)
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	int reqUTF8size (const WCHAR *wcU16)
	{
		return WideCharToMultiByte (CP_UTF8, 0, wcU16, -1, NULL, 0, NULL, NULL);
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	int reqUTF8sizel (const WCHAR *wcU16, int lenU16)
	{
		return WideCharToMultiByte (CP_UTF8, 0, wcU16, lenU16, NULL, 0, NULL, NULL);
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	int UTF8_from_WinU16 (char *chU8, int sizeU8, const WCHAR *wcU16)
	{
		return WideCharToMultiByte (CP_UTF8, 0, wcU16, -1, chU8, sizeU8, NULL, NULL);
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	int UTF8_from_WinU16l (char *chU8, int sizeU8, const WCHAR *wcU16, int lenU16)
	{
		return WideCharToMultiByte (CP_UTF8, 0, wcU16, lenU16, chU8, sizeU8, NULL, NULL);
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	int reqWinU16wchars (const char *chU8)
	{
		return MultiByteToWideChar (CP_UTF8, 0, chU8, -1, NULL, 0);
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	int WinU16_from_UTF8 (WCHAR *wcU16, int sizeU16, const char *chU8)
	{
		return MultiByteToWideChar (CP_UTF8, 0, chU8, -1, wcU16, sizeU16);
	}
#endif

int reqWinU16wcharsFileName (const char *ccU8FileName)
{
	if (!strncmp (ccU8FileName, ccLongFileNamePrefix, LENOF_LONGFILENAMEPREFIX))
	{
		return reqWinU16wchars (ccU8FileName);
	} else
	{
		return reqWinU16wchars (ccU8FileName) + (int) LENOF_LONGFILENAMEPREFIX;
	}
}

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	BOOL HasLongFileNamePrefixW (const WCHAR *wcFileName)
	{
		ASSERT (NULL != wcFileName);
		if (wcFileName)
		{
			return !wcsncmp (wcFileName, wcLongFileNamePrefix, LENOF_LONGFILENAMEPREFIX);
		}
		return FALSE;
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	BOOL HasLongFileNamePrefixU8 (const char *ccFileName)
	{
		ASSERT (NULL != ccFileName);
		if (ccFileName)
		{
			return !strncmp (ccFileName, ccLongFileNamePrefix, LENOF_LONGFILENAMEPREFIX);
		}
		return FALSE;
	}
#endif

int WinU16_from_UTF8_FileName (WCHAR *wcU16FileName, int sizeU16, const char *ccU8FileName)
{
	if (!strncmp (ccU8FileName, ccLongFileNamePrefix, LENOF_LONGFILENAMEPREFIX))
	{
		return WinU16_from_UTF8 (wcU16FileName, sizeU16, ccU8FileName);
	} else
	{
		ASSERT (LENOF_LONGFILENAMEPREFIX == LENOFLONGFILENAMEPREFIX);
		if (sizeU16 > (int) LENOF_LONGFILENAMEPREFIX)
		{
			int	iRet;

			wcsncpy_s (wcU16FileName, sizeU16, wcLongFileNamePrefix, _TRUNCATE);
			iRet = WinU16_from_UTF8	(
						wcU16FileName + LENOF_LONGFILENAMEPREFIX,
						sizeU16 - LENOF_LONGFILENAMEPREFIX,
						ccU8FileName
									)
					+ LENOF_LONGFILENAMEPREFIX;
			forward_to_backslashW (wcU16FileName, sizeU16);
			return iRet;
		}
		// Output buffer too short.
		return 0;
	}
}

WCHAR *AllocWinU16_from_UTF8 (const char *ccU8)
{
	WCHAR	*pwc	= NULL;
	
	if (ccU8)
	{
		int	iRequiredSize = reqWinU16wchars (ccU8);
		pwc = ubf_malloc (iRequiredSize * sizeof (WCHAR));
		if (pwc)
		{
			WinU16_from_UTF8 (pwc, iRequiredSize, ccU8);
			return pwc;
		}
	}
	return NULL;
}

WCHAR *AllocWinU16_from_UTF8_00 (const char *ccU8)
{
	WCHAR	*pwc	= NULL;
	
	if (ccU8)
	{
		int	iRequiredSize = reqWinU16wchars (ccU8) + 1;
		pwc = ubf_malloc (iRequiredSize * sizeof (WCHAR));
		if (pwc)
		{
			WinU16_from_UTF8 (pwc, iRequiredSize, ccU8);
			pwc [iRequiredSize - 1] = ASCII_NUL;
			return pwc;
		}
	}
	return NULL;
}

WCHAR *AllocWinU16_from_UTF8_FileName (const char *ccU8FileName)
{
	WCHAR	*pwc	= NULL;
	
	if (ccU8FileName)
	{
		int	iRequiredSize = reqWinU16wcharsFileName (ccU8FileName);
		pwc = ubf_malloc (iRequiredSize * sizeof (WCHAR));
		if (pwc)
		{
			WinU16_from_UTF8_FileName (pwc, iRequiredSize, ccU8FileName);
		}
	}
	return pwc;
}

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void DoneWinU16 (WCHAR *pwc)
	{
		if (pwc)
			ubf_free (pwc);
	}
#endif

char *AllocU8_from_WinU16 (const WCHAR *wc16)
{
	char	*pc	= NULL;
	
	if (wc16)
	{
		int	iRequiredSize = reqUTF8size (wc16);
		pc = ubf_malloc (iRequiredSize);
		if (pc)
		{
			UTF8_from_WinU16 (pc, iRequiredSize, wc16);
		}
	}
	return pc;
}

#ifdef HAVE_STRWILDCARDS
	char *AllocU8path_from_U8path_and_WinU16FileName (const char *ccPath, WCHAR *wcFileName)
	{
		ASSERT (NULL != ccPath);
		ASSERT (NULL != wcFileName);
	
		if (NULL == ccPath || NULL == wcFileName)
			return NULL;
		
		size_t		stPath		= strlen (ccPath);
		const char	*ccWCfname;
		size_t		stWCflen;										// Length of the wildcard
																	//	filename, if there is
																	//	one.
		char		*ccRet		= NULL;
		int			iReq;

		// Must not be an empty string or end with a directory separator.
		ASSERT (0 < stPath);										// Not supported.
		ASSERT ('\\' != ccPath [stPath - 1]);						// Not supported.
		ASSERT ('/' != ccPath [stPath - 1]);						// Not supported.
		if (0 == stPath)
			return NULL;
		ccWCfname = wildCardFileName (&stWCflen, ccPath);
		iReq = reqUTF8size (wcFileName);
		if (NULL == ccWCfname)
		{	// We got no wildcards in the filename. This is a simple copy operation.
			ccRet = ubf_malloc (stPath + iReq + 2);
			if (ccRet)
			{
				memcpy (ccRet, ccPath, stPath);
				memcpy (ccRet + stPath, "\\", 1);
				UTF8_from_WinU16 (ccRet + stPath + 1, iReq, wcFileName);
			}
			return ccRet;
		}
		// Note that iReq contains the NUL terminator already but to be sure we add
		//	2 octets more.
		ccRet = ubf_malloc (stPath + stWCflen + iReq + 2);
		if (ccRet)
		{
			if (ubf_isdirseparator (ccWCfname [0]))
				stPath -= stWCflen - 1;
			else
				stPath -= stWCflen;
			memcpy (ccRet, ccPath, stPath);
			UTF8_from_WinU16 (ccRet + stPath, iReq, wcFileName);
			/*
			if (ccWCfname && stWCflen)
			{	// We got a mask that needs to be added.
				memcpy (ccRet + stPath + iReq - 1, ccWCfname, stWCflen + 1);
			}
			*/
			forward_to_backslashU8 (ccRet, iReq);
		}
		return ccRet;
	}
#endif

#if defined (HAVE_STRWILDCARDS) && defined (HAVE_MEMBUF)
	bool U8pathSMEMBUF_from_U8path_and_WinU16FileName	(
					SMEMBUF				*pmbResult,
					const char			*ccPath,
					size_t				lnPath,
					WCHAR				*wcFileName
														)
	{
		ASSERT (isInitialisedSMEMBUF (pmbResult));
		ASSERT (NULL != ccPath);
		ASSERT (NULL != wcFileName);

		if (NULL == ccPath || NULL == wcFileName)
			return false;

		size_t		stPath		= USE_STRLEN == lnPath ? strlen (ccPath) : lnPath;
		const char	*ccWCfname;
		size_t		stWCflen;										// Length of the wildcard
																	//	filename, if there is
																	//	one.
		int			iReq;

		// Must not be an empty string or end with a directory separator.
		ASSERT (0 < stPath);										// Not supported.
		ASSERT ('\\' != ccPath [stPath - 1]);						// Not supported.
		ASSERT ('/' != ccPath [stPath - 1]);						// Not supported.
		if (0 == stPath)
			return false;
		ccWCfname = wildCardFileName (&stWCflen, ccPath);
		iReq = reqUTF8size (wcFileName);
		if (NULL == ccWCfname)
		{	// We got no wildcards in the filename. This is a simple copy operation.
			//ccRet = ubf_malloc (stPath + iReq + 2);
			growToSizeSMEMBUF (pmbResult, stPath + iReq + 2);
			if (isUsableSMEMBUF (pmbResult))
			{
				memcpy (pmbResult->buf.pch, ccPath, stPath);
				memcpy (pmbResult->buf.pch + stPath, "\\", 1);
				UTF8_from_WinU16 (pmbResult->buf.pch + stPath + 1, iReq, wcFileName);
				return true;
			}
			return false;
		}
		// Note that iReq contains the NUL terminator already but to be sure we add
		//	2 octets more.
		growToSizeSMEMBUF (pmbResult, stPath + stWCflen + iReq + 2);
		if (isUsableSMEMBUF (pmbResult))
		{
			stPath -= stWCflen - 1;								// We keep the dir separator.
			memcpy (pmbResult->buf.pch, ccPath, stPath);
			UTF8_from_WinU16 (pmbResult->buf.pch + stPath, iReq, wcFileName);
			if (ccWCfname && stWCflen)
			{	// We got a mask that needs to be added.
				memcpy (pmbResult->buf.pch + stPath + iReq - 1, ccWCfname, stWCflen + 1);
			}
			forward_to_backslashU8 (pmbResult->buf.pch, iReq);
			return true;
		}
		return false;
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void DoneU8 (char *pch)
	{
		if (pch)
			ubf_free (pch);
	}
#endif

WCHAR *AllocWinU16fromU8orUseThreshold (WCHAR *pwcStackVar, const char *pchU8)
{
	WCHAR	*pwcRet	= NULL;
	
	if (pchU8)
	{
		int		iLen;
		
		iLen = reqWinU16wchars (pchU8);
		if (iLen <= WINAPI_U8_HEAP_THRESHOLD)
		{
			WinU16_from_UTF8 (pwcStackVar, iLen, pchU8);
			pwcRet = pwcStackVar;
		} else
		{
			pwcRet = AllocWinU16_from_UTF8 (pchU8);
		}
	}
	return pwcRet;
}

WCHAR *AllocWinU16fromU8orUseThreshold_00 (WCHAR *pwcStackVar, const char *pchU8)
{
	WCHAR	*pwcRet	= NULL;
	
	if (pchU8)
	{
		int		iLen;
		
		iLen = reqWinU16wchars (pchU8) + 1;
		if (iLen <= WINAPI_U8_HEAP_THRESHOLD)
		{
			WinU16_from_UTF8 (pwcStackVar, iLen, pchU8);
			pwcStackVar [iLen - 1] = ASCII_NUL;
			pwcRet = pwcStackVar;
		} else
		{
			pwcRet = AllocWinU16_from_UTF8_00 (pchU8);
		}
	}
	return pwcRet;
}

WCHAR *AllocWinU16fromU8orUseThresholdLongFileName (WCHAR *pwcStackVar, const char *pchU8)
{
	int		iLen;
	WCHAR	*pwcRet	= NULL;
	
	if (pchU8)
	{
		iLen = reqWinU16wcharsFileName (pchU8);
		if (iLen <= WINAPI_U8_HEAP_THRESHOLD)
		{
			WinU16_from_UTF8_FileName (pwcStackVar, iLen, pchU8);
			pwcRet = pwcStackVar;
		} else
		{
			pwcRet = AllocWinU16_from_UTF8_FileName (pchU8);
		}
	}
	return pwcRet;
}

void DoneWinU16fromU8orUseThreshold (WCHAR *pwcHeapVar, WCHAR *pwcStackVar)
{
	ASSERT (NULL != pwcStackVar);
	if (pwcHeapVar && pwcHeapVar != pwcStackVar)
		DoneWinU16 (pwcHeapVar);
}

char **AllocU8ArgsFromWinU16 (int argc, WCHAR *argvw [])
{
	char **arr;
	
	arr = ubf_malloc (argc * sizeof (char *));
	if (arr)
	{
		int i = 0;
		while (i < argc)
		{
			arr [i] = AllocU8_from_WinU16 (argvw [i]);
			++ i;
		}
	}
	return arr;
}

void DoneU8Args (int argc, char *args [])
{
	char **arr = args;
	int i;
	
	for (i = 0; i < argc; ++ i)
	{
		DoneU8 (args [i]);
	}
	ubf_free (arr);
}

#ifdef HAVE_SHELLAPI
	#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
		WCHAR **CmdLineArgsW (int *nArgs)
		{
			return CommandLineToArgvW (GetCommandLineW (), nArgs);
		}
	#endif
#endif

BOOL CopyFileU8(
	LPCSTR	lpExistingFileNameU8,
	LPCSTR	lpNewFileNameU8,
	BOOL	bFailIfExists
)
{
	WCHAR	wcExisting	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcNewFile	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcExisting;
	WCHAR	*pwcNewFile;
	BOOL	bRet		= FALSE;

	pwcExisting	= AllocWinU16fromU8orUseThreshold (wcExisting, lpExistingFileNameU8);
	pwcNewFile	= AllocWinU16fromU8orUseThreshold (wcNewFile, lpNewFileNameU8);
	bRet = CopyFileW (pwcExisting, pwcNewFile, bFailIfExists);
	DoneWinU16fromU8orUseThreshold (pwcExisting, wcExisting);
	DoneWinU16fromU8orUseThreshold (pwcNewFile, wcNewFile);
	return bRet;
}

BOOL CopyFileU8long(
	LPCSTR	lpExistingFileNameU8,
	LPCSTR	lpNewFileNameU8,
	BOOL	bFailIfExists
)
{
	WCHAR	wcExisting	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcNewFile	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcExisting;
	WCHAR	*pwcNewFile;
	BOOL	bRet		= FALSE;
	
	pwcExisting	= AllocWinU16fromU8orUseThresholdLongFileName (wcExisting, lpExistingFileNameU8);
	pwcNewFile	= AllocWinU16fromU8orUseThresholdLongFileName (wcNewFile, lpNewFileNameU8);
	bRet = CopyFileW (pwcExisting, pwcNewFile, bFailIfExists);
	DoneWinU16fromU8orUseThreshold (pwcExisting, wcExisting);
	DoneWinU16fromU8orUseThreshold (pwcNewFile, wcNewFile);
	return bRet;
}

BOOL CreateAllFoldersW (WCHAR *wcPath, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	DWORD	len						= (DWORD) wcslen (wcPath);

	if (len && len < WINAPI_U8_HEAP_THRESHOLD)
	{
		WCHAR	wcPathToCreate [WINAPI_U8_HEAP_THRESHOLD];
		memcpy (wcPathToCreate, wcPath, ((size_t) len + 1) * sizeof (WCHAR));

		WCHAR	*wc = wcPathToCreate;
		if (len > (sizeof (wcLongFileNamePrefix) - sizeof (WCHAR)) / sizeof (WCHAR))
		{
			// Ignore the long path prefix if we got one.
			if (!memcmp (wc, wcLongFileNamePrefix, sizeof (wcLongFileNamePrefix) - sizeof (WCHAR)))
			{
				wc += (sizeof (wcLongFileNamePrefix) - sizeof (WCHAR)) / sizeof (WCHAR);
				len -= (sizeof (wcLongFileNamePrefix) - sizeof (WCHAR)) / sizeof (WCHAR);
			}
		}
		// We don't want to call createDirectoryW on "D:" or "\\server".
		BOOL	bIgnoreFirst	= FALSE;
		if (len > 2)
		{
			if (!memcmp (wc, L"\\\\", 4))
			{	// Network resource "\\server".
				wc += 2;
				//len -= 2;
				bIgnoreFirst = TRUE;
			} else
			if (!memcmp (wc + 1, L":", sizeof (WCHAR)))
			{	// Root drive "D:"
				wc += 2;
				//len -= 2;
				bIgnoreFirst = TRUE;
			}
		}
		while (*wc)
		{
			while (*wc && L'\\' != *wc)
				++ wc;
			if (*wc)
			{
				*wc = L'\0';
				if (bIgnoreFirst)
				{
					bIgnoreFirst = FALSE;
				} else
				{
					CreateDirectoryIfNotExistsW (wcPathToCreate, lpSecurityAttributes);
				}
				*wc = L'\\';
				++ wc;
			}
		}
		return CreateDirectoryIfNotExistsW (wcPathToCreate, lpSecurityAttributes);
	}
	return FALSE;
}

BOOL CreateAllFoldersU8 (const char *szPath, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	WCHAR	wcPath	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcPath;
	BOOL	bRet		= FALSE;
	
	pwcPath	= AllocWinU16fromU8orUseThresholdLongFileName (wcPath, szPath);
	bRet = CreateAllFoldersW (pwcPath, lpSecurityAttributes);
	DoneWinU16fromU8orUseThreshold (pwcPath, wcPath);
	return bRet;
}

BOOL CreateDirectoryIfNotExistsW (WCHAR *wcDir, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	BOOL bRet = CreateDirectoryW (wcDir, lpSecurityAttributes);
	if (bRet)
	{
		return TRUE;
	} else
	{	// We want to return TRUE if it already exists.
		DWORD dwErr = GetLastError ();
		if (ERROR_ALREADY_EXISTS == dwErr)
			return TRUE;
	}
	return FALSE;
}

BOOL CreateDirectoryIfNotExistsU8 (const char *ccDirU8, LPSECURITY_ATTRIBUTES lpSecurityAttributes)
{
	BOOL bRet = CreateDirectoryU8 (ccDirU8, lpSecurityAttributes);
	if (bRet)
	{
		return TRUE;
	} else
	{	// We want to return TRUE if it already exists.
		DWORD dwErr = GetLastError ();
		if (ERROR_ALREADY_EXISTS == dwErr)
			return TRUE;
	}
	return FALSE;
}

BOOL CreateDirectoryU8(
  LPCSTR                lpPathNameU8,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	WCHAR	wcPathName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcPathName;
	DWORD	dwRet		= FALSE;

	pwcPathName = AllocWinU16fromU8orUseThreshold (wcPathName, lpPathNameU8);
	dwRet = CreateDirectoryW (pwcPathName, lpSecurityAttributes);
	DoneWinU16fromU8orUseThreshold (pwcPathName, wcPathName);
	return dwRet;
}

BOOL CreateDirectoryU8long(
  LPCSTR                lpPathNameU8,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes
)
{
	WCHAR	wcPathName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcPathName;
	DWORD	dwRet		= FALSE;

	pwcPathName = AllocWinU16fromU8orUseThresholdLongFileName (wcPathName, lpPathNameU8);
	dwRet = CreateDirectoryW (pwcPathName, lpSecurityAttributes);
	DoneWinU16fromU8orUseThreshold (pwcPathName, wcPathName);
	return dwRet;
}

HANDLE CreateEventU8(
  LPSECURITY_ATTRIBUTES lpEventAttributes,
  BOOL                  bManualReset,
  BOOL                  bInitialState,
  LPCSTR                lpName
)
{
	if (lpName)
	{	// See
		//	https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventw .
		//	The name is limited to MAX_PATH characters.
		int iRequiredU16size = reqWinU16wcharsFileName (lpName);
		if (iRequiredU16size > MAX_PATH)
		{
			return NULL;
		}
		WCHAR	wcName [MAX_PATH + 1];
		WinU16_from_UTF8 (wcName, MAX_PATH + 1, lpName);
		return CreateEventW (lpEventAttributes, bManualReset, bInitialState, wcName);
	}
	return CreateEventW (lpEventAttributes, bManualReset, bInitialState, NULL);
}

HANDLE CreateEventExU8(
  LPSECURITY_ATTRIBUTES lpEventAttributes,
  LPCSTR                lpName,
  DWORD                 dwFlags,
  DWORD                 dwDesiredAccess
)
{
	if (lpName)
	{	// See
		//	https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createeventw .
		//	The name is limited to MAX_PATH characters.
		int iRequiredU16size = reqWinU16wcharsFileName (lpName);
		if (iRequiredU16size > MAX_PATH)
		{
			return NULL;
		}
		WCHAR	wcName [MAX_PATH + 1];
		WinU16_from_UTF8 (wcName, MAX_PATH + 1, lpName);
		return CreateEventExW (lpEventAttributes, wcName, dwFlags, dwDesiredAccess);
	}
	return CreateEventExW (lpEventAttributes, NULL, dwFlags, dwDesiredAccess);
}

HANDLE CreateFileMappingU8(
  HANDLE                hFile,
  LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
  DWORD                 flProtect,
  DWORD                 dwMaximumSizeHigh,
  DWORD                 dwMaximumSizeLow,
  LPCSTR                lpName
)
{
	if (lpName)
	{
		WCHAR	wcName [1024];
		WinU16_from_UTF8 (wcName, 1024, lpName);
		return CreateFileMappingW	(
					hFile, lpFileMappingAttributes, flProtect,
					dwMaximumSizeHigh, dwMaximumSizeLow, wcName
									);
	}
	return CreateFileMappingW	(
				hFile, lpFileMappingAttributes, flProtect,
				dwMaximumSizeHigh, dwMaximumSizeLow, NULL
								);
}

HANDLE CreateFileU8(
  LPCSTR                lpFileNameU8,
  DWORD                 dwDesiredAccess,
  DWORD                 dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD                 dwCreationDisposition,
  DWORD                 dwFlagsAndAttributes,
  HANDLE                hTemplateFile
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	HANDLE	hRet;
	
	pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	hRet = CreateFileW	(
			pwcFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
						);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return hRet;
}

HANDLE CreateFileU8long(
  LPCSTR                lpFileNameU8,
  DWORD                 dwDesiredAccess,
  DWORD                 dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD                 dwCreationDisposition,
  DWORD                 dwFlagsAndAttributes,
  HANDLE                hTemplateFile
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	HANDLE	hRet;
	
	pwcFileName = AllocWinU16fromU8orUseThresholdLongFileName (wcFileName, lpFileNameU8);
	hRet = CreateFileW	(
			pwcFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile
						);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return hRet;
}

void STARTUPINFOW_from_STARTUPINFOA (STARTUPINFOW *siw, STARTUPINFOA *sia)
{
	ASSERT (NULL != siw);
	ASSERT (NULL != sia);
	ASSERT (NULL == sia->lpReserved);
	ASSERT (0 == sia->cbReserved2);
	ASSERT (NULL == sia->lpReserved2);
	ASSERT (sizeof (STARTUPINFOW) == sizeof (STARTUPINFOA));
	siw->cb					= sia->cb;
	ASSERT (NULL == sia->lpReserved);
	siw->lpReserved			= NULL;
	siw->lpDesktop			= NULL;
	siw->lpTitle			= NULL;
	siw->dwX				= sia->dwX;
	siw->dwY				= sia->dwY;
	siw->dwXSize			= sia->dwXSize;
	siw->dwYSize			= sia->dwYSize;
	siw->dwXCountChars		= sia->dwXCountChars;
	siw->dwYCountChars		= sia->dwYCountChars;
	siw->dwFillAttribute	= sia->dwFillAttribute;
	siw->dwFlags			= sia->dwFlags;
	siw->wShowWindow		= sia->wShowWindow;
	siw->cbReserved2		= sia->cbReserved2;
	siw->hStdInput			= sia->hStdInput;
	siw->hStdOutput			= sia->hStdOutput;
	siw->hStdError			= sia->hStdError;
	if (sia->lpDesktop)
	{
		siw->lpDesktop = AllocWinU16_from_UTF8 (sia->lpDesktop);
	}
	if (sia->lpTitle)
	{
		siw->lpTitle = AllocWinU16_from_UTF8 (sia->lpTitle);
	}
}

void DoneSTARTUPINFOW (STARTUPINFOW *siw)
{
	ASSERT (NULL != siw);
	ASSERT (NULL == siw->lpReserved);
	ubf_free_accept_NULL (siw->lpDesktop);
	ubf_free_accept_NULL (siw->lpTitle);
}

BOOL CloseHandleAndInvalidate (HANDLE *phHandle)
{
	ASSERT (NULL != phHandle);
	if (INVALID_HANDLE_VALUE != *phHandle)
	{
		BOOL b = CloseHandle (*phHandle);
		*phHandle = INVALID_HANDLE_VALUE;
		return b;
	}
	return FALSE;
}

HANDLE CreateMutexU8(
  LPSECURITY_ATTRIBUTES lpMutexAttributes,
  BOOL                  bInitialOwner,
  LPCSTR                lpName
)
{
	WCHAR	wcName [MAX_PATH + 1];
	
	WinU16_from_UTF8 (wcName, MAX_PATH + 1, lpName);
	return CreateMutexW (lpMutexAttributes, bInitialOwner, wcName);
}

HANDLE CreateMutexExU8(
  LPSECURITY_ATTRIBUTES lpMutexAttributes,
  LPCSTR                lpName,
  DWORD                 dwFlags,
  DWORD                 dwDesiredAccess
)
{
	WCHAR	wcName [MAX_PATH + 1];
	
	WinU16_from_UTF8 (wcName, MAX_PATH + 1, lpName);
	return CreateMutexExW (lpMutexAttributes, wcName, dwFlags, dwDesiredAccess);
}

/*
	This is the MyCreatePipeEx () function originally found at
	http://www.davehart.net/remote/PipeEx.c .

	Taken from https://web.archive.org/web/20150125081922/http://www.davehart.net:80/remote/PipeEx.c .

	The function has been modified for UTF-16. The ID of the current thread is additionally
	part of the pipe name to make it safe for multi-threading.
*/

static volatile LONG PipeSerialNumber;

BOOL
APIENTRY
MyCreatePipeEx(
    OUT LPHANDLE lpReadPipe,
    OUT LPHANDLE lpWritePipe,
    IN LPSECURITY_ATTRIBUTES lpPipeAttributes,
    IN DWORD nSize,
    DWORD dwReadMode,
    DWORD dwWriteMode
    )

/*++

Routine Description:

    The CreatePipeEx API is used to create an anonymous pipe I/O device.
    Unlike CreatePipe FILE_FLAG_OVERLAPPED may be specified for one or
    both handles.
    Two handles to the device are created.  One handle is opened for
    reading and the other is opened for writing.  These handles may be
    used in subsequent calls to ReadFile and WriteFile to transmit data
    through the pipe.

Arguments:

    lpReadPipe - Returns a handle to the read side of the pipe.  Data
        may be read from the pipe by specifying this handle value in a
        subsequent call to ReadFile.

    lpWritePipe - Returns a handle to the write side of the pipe.  Data
        may be written to the pipe by specifying this handle value in a
        subsequent call to WriteFile.

    lpPipeAttributes - An optional parameter that may be used to specify
        the attributes of the new pipe.  If the parameter is not
        specified, then the pipe is created without a security
        descriptor, and the resulting handles are not inherited on
        process creation.  Otherwise, the optional security attributes
        are used on the pipe, and the inherit handles flag effects both
        pipe handles.

    nSize - Supplies the requested buffer size for the pipe.  This is
        only a suggestion and is used by the operating system to
        calculate an appropriate buffering mechanism.  A value of zero
        indicates that the system is to choose the default buffering
        scheme.

Return Value:

    TRUE - The operation was successful.

    FALSE/NULL - The operation failed. Extended error status is available
        using GetLastError.

--*/

{
	ASSERT (NULL != lpReadPipe);
	ASSERT (NULL != lpWritePipe);

    HANDLE ReadPipeHandle, WritePipeHandle;
    DWORD dwError;
    WCHAR PipeNameBuffer[ MAX_PATH ];

    //
    // Only one valid OpenMode flag - FILE_FLAG_OVERLAPPED
    //

    if ((dwReadMode | dwWriteMode) & (~FILE_FLAG_OVERLAPPED)) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }

    //
    //  Set the default timeout to 120 seconds
    //

    if (nSize == 0) {
        nSize = 4096;
        }

	// There's certainly a race condition here but even if this happens,
	//	we still got our unique thread ID, hence it's practically impossible to accidentally
	//	create two pipes with the same name.
	InterlockedIncrement (&PipeSerialNumber);
    swprintf( PipeNameBuffer, MAX_PATH,
				L"\\\\.\\Pipe\\culRWinAPI_U8.%08x.%08x.%08x",
				GetCurrentProcessId (),
				GetCurrentThreadId (),
				(ULONG) PipeSerialNumber
           );

    ReadPipeHandle = CreateNamedPipeW(
                         PipeNameBuffer,
                         PIPE_ACCESS_INBOUND | dwReadMode,
                         PIPE_TYPE_BYTE | PIPE_WAIT,
                         1,             // Number of pipes
                         nSize,         // Out buffer size
                         nSize,         // In buffer size
                         120 * 1000,    // Timeout in ms
                         lpPipeAttributes
                         );

    if (! ReadPipeHandle) {
        return FALSE;
    }

    WritePipeHandle = CreateFileW(
                        PipeNameBuffer,
                        GENERIC_WRITE,
                        0,                         // No sharing
                        lpPipeAttributes,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | dwWriteMode,
                        NULL                       // Template file
                      );

    if (INVALID_HANDLE_VALUE == WritePipeHandle) {
        dwError = GetLastError();
        CloseHandle( ReadPipeHandle );
        SetLastError(dwError);
        return FALSE;
    }

    *lpReadPipe = ReadPipeHandle;
    *lpWritePipe = WritePipeHandle;
    return( TRUE );
}

BOOL CreatePipeOverlapped(
		PHANDLE					hReadPipe,
		bool					bOverlappedRead,
		PHANDLE					hWritePipe,
		bool					bOverlappedWrite,
		LPSECURITY_ATTRIBUTES	lpPipeAttributes,
		DWORD					nSize
)
{
	DWORD dwReadMode	= bOverlappedRead	? FILE_FLAG_OVERLAPPED : 0;
	DWORD dwWriteMode	= bOverlappedWrite	? FILE_FLAG_OVERLAPPED : 0; 
	BOOL b = MyCreatePipeEx	(
				hReadPipe, hWritePipe,
				lpPipeAttributes, nSize,
				dwReadMode, dwWriteMode
							);
	return b;
}

BOOL CreateProcessU8EnvW(
  LPCSTR                lpApplicationName,
  LPSTR                 lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL                  bInheritHandles,
  DWORD                 dwCreationFlags,
  LPVOID                lpEnvironment,
  LPCSTR                lpCurrentDirectory,
  LPSTARTUPINFOA        lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
)
{
	ASSERT (NULL != lpStartupInfo);
	
	WCHAR	*wcApplicationName		= NULL;
	WCHAR	*wcCommandLine			= NULL;
	WCHAR	*wcCurrentDirectory		= NULL;
	if (lpApplicationName)
		wcApplicationName	= AllocWinU16_from_UTF8 (lpApplicationName);
	if (lpCommandLine)
		wcCommandLine		= AllocWinU16_from_UTF8 (lpCommandLine);
	if (lpCurrentDirectory)
		wcCurrentDirectory	= AllocWinU16_from_UTF8_FileName (lpCurrentDirectory);
	STARTUPINFOW	sw;
	STARTUPINFOW	*psw;
	if (dwCreationFlags & EXTENDED_STARTUPINFO_PRESENT)
	{
		STARTUPINFOA	*psa;
		STARTUPINFOEXW	swx;
		psa = &((STARTUPINFOEXA *) lpStartupInfo)->StartupInfo;
		swx.lpAttributeList = ((STARTUPINFOEXA *) lpStartupInfo)->lpAttributeList;
		STARTUPINFOW_from_STARTUPINFOA (&swx.StartupInfo, psa);
		// Pointer for CreateProcessW (). It is happy with the (STARTUPINFOW *)
		//	although we know it's actually a (STARTUPINFOEXW *).
		psw = (STARTUPINFOW *) &swx;
	} else
	{
		STARTUPINFOW_from_STARTUPINFOA (&sw, lpStartupInfo);
		psw = &sw;		
	}
	BOOL	bRet;
	bRet = CreateProcessW	(
				wcApplicationName,
				wcCommandLine,
				lpProcessAttributes,
				lpThreadAttributes,
				bInheritHandles,
				dwCreationFlags,
				lpEnvironment,
				wcCurrentDirectory,
				psw,
				lpProcessInformation
							);
	if (dwCreationFlags & EXTENDED_STARTUPINFO_PRESENT)
		DoneSTARTUPINFOW (&((STARTUPINFOEXW *) psw)->StartupInfo);
	else
		DoneSTARTUPINFOW (psw);
	DoneWinU16 (wcApplicationName);
	DoneWinU16 (wcCommandLine);
	DoneWinU16 (wcCurrentDirectory);
	return bRet;
}

BOOL CreateProcessU8(
  LPCSTR                lpApplicationName,
  LPSTR                 lpCommandLine,
  LPSECURITY_ATTRIBUTES lpProcessAttributes,
  LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL                  bInheritHandles,
  DWORD                 dwCreationFlags,
  LPVOID                lpEnvironment,
  LPCSTR                lpCurrentDirectory,
  LPSTARTUPINFOA        lpStartupInfo,
  LPPROCESS_INFORMATION lpProcessInformation
)
{
	ASSERT (NULL != lpStartupInfo);

	if (lpEnvironment)
	{
		WCHAR *wcEnv = CreateWinU16EnvironmentFromU8 (lpEnvironment);
		BOOL	bRet;
	
		bRet = CreateProcessU8EnvW	(
					lpApplicationName,
					lpCommandLine,
					lpProcessAttributes,
					lpThreadAttributes,
					bInheritHandles,
					dwCreationFlags,
					wcEnv,
					lpCurrentDirectory,
					lpStartupInfo,
					lpProcessInformation
									);
		DoneWinU16Environment (wcEnv);
		return bRet;
	} else
	{
		return CreateProcessU8EnvW	(
					lpApplicationName,
					lpCommandLine,
					lpProcessAttributes,
					lpThreadAttributes,
					bInheritHandles,
					dwCreationFlags,
					NULL,
					lpCurrentDirectory,
					lpStartupInfo,
					lpProcessInformation
									);
	}
}

HANDLE CreateSemaphoreU8(
  LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
  LONG                  lInitialCount,
  LONG                  lMaximumCount,
  LPCSTR                lpNameU8
)
{
	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createsemaphorew .
	//	The name is limited to MAX_PATH characters.
	WCHAR wcSemName [MAX_PATH + 1];
	WinU16_from_UTF8 (wcSemName, MAX_PATH + 1, lpNameU8);
	return CreateSemaphoreW (lpSemaphoreAttributes, lInitialCount, lMaximumCount, wcSemName);
}

#ifdef HAVE_ADVAPI32
SC_HANDLE CreateServiceU8(
  SC_HANDLE hSCManager,
  LPCSTR    lpServiceName,
  LPCSTR    lpDisplayName,
  DWORD     dwDesiredAccess,
  DWORD     dwServiceType,
  DWORD     dwStartType,
  DWORD     dwErrorControl,
  LPCSTR    lpBinaryPathName,
  LPCSTR    lpLoadOrderGroup,
  LPDWORD   lpdwTagId,
  LPCSTR    lpDependencies,
  LPCSTR    lpServiceStartName,
  LPCSTR    lpPassword
)
{
	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-createservicea .
	//	The maximum string length for lpServiceName is 256 characters. The same goes for
	//	lpDisplayName. It is the caller's responsibility to adhere to these size limits.
	WCHAR	wcServiceName [512];
	WinU16_from_UTF8 (wcServiceName, 512, lpServiceName);
	WCHAR	wcDisplayName [512];
	WinU16_from_UTF8 (wcDisplayName, 512, lpDisplayName);
	WCHAR	*wcBinaryPathName	= AllocWinU16_from_UTF8 (lpBinaryPathName);
	WCHAR	*wcLoadOrderGroup	= AllocWinU16_from_UTF8 (lpLoadOrderGroup);
	WCHAR	*wcDependencies		= AllocWinU16_from_UTF8 (lpDependencies);
	WCHAR	*wcServiceStartName	= AllocWinU16_from_UTF8 (lpServiceStartName);
	WCHAR	*wcPassword			= AllocWinU16_from_UTF8 (lpPassword);
	SC_HANDLE	scReturn;
	scReturn = CreateServiceW	(
					hSCManager,
					wcServiceName,
					wcDisplayName,
					dwDesiredAccess,
					dwServiceType,
					dwStartType,
					dwErrorControl,
					wcBinaryPathName,
					wcLoadOrderGroup,
					lpdwTagId,
					wcDependencies,
					wcServiceStartName,
					wcPassword
								);
	ubf_free (wcBinaryPathName);
	ubf_free (wcLoadOrderGroup);
	ubf_free (wcDependencies);
	ubf_free (wcServiceStartName);
	ubf_free (wcPassword);
	return scReturn;
}
#endif

WCHAR **CreateWin16tringArrayFromU8 (const char **pu8, size_t numStrings)
{
	WCHAR		**pwcRet;
	
	pwcRet = ubf_malloc (sizeof (WCHAR *) * numStrings);
	if (pwcRet)
	{
		size_t n = 0;
		while (n < numStrings)
		{
			pwcRet [n] = AllocWinU16_from_UTF8 (pu8 [n]);
			++ n;
		}
	}
	return pwcRet;
}

void DoneWin16StringArray (WCHAR **pwcStringArray, size_t numStrings)
{
	size_t n = 0;
	while (n < numStrings)
	{
		ubf_free (pwcStringArray [n]);
		++ n;
	}
	ubf_free (pwcStringArray);
}

#ifdef _MSC_VER
#pragma warning (disable: 4706)									// Assignment within condition.
#endif

WCHAR *CreateWinU16EnvironmentFromU8 (const char *szU8Environment)
{
	if (szU8Environment)
	{
		const char	*chU		= szU8Environment;
		size_t		stU;
		int			stW			= 0;
		WCHAR		*wcRet;
		
		while ((stU = strlen (chU)))
		{
			stW += reqWinU16wchars (chU);
			chU += stU + 1;										// Next env variable or second NUL.
		}
		stW += 2;												// For the terminating NUL.
		wcRet = ubf_malloc (stW * sizeof (WCHAR));
		if (wcRet)
		{
			WCHAR *wc = wcRet;
			chU = szU8Environment;
			while ((stU = strlen (chU)))
			{
				stW = reqWinU16wchars (chU);
				WinU16_from_UTF8 (wc, stW, chU);
				wc += (size_t) stW + 1;
				chU += stU + 1;									// Next env variable or end.
			}
			wc [0] = L'\0';
		}
		return wcRet;
	}
	return NULL;
}

#ifdef _MSC_VER
#pragma warning (default: 4706)									// Assignment within condition.
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void DoneWinU16Environment (WCHAR *wcEnvironment)
	{
		ubf_free (wcEnvironment);
	}
#endif

#ifdef HAVE_ADVAPI32
BOOL DecryptFileU8(
  LPCSTR lpFileName,
  DWORD  dwReserved
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	BOOL	bRet;

	pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileName);
	// See
	//	https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-decryptfilew .
	//	The parameter dwReserved must be zero.
	//bRet = DecryptFileW (pwcFileName, dwReserved);
	ASSERT (0 == dwReserved);
	UNREFERENCED_PARAMETER (dwReserved);

	bRet = DecryptFileW (pwcFileName, 0);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}
#endif

BOOL DeleteFileU8(
  LPCSTR lpFileNameU8
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	BOOL	bRet;

	pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	bRet = DeleteFileW (pwcFileName);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}

BOOL DeleteFileU8long(
  LPCSTR lpFileNameU8
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	BOOL	bRet;

	pwcFileName = AllocWinU16fromU8orUseThresholdLongFileName (wcFileName, lpFileNameU8);
	bRet = DeleteFileW (pwcFileName);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}

#ifdef HAVE_ADVAPI32
BOOL EncryptFileU8(
  LPCSTR lpFileNameU8
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	BOOL	bRet;

	pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	bRet = EncryptFileW (pwcFileName);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}
#endif


#ifdef HAVE_ADVAPI32
BOOL FileEncryptionStatusU8(
  LPCSTR  lpFileNameU8,
  LPDWORD lpStatus
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	BOOL	bRet;

	pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	bRet = FileEncryptionStatusW (pwcFileName, lpStatus);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}
#endif

BOOL FileExistsU8 (const char *lpszFilenameU8)
{
	HANDLE hFile = CreateFileU8		(
						lpszFilenameU8, 0,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						NULL, OPEN_EXISTING, 0, NULL
									);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		if (GetFileAttributesU8 (lpszFilenameU8) & FILE_ATTRIBUTE_DIRECTORY)
			return FALSE;
		else
			return TRUE;
	}
	return FALSE;
}

BOOL FileExistsU8long (const char *lpszFilenameU8)
{
	HANDLE hFile = CreateFileU8long	(
						lpszFilenameU8, 0,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						NULL, OPEN_EXISTING, 0, NULL
									);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		if (GetFileAttributesU8long (lpszFilenameU8) & FILE_ATTRIBUTE_DIRECTORY)
			return FALSE;
		else
			return TRUE;
	}
	return FALSE;
}

BOOL FileOrPathExistsU8 (const char *lpszFilenameU8)
{
	HANDLE hFile = CreateFileU8	(
						lpszFilenameU8, 0,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						NULL, OPEN_EXISTING, 0, NULL
								);	
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
		return TRUE;
	} else
	{	// Maybe it's a directory.
		WIN32_FIND_DATAW	_fileEntry;
		HANDLE hDir = FindFirstFileU8 (lpszFilenameU8, &_fileEntry);
		if (INVALID_HANDLE_VALUE == hDir)
		{
		} else
		{
			FindClose (hDir);
			return TRUE;
		}
	}
	return (FALSE);
}

BOOL FileOrPathExistsU8long (const char *lpszFilenameU8)
{
	HANDLE hFile = CreateFileU8long	(
						lpszFilenameU8, 0,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						NULL, OPEN_EXISTING, 0, NULL
									);	
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
		return TRUE;
	} else
	{	// Maybe it's a directory.
		WIN32_FIND_DATAW	_fileEntry;
		HANDLE hDir = FindFirstFileU8 (lpszFilenameU8, &_fileEntry);
		if (INVALID_HANDLE_VALUE == hDir)
		{
		} else
		{
			FindClose (hDir);
			return TRUE;
		}
	}
	return (FALSE);
}

HANDLE FindFirstChangeNotificationU8(
  LPCSTR lpPathNameU8,
  BOOL   bWatchSubtree,
  DWORD  dwNotifyFilter
)
{
	WCHAR	wcPathName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcPathName;
	HANDLE	hRet;
	
	pwcPathName = AllocWinU16fromU8orUseThreshold (wcPathName, lpPathNameU8);
	hRet = FindFirstChangeNotificationW (pwcPathName, bWatchSubtree, dwNotifyFilter);
	DoneWinU16fromU8orUseThreshold (pwcPathName, wcPathName);
	return hRet;
}

HANDLE FindFirstChangeNotificationU8long(
  LPCSTR lpPathNameU8,
  BOOL   bWatchSubtree,
  DWORD  dwNotifyFilter
)
{
	WCHAR	wcPathName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcPathName;
	HANDLE	hRet;
	
	pwcPathName = AllocWinU16fromU8orUseThresholdLongFileName (wcPathName, lpPathNameU8);
	hRet = FindFirstChangeNotificationW (pwcPathName, bWatchSubtree, dwNotifyFilter);
	DoneWinU16fromU8orUseThreshold (pwcPathName, wcPathName);
	return hRet;
}

HANDLE FindFirstFileExU8(
  LPCSTR             lpFileNameU8,
  FINDEX_INFO_LEVELS fInfoLevelId,
  LPVOID             lpFindFileData,
  FINDEX_SEARCH_OPS  fSearchOp,
  LPVOID             lpSearchFilter,
  DWORD              dwAdditionalFlags
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	HANDLE	hRet;
	
	/*
		See
		https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfileexa .
		
		Excerpt from 2021-02-05:
		
		lpSearchFilter

		A pointer to the search criteria if the specified fSearchOp needs structured search
		information.
		At this time, none of the supported fSearchOp values require extended search
		information. Therefore, this pointer must be NULL.
	*/
	ASSERT (NULL == lpSearchFilter);
	pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	hRet = FindFirstFileExW	(
				pwcFileName, fInfoLevelId, lpFindFileData,
				fSearchOp, lpSearchFilter, dwAdditionalFlags
							);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return hRet;
}

HANDLE FindFirstFileExU8long(
  LPCSTR             lpFileNameU8,
  FINDEX_INFO_LEVELS fInfoLevelId,
  LPVOID             lpFindFileData,
  FINDEX_SEARCH_OPS  fSearchOp,
  LPVOID             lpSearchFilter,
  DWORD              dwAdditionalFlags
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	HANDLE	hRet;
	
	/*
		See
		https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfileexa .
		
		Excerpt from 2021-02-05:
		
		lpSearchFilter

		A pointer to the search criteria if the specified fSearchOp needs structured search
		information.
		At this time, none of the supported fSearchOp values require extended search
		information. Therefore, this pointer must be NULL.
	*/
	ASSERT (NULL == lpSearchFilter);
	pwcFileName = AllocWinU16fromU8orUseThresholdLongFileName (wcFileName, lpFileNameU8);
	hRet = FindFirstFileExW	(
				pwcFileName, fInfoLevelId, lpFindFileData,
				fSearchOp, lpSearchFilter, dwAdditionalFlags
							);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return hRet;
}

HANDLE FindFirstFileU8(
  LPCSTR             lpFileNameU8,
  LPWIN32_FIND_DATAW lpFindFileData
)
{
	return FindFirstFileExU8	(
				lpFileNameU8, FindExInfoBasic, lpFindFileData,
				FindExSearchNameMatch, NULL, FIND_FIRST_EX_CASE_SENSITIVE
								);
}

HANDLE FindFirstFileU8long(
  LPCSTR             lpFileNameU8,
  LPWIN32_FIND_DATAW lpFindFileData
)
{
	return FindFirstFileExU8long	(
				lpFileNameU8, FindExInfoBasic, lpFindFileData,
				FindExSearchNameMatch, NULL, FIND_FIRST_EX_CASE_SENSITIVE
									);
}

void WIN32_FIND_DATAU8_from_WIN32_FIND_DATAW (WIN32_FIND_DATAU8 *pwfdU8, WIN32_FIND_DATAW *pwfdW)
{
	ASSERT (NULL != pwfdU8);
	ASSERT (NULL != pwfdW);
	UTF8_from_WinU16 (pwfdU8->cFileName, UTF8_MAX_PATH, pwfdW->cFileName);
	pwfdU8->dwFileAttributes	= pwfdW->dwFileAttributes;
	pwfdU8->ftCreationTime		= pwfdW->ftCreationTime;
	pwfdU8->ftLastAccessTime	= pwfdW->ftLastAccessTime;
	pwfdU8->ftLastWriteTime		= pwfdW->ftLastWriteTime;
	pwfdU8->nFileSizeHigh		= pwfdW->nFileSizeHigh;
	pwfdU8->nFileSizeLow		= pwfdW->nFileSizeLow;
	pwfdU8->dwReserved0			= pwfdW->dwReserved0;
	pwfdU8->dwReserved1			= pwfdW->dwReserved1;
	#ifdef _MAC													// No idea. That's so in
																//	the Windows header
																//	file.
		pwfdU8->dwFileType		= pwfdW->dwFileType;
		pwfdU8->dwCreatorType	= pwfdW->dwCreatorType;
		pwfdU8->wFinderFlags	= pwfdW->wFinderFlags;
	#endif
}

void copyWIN32_FIND_DATAW (WIN32_FIND_DATAW *pwfdDest, WIN32_FIND_DATAW *pwfdSrc)
{
	ASSERT (NULL != pwfdDest);
	ASSERT (NULL != pwfdSrc);

	memcpy (pwfdDest, pwfdSrc, sizeof (WIN32_FIND_DATAW));
}

void copyWIN32_FIND_DATAU8 (WIN32_FIND_DATAU8 *pu8fdDest, WIN32_FIND_DATAU8 *pu8fdSrc)
{
	ASSERT (NULL != pu8fdDest);
	ASSERT (NULL != pu8fdSrc);

	memcpy (pu8fdDest, pu8fdSrc, sizeof (WIN32_FIND_DATAU8));
}

/*
	The skeleton for this function has been taken from
	https://github.com/Arvanaghi/Windows-DLL-Injector/blob/master/Source/DLL_Injector.c .
*/
#ifdef HAVE_KERNEL32
HANDLE FindProcessByNameU8 (const char *szProcessName)
{
	HANDLE				hProcessSnap;
	HANDLE				hProcess;
	PROCESSENTRY32		pe32;
	int					iU16size;
	
	iU16size = reqWinU16wchars (szProcessName);
	WCHAR wcProcessName [1024];
	if (iU16size > 1024)
	{
		ASSERT (FALSE);
		return FALSE;
	}
	WinU16_from_UTF8 (wcProcessName, 1024, szProcessName);
	
	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot (TH32CS_SNAPPROCESS, 0);
	if (INVALID_HANDLE_VALUE == hProcessSnap)
		return NULL;
	// Set the size of the structure before using it.
	pe32.dwSize = sizeof (PROCESSENTRY32);
	// Retrieve information about the first process,
	// and exit if unsuccessful
	if (!Process32First (hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return FALSE;
	}
	// Now walk through the snapshot.
	do
	{
		if (!wcscmp (pe32.szExeFile, wcProcessName))
		{
			//wprintf(L"[+] The process %s was found in memory.\n", pe32.szExeFile);
			hProcess = OpenProcess (PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
			return hProcess;
		}

	} while (Process32Next (hProcessSnap, &pe32));
	//printf("[---] %s has not been loaded into memory, aborting.\n", wcProcessName);
	SetLastError (ERROR_FILE_NOT_FOUND);
	return NULL;
}
#endif

BOOL GetComputerNameExU8(
  COMPUTER_NAME_FORMAT NameType,
  LPSTR                lpBuffer,
  LPDWORD              lpnSize
)
{
	BOOL	bRet	= FALSE;
	
	if (lpBuffer)
	{
		if (lpnSize)
		{
			DWORD	dwSize;
			dwSize = *lpnSize;
			WCHAR *pwc = ubf_malloc (*lpnSize * sizeof (WCHAR));
			if (pwc)
			{
				bRet = GetComputerNameExW (NameType, pwc, &dwSize);
				if (bRet)
				{
					ASSERT (*lpnSize <= INT_MAX);
					int	iReqU8size = reqUTF8size (pwc);
					if (iReqU8size > (int) *lpnSize)
					{
						SetLastError (ERROR_MORE_DATA);
						bRet = FALSE;
					} else
					{
						UTF8_from_WinU16 (lpBuffer, iReqU8size, pwc);
					}
					*lpnSize = iReqU8size ? (DWORD) iReqU8size - 1 : 0;
				} else
				{
					ASSERT (ERROR_MORE_DATA == GetLastError ());
					if (lpnSize)
						*lpnSize *= 2;
				}
				ubf_free (pwc);
			}
		} else
		{	// C6387 because lpnSize is 0. We need to let the API handle this case.
			bRet = GetComputerNameExW (NameType, NULL, lpnSize);
		}
	} else
	{
		bRet = GetComputerNameExW (NameType, NULL, lpnSize);
		if (FALSE == bRet)
		{
			ASSERT (ERROR_MORE_DATA == GetLastError ());
			if (lpnSize)
				*lpnSize *= 4;
		}
	}
	return bRet;
}

BOOL GetComputerNameU8(
  LPSTR   lpBuffer,
  LPDWORD lpnSize
)
{
	return GetComputerNameExU8 (ComputerNameNetBIOS, lpBuffer, lpnSize);
}

DWORD GetEnvironmentVariableU8(
  char		*szNameU8,
  char		*szOut,
  DWORD		nSize
)
{
	WCHAR	wcName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcName;
	DWORD	dwRet;
	WCHAR	*pwcOut		= NULL;

	pwcName = AllocWinU16fromU8orUseThreshold (wcName, szNameU8);
	if (nSize)
	{
		pwcOut = ubf_malloc (sizeof (WCHAR) * reqUTF16charsFromUTF8chars (nSize));
		dwRet = GetEnvironmentVariableW (pwcName, pwcOut, nSize);
	} else
	{
		dwRet = GetEnvironmentVariableW (pwcName, pwcOut, nSize);
		if (dwRet)
		{
			if (dwRet < nSize)
			{
				UTF8_from_WinU16 (szOut, (int) nSize, pwcOut);
				dwRet = reqUTF8size (pwcOut);
			} else
				dwRet = reqUTF8sizeFromUTF16wchars (dwRet);
		}
	}
	if (NULL != pwcOut)
		ubf_free (pwcOut);
	DoneWinU16fromU8orUseThreshold (pwcName, wcName);
	return dwRet;
}

DWORD GetFileAttributesU8(
  LPCSTR lpFileNameU8
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	DWORD	dwRet;

	pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	dwRet = GetFileAttributesW (pwcFileName);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return dwRet;
}

DWORD GetFileAttributesU8long(
  LPCSTR lpFileNameU8
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	DWORD	dwRet;

	pwcFileName = AllocWinU16fromU8orUseThresholdLongFileName (wcFileName, lpFileNameU8);
	dwRet = GetFileAttributesW (pwcFileName);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return dwRet;
}

BOOL GetFilePointerEx (LARGE_INTEGER *liCurrFilePointer, HANDLE hFile)
{
	LARGE_INTEGER	liZero;

	ASSERT (NULL != liCurrFilePointer);
	liZero.QuadPart = 0;
	BOOL b = SetFilePointerEx (hFile, liZero, liCurrFilePointer, FILE_CURRENT);
	return b;
}

#ifdef HAVE_ADVAPI32
BOOL GetFileSecurityU8(
		LPCSTR					lpFileNameU8,
		SECURITY_INFORMATION	RequestedInformation,
		PSECURITY_DESCRIPTOR	pSecurityDescriptor,
		DWORD					nLength,
		LPDWORD					lpnLengthNeeded
)
{
	WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcFileName;
	BOOL	bRet;

	pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	bRet = GetFileSecurityW (pwcFileName, RequestedInformation, pSecurityDescriptor,
							nLength, lpnLengthNeeded);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}
#endif

enum en_wapi_fs_type GetFileSystemType (const char *chDriveRoot)
{
	DWORD	MaximumComponentLength;
	DWORD	FileSystemFlags;
	char	cFileSystemName [64];

	if (GetVolumeInformationU8 (chDriveRoot,
							NULL, 0,
							NULL,
							&MaximumComponentLength,
							&FileSystemFlags,
							cFileSystemName, 64))
	{
		if (!strcmp (cFileSystemName, "NTFS"))
			return FS_TYPE_NTFS;
		if (!strcmp (cFileSystemName, "FAT"))
			return FS_TYPE_FAT;
		if (!strcmp (cFileSystemName, "FAT32"))
			return FS_TYPE_FAT32;
	}
	_ASSERT (FALSE);
	return FS_TYPE_ERROR;
}

#ifdef DEBUG
	BOOL IsFileSystemFAT (const char *chDriveRoot)
	{
		return FS_TYPE_FAT == GetFileSystemType (chDriveRoot);
	}
#endif

#ifdef DEBUG
	BOOL IsFileSystemFAT32 (const char *chDriveRoot)
	{
		return FS_TYPE_FAT32 == GetFileSystemType (chDriveRoot);
	}
#endif

#ifdef DEBUG
	BOOL IsFileSystemFATorFAT32 (const char *chDriveRoot)
	{
		return IsFileSystemFAT32 (chDriveRoot) || IsFileSystemFAT (chDriveRoot);
	}
#endif

#ifdef DEBUG
	BOOL IsFileSystemNTFS (const char *chDriveRoot)
	{
		return FS_TYPE_NTFS == GetFileSystemType (chDriveRoot);
	}
#endif

DWORD GetFullPathNameU8(
  LPCSTR lpFileName,
  DWORD  nBufferLength,
  LPSTR  lpBuffer,
  LPSTR  *lpFilePart
)
{
	int			iRequiredU16size;
	WCHAR		wcFileName [4096];
	WCHAR		*pwcFileName;
	WCHAR		*pFilePartW			= NULL;
	
	// Note that as of 2021-04-19 the documentation for GetFullPathNameW () at
	//	https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnamew
	//	is incorrect. The function does not support long filenames, at least not as the
	//	lpFileName parameter.
	/*
		Excerpt (which is wrong):

		lpFileName

		The name of the file.

		This parameter can be a short (the 8.3 form) or long file name. This string can also be
		a share or volume name.

		In the ANSI version of this function, the name is limited to MAX_PATH characters. To
		extend this limit to 32,767 wide characters, call the Unicode version of the function
		(GetFullPathNameW), and prepend "\\?\" to the path. For more information, see Naming a File.
	*/
	if (lpFileName)
	{
		//iRequiredU16size = reqWinU16wcharsFileName (lpFileName);
		iRequiredU16size = reqWinU16wchars (lpFileName);
		if (iRequiredU16size <= 4096)
		{
			WinU16_from_UTF8 (wcFileName, iRequiredU16size, lpFileName);
			pwcFileName = wcFileName;
		} else
		{
			pwcFileName = AllocWinU16_from_UTF8_FileName (lpFileName);
		}
	} else
	{
		pwcFileName			= NULL;
	}
	DWORD	dwRet;
	if (lpBuffer && nBufferLength)
	{
		WCHAR		*pwcBuffer;
		//WCHAR		*pwcToConv;
		DWORD		dwWbuflen;
		int			iU8size;
		
		// Not supported/documentation doesn't mention that relative paths cannot
		//	contain "." and "..".
		//dwWbuflen = (nBufferLength + LENOFLONGFILENAMEPREFIX) * sizeof (WCHAR);
		dwWbuflen = nBufferLength * sizeof (WCHAR);
		pwcBuffer = ubf_malloc (dwWbuflen);
		if (pwcBuffer)
		{
			//ASSERT (FALSE);
			// This was used to find out that the documentation is wrong. A long filename
			//	prefix is not supported by GetFullPathNameW ().
			//pwcFileName += 4;
			dwRet = GetFullPathNameW (pwcFileName, dwWbuflen / 2, pwcBuffer, &pFilePartW);
			/*
			// If the caller provided a long file name prefix we give it back to them.
			//	If not, we remove it from the returned path before we convert to UTF-8.
			if	(
						lpFileName
					&&	!HasLongFileNamePrefixU8 (lpFileName)
					&&	HasLongFileNamePrefixW (pwcBuffer)
				)
				pwcToConv = pwcBuffer + LENOFLONGFILENAMEPREFIX;
			else
				pwcToConv = pwcBuffer;
			*/
			iU8size = reqUTF8size (pwcBuffer);
			ASSERT (nBufferLength <= INT_MAX);
			if (iU8size < (int) nBufferLength)
			{
				if (iU8size)
				{
					UTF8_from_WinU16 (lpBuffer, iU8size, pwcBuffer);
					-- iU8size;
					ASSERT (strlen (lpBuffer) == (size_t) iU8size);
					ASSERT ((int) strlen (lpBuffer) == iU8size);
					dwRet = iU8size;
				} else
				{
					dwRet = 0;
				}
			} else
			{
				dwRet = GetFullPathNameW (pwcFileName, 0, NULL, NULL);
				dwRet *= 4;										// Up to 4 octets per WCHAR.
			}
			ubf_free (pwcBuffer);
		} else
		{
			SetLastError (ERROR_NOT_ENOUGH_MEMORY);
			dwRet = 0;
		}
	} else
	{	// C6387 because pwcFileName could be NULL. This is between our caller and the API,
		//	not us.
		dwRet = GetFullPathNameW (pwcFileName, 0, NULL, NULL);
		dwRet *= 4;												// Up to 4 octets per WCHAR.
	}
	if (pwcFileName != wcFileName)
	{
		ubf_free (pwcFileName);
	}
	if (lpFilePart)
	{
		/*
			Excerpt from
			https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getfullpathnamew :
			
			"A pointer to a buffer that receives the address (within lpBuffer) of the final
			file name component in the path.
			This parameter can be NULL.
			If lpBuffer refers to a directory and not a file, lpFilePart receives zero."

			Tests showed that GetFullPathNameW () seems to distinguish between a file and
			a directory only by checking for a trailing backslash. This is not relevant for
			us because we check pFilePartW, which is what we've passed on to
			GetFullPathNameW () ourselves.
		*/
		if (pFilePartW && *pFilePartW)
		{
			char *ccFilePart;
			ccFilePart = strrchr (lpBuffer, '\\');
			if (!ccFilePart)
				ccFilePart = strrchr (lpBuffer, '/');
			if (ccFilePart)
				++ ccFilePart;									// First octet after slash.
			*lpFilePart = ccFilePart;
		} else
		{
			*lpFilePart = 0;
		}
	}
	return dwRet;
}

DWORD GetModuleFileNameU8(
		HMODULE	hModule,
		LPSTR	lpFilenameU8,
		DWORD	nSize
)
{
	DWORD	dwRet;
	WCHAR	wcModuleName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcModuleName = wcModuleName;
	int		iu8;
	
	if (0 == nSize)
		return 0;

	dwRet = GetModuleFileNameW (hModule, pcModuleName, WINAPI_U8_HEAP_THRESHOLD);
	if (WINAPI_U8_HEAP_THRESHOLD == dwRet)
	{	// Stack buffer (WINAPI_U8_HEAP_THRESHOLD) too small.
		DWORD dwtot = WINAPI_U8_HEAP_THRESHOLD;
		do
		{
			if (pcModuleName != wcModuleName)
				ubf_free (pcModuleName);
			dwtot += WINAPI_U8_HEAP_THRESHOLD;
			pcModuleName = ubf_malloc (sizeof (WCHAR) * dwtot);
			if (NULL == pcModuleName)
				return 0;
			dwRet = GetModuleFileNameW (hModule, pcModuleName, dwtot);
		} while (dwRet == dwtot);
	}
	iu8 = reqUTF8size (pcModuleName);
	ASSERT (0 < iu8);
	if ((DWORD) iu8 < nSize)
	{	// Success.
		int iRet = UTF8_from_WinU16 (lpFilenameU8, iu8, pcModuleName);
		if (pcModuleName != wcModuleName)
			ubf_free (pcModuleName);
		ASSERT (0 < iRet);
		return (DWORD) iRet - 1;
	}
	// Buffer provided by the user too small.
	#ifdef GETMODULEFILENAMEU8_ASSERT_BUFFER_TOO_SMALL
		ASSERT (FALSE);
	#endif
	UTF8_from_WinU16 (lpFilenameU8, (int) nSize, pcModuleName);
	lpFilenameU8 [nSize - 1] = ASCII_NUL;
	if (pcModuleName != wcModuleName)
		ubf_free (pcModuleName);
	SetLastError (ERROR_INSUFFICIENT_BUFFER);
	return nSize;
}

HMODULE GetModuleHandleU8(
  LPCSTR lpModuleName
)
{
	int iModuleName = reqWinU16wchars (lpModuleName);
	if (iModuleName <= WINAPI_U8_HEAP_THRESHOLD)
	{
		WCHAR	wcModuleName [WINAPI_U8_HEAP_THRESHOLD];
		WinU16_from_UTF8 (wcModuleName, iModuleName, lpModuleName);
		return GetModuleHandleW (wcModuleName);
	}
	ASSERT (FALSE);
	return NULL;
}

typedef enum en_StringOrArray
{
	ensa_String,
	ensa_Array
} enStrOrArr;

size_t copyStrOrArrSizeInReturnValue (char *szReturned, WCHAR *pwc, DWORD nSize, enStrOrArr w)
{
	size_t	stRet;

	switch (w)
	{
		// Single string.
		case ensa_String:
			UTF8_from_WinU16 (szReturned, nSize, pwc);
			return reqUTF8size (pwc);

		// An array. That's a bit more complicated.
		case ensa_Array:
			stRet = 0;
			while (pwc [0])
			{
				stRet += reqUTF8size (pwc);					// Includes NUL.
				if (stRet <= nSize)
				{
					UTF8_from_WinU16 (szReturned, nSize, pwc);
					nSize -= (DWORD) stRet;
					szReturned += stRet;
				}
				pwc += wcslen (pwc);
				pwc ++;										// Jump over NUL.
			}
			return stRet;
	}
	// Bug.
	_ASSERT (FALSE);
	return (size_t) -1;
}

DWORD GetPrivateProfileStringU8(
	LPCSTR lpAppName,
	LPCSTR lpKeyName,
	LPCSTR lpDefault,
	LPSTR  lpReturnedString,
	DWORD  nSize,
	LPCSTR lpFileName
)
{
	WCHAR	wcAppName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcKeyName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcDefault	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcFileName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcAppName		= NULL;
	WCHAR	*pwcKeyName		= NULL;
	WCHAR	*pwcDefault		= NULL;
	WCHAR	*pwcFileName	= NULL;
	WCHAR	*pwcReturned;
	DWORD	dwRet;
	size_t	sizeRet;

	if (0 == nSize)
	{
		return 0;
	}
	ASSERT (0 < nSize);
	if (lpAppName)
		pwcAppName	= AllocWinU16fromU8orUseThreshold (wcAppName, lpAppName);
	if (lpKeyName)
		pwcKeyName	= AllocWinU16fromU8orUseThreshold (wcKeyName, lpKeyName);
	if (lpDefault)
		pwcDefault	= AllocWinU16fromU8orUseThreshold (wcDefault, lpDefault);
	// Note that the documentation does not explicitely mention a long file name
	//	prefix. See
	//	https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getprivateprofilestringw .
	if (lpFileName)
		pwcFileName	= AllocWinU16fromU8orUseThreshold (wcFileName, lpFileName);
	pwcReturned = ubf_malloc (((size_t) nSize) * 4);
	dwRet = GetPrivateProfileStringW	(
				pwcAppName, pwcKeyName, pwcDefault, pwcReturned, nSize, pwcFileName
										);
	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getprivateprofilestringw .
	/*
		If neither lpAppName nor lpKeyName is NULL and the supplied destination buffer is
		too small to hold the requested string, the string is truncated and followed by a
		null character, and the return value is equal to nSize minus one.
	*/
	if (lpAppName && lpKeyName)
	{
		sizeRet = copyStrOrArrSizeInReturnValue (lpReturnedString, pwcReturned, nSize, ensa_String);
		if (dwRet == nSize * 4 - 1)
		{
			dwRet = nSize - 1;
			lpReturnedString [dwRet - 1] = '\0';
		}
		if (sizeRet >= nSize)
		{
			dwRet = nSize - 1;
			lpReturnedString [dwRet - 1] = '\0';
		}
	} else
	/*
		If either lpAppName or lpKeyName is NULL and the supplied destination buffer is too
		small to hold all the strings, the last string is truncated and followed by two null
		characters. In this case, the return value is equal to nSize minus two.
	*/
	if (NULL == lpAppName || NULL == lpKeyName)
	{
		sizeRet = copyStrOrArrSizeInReturnValue (lpReturnedString, pwcReturned, nSize, ensa_Array);
		if (dwRet == nSize * 4 - 2)
		{
			if (nSize >= 2)
			{
				dwRet = nSize - 2;
				lpReturnedString [dwRet - 2] = '\0';
				lpReturnedString [dwRet - 1] = '\0';
			} else
			{
				lpReturnedString [dwRet - 1] = '\0';
				dwRet = 0;
			}
		}
		if (sizeRet >= nSize)
		{
			// Not implemented yet.
			ASSERT (FALSE);
		}
	}
	if (lpAppName)
		DoneWinU16fromU8orUseThreshold (pwcAppName, wcAppName);
	if (lpKeyName)
		DoneWinU16fromU8orUseThreshold (pwcKeyName, wcKeyName);
	if (lpDefault)
		DoneWinU16fromU8orUseThreshold (pwcDefault, wcDefault);
	if (lpFileName)
		DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	ubf_free (pwcReturned);
	return dwRet;
}

UINT GetSystemDirectoryU8(
	LPSTR	lpBuffer,
	UINT	uSize
)
{
	WCHAR	wcSysDir [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcSysDir = wcSysDir;

	if (uSize > INT_MAX)
		return 0;
	UINT uReqSize = GetSystemDirectoryW (pcSysDir, WINAPI_U8_HEAP_THRESHOLD);
	if (0 == uReqSize)
		return 0;
	if (uReqSize >= WINAPI_U8_HEAP_THRESHOLD)
	{
		pcSysDir = ubf_malloc (sizeof (wchar_t) * uReqSize);
		if (pcSysDir)
			GetSystemDirectoryW (pcSysDir, uReqSize);
	}

	int iReq = reqUTF8size (pcSysDir);
	if ((int) uSize >= iReq)
	{
		iReq = UTF8_from_WinU16 (lpBuffer, uSize, pcSysDir);
		if (iReq)
			-- iReq;
	}
	if (pcSysDir && pcSysDir != wcSysDir)
		ubf_free (pcSysDir);
	return (UINT) iReq;
}

static const char	*szWinSystemDirectory;
static size_t		lnWinSystemDirectory;

const char *SystemDirectoryU8 (void)
{
	if (NULL == szWinSystemDirectory)
	{
		UINT ui = GetSystemDirectoryU8 (NULL, 0);
		szWinSystemDirectory = ubf_malloc (ui);
		if (szWinSystemDirectory)
		{
			UINT u2 = GetSystemDirectoryU8 ((char *) szWinSystemDirectory, ui);
			ASSERT (ui == u2 + 1);
			lnWinSystemDirectory = u2;
		}
	}
	return szWinSystemDirectory;
}

size_t SystemDirectoryU8len (void)
{
	if (0 == lnWinSystemDirectory)
		SystemDirectoryU8 ();
	return lnWinSystemDirectory;
}

void DoneSystemDirectoryU8 (void)
{
	if (szWinSystemDirectory)
		ubf_free ((char *) szWinSystemDirectory);
	szWinSystemDirectory = NULL;
	lnWinSystemDirectory = 0;
}

UINT GetSystemWindowsDirectoryU8(
	LPSTR	lpBuffer,
	UINT	uSize
)
{
	WCHAR	wcWinDir [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcWinDir = wcWinDir;

	if (uSize > INT_MAX)
		return 0;
	UINT uReqSize = GetSystemWindowsDirectoryW (pcWinDir, WINAPI_U8_HEAP_THRESHOLD);
	if (0 == uReqSize)
		return 0;
	if (uReqSize >= WINAPI_U8_HEAP_THRESHOLD)
	{
		pcWinDir = ubf_malloc (sizeof (wchar_t) * uReqSize);
		if (pcWinDir)
			GetSystemWindowsDirectoryW (pcWinDir, uReqSize);
	}

	int iReq = reqUTF8size (pcWinDir);
	if ((int) uSize >= iReq)
	{
		iReq = UTF8_from_WinU16 (lpBuffer, uSize, pcWinDir);
		if (iReq)
			-- iReq;
	}
	if (pcWinDir && pcWinDir != wcWinDir)
		ubf_free (pcWinDir);
	return (UINT) iReq;
}

static const char	*szWinSystemWindowsDirectory;
static size_t		lnWinSystemWindowsDirectory;

const char *SystemWindowsDirectoryU8 (void)
{
	if (NULL == szWinSystemWindowsDirectory)
	{
		UINT ui = GetSystemWindowsDirectoryU8 (NULL, 0);
		szWinSystemWindowsDirectory = ubf_malloc (ui);
		if (szWinSystemWindowsDirectory)
		{
			UINT u2 = GetSystemWindowsDirectoryU8 ((char *) szWinSystemWindowsDirectory, ui);
			ASSERT (ui == u2 + 1);
			lnWinSystemWindowsDirectory = u2;
		}
	}
	return szWinSystemWindowsDirectory;
}

size_t SystemWindowsDirectoryU8len (void)
{
	if (0 == lnWinSystemWindowsDirectory)
		SystemWindowsDirectoryU8 ();
	return lnWinSystemWindowsDirectory;
}

void DoneSystemWindowsDirectoryU8 (void)
{
	if (szWinSystemWindowsDirectory)
		ubf_free ((char *) szWinSystemWindowsDirectory);
	szWinSystemWindowsDirectory = NULL;
	lnWinSystemWindowsDirectory = 0;
}

#ifdef HAVE_ADVAPI32
	BOOL GetUserNameU8(
	  LPSTR   lpBuffer,
	  LPDWORD pcbBuffer
	)
	{
		BOOL	bRet;
		DWORD	dwLen;
	
		ASSERT (NULL != lpBuffer);
		ASSERT (NULL != pcbBuffer);
		// As of 2021-01-24 and Windows 10 SDK UNLEN is 256. See
		//	https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-getusernamew
		//	for more information.
		WCHAR	wcUserName [UNLEN + 1];
		dwLen = UNLEN + 1;
		#ifdef DEBUG
			bRet = GetUserNameW (wcUserName, &dwLen);
		#else
			GetUserNameW (wcUserName, &dwLen);
		#endif
		ASSERT (FALSE != bRet);
		ASSERT (UNLEN >= dwLen);
		int iReq = reqUTF8size (wcUserName);
		ASSERT (INT_MAX > *pcbBuffer);
		if (iReq <= (int) *pcbBuffer)
		{
			UTF8_from_WinU16 (lpBuffer, iReq, wcUserName);
			bRet = TRUE;
		} else
		{
			SetLastError (ERROR_INSUFFICIENT_BUFFER);
			bRet = FALSE;
		}
		*pcbBuffer = (DWORD) iReq;
		return bRet;
	}
#endif

#ifdef HAVE_USERENV
BOOL GetDefaultUserProfileDirectoryU8(
	LPSTR   lpProfileDirU8,
	LPDWORD lpcchSize
)
{
	ASSERT (NULL != lpcchSize);

	BOOL	bRet = false;
	WCHAR	wcProfDir [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcProfDir;

	DWORD	dw = 0;
	GetDefaultUserProfileDirectoryW (NULL, &dw);
	if (dw <= WINAPI_U8_HEAP_THRESHOLD)
		pcProfDir = wcProfDir;
	else
		pcProfDir = malloc (dw * sizeof (WCHAR));
	if (pcProfDir)
	{
		BOOL b = GetDefaultUserProfileDirectoryW (pcProfDir, &dw);
		if (b)
		{
			int r = reqUTF8size (pcProfDir);
			if (*lpcchSize >= (DWORD) r)
			{
				UTF8_from_WinU16 (lpProfileDirU8, r, pcProfDir);
				bRet = true;
			}
			*lpcchSize = r;
		}
		if (wcProfDir != pcProfDir)
			free (pcProfDir);
	}
	return bRet;
}
#endif

#ifdef HAVE_USERENV
BOOL GetUserProfileDirectoryU8(
	HANDLE  hToken,
	LPSTR   lpProfileDirU8,
	LPDWORD lpcchSize
)
{
	ASSERT (NULL != lpcchSize);

	BOOL	bRet = false;
	WCHAR	wcProfDir [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcProfDir;

	DWORD	dw = 0;
	GetUserProfileDirectoryW (hToken, NULL, &dw);
	if (dw <= WINAPI_U8_HEAP_THRESHOLD)
		pcProfDir = wcProfDir;
	else
		pcProfDir = malloc (dw * sizeof (WCHAR));
	if (pcProfDir)
	{
		bool b = GetUserProfileDirectoryW (hToken, pcProfDir, &dw);
		if (b)
		{
			int r = reqUTF8size (pcProfDir);
			if (*lpcchSize >= (DWORD) r)
			{
				UTF8_from_WinU16 (lpProfileDirU8, r, pcProfDir);
				bRet = true;
			}
			*lpcchSize = r;
		}
		if (wcProfDir != pcProfDir)
			free (pcProfDir);
	}
	return bRet;
}
#endif

BOOL GetVolumeInformationU8(
  LPCSTR  lpRootPathName,
  LPSTR   lpVolumeNameBuffer,
  DWORD   nVolumeNameSize,
  LPDWORD lpVolumeSerialNumber,
  LPDWORD lpMaximumComponentLength,
  LPDWORD lpFileSystemFlags,
  LPSTR   lpFileSystemNameBuffer,
  DWORD   nFileSystemNameSize
)
{
	int		iReqSize;
	WCHAR	wcFileSystemNameBuffer [MAX_PATH + 1];				// The file system's name to query.
	BOOL	bRet		= FALSE;

	ASSERT (NULL == lpVolumeNameBuffer);						// Currently not supported.
	ASSERT (0 == nVolumeNameSize);								// Currently not supported.
	ASSERT (nFileSystemNameSize <= INT_MAX);

	UNREFERENCED_PARAMETER (lpVolumeNameBuffer);
	iReqSize = reqWinU16wchars (lpRootPathName);
	if (iReqSize)
	{
		if (iReqSize <= WINAPI_U8_HEAP_THRESHOLD)
		{
			WCHAR	wcRFN [WINAPI_U8_HEAP_THRESHOLD];
			WinU16_from_UTF8 (wcRFN, iReqSize, lpRootPathName);
			// See
			//	https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-getvolumeinformationw .
			bRet = GetVolumeInformationW	(
							wcRFN, NULL, nVolumeNameSize,
							lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags,
							wcFileSystemNameBuffer, MAX_PATH + 1
											);
		} else
		{
			ASSERT (FALSE);
			return FALSE;
		}
		if (bRet)
		{
			iReqSize = reqUTF8size (wcFileSystemNameBuffer);
			if (iReqSize <= (int) nFileSystemNameSize)
			{
				UTF8_from_WinU16 (lpFileSystemNameBuffer, iReqSize, wcFileSystemNameBuffer);
				return bRet;
			}
		}
	}
	ASSERT (FALSE);
	return FALSE;
}

BOOL GetWinErrorTextU8 (char *lpszBuf, DWORD dwSize, DWORD dwError)
{
	int		iReqSize;
	DWORD	dwRet									= 1;
	WCHAR	wcErrText [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcErrText								= wcErrText;
	DWORD	dwErrText								= WINAPI_U8_HEAP_THRESHOLD;

	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar .
	//iReqSize = MultiByteToWideChar (CP_UTF8, 0, lpRootPathName, -1, NULL, 0);

	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessagew .
	do
	{
		if (0 == dwRet)
		{
			if (pcErrText != wcErrText)
				free (pcErrText);
			ASSERT (INT_MAX < MAXDWORD);
			if (dwErrText <= INT_MAX - WINAPI_U8_HEAP_THRESHOLD)
				dwErrText += WINAPI_U8_HEAP_THRESHOLD;
			else
				return false;
			pcErrText = malloc (sizeof (WCHAR) * dwErrText);
		}
		if (pcErrText)
		{
			dwRet = FormatMessageW	(
							FORMAT_MESSAGE_FROM_SYSTEM
						|	FORMAT_MESSAGE_ARGUMENT_ARRAY
						|	FORMAT_MESSAGE_IGNORE_INSERTS,
						NULL,
						dwError,
						0,
						pcErrText,
						dwErrText - 1,
						NULL
									);
		} else
		{
			return false;
		}
	} while (0 == dwRet && ERROR_INSUFFICIENT_BUFFER == GetLastError ());
	if (dwRet > 2)
    {	// Remove the line ending (CR/LF).
		pcErrText [dwRet - 2] = L'\0';
		// Convert to UTF-8.
		iReqSize = reqUTF8size (pcErrText);
		char *szU8 = malloc (iReqSize);
		if (szU8)
		{
			UTF8_from_WinU16 (szU8, iReqSize, pcErrText);
			snprintf (lpszBuf, dwSize, "\"%s\" (Error %lu, 0%lXh)", szU8, dwError, dwError);
			free (szU8);
			if (pcErrText != wcErrText)
				free (pcErrText);
			return TRUE;
		}
    }
	_ASSERT (FALSE);										// Must be a bug.
	if (pcErrText != wcErrText)
		free (pcErrText);
	return FALSE;
}

#ifdef HAVE_ADVAPI32
	BOOL InitiateSystemShutdownExU8(
		const char *lpMachineNameU8,
		const char *lpMessageU8,
		DWORD  dwTimeout,
		BOOL   bForceAppsClosed,
		BOOL   bRebootAfterShutdown,
		DWORD  dwReason
	)
	{
		WCHAR	wcMachineName	[WINAPI_U8_HEAP_THRESHOLD];
		WCHAR	wcMessage		[WINAPI_U8_HEAP_THRESHOLD];
		WCHAR	*pwcMachineName	= NULL;
		WCHAR	*pwcMessage		= NULL;
		BOOL	bRet;
		
		pwcMachineName	= AllocWinU16fromU8orUseThreshold (wcMachineName, lpMachineNameU8);
		pwcMessage		= AllocWinU16fromU8orUseThreshold (wcMessage, lpMessageU8);
		
		bRet = InitiateSystemShutdownExW	(
					pwcMachineName, pwcMessage,
					dwTimeout, bForceAppsClosed, bRebootAfterShutdown,
					dwReason
											);
		DoneWinU16fromU8orUseThreshold (wcMachineName, pwcMachineName);
		DoneWinU16fromU8orUseThreshold (wcMessage, pwcMessage);
		return bRet;
	}
#endif
	
#ifdef HAVE_ADVAPI32
	DWORD InitiateShutdownU8(
		const char *lpMachineNameU8,
		const char *lpMessageU8,
		DWORD  dwGracePeriod,
		DWORD  dwShutdownFlags,
		DWORD  dwReason
		)
	{
		WCHAR	wcMachineName	[WINAPI_U8_HEAP_THRESHOLD];
		WCHAR	wcMessage		[WINAPI_U8_HEAP_THRESHOLD];
		WCHAR	*pwcMachineName	= NULL;
		WCHAR	*pwcMessage		= NULL;
		BOOL	bRet;
		
		pwcMachineName	= AllocWinU16fromU8orUseThreshold (wcMachineName, lpMachineNameU8);
		pwcMessage		= AllocWinU16fromU8orUseThreshold (wcMessage, lpMessageU8);
		
		bRet = InitiateShutdownW	(
					pwcMachineName, pwcMessage,
					dwGracePeriod, dwShutdownFlags,
					dwReason
									);
		DoneWinU16fromU8orUseThreshold (wcMachineName, pwcMachineName);
		DoneWinU16fromU8orUseThreshold (wcMessage, pwcMessage);
		return bRet;
	}
#endif
	
BOOL IsCmdArgumentSwitchW	(
			WCHAR							*wcArgument,
			const WCHAR						*wcConstIs,
			size_t							nRelevant,
			enum en_is_cmd_argument			enHow,
			enum en_is_cmd_case_sensitive	enCase
                            )
{
	UNREFERENCED_PARAMETER (nRelevant);
	UNREFERENCED_PARAMETER (enHow);
	UNREFERENCED_PARAMETER (enCase);

	ASSERT (NULL != wcArgument);
	ASSERT (NULL != wcConstIs);
	ASSERT (0 != wcArgument [0]);
	ASSERT (0 != wcConstIs [0]);
	if (EN_IS_CMD_ARG_ALLOW_NO_DASH_ONLY == enHow && L'-' == wcConstIs [0])
		return FALSE;

	// To do!!!
	if (wcscmp (wcArgument, wcConstIs))
	{	// We allow both options ("-delete" and "delete") if the argument
		//	is supposed to start with a "-".
		if (L'-' == wcConstIs [0])
		{
			return !wcscmp (wcArgument, wcConstIs + 1);			
		}
	} else
	{
		return TRUE;
	}
	return FALSE;
}

BOOL IsEqualUpToW	(
			const WCHAR						*wc1,
			const WCHAR						*wc2,
			size_t							n,
			enum en_is_cmd_case_sensitive	cse
					)
{
	size_t		st1 = wcslen (wc1);
	size_t		st2 = wcslen (wc2);
	
	ASSERT (st1 >= n);											// That's what the function is
																//	about. To be fixed by the
																//	caller.
	ASSERT	(		EN_IS_CMD_CASE_SENSITIVE	== cse
				||	EN_IS_CMD_CASE_INSENSITIVE	== cse
			);
	if (st1 < n)
		return FALSE;
	if (st2 < n)
		return FALSE;
	if (st1 == st2)
	{
		return	EN_IS_CMD_CASE_SENSITIVE == cse		?
				0 == wcscmp (wc1, wc2)				:			// Case-sensitive.
				0 == _wcsicmp (wc1, wc2)			;			// Case-insensitive.
	}
	if (st1 < st2)
		return FALSE;
	// String wc1 is longer than the first one.
	ASSERT (st2 >= n);											// This is the only option left
																//	here anyway.
	return 0 == wcsncmp (wc1, wc2, st2);
}

BOOL IsPathDirectoryU8 (const char *szPath)
{
	// We first try an absolute path.
	DWORD dwAttrib = GetFileAttributesU8long (szPath);
	// If this didn't work we assume that the path is relative and can't
	//	have a long filename prefix.
	if (INVALID_FILE_ATTRIBUTES == dwAttrib)
		dwAttrib = GetFileAttributesU8 (szPath);
	return	(
					(dwAttrib != INVALID_FILE_ATTRIBUTES)
				&&	(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
			);
}

BOOL IsPathDirectoryW (const WCHAR *wcPath)
{
	DWORD dwAttrib = GetFileAttributesW (wcPath);
	return	(
					(dwAttrib != INVALID_FILE_ATTRIBUTES)
				&&	(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)
			);
}

HMODULE LoadLibraryExU8(
	char	*szLibFileNameU8,
	HANDLE	hFile,
	DWORD	dwFlags
)
{
	HANDLE	hModule	= NULL;

	WCHAR	wcLibName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcLibName	= NULL;

	pwcLibName	= AllocWinU16fromU8orUseThreshold (wcLibName, szLibFileNameU8);
	if (pwcLibName)
	{
		hModule = LoadLibraryExW (pwcLibName, hFile, dwFlags);
		DoneWinU16fromU8orUseThreshold (pwcLibName, wcLibName);
	}
	return hModule;
}

HMODULE LoadLibraryU8(
  char *chLibFileNameU8
)
{
	HANDLE	hModule	= NULL;

	WCHAR	wcLibName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcLibName	= NULL;

	pwcLibName	= AllocWinU16fromU8orUseThreshold (wcLibName, chLibFileNameU8);
	if (pwcLibName)
	{
		hModule = LoadLibraryW (pwcLibName);
		DoneWinU16fromU8orUseThreshold (pwcLibName, wcLibName);
	}
	return hModule;
}

#ifdef HAVE_ADVAPI32
    BOOL LookupAccountNameU8(
      LPCSTR        lpSystemName,
      LPCSTR        lpAccountName,
      PSID          Sid,
      LPDWORD       cbSid,
      LPSTR         ReferencedDomainName,
      LPDWORD       cchReferencedDomainName,
      PSID_NAME_USE peUse
    )
    {
        ASSERT (NULL != cchReferencedDomainName);
    
        WCHAR	wcSystemName	[1024];
        WCHAR	wcAccountName	[1024];
        WCHAR	*pwcSystemName				= NULL;
        WCHAR	*pwcAccountName				= NULL;
        WCHAR	*pwcDomain;
    
        if (lpSystemName)
        {
            WinU16_from_UTF8 (wcSystemName, 1024, lpSystemName);
            pwcSystemName = wcSystemName;
        }
        if (lpAccountName)
        {
            WinU16_from_UTF8 (wcAccountName, 1024, lpAccountName);
            pwcAccountName = wcAccountName;
        }
        DWORD	dwReferenceDomainName	= *cchReferencedDomainName;
        pwcDomain = ubf_malloc (dwReferenceDomainName * sizeof (WCHAR) * 2);
        BOOL	bRet;
        bRet = LookupAccountNameW	(
                    pwcSystemName, pwcAccountName,
                    Sid, cbSid,
                    pwcDomain, &dwReferenceDomainName,
                    peUse
                                    );
        int iRequiredDomainSize = reqUTF8size (pwcDomain);
        UTF8_from_WinU16 (ReferencedDomainName, *cchReferencedDomainName, pwcDomain);
        // See
        //	https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-lookupaccountnamew .
        /*
            cchReferencedDomainName
    
            A pointer to a variable. On input, this value specifies the size, in TCHARs, of the
            ReferencedDomainName buffer. If the function fails because the buffer is too small,
            this variable receives the required buffer size, including the terminating null
            character. If the ReferencedDomainName parameter is NULL, this parameter must be zero.
            
            It doesn't say anything that we should store that value back unless the required
            size of the buffer is bigger than what the caller provided.
        */
        ASSERT (dwReferenceDomainName <= INT_MAX);
        if (iRequiredDomainSize > (int) dwReferenceDomainName)
            *cchReferencedDomainName = dwReferenceDomainName;
        ubf_free (pwcDomain);
        return bRet;
    }
#endif

#ifdef HAVE_ADVAPI32
    BOOL LookupAccountSidU8(
        LPCSTR        lpSystemNameU8,
        PSID          Sid,
        LPSTR         NameU8,
        LPDWORD       cchNameU8,
        LPSTR         ReferencedDomainNameU8,
        LPDWORD       cchReferencedDomainNameU8,
        PSID_NAME_USE peUse
    )
    {
        WCHAR	*pwcSystemName	= NULL;
        WCHAR	*pwcName		= NULL;
        WCHAR	*pwcDomainName	= NULL;
        WCHAR	wcSystemName	[WINAPI_U8_HEAP_THRESHOLD];
        DWORD	dwName;
        DWORD	dwDomainName;
        size_t	stName;
        size_t	stDomainName;
        int		iReq;
        BOOL	bRet			= FALSE;
        
        // Not sure if the Windows API accepts NULL for the lengths parameters.
        ASSERT (NULL != cchNameU8);
        ASSERT (NULL != cchReferencedDomainNameU8);
        
        // Parameter lpSystemNameU8 -> pwcSystemName.
        if (lpSystemNameU8)
        {
            iReq = reqWinU16wchars (lpSystemNameU8);
            if (iReq <= WINAPI_U8_HEAP_THRESHOLD)
            {
                WinU16_from_UTF8 (wcSystemName, iReq, lpSystemNameU8);
                pwcSystemName = wcSystemName;
            } else
            {
                pwcSystemName = AllocWinU16_from_UTF8 (lpSystemNameU8);
                if (pwcSystemName)
                {
                    WinU16_from_UTF8 (pwcSystemName, iReq, lpSystemNameU8);
                }
            }
        }
        if (NameU8)
        {	// User obviously expects this parameter back.
            stName = sizeof (WCHAR) * *cchNameU8;
            pwcName = ubf_malloc (stName);
        }
        if (ReferencedDomainNameU8)
        {	// User obviously expects this parameter back.
            stDomainName = sizeof (WCHAR) * *cchReferencedDomainNameU8;
            pwcDomainName = ubf_malloc (stDomainName);
        }
        // Although not quite correct (UTF-8 can require up to 50 % more than UTF-16),
        //	we use the original sizes the user has provided.
        dwName			= *cchNameU8;
        dwDomainName	= *cchReferencedDomainNameU8;
        bRet = LookupAccountSidW	(
                    pwcSystemName, Sid, pwcName, &dwName, pwcDomainName, &dwDomainName, peUse
                                    );
        if (dwName != *cchNameU8)
        {	// The Windows API has changed the length of the name.
        
        }
        if (dwDomainName != *cchReferencedDomainNameU8)
        {	// The Windows API has changed the length of the reference domain name.
        
        }
        ubf_free_accept_NULL (pwcDomainName);
        ubf_free_accept_NULL (pwcName);
        if (NULL != pwcSystemName && pwcSystemName != wcSystemName)
            DoneWinU16 (pwcSystemName);
        return bRet;
    }
#endif

#ifdef HAVE_USER32
    int MessageBoxU8(
      HWND   hWnd,
      LPCSTR lpText,
      LPCSTR lpCaption,
      UINT   uType
    )
    {
        return MessageBoxExU8 (hWnd, lpText, lpCaption, uType, 0);
    }
#endif

#ifdef HAVE_USER32
    int MessageBoxExU8(
      HWND   hWnd,
      LPCSTR lpText,
      LPCSTR lpCaption,
      UINT   uType,
      WORD   wLanguageId
    )
    {
        WCHAR	wcText		[WINAPI_U8_HEAP_THRESHOLD];
        WCHAR	wcCaption	[WINAPI_U8_HEAP_THRESHOLD];
        WCHAR	*pwcText;
        WCHAR	*pwcCaption;
        int		iRetVal;
    
        pwcText		= AllocWinU16fromU8orUseThreshold (wcText, lpText);
        pwcCaption	= AllocWinU16fromU8orUseThreshold (wcCaption, lpCaption);
        iRetVal = MessageBoxExW (hWnd, pwcText, pwcCaption, uType, wLanguageId);
        DoneWinU16fromU8orUseThreshold (pwcText, wcText);
        DoneWinU16fromU8orUseThreshold (pwcCaption, wcCaption);
        return iRetVal;
    }
#endif

#ifdef HAVE_WINUSER
	BOOL PostSysCommandWPARAMtoThisConsole (WPARAM wparam)
	{
		HWND		hConsoleWindow;
	
		hConsoleWindow = GetConsoleWindow ();
		if (hConsoleWindow)
		{
			return PostMessageW (hConsoleWindow, WM_SYSCOMMAND, wparam, (LPARAM) NULL);
		}
		return FALSE;
	}
#endif

#ifdef HAVE_WINUSER
	BOOL MaxiMiseThisConsoleWindow (void)
	{
		return PostSysCommandWPARAMtoThisConsole (SC_MAXIMIZE);
	}
#endif

#ifdef HAVE_WINUSER
	BOOL MiniMiseThisConsoleWindow (void)
	{
		return PostSysCommandWPARAMtoThisConsole (SC_MINIMIZE);
	}
#endif

#ifdef HAVE_WINUSER
	BOOL RestoreThisConsoleWindow (void)
	{
		return PostSysCommandWPARAMtoThisConsole (SC_RESTORE);
	}
#endif

BOOL MoveFileU8(
  const char *lpExistingFileNameU8,
  const char *lpNewFileNameU8
)
{
	BOOL	bRet;
	WCHAR	wcSrc [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcSrc;
	WCHAR	wcTgt [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcTgt;
	
	pwcSrc = AllocWinU16fromU8orUseThreshold (wcSrc, lpExistingFileNameU8);
	pwcTgt = AllocWinU16fromU8orUseThreshold (wcTgt, lpNewFileNameU8);
	bRet = MoveFileW (pwcSrc, pwcTgt);
	DoneWinU16fromU8orUseThreshold (pwcTgt, wcTgt);
	DoneWinU16fromU8orUseThreshold (pwcSrc, wcSrc);
	return bRet;
}

BOOL MoveFileU8long(
  const char *lpExistingFileNameU8,
  const char *lpNewFileNameU8
)
{
	BOOL	bRet;
	WCHAR	wcSrc [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcSrc;
	WCHAR	wcTgt [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcTgt;
	
	pwcSrc = AllocWinU16fromU8orUseThresholdLongFileName (wcSrc, lpExistingFileNameU8);
	pwcTgt = AllocWinU16fromU8orUseThresholdLongFileName (wcTgt, lpNewFileNameU8);
	bRet = MoveFileW (pwcSrc, pwcTgt);
	DoneWinU16fromU8orUseThreshold (pwcTgt, wcTgt);
	DoneWinU16fromU8orUseThreshold (pwcSrc, wcSrc);
	return bRet;
}

BOOL MoveFileExU8(
  const char *lpExistingFileNameU8,
  const char *lpNewFileNameU8,
  DWORD dwFlags
)
{
	BOOL	bRet;
	WCHAR	wcSrc [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcSrc;
	WCHAR	wcTgt [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcTgt;

	pwcSrc = AllocWinU16fromU8orUseThreshold (wcSrc, lpExistingFileNameU8);
	pwcTgt = AllocWinU16fromU8orUseThreshold (wcTgt, lpNewFileNameU8);
	bRet = MoveFileExW (pwcSrc, pwcTgt, dwFlags);
	DoneWinU16fromU8orUseThreshold (pwcTgt, wcTgt);
	DoneWinU16fromU8orUseThreshold (pwcSrc, wcSrc);
	return bRet;
}

BOOL MoveFileExU8long(
  const char *lpExistingFileNameU8,
  const char *lpNewFileNameU8,
  DWORD dwFlags
)
{
	BOOL	bRet;
	WCHAR	wcSrc [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcSrc;
	WCHAR	wcTgt [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcTgt;

	pwcSrc = AllocWinU16fromU8orUseThresholdLongFileName (wcSrc, lpExistingFileNameU8);
	pwcTgt = AllocWinU16fromU8orUseThresholdLongFileName (wcTgt, lpNewFileNameU8);
	bRet = MoveFileExW (pwcSrc, pwcTgt, dwFlags);
	DoneWinU16fromU8orUseThreshold (pwcTgt, wcTgt);
	DoneWinU16fromU8orUseThreshold (pwcSrc, wcSrc);
	return bRet;
}

BOOL MoveFileWithProgressU8(
  const char			*lpExistingFileNameU8,
  const char			*lpNewFileNameU8,
  LPPROGRESS_ROUTINE	lpProgressRoutine,
  LPVOID				lpData,
  DWORD					dwFlags
)
{
	BOOL	bRet;
	WCHAR	wcSrc [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcSrc;
	WCHAR	wcTgt [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcTgt;

	pwcSrc = AllocWinU16fromU8orUseThreshold (wcSrc, lpExistingFileNameU8);
	pwcTgt = AllocWinU16fromU8orUseThreshold (wcTgt, lpNewFileNameU8);
	bRet = MoveFileWithProgressW (pwcSrc, pwcTgt, lpProgressRoutine, lpData, dwFlags);
	DoneWinU16fromU8orUseThreshold (pwcTgt, wcTgt);
	DoneWinU16fromU8orUseThreshold (pwcSrc, wcSrc);
	return bRet;
}

BOOL MoveFileWithProgressU8long(
  const char			*lpExistingFileNameU8,
  const char			*lpNewFileNameU8,
  LPPROGRESS_ROUTINE	lpProgressRoutine,
  LPVOID				lpData,
  DWORD					dwFlags
)
{
	BOOL	bRet;
	WCHAR	wcSrc [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcSrc;
	WCHAR	wcTgt [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcTgt;

	pwcSrc = AllocWinU16fromU8orUseThresholdLongFileName (wcSrc, lpExistingFileNameU8);
	pwcTgt = AllocWinU16fromU8orUseThresholdLongFileName (wcTgt, lpNewFileNameU8);
	bRet = MoveFileWithProgressW (pwcSrc, pwcTgt, lpProgressRoutine, lpData, dwFlags);
	DoneWinU16fromU8orUseThreshold (pwcTgt, wcTgt);
	DoneWinU16fromU8orUseThreshold (pwcSrc, wcSrc);
	return bRet;
}

#ifdef HAVE_SHELLAPI
	BOOL MoveToRecycleBinU8 (const char *chFileName8)
	{
		WCHAR	wcFileName [WINAPI_U8_HEAP_THRESHOLD];
		WCHAR	*pwcFileName;
		BOOL	bRet;
		
		pwcFileName = AllocWinU16fromU8orUseThreshold_00 (wcFileName, chFileName8);

		SHFILEOPSTRUCTW		s;
	
		memset (&s, 0, sizeof (SHFILEOPSTRUCTW));
		s.hwnd		= NULL;
		s.wFunc		= FO_DELETE;
		s.pFrom		= pwcFileName;
		s.pTo		= NULL;
		s.fFlags	=	FOF_ALLOWUNDO	| FOF_NOCONFIRMATION	| FOF_NOCONFIRMMKDIR |
						FOF_NOERRORUI	| FOF_RENAMEONCOLLISION	| 
						FOF_SILENT		| FOF_FILESONLY;
		s.fAnyOperationsAborted	= FALSE;
		s.hNameMappings			= NULL;
		s.lpszProgressTitle		= NULL;
		int iR = SHFileOperationW (&s);

		DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
		bRet = 0 == iR;
		return bRet;
	}
#endif

#ifdef HAVE_SHELLAPI
	BOOL MoveToRecycleBinW (const WCHAR *wcFileName)
	{
		SHFILEOPSTRUCTW		s;
	
		memset (&s, 0, sizeof (SHFILEOPSTRUCTW));
		s.hwnd		= NULL;
		s.wFunc		= FO_DELETE;
		s.pFrom		= wcFileName;
		s.pTo		= NULL;
		s.fFlags	=	FOF_ALLOWUNDO	| FOF_NOCONFIRMATION	| FOF_NOCONFIRMMKDIR |
						FOF_NOERRORUI	| FOF_RENAMEONCOLLISION	| 
						FOF_SILENT		| FOF_FILESONLY;
		s.fAnyOperationsAborted	= FALSE;
		s.hNameMappings			= NULL;
		s.lpszProgressTitle		= NULL;
		int iR = SHFileOperationW (&s);

		// See
		//	https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shfileoperationw .
		_ASSERT_EXPR (0x7C != iR, "Parameter wcFileName requires two NUL ternimators!");

		return 0 == iR;
	}
#endif

#ifdef HAVE_NETAPI32
	NET_API_STATUS NET_API_FUNCTION NetUserEnumU8(
		LPCSTR  servernameU8,
		DWORD   level,
		DWORD   filter,
		LPBYTE  *bufptr,
		DWORD   prefmaxlen,
		LPDWORD entriesread,
		LPDWORD totalentries,
		PDWORD  resume_handle
	)
	{
		WCHAR			wcServerName	[WINAPI_U8_HEAP_THRESHOLD];
		WCHAR			*pcServerName;

		pcServerName = AllocWinU16fromU8orUseThreshold (wcServerName, servernameU8);
		NET_API_STATUS nap = NetUserEnum	(
								pcServerName, level, filter, bufptr, prefmaxlen,
								entriesread, totalentries, resume_handle
											);
		DoneWinU16fromU8orUseThreshold (pcServerName, wcServerName);
		return nap;
	}
#endif

HANDLE OpenEventU8(
	DWORD  dwDesiredAccess,
	BOOL   bInheritHandle,
	LPCSTR lpName
)
{
	HANDLE	hRet;
	WCHAR	wcName [MAX_PATH + 1];

	/*
		The name of an event can only be MAX_PATH characters long, hence we can implement
		this function in a very simple manner. See
		https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createmutexw .
	*/

	WinU16_from_UTF8 (wcName, MAX_PATH + 1, lpName);
	hRet = OpenEventW (dwDesiredAccess, bInheritHandle, wcName);
	return hRet;
}

#ifdef HAVE_ADVAPI32
HANDLE OpenEventLogU8(
	LPCSTR lpUNCServerNameU8,
	LPCSTR lpSourceNameU8
)
{
	WCHAR   wcUNCServerName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR   wcSourceName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR   *pcUNCServerName;
	WCHAR   *pcSourceName;
	HANDLE  hRet;

	pcUNCServerName = AllocWinU16fromU8orUseThreshold (wcUNCServerName, lpUNCServerNameU8);
	pcSourceName    = AllocWinU16fromU8orUseThreshold (wcSourceName, lpSourceNameU8);

	hRet = OpenEventLogW (pcUNCServerName, pcSourceName);

	DoneWinU16fromU8orUseThreshold (pcUNCServerName, wcUNCServerName);
	DoneWinU16fromU8orUseThreshold (pcSourceName, wcSourceName);

	return hRet;
}
#endif

#ifdef HAVE_ADVAPI32
BOOL ClearEventLogU8(
  HANDLE  hEventLog,
  LPCSTR lpBackupFileNameU8
)
{
	WCHAR   wcBackupFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR   *pcBackupFileName;
	BOOL    bRet;

	pcBackupFileName = AllocWinU16fromU8orUseThreshold (wcBackupFileName, lpBackupFileNameU8);
	bRet = ClearEventLogW (hEventLog, pcBackupFileName);
	DoneWinU16fromU8orUseThreshold (pcBackupFileName, wcBackupFileName);
	return bRet;
}
#endif

HANDLE OpenFileMappingU8(
	DWORD  dwDesiredAccess,
	BOOL   bInheritHandle,
	LPCSTR lpName
)
{
	HANDLE	hRet;
	WCHAR	wcName [MAX_PATH + 1];

	WinU16_from_UTF8 (wcName, MAX_PATH + 1, lpName);
	hRet = OpenFileMappingW (dwDesiredAccess, bInheritHandle, wcName);
	return hRet;
}

HANDLE OpenMutexU8(
  DWORD		dwDesiredAccess,
  BOOL		bInheritHandle,
  LPCSTR	lpNameU8
)
{
	/*
		The name of a mutex can only be MAX_PATH characters long, hence we can implement
		this function slightly simpler. See
		https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createmutexw .

		WCHAR	wcMutexName [WINAPI_U8_HEAP_THRESHOLD];
		WCHAR	*pwcMutexName;
		HANDLE	hRet;

		pwcMutexName = AllocWinU16fromU8orUseThreshold (wcMutexName, lpNameU8);
		hRet = OpenMutexW (dwDesiredAccess, bInheritHandle, pwcMutexName);
		DoneWinU16fromU8orUseThreshold (pwcMutexName, wcMutexName);
		return hRet;
	*/

	WCHAR	wcName [MAX_PATH + 1];

	WinU16_from_UTF8 (wcName, MAX_PATH + 1, lpNameU8);
	return OpenMutexW (dwDesiredAccess, bInheritHandle, wcName);
}

#ifdef HAVE_ADVAPI32
SC_HANDLE OpenSCManagerU8(
  LPCSTR lpMachineName,
  LPCSTR lpDatabaseName,
  DWORD  dwDesiredAccess
)
{
	WCHAR	wcMachineName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcDatabaseName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcMachineName;
	WCHAR	*pwcDatabaseName;
	SC_HANDLE	hRet;

	pwcMachineName	= AllocWinU16fromU8orUseThreshold (wcMachineName, lpMachineName);
	pwcDatabaseName	= AllocWinU16fromU8orUseThreshold (wcDatabaseName, lpDatabaseName);

	hRet = OpenSCManagerW (pwcMachineName, pwcDatabaseName, dwDesiredAccess);

	DoneWinU16fromU8orUseThreshold (pwcDatabaseName, wcDatabaseName);
	DoneWinU16fromU8orUseThreshold (pwcMachineName, wcMachineName);

	return hRet;
}
#endif

#ifdef HAVE_ADVAPI32
SC_HANDLE OpenServiceU8(
  SC_HANDLE hSCManager,
  LPCSTR    lpServiceName,
  DWORD     dwDesiredAccess
)
{
	if (lpServiceName)
	{
		// See
		//	https://docs.microsoft.com/en-us/windows/win32/api/winsvc/nf-winsvc-openservicew .
		//	The maximum string length for lpServiceName is 256 characters. We leave it
		//	with the caller to ensure this length is not exceeded.
		WCHAR	wcServiceName [512];
		WinU16_from_UTF8 (wcServiceName, 512, lpServiceName);
		return OpenServiceW (hSCManager, wcServiceName, dwDesiredAccess);
	} else
	{
		return OpenServiceW (hSCManager, NULL, dwDesiredAccess);
	}
}
#endif

BOOL PathsEqualfromEndW (WCHAR *exe, WCHAR *arg)
{
	DWORD	lene	= (DWORD) wcslen (exe);
	DWORD	lena	= (DWORD) wcslen (arg);

	if (lene && lena)
	{
		while (lene && lena && towupper (*(exe + lene)) == towupper (*(arg + lena)))
		{
			-- lene;
			-- lena;
		}
		if (0 == lene || 0 == lena)
		{	// One of the two parameters has been fully consumed.
			return TRUE;
		}
		if (lene && L'\\' == *(exe + lene))
		{
			return TRUE;
		}
		if (lena && L'\\' == *(arg + lena))
		{
			return TRUE;
		}
	}
	// If both strings have no length we want that they are not treated
	//	as equal.
	return FALSE;
}

#ifdef HAVE_SHLWAPI
BOOL PathIsUNCU8(
	LPSTR	pszPathU8
)
{	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/shlwapi/nf-shlwapi-pathisuncw .
	//	Can only be up to MAX_PATH.
	WCHAR	wcPath [MAX_PATH + 1];

	WinU16_from_UTF8 (wcPath, MAX_PATH + 1, pszPathU8);
	return PathIsUNCW (wcPath);
}
#endif

#ifdef HAVE_SHLWAPI
BOOL PathIsNetworkPathU8(
	LPSTR	pszPathU8
)
{	// See
	//	https://learn.microsoft.com/en-us/windows/win32/api/shlwapi/nf-shlwapi-pathisnetworkpathw .
	//	Can only be up to MAX_PATH.
	WCHAR	wcPath [MAX_PATH + 1];

	WinU16_from_UTF8 (wcPath, MAX_PATH + 1, pszPathU8);
	return PathIsNetworkPathW (wcPath);
}
#endif

#ifdef HAVE_SHLWAPI
LPCSTR PathFindNextComponentU8(
	LPSTR pszPathU8
)
{	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/shlwapi/nf-shlwapi-pathfindnextcomponentw .
	/*
		Excerpt:
		"PathFindNextComponent walks a path string until it encounters a backslash ("\"), ignores
		everything up to that point including the backslash, and returns the rest of the path.
		Therefore, if a path begins with a backslash (such as \path1\path2), the function simply
		removes the initial backslash and returns the rest (path1\path2)."

		OUTPUT:
		===========
		The path c:\path1\path2\file.txt returns path1\path2\file.txt
		The path path1\path2\file.txt returns path2\file.txt
		The path path2\file.txt returns file.txt
		The path file.txt returns a pointer to the terminating null character
		The terminating null character returns NULL

		The path \path1 returns path1
		The path \path1\path2 returns path1\path2
		The path path1\path2 returns path2
		The path c:\file.txt returns file.txt
	*/
	LPCSTR pszRet = NULL;
	if (pszPathU8 && *pszPathU8)
	{
		pszRet = strchr (pszPathU8, '\\');
		if (pszRet)
			++ pszRet;
		else
			pszRet = pszPathU8 + strlen (pszPathU8);
	}
	return pszRet;
}
#endif

#ifdef HAVE_ADVAPI32
HANDLE RegisterEventSourceU8(
  LPCSTR lpUNCServerName,
  LPCSTR lpSourceName
)
{
	WCHAR	wcUNCServerName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcSourceName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcUNCServerName;
	WCHAR	*pwcSourceName;
	HANDLE	hRet;
	
	pwcUNCServerName	= AllocWinU16fromU8orUseThreshold (wcUNCServerName, lpUNCServerName);
	pwcSourceName		= AllocWinU16fromU8orUseThreshold (wcSourceName, lpSourceName);
	hRet = RegisterEventSourceW (pwcUNCServerName, pwcSourceName);
	DoneWinU16fromU8orUseThreshold (pwcSourceName, wcSourceName);
	DoneWinU16fromU8orUseThreshold (pwcUNCServerName, wcUNCServerName);
	return hRet;
}
#endif

#ifdef HAVE_ADVAPI32
SERVICE_STATUS_HANDLE RegisterServiceCtrlHandlerU8(
  LPCSTR             lpServiceName,
  LPHANDLER_FUNCTION lpHandlerProc
)
{
	if (lpServiceName)
	{
		// Note that the maximum length for lpServiceName is 256 characters.
		//	We leave it with the caller to ensure this is adhered to.
		WCHAR	wcServiceName [512];
		WinU16_from_UTF8 (wcServiceName, 512, lpServiceName);
		return RegisterServiceCtrlHandlerW (wcServiceName, lpHandlerProc);
	}
	return RegisterServiceCtrlHandlerW (NULL, lpHandlerProc);
}
#endif

#ifdef HAVE_ADVAPI32
SERVICE_STATUS_HANDLE RegisterServiceCtrlHandlerExU8(
  LPCSTR                lpServiceName,
  LPHANDLER_FUNCTION_EX lpHandlerProc,
  LPVOID                lpContext
)
{
	if (lpServiceName)
	{
		// Note that the maximum length for lpServiceName is 256 characters.
		//	We leave it with the caller to ensure this is adhered to.
		WCHAR	wcServiceName [512];
		WinU16_from_UTF8 (wcServiceName, 512, lpServiceName);
		return RegisterServiceCtrlHandlerExW (wcServiceName, lpHandlerProc, lpContext);
	}
	return RegisterServiceCtrlHandlerExW (NULL, lpHandlerProc, lpContext);
}
#endif

BOOL RemoveDirectoryU8(
	LPCSTR lpPathNameU8
)
{
	WCHAR wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpPathNameU8);
	BOOL bRet = RemoveDirectoryW (pwcFileName);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}

BOOL RemoveDirectoryU8long(
	LPCSTR lpPathNameU8
)
{
	WCHAR wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcFileName = AllocWinU16fromU8orUseThresholdLongFileName (wcFileName, lpPathNameU8);
	BOOL bRet = RemoveDirectoryW (pwcFileName);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}

#ifdef HAVE_ADVAPI32
BOOL ReportEventU8(
  HANDLE hEventLog,
  WORD   wType,
  WORD   wCategory,
  DWORD  dwEventID,
  PSID   lpUserSid,
  WORD   wNumStrings,
  DWORD  dwDataSize,
  LPCSTR *lpStrings,
  LPVOID lpRawData
)
{
	BOOL	bRet			= FALSE;
	WCHAR	**wstrArray;
	
	wstrArray = CreateWin16tringArrayFromU8 (lpStrings, wNumStrings);
	if (wstrArray)
	{
		bRet = ReportEventW	(
					hEventLog, wType, wCategory, dwEventID, lpUserSid, wNumStrings,
					dwDataSize, (const WCHAR **) wstrArray, lpRawData
							);
		DoneWin16StringArray (wstrArray, wNumStrings);
	}
	return bRet;
}
#endif

void SetConsoleCodePageToANSI (void)
{
	SetConsoleCP (CP_UTF8);
	SetConsoleOutputCP (1252);
}

void SetConsoleCodePageToUTF8 (void)
{
	SetConsoleCP (CP_UTF8);
	SetConsoleOutputCP (CP_UTF8);
}

bool SetConsoleEnableANSI (void)
{
	bool b = false;

	HANDLE hConsole = GetStdHandle (STD_OUTPUT_HANDLE);
	if (INVALID_HANDLE_VALUE != hConsole)
	{
		DWORD	dwMode;
		b = GetConsoleMode (hConsole, &dwMode);
		if (b)
		{
			dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
			b = SetConsoleMode (hConsole, dwMode);
		}
	}
	return b;
}

int WinSetStdoutToUTF16 (void)
{
	// Change stdout to Unicode UTF-16
    return _setmode(_fileno(stdout), _O_U16TEXT);
}

int WinSetStdinToUTF16 (void)
{
	// Change stdout to Unicode UTF-16
    return _setmode(_fileno(stdin), _O_U16TEXT);
}

BOOL SetCurrentDirectoryU8(
  const char *lpPathNameU8
)
{
	WCHAR wcPathName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcPathName	= AllocWinU16fromU8orUseThreshold (wcPathName, lpPathNameU8);
	BOOL bRet			= SetCurrentDirectoryW (pwcPathName);
	DoneWinU16fromU8orUseThreshold (pwcPathName, wcPathName);
	return bRet;
}

BOOL SetCurrentDirectoryU8long(
  const char *lpPathNameU8
)
{
	WCHAR wcPathName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcPathName	= AllocWinU16fromU8orUseThresholdLongFileName (wcPathName, lpPathNameU8);
	BOOL bRet			= SetCurrentDirectoryW (pwcPathName);
	DoneWinU16fromU8orUseThreshold (pwcPathName, wcPathName);
	return bRet;
}

DWORD SetFileAttributesU8(
  LPCSTR	lpFileNameU8,
  DWORD		dwFileAttributes
)
{
	WCHAR wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcFileName	= AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	DWORD dwRet			= SetFileAttributesW (pwcFileName, dwFileAttributes);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return dwRet;
}

DWORD SetFileAttributesU8long(
  LPCSTR	lpFileNameU8,
  DWORD		dwFileAttributes
)
{
	WCHAR wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcFileName	= AllocWinU16fromU8orUseThresholdLongFileName (wcFileName, lpFileNameU8);
	DWORD dwRet			= SetFileAttributesW (pwcFileName, dwFileAttributes);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return dwRet;
}

#ifdef HAVE_ADVAPI32
BOOL SetFileSecurityU8(
  char					*lpFileNameU8,
  SECURITY_INFORMATION	SecurityInformation,
  PSECURITY_DESCRIPTOR	pSecurityDescriptor
)
{
	WCHAR wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	BOOL bRet = SetFileSecurityW (pwcFileName, SecurityInformation, pSecurityDescriptor);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return bRet;
}
#endif

/*
	ToDo!!!
	Doesn't seem to work!!!
*/
bool TerminateChildProcess (HANDLE hProcess)
{
	bool b = true;

	b &= GenerateConsoleCtrlEvent (CTRL_C_EVENT, GetProcessId (hProcess));
	b &= GenerateConsoleCtrlEvent (CTRL_BREAK_EVENT, GetProcessId (hProcess));

	DWORD dwWait = WaitForSingleObject (hProcess, 500);
	if (WAIT_OBJECT_0 == dwWait)
		return true;

	// Child still running.
	b &= TerminateProcess (hProcess, ERROR_PROCESS_ABORTED);
	dwWait = WaitForSingleObject (hProcess, 500);
	if (WAIT_OBJECT_0 == dwWait)
		return true;

	return b;
}

BOOL IsFirstArgumentExeArgumentW (int *pargc, WCHAR **pargv [])
{
	ASSERT (NULL != pargc);

	if (*pargc)
	{
		WCHAR *arg	= *pargv [0];
		// Remove "." and ".." prefixes.
		if (L'.' == arg [0])
			++ arg;
		if (L'.' == arg [0])
			++ arg;

		WCHAR wcExe [4096];
		DWORD dwLenModuleFileName;

		if ((dwLenModuleFileName = GetModuleFileNameW (NULL, wcExe, 4096)) < 4096)
		{
			if (PathsEqualfromEndW (wcExe, arg))
			{	// Is executable argument.
				return TRUE;
			}
			WCHAR *wc = wcsrchr (wcExe, L'.');
			if (wc)
				*wc = L'\0';
			if (PathsEqualfromEndW (wcExe, arg))
			{	// Is executable argument.
				return TRUE;
			}
		} else
		{	// The executable path is too long.
			ASSERT (FALSE);
		}
	}
	return FALSE;
}

BOOL SwallowExeArgumentW (int *pargc, WCHAR **pargv [])
{
	ASSERT (NULL != pargc);

	if (IsFirstArgumentExeArgumentW (pargc, pargv))
	{
		*pargc -= 1;										// Argument counter.
		*pargv += 1;										// Point to new first argument.
		return TRUE;
	}
	return FALSE;
}

WCHAR **InsertExeArgumentIfMissingW(int *pargc, WCHAR **pargv [])
{
	ASSERT (NULL != pargc);

	if (IsFirstArgumentExeArgumentW (pargc, pargv))
	{
		return *pargv;
	}

	WCHAR	wcExe [4096];
	DWORD	dwLenModuleFileName;
	WCHAR	**pwcRet;
	int		n;

	++ *pargc;
	if ((dwLenModuleFileName = GetModuleFileNameW (NULL, wcExe, 4096)) < 4096)
	{
		pwcRet = ubf_malloc (*pargc * sizeof (WCHAR*));
		if (pwcRet)
		{
			pwcRet [0] = _wcsdup (wcExe);
			for (n = 1; n < *pargc; ++ n)
			{
				pwcRet [n] = *pargv [n - 1];
			}
		}
		return pwcRet;
	} else
	{	// The executable path is too long.
		ASSERT (FALSE);
	}
	return *pargv;
}

#ifdef _MSC_VER
#pragma warning (default: 4456)									// Previous declaration hidden
																//	(wcExe).
#endif

#ifdef HAVE_MPR
DWORD WNetGetUniversalNameU8(
	LPCSTR  lpLocalPathU8,
	DWORD   dwInfoLevel,
	LPVOID  lpBuffer,
	LPDWORD lpBufferSize
)
{
	int iReqSize = reqWinU16wchars (lpLocalPathU8);
	if (iReqSize <= WINAPI_U8_HEAP_THRESHOLD)
	{
		WCHAR wcPath [WINAPI_U8_HEAP_THRESHOLD];
		WinU16_from_UTF8 (wcPath, iReqSize, lpLocalPathU8);
		return WNetGetUniversalNameW (wcPath, dwInfoLevel, lpBuffer, lpBufferSize);
	} else
	{
		DWORD	dwRet;
		WCHAR *pwcPath = AllocWinU16_from_UTF8 (lpLocalPathU8);
		if (pwcPath)
		{
			dwRet = WNetGetUniversalNameW (pwcPath, dwInfoLevel, lpBuffer, lpBufferSize);
			DoneWinU16 (pwcPath);
			return dwRet;
		}
	}
	ASSERT (FALSE);
	return FALSE;
}
#endif

BOOL WritePrivateProfileStringU8(
	LPCSTR lpAppNameU8,
	LPCSTR lpKeyNameU8,
	LPCSTR lpStringU8,
	LPCSTR lpFileNameU8
)
{
	WCHAR	wcAppName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcKeyName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcString	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcFileName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcAppName		= NULL;
	WCHAR	*pwcKeyName		= NULL;
	WCHAR	*pwcString		= NULL;
	WCHAR	*pwcFileName	= NULL;
	BOOL	bRet			= FALSE;
	
	if (lpAppNameU8)
		pwcAppName	= AllocWinU16fromU8orUseThreshold (wcAppName, lpAppNameU8);
	if (lpKeyNameU8)
		pwcKeyName	= AllocWinU16fromU8orUseThreshold (wcKeyName, lpKeyNameU8);
	if (lpStringU8)
		pwcString	= AllocWinU16fromU8orUseThreshold (wcString, lpStringU8);
	if (lpFileNameU8)
		pwcFileName	= AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	bRet = WritePrivateProfileStringW (pwcAppName, pwcKeyName, pwcString, pwcFileName);
	if (lpFileNameU8)
		DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	if (lpStringU8)
		DoneWinU16fromU8orUseThreshold (pwcString, wcString);
	if (lpKeyNameU8)
		DoneWinU16fromU8orUseThreshold (pwcKeyName, wcKeyName);
	if (lpAppNameU8)
		DoneWinU16fromU8orUseThreshold (pwcAppName, wcAppName);
	return bRet;
}

/*
	Only an example! The API does not support long filename prefixes!
	
BOOL WritePrivateProfileStringU8long(
	LPCSTR lpAppNameU8,
	LPCSTR lpKeyNameU8,
	LPCSTR lpStringU8,
	LPCSTR lpFileNameU8absolute
)
{
	WCHAR	wcAppName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcKeyName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcString	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	wcFileName	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pwcAppName		= NULL;
	WCHAR	*pwcKeyName		= NULL;
	WCHAR	*pwcString		= NULL;
	WCHAR	*pwcFileName	= NULL;
	BOOL	bRet			= FALSE;
	
	pwcAppName	= AllocWinU16fromU8orUseThreshold (wcAppName, lpAppNameU8);
	pwcKeyName	= AllocWinU16fromU8orUseThreshold (wcKeyName, lpKeyNameU8);
	pwcString	= AllocWinU16fromU8orUseThreshold (wcString, lpStringU8);
	pwcFileName	= AllocWinU16fromU8orUseThresholdLongFileName (wcFileName, lpFileNameU8absolute);
	bRet = WritePrivateProfileStringW (pwcAppName, pwcKeyName, pwcString, pwcFileName);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	DoneWinU16fromU8orUseThreshold (pwcString, wcString);
	DoneWinU16fromU8orUseThreshold (pwcKeyName, wcKeyName);
	DoneWinU16fromU8orUseThreshold (pwcAppName, wcAppName);
	return bRet;
}
*/

int vfprintfU8toU16stream (FILE *stream, const char *format, va_list ap)
{
	int			iReq;
	int			iRet		= -1;

	iReq = vsnprintf (NULL, 0, format, ap);

	WCHAR	wcToPrint	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcToPrint;

	if (iReq < WINAPI_U8_HEAP_THRESHOLD)
	{
		char	szToPrint	[WINAPI_U8_HEAP_THRESHOLD];

		vsnprintf (szToPrint, WINAPI_U8_HEAP_THRESHOLD, format, ap);
		pcToPrint = AllocWinU16fromU8orUseThreshold (wcToPrint, szToPrint);
		if (pcToPrint)
		{
			iRet = fwprintf (stream, pcToPrint);
			DoneWinU16fromU8orUseThreshold (pcToPrint, wcToPrint);
		}
	} else
	{
		char	*szToPrint = ubf_malloc (((size_t) (iReq)) + 1);

		if (szToPrint)
		{
			vsnprintf (szToPrint, ((size_t) (iReq)) + 1, format, ap);
			pcToPrint = AllocWinU16fromU8orUseThreshold (wcToPrint, szToPrint);
			if (pcToPrint)
			{
				iRet = fwprintf (stream, pcToPrint);
				DoneWinU16fromU8orUseThreshold (pcToPrint, wcToPrint);
			}
			ubf_free (szToPrint);
		}
	}
	return iRet;
}

int fprintfU8toU16stream (FILE *stream, const char *format, ...)
{
	int			iRet		= -1;
	va_list		args;

	va_start (args, format);
	iRet = vfprintfU8toU16stream (stream, format, args);
	va_end (args);

	return iRet;
}

int putsU8toU16stdout (const char *strU8)
{
	WCHAR	wcToPrint	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcToPrint;
	int		iRet		= EOF;

	pcToPrint = AllocWinU16fromU8orUseThreshold (wcToPrint, strU8);
	if (pcToPrint)
	{
		iRet = _putws (pcToPrint);
		DoneWinU16fromU8orUseThreshold (pcToPrint, wcToPrint);
	}
	return iRet;
}

#endif														// Of #ifdef _WIN32.
/****************************************************************************************

	File		WinAPI_U8_Test.c
	Why:		Test module for WinAPI_U8.
	OS:			Windows
	Author:		Thomas
	Created:	2021-04-17

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2021-04-17	Thomas			Created.

****************************************************************************************/

#ifdef _WIN32

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./WinAPI_U8.h"
	#include "./WinAPI_U8_Test.h"
	#include "./WinAPI_ReadDirFncts.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
		#include "./ubfmem.h"
		#include "./strisdotordotdot.h"
	#else
		#include "./../../dbg/ubfdebug.h"
		#include "./../../mem/ubfmem.h"
		#include "./../../string/strisdotordotdot.h"
	#endif

#endif

#ifdef BUILD_TEST_WINAPI_U8_FNCT
	bool Test_AllocU8path_from_U8path_and_WinU16FileName (void)
	{
		bool		b			= true;
		char		*chU8Path;
		
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:\\Test\\Fil*", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:\\Test\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:*", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:\\Test", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:\\Test\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:\\Test\\*", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:\\Test\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:\\Test\\*?", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:\\Test\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:\\Test\\?", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:\\Test\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:\\Test\\??????????", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:\\Test\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:\\Test\\?????????*", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:\\Test\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:?", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:*?", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("D:????????????", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "D:File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("\\\\Server\\share", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "\\\\Server\\share\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("\\\\Server\\share*", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "\\\\Server\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("\\\\Server\\share?", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "\\\\Server\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("\\\\Server\\share*?", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "\\\\Server\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("\\\\Server\\?????????", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "\\\\Server\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);
		chU8Path = AllocU8path_from_U8path_and_WinU16FileName ("\\\\Server", L"File.txt");
		ubf_expect_bool_AND (b, !memcmp (chU8Path, "\\\\Server\\File.txt", strlen (chU8Path)));
		DoneU8 (chU8Path);

		return b;
	}
#endif

#ifdef BUILD_TEST_WINAPI_U8_FNCT
	bool Test_WinAPI_U8 (void)
	{
		bool b;

		b = Test_AllocU8path_from_U8path_and_WinU16FileName ();
		ubf_expect_bool_AND (b, !isDotOrDotDotW (L""));
		ubf_expect_bool_AND (b, !isDotOrDotDotW (L"A"));
		ubf_expect_bool_AND (b, isDotOrDotDotW (L"."));
		ubf_expect_bool_AND (b, isDotOrDotDotW (L".."));

		char			sz [1024];
		unsigned int	ui, u2, u3;

		/*
			Return value checks for GetSystemDirectoryU8 () and GetSystemWindowsDirectoryU8 ().

			From
			https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemdirectoryw
			and
			https://learn.microsoft.com/en-us/windows/win32/api/sysinfoapi/nf-sysinfoapi-getsystemwindowsdirectoryw :

			Return value

			If the function succeeds, the return value is the length, in TCHARs, of the string
			copied to the buffer, not including the terminating null character. If the length
			is greater than the size of the buffer, the return value is the size of the buffer
			required to hold the path, including the terminating null character.

			If the function fails, the return value is zero. To get extended error information,
			call GetLastError.
		*/
		ui = GetSystemDirectoryU8 (sz, 1024);
		ubf_expect_bool_AND (b, ui > 0);
		// We assume that the returned directory is not longer than 1024 characters.
		for (u2 = 0; u2 < ui; ++ u2)
		{
			u3 = GetSystemDirectoryU8 (sz, u2);
			ubf_expect_bool_AND (b, ui + 1 == u3);
		}
		u2 = GetSystemDirectoryU8 (sz, ui);
		ubf_expect_bool_AND (b, ui + 1 == u2);
		const char *szSysDir = SystemDirectoryU8 ();
		ubf_expect_bool_AND (b, 0 == memcmp (sz, szSysDir, SystemDirectoryU8len () + 1));
		DoneSystemDirectoryU8 ();
		/*
			The same applies to our GetSystemWindowsDirectoryU8 () function.
		*/
		ui = GetSystemWindowsDirectoryU8 (sz, 1024);
		ubf_expect_bool_AND (b, ui > 0);
		// We assume that the returned directory is not longer than 1024 characters.
		for (u2 = 0; u2 < ui; ++ u2)
		{
			u3 = GetSystemWindowsDirectoryU8 (sz, u2);
			ubf_expect_bool_AND (b, ui + 1 == u3);
		}
		u2 = GetSystemWindowsDirectoryU8 (sz, ui);
		ubf_expect_bool_AND (b, ui + 1 == u2);
		const char *szWinDir = SystemWindowsDirectoryU8 ();
		ubf_expect_bool_AND (b, 0 == memcmp (sz, szWinDir, SystemWindowsDirectoryU8len () + 1));
		DoneSystemWindowsDirectoryU8 ();

		b &= GetWinErrorTextU8 (sz, 1024, 455);
		ubf_assert_true (b);
		b &= GetWinErrorTextU8 (sz, 1024, 456);
		ubf_assert_true (b);
		b &= GetWinErrorTextU8 (sz, 1024, 457);
		ubf_assert_true (b);


		return b;
	}
#endif

#endif														// Of #ifdef _WIN32.
/****************************************************************************************

	File		WinAPI_ReadDirFncts.c
	Why:		Functions to read directory contents
	OS:			Windows
	Created:	2022-01-15

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2022-01-15	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _CRT_RAND_S
#define _CRT_RAND_S
#endif

#include <stdlib.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./WinAPI_ReadDirFncts.h"
	#include "./WinAPI_U8.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
		#include "./ubfmem.h"
		#include "./strfilesys.h"
		#include "./strisdotordotdot.h"
		#include "./strmembuf.h"
		#include "./strwildcards.h"
	#else
		#include "./../../dbg/ubfdebug.h"
		#include "./../../mem/ubfmem.h"
		#include "./../../string/strfilesys.h"
		#include "./../../string/strisdotordotdot.h"
		#include "./../../string/strmembuf.h"
		#include "./../../string/strwildcards.h"
	#endif

#endif

#ifdef _WIN32

BOOL ReadDirectoryEntriesSDIRW_DefaultIgnoreCondition (SRDIRONEENTRYSTRUCT *pign)
{
	ubf_assert_non_NULL (pign);

	return
			isDotOrDotDotW (pign->pwfd->cFileName)
		||	pign->pwfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN
		||	pign->pwfd->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM
	;
}

SDIRW *ReadDirectoryEntriesSDIRW_WU8_ex	(
						void					*strPathWorU8,
						enum enrdirentssdirw	u,
						DWORD					*pdwEntries,
						IgnoreCondCB			pIgnore,
						void					*pCustom,
						RDEMallocCB				pMalloc
										)
{
	HANDLE				hFind				= INVALID_HANDLE_VALUE;
	SDIRW				*swdFirst			= NULL;
	SDIRW				*swd;
	SDIRW				*swdPrev			= NULL;
	WIN32_FIND_DATAW	wfdLocal;
	DWORD				dwEnts				= 0;
	SRDIRONEENTRYSTRUCT	sicData;

	// Distinguish between UFT-16 and UTF-8 version.
	WCHAR				*pwstrPath;
	char				*pstrPath;

	switch (u)
	{
		case EN_READ_DIR_ENTS_SDIRW_UTF8:
			pwstrPath = (WCHAR *) strPathWorU8;
			hFind = FindFirstFileW (pwstrPath, &wfdLocal);
			break;
		case EN_READ_DIR_ENTS_SDIRW_UTF16:
			pstrPath = (char *) strPathWorU8;
			hFind = FindFirstFileU8 (pstrPath, &wfdLocal);
			break;
	}

	// Parameter for the ignore callback funciton.
	sicData.UTF8orUTF16.pstrPathWorU8	= strPathWorU8;
	sicData.pwfd						= &wfdLocal;
	sicData.u							= u;
	sicData.pCustom						= pCustom;
	// Ignore condition callback function.
	IgnoreCondCB pIgnoreEntry =	pIgnore	? pIgnore
										: (IgnoreCondCB) ReadDirectoryEntriesSDIRW_DefaultIgnoreCondition;
	// Memory allocation callback function.
	#ifdef UBF_MEM_DEBUG_USE_OUR_DEBUG_FUNCS
		RDEMallocCB pMallocEntry =
			pMalloc ? pMalloc : (RDEMallocCB) ubf_malloc;
	#else
		RDEMallocCB pMallocEntry =
			pMalloc ? pMalloc : (RDEMallocCB) malloc;
	#endif
	if (INVALID_HANDLE_VALUE == hFind)
	{	// Maybe no files or whatever.
		goto Return;
	}
	do
	{	// Go through the folder and pick up each file. Ignore accordingly.
		if (pIgnoreEntry (&sicData))
		{	// Entries to be ignored.
		} else
		{
			swd = pMallocEntry (sizeof (SDIRW));
			if (NULL == swd)
			{	// Not enough space on the heap.
				swdFirst = ReleaseDirectoryEntriesSDIRW (swdFirst);
				dwEnts = (DWORD) -1;
				SetLastError (ERROR_NOT_ENOUGH_MEMORY);
				goto Return;
			}
			++ dwEnts;										// Amount of found entries.
			swd->_next = NULL;
			copyWIN32_FIND_DATAW (&swd->wfdW, &wfdLocal);
			if (NULL == swdFirst)
				swdFirst = swd;
			if (swdPrev)
				swdPrev->_next = swd;
			swdPrev = swd;
		}
	} while (FindNextFileW (hFind, &wfdLocal) != 0);
	FindClose (hFind);
Return:
	//printf ("%d entries created.\n", dwEnts);
	if (pdwEntries)
		*pdwEntries = dwEnts;
	return swdFirst;
}

SDIRW *ReadDirectoryEntriesSDIRW_W		(
						WCHAR					*wstrPath,
						DWORD					*pdwEntries,
						IgnoreCondCB			ig,
						void					*pCustom,
						RDEMallocCB				pMalloc
										)
{
	return ReadDirectoryEntriesSDIRW_WU8_ex (wstrPath, EN_READ_DIR_ENTS_SDIRW_UTF16,
				pdwEntries, ig, pCustom, pMalloc);
}

SDIRW *ReadDirectoryEntriesSDIRW_U8		(
						char					*strPathU8,
						DWORD					*pdwEntries,
						IgnoreCondCB			ig,
						void					*pCustom,
						RDEMallocCB				pMalloc
										)
{
	return ReadDirectoryEntriesSDIRW_WU8_ex (strPathU8, EN_READ_DIR_ENTS_SDIRW_UTF8,
				pdwEntries, ig, pCustom, pMalloc);
}

SDIRW *ReleaseDirectoryEntriesSDIRW (SDIRW *swd)
{
	SDIRW	*swdNext;

	while (swd)
	{
		swdNext = swd->_next;
		ubf_free (swd);
		swd = swdNext;
	}
	return NULL;
}

uint64_t ReleaseDirectoryEntriesSDIRW_cnt (SDIRW *swd)
{
	SDIRW		*swdNext;
	uint64_t	uiRet		= 0;

	while (swd)
	{
		swdNext = swd->_next;
		ubf_free (swd);
		swd = swdNext;
		++ uiRet;
	}
	return uiRet;
}

#ifdef HAVE_MEMBUF
	size_t ForEachDirectoryEntryU8_Ex	(
					const char				*strPathU8,
					pForEachDirEntryU8		fedEnt,
					void					*pCustom,
					size_t					*pnSubLevels,
					SMEMBUF                 *pmb
										)
	{
		HANDLE				hFind;
		WIN32_FIND_DATAW	wfdW;
		size_t				uiEnts				= 0;			// The return value.
		SRDIRONEENTRYSTRUCT	sdOneEntry;
		DWORD				dwErrToReturn;

		sdOneEntry.UTF8orUTF16.pstrPathWorU8	= strPathU8;
		sdOneEntry.pwfd							= &wfdW;
		sdOneEntry.u							= EN_READ_DIR_ENTS_SDIRW_UTF8;
		sdOneEntry.pCustom						= pCustom;
		hFind = FindFirstFileU8 (strPathU8, &wfdW);
		if (INVALID_HANDLE_VALUE == hFind)
		{	// Maybe no files or whatever. Remember the system error code here.
			dwErrToReturn = GetLastError ();
			goto Return;
		}
		do
		{
			// Go through the folder and pick up each entry.
			if (!isDotOrDotDotW (wfdW.cFileName))
			{
				++ uiEnts;
				if (fedEnt)
				{
					if (!fedEnt (&sdOneEntry))
						break;
				}
				if	(
							wfdW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
						&&	pnSubLevels
						&&	*pnSubLevels
					)
				{
					*pnSubLevels -= 1;
					bool bSubpath = U8pathSMEMBUF_from_U8path_and_WinU16FileName    (
										pmb, strPathU8, USE_STRLEN, wfdW.cFileName
																					);
					if (bSubpath)
					{	// Recursively invoke us again.
						uiEnts += ForEachDirectoryEntryU8_Ex	(
												pmb->buf.pch,
												fedEnt,
												pCustom,
												pnSubLevels,
												pmb
																);
					}
					*pnSubLevels += 1;
				}
			}
		} while (FindNextFileW (hFind, &wfdW) != 0);

		// We want the caller to be able to obtain the last error produced by FindNextFileW ()
		//	instead of FindClose ().
		dwErrToReturn = GetLastError ();
		FindClose (hFind);

	Return:
		SetLastError (dwErrToReturn);
		return uiEnts;
	}
#endif

size_t ForEachDirectoryEntryU8		(
				const char				*strPathU8,
				pForEachDirEntryU8		fedEnt,
				void					*pCustom,
				size_t					*pnSubLevels
									)
{
	HANDLE				hFind;
	WIN32_FIND_DATAW	wfdW;
	size_t				uiEnts				= 0;			// The return value.
	SRDIRONEENTRYSTRUCT	sdOneEntry;
	DWORD				dwErrToReturn;

	sdOneEntry.UTF8orUTF16.pstrPathWorU8	= strPathU8;
	sdOneEntry.pwfd							= &wfdW;
	sdOneEntry.u							= EN_READ_DIR_ENTS_SDIRW_UTF8;
	sdOneEntry.pCustom						= pCustom;
	hFind = FindFirstFileU8 (strPathU8, &wfdW);
	if (INVALID_HANDLE_VALUE == hFind)
	{	// Maybe no files or whatever.
		return uiEnts;
	}
	do
	{
		// Go through the folder and pick up each entry.
		if (!isDotOrDotDotW (wfdW.cFileName))
		{
			++ uiEnts;
			if (fedEnt)
			{
				if (!fedEnt (&sdOneEntry))
					break;
			}
			if	(
						wfdW.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
					&&	pnSubLevels
					&&	*pnSubLevels
				)
			{
				*pnSubLevels -= 1;
				char *subPath = AllocU8path_from_U8path_and_WinU16FileName (strPathU8, wfdW.cFileName);
				if (subPath)
				{	// Recursively invoke us again.
					uiEnts += ForEachDirectoryEntryU8	(
									subPath,
									fedEnt,
									pCustom,
									pnSubLevels
														);
					DoneU8 (subPath);
				}
				*pnSubLevels += 1;
			}
		}
	} while (FindNextFileW (hFind, &wfdW) != 0);

	// We want the caller to be able to obtain the last error produced by FindNextFileW ()
	//	instead of FindClose ().
	dwErrToReturn = GetLastError ();
	FindClose (hFind);

	SetLastError (dwErrToReturn);
	return uiEnts;
}

static const char	ccCoverAllMask []	= "\\*";
#define SIZCOVERALLMSK	(sizeof (ccCoverAllMask))
#define LENCOVERALLMSK	(sizeof (ccCoverAllMask) - 1)

/*
	Requires correct path navigators.
	Requires correct lengths.
	No NULL checks.
*/
size_t ForEachDirEntryMaskU8intern	(
				const char				*szFolder,
				size_t					lnFolder,
				pForEachDirEntryU8		fedEnt,
				void					*pCustom,
				size_t					*pnSubLevels,
				SRDIRONEENTRYSTRUCT		*psdE
									)
{
	ubf_assert_non_NULL	(psdE);
	ubf_assert			(USE_STRLEN != lnFolder);
	ubf_assert			(USE_STRLEN != psdE->lnFileMask);

	size_t				uiEnts			= 0;				// The return value.
	DWORD				dwErrToReturn	= ERROR_SUCCESS;

	growToSizeSMEMBUF (&psdE->mbSearchPath, lnFolder + SIZCOVERALLMSK);
	if (isUsableSMEMBUF (&psdE->mbSearchPath))
	{
		memcpy (psdE->mbSearchPath.buf.pch, szFolder, lnFolder);
		memcpy (psdE->mbSearchPath.buf.pch + lnFolder, ccCoverAllMask, SIZCOVERALLMSK);
		psdE->lnSearchPath = lnFolder + LENCOVERALLMSK;

		HANDLE hFind = FindFirstFileU8long (psdE->mbSearchPath.buf.pch, psdE->pwfd);
		if (INVALID_HANDLE_VALUE == hFind)
		{	// Maybe no files or whatever.
			return uiEnts;
		}

		ubf_assert_non_0 (sizeof (UBF_WIN_DIR_SEP_STR) - 1);
		ubf_assert_non_0 (1 == sizeof (UBF_WIN_DIR_SEP_STR) - 1);
		growToSizeRetainSMEMBUF (&psdE->mbFullPathU8, lnFolder + sizeof (UBF_WIN_DIR_SEP_STR) - 1 + UTF8_MAX_PATH);
		if (isUsableSMEMBUF (&psdE->mbFullPathU8))
		{
			memcpy	(
				psdE->mbFullPathU8.buf.pch + lnFolder,
				UBF_WIN_DIR_SEP_STR,
				sizeof (UBF_WIN_DIR_SEP_STR)
					);
			psdE->szPathU8		= psdE->mbFullPathU8.buf.pch + psdE->lnInitPathU8 + 1;
			psdE->szFileNameU8	= psdE->szPathU8;
			bool bContinue		= true;
			do
			{	// Go through the folder and pick up each entry.
				if (!isDotOrDotDotW (psdE->pwfd->cFileName))
				{
					++ uiEnts;
					psdE->stFileNameU8 = UTF8_from_WinU16	(
												psdE->szFileNameU8, UTF8_MAX_PATH,
												psdE->pwfd->cFileName
															);
					ubf_assert (0 < psdE->stFileNameU8);
					psdE->lnPathU8 = psdE->stFileNameU8 - 1;
					bool bMatch = matchWildcardPattern	(
									psdE->szPathU8,		psdE->lnPathU8,
									psdE->szFileMask,	psdE->lnFileMask
														);
					bContinue = fedEnt && bMatch ? fedEnt (psdE) : true;
					if (
								bContinue
							&&	pnSubLevels
							&&	*pnSubLevels
							&&	psdE->pwfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
						)
					{
						*pnSubLevels -= 1;
						/*
						psdE->lnSearchPath = SMEMBUFstrconcat	(
												&psdE->mbSearchPath,	psdE->lnSearchPath,
												psdE->szFileNameU8,		psdE->stFileNameU8 - 1,
												64
																);
						*/
						// Recursively invoke us again.
						//size_t lnFullPath = lnFolder + 1;	// Includes terminating path separator.
						psdE->lnFullPathU8 = lnFolder + 1 + psdE->stFileNameU8 - 1;
						ubf_assert (strlen (psdE->mbFullPathU8.buf.pch) == psdE->lnFullPathU8);
						uiEnts = ForEachDirEntryMaskU8intern	(
										psdE->mbFullPathU8.buf.pcc,
										psdE->lnFullPathU8,
										fedEnt, pCustom, pnSubLevels, psdE
																);
						*pnSubLevels += 1;
					}
				}
			} while (bContinue && FindNextFileW (hFind, psdE->pwfd));

			// We want the caller to be able to obtain the last error produced by FindNextFileW ()
			//	instead of FindClose ().
			dwErrToReturn = GetLastError ();
			FindClose (hFind);
		} else
			dwErrToReturn = ERROR_NOT_ENOUGH_MEMORY;
	} else
		dwErrToReturn = ERROR_NOT_ENOUGH_MEMORY;
	SetLastError (dwErrToReturn);
	return uiEnts;
}

size_t ForEachDirectoryEntryMaskU8	(
				const char				*strFolderU8,
				size_t					lenFolderU8,
				const char				*strFileMaskU8,
				size_t					lenFileMaskU8,
				pForEachDirEntryU8		fedEnt,
				void					*pCustom,
				size_t					*pnSubLevels
									)
{
	ubf_assert_non_NULL	(strFolderU8);
	ubf_assert_non_0	(lenFolderU8);

	size_t	uiEnts		= 0;
	size_t	folderU8len = str_remove_last_dir_separator (strFolderU8, lenFolderU8);
	char	*szFolder;
	if (folderU8len)
	{
		szFolder = ubf_malloc (folderU8len + 1);
		if (szFolder)
		{
			memcpy (szFolder, strFolderU8, folderU8len); szFolder [folderU8len] = ASCII_NUL;
			str_correct_dir_separators (szFolder, folderU8len);
			str_remove_path_navigators (szFolder, &folderU8len);
			size_t fileMaskU8len = strFileMaskU8 ? (USE_STRLEN == lenFileMaskU8 ? strlen (strFileMaskU8) : lenFileMaskU8) : 0;

			WIN32_FIND_DATAW	wfdW;
			SRDIRONEENTRYSTRUCT	sdOneEntry;
			sdOneEntry.UTF8orUTF16.pstrPathWorU8	= strFolderU8;
			sdOneEntry.u							= EN_READ_DIR_ENTS_SDIRW_UTF8;
			sdOneEntry.pwfd							= &wfdW;
			sdOneEntry.pCustom						= pCustom;
			sdOneEntry.szFileMask					= strFileMaskU8;
			sdOneEntry.lnFileMask					= fileMaskU8len;
			sdOneEntry.szPathU8						= NULL;

			INITSMEMBUF (sdOneEntry.mbSearchPath);
			INITSMEMBUF (sdOneEntry.mbFullPathU8);

			sdOneEntry.lnInitPathU8 = folderU8len;
			size_t stToReserve = folderU8len + sizeof (UBF_WIN_DIR_SEP_STR) - 1 + UTF8_MAX_PATH;
			growToSizeSMEMBUF (&sdOneEntry.mbFullPathU8, stToReserve);
			if (isUsableSMEMBUF (&sdOneEntry.mbFullPathU8))
			{
				memcpy (sdOneEntry.mbFullPathU8.buf.pch, szFolder, folderU8len);
				memcpy	(
					sdOneEntry.mbFullPathU8.buf.pch + folderU8len,
					UBF_WIN_DIR_SEP_STR, sizeof (UBF_WIN_DIR_SEP_STR)
						);
				uiEnts = ForEachDirEntryMaskU8intern	(
							szFolder, folderU8len,
							fedEnt, pCustom, pnSubLevels, &sdOneEntry
														);
			}

			DONESMEMBUF (sdOneEntry.mbFullPathU8);
			DONESMEMBUF (sdOneEntry.mbSearchPath);
			ubf_free (szFolder);
		}
	} else
		SetLastError (ERROR_INVALID_PARAMETER);
	return uiEnts;
}

#ifdef CUNILOG_BUILD_READDIR_TESTFNCT
	static uint64_t uiTstGlob;

	bool perFile (SRDIRONEENTRYSTRUCT *psdE)
	{
		ubf_assert_non_NULL (psdE);
		ubf_assert (EN_READ_DIR_ENTS_SDIRW_UTF8 == psdE->u);

		char cFilename [UTF8_MAX_PATH];
		UTF8_from_WinU16 (cFilename, UTF8_MAX_PATH, psdE->pwfd->cFileName);
		printf ("P: %s, %s\n", psdE->UTF8orUTF16.chPathU8, cFilename);
		++ uiTstGlob;
		if (uiTstGlob >= 100)
			ubf_assert (false);
		return true;
	}

	bool ForEachDirectoryEntryMaskU8TestFnct (void)
	{
		bool b = true;

		size_t	ui	= SIZE_MAX;
		size_t	n;

		n = ForEachDirectoryEntryMaskU8	(
				"C:\\temp",		USE_STRLEN,
				"*.txt",		USE_STRLEN,
				perFile, NULL, &ui
										);
		UNREFERENCED_PARAMETER (n);
		/*
		n = ForEachDirectoryEntryMaskU8	(
				"C:/temp/",		USE_STRLEN,
				"*.txt",		USE_STRLEN,
				perFile, NULL, &ui
										);
		UNREFERENCED_PARAMETER (n);
		n = ForEachDirectoryEntryMaskU8	(
				"C:\\temp",		USE_STRLEN,
				"*.txt",		USE_STRLEN,
				perFile, NULL, &ui
										);
		UNREFERENCED_PARAMETER (n);

		n = ForEachDirectoryEntryMaskU8	(
				"C:\\temp\\",	USE_STRLEN,
				"*.txt",		USE_STRLEN,
				perFile, NULL, &ui
										);
		*/

		UNREFERENCED_PARAMETER (ui);
		UNREFERENCED_PARAMETER (n);
		UNREFERENCED_PARAMETER (b);

		return b;
	}
#endif

#endif														// Of #ifdef _WIN32.
/****************************************************************************************

	File		CompressNTFS_U8.c
	Why:		Performs NTFS compression.
	OS:			Windows
	Created:	2020-12-06

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2020-12-06	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifdef _WIN32

#include <windows.h>
#ifdef _MSC_VER
	#include <crtdbg.h>
#endif

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./CompressNTFS_U8.h"
	#include "./WinAPI_U8.h"

#endif

#ifndef UNICODE
	#error This module can only be used in Windows applications compiled for unicode
#endif

static bool DeviceIoControlNTFScompression (HANDLE hFile, USHORT usState)
{
	DWORD		dwBytesReturned;
	bool		bResult;

	bResult = DeviceIoControl	(
					hFile,						// Handle to file or directory
					FSCTL_SET_COMPRESSION,		// dwIoControlCode
					&usState,					// Input buffer
					sizeof (USHORT),			// Size of input buffer
					NULL,						// lpOutBuffer
					0,							// nOutBufferSize
					&dwBytesReturned,			// Number of bytes returned
					NULL						// OVERLAPPED structure
								);
	return bResult;
}

bool CompressFileNTFS_U8ExLong (const char *chFileName, USHORT usState)
{	// Compress or uncompress theFossil_Commit given file.
	//
	//	usState can be:
	//	0 = COMPRESSION_FORMAT_NONE
	//	1 = COMPRESSION_FORMAT_DEFAULT
	//	2 = COMPRESSION_FORMAT_LZNT1
	//
	//  Returns TRUE if successful, FALSE if an error occurred.
	//

	HANDLE		hFile;
	bool		bResult;

	hFile = CreateFileU8long	(
					chFileName,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_DELETE + FILE_SHARE_READ + FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL + FILE_FLAG_SEQUENTIAL_SCAN,
					NULL		
								);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = GetLastError ();
		UNREFERENCED_PARAMETER (dwErr);
		return false;
	}
	bResult = DeviceIoControlNTFScompression (hFile, usState);
	if (!CloseHandle (hFile))
		return false;
	return (bResult);
}

bool CompressFileNTFS_U8Ex (const char *chFileName, USHORT usState)
{	// Compress or uncompress the given file.
	//
	//	usState can be:
	//	0 = COMPRESSION_FORMAT_NONE
	//	1 = COMPRESSION_FORMAT_DEFAULT
	//	2 = COMPRESSION_FORMAT_LZNT1
	//
	//  Returns TRUE if successful, FALSE if an error occurred.
	//

	HANDLE		hFile;
	bool		bResult;

	hFile = CreateFileU8		(
					chFileName,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_DELETE + FILE_SHARE_READ + FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL + FILE_FLAG_SEQUENTIAL_SCAN,
					NULL		
								);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD dwErr = GetLastError ();
		UNREFERENCED_PARAMETER (dwErr);
		return false;
	}
	bResult = DeviceIoControlNTFScompression (hFile, usState);
	if (!CloseHandle (hFile))
		return false;
	return (bResult);
}

bool CompressFileNTFS_U8long (const char *chFileName)
{
	return (CompressFileNTFS_U8ExLong (chFileName, COMPRESSION_FORMAT_DEFAULT));
}

bool CompressFileNTFS_U8 (const char *chFileName)
{
	return (CompressFileNTFS_U8Ex (chFileName, COMPRESSION_FORMAT_DEFAULT));
}

bool UnCompressFileNTFS_U8long (const char *chFileName)
{
	return (CompressFileNTFS_U8ExLong (chFileName, COMPRESSION_FORMAT_NONE));
}

bool UnCompressFileNTFS_U8 (const char *chFileName)
{
	return (CompressFileNTFS_U8Ex (chFileName, COMPRESSION_FORMAT_NONE));
}

enntfscompressresult IsFileNTFSCompressedByName (const char *szFileName)
{
	DWORD dwAtts = GetFileAttributesU8 (szFileName);
	
	if (INVALID_FILE_ATTRIBUTES != dwAtts)
	{
		return (dwAtts & FILE_ATTRIBUTE_COMPRESSED) ? ntfscompress_compressed : ntfscompress_uncompressed;
	}
	return ntfscompress_error;
}

enntfscompressresult IsFileNTFSCompressedByHandle (HANDLE hFile)
{
	BY_HANDLE_FILE_INFORMATION	bh;

	if (GetFileInformationByHandle (hFile, &bh))
	{
		return (bh.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) ? ntfscompress_compressed : ntfscompress_uncompressed;
	}
	return ntfscompress_error;
}

#endif														// Of #ifdef _WIN32.
/****************************************************************************************

	File:		WinSharedMutex.c
	Why:		Implements a mutex for interprocess-locking.
	OS:			C99
	Author:		Thomas
	Created:	2021-02-23
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2021-02-23	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
MIT License

Copyright (c) 2018 Oleg Yamnikov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifdef _WIN32

#ifndef CUNILOG_USE_COMBINED_MODULE
	#include "./WinSharedMutex.h"
#endif

shared_mutex_t WinInitSharedMutex(const char *name)
{
	shared_mutex_t s = malloc (sizeof (struct ssharedmutext));
	if (s)
	{
		s->h = CreateMutexU8 (NULL, FALSE, name);
		if (s->h)
		{
			DWORD dwErr = GetLastError ();
			s->bCreatedHere = ERROR_ALREADY_EXISTS != dwErr;
		} else
			goto fail;
	}
	return s;

fail:
	free (s);
	return NULL;
}

int WinCloseSharedMutex(shared_mutex_t mutex)
{
	return CloseHandle (mutex->h) ? 0 : 1;
}

void WinDestroySharedMutex(shared_mutex_t mutex)
{	// In Windows, the mutex is destroyed automatically when the last open
	//	handle to it is closed. Refer to
	//	https://docs.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createmutexw
	//	for clarification.
	if (mutex->h)
		CloseHandle (mutex->h);
	free (mutex);
}

#endif															// Of #ifdef _WIN32
/****************************************************************************************

	File:		WinExeFileName.c
	Why:		Obtains the name of the executable.
	OS:			Windows.
	Author:		Thomas
	Created:	2024-05-21

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-05-21	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <malloc.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./WinExeFileName.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./WinAPI_U8.h"
		#include "./unref.h"
		#include "./memstrstr.h"
		#include "./ubfdebug.h"
		#include "./Warnings.h"
	#else
		#include "./WinAPI_U8.h"
		#include "./../../pre/unref.h"
		#include "./../../pre/Warnings.h"
		#include "./../../mem/memstrstr.h"
		#include "./../../dbg/ubfdebug.h"
		#include "./../../pre/Warnings.h"
	#endif

#endif

#ifdef OS_IS_WINDOWS

	SMEMBUF mbOurExecutablePath = SMEMBUF_INITIALISER;
	size_t	lnOurExectuablePath = 0;

	/*
		Stores our executable's module name and its length in the variables above.
	*/
	static void StoreExecutableModuleName (SMEMBUF *mb, size_t ln)
	{
		ubf_assert_non_NULL (mb);
		ubf_assert (isInitialisedSMEMBUF (mb));
		ubf_assert_non_0 (ln);
		ubf_assert (USE_STRLEN != ln);

		if (!isUsableSMEMBUF (&mbOurExecutablePath))
		{
			copySMEMBUF (&mbOurExecutablePath, mb);
			lnOurExectuablePath = ln;

			#ifdef DEBUG
				size_t sl1 = strlen (mbOurExecutablePath.buf.pch);
				UNUSED (sl1);
			#endif

			// Ensure we have a NUL terminator.
			ubf_assert (0 == mbOurExecutablePath.buf.pch [ln]);
			ubf_assert (strlen (mbOurExecutablePath.buf.pch) == ln);
		}
	}

	/*
		If we already obtained our own executable's module name, the function resizes
		the buffer of mb to the size of mbOurExecutablePath and copies it in, basically
		returning a copy of our executable's module name.

		The function returns the length of the previously obtained module name, or 0
		if none has been obtained yet.
	*/
	static size_t WeHaveExecutableModuleNameAlready (SMEMBUF *mb)
	{
		ubf_assert_non_NULL (mb);
		ubf_assert (isInitialisedSMEMBUF (mb));

		if (isUsableSMEMBUF (&mbOurExecutablePath))
		{
			copySMEMBUF (mb, &mbOurExecutablePath);
			// Note that we do not check here if the copy operation was successful.
			//	We leave this within the caller's responsibility.
			return lnOurExectuablePath;
		}
		return 0;
	}

	DISABLE_WARNING_ASSIGNMENT_WITHIN_CONDITIONAL_EXPRESSION ()

	size_t WinObtainExecutableModuleName (SMEMBUF *mb)
	{
		ubf_assert_non_NULL (mb);

		size_t len;												// The return value.
		if (len = WeHaveExecutableModuleNameAlready (mb))
			return len;

		DWORD	dwCurrSiz	= EXEFILENAME_MALLOC_BLOCKSIZE;
		DWORD	dw;
		char	*szExe;

		do
		{
			szExe = ubf_malloc (dwCurrSiz);
			if (szExe)
			{
				dw = GetModuleFileNameU8 (NULL, szExe, dwCurrSiz);
				if (dw && dw < dwCurrSiz)
				{	// See
					//	https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-getmodulefilenamew .
					size_t	elen = dw;
					growToSizeSMEMBUF (mb, elen + 1);
					if (isUsableSMEMBUF (mb))
					{
						size_t l = strlen (szExe);
						UNUSED_PARAMETER (l);
						memcpy (mb->buf.pch, szExe, elen + 1);
						size_t m = strlen (mb->buf.pch);
						UNUSED_PARAMETER (m);
						StoreExecutableModuleName (mb, elen);
						ubf_free (szExe);
						return dw;
					}
				}
				ubf_free (szExe);
				if (0 == dw)
				{
					ubf_assert (false);
					return false;
				}
				dwCurrSiz += EXEFILENAME_MALLOC_BLOCKSIZE;
			} else
				break;
		} while (dwCurrSiz < INT16_MAX);
		return 0;
	}

	DEFAULT_WARNING_ASSIGNMENT_WITHIN_CONDITIONAL_EXPRESSION ()

	DISABLE_WARNING_ASSIGNMENT_WITHIN_CONDITIONAL_EXPRESSION ()

	size_t WinObtainAppNameFromExecutableModule (SMEMBUF *mb)
	{
		ubf_assert_non_NULL (mb);

		size_t	l;

		if ((l = WinObtainExecutableModuleName (mb)))
		{
		
			// This works only on Windows:
			char *dot = memstrrchr (mb->buf.pch, l, '.');
			if (dot)
			{
				*dot = '\0';
				char *sl = memstrrchr (mb->buf.pch, strlen (mb->buf.pch), '\\');
				if (sl)
				{
					char	*sn = sl + 1;							// From after '\\'
					size_t	len = dot - sn;							//	to before '\0'.
					memmove (mb->buf.pch, sn, len + 1);
					return len;
				}
				// No backslash. Not sure if this needs a different implementation.
				ubf_assert (false);
			}
		}
		return 0;
	}

	DEFAULT_WARNING_ASSIGNMENT_WITHIN_CONDITIONAL_EXPRESSION ()

	size_t WinObtainPathFromExecutableModule (SMEMBUF *mb)
	{
		ubf_assert_non_NULL (mb);

		if (WinObtainExecutableModuleName (mb))
		{
			size_t	l = strlen (mb->buf.pch);
			UNUSED_PARAMETER (l);
		
			char *sl = memstrrchr (mb->buf.pch, strlen (mb->buf.pch), '\\');
			if (sl)
			{
				++ sl;
				*sl = '\0';
				return sl - mb->buf.pch;
			}
			// No backslash. Not sure if this needs a different implementation.
			ubf_assert (false);
		}
		return 0;
	}

#endif														// Of #ifdef OS_IS_WINDOWS
/****************************************************************************************

	File:		PsxCompressFile.c
	Why:		Module to compress files on POSIX platforms.
	OS:			POSIX.
	Author:		Thomas
	Created:	2024-10-17

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-10-17	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./PsxCompressFile.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./platform.h"
	#else
		#include "./../../pre/platform.h"
	#endif

#endif

#ifdef PLATFORM_IS_POSIX

#ifdef CHAT_GPT_NONSENSE
/*
	2024-10-17

	This code supposed to be for the btrfs file system, created by ChatGPT.
	
	Q: "How do I write a C program that compresses a file on the btrfs file system?"

	Obviously, this code is utter nonsense. I shall leave it here for future references
	whenever someone suggests ChatGPT as being a useful tool.
*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/btrfs.h>
#include <string.h>
#include <stdlib.h>

void enable_compression (const char *path)
{
	int fd = open (path, O_RDWR);
	if (fd < 0)
	{
		perror ("Failed to open file");
		return;
	}

	// Set the compression type to zlib (you can also use lzo or zstd)
	struct btrfs_ioctl_set_compression_args comp_args;
	memset (&comp_args, 0, sizeof (comp_args));
	comp_args.compression = BTRFS_COMPRESS_ZLIB;  // Choose your compression type

	if (ioctl (fd, BTRFS_IOC_SET_COMPRESSION, &comp_args) < 0)
	{
		perror ("Failed to set compression");
		close (fd);
		return;
	}

	printf ("Compression enabled for %s\n", path);
	close (fd);
}

int main (int argc, char *argv [])
{
	if (argc < 2)
	{
		fprintf (stderr, "Usage: %s <file_path>\n", argv [0]);
		return 1;
	}

	enable_compression (argv [1]);
	return 0;
}
#endif


#endif														// Of #ifdef PLATFORM_IS_POSIX.
/****************************************************************************************

	File:		PsxExeFileName.c
	Why:		Obtains the name of the executable.
	OS:			POSIX.
	Author:		Thomas
	Created:	2024-10-14

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-10-14	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <malloc.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./PsxExeFileName.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./membuf.h"
		#include "./memstrstr.h"
		#include "./ubfdebug.h"
		#include "./strintuint.h"
		#include "./unref.h"
	#else
		#include "./../../mem/membuf.h"
		#include "./../../mem/memstrstr.h"
		#include "./../../dbg/ubfdebug.h"
		#include "./../../string/strintuint.h"
		#include "./../../pre/unref.h"
	#endif

#endif

#ifdef PLATFORM_IS_POSIX

#include <unistd.h>

SMEMBUF	mbOurExecutablePath		= SMEMBUF_INITIALISER;
size_t	lenOurExectuablePath;

/*
	Stores our executable's module name and its length in the variables above.
*/
static void StoreExecutableModuleName (SMEMBUF *mb, size_t ln)
{
	ubf_assert_non_NULL (mb);
	ubf_assert (isInitialisedSMEMBUF (mb));
	ubf_assert_non_0 (ln);
	ubf_assert (USE_STRLEN != ln);

	if (!isUsableSMEMBUF (&mbOurExecutablePath))
	{
		copySMEMBUF (&mbOurExecutablePath, mb);
		lenOurExectuablePath = ln;

		// Ensure we have a NUL terminator.
		ubf_assert (0 == mbOurExecutablePath.buf.pch [ln]);
		ubf_assert (strlen (mbOurExecutablePath.buf.pch) == ln);
	}
}

/*
	If we already obtained our own executable's module name, the function resizes
	the buffer of mb to the size of mbOurExecutablePath and copies it in, basically
	returning a copy of our executable's module name.

	The function returns the length of the previously obtained module name, or 0
	if none has been obtained yet.
*/
static size_t WeHaveExecutableModuleNameAlready (SMEMBUF *mb)
{
	ubf_assert_non_NULL (mb);
	ubf_assert (isInitialisedSMEMBUF (mb));

	if (isUsableSMEMBUF (&mbOurExecutablePath))
	{
		copySMEMBUF (mb, &mbOurExecutablePath);
		// Note that we do not check here if the copy operation was successful.
		//	We leave this within the caller's responsibility.
		return lenOurExectuablePath;
	}
	return 0;
}

static unsigned int		uiDetExeMethod;							// The strategy used to
																//	determine the executable's
																//	path/folder.

#ifdef OS_IS_MACOS
	/*
		Apple devices. See
		https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/dyld.3.html .
	*/
	#include <mach-o/dyld.h>

	size_t PsxObtainExecutableModuleName (SMEMBUF *mb)
	{
		ubf_assert_non_NULL (mb);

		size_t len;												// The return value.
		if (len = WeHaveExecutableModuleNameAlready (mb))
			return len;

		chPath = ubf_malloc (exe_size);
		while (chPath && -1 == _NSGetExecutablePath (chPath, &exe_size))
		{
			ubf_free (chPath);
			exe_size += 256;
			#ifdef DEBUG
			if (8192 <= exe_size)
			{	// Emergency brake in debug version. In the release version we got no
				//	choice but to use however long the path is.
				ubf_assert (FALSE);
			}
			#endif
			chPath = ubf_malloc (exe_size);
			ubf_assert (NULL != chPath);
		}
		if (chPath)
		{
			ch_ubf_Executable_Path = chPath;
			ln_ubf_Executable_Path = (size_t) exe_size - 1;
			return ch_ubf_Executable_Path;
		}
		return NULL;
	}
#else
	/*
		Other Unixes. See
		https://stackoverflow.com/questions/24581908/c-lstat-on-proc-pid-exe .
	*/
	static size_t pid_as_string (char *szPID)
	{
		int pid = getpid ();
		ubf_assert (0 < pid);								// Negative PIDs relate to a group.
		if (pid > 0)
		{	// We only expect a normal PID.
			uint64_t ui = pid;
			return ubf_str_from_uint64 (szPID, ui);
		}
		return 0;
	}

	/*
		Check if these links exist on various operating systems anduse the one
		that does exist. The first one's Linux.
	*/
	const char szProc	[] = "/proc/";
	const char szExe	[] = "/exe";

	size_t PsxObtainExecutableModuleName (SMEMBUF *mb)
	{
		ubf_assert_non_NULL (mb);

		size_t len;
		if ((len = WeHaveExecutableModuleNameAlready (mb)))
			return len;

		char			cPath [1024];
		size_t			exe_size = 1024;
		size_t			exe_used;

		char szCheckLnk [32 + UBF_UINT64_SIZE];
		char *sz = szCheckLnk;
		uiDetExeMethod = 1;

		// "/proc/<pid>/exe"
		memcpy (sz, szProc, sizeof (szProc) - 1);
		sz += pid_as_string (sz + sizeof (szProc) - 1);
		memcpy (sz, szExe, sizeof (szExe) - 1);
		sz [sizeof (szExe) - 1] = '\0';

		exe_used = readlink (szCheckLnk, cPath, exe_size - 1);
		if ((size_t) -1 == exe_used)
		{	// FreeBSD.
			++ uiDetExeMethod;
			exe_used = readlink ("/proc/curproc/file", cPath, exe_size - 1);
			if ((size_t) -1 == exe_used)
			{	// Solaris.
				++ uiDetExeMethod;
				exe_used = readlink ("/proc/self/path/a.out", cPath, exe_size - 1);
				if ((size_t) -1 == exe_used)
				{	// Linux again.
					++ uiDetExeMethod;
					exe_used = readlink ("/proc/self/exe", cPath, exe_size - 1);
					if ((size_t) -1 == exe_used)
					{	// What else can we try?
						++ uiDetExeMethod;
						ubf_assert (false);
						ASSERT (false);
						return 0;
					}
				}
			}
		}
		if (exe_used == exe_size)
		{	// Buffer is too small.
			ubf_assert (false);
			return 0;
		}
		if (growToSizeSMEMBUF (mb, exe_used + 1))
		{
			memcpy (mb->buf.pch, cPath, exe_used);
			mb->buf.pch [exe_used] = '\0';
			StoreExecutableModuleName (mb, exe_used);
		}
		return exe_used;
	}
#endif

size_t PsxObtainAppNameFromExecutableModule (SMEMBUF *mb)
{
	ubf_assert_non_NULL (mb);

	size_t	l;

	if ((l = PsxObtainExecutableModuleName (mb)))
	{
		// This only works on POSIX.		
		char *sl = memstrrchr (mb->buf.pch, strlen (mb->buf.pch), '/');
		if (sl)
		{
			char	*sn = sl + 1;
			size_t	len = mb->buf.pch + l - sn;
			memmove (mb->buf.pch, sn, len + 1);
			return len;
		}
		// No forward slash. Not sure if this needs a different implementation.
		ubf_assert (false);
	}
	return 0;
}

size_t PsxObtainPathFromExecutableModule (SMEMBUF *mb)
{
	ubf_assert_non_NULL (mb);

	if (PsxObtainExecutableModuleName (mb))
	{
		size_t	l = strlen (mb->buf.pch);
		UNREFERENCED_PARAMETER (l);
		
		char *sl = memstrrchr (mb->buf.pch, strlen (mb->buf.pch), '/');
		if (sl)
		{
			++ sl;
			*sl = '\0';
			return sl - mb->buf.pch;
		}
		// No forward slash. Not sure if this needs a different implementation.
		ubf_assert (false);
	}
	return 0;
}

#endif													// Of #ifdef OS_IS_POSIX.
/****************************************************************************************

	File:		PsxReadDirFncts.c
	Why:		POSIX functions to read directory/folder contents.
	OS:			POSIX.
	Author:		Thomas
	Created:	2024-10-22

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-10-22	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <malloc.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./PsxReadDirFncts.h"
	#include "./../../pre/unref.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./membuf.h"
		#include "./ubfdebug.h"
		#include "./strintuint.h"
	#else
		#include "./../../mem/membuf.h"
		#include "./../../dbg/ubfdebug.h"
		#include "./../../string/strintuint.h"
	#endif

#endif

#ifdef PLATFORM_IS_POSIX

#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>

uint64_t ForEachPsxDirEntry	(
				const char				*strFolderName,
				pForEachPsxDirEntry		fedEnt,
				void					*pCustom,
				unsigned int			*pnSubLevels
							)
{
	uint64_t			uiEnts				= 0;			// The return value.
	DIR					*dir;
	struct dirent		*dent;
	unsigned char		tpy;
	SONEDIRENT			pod;

	UNUSED (pnSubLevels);

	pod.pCustom = pCustom;

	dir = opendir (strFolderName);
	if (dir)
	{
		while (NULL != (dent = readdir (dir)))
		{
			tpy = dent->d_type;
			if (DT_DIR == tpy)
			{	// Recursion is currently not supported because Cunilog doesn't
				//	require it.
			} else
			if (DT_REG == tpy)
			{	// Regular file.
				++ uiEnts;
				pod.dirEnt = dent;
				if (!fedEnt (&pod))
					break;
			} else
			if (DT_UNKNOWN == tpy)
			{	// See https://man7.org/linux/man-pages/man3/readdir.3.html .
				/*
					Excerpt:
					Currently, only some filesystems (among them: Btrfs, ext2,
					ext3, and ext4) have full support for returning the file
					type in d_type.  All applications must properly handle a
					return of DT_UNKNOWN.
				*/

				/*
				if (!isDotOrDotDotW (dent->d_name))
				{
					
				}
				*/
			}
		}
		closedir (dir);
	} else
	{	// Fail. See https://man7.org/linux/man-pages/man3/opendir.3.html .
		/*
			   EACCES	Permission denied.
			   EBADF	fd is not a valid file descriptor opened for reading.
			   EMFILE	The per-process limit on the number of open file
						descriptors has been reached.
			   ENFILE	The system-wide limit on the total number of open files
						has been reached.
			   ENOENT	Directory does not exist, or name is an empty string.
			   ENOMEM	Insufficient memory to complete the operation.
			   ENOTDIR	name is not a directory.
		*/
	}
	return uiEnts;
}



#endif													// Of #ifdef OS_IS_POSIX.
/****************************************************************************************

	File:		PsxHome.c
	Why:		Obtains a user's home directory on POSIX.
	OS:			POSIX, Windows.
	Author:		Thomas
	Created:	2024-11-03

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-11-03	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./PsxHome.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
		#include "./platform.h"
		#include "./strmembuf.h"
	#else
		#include "./../../dbg/ubfdebug.h"
		#include "./../../pre/platform.h"
		#include "./../../string/strmembuf.h"
	#endif

#endif

#ifdef PLATFORM_IS_POSIX

#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <unistd.h>
#include <stdlib.h>

static bool	bInitialised;
static SMEMBUF		smbUserName			= SMEMBUF_INITIALISER;
static size_t		lenUserName;
static SMEMBUF		smbUserHome			= SMEMBUF_INITIALISER;
static size_t		lenUserHome;
static SMEMBUF		smbXDG_DATA_HOME	= SMEMBUF_INITIALISER;
static size_t		lenXDG_DATA_HOME;
static bool			bXDG_DATA_HOMEunavailable;
static SMEMBUF		smbUserTrashFolder	= SMEMBUF_INITIALISER;
static size_t		lenUserTrashFolder;

static SMEMBUF		smbUserID			= SMEMBUF_INITIALISER;
static size_t		lenUserID;
static uid_t		uidUserID;

// See
//	https://wiki.archlinux.org/title/XDG_Base_Directory .
bool obtainXDG_DATA_HOME (void)
{
	if (bXDG_DATA_HOMEunavailable)
		return false;
	char *sz = getenv ("XDG_DATA_HOME");
	if (sz)
	{
		lenXDG_DATA_HOME = SMEMBUFfromStr (&smbXDG_DATA_HOME, sz, strlen (sz) + 1);
		if (lenXDG_DATA_HOME) -- lenXDG_DATA_HOME;
		return true;
	}
	bXDG_DATA_HOMEunavailable = true;
	return false;
}

static inline void setTxtUserID (uid_t uid)
{
	if (uid != uidUserID)
	{
		lenUserID = SMEMBUFstrFromUINT64 (&smbUserID, uid);
		if (lenUserID) -- lenUserID;
	}
}

static inline uid_t getOurUserID (void)
{
	uid_t	uid;
	
	// Not sure what should be used here. The real or the effective user id?
	uid = geteuid ();
	//uid = getuid ();
	uidUserID = uid;
	
	setTxtUserID (uid);
	return uid;
}

bool obtainCurrentUserNamePOSIX (void)
{
	uid_t uid = getOurUserID ();
	const struct passwd *pw = getpwuid (uid);
	if (pw)
	{
		lenUserName = SMEMBUFfromStr (&smbUserName, pw->pw_name, strlen (pw->pw_name) + 1);
		if (lenUserName) -- lenUserName;
		return true;
	}
	
	ubf_assert_msg (false, "obtainCurrentUserNamePOSIX () failed.");
	return false;
}

static inline const char *ccCurrentUserName (void)
{
	if (NULL == smbUserName.buf.pcc)
		obtainCurrentUserNamePOSIX ();
    return smbUserName.buf.pcc;    
}

bool InitPsxHome (void)
{
	bool bRet;
	
	obtainXDG_DATA_HOME ();
	bRet = obtainCurrentUserNamePOSIX ();
	
	return bRet;
}

// See https://www.man7.org/linux/man-pages/man3/getpwnam.3.html .
const char *UserHomeDirectoryPOSIX (size_t *plen)
{
	if (!bInitialised)
	{
		struct passwd pw;
		struct passwd *ppw;
		
		const char *szMe = ccCurrentUserName ();
		if (szMe)
		{
			size_t siz = sysconf (_SC_GETPW_R_SIZE_MAX);
			if ((size_t) - 1 == siz)						// Max. size could not be determined.
				siz = 16384;								// Should be sufficient enough.
			char *pwdbuf = malloc (siz);
			if (pwdbuf)
			{
				getpwnam_r (szMe, &pw, pwdbuf, siz, &ppw);
				if (ppw)
				{	
					lenUserHome = SMEMBUFfromStr	(
						&smbUserHome, ppw->pw_dir, strlen (ppw->pw_dir) + 1
													);
					if (lenUserHome) -- lenUserHome;
				}
				free (pwdbuf);
			}
		}
	}
	if (plen)
		*plen = lenUserHome;
	return smbUserHome.buf.pcc;
}

static const char	ccTrash		[]	= "Trash";
static const char	ccHTrash	[]	= ".local/share/Trash";

static void buildXDG_DATA_HomeTrashName (void)
{
	// $HOME/.local/share + / + Trash + NUL
	growToSizeSMEMBUF (&smbUserTrashFolder, lenXDG_DATA_HOME + sizeof (ccTrash) + 1);
	if (isUsableSMEMBUF (&smbUserTrashFolder))
	{
		memcpy (smbUserTrashFolder.buf.pch, smbXDG_DATA_HOME.buf.pch, lenXDG_DATA_HOME);
		smbUserTrashFolder.buf.pch [lenXDG_DATA_HOME] = '/';
		memcpy (smbUserTrashFolder.buf.pch + lenXDG_DATA_HOME + 1, ccTrash, sizeof (ccTrash));
		lenUserTrashFolder = lenXDG_DATA_HOME + sizeof (ccTrash);
	}
}

static void buildUserHomeTrashName (void)
{
	if (NULL == smbUserHome.buf.pcc)
		UserHomeDirectoryPOSIX (NULL);
	if (smbUserHome.buf.pcc)
	{
		growToSizeSMEMBUF (&smbUserTrashFolder, lenUserHome + sizeof (ccHTrash) + 1);
		if (isUsableSMEMBUF (&smbUserTrashFolder))
		{
			memcpy (smbUserTrashFolder.buf.pch, smbUserHome.buf.pch, lenUserHome);
			smbUserTrashFolder.buf.pch [lenUserHome] = '/';
			memcpy (smbUserTrashFolder.buf.pch + lenUserHome + 1, ccHTrash, sizeof (ccHTrash));
			lenUserTrashFolder = lenUserHome + sizeof (ccHTrash);
		}
	}
}

const char *UserTrashFolder (size_t *plen)
{
	if (NULL == smbUserTrashFolder.buf.pcc)
	{
		if (obtainXDG_DATA_HOME ())
			buildXDG_DATA_HomeTrashName ();
		else
			buildUserHomeTrashName ();
	}
	if (plen)
		*plen = lenUserTrashFolder;
	return smbUserTrashFolder.buf.pcc;
}

#endif														// Of #ifdef PLATFORM_IS_POSIX.

#ifdef PLATFORM_IS_POSIX
	#ifdef PSXHOME_BUILD_TEST_FNCT
		bool TestPsxHome (void)
		{
			bool	b = false;
			
			b = obtainXDG_DATA_HOME ();
			size_t st;
			const char *szTrash = UserTrashFolder (&st);
			b = NULL != szTrash;
			return b;
		}
	#endif
#endif
/****************************************************************************************

	File:		PsxTrash.c
	Why:		Handles the trash can for POSIX.
	OS:			POSIX, Windows.
	Author:		Thomas
	Created:	2024-11-03

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-11-03	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./PsxTrash.h"
	#include "./PsxHome.h"
	
	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE

		#include "./ubfdebug.h"
		#include "./platform.h"
		#include "./strmembuf.h"
		#include "./ubf_date_and_time.h"

	#else

		#include "./../../dbg/ubfdebug.h"
		#include "./../../pre/platform.h"
		#include "./../../string/strmembuf.h"
		#include "./../../datetime/ubf_date_and_time.h"

	#endif

#endif

#ifdef OS_IS_LINUX

#include <string.h>
#include <errno.h>

// The filename extension of files in the "info" folder.
const char szDotTrashInfo	[]	= ".trashinfo";
// The info that goes in the file in the "info" folder with filename extension ".trashinfo".
const char szTrashInfo		[]	= "[Trash Info]";
const char szPath			[]	= "Path=";
const char szDelDate		[]	= "DeletionDate=";

static SMEMBUF smbTrashInfo			= SMEMBUF_INITIALISER;	// The ".trashinfo" file in "info".
static SMEMBUF smbTrashInfoFilename	= SMEMBUF_INITIALISER;	// Its filename.

static inline bool createTrashInfo (const char *szFilename, size_t lnFilename)
{
	size_t len =		sizeof (szTrashInfo)				// "[Trash Info]" + LF
					+	sizeof (szPath) + lnFilename		// "Path=" + szFilename + LF.
					+	sizeof (szDelDate) + LEN_ISO8601DATETIMESTAMP_NO_OFFS;
					
	return false;
}

/*
	There's two cases:
	
	1.	Per user:		~/.local/share/Trash/files
						Create manually if it doesn't exist.

	2.	Per volume:		/media/<username>/<volume_name>/.Trash/
						For instance	/media/thomas/sda1/.Trash/files/
										/media/thomas/sda1/.Trash/info/ (for metadata)
										
	
*/
bool MoveFileToTrashPOSIX (const char *szFilename)
{
	ubf_assert_non_NULL (szFilename);
	
	size_t		lnHome;
	const char *szHome = UserHomeDirectoryPOSIX (&lnHome);
	if (lnHome)
	{
		ubf_assert_non_NULL (szHome);
		
		size_t lnFilename = strlen (szFilename);
		if (createTrashInfo (szFilename, lnFilename))
		{

		}
		return false;
	}
	errno = ENOSYS;											// Function not implemented.
	return false;
}

#endif														// Of #ifdef OS_IS_LINUX.
/****************************************************************************************

	File:		PsxSharedMutex.c
	Why:		Implements a mutex for interprocess-locking.
	OS:			C99
	Author:		Thomas
	Created:	2021-02-23
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2021-02-23	Thomas			Created. See
							 https://gist.github.com/marcetcheverry/d2f62672ee98bf54bfad7b8bc0840616
							 and
							 https://gist.github.com/yamnikov-oleg/abf61cf96b4867cbf72d .

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
MIT License

Copyright (c) 2018 Oleg Yamnikov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./PsxSharedMutex.h"

#endif

#ifdef PLATFORM_IS_POSIX

#include <errno.h> // errno, ENOENT
#include <fcntl.h> // O_RDWR, O_CREATE
#include <linux/limits.h> // NAME_MAX
#include <sys/mman.h> // shm_open, shm_unlink, mmap, munmap,
					  // PROT_READ, PROT_WRITE, MAP_SHARED, MAP_FAILED
#include <sys/stat.h> /* For mode constants */
#include <unistd.h> // ftruncate, close
#include <stdio.h> // perror
#include <stdlib.h> // malloc, free
#include <string.h> // strcpy

shared_mutex_t PsxInitSharedMutex(const char *name, mode_t mode) {
  shared_mutex_t mutex = {NULL, 0, NULL, 0};
  errno = 0;

  // Open existing shared memory object, or create one.
  // Two separate calls are needed here, to mark fact of creation
  // for later initialization of pthread mutex.
  mutex.shm_fd = shm_open(name, O_RDWR, mode);
  if (errno == ENOENT) {
	mutex.shm_fd = shm_open(name, O_RDWR|O_CREAT, mode);
	mutex.created = 1;
	// Change permissions of shared memory, so every body can access it. Avoiding the umask of shm_open
	if (fchmod(mutex.shm_fd, mode) != 0) {
	  //perror("fchmod");
	}
  }
  if (mutex.shm_fd == -1) {
	//perror("shm_open");
	return mutex;
  }

  // Truncate shared memory segment so it would contain
  // pthread_mutex_t.
  if (ftruncate(mutex.shm_fd, sizeof(pthread_mutex_t)) != 0) {
	//perror("ftruncate");
	return mutex;
  }

  // Map pthread mutex into the shared memory.
  void *addr = mmap(
	NULL,
	sizeof(pthread_mutex_t),
	PROT_READ|PROT_WRITE,
	MAP_SHARED,
	mutex.shm_fd,
	0
  );
  if (addr == MAP_FAILED) {
	//perror("mmap");
	return mutex;
  }
  pthread_mutex_t *mutex_ptr = (pthread_mutex_t *)addr;

  // If shared memory was just initialized -
  // initialize the mutex as well.
  if (mutex.created) {
	pthread_mutexattr_t attr;
	if (pthread_mutexattr_init(&attr)) {
	  //perror("pthread_mutexattr_init");
	  return mutex;
	}
	if (pthread_mutexattr_setpshared(&attr, PTHREAD_PROCESS_SHARED)) {
	  //perror("pthread_mutexattr_setpshared");
	  return mutex;
	}
	if (pthread_mutex_init(mutex_ptr, &attr)) {
	  //perror("pthread_mutex_init");
	  return mutex;
	}
  }
  mutex.ptr = mutex_ptr;
  mutex.name = (char *)malloc(NAME_MAX+1);
  strcpy(mutex.name, name);
  return mutex;
}

int PsxCloseSharedMutex(shared_mutex_t mutex) {
  if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
	//perror("munmap");
	return -1;
  }
  mutex.ptr = NULL;
  if (close(mutex.shm_fd)) {
	//perror("close");
	return -1;
  }
  mutex.shm_fd = 0;
  free(mutex.name);
  return 0;
}

int PsxDestroySharedMutex(shared_mutex_t mutex) {
  if ((errno = pthread_mutex_destroy(mutex.ptr))) {
	//perror("pthread_mutex_destroy");
	return -1;
  }
  if (munmap((void *)mutex.ptr, sizeof(pthread_mutex_t))) {
	//perror("munmap");
	return -1;
  }
  mutex.ptr = NULL;
  if (close(mutex.shm_fd)) {
	//perror("close");
	return -1;
  }
  mutex.shm_fd = 0;
  if (shm_unlink(mutex.name)) {
	//perror("shm_unlink");
	return -1;
  }
  free(mutex.name);
  return 0;
}

#endif															// Of #ifdef UBF_LINUX.
/****************************************************************************************

	File:		TrashCan.c
	Why:		Trash can function for Apple devices.
	OS:			C99
	Author:		Robert Guetzkow; Thomas (wrapper module only)
	Created:	2024-10-21
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-10-21	Thomas			This wrapper module created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This module is mainly a wrapper for Robert Guetzkow's trashcan_soft_delete () function
	for Apple devices. See
	https://github.com/robertguetzkow/libtrashcan/blob/master/src/trashcan.c .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/* MIT License
	*
	* Copyright (c) 2019 Robert Guetzkow
	* 
	* Permission is hereby granted, free of charge, to any person obtaining a copy
	* of this software and associated documentation files (the "Software"), to deal
	* in the Software without restriction, including without limitation the rights
	* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	* copies of the Software, and to permit persons to whom the Software is
	* furnished to do so, subject to the following conditions:
	*
	* The above copyright notice and this permission notice shall be included in all
	* copies or substantial portions of the Software.
	*
	* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	* SOFTWARE. 
	*/

// See	https://github.com/robertguetzkow/libtrashcan/blob/master/src/trashcan.c .

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./TrashCan.h"

#endif

#ifdef OS_IS_MACOS

static int trashcan_soft_delete_apple (const char *path)
{
	ubf_assert_non_NULL (put);
	int ret = LIBTRASHCAN_ERROR;

	Class NSAutoreleasePoolClass = objc_getClass("NSAutoreleasePool");
	SEL allocSel = sel_registerName("alloc");
	SEL initSel = sel_registerName("init");
	id poolAlloc = ((id(*)(Class, SEL))objc_msgSend)(NSAutoreleasePoolClass, allocSel);
	id pool = ((id(*)(id, SEL))objc_msgSend)(poolAlloc, initSel);

	Class NSStringClass = objc_getClass("NSString");
	SEL stringWithUTF8StringSel = sel_registerName("stringWithUTF8String:");
	id pathString = ((id(*)(Class, SEL, const char*))objc_msgSend)(NSStringClass, stringWithUTF8StringSel, path);

	Class NSFileManagerClass = objc_getClass("NSFileManager");
	SEL defaultManagerSel = sel_registerName("defaultManager");
	id fileManager = ((id(*)(Class, SEL))objc_msgSend)(NSFileManagerClass, defaultManagerSel);

	Class NSURLClass = objc_getClass("NSURL");
	SEL fileURLWithPathSel = sel_registerName("fileURLWithPath:");
	id nsurl = ((id(*)(Class, SEL, id))objc_msgSend)(NSURLClass, fileURLWithPathSel, pathString);

	SEL trashItemAtURLSel = sel_registerName("trashItemAtURL:resultingItemURL:error:");
	BOOL deleteSuccessful = ((BOOL(*)(id, SEL, id, id, id))objc_msgSend)(fileManager, trashItemAtURLSel, nsurl, nil, nil);

	if (deleteSuccessful)
	{
		ret = LIBTRASHCAN_SUCCESS;
	}

	SEL drainSel = sel_registerName("drain");
	((void(*)(id, SEL))objc_msgSend)(pool, drainSel);

	return ret;
}

#endif														// Of #ifdef OS_IS_MACOS.
/****************************************************************************************

	File:		CompressFile.c
	Why:		Module to compress a file on different platforms.
	OS:			POSIX, Windows.
	Author:		Thomas
	Created:	2024-10-17

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-10-17	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./CompressFile.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE

		#include "./ubfdebug.h"
		#include "./platform.h"
		#include "./unref.h"

	#else

		#include "./../dbg/ubfdebug.h"
		#include "./../pre/platform.h"
		#include "./../pre/unref.h"

	#endif

#endif

#ifdef DEBUG
	enfilecompressresult IsFileCompressedByName (const char *szFilename)
	{
		#if defined (OS_IS_WINDOWS)

			#ifdef DEBUG
				// Since the NTFS compressor provides a different enum, we need to make sure that
				//	its values are identical to the ones in enfilecompressresult.
				int a, b;
				a = fscompress_compressed;		b = ntfscompress_compressed;	ubf_assert (a == b);
				a = fscompress_uncompressed;	b = ntfscompress_uncompressed;	ubf_assert (a == b);
				a = fscompress_error;			b = ntfscompress_error;			ubf_assert (a == b);
			#endif

			enfilecompressresult r = (enfilecompressresult) IsFileNTFSCompressedByName (szFilename);
			return r;

		#elif defined (OS_IS_LINUX)

			UNUSED (szFilename);
			return fscompress_error;
			
		#endif

	}
#endif

bool FScompressFileByName (const char *szFilename)
{
	#if defined (OS_IS_WINDOWS)

		return CompressFileNTFS_U8 (szFilename);

	#elif defined (OS_IS_LINUX)
	
		UNUSED (szFilename);
		return false;
		
	#endif
}
/****************************************************************************************

	File:		ExeFileName.c
	Why:		Obtains the name of the executable.
	OS:			C99.
	Author:		Thomas
	Created:	2024-05-21

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-05-21	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <malloc.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./ExeFileName.h"

#endif
/****************************************************************************************

	File:		ProcessHelpers.c
	Why:		Functions for creating and handling processes.
	OS:			C99
	Author:		Thomas
	Created:	2025-06-05
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-06-05	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_BUILD_WITHOUT_PROCESS_HELPERS

#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./ProcessHelpers.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE

		#if defined (PLATFORM_IS_WINDOWS)
			#include "./WinAPI_U8.h"
		#elif defined (PLATFORM_IS_POSIX)

		#elif
			#error Not supported
		#endif

		#include "./ubfdebug.h"
		#include "./membuf.h"
		#include "./unref.h"
		#include "./strnewline.h"
		#include "./strmembuf.h"

		#ifdef PROCESS_HELPERS_BUILD_TEST_FNCT

		#endif

	#else

		#if defined (PLATFORM_IS_WINDOWS)
			#include "./Windows/WinAPI_U8.h"
		#elif defined (PLATFORM_IS_POSIX)

		#elif
			#error Not supported
		#endif

		#include "./../dbg/ubfdebug.h"
		#include "./../mem/membuf.h"
		#include "./../pre/unref.h"
		#include "./../string/strnewline.h"
		#include "./../string/strmembuf.h"

		#ifdef PROCESS_HELPERS_BUILD_TEST_FNCT

		#endif

	#endif

#endif

/*
*/
size_t phlpsStdBufSize = PRCHLPS_DEF_EXCESS_BUFFER;

size_t ProcessHelpersSetBufferSize (size_t bufsize)
{
	ubf_assert_non_0 (bufsize);

	size_t prev = phlpsStdBufSize;
	phlpsStdBufSize = bufsize;
	return prev;
}

/*
	Squeezes the executable's name plus all arguments in a single string.
	The executable's name is going to be the first argument.
*/
char *CreateArgsList (const char *szExecutable, int argc, const char *argv [], bool bNoExeArg)
{
	ubf_assert (NULL != szExecutable);

	int		n;
	size_t	r	= 0;
	size_t	l;

	if (!bNoExeArg)
	{
		r += 2;												// For the double quotes.
		r += strlen (szExecutable);
		++ r;												// NUL.
	}
	for (n = 0; n < argc; ++ n)
	{
		if (n)
			++ r;											// For the space between args.
		r += strlen (argv [n]);
	}
	r += 0 == r ? 1 : n ? 1 : 0;							// NUL.
	char *szArgsList = ubf_malloc (r);
	if (szArgsList)
	{
		if (1 == r)
		{
			szArgsList [0] = ASCII_NUL;
			return szArgsList;
		}

		char *wri = szArgsList;

		if (!bNoExeArg)
		{
			*wri ++ = '\"';
			l = strlen (szExecutable);
			memcpy (wri, szExecutable, l);
			wri += l;
			*wri ++ = '\"';
			*wri = ASCII_NUL;
		}
		for (n = 0; n < argc; ++ n)
		{
			if (n || !bNoExeArg)
				*wri ++ = ' ';
			l = strlen (argv [n]);
			memcpy (wri, argv [n], l);
			wri += l;
			*wri = ASCII_NUL;
		}
		ubf_assert (strlen (szArgsList) == r - 1);
		return szArgsList;
	}
	return NULL;
}

char *CreateArgsListFromString (const char *szExecutable, const char *szCmdLine, bool bNoExeArg)
{
	size_t	r	= 0;
	size_t	l;

	if (!bNoExeArg)
	{
		r += 2;												// For the double quotes.
		r += strlen (szExecutable);
		++ r;												// NUL.
	}
	if (szCmdLine)
	{
		r += strlen (szCmdLine);
		++ r;
	}
	r += 0 == r ? 1 : 0;									// NUL.
	char *szArgsList = ubf_malloc (r);
	if (szArgsList)
	{
		if (1 == r)
		{
			szArgsList [0] = ASCII_NUL;
			return szArgsList;
		}

		char *wri = szArgsList;

		if (!bNoExeArg)
		{
			*wri ++ = '\"';
			l = strlen (szExecutable);
			memcpy (wri, szExecutable, l);
			wri += l;
			*wri ++ = '\"';
			*wri = ASCII_NUL;
		}
		l = szCmdLine ? strlen (szCmdLine) : 0;
		if (szCmdLine && l)
		{
			if (!bNoExeArg)
				*wri ++ = ' ';
			memcpy (wri, szCmdLine, l);
			wri += l;
			*wri = ASCII_NUL;
		}
		ubf_assert (strlen (szArgsList) <= r - 1);
		return szArgsList;
	}
	return NULL;
}

void DoneArgsList (char *szArgsList)
{
	if (szArgsList)
		ubf_free (szArgsList);
}

#ifdef PLATFORM_IS_WINDOWS
	typedef struct sPrcHlpsInOutBuf
	{
		SMEMBUF					smb;
		char					*szWrite;
		size_t					lenSmb;
		bool					bIOcomplete;
		bool					bNeedsWait;
		bool					bEOF;
		OVERLAPPED				ovlpd;
		enRCmdCBval				cbretval;
		HANDLE					hEvent;
		DWORD					dwTransferred;
		HANDLE					hDEvent;					// Dummy event.
	} SPRCHLPSINOUTBUF;

	typedef struct ph_sChildHandles
	{
		HANDLE	hChildInpRD;
		HANDLE	hChildInpWR;

		HANDLE	hChildOutRD;
		HANDLE	hChildOutWR;

		HANDLE	hChildErrRD;
		HANDLE	hChildErrWR;
	} PH_SCHILDHANDLES;

	static bool CloseInheritableHandlesForChild (PH_SCHILDHANDLES *ph)
	{
		ubf_assert_non_NULL (ph);

		bool b = true;
		b &= CloseHandle (ph->hChildInpRD);
		b &= CloseHandle (ph->hChildInpWR);
		b &= CloseHandle (ph->hChildOutRD);
		b &= CloseHandle (ph->hChildOutWR);
		b &= CloseHandle (ph->hChildErrRD);
		b &= CloseHandle (ph->hChildErrWR);
		return b;
	}

	/*
		Derived from:
		https://learn.microsoft.com/en-gb/windows/win32/procthread/creating-a-child-process-with-redirected-input-and-output?redirectedfrom=MSDN
	*/
	static bool CreateInheritableHandlesForChild (PH_SCHILDHANDLES *ph)
	{
		ubf_assert_non_NULL (ph);

		SECURITY_ATTRIBUTES	saAttr;

		saAttr.nLength				= sizeof (saAttr);
		saAttr.bInheritHandle		= true;
		saAttr.lpSecurityDescriptor	= NULL;

		bool b = true;
		b &= CreatePipeOverlapped	(
				&ph->hChildInpRD, false,
				&ph->hChildInpWR, true,
				&saAttr, 0
									);
		ubf_assert_true (b);
		b &= CreatePipeOverlapped	(
				&ph->hChildOutRD, true,
				&ph->hChildOutWR, false,
				&saAttr, 0
									);
		ubf_assert_true (b);
		b &= CreatePipeOverlapped	(
				&ph->hChildErrRD, true,
				&ph->hChildErrWR, false,
				&saAttr, 0
									);
		ubf_assert_true (b);
		// The child won't write to its input handle, and it won't read from its output handles.
		b &= SetHandleInformation (ph->hChildInpWR, HANDLE_FLAG_INHERIT, 0);	ubf_assert_true (b);
		b &= SetHandleInformation (ph->hChildOutRD, HANDLE_FLAG_INHERIT, 0);	ubf_assert_true (b);
		b &= SetHandleInformation (ph->hChildErrRD, HANDLE_FLAG_INHERIT, 0);	ubf_assert_true (b);
		return b;
	}

	/*
	DWORD DataAvailableOnPipe (HANDLE hPipe)
	{
		DWORD dwAvailable = 0;
		bool b = PeekNamedPipe (hPipe, NULL, 0, NULL, &dwAvailable, NULL);
		UNUSED (b);
		ubf_assert_true (b);
		return dwAvailable;
	}
	*/

	static DWORD avlbleSiz (SPRCHLPSINOUTBUF *pb)
	{
		ubf_assert_non_NULL	(pb);
		ubf_assert_non_NULL	(pb->smb.buf.pcc);
		ubf_assert_non_NULL	(pb->szWrite);
		ubf_assert			(pb->lenSmb + 2 < pb->smb.size);

		size_t s = 0;

		if (pb->smb.size)
		{
			s = pb->smb.size - pb->lenSmb - 2;
		}
		return (DWORD) s & 0xFFFFFFFF;
	}

	static enRCmdCBval callOutCB (rcmdOutCB cb, uint16_t flags, char *buf, size_t blen, void *pCustom)
	{
		enRCmdCBval	rv			= enRunCmdRet_Continue;
		char		cDummy []	= "";
		char		*pOut		= blen ? buf : cDummy;

		if (cb)
		{
			if (flags & RUNCMDPROC_CALLB_INTOUT)
			{
				if (flags & RUNCMDPROC_CALLB_STDOUT)
					rv = cb (pOut, blen, pCustom);
			} else
			{	// Implied but not checked: (flags & RUNCMDPROC_CALLB_INTERR)
				if (flags & RUNCMDPROC_CALLB_STDERR)
					rv = cb (pOut, blen, pCustom);
			}
		}
		return rv;
	}

	static enRCmdCBval callInpCB (rcmdInpCB cb, uint16_t flags, SPRCHLPSINOUTBUF *psb, void *pCustom)
	{
		enRCmdCBval	rv = enRunCmdRet_Continue;

		size_t stLen = psb->lenSmb;
		if (cb && flags & RUNCMDPROC_CALLB_STDINP)
			rv = cb (&psb->smb, &stLen, pCustom);
		psb->lenSmb = (DWORD) stLen & 0x00000000FFFFFFFF;
		return rv;
	}

	static void handleHow_AsIs		(
					SPRCHLPSINOUTBUF	*sb,
					rcmdOutCB			cb,
					uint16_t			flags,
					void				*pCustom
									)
	{
		ubf_assert_non_NULL	(sb);
		ubf_assert_0		(sb->lenSmb);					// Should never change.

		if (enRunCmdRet_Continue == sb->cbretval)
		{
			if (sb->dwTransferred)
			{
				sb->szWrite [sb->dwTransferred]		= ASCII_NUL;
				sb->szWrite [sb->dwTransferred + 1]	= ASCII_NUL;
				sb->cbretval = callOutCB (cb, flags, sb->szWrite, sb->dwTransferred, pCustom);
				sb->dwTransferred					= 0;
				sb->szWrite	= sb->smb.buf.pch;
			}
		}
	}

	#ifdef _MSC_VER
	#pragma warning (disable: 4706)							// Assignment within condition.
	#endif

	static void handleHow_OneLine	(
					SPRCHLPSINOUTBUF	*sb,
					rcmdOutCB			cb,
					uint16_t			flags,
					void				*pCustom
									)
	{
		ubf_assert_non_NULL (sb);

		if (enRunCmdRet_Continue == sb->cbretval)
		{
			if (sb->dwTransferred)
			{
				sb->lenSmb += (size_t) sb->dwTransferred;
				size_t	lnNewLn;
				char	*szNewLn;
				while (sb->lenSmb && (szNewLn = strFirstLineEnding_l (sb->smb.buf.pcc, sb->lenSmb, &lnNewLn)))
				{
					size_t lnLine = szNewLn - sb->smb.buf.pch;
					szNewLn [0] = ASCII_NUL;
					sb->cbretval = callOutCB (cb, flags, sb->smb.buf.pch, lnLine, pCustom);
					if (lnLine + lnNewLn == sb->lenSmb)
					{	// Fully consumed.
						sb->lenSmb = 0;
					} else
					{	// Read data exceeds beyond a single line ending.
						sb->lenSmb -= (DWORD) (lnLine + lnNewLn) & 0x00000000FFFFFFFF;
						memmove	(
							sb->smb.buf.pch,
							szNewLn + lnNewLn,
							sb->lenSmb
								);
					}
				}
				if (sb->lenSmb >= sb->smb.size - 2)
				{
					growToSizeRetainSMEMBUF (&sb->smb, sb->smb.size + phlpsStdBufSize);
				}
				sb->szWrite = sb->smb.buf.pch + sb->lenSmb;
				sb->dwTransferred = 0;
			}
		}
	}

	#ifdef _MSC_VER
	#pragma warning (default: 4706)							// Assignment within condition.
	#endif

	static void handleHow_All	(
					SPRCHLPSINOUTBUF	*sb
								)
	{
		ubf_assert_non_NULL (sb);

		//printf ("-> Data: %p, Length: %zu\n", sb->smb.buf.pch, sb->lenSmb);
		if (sb->dwTransferred)
		{
			sb->szWrite	+= sb->dwTransferred;
			sb->lenSmb	+= sb->dwTransferred;
			sb->dwTransferred = 0;
			if (sb->lenSmb >= sb->smb.size - 2)
			{
				growToSizeRetainSMEMBUF (&sb->smb, sb->smb.size + phlpsStdBufSize);
				sb->szWrite = sb->smb.buf.pch + sb->lenSmb;
			}
		}
		//printf ("<- Data: %p, Length: %zu\n", sb->smb.buf.pch, sb->lenSmb);
	}

	static void handleHow_Finalise		(
							SPRCHLPSINOUTBUF	*sb,
							rcmdOutCB			cb,
							uint16_t			flags,
							enRCmdCBhow			cbHow,
							void				*pCustom
											)
	{
		UNUSED (cbHow);

		if (sb->bEOF && enRunCmdHow_All == cbHow && sb->lenSmb)
		{
			ubf_assert (sb->lenSmb <= sb->smb.size - 2);
			sb->smb.buf.pch [sb->lenSmb]		= ASCII_NUL;
			sb->smb.buf.pch [sb->lenSmb + 1]	= ASCII_NUL;
			callOutCB (cb, flags, sb->smb.buf.pch, sb->lenSmb, pCustom);
		}
	}

	static inline DWORD getLastWindowsError (DWORD *pdw)
	{
		ubf_assert_non_NULL (pdw);

		*pdw = GetLastError ();
		return *pdw;
	}

	void readFromHandle (SPRCHLPSINOUTBUF *sb, HANDLE hdl)
	{
		ubf_assert_non_NULL (sb);
		
		DWORD	dwErr;

		if (!sb->bNeedsWait)
		{
			sb->bIOcomplete = ReadFile (hdl, sb->szWrite, avlbleSiz (sb), &sb->dwTransferred, &sb->ovlpd);
			if (!sb->bIOcomplete && ERROR_IO_PENDING == getLastWindowsError (&dwErr))
				sb->bNeedsWait = true;
			ubf_assert (ERROR_INVALID_USER_BUFFER != getLastWindowsError (&dwErr));
		}
	}

	void writeToHandle (SPRCHLPSINOUTBUF *sb, HANDLE hdl)
	{
		ubf_assert_non_NULL (sb);
		
		DWORD	dwErr;

		if (!sb->bNeedsWait)
		{
			DWORD dwToTransfer = (DWORD) sb->lenSmb;
			sb->bIOcomplete = WriteFile (hdl, sb->smb.buf.pcc, dwToTransfer, &sb->dwTransferred, &sb->ovlpd);
			sb->lenSmb -= sb->dwTransferred;
			if (!sb->bIOcomplete && ERROR_IO_PENDING == getLastWindowsError (&dwErr))
				sb->bNeedsWait = true;
			ubf_assert (ERROR_INVALID_USER_BUFFER != getLastWindowsError (&dwErr));
		}
	}

	void checkOverlapped (SPRCHLPSINOUTBUF *sb, HANDLE hdl)
	{
		ubf_assert_non_NULL (sb);
		
		DWORD	dwErr;

		if (sb->bNeedsWait)
		{
			sb->bIOcomplete = GetOverlappedResult (hdl, &sb->ovlpd, &sb->dwTransferred, false);
			if (sb->bIOcomplete)
				sb->bNeedsWait = false;
			sb->bEOF = !sb->bIOcomplete && ERROR_HANDLE_EOF == getLastWindowsError (&dwErr);
		}
	}

	static void handleReceiveBuffer		(
					SPRCHLPSINOUTBUF		*sb,
					rcmdOutCB				cb,
					uint16_t				flags,
					enRCmdCBhow				cbHow,
					void					*pCustom
										)
	{
		ubf_assert_non_NULL (sb);

		if (sb->bIOcomplete && sb->dwTransferred)
		{
			switch (cbHow)
			{
				case enRunCmdHow_AsIs:
				case enRunCmdHow_AsIs0:
					if (sb->bIOcomplete)
						handleHow_AsIs (sb, cb, flags, pCustom);
					break;
				case enRunCmdHow_OneLine:
					if (sb->bIOcomplete)
						handleHow_OneLine (sb, cb, flags, pCustom);
					break;
				case enRunCmdHow_All:
					handleHow_All (sb);
					break;
			}
		}
	}

	#define OUTFLGS		(uiCBflags | RUNCMDPROC_CALLB_INTOUT)
	#define ERRFLGS		(uiCBflags | RUNCMDPROC_CALLB_INTERR)

	bool HandleCommPipes	(
			HANDLE				hProcess,
			PH_SCHILDHANDLES	*pph,
			SRCMDCBS			*pCBs,
			enRCmdCBhow			cbHow,
			uint16_t			uiCBflags,
			void				*pCustom
							)
	{
		bool				bRet	= false;

		// Standard input (stdin) of child process.
		SPRCHLPSINOUTBUF	sbInp;
		memset (&sbInp, 0, sizeof (SPRCHLPSINOUTBUF));
		INITSMEMBUF (sbInp.smb);
		sbInp.cbretval		= enRunCmdRet_Continue;
		sbInp.hEvent		= CreateEvent (NULL, true, false, NULL);
		sbInp.ovlpd.hEvent	= sbInp.hEvent;
		sbInp.dwTransferred	= 0;
		sbInp.hDEvent		= CreateEvent (NULL, true, false, NULL);

		// Standard output (stdout) of child process.
		SPRCHLPSINOUTBUF	sbOut;
		memset (&sbOut, 0, sizeof (SPRCHLPSINOUTBUF));
		INITSMEMBUF (sbOut.smb);
		sbOut.cbretval		= enRunCmdRet_Continue;
		sbOut.hEvent		= CreateEvent (NULL, true, false, NULL);
		sbOut.ovlpd.hEvent	= sbOut.hEvent;
		sbOut.dwTransferred	= 0;
		sbOut.hDEvent		= CreateEvent (NULL, true, false, NULL);

		// Standard error (stderr) of child process.
		SPRCHLPSINOUTBUF	sbErr;
		memset (&sbErr, 0, sizeof (SPRCHLPSINOUTBUF));
		INITSMEMBUF (sbErr.smb);
		sbErr.cbretval		= enRunCmdRet_Continue;
		sbErr.hEvent		= CreateEvent (NULL, true, false, NULL);
		sbErr.ovlpd.hEvent	= sbErr.hEvent;
		sbErr.dwTransferred	= 0;
		sbErr.hDEvent		= CreateEvent (NULL, true, false, NULL);

		if	(
					NULL == sbInp.hEvent
				||	NULL == sbOut.hEvent
				||	NULL == sbErr.hEvent
				||	NULL == sbInp.hDEvent
				||	NULL == sbOut.hDEvent
				||	NULL == sbErr.hDEvent
			)
			goto Escape;

		//growToSizeSMEMBUF (&sbInp.smb, phlpsStdBufSize);
		growToSizeSMEMBUF (&sbOut.smb, phlpsStdBufSize);
		growToSizeSMEMBUF (&sbErr.smb, phlpsStdBufSize);
		sbOut.szWrite = sbOut.smb.buf.pch;
		sbErr.szWrite = sbErr.smb.buf.pch;

		//DWORD dwErr;

		do
		{
			bool bNoWaitRequired;
			do
			{
				// We call the write input handler callback function with every iteration,
				//	provided that the send buffer is empty.
				if (enRunCmdRet_Continue == sbInp.cbretval && 0 == sbInp.lenSmb)
					sbInp.cbretval = callInpCB (pCBs->cbInp, uiCBflags, &sbInp, pCustom);
				if (isUsableSMEMBUF (&sbInp.smb))
				{
					writeToHandle		(&sbInp, pph->hChildInpWR);
					checkOverlapped		(&sbInp, pph->hChildInpWR);
				}
				if (isUsableSMEMBUF (&sbOut.smb))
				{
					readFromHandle		(&sbOut, pph->hChildOutRD);
					checkOverlapped		(&sbOut, pph->hChildOutRD);
					handleReceiveBuffer	(&sbOut, pCBs->cbOut, OUTFLGS, cbHow, pCustom);
				} else
					goto Escape;
				if (isUsableSMEMBUF (&sbErr.smb))
				{
					readFromHandle		(&sbErr, pph->hChildErrRD);
					checkOverlapped		(&sbErr, pph->hChildErrRD);
					handleReceiveBuffer	(&sbErr, pCBs->cbErr, ERRFLGS, cbHow, pCustom);
				} else
					goto Escape;
				bNoWaitRequired = !(sbOut.bNeedsWait && sbErr.bNeedsWait);
			} while (bNoWaitRequired);

			#define PHNHDLS (4)
			HANDLE h [PHNHDLS];
			if (sbOut.bNeedsWait) h [0] = sbOut.hEvent; else h [0] = sbOut.hDEvent;
			if (sbErr.bNeedsWait) h [1] = sbErr.hEvent; else h [1] = sbErr.hDEvent;
			if (sbInp.bNeedsWait) h [2] = sbInp.hEvent; else h [2] = sbInp.hDEvent;
			h [3] = hProcess;
			DWORD dw = WaitForMultipleObjects (PHNHDLS, h, false, INFINITE);
			//printf ("Wait complete with %d.\n", dw);
			if (WAIT_OBJECT_0 <= dw && dw < WAIT_OBJECT_0 + PHNHDLS)
			{
				switch (dw - WAIT_OBJECT_0)
				{
					// The process has exited/terminated.
					case 3:		sbOut.bEOF = true;
								sbErr.bEOF = true;
								sbInp.bEOF = true;
								break;

					// We don't check which operation completed the wait. We just check all
					//	of them.
					default:	checkOverlapped		(&sbOut, pph->hChildOutRD);
								handleReceiveBuffer	(&sbOut, pCBs->cbOut, OUTFLGS, cbHow, pCustom);

								checkOverlapped		(&sbErr, pph->hChildErrRD);
								handleReceiveBuffer	(&sbErr, pCBs->cbErr, ERRFLGS, cbHow, pCustom);

								checkOverlapped		(&sbInp, pph->hChildInpWR);
								writeToHandle		(&sbInp, pph->hChildInpWR);
								break;
				}
			}

			if	(
						enRunCmdRet_Terminate		== sbOut.cbretval
					||	enRunCmdRet_TerminateFail	== sbOut.cbretval
					||	enRunCmdRet_Terminate		== sbErr.cbretval
					||	enRunCmdRet_TerminateFail	== sbErr.cbretval
					||	enRunCmdRet_Terminate		== sbInp.cbretval
					||	enRunCmdRet_TerminateFail	== sbInp.cbretval
				)
			{
				bool bc = TerminateChildProcess (hProcess);
				ubf_assert_true (bc);
				UNUSED (bc);

				if	(
							enRunCmdRet_TerminateFail	== sbOut.cbretval
						||	enRunCmdRet_TerminateFail	== sbErr.cbretval
						||	enRunCmdRet_TerminateFail	== sbInp.cbretval
					)
					goto Escape;
			}
		} while (!sbOut.bEOF && !sbErr.bEOF && !sbInp.bEOF);

		bRet = true;

		handleHow_Finalise (&sbOut, pCBs->cbOut, OUTFLGS, cbHow, pCustom);
		handleHow_Finalise (&sbErr, pCBs->cbErr, ERRFLGS, cbHow, pCustom);

		Escape:
		DONESMEMBUF (sbInp.smb);
		DONESMEMBUF (sbOut.smb);
		DONESMEMBUF (sbErr.smb);

		// We better cancel outstanding I/O operations. See
		//	https://learn.microsoft.com/en-us/windows/win32/FileIO/cancelio .
		CancelIo (pph->hChildOutRD);
		CancelIo (pph->hChildErrRD);
		CancelIo (pph->hChildInpWR);

		if (sbInp.hEvent)	CloseHandle (sbInp.hEvent);
		if (sbInp.hDEvent)	CloseHandle (sbInp.hDEvent);
		if (sbOut.hEvent)	CloseHandle (sbOut.hEvent);
		if (sbOut.hDEvent)	CloseHandle (sbOut.hDEvent);
		if (sbErr.hEvent)	CloseHandle (sbErr.hEvent);
		if (sbErr.hDEvent)	CloseHandle (sbErr.hDEvent);

		return bRet;
	}

	bool HandleCommunication	(
			HANDLE					hProcess,
			PH_SCHILDHANDLES		*pph,
			SRCMDCBS				*pCBs,
			enRCmdCBhow				cbHow,
			uint16_t				uiCBflags,
			void					*pCustom
								)
	{
		ubf_assert_non_NULL (hProcess);
		ubf_assert_non_NULL (pph);

		SRCMDCBS cbs;
		if (NULL == pCBs)
		{
			ZeroMemory (&cbs, sizeof (SRCMDCBS));
			pCBs = &cbs;
		}

		bool b = HandleCommPipes (hProcess, pph, pCBs, cbHow, uiCBflags, pCustom);

		return b;
	}
#endif

#if defined (PLATFORM_IS_WINDOWS)

	bool CreateAndRunCmdProcessCaptureStdout	(
			const char				*szExecutable,
			const char				*szCmdLine,
			const char				*szWorkingDir,
			SRCMDCBS				*pCBs,
			enRCmdCBhow				cbHow,					// How to call the callback functions.
			uint16_t				uiRCflags,				// One or more of the RUNCMDPROC_
															//	flags.
			void					*pCustom				// Passed on unchanged to callback
															//	functions.
												)
	{
		if (NULL == szExecutable)
			return false;

		PROCESS_INFORMATION	pi;
		ZeroMemory (&pi, sizeof(pi));

		PH_SCHILDHANDLES	ph;
		bool				b;

		b = CreateInheritableHandlesForChild (&ph);		ubf_assert_true (b);
		if (b)
		{
			STARTUPINFOA		si;
			ZeroMemory (&si, sizeof(si));
			si.cb			= sizeof(si);
			si.hStdInput	= ph.hChildInpRD;
			si.hStdOutput	= ph.hChildOutWR;
			si.hStdError	= ph.hChildErrWR;
			si.dwFlags		= STARTF_USESTDHANDLES;

			//char *szArgsList = CreateArgsList (szExecutable, argc, argv, uiCBflags & RUNCMDPROC_EXEARG_NOEXE);
			char *szArgsList = CreateArgsListFromString	(
									szExecutable, szCmdLine, uiRCflags & RUNCMDPROC_EXEARG_NOEXE
														);
			if (szArgsList)
			{
				DWORD dwCreationFlags	= CREATE_NEW_PROCESS_GROUP;

				b &= CreateProcessU8	(
						szExecutable, szArgsList,			// Command line arguments.
						NULL, NULL, true,					// Inherits our handles.
						dwCreationFlags,					// Creation flags.
						NULL,								// Use our environment block.
						szWorkingDir,						// Child's startup folder.
						&si,								// Pointer to STARTUPINFOA.
						&pi									// Pointer to PROCESS_INFORMATION.
										);

				DoneArgsList (szArgsList);

				// This function returns false if a callback funciton returned enRunCmdRet_TerminateFail.
				b &= HandleCommunication (pi.hProcess, &ph, pCBs, cbHow, uiRCflags, pCustom);
			}
		}

		// Close process and thread handles.				// May have terminated already.
		b &= CloseHandle (pi.hProcess);						// ubf_assert_true (b);
		b &= CloseHandle (pi.hThread);						// ubf_assert_true (b);
		b &= CloseInheritableHandlesForChild (&ph);
		return b;
	}

#elif defined (PLATFORM_IS_POSIX)

	// Taken from somewhere. Won't build and totally untested.
	bool CreateAndRunCmdProcessCaptureStdout	(
			const char				*szExecutable,
			int						argc,
			const char				*argv [],
			const char				*szWorkingDir,
			SRCMDCBS				*pCBs,
			enRCmdCBhow				cbHow,					// How to call the callback functions.
			uint16_t				uiCBflags,				// One or more of the RUNCMDPROC_CBFLAG_
															//	flags.
			void					*pCusom					// Passed on unchanged to callback
															//	functions.
												)
	{
		#include <stdio.h>
		#include <stdlib.h>
		#include <unistd.h>

		int main() {
			int pipefd[2];
			if (pipe(pipefd) == -1) {
				perror("pipe");
				exit(EXIT_FAILURE);
			}

			pid_t pid = fork();
			if (pid == -1) {
				perror("fork");
				exit(EXIT_FAILURE);
			}

			if (pid == 0) {  // Child process
				close(pipefd[0]);  // Close unused read end
				dup2(pipefd[1], STDOUT_FILENO);  // Redirect stdout to pipe
				close(pipefd[1]);

				execl("/path/to/executable", "executable", (char *) NULL);
				perror("execl");  // Exec failed
				exit(EXIT_FAILURE);
			} else {  // Parent process
				close(pipefd[1]);  // Close unused write end
				char buffer[128];
				ssize_t count;
				while ((count = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
					buffer[count] = '\0';  // Null terminate
					printf("Output: %s", buffer);
				}
				close(pipefd[0]);
			}

			return 0;
		}
	}

#elif

	// Neither Windows nor POSIX.
	#error Not supported

#endif

#ifdef PROCESS_HELPERS_BUILD_TEST_FNCT
	static unsigned int uiLn = 0;

	enRCmdCBval cbInp (SMEMBUF *psmb, size_t *plnData, void *pCustom)
	{
		UNUSED (pCustom);

		*plnData = SMEMBUFfromStr (psmb, "dir\ndir\nexit\n", USE_STRLEN);
		return enRunCmdRet_Continue;
	}
	enRCmdCBval cbOutWhoAmI (const char *szData, size_t lnData, void *pCustom)
	{
		ubf_assert_non_NULL	(szData);
		ubf_assert			((void *) 1 == pCustom);
		UNUSED (pCustom);

		if (szData && lnData)
			return enRunCmdRet_Continue;
		return enRunCmdRet_Ignore;
	}
	enRCmdCBval cbErrWhoAmI (const char *szData, size_t lnData, void *pCustom)
	{
		UNUSED (szData);
		UNUSED (lnData);
		UNUSED (pCustom);

		ubf_assert_non_NULL	(szData);
		//ubf_assert			((void *) 1 == pCustom);

		if (szData && lnData)
			return enRunCmdRet_Continue;
		return enRunCmdRet_Ignore;
	}

	enRCmdCBval cbOut (const char *szData, size_t lnData, void *pCustom)
	{
		UNUSED (szData);
		UNUSED (lnData);
		UNUSED (pCustom);

		printf ("Data: %p, Length: %zu\n", szData, lnData);
		puts (szData);
		printf ("Data: %p, Length: %zu\n", szData, lnData);
		return enRunCmdRet_Continue;
	}
	enRCmdCBval cbOutOneLine (const char *szData, size_t lnData, void *pCustom)
	{
		UNUSED (szData);
		UNUSED (lnData);
		UNUSED (pCustom);

		ubf_assert (strlen (szData) == lnData);

		if (0 == uiLn)
			puts ("");
		
		++ uiLn;
		if (lnData)
			printf ("%5.5d %s\n", uiLn, szData);
		else
			printf ("%5.5d\n", uiLn);
		//return enRunCmdRet_TerminateFail;
		return enRunCmdRet_Continue;
	}
	enRCmdCBval cbErr (const char *szData, size_t lnData, void *pCustom)
	{
		UNUSED (szData);
		UNUSED (lnData);
		UNUSED (pCustom);

		ASSERT (false);
		return enRunCmdRet_Continue;
	}

	bool ProcessHelpersTestFnct (void)
	{
		bool				b		= true;
		char				*sz;

		sz = CreateArgsList ("exe", 0, NULL, true);
		ubf_expect_bool_AND (b, !memcmp (sz, "", 1));
		ubf_free (sz);
		sz = CreateArgsList ("exe", 0, NULL, false);
		ubf_expect_bool_AND (b, !memcmp (sz, "\"exe\"", 6));
		ubf_free (sz);
		sz = CreateArgsList ("exe exe", 0, NULL, false);
		ubf_expect_bool_AND (b, !memcmp (sz, "\"exe exe\"", 10));
		ubf_free (sz);
		char *args [10];
		args [0] = "arg1";
		args [1] = "arg2";
		args [2] = "arg3";
		sz = CreateArgsList ("exe", 1, args, false);
		ubf_expect_bool_AND (b, !memcmp (sz, "\"exe\" arg1", 11));
		ubf_free (sz);
		sz = CreateArgsList ("exe", 1, args, true);
		ubf_expect_bool_AND (b, !memcmp (sz, "arg1", 5));
		ubf_free (sz);
		sz = CreateArgsList ("exe", 2, args, true);
		ubf_expect_bool_AND (b, !memcmp (sz, "arg1 arg2", 10));
		ubf_free (sz);
		sz = CreateArgsList ("exe", 3, args, true);
		ubf_expect_bool_AND (b, !memcmp (sz, "arg1 arg2 arg3", 15));
		ubf_free (sz);
		sz = CreateArgsList ("exe", 2, args, false);
		ubf_expect_bool_AND (b, !memcmp (sz, "\"exe\" arg1 arg2", 16));
		ubf_free (sz);
		sz = CreateArgsList ("exe", 3, args, false);
		ubf_expect_bool_AND (b, !memcmp (sz, "\"exe\" arg1 arg2 arg3", 21));
		ubf_free (sz);

		SRCMDCBS cbs;
		cbs.cbInp = cbInp;
		cbs.cbOut = cbOutOneLine;
		cbs.cbErr = cbErr;

		uint16_t cbflgs	=		RUNCMDPROC_CALLB_STDINP
							|	RUNCMDPROC_CALLB_STDOUT
							|	RUNCMDPROC_CALLB_STDERR;

		const char *argv [10];
		argv [0] = "/C DIR";

		#if defined (PLATFORM_IS_WINDOWS)

			char szSystemFolder [512];
			UINT uiw = GetSystemDirectoryU8 (szSystemFolder, 512);
			if (uiw > 512 - 1)
			{	// Buffer too small.
				ubf_assert_msg (false, "Increase static buffer of currently 512 octets! It's only for tests.");
				return false;
			}

			//char *szURL = "https://raw.githubusercontent.com/cunilog/cunilog/refs/heads/master/src/c/combined/cunilog_combined.c";
			char szArgs [1024];
			//snprintf (szArgs, 1024, "%s --silent --show-error", szURL);
			snprintf (szArgs, 1024, "/C dir");
			cbs.cbOut = cbOut;

			char szCmd [2056];

			//cunilog_printf ("Retrieving log file from \"%s\"...\n", szURL);
			/*
			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\cmd.exe",
					szArgs, NULL,
					&cbs, enRunCmdHow_All, cbflgs, NULL
														);
			*/

			snprintf (szArgs, 1024, "/C type ..\\..\\..\\..\\src\\def\\libcunilog.def");
			snprintf (szArgs, 1024, "/C type ..\\..\\..\\..\\src\\c\\combined\\cunilog_combined.c");
			//snprintf (szArgs, 1024, "/C type testcunilog.vcxproj");
			
			snprintf (szCmd, 2048, "%s%s", szSystemFolder, "cmd.exe");

			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\cmd.exe",
					szArgs, NULL,
					&cbs, enRunCmdHow_All, cbflgs, NULL
														);

			cbs.cbOut = cbOutOneLine;
			snprintf (szArgs, 1024, "/C type ..\\..\\..\\..\\src\\def\\libcunilog.def");
			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\cmd.exe",
					szArgs, NULL,
					&cbs, enRunCmdHow_OneLine, cbflgs, NULL
														);

			uiLn = 0;
			snprintf (szArgs, 1024, "/C type ..\\..\\..\\..\\src\\c\\combined\\cunilog_combined.c");
			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\cmd.exe",
					szArgs, NULL,
					&cbs, enRunCmdHow_OneLine, cbflgs, NULL
														);


			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\cmd.exe",
					"/C dir", NULL,
					&cbs, enRunCmdHow_All, cbflgs, NULL
														);

			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\cmd.exe",
					"/C dir", NULL,
					&cbs, enRunCmdHow_OneLine, cbflgs, NULL
														);

			cbs.cbOut = cbOutWhoAmI;
			cbs.cbErr = cbErrWhoAmI;
			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\whoami.exe",
					"", NULL,
					&cbs, enRunCmdHow_OneLine, cbflgs, (void *) 1
														);
			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\whoami.exe",
					"some argument that fails whoami", NULL,
					&cbs, enRunCmdHow_OneLine, cbflgs, (void *) 1
														);

			cbs.cbOut = cbOut;

			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\cmd.exe",
					"/C dir", NULL,
					&cbs, enRunCmdHow_OneLine, cbflgs, NULL
														);

			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\whoami.exe",
					"", NULL,
					&cbs, enRunCmdHow_AsIs, cbflgs, NULL
														);

			cbs.cbOut = cbOut;

			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\whoami.exe",
					"", NULL,
					&cbs, enRunCmdHow_OneLine, cbflgs, NULL
														);

			b &= CreateAndRunCmdProcessCaptureStdout	(
					"C:\\Windows\\System32\\cmd.exe",
					"/C dir", NULL,
					&cbs, enRunCmdHow_All, cbflgs, NULL
														);

		#elif defined (PLATFORM_IS_POSIX)

			b &= CreateAndRunProcessCaptureStdout	(
					"whoami",
					1, argv, NULL,
					&cbs, enRunCmdHow_AsIs, cbflgs, NULL
													);

		#elif
			b = false;
		#endif
		//ubf_expect_true (b);

		return b;
	}
#endif

#endif														// Of #ifndef CUNILOG_BUILD_WITHOUT_PROCESS_HELPERS.
/****************************************************************************************

	File:		SharedMutex.c
	Why:		Implements a mutex for interprocess-locking.
	OS:			C99
	Author:		Thomas
	Created:	2021-02-23
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2021-02-23	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
MIT License

Copyright (c) 2018 Oleg Yamnikov

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./SharedMutex.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
	#else
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

shared_mutex_t InitSharedMutex (const char *name)
{
	ubf_assert (LENOFSTR (UBF_SHARED_MUTEX_GLOBAL_PFX) > 0);

	char	cName [NAME_MAX + LENOFSTR (UBF_SHARED_MUTEX_GLOBAL_PFX)];
	char	namebuf [NAME_MAX];
	
	ubf_assert (NULL != name);
	ubf_assert (0 != name [0]);
	ubf_assert ('\\' != name [0]);
	ubf_assert ('/' != name [0]);

	memcpy (namebuf, name, strlen (name) < NAME_MAX ? strlen (name) + 1 : NAME_MAX - 1);
	namebuf [NAME_MAX - 1] = '\0';

	ubf_assert (NAME_MAX > strlen (name));
	memcpy (cName, UBF_SHARED_MUTEX_GLOBAL_PFX, LENOFSTR (UBF_SHARED_MUTEX_GLOBAL_PFX));
	memcpy (cName + LENOFSTR (UBF_SHARED_MUTEX_GLOBAL_PFX), namebuf, strlen (namebuf) + 1);

	#if defined (PLATFORM_IS_WINDOWS)
		return WinInitSharedMutex (cName);
	#elif defined (PLATFORM_IS_POSIX)
		return PsxInitSharedMutex (cName, S_IRUSR | S_IWUSR);
	#elif
		#error Not supported
	#endif
}

int CloseSharedMutex (shared_mutex_t mutex)
{
	#ifdef PLATFORM_IS_POSIX
		return PsxCloseSharedMutex (mutex);
	#endif
	#ifdef PLATFORM_IS_WINDOWS
		return WinCloseSharedMutex (mutex);
	#endif
}

void DestroySharedMutex (shared_mutex_t mutex)
{
	#ifdef PLATFORM_IS_POSIX
		PsxDestroySharedMutex (mutex);
	#endif
	#ifdef PLATFORM_IS_WINDOWS
		WinDestroySharedMutex (mutex);
	#endif
}

bool EnterSharedMutex (shared_mutex_t mutex)
{
	#ifdef PLATFORM_IS_POSIX
		return 0 == pthread_mutex_lock (mutex.ptr);
	#endif
	#ifdef PLATFORM_IS_WINDOWS
		DWORD dw;
		dw = WaitForSingleObject (mutex->h, INFINITE);
		return WAIT_OBJECT_0 == dw;
	#endif
}

bool LeaveSharedMutex (shared_mutex_t mutex)
{
	#ifdef PLATFORM_IS_POSIX
		return 0 == pthread_mutex_unlock (mutex.ptr);
	#endif
	#ifdef PLATFORM_IS_WINDOWS
		return ReleaseMutex (mutex->h);
	#endif
}

bool HaveWeCreatedSharedMutex (shared_mutex_t mutex)
{
	#if defined (PLATFORM_IS_WINDOWS)
		return mutex->bCreatedHere;
	#elif defined (PLATFORM_IS_POSIX)
		return 0 != mutex.created;
	#elif
		#error Not supported
	#endif
}
/****************************************************************************************

	File:		UserHome.c
	Why:		User home directory functions.
	OS:			C99.
	Author:		Thomas
	Created:	2025-03-03
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-03-03	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdbool.h>
#include <stdint.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./UserHome.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE

		#include "./externC.h"
		#include "./platform.h"
		#include "./ubfdebug.h"

		#if defined (PLATFORM_IS_WINDOWS)
			#include "./WinAPI_U8.h"
		#elif defined (PLATFORM_IS_POSIX)
			#include "./PsxHome.h"
		#endif
		
	#else

		#include "./../pre/externC.h"
		#include "./../pre/platform.h"
		#include "./../dbg/ubfdebug.h"

		#if defined (PLATFORM_IS_WINDOWS)
			#include "./../OS/Windows/WinAPI_U8.h"
		#elif defined (PLATFORM_IS_POSIX)
			#include "./../OS/POSIX/PsxHome.h"
		#endif

	#endif

#endif


size_t ObtainUserHomeDirectoy (SMEMBUF *mb)
{
	ubf_assert_non_NULL (mb);
	ubf_assert (isInitialisedSMEMBUF (mb));

	size_t	r = 0;

	#if defined (PLATFORM_IS_WINDOWS)

		HANDLE	hToken	= 0;
		DWORD	dw		= 0;
		
		if (OpenProcessToken (GetCurrentProcess (), TOKEN_QUERY, &hToken))
		{
			GetUserProfileDirectoryU8 (hToken, NULL, &dw);
			growToSizeSMEMBUF (mb, (size_t) dw + 1);
			if (isUsableSMEMBUF (mb))
			{
				bool b = GetUserProfileDirectoryU8 (hToken, mb->buf.pch, &dw);
				if (b && dw > 1)
				{
					// This check is normally not required since we actually already
					//	know that the profile folder comes without a directory separator
					//	but we're going to play safe here.
					if ('\\' != mb->buf.pch [dw - 2])
					{
						++ dw;
						mb->buf.pch [dw - 2] = '\\';
						mb->buf.pch [dw - 1] = '\0';
					}
					ubf_assert ('\0' == mb->buf.pch [dw - 1]);
					r = (size_t) dw - 1;
				}
			}
			CloseHandle (hToken);
		}

	#elif defined (PLATFORM_IS_POSIX)

		size_t lnH;
		const char *szH = UserHomeDirectoryPOSIX (&lnH);
		// Same here:
		// This check is normally not required since we actually already
		//	know that the profile folder comes without a directory separator
		//	but we're going to play safe here.
		if (lnH && '/' != szH [lnH])
		{
			r = SMEMBUFfromStrReserveBytes (mb, szH, lnH + 1, 1) - 1;
			if (r)
			{
				mb->buf.pch [lnH]		= '/';
				mb->buf.pch [lnH + 1]	= '\0';
				++ r;
			}
		} else
			r = SMEMBUFfromStr (mb, szH, lnH + 1) - 1;

	#else

		#error Not supported!

	#endif

	return r;
}
/****************************************************************************************

	File:		ExtCompressors.c
	Why:		Interface module/unit for external compressors.
	OS:			POSIX, Windows.
	Author:		Thomas
	Created:	2024-11-01

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-11-01	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE
	#include "./ExtCompressors.h"
#endif

/****************************************************************************************

	File:		ubfmem.c
	Why:		Functions for memory handling.
	OS:			C99
	Author:		Thomas
	Created:	2016-10-31
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2016-10-31	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifdef _MSC_VER
	#include <crtdbg.h>
#else
	// We need this if the Microsoft compiler is not used.
	#include <assert.h>
#endif

#include <malloc.h>
#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./ubfmem.h"

	#ifdef UBF_MEM_DEBUG_USE_OUR_DEBUG_FUNCS
		#include "./../dbg/ubfdebug.h"
		#include "./../mem/ubfmemdebug.h"
	#endif

#endif

#ifdef _WIN32
	#ifdef USE_WINDOWS_NATIVE_HEAP_FUNCTIONS
		#include <windows.h>

		/*
		void *memset (void *ptr, int value, size_t num)
		{
			char *p = ptr;

			while (num-- > 0)
				*p ++ = value;
			return ptr;
		}
		*/

		/*
			memcpy
			memcmp

			Because we are not using the run-time library to obtain a small executable size we
			got to provide our own standard functions.
		*/
		/*
		void *memcpy (void *dest, const void *src, size_t len)
		{
			char *d = dest;
			const char *s = src;

			while (len--)
			{
				*d++ = *s++;
			}
			return dest;
		}
		*/

		int memcmp (const void *str1, const void *str2, size_t count)
		{
			const unsigned char *s1 = (const unsigned char*) str1;
			const unsigned char *s2 = (const unsigned char*) str2;

			while (count-- > 0)
			{
				if (*s1 ++ != *s2 ++)
					return s1 [-1] < s2 [-1] ? -1 : 1;
			}
			return 0;
		}

	#endif
#endif

#ifdef UBF_MEM_DEBUG_USE_OUR_DEBUG_FUNCS
	void *ubf_malloc	(size_t size)
	{	// See https://www.tutorialspoint.com/c_standard_library/c_function_malloc.htm .
		void	*pnew;
	
		pnew = ubf_memdbg_malloc (size);
		return pnew;
	}

	void *ubf_calloc	(size_t nitems, size_t size)
	{	// See https://www.tutorialspoint.com/c_standard_library/c_function_calloc.htm .
		void	*pnew;
	
		pnew = ubf_memdbg_calloc (nitems, size);
		return pnew;
	}

	void *ubf_realloc	(void *ptr, size_t new_size)
	{	// See https://www.tutorialspoint.com/c_standard_library/c_function_realloc.htm .
		void	*pnew;

		pnew = ubf_memdbg_realloc (ptr, new_size);
		return pnew;
	}

	char *ubf_strdup	(const char *cc)
	{
		char		*pchnew;

		pchnew = ubf_memdbg_strdup (cc);			
		return pchnew;
	}
	
	void ubf_free		(void *ptr)
	{	// See https://www.tutorialspoint.com/c_standard_library/c_function_free.htm .
		/*
			The debug version does not allow a NULL pointer argument. Use

			#ifdef DEBUG
				if (p)
			#endif
					ubf_free (p);

			or
				ubf_free_accept_NULL (p)

			if you expect a NULL pointer to be passed to ubf_free ().
		*/
		ubf_assert (UBF_NULL != ptr);
		ubf_assert (NULL != ptr);
		ubf_memdbg_free (ptr);
	}
	
	void ubf_free_accept_NULL	(void *ptr)
	{
		#ifdef DEBUG
			if (ptr)
		#endif
				ubf_memdbg_free (ptr);
	}

	void *ubf_memcpy	(void *bse, void *dst, const void *src, size_t len)
	{
		return ubf_memdbg_memcpy (bse, dst, src, len);
	}

	void *ubf_memmove	(void *bse, void *dst, const void *src, size_t len)
	{
		return ubf_memdbg_memmove (bse, dst, src, len);
	}
	
	void *ubf_memset (void *bse, void *ptr, int value, size_t num)
	{
		return ubf_memdbg_memset (bse, ptr, value, num);
	}
	
	int ubf_memcmp (const void *bse, const void *ptr1, const void *ptr2, size_t num)
	{
		return ubf_memdbg_memcmp (bse, ptr1, ptr2, num);
	}
	
	void ubf_mem_init_ptr (void *btm, size_t usize)
	{
		ubf_memdbg_fill_alloc_guards_botm (btm, usize);
	}
	
	void *ubf_mem_usr_ptr (void *btm)
	{
		ubf_memdbg_check_alloc_guards_botm (btm);
		return ubf_memdbg_get_base_from_botm (btm);
	}
	
	void ubf_mem_chk_ptr (void *btm)
	{
		ubf_memdbg_check_alloc_guards_botm (btm);
	}
	
	void ubf_mem_chk_bse (void *bse)
	{
		ubf_memdbg_check_alloc_guards_base (bse);
	}
	
	void ubf_mem_test_memdbg (void)
	{
		ubf_memdbg_test_module ();
	}
	
	void ubf_mem_done (void)
	{
	
	}
#endif

#ifdef OS_IS_WINDOWS
	#ifdef DEBUG
	void *ubf_aligned_malloc_intern (size_t alignment, size_t size)
	{	// It's easy to confuse the parameters. The first one's the alignment, the second one
		//	is the size of the memory block to allocate. We assume here that size is always
		//	greater or equal to alignment. Maybe this catches some bugs.
		ASSERT (size >= alignment);								// Parameters confused?
		return _aligned_malloc (alignment, size);
	}
	#else
		// Covered in the header.
		//return _aligned_malloc (alignment, size);
	#endif
#else
	// POSIX.
	#include <stdlib.h>
	
	void *ubf_aligned_malloc_intern (size_t alignment, size_t size)
	{
		void	*pRet = NULL;
		
		// It's easy to confuse the parameters. The first one's the alignment, the second one
		//	is the size of the memory block to allocate. We assume here that size is always
		//	greater or equal to alignment. Maybe this catches some bugs.
		ASSERT (size >= alignment);								// Parameters confused?
		// See http://pubs.opengroup.org/onlinepubs/9699919799/functions/posix_memalign.html .
		if (posix_memalign (&pRet, alignment, size))
			return NULL;
		return pRet;
	}
#endif

#ifdef DEBUG
	/*
	void *ubf_alloca (size_t size)
	{
		return alloca (size);
	}
	*/
#endif

void *ubf_memdup (const void *mem, size_t len)
{
	void	*dup;
	
	dup = ubf_malloc (len);
	if (dup)
	{
		if (len)
			memcpy (dup, mem, len);
	}
	return dup;
}

void *ubf_memdup_siz (const void *mem, size_t len, size_t siz)
{
	void	*dup;
	
	//ubf_assert (siz >= len);
	
	dup = ubf_malloc (siz);
	if (dup)
	{
		if (len && siz > len)
			memcpy (dup, mem, len);
	}
	return dup;
}
/****************************************************************************************

	File:		bulkmalloc.c
	Why:		Functions for bulk dynamic memory handling.
	OS:			C99
	Author:		Thomas
	Created:	2021-07-29

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2021-07-29	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdbool.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./bulkmalloc.h"
	#include "./ubfmem.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ArrayMacros.h"
		#include "./ubf_date_and_time.h"
		#include "./ubfdebug.h"
	#else
		#include "./../pre/ArrayMacros.h"
		#include "./../datetime/ubf_date_and_time.h"
		#include "./../dbg/ubfdebug.h"
	#endif

	#ifdef BUILD_BULKMALLOC_TEST_FUNCTIONS
		#include <stdio.h>
	#endif

#endif

static void AssertAlignment (uint16_t alignment)
{
	ubf_assert	(
						   0 == alignment
					||	   1 == alignment
					||	   8 == alignment
					||	  16 == alignment
					||	  32 == alignment
					||	  64 == alignment
					||	 128 == alignment
					||	 256 == alignment
					||	 512 == alignment
					||	1024 == alignment
					||	2048 == alignment
					||	4096 == alignment
				);
	UNREFERENCED_PARAMETER (alignment);
}

size_t CalculateSizeFromUnit (size_t size, enum en_sbulkmem_unit unit)
{
	size_t		stRet	= 0;

	ubf_assert ((size_t) -1 != size);
	// Some assertions aboout the unit constants.
	ubf_assert (EN_SBULKMEM_UNIT_BYTES == EN_SBULKMEM_UNIT_OCTETS);
	ubf_assert (EN_SBULKMEM_UNIT_KiB > EN_SBULKMEM_UNIT_BYTES);
	ubf_assert (EN_SBULKMEM_UNIT_MiB > EN_SBULKMEM_UNIT_KiB);
	ubf_assert (EN_SBULKMEM_UNIT_GiB > EN_SBULKMEM_UNIT_MiB);
	if (size)
	{
		stRet = size;
		stRet *= unit >= EN_SBULKMEM_UNIT_KiB ? 1024 : 1;
		stRet *= unit >= EN_SBULKMEM_UNIT_MiB ? 1024 : 1;
		stRet *= unit >= EN_SBULKMEM_UNIT_GiB ? 1024 : 1;
	} else
	{
		stRet = DEFAULT_SBULKMEM_SIZE;
	}
	return stRet;
}

size_t CalculateAlignedSize (size_t size, uint16_t alignment)
{
	size_t	stRequested;

	ubf_assert ((size_t) -1 != size);
	switch (alignment)
	{
		case 1:
			stRequested = size;
			break;
		case 0:
			alignment = DEFAULT_SBULKMEM_ALIGNMENT;
		default:
			AssertAlignment (alignment);
			stRequested = ALIGNED_SIZE (size, alignment);
			break;
	}
	return stRequested;
}

void *AlignMemblock (void *pBlock, size_t size, uint16_t alignment)
{
	size_t	asize = CalculateAlignedSize (size, alignment);
	ubf_assert (asize >= size);

	char	*p	= pBlock;
	p += asize - size;
	return p;
}

#ifdef DEBUG
	void InitSBULKMEM (SBULKMEM *pPlinth, size_t defaultSize)
	{
		ubf_assert_non_NULL (pPlinth);

		pPlinth->psbulkFirst	=	NULL;
		pPlinth->psbulkLast		=	NULL;
		pPlinth->stDefaultSize		= defaultSize;
		pPlinth->uiFlags			= 0;
		#ifdef BUILD_POPSBULKMEM
			pPlinth->psbulkLastUsed	= NULL;
		#endif
	}
#endif

SBULKMEM *AllocInitSBULKMEM	(
			SBULKMEM						*pPlinth,
			size_t							stDefaultSize
							)
{
	SBULKMEM	*plinth;
	size_t		stTotal;
	size_t		stSBULKMEM			= ALIGNED_SIZE (sizeof (SBULKMEM), DEFAULT_SBULKMEM_ALIGNMENT);
	size_t		stSBULKMEMBLOCK		= ALIGNED_SIZE (sizeof (SBULKMEMBLOCK), DEFAULT_SBULKMEM_ALIGNMENT);
	size_t		stMemblockSize		= ALIGNED_SIZE (stDefaultSize, DEFAULT_SBULKMEM_ALIGNMENT);

	ubf_assert (0 != stDefaultSize);
	ubf_assert ((size_t) -1 != stDefaultSize);
	ubf_assert (1 == sizeof (char));
	ubf_assert (1 == sizeof (uint8_t));

	if (pPlinth)
	{
		plinth = pPlinth;
		if (pPlinth->psbulkFirst)
		{
			GrowSBULKMEM (plinth, 0);
		} else
		{
			SBULKMEMBLOCK		*pbm;
			
			stTotal								= stSBULKMEMBLOCK + stMemblockSize;
			plinth->uiFlags						= 0;
			plinth->stDefaultSize				= stMemblockSize;
			pbm = ubf_malloc (stTotal);
			if (pbm)
			{
				char			*pWork;							// Just a pointer to work with.
	
				pWork = (char *) pbm;
				pWork += stSBULKMEMBLOCK;
				plinth->psbulkFirst				= pbm;
				plinth->psbulkLast				= pbm;
				pbm->uiFlags					= USBM_STRUCT_ALLOCATED | USBM_STRUCT_INITIALISED;
				pbm->pMemBlockBase				= pWork;
				pbm->pNextAvailable				= pWork;
				pbm->pNextElement				= NULL;
				pbm->stSize						= stMemblockSize;
				pbm->stFreeSize					= stMemblockSize;
			} else
			{
				plinth->psbulkFirst				= NULL;
				plinth->psbulkLast				= NULL;
			}
		}
	} else
	{
		stTotal									= stSBULKMEM + stSBULKMEMBLOCK + stMemblockSize;
		plinth = ubf_malloc (stTotal);
		if (plinth)
		{
			char			*pWork;							// Just a pointer to work with.
	
			plinth->uiFlags						= USBM_STRUCT_ALLOCATED | USBM_STRUCT_INITIALISED;
			plinth->stDefaultSize				= stMemblockSize;
			pWork = (char *) plinth;
			pWork += stSBULKMEM;
			plinth->psbulkFirst					= (SBULKMEMBLOCK *) pWork;
			plinth->psbulkLast					= (SBULKMEMBLOCK *) pWork;
			plinth->psbulkFirst->pMemBlockBase	= (void *) (pWork + stSBULKMEM + stSBULKMEMBLOCK);
			plinth->psbulkFirst->pNextAvailable	= (void *) (pWork + stSBULKMEM + stSBULKMEMBLOCK);
			plinth->psbulkFirst->pNextElement	= NULL;
			plinth->psbulkFirst->stSize			= stMemblockSize;
			plinth->psbulkFirst->stFreeSize		= stMemblockSize;
			plinth->psbulkFirst->uiFlags		= 0;
		#ifdef BUILD_POPSBULKMEM
			plinth->psbulkLastUsed				= NULL;
		#endif
		}
	}
	return plinth;
}

void EmptySBULKMEM (SBULKMEM *pPlinth)
{
	ubf_assert_non_NULL (pPlinth);

	SBULKMEMBLOCK *p = pPlinth->psbulkFirst;
	while (p)
	{
		p->pNextAvailable		= p->pMemBlockBase;
		p->stFreeSize			= p->stSize;
		p						= p->pNextElement;
	}
	#ifdef BUILD_POPSBULKMEM
		pPlinth->psbulkLastUsed	= NULL;
	#endif
}

SBULKMEM *DoneSBULKMEM (SBULKMEM *pPlinth)
{
	SBULKMEMBLOCK	*p;
	SBULKMEMBLOCK	*l;

	ubf_assert (NULL != pPlinth);
	p = pPlinth->psbulkFirst;
	while (p)
	{
		if (p->uiFlags & USBM_MEM_BLOCK_ALLOCATED && p->pMemBlockBase)
		{
			ubf_free (p->pMemBlockBase);
		}
		l = p;
		p = p->pNextElement;
		if (l->uiFlags & USBM_STRUCT_ALLOCATED)
			ubf_free (l);
	}
	if (pPlinth->uiFlags & USBM_MEM_BLOCK_ALLOCATED)
	{
		ubf_free (pPlinth);
		return NULL;
	}
	pPlinth->psbulkFirst	= NULL;
	pPlinth->psbulkLast		= NULL;
#ifdef BUILD_POPSBULKMEM
	pPlinth->psbulkLastUsed	= NULL;
#endif
	return pPlinth;
}

void AddSBULKMEMBLOCKtoPlinth (SBULKMEM *pPlinth, SBULKMEMBLOCK *pnew)
{
	ubf_assert_non_NULL (pPlinth);
	ubf_assert_non_NULL (pnew);
	
	if (pPlinth->psbulkLast)
	{
		pPlinth->psbulkLast->pNextElement		= pnew;
		pPlinth->psbulkLast						= pnew;
	} else
	{
		pPlinth->psbulkFirst					= pnew;
		pPlinth->psbulkLast						= pnew;
	}
}

SBULKMEMBLOCK *AllocAndAddEmptySBULKMEMBLOCK (SBULKMEM *pPlinth)
{
	SBULKMEMBLOCK	*pnew;
	size_t			stSBULKMEMBLOCK	= ALIGNED_SIZE (sizeof (SBULKMEMBLOCK), DEFAULT_SBULKMEM_ALIGNMENT);

	ubf_assert_non_NULL (pPlinth);
	pnew = ubf_malloc (stSBULKMEMBLOCK);
	if (pnew)
	{
		pnew->uiFlags							= USBM_STRUCT_ALLOCATED;
		pnew->pMemBlockBase						= NULL;
		pnew->pNextAvailable					= NULL;
		pnew->pNextElement						= NULL;
		pnew->stFreeSize						= 0;
		pnew->stSize							= 0;
		AddSBULKMEMBLOCKtoPlinth (pPlinth, pnew);
	}
	return pnew;
}

SBULKMEMBLOCK *GrowSBULKMEM	(
			SBULKMEM						*pPlinth,
			size_t							size
							)
{
	SBULKMEMBLOCK	*pnew;

	ubf_assert_non_NULL (pPlinth);
	ubf_assert ((size_t) -1 != size);

	if (size <= pPlinth->stDefaultSize)
		size = pPlinth->stDefaultSize;
	if (size)
	{
		size_t			stSBULKMEMBLOCK	= ALIGNED_SIZE (sizeof (SBULKMEMBLOCK), DEFAULT_SBULKMEM_ALIGNMENT);
		size_t			stMemoryBlock	= CalculateAlignedSize (size, DEFAULT_SBULKMEM_ALIGNMENT);

		pnew = ubf_malloc (stSBULKMEMBLOCK + stMemoryBlock);
		if (pnew)
		{
			pnew->uiFlags						= USBM_STRUCT_ALLOCATED;
			pnew->pMemBlockBase					= ((char *) pnew) + stSBULKMEMBLOCK;
			pnew->pNextAvailable				= ((char *) pnew) + stSBULKMEMBLOCK;
			pnew->pNextElement					= NULL;
			pnew->stSize						= stMemoryBlock;
			pnew->stFreeSize					= stMemoryBlock;
			AddSBULKMEMBLOCKtoPlinth (pPlinth, pnew);
		}
	} else
	{	// 0 == size.
		pnew = AllocAndAddEmptySBULKMEMBLOCK (pPlinth);
	}
	return pnew;
}

SBULKMEMBLOCK *FindAvailableSizeOrGrow	(
			SBULKMEM						*p,
			size_t							size,
			enum en_sbulkmem_allow_growth	grow
										)
{
	ubf_expect_non_NULL (p);
	ubf_assert ((size_t) -1 != size);

	SBULKMEMBLOCK	*pbm	= p->psbulkFirst;

	while (pbm)
	{
		if (size <= pbm->stFreeSize)
			return pbm;
		pbm = pbm->pNextElement;
	}
	if (EN_SBULKMEM_CANNOT_GROW == grow)
		return NULL;
	// Note that size has been aligned here already.
	pbm = GrowSBULKMEM (p, size);
	if (pbm)
	{
		ubf_assert (pbm->stFreeSize >= size);
		ubf_assert (pbm->stSize >= size);
	}
	return pbm;
}

void *GetMemFromSBULKMEMBLOCKifAvailable (SBULKMEMBLOCK *pbm, size_t size)
{
	void	*pRet		= NULL;

	ubf_assert_not_NULL (pbm);
	ubf_assert ((size_t) -1 != size);

	if (pbm->stFreeSize >= size)
	{
		pRet			= pbm->pNextAvailable;
		char	*p		= pRet;

		p += size;
		pbm->pNextAvailable		= p;
		pbm->stFreeSize			-= size;
	}
	return pRet;
}

void *GetMemFromSBULKMEM	(
			SBULKMEM						*pPlinth,
			size_t							size,
			uint8_t							alignment,
			enum en_sbulkmem_allow_growth	grow
							)
{
	SBULKMEMBLOCK		*pbm;
	size_t				stRequested;

	ubf_assert_non_NULL (pPlinth);
	ubf_assert ((size_t) -1 != size);

	AssertAlignment (alignment);
	stRequested = CalculateAlignedSize (size, alignment);
	pbm = FindAvailableSizeOrGrow (pPlinth, stRequested, grow);
	if (pbm)
	{
		void *pRet = NULL;

		pRet = GetMemFromSBULKMEMBLOCKifAvailable (pbm, stRequested);
		// Align the returned block.
		ubf_assert (stRequested >= size);

		char *p = pRet;
		p += stRequested - size;
		#ifdef BUILD_POPSBULKMEM
			pPlinth->psbulkLastUsed = pbm;
		#endif
		return p;
	}
	return NULL;
}

/*
	Copy of GetMemFromSBULKMEM () with the most common parameters.
	Alignment is 8 for 16 bit CPUs and 16 for 64 bit CPUs, and the block can grow.
*/
void *GetAlignedMemFromSBULKMEMgrow	(
			SBULKMEM						*pPlinth,
			size_t							size
										)
{
	SBULKMEMBLOCK		*pbm;
	size_t				stRequested;

	ubf_assert (NULL != pPlinth);
	ubf_assert ((size_t) -1 != size);

	stRequested = ALIGNED_SIZE (size, DEFAULT_SBULKMEM_ALIGNMENT);
	pbm = FindAvailableSizeOrGrow (pPlinth, stRequested, EN_SBULKMEM_CAN_GROW);
	if (pbm)
	{
		void *pRet = NULL;

		pRet = GetMemFromSBULKMEMBLOCKifAvailable (pbm, stRequested);
		// Align the returned block.
		ubf_assert (stRequested >= size);

		char *p = pRet;
		p += stRequested - size;
		#ifdef BUILD_POPSBULKMEM
			pPlinth->psbulkLastUsed = pbm;
		#endif
		return p;
	}
	return NULL;
}

void *GetAlignedMemFromSBULKMEMnogrow	(
			SBULKMEM						*pPlinth,
			size_t							size
										)
{
	ubf_assert (NULL != pPlinth);
	ubf_assert ((size_t) -1 != size);

	return GetMemFromSBULKMEM (pPlinth, size, DEFAULT_SBULKMEM_ALIGNMENT, EN_SBULKMEM_CANNOT_GROW);
}

void *GetUnalignedMemFromSBULKMEMgrow	(
			SBULKMEM						*pPlinth,
			size_t							size
										)
{
	ubf_assert (NULL != pPlinth);
	ubf_assert ((size_t) -1 != size);

	return GetMemFromSBULKMEM (pPlinth, size, 1, EN_SBULKMEM_CAN_GROW);
}

void *GetUnalignedMemFromSBULKMEMnogrow	(
			SBULKMEM						*pPlinth,
			size_t							size
										)
{
	ubf_assert (NULL != pPlinth);
	ubf_assert ((size_t) -1 != size);

	return GetMemFromSBULKMEM (pPlinth, size, 1, EN_SBULKMEM_CANNOT_GROW);
}

void popSBULKMEMBLOCK					(
			SBULKMEMBLOCK					*pb,
			size_t							size
										)
{
	ubf_assert_non_NULL (pb);
	ubf_assert ((char *) pb->pNextAvailable - size >= (char *) pb->pMemBlockBase);
	
	pb->pNextAvailable	= (char *) pb->pNextAvailable - size;
	pb->stFreeSize		+= size;
}

#ifdef BUILD_POPSBULKMEM
	void popSBULKMEM						(
				SBULKMEM						*pPlinth,
				size_t							size
											)
	{
		ubf_assert_non_NULL (pPlinth);
		ubf_assert_non_NULL (pPlinth->psbulkLastUsed);
		
		popSBULKMEMBLOCK (pPlinth->psbulkLastUsed, size);
	}
#endif

void getSBULKMEMstats (SBULKMEMSTATS *pStats, SBULKMEM *pPlinth)
{
	ubf_assert_non_NULL (pStats);
	ubf_assert_non_NULL (pPlinth);

	pStats->stDefaultSize		= pPlinth->stDefaultSize;
	pStats->nSBULKMEMBLOCKs		= 0;
	pStats->stFreeLargest		= 0;
	pStats->stFreeSmallest		= (size_t) -1;
	pStats->stTotal				= 0;
	pStats->stFreeTotal			= 0;
	pStats->stFreeAverage		= 0;

	SBULKMEMBLOCK	*p	= pPlinth->psbulkFirst;
	while (p)
	{
		++ pStats->nSBULKMEMBLOCKs;
		pStats->stTotal += p->stSize;
		pStats->stFreeTotal += p->stFreeSize;
		if (p->stFreeSize > pStats->stFreeLargest)
			pStats->stFreeLargest = p->stFreeSize;
		if (p->stFreeSize < pStats->stFreeSmallest)
			pStats->stFreeSmallest = p->stFreeSize;
		p = p->pNextElement;
	}
	if (pStats->nSBULKMEMBLOCKs)
	{
		pStats->stFreeAverage = pStats->stFreeTotal / pStats->nSBULKMEMBLOCKs;
	} else
	{
		pStats->stFreeSmallest = 0;
	}
}

#ifdef BUILD_BULKMALLOC_TEST_FUNCTIONS
	bool bulkmalloc_test_fnct (void)
	{
		bool			b		= true;
		SBULKMEM		sbm		= SBULKMEM_INITIALISER (128);
		//SBULKMEM		*pbm;
		void			*p;
		SBULKMEMBLOCK	*pbmb;

		// The SBULKMEM structure doesn't have any memory block attached.
		p = GetUnalignedMemFromSBULKMEMnogrow (&sbm, 1);
		ubf_assert_NULL (p);
		b &= NULL == p;
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 1);
		ubf_assert_NULL (p);
		b &= NULL == p;
		p = GetUnalignedMemFromSBULKMEMnogrow (&sbm, 1024);
		ubf_assert_NULL (p);
		b &= NULL == p;
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 1024);
		ubf_assert_NULL (p);
		b &= NULL == p;
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 1022);
		ubf_assert_NULL (p);
		b &= NULL == p;

		pbmb = GrowSBULKMEM (&sbm, 0);
		ubf_assert_non_NULL (pbmb);
		b &= NULL != pbmb;
		pbmb = GrowSBULKMEM (&sbm, 12);
		ubf_assert_non_NULL (pbmb);
		b &= NULL != pbmb;
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 1022);
		ubf_assert_NULL (p);
		b &= NULL == p;
		// This should succeed.
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 16);
		ubf_assert_non_NULL (p);
		b &= NULL != p;
		p = GetAlignedMemFromSBULKMEMgrow (&sbm, 12);
		ubf_assert_non_NULL (p);
		b &= NULL != p;

		DoneSBULKMEM (&sbm);

		p = GetAlignedMemFromSBULKMEMgrow (&sbm, 1024);
		ubf_assert_non_NULL (p);
		b &= NULL != p;
		if (p)
		{
			memset (p, 0, 1024);
		}
		DoneSBULKMEM (&sbm);

		/*
			Let's compare the two solutions when it comes to speed.
			We allocate 100 000 memory blocks with 128 octets each, write to the block,
			then deallocate it again. Method 2) allocates 200 MiB.

			1) malloc () and free ()
			2) AllocInitSBULKMEM (), GetMemFromSBULKMEM (), and DoneSBULKMEM ()
		*/
		SBULKMEM	*pTest;

		FILETIME	ft1, ft2;
		uint64_t	ut1, ut2, ut3;
		uint64_t	u;

		GetSystemTimeAsFileTime (&ft1);
		char *pCh [100000];
		for (u = 0; u < 100000; ++u)
		{
			pCh [u] = malloc (128);
			if (pCh [u])
				memset (pCh [u], 0, 128);
		}
		for (u = 0; u < 100000; ++u)
		{
			free (pCh [u]);
		}
		GetSystemTimeAsFileTime (&ft2);
		ut1 = ft1.dwHighDateTime;
		ut1 <<= 32;
		ut1 += ft1.dwLowDateTime;
		ut2 = ft2.dwHighDateTime;
		ut2 <<= 32;
		ut2 += ft2.dwLowDateTime;
		ut3 = ut2 - ut1;
		ut3 /= 10000;			// milliseconds.
		printf ("\nTime taken by malloc ():\t\t %" PRIu64 " ms.\n", ut3);

		GetSystemTimeAsFileTime (&ft1);
		pTest = AllocInitSBULKMEM (NULL, 200 * 1024 * 1024);
		if (pTest)
		{
			for (u = 0; u < 100000; ++u)
			{
				pCh [u] = GetMemFromSBULKMEM (pTest, 128, 0, EN_SBULKMEM_CANNOT_GROW);
				if (pCh [u])
					memset (pCh [u], 0, 128);
			}
			DoneSBULKMEM (pTest);
			GetSystemTimeAsFileTime (&ft2);
			ut1 = ft1.dwHighDateTime;
			ut1 <<= 32;
			ut1 += ft1.dwLowDateTime;
			ut2 = ft2.dwHighDateTime;
			ut2 <<= 32;
			ut2 += ft2.dwLowDateTime;
			ut3 = ut2 - ut1;
			ut3 /= 10000;			// milliseconds.
			printf ("Time taken by GetMemFromSBULKMEM ():\t %" PRIu64 " ms.\n", ut3);
		}
		/*
			End of speed comparison.
		*/

		// Example 1 used for the top of the header file.
		//	->
		#define OUR_BULK_DATA_ARRAY_SIZE	(100)

		SBULKMEM	sbulk	= SBULKMEM_INITIALISER (DEFAULT_SBULKMEM_SIZE);
		void		*pDat2 [OUR_BULK_DATA_ARRAY_SIZE];
		int			i;

		for (i = 0; i < OUR_BULK_DATA_ARRAY_SIZE; ++i)
		{
			pDat2 [i] = GetMemFromSBULKMEM (&sbulk, 1024, 8, EN_SBULKMEM_CAN_GROW);
			if (pDat2 [i])
			{
				memset (pDat2 [i], 0xFF, 1024);
			}
		}
		DoneSBULKMEM (&sbulk);
		//	<-

		// Example 2 used for the top of the header file.
		//	->
		#define OUR_BULK_DATA_ARRAY_SIZE	(100)
		SBULKMEM	*pBulk	= AllocInitSBULKMEM (NULL, DEFAULT_SBULKMEM_SIZE);
		if (pBulk)
		{
			void		*pData [OUR_BULK_DATA_ARRAY_SIZE];
			int			j;

			for (j = 0; j < OUR_BULK_DATA_ARRAY_SIZE; ++j)
			{
				pData [j] = GetMemFromSBULKMEM (&sbulk, 1024, 0, EN_SBULKMEM_CAN_GROW);
				if (pData [j])
				{
					memset (pData [j], 0xFF, 1024);
				}
			}
			DoneSBULKMEM (&sbulk);
		}
		//	<-

		return b;
	}
#endif
/****************************************************************************************

	File:		memstrstr.c
	Created:	?

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2014-03-10	Thomas			This history added.
2016-12-09	Thomas			Definitions for TRUE and FALSE removed.
2019-10-13	Thomas			Include files moved to the header.
2024-05-21	Thomas			Function memstrrchr () fixed.

	The original version of this function has been taken from
	http://www.koders.com/c/fid2330745E0E8C0A0F5E2CF94799642712318471D0.aspx?s=getopt#L459
	which is buggy and doesn't work under certain circumstances.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "memstrstr.h"

#endif

/*
	Note: These comments are only here for convenience. Refer to the
	 ones in memstrstr.h. They might be more accurate.

	Function:	memstrstr

	The function returns a pointer to the first occurrence of s2 in s1.
	If s2 cannot be found in s1 the return value is NULL. If size2 is
	0 the function returns s1.

	The function memstrstr is basically a replacement for strstr
	(http://www.cppreference.com/stdstring/strstr.html) but does not require
	NULL terminated strings. It can handle any buffer which can contain any
	byte sequence including NUL characters.

	The original version of this function has been taken from
	http://www.koders.com/c/fid2330745E0E8C0A0F5E2CF94799642712318471D0.aspx?s=getopt#L459
	which needed fixing first. The page has disappeared in the meantime (2018). The
	Wayback Machine also holds no copy of it.
*/
char *memstrstr (const char *s1, size_t size1, const char *s2, size_t size2)
{
	size_t		count1		= size1;
	size_t		count2		= size2;
	size_t		count3		= 0;
	const char	*s1_ptr		= s1;
	const char	*s2_ptr		= s2;

	if (0 == size2)
		return (char *) s1;
	while (count1 --)
	{
		if (*s1_ptr ++ == *s2_ptr ++)
		{
			if (-- count2 == 0)
				return (char *) (s1_ptr - size2);
			count3 ++;
			continue;
		} else
		{
			if (count3)
			{
				s1_ptr -= count3;
				count1 += count3;
				count3 = 0;
			}
		}
		count2 = size2;
		s2_ptr = s2;
	}
	return NULL;
}

/*
	Note: These comments are only here for convenience. Refer to the
	 ones in memstrstr.h. They might be more accurate.

	Like memstrstr but only returns the nth occurrance of s2 in s1 if
	s2 has been found amount times in s1. Returns NULL if s1 does not
	contain s2 amount times.

	Example:
	s1: "This is a test for memstrstrn."
	s2: "e"
	amount: 2 (second occurrance of s2 in s1)
	Returns "emstrstrn."
*/
char *memstrstrn (char *s1, size_t size1, char *s2, size_t size2, size_t amount)
{
	char		*chF	= s1;
	char		*chM	= s1 + size1 - 1;		// Max chF.
	size_t		a		= amount;
	size_t		st1		= size1;

	if (amount > size1)
		return NULL;
	while (a --)
	{
		if (chF)
		{
			chF = memstrstr (chF, st1, s2, size2);
			if ((chF) && (chF < chM))
			{
				chF ++;
				st1 = size1 - (size_t)(chF - s1);
			} else
			{
				if (a)
					return NULL;
			}
		} else
			return NULL;
	}
	return chF;
}

size_t memchrreplace (char *s, size_t size, char s1, char s2)
{
	size_t r	= 0;
	size_t o	= 0;

	while (o < size)
	{
		if (s [o] == s1)
		{
			s [o] = s2;
			++ r;
		}
		++ o;
	}
	return r;
}

char *memstrchr (const char *str, size_t len, char character)
{
	while (len && *str != character)
	{
		++ str;
		-- len;
	}
	return (char *) (len && *str == character ? str : NULL);
}

char *memstrrchr (const char *str, size_t len, char character)
{
	const char *sr = str;
	while (len && str [len - 1] != character)
	{
		-- len;
	}
	sr += len ? len - 1 : 0;
	return (char *) (*sr == character ? sr : NULL);
}
/****************************************************************************************

	File:		VectorC.c
	Why:		Implements vector types in C.
	OS:			-
	Author:		Thomas
	Created:	2019-12-09
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2019-12-09	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	Implements vectors (variable arrays) in C. If you change this text please copy
	and paste it in the code or header file too.
	
	The code has been inspired by several different sources and changed/extended.
	
	Sources:
	https://github.com/rxi/vec
	https://eddmann.com/posts/implementing-a-dynamic-vector-array-in-c/
	https://stackoverflow.com/questions/4694401/how-to-replicate-vector-in-c
	https://github.com/Mashpoe/c-vector
	
	Most of the code comes from https://github.com/rxi/vec .
	A good introduction can be found at https://jameshfisher.com/2017/04/06/rxi-vec/ .

	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Since most of the code is from the rxi implementation: Copyright (c) 2014 rxi

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/** 
 * Copyright (c) 2014 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See LICENSE for details.
 */
 
#ifndef CUNILOG_USE_COMBINED_MODULE

    #include "./VectorC.h"

#endif

int vec_expand_(char **data, size_t *length, size_t *capacity, size_t memsz) {
  if (*length + 1 > *capacity) {
    void *ptr;
    size_t n = (*capacity == 0) ? 1 : *capacity << 1;
    ptr = realloc(*data, n * memsz);
    if (ptr == NULL) return -1;
    *data = ptr;
    *capacity = n;
  }
  return 0;
}


int vec_reserve_(char **data, size_t *length, size_t *capacity, size_t memsz, size_t n) {
  (void) length;
  if (n > *capacity) {
    void *ptr = realloc(*data, n * memsz);
    if (ptr == NULL) return -1;
    *data = ptr;
    *capacity = n;
  }
  return 0;
}


int vec_reserve_po2_(
  char **data, size_t *length, size_t *capacity, size_t memsz, size_t n
) {
  size_t n2 = 1;
  if (n == 0) return 0;
  while (n2 < n) n2 <<= 1;
  return vec_reserve_(data, length, capacity, memsz, n2);
}


int vec_compact_(char **data, size_t *length, size_t *capacity, size_t memsz) {
  if (*length == 0) {
    free(*data);
    *data = NULL;
    *capacity = 0;
    return 0;
  } else {
    void *ptr;
    size_t n = *length;
    ptr = realloc(*data, n * memsz);
    if (ptr == NULL) return -1;
    *capacity = n;
    *data = ptr;
  }
  return 0;
}


int vec_insert_(char **data, size_t *length, size_t *capacity, size_t memsz,
                 size_t idx
) {
  int err = vec_expand_(data, length, capacity, memsz);
  if (err) return err;
  memmove(*data + (idx + 1) * memsz,
          *data + idx * memsz,
          (size_t) ((*length - idx) * memsz));
  return 0;
}


void vec_splice_(char **data, size_t *length, size_t *capacity, size_t memsz,
                 size_t start, size_t count
) {
  (void) capacity;
  memmove(*data + start * memsz,
          *data + (start + count) * memsz,
          (size_t) ((*length - start - count) * memsz));
}


void vec_swapsplice_(char **data, size_t *length, size_t *capacity, size_t memsz,
                     size_t start, size_t count
) {
  (void) capacity;
  memmove(*data + start * memsz,
          *data + (*length - count) * memsz,
          (size_t) (count * memsz));
}


void vec_swap_(char **data, size_t *length, size_t *capacity, size_t memsz,
               size_t idx1, size_t idx2
) {
  unsigned char *a, *b, tmp;
  size_t count;
  (void) length;
  (void) capacity;
  if (idx1 == idx2) return;
  a = (unsigned char*) *data + idx1 * memsz;
  b = (unsigned char*) *data + idx2 * memsz;
  count = memsz;
  while (count--) {
    tmp = *a;
    *a = *b;
    *b = tmp;
    a++; b++;
  }
}
/****************************************************************************************

	File		shortmonths.h
	Why:		An array with the short month names in English.
	OS:			C99
	Created:	2025-02-02

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-02-02	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./externC.h"
		#include "./platform.h"
	#else
		#include "./../pre/externC.h"
		#include "./../pre/platform.h"
	#endif

#endif

const char ccdtMnths [12][4] =
		{"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
/****************************************************************************************

	File:		ubf_time.c
	Why:		Contains structures and functions to work with UBF_TIMESTAMP
				and SUBF_TIMESTRUCT structures.
	OS:			C99.
	Author:		Thomas
	Created:	2018-03-29
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2018-03-29	Thomas			Created.
2020-09-23	Thomas			Extended.
2021-01-01	Thomas			Format of UBF_TIMESTAMP changed. The old one had a few
							 issues.
							 
****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// Function ftime ():
/*
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./ubf_times.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
	#else
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996)									// MSVC says some functions are unsafe.
#endif

UBF_TIMESTAMP UBF_TIMESTAMP_OFFSET_BITS (SUBF_TIMESTRUCT *ts)
{
	UBF_TIMESTAMP			s;

	ubf_assert_non_NULL (ts);

	/*
		Specification from 2021-01-01:

		Bit 76543210
			3210|||.	UTC offset in hours
				0||.	1: UTC offset is negative
				 ||.	0: UTC offset is positive
				 0|.	1: UTC offset + 30 minutes
				  |.	0: UTC offset unchanged
				  0.	1: UTC offset + 15 minutes
						0: UTC offset unchanged
	*/
	s = ts->uOffsetHours;
	s <<= 4;
	s |= ts->bOffsetNegative ? 0x08 : 0;
	switch (ts->uOffsetMinutes)
	{	// Can be: 0, 15, 30, and 45.
	case 0:
		break;
	case 15:
		s |= 0x02;												// Bit 0.
		break;
	case 30:
		s |= 0x04;												// Bit 1.
		break;
	case 45:
		s |= 0x06;												// Bit 0 and 1.
		break;
	default:
		ubf_assert (false);
	}
	return s;
}

void SUBF_TIMESTRUCT_to_UBF_TIMESTAMP (UBF_TIMESTAMP *t, SUBF_TIMESTRUCT *ts)
{
	ubf_assert (NULL != t);
	ubf_assert (NULL != ts);
	ubf_assert (ts->uMicrosecond < 1000);
	ubf_assert (ts->uMillisecond < 1000);

	UBF_TIMESTAMP			s;

	s = UBF_TIMESTAMP_OFFSET_BITS (ts);
	s |= UBF_TIMESTAMP_MICROSECOND_BITS (ts);
	s |= UBF_TIMESTAMP_MILLISECOND_BITS (ts);
	s |= UBF_TIMESTAMP_SECOND_BITS (ts);
	s |= UBF_TIMESTAMP_MINUTE_BITS (ts);
	s |= UBF_TIMESTAMP_HOUR_BITS (ts);
	s |= UBF_TIMESTAMP_DAY_BITS (ts);
	s |= UBF_TIMESTAMP_MONTH_BITS (ts);
	s |= UBF_TIMESTAMP_YEAR_BITS (ts);
	*t = s;
}

#ifdef OS_IS_ANDROID
	#ifdef BUILD_ANDROID_FTIME_HERE
		// See https://github.com/01org/android-bluez-bionic/blob/master/libc/bionic/ftime.c .
		//	Without this we'll get an "undefined reference to ftime" further down.

		/*
		 * Copyright (C) 2008 The Android Open Source Project
		 * All rights reserved.
		 *
		 * Redistribution and use in source and binary forms, with or without
		 * modification, are permitted provided that the following conditions
		 * are met:
		 *  * Redistributions of source code must retain the above copyright
		 *    notice, this list of conditions and the following disclaimer.
		 *  * Redistributions in binary form must reproduce the above copyright
		 *    notice, this list of conditions and the following disclaimer in
		 *    the documentation and/or other materials provided with the
		 *    distribution.
		 *
		 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
		 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
		 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
		 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
		 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
		 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
		 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
		 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
		 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
		 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
		 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
		 * SUCH DAMAGE.
		 */

		int ftime (struct timeb *tb)
		{
			struct timeval  tv;
			struct timezone tz;
	
			if (gettimeofday (&tv, &tz) < 0)
				return -1;
	
			tb->time    = tv.tv_sec;
			tb->millitm = (tv.tv_usec + 500) / 1000;
	
			if (tb->millitm == 1000) {
				++tb->time;
				tb->millitm = 0;
			}
			tb->timezone = (short) tz.tz_minuteswest;
			tb->dstflag  = (short) tz.tz_dsttime;
	
			return 0;
		}
	#endif
#endif

void GetSystemTime_SUBF_TIMESTRUCT (SUBF_TIMESTRUCT *pts)
{
	ubf_assert (NULL != pts);
	
	unsigned short	ms;

	#ifdef PLATFORM_IS_WINDOWS
		struct	timeb	timebuffer;
		_ftime (&timebuffer);
		ms = timebuffer.millitm;
	#else
		struct timeval	tv;
		struct timezone	tz;
		gettimeofday (&tv, &tz);
		ms = (tv.tv_usec + 500) / 1000;
	#endif

	time_t t_time;
	time (&t_time);
	struct tm stime;
	gmtime_r (&t_time, &stime);
	
	pts->uYear				= (unsigned int) stime.tm_year + 1900;
	pts->uMonth				= (unsigned int) stime.tm_mon + 1;
	pts->uDay				= (unsigned int) stime.tm_mday;
	pts->uHour				= (unsigned int) stime.tm_hour;
	pts->uMinute			= (unsigned int) stime.tm_min;
	pts->uSecond			= (unsigned int) stime.tm_sec;
	pts->uMillisecond		= ms;
	pts->uMicrosecond		= 0;
	pts->uOffsetHours		= 0;
	pts->uOffsetMinutes		= 0;
	pts->bOffsetNegative	= false;
}

void GetSystemTime_UBF_TIMESTAMP (UBF_TIMESTAMP *ut)
{
	SUBF_TIMESTRUCT			ts;

	GetSystemTime_SUBF_TIMESTRUCT (&ts);
	SUBF_TIMESTRUCT_to_UBF_TIMESTAMP (ut, &ts);
}

UBF_TIMESTAMP GetSystemTimeAsUBF_TIMESTAMP (void)
{
	SUBF_TIMESTRUCT	ts;
	UBF_TIMESTAMP	ut;

	GetSystemTime_SUBF_TIMESTRUCT (&ts);
	SUBF_TIMESTRUCT_to_UBF_TIMESTAMP (&ut, &ts);
	return ut;
}

void GetLocalTime_SUBF_TIMESTRUCT_psx (SUBF_TIMESTRUCT *pts)
{
	int				i_adjustment_hours;
	int				i_adjustment_mins;
	unsigned short	ms;
	bool			bOffsetNegative = false;

	#ifdef PLATFORM_IS_WINDOWS
		struct	timeb	timebuffer;
		_ftime (&timebuffer);
		if (timebuffer.timezone < 0)
		{
			bOffsetNegative = true;
			timebuffer.timezone *= -1;
		}
		i_adjustment_hours = timebuffer.timezone / 60;
		i_adjustment_mins  = timebuffer.timezone % 60;
		ms = timebuffer.millitm;
	#else
		struct timeval	tv;
		struct timezone	tz;
		gettimeofday (&tv, &tz);
		if (tz.tz_minuteswest < 0)
		{
		bOffsetNegative = true;
			tz.tz_minuteswest *= -1;
		}
		i_adjustment_hours	= tz.tz_minuteswest / 60;
		i_adjustment_mins	= tz.tz_minuteswest % 60;
		ms = (tv.tv_usec + 500) / 1000;
	#endif

	time_t t_time;
	time (&t_time);
	struct tm *stime;
	stime = localtime (&t_time);
	
	// Adjust daylight saving time (DST) value.
	//  DST automatically implies +1 hour (+01:00).
	if (stime->tm_isdst > 0) i_adjustment_hours++;
    
	// Store the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss.mis+01:00
	pts->uYear				= (unsigned int) stime->tm_year + 1900;
	pts->uMonth				= (unsigned int) stime->tm_mon + 1;
	pts->uDay				= (unsigned int) stime->tm_mday;
	pts->uHour				= (unsigned int) stime->tm_hour;
	pts->uMinute			= (unsigned int) stime->tm_min;
	pts->uSecond			= (unsigned int) stime->tm_sec;
	pts->uMillisecond		= ms;
	pts->uMicrosecond		= 0;
	pts->uOffsetHours		= (unsigned int) i_adjustment_hours;
	pts->uOffsetMinutes		= (unsigned int) i_adjustment_mins;
	pts->bOffsetNegative	= bOffsetNegative;
}

/*
	This function is very slow!
*/
#ifdef _WIN32
	void GetLocalTime_SUBF_TIMESTRUCT_win (SUBF_TIMESTRUCT *pts)
	{
		//_ASSERT (FALSE);
		// See https://msdn.microsoft.com/en-us/library/windows/desktop/ms724950(v=vs.85).aspx .
		SYSTEMTIME						st;
		// See https://msdn.microsoft.com/en-us/library/windows/desktop/ms725481(v=vs.85).aspx .
		TIME_ZONE_INFORMATION			tzi;
		// See
		//	https://docs.microsoft.com/en-us/windows/win32/api/timezoneapi/ns-timezoneapi-dynamic_time_zone_information .
		//DYNAMIC_TIME_ZONE_INFORMATION	tdi;
		LONG							lBias;					// UTC = local time + bias
																//	(in minutes).
	
		// See https://msdn.microsoft.com/en-us/library/windows/desktop/ms724390(v=vs.85).aspx .
		//GetSystemTime (&st);
		GetLocalTime (&st);
		pts->uYear				= (unsigned int) st.wYear;
		pts->uMonth				= (unsigned int) st.wMonth;
		pts->uDay				= (unsigned int) st.wDay;
		pts->uHour				= (unsigned int) st.wHour;
		pts->uMinute			= (unsigned int) st.wMinute;
		pts->uSecond			= (unsigned int) st.wSecond;
		pts->uMillisecond		= (unsigned int) st.wMilliseconds;
		pts->uMicrosecond		= 0;
		// See
		//	https://docs.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-gettimezoneinformation .
		DWORD	dwTZ;
		dwTZ = GetTimeZoneInformation (&tzi);
		// See
		//	https://docs.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-getdynamictimezoneinformation .
		//	We're ok with GetTimeZoneInformation () and don't need this.
		//GetDynamicTimeZoneInformation (&tdi);
		lBias = tzi.Bias;										// Note that the bias is negative
																//	for positive offsets.
		// See
		//	https://docs.microsoft.com/en-us/windows/win32/api/timezoneapi/nf-timezoneapi-gettimezoneinformation .
		if (TIME_ZONE_ID_DAYLIGHT == dwTZ)
		{
			lBias += tzi.DaylightBias;		
		}
		if (lBias <= 0)											// Note: 0 is positive.
		{
			lBias *= -1;
			pts->bOffsetNegative = FALSE;
		} else
		{
			pts->bOffsetNegative = TRUE;
		}
		pts->uOffsetHours		= (unsigned int) lBias / 60;
		pts->uOffsetMinutes		= lBias % 60;
	}
#endif

#ifdef DEBUG
	void GetLocalTime_SUBF_TIMESTRUCT (SUBF_TIMESTRUCT *pts)
	{
		ubf_assert (NULL != pts);
		if (pts)
		{
			#ifdef OS_IS_WINDOWS
				// The POSIX version is twice as fast here as the Windows
				//	version in debug versions and three times as fast in
				//	release builds.
				GetLocalTime_SUBF_TIMESTRUCT_psx (pts);
				//GetLocalTime_SUBF_TIMESTRUCT_win (pts);
			#else
				GetLocalTime_SUBF_TIMESTRUCT_psx (pts);
			#endif
		}
	}
#endif

/*
	Previously used.

	bool GetLocalTime_UBF_TIMESTAMP (UBF_TIMESTAMP *ut)
	{
		SUBF_TIMESTRUCT			ts;

		GetLocalTime_SUBF_TIMESTRUCT (&ts);
		return SUBF_TIMESTRUCT_to_UBF_TIMESTAMP (ut, &ts);
	}
*/

void GetLocalTime_UBF_TIMESTAMP (UBF_TIMESTAMP *ut)
{
	ubf_assert_non_NULL (ut);

	int				i_adjustment_hours;
	int				i_adjustment_mins;
	unsigned short	ms;
	bool			bOffsetNegative = false;

	#ifdef PLATFORM_IS_WINDOWS
		struct	timeb	timebuffer;
		_ftime (&timebuffer);
		if (timebuffer.timezone < 0)
		{
			bOffsetNegative = true;
			timebuffer.timezone *= -1;
		}
		i_adjustment_hours = timebuffer.timezone / 60;
		i_adjustment_mins  = timebuffer.timezone % 60;
		ms = timebuffer.millitm;
	#else
		struct timeval	tv;
		struct timezone	tz;
		gettimeofday (&tv, &tz);
		if (tz.tz_minuteswest < 0)
		{
			bOffsetNegative = true;
			tz.tz_minuteswest *= -1;
		}
		i_adjustment_hours	= tz.tz_minuteswest / 60;
		i_adjustment_mins	= tz.tz_minuteswest % 60;
		ms = (tv.tv_usec + 500) / 1000;
	#endif

	time_t t_time;
	time (&t_time);
	struct tm *stime;
	stime = localtime (&t_time);
	
	// Adjust daylight saving time (DST) value.
	//  DST automatically implies +1 hour (+01:00).
	if (stime->tm_isdst > 0) i_adjustment_hours++;
    
	SET_UBF_TIMESTAMP_OFFSET_BITS	(
		*ut, (UBF_TIMESTAMP) i_adjustment_hours, (UBF_TIMESTAMP) i_adjustment_mins, bOffsetNegative
									);
	*ut |= SET_UBF_TIMESTAMP_YEAR_BITS			((UBF_TIMESTAMP) stime->tm_year + 1900);
	*ut |= SET_UBF_TIMESTAMP_MONTH_BITS			((UBF_TIMESTAMP) stime->tm_mon + 1);
	*ut |= SET_UBF_TIMESTAMP_DAY_BITS			((UBF_TIMESTAMP) stime->tm_mday);
	*ut |= SET_UBF_TIMESTAMP_HOUR_BITS			((UBF_TIMESTAMP) stime->tm_hour);
	*ut |= SET_UBF_TIMESTAMP_MINUTE_BITS		((UBF_TIMESTAMP) stime->tm_min);
	*ut |= SET_UBF_TIMESTAMP_SECOND_BITS		((UBF_TIMESTAMP) stime->tm_sec);
	*ut |= SET_UBF_TIMESTAMP_MILLISECOND_BITS	((UBF_TIMESTAMP) ms);

	// Currently still unused.
	//*ut |= SET_UBF_TIMESTAMP_MICROSECOND_BITS (0);
}

UBF_TIMESTAMP SystemTime_UBF_TIMESTAMP (void)
{
	UBF_TIMESTAMP	dts;
	GetSystemTime_UBF_TIMESTAMP (&dts);
	return dts;
}

UBF_TIMESTAMP LocalTime_UBF_TIMESTAMP (void)
{
	UBF_TIMESTAMP	dts;
	GetLocalTime_UBF_TIMESTAMP (&dts);
	return dts;
}

/*
	A note about tests.

	Functions in this module (ubf_times) are tested by the test function in module ubf_date_and_time.c/h.

	Please do not place any code below. Please add code only above this comment block.
*/
/****************************************************************************************

	File:		ubf_date_and_time.c
	Why:		Functions for handling date and time.
	OS:			Windows NT Ver. 4.0/2000/XP/Vista/7/8...
	Author:		Thomas
	Created:	2016-11-02
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2016-11-02	Thomas			Created.
2017-08-29	Thomas			Function GetISO8601DateTimeStampT () added.
2018-04-06	Thomas			Some calculations taken from Logtext's GetCurrentDateTimeText.c.
2019-08-13	Thomas			Updated to use the C99 printf () format specifiers.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdio.h>
#include <inttypes.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./ubf_date_and_time.h"
	#include "./shortmonths.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./strintuint.h"
		#include "./ubfdebug.h"
		#include "./unref.h"
	#else
		#include "./../string/strintuint.h"
		#include "./../dbg/ubfdebug.h"
		#include "./../pre/unref.h"
	#endif

	#ifdef PLATFORM_IS_POSIX
		#include "./timespecfncts.h"
	#endif
#endif

void GetReadableTimeSpan			(
										char		*chBuf,
										size_t		stLen,
										ULONGLONG 	ullTime
									)
{
	ULONGLONG		ullDays;
	ULONGLONG		ullHours;
	ULONGLONG		ullMinutes;
	ULONGLONG		ullSeconds;
	ULONGLONG		ullMiSeconds;
	
	ubf_assert (chBuf != NULL);
	ubf_assert (stLen != 0);
	ullDays		 = ullTime / FT_DAY;
	ullTime		-= ullDays * FT_DAY;
	ullHours	 = ullTime / FT_HOUR;
	ullTime		-= ullHours * FT_HOUR;
	ullMinutes	 = ullTime / FT_MINUTE;
	ullTime		-= ullMinutes * FT_MINUTE;
	ullSeconds	 = ullTime / FT_SECOND;
	ullTime		-= ullSeconds * FT_SECOND;
	ullMiSeconds = ullTime / FT_MICROSECOND;
	// Example: "  7d 19:22:10.731322"
	snprintf (chBuf, stLen, "%" PRIu64 "d %2.2" PRIu64 ":%2.2" PRIu64 ":%2.2" PRIu64".%6.6" PRIu64,
			ullDays, ullHours, ullMinutes, ullSeconds, ullMiSeconds);
}

void GetReadableTimeSpanFILETIME	(
										char		*chBuf,
										size_t		stLen,
										FILETIME	ftTime
									)
{
	GetReadableTimeSpan (chBuf, stLen, ULONGLONGfromFILETIME (ftTime));
}

ULONGLONG GetSystemTimeAsULONGLONG (void)
{
	#ifdef OS_IS_ANDROID
		// See
		//	https://stackoverflow.com/questions/3832097/how-to-get-the-current-time-in-native-android-code .
		struct timespec		ts;
		
		clock_gettime (CLOCK_REALTIME, &ts);
		return ULONGLONG_from_timespec (&ts);
	#endif
	#ifdef OS_IS_IOS
		struct timespec		ts;
		
		clock_gettime (CLOCK_REALTIME, &ts);
		return ULONGLONG_from_timespec (&ts);
	#endif
	#ifdef OS_IS_LINUX
		struct timespec		ts;
		
		clock_gettime (CLOCK_REALTIME, &ts);
		return ULONGLONG_from_timespec (&ts);
	#endif
	#ifdef OS_IS_OSX
		struct timespec		ts;
		
		clock_gettime (CLOCK_REALTIME, &ts);
		return ULONGLONG_from_timespec (&ts);
	#endif
	#ifdef OS_IS_UNIX
		struct timespec		ts;
		
		clock_gettime (CLOCK_REALTIME, &ts);
		return ULONGLONG_from_timespec (&ts);
	#endif
	#ifdef OS_IS_WINDOWS
		// Windows has a single API GetSysteTimeAsFileTime () for this.
		//	See https://msdn.microsoft.com/en-us/library/windows/desktop/ms724397(v=vs.85).aspx .
		FILETIME		ft;

		GetSystemTimeAsFileTime (&ft);
		return ULONGLONGfromFILETIME(ft);
	#endif
}

ULONGLONG GetSystemTimeAsULONGLONGrel (void)
{
	#ifdef OS_IS_ANDROID
		// See
		//	https://stackoverflow.com/questions/3832097/how-to-get-the-current-time-in-native-android-code
		//	and http://man7.org/linux/man-pages/man2/clock_gettime.2.html .
		struct timespec		ts;
		
		clock_gettime (CLOCK_MONOTONIC_COARSE, &ts);
		return ULONGLONG_from_timespec_rel (&ts);
	#endif
	#ifdef OS_IS_IOS
		// See
		//	https://stackoverflow.com/questions/3832097/how-to-get-the-current-time-in-native-android-code
		//	and http://man7.org/linux/man-pages/man2/clock_gettime.2.html .
		struct timespec		ts;
		
		clock_gettime (CLOCK_MONOTONIC_COARSE, &ts);
		return ULONGLONG_from_timespec_rel (&ts);
	#endif
	#ifdef OS_IS_LINUX
		// See
		//	https://stackoverflow.com/questions/3832097/how-to-get-the-current-time-in-native-android-code
		//	and http://man7.org/linux/man-pages/man2/clock_gettime.2.html .
		struct timespec		ts;
		
		clock_gettime (CLOCK_MONOTONIC_COARSE, &ts);
		return ULONGLONG_from_timespec_rel (&ts);
	#endif
	#ifdef OS_IS_OSX
		// See
		//	https://stackoverflow.com/questions/3832097/how-to-get-the-current-time-in-native-android-code
		//	and http://man7.org/linux/man-pages/man2/clock_gettime.2.html .
		struct timespec		ts;
		
		clock_gettime (CLOCK_MONOTONIC_COARSE, &ts);
		return ULONGLONG_from_timespec_rel (&ts);
	#endif
	#ifdef OS_IS_UNIX
		// See
		//	https://stackoverflow.com/questions/3832097/how-to-get-the-current-time-in-native-android-code
		//	and http://man7.org/linux/man-pages/man2/clock_gettime.2.html .
		struct timespec		ts;
		
		clock_gettime (CLOCK_MONOTONIC_COARSE, &ts);
		return ULONGLONG_from_timespec_rel (&ts);
	#endif
	#ifdef OS_IS_WINDOWS
		// Windows has a single API GetSysteTimeAsFileTime () for this.
		//	See https://msdn.microsoft.com/en-us/library/windows/desktop/ms724397(v=vs.85).aspx .
		FILETIME		ft;

		GetSystemTimeAsFileTime (&ft);
		return ULONGLONGfromFILETIME(ft);
	#endif
}

#ifdef PLATFORM_IS_POSIX
	void GetSystemTimeAsFileTime (FILETIME *ft)
	{
		ubf_assert_non_NULL (ft);
	
		ULONGLONG ull = GetSystemTimeAsULONGLONG ();
		FILETIMEfromULONGLONG (*ft, ull);
	}
#endif

void SetFILETIMEtoZero (FILETIME *ft)
{
	ubf_assert_non_NULL (ft);
	
	ft->dwHighDateTime	= 0;
	ft->dwLowDateTime	= 0;
}

void SetFILETIMEtoMaxFILETIME (FILETIME *ft)
{
	ubf_assert_non_NULL (ft);
	
	ft->dwHighDateTime	= UINT32_MAX;
	ft->dwLowDateTime	= UINT32_MAX;
}

void cpyFILETIME (FILETIME *cunilog_restrict fttarget, FILETIME *cunilog_restrict ftsource)
{
	ubf_assert_non_NULL	(fttarget);
	ubf_assert_non_NULL	(ftsource);
	ubf_assert			(fttarget != ftsource);

	fttarget->dwHighDateTime	= ftsource->dwHighDateTime;
	fttarget->dwLowDateTime		= ftsource->dwLowDateTime;
}

int cmpFILETIME (FILETIME *cunilog_restrict ft1, FILETIME *cunilog_restrict ft2)
{
	ubf_assert_non_NULL	(ft1);
	ubf_assert_non_NULL	(ft2);
	ubf_assert			(ft1 != ft2);

	if (ft1->dwHighDateTime	> ft2->dwHighDateTime)
		return 1;
	if (ft1->dwHighDateTime	< ft2->dwHighDateTime)
		return -1;
	if (ft1->dwLowDateTime	> ft2->dwLowDateTime)
		return 1;
	if (ft1->dwLowDateTime	< ft2->dwLowDateTime)
		return -1;
	return 0;
}

bool isFileTimeNotZero (FILETIME *ft)
{
	ubf_assert_non_NULL (ft);
	
	return ft->dwLowDateTime || ft->dwHighDateTime;
}

bool isFileTimeZero (FILETIME *ft)
{
	ubf_assert_non_NULL (ft);
	
	return (!ft->dwLowDateTime) && (!ft->dwHighDateTime);
}

// Renamed. 2019-11-30, Thomas.
//void UnixTimeToFileTime (time_t t, LPFILETIME pft)
void FILETIMEfromUnixTimeT (FILETIME *pft, time_t t)
{	// https://support.microsoft.com/en-gb/kb/167296

	LONGLONG ll;												// Note that LONGLONG is
																//	a 64-bit value.

	//ubf_assert_msg (8 == sizeof (t), "FILETIMEfromUnixTimeT () - size of time_t is not 8 octets (64 bit");
	/*
	if (8 != sizeof (t))
	{	// This applies to Android.
		ubf_debug_puts ("FILETIMEfromUnixTimeT () - size of time_t is not 8 octets (64 bit)");
	}
	*/
	// See
	//	https://docs.microsoft.com/en-us/windows/win32/sysinfo/converting-a-time-t-value-to-a-file-time
	switch (sizeof (time_t))
	{
		case 4:	// time_t is 32 bit.
				//ubf_assert_msg (FALSE, "FILETIMEfromUnixTimeT () - size of time_t is 4 octets (32 bit)");
				//ubf_debug_puts ("FILETIMEfromUnixTimeT () - size of time_t is 4 octets (32 bit)");
				#ifdef Int32x32To64
					ll = Int32x32To64 (t, 10000000) + 116444736000000000;
				#else
					ll = (t * 10000000) + 116444736000000000;
				#endif
				pft->dwLowDateTime = (uint32_t) ll;
				pft->dwHighDateTime = (uint32_t) (ll >> 32);
				break;
		case 8: // time_t is 64 bit.
				ll = (t * 10000000) + 116444736000000000;
				pft->dwLowDateTime = (uint32_t) ll;
				pft->dwHighDateTime = (uint32_t) (ll >> 32);
				break;
		default:
				ubf_assert_msg (false, "FILETIMEfromUnixTimeT () - incompatible/unexptected size of time_t");
				break;
	}
}

void UnixTimeToSystemTime (time_t t, LPSYSTEMTIME pst)
{	// See https://msdn.microsoft.com/en-us/library/windows/desktop/ms724950(v=vs.85).aspx and
	//	http://www.cplusplus.com/reference/ctime/tm/ for a comparison of the two structures.

	#ifdef UBF_WINDOWS
		FILETIME ft;

		FILETIMEfromUnixTime (&ft, t);
		FileTimeToSystemTime (&ft, pst);
	#else
		// Everything else should be POSIX compatible.
		struct tm	tM;

		gmtime_r (&t, &tM);										// Won't adjust for local time.
		pst->wMilliseconds	= 0;								// Copy the members.
		pst->wSecond		= (uint16_t) tM.tm_sec;
		pst->wMinute		= (uint16_t) tM.tm_min;
		pst->wHour			= (uint16_t) tM.tm_hour;
		pst->wDay			= (uint16_t) tM.tm_mday;
		pst->wMonth			= (uint16_t) tM.tm_mon		+ 1;
		pst->wYear			= (uint16_t) tM.tm_year;
		pst->wDayOfWeek		= (uint16_t) tM.tm_wday;
	#endif
}

LONGLONG FileTime_to_POSIX (FILETIME ft)
{
	LARGE_INTEGER	date;
	LARGE_INTEGER	adjust;
	
	#ifdef UBF_WINDOWS
		date.HighPart	= (LONG) ft.dwHighDateTime;
		date.LowPart	= ft.dwLowDateTime;
	#else
		date.u.HighPart	= ft.dwHighDateTime;
		date.u.LowPart	= ft.dwLowDateTime;
	#endif
	// 100-nanoseconds = milliseconds * 10000
	adjust.QuadPart = 11644473600000 * 10000;
	// removes the diff between 1970 and 1601
	date.QuadPart -= adjust.QuadPart;
	// converts back from 100-nanoseconds to seconds
	return date.QuadPart / 10000000;
}

void GetISO8601Date (char *chISO8601Date)
{
	struct tm	tmRes;
	time_t		t;
	size_t		st;

	#ifdef _DEBUG
		int		i;

		// Write to the entire buffer to see if it fails with an access violation.
		for (i = 0; i < SIZ_ISO8601DATE; i ++)
		{
			chISO8601Date [i] = '\xFF';
		}
		chISO8601Date [SIZ_ISO8601DATE - 1] = '\xFF';
	#endif

	time (&t);													// Retrieves the times in UTC on Windows.
	localtime_r (&t, &tmRes);									// Adjust for local time.
	//gmtime_r (&t, &tmRes);									// Won't adjust for local time.
	st = strftime (chISO8601Date, SIZ_ISO8601DATE,
				"%Y-%m-%d", &tmRes);
	ubf_assert (st > 0 && st < SIZ_ISO8601DATE);
	UNUSED_PARAMETER (st);
}

void GetISO8601Week_s (char *chISO8601Week)
{
	struct tm		tmRes;
	time_t			t;
	unsigned int	week;
	uint32_t		newYear;

	time (&t);													// Retrieves the times in UTC on Windows.
	localtime_r (&t, &tmRes);									// Adjust for local time.
	week = GetISO8601WeekNumberFromDate (tmRes.tm_year + 1900, tmRes.tm_mon + 1, tmRes.tm_mday, &newYear);
	snprintf (chISO8601Week, SIZ_ISO8601YEARANDWEEK, "%4.4u-W%2.2u", (unsigned int) newYear, week);
}

void GetISO8601Week_c (char *chISO8601Week)
{
	struct tm		tmRes;
	time_t			t;
	unsigned int	week;
	uint32_t		newYear;

	time (&t);													// Retrieves the times in UTC on Windows.
	localtime_r (&t, &tmRes);									// Adjust for local time.
	week = GetISO8601WeekNumberFromDate (tmRes.tm_year + 1900, tmRes.tm_mon + 1, tmRes.tm_mday, &newYear);
	/*
	snprintf (chISO8601Week, SIZ_ISO8601YEARANDWEEK, "%4.4u-W%2.2u", (unsigned int) newYear, week);
	*/
	ubf_str0_from_uint16 (chISO8601Week, 4, (uint16_t) newYear);
	chISO8601Week += 4;
	*chISO8601Week ++ = '-';
	*chISO8601Week ++ = 'W';
	ubf_str0_from_59max (chISO8601Week, (uint8_t) week);
	chISO8601Week += 2;
	*chISO8601Week= '\0';
}

void GetISO8601DateTimeStamp (char *chISO8601DateTimeStamp)
{	// Retrieves the current date/time as a text in the
	// International Standard ISO 8601 format:
	// YYYY-MM-DD HH:MI:SS +/-TDIF
	//
	// Example: YYYY-MM-DD HH:MI:SS+01:00
	//          YYYY-MM-DD HH:MI:SS-04:00
	//
	// This code should work on Unix/Linux platforms and on Windows. See the definition of the
	//	gmtime_r () and localtime_r () macros in the header file. Windows only got the gmtime_s ()
	//	and localtime_s () APIs with swapped parameters.

	struct tm	tmRes;
	time_t		t;
	size_t		st;

	#ifdef _DEBUG
		int		i;

		// Write to the entire buffer to see if it fails with an access violation.
		for (i = 0; i < SIZ_ISO8601DATETIMESTAMP; i ++)
		{
			chISO8601DateTimeStamp [i] = '\xFF';
		}
		chISO8601DateTimeStamp [SIZ_ISO8601DATETIMESTAMP - 1] = '\xFF';
	#endif

	time (&t);													// Retrieves the times in UTC on Windows.
	localtime_r (&t, &tmRes);									// Adjust for local time.
	//gmtime_r (&t, &tmRes);									// Won't adjust for local time.
	st = strftime (chISO8601DateTimeStamp, SIZ_ISO8601DATETIMESTAMP,
				"%Y-%m-%d %H:%M:%S%z", &tmRes);
	/*	The correct ISO 8601 implementation requires a "T" between date and time. We're not using this
		 notation.
			st = strftime (chISO8601DateTimeStamp, SIZ_ISO8601DATETIMESTAMP,
						"%Y-%m-%dT%H:%M:%S%z", &tmRes);
	*/
	ubf_assert (st > 0 && st < SIZ_ISO8601DATETIMESTAMP);
	UNUSED_PARAMETER (st);
	// We got the offset as "-0000" or "+1000", but we want the offset with a colon between hours and minutes.
	chISO8601DateTimeStamp [24] = chISO8601DateTimeStamp [23];
	chISO8601DateTimeStamp [23] = chISO8601DateTimeStamp [22];
	chISO8601DateTimeStamp [22] = ':';
	chISO8601DateTimeStamp [25] = '\0';
	// PicoC (and probably other implementations too) produces a "-" instead of "+"
	//	when the offset is 0.
	if	(
			'0' == chISO8601DateTimeStamp [20]	&&
			'0' == chISO8601DateTimeStamp [21]	&&
			'0' == chISO8601DateTimeStamp [23]	&&
			'0' == chISO8601DateTimeStamp [24]	&&
			'-' == chISO8601DateTimeStamp [19]
		)
		chISO8601DateTimeStamp [19] = '+';
}

void GetISO8601DateTimeStampT (char *chISO8601DateTimeStamp)
{
	GetISO8601DateTimeStamp (chISO8601DateTimeStamp);
	chISO8601DateTimeStamp [10] = 'T';
}

#ifdef _MSC_VER
#pragma warning (disable: 4996)									// MSVC says some functions are unsafe.
#endif

void GetISO8601DateTimeStampMS (char *chISO8601DateTimeStampMS)
{
	time_t			t_time;
	struct	tm		*stime;
	int				i_adjustment_hours;
	int				i_adjustment_mins;
	unsigned short	ms;
	
	#ifdef PLATFORM_IS_WINDOWS
		struct	timeb	timebuffer;
		_ftime (&timebuffer);
		// Retrieve offset to UTC.
		//  The value is given in minutes by _ftime.
		// Some tests:
		// timebuffer.timezone = 200;
		// timebuffer.timezone = -200;
		// timebuffer.timezone = 60;
		// timebuffer.timezone = -60;
		i_adjustment_hours = timebuffer.timezone / 60;
		i_adjustment_mins  = timebuffer.timezone % 60;
		ms = timebuffer.millitm;
	#else
		struct timeval	tv;
		struct timezone	tz;
		gettimeofday (&tv, &tz);
		i_adjustment_hours	= tz.tz_minuteswest / 60;
		i_adjustment_mins	= tz.tz_minuteswest % 60;
		ms = tv.tv_usec + 500 / 1000;
	#endif

	// Unfortunately it is counted by moving westward, which
	//  is still not the proper notation. Turn it.
	if (i_adjustment_hours != 0)
		i_adjustment_hours-= (2 * i_adjustment_hours);

	if (i_adjustment_mins < 0)
		i_adjustment_mins-=  (2 * i_adjustment_mins);
	//printf ("Time offset: %2.2d:%2.2d\n", i_adjustment_hours, i_adjustment_mins);

	time (&t_time);
	stime = localtime (&t_time);
	
	// Adjust daylight saving time (DST) value.
	//  DST automatically implies +1 hour (+01:00).
	if (stime->tm_isdst > 0) i_adjustment_hours++;
    
	// Store the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss.mis+01:00
	sprintf (chISO8601DateTimeStampMS, "%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d.%3.3d+%2.2d:%2.2d",
    stime->tm_year + 1900,
	stime->tm_mon + 1,
    stime->tm_mday,
    stime->tm_hour,
    stime->tm_min,
	stime->tm_sec,
	ms,
	i_adjustment_hours,
	i_adjustment_mins);
}

#ifdef _MSC_VER
#pragma warning (default: 4996)									// MSVC says some functions are unsafe.
#endif

void GetISO8601DateTimeStampMST (char *chISO8601DateTimeStampMS)
{
	GetISO8601DateTimeStampMS (chISO8601DateTimeStampMS);
	chISO8601DateTimeStampMS [10] = 'T';
}

void ubf_get_system_time (FILETIME *ft)
{
	#ifdef OS_IS_ANDROID
		// Assuming that Android supports POSIX.
		time_t		t;

		time (&t);
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef OS_IS_IOS
		// Assuming that iOS supports POSIX.
		time_t		t;

		time (&t);
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef OS_IS_LINUX
		time_t		t;

		time (&t);
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef OS_IS_OSX
		// Assuming that OSX supports POSIX.
		time_t		t;

		time (&t);
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef OS_IS_UNIX
		time_t		t;

		time (&t);
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef OS_IS_WINDOWS
		/*	Windows has a single API GetSysteTimeAsFileTime () for this.
			See https://msdn.microsoft.com/en-us/library/windows/desktop/ms724397(v=vs.85).aspx .
		*/
		GetSystemTimeAsFileTime (ft);
	#endif
}

/* Maybe we don't need this function at all. 2018-12-30, Thomas.
void ubf_get_local_time (FILETIME *ft)
{	// This is still the system time on Linux/Unix platforms.
	#ifdef UBF_ANDROID
		// Assuming that Android supports POSIX.
		struct tm	tmRes;
		time_t		t;

		time (&t);
		localtime_r (&t, &tmRes);								// Adjust for local time.
		// Very old Android NDKs don't know this function.
		//t = timegm (&tmRes);									// Convert back by assuming we got UTC.
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef UBF_IOS
		// Assuming that iOS supports POSIX.
		struct tm	tmRes;
		time_t		t;

		time (&t);
		localtime_r (&t, &tmRes);								// Adjust for local time.
		t = timegm (&tmRes);									// Convert back by assuming we got UTC.
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef UBF_LINUX
        // Assuming that Linux can handle POSIX.
        struct tm	tmRes;
        time_t		t;

        time (&t);
        localtime_r (&t, &tmRes);								// Adjust for local time.
        t = timegm (&tmRes);									// Convert back by assuming we got UTC.
		FILETIMEfromUnixTimeT (ft, t);
    #endif
	#ifdef UBF_OSX
		// Assuming that OSX supports POSIX.
		struct tm	tmRes;
		time_t		t;

		time (&t);
		localtime_r (&t, &tmRes);								// Adjust for local time.
		t = timegm (&tmRes);									// Convert back by assuming we got UTC.
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef UBF_UNIX
		struct tm	tmRes;
		time_t		t;

		time (&t);
		localtime_r (&t, &tmRes);								// Adjust for local time.
		t = timegm (&tmRes);									// Convert back by assuming we got UTC.
		FILETIMEfromUnixTimeT (ft, t);
	#endif
	#ifdef UBF_WINDOWS
		SYSTEMTIME	st;
		
		GetLocalTime (&st);
		SystemTimeToFileTime (&st, ft);
	#endif
}
*/

void ubf_get_system_time_ULONGLONG (ULONGLONG *pulltime)
{
	ubf_assert (NULL != pulltime);
	*pulltime = GetSystemTimeAsULONGLONG ();
}

void ubf_get_system_time_ULONGLONG_rel (ULONGLONG *pulltime)
{
	ubf_assert (NULL != pulltime);
	*pulltime = GetSystemTimeAsULONGLONGrel ();
}

// This function is only required on Windows. See https://linux.die.net/man/3/clock_gettime
//	for more information. The function has been taken from
//	https://stackoverflow.com/questions/5404277/porting-clock-gettime-to-windows .
#ifdef OS_IS_WINDOWS
	int clock_gettime (int iclk_id, struct timespec *spec)
	{
		int64_t		wintime;
	
		#ifdef _DEBUG
			switch (iclk_id)
			{	// These are the clock types we support.
				case CLOCK_REALTIME:
				case CLOCK_MONOTONIC:
					break;
				default:
					ubf_assert (FALSE);
			}
		#else
			UNREFERENCED_PARAMETER (iclk_id);
		#endif
		GetSystemTimeAsFileTime ((FILETIME *) &wintime);
		wintime			-= 116444736000000000;					// 1601-01-01 to 1970-01-01.
		spec->tv_sec	= wintime / 10000000;					// Seconds.
		spec->tv_nsec	= wintime % 10000000 * 100;				// Nano seconds.
		return 0;												// Always return success.
	}
#endif

bool IsLeapYear (uint32_t uiYear)
{	// Implements (2) and (3). See below.

	/*
		Which one looks better or is easier to read/understand?

		return 0 == uiYear % 4 && uiYear % 100 || 0 == uiYear % 400;
		return 0 == uiYear % 400 ? true : 0 == uiYear % 100 ? false : 0 == uiYear % 4 ? true : false;
	*/
	return 0 == uiYear % 4 && uiYear % 100 || 0 == uiYear % 400;
}

// The lookup table for GetISO8601DayOfYear ().
static uint32_t m_Month [12] =
{
	0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
};

uint32_t GetISO8601DayOfYear (uint32_t Y, uint32_t M, uint32_t D)
{	// Implements (4). See below.
	uint32_t DayOfYearNumber;
	
	ubf_assert (0 < M);
	ubf_assert (13 > M);
	DayOfYearNumber = D + m_Month [M - 1];
	/*
	if (IsLeapYear (Y) && M > 2)
		DayOfYearNumber += 1;
	*/
	DayOfYearNumber += IsLeapYear (Y) && M > 2 ? 1 : 0;
	return DayOfYearNumber;
}

uint32_t GetISO8601Jan1WeekDay (uint32_t Y)
{	// Implements (5). See below.
	uint32_t YY, C, G;
	
	YY = (Y - 1) % 100;
	C = (Y - 1) - YY;
	G = YY + (YY / 4);
	return (1 + (((((C / 100) % 4) * 5) + G) % 7));
}

uint32_t GetISO8601WeekDay (uint32_t Y, uint32_t M, uint32_t D)
{	// Implements (6). See below. This function is not used.
	uint32_t	H;
	
	H = GetISO8601DayOfYear (Y, M, D) + (GetISO8601Jan1WeekDay (Y) - 1);
	return (1 + ((H -1) % 7));
}

uint32_t GetISO8601WeekNumberFromDate (uint32_t Y, uint32_t M, uint32_t D, uint32_t *aY)
{
/*
	Implementation of http://personal.ecu.edu/mccartyr/ISOwdALG.txt:

    Algorithm for Converting Gregorian Dates to ISO 8601 Week Date
                            (Y2K  Compliant)
                           Rick McCarty, 1999

                     From: Gregorian Year-Month-Day
                 To: ISO YearNumber-WeekNumber-Weekday 

ISO 8601 specifies that Week 01 of the year is the week containing
the first Thursday; Monday is Weekday 1, Sunday is Weekday 7;
WeekNumber requires two digits (W01, W02, etc.; "W" is optional)

Algorithm Conventions:
   "/" = integer division, discard remainder  (5/2 = 2)
   "%" = modulus, keep only remainder    (5%2 = 1)
   "&" = concatenation   ("W" & 12 = "W12")
   "!=" = unequal   (7 != 8  is true)
   "+=" = add right value to left variable,
             if F = 3, then (F += 4) yields F = 7
   "-=" = subtract right value from left variable

1. Convert input to Y M D
   Y = Year     (full specification; input 98 = year 0098)
		(Y must be larger than -1)
   M = Month    (1 through 12)
   D = Day      (1 through 31)
2. Find if Y is LeapYear
   if (Y % 4 = 0  and  Y % 100 != 0) or Y % 400 = 0
      then
         Y is LeapYear
      else
         Y is not LeapYear
3. Find if Y-1 is LeapYear
4. Find the DayOfYearNumber for Y M D
   Mnth[1] = 0    Mnth[4] = 90    Mnth[7] = 181   Mnth[10] = 273
   Mnth[2] = 31   Mnth[5] = 120   Mnth[8] = 212   Mnth[11] = 304
   Mnth[3] = 59   Mnth[6] = 151   Mnth[9] = 243   Mnth[12] = 334
   DayOfYearNumber = D + Mnth[M]
   if Y is LeapYear and M > 2
      then
         DayOfYearNumber += 1
5. Find the Jan1Weekday for Y (Monday=1, Sunday=7)
   YY = (Y-1) % 100
   C = (Y-1) - YY
   G = YY + YY/4
   Jan1Weekday = 1 + (((((C / 100) % 4) x 5) + G) % 7)
6. Find the Weekday for Y M D
   H = DayOfYearNumber + (Jan1Weekday - 1)
   Weekday = 1 + ((H -1) % 7)
7. Find if Y M D falls in YearNumber Y-1, WeekNumber 52 or 53
   if DayOfYearNumber <= (8-Jan1Weekday) and Jan1Weekday > 4
      then
         YearNumber = Y - 1 
         if Jan1Weekday = 5 or (Jan1Weekday = 6 and Y-1 is LeapYear)
            then
               WeekNumber = 53
            else 
               WeekNumber = 52
      else 
         YearNumber = Y
8. Find if Y M D falls in YearNumber Y+1, WeekNumber 1
   if YearNumber = Y
      then
         if Y is LeapYear
            then 
               I = 366
               else
               I = 365
         if (I - DayOfYearNumber) < (4 - Weekday)
            then
               YearNumber = Y + 1
               WeekNumber = 1
9. Find if Y M D falls in YearNumber Y, WeekNumber 1 through 53
   if YearNumber = Y
      then
         J = DayOfYearNumber + (7 - Weekday) + (Jan1Weekday -1)
         WeekNumber = J / 7
         if Jan1Weekday > 4
               WeekNumber -= 1
10. Output ISO Week Date:
   if WeekNumber < 10
      then
         WeekNumber = "0" & WeekNumber  (WeekNumber requires 2 digits)
   Print: YearNumber & "-" & WeekNumber & "-" & Weekday    (Optional: "-W" & WeekNumber)
*/

	bool		bLeapYear;										// Is Y a leapyear?
	bool		bPLeapYear;										// Is Y - 1 a leapyear?
	uint32_t	DayOfYearNumber;								// The number of the day of the year.
	uint32_t	Jan1Weekday;									// Which day is Jan 1?
	uint32_t	Weekday;										// The day of the week (Y, M, D).
	uint32_t	WeekNumber;										// The calendar week number.
	uint32_t	YearNumber;										// Y for calculations.
	long	I;													// How many days a year has.

	// (1) comes in as parameters Y, M, and D.

	// (2):
	bLeapYear = IsLeapYear (Y);

	// (3).
	bPLeapYear = IsLeapYear (Y - 1);
	
	// (4).
	DayOfYearNumber = GetISO8601DayOfYear (Y, M, D);

	// (5).
	Jan1Weekday = GetISO8601Jan1WeekDay (Y);

	// (6).
	Weekday = 1 + ((DayOfYearNumber + (Jan1Weekday - 1) - 1) % 7);

	// Disables the compiler warning that the variable is potentially uninitialised.
	WeekNumber = 0;

	// (7).
	if (DayOfYearNumber <= (8 - Jan1Weekday) && Jan1Weekday > 4)
	{
		YearNumber = Y - 1;
		if (Jan1Weekday == 5 || (Jan1Weekday == 6 && bPLeapYear))
			WeekNumber = 53;
		else 
			WeekNumber = 52;
	} else 
		YearNumber = Y;

	// (8).
	if (YearNumber == Y)
	{
		if (bLeapYear)
			I = 366;
		else
			I = 365;
		if (((int32_t) I - (int32_t) DayOfYearNumber) < ((int32_t) 4 - (int32_t) Weekday))
		{
			YearNumber = Y + 1;
			WeekNumber = 1;
		}
	}
	if (aY)						// Return the corrected (adjusted) year number so that
		*aY = YearNumber;		//  the caller can use it.
	// Note that this year number does not need to be identical to the default year
	//  number. It can be Y - 1.

	// (9).
	if (YearNumber == Y)
	{
		WeekNumber = (DayOfYearNumber + (7 - Weekday) + (Jan1Weekday - 1)) / 7;
		if (Jan1Weekday > 4)
			WeekNumber -= 1;
	}

	// (10).
	//  Not required in C.

	return WeekNumber;
}

bool HasMonth28Days (uint8_t m, uint32_t y)
{
	ubf_assert (m > 0);
	ubf_assert (m < 12 + 1);
	return 2 == m && !IsLeapYear (y) ? true : false;
}

bool HasMonth29Days (uint8_t m, uint32_t y)
{
	ubf_assert (m > 0);
	ubf_assert (m < 12 + 1);
	return 2 == m && IsLeapYear (y) ? true : false;
}

// The lookup table for HasMonth30Days ().
bool m_30days [] = { 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0 };

bool HasMonth30Days (uint8_t m)
{
	ubf_assert (m > 0);
	ubf_assert (m < 12 + 1);
	return m_30days [m - 1];
}

// The lookup table for HasMonth31Days ().
bool m_31days [] = { 1, 0, 1, 0, 1, 0, 1, 1, 0, 1, 0, 1 };

bool HasMonth31Days (uint8_t m)
{
	ubf_assert (m > 0);
	ubf_assert (m < 12 + 1);
	return m_30days [m - 1];
}

uint8_t DaysInMonth (uint8_t m, uint32_t y)
{
	return HasMonth28Days (m, y) ? 28 : (HasMonth30Days (m) ? 30 : 31);
}

int _DayOfWeek (int y, int m, int d)
{	// See https://stackoverflow.com/questions/6054016/c-program-to-find-day-of-week-given-date
	//	and
	//	https://en.wikipedia.org/wiki/Determination_of_the_day_of_the_week#Implementation-dependent_methods
	//	(Tomohiko Sakamoto).
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	
	ubf_assert (   0 < m);
	ubf_assert (  13 > m);
	ubf_assert (1752 > y);										// In the U.K.
	ubf_assert (   0 < d);
	ubf_assert (  32 > d);
	y -= m < 3;
	return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
}

int DayOfWeek (int y, int m, int d)
{
	int dow = _DayOfWeek (y, m, d);								// 0 == Sunday.
	return dow ? ++ dow : 6;									// 0 == Monday, 6 == Sunday.
}

void FILETIME_to_ISO8601 (char *chISO, FILETIME *ft)
{
	UNREFERENCED_PARAMETER (chISO);
	UNREFERENCED_PARAMETER (ft);
	ubf_assert (false);
}

bool IsBuildYearOrNewer_UBF_TIMESTAMP (UBF_TIMESTAMP *pts)
{
	ubf_assert (NULL != pts);

	UBF_TIMESTAMP	u	= *pts;
	bool			bRet;

	bRet =	UBF_TIMESTAMP_YEAR			(u) >= BuildYear_uint64 ();	// Year.
	bRet &=	UBF_TIMESTAMP_MONTH			(u) > 0;					// Month lowest.
	bRet &=	UBF_TIMESTAMP_MONTH			(u) < 13;					// Month highest.
	bRet &=	UBF_TIMESTAMP_DAY			(u) > 0;					// Day lowest.
	bRet &=	UBF_TIMESTAMP_DAY			(u) < 32;					// Day highest.
	bRet &=	UBF_TIMESTAMP_HOUR			(u) < 24;					// Hour highest.
	bRet &= UBF_TIMESTAMP_MINUTE		(u) < 60;					// Minute highest.
	bRet &= UBF_TIMESTAMP_SECOND		(u) < 60;					// Second highest.
	bRet &=	UBF_TIMESTAMP_MILLISECOND	(u) < 1000;					// Millisecond highest.
	bRet &=	UBF_TIMESTAMP_MICROSECOND	(u) < 1000;					// Microsecond highest.
	return bRet;
}

bool ValuesWithinLimits_SUBF_TIMESTRUCT (SUBF_TIMESTRUCT *ts)
{
	ubf_assert_non_NULL (ts);

	return	(
					ts->uYear	> 1999
				&&	ts->uYear	< 3023
				&&	ts->uMonth	> 0
				&&	ts->uMonth	< 13
				&&	ts->uDay	> 0
				&&	ts->uDay	< 32
				&&	ts->uMinute	< 60
				&&	ts->uHour	< 24
				&&	ts->uSecond	< 60
			);
}

bool UBF_TIMESTAMP_to_SUBF_TIMESTRUCT (SUBF_TIMESTRUCT *ts, UBF_TIMESTAMP t)
{
	ubf_assert_non_NULL (ts);

	ts->uOffsetHours	= UBF_TIMESTAMP_OFFSETHOURS (t);
	/*
	ts->uOffsetMinutes	= 0;
	if (t & 0x04)											// Bit 2 (0100).
		ts->uOffsetMinutes += 30;
	if (t & 0x02)											// Bit 1 (0010).
		ts->uOffsetMinutes += 15;
	*/
	ts->uOffsetMinutes	= UBF_TIMESTAMP_OFFSETMINUTES (t);
	ts->bOffsetNegative = UBF_TIMESTAMP_OFFSETNEGATIVE (t) ? true : false;
	ts->uYear			= UBF_TIMESTAMP_YEAR (t);
	ts->uMonth			= UBF_TIMESTAMP_MONTH (t);
	ts->uDay			= UBF_TIMESTAMP_DAY (t);
	ts->uHour			= UBF_TIMESTAMP_HOUR (t);
	ts->uMinute			= UBF_TIMESTAMP_MINUTE (t);
	ts->uSecond			= UBF_TIMESTAMP_SECOND (t);
	ts->uMillisecond	= UBF_TIMESTAMP_MILLISECOND (t);
	ts->uMicrosecond	= UBF_TIMESTAMP_MICROSECOND (t);
	return ValuesWithinLimits_SUBF_TIMESTRUCT (ts);
}

void SUBF_TIMESTRUCT_to_ISO8601 (char *chISO, SUBF_TIMESTRUCT *pts)
{
	ubf_assert (NULL != chISO);
	ubf_assert (NULL != pts);
	// Stores the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss.mis+01:00
	// YYYY-MM-DD HH:MI:SS.000+01:00
	snprintf	(
		chISO, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d.%3.3d+%2.2d:%2.2d",
		pts->uYear,
		pts->uMonth,
		pts->uDay,
		pts->uHour,
		pts->uMinute,
		pts->uSecond,
		pts->uMillisecond,
		pts->uOffsetHours,
		pts->uOffsetMinutes
				);
	if (pts->bOffsetNegative)									// Offset is negative.
		chISO [23] = '-';
}

/*
void UBF_TIMESTAMP_to_ISO8601 (char *chISO, UBF_TIMESTAMP ts)
{
	SUBF_TIMESTRUCT		t;

	ubf_assert (NULL != chISO);
	UBF_TIMESTAMP_to_SUBF_TIMESTRUCT (&t, ts);					// Convert the structure.
	SUBF_TIMESTRUCT_to_ISO8601 (chISO, &t);
}
*/

void ISO8601_from_UBF_TIMESTAMPs (char *chISO, UBF_TIMESTAMP ts)
{
	// Stores the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss.mis+01:00
	// YYYY-MM-DD HH:MI:SS.000+01:00
	snprintf	(
		chISO, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u.%3.3u+%2.2u:%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts),
		(unsigned int) UBF_TIMESTAMP_MINUTE (ts),
		(unsigned int) UBF_TIMESTAMP_SECOND (ts),
		(unsigned int) UBF_TIMESTAMP_MILLISECOND (ts),
		(unsigned int) UBF_TIMESTAMP_OFFSETHOURS (ts),
		(unsigned int) UBF_TIMESTAMP_OFFSETMINUTES (ts)
				);
	if (UBF_TIMESTAMP_OFFSETNEGATIVE (ts))						// Offset is negative.
		chISO [23] = '-';
}

void ISO8601_from_UBF_TIMESTAMPc (char *chISO, UBF_TIMESTAMP ts)
{
	// Stores the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss.mis+01:00
	// YYYY-MM-DD HH:MI:SS.000+01:00
	/*
	snprintf	(
		chISO, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2u %2.2u:%2.2u:%2.2u.%3.3u+%2.2u:%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts),
		(unsigned int) UBF_TIMESTAMP_MINUTE (ts),
		(unsigned int) UBF_TIMESTAMP_SECOND (ts),
		(unsigned int) UBF_TIMESTAMP_MILLISECOND (ts),
		(unsigned int) UBF_TIMESTAMP_OFFSETHOURS (ts),
		(unsigned int) UBF_TIMESTAMP_OFFSETMINUTES (ts)
				);
	*/

	ubf_str0_from_uint16 (chISO, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISO += 4;
	*chISO ++ = '-';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISO += 2;
	*chISO ++ = '-';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISO += 2;
	*chISO ++ = ' ';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	chISO += 2;
	*chISO ++ = ':';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_MINUTE (ts));
	chISO += 2;
	*chISO ++ = ':';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_SECOND (ts));
	chISO += 2;
	*chISO ++ = '.';
	ubf_str0_from_uint16 (chISO, 3, (unsigned int) UBF_TIMESTAMP_MILLISECOND (ts));
	chISO += 3;
	*chISO ++ = '+';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_OFFSETHOURS (ts));
	chISO += 2;
	*chISO ++ = ':';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_OFFSETMINUTES (ts));
	chISO += 2;
	*chISO = '\0';

	if (UBF_TIMESTAMP_OFFSETNEGATIVE (ts))						// Offset is negative.
		chISO [23] = '-';
}

void ISO8601T_from_UBF_TIMESTAMPs (char *chISO, UBF_TIMESTAMP ts)
{
	// Stores the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss.mis+01:00
	// YYYY-MM-DD HH:MI:SS.000+01:00
	snprintf	(
		chISO, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2uT%2.2u:%2.2u:%2.2u.%3.3u+%2.2u:%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts),
		(unsigned int) UBF_TIMESTAMP_MINUTE (ts),
		(unsigned int) UBF_TIMESTAMP_SECOND (ts),
		(unsigned int) UBF_TIMESTAMP_MILLISECOND (ts),
		(unsigned int) UBF_TIMESTAMP_OFFSETHOURS (ts),
		(unsigned int) UBF_TIMESTAMP_OFFSETMINUTES (ts)
				);
	if (UBF_TIMESTAMP_OFFSETNEGATIVE (ts))						// Offset is negative.
		chISO [23] = '-';
}

void ISO8601T_from_UBF_TIMESTAMPc (char *chISO, UBF_TIMESTAMP ts)
{
	// Stores the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss.mis+01:00
	// YYYY-MM-DD HH:MI:SS.000+01:00
	/*
	snprintf	(
		chISO, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2uT%2.2u:%2.2u:%2.2u.%3.3u+%2.2u:%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts),
		(unsigned int) UBF_TIMESTAMP_MINUTE (ts),
		(unsigned int) UBF_TIMESTAMP_SECOND (ts),
		(unsigned int) UBF_TIMESTAMP_MILLISECOND (ts),
		(unsigned int) UBF_TIMESTAMP_OFFSETHOURS (ts),
		(unsigned int) UBF_TIMESTAMP_OFFSETMINUTES (ts)
				);
	*/

	ubf_str0_from_uint16 (chISO, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISO += 4;
	*chISO ++ = '-';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISO += 2;
	*chISO ++ = '-';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISO += 2;
	*chISO ++ = 'T';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	chISO += 2;
	*chISO ++ = ':';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_MINUTE (ts));
	chISO += 2;
	*chISO ++ = ':';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_SECOND (ts));
	chISO += 2;
	*chISO ++ = '.';
	ubf_str0_from_uint16 (chISO, 3, (unsigned int) UBF_TIMESTAMP_MILLISECOND (ts));
	chISO += 3;
	*chISO ++ = '+';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_OFFSETHOURS (ts));
	chISO += 2;
	*chISO ++ = ':';
	ubf_str0_from_59max (chISO, (unsigned int) UBF_TIMESTAMP_OFFSETMINUTES (ts));
	chISO += 2;
	*chISO= '\0';

	if (UBF_TIMESTAMP_OFFSETNEGATIVE (ts))						// Offset is negative.
		chISO [23] = '-';
}

void ISO8601Date_from_UBF_TIMESTAMPs (char *chISODateOnly, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd
	// YYYY-MM-DD
	snprintf	(
		chISODateOnly, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts)
				);
}

void ISO8601Date_from_UBF_TIMESTAMPc (char *chISODateOnly, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd
	// YYYY-MM-DD
	/*
	snprintf	(
		chISODateOnly, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts)
				);
	*/
	ubf_str0_from_uint16 (chISODateOnly, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISODateOnly += 4;
	*chISODateOnly ++ = '-';
	ubf_str0_from_59max (chISODateOnly, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISODateOnly += 2;
	*chISODateOnly ++ = '-';
	ubf_str0_from_59max (chISODateOnly, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISODateOnly += 2;
	*chISODateOnly= '\0';
}

void ISO8601YearAndWeek_from_UBF_TIMESTAMPs (char *chISO8601YearAndWeek, UBF_TIMESTAMP ts)
{
	unsigned int	week;
	uint32_t		newYear;

	week = GetISO8601WeekNumberFromDate	(
		UBF_TIMESTAMP_YEAR (ts), UBF_TIMESTAMP_MONTH (ts), UBF_TIMESTAMP_DAY (ts), &newYear
										);
	snprintf (chISO8601YearAndWeek, SIZ_ISO8601YEARANDWEEK, "%4.4u-W%2.2u", (unsigned int) newYear, week);
}

void ISO8601YearAndWeek_from_UBF_TIMESTAMPc (char *chISO8601YearAndWeek, UBF_TIMESTAMP ts)
{
	unsigned int	week;
	uint32_t		newYear;

	week = GetISO8601WeekNumberFromDate	(
		UBF_TIMESTAMP_YEAR (ts), UBF_TIMESTAMP_MONTH (ts), UBF_TIMESTAMP_DAY (ts), &newYear
										);
	ubf_str0_from_uint16 (chISO8601YearAndWeek, 4, (uint16_t) newYear);
	chISO8601YearAndWeek += 4;
	*chISO8601YearAndWeek ++ = '-';
	*chISO8601YearAndWeek ++ = 'W';
	ubf_str0_from_59max (chISO8601YearAndWeek, (uint8_t) week);
	chISO8601YearAndWeek += 2;
	*chISO8601YearAndWeek= '\0';
}

void ISO8601DateAndHour_from_UBF_TIMESTAMPs (char *chISODateAndHour, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh
	// YYYY-MM-DD HH
	snprintf	(
		chISODateAndHour, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2u %2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts)
				);
}

void ISO8601DateAndHour_from_UBF_TIMESTAMPc (char *chISODateAndHour, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh
	// YYYY-MM-DD HH
	/*
	snprintf	(
		chISODateAndHour, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2u %2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts)
				);
	*/

	ubf_str0_from_uint16 (chISODateAndHour, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISODateAndHour += 4;
	*chISODateAndHour ++ = '-';
	ubf_str0_from_59max (chISODateAndHour, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISODateAndHour += 2;
	*chISODateAndHour ++ = '-';
	ubf_str0_from_59max (chISODateAndHour, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISODateAndHour += 2;
	*chISODateAndHour ++ = ' ';
	ubf_str0_from_59max (chISODateAndHour, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	chISODateAndHour += 2;
	*chISODateAndHour= '\0';
}

void ISO8601TDateAndHour_from_UBF_TIMESTAMPs (char *chISODateAndHour, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh
	// YYYY-MM-DD HH
	snprintf	(
		chISODateAndHour, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2uT%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts)
				);
}

void ISO8601TDateAndHour_from_UBF_TIMESTAMPc (char *chISODateAndHour, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh
	// YYYY-MM-DD HH

	ubf_str0_from_uint16 (chISODateAndHour, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISODateAndHour += 4;
	*chISODateAndHour ++ = '-';
	ubf_str0_from_59max (chISODateAndHour, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISODateAndHour += 2;
	*chISODateAndHour ++ = '-';
	ubf_str0_from_59max (chISODateAndHour, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISODateAndHour += 2;
	*chISODateAndHour ++ = 'T';
	ubf_str0_from_59max (chISODateAndHour, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	chISODateAndHour += 2;
	*chISODateAndHour= '\0';
}

void ISO8601DateHourAndMinute_from_UBF_TIMESTAMPs (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh:mm
	// YYYY-MM-DD HH:MM
	snprintf	(
		chISODateHourAndMinute, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2u %2.2u:%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts),
		(unsigned int) UBF_TIMESTAMP_MINUTE (ts)
				);
}

void ISO8601DateHourAndMinute_from_UBF_TIMESTAMPc (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	ubf_str0_from_uint16 (chISODateHourAndMinute, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISODateHourAndMinute += 4;
	*chISODateHourAndMinute ++ = '-';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = '-';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = ' ';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = ':';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_MINUTE (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute= '\0';
}

void ISO8601DateHourAndMinute_from_UBF_TIMESTAMP_ (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	ubf_str0_from_uint16 (chISODateHourAndMinute, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISODateHourAndMinute += 4;
	*chISODateHourAndMinute ++ = '-';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = '-';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = ' ';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = '_';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_MINUTE (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute= '\0';
}

void ISO8601TDateHourAndMinute_from_UBF_TIMESTAMPs (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh:mm
	// YYYY-MM-DD HH:MM
	snprintf	(
		chISODateHourAndMinute, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2uT%2.2u:%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts),
		(unsigned int) UBF_TIMESTAMP_MINUTE (ts)
				);
}

void ISO8601TDateHourAndMinute_from_UBF_TIMESTAMPc (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh:mm
	// YYYY-MM-DD HH:MM

	ubf_str0_from_uint16 (chISODateHourAndMinute, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISODateHourAndMinute += 4;
	*chISODateHourAndMinute ++ = '-';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = '-';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = 'T';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = ':';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_MINUTE (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute= '\0';
}

void ISO8601TDateHourAndMinute_from_UBF_TIMESTAMP_ (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	ubf_str0_from_uint16 (chISODateHourAndMinute, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISODateHourAndMinute += 4;
	*chISODateHourAndMinute ++ = '-';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = '-';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = 'T';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute ++ = '_';
	ubf_str0_from_59max (chISODateHourAndMinute, (unsigned int) UBF_TIMESTAMP_MINUTE (ts));
	chISODateHourAndMinute += 2;
	*chISODateHourAndMinute= '\0';
}

void ISO8601Year_from_UBF_TIMESTAMPs (char *chYear, UBF_TIMESTAMP ts)
{
	// YYYY
	snprintf	(
		chYear, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u", (unsigned int) UBF_TIMESTAMP_YEAR (ts)
				);
}

void ISO8601Year_from_UBF_TIMESTAMPc (char *chYear, UBF_TIMESTAMP ts)
{
	// YYYY
	ubf_str0_from_uint16 (chYear, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
}

void ISO8601YearAndMonth_from_UBF_TIMESTAMPs (char *chISOYearAndMonth, UBF_TIMESTAMP ts)
{
	// yyyy-mm
	// YYYY-MM
	snprintf	(
		chISOYearAndMonth, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts)
				);
}

void ISO8601YearAndMonth_from_UBF_TIMESTAMPc (char *chISOYearAndMonth, UBF_TIMESTAMP ts)
{
	// yyyy-mm
	// YYYY-MM
	/*
	snprintf	(
		chISOYearAndMonth, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts)
				);
	*/

	ubf_str0_from_uint16 (chISOYearAndMonth, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	chISOYearAndMonth += 4;
	*chISOYearAndMonth ++ = '-';
	ubf_str0_from_59max (chISOYearAndMonth, (unsigned int) UBF_TIMESTAMP_MONTH (ts));
	chISOYearAndMonth += 2;
	*chISOYearAndMonth= '\0';
}

void UBF_TIMESTAMP_to_ISO8601_Holocene (char *chISO, UBF_TIMESTAMP ts)
{
	SUBF_TIMESTRUCT		t;

	ubf_assert (NULL != chISO);
	UBF_TIMESTAMP_to_SUBF_TIMESTRUCT (&t, ts);					// Convert the structure.
	// Stores the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss.mis+01:00
	// YYYY-MM-DD HH:MI:SS.000+01:00
	snprintf	(
		chISO, SIZ_ISO8601DATETIMESTAMPMS_HOL,
		"%5.5d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d.%3.3d+%2.2d:%2.2d",
		t.uYear + 10000,
		t.uMonth,
		t.uDay,
		t.uHour,
		t.uMinute,
		t.uSecond,
		t.uMillisecond,
		t.uOffsetHours,
		t.uOffsetMinutes
				);
	if (t.bOffsetNegative)										// Offset is negative.
		chISO [24] = '-';
}

void SUBF_TIMESTRUCT_to_ISO8601_no_ms (char *chISO, SUBF_TIMESTRUCT *t)
{
	ubf_assert (NULL != chISO);
	// Stores the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss+01:00
	// YYYY-MM-DD HH:MI:SS+01:00
	snprintf	(
		chISO, SIZ_ISO8601DATETIMESTAMP,
		"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d+%2.2d:%2.2d",
		t->uYear,
		t->uMonth,
		t->uDay,
		t->uHour,
		t->uMinute,
		t->uSecond,
		t->uOffsetHours,
		t->uOffsetMinutes
				);
	if (t->bOffsetNegative)										// Offset is negative.
		chISO [19] = '-';
}

void UBF_TIMESTAMP_to_ISO8601_no_ms (char *chISO, UBF_TIMESTAMP ts)
{
	SUBF_TIMESTRUCT		t;

	ubf_assert (NULL != chISO);
	UBF_TIMESTAMP_to_SUBF_TIMESTRUCT (&t, ts);					// Convert the structure.
	SUBF_TIMESTRUCT_to_ISO8601_no_ms (chISO, &t);
}

void ISO8601_from_UBF_TIMESTAMP_no_ms_Holocene (char *chISO, UBF_TIMESTAMP ts)
{
	SUBF_TIMESTRUCT		t;

	ubf_assert (NULL != chISO);
	UBF_TIMESTAMP_to_SUBF_TIMESTRUCT (&t, ts);					// Convert the structure.
	// Stores the output for date/time including milliseconds.
	// yyyy-mm-dd hh:mi:ss+01:00
	// YYYY-MM-DD HH:MI:SS+01:00
	snprintf	(
		chISO, SIZ_ISO8601DATETIMESTAMP_HOL,
		"%4.4d-%2.2d-%2.2d %2.2d:%2.2d:%2.2d+%2.2d:%2.2d",
		t.uYear + 10000,
		t.uMonth,
		t.uDay,
		t.uHour,
		t.uMinute,
		t.uSecond,
		t.uOffsetHours,
		t.uOffsetMinutes
				);
	if (t.bOffsetNegative)										// Offset is negative.
		chISO [20] = '-';
}

void NCSADATETIME_from_UBF_TIMESTAMP (char *szncsadtim, UBF_TIMESTAMP ts)
{
	char *szOutp = szncsadtim;								// For debugging.
	UNUSED (szOutp);

	*szncsadtim ++ = '[';
	ubf_str0_from_59max (szncsadtim, (unsigned int) UBF_TIMESTAMP_DAY (ts));
	szncsadtim += 2;
	*szncsadtim ++ = '/';
	unsigned int uidxMnth = (unsigned int) UBF_TIMESTAMP_MONTH (ts);
	// Handle malformed bits.
	uidxMnth &= 0x3F;
	ubf_assert (12 > uidxMnth);
	uidxMnth = 12 <= uidxMnth ? 11 : uidxMnth;
	memcpy (szncsadtim, ccdtMnths [uidxMnth], 3);
	szncsadtim += 3;
	*szncsadtim ++ = '/';
	ubf_str0_from_uint16 (szncsadtim, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
	szncsadtim += 4;
	*szncsadtim ++ = ':';
	ubf_str0_from_59max (szncsadtim, (unsigned int) UBF_TIMESTAMP_HOUR (ts));
	szncsadtim += 2;
	*szncsadtim ++ = ':';
	ubf_str0_from_59max (szncsadtim, (unsigned int) UBF_TIMESTAMP_MINUTE (ts));
	szncsadtim += 2;
	*szncsadtim ++ = ':';
	ubf_str0_from_59max (szncsadtim, (unsigned int) UBF_TIMESTAMP_SECOND (ts));
	szncsadtim += 2;
	*szncsadtim ++ = ' ';
	*szncsadtim ++ = UBF_TIMESTAMP_OFFSETNEGATIVE (ts) ? '-' : '+';
	ubf_str0_from_59max (szncsadtim, (unsigned int) UBF_TIMESTAMP_OFFSETHOURS (ts));
	szncsadtim += 2;
	ubf_str0_from_59max (szncsadtim, (unsigned int) UBF_TIMESTAMP_OFFSETMINUTES (ts));
	szncsadtim += 2;
	*szncsadtim ++ = ']';
	*szncsadtim = '\0';
}

uint64_t MS_from_SUBF_TIMESTRUCT (SUBF_TIMESTRUCT *pts)
{
	uint64_t	uiRet;

	ubf_assert (NULL != pts);
	uiRet = pts->uMillisecond;
	uiRet += (uint64_t) pts->uSecond	* NUM_MS_IN_SECOND;
	uiRet += (uint64_t) pts->uMinute	* NUM_MS_IN_MINUTE;
	uiRet += (uint64_t) pts->uHour		* NUM_MS_IN_HOUR;
	return uiRet;
}

void GetLocalDateTime_ISO8601T_noMSnoOffs (char *szISO)
{
	time_t t_time;
	time (&t_time);
	struct tm *stime;
	stime = localtime (&t_time);
	
	// Store the output for date/time:
	// yyyy-mm-ddThh:mi:ss
	ubf_str_from_uint16 (szISO,			(uint16_t) stime->tm_year + 1900);
	szISO [4] = '-';
	ubf_str0_from_59max (szISO + 5,		(uint8_t) stime->tm_mon + 1);
	szISO [7] = '-';
	ubf_str0_from_59max (szISO + 8,		(uint8_t) stime->tm_mday);
	szISO [10] = 'T';
	ubf_str0_from_59max (szISO + 11,	(uint8_t) stime->tm_hour);
	szISO [13] = ':';
	ubf_str0_from_59max (szISO + 14,	(uint8_t) stime->tm_min);
	szISO [16] = ':';
	ubf_str0_from_59max (szISO + 17,	(uint8_t) stime->tm_sec);
}

void GetLocalDateTime_ISO8601T_noMSnoOffs_n (char *szISO)
{
	time_t t_time;
	time (&t_time);
	struct tm *stime;
	stime = localtime (&t_time);
	
	// Store the output for date/time:
	// yyyy-mm-ddThh:mi:ss
	ubf_str_from_uint16 (szISO,			(uint16_t) stime->tm_year + 1900);
	szISO [4] = '-';
	ubf_str0_from_59max (szISO + 5,		(uint8_t) stime->tm_mon + 1);
	szISO [7] = '-';
	ubf_str0_from_59max (szISO + 8,		(uint8_t) stime->tm_mday);
	szISO [10] = 'T';
	ubf_str0_from_59max (szISO + 11,	(uint8_t) stime->tm_hour);
	szISO [13] = ':';
	ubf_str0_from_59max (szISO + 14,	(uint8_t) stime->tm_min);
	szISO [16] = ':';
	ubf_str0_from_59max (szISO + 17,	(uint8_t) stime->tm_sec);
	//GetLocalDateTime_ISO8601T_noMSnoOffs (szISO);
	szISO [19] = '\0';
}

uint64_t BuildYear_uint64 (void)
{
	const char	*ccDate = __DATE__;
	uint64_t	u64year;

	ubf_uint64_from_str_n (&u64year, ccDate + 7, 4, enUintFromStrDontAllowPlus);
	return u64year;
}

uint16_t BuildYear_uint16 (void)
{
	return BuildYear_uint64 () & 0xFFFF;
}

bool is_datetimestampformat_l (const char *str, size_t len)
{
	size_t		ln	= USE_STRLEN == len ? strlen (str) : len;
	const char	*sz = str;

	if (4 <= ln)
	{
		// We require at least a year ("2025").
		if (!is_number_str_l (sz, 4))
			return false;
		ln -= 4;
		sz += 4;
		if (0 == ln)
			return true;

		// "2025-"
		if ('-' == sz [0])
		{
			++ sz;
			-- ln;
		}
		if (0 == ln)
			return true;
		if (2 <= ln)
		{
			//	"2025-07" or "202507"
			if (!is_number_str_l (sz, 2))
				return false;
			ln -= 2;
			sz += 2;
			if (0 == ln)
				return true;
			// "2025-07-"
			if ('-' == sz [0])
			{
				++ sz;
				-- ln;
			}
			if (0 == ln)
				return true;
			if (2 <= ln)
			{
				//	"2025-07-05" or "20250705"
				if (!is_number_str_l (sz, 2))
					return false;
				ln -= 2;
				sz += 2;
				if (0 == ln)
					return true;
				// Separator between date and time or 'T' required.
				if (' ' == sz [0] || 'T' == sz [0])
				{
					++ sz;
					-- ln;
				} else
					return false;
				if (0 == ln)
					return true;
				if (2 <= ln)
				{
					//	"2025-07-05 16" or "2025070516"
					if (!is_number_str_l (sz, 2))
						return false;
					ln -= 2;
					sz += 2;
					if (0 == ln)
						return true;
					if (':' == sz [0])
					{
						++ sz;
						-- ln;
					}
					if (0 == ln)
						return true;
					if (2 <= ln)
					{
						//	"2025-07-05 16:10" or "202507051610"
						if (!is_number_str_l (sz, 2))
							return false;
						ln -= 2;
						sz += 2;
						if (0 == ln)
							return true;
						if (':' == sz [0])
						{
							++ sz;
							-- ln;
						} else
							return false;
						if (0 == ln)
							return true;
						if (2 <= ln)
						{
							//	"2025-07-05 16:10:56" or "20250705161056"
							if (!is_number_str_l (sz, 2))
								return false;
							ln -= 2;
							sz += 2;
							if (0 == ln)
								return true;
							if ('.' == sz [0])
							{
								++ sz;
								-- ln;
							} else
								return false;
							if (0 == ln)
								return true;
							if (ln)
							{
								//	"2025-07-05 16:10:56.1"		or "202507051610561"
								//	"2025-07-05 16:10:56.12"	or "2025070516105612"
								//	"2025-07-05 16:10:56.123"	or "20250705161056123"
								if (!is_number_str_l (sz, 1))
									return false;
								-- ln;
								++ sz;
								if (0 == ln)
									return true;
								if (ln)
								{
									if (!is_number_str_l (sz, 1))
										return false;
									-- ln;
									++ sz;
									if (ln)
									{
										if (!is_number_str_l (sz, 1))
											return false;
										-- ln;
										++ sz;
									}
								}
								if (0 == ln)
									return true;
							}
						}
					}
				}
			}
		}
	}
	return false;
}

bool FormattedMilliseconds (char *chFormatted, const uint64_t uiTimeInMilliseconds)
{
	uint64_t	uHours;
	uint64_t	uMinutes;
	uint64_t	uSeconds;
	uint64_t	uMilliseconds;
	uint64_t	uH = 1000 * 3600;								// Hours.
	uint64_t	uM = 1000 * 60;									// Minutes.

	uHours			= uiTimeInMilliseconds / uH;
	uMilliseconds	= uiTimeInMilliseconds - (uHours * uH);
	uMinutes		= uMilliseconds / uM;
	uMilliseconds	-= uMinutes * uM;
	uSeconds		= uMilliseconds / 1000;
	uMilliseconds	-= uSeconds * 1000;
	//if (uHours <= 99 && uMinutes <= 59 && uSeconds <= 59 && uMilliseconds <= 999)
	if (uHours < 100)
	{
		//sprintf (chFormatted, "%2.2" PRIu64 ":%2.2" PRIu64 ":%2.2" PRIu64 ".%3.3" PRIu64,
		//	uHours, uMinutes, uSeconds, uMilliseconds);
		// This is faster:
		char	*ch		= chFormatted;
		ubf_str0_from_uint64 (ch, 2, uHours);
		ch += 2;
		ch [0] = ':';
		ch ++;
		ubf_str0_from_uint64 (ch, 2, uMinutes);
		ch += 2;
		ch [0] = ':';
		ch ++;
		ubf_str0_from_uint64 (ch, 2, uSeconds);
		ch += 2;
		ch [0] = '.';
		ch ++;
		ubf_str0_from_uint64 (ch, 3, uMilliseconds);
		return true;
	}
	return false;
}

#ifdef UBF_TIME_BUILD_UBF_TIMES_TEST_FUNCTION
	bool Test_ubf_times_functions (void)
	{
		char			ch [FORMATTEDMILLISECONDS_SIZE];
		UBF_DEF_GUARD_VAR (var, "0123456789");
		uint64_t		u;
		bool			b;
		UBF_TIMESTAMP	ut;
		UBF_TIMESTAMP	ut2;
		SUBF_TIMESTRUCT	ts;
		char			cOut [1024];

		b = true;

		// Some tests for obtaining the leap year.
		b &=  IsLeapYear (1600);
		b &= !IsLeapYear (1700);
		b &= !IsLeapYear (1800);
		b &= !IsLeapYear (1900);
		b &=  IsLeapYear (2000);
		b &=  IsLeapYear (2016);
		b &=  IsLeapYear (2020);
		b &= !IsLeapYear (2021);
		b &= !IsLeapYear (2022);
		b &= !IsLeapYear (2023);
		b &=  IsLeapYear (2024);
		b &=  IsLeapYear (2028);
		b &=  IsLeapYear (2032);
		b &= !IsLeapYear (2033);
		b &= !IsLeapYear (2034);
		b &= !IsLeapYear (2033);
		b &=  IsLeapYear (2036);
		b &= !IsLeapYear (2037);
		b &= !IsLeapYear (2038);
		b &= !IsLeapYear (2039);
		b &=  IsLeapYear (2040);
		b &=  IsLeapYear (2052);
		b &=  IsLeapYear (2056);
		b &=  IsLeapYear (2060);
		b &=  IsLeapYear (2096);
		b &= !IsLeapYear (2097);
		b &= !IsLeapYear (2099);
		b &= !IsLeapYear (2100);
		b &= !IsLeapYear (2101);
		b &=  IsLeapYear (2104);
		b &= !IsLeapYear (2200);
		b &= !IsLeapYear (2300);
		b &=  IsLeapYear (2400);

		GetSystemTime_UBF_TIMESTAMP (&ut);
		b &= IsBuildYearOrNewer_UBF_TIMESTAMP (&ut);
		ubf_assert (b);

		u = 1000;
		while (u --)
		{
			GetSystemTime_UBF_TIMESTAMP (&ut);
			SUBF_TIMESTRUCT_from_UBF_TIMESTAMP (&ts, ut);
			UBF_TIMESTAMP_from_SUBF_TIMESTRUCT (&ut2, &ts);
			b &= ut == ut2;
			ubf_assert (b);
			GetLocalTime_UBF_TIMESTAMP (&ut);
			SUBF_TIMESTRUCT_from_UBF_TIMESTAMP (&ts, ut);
			UBF_TIMESTAMP_from_SUBF_TIMESTRUCT (&ut2, &ts);
			b &= ut == ut2;
			ubf_assert (b);
		}

		// Create our own timestamp.
		//	2022-11-04 16:20:07.400
		ts.uYear				= 2022;
		ts.uMonth				= 11;
		ts.uDay					= 4;
		ts.uHour				= 16;
		ts.uMinute				= 20;
		ts.uSecond				= 7;
		ts.uMillisecond			= 400;
		ts.uMicrosecond			= 500;
		ts.uOffsetHours			= 8;
		ts.uOffsetMinutes		= 30;
		ts.bOffsetNegative		= false;
		//ASSERT (false);
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		// 84h = 8 for uOffsetHours, 4 for 30 offset minutes.
		b &= 0x84 == ut;
		ubf_assert (0x84 == ut);
		ts.uOffsetHours			= 1;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		// 14h = 1 for uOffsetHours, 4 for 30 offset minutes.
		b &= 0x14 == ut;
		ubf_assert (b);
		u = UBF_TIMESTAMP_OFFSETMINUTES (ut);
		b &= 30 == u;
		ubf_assert (30 == u);
		ts.uOffsetHours			= 0;
		ts.uOffsetMinutes		= 45;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		u = UBF_TIMESTAMP_OFFSETMINUTES (ut);
		b &= 45 == u;
		ubf_assert (45 == u);
		ts.uOffsetMinutes		= 15;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		u = UBF_TIMESTAMP_OFFSETMINUTES (ut);
		b &= 15 == u;
		ubf_assert (15 == u);
		ts.uOffsetMinutes		= 0;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		u = UBF_TIMESTAMP_OFFSETMINUTES (ut);
		b &= 0 == u;
		ubf_assert (0 == u);

		ts.uOffsetHours			= 8;
		ts.uOffsetMinutes		= 30;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		ut |= UBF_TIMESTAMP_YEAR_BITS (&ts);
		ut |= UBF_TIMESTAMP_MONTH_BITS (&ts);
		ut |= UBF_TIMESTAMP_DAY_BITS (&ts);
		ut |= UBF_TIMESTAMP_HOUR_BITS (&ts);
		ut |= UBF_TIMESTAMP_MINUTE_BITS (&ts);
		ut |= UBF_TIMESTAMP_SECOND_BITS (&ts);
		ut |= UBF_TIMESTAMP_MILLISECOND_BITS (&ts);
		ut |= UBF_TIMESTAMP_MICROSECOND_BITS (&ts);
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		uint32_t uYear;
		uint32_t uWeek = GetISO8601WeekNumberFromDate (ts.uYear, ts.uMonth, ts.uDay, &uYear);
		UNUSED_PARAMETER (uWeek);
		b &= 44 == uWeek;
		b &= 2022 == uYear;
		ubf_assert (b);
		ubf_assert (44 == uWeek);
		ubf_assert (2022 == uYear);
		ISO8601YearAndWeek_from_UBF_TIMESTAMP (cOut, ut);
		b &= !memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK);
		ubf_assert (b);

		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601Year_from_UBF_TIMESTAMPs (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601Year_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601DateAndHour_from_UBF_TIMESTAMP (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11-04 16", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601DateAndHour_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11-04 16", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601_from_UBF_TIMESTAMP (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11-04 16:20:07.400+08:30", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11-04 16:20:07.400+08:30", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndWeek_from_UBF_TIMESTAMP (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndWeek_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndMonth_from_UBF_TIMESTAMPs (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11", SIZ_ISO8601YEARANDMONTH));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndMonth_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11", SIZ_ISO8601YEARANDMONTH));

		// The same again but with a negative offset of 45 minutes.
		ts.uOffsetHours			= 8;
		ts.uOffsetMinutes		= 45;
		ts.bOffsetNegative		= true;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		ut |= UBF_TIMESTAMP_YEAR_BITS (&ts);
		ut |= UBF_TIMESTAMP_MONTH_BITS (&ts);
		ut |= UBF_TIMESTAMP_DAY_BITS (&ts);
		ut |= UBF_TIMESTAMP_HOUR_BITS (&ts);
		ut |= UBF_TIMESTAMP_MINUTE_BITS (&ts);
		ut |= UBF_TIMESTAMP_SECOND_BITS (&ts);
		ut |= UBF_TIMESTAMP_MILLISECOND_BITS (&ts);
		ut |= UBF_TIMESTAMP_MICROSECOND_BITS (&ts);
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601Year_from_UBF_TIMESTAMPs (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601Year_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601DateAndHour_from_UBF_TIMESTAMP (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11-04 16", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601DateAndHour_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11-04 16", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601_from_UBF_TIMESTAMP (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11-04 16:20:07.400-08:45", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-11-04 16:20:07.400-08:45", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndWeek_from_UBF_TIMESTAMP (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndWeek_from_UBF_TIMESTAMPc (cOut, ut);
		ubf_expect_bool_AND (b, !memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));

		u = 0;
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "00:00:00.000", FORMATTEDMILLISECONDS_SIZE));
		u = 500;
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "00:00:00.500", FORMATTEDMILLISECONDS_SIZE));
		u = 999;
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "00:00:00.999", FORMATTEDMILLISECONDS_SIZE));
		u = 1999;
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "00:00:01.999", FORMATTEDMILLISECONDS_SIZE));
		u = 59999;
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "00:00:59.999", FORMATTEDMILLISECONDS_SIZE));
		u = 50 * 1000 * 3600;
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "50:00:00.000", FORMATTEDMILLISECONDS_SIZE));
		u = 99 * 1000 * 3600;
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "99:00:00.000", FORMATTEDMILLISECONDS_SIZE));
		u = (99 * 1000 * 3600) + (1000 * 59);
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "99:00:59.000", FORMATTEDMILLISECONDS_SIZE));
		u = (99 * 1000 * 3600) + (1000 * 59 * 60) + (1000 * 59);
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "99:59:59.000", FORMATTEDMILLISECONDS_SIZE));
		u = (99 * 1000 * 3600) + (1000 * 59 * 60) + (1000 * 59) + 999;
		b &= FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_expect_bool_AND (b, !memcmp (ch, "99:59:59.999", FORMATTEDMILLISECONDS_SIZE));
		// Should cause an overflow and lead to a return value of FALSE.
		u = (99 * 1000 * 3600) + (1000 * 59 * 60) + (1000 * 59) + 999 + 1;
		b &= !FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		// Should not have been touched by the function due to it returning FALSE.
		ubf_expect_bool_AND (b, !memcmp (ch, "99:59:59.999", FORMATTEDMILLISECONDS_SIZE));

		// Test bit constants.
		UBF_TIMESTAMP	stamp;
		UBF_TIMESTAMP	maskd;
		GetLocalTime_UBF_TIMESTAMP (&stamp);
		uint64_t uio1 = UBF_TIMESTAMP_OFFSETMINUTES (stamp);
		uint64_t uio2 = UBF_TIMESTAMP_OFFSETMINUTES (stamp & UBF_TIMESTAMP_KEEP_OFFSET_BITS);
		ubf_expect_bool_AND (b, uio1 == uio2);
		uio1 = UBF_TIMESTAMP_OFFSETHOURS (stamp);
		uio2 = UBF_TIMESTAMP_OFFSETHOURS (stamp & UBF_TIMESTAMP_KEEP_OFFSET_BITS);
		ubf_expect_bool_AND (b, uio1 == uio2);
		uio1 = UBF_TIMESTAMP_SECOND (stamp);
		uio2 = UBF_TIMESTAMP_SECOND (stamp & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS);
		ubf_expect_bool_AND (b, uio1 == uio2);
		uio1 = UBF_TIMESTAMP_MINUTE (stamp);
		uio2 = UBF_TIMESTAMP_MINUTE (stamp & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS);
		ubf_expect_bool_AND (b, uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS;
		ubf_expect_bool_AND (b, 0 == UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		uio1 = UBF_TIMESTAMP_HOUR (stamp);
		uio2 = UBF_TIMESTAMP_HOUR (stamp & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS);
		ubf_expect_bool_AND (b, uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS;
		ubf_expect_bool_AND (b, 0 == UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_expect_bool_AND (b, 0 == UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		uio1 = UBF_TIMESTAMP_DAY (stamp);
		uio2 = UBF_TIMESTAMP_DAY (stamp & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS);
		ubf_expect_bool_AND (b, uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS;
		ubf_assert_0 (UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_HOUR (maskd & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS));
		uio1 = UBF_TIMESTAMP_MONTH (stamp);
		uio2 = UBF_TIMESTAMP_MONTH (stamp & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS);
		ubf_expect_bool_AND (b, uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS;
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_HOUR (maskd & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_DAY (maskd & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS));
		uio1 = UBF_TIMESTAMP_YEAR (stamp);
		uio2 = UBF_TIMESTAMP_YEAR (stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS);
		ubf_assert (uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS;
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_HOUR (maskd & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_DAY (maskd & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS));
		ubf_expect_bool_AND_0 (b, UBF_TIMESTAMP_MONTH (maskd & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS));

		char sz1 [1024];
		char sz2 [1024];

		GetISO8601Week_s (sz1);
		GetISO8601Week_c (sz2);
		if (memcmp (sz1, sz2, SIZ_ISO8601WEEK))
		{	// We assume the test ran just the moment the week number changed.
			GetISO8601Week_s (sz1);
			GetISO8601Week_c (sz2);
			b &= !memcmp (sz1, sz2, SIZ_ISO8601WEEK);
			ubf_assert (!memcmp (sz1, sz2, SIZ_ISO8601WEEK));
		} else
		{
			b &= !memcmp (sz1, sz2, SIZ_ISO8601WEEK);
			ubf_assert (!memcmp (sz1, sz2, SIZ_ISO8601WEEK));
		}

		UBF_TIMESTAMP	tt;
		ULONGLONG		u1;
		ULONGLONG		u2;
		UNUSED_PARAMETER (u2);
		UNUSED (u1);

		tt = LocalTime_UBF_TIMESTAMP ();
		u1 = GetSystemTimeAsULONGLONG ();

		char szTS1 [SIZ_ISO8601DATETIMESTAMPMS];
		char szTS2 [SIZ_ISO8601DATETIMESTAMPMS];
		
		ISO8601_from_UBF_TIMESTAMP	(szTS1, tt);
		ISO8601_from_UBF_TIMESTAMPc	(szTS2, tt);
		ubf_expect_bool_AND (b, !memcmp (szTS1, szTS2, SIZ_ISO8601DATETIMESTAMPMS));

		tt = LocalTime_UBF_TIMESTAMP ();
		ISO8601T_from_UBF_TIMESTAMP  (szTS1, tt);
		ISO8601T_from_UBF_TIMESTAMPc (szTS2, tt);
		ubf_expect_bool_AND (b, !memcmp (szTS1, szTS2, SIZ_ISO8601DATETIMESTAMPMS));

		char szISO_1 [SIZ_ISO8601DATETIMESTAMP_NO_OFFS];
		char szISO_2 [SIZ_ISO8601DATETIMESTAMP_NO_OFFS];
		szISO_1 [LEN_ISO8601DATETIMESTAMP_NO_OFFS] = 'X';
		GetLocalDateTime_ISO8601T_noMSnoOffs	(szISO_1);
		GetLocalDateTime_ISO8601T_noMSnoOffs_n	(szISO_2);
		if (memcmp (szISO_1, szISO_2, LEN_ISO8601DATETIMESTAMP_NO_OFFS))
		{	// Hit during a second change.
			GetLocalDateTime_ISO8601T_noMSnoOffs	(szISO_1);
			GetLocalDateTime_ISO8601T_noMSnoOffs_n	(szISO_2);
		}
		ubf_expect_bool_AND_0 (b, memcmp (szISO_1, szISO_2, LEN_ISO8601DATETIMESTAMP_NO_OFFS));
		ubf_expect_bool_AND (b, 'X'		== szISO_1 [LEN_ISO8601DATETIMESTAMP_NO_OFFS]);
		ubf_expect_bool_AND (b, '\0'	== szISO_2 [LEN_ISO8601DATETIMESTAMP_NO_OFFS]);

		// Test the NCSA datetime stamp. We're not really testing here. We only
		//	check for a buffer overrun.
		char ctncsa [SIZ_NCSA_COMMON_LOG_DATETIME + 1];
		ctncsa [SIZ_NCSA_COMMON_LOG_DATETIME] = '#';
		NCSADATETIME_from_UBF_TIMESTAMP (ctncsa, GetSystemTimeAsUBF_TIMESTAMP ());
		ubf_expect_bool_AND (b, ASCII_NUL == ctncsa [LEN_NCSA_COMMON_LOG_DATETIME]);
		ubf_expect_bool_AND (b, '#' == ctncsa [SIZ_NCSA_COMMON_LOG_DATETIME]);

		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("", 0));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("1", 1));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("12", 1));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("12", 2));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("123", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("1234", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("0000-0", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 ", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00T", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00T00", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00:", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("0000-00-00 00:0", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00:00", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00:00:", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("0000-00-00 00:00:0", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00:00:00", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00:00:00.", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00:00:00.1", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00:00:00.12", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00 00:00:00.123", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("0000-00-00 00:00:00.1 ", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("0000-00-00 00:00:00.12 ", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("0000-00-00 00:00:00.123 ", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("0000-00-00T00:00:00.123", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("0000-00-00T00:00:00.123 ", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("0000-00-00T00:00:00.1 23 ", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("9999", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("999911", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("9999111", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("99991122", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("99991122 16", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122x16", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("9999112216", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122 16 ", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("99991122 16:", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122 16:0", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("99991122 16:00", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("99991122 16:00:", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122 16:00:0", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("99991122 16:00:00", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("99991122T16:00:00", USE_STRLEN));
		ubf_expect_bool_AND (b, is_datetimestampformat_l ("99991122T16:00:00.123", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122T16:00:00x123", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122T16:00:00123", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122T160000123", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122T16 00 00 123", USE_STRLEN));
		ubf_expect_bool_AND (b, !is_datetimestampformat_l ("99991122T16.00.00.123", USE_STRLEN));

		// Timings.
		/*
			We found that the psx function is twice as fast as the win version.
		*/
		/*
		u1 = GetSystemTimeAsULONGLONG ();
		unsigned int ui;
		for (ui = 0; ui < 10000000; ++ ui)
		{
			tt = LocalTime_UBF_TIMESTAMP ();
		}
		u2 = GetSystemTimeAsULONGLONG ();
		char timespan [128];
		GetReadableTimeSpan (timespan, 128, u2 - u1);
		puts (timespan);
		puts ("");
		*/

		//UBF_TIMESTAMP	tt;
		//ULONGLONG		u1;
		//ULONGLONG		u2;

		/*
		// Timing.
		unsigned int ui;
		unsigned int uiIts = 1000000000;
		uiIts = 100000000;
		//unsigned int uiIts = 100;
		char timespan [128];

		u1 = GetSystemTimeAsULONGLONG ();
		for (ui = 0; ui < uiIts; ++ ui)
		{
			ISO8601_from_UBF_TIMESTAMP_s	(szTS1, tt);
		}
		u2 = GetSystemTimeAsULONGLONG ();
		GetReadableTimeSpan (timespan, 128, u2 - u1);
		puts (timespan);
		puts ("");

		u1 = GetSystemTimeAsULONGLONG ();
		for (ui = 0; ui < uiIts; ++ ui)
		{
			ISO8601_from_UBF_TIMESTAMP_c	(szTS1, tt);
		}
		u2 = GetSystemTimeAsULONGLONG ();
		GetReadableTimeSpan (timespan, 128, u2 - u1);
		puts (timespan);
		puts ("");
		*/

		/*
			This takes around 1:30, which is 90 seconds, i.e. around 90 ns per iteration.
			This test was to find out whether it's worth using ns for the timestamp.

		UBF_TIMESTAMP tsp;
		UBF_TIMESTAMP t2 = 0;
		uint64_t uits1 = 1000000000;
		uint64_t ux;
		uits1 = 1000000000;
		//unsigned int uiIts = 100;
		char timespan [128];

		u1 = GetSystemTimeAsULONGLONG ();
		for (ux = 0; ux < uits1; ++ ux)
		{
			GetLocalTime_UBF_TIMESTAMP (&tsp);
			t2 += tsp;
		}
		u2 = GetSystemTimeAsULONGLONG ();
		GetReadableTimeSpan (timespan, 128, u2 - u1);
		puts (timespan);
		puts ("");
		printf ("%" PRId64, t2);
		exit (0);
		*/

		return b;
	}
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4996)									// MSVC says some sprintf is unsafe.
#endif
/****************************************************************************************

	File:		timespecfncts.c
	Why:		Functions and macros that deal with struct timespec.
	OS:			POSIX/C99.
	Author:		Thomas
	Created:	2019-12-10
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2019-12-10	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./timespecfncts.h"
	#include "./ubf_times.h"
	#include "./ubf_date_and_time.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./../dbg/ubfdebug.h"
	#else
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

/*
	The differences between the timeval and timespec structures:
	
	struct timeval {
		long    tv_sec;
		long    tv_usec;										// Microseconds.
	};
	
	struct timespec {
		long    tv_sec;
		long    tv_nsec;										// Nanoseconds.
	};
*/

void timespec_from_ULONGLONG (struct timespec *ts, ULONGLONG ullDiff)
{
	ULONGLONG	ull;
	
	ubf_assert (NULL != ts);
	ull = ullDiff % FT_SECOND;
	// Everything smaller than a second is now in ull.
	//	We need the value in nanosecons (ns). 1 us = 1000 ns.
	//	FILETIME stores the amount in 100 ns intervals.
	ts->tv_nsec	= (long) (ull * 100);
	// The seconds go to the seconds member.
	ts->tv_sec	= (long long) ullDiff / (long long) FT_SECOND;
}

ULONGLONG ULONGLONG_from_timespec (struct timespec *ts)
{
	ULONGLONG	ull;
	
	ubf_assert (NULL != ts);
	ull =	(ULONGLONG) ts->tv_sec * FT_SECOND;
	ull +=	(ULONGLONG) ts->tv_nsec / FT_D_NANOSECOND;
	ull +=	116444736000000000;
	return ull;
}

ULONGLONG ULONGLONG_from_timespec_rel (struct timespec *ts)
{
	ULONGLONG	ull;
	
	ubf_assert (NULL != ts);
	ull =	(ULONGLONG) ts->tv_sec * FT_SECOND;
	ull +=	(ULONGLONG) ts->tv_nsec / FT_D_NANOSECOND;
	return ull;
}

bool timespec_substract_ULONGLONG (struct timespec *ts, ULONGLONG ullDiff)
{
	struct timespec		pv;
	bool				bFits	= true;
	
	timespec_from_ULONGLONG (&pv, ullDiff);
	if (ts->tv_sec >= pv.tv_sec)
	{
		ts->tv_sec -= pv.tv_sec;
	} else
	{
		ts->tv_sec = 0;
		bFits = false;
	}
	if (ts->tv_nsec >= pv.tv_nsec)
	{
		ts->tv_nsec -= pv.tv_nsec;
	} else
	{
		ts->tv_nsec = 0;
		bFits = false;
	}
	return bFits;
}

#ifdef TIMESPECFNCTS_BUILD_TEST
	bool timespec_testfunction (void)
	{
		struct timespec		ts		= TIMESPEC_INIT_ZERO;
		ULONGLONG			ul1;
		ULONGLONG			ul2;
		bool				bRet	= false;
		
		ubf_assert (0 == ts.tv_sec);
		ubf_assert (0 == ts.tv_nsec);
		
		ubf_get_system_time_ULONGLONG (&ul1);
		Sleep_s (1);											// 1 second.
		ubf_get_system_time_ULONGLONG (&ul2);
		ubf_assert (ul2 > ul1);
		timespec_from_ULONGLONG (&ts, ul2 - ul1);
		ubf_assert (1		== ts.tv_sec);						// Should have 1 second.
		ubf_assert (20000000	> ts.tv_nsec);					// Will most likely have
																//	something between 20 000 000
																//	and 0.
		ubf_get_system_time_ULONGLONG (&ul1);
		Sleep_ms (500);											// 500 ms.
		ubf_get_system_time_ULONGLONG (&ul2);
		ubf_assert (ul2 > ul1);
		timespec_from_ULONGLONG (&ts, ul2 - ul1);
		ubf_assert (0		== ts.tv_sec);						// Should have 0 seconds.
		ubf_assert (600000000	> ts.tv_nsec);					// Will most likely be
																//	something between 600000000
																//	and 0, depending on the
																//	granularity. On Windows
																//	it's a bit more than
																//	500 million
																//	(500000000).
		return bRet;
	}
#endif
/****************************************************************************************

	File		ISO__DATE__.c
	Why:		Predefined macros __DATE__ and __TIME__ in ISO 8601 format.
	OS:			C99
	Created:	2024-10-29

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-10-29	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./ISO__DATE__.h"
	#include "./shortmonths.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfmem.h"
		#include "./memstrstr.h"
		#include "./Warnings.h"
		#include "./ubfdebug.h"
	#else
		#include "./../mem/ubfmem.h"
		#include "./../mem/memstrstr.h"
		#include "./../pre/Warnings.h"
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

// This is what the preprocessor gives us. See
//	https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html and
//	https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170 .
//	__DATE__: "Jan 14 2012".
//	__TIME__: "23:59:01".
static const char cc__DATE__ [] = __DATE__;
static const char cc__TIME__ [] = __TIME__;

static const char *ccDigMns [12] =	{
			"01",  "02",  "03",  "04",  "05",  "06",  "07",  "08",  "09",  "10",  "11",  "12"
									};

static char szISO__DATE__ [SIZ_ISO8601DATE];

const char *szBuild_ISO__DATE__ (void)
{
	ubf_assert (11	== strlen (cc__DATE__));

	/*
		if (szISO__DATE__ [0])
			return szISO__DATE__;

		In case of race condition in multi-threaded applications, this is probably
		a more appropriate check:
	*/
	if (szISO__DATE__ [LEN_ISO8601DATE - 1])
		return szISO__DATE__;

	int m;
	for (m = 0; m < 12; ++ m)
	{
		if (!memcmp (ccdtMnths [m], cc__DATE__, 3))
			break;
	}
	ubf_assert_msg (m < 12, "Bug! Month misspelled!");
	// Year, like "2024".
	memcpy (szISO__DATE__, cc__DATE__ + 7, 4);
	szISO__DATE__ [4] = '-';
	// Month, like "10".
	if (m < 12)
		memcpy (szISO__DATE__ + 5, ccDigMns [m], 2);
	else
		memcpy (szISO__DATE__ + 5, "\?\?", 2);
	szISO__DATE__ [7] = '-';
	// Day, like "30".
	memcpy (szISO__DATE__ + 8, cc__DATE__ + 4, 2);
	// The predefined macro __DATE__ places a space before the day.
	//	We however want a '0'.
	if (' ' == *(szISO__DATE__ + 8))
		szISO__DATE__ [8] = '0';
	szISO__DATE__ [LEN_ISO8601DATE] = '\0';
	return szISO__DATE__;
}

#if defined (DEBUG) && !defined (CUNILOG_BUILD_SHARED_LIBRARY)
	const char *szBuild_ISO__TIME__ (void)
	{
		ubf_assert (8 == strlen (cc__TIME__));

		return cc__TIME__;
	}
#endif

static char szISO__DATE__TIME__ [SIZ_ISO8601DATETIMESTAMP_NO_OFFS];

const char *szBuild_ISO__DATE__TIME__ (void)
{
	ubf_assert (11	== strlen (cc__DATE__));
	ubf_assert (8	== strlen (cc__TIME__));

	/*
		if (szISO__DATE__TIME__ [0])
			return szISO__DATE__TIME__;

		In case of race condition in multi-threaded applications, this is probably
		a more appropriate check:
	*/
	if (szISO__DATE__TIME__ [SIZ_ISO8601DATETIMESTAMP_NO_OFFS - 1])
		return szISO__DATE__TIME__;
	
	szBuild_ISO__DATE__ ();
	memcpy (szISO__DATE__TIME__, szISO__DATE__, LEN_ISO8601DATE);
	szISO__DATE__TIME__ [LEN_ISO8601DATE] = ' ';
	memcpy (szISO__DATE__TIME__ + LEN_ISO8601DATE + 1, cc__TIME__, 9);
	return szISO__DATE__TIME__;
}

DISABLE_WARNING_ASSIGNMENT_WITHIN_CONDITIONAL_EXPRESSION ()

size_t replace_ISO_DATE_ (char *sz, size_t len)
{
	szBuild_ISO__DATE__ ();

	size_t l = USE_STRLEN == len ? strlen (sz) : len;
	if (LEN_ISO8601DATE > l)
		return 0;

	size_t	r		= 0;
	char	*check	= sz;
	char	*found;

	while ((found = memstrstr (check, l, "_ISO_DATE_", LEN_ISO8601DATE)))
	{
		memcpy (found, szISO__DATE__, LEN_ISO8601DATE);
		check = found + LEN_ISO8601DATE;
		if (check + LEN_ISO8601DATE < sz + l)
		{
			l -= check - sz;
			++ r;
		} else
			break;
	}
	return r;
}

DEFAULT_WARNING_ASSIGNMENT_WITHIN_CONDITIONAL_EXPRESSION ()

#ifdef ISO_DATE_BUILD_TEST_FNCT
	bool ISO_DATE_Test_function (void)
	{
		bool b = true;

		const char *szi = szBuild_ISO__DATE__ ();
		ubf_expect_bool_AND (b, LEN_ISO8601DATE == strlen (szi));

		return b;
	}
#endif
/****************************************************************************************

	File:		ubfdebug.c
	Why:		Contains debugging functions.
	OS:			Windows/POSIX
	Author:		Thomas
	Created:	2017-10-24
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2017-10-24	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

// Windows warnings.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef _CRT_RAND_S
#define _CRT_RAND_S
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./ubfdebug.h"

	#ifdef UBF_STRINTUINT_USE_CURR_DIR
		#include "./strintuint.h"
	#else
		#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
			#include "./strintuint.h"
		#else
			#include "./../string/strintuint.h"
		#endif
	#endif
	#ifdef UBF_DEBUG_HEX_DUMP
		#include "./ubfdump.h"
	#endif

#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	#if defined (UBF_USE_DBG_ABRT_OUTPUT_FNCTS) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
		const char	szDbgMsgM01	[]	= "ubf_debug_assert () - Failed \"";
		size_t		lnDbgMsgM01		= sizeof (szDbgMsgM01) - 1;
		const char	szDbgMsgM02 []	= "\" in \"";
		size_t		lnDbgMsgM02		= sizeof (szDbgMsgM02) - 1;
		const char	szDbgMsgM03 []	= "\", line ";
		size_t		lnDbgMsgM03		= sizeof (szDbgMsgM03) - 1;
		const char	szDbgMsgM04 []	= "...";
		size_t		lnDbgMsgM04		= sizeof (szDbgMsgM04) - 1;
		const char	szDbgMsgM05 []	= "ubf_debug_assert () - Testing \"";
		size_t		lnDbgMsgM05		= sizeof (szDbgMsgM05) - 1;
		const char	szDbgMsgM06 []	= "ubf_debug_assert () - \"";
		size_t		lnDbgMsgM06		= sizeof (szDbgMsgM06) - 1;
		const char	szDbgMsgM07 []	= "\" passed.";
		size_t		lnDbgMsgM07		= sizeof (szDbgMsgM07) - 1;
	#endif

	#if defined (UBF_USE_DBG_ABRT_OUTPUT_FNCTS) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
		void ubf_debug_assert		(
						bool			bAssert,
						const char		*chDebugMessage,
						const char		*chFilePath,
						unsigned int	uiLineNum
									)
		{
			char	cMsg [UBF_DEBUG_MSG_STRLEN];
			char	*pMsg;
			char	cNum [UBF_UINT64_SIZE];
			size_t	lNum;

			size_t	lnDebugMessage	= strlen (chDebugMessage);
			size_t	lnFilePath		= strlen (chFilePath);
			size_t	lnTotal			= lnDbgMsgM01 + lnDebugMessage + lnDbgMsgM02
									+ lnFilePath + lnDbgMsgM03 + UBF_UINT64_SIZE
									+ lnDbgMsgM04;
			char	*ch;
			
			if (lnTotal < UBF_DEBUG_MSG_STRLEN)
			{
				ch		= cMsg;
				pMsg	= cMsg;
			} else
			{
				pMsg	= malloc (lnTotal + 1);
				if (NULL == pMsg)
				{	// Ouch!
					_ASSERT (false);
					return;
				}
				ch		= pMsg;
			}

			if (bAssert)
			{

			} else
			{
				ubf_str_from_uint64 (cNum, (uint64_t) uiLineNum);
				lNum = strlen (cNum);
				memcpy (ch, szDbgMsgM01, lnDbgMsgM01);			ch += lnDbgMsgM01;
				memcpy (ch, chDebugMessage, lnDebugMessage);	ch += lnDebugMessage;
				memcpy (ch, szDbgMsgM02, lnDbgMsgM02);			ch += lnDbgMsgM02;
				memcpy (ch, chFilePath, lnFilePath);			ch += lnFilePath;
				memcpy (ch, szDbgMsgM03, lnDbgMsgM03);			ch += lnDbgMsgM03;
				memcpy (ch, cNum, lNum);						ch += lNum;
				memcpy (ch, szDbgMsgM04, lnDbgMsgM04);			ch += lnDbgMsgM04;
				*ch = '\0';

				/*
				strcpy (chMsg, "ubf_debug_assert () - Failed \"");
				strncat (chMsg, chDebugMessage, UBF_DEBUG_MSG_STRLEN - strlen (chMsg) - 1);
				strncat (chMsg, "\" in \"", UBF_DEBUG_MSG_STRLEN - strlen (chMsg) - 1);
				strncat (chMsg, chFilePath, UBF_DEBUG_MSG_STRLEN - strlen (chMsg) - 1);
				strncat (chMsg, "\", line ", UBF_DEBUG_MSG_STRLEN - strlen (chMsg) - 1);
				strncat (chMsg, cNum, UBF_DEBUG_MSG_STRLEN - strlen (chMsg) - 1);
				strncat (chMsg, "...", UBF_DEBUG_MSG_STRLEN - strlen (chMsg) - 1);
				*/

				ubf_debug_puts (pMsg);
				_ASSERT (bAssert);
				/*
				strcpy (chMsg, "ubf_debug_assert () - \"");
				strncat (chMsg, chDebugMessage, UBF_DEBUG_MSG_STRLEN - strlen (chMsg) - 1);
				strncat (chMsg, "\" passed.", UBF_DEBUG_MSG_STRLEN - strlen (chMsg) - 1);
				ubf_debug_puts (chMsg);
				*/
			}
		}
	#endif

	#if defined (UBF_USE_DBG_ABRT_OUTPUT_FNCTS) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
		void ubf_debug_assert_pass	(
						bool			bAssert,
						const char		*chDebugMessage,
						const char		*chFilePath,
						unsigned int	uiLineNum
									)
		{
			char	cMsg [UBF_DEBUG_MSG_STRLEN];
			char	*pMsg;
			char	cNum [UBF_UINT64_SIZE];
			size_t	lNum;

			size_t	lnDebugMessage	= strlen (chDebugMessage);
			size_t	lnFilePath		= strlen (chFilePath);
			size_t	lnTotal			= lnDbgMsgM05 + lnDebugMessage + lnDbgMsgM02
									+ lnFilePath + lnDbgMsgM03 + UBF_UINT64_SIZE
									+ lnDbgMsgM04;
			char	*ch;
			
			if (lnTotal < UBF_DEBUG_MSG_STRLEN)
			{
				ch		= cMsg;
				pMsg	= cMsg;
			} else
			{
				pMsg	= malloc (lnTotal + 1);
				if (NULL == pMsg)
				{	// Ouch!
					_ASSERT (false);
					return;
				}
				ch		= pMsg;
			}

			ubf_str_from_uint64 (cNum, (uint64_t) uiLineNum);
			lNum = strlen (cNum);

			memcpy (ch, szDbgMsgM05, lnDbgMsgM05);			ch += lnDbgMsgM05;
			memcpy (ch, chDebugMessage, lnDebugMessage);	ch += lnDebugMessage;
			memcpy (ch, szDbgMsgM02, lnDbgMsgM02);			ch += lnDbgMsgM02;
			memcpy (ch, chFilePath, lnFilePath);			ch += lnFilePath;
			memcpy (ch, szDbgMsgM03, lnDbgMsgM03);			ch += lnDbgMsgM03;
			memcpy (ch, cNum, lNum);						ch += lNum;
			memcpy (ch, szDbgMsgM04, lnDbgMsgM04);			ch += lnDbgMsgM04;
			*ch = '\0';

			/*
			strcpy (cMsg, "ubf_debug_assert () - Testing \"");
			strncat (cMsg, chDebugMessage, UBF_DEBUG_MSG_STRLEN - strlen (cMsg) - 1);
			strncat (cMsg, "\" in \"", UBF_DEBUG_MSG_STRLEN - strlen (cMsg) - 1);
			strncat (cMsg, chFilePath, UBF_DEBUG_MSG_STRLEN - strlen (cMsg) - 1);
			strncat (cMsg, "\", line ", UBF_DEBUG_MSG_STRLEN - strlen (cMsg) - 1);
			strncat (cMsg, cNum, UBF_DEBUG_MSG_STRLEN - strlen (cMsg) - 1);
			strncat (cMsg, "...", UBF_DEBUG_MSG_STRLEN - strlen (cMsg) - 1);
			*/

			ubf_debug_puts (pMsg);
			_ASSERT (bAssert);
			
			ch = pMsg;
			memcpy (ch, szDbgMsgM06, lnDbgMsgM06);			ch += lnDbgMsgM06;
			memcpy (ch, chDebugMessage, lnDebugMessage);	ch += lnDebugMessage;
			memcpy (ch, szDbgMsgM07, lnDbgMsgM07);			ch += lnDbgMsgM07;
			*ch = '\0';

			/*
			strcpy (cMsg, "ubf_debug_assert () - \"");
			strncat (cMsg, chDebugMessage, UBF_DEBUG_MSG_STRLEN - strlen (cMsg) - 1);
			strncat (cMsg, "\" passed.", UBF_DEBUG_MSG_STRLEN - strlen (cMsg) - 1);
			*/

			ubf_debug_puts (pMsg);
		}
	#endif
#endif

#ifdef DEBUG
	void ubf_debug_fill_mem (void *mem, size_t len)
	{
		ubf_assert (NULL != mem);
		ubf_assert (0 != len);
		memset (mem, UBF_ERROR_CHAR, len);
	}
#endif

#ifdef DEBUG
	void ubf_debug_check_mem (char *mem, size_t len)
	{
		size_t	st;

		ubf_assert (NULL != mem);
		ubf_assert (0 < len);
		ubf_assert (UBF_ERROR_CHAR == mem [0]);
		for (st = 0; st < len; ++ len)
		{
			ubf_assert (UBF_ERROR_CHAR == mem [st]);
		}
	}
#endif

#ifdef DEBUG
	void ubf_debug_fill_mem_with (void *mem, char c, size_t len)
	{
		ubf_assert (NULL != mem);
		ubf_assert (0 != len);
		memset (mem, c, len);
	}
#endif

#ifdef DEBUG
	void ubf_debug_check_mem_with (char *mem, char c, size_t len)
	{
		size_t	st;

		ubf_assert (NULL != mem);
		ubf_assert (0 < len);
		ubf_assert (c == mem [0]);
		for (st = 0; st < len; ++len)
		{
			ubf_assert (c == mem [st]);
		}
	}
#endif

#ifdef UBF_DEBUG_HEX_DUMP
	void ubf_debug_hex_dump (const char *chOutput, size_t stOutput)
	{
		ubf_data_dump_puts (chOutput, stOutput, NULL);
	}
#endif

#ifdef UBF_DEVICE_GUI
	#ifdef DEBUG
	void ubf_debug_printf (const char *format, ...)
	{
		va_list		args;
		va_start (args, format);
		vprintf (format, args);
		va_end (args);
		fflush (stdout);
		fflush (stderr);
	}

	void ubf_debug_puts (const char *str)
	{
		//ubf_qdebug_puts (str);
		puts (str);
		fflush (stdout);
		fflush (stderr);
	}
	#endif
#endif
	
#ifdef UBF_DEBUG_USE_PRINTF
	int ubf_debug_printf (const char *format, ...)
	{
		int			iRet;
		va_list		args;

		va_start (args, format);
		iRet = vprintf	(format, args);
		va_end (args);
		return iRet;
	}

	int ubf_debug_puts (const char *str)
	{
		return puts (str);
	}
#endif
/****************************************************************************************

	File:		dbgcountandtrack.c
	Why:		Simple counter debug module
	OS:			-
	Author:		Thomas
	Created:	2025-01-22

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-01-22	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>
#include <stdlib.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./dbgcountandtrack.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
	#else
		#include "./ubfdebug.h"
	#endif

#endif

#ifndef UNUSED
#define UNUSED(var) (void)(var)
#endif

#ifdef DEBUG
	void cpyDBGcountandtrack (char *szDst, const char *szSrc)
	{
		size_t l = strlen (szSrc);

		if (l < DBGCOUNTANDRACK_MAX_STRING)
			memcpy (szDst, szSrc, l + 1);
		else
		{
			memcpy (szDst, szSrc, DBGCOUNTANDRACK_MAX_STRING - 1);
			szDst [DBGCOUNTANDRACK_MAX_STRING] = ASCII_NUL;
		}
	}
#endif

#ifdef DEBUG
	void setMembers	(
			SDBGCOUNTANDTRACK *t, size_t value, const char *file, const char *func,
			unsigned int line
					)
	{
		t->value = value;
		cpyDBGcountandtrack (t->cSourceFileName,	file);
		cpyDBGcountandtrack (t->cFunctionName,		func);
		t->uiLine = line;
	}
#endif

#ifdef DEBUG
	void resetDBGcountandtrack	(
			SDBGTRACKER *pt, const char *szFile, const char *szFunc, unsigned int line
								)
	{
		cpyDBGcountandtrack (pt->cSourceFileName,	szFile);
		cpyDBGcountandtrack (pt->cFunctionName,		szFunc);
		pt->uiLine	= line;
		pt->value	= 0;
		pt->n		= 0;
		pt->t		= 0;
	}
#endif

#ifdef DEBUG
	void trackDBGcountandtrack	(
			SDBGTRACKER *pt, size_t value, const char *szFile, const char *szFunc,
			unsigned int line
								)
	{
		if (pt->size > pt->n)
		{
			setMembers (&pt->trackers [pt->n], value, szFile, szFunc, line);
			++ pt->n;
		} else
		{
			size_t currSize = sizeof (SDBGCOUNTANDTRACK) * pt->size;
			size_t resvSize = sizeof (SDBGCOUNTANDTRACK) * DBGCOUNTANDTRACK_RESERVE_NUM;
			SDBGCOUNTANDTRACK *t = malloc (currSize + resvSize);
			if (t)
			{
				if (pt->size)
				{
					memcpy (t, pt->trackers, currSize);
					free (pt->trackers);
				}
				pt->trackers = t;
				pt->n = pt->size;
				pt->size += DBGCOUNTANDTRACK_RESERVE_NUM;
				setMembers (&pt->trackers [pt->n], value, szFile, szFunc, line);
				++ pt->n;
			}
		}
		pt->value = value;
	}
#endif

#ifdef DEBUG
	void trackcheckDBGcountandtrack	(
			SDBGTRACKER *pt, size_t value, const char *szFile, const char *szFunc,
			unsigned int line
									)
	{
		UNUSED (value);
		UNUSED (szFile);
		UNUSED (szFunc);
		UNUSED (line);

		if (pt->size > pt->n)
		{
			ubf_assert (value == pt->trackers [pt->t].value);
			++ pt->t;
			return;
		}
		ubf_assert (false);
	}
#endif
/****************************************************************************************

	File:		stransi.c
	Why:		Functions for ANSI escape codes.
	OS:			C99
	Author:		Thomas
	Created:	2025-02-14

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-02-14	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./stransi.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./externC.h"
		#include "./ubfmem.h"
		#include "./ubfdebug.h"
	#else
		#include "./../pre/externC.h"
		#include "./../mem/ubfmem.h"
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

/*
	The Wikipedia article https://en.wikipedia.org/wiki/ANSI_escape_code has been used
	as the basis to determine if sz is an ANSI escape sequence:

	https://en.wikipedia.org/wiki/ANSI_escape_code#Control_Sequence_Introducer_commands

	"For Control Sequence Introducer, or CSI, commands, the ESC [ (written as \e[, \x1b[
	or \033[ in several programming languages) is followed by any number (including none)
	of "parameter bytes" in the range 0x30-0x3F (ASCII 0-9:;<=>?), then by any number of
	"intermediate bytes" in the range 0x20-0x2F (ASCII space and !"#$%&'()*+,-./), then
	finally by a single "final byte" in the range 0x40-0x7E (ASCII @A-Z[\]^_`a-z{|}~)"
*/
#define UC	unsigned char

size_t isANSIescSequence (const char *sz, size_t ln)
{
	size_t n = 0;

	ln = USE_STRLEN == ln ? strlen (sz) : ln;

	if (ln && (UC) ASCII_ESC == (UC) sz [n])
	{
		++ n;
		if (n < ln && (UC) '[' == (UC) sz [n])
		{
			++ n;
			while	(n < ln && (UC) '\x30' <= (UC) sz [n] && (UC) '\x3F' >= (UC) sz [n])	++ n;
			while	(n < ln && (UC) '\x20' <= (UC) sz [n] && (UC) '\x2F' >= (UC) sz [n])	++ n;
			if		(n < ln && (UC) '\x40' <= (UC) sz [n] && (UC) '\x7E' >= (UC) sz [n])	++ n;
		}
	}
	return n;
}

size_t removeANSIescSequences (char *sz, size_t ln)
{
	ln = USE_STRLEN == ln ? strlen (sz) : ln;

	size_t rt	= ln;
	size_t lo	= ln;
	size_t le;

	while (ln)
	{
		le = isANSIescSequence (sz, ln);
		if (le)
		{
			rt -= le;
			ln -= le;
			if (ln)
				memmove (sz, sz + le, ln);
		} else
		{
			++ sz;
			-- ln;
		}
	};
	if (rt < lo)
		sz [0] = ASCII_NUL;
	return rt;
}

#ifdef STRANSI_BUILD_TEST_FNCT
	bool stransi_test_fnct (void)
	{
		bool	b = true;
		char	sz [1024];

		b &= 0 == isANSIescSequence ("A normal text", USE_STRLEN);
		b &= 1 == isANSIescSequence ("\x1B", 1);
		b &= 2 == isANSIescSequence ("\x1B[", 2);
		b &= LEN_ANSI_RESET == isANSIescSequence (STR_ANSI_RESET, LEN_ANSI_RESET);
		
		b &= 3 == removeANSIescSequences ("ABC", 3);
		strcpy (sz, "ABC\x1B");
		b &= 3 == removeANSIescSequences (sz, 4);
		ubf_expect_bool_AND (b, !memcmp ("ABC", sz, 4));
		strcpy (sz, "ABC\x1B[");
		b &= 3 == removeANSIescSequences (sz, 5);
		ubf_expect_bool_AND (b, !memcmp ("ABC", sz, 4));
		strcpy (sz, STR_ANSI_FGCOL_BRIGHT_BLACK "ABC" STR_ANSI_FGCOL_BRIGHT_BLACK);
		b &= 3 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp ("ABC", sz, 4));
		strcpy (sz, STR_ANSI_FGCOL_BRIGHT_BLACK "ABC" STR_ANSI_FGCOL_BRIGHT_BLACK "ABC");
		b &= 6 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp ("ABCABC", sz, 7));
		strcpy (sz, "\0x1B\0x1B\0x1B\0x1B\0x1B\0x1B\0x1B\0x1B\0x1B\0x1B");
		b &= 0 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp ("", sz, 1));
		strcpy (sz, "[");
		b &= 1 == removeANSIescSequences (sz, 1);
		ubf_expect_bool_AND (b, !memcmp ("[", sz, 2));
		strcpy (sz, "\033[n;nH");							// "n" is not a number.
		b &= 3 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp (";nH", sz, 4));
		strcpy (sz, "\033[9999;8888H");
		b &= 0 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp ("", sz, 1));
		strcpy (sz, "\033[111122223333444455557777888899990000;111122223333444455557777888899990000H");
		b &= 0 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp ("", sz, 1));
		strcpy (sz, "ABC\033[1K");
		b &= 3 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp ("ABC", sz, 4));
		strcpy (sz, "\033[?25hABC");
		b &= 3 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp ("ABC", sz, 4));
		strcpy (sz, "ABC[ABC");
		b &= 7 == removeANSIescSequences (sz, USE_STRLEN);
		ubf_expect_bool_AND (b, !memcmp ("ABC[ABC", sz, 8));


		return b;
	}
#endif
/****************************************************************************************

	File:		check_utf8.c
	Why:		Checks for valid UTF-8
	OS:			C99
	Author:		Thomas
	Created:	2025-01-27
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-01-27	Thomas			This history created.
							Acquired from https://github.com/yasuoka/check_utf8 .
							Thanks to YASUOKA Masahiko.
							Function renamed to c_check_utf8 ().

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
	For the original unchanged version, see https://github.com/yasuoka/check_utf8 .

	Note that this file must be saved in UTF-8 encoding without a BOM.
*/

/*
 * Copyright (c) 2024 YASUOKA Masahiko <yasuoka@yasuoka.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#ifndef USE_STRLEN
#define USE_STRLEN						((size_t) -1)
#endif

bool c_check_utf8(const char *str, size_t len)
{
	int		 i, bytes;
	unsigned int	 oct, ch;
	const char	*pos, *end;

	len = USE_STRLEN == len ? strlen (str) : len;

	pos = str;
	end = str + len;
	while (pos < end) {
		oct = *pos;
		if ((oct & 0x80) == 0) {
			bytes = 1;
			ch = oct & 0x7f;
		} else if ((oct & 0xe0) == 0xc0) {
			bytes = 2;
			ch = oct & 0x1f;
		} else if ((oct & 0xf0) == 0xe0) {
			bytes = 3;
			ch = oct & 0xf;
		} else if ((oct & 0xf8) == 0xf0) {
			bytes = 4;
			ch = oct & 0x7;
		} else
			return false;
		if (pos + bytes > end)
			return false;
		for (i = 1; i < bytes; i++) {
			oct = pos[i];
			if ((oct & 0xc0) != 0x80)
				return false;
			ch = ch << 6 | (oct & 0x3f);
		}
		if (!(bytes == 1 || (bytes == 2 && ch >= 0x80) ||
		    (bytes == 3 && ch >= 0x800) ||
		    (bytes == 4 && ch >= 0x10000)))
			return false;
		pos += bytes;
	}

	return true;
}

/*
	https://github.com/yasuoka/check_utf8/blob/main/check_utf8_test.c
*/
#ifdef U_CHECK_UTF8_BUILD_TEST_FNCT
	bool Check_utf8_test_function (void)
	{
		bool b = true;

		b &= c_check_utf8("ほげほげ", 12);
		//ほげほげ in Shift-JIS
		b &= c_check_utf8("\x82\xd9\x82\xb0\x82\xd9\x82\xb0", 8) == false;
		//copyright mark in UTF-8
		b &= c_check_utf8 ("\xC2\xA9", 2);
		b &= c_check_utf8("©", 2) == true;
		// face in medical mask in UTF-8
		b &= c_check_utf8("\xF0\x9F\x98\xB7", 4) == true;
		// length invalid
		b &= c_check_utf8("ふがふが", 11) == false;
		// ascii
		b &= c_check_utf8("Hello world.", 12) == true;
		// empty
		b &= c_check_utf8("", 1) == true;
		// specials
		b &= c_check_utf8("\t\b", 2) == true;

		return b;
	}
#endif
/****************************************************************************************

	File:		strcustomfmt.c
	Why:		Functions for user-defined format specifiers.
	OS:			C99
	Author:		Thomas
	Created:	2024-12-26

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-12-26	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strcustomfmt.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		//#include "./memstrstr.h"
		#include "./ubfdebug.h"
		#include "./unref.h"
	#else
		//#include "./../mem/memstrstr.h"
		#include "./../dbg/ubfdebug.h"
		#include "./../pre/unref.h"
	#endif

#endif

/*
	This function does NOT set *pidx when it returns NULL (when no custom format specifier can
	be found). Debug versions set *pidx to (size_t) -1 in this case, which hopefully causes an access
	violation somewhere in the caller. So, when you call this function, check for (size_t) -1 with
	an assertion, like:

	size_t idx;
	const char *cc = strcustomfmtFindNextCust (&idx, ...);
	ubf_assert ((size_t) -1 != idx);

	If you use your own assertion macro(s), make sure this isn't done in release versions to avoid
	undefined behaviour. Check the return value first! If the function returns NULL, *pidx is NOT
	set!
*/
static inline const char *strcustomfmtFindNextCust (size_t *pidx, SSTRCUSTFMTBASE *pb, const char *cust, size_t len)
{
	ubf_assert_non_NULL	(pidx);
	ubf_assert_non_NULL	(pb);
	ubf_assert_non_NULL	(cust);
	ubf_assert			((size_t) -1 != len);

	while (len)
	{
		size_t idx = 0;
		while (idx < pb->n)
		{
			ubf_assert_non_NULL	(pb->psstrcustfmt [idx].ccFormatSpecifier);
			ubf_assert_non_0	(pb->psstrcustfmt [idx].ccFormatSpecifier [0]);
			ubf_assert_non_0	(pb->psstrcustfmt [idx].lenFormatSpecifier);
			ubf_assert			(USE_STRLEN != pb->psstrcustfmt [idx].lenFormatSpecifier);

			if (len >= pb->psstrcustfmt [idx].lenFormatSpecifier)
			{
				if (!memcmp (cust, pb->psstrcustfmt [idx].ccFormatSpecifier, pb->psstrcustfmt [idx].lenFormatSpecifier))
				{
					*pidx = idx;
					return cust;
				}
			}
			++ idx;
		}
		++ cust;
		-- len;
	}
	#ifdef DEBUG
		*pidx = (size_t) -1;
	#endif
	return NULL;
}

static inline size_t getStringLength (const char *cc, size_t len)
{
	return USE_STRLEN == len ? strlen (cc) : len;
}

size_t strcustomfmtGetRqSizeCustVA (SSTRCUSTFMTBASE *pb, size_t lenCust, const char *cust, va_list ap)
{
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (cust);

	void		*v;
	size_t		idx;

	size_t		len = getStringLength (cust, lenCust);
	size_t		ret = len;
	const char	*cp = cust;
	const char	*cc	= strcustomfmtFindNextCust (&idx, pb, cust, len);
	while (cc)
	{
		ubf_assert_non_NULL	(pb->psstrcustfmt [idx].ccFormatSpecifier);
		ubf_assert_non_0	(pb->psstrcustfmt [idx].ccFormatSpecifier [0]);
		ubf_assert_non_0	(pb->psstrcustfmt [idx].lenFormatSpecifier);
		ubf_assert			(USE_STRLEN != pb->psstrcustfmt [idx].lenFormatSpecifier);

		v	= va_arg (ap, void *);
		len	-= pb->psstrcustfmt [idx].lenFormatSpecifier;
		size_t st2 = cc - cp;
		len	-= st2;
		cp	+= st2 + pb->psstrcustfmt [idx].lenFormatSpecifier;
		ret	-= pb->psstrcustfmt [idx].lenFormatSpecifier;
		ret	+= pb->psstrcustfmt [idx].getRqSize (v, &pb->psstrcustfmt [idx]);
		cc	= strcustomfmtFindNextCust (&idx, pb, cp, len);
	}
	return ret;
}

size_t strcustomfmtGetRqSizeCust (SSTRCUSTFMTBASE *pb, size_t lenCust, const char *cust, ...)
{
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (cust);

	va_list ap;
	va_start (ap, cust);
	size_t ret = strcustomfmtGetRqSizeCustVA (pb, lenCust, cust, ap);
	va_end (ap);

	return ret;
}

static inline void storeNonFormatSpecifiers (char **buf, size_t *siz, const char *txt, size_t len)
{
	ubf_assert_non_NULL (buf);
	ubf_assert_non_NULL (*buf);
	ubf_assert_non_NULL (siz);

	memcpy (*buf, txt, len);
	*buf += len;
	*siz -= len;
}

size_t strcustomfmtStoreDataCustVA (char *strBuf, size_t sizBuf, SSTRCUSTFMTBASE *pb, size_t lenCust, const char *cust, va_list ap)
{
	ubf_assert_non_NULL (strBuf);
	ubf_assert_non_0	(sizBuf);
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (cust);

	void *v;
	size_t		idx;

	size_t		s;
	char		*cb = strBuf;
	size_t		sb	= sizBuf;
	size_t		len = getStringLength (cust, lenCust);
	size_t		ret = len;
	const char	*cp = cust;
	const char	*cc = strcustomfmtFindNextCust (&idx, pb, cust, len);
	while (cc)
	{
		ubf_assert_non_NULL	(pb->psstrcustfmt [idx].ccFormatSpecifier);
		ubf_assert_non_0	(pb->psstrcustfmt [idx].ccFormatSpecifier [0]);
		ubf_assert_non_0	(pb->psstrcustfmt [idx].lenFormatSpecifier);
		ubf_assert			(USE_STRLEN != pb->psstrcustfmt [idx].lenFormatSpecifier);

		v	= va_arg (ap, void *);
		len	-= pb->psstrcustfmt [idx].lenFormatSpecifier;
		storeNonFormatSpecifiers (&cb, &sb, cp, cc - cp);
		size_t st2 = cc - cp;
		len	-= st2;
		cp	+= st2 + pb->psstrcustfmt [idx].lenFormatSpecifier;
		ret	-= pb->psstrcustfmt [idx].lenFormatSpecifier;
		s	= pb->psstrcustfmt [idx].storeData (cb, sb, v, &pb->psstrcustfmt [idx]);
		ret	+= s;
		cb	+= s;
		sb	-= s;
		cc	= strcustomfmtFindNextCust (&idx, pb, cp, len);
	}
	if (len)
	{
		memcpy (cb, cp, len);
		cb += len;
	}
	*cb = ASCII_NUL;
	return ret;
}

size_t strcustomfmtStoreDataCust (char *strBuf, size_t sizBuf, SSTRCUSTFMTBASE *pb, size_t lenCust, const char *cust, ...)
{
	ubf_assert_non_NULL (strBuf);
	ubf_assert_non_0	(sizBuf);
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (cust);

	va_list ap;
	va_start (ap, cust);
	size_t ret = strcustomfmtStoreDataCustVA (strBuf, sizBuf, pb, lenCust, cust, ap);
	va_end (ap);
	return ret;
}

#ifdef STRCUSTOMFMT_BUILD_TEST_FNCTS

	size_t ourReqSize (void *pCustFmtArg, SSTRCUSTFMT *pSSTRCUSTFMT)
	{
		UNUSED (pSSTRCUSTFMT);

		if (pCustFmtArg)
		{
			return 4;
		} else
		{
			return 10;
		}
	}

	size_t ourStoData (char *szBuf, size_t len, void *pCustFmtArg, SSTRCUSTFMT *pSSTRCUSTFMT)
	{
		UNUSED (len);
		UNUSED (pSSTRCUSTFMT);

		if (pCustFmtArg)
		{
			memcpy (szBuf, "ABCD", 4);
			return 4;
		} else
		{
			memcpy (szBuf, "0123456789", 10);
			return 10;
		}
	}

	// Size to remember.
	size_t size2;

	size_t theSize2 (void *pCustFmtArg, SSTRCUSTFMT *pSSTRCUSTFMT)
	{
		ubf_assert_non_NULL (pCustFmtArg);
		ubf_assert_non_NULL (pSSTRCUSTFMT);

		const char *ccCustomFormatArgumentExpanded = pCustFmtArg;
		size2 = strlen (ccCustomFormatArgumentExpanded) + 1;
		SSTRCUSTFMT *p = pSSTRCUSTFMT;
		p->pCustom = &size2;
		return size2 - 1;
	}

	size_t theData2 (char *szBuf, size_t len, void *pCustFmtArg, SSTRCUSTFMT *pSSTRCUSTFMT)
	{
		ubf_assert_non_NULL (pCustFmtArg);
		ubf_assert_non_NULL (pSSTRCUSTFMT);

		UNUSED (len);

		const char *ccCustomFormatArgumentExpanded = pCustFmtArg;
		SSTRCUSTFMT *p = pSSTRCUSTFMT;
		size_t size = *(size_t *)p->pCustom;
		size_t s2 = strlen (ccCustomFormatArgumentExpanded) + 1;
		ubf_assert (s2 == size);
		ubf_assert (s2 <= len);
		memcpy (szBuf, ccCustomFormatArgumentExpanded, s2);
		return size - 1;
	}

	bool TestStrCustomFmt (void)
	{
		bool br = true;

		SSTRCUSTFMTBASE	b;
		SSTRCUSTFMT		f [3];

		// Format specifiers only for now.
		f [0].ccFormatSpecifier		= "{a}";
		f [0].lenFormatSpecifier	= 3;
		f [1].ccFormatSpecifier		= "#1";
		f [1].lenFormatSpecifier	= 2;
		f [2].ccFormatSpecifier		= "{x}";
		f [2].lenFormatSpecifier	= 3;

		b.psstrcustfmt				= f;
		b.n							= 3;

		const char	*cc;
		size_t		idx;

		cc = strcustomfmtFindNextCust (&idx, &b, "{v}", 3);
		ubf_assert_NULL (cc);
		br &= NULL == cc;
		cc = strcustomfmtFindNextCust (&idx, &b, "{a}", 3);
		br &= !memcmp ("{a}", cc, 4);
		ubf_assert (br);
		cc = strcustomfmtFindNextCust (&idx, &b, "   ", 3);
		ubf_assert_NULL (cc);
		br &= NULL == cc;
		cc = strcustomfmtFindNextCust (&idx, &b, "#1", 2);
		br &= !memcmp ("#1", cc, 2);
		ubf_assert (br);

		// Let's check the callback functions.
		f [0].getRqSize				= ourReqSize;
		f [0].storeData				= ourStoData;
		size_t s1 = strcustomfmtGetRqSizeCust (&b, 3, "{a}", NULL);
		char ch [256];
		size_t s2 = strcustomfmtStoreDataCust (ch, 256, &b, 3, "{a}", NULL);
		ubf_assert (s1 == s2);
		br &= s1 == s2;
		br &= !memcmp (ch, "0123456789", 10);
		ubf_assert (br);

		// A bit more complicated.
		f [1].getRqSize				= ourReqSize;
		f [1].storeData				= ourStoData;
		memset (ch, 0, 256);
		char	x = 0;
		s1 = strcustomfmtGetRqSizeCust (			&b, USE_STRLEN, "None {a} // #1, {a}xxx", NULL, &x, NULL);
		s2 = strcustomfmtStoreDataCust (ch, 256,	&b, USE_STRLEN, "None {a} // #1, {a}xxx", NULL, &x, NULL);
		ubf_assert (s1 == s2);
		ubf_assert (!memcmp (ch, "None 0123456789 // ABCD, 0123456789xxx", 39));

		// Note that the spec says that space for a NUL terminator is required, hence s1 + 1.
		s1 = strcustomfmtGetRqSizeCust (&b, USE_STRLEN, "None {a} // #1, {a}xxx", NULL, &x, NULL);
		s2 = strcustomfmtStoreDataCust (ch, s1 + 1, &b, USE_STRLEN, "None {a} // #1, {a}xxx", NULL, &x, NULL);
		ubf_assert (s1 == s2);
		ubf_assert (!memcmp (ch, "None 0123456789 // ABCD, 0123456789xxx", 39));

		f [2].getRqSize = theSize2;
		f [2].storeData = theData2;
		s1 = strcustomfmtGetRqSizeCust (&b, USE_STRLEN,	"This is our {x}.", "custom format expanded");
		s2 = strcustomfmtStoreDataCust (ch, s1 + 1, &b, USE_STRLEN, "This is our {x}.", "custom format expanded");
		ubf_assert (s1 == s2);
		ubf_assert (!memcmp (ch, "This is our custom format expanded.", 36));

		// Empty. The spec says that even if the length is 0 there's still one octet for a
		//	NUL terminator.
		memset (ch, 0, 256);
		s1 = strcustomfmtGetRqSizeCust (&b, USE_STRLEN, "", NULL, &x, NULL);
		s2 = strcustomfmtStoreDataCust (ch, 256, &b, USE_STRLEN, "", NULL, &x, NULL);
		ubf_assert (s1 == s2);
		s1 = strcustomfmtGetRqSizeCust (&b, 0, "", NULL, &x, NULL);
		s2 = strcustomfmtStoreDataCust (ch, 256, &b, 0, "", NULL, &x, NULL);
		ubf_assert (s1 == s2);

		return br;
	}

#endif														// Of #ifdef STRCUSTOMFMT_BUILD_TEST_FNCTS.
/****************************************************************************************

	File:		strfilesys.c
	Why:		File system string functions.
	OS:			C99.
	Author:		Thomas
	Created:	2023-05-27
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2023-05-27	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strfilesys.h"
	#include "./ubfcharscountsandchecks.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./memstrstr.h"
		#include "./ubfdebug.h"
		#include "./platform.h"
		#include "./Warnings.h"
	#else
		#include "./../mem/memstrstr.h"
		#include "./../dbg/ubfdebug.h"
		#include "./../pre/platform.h"
		#include "./../pre/Warnings.h"
	#endif

#endif

size_t str_correct_dir_separators (char *str, size_t len)
{
	size_t	n;
	size_t	r	= 0;
	
	len = (size_t) -1 == len ? strlen (str) : len;
	for (n = 0; n < len; ++ n)
	{
		if (isWrongDirectorySeparator (str [n]))
		{
			str [n] = UBF_DIR_SEP;
			++ r;
		}
	}
	return r;
}

/*
	Original (most likely slower) function.

	static char		cNavWin [4] 	= {'.','.', UBF_WIN_DIR_SEP, 0};
	static char		cNavUnx [4] 	= {'.','.', UBF_PSX_DIR_SEP, 0};
	static size_t	lenNav		= sizeof (cNavWin) - 1;

	char *str_find_path_navigator (char *szString, size_t lnString)
	{
		char *chW = memstrstr (szString, lnString, cNavWin, lenNav);
		char *chU = memstrstr (szString, lnString, cNavUnx, lenNav);
		return chW && chU ? (chW < chU ? chW : chU) : (chW ? chW : chU);
	}
*/

static char		cDoubleDot [2]		= {'.', '.'};
const size_t	lenNav				= 3;

DISABLE_WARNING_ASSIGNMENT_WITHIN_CONDITIONAL_EXPRESSION ()

char *str_find_path_navigator (char *szString, size_t lnString)
{
	ubf_assert_non_NULL (szString);
	ubf_assert (USE_STRLEN != lnString);					// Not supported in this function.

	char *szNavigator;

	while ((szNavigator = memstrstr (szString, lnString, cDoubleDot, 2)))
	{
		lnString -= szNavigator - szString;
		if (lnString > 2)
		{
			if ('/' == szNavigator [2] || '\\' == szNavigator [2])
				return szNavigator;
			szString = szNavigator + 1;
			-- lnString;
		} else
			return NULL;
	}
	return NULL;
}

size_t str_remove_path_navigators (char *chPath, size_t *pLen)
{
	ubf_assert_non_NULL (chPath);

	size_t		replaced		= 0;
	char		*ch;
	char		*chL;
	char		*chM;
	char		c;
	size_t		mov;
	size_t		len;
	
	size_t lnPath = pLen && SUBF_STRING_USE_STRLEN != *pLen ? *pLen : strlen (chPath);
	ubf_assert (0 < lnPath);
	if (lnPath > 4)											// Minimum is "a/../" (5).
	{	
		char *chS = chPath;
		size_t lnLeft = lnPath;
		if (':' == chS [1])
		{	// Windows "C:".
			ubf_assert (2 <= lnLeft);
			chS		+= 2;
			lnLeft	-= 2;
		}
		c = *chS;
		// Search for the first character that is neither a period nor a slash.
		while (lnLeft && ('.' == c || ubf_isdirseparator (c)))
		{
			++ chS;
			-- lnLeft;
			c = *chS;
		}
		chM = chS;
		while ((ch = str_find_path_navigator (chS, lnLeft)))
		{
			chL = ch;										// "../".
			if (chL > chM + 1)
				chL -= 2;									// "x/../".
			else
				break;
			while (chL >= chS && !ubf_isdirseparator (*chL))
				-- chL;
			++ chL;
			mov = lnPath - (size_t) (ch + lenNav - chPath);
			len = (size_t) (lenNav + ch - chL);
			if (lnPath >= len)
			{
				memmove (chL, ch + lenNav, mov);
				lnPath -= len;
				lnLeft -= len;
				++ replaced;
			}
		}
	}
	if (replaced)
		chPath [lnPath] = '\0';
	if (pLen)
		*pLen = lnPath;
	return replaced;
}

bool ubf_correct_directory_separators_str	(
			char			*chPath,
			size_t			*cunilog_restrict plenPath,
			size_t			*cunilog_restrict pReps
											)
{
	size_t			stRet	= 0;
	size_t			st;
	size_t			stLen;
	char			*ch		= chPath;
	char			ds [3]	= {UBF_DIR_SEP, UBF_DIR_SEP, 0};	// Double separator.
	
	ubf_assert (NULL != chPath);
	if (chPath)
	{
		st = plenPath
				 ? (SUBF_STRING_USE_STRLEN == *plenPath ? strlen (chPath) : *plenPath)
				 : strlen (chPath);
		ubf_assert (0 < st);
		if (st)
		{
			stLen = st;
			while (st --)
			{	// Replace all directory separators of the wrong platform. On POSIX
				//	platforms the backslash is replaced by a forward slash, and on Windows
				//	platforms it is the other way round.
				if (UBF_WRONG_DIR_SEP == *ch)
				{
					*ch = UBF_DIR_SEP;
					++ stRet;
				}
				++ ch;
			}
			// Remove duplicate separators. We're not going to touch the first double
			//	slash of "\\server\share".
			if (st > 2)
			{
				ch = chPath + 2;
				while ((ch = strstr (ch, ds)))
				{
					-- stLen;
					memmove (ch, ch + 1, chPath + stLen - ch);
					++ stRet;
				}
			}
			if (plenPath)
				*plenPath = stLen;
			if (pReps)
				*pReps = stRet;
			return true;
		}
	}
	return false;
}

DEFAULT_WARNING_ASSIGNMENT_WITHIN_CONDITIONAL_EXPRESSION ()

void ubf_change_directory_separators (char *szPath, size_t len, const char newSeparator)
{
	size_t	l = (size_t) -1 == len ? strlen (szPath) : len;
	size_t	i;
	
	for (i = 0; i < l; ++i)
	{
		if (is_directory_separator (szPath [i]))
			szPath [i] = newSeparator;
	}
}

size_t ubf_len_with_last_directory_separator (const char *szPath, size_t len)
{
	size_t	l = (size_t) -1 == len ? strlen (szPath) : len;
	
	if (szPath)
	{
		if (l)
		{
			while (l --)
			{
				if (is_directory_separator (szPath [l]))
					return l + 1;
			}
		}
		// This actually only applies to Windows. A colon can also be a separator in case of
		//	"C:file.ext". Although this is actually not a real absolute path, we allow it here.
		if (2 <= l && ubf_isalpha (szPath [0]) && ':' == szPath [1])
			return 2;
	}
	return 0;
}

size_t str_remove_last_dir_separator (const char *str, size_t len)
{
	len = str ? (USE_STRLEN == len ? strlen (str) : len) : 0;
	if (len)
	{
		if (is_path_separator (str [len - 1]))
			-- len;
	}
	return len;
}

#ifdef BUILD_DEBUG_UBF_STRFILESYS_TESTS
	bool ubf_test_ubf_strfilesys (void)
	{
		bool			bRet	= false;
		size_t			stLen;
		size_t			st;
		char			chPath [4096];
		size_t			i;
		bool			b		= true;
		size_t			stReps;
		char			*sz;
	
		ubf_assert ((size_t) -1 == SUBF_STRING_UNKNOWN_LENGTH);
		ubf_assert ((size_t) -1 == SUBF_STRING_USE_STRLEN);

		// Function str_find_path_navigator ().
		strcpy (chPath, "..");
		sz = str_find_path_navigator (chPath, 2);
		ubf_assert_NULL (sz);

		strcpy (chPath, "");
		sz = str_find_path_navigator (chPath, 0);
		ubf_assert_NULL (sz);

		strcpy (chPath, "ABC../");
		sz = str_find_path_navigator (chPath, 6);
		ubf_assert (sz == chPath + 3);

		strcpy (chPath, "abc..1..2..3..4..5");
		sz = str_find_path_navigator (chPath, 18);
		ubf_assert_NULL (sz);

	// Function ubf_correct_directory_separators_str ().
	#ifdef PLATFORM_IS_WINDOWS
		strcpy (chPath, "\\\\server\\share");
	#else
		strcpy (chPath, "//server/share");
	#endif
		// With (size_t) -1 and NULL. Both should behave identically.
		stLen = SUBF_STRING_USE_STRLEN;
		b &= ubf_correct_directory_separators_str (chPath, &stLen, &stReps);
		ubf_expect_TRUE (b);
		ubf_expect_bool_AND (b, 0 == stReps);
		b &= ubf_correct_directory_separators_str (chPath, NULL, &stReps);
		ubf_expect_bool_AND (b, 0 == stReps);
		// Number of replacements not returned.
		b &= ubf_correct_directory_separators_str (chPath, NULL, NULL);
		ubf_expect_TRUE (b);
		ubf_expect_bool_AND (b, 0 == stReps);
	#ifdef PLATFORM_IS_WINDOWS
		strcpy (chPath, "//server/share");
	#else
		strcpy (chPath, "\\\\server\\share");
	#endif
		st = strlen (chPath);
		stLen = SUBF_STRING_USE_STRLEN;
		b &= ubf_correct_directory_separators_str (chPath, &stLen, &stReps);
		ubf_expect_TRUE (b);
		ubf_expect_bool_AND (b, st == stLen);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, 3 == stReps);
	// Return value 5 because 4 wrong separators replaced + one duplicate.
	#ifdef PLATFORM_IS_WINDOWS
		strcpy (chPath, "//server//share");
	#else
		strcpy (chPath, "\\\\server\\\\share");
	#endif
		b = ubf_correct_directory_separators_str (chPath, NULL, &stReps);
		ubf_expect_TRUE (b);
		ubf_expect_bool_AND (b, 5 == stReps);
	
		// Function str_remove_path_navigators ().
		strcpy (chPath, "D:");
		i = str_remove_path_navigators (chPath, NULL);
		// Minimum length is 4.
		ubf_expect_bool_AND (b, 0 == i);
		// We can't test with a length of 0 as str_remove_path_navigators () asserts
		//	that the length is > 0.
		stLen = 1;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 0 == i);
		ubf_expect_bool_AND (b, 1 == stLen);

		strcpy (chPath, "D:\\path1\\path2");
		stLen = SUBF_STRING_UNKNOWN_LENGTH;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 0 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "D:\\path1\\path2", strlen (chPath)));

		strcpy (chPath, "D:\\path1\\..\\path2");
		stLen = SUBF_STRING_UNKNOWN_LENGTH;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 1 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "D:\\path2", strlen (chPath)));

		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/p01/p02/p03/p04/../../pnew");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/p01/p02/pnew", strlen (chPath)));

		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/p01/p02/p03nnnnnnnnnnnnnnnn/p04/../../pnew");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/p01/p02/pnew", strlen (chPath)));

		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/p01/p02/p03nnnnnnnnnnnnnnnn/p04kkkkkkkkkkkkkkkkkkkkkkkkkkkkk/../../pnew");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/p01/p02/pnew", strlen (chPath)));

		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/p01/p02/d/d/../../pnew");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/p01/p02/pnew", strlen (chPath)));

		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/p01/p02/p03/p04/../../pnew/p05/p06/../p07/p08");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 3 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/p01/p02/pnew/p05/p07/p08", strlen (chPath)));

		// Cannot be resolved. Two path parts need to stay.
		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/p01/p02/p03/p04/../../../../../../p07/p08");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 4 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/../../p07/p08", strlen (chPath)));

		// Each part only 1 octet long.
		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/1/2/3/4/../../../7/8");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 3 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/1/7/8", strlen (chPath)));

		// Invalid navigators.
		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/1/2/3/4/./././7/8");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 0 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/1/2/3/4/./././7/8", strlen (chPath)));

		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/1/2/3/4/././../7/8");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 1 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/1/2/3/4/./7/8", strlen (chPath)));

		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "/1/2/3/4/./../../7/8");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/1/2/3/7/8", strlen (chPath)));

		// With a Windows drive letter.
		stLen = SUBF_STRING_LENGTH_UNKNOWN;
		strcpy (chPath, "X:/1/2/3/4/./../../7/8");
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, strlen (chPath) == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "X:/1/2/3/7/8", strlen (chPath)));

		// Check NUL terminator.
		strcpy (chPath, "/1/2/../4/X");
		stLen = 10;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 1 == i);
		ubf_expect_bool_AND (b, 5 == stLen);
		ubf_assert (!memcmp (chPath, "/1/4/", 6));

		/*
			From the comments:

			"../../test1/test2/test3/../../file.ext"	->	"../../test1/file.ext"
			"../../../1/2/3/4/5/6/7/../f.ext"			->	"../../../1/2/3/4/5/6/f.ext"
			"C:this is a test/test1/test2/../file.ext"	->	"C:this is a test/test1/file.ext"
			"/1/2/3/4/../../../../f.e"					->	"/f.e"
			"1/../f"									->	"f"
			"../f"										->	"../f"
			"1/2/../../f"								->	"f"
			"1/2/../../../f"							->	"../f"
		*/
		strcpy (chPath, "../../test1/test2/test3/../../file.ext");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, 20 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "../../test1/file.ext", 21));

		strcpy (chPath, "../../../1/2/3/4/5/6/7/../f.ext");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 1 == i);
		ubf_expect_bool_AND (b, 26 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "../../../1/2/3/4/5/6/f.ext", 27));

		strcpy (chPath, "C:this is a test/test1/test2/../file.ext");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 1 == i);
		ubf_expect_bool_AND (b, 31 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "C:this is a test/test1/file.ext", 32));

		strcpy (chPath, "/1/2/3/4/../../../../f.e");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 4 == i);
		ubf_expect_bool_AND (b, 4 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/f.e", 5));

		strcpy (chPath, "1/../f");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 1 == i);
		ubf_expect_bool_AND (b, 1 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "f", 2));

		strcpy (chPath, "../f");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 0 == i);
		ubf_expect_bool_AND (b, 4 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "../f", 5));

		strcpy (chPath, "1/2/../../f");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, 1 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "f", 2));

		strcpy (chPath, "1/2/../../../f");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 2 == i);
		ubf_expect_bool_AND (b, 4 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "../f", 5));
		/*
			End of tests from the comments.
		*/

		// Nothing to resolve.
		strcpy (chPath, "/../");
		stLen = USE_STRLEN;
		i = str_remove_path_navigators (chPath, &stLen);
		ubf_expect_bool_AND (b, 0 == i);
		ubf_expect_bool_AND (b, 4 == stLen);
		ubf_expect_bool_AND (b, !memcmp (chPath, "/../", 5));

		st = ubf_len_with_last_directory_separator ("C:\\", USE_STRLEN);
		ubf_expect_bool_AND (b, 3 == st);
		st = ubf_len_with_last_directory_separator ("/home/file", USE_STRLEN);
		ubf_expect_bool_AND (b, 6 == st);
		st = ubf_len_with_last_directory_separator ("/", USE_STRLEN);
		ubf_expect_bool_AND (b, 1 == st);
		st = ubf_len_with_last_directory_separator ("Fridolin", USE_STRLEN);
		ubf_expect_bool_AND (b, 0 == st);
		st = ubf_len_with_last_directory_separator ("///", USE_STRLEN);
		ubf_expect_bool_AND (b, 3 == st);
		st = ubf_len_with_last_directory_separator ("///", 3);
		ubf_expect_bool_AND (b, 3 == st);
		st = ubf_len_with_last_directory_separator ("C:\\", 3);
		ubf_expect_bool_AND (b, 3 == st);
		st = ubf_len_with_last_directory_separator ("C:\\directory", USE_STRLEN);
		ubf_expect_bool_AND (b, 3 == st);
		st = ubf_len_with_last_directory_separator ("C:/file", USE_STRLEN);
		ubf_expect_bool_AND (b, 3 == st);
		st = ubf_len_with_last_directory_separator ("C:file", USE_STRLEN);
		ubf_expect_bool_AND (b, 2 == st);
		st = ubf_len_with_last_directory_separator ("C:file.ext", USE_STRLEN);
		ubf_expect_bool_AND (b, 2 == st);
	
		// Return the test result.
		bRet = b;
		return bRet;
	}
#endif
/****************************************************************************************

	File:		strhex.c
	Why:		Functions for hexadecimal digits.
	OS:			C99
	Author:		Thomas
	Created:	2020-03-07
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2020-03-07	Thomas			Created. Functions have been moved from ubfsecure.c/h into
							 this module.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./unref.h"
		#include "./Warnings.h"
		#include "./strhex.h"
		#include "./strintuint.h"
		#include "./ubfmem.h"
		#include "./ubfdebug.h"
	#else
		#include "./../pre/unref.h"
		#include "./../pre/Warnings.h"
		#include "./strhex.h"
		#include "./strintuint.h"
		#include "./../mem/ubfmem.h"
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

#ifdef DEBUG
	bool ubf_is_octal_digit (char c)
	{
		return ((unsigned char) c >= '0' && (unsigned char) c <= '7');
	}
#endif

#ifdef DEBUG
	bool ubf_is_hex_digit (char c)
	{
		return	(
					((unsigned char) c >= '0' && (unsigned char) c <= '9') ||
					((unsigned char) c >= 'A' && (unsigned char) c <= 'F') ||
					((unsigned char) c >= 'a' && (unsigned char) c <= 'f')
				);
	}
#endif

#ifdef DEBUG
	unsigned char ubf_value_of_ASCII_hex (const char c)
	{
		unsigned char	uc	= (unsigned char) c;
		
		uc =	uc >= '0' && uc <= '9' ? uc - '0' :
				uc >= 'a' && uc <= 'f' ? uc - 'a' + 10 :
				uc >= 'A' && uc <= 'F' ? uc - 'A' + 10 :
				UBF_INVALID_HEX_CHAR;							// Error.
		return uc;
	}
#endif

#ifdef DEBUG
	unsigned char ubf_value_of_ASCII_oct (const char c)
	{
		unsigned char	uc	= (unsigned char) c;
		
		return uc >= '0' && uc <= '7' ? uc - '0' : UBF_INVALID_OCT_CHAR;
	}
#endif

unsigned char ubf_value_of_ASCII_bin (const char c)
{
	unsigned char	uc	= (unsigned char) c;
	
	return '0' == uc || '1' == uc ? uc - '0' : UBF_INVALID_BIN_CHAR;
}

size_t ubf_octet_from_hex (unsigned char *o, const char *chHx)
{
	unsigned char	c1;
	unsigned char	c0;
	
	c1 = ubf_value_of_ASCII_hex (*chHx);
	++ chHx;
	if (UBF_INVALID_HEX_CHAR == c1)
	{
		return 0;
	}
	ubf_assert (0 == ('\xF0' & c1));
	c0 = ubf_value_of_ASCII_hex (*chHx);
	if (UBF_INVALID_HEX_CHAR == c0)
	{
		*o = c1;
		return 1;
	}
	c1 <<= 4;
	ubf_assert (0 == ('\xF0' & c0));
	c1 += c0;
	*o = c1;
	return 2;
}

size_t ubf_octet_from_oct (unsigned char *o, const char *chOc)
{
	unsigned char	c2;
	unsigned char	c1;
	unsigned char	c0;
	
	c2 = ubf_value_of_ASCII_oct (*chOc);
	++ chOc;
	if (UBF_INVALID_OCT_CHAR == c2)
	{
		return 0;
	}
	ubf_assert (0 == ('\xF8' & c2));
	c1 = ubf_value_of_ASCII_oct (*chOc);
	++ chOc;
	if (UBF_INVALID_OCT_CHAR == c1)
	{
		*o = c2;
		return 1;
	}
	ubf_assert (0 == ('\xF8' & c1));
	c2 <<= 3;
	c2 += c1;
	c0 = ubf_value_of_ASCII_oct (*chOc);
	if (UBF_INVALID_OCT_CHAR == c0)
	{
		*o = c2;
		return 2;
	}
	if (c2 << 3 <= 255 - c0)
	{
		ubf_assert (0 == ('\xF8' & c0));
		c2 <<= 3;
		c2 += c0;
		*o = c2;
		return 3;
	}
	*o = c2;
	return 2;
}

size_t ubf_octet_from_bin (unsigned char *o, const char *chOb)
{
	unsigned char	ucb	= 0;
	unsigned char	uct;
	unsigned int	ui	= 8;
	
	ubf_assert (1 == sizeof (unsigned char));
	while (ui)
	{
		uct = ubf_value_of_ASCII_bin (*chOb);
		++ chOb;
		if (UBF_INVALID_BIN_CHAR == uct)
			break;
		ucb <<= 1;
		ucb += uct;
		-- ui;
	}
	*o = ucb;
	return 8 - ui;
}

#ifdef _MSC_VER
#pragma warning (disable: 4706)									// Assignment within conditional expression.
#endif

size_t ubf_uint16_from_hex (uint16_t *pui16, const char *chHx16)
{
	size_t		stRet = 0;
	uint16_t	uiRet = 0;
	unsigned char	c = UBF_INVALID_HEX_CHAR;

	while	(
					stRet < UINT16_ASCII_HEX_DIGITS
				&&	(UBF_INVALID_HEX_CHAR != (c = ubf_value_of_ASCII_hex (chHx16 [stRet])))
			)
	{
		uiRet <<= 4;
		uiRet += c;
		++ stRet;
	}
	if (stRet && pui16)
		*pui16 = uiRet;
	return stRet;
}

size_t ubf_uint32_from_hex (uint32_t *pui32, const char *chHx32)
{
	size_t		stRet	= 0;
	uint32_t	uiRet	= 0;
	unsigned char	c	= UBF_INVALID_HEX_CHAR;
	
	while	(
					stRet < UINT32_ASCII_HEX_DIGITS
				&&	(UBF_INVALID_HEX_CHAR != (c = ubf_value_of_ASCII_hex (chHx32 [stRet])))
			)
	{
		uiRet <<= 4;
		uiRet += c;
		++ stRet;
	}
	if (stRet && pui32)
		*pui32 = uiRet;
	return stRet;
}

size_t ubf_uint64_from_hex (uint64_t *pui64, const char *chHx64)
{
	size_t		stRet	= 0;
	uint64_t	uiRet	= 0;
	unsigned char	c	= UBF_INVALID_HEX_CHAR;
	
	while	(
					stRet < UINT64_ASCII_HEX_DIGITS
				&&	(UBF_INVALID_HEX_CHAR != (c = ubf_value_of_ASCII_hex (chHx64 [stRet])))
			)
	{
		uiRet <<= 4;
		uiRet += c;
		++ stRet;
	}
	if (stRet && pui64)
		*pui64 = uiRet;
	return stRet;
}

uint64_t uint64_from_hex_str (const char *hexstr)
{
	uint64_t			u = 0;
	unsigned char		c;

	while ((c = (unsigned char) *hexstr ++))
	{
		if (c > '0' - 1 && c < '9' + 1)
		{
			c -= '0';
		} else
		if (c > 'a' - 1 && c < 'f' + 1)
		{
			c -= 'a';
			c += 10;
		} else
		if (c > 'A' - 1 && c < 'F' + 1)
		{
			c -= 'A';
			c += 10;
		}else
			return u;
		u <<= 4;
		u += c;
	}
	return u;
}

#ifdef _MSC_VER
#pragma warning (default: 4706)									// Assignment within conditional expression.
#endif

uint64_t uint64_from_dec_or_hex_str (const char *decorhex)
{
	uint64_t		ui;

	if (!strncmp (decorhex, "0x", 2) || !strncmp (decorhex, "0X", 2))
	{
		ui = uint64_from_hex_str (decorhex + 2);
	} else
	{
		ubf_uint64_from_str (&ui, decorhex);
	}
	return ui;
}

bool ubf_hex2bin	(
			unsigned char		*bin_output,					// The output.
			const size_t		bin_maxlen,						// Length of the output buffer,
																//	in octets (bytes).
			const char			*hex_input,						// The input in hexadecimal
																//	ASCII representation.
			const size_t		hex_len,						// Length of the input buffer,
																//	in octets (bytes).
			size_t *const		pbin_len						// The amount of bytes written.
					)
{
	const char				*read			= hex_input;
	unsigned char			*write			= bin_output;
	
	if (hex_len && 0 == (hex_len % 2))
	{	// The hexadecimal input string must be dividable by 2 since two hexadecimal
		//	digits are required per octet to write.
		unsigned char		cw;									// The octet to write.
		unsigned char		c;
		//size_t				bytes_max		= bin_maxlen;
		size_t				bytes_written	= 0;
		size_t				bytes_read		= hex_len;
		
		while (bin_maxlen && bytes_read > 1)
		{
			cw = ubf_value_of_ASCII_hex (read [0]);
			if (UBF_INVALID_HEX_CHAR == cw)
			{
				if (pbin_len)
					*pbin_len = bytes_written;
				return false;
			}
			cw <<= 4;											// This is the higher tetrade.
			c = ubf_value_of_ASCII_hex (read [1]);
			if (UBF_INVALID_HEX_CHAR == c)
			{
				if (pbin_len)
					*pbin_len = bytes_written;
				return false;
			}
			cw += c;											// This is the lower tetrade.
			*write = cw;
			read			+= 2;
			bytes_read		-= 2;
			//bytes_max		-= 2;
			bytes_written	+= 1;
			write			+= 1;
		}
		if (pbin_len)
			*pbin_len = bytes_written;
		return true;
	}
	if (pbin_len)
		*pbin_len = 0;
	return false;
}

// Used by the hex conversion functions. No NUL-terminator is stored.
DISABLE_WARNING_ARRAY_TOO_SMALL_FOR_NUL_TERMINATOR ()
static char				hexASCIIu [16]		= "0123456789ABCDEF";
static char				hexASCIIl [16]		= "0123456789abcdef";
DEFAULT_WARNING_ARRAY_TOO_SMALL_FOR_NUL_TERMINATOR ()

void asc_hex_from_octet (char *pc, uint8_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++	= hexASCIIu [((ui & 0xF0) >> 4) & 0x0F];
	*pc		= hexASCIIu [((ui & 0x0F))];
}

void asc_hex_from_octet_lower (char *pc, uint8_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++	= hexASCIIl [((ui & 0xF0) >> 4) & 0x0F];
	*pc		= hexASCIIl [((ui & 0x0F))];
}

void URI_encode_hex_from_octet (char *pc, uint8_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++ = '%';
	*pc ++	= hexASCIIu [((ui & 0xF0) >> 4) & 0x0F];
	*pc		= hexASCIIu [((ui & 0x0F))];
}

void URI_encode_hex_from_octet_lower (char *pc, uint8_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++ = '%';
	*pc ++	= hexASCIIl [((ui & 0xF0) >> 4) & 0x0F];
	*pc		= hexASCIIl [((ui & 0x0F))];
}

void asc_hex_from_word (char *pc, uint16_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++	= hexASCIIu [((ui & 0xF000) >> 12) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x0F00) >>  8) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x00F0) >>  4) & 0x0F];
	*pc		= hexASCIIu [((ui & 0x000F))];
}

void asc_hex_from_word_lower (char *pc, uint16_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++	= hexASCIIl [((ui & 0xF000) >> 12) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x0F00) >>  8) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x00F0) >>  4) & 0x0F];
	*pc		= hexASCIIl [((ui & 0x000F))];
}

void asc_hex_from_dword (char *pc, uint32_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++	= hexASCIIu [((ui & 0xF0000000) >> 28) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x0F000000) >> 24) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x00F00000) >> 20) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x000F0000) >> 16) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x0000F000) >> 12) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x00000F00) >>  8) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x000000F0) >>  4) & 0x0F];
	*pc		= hexASCIIu [((ui & 0x0000000F))];
}

void asc_hex_from_dword_lower (char *pc, uint32_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++	= hexASCIIl [((ui & 0xF0000000) >> 28) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x0F000000) >> 24) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x00F00000) >> 20) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x000F0000) >> 16) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x0000F000) >> 12) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x00000F00) >>  8) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x000000F0) >>  4) & 0x0F];
	*pc		= hexASCIIl [((ui & 0x0000000F))];
}

void asc_hex_from_qword (char *pc, uint64_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++	= hexASCIIu [((ui & 0xF000000000000000) >> 60) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x0F00000000000000) >> 56) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x00F0000000000000) >> 52) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x000F000000000000) >> 48) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x0000F00000000000) >> 44) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x00000F0000000000) >> 40) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x000000F000000000) >> 36) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x0000000F00000000) >> 32) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x00000000F0000000) >> 28) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x000000000F000000) >> 24) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x0000000000F00000) >> 20) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x00000000000F0000) >> 16) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x000000000000F000) >> 12) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x0000000000000F00) >>  8) & 0x0F];
	*pc ++	= hexASCIIu [((ui & 0x00000000000000F0) >>  4) & 0x0F];
	*pc		= hexASCIIu [((ui & 0x000000000000000F))];
}

void asc_hex_from_qword_lower (char *pc, uint64_t ui)
{
	ubf_assert_non_NULL (pc);

	*pc ++	= hexASCIIl [((ui & 0xF000000000000000) >> 60) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x0F00000000000000) >> 56) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x00F0000000000000) >> 52) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x000F000000000000) >> 48) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x0000F00000000000) >> 44) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x00000F0000000000) >> 40) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x000000F000000000) >> 36) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x0000000F00000000) >> 32) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x00000000F0000000) >> 28) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x000000000F000000) >> 24) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x0000000000F00000) >> 20) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x00000000000F0000) >> 16) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x000000000000F000) >> 12) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x0000000000000F00) >>  8) & 0x0F];
	*pc ++	= hexASCIIl [((ui & 0x00000000000000F0) >>  4) & 0x0F];
	*pc		= hexASCIIl [((ui & 0x000000000000000F))];
}

// The binary ASCII texts from 0000 to 1111. No NUL-terminator is stored.
DISABLE_WARNING_ARRAY_TOO_SMALL_FOR_NUL_TERMINATOR ()
static char				binASCII [4 * 16] = "0000000100100011010001010110011110001001101010111100110111101111";
DISABLE_WARNING_ARRAY_TOO_SMALL_FOR_NUL_TERMINATOR ()

void asc_bin_from_octet (char *pc, uint8_t ui)
{
	ubf_assert_non_NULL (pc);

	memcpy (pc, &binASCII [4 * (((ui & 0xF0) >> 4) & 0x0F)],		4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x0F)))], 4);
}

void asc_bin_from_word (char *pc, uint16_t ui)
{
	ubf_assert_non_NULL (pc);

	memcpy (pc, &binASCII [4 * (((ui & 0xF000) >> 12) & 0x0F)],		4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x0F00) >>  8) & 0x0F)],		4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x00F0) >>  4) & 0x0F)],		4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x000F)))],					4);
}

void asc_bin_from_dword (char *pc, uint32_t ui)
{
	ubf_assert_non_NULL (pc);

	memcpy (pc, &binASCII [4 * (((ui & 0xF0000000) >> 28) & 0x0F)],	4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x0F000000) >> 24) & 0x0F)],	4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x00F00000) >> 20) & 0x0F)],	4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x000F0000) >> 16) & 0x0F)],	4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x0000F000) >> 12) & 0x0F)],	4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x00000F00) >>  8) & 0x0F)],	4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x000000F0) >>  4) & 0x0F)],	4);		pc += 4;
	memcpy (pc, &binASCII [4 * (((ui & 0x0000000F)))],				4);
}


void ubf_hex_from_octet (char *chResult, uint8_t ui)
{
	char				*t		= chResult;

	ubf_assert (NULL != chResult);
	#ifdef DEBUG
		// Test for access violation in debug version.
		memset (chResult, UBF_ERROR_CHAR, 2 + 1);
	#endif
	// Looks a bit odd but provides all necessary steps.
	*t ++ = hexASCIIu [((ui & 0xF0) >> 4) & 0x0F];
	*t ++ = hexASCIIu [ui & 0x0F];
	*t = 0;
}

void ubf_hex_from_word (char *chResult, uint16_t ui)
{
	char				*t		= chResult;

	ubf_assert (NULL != chResult);
	#ifdef DEBUG
		// Test for access violation in debug version.
		memset (chResult, UBF_ERROR_CHAR, UBF_HEX64_STRSIZ);
	#endif
	*t ++ = hexASCIIu [((ui & 0xF000) >> 12) & 0x000F];
	*t ++ = hexASCIIu [((ui & 0x0F00) >>  8) & 0x000F];
	*t ++ = hexASCIIu [((ui & 0x00F0) >>  4) & 0x000F];
	*t ++ = hexASCIIu [ui & 0x000F];
	*t = 0;
}

void ubf_hex_from_dword (char *chResult, uint32_t ui)
{
	char				*t		= chResult;

	ubf_assert (NULL != chResult);
	#ifdef DEBUG
		// Test for access violation in debug version.
		memset (chResult, UBF_ERROR_CHAR, UBF_HEX32_STRSIZ);
	#endif
	*t ++ = hexASCIIu [((ui & 0xF0000000) >> 28) & 0x0000000F];
	*t ++ = hexASCIIu [((ui & 0x0F000000) >> 24) & 0x0000000F];
	*t ++ = hexASCIIu [((ui & 0x00F00000) >> 20) & 0x0000000F];
	*t ++ = hexASCIIu [((ui & 0x000F0000) >> 16) & 0x0000000F];
	*t ++ = hexASCIIu [((ui & 0x0000F000) >> 12) & 0x0000000F];
	*t ++ = hexASCIIu [((ui & 0x00000F00) >>  8) & 0x0000000F];
	*t ++ = hexASCIIu [((ui & 0x000000F0) >>  4) & 0x0000000F];
	*t ++ = hexASCIIu [ui & 0x0000000F];
	*t = 0;
}

void ubf_hex_from_qword (char *chResult, uint64_t ui)
{
	char				*t		= chResult;

	ubf_assert (NULL != chResult);
	#ifdef DEBUG
		// Test for access violation in debug version.
		memset (chResult, UBF_ERROR_CHAR, UBF_HEX64_STRSIZ);
	#endif
	*t ++ = hexASCIIu [((ui & 0xF000000000000000) >> 60) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x0F00000000000000) >> 56) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x00F0000000000000) >> 52) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x000F000000000000) >> 48) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x0000F00000000000) >> 44) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x00000F0000000000) >> 40) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x000000F000000000) >> 36) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x0000000F00000000) >> 32) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x00000000F0000000) >> 28) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x000000000F000000) >> 24) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x0000000000F00000) >> 20) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x00000000000F0000) >> 16) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x000000000000F000) >> 12) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x0000000000000F00) >>  8) & 0x000000000000000F];
	*t ++ = hexASCIIu [((ui & 0x00000000000000F0) >>  4) & 0x000000000000000F];
	*t ++ = hexASCIIu [ui & 0x000000000000000F];
	*t = 0;
}

void asc_hex_zero_from_qword (char *chResult, size_t stWidthResult, uint64_t ui)
{
	char	chValue [UBF_HEX64_STRSIZ];
	char	*pVal	= chValue;

	if (stWidthResult)
	{
		asc_hex_from_qword (pVal, ui);
		if (stWidthResult > UBF_HEX64_STRLEN)
		{
			stWidthResult -= UBF_HEX64_STRLEN;
			memset (chResult, '0', stWidthResult);
			memcpy (chResult + stWidthResult, pVal, UBF_HEX64_STRLEN);
		} else
		if (stWidthResult < UBF_HEX64_STRLEN)
			memcpy (chResult, pVal + UBF_HEX64_STRLEN - stWidthResult, stWidthResult);
		else
			memcpy (chResult, pVal, stWidthResult + 1);
	}
}

void ubf_hex_str_zero_from_qword (char *chResult, size_t stWidthResult, uint64_t ui)
{
	// Space for a 64 bit value in ASCII plus the memory debug header and footer.
	//char		chValue [UBFMEMDBG_CHARS (UBF_HEX64_STRSIZ)];
	char		chValue [UBF_HEX64_STRSIZ];
	char		*pVal;
	
	ubf_mem_init_ptr (chValue, UBF_HEX64_STRSIZ);
	pVal = ubf_mem_usr_ptr (chValue);
	
	if (stWidthResult)
	{
		ubf_hex_from_qword (pVal, ui);
		if (stWidthResult > UBF_HEX64_STRLEN)
		{
			stWidthResult -= UBF_HEX64_STRLEN;
			memset (chResult, '0', stWidthResult);
			memcpy (chResult + stWidthResult, pVal, UBF_HEX64_STRSIZ);
		} else
		if (stWidthResult < UBF_HEX64_STRLEN)
			memcpy (chResult, pVal + UBF_HEX64_STRLEN - stWidthResult, stWidthResult + 1);
		else
			memcpy (chResult, pVal, stWidthResult + 1);
	} else
		*chResult = '\0';
}

void ubf_hex_str_blank_from_qword (char *chResult, size_t stWidthResult, uint64_t ui)
{
	char		*ch;
	
	ubf_hex_str_zero_from_qword (chResult, stWidthResult, ui);
	ch = chResult;
	while ('0' == *ch)
	{
		*ch = ' ';
		++ ch;
	}
}

bool UCS2UTF16bin_from_hex	(
			uint16_t			*u16result,						// The output as 16 bit UCS-2 or
																//	UTF-16.
			size_t				max_u16result,					// The size of the buffer
																//	u16result points to, in
																//	uint16_t (NOT octets).
			const char			*cchex,							// The input string consisting
																//	of hexadecimal ASCII characters,
																//	4 hexadecimal ASCII characters
																//	for each uint16_t (UCS-2 or
																//	UTF-16) written to u16result.
			size_t				len_cchex,						// The length of the input string,
																//	excluding NUL.
			size_t				*pU16written
							)
{
	size_t		written		= 0;

	if (len_cchex && cchex && 0 == (len_cchex % 4))
	{
		unsigned char	co [2];
		uint16_t		u16;
		const char		*cc		= cchex;
		size_t			lcc		= len_cchex;
		size_t			sw;
		bool			b;

		while (lcc)
		{
			b = ubf_hex2bin (co, 2, cc, 4, &sw);
			if (b)
			{
				if (max_u16result)
				{
					-- max_u16result;
					// Comes back as big endian but we don't know the endianness of the
					//	target plattform. Fix that here. First octet is the MSB, second
					//	octet is the LSB.
					u16 = (co [0] << 8) + co [1];				// Endianness of target platform.
					written	+= sw / 2;
					cc		+= 4;
					lcc		-= 4;
					if (u16result)
					{
						*u16result = u16;
						++ u16result;
					}
				} else
					break;
			} else
			{
				if (pU16written)
					*pU16written = written;
				return false;			
			}
		}
		if (pU16written)
			*pU16written = written;
		return true;		
	}
	if (pU16written)
		*pU16written = 0;
	return false;
}

void ubf_hex_simple_hash	(
				unsigned char	*pHash,		size_t	stLenHash,	// Hash and its length.
				const char		*pData,		size_t	stLenData	// Data and its length.
							)
{
	UNREFERENCED_PARAMETER (stLenHash);
	ubf_assert (32 == stLenHash);								// We expect a fixed length.
	ubf_assert (8 == sizeof (int64_t));							// We got to use this 4 times.
	
	int64_t			iHash	= 4711;
	signed char		c;
	size_t			size	= stLenData;
	const char		*p		= pData;
	
	while (size --)
	{
		c = (signed char) *p;
		iHash = c + (iHash << 6) + (iHash << 16) - iHash;
		++ p;
	}
	// The first 8 octets.
	memcpy (pHash, &iHash, sizeof (int64_t));
	size = stLenData / 2;
	p = pData;
	while (size --)
	{
		c = (signed char) *p;
		iHash = c + (iHash << 6) + (iHash << 16) - iHash;
		++ p;
	}
	// The second 8 octets.
	memcpy (pHash + sizeof (int64_t), &iHash, sizeof (int64_t));
	size = stLenData / 2;
	p = pData + size;
	while (size --)
	{
		c = (signed char) *p;
		iHash = c + (iHash << 6) + (iHash << 16) - iHash;
		++ p;
	}
	// The third 8 octets.
	memcpy (pHash + sizeof (int64_t) + sizeof (int64_t), &iHash, sizeof (int64_t));
	size = stLenData / 4;
	p = pData + (2 * size);
	while (size --)
	{
		c = (signed char) *p;
		iHash = c + (iHash << 6) + (iHash << 16) - iHash;
		++ p;
	}
	// The last (fourth) 8 octets.
	memcpy (pHash + sizeof (int64_t) + sizeof (int64_t) + sizeof (int64_t), &iHash, sizeof (int64_t));
}

#ifdef UBF_HEX_BUILD_TEST_FUNCTION
	bool ubf_hex_test_function (void)
	{
		bool			b			= true;					// Our return value.
		char			chT [32];
		unsigned char	o;
		size_t			st;

		ubf_expect_bool_AND (b, ubf_is_octal_digit ('0'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit ('0'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit ('1'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit ('2'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit ('3'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit ('4'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit ('5'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit ('6'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit ('7'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit ('8'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit ('9'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit ('A'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit ('Z'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit ('.'));
		
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('0'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('0'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('1'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('2'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('3'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('4'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('5'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('6'));
		ubf_expect_bool_AND (b, ubf_is_octal_digit_macro ('7'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit_macro ('8'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit_macro ('9'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit_macro ('A'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit_macro ('Z'));
		ubf_expect_bool_AND (b, !ubf_is_octal_digit_macro ('.'));
		
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('0'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('1'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('2'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('3'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('4'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('5'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('6'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('7'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('8'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('9'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('A'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('B'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('C'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('D'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('E'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('F'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('a'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('b'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('c'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('d'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('e'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit ('f'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit ('g'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit ('z'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit ('G'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit ('Z'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit ('.'));
		
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('0'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('1'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('2'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('3'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('4'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('5'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('6'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('7'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('8'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('9'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('A'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('B'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('C'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('D'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('E'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('F'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('a'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('b'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('c'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('d'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('e'));
		ubf_expect_bool_AND (b, ubf_is_hex_digit_macro ('f'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit_macro ('g'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit_macro ('z'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit_macro ('G'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit_macro ('Z'));
		ubf_expect_bool_AND (b, !ubf_is_hex_digit_macro ('.'));
		
		// Pretty much 100 % test coverage for the hexadecimal characters.
		ubf_expect_bool_AND (b,  0 == ubf_value_of_ASCII_hex ('0'));
		ubf_expect_bool_AND (b,  1 == ubf_value_of_ASCII_hex ('1'));
		ubf_expect_bool_AND (b,  2 == ubf_value_of_ASCII_hex ('2'));
		ubf_expect_bool_AND (b,  3 == ubf_value_of_ASCII_hex ('3'));
		ubf_expect_bool_AND (b,  4 == ubf_value_of_ASCII_hex ('4'));
		ubf_expect_bool_AND (b,  5 == ubf_value_of_ASCII_hex ('5'));
		ubf_expect_bool_AND (b,  6 == ubf_value_of_ASCII_hex ('6'));
		ubf_expect_bool_AND (b,  7 == ubf_value_of_ASCII_hex ('7'));
		ubf_expect_bool_AND (b,  8 == ubf_value_of_ASCII_hex ('8'));
		ubf_expect_bool_AND (b,  9 == ubf_value_of_ASCII_hex ('9'));
		ubf_expect_bool_AND (b, 10 == ubf_value_of_ASCII_hex ('a'));
		ubf_expect_bool_AND (b, 10 == ubf_value_of_ASCII_hex ('A'));
		ubf_expect_bool_AND (b, 11 == ubf_value_of_ASCII_hex ('b'));
		ubf_expect_bool_AND (b, 11 == ubf_value_of_ASCII_hex ('B'));
		ubf_expect_bool_AND (b, 12 == ubf_value_of_ASCII_hex ('c'));
		ubf_expect_bool_AND (b, 12 == ubf_value_of_ASCII_hex ('C'));
		ubf_expect_bool_AND (b, 13 == ubf_value_of_ASCII_hex ('d'));
		ubf_expect_bool_AND (b, 13 == ubf_value_of_ASCII_hex ('D'));
		ubf_expect_bool_AND (b, 14 == ubf_value_of_ASCII_hex ('e'));
		ubf_expect_bool_AND (b, 14 == ubf_value_of_ASCII_hex ('E'));
		ubf_expect_bool_AND (b, 15 == ubf_value_of_ASCII_hex ('f'));
		ubf_expect_bool_AND (b, 15 == ubf_value_of_ASCII_hex ('F'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex (' '));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('g'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('G'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('.'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('H'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('Z'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('\0'));

		ubf_expect_bool_AND (b,  0 == ubf_value_of_ASCII_hex_macro ('0'));
		ubf_expect_bool_AND (b,  1 == ubf_value_of_ASCII_hex_macro ('1'));
		ubf_expect_bool_AND (b,  2 == ubf_value_of_ASCII_hex_macro ('2'));
		ubf_expect_bool_AND (b,  3 == ubf_value_of_ASCII_hex_macro ('3'));
		ubf_expect_bool_AND (b,  4 == ubf_value_of_ASCII_hex_macro ('4'));
		ubf_expect_bool_AND (b,  5 == ubf_value_of_ASCII_hex_macro ('5'));
		ubf_expect_bool_AND (b,  6 == ubf_value_of_ASCII_hex_macro ('6'));
		ubf_expect_bool_AND (b,  7 == ubf_value_of_ASCII_hex_macro ('7'));
		ubf_expect_bool_AND (b,  8 == ubf_value_of_ASCII_hex_macro ('8'));
		ubf_expect_bool_AND (b,  9 == ubf_value_of_ASCII_hex_macro ('9'));
		ubf_expect_bool_AND (b, 10 == ubf_value_of_ASCII_hex_macro ('a'));
		ubf_expect_bool_AND (b, 10 == ubf_value_of_ASCII_hex_macro ('A'));
		ubf_expect_bool_AND (b, 11 == ubf_value_of_ASCII_hex_macro ('b'));
		ubf_expect_bool_AND (b, 11 == ubf_value_of_ASCII_hex_macro ('B'));
		ubf_expect_bool_AND (b, 12 == ubf_value_of_ASCII_hex_macro ('c'));
		ubf_expect_bool_AND (b, 12 == ubf_value_of_ASCII_hex_macro ('C'));
		ubf_expect_bool_AND (b, 13 == ubf_value_of_ASCII_hex_macro ('d'));
		ubf_expect_bool_AND (b, 13 == ubf_value_of_ASCII_hex_macro ('D'));
		ubf_expect_bool_AND (b, 14 == ubf_value_of_ASCII_hex_macro ('e'));
		ubf_expect_bool_AND (b, 14 == ubf_value_of_ASCII_hex_macro ('E'));
		ubf_expect_bool_AND (b, 15 == ubf_value_of_ASCII_hex_macro ('f'));
		ubf_expect_bool_AND (b, 15 == ubf_value_of_ASCII_hex_macro ('F'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex_macro (' '));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex_macro ('g'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex_macro ('G'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex_macro ('.'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex_macro ('H'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex_macro ('Z'));
		ubf_expect_bool_AND (b, UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex_macro ('\0'));

		// Let's do the same with the octal digits.
		ubf_expect_bool_AND (b,  0 == ubf_value_of_ASCII_oct ('0'));
		ubf_expect_bool_AND (b,  1 == ubf_value_of_ASCII_oct ('1'));
		ubf_expect_bool_AND (b,  2 == ubf_value_of_ASCII_oct ('2'));
		ubf_expect_bool_AND (b,  3 == ubf_value_of_ASCII_oct ('3'));
		ubf_expect_bool_AND (b,  4 == ubf_value_of_ASCII_oct ('4'));
		ubf_expect_bool_AND (b,  5 == ubf_value_of_ASCII_oct ('5'));
		ubf_expect_bool_AND (b,  6 == ubf_value_of_ASCII_oct ('6'));
		ubf_expect_bool_AND (b,  7 == ubf_value_of_ASCII_oct ('7'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct (' '));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('8'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('9'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('A'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('B'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('C'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('D'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('E'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('.'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('H'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('Z'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('\0'));
		
		ubf_expect_bool_AND (b,  0 == ubf_value_of_ASCII_oct_macro ('0'));
		ubf_expect_bool_AND (b,  1 == ubf_value_of_ASCII_oct_macro ('1'));
		ubf_expect_bool_AND (b,  2 == ubf_value_of_ASCII_oct_macro ('2'));
		ubf_expect_bool_AND (b,  3 == ubf_value_of_ASCII_oct_macro ('3'));
		ubf_expect_bool_AND (b,  4 == ubf_value_of_ASCII_oct_macro ('4'));
		ubf_expect_bool_AND (b,  5 == ubf_value_of_ASCII_oct_macro ('5'));
		ubf_expect_bool_AND (b,  6 == ubf_value_of_ASCII_oct_macro ('6'));
		ubf_expect_bool_AND (b,  7 == ubf_value_of_ASCII_oct_macro ('7'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro (' '));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('8'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('9'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('A'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('B'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('C'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('D'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('E'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('.'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('H'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('Z'));
		ubf_expect_bool_AND (b, UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct_macro ('\0'));
		
		// Binary.
		ubf_expect_bool_AND (b, 0 == ubf_value_of_ASCII_bin ('0'));
		ubf_expect_bool_AND (b, 1 == ubf_value_of_ASCII_bin ('1'));
		ubf_expect_bool_AND (b, UBF_INVALID_BIN_CHAR == ubf_value_of_ASCII_bin (' '));
		ubf_expect_bool_AND (b, UBF_INVALID_BIN_CHAR == ubf_value_of_ASCII_bin ('2'));
		ubf_expect_bool_AND (b, UBF_INVALID_BIN_CHAR == ubf_value_of_ASCII_bin ('\0'));
		
		strcpy (chT, "377");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_expect_bool_AND (b, 3 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 255 == o);

		strcpy (chT, "400");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_expect_bool_AND (b, 4 == o);
		o = ubf_value_of_ASCII_oct (chT [1]);
		ubf_expect_bool_AND (b, 0 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_expect_bool_AND (b, 2 == st);
		// 40o == 32d.
		ubf_expect_bool_AND (b, 32 == o);

		strcpy (chT, "401");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_expect_bool_AND (b, 4 == o);
		o = ubf_value_of_ASCII_oct (chT [1]);
		ubf_expect_bool_AND (b, 0 == o);
		o = ubf_value_of_ASCII_oct (chT [2]);
		ubf_expect_bool_AND (b, 1 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_expect_bool_AND (b, 2 == st);
		// 40o == 32d. The "1" in "401" is already ignored due to octet boundary.
		ubf_expect_bool_AND (b, 32 == o);

		strcpy (chT, "477");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_expect_bool_AND (b, 4 == o);
		o = ubf_value_of_ASCII_oct (chT [1]);
		ubf_expect_bool_AND (b, 7 == o);
		o = ubf_value_of_ASCII_oct (chT [2]);
		ubf_expect_bool_AND (b, 7 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_expect_bool_AND (b, 2 == st);
		// 47o == 39d.
		ubf_expect_bool_AND (b, 39 == o);

		strcpy (chT, "1110");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_expect_bool_AND (b, 1 == o);
		o = ubf_value_of_ASCII_oct (chT [1]);
		ubf_expect_bool_AND (b, 1 == o);
		o = ubf_value_of_ASCII_oct (chT [2]);
		ubf_expect_bool_AND (b, 1 == o);
		o = ubf_value_of_ASCII_oct (chT [3]);
		ubf_expect_bool_AND (b, 0 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_expect_bool_AND (b, 3 == st);
		// 111o == 73d == 49h == "I".
		ubf_expect_bool_AND (b, 73 == o);
		
		st = ubf_octet_from_bin (&o, "0");
		ubf_expect_bool_AND (b, 1 == st);
		ubf_expect_bool_AND (b, 0 == o);
		st = ubf_octet_from_bin (&o, "1");
		ubf_expect_bool_AND (b, 1 == st);
		ubf_expect_bool_AND (b, 1 == o);
		st = ubf_octet_from_bin (&o, "01");
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 1 == o);
		st = ubf_octet_from_bin (&o, "10");
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 2 == o);
		st = ubf_octet_from_bin (&o, "11");
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 3 == o);
		st = ubf_octet_from_bin (&o, "100");
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 4 == o);
		st = ubf_octet_from_bin (&o, "101");
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 5 == o);
		st = ubf_octet_from_bin (&o, "110");
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 6 == o);
		st = ubf_octet_from_bin (&o, "111");
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 7 == o);
		st = ubf_octet_from_bin (&o, "1000");
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 8 == o);
		st = ubf_octet_from_bin (&o, "1001");
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 9 == o);
		st = ubf_octet_from_bin (&o, "1010");
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 10 == o);
		st = ubf_octet_from_bin (&o, "1011");
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 11 == o);
		st = ubf_octet_from_bin (&o, "1100");
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 12 == o);
		st = ubf_octet_from_bin (&o, "1101");
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 13 == o);
		st = ubf_octet_from_bin (&o, "1110");
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 14 == o);
		st = ubf_octet_from_bin (&o, "1111");
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 15 == o);
		st = ubf_octet_from_bin (&o, "00000001");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 1 == o);
		st = ubf_octet_from_bin (&o, "000000001");				// Only 8 bits will be consumed.
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 0 == o);
		st = ubf_octet_from_bin (&o, "101010101");				// Only 8 bits will be consumed.
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 170 == o);
		st = ubf_octet_from_bin (&o, "00000010");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 2 == o);
		st = ubf_octet_from_bin (&o, "00000011");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 3 == o);
		st = ubf_octet_from_bin (&o, "00000100");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 4 == o);
		st = ubf_octet_from_bin (&o, "00010000");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 16 == o);
		st = ubf_octet_from_bin (&o, "00010001");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 17 == o);
		st = ubf_octet_from_bin (&o, "00100000");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 32 == o);
		st = ubf_octet_from_bin (&o, "00100001");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 33 == o);
		st = ubf_octet_from_bin (&o, "00100010");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 34 == o);
		st = ubf_octet_from_bin (&o, "00100011");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 35 == o);
		st = ubf_octet_from_bin (&o, "01000000");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 64 == o);
		st = ubf_octet_from_bin (&o, "10000000");
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, 128 == o);
		
		char szRes	[128];
		//uint8_t		ui8;

		// These functions do not write NUL terminators.
		asc_bin_from_octet (szRes, 0x00);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000000", szRes, 8));
		asc_bin_from_octet (szRes, 0x01);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000001", szRes, 8));
		asc_bin_from_octet (szRes, 0x02);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000010", szRes, 8));
		asc_bin_from_octet (szRes, 0x03);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000011", szRes, 8));
		asc_bin_from_octet (szRes, 0x04);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000100", szRes, 8));
		asc_bin_from_octet (szRes, 0x05);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000101", szRes, 8));
		asc_bin_from_octet (szRes, 0x06);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000110", szRes, 8));
		asc_bin_from_octet (szRes, 0x07);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000111", szRes, 8));
		asc_bin_from_octet (szRes, 0x08);
		ubf_expect_bool_AND (b, 0 == memcmp ("00001000", szRes, 8));
		asc_bin_from_octet (szRes, 0x09);
		ubf_expect_bool_AND (b, 0 == memcmp ("00001001", szRes, 8));
		asc_bin_from_octet (szRes, 0x0A);
		ubf_expect_bool_AND (b, 0 == memcmp ("00001010", szRes, 8));
		asc_bin_from_octet (szRes, 0x0B);
		ubf_expect_bool_AND (b, 0 == memcmp ("00001011", szRes, 8));
		asc_bin_from_octet (szRes, 0x0C);
		ubf_expect_bool_AND (b, 0 == memcmp ("00001100", szRes, 8));
		asc_bin_from_octet (szRes, 0x0D);
		ubf_expect_bool_AND (b, 0 == memcmp ("00001101", szRes, 8));
		asc_bin_from_octet (szRes, 0x0E);
		ubf_expect_bool_AND (b, 0 == memcmp ("00001110", szRes, 8));
		asc_bin_from_octet (szRes, 0x0F);
		ubf_expect_bool_AND (b, 0 == memcmp ("00001111", szRes, 8));
		asc_bin_from_octet (szRes, 0x10);
		ubf_expect_bool_AND (b, 0 == memcmp ("00010000", szRes, 8));
		asc_bin_from_octet (szRes, 0x11);
		ubf_expect_bool_AND (b, 0 == memcmp ("00010001", szRes, 8));
		asc_bin_from_octet (szRes, 0x12);
		ubf_expect_bool_AND (b, 0 == memcmp ("00010010", szRes, 8));
		asc_bin_from_octet (szRes, 0x13);
		ubf_expect_bool_AND (b, 0 == memcmp ("00010011", szRes, 8));
		asc_bin_from_octet (szRes, 0x15);
		ubf_expect_bool_AND (b, 0 == memcmp ("00010101", szRes, 8));
		asc_bin_from_octet (szRes, 0x16);
		ubf_expect_bool_AND (b, 0 == memcmp ("00010110", szRes, 8));
		asc_bin_from_octet (szRes, 0x19);
		ubf_expect_bool_AND (b, 0 == memcmp ("00011001", szRes, 8));
		asc_bin_from_octet (szRes, 0x20);
		ubf_expect_bool_AND (b, 0 == memcmp ("00100000", szRes, 8));
		asc_bin_from_octet (szRes, 0x22);
		ubf_expect_bool_AND (b, 0 == memcmp ("00100010", szRes, 8));
		asc_bin_from_octet (szRes, 0x24);
		ubf_expect_bool_AND (b, 0 == memcmp ("00100100", szRes, 8));
		asc_bin_from_octet (szRes, 0xA0);
		ubf_expect_bool_AND (b, 0 == memcmp ("10100000", szRes, 8));
		asc_bin_from_octet (szRes, 0xA1);
		ubf_expect_bool_AND (b, 0 == memcmp ("10100001", szRes, 8));
		asc_bin_from_octet (szRes, 0xFC);
		ubf_expect_bool_AND (b, 0 == memcmp ("11111100", szRes, 8));
		asc_bin_from_octet (szRes, 0xFD);
		ubf_expect_bool_AND (b, 0 == memcmp ("11111101", szRes, 8));
		asc_bin_from_octet (szRes, 0xFE);
		ubf_expect_bool_AND (b, 0 == memcmp ("11111110", szRes, 8));
		asc_bin_from_octet (szRes, 0xFF);
		ubf_expect_bool_AND (b, 0 == memcmp ("11111111", szRes, 8));

		asc_bin_from_word (szRes, 0x0000);
		ubf_expect_bool_AND (b, 0 == memcmp ("0000000000000000", szRes, 8));
		asc_bin_from_word (szRes, 0x1111);
		ubf_expect_bool_AND (b, 0 == memcmp ("0001000100010001", szRes, 8));
		asc_bin_from_word (szRes, 0x2222);
		ubf_expect_bool_AND (b, 0 == memcmp ("0010001000100010", szRes, 8));
		asc_bin_from_word (szRes, 0x3333);
		ubf_expect_bool_AND (b, 0 == memcmp ("0011001100110011", szRes, 8));
		asc_bin_from_word (szRes, 0xAAAA);
		ubf_expect_bool_AND (b, 0 == memcmp ("1010101010101010", szRes, 8));
		asc_bin_from_word (szRes, 0xAAAB);
		ubf_expect_bool_AND (b, 0 == memcmp ("1010101010101011", szRes, 8));
		asc_bin_from_word (szRes, 0xAABB);
		ubf_expect_bool_AND (b, 0 == memcmp ("1010101010111011", szRes, 8));
		asc_bin_from_word (szRes, 0xABBB);
		ubf_expect_bool_AND (b, 0 == memcmp ("1010101110111011", szRes, 8));
		asc_bin_from_word (szRes, 0xBBBB);
		ubf_expect_bool_AND (b, 0 == memcmp ("1011101110111011", szRes, 8));
		asc_bin_from_word (szRes, 0xFFFF);
		ubf_expect_bool_AND (b, 0 == memcmp ("1111111111111111", szRes, 8));

		asc_bin_from_dword (szRes, 0x00000000);
		ubf_expect_bool_AND (b, 0 == memcmp ("00000000000000000000000000000000", szRes, 8));
		asc_bin_from_dword (szRes, 0x11111111);
		ubf_expect_bool_AND (b, 0 == memcmp ("00010001000100010001000100010001", szRes, 8));
		asc_bin_from_dword (szRes, 0x22222222);
		ubf_expect_bool_AND (b, 0 == memcmp ("00100010001000100010001000100010", szRes, 8));
		asc_bin_from_dword (szRes, 0x33333333);
		ubf_expect_bool_AND (b, 0 == memcmp ("00110011001100110011001100110011", szRes, 8));
		asc_bin_from_dword (szRes, 0xAAAAAAAA);
		ubf_expect_bool_AND (b, 0 == memcmp ("10101010101010101010101010101010", szRes, 8));
		asc_bin_from_dword (szRes, 0xAAABAAAB);
		ubf_expect_bool_AND (b, 0 == memcmp ("10101010101010111010101010101011", szRes, 8));
		asc_bin_from_dword (szRes, 0xAABBAABB);
		ubf_expect_bool_AND (b, 0 == memcmp ("10101010101110111010101010111011", szRes, 8));
		asc_bin_from_dword (szRes, 0xABBBABBB);
		ubf_expect_bool_AND (b, 0 == memcmp ("10101011101110111010101110111011", szRes, 8));
		asc_bin_from_dword (szRes, 0xBBBBBBBB);
		ubf_expect_bool_AND (b, 0 == memcmp ("10111011101110111011101110111011", szRes, 8));
		asc_bin_from_dword (szRes, 0xFFFFBBBB);
		ubf_expect_bool_AND (b, 0 == memcmp ("11111111111111111011101110111011", szRes, 8));
		asc_bin_from_dword (szRes, 0xFFFFFFFF);
		ubf_expect_bool_AND (b, 0 == memcmp ("11111111111111111111111111111111", szRes, 8));

		// UCS2UTF16bin_from_hex.
		uint16_t		bin [128];
		char			*hex;
		
		bool			b2;

		hex = "003100320033";
		ubf_expect_bool_AND (b, false == UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 0, &st));
		// Should come back false.
		ubf_expect_bool_AND (b, 0 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 1, &st);
		ubf_expect_bool_AND (b, false == b2);
		ubf_expect_bool_AND (b, 0 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 2, &st);
		ubf_expect_bool_AND (b, false == b2);
		ubf_expect_bool_AND (b, 0 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 3, &st);
		ubf_expect_bool_AND (b, false == b2);
		ubf_expect_bool_AND (b, 0 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 4, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 1 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 5, &st);
		ubf_expect_bool_AND (b, false == b2);
		ubf_expect_bool_AND (b, 0 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 6, &st);
		ubf_expect_bool_AND (b, false == b2);
		ubf_expect_bool_AND (b, 0 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 7, &st);
		ubf_expect_bool_AND (b, false == b2);
		ubf_expect_bool_AND (b, 0 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 8, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 2 == st);
		b2 = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 12, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 3 == st);
		b2 = UCS2UTF16bin_from_hex (bin, 0, hex, 12, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 0 == st);
		b2 = UCS2UTF16bin_from_hex (bin, 1, hex, 12, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 1 == st);
		ubf_expect_bool_AND (b, 0x31 == bin [0]);
		b2 = UCS2UTF16bin_from_hex (bin, 2, hex, 12, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 0x31 == bin [0]);
		ubf_expect_bool_AND (b, 0x32 == bin [1]);
		b2 = UCS2UTF16bin_from_hex (bin, 3, hex, 12, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 0x31 == bin [0]);
		ubf_expect_bool_AND (b, 0x32 == bin [1]);
		ubf_expect_bool_AND (b, 0x33 == bin [2]);
		b2 = UCS2UTF16bin_from_hex (bin, 4, hex, 12, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 0x31 == bin [0]);
		ubf_expect_bool_AND (b, 0x32 == bin [1]);
		ubf_expect_bool_AND (b, 0x33 == bin [2]);
		// "@AB";
		hex = "004000410042";
		b2 = UCS2UTF16bin_from_hex (bin, 4, hex, 12, &st);
		ubf_expect_bool_AND (b, true == b2);
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 0x40 == bin [0]);
		ubf_expect_bool_AND (b, 0x41 == bin [1]);
		ubf_expect_bool_AND (b, 0x42 == bin [2]);
		
		// Tests for ubf_uint16_from_hex ().
		hex = "FFFF";
		uint16_t		ui16;
		uint32_t		ui32;
		uint64_t		ui64;
		
		ubf_expect_bool_AND (b, sizeof (uint16_t) == sizeof (wchar_t));
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 65535 == ui16);
		hex = "FF";
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 255 == ui16);
		hex = "10";
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 16 == ui16);
		hex = "100";
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 256 == ui16);
		hex = "FFF0";
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 65520 == ui16);
		hex = "";
		ui16 = 17;	// Should stay because no hex digit means the function should not
					//	change it.
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_expect_bool_AND (b, 0 == st);
		ubf_expect_bool_AND (b, 17 == ui16);
		
		unsigned char c;
		char ch [10];
		for (c = (unsigned char) '0'; c <= (unsigned char) '9'; c ++)
		{
			ch [0] = c;
			ch [1] = '\0';
			st = ubf_uint16_from_hex (&ui16, ch);
			ubf_expect_bool_AND (b, 1 == st);
			ubf_expect_bool_AND (b, (uint16_t) (c - (unsigned char) '0') == ui16);
		}
		for (c = (unsigned char) 'a'; c < (unsigned char) 'g'; c ++)
		{
			ch [0] = c;
			ch [1] = '\0';
			st = ubf_uint16_from_hex (&ui16, ch);
			ubf_expect_bool_AND (b, 1 == st);
			ubf_expect_bool_AND (b, (uint16_t) (10 + c - (unsigned char) 'a') == ui16);
		}
		for (c = (unsigned char) '0'; c <= (unsigned char) '9'; c ++)
		{
			ch [0] = c;
			ch [1] = '\0';
			st = ubf_uint32_from_hex (&ui32, ch);
			ubf_expect_bool_AND (b, 1 == st);
			ubf_expect_bool_AND (b, (uint32_t) (c - (unsigned char) '0') == ui32);
		}
		for (c = (unsigned char) 'a'; c < (unsigned char) 'g'; c ++)
		{
			ch [0] = c;
			ch [1] = '\0';
			st = ubf_uint64_from_hex (&ui64, ch);
			ubf_expect_bool_AND (b, 1 == st);
			ubf_expect_bool_AND (b, (uint64_t) (10 + (uint64_t) c - (unsigned char) 'a') == ui64);
		}
		
		hex = "FFFF";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 65535 == ui32);
		hex = "FF";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 255 == ui32);
		hex = "10";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 16 == ui32);
		hex = "100";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 256 == ui32);
		hex = "FFF0";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 65520 == ui32);
		hex = "";
		ui32 = 17;	// Should stay because no hex digit means the function should not
					//	change it.
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_expect_bool_AND (b, 0 == st);
		ubf_expect_bool_AND (b, 17 == ui32);
		hex = "FFFFFFFF";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, UINT32_MAX == ui32);

		hex = "FFFF";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 65535 == ui64);
		hex = "FF";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 255 == ui64);
		hex = "10";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 2 == st);
		ubf_expect_bool_AND (b, 16 == ui64);
		hex = "100";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 3 == st);
		ubf_expect_bool_AND (b, 256 == ui64);
		hex = "FFF0";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 4 == st);
		ubf_expect_bool_AND (b, 65520 == ui64);
		hex = "";
		ui64 = 17;	// Should stay because no hex digit means the function should not
					//	change it.
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 0 == st);
		ubf_expect_bool_AND (b, 17 == ui64);
		hex = "FFFFFFFF";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 8 == st);
		ubf_expect_bool_AND (b, UINT32_MAX == ui64);
		hex = "FFFFFFFFFFFFFFFF";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 16 == st);
		ubf_expect_bool_AND (b, UINT64_MAX == ui64);
		hex = "FFFFFFFFFFFFFFF0";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_expect_bool_AND (b, 16 == st);
		ubf_expect_bool_AND (b, UINT64_MAX - 15 == ui64);

		return b;
	}
#endif
/****************************************************************************************

	File:		strlineextract.h
	Why:		Line extractor.
	OS:			C99.
	Author:		Thomas
	Created:	2022-03-31

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2022-03-31	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strlineextract.h"
	#include "./strnewline.h"
	#include "./ubfcharscountsandchecks.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
		#include "./ArrayMacros.h"
	#else
		#include "./../dbg/ubfdebug.h"
		#include "./../pre/ArrayMacros.h"
	#endif

#endif

void InitSTRLINEINF (STRLINEINF *pi, void *pCustom)
{
	ubf_assert_non_NULL (pi);

	pi->absPosition				= 1;
	pi->charNumber				= 0;
	pi->lineNumber				= 0;
	pi->pCustom					= pCustom;
	pi->pStart					= NULL;
	pi->lnLength				= 0;
	#ifdef DEBUG
		pi->bInitialised		= true;
	#endif
}

char *ccCLineComment []	= {"//"};
char *ccULineComment []	= {"//", "#", ";", "+", "-", "!"};
char *ccSMultComment [] = {"/*"};
char *ccEMultComment [] = {"*/"};

void InitSTRLINECONFforUBFL (STRLINECONF *pc)
{
	ubf_assert_non_NULL (pc);
	pc->CharacterSet			= EN_STRLINEEXTRACT_UTF8;
	pc->tabSize					= 4;
	pc->pchLineCommentStr		= ccULineComment;
	pc->nLineCommentStr			= GET_ARRAY_LEN (ccULineComment);
	pc->pchStartMultiCommentStr	= ccSMultComment;
	pc->pchEndMultiCommentStr	= ccEMultComment;
	pc->nMultiCommentStr		= GET_ARRAY_LEN (ccSMultComment);
	#ifdef DEBUG
		pc->bInitialised		= true;
	#endif
}

void InitSTRLINECONFforC (STRLINECONF *pc)
{
	ubf_assert_non_NULL (pc);
	pc->CharacterSet			= EN_STRLINEEXTRACT_UTF8;
	pc->tabSize					= 4;
	pc->pchLineCommentStr		= ccCLineComment;
	pc->nLineCommentStr			= GET_ARRAY_LEN (ccCLineComment);
	pc->pchStartMultiCommentStr	= ccSMultComment;
	pc->pchEndMultiCommentStr	= ccEMultComment;
	pc->nMultiCommentStr		= GET_ARRAY_LEN (ccSMultComment);
	#ifdef DEBUG
		pc->bInitialised		= true;
	#endif
}
/*
	Checks that every start of a multi-line comment has a partner string
	in the array that contains the end of multi-line comments.
*/
bool SanityCheckMultiComments (STRLINECONF *pc)
{
	ubf_assert_non_NULL (pc);

	if (0 == pc->nMultiCommentStr)
		return true;
	if (pc && pc->pchStartMultiCommentStr && pc->pchEndMultiCommentStr)
	{
		size_t		n;

		for (n = 0; n < pc->nMultiCommentStr; ++ n)
		{	// Each start of a multi-line comment needs an end of a multi-line comment.
			ubf_assert_non_NULL (pc->pchStartMultiCommentStr [n]);
			ubf_assert_non_NULL (pc->pchEndMultiCommentStr [n]);
			if (NULL == pc->pchStartMultiCommentStr [n] && NULL == pc->pchEndMultiCommentStr [n])
				return false;
		}
		return true;
	}
	return false;
}

/*
	Returns true if the buffer p points to starts with the NUL terminated string
	sz. The parameter l is the length of the buffer p.

	Returns false if p is NULL.
	Returns false if l is 0.
	Returns false if sz is NULL or has no length.
*/
bool cmpBufStartsWith (const char *p, size_t l, const char *sz)
{
	ubf_assert (NULL != p);
	ubf_assert (NULL != sz);

	size_t	lc	= sz ? strlen (sz) : 0;
	if (p && lc && l && l >= lc)
	{
		return 0 == memcmp (p, sz, lc);
	}
	return false;
}

/*
	Returns the index of the found start of a multi-line comment + 1, or 0 if pb does not
	start with the start of a multi-line comment.
*/
unsigned int isStartMultiLineComment (char *pb, size_t lb, STRLINECONF *pc)
{
	ubf_assert (NULL != pb);
	ubf_assert (NULL != pc);

	unsigned int n;

	if (pc && pc->pchStartMultiCommentStr)
	{
		for (n = 0; n < pc->nMultiCommentStr; ++ n)
		{
			if (cmpBufStartsWith (pb, lb, pc->pchStartMultiCommentStr [n]))
				return n + 1;
		}
	}
	return 0;
}

/*
	Returns true if pb starts with the end string of the multi-line comment uMCn
	refers to. The parameter uMCn is the index of the start of a multi-line comment + 1
	as returned by isStartMultiLineComment ().
*/
bool isEndMultiLineComment (char *pb, size_t lb, STRLINECONF *pc, unsigned int uMCn)
{
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (pc);
	ubf_assert (0 < uMCn);
	
	if (pc && pc->pchEndMultiCommentStr && uMCn && pc->nMultiCommentStr >= uMCn)
	{
		return cmpBufStartsWith (pb, lb, pc->pchEndMultiCommentStr [uMCn - 1]);
	}
	return false;
}

/*
	Steps pb on to the next line and updates the STRLINEINF structure pi points
	to as it goes along.
*/
void nextLine (char **pb, size_t *pl, STRLINEINF *pi)
{
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (*pb);
	ubf_assert_non_NULL (pl);
	ubf_assert_non_NULL (pi);

	char			*ch	= *pb;
	unsigned int	nls	= 0;
	size_t			jmp	= 0;
	size_t			l	= *pl;

	while (l && 0 == (nls = strIsNewLine (ch, l, &jmp)))
	{
		-- l;
		++ ch;
		++ pi->absPosition;
	}
	if (nls)
	{
		l	-= jmp;
		ch	+= jmp;
		pi->absPosition += jmp;
		pi->charNumber = 1;
	}
	*pl = l;
	*pb = ch;
	pi->lineNumber += nls;
}

/*
	Swallows single-line comments up to the next line. Returns true if a line
	comment was swallowed.
*/
bool swallowLineComment (char **pb, size_t *pl, STRLINECONF *pc, STRLINEINF *pi)
{
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (*pb);
	ubf_assert_non_NULL (pc);
	ubf_assert_non_NULL (pi);

	unsigned int n	= 0;

	if (pc && pc->nLineCommentStr)
	{
		for (n = 0; n < pc->nLineCommentStr; ++ n)
		{
			if (cmpBufStartsWith (*pb, *pl, pc->pchLineCommentStr [n]))
			{
				nextLine (pb, pl, pi);
				// *pb should now point to the first character of a new line.
				return true;
			}
		}
	}
	return false;
}

/*
	Swallows a multi-line/block comment. Returns true when a block comment was swallowed.
*/
bool swallowMultiComment (char **pb, size_t *pl, STRLINECONF *pc, STRLINEINF *pi)
{
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (*pb);
	ubf_assert_non_NULL (pc);
	ubf_assert_non_NULL (pi);

	unsigned int	idx;
	size_t			l;
	unsigned int	nls;
	size_t			jmp;

	if (0 < (idx = isStartMultiLineComment (*pb, *pl, pc)))
	{
		l = strlen (pc->pchStartMultiCommentStr [idx - 1]);
		*pb += l;
		*pl -= l;
		++ pi->absPosition;
		while (*pl && **pb)
		{
			if (isEndMultiLineComment (*pb, *pl, pc, idx))
			{
				l = strlen (pc->pchEndMultiCommentStr [idx - 1]);
				*pb += l;
				*pl -= l;
				++ pi->absPosition;
				return true;
			} else
			{
				nls = strIsNewLine (*pb, *pl, &jmp);
				if (nls)
				{
					pi->lineNumber	+= nls;
					pi->absPosition	+= jmp;
					pi->charNumber = 1;
					*pb += jmp;
					*pl -= jmp;
				} else
				{
					*pb += 1;
					*pl -= 1;
					++ pi->absPosition;
				}
			}
		}
		ubf_assert_msg (false, "Error: End of block comment missing.");
	}
	return false;
}

/*
	Returns true when white space or a new line was swallowed.
*/
bool swallowEmptyAndWhiteSpaceLines (char **pb, size_t *pl, STRLINEINF *pi)
{
	ubf_assert_non_NULL (pb);
	ubf_assert_non_NULL (*pb);
	ubf_assert_non_NULL (pl);
	ubf_assert_non_NULL (pi);
	
	unsigned int		nls;
	size_t				jmp;
	bool				bRet	= false;
	
	if (pb && *pb && pl && pi)
	{
		while (*pb && *pl)
		{
			char	c;
			
			c = **pb;
			if (isWhiteSpace (c))
			{
				*pb += 1;
				*pl -= 1;
				++ pi->absPosition;
				++ pi->charNumber;
				bRet = true;
			} else
				break;
		}
		nls = strIsNewLine (*pb, *pl, &jmp);
		if (nls)
		{
			pi->lineNumber	+= nls;
			pi->absPosition	+= jmp;
			*pb += jmp;
			*pl -= jmp;
			bRet = true;
		}
	}
	return bRet;
}

/*
	Returns the new length of pb when trailing white space is removed.
*/
size_t getLengthTrailingWhiteSpaceRemoved (char *pb, size_t l)
{
	ubf_assert_non_NULL (pb);
	ubf_assert (0 < l);

	while (l && isWhiteSpace (pb [l - 1]))
	{
		-- l;
	}
	return l;
}

/*
	Returns the length of a line.
*/
size_t getLineLength (char *pb, size_t lb) //, STRLINECONF *pc)
{
	ubf_assert_non_NULL (pb);

	unsigned int	nls		= 0;
	size_t			jmp;
	char			*p		= pb;
	size_t			r		= 0;

	while (*p && lb && 0 == (nls = strIsNewLine (p, lb, &jmp)))
	{
		++ p;
		-- lb;
	}
	if (nls || 0 == lb)
	{
		r = p - pb;
		// Comment out the following line if trailing white space should not be removed.
		r = getLengthTrailingWhiteSpaceRemoved (pb, r);
	}
	return r;
}

/*
	The UTF-8 payload line extractor.
*/
unsigned int StrLineExtractU8	(
				char					*pBuf,
				size_t					lenBuf,
				STRLINECONF				*pConf,
				StrLineExtractCallback	cb,
				void					*pCustom
								)
{
	unsigned int		uiRet	= 0;						// Return value; counts how often
															//	the callback function has been
															//	called.

	ubf_assert_non_NULL (pConf);
	ubf_assert (EN_STRLINEEXTRACT_UTF8 == pConf->CharacterSet);

	if (SanityCheckMultiComments (pConf))
	{
		STRLINEINF			sLineInfo;
		bool				cbRet;
		bool				b;

		if (pBuf)
		{
			InitSTRLINEINF (&sLineInfo, pCustom);
			sLineInfo.lineNumber = 1;						// Line 1 unless buffer empty.
			while (lenBuf)
			{
				do	{
						b = swallowLineComment (&pBuf, &lenBuf, pConf, &sLineInfo);
						b |= swallowMultiComment (&pBuf, &lenBuf, pConf, &sLineInfo);
						b |= swallowEmptyAndWhiteSpaceLines (&pBuf, &lenBuf, &sLineInfo);
					} while (b);
				if (lenBuf)
				{
					// We now got a single line.
					sLineInfo.lnLength		= getLineLength (pBuf, lenBuf); //, pConf);
					sLineInfo.pStart		= pBuf;
					sLineInfo.charNumber	= 0;			// Currently not supported.
					if (cb)
					{
						cbRet = (*cb) (&sLineInfo);
						++ uiRet;
						if (!cbRet)
							break;
					}
					pBuf += sLineInfo.lnLength;
					lenBuf -= sLineInfo.lnLength;
				}
			}
		}
	}
	return uiRet;
}

unsigned int StrLineExtract	(
				void					*pBuf,
				size_t					lenBuf,
				STRLINECONF				*pConf,
				StrLineExtractCallback	cb,
				void					*pCustom
							)
{
	ubf_assert_non_NULL (pBuf);

	// Default STRLINECONF.
	STRLINECONF			conf;
	InitSTRLINECONFforUBFL (&conf);

	STRLINECONF			*pC		= pConf ? pConf : &conf;
	switch (pC->CharacterSet)
	{
		case EN_STRLINEEXTRACT_UTF8:
			return StrLineExtractU8 (pBuf, lenBuf, pC, cb, pCustom);
		case EN_STRLINEEXTRACT_UTF16:
			ubf_assert_msg (false, "Not implemented.");
			return 0;
	}
	return 0;
}
/****************************************************************************************

	File:		strnewline.c
	Why:		New line functions.
	OS:			C99
	Author:		Thomas
	Created:	2022-04-11

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2022-04-11	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	For build options, see header file.
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strnewline.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ArrayMacros.h"
		#include "./platform.h"
		#include "./ubfdebug.h"
		#include "./memstrstr.h"
	#else
		#include "./../pre//ArrayMacros.h"
		#include "./../pre/platform.h"
		#include "./../dbg/ubfdebug.h"
		#include "./../mem/memstrstr.h"
	#endif

#endif

/*
	This should be:

	static const char *aszCunilogNewLine [unilogNewLineAmountEnumValues - 1] =

	But we want to check the amount of elements at runtime.
*/
#if defined (CUNILOG_NEWLINE_POSIX_ONLY)
	const char *aszLineEndings [] =
	{
			"\n"				// 0 = cunilogNewLineSystem
	};
	size_t lenLineEndings [] =
	{
			1					// 0 = cunilogNewLineSystem		"\n"
	};
#elif defined (CUNILOG_NEWLINE_WINDOWS_ONLY)
	const char *aszLineEndings [] =
	{
			"\r\n"				// 0 = cunilogNewLineSystem
	};
	size_t lenLineEndings [] =
	{
			2,					// 0 = cunilogNewLineSystem		"\r\n"
	};
#elif defined (CUNILOG_NEWLINE_POSIX_AND_WINDOWS_ONLY)
	const char *aszLineEndings [] =
	{
		#ifdef PLATFORM_IS_WINDOWS
			"\r\n",				// 0 = cunilogNewLineSystem
		#else
			"\n",				// 0 = cunilogNewLineSystem
		#endif
			"\n",				// 1 = cunilogNewLinePOSIX
			"\r\n",				// 2 = cunilogNewLineWindows
	};
	size_t lenLineEndings [] =
	{
		#ifdef PLATFORM_IS_WINDOWS
			2,					// 0 = cunilogNewLineSystem		"\r\n"
		#else
			1,					// 0 = cunilogNewLineSystem		"\n"
		#endif
			1,					// 1 = cunilogNewLinePOSIX		"\n"
			2,					// 2 = cunilogNewLineWindows	"\r\n"
	};
#else
	const char *aszLineEndings [] =
	{
		#ifdef PLATFORM_IS_WINDOWS
			"\r\n",				// 0 = cunilogNewLineSystem
		#else
			"\n",				// 0 = cunilogNewLineSystem
		#endif
			"\n",				// 1 = cunilogNewLinePOSIX
			"\r\n",				// 2 = cunilogNewLineWindows
			"\r"				// 3 = cunilogNewLineApple
		,	"\n\r"				// 4 = cunilogNewLineRISCOS
		,	"\0x1E"				// 5 = cunilogNewLineQNXbef4
		,	"\0x15"				// 6 = cunilogNewLineIBMzOS
		,	"\0xC2\0x85"		// 7 = cunilogNewLineNEL
		,	"\0xE2\0x80\0xA8"	// 8 = cunilogNewLineLS
		,	"\0xE2\0x80\0xA9"	// 9 = cunilogNewLinePS
	};
	size_t lenLineEndings [] =
	{
		#ifdef PLATFORM_IS_WINDOWS
			2,					// 0 = cunilogNewLineSystem		"\r\n"
		#else
			1,					// 0 = cunilogNewLineSystem		"\n"
		#endif
			1,					// 1 = cunilogNewLinePOSIX		"\n"
			2,					// 2 = cunilogNewLineWindows	"\r\n"
			1					// 3 = cunilogNewLineApple		"\r"
		,	2					// 4 = cunilogNewLineRISCOS		"\n\r"
		,	1					// 5 = cunilogNewLineQNXbef4	"\0x1E"
		,	1					// 6 = cunilogNewLineIBMzOS		"\0x15"
		,	2					// 7 = cunilogNewLineNEL		"\0xC2\0x85"
		,	3					// 8 = cunilogNewLineLS			"\0xE2\0x80\0xA8"
		,	3					// 9 = cunilogNewLinePS			"\0xE2\0x80\0xA9"
	};
#endif

#ifdef DEBUG
	const char *ccLineEnding (newline_t nl)
	{
		ubf_assert (nl >= 0);
		ubf_assert (nl < cunilogNewLineAmountEnumValues);

		return aszLineEndings [nl];
	}
#endif

#ifdef DEBUG
	size_t lnLineEnding (newline_t nl)
	{
		ubf_assert (nl >= 0);
		ubf_assert (nl < cunilogNewLineAmountEnumValues);

		return lenLineEndings [nl];
	}
#endif

const char *szLineEnding (newline_t nl, size_t *pln)
{
	ubf_assert (nl >= 0);
	ubf_assert (nl < cunilogNewLineAmountEnumValues);
	ubf_assert_non_NULL (pln);

	*pln = lenLineEndings [nl];
	return aszLineEndings [nl];
}

#ifdef STRNEWLINE_FORCE_ORG_STRISNEWLINE
	unsigned int strIsNewLine (char *ch, size_t stLen, size_t *stJump)
	{	/* New line found?
			A new line is found when:
			<CR>		(0x0D)
			<CR><LF>	(0x0D)(0x0A)
			<LF>		(0x0A)
			This counts as two new lines (a single LF and a single CR):
			<LF><CR>	(0x0A)(0x0D)
		*/
		size_t				st;
		unsigned int		uiRet	= 0;

		if (ch && *ch)
		{
	
			#ifdef DEBUG
				// This was used to find the bug with the wrong line numbers. Thomas, 2018-10-29.
				//#define UBF_LANG_TEXTS_TEST_LINE_NUMBERS
				#ifdef UBF_LANG_TEXTS_TEST_LINE_NUMBERS
					char		chDbg [32];
				#endif
			#endif
	
			st = (size_t) -1 == stLen ? strlen (ch) : stLen;
	
			#ifdef DEBUG
				#ifdef UBF_LANG_TEXTS_TEST_LINE_NUMBERS
				if (st > 20 && uiLineCount)
				{
					memcpy (chDbg, ch, 31);
					chDbg [31] = '\0';
					printf ("Line %u, \"%s\"\n", *uiLineCount, chDbg);
				}
				#endif
			#endif
	
			if	(
						st > 1
					&&	ASCII_CR_VAL	==	(unsigned char) ch [0]
					&&	ASCII_LF_VAL	==	(unsigned char) ch [1]
				)
			{
				++ uiRet;
				if (stJump)
					*stJump			= 2;
				return true;
			}
			if	((unsigned char) ch [0] == ASCII_LF_VAL || (unsigned char) ch [0] == ASCII_CR_VAL)
			{
				++ uiRet;
				if (stJump)
					*stJump			= 1;
				return true;
			}
		}
		if (stJump)
			*stJump					= 0;
		return uiRet;
	}
#endif

size_t strIsLineEndings (const char *ch, size_t stLen, size_t *stJump)
{
	ubf_assert_non_NULL (ch);

	size_t	j	= 0;
	size_t	r	= 0;
	int		e;
	size_t	l;

	stLen = USE_STRLEN == stLen ? strlen (ch) : stLen;

	while (stLen)
	{
		e = 0;
		while (cunilogNewLineAmountEnumValues > e)
		{
			l = lnLineEnding (e);
			if (stLen >= l && !memcmp (ch, ccLineEnding (e), l))
			{
				++ r;
				j			+= l;
				ch			+= l;
				stLen		-= l;
				break;
			}
			++ e;
		}
		if (cunilogNewLineAmountEnumValues == e)
		{	// Not found.
			break;
		}
	}
	if (stJump)
		*stJump = j;
	return r;
}

char *strFirstLineEnding (const char *ch, size_t len)
{
	int		e;
	size_t	l;

	len = USE_STRLEN == len ? strlen (ch) : len;

	while (len)
	{
		e = 0;
		while (cunilogNewLineAmountEnumValues > e)
		{
			l = lnLineEnding (e);
			if (len >= l && !memcmp (ch, ccLineEnding (e), l))
				return (char *) ch;
			++ e;
		}
		++ ch;
		-- len;
	}
	return NULL;
}

char *strFirstLineEnding_l (const char *ch, size_t len, size_t *plLE)
{
	int		e;
	size_t	l;

	len = USE_STRLEN == len ? strlen (ch) : len;

	while (len)
	{
		e = 0;
		while (cunilogNewLineAmountEnumValues > e)
		{
			l = lnLineEnding (e);
			if (len >= l && !memcmp (ch, ccLineEnding (e), l))
			{
				if (*plLE)
					*plLE = l;
				return (char *) ch;
			}
			++ e;
		}
		++ ch;
		-- len;
	}
	return NULL;
}

size_t strRemoveLineEndingsFromEnd (const char *sz, size_t len)
{
	ubf_assert_non_NULL (sz);

	int		i;
	size_t	ln	= USE_STRLEN == len ? strlen (sz) : len;

	do
	{
		len = ln;
		i	= 0;
		while (ln && i < cunilogNewLineAmountEnumValues)
		{
			if (ln >= lenLineEndings [i])
				if (!memcmp (sz + ln - lenLineEndings [i], aszLineEndings [i], lenLineEndings [i]))
				{
					ln -= lenLineEndings [i];
					break;
				}
			++ i;
		}
	} while (len != ln);
	return ln;
}

#ifdef STRNEWLINE_BUILD_TEST
	bool test_strnewline (void)
	{
		ubf_assert (GET_ARRAY_LEN (aszLineEndings)				==	cunilogNewLineAmountEnumValues);
		ubf_assert (GET_ARRAY_LEN (lenLineEndings)				==	cunilogNewLineAmountEnumValues);

		bool			b			= true;
		size_t			st;
		size_t			us;
		char			sz [1024];
		char			*sr;

		#ifdef CUNILOG_NEWLINE_POSIX_ONLY

			strcpy (sz, "\r\n\r");
			us = strIsLineEndings (sz, strlen (sz), &st);
			ubf_expect_bool_AND (b, 0 == us);
			ubf_expect_bool_AND (b, 0 == st);

			strcpy (sz, "\n\r");
			us = strIsLineEndings (sz, strlen (sz), &st);
			ubf_expect_bool_AND (b, 1 == us);
			ubf_expect_bool_AND (b, 1 == st);

			strcpy (sz, "\n\n\n");
			us = strIsLineEndings (sz, strlen (sz), &st);
			ubf_expect_bool_AND (b, 3 == us);
			ubf_expect_bool_AND (b, 3 == st);

		#endif

		#ifdef CUNILOG_NEWLINE_EXTENDED

			strcpy (sz, "\r\n\r");
			us = strIsLineEndings (sz, strlen (sz), &st);
			ubf_assert (2 == us);
			b &= 2 == us;

			strcpy (sz, "ABC\r\n");
			sr = strFirstLineEnding (sz, USE_STRLEN);
			ubf_expect_bool_AND (b, !memcmp (sr, "\r\n", 3));

			strcpy (sz, "\r\n\r");
			us = strRemoveLineEndingsFromEnd (sz, strlen (sz));
			ubf_assert (0 == us);
			b &= 0 == us;

			strcpy (sz, "ABC\r\n\r");
			us = strRemoveLineEndingsFromEnd (sz, strlen (sz));
			ubf_assert (3 == us);
			b &= 3 == us;

			strcpy (sz, "A\rBC\r\n\r");
			us = strRemoveLineEndingsFromEnd (sz, strlen (sz));
			ubf_assert (4 == us);
			b &= 4 == us;

		#endif

		#ifdef CUNILOG_NEWLINE_POSIX_AND_WINDOWS_ONLY

			strcpy (sz, "\n\n\nABC");
			us = strIsLineEndings (sz, strlen (sz), &st);
			ubf_expect_bool_AND (b, 3 == us);
			ubf_expect_bool_AND (b, 3 == st);

			strcpy (sz, "\r\n\r\n\r\nABC");
			us = strIsLineEndings (sz, strlen (sz), &st);
			ubf_expect_bool_AND (b, 3 == us);
			ubf_expect_bool_AND (b, 6 == st);

			strcpy (sz, "\r\n\r");
			us = strIsLineEndings (sz, strlen (sz), &st);
			ubf_expect_bool_AND (b, 1 == us);
			ubf_expect_bool_AND (b, 2 == st);

			strcpy (sz, "\na");
			us = strIsLineEndings (sz, strlen (sz), &st);
			ubf_expect_bool_AND (b, 1 == us);

			strcpy (sz, "ABC\r\n");
			sr = strFirstLineEnding (sz, USE_STRLEN);
			ubf_expect_bool_AND (b, !memcmp (sr, "\r\n", 3));

			strcpy (sz, "ABC\r\n");
			sr = strFirstLineEnding_l (sz, USE_STRLEN, &st);
			ubf_expect_bool_AND (b, !memcmp (sr, "\r\n", 3));
			ubf_expect_bool_AND (b, 2 == st);

			strcpy (sz, "ABC\r\r\n");
			sr = strFirstLineEnding (sz, USE_STRLEN);
			ubf_expect_bool_AND (b, !memcmp (sr, "\r\n", 3));

			strcpy (sz, "\r\n\r");
			us = strRemoveLineEndingsFromEnd (sz, strlen (sz));
			ubf_expect_bool_AND (b, 3 == us);
			ubf_expect_bool_AND (b, !memcmp (sz, "\r\n\r", 4));

			strcpy (sz, "ABC\r\n\r");
			us = strRemoveLineEndingsFromEnd (sz, strlen (sz));
			ubf_expect_bool_AND (b, 6 == us);
			ubf_expect_bool_AND (b, !memcmp (sz, "ABC\r\n\r", 7));

		#endif

		return b;
	}
#endif
/****************************************************************************************

	File:		strhexdumpstructs.c
	Why:		Structures for data dumps.
	OS:			C99
	Author:		Thomas
	Created:	2024-09-10
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-09-10	Thomas			Created. Based on ubfdumpstructs.h, which is now obsolete.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	// This seems useless but suppresses the compiler warning about an empty unitl.
	#include "./strhexdumpstructs.h"

#endif
/****************************************************************************************

	File:		strhexdump.c
	Why:		Functions for data dumps.
	OS:			C99
	Author:		Thomas
	Created:	2024-09-10
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-09-10	Thomas			Created. Based on ubfdump.h, which is now obsolete.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strhexdump.h"
	#include "./strnewline.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
		#include "./ubfcharscountsandchecks.h"
		#include "./strhex.h"
	#else
		#include "./../dbg/ubfdebug.h"
		#include "./../string/ubfcharscountsandchecks.h"
		#include "./../string/strhex.h"
	#endif

#endif

#ifdef BUILD_STRHEXDUMP_TEST_FNCT
	#include <stdio.h>
#endif

/*
	Advanced hex dump. As of Jan 2025 the advanced hex dump is considered incomplete/abandoned.
	Use the simple hex dump instead. See further down.
*/

// The standard SUBF_DUMP_PARS structure. It is used if the parameter sDumpPars is NULL.
#ifdef DEBUG
SHEX_DUMP_PARS		sdp_default	=	{
						cunilogNewLineDefault,
						NULL, 0,				// The header ccHeader.
						NULL,					// No line start.
						0,						// Length of the line start.
						(size_t) -1,			// Value added to the offset. In the debug version we're outputting
												//	the data's address.
						8,						// The width of the offset in digits.
						": ",					// The separator between offset and values.
						2,						// Its length.
						16,						// The default line width, i.e the amount of data bytes to display.
						1,						// The width of a value in octets (bytes).
						" ",					// The value separator.
						1,						// Its length.
						"-",					// The value separator between the first and the second half of the values.
						1,
						" ",					// ASCII display text separator.
						1,
						".",					// Unprintable character.
						1,
						"  ",					// Fill string if the data is shorter than a line boundary (value).
						2,
						" ",					// Fill character if the data doesn't fit a line boundary (ASCII display text).
						1,
						STR_DATA_DUMP_OPTS_NONE,
						0						// No error.
									};
#else
SHEX_DUMP_PARS		sdp_default	=	{
						cunilogNewLineDefault,
						NULL, 0,				// The header ccHeader.
						NULL,					// No line start.
						0,						// Length of the line start.
						0,						// Value added to the offset.
						4,						// The width of the offset in digits.
						": ",					// The separator between offset and values.
						2,						// Its length.
						16,						// The default line width, i.e the amount of data bytes to display.
						1,						// The width of a value in octets (bytes).
						" ",					// The value separator.
						1,						// Its length.
						"-",					// The value separator between the first and the second half of the values.
						1,
						" ",					// ASCII display text separator.
						1,
						".",					// Unprintable character.
						1,
						"  ",					// Fill string if the data is shorter than a line boundary (value).
						2,
						" ",					// Fill character if the data doesn't fit a line boundary (ASCII display text).
						1,
						STR_DATA_DUMP_OPTS_NONE,
						0						// No error.
									};
#endif

void ubf_data_dump_def_SUBF_DUMP_PARS (SHEX_DUMP_PARS *s)
{
	ubf_assert (NULL != s);

	memcpy (s, &sdp_default, sizeof (SHEX_DUMP_PARS));
}

static const char *szLineEndingFromSHEX_DUMP_PARS (SHEX_DUMP_PARS *pd)
{
	ubf_assert_non_NULL (pd);

	return ccLineEnding (pd->le);
}

static size_t lnLineEndingFromSHEX_DUMP_PARS (SHEX_DUMP_PARS *pd)
{
	ubf_assert_non_NULL (pd);

	return lnLineEnding (pd->le);
}

void writeOffsetQword (unsigned char *ucOut, SHEX_DUMP_PARS *sdp, uint64_t uiValue)
{
	if (sdp->uiOptions & STR_DATA_DUMP_OPTS_OFFS_USE_BLANKS)
	{
		ubf_hex_str_blank_from_qword	(		// Use leading blanks in the offset.
				(char *) ucOut,
				sdp->uiOffsetWidth,
				uiValue
										);
	} else
	{
		ubf_hex_str_zero_from_qword		(
				(char *) ucOut,				// Use leading zeros in the offset.
				sdp->uiOffsetWidth,
				uiValue
										);
	}
}

size_t widthOfValues (SHEX_DUMP_PARS *pdp, SHEX_DUMP_SAN *pob)
{
	ubf_assert_non_NULL (pdp);
	ubf_assert (0 < pdp->uinValues);

	size_t widthOfValues = pob->lenDispSeparator;

	/*
		xx|xx|xx|xx|xx|xx:	One separator less than values.
	*/
	size_t widthOfSepars = pdp->uinValues * pob->lenValSeparator - pob->lenValSeparator;
	if (pdp->uinValues > 1)
	{	// Remove one separator and add the separator that splits the values in half instead.
		widthOfSepars -= pob->lenValSeparator;
		widthOfSepars += pob->lenHalfLineSep;
	}
	if (pdp->uiOptions & STR_DATA_DUMP_OPTS_BINARY)
	{	// Binary format. Every octet requires 8 characters.
		widthOfValues = (size_t) pdp->uinValues * 8;
	} else
	if (pdp->uiOptions & STR_DATA_DUMP_OPTS_DECIMAL)
	{	// Decimal format. Every octet requires 3 characters (000...255).
		widthOfValues = (size_t) pdp->uinValues * 3;
	} else
	{	// Hexadecimal format. Every octet requires 2 characters.
		widthOfValues = (size_t) pdp->uinValues * 2;
	}
	return widthOfValues + widthOfSepars;
}

static size_t widthOfDisplay (SHEX_DUMP_PARS *pdp)
{
	ubf_assert_non_NULL (pdp);
	ubf_assert (0 < pdp->uinValues);

	// Each octet/byte is displayed as a single ASCII character.
	//	Note that this function only exists in case of future changes,
	//	for instance when several values might be combined.
	return pdp->uinValues;
}

static size_t obtainCorrectedStringLength (const char *ccStr, size_t stLen)
{
	size_t stRet = 0;

	if (ccStr)
	{
		stRet = USE_STRLEN == stLen ? strlen (ccStr) : stLen;
	}
	return stRet;
}

size_t required_data_dump_size	(
			size_t							stDumpDataLen,	// The length of the data to dump.
			SHEX_DUMP_PARS					*pdp,			// The SUBF_DUMP_PARS structure.
			SHEX_DUMP_SAN					*pob			// Already obtained.
									)
{
	ubf_assert (USE_STRLEN != stDumpDataLen);
	ubf_assert_non_NULL (pdp);
	ubf_assert_non_NULL (pob);

	if (!pob->bInitialised)
	{
		if (pdp->ccHeader)
		{
			/*
				The spec for the header says that an empty (not NULL!) header should produce a
				single line ending. If ccHeader is NULL, no header is used.
			*/
			pob->lenHeader = USE_STRLEN == pdp->lenHeader ? strlen (pdp->ccHeader) : pdp->lenHeader;
			pob->lenHeader += lnLineEndingFromSHEX_DUMP_PARS (pdp);
		} else
			pob->lenHeader = 0;
		/*
			If lenHeader is 0, we don't want a header.
			If lenHeader is equal to a line ending, the header consists of a single line ending.
			Any higher value means a header including line ending.
			Any lower value than a line ending is an error.
		*/
		ubf_assert (0 == pob->lenHeader || lnLineEndingFromSHEX_DUMP_PARS (pdp) <= pob->lenHeader);

		pob->lenOffset			= pdp->uiOffsetWidth;
		pob->lenLineStart		= obtainCorrectedStringLength (pdp->ccLineStart,	pdp->lenLineStart);
		pob->lenOffsetSep		= obtainCorrectedStringLength (pdp->ccOffsetSep,	pdp->lenOffsetSep);
		pob->lenValSeparator	= obtainCorrectedStringLength (pdp->ccValSeparator,	pdp->lenValSeparator);
		pob->lenHalfLineSep		= obtainCorrectedStringLength (pdp->ccHalfLineSep,	pdp->lenHalfLineSep);
		pob->lenNoMoreDataVal	= obtainCorrectedStringLength (pdp->ccHalfLineSep,	pdp->lenHalfLineSep);
		pob->lenUnprintable		= obtainCorrectedStringLength (pdp->ccHalfLineSep,	pdp->lenHalfLineSep);
		pob->lenNoMoreDataDisp	= obtainCorrectedStringLength (pdp->ccHalfLineSep,	pdp->lenHalfLineSep);
		pob->lenDispSeparator	= obtainCorrectedStringLength (pdp->ccDisplaySep,	pdp->lenDispSep);
		pob->lenValuesWidth		= widthOfValues (pdp, pob);
		pob->lenOneLine			=	pdp->lenLineStart			+
									pdp->uiOffsetWidth			+
									pob->lenOffsetSep			+
									pob->lenValuesWidth			+
									pob->lenDispSeparator		+
									widthOfDisplay (pdp)		+
									lnLineEndingFromSHEX_DUMP_PARS (pdp);
		pob->bInitialised		= true;
	}

	size_t stRetVal =	pob->lenHeader										+
						(stDumpDataLen / pdp->uinValues * pob->lenOneLine)	+
						(stDumpDataLen % pdp->uinValues * pob->lenOneLine);
	// NUL terminator.
	++ stRetVal;
	return stRetVal;
}

static size_t dump_header (char *ch, SHEX_DUMP_PARS *pd, SHEX_DUMP_SAN *ps)
{
	size_t r = ps->lenHeader;

	if (r)
	{
		size_t l = lnLineEndingFromSHEX_DUMP_PARS (pd);

		// Internal error. See explanation in required_data_dump_length ().
		ubf_assert (r >= l);

		if (l < r)
			memcpy (ch, pd->ccHeader, r - l);
		memcpy (ch + r - l, szLineEndingFromSHEX_DUMP_PARS (pd), l);
	}
	return r;
}

static size_t dump_linestart (char *ch, SHEX_DUMP_PARS *pd, SHEX_DUMP_SAN *ps)
{
	memcpy (ch, pd->ccLineStart, ps->lenLineStart);
	return ps->lenLineStart;
}

static size_t dump_offset (char *ch, SHEX_DUMP_PARS *pd, SHEX_DUMP_SAN *ps)
{
	asc_hex_zero_from_qword (ch, pd->uiOffsetWidth, ps->stNextOffset);
	ch += pd->uiOffsetWidth;
	memcpy (ch, pd->ccOffsetSep, ps->lenOffsetSep);
	ps->stNextOffset += pd->uinValues;
	return pd->uiOffsetWidth + ps->lenOffsetSep;
}

static size_t dumpTextIn (char *dest, const char *source, size_t len)
{
	memcpy (dest, source, len);
	return len;
}

static size_t dumpPrintableChar (char *dest, char c, SHEX_DUMP_PARS *pd, SHEX_DUMP_SAN *ps)
{
	if (ubf_is_printable_ASCII (c))
	{
		*dest = c;
		return 1;
	}
	memcpy (dest, pd->ccUnprintable, ps->lenUnprintable);
	return ps->lenUnprintable;
}

static size_t dump_values (char *ch, SHEX_DUMP_PARS *pd, SHEX_DUMP_SAN *ps)
{
	uint8_t		u2	= pd->uinValues / 2;
	char		*pVal = ch;
	char		*pDsp = ch + ps->lenValuesWidth + ps->lenDispSeparator;

	uint8_t		ui	= 0;
	while (ui ++ < pd->uinValues)
	{
		if (ps->lenValToDumpLeft)
		{	// Value/hex dump.
			asc_hex_from_octet (pVal, ps->ccNxtValToDump [0]);
			pVal += 2;
			ps->lenValToDumpLeft --;
		} else
			pVal += dumpTextIn (pVal, pd->ccNoMoreDataVal, ps->lenNoMoreDataVal);
		if (ui != u2)
		{
			if (ui < pd->uinValues)
				pVal += dumpTextIn (pVal, pd->ccValSeparator, ps->lenValSeparator);
			else
				pVal += dumpTextIn (pVal, pd->ccDisplaySep, ps->lenDispSeparator);
		} else
			pVal += dumpTextIn (pVal, pd->ccHalfLineSep, ps->lenHalfLineSep);
		pDsp += dumpPrintableChar (pDsp, ps->ccNxtValToDump [0], pd, ps);
		++ ps->ccNxtValToDump;
	}
	return pDsp - ch;
}

static size_t dumpLineEnding (char *ch, SHEX_DUMP_PARS *pd)
{
	size_t len = lnLineEndingFromSHEX_DUMP_PARS (pd);
	memcpy (ch, szLineEndingFromSHEX_DUMP_PARS (pd), len);
	return len;
}

size_t ubf_data_dump_SMEMBUF	(
		SMEMBUF				*mbOutput,						// The target SUBF_STRING.
		const char			*ccDumpData,					// The data to dump.
		size_t				lenDumpData,					// The length of the data to dump.
		SHEX_DUMP_PARS		*psDumpPars,					// The SUBF_DUMP_PARS structure.
		SHEX_DUMP_SAN		*pdSanDPars						// Can be carried to the next invokation.
								)
{
	ubf_assert_non_NULL (mbOutput);
	ubf_assert	(
						(NULL == ccDumpData && 0 == lenDumpData)
					||	(USE_STRLEN == lenDumpData && ccDumpData)
					||	(ccDumpData)
				);

	SHEX_DUMP_SAN		ourSan;

	if (NULL == pdSanDPars)
	{
		ourSan.bInitialised = false;
		pdSanDPars = &ourSan;
	}
	lenDumpData	= USE_STRLEN == lenDumpData ? strlen (ccDumpData) : lenDumpData;
	psDumpPars = psDumpPars ? psDumpPars : &sdp_default;
	pdSanDPars->stNextOffset = (size_t) -1 == psDumpPars->stFirstOffset
								? (size_t) ccDumpData : psDumpPars->stFirstOffset;
	pdSanDPars->ccNxtValToDump = ccDumpData;
	pdSanDPars->lenValToDumpLeft = lenDumpData;

	size_t stRet = 0;
	size_t stReq = required_data_dump_size (lenDumpData, psDumpPars, pdSanDPars);
	growToSizeSMEMBUF (mbOutput, stReq);
	if (isUsableSMEMBUF (mbOutput))
	{
		char *ch = mbOutput->buf.pch;

		ch += dump_header (ch, psDumpPars, pdSanDPars);

		while (pdSanDPars->lenValToDumpLeft)
		{
			ch += dump_linestart	(ch, psDumpPars, pdSanDPars);
			ch += dump_offset		(ch, psDumpPars, pdSanDPars);
			ch += dump_values		(ch, psDumpPars, pdSanDPars);
			ch += dumpLineEnding	(ch, psDumpPars);
		}
		*ch = '\0';
		stRet = ch - mbOutput->buf.pch;

		// Return value and prepared buffer should be identical, apart
		//	from the NUL that's missing in the return value.
		ubf_assert (stReq == stRet + 1);
	}
	return stRet;
}

#ifdef nix
/*
bool ubf_data_dump_puts	(
							const char			*chDumpData,	// The data to dump.
							size_t				stDumpDataLen,	// The length of the data to dump.
							SHEX_DUMP_PARS		*sDumpPars		// The SUBF_DUMP_PARS structure.
						)
{
	SUBF_STRING		s		= SUBF_STRING_INITIALISER;
	BOOL			b;

	ubf_assert (NULL != chDumpData);
	ubf_assert (0 != stDumpDataLen);
	if (chDumpData && stDumpDataLen)
	{
		b = ubf_data_dump_SUBF_STRING (&s, chDumpData, stDumpDataLen, sDumpPars, FALSE);
		if (b)
			ubf_puts (s.buf);
		UBF_FREE_SUBF_STRING (s);
	} else
		b = FALSE;
	fflush (stdout);
	return b;
}

#ifdef DEBUG
	bool ubf_debug_data_dump_puts	(
							const char			*chDumpData,	// The data to dump.
							size_t				stDumpDataLen,	// The length of the data to dump.
							SHEX_DUMP_PARS		*sDumpPars		// The SUBF_DUMP_PARS structure.
									)
	{
		return ubf_data_dump_puts (chDumpData, stDumpDataLen, sDumpPars);
	}
#endif
*/

#endif

/*
	Simple hex dump. This is what you need/want.
*/

typedef struct sEnDataDmpW
{
	ddumpWidth		en;
	size_t			width;
} SENDATADMPW;

static SENDATADMPW sEw [] =
{
		{enDataDumpWidth16,	16}
	,	{enDataDumpWidth32,	32}
};

static inline size_t nValuesFromEnum (ddumpWidth width)
{
	return sEw [width].width;
}

static inline size_t hexValuesWidth (size_t nValues)
{
	// "00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00 "
	return 3 * nValues + 2;									// "- " = + 2.
}

static size_t valuesWidthOneLine (ddumpWidth width, newline_t nl)
{
	// "\t00000000: "
	size_t lenTot = 1 + 8 + 1 + 1;							// TAB, 8 chars, colon, SPC.

	// "00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00 "
	size_t nVals	= nValuesFromEnum (width);
	size_t lenHex	= hexValuesWidth (nVals);

	// "........ ........" + <line ending>
	lenTot += lenHex + nVals + 1 + lnLineEnding (nl);
	return lenTot;
}

/*
	"\t00000000: 00 00 00 00 00 00 00 00 - 00 00 00 00 00 00 00 00 ........ - ........" + <line ending>

*/
size_t hxdmpRequiredSize		(
		size_t				lenDumpData,					// The length of the data to dump.
		ddumpWidth			width,							// Output width.
		newline_t			nl
								)
{
	size_t nOcts = nValuesFromEnum (width);
	size_t wLine = valuesWidthOneLine (width, nl);
	size_t lines = lenDumpData / nOcts;
	lines += lenDumpData % nOcts ? 1 : 0;
	size_t total = lines * wLine;
	++ total;												// NUL terminator.
	return total;
}

/*
	Writes one line to szOut and returns the amount of octets consumed from ccData.
*/
static size_t write_out_one_line	(
				char				**pszOut,				// Output buffer.
				size_t				offset,					// Offset start.
				const unsigned char	*ccData,				// Data to dump.
				size_t				lnData,					// Length of data to dump.
				ddumpWidth			width,					// n Values per line.
				newline_t			nl						// New line sequence to add.
									)
{
	ubf_assert_non_NULL (pszOut);
	ubf_assert_non_NULL (*pszOut);

	char	*szOut		= *pszOut;

	#ifdef DEBUG
	size_t	dbgWidth1	= valuesWidthOneLine (width, nl);
	char	*szOrg		= szOut;
	#endif

	// "\t"
	*szOut ++ = ASCII_TAB;

	// Offset "\t00000000: ".
	uint32_t uiOffs = offset & UINT32_MAX;
	asc_hex_from_dword (szOut, uiOffs);
	szOut += 8;
	memcpy (szOut, ": ", 2);
	szOut += 2;

	size_t	lenConsumed	= 0;
	size_t	nValues		= nValuesFromEnum (width);
	size_t	widthValues	= hexValuesWidth (nValues);
	char	*szAsc		= szOut + widthValues;				// ASCII output.
	size_t	pos			= 1;
	size_t	lidx;

	for (lidx = 0; lidx < nValues; ++ lidx)
	{
		if (lnData)
		{
			// Hex output.
			asc_hex_from_octet (szOut, ccData [lidx]);
			szOut += 2;
			szOut [0] = ' ';
			++ szOut;

			// ASCII output.
			if (ubf_is_printable_ASCII (ccData [lidx]))
				szAsc [0] = ccData [lidx];
			else
				szAsc [0] = '.';
			-- lnData;
			++ lenConsumed;
		} else
		{
			memcpy (szOut, "   ", 3);
			szOut += 3;
			szAsc [0] = ' ';
		}

		if (pos == nValues / 2)
		{
			memcpy (szOut, "- ", 2);
			szOut += 2;
			++ szAsc;
			szAsc [0] = ' ';
		}
		++ pos;
		++ szAsc;
	}
	memcpy (szAsc, ccLineEnding (nl), lnLineEnding (nl) + 1);
	szAsc += lnLineEnding (nl);
	szAsc [0] = ASCII_NUL;
	
	#ifdef DEBUG
	size_t dbgWidth2 = szAsc - szOrg;
	ubf_assert (dbgWidth1 == dbgWidth2);
	#endif

	*pszOut = szAsc;
	return lenConsumed;
}

size_t hxdmpWriteHexDump		(
		char				*szOutput,						// The output.
		const unsigned char	*ccDumpData,					// The data to dump.
		size_t				lenDumpData,					// The length of the data to dump.
		ddumpWidth			width,
		newline_t			nl
								)
{
	ubf_assert_non_NULL (ccDumpData);
	ubf_assert_non_0 (lenDumpData);

	char	*szDumpOut	= szOutput;
	size_t	lnDumpData	= lenDumpData;
	size_t	startOffst	= 0;
	size_t	lnConsumed;
	do
	{
		lnConsumed = write_out_one_line (&szDumpOut, startOffst, ccDumpData, lnDumpData, width, nl);
		startOffst += lnConsumed;
		ccDumpData += lnConsumed;
		lnDumpData -= lnConsumed;
	} while (lnDumpData);

	size_t lnRet = szDumpOut - szOutput;
	return lnRet;
}

/*
	Some tests.
*/
#ifdef BUILD_STRHEXDUMP_TEST_FNCT
	bool test_strhexdump (void)
	{
		bool	bRet = false;
		
		/*
		size_t	st1, st2;

		SMEMBUF	buf = SMEMBUF_INITIALISER;
		SHEX_DUMP_SAN	s;
		s.bInitialised = false;

		const char ccDump [] = "01ABCDEFGHIJKLMN";
		size_t lnDump = sizeof (ccDump) - 1;

		st1 = required_data_dump_size (lnDump, &sdp_default, &s);
		st2 = ubf_data_dump_SMEMBUF (&buf, ccDump, lnDump, NULL, &s);
		ubf_assert (st1 == st2 + 1);
		puts (buf.buf.pch);
		*/

		return bRet;
	}
#endif
/****************************************************************************************

	File:		strintuint.c
	Why:		Conversion functions between character strings and integers.
	OS:			C99.
	Author:		Thomas
	Created:	2019-08-28
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2019-08-28	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


	The functions ubf_itoa (), ubf_n_itoa10 (), ubf_str_from_uint16 (), ubf_str_from_int64 (),
	and ubf_str_from_uint64 () are (c) Lukas Chmela and derived from the versions at
	http://www.strudel.org.uk/itoa/ :

	* C++ version 0.4 char* style "itoa":
	* Written by Lukas Chmela
	* Released under GPLv3.

*/

#ifdef _WIN32
	#ifdef USE_WINDOWS_NATIVE_HEAP_FUNCTIONS
		#ifndef DONT_USE_UBF_TYPE_DEFINITIONS
		#define DONT_USE_UBF_TYPE_DEFINITIONS

			int our_own_inline_isdigit (int c)
			{
				return c >= '0' && c <= '9';
			}

			#ifndef isdigit
			#define isdigit(c)		our_own_inline_isdigit (c)
			#endif

		#endif
	#endif
#endif

#include <ctype.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strintuint.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfmem.h"
		#include "./Warnings.h"
		#include "./ubfdebug.h"
	#else
		#include "./../mem/ubfmem.h"
		#include "./../pre/Warnings.h"

			//#ifndef UBF_UBFDEBUG_USE_CURR_DIR - Obsolete/not used anymore.
		#include "./../dbg/ubfdebug.h"
	#endif
#endif

#ifdef UBF_BUILD_STRINTUINT_TEST
	#include <stdio.h>
#endif

#ifdef UBUILD_C_ITO_ALPHABET_IN_HERE
	char	c_ito_alphabet []	=
		"zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz";
#endif

/*
	See http://www.strudel.org.uk/itoa/ .
	2018-04-09, Thomas.
*/
/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukas Chmela
 * Released under GPLv3.

 */
char* ubf_itoa (int value, char* result, int base)
{
	// check that the base is valid
	if (base < 2 || base > 36) { *result = ASCII_NUL; return result; }

	char* ptr = result, *ptr1 = result, tmp_char;
	int tmp_value;

	do {
		tmp_value = value;
		value /= base;
		*ptr++ = c_ito_alphabet [35 + (tmp_value - value * base)];
	} while ( value );

	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	*ptr-- = ASCII_NUL;
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return result;
}

size_t ubf_n_itoa10 (int value, char* result)
{
	char* ptr	= result, *ptr1 = result, tmp_char;
	int			tmp_value;
	char		*r;

	do {
		tmp_value = value;
		value /= 10;
		*ptr++ = c_ito_alphabet [35 + (tmp_value - value * 10)];
	} while ( value );
	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	r = ptr;
	*ptr-- = ASCII_NUL;
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return ((size_t) (r - result));
}

DISABLE_WARNING_ARRAY_TOO_SMALL_FOR_NUL_TERMINATOR ()

char c_0_to_59_str0 [62][2] =
{
		"00", "01", "02", "03", "04", "05", "06", "07", "08", "09"
	,	"10", "11", "12", "13", "14", "15", "16", "17", "18", "19"
	,	"20", "21", "22", "23", "24", "25", "26", "27", "28", "29"
	,	"30", "31", "32", "33", "34", "35", "36", "37", "38", "39"
	,	"40", "41", "42", "43", "44", "45", "46", "47", "48", "49"
	,	"50", "51", "52", "53", "54", "55", "56", "57", "58", "59"

	/*
		See https://en.cppreference.com/w/c/chrono/tm .
		"Range allows for a positive leap second. Two leap seconds in the same minute are
		not allowed (the C89 range 0..61 was a defect)."
	*/
	,	"60", "61"
};

DEFAULT_WARNING_ARRAY_TOO_SMALL_FOR_NUL_TERMINATOR ()

#ifdef DEBUG
	void ubf_str0_from_59max (char *result, uint8_t ui59s)
	{
		ubf_assert (ui59s < 62);

		memcpy (result, c_0_to_59_str0 [ui59s], 2);
	}
#endif

#ifdef DEBUG
	void ubf_str0_from_59max_n (char *result, uint8_t ui59s)
	{
		ubf_assert (ui59s < 62);

		memcpy (result, c_0_to_59_str0 [ui59s], 2);
		result [2] = '\0';
	}
#endif

size_t ubf_str_from_uint8 (char *result, uint8_t ui8)
{
	uint16_t	t;
	size_t		s			= 1;
	char		*res		= result;
	
	memset (res, '0', 3);
	t = ui8;
	if (ui8 >= 100)
	{
		ui8 /= 100;
		*res++ = c_ito_alphabet [35 + ui8];
		t -= ui8 * 100;
		ui8 = t % 100;
		s = 3;
		if (ui8 >= 10)
		{
			ui8 /= 10;
			*res++ = c_ito_alphabet [35 + ui8];
			t -= ui8 * 10;
			ui8 = t % 10;
			if (ui8)
				*res++ = c_ito_alphabet [35 + ui8];
		} else
		if (ui8)
		{
			++ res;
			*res++ = c_ito_alphabet [35 + ui8];
		}
	} else
	if (ui8 >= 10)
	{
		ui8 /= 10;
		*res++ = c_ito_alphabet [35 + ui8];
		t -= ui8 * 10;
		ui8 = t % 10;
		s = 2;
		if (ui8)
			*res++ = c_ito_alphabet [35 + ui8];
	} else
	if (ui8)
		*res++ = c_ito_alphabet [35 + ui8];
	result [s] = ASCII_NUL;
	return s;
}

size_t ubf_str_from_uint16 (char *result, uint16_t ui16)
{
	char*		ptr			= result, *ptr1 = result, tmp_char;
	uint16_t	tmp_value;
	char		*r;

	do {
		tmp_value = ui16;
		ui16 /= 10;
		*ptr++ = c_ito_alphabet [35 + (tmp_value - ui16 * 10)];
	} while (ui16);
	r = ptr;
	*ptr-- = ASCII_NUL;
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return ((size_t) (r - result));
}

/*
	Based on
	https://github.com/bminor/newlib/blob/master/newlib/libc/stdlib/utoa.c .

	This version is ~ 10 % slower.
*/

static const char szdigits[] = "0123456789abcdefghijklmnopqrstuvwxyz";

size_t ubf_str_from_uint16_2 (char *result, uint16_t ui16)
{

	int i, j;
	unsigned remainder;
	char c;
  
	/* Check base is supported. */
	/*
		Our base is always 10.
	if ((base < 2) || (base > 36))
	{ 
		str[0] = '\0';
		return NULL;
	}  
	*/

	/* Convert to string. Digits are in reverse order.  */
	i = 0;
	do 
	{
		remainder = ui16 % 10;
		result[i++] = szdigits[remainder];
		ui16 = ui16 /10;
	} while (ui16 != 0);  
	result[i] = '\0'; 
  
	size_t r = i;

	/* Reverse string.  */
	for (j = 0, i--; j < i; j++, i--)
	{
		c = result[j];
		result[j] = result[i];
		result[i] = c; 
	}       
  
  return r;
}

size_t ubf_str0_from_uint16 (char *result, size_t digits, uint16_t ui16)
{
	ubf_assert_non_0 (digits);

	char	cResult [UBF_UINT16_SIZE];
	size_t	st;

	st = ubf_str_from_uint16 (cResult, ui16);
	if (st < digits)
	{
		memset (result, '0', digits - st);
		memcpy (result + digits - st, cResult, st);
	} else
	if (st == digits)
	{	// No leading zeroes due to st == digits.
		memcpy (result, cResult, st);
	} else
	//if (st > digits)										// Only this option left.
	{
		memcpy (result, cResult + st - digits, digits);
		st = STR0_NOT_ENOUGH_DIGITS;
	}
	result [digits] = ASCII_NUL;
	return st;
}

size_t ubf_str_from_int64 (char *result, int64_t i64)
{
	char* ptr	= result, *ptr1 = result, tmp_char;
	int64_t		tmp_value;
	char		*r;

	do {
		tmp_value = i64;
		i64 /= 10;
		*ptr++ = c_ito_alphabet [35 + (tmp_value - i64 * 10)];
	} while ( i64 );
	// Apply negative sign
	if (tmp_value < 0) *ptr++ = '-';
	r = ptr;
	*ptr-- = ASCII_NUL;
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return ((size_t) (r - result));
}

size_t ubf_str_from_uint64 (char *result, uint64_t ui64)
{
	char*		ptr			= result, *ptr1 = result, tmp_char;
	uint64_t	tmp_value;
	char		*r;

	do {
		tmp_value = ui64;
		ui64 /= 10;
		*ptr++ = c_ito_alphabet [35 + (tmp_value - ui64 * 10)];
	} while (ui64);
	r = ptr;
	*ptr-- = ASCII_NUL;
	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}
	return ((size_t) (r - result));
}

size_t ubf_str0_from_uint64 (char *result, size_t digits, uint64_t ui64)
{
	ubf_assert_non_0 (digits);

	char	cResult [UBF_UINT64_SIZE];
	size_t	st;
	
	st = ubf_str_from_uint64 (cResult, ui64);
	if (st < digits)
	{
		memset (result, '0', digits - st);
		memcpy (result + digits - st, cResult, st);
	} else
	if (st == digits)
	{	// No leading zeroes due to st == digits.
		memcpy (result, cResult, st);
	} else
	//if (st > digits)										// Only this option left.
	{
		memcpy (result, cResult + st - digits, digits);
		st = STR0_NOT_ENOUGH_DIGITS;
	}
	result [digits] = ASCII_NUL;
	return st;
}

size_t ubf_str__from_uint64 (char *result, size_t digits, uint64_t ui64)
{
	ubf_assert_non_0 (digits);

	char	cResult [UBF_UINT64_SIZE];
	size_t	st;
	
	st = ubf_str_from_uint64 (cResult, ui64);
	if (st < digits)
	{
		memset (result, ' ', digits - st);
		memcpy (result + digits - st, cResult, st);
	} else
	if (st == digits)
	{	// No leading zeroes due to st == digits.
		memcpy (result, cResult, st);
	} else
	//if (st > digits)										// Only this option left.
	{
		memcpy (result, cResult + st - digits, digits);
		st = STR0_NOT_ENOUGH_DIGITS;
	}
	result [digits] = ASCII_NUL;
	return st;
}

#ifdef UBF_BUILD_STR0_FROM_UINT64_TEST
	void ubf_test_str0_from_uint64 (void)
	{
		char		c [UBF_UINT64_LEN + 1];						// Needs room for a NUL.
		UBF_DEF_GUARD_VAR (v, "012345");
		uint64_t	u;
		size_t		s;
		
		// Check our definitions first.
		ubf_assert (UBF_UINT8_SIZE == UBF_UINT8_LEN + 1);
		ubf_assert (UBF_UINT16_SIZE == UBF_UINT16_LEN + 1);
		ubf_assert (UBF_UINT32_SIZE == UBF_UINT32_LEN + 1);
		ubf_assert (UBF_UINT64_SIZE == UBF_UINT64_LEN + 1);
		u = 0;
		s = ubf_str_from_int64 (c, u);
		ubf_assert (1 == s);
		u = 9;
		s = ubf_str_from_int64 (c, u);
		ubf_assert (1 == s);
		u = 10;
		s = ubf_str_from_int64 (c, u);
		ubf_assert (2 == s);
		u = 100;
		s = ubf_str_from_int64 (c, u);
		ubf_assert (3 == s);
		u = 999;
		s = ubf_str_from_int64 (c, u);
		ubf_assert (3 == s);
		u = 9999;
		s = ubf_str_from_int64 (c, u);
		ubf_assert (4 == s);
		u = 1000000;
		s = ubf_str_from_int64 (c, u);
		ubf_assert (7 == s);
		u = 0;
		s = ubf_str0_from_uint64 (c, 0, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (0 == s);
		//assert (false);
		u = 10;
		s = ubf_str0_from_uint64 (c, 2, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (2 == s);
		ubf_assert (!memcmp (c, "10", 3));
		u = 100;
		s = ubf_str0_from_uint64 (c, 3, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (3 == s);
		ubf_assert (!memcmp (c, "100", 4));
		u = 100;
		s = ubf_str0_from_uint64 (c, 5, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (3 == s);
		ubf_assert (!memcmp (c, "00100", 6));
		u = 99;
		s = ubf_str0_from_uint64 (c, 10, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (2 == s);
		ubf_assert (!memcmp (c, "0000000099", 11));
		u = 9999;
		s = ubf_str0_from_uint64 (c, UBF_UINT64_LEN, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (4 == s);
		ubf_assert (!memcmp (c, "00000000000000009999", 11));
		u = 9999;
		// Too short.
		s = ubf_str0_from_uint64 (c, 2, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (STR0_NOT_ENOUGH_DIGITS == s);
		//ubf_assert (!memcmp (c, "0000000099", 11));

		// The 16 bit version.
		s = ubf_str0_from_uint16 (c, 10, 3000);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (4 == s);
		ubf_assert (!memcmp (c, "0000003000", 10));
	}
#endif

bool ubf_uint64_from_str (uint64_t *ui, const char *chStr)
{
	uint64_t			u				= 0;
	const unsigned char	*ch;
	unsigned char		c;

	ch = (const unsigned char *) chStr;
	if (ch)
	{
		c = *ch;
		if ('+' == c)
		{
			ch ++;
			c = *ch;
		}
		while (c)
		{
			if (!isdigit (c))
				break;
			c -= '0';
			if (u > UINT64_MAX / 10)
				return false;
			u *= 10;
			if (u > UINT64_MAX - c)
				return false;
			u += c;
			ch ++;
			c = *ch;
		}
	}
	if (ui)
		*ui = u;
	return true;
}

size_t ubf_uint64_from_str_n (uint64_t *ui, const char *chStr, size_t nLen, enum enUintFromStrN s)
{
	uint64_t			u				= 0;
	const unsigned char	*ch;
	unsigned char		c;
	size_t				r				= 0;

	ch = (const unsigned char *) chStr;
	if (ch)
	{
		c = *ch;
		if ('+' == c && enUintFromStrAllowPlus == s)
		{
			ch ++;
			c = *ch;
			++ r;
		}
		while (c && nLen)
		{
			if (!isdigit (c))
				break;
			c -= '0';
			if (u > UINT64_MAX / 10)
				return r;
			u *= 10;
			if (u > UINT64_MAX - c)
				return r;
			u += c;
			ch ++;
			c = *ch;
			++ r;
			-- nLen;
		}
	}
	if (ui)
		*ui = u;
	return r;
}

bool is_number_str_l (const char *str, size_t len)
{
	len = USE_STRLEN == len ? strlen (str) : len;

	for (size_t st = 0; st < len; ++ st)
	{
		if (!ubf_isdigit (str [st]))
			return false;
	}
	return true;
}

bool ubf_int64_from_str (int64_t *pi, const char *chStr)
{
	int64_t				i				= 0;
	const unsigned char	*ch;
	unsigned char		c;
	bool				bneg			= false;

	ch = (const unsigned char *) chStr;
	if (ch)
	{
		c = *ch;
		switch (c)
		{
		case '-':
			bneg = true;
			// Falls through. See
			//	https://developers.redhat.com/blog/2017/03/10/wimplicit-fallthrough-in-gcc-7/ .
			UBF_FALLTHROUGH_HINT
		case '+':
			++ ch;
			c = *ch;
			break;
		default:
			break;
		}
		while (c)
		{
			if (!isdigit (c))
				return false;
			c -= '0';
			if (bneg)
			{
				if (i < INT64_MIN / 10)
					return false;
				i *= 10;
				if (i < INT64_MIN + c)
					return false;
				i -= c;
			} else
			{
				if (i > INT64_MAX / 10)
					return false;
				i *= 10;
				if (i > INT64_MAX - c)
					return false;
				i += c;
			}
			ch ++;
			c = *ch;
		}
	}
	if (pi)
		*pi = i;
	return true;
}

size_t ubf_str_to_int64 (const char *chStr, int64_t *pi)
{
	int64_t				i				= 0;
	const unsigned char	*ch;
	unsigned char		c;
	bool				bneg			= false;
	//size_t				r				= 0;

	ch = (const unsigned char *) chStr;
	if (ch)
	{
		c = *ch;
		switch (c)
		{
		case '-':
			bneg = true;
			// Falls through. See
			//	https://developers.redhat.com/blog/2017/03/10/wimplicit-fallthrough-in-gcc-7/ .
			UBF_FALLTHROUGH_HINT
		case '+':
			ch ++;
			c = *ch;
			break;
		default:
			break;
		}
		while (c)
		{
			if (!isdigit (c))
				break;
			c -= '0';
			if (bneg)
			{
				if (i < INT64_MIN / 10)
					break;
				i *= 10;
				if (i < INT64_MIN + c)
					break;
				i -= c;
			} else
			{
				if (i > INT64_MAX / 10)
					break;
				i *= 10;
				if (i > INT64_MAX - c)
					break;
				i += c;
			}
			ch ++;
			//r ++;
			c = *ch;
		}
	}
	if (pi)
		*pi = i;
	return (size_t) (ch - (const unsigned char *) chStr);
}

bool ubf_uint16_from_str (uint16_t *pus, const char *chStr)
{
	uint64_t		ui;
	uint16_t		us;

	if (ubf_uint64_from_str (&ui, chStr))
	{
		if (ui > USHRT_MAX)
			return false;
		us = (uint16_t) (ui & USHRT_MAX);
		if (pus)
			*pus = us;
		return true;
	}
	return false;
}

/*
	This function takes 4 (!) times longer than ubf_str_from_uint64 (). Test code can be
	found below the function.
	
void ubf_strd_from_uint64 (char *chStr, uint64_t u64)
{
	// C99.
	ubf_assert (UBF_UINT64_STRLEN > 20);							// Just in case.
	snprintf (chStr, UBF_UINT64_STRLEN, "%"PRIu64, u64);
	ubf_assert ('\0' != chStr [0]);								// Just in case.
}

	// Speed test for the functions ubf_str_from_uint64 () and ubf_strd_from_uint64 ().
	ULONGLONG	ullStart;
	ULONGLONG	ullEnd;
	char szTimeSpan [128];
	uint64_t uTest = 2000000;
	char szUINT64 [1024];
	ubf_get_system_time_ULONGLONG (&ullStart);
	for (uint64_t u = uTest; u; u --)
	{
		ubf_strd_from_uint64 (szUINT64, u);
	}
	ubf_get_system_time_ULONGLONG (&ullEnd);
	GetReadableTimeSpan (szTimeSpan, 128, ullEnd - ullStart);
	ubf_debug_printf ("Function test ubf_strd_from_uint64 (): %s.", szTimeSpan);
	ubf_get_system_time_ULONGLONG (&ullStart);
	for (uint64_t u = uTest; u; u --)
	{
		ubf_str_from_uint64 (szUINT64, u);
	}
	ubf_get_system_time_ULONGLONG (&ullEnd);
	GetReadableTimeSpan (szTimeSpan, 128, ullEnd - ullStart);
	ubf_debug_printf ("Function test ubf_str_from_uint64 () : %s.", szTimeSpan);
	ubf_get_system_time_ULONGLONG (&ullStart);
	for (uint64_t u = uTest; u; u --)
	{
		ubf_strd_from_uint64 (szUINT64, u);
	}
	ubf_get_system_time_ULONGLONG (&ullEnd);
	GetReadableTimeSpan (szTimeSpan, 128, ullEnd - ullStart);
	ubf_debug_printf ("Function test ubf_strd_from_uint64 (): %s.", szTimeSpan);
	ubf_get_system_time_ULONGLONG (&ullStart);
	for (uint64_t u = uTest; u; u --)
	{
		ubf_str_from_uint64 (szUINT64, u);
	}
	ubf_get_system_time_ULONGLONG (&ullEnd);
	GetReadableTimeSpan (szTimeSpan, 128, ullEnd - ullStart);
	ubf_debug_printf ("Function test ubf_str_from_uint64 () : %s.", szTimeSpan);
*/

#ifdef UBF_BUILD_STRINTUINT_TEST
	bool ubf_test_str_from_uint8 (void)
	{
		char			res [UBF_UINT8_SIZE];
		char			ref [32];
		size_t			s;
		bool			b = true;
		unsigned int	u;
		
		s = ubf_str_from_uint8 (res, 100);
		s = ubf_str_from_uint8 (res, 200);

		for (u = 0; u < 256; ++u)
		{
			/* Used for debugging.
			if (10 == u)
				ASSERT (false);
			if (100 == u)
				ASSERT (false);
			if (110 == u)
				ASSERT (false);
			if (200 == u)
				ASSERT (false);
			*/
			snprintf (ref, 32, "%u", u);
			s = ubf_str_from_uint8 (res, (uint8_t) u);
			if (10 > u)
			{
				b &= 1 == s;
				ubf_assert (true == b);
			} else
			if (100 > u)
			{
				b &= 2 == s;
				ubf_assert (true == b);
			} else
			{
				b &= 3 == s;
				ubf_assert (true == b);
			}
			b &= !memcmp (res, ref, s + 1);
			ubf_assert (true == b);
		}
		return b;
	}

	bool ubf_test_str0 (void)
	{
		bool		b = true;
		char		c [UBF_UINT64_LEN + 1];						// Needs room for a NUL.
		UBF_DEF_GUARD_VAR (v, "012345");
		uint64_t	u;
		size_t		s;

		// Check our definitions first.
		ubf_assert (UBF_UINT8_SIZE == UBF_UINT8_LEN + 1);
		ubf_assert (UBF_UINT16_SIZE == UBF_UINT16_LEN + 1);
		ubf_assert (UBF_UINT32_SIZE == UBF_UINT32_LEN + 1);
		ubf_assert (UBF_UINT64_SIZE == UBF_UINT64_LEN + 1);
		u = 0;
		s = ubf_str_from_int64 (c, u);
		ubf_expect_bool_AND (b, 1 == s);
		b = 1 == s;
		u = 9;
		s = ubf_str_from_int64 (c, u);
		ubf_expect_bool_AND (b, 1 == s);
		b &= 1 == s;
		u = 10;
		s = ubf_str_from_int64 (c, u);
		ubf_expect_bool_AND (b, 2 == s);
		u = 100;
		s = ubf_str_from_int64 (c, u);
		ubf_expect_bool_AND (b, 3 == s);
		u = 999;
		s = ubf_str_from_int64 (c, u);
		ubf_expect_bool_AND (b, 3 == s);
		u = 9999;
		s = ubf_str_from_int64 (c, u);
		ubf_expect_bool_AND (b, 4 == s);
		u = 1000000;
		s = ubf_str_from_int64 (c, u);
		ubf_expect_bool_AND (b, 7 == s);
		u = 0;
		s = ubf_str0_from_uint64 (c, 1, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 1 == s);
		//assert (false);
		u = 10;
		s = ubf_str0_from_uint64 (c, 2, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 2 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "10", 3));
		u = 100;
		s = ubf_str0_from_uint64 (c, 3, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 3 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "100", 4));
		u = 100;
		s = ubf_str0_from_uint64 (c, 5, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 3 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "00100", 6));
		u = 99;
		s = ubf_str0_from_uint64 (c, 10, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 2 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "0000000099", 11));
		u = 9999;
		s = ubf_str0_from_uint64 (c, UBF_UINT64_LEN, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 4 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "00000000000000009999", 11));
		u = 9999;
		// Too short.
		s = ubf_str0_from_uint64 (c, 2, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, STR0_NOT_ENOUGH_DIGITS == s);
		//ubf_assert (!memcmp (c, "0000000099", 11));

		s = ubf_str0_from_uint64 (c, 10, 3000);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 4 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "0000003000", 11));

		s = ubf_str0_from_uint64 (c, 10, 200);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 3 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "0000000200", 11));

		s = ubf_str0_from_uint64 (c, 3, 555);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 3 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "555", 4));

		s = ubf_str0_from_uint64 (c, 2, 200);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, STR0_NOT_ENOUGH_DIGITS == s);
		ubf_expect_bool_AND (b, !memcmp (c, "00", 3));

		s = ubf_str0_from_uint64 (c, 3, 1999);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, STR0_NOT_ENOUGH_DIGITS == s);
		ubf_expect_bool_AND (b, !memcmp (c, "999", 3));

		// The 16 bit version.
		s = ubf_str0_from_uint16 (c, 10, 3000);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 4 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "0000003000", 11));

		s = ubf_str0_from_uint16 (c, 10, 200);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 3 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "0000000200", 11));

		s = ubf_str0_from_uint16 (c, 3, 555);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, 3 == s);
		ubf_expect_bool_AND (b, !memcmp (c, "555", 4));

		s = ubf_str0_from_uint16 (c, 2, 200);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, STR0_NOT_ENOUGH_DIGITS == s);
		ubf_expect_bool_AND (b, !memcmp (c, "00", 3));

		s = ubf_str0_from_uint16 (c, 3, 1999);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_expect_bool_AND (b, STR0_NOT_ENOUGH_DIGITS == s);
		ubf_expect_bool_AND (b, !memcmp (c, "999", 3));

		// Min.
		ubf_str0_from_59max (c, 0);
		ubf_expect_bool_AND (b, !memcmp (c, "00", 2));
		ubf_str0_from_59max (c, 61);
		ubf_expect_bool_AND (b, !memcmp (c, "61", 2));
		// NUL-terminated.
		ubf_str0_from_59max_n (c, 0);
		ubf_expect_bool_AND (b, !memcmp (c, "00", 3));
		// Max.
		ubf_str0_from_59max_n (c, 61);
		ubf_expect_bool_AND (b, !memcmp (c, "61", 3));

		ubf_expect_bool_AND (b, is_number_str_l ("0", 1));
		ubf_expect_bool_AND (b, is_number_str_l ("1", 1));
		ubf_expect_bool_AND (b, is_number_str_l ("2", 1));
		ubf_expect_bool_AND (b, is_number_str_l ("9", 1));
		ubf_expect_bool_AND (b, is_number_str_l ("10", 2));
		ubf_expect_bool_AND (b, !is_number_str_l ("abcd", 4));
		ubf_expect_bool_AND (b, is_number_str_l ("000000000000000000000000000000000000000000000000000000000000", USE_STRLEN));
		ubf_expect_bool_AND (b, is_number_str_l ("111111111111111111111111111111111111111111111111111111111111", USE_STRLEN));
		ubf_expect_bool_AND (b, is_number_str_l ("999999999999999999999999999999999999999999999999999999999999", USE_STRLEN));
		ubf_expect_bool_AND (b, is_number_str_l ("1234abc", 4));
		ubf_expect_bool_AND (b, !is_number_str_l ("1234abc", 5));


		return b;
	}

	bool Test_strintuint (void)
	{
		bool b = ubf_test_str_from_uint8 ();
		ubf_assert_true (b);
		b &= ubf_test_str0 ();
		return b;
	}
#endif

/****************************************************************************************

	File:		strabsolutepath.c
	Why:		String functions for absolute/relative paths.
	OS:			C99
	Author:		Thomas
	Created:	2021-07-10
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2024-05-21	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	The functions in this module do not use any file system functions.
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdbool.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strisabsolutepath.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
	#else
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

#ifdef DEBUG
	bool is_absolute_path_unix (const char *chPath)
	{
		ubf_assert_non_NULL (chPath);

		return '/' == chPath [0];
	}
#endif

bool is_absolute_path_win (const char *chPath)
{
	ubf_assert_non_NULL (chPath);

	return is_unc_path (chPath) || is_absolute_drive_path (chPath);
}

#ifdef DEBUG
	bool is_absolute_path (const char *chPath)
	{
		ubf_assert_non_NULL (chPath);

		return (is_absolute_path_unix (chPath) || is_absolute_path_win (chPath));
	}
#endif

#ifdef DEBUG
	bool is_absolute_path_unix_l (const char *chPath, size_t len)
	{
		ubf_assert_non_NULL (chPath);
		ubf_assert ((size_t) -1 != len);

		return len && '/' == chPath [0];
	}
#endif

#ifdef DEBUG
	bool is_absolute_path_win_l (const char *chPath, size_t len)
	{
		ubf_assert_non_NULL (chPath);
		ubf_assert ((size_t) -1 != len);

		return (len > 1 && is_unc_path (chPath)) || ((len > 2) && is_absolute_drive_path (chPath));
	}
#endif

#ifdef DEBUG
	bool is_absolute_path_l (const char *chPath, size_t len)
	{
		ubf_assert_non_NULL (chPath);
		ubf_assert ((size_t) -1 != len);

		return (is_absolute_path_unix_l (chPath, len) || is_absolute_path_win_l (chPath, len));
	}
#endif
/****************************************************************************************

	File:		strisdotordotdot.c
	Why:		Checks for "." and ".." file/folder names.
	OS:			C99.
	Author:		Thomas
	Created:	2022-05-18

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2022-05-18	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strisdotordotdot.h"

#endif

#ifdef OS_IS_WINDOWS
	bool isDotOrDotDotW (wchar_t *wcPath)
	{	// Identify "." and ".." entries.
		return
				L'.' == wcPath [0]
			&&	(
						L'\0' == wcPath [1]
					||	(L'.' == wcPath [1] && L'\0' == wcPath [2])
				)
			? true : false;
	}
#endif

bool isDotOrDotDotU8 (char *cPath)
{	// Identify "." and ".." entries.
	return
			'.' == cPath [0]
		&&	(
					'\0' == cPath [1]
				||	('.' == cPath [1] && '\0' == cPath [2])
			)
		? true : false;
}
/****************************************************************************************

	File		struri.c
	Why:		Helpers for URIs.
	OS:			C99
	Created:	2025-01-06

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-01-06	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./struri.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./externC.h"
		#include "./strhex.h"
		#include "./ubfdebug.h"
	#else
		#include "./../pre/externC.h"
		#include "./../string/strhex.h"
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

static inline bool isValidURIcharacter (const char c)
{
	return (isalnum (c) || '-' == c || '_' == c || '.' == c || '~' == c);
}

// Function to encode a string in URI format.
size_t uri_encode_str	(
			char			*cunilog_restrict szURIencoded,
			const char		*cunilog_restrict str,
			size_t			len
						)
{
	ubf_assert (szURIencoded != str);

	size_t			ret     = 0;

	len = USE_STRLEN ? strlen (str) : len;

	unsigned char   c;
	if (szURIencoded)
	{
		while (len)
		{
			c = str [0];
			if (isValidURIcharacter (c))
			{
				*szURIencoded ++ = c;
				++ ret;
			} else
			{
				URI_encode_hex_from_octet (szURIencoded, c);
				szURIencoded	+= 3;
				ret				+= 3;
			}
			-- len;
			++ str;
		}
		*szURIencoded = ASCII_NUL;
	} else
	{
		while (len)
		{
			c = str [0];
			if (isValidURIcharacter (c))
				++ ret;
			else
				ret += 3;
			-- len;
			++ str;
		}
		++ ret;
	}
	return ret;
}
/****************************************************************************************

	File:		strmembuf.c
	Why:		String helper functions for SMEMBUF structures.
	OS:			C99
	Author:		Thomas
	Created:	2023-05-25

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2023-05-25	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./strintuint.h"
		#include "./strmembuf.h"
		#include "./membuf.h"
		#include "./ubfmem.h"
		#include "./ubfdebug.h"
	#else
		#include "./strintuint.h"
		#include "./strmembuf.h"
		#include "./../mem/membuf.h"
		#include "./../mem/ubfmem.h"
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

size_t SMEMBUFfromStrReserveBytes (SMEMBUF *pmb, const char *str, size_t len, size_t reserve)
{
	ubf_assert_non_NULL (pmb);
	ubf_assert_non_NULL (str);
	ubf_assert_non_0 (len);

	size_t l = USE_STRLEN == len ? strlen (str) : len;
	size_t lstr = l;
	l += reserve;
	
	growToSizeSMEMBUF (pmb, l + 1);
	if (isUsableSMEMBUF (pmb))
	{
		memcpy (pmb->buf.pch, str, lstr);
		pmb->buf.pch [lstr] = ASCII_NUL;
		return lstr;
	}
	return 0;
}

size_t SMEMBUFfromStr (SMEMBUF *pmb, const char *str, size_t len)
{
	ubf_assert_non_NULL (pmb);
	ubf_assert_non_NULL (str);
	ubf_assert_non_0 (len);

	size_t l = USE_STRLEN == len ? strlen (str) : len;
	growToSizeSMEMBUF (pmb, l + 1);
	if (isUsableSMEMBUF (pmb))
	{
		memcpy (pmb->buf.pch, str, l);
		pmb->buf.pch [l] = ASCII_NUL;
		return l;
	}
	return 0;
}

size_t SMEMBUFstrFromUINT64 (SMEMBUF *pmb, uint64_t ui)
{
	ubf_assert_non_NULL (pmb);
	
	char asc [UBF_UINT64_SIZE];
	size_t l = ubf_str_from_uint64 (asc, ui);
	return SMEMBUFfromStr (pmb, asc, l);
}

size_t SMEMBUFstrconcat (SMEMBUF *pmb, size_t len, char *str, size_t lenstr, size_t reserve)
{
	ubf_assert_non_NULL	(pmb);
	ubf_assert			(isInitialisedSMEMBUF (pmb));

	len = USE_STRLEN == len ? strlen (pmb->buf.pcc) : len;
	if (str)
	{
		lenstr = USE_STRLEN == lenstr ? strlen (str) : lenstr;
		if (pmb->size > len + lenstr)
		{	// Both strings fit in the current buffer.
			memcpy (pmb->buf.pch + len, str, lenstr);
			len += lenstr;
			pmb->buf.pch [len] = ASCII_NUL;
		} else
		{	// The current buffer is too small.
			size_t st = ALIGNED_SIZE (len + lenstr + 1 + reserve, STRMEMBUF_DEFAULT_ALIGNMENT);
			char *sz = ubf_malloc (st);
			if (sz)
			{
				memcpy (sz, pmb->buf.pch, len);
				memcpy (sz + len, str, lenstr);
				len += lenstr;
				sz [len] = ASCII_NUL;
				ubf_free (pmb->buf.pch);
				pmb->buf.pch	= sz;
				pmb->size		= st;
			} else
			{	// Heap allocation failed.
				doneSMEMBUF (pmb);
				len = 0;
			}
		}
	}
	return len;
}
/****************************************************************************************

	File:		strwildcards.h
	Why:		Wildcard functions
	OS:			C99.
	Author:		Thomas
	Created:	2022-03-21
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2022-03-21	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./strwildcards.h"
	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./ubfdebug.h"
		#include "./memstrstr.h"
		#include "./Warnings.h"
	#else
		#include "./../dbg/ubfdebug.h"
		#include "./../mem/memstrstr.h"
		#include "./../pre/Warnings.h"
	#endif

#endif

const char *wildCardFileName (size_t *plen, const char *ccPath)
{
	ubf_assert (NULL != ccPath);

	// Must not be NULL and not be an empty string.
	if (ccPath && ccPath [0])
	{
		size_t		stPath		= strlen (ccPath);
		const char	*ccEnd		= NULL;
		
		// Must not end with a directory separator.
		ubf_assert (0 < stPath);							// Not supported.
		ubf_assert ('\\' != ccPath [stPath - 1]);			// Not supported.
		ubf_assert ('/' != ccPath [stPath - 1]);			// Not supported.
		if (strchr (ccPath, '*') || strchr (ccPath, '\?') || ':' == ccPath [stPath - 1])
		{	// According to
			//	https://docs.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-findfirstfilew ,
			//	only the filename can contain wildcard characters.
			ccEnd = strrchr (ccPath, '\\');
			if (NULL == ccEnd)
				ccEnd = strrchr (ccPath, '/');
			if (NULL == ccEnd)
			{
				ccEnd = strrchr (ccPath, ':');
				if (ccEnd)
				{	// Jump over the directory separator.
					++ ccEnd;
				}
			}
			ubf_assert (NULL != ccEnd);
			ubf_assert (ccEnd > ccPath);
			stPath -= ccEnd - ccPath;
			if (plen)
				*plen = stPath;
			return ccEnd;
		}
	}
	if (plen)
		*plen = 0;
	return NULL;
}

size_t lenPathWithoutWildcardFileName (const char *ccPath)
{
	const char	*cc;
	size_t		stL;
	size_t		stW;
	size_t		stRet	= 0;
	
	ubf_assert (NULL != ccPath);
	if (ccPath && ccPath [0])
	{
		stL = strlen (ccPath);
		cc = wildCardFileName (&stW, ccPath);
		if (cc && stW)
			stRet = stL - stW;
		else
			stRet = stL;
	}
	return stRet;
}

static inline const char *nextPathSeparator (const char *str, size_t *idx, size_t len)
{
	ubf_assert_non_NULL (str);
	ubf_assert_non_NULL (idx);

	while (len && '/' != str [*idx] && '\\' != str [*idx])
	{
		-- len;
		++ *idx;
	}
	return (len && ('/' == str [*idx] || '\\' == str [*idx]) ? str + *idx : NULL);
}

static inline const char *nextCharOrPathSeparator (const char *str, size_t *idx, char c, size_t len)
{
	ubf_assert_non_NULL (str);
	ubf_assert_non_NULL (idx);

	while (len && '/' != str [*idx] && '\\' != str [*idx] && c != str [*idx])
	{
		-- len;
		++ *idx;
	}
	return (len && ('/' == str [*idx] || '\\' == str [*idx] || c == str [*idx]) ? str + *idx : NULL);
}

static inline const char *nextChar (const char *str, size_t *idx, char c, size_t len)
{
	ubf_assert_non_NULL (str);
	ubf_assert_non_NULL (idx);

	while (len && c != str [*idx])
	{
		-- len;
		++ *idx;
	}
	return (len && c == str [*idx] ? str + *idx : NULL);
}

enum hasGlobCharAfterStar
{
	en_globCharAfterStar,
	en_globDepleted,
	en_globCharAfterDoubleStar,
	en_globDoubleCharDepleted
};
typedef enum hasGlobCharAfterStar enGl;

static inline enGl globCharAfterStar	(
		char			*cunilog_restrict c,
		size_t			*cunilog_restrict g,
		const char		*cunilog_restrict ccGlob,
		size_t			*cunilog_restrict lnGlob
										)
{
	ubf_assert_non_NULL	(c);
	ubf_assert_non_NULL	(g);
	ubf_assert_non_NULL	(lnGlob);

	++ *g;

	if (*g < *lnGlob && '*' == ccGlob [*g])
	{
		++ *g;

		while (*g < *lnGlob)
		{
			*c = ccGlob [*g];
			if ('*' != *c)
				return en_globCharAfterDoubleStar;
			++ *g;
		}
		return en_globDoubleCharDepleted;
	}

	while (*g < *lnGlob)
	{
		*c = ccGlob [*g];
		if ('*' != *c)
			return en_globCharAfterStar;
		++ *g;
	}
	return en_globDepleted;
}

/*
	Comparison function or macro.
*/
#ifdef STRWILDCARDS_CHARSEQUAL_FNCT
	// For easier debugging.
	static inline bool charsEqual (char c1, char c2)
	{
		if ('/' == c1 && '\\' == c2)
			return true;
		if ('\\' == c1 && '/' == c2)
			return true;
		if (c1 == c2)
			return true;
		return false;
	}
#else
	#define charsEqual(c1, c2)							\
		(												\
				(c1 == c2)								\
			||	('/'	== c1 && '\\'	== c2)			\
			||	('\\'	== c1 && '/'	== c2)			\
		)
#endif

// Neither r nor p are uninitialised.
DISABLE_WARNING_POTENTIALLY_UNINITIALISED_VARIABLE_USED ()
DISABLE_WARNING_POTENTIALLY_UNINITIALISED_LOCAL_POINTER_USED ()

static inline const char *handleGlobStars	(
							bool		*r,
							size_t		*cunilog_restrict s,
							size_t		*cunilog_restrict g,
							const char	*cunilog_restrict ccStri,
							size_t		*cunilog_restrict lnStri,
							const char	*cunilog_restrict ccGlob,
							size_t		*cunilog_restrict lnGlob
											)
{
	ubf_assert_non_NULL	(r);
	ubf_assert_non_NULL	(s);
	ubf_assert_non_NULL	(g);
	ubf_assert_non_NULL	(ccStri);
	ubf_assert_non_NULL	(lnStri);
	ubf_assert_non_NULL	(ccGlob);
	ubf_assert_non_NULL	(lnGlob);

	char		c;
	const char	*p;
	enGl		gl;

	gl = globCharAfterStar (&c, g, ccGlob, lnGlob);
	ubf_assert	(
						en_globCharAfterStar		== gl
					||	en_globDepleted				== gl
					||	en_globCharAfterDoubleStar	== gl
					||	en_globDoubleCharDepleted	== gl
				);
	switch (gl)
	{
		case en_globCharAfterDoubleStar:
			p = nextChar (ccStri, s, c, *lnStri - *s);
			*r = false;
			break;
		case en_globDoubleCharDepleted:
			p = NULL;
			*r = true;
			break;
		case en_globCharAfterStar:
			p = nextCharOrPathSeparator (ccStri, s, c, *lnStri - *s);
			*r = false;
			break;
		case en_globDepleted:
			p = nextPathSeparator (ccStri, s, *lnStri - *s);
			*r = true;
			break;
	}
	return p;
}

static inline bool globMatchInt	(
		const char		*cunilog_restrict ccStri,
		size_t			*cunilog_restrict lnStri,
		const char		*cunilog_restrict ccGlob,
		size_t			*cunilog_restrict lnGlob
								)
{
	ubf_assert_non_NULL	(ccStri);
	ubf_assert_non_NULL	(lnStri);
	ubf_assert_non_NULL	(ccGlob);
	ubf_assert_non_NULL	(lnGlob);

	size_t		s		= 0;
	size_t		g		= 0;
	bool		r;
	const char *p;

	while (s < *lnStri && g < *lnGlob)
	{
		switch (ccGlob [g])
		{
			case '\?':
				break;
			case '*':
				p = handleGlobStars (&r, &s, &g, ccStri, lnStri, ccGlob, lnGlob);
				if (NULL == p)
					return r;
				break;
			default:
				if (!charsEqual (ccStri [s], ccGlob [g]))
					return false;
				break;
		}
		++ s;
		++ g;
	}
	// "a" against "a*" or "a********".
	while (g < *lnGlob && '*' == ccGlob [g])
		++ g;
	return s - *lnStri == g - *lnGlob;
}

DEFAULT_WARNING_POTENTIALLY_UNINITIALISED_VARIABLE_USED ()
DEFAULT_WARNING_POTENTIALLY_UNINITIALISED_LOCAL_POINTER_USED ()

bool matchWildcardPattern	(
		const char		*cunilog_restrict ccStri,	size_t lnStri,
		const char		*cunilog_restrict ccGlob,	size_t lnGlob
							)
{
	lnStri = USE_STRLEN == lnStri ? strlen (ccStri) : lnStri;
	lnGlob = USE_STRLEN == lnGlob ? strlen (ccGlob) : lnGlob;

	if (lnGlob)
	{
		if (lnStri)
			return globMatchInt (ccStri, &lnStri, ccGlob, &lnGlob);
		return false;
	}
	return true;
}

static inline const wchar_t *nextPathSeparatorW (const wchar_t *str, size_t *idx, size_t len)
{
	ubf_assert_non_NULL (str);
	ubf_assert_non_NULL (idx);

	while (len && L'/' != str [*idx] && L'\\' != str [*idx])
	{
		-- len;
		++ *idx;
	}
	return (len && (L'/' == str [*idx] || L'\\' == str [*idx]) ? str + *idx : NULL);
}

static inline const wchar_t *nextCharOrPathSeparatorW (const wchar_t *str, size_t *idx, wchar_t c, size_t len)
{
	ubf_assert_non_NULL (str);
	ubf_assert_non_NULL (idx);

	while (len && L'/' != str [*idx] && L'\\' != str [*idx] && c != str [*idx])
	{
		-- len;
		++ *idx;
	}
	return (len && (L'/' == str [*idx] || L'\\' == str [*idx] || c == str [*idx]) ? str + *idx : NULL);
}

static inline const wchar_t *nextCharW (const wchar_t *str, size_t *idx, wchar_t c, size_t len)
{
	ubf_assert_non_NULL (str);
	ubf_assert_non_NULL (idx);

	while (len && c != str [*idx])
	{
		-- len;
		++ *idx;
	}
	return (len && c == str [*idx] ? str + *idx : NULL);
}

static inline enGl globCharAfterStarW	(
		wchar_t			*cunilog_restrict c,
		size_t			*cunilog_restrict g,
		const wchar_t	*cunilog_restrict ccGlob,
		size_t			*cunilog_restrict lnGlob
										)
{
	ubf_assert_non_NULL	(c);
	ubf_assert_non_NULL	(g);
	ubf_assert_non_NULL	(lnGlob);

	++ *g;

	if (*g < *lnGlob && L'*' == ccGlob [*g])
	{
		++ *g;

		while (*g < *lnGlob)
		{
			*c = ccGlob [*g];
			if (L'*' != *c)
				return en_globCharAfterDoubleStar;
			++ *g;
		}
		return en_globDoubleCharDepleted;
	}

	while (*g < *lnGlob)
	{
		*c = ccGlob [*g];
		if (L'*' != *c)
			return en_globCharAfterStar;
		++ *g;
	}
	return en_globDepleted;
}

#ifdef STRWILDCARDS_CHARSEQUAL_FNCT
	// For easier debugging.
	static inline bool charsEqualW (wchar_t c1, wchar_t c2)
	{
		if (L'/' == c1 && L'\\' == c2)
			return true;
		if (L'\\' == c1 && L'/' == c2)
			return true;
		if (c1 == c2)
			return true;
		return false;
	}
#else
	#define charsEqualW(c1, c2)							\
		(												\
				(c1 == c2)								\
			||	(L'/'	== c1 && L'\\'	== c2)			\
			||	(L'\\'	== c1 && L'/'	== c2)			\
		)
#endif

// Neither r nor p are uninitialised.
DISABLE_WARNING_POTENTIALLY_UNINITIALISED_VARIABLE_USED ()
DISABLE_WARNING_POTENTIALLY_UNINITIALISED_LOCAL_POINTER_USED ()

static inline const wchar_t *handleGlobStarsW	(
							bool			*r,
							size_t			*cunilog_restrict s,
							size_t			*cunilog_restrict g,
							const wchar_t	*cunilog_restrict ccStri,
							size_t			*cunilog_restrict lnStri,
							const wchar_t	*cunilog_restrict ccGlob,
							size_t			*cunilog_restrict lnGlob
												)
{
	ubf_assert_non_NULL	(r);
	ubf_assert_non_NULL	(s);
	ubf_assert_non_NULL	(g);
	ubf_assert_non_NULL	(ccStri);
	ubf_assert_non_NULL	(lnStri);
	ubf_assert_non_NULL	(ccGlob);
	ubf_assert_non_NULL	(lnGlob);

	wchar_t			c;
	const wchar_t	*p;
	enGl			gl;

	gl = globCharAfterStarW (&c, g, ccGlob, lnGlob);
	ubf_assert	(
						en_globCharAfterStar		== gl
					||	en_globDepleted				== gl
					||	en_globCharAfterDoubleStar	== gl
					||	en_globDoubleCharDepleted	== gl
				);
	switch (gl)
	{
		case en_globCharAfterDoubleStar:
			p = nextCharW (ccStri, s, c, *lnStri - *s);
			*r = false;
			break;
		case en_globDoubleCharDepleted:
			p = NULL;
			*r = true;
			break;
		case en_globCharAfterStar:
			p = nextCharOrPathSeparatorW (ccStri, s, c, *lnStri - *s);
			*r = false;
			break;
		case en_globDepleted:
			p = nextPathSeparatorW (ccStri, s, *lnStri - *s);
			*r = true;
			break;
	}
	return p;
}

static inline bool globMatchIntW	(
		const wchar_t	*cunilog_restrict ccStri,
		size_t			*cunilog_restrict lnStri,
		const wchar_t	*cunilog_restrict ccGlob,
		size_t			*cunilog_restrict lnGlob
									)
{
	ubf_assert_non_NULL	(ccStri);
	ubf_assert_non_NULL	(lnStri);
	ubf_assert_non_NULL	(ccGlob);
	ubf_assert_non_NULL	(lnGlob);

	size_t			s		= 0;
	size_t			g		= 0;
	bool			r;
	const wchar_t *p;

	while (s < *lnStri && g < *lnGlob)
	{
		switch (ccGlob [g])
		{
			case L'\?':
				break;
			case L'*':
				p = handleGlobStarsW (&r, &s, &g, ccStri, lnStri, ccGlob, lnGlob);
				if (NULL == p)
					return r;
				break;
			default:
				if (!charsEqualW (ccStri [s], ccGlob [g]))
					return false;
				break;
		}
		++ s;
		++ g;
	}
	// "a" against "a*" or "a********".
	while (g < *lnGlob && L'*' == ccGlob [g])
		++ g;
	return s - *lnStri == g - *lnGlob;
}

DEFAULT_WARNING_POTENTIALLY_UNINITIALISED_VARIABLE_USED ()
DEFAULT_WARNING_POTENTIALLY_UNINITIALISED_LOCAL_POINTER_USED ()

bool matchWildcardPatternW	(
		const wchar_t	*cunilog_restrict ccStri,	size_t lnStri,
		const wchar_t	*cunilog_restrict ccGlob,	size_t lnGlob
							)
{
	lnStri = USE_STRLEN == lnStri ? wcslen (ccStri) : lnStri;
	lnGlob = USE_STRLEN == lnGlob ? wcslen (ccGlob) : lnGlob;

	if (lnGlob)
	{
		if (lnStri)
			return globMatchIntW (ccStri, &lnStri, ccGlob, &lnGlob);
		return false;
	}
	return true;
}

#ifdef STRWILDCARDS_BUILD_TEST_FNCT
	bool strwildcards_test_function (void)
	{
		bool b = true;

		/*
			Note that in C99 a question mark can either be escaped (as in "\?") or
			stand alone (as in "?"). See C99 standard, 6.4.4.4 and 6.4.5.
			Some of the tests below escape it, others don't.
		*/
		// ASCII/UTF-8.
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a", 1, "a*", 2));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a", 1, "*a", 2));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a", 0, "b", 0));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("abc", 0, "b", 0));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a", 1, "b", 0));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("a", 1, "b", 1));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a", 1, "a", 1));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a/b/c", USE_STRLEN, "\?/\?/\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("abc", 3, "a*", 2));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("abc", 3, "*bc", 3));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("abc", 3, "a*c", 3));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("abc", 3, "a*d", 3));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("abc", 3, "d*d", 3));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("abc", 3, "*", 1));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("abc", 3, "d", 1));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("abc", 3, "*x", 1));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("abc", 3, "*x", 2));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a/b/c", USE_STRLEN, "*\\*\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a/b/c", USE_STRLEN, "*\\b\\c", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("a/b/c", USE_STRLEN, "*\\d\\e", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("a/b/c", USE_STRLEN, "a\\b\\e", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a/b/c", USE_STRLEN, "a\\b\\c", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("a/b/c", USE_STRLEN, "a\\*\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("a/b/c", USE_STRLEN, "a\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("abcdef", USE_STRLEN, "a\?c*f", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("abcdef", USE_STRLEN, "a\?c*fx", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("0123456789", USE_STRLEN, "0123456789", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("0123456789", USE_STRLEN, "0123\?56789", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("0123\0""456789", 11, "0123\0""\?56789", 11));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/home/user", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/home/use\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/home/us\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/home/u\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/home/\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/home\?\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/hom\?\?\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/h*/user", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/home/user", USE_STRLEN, "/h*/usxr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/*/us*r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/home/user", USE_STRLEN, "/*/us*x", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/home/user", USE_STRLEN, "/*/us*\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/*/us**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/***", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/****", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/*****", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/user", USE_STRLEN, "/*/\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/*\?usr", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/*\?/usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/*\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/*?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "\?*\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "\?*\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "\\*\\\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/?\?\?\?/?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "?home?usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "?h*m*?u*r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/**x", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/**sr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/**r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/**usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/***r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/****r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/*****r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/*****b", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/*********************b", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/*********************r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/home/usr", USE_STRLEN, "/**?", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/", USE_STRLEN, "/*?", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/", USE_STRLEN, "/**?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/1234567", USE_STRLEN, "/?**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/1", USE_STRLEN, "/?**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/1", USE_STRLEN, "/?***", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("/1", USE_STRLEN, "/??**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("/1", USE_STRLEN, "?**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("1234/67", USE_STRLEN, "?**7", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("1234/67", USE_STRLEN, "?**67", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("1234/67", USE_STRLEN, "?**/67", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("1234/67", USE_STRLEN, "?**4/67", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("0123", USE_STRLEN, "?**123", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("123", USE_STRLEN, "?**123", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("1/2/3", USE_STRLEN, "1?2?3", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("1/2/3", USE_STRLEN, "1/2?3", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPattern ("1/2/3", USE_STRLEN, "1/2?*", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPattern ("A", 1, "a", 1));

		// Wide characters, which means UTF-16.
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a", 1, L"a*", 2));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a", 1, L"*a", 2));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a", 0, L"b", 0));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"abc", 0, L"b", 0));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a", 1, L"b", 0));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"a", 1, L"b", 1));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a", 1, L"a", 1));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a/b/c", USE_STRLEN, L"\?/\?/\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"abc", 3, L"a*", 2));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"abc", 3, L"*bc", 3));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"abc", 3, L"a*c", 3));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"abc", 3, L"a*d", 3));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"abc", 3, L"d*d", 3));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"abc", 3, L"*", 1));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"abc", 3, L"d", 1));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"abc", 3, L"*x", 1));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"abc", 3, L"*x", 2));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a/b/c", USE_STRLEN, L"*\\*\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a/b/c", USE_STRLEN, L"*\\b\\c", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"a/b/c", USE_STRLEN, L"*\\d\\e", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"a/b/c", USE_STRLEN, L"a\\b\\e", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a/b/c", USE_STRLEN, L"a\\b\\c", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"a/b/c", USE_STRLEN, L"a\\*\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"a/b/c", USE_STRLEN, L"a\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"abcdef", USE_STRLEN, L"a\?c*f", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"abcdef", USE_STRLEN, L"a\?c*fx", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"0123456789", USE_STRLEN, L"0123456789", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"0123456789", USE_STRLEN, L"0123\?56789", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"0123\0"L"456789", 11, L"0123\0"L"\?56789", 11));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/home/user", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/home/use\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/home/us\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/home/u\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/home/\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/home\?\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/hom\?\?\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/h*/user", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/h*/usxr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/*/us*r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/*/us*x", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/*/us*\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/*/us**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/***", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/****", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/*****", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/user", USE_STRLEN, L"/*/\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/*\?usr", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/*\?/usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/*\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/*?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"\?*\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"\?*\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"\\*\\\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/?\?\?\?/?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"?home?usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"?h*m*?u*r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/**x", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/**sr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/**r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/**usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/***r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/****r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/*****r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/*****b", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/*********************b", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/*********************r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/home/usr", USE_STRLEN, L"/**?", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/", USE_STRLEN, L"/*?", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/", USE_STRLEN, L"/**?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/1234567", USE_STRLEN, L"/?**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/1", USE_STRLEN, L"/?**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/1", USE_STRLEN, L"/?***", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"/1", USE_STRLEN, L"/??**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"/1", USE_STRLEN, L"?**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"1234/67", USE_STRLEN, L"?**7", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"1234/67", USE_STRLEN, L"?**67", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"1234/67", USE_STRLEN, L"?**/67", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"1234/67", USE_STRLEN, L"?**4/67", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"0123", USE_STRLEN, L"?**123", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"123", USE_STRLEN, L"?**123", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"1/2/3", USE_STRLEN, L"1?2?3", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"1/2/3", USE_STRLEN, L"1/2?3", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== matchWildcardPatternW (L"1/2/3", USE_STRLEN, L"1/2?*", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== matchWildcardPatternW (L"A", 1, L"a", 1));

		return b;
	}
#endif
/****************************************************************************************

	File:		ubfcharscountsandchecks.c
	Why:		Character counting and check routines
	OS:			C99
	Author:		Thomas
	Created:	2020-08-31
  
History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2020-08-31	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <ctype.h>
#include <string.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./ubfcharscountsandchecks.h"

#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	bool ubf_is_printable_ASCII (char c)
	{	// From SPC (20h, 32d) to '~' (7Eh, 126d).
		return ((unsigned char) c > 0x1F && (unsigned char) c < 0x7F);
	}
#endif

bool str_has_only_printable_ASCII (const char *sz, size_t len)
{
	len = STRLENSZ (sz, len);

	unsigned int ui;

	for (ui = 0; ui < len; ++ ui)
	{
		if (!ubf_is_printable_ASCII (sz [ui]))
			return false;
	}
	return true;
}

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	bool ubf_is_letter (char c)
	{
		return	(
				((unsigned char) c > 'A' - 1 && (unsigned char) c < 'Z' + 1)
			||	((unsigned char) c > 'a' - 1 && (unsigned char) c < 'z' + 1)
				);
	}
#endif

char *ubf_is_letter_until (char *ch, char c)
{
	while (*ch && c != *ch)
	{
		if (!ubf_is_letter (*ch))
		{
			return NULL;
		}
		++ ch;
	}
	if (c == *ch)
	{
		return ch;
	}
	return NULL;
}

/*
	Returns true when c is a white space character.
*/
#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	bool isWhiteSpace (char c)
	{
		return	(
						ASCII_SPC			== c
					||	ASCII_TAB			== c
					||	ASCII_VT			== c
				);
	}
#endif

#ifdef _MSC_VER
#pragma warning (default: 4706)									// Assignment within conditional expression.
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	bool isDirectorySeparator (char c)
	{
		return ('/' == c || '\\' == c);
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	bool isWrongDirectorySeparator (char c)
	{
		return (UBF_WRONG_DIR_SEP == c);
	}
#endif

size_t ubf_count_numerical_digits (const char *cc)
{
	size_t		stRet = 0;
	
	while (*cc)
	{
		if  (	/*
						(unsigned char) '0' - 1 < (unsigned char) *cc
					&&	(unsigned char) '9' + 1 > (unsigned char) *cc
				*/
				ubf_isdigit (*cc)
			)
		{
			++ stRet;
		}
		++ cc;
	}
	return stRet;
}

size_t ubf_count_starts_with_numerical_digits_up_to (const char *cc, size_t len)
{
	size_t		stRet = 0;
	
	while (*cc && len -- && ubf_isdigit (*cc))
	{
		++ stRet;
		++ cc;
	}
	return stRet;
}

size_t ubf_count_special_characters (const char *cc)
{
	size_t		stRet = 0;
	
	while (*cc)
	{
		if (ubf_is_special (*cc))
		{
			++ stRet;
		}
		++ cc;
	}
	return stRet;
}

size_t ubf_count_char (const char *cc, char c)
{
	size_t		stRet = 0;
	
	while (*cc)
	{
		if  (c == *cc)
		{
			++ stRet;
		}
		++ cc;
	}
	return stRet;
}

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	size_t ubf_obtain_strlen (const char *sz, size_t providedLength)
	{
		return sz ? (USE_STRLEN == providedLength ? strlen (sz) : providedLength) : 0;
	}
#endif
/****************************************************************************************

	File		cunilogerrors.c
	Why:		Cunilog return and error codes.
	OS:			C99
	Created:	2025-04-03

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-04-03	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include <stdbool.h>
	#include <inttypes.h>

	#include "./cunilogerrors.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./externC.h"
		#include "./ubfmem.h"
	#else
		#include "./../pre/externC.h"
		#include "./../mem/ubfmem.h"
	#endif

#endif

/****************************************************************************************

	File:		cunilogstructs.c
	Why:		Structures for cunilog.
	OS:			C99.
	Author:		Thomas
	Created:	2022-10-05

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2022-10-05	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./cunilogstructs.h"

#endif

const char		*szCunilogLogFileNameExtension	= ".log";
const wchar_t	*wcCunilogLogFileNameExtension	= L".log";
const size_t	lenCunilogLogFileNameExtension	= 4;			// ".log"
const size_t	sizCunilogLogFileNameExtension	= 4 + 1;		// ".log" + NUL.

/*
	The default event severity type.
*/
cueventsevfmtpy	cunilogEvtSeverityTypeDefault = cunilogEvtSeverityTypeChars3;

/*
	Default ANSI escape colour output for the cunilog_puts... and cunilog_printf...
	type functions.
*/
bool bUseCunilogDefaultOutputColour				= true;
/****************************************************************************************

	File		cunilogevtcmdsstructs.h
	Why:		Cunilog helper functions for event commands.
	OS:			C99
	Created:	2025-02-18

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-02-18	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	//#include <stdbool.h>
	//#include <inttypes.h>
	#include "./cunilogevtcmdsstructs.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./externC.h"
		#include "./ubfmem.h"
	#else
		#include "./../pre/externC.h"
		#include "./../mem/ubfmem.h"
	#endif

#endif
/****************************************************************************************

	File		cunilogevtcmds.c
	Why:		Cunilog helper functions for event commands.
	OS:			C99
	Created:	2025-02-17

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-02-17	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./cunilogstructs.h"
	#include "./cunilogevtcmds.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./externC.h"
		#include "./ArrayMacros.h"
		#include "./platform.h"
		#include "./ubfmem.h"
		#include "./ubfdebug.h"
	#else
		#include "./../pre/externC.h"
		#include "./../pre/ArrayMacros.h"
		#include "./../pre/platform.h"
		#include "./../mem/ubfmem.h"
		#include "./../dbg/ubfdebug.h"
	#endif

#endif

#ifdef PLATFORM_IS_POSIX
	#include <pthread.h>
#endif

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS

#define SIZCMDENUM	(sizeof (enum cunilogEvtCmd))

typedef struct sculCmdArr
{
	size_t		siz;
} SCULCMDARR;

SCULCMDARR culCmdSizes [] =
{
		SIZCMDENUM + sizeof (bool)							// cunilogCmdConfigUseColourForEcho
	,	SIZCMDENUM + sizeof (cueventsevfmtpy)				// cunilogCmdConfigEventSeverityFormatType
	,	SIZCMDENUM + sizeof (newline_t)						// cunilogCmdConfigCunilognewline
	,	SIZCMDENUM + sizeof (enum cunilogprocesstask)		// cunilogCmdConfigDisableTaskProcessors
	,	SIZCMDENUM + sizeof (enum cunilogprocesstask)		// cunilogCmdConfigEnableTaskProcessors
	,	SIZCMDENUM											// cunilogConfigDisableEchoProcessor
	,	SIZCMDENUM											// cunilogConfigEnableEchoProcessor
	,	SIZCMDENUM + sizeof (cunilogprio)					// cunilogCmdConfigSetLogPriority
};

#ifdef DEBUG
	bool culIsValidCmd (enum cunilogEvtCmd cmd)
	{
		ubf_assert (GET_ARRAY_LEN (culCmdSizes) == cunilogCmdConfigXAmountEnumValues);

		return 0 <= cmd && cunilogCmdConfigXAmountEnumValues > cmd;
	}
#endif

size_t culCmdRequiredSize (enum cunilogEvtCmd cmd)
{
	ubf_assert (0 <= cmd);
	ubf_assert (cunilogCmdConfigXAmountEnumValues > cmd);
	ubf_assert (GET_ARRAY_LEN (culCmdSizes) == cunilogCmdConfigXAmountEnumValues);

	if (culIsValidCmd (cmd))
		return culCmdSizes [cmd].siz;
	return CUNILOG_CMD_INVALID_SIZE;
}

void culCmdStoreEventCommand (unsigned char *szOut, enum cunilogEvtCmd cmd)
{
	ubf_assert_non_NULL (szOut);
	ubf_assert (sizeof (enum cunilogEvtCmd) == sizeof (cmd));
	ubf_assert (0 <= cmd);
	ubf_assert (cunilogCmdConfigXAmountEnumValues > cmd);
	ubf_assert (GET_ARRAY_LEN (culCmdSizes) == cunilogCmdConfigXAmountEnumValues);

	memcpy (szOut, &cmd, sizeof (cmd));
}

void culCmdStoreCmdConfigUseColourForEcho (unsigned char *szOut, bool bUseColour)
{
	ubf_assert_non_NULL (szOut);

	culCmdStoreEventCommand (szOut, cunilogCmdConfigUseColourForEcho);
	memcpy (szOut + sizeof (enum cunilogEvtCmd), &bUseColour, sizeof (bool));
}

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_SEVERITY_TYPE
	void culCmdStoreConfigEventSeverityFormatType (unsigned char *szOut, cueventsevfmtpy sevTpy)
	{
		ubf_assert_non_NULL (szOut);
		ubf_assert (sizeof (cueventsevfmtpy) == sizeof (sevTpy));

		culCmdStoreEventCommand (szOut, cunilogCmdConfigEventSeverityFormatType);
		memcpy (szOut + sizeof (enum cunilogEvtCmd), &sevTpy, sizeof (sevTpy));
	}
#endif

void culCmdStoreCmdConfigCunilognewline (unsigned char *szOut, newline_t nl)
{
	ubf_assert_non_NULL (szOut);
	ubf_assert (sizeof (newline_t) == sizeof (nl));

	culCmdStoreEventCommand (szOut, cunilogCmdConfigCunilognewline);
	memcpy (szOut + sizeof (enum cunilogEvtCmd), &nl, sizeof (nl));
}

/*
	These declarations are from cunilog.h. They are defined in cunilog.c.
*/
void ConfigCUNILOG_TARGETdisableTaskProcessors (CUNILOG_TARGET *put, enum cunilogprocesstask task);
void ConfigCUNILOG_TARGETenableTaskProcessors (CUNILOG_TARGET *put, enum cunilogprocesstask task);
void ConfigCUNILOG_TARGETdisableEchoProcessor (CUNILOG_TARGET *put);
void ConfigCUNILOG_TARGETenableEchoProcessor (CUNILOG_TARGET *put);

void culCmdStoreCmdConfigDisableTaskProcessors (unsigned char *szOut, enum cunilogprocesstask task)
{
	ubf_assert_non_NULL (szOut);

	culCmdStoreEventCommand (szOut, cunilogCmdConfigDisableTaskProcessors);
	memcpy (szOut + sizeof (enum cunilogEvtCmd), &task, sizeof (task));
}

void culCmdStoreCmdConfigEnableTaskProcessors (unsigned char *szOut, enum cunilogprocesstask task)
{
	ubf_assert_non_NULL (szOut);

	culCmdStoreEventCommand (szOut, cunilogCmdConfigEnableTaskProcessors);
	memcpy (szOut + sizeof (enum cunilogEvtCmd), &task, sizeof (task));
}

void culCmdConfigDisableTaskProcessors (CUNILOG_TARGET *put, unsigned char *szData)
{
	enum cunilogprocesstask task;

	memcpy (&task, szData, sizeof (task));
	ubf_assert (0 <= task);
	ubf_assert (task < cunilogProcessXAmountEnumValues);

	ConfigCUNILOG_TARGETdisableTaskProcessors (put, task);
}

void culCmdConfigEnableTaskProcessors (CUNILOG_TARGET *put, unsigned char *szData)
{
	enum cunilogprocesstask task;

	memcpy (&task, szData, sizeof (task));
	ubf_assert (0 <= task);
	ubf_assert (task < cunilogProcessXAmountEnumValues);

	ConfigCUNILOG_TARGETenableTaskProcessors (put, task);
}

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_SEVERITY_TYPE
	void culCmdStoreConfigLogThreadPriority (unsigned char *szOut, cunilogprio prio)
	{
		ubf_assert_non_NULL (szOut);
		ubf_assert (sizeof (cunilogprio) == sizeof (prio));

		culCmdStoreEventCommand (szOut, cunilogCmdConfigSetLogPriority);
		memcpy (szOut + sizeof (enum cunilogEvtCmd), &prio, sizeof (prio));
	}
#endif

#ifdef OS_IS_WINDOWS
	int icuWinPrioTable [cunilogPrioAmountEnumValues] =
	{
			/* cunilogPrioNormal			*/	THREAD_PRIORITY_NORMAL
		,	/* cunilogPrioBelowNormal		*/	THREAD_PRIORITY_BELOW_NORMAL
		,	/* cunilogPrioLow				*/	THREAD_PRIORITY_LOWEST
		,	/* cunilogPrioIdle				*/	THREAD_PRIORITY_IDLE
		,	/* cunilogPrioBeginBackground	*/	THREAD_MODE_BACKGROUND_BEGIN
		,	/* cunilogPrioEndBackground		*/	THREAD_MODE_BACKGROUND_END
	};
#else
	// These values haven't been tested yet! I (Thomas) just made them up in
	//	the hope they might do well enough.
	int icuPsxPrioTable [cunilogPrioAmountEnumValues] =
	{
			/* cunilogPrioNormal			*/	0
		,	/* cunilogPrioBelowNormal		*/	5
		,	/* cunilogPrioLow				*/	10
		,	/* cunilogPrioIdle				*/	19
		,	/* cunilogPrioBeginBackground	*/	19
		,	/* cunilogPrioEndBackground		*/	0
	};
#endif

#ifdef OS_IS_WINDOWS
	static bool SetWinCurrThreadPriority (int prio)
	{
		HANDLE hThread = GetCurrentThread ();
		return SetThreadPriority (hThread, prio);
	}
#else
	static bool SetPsxCurrThreadPriority (int prio)
	{	// See https://man7.org/linux/man-pages/man3/pthread_setschedprio.3.html .
		pthread_t tThread = pthread_self ();
		return 0 == pthread_setschedprio (tThread, prio);
	}
#endif

bool culCmdSetCurrentThreadPriority (cunilogprio prio)
{
	ubf_assert			(0 <= prio);
	ubf_assert			(prio < cunilogPrioAmountEnumValues);

	#ifdef PLATFORM_IS_WINDOWS
		return SetWinCurrThreadPriority (icuWinPrioTable [prio]);
	#else
		return SetPsxCurrThreadPriority (icuPsxPrioTable [prio]);
	#endif
}

void culCmdConfigSetLogPriority (unsigned char *szData)
{
	ubf_assert_non_NULL (szData);

	cunilogprio prio;

	memcpy (&prio, szData, sizeof (cunilogprio));

	#ifdef DEBUG
		bool b = culCmdSetCurrentThreadPriority (prio);
		ubf_assert_true (b);
	#else
		culCmdSetCurrentThreadPriority (prio);
	#endif
}

void culCmdChangeCmdConfigFromCommand (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->szDataToLog);
	ubf_assert_non_0	(pev->lenDataToLog);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);
	ubf_assert			(cunilogEvtTypeCommand == pev->evType);

	CUNILOG_TARGET *put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);

	unsigned char		*szData = pev->szDataToLog;
	enum cunilogEvtCmd	cmd;

	memcpy (&cmd, szData, sizeof (enum cunilogEvtCmd));
	szData += sizeof (enum cunilogEvtCmd);

	bool boolVal;

	switch (cmd)
	{
		case cunilogCmdConfigUseColourForEcho:
			#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
				memcpy (&boolVal, szData, sizeof (bool));
				if (boolVal)
					cunilogTargetSetUseColourForEcho (put);
				else
					cunilogTargetClrUseColourForEcho (put);
			#endif
			break;
		case cunilogCmdConfigEventSeverityFormatType:
			#ifndef CUNILOG_BUILD_WITHOUT_EVENT_SEVERITY_TYPE
				memcpy (&put->evSeverityType, szData, sizeof (cueventsevfmtpy));
			#endif
			break;
		case cunilogCmdConfigCunilognewline:
			memcpy (&put->unilogNewLine, szData, sizeof (newline_t));
			ubf_assert (0 <=put->unilogNewLine);
			ubf_assert (cunilogNewLineAmountEnumValues > put->unilogNewLine);
			break;
		case cunilogCmdConfigDisableTaskProcessors:
			culCmdConfigDisableTaskProcessors (put, szData);
			break;
		case cunilogCmdConfigEnableTaskProcessors:
			culCmdConfigEnableTaskProcessors (put, szData);
			break;
		case cunilogCmdConfigDisableEchoProcessor:
			ConfigCUNILOG_TARGETdisableEchoProcessor (put);
			break;
		case cunilogCmdConfigEnableEchoProcessor:
			ConfigCUNILOG_TARGETenableEchoProcessor (put);
			break;
		case cunilogCmdConfigSetLogPriority:
			culCmdConfigSetLogPriority (szData);
			break;
	}
}

#endif														// Of #ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS.
/****************************************************************************************

	File:		cunilog.c
	Why:		Interface functions for cunilog.
	OS:			C99.
	Author:		Thomas
	Created:	2022-10-05

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2022-10-05	Thomas			Created.

****************************************************************************************/

/*
	This file is maintained as part of Cunilog. See https://github.com/cunilog .
*/

/*
	This code is covered by the MIT License. See https://opensource.org/license/mit .

	Copyright (c) 2024, 2025 Thomas

	Permission is hereby granted, free of charge, to any person obtaining a copy of this
	software and associated documentation files (the "Software"), to deal in the Software
	without restriction, including without limitation the rights to use, copy, modify,
	merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
	permit persons to whom the Software is furnished to do so, subject to the following
	conditions:

	The above copyright notice and this permission notice shall be included in all copies
	or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
	INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
	PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
	HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
	CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
	OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <stdbool.h>
#include <stdarg.h>

#ifndef CUNILOG_USE_COMBINED_MODULE

	#include "./cunilog.h"
	#include "./cunilogerrors.h"
	#include "./cunilogevtcmds.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
	
		#include "./ubf_date_and_time.h"
		#include "./ubfdebug.h"
		#include "./ArrayMacros.h"
		#include "./membuf.h"
		#include "./check_utf8.h"
		#include "./ubfcharscountsandchecks.h"
		#include "./stransi.h"
		#include "./strfilesys.h"
		#include "./strintuint.h"
		#include "./strhexdump.h"
		#include "./strmembuf.h"
		#include "./strisabsolutepath.h"
		#include "./strnewline.h"
		#include "./CompressFile.h"
		#include "./ExeFileName.h"
		#include "./UserHome.h"
		
		#if defined (PLATFORM_IS_WINDOWS)
			#include "./WinAPI_U8.h"
			#include "./WinAPI_ReadDirFncts.h"
		#elif defined (PLATFORM_IS_POSIX)
			#include "./PsxCompressFile.h"
			#include "./PsxReadDirFncts.h"
			#include "./PsxTrash.h"
			#include "./PsxHome.h"
		#endif
		
		#include "./unref.h"
		
	#else
	
		#include "./../datetime/ubf_date_and_time.h"
		#include "./../dbg/ubfdebug.h"
		#include "./../pre/ArrayMacros.h"
		#include "./../mem/membuf.h"
		#include "./../string/check_utf8.h"
		#include "./../string/ubfcharscountsandchecks.h"
		#include "./../string/stransi.h"
		#include "./../string/strfilesys.h"
		#include "./../string/strintuint.h"
		#include "./../string/strhexdump.h"
		#include "./../string/strmembuf.h"
		#include "./../string/strisabsolutepath.h"
		#include "./../string/strnewline.h"
		#include "./../string/strwildcards.h"
		#include "./../OS/CompressFile.h"
		#include "./../OS/ExeFileName.h"
		#include "./../OS/UserHome.h"
		
		#if defined (PLATFORM_IS_WINDOWS)
			#include "./../OS/Windows/WinAPI_U8.h"
			#include "./../OS/Windows/WinAPI_ReadDirFncts.h"
		#elif defined (PLATFORM_IS_POSIX)
			#include "./../OS/POSIX/PsxCompressFile.h"
			#include "./../OS/POSIX/PsxReadDirFncts.h"
			#include "./../OS/POSIX/PsxTrash.h"
			#include "./../OS/POSIX/PsxHome.h"
		#endif
		
		#include "./../pre/unref.h"
		
	#endif

#endif

#ifdef PLATFORM_IS_POSIX
	#include <errno.h>
	#include <unistd.h>
#endif

static CUNILOG_TARGET CUNILOG_TARGETstatic;
CUNILOG_TARGET *pCUNILOG_TARGETstatic	= &CUNILOG_TARGETstatic;

/*
	Copy from the structure header cunilogstructs.h to avoid switching between code and header
	during development. Can/could be deleted when finalised.

typedef struct cunilog_processor
{
	enum cunilogprocesstask			task;					// What to apply.
	enum cunilogprocessfrequency	freq;					// When to apply
															//	(frequency/theshold type).

	// Trigger threshold and its current value.
	uint64_t						thr;					// Required value of cur before this
															//	processor is applied/run.
	uint64_t						cur;					// The current counter value.

	void							*pData;					// Pointer to processor-specific data.
	uint64_t						uiOpts;					// Option flags. See OPT_CUNPROC_
															//	definitions below.
} CUNILOG_PROCESSOR;

*/

/*
	Our default static pData structures for our standard processors.
*/
/*
CUNILOG_LOGFILE			defcuppLogFile						=
	CUNILOG_INIT_DEF_CUNILOG_LOGFILE ();
CUNILOG_ROTATION_DATA	defcuppRotatorRenameLogfiles		=
	CUNILOG_INIT_DEF_CUNILOG_ROTATION_DATA_RENAME_LOGFILES
		();
CUNILOG_ROTATION_DATA	defcuppRotatorFS_compress			=
	CUNILOG_INIT_DEF_CUNILOG_ROTATION_DATA_FS_COMPRESS
		(CUNILOG_DEFAULT_ROTATOR_KEEP_UNCOMPRESSED);
CUNILOG_ROTATION_DATA	defcuppRotatorMove_to_recycle_bin	=
	CUNILOG_INIT_DEF_CUNILOG_ROTATION_DATA_MOVE_TO_RECYCLE_BIN
		(CUNILOG_DEFAULT_ROTATOR_KEEP_NONTRASHED);
CUNILOG_ROTATION_DATA	defcuppRotatorDelete				=
	CUNILOG_INIT_DEF_CUNILOG_ROTATION_DATA_DELETE
		(CUNILOG_DEFAULT_ROTATOR_KEEP_NONDELETED);
*/
/*
	Our default processors. Only used for our static CUNILOG_TARGET structure.
*/
/*
CUNILOG_PROCESSOR		defcuppEcho							=
	CUNILOG_INIT_DEF_ECHO_PROCESSOR;
CUNILOG_PROCESSOR		defcuppUpdateLogfileName			=
	CUNILOG_INIT_DEF_UPDATELOGFILENAME_PROCESSOR;
CUNILOG_PROCESSOR		defcuppWriteToLogfile				=
	CUNILOG_INIT_DEF_WRITETTOLOGFILE_PROCESSOR		(&defcuppLogFile);
CUNILOG_PROCESSOR		defcuppFlushLogFile					=
	CUNILOG_INIT_DEF_FLUSHLOGFILE_PROCESSOR;
CUNILOG_PROCESSOR		defcuppRotateLogfilesRename			=
	CUNILOG_INIT_DEF_RENAMELOGFILES_PROCESSOR		(&defcuppRotatorRenameLogfiles);
CUNILOG_PROCESSOR		defcuppRotateLogfilesFScompress		=
	CUNILOG_INIT_DEF_LOGFILESFSCOMPRESS_PROCESSOR	(&defcuppRotatorFS_compress);
CUNILOG_PROCESSOR		defcuppRotateLogfilesMoveToTrash	=
	CUNILOG_INIT_DEF_LOGFILESMOVETOTRASH_PROCESSOR	(&defcuppRotatorMove_to_recycle_bin);
*/
// The list with the processors.
/*
CUNILOG_PROCESSOR	*defcupp [] =
{
	&defcuppEcho,											// Writes to console.
	&defcuppUpdateLogfileName,								// Updates the date/timestamp within
															//	the log file's name.
	&defcuppWriteToLogfile,									// Writes out to log file.
	&defcuppFlushLogFile,									// Flushes the log file.
	&defcuppRotateLogfilesRename,							// Rename the log files.
	&defcuppRotateLogfilesFScompress,						// Rotates the log files.
	&defcuppRotateLogfilesMoveToTrash						// Rotates the log files.
};
*/

CUNILOG_PROCESSOR **CreateCopyCUNILOG_PROCESSORs (CUNILOG_PROCESSOR *cps [], unsigned int n)
{
	ubf_assert_non_NULL	(cps);
	ubf_assert_non_NULL	(cps [0]);
	ubf_assert_non_0	(n);

	// First we calculate the required space for the copy. An alignment of
	//	CUNILOG_POINTER_ALIGNMENT may prevent a cache miss or two.

	// Space for the array of pointers to processors, plus the actual processors.
	size_t stTot = ALIGNED_SIZE (n * sizeof (CUNILOG_PROCESSOR *), CUNILOG_POINTER_ALIGNMENT);
	stTot += n * ALIGNED_SIZE (sizeof (CUNILOG_PROCESSOR), CUNILOG_POINTER_ALIGNMENT);

	// Space for the pData members.
	unsigned int u;
	for (u = 0; u < n; ++ u)
	{
		switch (cps [u]->task)
		{
			case cunilogProcessWriteToLogFile:
				/* The CUNILOG_LOGFILE structure now belongs to the target.
				stTot += ALIGNED_SIZE (sizeof (CUNILOG_LOGFILE), CUNILOG_POINTER_ALIGNMENT);
				*/
				break;
			case cunilogProcessRotateLogfiles:
				stTot += ALIGNED_SIZE (sizeof (CUNILOG_ROTATION_DATA), CUNILOG_POINTER_ALIGNMENT);
				break;
			case cunilogProcessCustomProcessor:
				stTot += ALIGNED_SIZE (sizeof (CUNILOG_CUSTPROCESS), CUNILOG_POINTER_ALIGNMENT);
				break;
			default:
				break;
		}
	}

	CUNILOG_PROCESSOR **cpn = ubf_malloc (stTot);
	if (cpn)
	{
		unsigned char *p = (unsigned char *) cpn;
		// We leave space for the array of pointers.
		p += ALIGNED_SIZE (n * sizeof (CUNILOG_PROCESSOR *), CUNILOG_POINTER_ALIGNMENT);

		for (u = 0; u < n; ++ u)
		{
			memcpy (p, cps [u], sizeof (CUNILOG_PROCESSOR));
			cpn [u] = (CUNILOG_PROCESSOR *) p;
			p += ALIGNED_SIZE (sizeof (CUNILOG_PROCESSOR), CUNILOG_POINTER_ALIGNMENT);

			switch (cps [u]->task)
			{
				case cunilogProcessWriteToLogFile:
					/* The CUNILOG_LOGFILE structure now belongs to the target.
					memcpy (p, cps [u]->pData, sizeof (CUNILOG_LOGFILE));
					cpn [u]->pData = p;
					p += ALIGNED_SIZE (sizeof (CUNILOG_LOGFILE), CUNILOG_POINTER_ALIGNMENT);
					*/
					break;
				case cunilogProcessRotateLogfiles:
					memcpy (p, cps [u]->pData, sizeof (CUNILOG_ROTATION_DATA));
					cpn [u]->pData = p;
					p += ALIGNED_SIZE (sizeof (CUNILOG_ROTATION_DATA), CUNILOG_POINTER_ALIGNMENT);
					break;
				case cunilogProcessCustomProcessor:
					memcpy (p, cps [u]->pData, sizeof (CUNILOG_CUSTPROCESS));
					cpn [u]->pData = p;
					p += ALIGNED_SIZE (sizeof (CUNILOG_CUSTPROCESS), CUNILOG_POINTER_ALIGNMENT);
					break;
				default:
					break;
			}
		}
	}
	return cpn;
}

void *DoneCopyCUNILOG_PROCESSORs (CUNILOG_PROCESSOR *cps [])
{
	ubf_assert_non_NULL (cps);

	ubf_free (cps);
	return NULL;
}

CUNILOG_PROCESSOR **CreateNewDefaultProcessors (unsigned int *pn)
{
	// The return value is useless without knowing the amount of processors.
	ubf_assert_non_NULL (pn);

	/*
		We simply place default processors on the stack andcreate a copy of them on the heap.
	*/
	// Our pData structures.
	CUNILOG_ROTATION_DATA	stkcuppRotatorRenameLogfiles		=
		CUNILOG_INIT_DEF_CUNILOG_ROTATION_DATA_RENAME_LOGFILES;
	CUNILOG_ROTATION_DATA	stkcuppRotatorFS_compress			=
		CUNILOG_INIT_DEF_CUNILOG_ROTATION_DATA_FS_COMPRESS
			(CUNILOG_DEFAULT_ROTATOR_KEEP_UNCOMPRESSED);
	CUNILOG_ROTATION_DATA	stkcuppRotatorMove_to_recycle_bin	=
		CUNILOG_INIT_DEF_CUNILOG_ROTATION_DATA_MOVE_TO_RECYCLE_BIN
			(CUNILOG_DEFAULT_ROTATOR_KEEP_NONTRASHED);
	/*
	CUNILOG_ROTATION_DATA	stkcuppRotatorDelete				=
		CUNILOG_INIT_DEF_CUNILOG_ROTATION_DATA_DELETE
			(CUNILOG_DEFAULT_ROTATOR_KEEP_NONDELETED);
	*/
	// The processors.
	CUNILOG_PROCESSOR		stkcuppEcho							=
		CUNILOG_INIT_DEF_ECHO_PROCESSOR;
	CUNILOG_PROCESSOR		stkcuppUpdateLogfileName			=
		CUNILOG_INIT_DEF_UPDATELOGFILENAME_PROCESSOR;
	CUNILOG_PROCESSOR		stkcuppWriteToLogfile				=
		CUNILOG_INIT_DEF_WRITETTOLOGFILE_PROCESSOR;
	CUNILOG_PROCESSOR		stkcuppFlushLogFile					=
		CUNILOG_INIT_DEF_FLUSHLOGFILE_PROCESSOR;
	CUNILOG_PROCESSOR		stkcuppRotateLogfilesRename			=
		CUNILOG_INIT_DEF_RENAMELOGFILES_PROCESSOR		(&stkcuppRotatorRenameLogfiles);
	CUNILOG_PROCESSOR		stkcuppRotateLogfilesFScompress		=
		CUNILOG_INIT_DEF_LOGFILESFSCOMPRESS_PROCESSOR	(&stkcuppRotatorFS_compress);
	CUNILOG_PROCESSOR		stkcuppRotateLogfilesMoveToTrash	=
		CUNILOG_INIT_DEF_LOGFILESMOVETOTRASH_PROCESSOR	(&stkcuppRotatorMove_to_recycle_bin);

	// The list with the processors.
	CUNILOG_PROCESSOR	*stkcupp [] =
	{
		&stkcuppEcho,											// Writes to console.
		&stkcuppUpdateLogfileName,								// Updates the date/timestamp within
																//	the log file's name.
		&stkcuppWriteToLogfile,									// Writes out to log file.
		&stkcuppFlushLogFile,									// Flushes the log file.
		&stkcuppRotateLogfilesRename,							// Rename the log files.
		&stkcuppRotateLogfilesFScompress,						// Rotates the log files.
		&stkcuppRotateLogfilesMoveToTrash						// Rotates the log files.
	};

	unsigned int n = ARRAYSIZE (stkcupp);
	CUNILOG_PROCESSOR **cps = CreateCopyCUNILOG_PROCESSORs (stkcupp, n);
	if (cps)
		*pn = n;
	return cps;
}

/*
	Space required for the date/timestamp in a filename.
*/
size_t arrLengthTimeStampFromPostfix [] =					// [unilogPostfixAmountEnumValues]
{
	/* cunilogPostfixNone					*/		0
	/* cunilogPostfixMinute					*/	,	LEN_ISO8601DATEHOURANDMINUTE
	/* cunilogPostfixMinuteT				*/	,	LEN_ISO8601DATEHOURANDMINUTE
	/* cunilogPostfixHour					*/	,	LEN_ISO8601DATEANDHOUR
	/* cunilogPostfixHourT					*/	,	LEN_ISO8601DATEANDHOUR
	/* cunilogPostfixDay					*/	,	LEN_ISO8601DATE
	/* cunilogPostfixWeek					*/	,	LEN_ISO8601YEARANDWEEK
	/* cunilogPostfixMonth					*/	,	LEN_ISO8601YEARANDMONTH
	/* cunilogPostfixYear					*/	,	LEN_ISO8601YEAR

	/* cunilogPostfixLogMinute				*/	,	LEN_ISO8601DATEHOURANDMINUTE
	/* cunilogPostfixLogMinuteT				*/	,	LEN_ISO8601DATEHOURANDMINUTE
	/* cunilogPostfixLogHour				*/	,	LEN_ISO8601DATEANDHOUR
	/* cunilogPostfixLogHourT				*/	,	LEN_ISO8601DATEANDHOUR
	/* cunilogPostfixLogDay					*/	,	LEN_ISO8601DATE
	/* cunilogPostfixLogWeek				*/	,	LEN_ISO8601YEARANDWEEK
	/* cunilogPostfixLogMonth				*/	,	LEN_ISO8601YEARANDMONTH
	/* cunilogPostfixLogYear				*/	,	LEN_ISO8601YEAR

													// Space for "." + UNIT64.
	/* cunilogPostfixDotNumberMinutely		*/	,	1 + UBF_UINT64_LEN
	/* cunilogPostfixDotNumberHourly		*/	,	1 + UBF_UINT64_LEN
	/* cunilogPostfixDotNumberDaily			*/	,	1 + UBF_UINT64_LEN
	/* cunilogPostfixDotNumberWeekly		*/	,	1 + UBF_UINT64_LEN
	/* cunilogPostfixDotNumberMonthly		*/	,	1 + UBF_UINT64_LEN
	/* cunilogPostfixDotNumberYearly		*/	,	1 + UBF_UINT64_LEN
};

#ifdef DEBUG
	size_t lenDateTimeStampFromPostfix (enum cunilogpostfix postfix)
	{
		ubf_assert (0 <= postfix);
		ubf_assert (postfix < cunilogPostfixAmountEnumValues);

		return arrLengthTimeStampFromPostfix [postfix];
	}
#endif

/*
	Note the wildcard mask for "YYYY-MM-DD HH_MI" (cunilogPostfixMinute) and
	"YYYY-MM-DDTHH_MI" (cunilogPostfixMinuteT). Hours and minutes are separated by an
	underscore instead of a colon.
*/
const char *arrPostfixWildcardMask []	=		// [cunilogPostfixAmountEnumValues]
{
	"",											// cunilogPostfixNone
	"\?\?\?\?-\?\?-\?\? \?\?_\?\?",				// cunilogPostfixMinute					"YYYY-MM-DD HH_MI".
	"\?\?\?\?-\?\?-\?\?T\?\?_\?\?",				// cunilogPostfixMinuteT				"YYYY-MM-DDTHH_MI".
	"\?\?\?\?-\?\?-\?\? \?\?",					// cunilogPostfixHour					"YYYY-MM-DD HH".
	"\?\?\?\?-\?\?-\?\?T\?\?",					// cunilogPostfixHourT					"YYYY-MM-DDTHH".
	"\?\?\?\?-\?\?-\?\?",						// cunilogPostfixDay					"YYYY-MM-DD".
	"\?\?\?\?-W\?\?",							// cunilogPostfixWeek					"YYYY-Wnn".
	"\?\?\?\?-\?\?",							// cunilogPostfixMonth					"YYYY-MM".
	"\?\?\?\?",									// cunilogPostfixYear					"YYYY".

	// Current/active logfile has no postfix.
	//	"file.log", "file_YYYY-MM-DD HH_MI.log", etc.
	"\?\?\?\?-\?\?-\?\? \?\?_\?\?",				// cunilogPostfixLogMinute				"YYYY-MM-DD HH_MI".
	"\?\?\?\?-\?\?-\?\?T\?\?_\?\?",				// cunilogPostfixLogMinuteT				"YYYY-MM-DDTHH_MI".
	"\?\?\?\?-\?\?-\?\? \?\?",					// cunilogPostfixLogHour				"YYYY-MM-DD HH".
	"\?\?\?\?-\?\?-\?\?T\?\?",					// cunilogPostfixLogHourT				"YYYY-MM-DDTHH".
	"\?\?\?\?-\?\?-\?\?",						// cunilogPostfixLogDay					"YYYY-MM-DD".
	"\?\?\?\?-W\?\?",							// cunilogPostfixLogWeek				"YYYY-Wnn".
	"\?\?\?\?-\?\?",							// cunilogPostfixLogMonth				"YYYY-MM".
	"\?\?\?\?",									// cunilogPostfixLogYear				"YYYY".

	//	"file.log", "file.log.1", "file.log.2", etc.
	"",											// cunilogPostfixDotNumberMinutely
	"",											// cunilogPostfixDotNumberHourly
	"",											// cunilogPostfixDotNumberDaily
	"",											// cunilogPostfixDotNumberWeekly
	"",											// cunilogPostfixDotNumberMonthly
	""											// cunilogPostfixDotNumberYearly
};

#ifdef DEBUG
	const char *postfixMaskFromLogPostfix (enum cunilogpostfix postfix)
	{
		ubf_assert (postfix >= 0);
		ubf_assert (postfix < cunilogPostfixAmountEnumValues);

		return arrPostfixWildcardMask [postfix];
	}
#endif

static void noPostfix (char *sz, UBF_TIMESTAMP ts)
{
	UNUSED_PARAMETER (sz);
	UNUSED_PARAMETER (ts);
}

/*
	This should be:
	
	void (*obtainTimeStampAsString [unilogPostfixAmountEnumValues]) (char *, UBF_TIMESTAMP) =
	
	But then we wouldn't be able to check for a mismatch at runtime.
*/
void (*obtainTimeStampAsString []) (char *, UBF_TIMESTAMP) =
{
	/* cunilogPostfixNone					*/		noPostfix
	/* cunilogPostfixMinute					*/	,	ISO8601DateHourAndMinute_from_UBF_TIMESTAMP_
	/* cunilogPostfixMinuteT				*/	,	ISO8601TDateHourAndMinute_from_UBF_TIMESTAMP_
	/* cunilogPostfixHour					*/	,	ISO8601DateAndHour_from_UBF_TIMESTAMPc
	/* cunilogPostfixHourT					*/	,	ISO8601TDateAndHour_from_UBF_TIMESTAMPc
	/* cunilogPostfixDay					*/	,	ISO8601Date_from_UBF_TIMESTAMPc
	/* cunilogPostfixWeek					*/	,	ISO8601YearAndWeek_from_UBF_TIMESTAMPc
	/* cunilogPostfixMonth					*/	,	ISO8601YearAndMonth_from_UBF_TIMESTAMPc
	/* cunilogPostfixYear					*/	,	ISO8601Year_from_UBF_TIMESTAMPc

	/* cunilogPostfixLogMinute				*/	,	ISO8601DateHourAndMinute_from_UBF_TIMESTAMP_
	/* cunilogPostfixLogMinuteT				*/	,	ISO8601TDateHourAndMinute_from_UBF_TIMESTAMP_
	/* cunilogPostfixLogHour				*/	,	ISO8601DateAndHour_from_UBF_TIMESTAMPc
	/* cunilogPostfixLogHourT				*/	,	ISO8601TDateAndHour_from_UBF_TIMESTAMPc
	/* cunilogPostfixLogDay					*/	,	ISO8601Date_from_UBF_TIMESTAMPc
	/* cunilogPostfixLogWeek				*/	,	ISO8601YearAndWeek_from_UBF_TIMESTAMPc
	/* cunilogPostfixLogMonth				*/	,	ISO8601YearAndMonth_from_UBF_TIMESTAMPc
	/* cunilogPostfixLogYear				*/	,	ISO8601Year_from_UBF_TIMESTAMPc

	/* cunilogPostfixDotNumberMinutely		*/	,	noPostfix
	/* cunilogPostfixDotNumberHourly		*/	,	noPostfix
	/* cunilogPostfixDotNumberDaily			*/	,	noPostfix
	/* cunilogPostfixDotNumberWeekly		*/	,	noPostfix
	/* cunilogPostfixDotNumberMonthly		*/	,	noPostfix
	/* cunilogPostfixDotNumberYearly		*/	,	noPostfix
};

#ifdef DEBUG
	void obtainDateAndTimeStamp (char *szWhere, UBF_TIMESTAMP ts, enum cunilogpostfix postfix)
	{
		ubf_assert_non_NULL (szWhere);
		ubf_assert (postfix >= 0);
		ubf_assert (postfix < cunilogPostfixAmountEnumValues);

		obtainTimeStampAsString [postfix] (szWhere, ts);
	}
#else
	#define obtainDateAndTimeStamp(sw, ts, pfx)			\
		obtainTimeStampAsString [(pfx)] ((sw), (ts))
#endif

#ifdef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#define needsOrHasLocker() (false)
#else
	#define needsOrHasLocker(put)						\
		(!(cunilogSingleThreaded == (put)->culogType))
#endif

#ifdef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#define canBenefitFromSpincount() (false)
#else
	#define canBenefitFromSpincount(put)											\
		(																			\
				(cunilogSingleThreadedSeparateLoggingThread	== (put)->culogType)	\
			||	(cunilogMultiThreadedSeparateLoggingThread	== (put)->culogType)	\
		)
#endif

#ifdef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#define hasSeparateLoggingThread() (false)
#else
	#define hasSeparateLoggingThread(put)						\
		canBenefitFromSpincount (put)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#ifdef PLATFORM_IS_WINDOWS
		static inline void InitCriticalSection (CUNILOG_TARGET *put)
		{
			if (canBenefitFromSpincount (put))
			{
				#ifdef OS_IS_WINDOWS_XP
					InitializeCriticalSectionAndSpinCount	(
						&put->cl.cs,
						CUNILOG_WINDOWS_CRITICAL_SECTION_SPIN_COUNT
															);
				#else
					InitializeCriticalSectionEx	(
						&put->cl.cs,
						CUNILOG_WINDOWS_CRITICAL_SECTION_SPIN_COUNT,
						CRITICAL_SECTION_NO_DEBUG_INFO
												);
				#endif
			} else
			{
				InitializeCriticalSection (&put->cl.cs);
			}
		}
	#else
		static inline void InitCriticalSection (CUNILOG_TARGET *put)
		{
			pthread_mutex_init (&put->cl.mt, NULL);
			/*
			if (canBenefitFromSpincount (put))
				pthread_mutex_init (&put->cl.mt, NULL);
			else
				pthread_mutex_init (&put->cl.mt, NULL);
			*/
		}
	#endif
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline void InitCUNILOG_LOCKER (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (needsOrHasLocker (put))
		{
			InitCriticalSection (put);
		}
		#ifdef CUNILOG_BUILD_DEBUG_TEST_LOCKER
			put->cl.bInitialised = true;
		#endif
	}
#else
	#define InitCUNILOG_LOCKER(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline void EnterCUNILOG_LOCKER (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (needsOrHasLocker (put))
		{
			#ifdef CUNILOG_BUILD_DEBUG_TEST_LOCKER
				ubf_assert_true (put->cl.bInitialised);
			#endif

			#ifdef OS_IS_WINDOWS
				EnterCriticalSection (&put->cl.cs);
			#else
				pthread_mutex_lock (&put->cl.mt);
			#endif
			cunilogSetDebugQueueLocked (put);
		}
	}
#else
	#define EnterCUNILOG_LOCKER(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#ifdef PLATFORM_IS_WINDOWS
		static inline void LeaveCUNILOG_LOCKER (CUNILOG_TARGET *put)
		{
			ubf_assert_non_NULL (put);

			if (needsOrHasLocker (put))
			{
				#ifdef CUNILOG_BUILD_DEBUG_TEST_LOCKER
					ubf_assert_true (put->cl.bInitialised);
				#endif

				LeaveCriticalSection (&put->cl.cs);
				cunilogClrDebugQueueLocked (put);
			}
		}
	#else
		static inline void LeaveCUNILOG_LOCKER (CUNILOG_TARGET *put)
		{
			ubf_assert_non_NULL (put);

			if (needsOrHasLocker (put))
			{
				#ifdef CUNILOG_BUILD_DEBUG_TEST_LOCKER
					ubf_assert_true (put->cl.bInitialised);
				#endif

				pthread_mutex_unlock (&put->cl.mt);
				cunilogClrDebugQueueLocked (put);
			}
		}
	#endif
#else
	#define LeaveCUNILOG_LOCKER(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#ifdef OS_IS_WINDOWS
		static inline void DestroyCriticalSection (CUNILOG_TARGET *put)
		{
			DeleteCriticalSection (&put->cl.cs);
		}
	#else
		static inline void DestroyCriticalSection (CUNILOG_TARGET *put)
		{
			pthread_mutex_destroy (&put->cl.mt);
		}
	#endif
#endif
		
#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline void DoneCUNILOG_LOCKER (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		#ifdef CUNILOG_BUILD_DEBUG_TEST_LOCKER
			ubf_assert_true (put->cl.bInitialised);
		#endif

		if (needsOrHasLocker (put))
			DestroyCriticalSection (put);
		#ifdef CUNILOG_BUILD_DEBUG_TEST_LOCKER
			put->cl.bInitialised = false;
		#endif
	}
#else
	#define DoneCUNILOG_LOCKER(x)
#endif

void InitSCUNILOGNPI (SCUNILOGNPI *pni)
{
	ubf_assert_non_NULL (pni);

	initSMEMBUF (&pni->smbRotMask);
	pni->lenRotMask			= 0;
	pni->nIgnoredTotal		= 0;
	pni->pNextVecCunilogFls	= NULL;
}

void DoneSCUNILOGNPI (SCUNILOGNPI *pni)
{
	ubf_assert_non_NULL (pni);

	if (isUsableSMEMBUF (&pni->smbRotMask))
		doneSMEMBUF (&pni->smbRotMask);
}

/*
	ObtainCurrentWorkingDirectoy

	Returns the current working directory with an ending forward or backslash as
	an SMEMBUF. Should probably go in its own module at some point as it certainly
	does not belong here.

	The function returns the amount of characters it has written to the buffer of
	the SMEMBUF structure, not including a terminating NUL character.
*/
static size_t ObtainCurrentWorkingDirectoy (SMEMBUF *mb)
{
	ubf_assert_non_NULL (mb);
	ubf_assert (isInitialisedSMEMBUF (mb));

	size_t	r = 0;

	#if defined (PLATFORM_IS_WINDOWS)

		WCHAR wc [1];
		DWORD dw = GetCurrentDirectoryW (1, wc);
		WCHAR *pw = malloc (sizeof (WCHAR) * dw + sizeof (WCHAR));
		if (pw)
		{
			DWORD dn = GetCurrentDirectoryW (dw, pw);
			ubf_assert (dn);
			ubf_assert (dn < dw);
			if (dn)
			{
				int rq = reqUTF8sizeFromUTF16wchars (dn);
				growToSizeSMEMBUF (mb, rq);
				if (isUsableSMEMBUF (mb))
				{
					int n = UTF8_from_WinU16 (mb->buf.pch, rq, pw);
					if (n)
					{
						mb->buf.pch [n - 1]	= '\\';
						mb->buf.pch [n]		= '\0';
					}
					r = n;
				}
			}
			free (pw);
		}

	#elif defined (PLATFORM_IS_POSIX)

		size_t	siz		= 32;
		char	*pr		= NULL;
		bool	rtry	= true;

		while (rtry)
		{
			growToSizeSMEMBUF (mb, siz + 1);
			if (isUsableSMEMBUF (mb))
			{
				pr = getcwd (mb->buf.pch, siz);
				if (pr)
				{
					size_t l = strlen (pr);
					pr [l]		= '/';
					pr [l + 1]	= '\0';
					r = l + 1;
					rtry = false;
				} else
				{
					if (ERANGE == errno)
						siz += 32;
					else
						rtry = false;
				}
			}
		}

	#else

		#error Not supported!

	#endif

	return r;
}

static size_t ObtainRelativeLogPathBase (SMEMBUF *mb, enCunilogRelPath relLogPath)
{
	switch (relLogPath)
	{
		#ifdef DEBUG
		case cunilogPath_isAbsolute:
			ubf_assert_msg (false, "This should have been caught before we got here");
			// Hopefully this causes an access violation/segmentation fault.
			return CUNILOG_SIZE_ERROR;
		#endif
		case cunilogPath_relativeToExecutable:
			return ObtainPathFromExecutableModule (mb);
		case cunilogPath_relativeToCurrentDir:
			return ObtainCurrentWorkingDirectoy (mb);
		case cunilogPath_relativeToHomeDir:
			return ObtainUserHomeDirectoy (mb);
	}
	return 0;
}

bool CunilogGetAbsPathFromAbsOrRelPath	(
		SMEMBUF				*psmb,
		size_t				*plmb,
		const char			*szAbsOrRelPath,
		size_t				lnAbsOrRelPath,
		enCunilogRelPath	absOrRelPath
										)
{
	ubf_assert_non_NULL (psmb);
	ubf_assert_non_NULL (szAbsOrRelPath);
	ubf_assert_non_0	(lnAbsOrRelPath);
	ubf_assert			(isInitialisedSMEMBUF (psmb));

	SMEMBUF b		= SMEMBUF_INITIALISER;
	lnAbsOrRelPath	= USE_STRLEN == lnAbsOrRelPath ? strlen (szAbsOrRelPath) : lnAbsOrRelPath;
	size_t	ln		= lnAbsOrRelPath;

	if (is_absolute_path_l (szAbsOrRelPath, ln))
	{
		if (!isDirSep (szAbsOrRelPath [ln - 1]))
		{
			if (SMEMBUFfromStrReserveBytes (&b, szAbsOrRelPath, ln, 2))
			{
				b.buf.pch [ln] = UBF_DIR_SEP;
				++ ln;
				b.buf.pch [ln] = ASCII_NUL;
			}
		} else
		{
			SMEMBUFfromStr (&b, szAbsOrRelPath, ln);
		}
	} else
	{	// The path is relative. It cannot be absolute.
		if (cunilogPath_isAbsolute == absOrRelPath)
			return false;
		SMEMBUF	t	= SMEMBUF_INITIALISER;
		size_t	lp	= ObtainRelativeLogPathBase (&t, absOrRelPath);
		ln = lp + lnAbsOrRelPath;
		if (!isDirSep (szAbsOrRelPath [lnAbsOrRelPath - 1]))
		{
			growToSizeSMEMBUF (&b, ln + 1);
			if (isUsableSMEMBUF (&b))
			{
				copySMEMBUF (&b, &t);
				memcpy (b.buf.pch + lp, szAbsOrRelPath, lnAbsOrRelPath);
				b.buf.pch [lp + lnAbsOrRelPath] = UBF_DIR_SEP;
				++ ln;
			}
		} else
		{
			growToSizeSMEMBUF (&b, ln);
			if (isUsableSMEMBUF (&b))
				memcpy (b.buf.pch + lp, szAbsOrRelPath, lnAbsOrRelPath);
		}
		doneSMEMBUF (&t);
	}
	str_correct_dir_separators (b.buf.pch, ln);
	str_remove_path_navigators (b.buf.pch, &ln);
	copySMEMBUFsiz (psmb, &b, ln + 1);						// NUL-terminated.
	doneSMEMBUF (&b);
	if (plmb)
		*plmb = ln;											// The length of the destination.
	return true;
}

char *createLogPathInCUNILOG_TARGET	(
		CUNILOG_TARGET *put, const char *szLogPath, size_t len, enCunilogRelPath relLogPath
									)
{
	ubf_assert_non_NULL (put);

	initSMEMBUF (&put->mbLogPath);
	if (szLogPath)
	{
		ubf_assert (0 != len);

		bool b;

		// The function only fails if szLogPath is NULL or relative but cunilogLogPath_isAbsolute
		//	has been given.
		b = CunilogGetAbsPathFromAbsOrRelPath (
				&put->mbLogPath, &put->lnLogPath, szLogPath, len, relLogPath
												);
		if (!b)
		{
			SetCunilogError (put, CUNILOG_ERROR_ABS_OR_REL_PATH, CUNILOG_SYSTEM_ERROR_NOT_SUPPORTED);
			return NULL;
		}
	} else
	{
		// If szLogPath is NULL its length should be 0.
		ubf_assert_0 (len);

		// Cannot be absolute.
		if (cunilogPath_isAbsolute == relLogPath)
		{
			SetCunilogError	(put, CUNILOG_ERROR_ABS_OR_REL_PATH, CUNILOG_SYSTEM_ERROR_NOT_SUPPORTED);
			return NULL;
		}

		// No path given. We use the path specified with relLogPath.
		put->lnLogPath = ObtainRelativeLogPathBase (&put->mbLogPath, relLogPath);
		if (CUNILOG_SIZE_ERROR == put->lnLogPath)
		{
			SetCunilogError (put, CUNILOG_ERROR_PATH_BASE, CUNILOG_SYSTEM_ERROR_BUFFER_OVERFLOW);
			return NULL;
		}
		if (0 == put->lnLogPath)
		{
			SetCunilogError	(put, CUNILOG_ERROR_PATH_BASE, CUNILOG_SYSTEM_ERROR_NOT_SUPPORTED);
			return NULL;
		}
	}
	put->uiOpts |= CUNILOGTARGET_LOGPATH_ALLOCATED;
	return put->mbLogPath.buf.pch;
}

char *CreateLogPath_smb	(
		SMEMBUF *psmb, size_t *psiz, const char *szLogPath, size_t len, enCunilogRelPath relLogPath
						)
{
	ubf_assert_non_NULL (psmb);
	ubf_assert_non_NULL (psiz);

	if (szLogPath)
	{
		ubf_assert (0 != len);

		bool b;
		b = CunilogGetAbsPathFromAbsOrRelPath (psmb, psiz, szLogPath, len, relLogPath);
		// The function only fails if szLogPath is NULL or relative but cunilogLogPath_isAbsolute
		//	has been given.
		if (!b)
			return NULL;
	} else
	{
		// If szLogPath is NULL its length should be 0.
		ubf_assert_0 (len);

		// Cannot be absolute.
		if (cunilogPath_isAbsolute == relLogPath)
			return NULL;

		// No path given. We use the path specified with relLogPath.
		*psiz = ObtainRelativeLogPathBase (psmb, relLogPath);
	}
	return psmb->buf.pch;
}

static inline const char *RemoveSlashesFromStart (const char *szAppName, size_t *plen)
{
	ubf_assert_non_NULL (plen);
	ubf_assert_non_NULL (szAppName);
	ubf_assert_non_0 (*plen);

	while (*plen && isDirectorySeparator (szAppName [0]))
	{
		*plen		-= 1;
		szAppName	+= 1;
	}
	return szAppName;
}

char *createAppNameInCUNILOG_TARGET (CUNILOG_TARGET *put, const char *szAppName, size_t len)
{
	ubf_assert_non_NULL (put);

	initSMEMBUF (&put->mbAppName);
	if (szAppName && len)
	{
		size_t l = len;
		szAppName = RemoveSlashesFromStart (szAppName, &l);
		ubf_assert_non_NULL (szAppName);
		ubf_assert_non_0 (l);

		if (szAppName && l)
		{
			SMEMBUFfromStr (&put->mbAppName, szAppName, l);
			str_correct_dir_separators (put->mbAppName.buf.pch, l);
			put->lnAppName = l;
		} else
			return NULL;
	} else
	{
		put->lnAppName = ObtainAppNameFromExecutableModule (&put->mbAppName);
		if (0 == put->lnAppName)
			return NULL;
	}
	put->uiOpts |= CUNILOGTARGET_APPNAME_ALLOCATED;
	return put->mbAppName.buf.pch;
}

// Would be	frequencyTbl [cunilogPostfixAmountEnumValues], but we want to check the array's
//	number of elements with our self-test function.
enum cunilogprocessfrequency frequencyTbl [] =
{
	/* cunilogPostfixNone				*/			cunilogProcessAppliesTo_nAlways
	/* cunilogPostfixMinute				*/		,	cunilogProcessAppliesTo_MinuteChanged
	/* cunilogPostfixMinuteT			*/		,	cunilogProcessAppliesTo_MinuteChanged
	/* cunilogPostfixHour				*/		,	cunilogProcessAppliesTo_HourChanged
	/* cunilogPostfixHourT				*/		,	cunilogProcessAppliesTo_HourChanged
	/* cunilogPostfixDay				*/		,	cunilogProcessAppliesTo_DayChanged
	/* cunilogPostfixWeek				*/		,	cunilogProcessAppliesTo_WeekChanged
	/* cunilogPostfixMonth				*/		,	cunilogProcessAppliesTo_MonthChanged
	/* cunilogPostfixYear				*/		,	cunilogProcessAppliesTo_YearChanged

	/* cunilogPostfixLogMinute			*/		,	cunilogProcessAppliesTo_MinuteChanged
	/* cunilogPostfixLogMinuteT			*/		,	cunilogProcessAppliesTo_MinuteChanged
	/* cunilogPostfixLogHour			*/		,	cunilogProcessAppliesTo_HourChanged
	/* cunilogPostfixLogHourT			*/		,	cunilogProcessAppliesTo_HourChanged
	/* cunilogPostfixLogDay				*/		,	cunilogProcessAppliesTo_DayChanged
	/* cunilogPostfixLogWeek			*/		,	cunilogProcessAppliesTo_WeekChanged
	/* cunilogPostfixLogMonth			*/		,	cunilogProcessAppliesTo_MonthChanged
	/* cunilogPostfixLogYear			*/		,	cunilogProcessAppliesTo_YearChanged

	/* cunilogPostfixDotNumberMinutely	*/		,	cunilogProcessAppliesTo_MinuteChanged
	/* cunilogPostfixDotNumberHourly	*/		,	cunilogProcessAppliesTo_HourChanged
	/* cunilogPostfixDotNumberDaily		*/		,	cunilogProcessAppliesTo_DayChanged
	/* cunilogPostfixDotNumberWeekly	*/		,	cunilogProcessAppliesTo_WeekChanged
	/* cunilogPostfixDotNumberMonthly	*/		,	cunilogProcessAppliesTo_MonthChanged
	/* cunilogPostfixDotNumberYearly	*/		,	cunilogProcessAppliesTo_YearChanged
};

static inline enum cunilogprocessfrequency defaultFrequencyFromPostfix (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert			(0 <= put->culogPostfix);
	ubf_assert			(cunilogPostfixAmountEnumValues > put->culogPostfix);

	return frequencyTbl [put->culogPostfix];
}

static inline bool hasLogPostfix (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	return	(
					cunilogPostfixLogMinute	<= put->culogPostfix
				&&	cunilogPostfixLogYear	>= put->culogPostfix
			);
}


static inline bool hasDotNumberPostfix (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	return	(
					cunilogPostfixDotNumberMinutely	<= put->culogPostfix
				&&	cunilogPostfixDotNumberYearly	>= put->culogPostfix
			);
}

static inline void correctDefaultFrequency (CUNILOG_PROCESSOR *cp, CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->cprocessors);
	ubf_assert_non_0	(put->nprocessors);
	ubf_assert_non_NULL	(cp);

	if (cunilogProcessAppliesTo_Auto == cp->freq)
	{
		switch (cp->task)
		{
			case cunilogProcessNoOperation:
			case cunilogProcessEchoToConsole:
			case cunilogProcessWriteToLogFile:
			case cunilogProcessCustomProcessor:
				cp->freq = cunilogProcessAppliesTo_nAlways;
				break;
			case cunilogProcessUpdateLogFileName:
				if (hasLogPostfix (put) || hasDotNumberPostfix (put))
				{
					cp->freq = cunilogProcessAppliesTo_nAlways;
					optCunProcSetOPT_CUNPROC_DISABLED (cp->uiOpts);
					break;
				}
				// Falls through.
			case cunilogProcessFlushLogFile:
			case cunilogProcessRotateLogfiles:
				cp->freq = defaultFrequencyFromPostfix (put);
				break;
			case cunilogProcessXAmountEnumValues:
				ubf_assert_msg (false, "Illegal value");
				break;
		}
	}
}

static inline void defaultProcessorParameters (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->cprocessors);
	ubf_assert_non_0	(put->nprocessors);

	CUNILOG_PROCESSOR		*cp;
	CUNILOG_ROTATION_DATA	*prPrev				= NULL;
	CUNILOG_ROTATION_DATA	*prCurr;
	//CUNILOG_LOGFILE			*pLF				= NULL;

	unsigned int n = 0;
	while (n < put->nprocessors)
	{
		cp = put->cprocessors [n];
		correctDefaultFrequency (cp, put);
		switch (cp->task)
		{
			case cunilogProcessRotateLogfiles:
				prCurr = cp->pData;
				if (hasDotNumberPostfix (put))
				{	// For this rotator the trick with remembering how many files we've
					//	had so far doesn't work. However, we need to sort the retrieved
					//	files list with active dot number sorting.
					cunilogTargetSetFSneedsSorting	(put);
				} else
				/*
				if (hasLogPostfix (put))
				{	// The current/active logfile for this rotator is also "<logfile>.log".
				} else
				*/
				{
					if (prPrev && CUNILOG_MAX_ROTATE_AUTO == prPrev->nMaxToRotate)
						prPrev->nMaxToRotate = prCurr->nIgnore;
					prPrev = prCurr;
				}
				break;
			case cunilogProcessWriteToLogFile:
				ubf_assert_NULL (cp->pData);
				/*
				pLF = cp->pData;
				*/
				break;
			case cunilogProcessFlushLogFile:
				ubf_assert_NULL (cp->pData);
				/*	Now belongs to the target.
				// If we abort here, the caller has supplied a cunilogProcessFlushLogFile
				//	processor without supplying a cunilogProcessWriteToLogFile processor
				//	first.
				ubf_assert_non_NULL (pLF);
				if (NULL == cp->pData)
					cp->pData = pLF;
				*/
				break;
		}
		++ n;
	}
}

static inline void zeroProcessors (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	put->cprocessors	= NULL;
	put->nprocessors	= 0;
}

static void DoneCUNILOG_TARGETprocessors (CUNILOG_TARGET *put);

/*
	In the target initialisers zeroProcessors () must be called before prepareProcessors ().
*/
static bool prepareProcessors (CUNILOG_TARGET *put, CUNILOG_PROCESSOR **cp, unsigned int np)
{
	ubf_assert_non_NULL		(put);

	// The caller does not wish default processors. The function
	//	ConfigCUNILOG_TARGETprocessorList () must be called before the target is usable.
	if (put->uiOpts & CUNILOGTARGET_NO_DEFAULT_PROCESSORS)
	{
		ubf_assert_NULL		(put->cprocessors);
		ubf_assert_0		(put->nprocessors);
		return true;
	}

	/*
	if (put->cprocessors && put->nprocessors)
	{
		DoneCUNILOG_TARGETprocessors (put);
	}
	*/

	if (NULL == cp || 0 == np)
	{
		ubf_assert (NULL == cp);
		ubf_assert (0 == np);

		put->cprocessors = CreateNewDefaultProcessors (&put->nprocessors);
		if (put->cprocessors)
			cunilogTargetSetProcessorsAllocatedFlag (put);
		else
		{
			SetCunilogSystemError (put, CUNILOG_ERROR_HEAP_ALLOCATION);
			return false;
		}
	} else
	{
		put->cprocessors = cp;
		put->nprocessors = np;
	}
	defaultProcessorParameters (put);
	return true;
}

static inline void createLogfilesSearchMaskDotNumberPostfix (CUNILOG_TARGET *put)
{
	// We go for "<appname>.log*". This needs to be tested on POSIX.
	memcpy (put->mbLogFileMask.buf.pch, put->mbAppName.buf.pcc, put->lnAppName);
	memcpy	(
		put->mbLogFileMask.buf.pch + put->lnAppName,
		szCunilogLogFileNameExtension,
		lenCunilogLogFileNameExtension
			);
	char *szAster =		put->mbLogFileMask.buf.pch
					+	put->lnAppName
					+	lenCunilogLogFileNameExtension;
	memcpy (szAster , "*", 2);
	put->lnLogFileMask = put->lnAppName + lenCunilogLogFileNameExtension + 1;
	ubf_assert (strlen (put->mbLogFileMask.buf.pcc) == put->lnLogFileMask);
}

static inline void createLogfilesSearchMask (CUNILOG_TARGET *put)
{
	// Create the wildcard/search mask.
	memcpy (put->mbLogFileMask.buf.pch, put->mbAppName.buf.pcc, put->lnAppName);
	put->mbLogFileMask.buf.pch [put->lnAppName] = '_';
	memcpy	(
		put->mbLogFileMask.buf.pch + put->lnAppName + 1,
		postfixMaskFromLogPostfix (put->culogPostfix),
		lenDateTimeStampFromPostfix (put->culogPostfix)
			);
	memcpy	(
			put->mbLogFileMask.buf.pch
		+	put->lnAppName + 1
		+	lenDateTimeStampFromPostfix (put->culogPostfix),
		szCunilogLogFileNameExtension,
		sizCunilogLogFileNameExtension
			);
	put->lnLogFileMask	=	put->lnAppName + 1
						+	lenDateTimeStampFromPostfix (put->culogPostfix)
						+	lenCunilogLogFileNameExtension;
}

static bool prepareCUNILOG_TARGETforLogging (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	size_t	lnRoomForStamp	= lenDateTimeStampFromPostfix (put->culogPostfix);
	size_t	lnUnderscore;
	bool	bHasLogOrNumberPostfix	= hasLogPostfix (put) || hasDotNumberPostfix (put);
	
	// The underscore that separates the appname from the timestamp.
	if (bHasLogOrNumberPostfix)
		lnUnderscore = 0;
	else
		lnUnderscore = lnRoomForStamp ? 1 : 0;

	// The longest one is cunilogPostfixDotNumberDescending.
	ubf_assert (lnRoomForStamp <= UBF_UINT64_LEN + 1);

	size_t lnTotal;
	lnTotal = put->lnLogPath + put->lnAppName + lnUnderscore;

	// The pointer to the ASCII timestamp within the filename allows us to later update only
	//	the timestamp within the log file's name. We add this index/length to the filename
	//	further down to get its position.
	size_t idxStamp = lnTotal;
	lnTotal += lnRoomForStamp
				+ lenCunilogLogFileNameExtension
				+ 1		// We need an underscore in case of cunilogPostfixLog... types.
				+ 1;	// A terminating NUL character so that we can use the log file's
						//	name directly in OS APIs.

	initSMEMBUFtoSize (&put->mbLogfileName, lnTotal);
	if (isUsableSMEMBUF (&put->mbLogfileName))
		cunilogTargetSetLogFileAllocatedFlag (put);
	initSMEMBUFtoSize (&put->mbFilToRotate, lnTotal);
	if (isUsableSMEMBUF (&put->mbFilToRotate))
		cunilogTargetSetFileToRotateAllocatedFlag (put);

	size_t stRequiredForLogFileMask =	put->lnAppName
									+	lnUnderscore
									+	lnRoomForStamp
									+	sizCunilogLogFileNameExtension;
	// We also need an underscore in the search mask for LogPostfixes.
	if (hasLogPostfix (put))
		++ stRequiredForLogFileMask;

	initSMEMBUFtoSize (&put->mbLogFileMask, stRequiredForLogFileMask);
	if (isUsableSMEMBUF (&put->mbLogFileMask))
		cunilogTargetSetLogFileMaskAllocatedFlag (put);

	bool bUsablembs =		isUsableSMEMBUF (&put->mbLogfileName)
						&&	isUsableSMEMBUF (&put->mbLogFileMask)
						&&	isUsableSMEMBUF (&put->mbLogFileMask);
	if (bUsablembs)
	{
		// Remember the position of the timestamp for quick and easy update.
		put->szDateTimeStamp = put->mbLogfileName.buf.pch + idxStamp;

		char *pLogWrite = put->mbLogfileName.buf.pch;
		memcpy (pLogWrite, put->mbLogPath.buf.pch, put->lnLogPath);
		pLogWrite += put->lnLogPath;
		memcpy (pLogWrite, put->mbAppName.buf.pch, put->lnAppName);
		pLogWrite += put->lnAppName;

		if (lnUnderscore)
		{
			pLogWrite [0] = '_';
			++ pLogWrite;
		}

		if (hasDotNumberPostfix (put))
			createLogfilesSearchMaskDotNumberPostfix (put);
		else
		if (hasLogPostfix (put))
			createLogfilesSearchMask (put);
		else
		{
			createLogfilesSearchMask (put);
			pLogWrite += lenDateTimeStampFromPostfix (put->culogPostfix);
		}
		memcpy (pLogWrite, szCunilogLogFileNameExtension, sizCunilogLogFileNameExtension);

		// Create name of the found file.
		copySMEMBUF (&put->mbFilToRotate, &put->mbLogPath);

		ubf_assert (0 < CUNILOG_INITIAL_EVENTLINE_SIZE);
		initSMEMBUFtoSize (&put->mbLogEventLine, CUNILOG_INITIAL_EVENTLINE_SIZE);

		#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
			initSMEMBUFtoSize (&put->mbColEventLine, CUNILOG_INITIAL_COLEVENTLINE_SIZE);
		#endif

		cunilogSetTargetInitialised (put);
		return true;
	} else
	{
		SetCunilogSystemError (put, CUNILOG_ERROR_HEAP_ALLOCATION);
		put->szDateTimeStamp = NULL;
		ubf_assert (false);
		return false;
	}
}

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	bool InitCUNILOG_TARGETqueuesemaphore (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (HAS_CUNILOG_TARGET_A_QUEUE (put))
		{
			#ifdef OS_IS_WINDOWS
			
				// See
				//	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createsemaphorew .
				put->sm.hSemaphore = CreateSemaphoreW (NULL, 0, MAXLONG, NULL);
				if (NULL == put->sm.hSemaphore)
					SetCunilogSystemError (put, CUNILOG_ERROR_SEMAPHORE);
				return NULL != put->sm.hSemaphore;
				
			#else
			
				int i = sem_init (&put->sm.tSemaphore, 0, 0);
				ubf_assert (0 == i);
				if (0 != i)
					SetCunilogSystemError (put, CUNILOG_ERROR_SEMAPHORE);
				return 0 == i;
				
			#endif
		}
		// If no semaphore is required, not creating it is seen as success.
		return true;
	}
#else
	#define InitCUNILOG_TARGETqueuesemaphore(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	void DoneCUNILOG_TARGETqueuesemaphore (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (HAS_CUNILOG_TARGET_A_QUEUE (put))
		{
			#ifdef OS_IS_WINDOWS
				bool b = CloseHandle (put->sm.hSemaphore);
				ubf_assert_true (b);
				UNUSED (b);
				put->sm.hSemaphore = NULL;
			#else
				int i = sem_destroy (&put->sm.tSemaphore);
				ubf_assert (0 == i);
				UNUSED (i);
			#endif
		}
	}
#else
	#define DoneCUNILOG_TARGETqueuesemaphore(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static bool InitCUNILOG_TARGETqueue (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (HAS_CUNILOG_TARGET_A_QUEUE (put))
		{
			put->qu.first	= NULL;
			put->qu.last	= NULL;
			put->qu.num		= 0;
		}
		// If no semaphore is required, not creating it is seen as success.
		return true;
	}
#else
	#define InitCUNILOG_TARGETqueue(x)
#endif

static bool StartSeparateLoggingThread_ifNeeded (CUNILOG_TARGET *put)
;

#ifdef PLATFORM_IS_WINDOWS
	static culogconcp ourCunilogConsoleOutputCodePage = cunilogConsoleIsUninitialised;

	void CunilogSetConsoleTo (culogconcp cp)
	{
		ubf_assert	(
							cunilogConsoleIsUTF8	== cp
						||	cunilogConsoleIsUTF16	== cp
						||	cunilogConsoleIsNeither	== cp
					);

		switch (cp)
		{
			case cunilogConsoleIsUTF8:
				SetConsoleCodePageToUTF8 ();	break;
			case cunilogConsoleIsUTF16:
				WinSetStdinToUTF16 ();
				WinSetStdoutToUTF16 ();			break;
			case cunilogConsoleIsNeither:		break;
			default:
				SetConsoleCodePageToUTF8 ();	break;
		}
		ourCunilogConsoleOutputCodePage = cp;
	}
#endif

#ifdef PLATFORM_IS_WINDOWS
	enum enbANSIescapeSequencesEnabled
	{
		ansiEscapeSequsUninitialised,
		ansiEscapeSequsEnabled,
		ansiEscapeSequsDisabled
	};
	static bool bANSIescapeSequencesEnabled;

	bool CunilogEnableANSI (void)
	{
		HANDLE	hConsole	= GetStdHandle (STD_OUTPUT_HANDLE);
		DWORD	dwMode		= 0;

		if (INVALID_HANDLE_VALUE != hConsole)
		{
			if (GetConsoleMode (hConsole, &dwMode))
			{
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				if (SetConsoleMode (hConsole, dwMode))
				{
					bANSIescapeSequencesEnabled = ansiEscapeSequsEnabled;
					return true;
				}
			}
		}
		return false;
	}

	bool CunilogDisableANSI (void)
	{
		HANDLE	hConsole	= GetStdHandle (STD_OUTPUT_HANDLE);
		DWORD	dwMode		= 0;

		if (INVALID_HANDLE_VALUE != hConsole)
		{
			if (GetConsoleMode (hConsole, &dwMode))
			{
				dwMode &= ~ ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				if (SetConsoleMode (hConsole, dwMode))
				{
					bANSIescapeSequencesEnabled = ansiEscapeSequsDisabled;
					return true;
				}
			}
		}
		return false;
	}
	
	bool CunilogIsANSIenabled (void)
	{
		return ansiEscapeSequsEnabled == bANSIescapeSequencesEnabled;
	}

	static inline void CunilogEnableANSIifNotInitialised (void)
	{
		if (ansiEscapeSequsUninitialised == bANSIescapeSequencesEnabled)
			CunilogEnableANSI ();
	}
#endif

char *CunilogGetEnv (const char *szName)
{
	ubf_assert_non_NULL (szName);

	#if defined (PLATFORM_IS_WINDOWS)
		return getenv (szName);
	#elif defined (PLATFORM_IS_POSIX)
		return secure_getenv (szName);
	#elif
		#error Not supported
	#endif
}

bool Cunilog_Have_NO_COLOR (void)
{
	char *szNoCol = CunilogGetEnv ("NO_COLOR");
	return szNoCol && szNoCol [0];
}

#ifdef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	// This is the only type possible in a single-threaded environment.
	#define unilogTypeFromArgument(type)					\
				(cunilogSingleThreaded)
#else
	#define unilogTypeFromArgument(type)					\
				(type)
#endif

/*
	This function is not used anymore. The configurable hex dump is not supported
	anymore.
*/
static inline void InitCUNILOG_TARGETdumpstructs (CUNILOG_TARGET *put)
{
	UNUSED (put);

	ubf_assert_non_NULL (put);

	//put->psdump = NULL;
}

static inline void initCUNILOG_TARGEToptionFlags (CUNILOG_TARGET *put, runProcessorsOnStartup rp)
{
	ubf_assert_non_NULL (put);
	ubf_assert	(
						cunilogRunProcessorsOnStartup		== rp
					||	cunilogDontRunProcessorsOnStartup	== rp
				);

	put->uiOpts = CUNILOGTARGET_NO_FLAGS;
	if (cunilogRunProcessorsOnStartup == rp)
		cunilogTargetSetRunProcessorsOnStartup (put);

	#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
		cunilogTargetSetUseColourForEcho (put);
	#endif
}

static inline void initFilesListInCUNILOG_TARGET (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	vec_init (&put->fls);
	InitSBULKMEM (&put->sbm, CUNILOG_STD_BULK_ALLOC_SIZE);

	// Note that we do not allocate any bulk memory here yet. If for instance the caller
	//	hasn't provided a rotation processor we may not even need one.
}

static inline void initPrevTimestamp (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	memset (put->cPrevDateTimeStamp, 0, LEN_ISO8601DATEHOURANDMINUTE);
}

static inline void savePrevTimestamp (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	
	CUNILOG_TARGET	*put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);

	ubf_assert (cunilogPostfixYear >= put->culogPostfix);

	size_t lenPostfixStamp;
	lenPostfixStamp = lenDateTimeStampFromPostfix (put->culogPostfix);
	ubf_assert (0 < lenPostfixStamp);
	ubf_assert (LEN_ISO8601DATEHOURANDMINUTE >= lenPostfixStamp);

	memcpy (put->cPrevDateTimeStamp, put->szDateTimeStamp, lenPostfixStamp);
	obtainDateAndTimeStamp (put->szDateTimeStamp, pev->stamp, put->culogPostfix);
	put->szDateTimeStamp [lenPostfixStamp] = '.';
}

static inline bool requiresNewLogFile (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);


	int r = memcmp	(
				put->szDateTimeStamp, put->cPrevDateTimeStamp,
				lenDateTimeStampFromPostfix (put->culogPostfix)
					);
	return r > 0;
}

#ifdef PLATFORM_IS_POSIX
	// The plain logging folder name is not used on Windows. It is only required
	//	for opendir () on POSIX. This might change in the future as I can imagine
	//	that some custom processor might also benefit from it.
	static void InitCUNILOG_TARGETmbLogFold (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (isUsableSMEMBUF (&put->mbLogPath));
		ubf_assert (0 < put->lnLogPath);
		size_t lenLogp = put->lnLogPath - 1;
		ubf_assert (0 < lenLogp);

		ubf_assert	(
							'/'		== put->mbLogPath.buf.pch [lenLogp]
						||	'\\'	== put->mbLogPath.buf.pch [lenLogp]
					);

		initSMEMBUF (&put->mbLogFold);
		growToSizeSMEMBUF (&put->mbLogFold, put->lnLogPath);
		if (isUsableSMEMBUF (&put->mbLogFold))
		{
			memcpy (put->mbLogFold.buf.pch, put->mbLogPath.buf.pch, lenLogp);
			put->mbLogFold.buf.pch [lenLogp] = '\0';
			put->lnLogFold = lenLogp;
			return;
		}
		// We should never get here.
		ubf_assert (false);
		put->lnLogFold = 0;
	}
#else
	#define InitCUNILOG_TARGETmbLogFold(x)
#endif

static inline void cunilogInitCUNILOG_LOGFILE (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	#ifdef OS_IS_WINDOWS
		put->logfile.hLogFile = NULL;
	#else
		put->logfile.fLogFile = NULL;
	#endif
}

static inline bool cunilogOpenLogFile (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert			(isInitialisedSMEMBUF (&put->mbLogfileName));

	#ifdef PLATFORM_IS_WINDOWS
		put->logfile.hLogFile = CreateFileU8	(
						put->mbLogfileName.buf.pcc,
						CUNILOG_DEFAULT_OPEN_MODE,
						FILE_SHARE_DELETE | FILE_SHARE_READ,
						NULL, OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
						NULL
									);
		return NULL != put->logfile.hLogFile && INVALID_HANDLE_VALUE != put->logfile.hLogFile;
	#else
		// We always (and automatically) append.
		put->logfile.fLogFile = fopen (put->mbLogfileName.buf.pcc, CUNILOG_DEFAULT_OPEN_MODE);
		return NULL != put->logfile.fLogFile;
	#endif
}

static inline void cunilogCloseCUNILOG_LOGFILEifOpen (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	#ifdef OS_IS_WINDOWS
		if (put->logfile.hLogFile)
		{
			CloseHandle (put->logfile.hLogFile);
			put->logfile.hLogFile = NULL;
		}
	#else
		if (put->logfile.fLogFile)
		{
			fclose (put->logfile.fLogFile);
			put->logfile.fLogFile = NULL;
		}
	#endif
}

static inline bool initCommonMembersAndPrepareCUNILOG_TARGET (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	put->error								= CUNILOG_NO_ERROR;
	str_remove_path_navigators (put->mbLogPath.buf.pch, &put->lnLogPath);
	#ifndef CUNILOG_BUILD_WITHOUT_ERROR_CALLBACK
		put->errorCB						= NULL;
	#endif
	InitSCUNILOGNPI							(&put->scuNPI);
	DBG_INIT_CNTTRACKER						(put->evtLineTracker);
	#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
		put->nPendingNoRotEvts				= 0;
		put->nPausedEvents					= 0;
	#endif
	put->dumpWidth							= enDataDumpWidth16;
	put->evSeverityType						= cunilogEvtSeverityTypeDefault;
	initPrevTimestamp						(put);
	InitCUNILOG_TARGETmbLogFold				(put);
	InitCUNILOG_TARGETdumpstructs			(put);
	InitCUNILOG_LOCKER						(put);
	InitCUNILOG_TARGETqueuesemaphore		(put);
	InitCUNILOG_TARGETqueue					(put);
	initFilesListInCUNILOG_TARGET			(put);
	cunilogInitCUNILOG_LOGFILE				(put);
	bool b;
	b = StartSeparateLoggingThread_ifNeeded	(put);
	if (b)
	{
		ResetCunilogError (put);
		return prepareCUNILOG_TARGETforLogging	(put);
	}
	return false;
}

#ifdef DEBUG
	void assertSaneParametersCUNILOG_TARGET	(
	  enCunilogRelPath		relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
											)
	{
		ubf_assert (0 <= relLogPath);
		ubf_assert (cunilogPath_XAmountEnumValues > relLogPath);
		ubf_assert (0 <= type);
		ubf_assert (cunilogTypeAmountEnumValues > type);
		ubf_assert (0 <= postfix);
		ubf_assert (cunilogPostfixAmountEnumValues >= postfix);
		ubf_assert (0 <= unilogTSformat);
		ubf_assert (cunilogEvtTS_AmountEnumValues > unilogTSformat);
		ubf_assert (0 <= unilogNewLine);
		ubf_assert (cunilogNewLineAmountEnumValues > unilogNewLine);
		ubf_assert (0 <= rp);
		ubf_assert (cunilogRunProcessors_AmountEnumValues > rp);
	}
#else
	#define assertSaneParametersCUNILOG_TARGET(rP, tpy, psf, ts, nl, rp)
#endif

CUNILOG_TARGET *InitCUNILOG_TARGETex
(
	  CUNILOG_TARGET			*put				// Must not be NULL.
	, const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szAppName			// Application name.
	, size_t					lenAppName			// Length of szApplication.
	, enCunilogRelPath			relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, CUNILOG_PROCESSOR			**cuProcessorList	// One or more post-processors.
	, unsigned int				nProcessors			// Number of processors.
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
)
{
	ubf_assert_non_NULL (put);
	assertSaneParametersCUNILOG_TARGET	(
		relLogPath, type, postfix, unilogTSformat, unilogNewLine, rp
										);

	size_t			lnLogPath		= (size_t) -1 != lenLogPath	? lenLogPath : strlen (szLogPath);
	size_t			lnAppName		= (size_t) -1 != lenAppName	? lenAppName : strlen (szAppName);

	initCUNILOG_TARGEToptionFlags (put, rp);
	put->culogPostfix		= postfix;
	put->culogType			= unilogTypeFromArgument (type);
	put->unilogEvtTSformat	= unilogTSformat;
	put->unilogNewLine		= unilogNewLine;

	// The function sets put->error for us.
	char *szLP = createLogPathInCUNILOG_TARGET (put, szLogPath, lnLogPath, relLogPath);
	if (NULL == szLP && cunilogPath_isAbsolute == relLogPath)
	{
		SetCunilogError (put, CUNILOG_ERROR_ABS_OR_REL_PATH, CUNILOG_SYSTEM_ERROR_NOT_SUPPORTED);
		return NULL;
	}
	char *sz = createAppNameInCUNILOG_TARGET (put, szAppName, lnAppName);
	if (NULL == sz)
	{
		SetCunilogSystemError (put, CUNILOG_ERROR_APPNAME);
		return NULL;
	}
	zeroProcessors (put);
	bool b = prepareProcessors (put, cuProcessorList, nProcessors);
	if (b)
	{
		return initCommonMembersAndPrepareCUNILOG_TARGET (put) ? put : NULL;
	} else
		return false;
}

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	CUNILOG_TARGET *InitCUNILOG_TARGET
	(
		  CUNILOG_TARGET			*put				// Must not be NULL.
		, const char				*szLogPath			// Path to the logging information.
		, size_t					lenLogPath			// Length of szLogPath
		, const char				*szAppName			// Application name.
		, size_t					lenAppName			// Length of szApplication.
		, enCunilogRelPath			relLogPath			// Rel. to home, exe, or current dir.
		, enum cunilogtype			type
	)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (0 <= relLogPath);
		ubf_assert (cunilogPath_XAmountEnumValues > relLogPath);
		ubf_assert (0 <= type);
		ubf_assert (cunilogTypeAmountEnumValues > type);

		CUNILOG_TARGET	*prt;

		prt = InitCUNILOG_TARGETex	(
				put,
				szLogPath,	lenLogPath,
				szAppName,	lenAppName,
				relLogPath,
				type,
				cunilogPostfixDefault,
				NULL, 0,
				cunilogEvtTS_Default,
				cunilogNewLineDefault,
				cunilogRunProcessorsOnStartup
									);
		return prt;
	}
#endif

CUNILOG_TARGET *CreateNewCUNILOG_TARGET
(
	  const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szAppName			// Application name.
	, size_t					lenAppName			// Length of szApplication.
	, enCunilogRelPath			relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, CUNILOG_PROCESSOR			**cuProcessorList	// One or more post-processors.
	, unsigned int				nProcessors			// Number of processors.
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
)
{
	assertSaneParametersCUNILOG_TARGET	(
		relLogPath, type, postfix, unilogTSformat, unilogNewLine, rp
										);

	//_ASSERT (false);

	CUNILOG_TARGET	*pu;							// Return value.
	size_t			lnUNILOGTARGET	= ALIGNED_SIZE (sizeof (CUNILOG_TARGET), CUNILOG_DEFAULT_ALIGNMENT);
	size_t			lnTotal			= lnUNILOGTARGET;
	size_t			lnLP;
	size_t			lnAP;
	SMEMBUF			logpath			= SMEMBUF_INITIALISER;
	SMEMBUF			appname			= SMEMBUF_INITIALISER;

	size_t			lnLogPath		= (size_t) -1 != lenLogPath	? lenLogPath : strlen (szLogPath);
	size_t			lnAppName		= (size_t) -1 != lenAppName	? lenAppName : strlen (szAppName);

	if (szLogPath && lnLogPath)
	{
		size_t ln;
		char *szLP = CreateLogPath_smb (&logpath, &ln, szLogPath, lnLogPath, relLogPath);

		#ifndef CUNILOG_BUILD_TEST_FNCTS
			ubf_assert_msg	(
				szLP,
				"szLogPath cannot be relative or NULL if cunilogLogPath_isAbsolute is given"
							);
		#endif

		if (NULL == szLP)
			return NULL;

		lnLogPath = ln;
		if (!isDirSep (logpath.buf.pch [ln -1]))
			lnLP = ALIGNED_SIZE (ln + 2, CUNILOG_DEFAULT_ALIGNMENT);
		else 
			lnLP = ALIGNED_SIZE (ln + 1, CUNILOG_DEFAULT_ALIGNMENT);
		lnTotal += lnLP;
	} else
	{	// No log path given.
		if (cunilogPath_isAbsolute == relLogPath)
		{
			#ifndef CUNILOG_BUILD_TEST_FNCTS
				ubf_assert_msg	(
					false,
					"szLogPath cannot be relative or NULL if cunilogLogPath_isAbsolute is given"
								);
			#endif
			return NULL;
		}
		lnLogPath = ObtainRelativeLogPathBase (&logpath, relLogPath);
		ubf_assert_non_0 (lnLogPath);
		lnLP = ALIGNED_SIZE (lnLogPath + 1, CUNILOG_DEFAULT_ALIGNMENT);
		lnTotal += lnLP;
	}
	if (szAppName && lnAppName)
	{
		lnAP = ALIGNED_SIZE (lnAppName + 1, CUNILOG_DEFAULT_ALIGNMENT);
		lnTotal += lnAP;
	} else
	{	// No application name given.
		lnAppName = ObtainAppNameFromExecutableModule (&appname);
		ubf_assert_non_0 (lnAppName);
		szAppName = appname.buf.pch;
		lnAP = ALIGNED_SIZE (lnAppName + 1, CUNILOG_DEFAULT_ALIGNMENT);
		lnTotal += lnAP;
	}
	pu = ubf_malloc (lnTotal);
	if (pu)
	{
		initCUNILOG_TARGEToptionFlags (pu, rp);
		cunilogTargetSetTargetAllocatedFlag (pu);
		initSMEMBUF (&pu->mbLogPath);
		pu->mbLogPath.buf.pcc = (char *) pu + ALIGNED_SIZE (lnUNILOGTARGET, CUNILOG_DEFAULT_ALIGNMENT);
		memcpy (pu->mbLogPath.buf.pch, logpath.buf.pch, lnLogPath + 1);
		if (!isDirSep (logpath.buf.pch [lnLogPath -1]))
		{
			pu->mbLogPath.buf.pch [lnLogPath]		= UBF_DIR_SEP;
			pu->mbLogPath.buf.pch [lnLogPath + 1]	= ASCII_NUL;
			pu->mbLogPath.size						= lnLogPath + 2;
			pu->lnLogPath							= lnLogPath + 1;
		} else
		{
			pu->mbLogPath.size						= lnLogPath + 1;
			pu->lnLogPath							= lnLogPath;
		}
		str_correct_dir_separators (pu->mbLogPath.buf.pch, lnLogPath);
		pu->mbAppName.buf.pch = pu->mbLogPath.buf.pch + lnLP;
		memcpy (pu->mbAppName.buf.pch, szAppName, lnAppName);
		str_correct_dir_separators (pu->mbAppName.buf.pch, lnAppName);
		pu->mbAppName.size		= lnAppName;
		pu->lnAppName			= lnAppName;
		pu->culogPostfix		= postfix;
		pu->culogType			= unilogTypeFromArgument (type);
		pu->unilogEvtTSformat	= unilogTSformat;
		pu->unilogNewLine		= unilogNewLine;
		zeroProcessors (pu);
		prepareProcessors (pu, cuProcessorList, nProcessors);
		initCommonMembersAndPrepareCUNILOG_TARGET (pu);
	}
	if (isUsableSMEMBUF (&logpath))
		doneSMEMBUF (&logpath);
	if (isUsableSMEMBUF (&appname))
		doneSMEMBUF (&appname);
	return pu;
}

CUNILOG_TARGET *InitOrCreateCUNILOG_TARGET
(
	  CUNILOG_TARGET			*put				// If NULL, a new structure is allocated.
	, const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szAppName			// Application name.
	, size_t					lenAppName			// Length of szApplication.
	, enCunilogRelPath			relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, CUNILOG_PROCESSOR			**cuProcessorList	// One or more post-processors.
	, unsigned int				nProcessors			// Number of processors.
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
)
{
	assertSaneParametersCUNILOG_TARGET	(
		relLogPath, type, postfix, unilogTSformat, unilogNewLine, rp
										);

	CUNILOG_TARGET	*pu;

	if (put)
	{
		pu = put;
		InitCUNILOG_TARGETex	(
			pu,
			szLogPath, lenLogPath, szAppName, lenAppName, relLogPath,
			type, postfix, cuProcessorList, nProcessors,
			unilogTSformat, unilogNewLine, rp
							);
	} else
	{
		pu = CreateNewCUNILOG_TARGET	(
				szLogPath, lenLogPath, szAppName, lenAppName, relLogPath,
				type, postfix, cuProcessorList, nProcessors, 
				unilogTSformat, unilogNewLine, rp
									);
	}
	if (pu && pu->mbAppName.buf.pch)
	{
	} else
	{
		pu = DoneCUNILOG_TARGET (pu);
	}
	return pu;
}

CUNILOG_TARGET *InitCUNILOG_TARGETstaticEx
(
	  const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szApplication		// Application name.
	, size_t					lenApplication		// Length of szApplication.
	, enCunilogRelPath			relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, CUNILOG_PROCESSOR			**cuProcessorList	// One or more post-processors.
	, unsigned int				nProcessors			// Number of processors.
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
)
{
	assertSaneParametersCUNILOG_TARGET	(
		relLogPath, type, postfix, unilogTSformat, unilogNewLine, rp
										);

	return InitCUNILOG_TARGETex	(
		pCUNILOG_TARGETstatic, szLogPath, lenLogPath, szApplication, lenApplication, relLogPath,
		type, postfix, cuProcessorList, nProcessors, unilogTSformat, unilogNewLine, rp
								);
}

CUNILOG_TARGET *InitCUNILOG_TARGETstatic
(
	  const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szApplication		// Application name.
	, size_t					lenApplication		// Length of szApplication.
	, enCunilogRelPath			relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
)
{
	ubf_assert (0 <= relLogPath);
	ubf_assert (cunilogPath_XAmountEnumValues > relLogPath);
	ubf_assert (0 <= type);
	ubf_assert (cunilogTypeAmountEnumValues > type);

	return InitCUNILOG_TARGETex	(
				pCUNILOG_TARGETstatic,
				szLogPath,		lenLogPath,
				szApplication,	lenApplication,
				relLogPath,
				type,
				cunilogPostfixDefault,
				NULL, 0,
				cunilogEvtTS_Default,
				cunilogNewLineDefault,
				cunilogRunProcessorsOnStartup
								);
}

const char *GetAbsoluteLogPathCUNILOG_TARGET (CUNILOG_TARGET *put, size_t *plen)
{
	ubf_assert_non_NULL	(put);
	ubf_assert (cunilogIsTargetInitialised (put));

	if (cunilogIsTargetInitialised (put) && isUsableSMEMBUF (&put->mbLogPath))
	{
		if (plen)
			*plen = put->lnLogPath;
		return put->mbLogPath.buf.pcc;
	}
	return NULL;
}

const char *GetAbsoluteLogPathCUNILOG_TARGET_static (size_t *plen)
{
	ubf_assert_non_NULL	(pCUNILOG_TARGETstatic);
	ubf_assert (cunilogIsTargetInitialised (pCUNILOG_TARGETstatic));

	return GetAbsoluteLogPathCUNILOG_TARGET (pCUNILOG_TARGETstatic, plen);
}

CUNILOG_PROCESSOR *GetCUNILOG_PROCESSOR	(
						CUNILOG_TARGET				*put,
						enum cunilogprocesstask		task,
						unsigned int				n
										)
{
	ubf_assert_non_NULL	(put);
	ubf_assert			(0 <= task);
	ubf_assert			(cunilogProcessXAmountEnumValues > task);

	if (n < put->nprocessors)
	{
		unsigned int fnd = 0;
		unsigned int npr = 0;

		while (npr < put->nprocessors)
		{
			if (task == put->cprocessors [npr]->task)
			{
				if (fnd == n)
					return put->cprocessors [npr];
				++ fnd;
			}
			++ npr;
		}
	}
	return NULL;
}

CUNILOG_PROCESSOR *GetCUNILOG_PROCESSORrotationTask	(
						CUNILOG_TARGET				*put,
						enum cunilogrotationtask	rot,
						unsigned int				n
													)
{
	ubf_assert_non_NULL	(put);
	ubf_assert			(0 <= rot);
	ubf_assert			(cunilogrotationtask_XAmountEnumValues > rot);

	unsigned int fnd	= 0;
	unsigned int np		= 0;

	CUNILOG_ROTATION_DATA	*prd;

	while (np < put->nprocessors)
	{
		if (cunilogProcessRotateLogfiles == put->cprocessors [np]->task)
		{
			prd = put->cprocessors [np]->pData;
			ubf_assert_non_NULL (prd);

			if (rot == prd->tsk)
			{
				if (fnd == n)
					return put->cprocessors [np];
				++ fnd;
			}
		}
		++ np;
	}
	return NULL;
}

#ifndef CUNILOG_BUILD_WITHOUT_ERROR_CALLBACK
	void ConfigCUNILOG_TARGETerrorCallbackFunction (CUNILOG_TARGET *put, cunilogErrCallback errorCB)
	{
		ubf_assert_non_NULL (put);

		put->errorCB = errorCB;
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void ConfigCUNILOG_TARGETeventStampFormat (CUNILOG_TARGET *put, enum cunilogeventTSformat tsf)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= tsf);
		ubf_assert			(cunilogEvtTS_AmountEnumValues > tsf);

		put->unilogEvtTSformat = tsf;		
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void ConfigCUNILOG_TARGETcunilognewline (CUNILOG_TARGET *put, newline_t nl)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= nl);
		ubf_assert			(cunilogNewLineAmountEnumValues > nl);

		put->unilogNewLine = nl;
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void ConfigCUNILOG_TARGETeventSeverityFormatType	(
			CUNILOG_TARGET				*put,
			cueventsevfmtpy				eventSeverityFormatType
														)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (eventSeverityFormatType > 0);
		ubf_assert (eventSeverityFormatType <= cunilogEvtSeverityTypeChars9InBrackets);

		put->evSeverityType = eventSeverityFormatType;
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
	#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
		void ConfigCUNILOG_TARGETuseColourForEcho (CUNILOG_TARGET *put, bool bUseColour)
		{
			ubf_assert_non_NULL (put);

			if (bUseColour)
				cunilogTargetSetUseColourForEcho (put);
			else
				cunilogTargetClrUseColourForEcho (put);
		}
	#endif
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void ConfigCUNILOG_TARGETrunProcessorsOnStartup (CUNILOG_TARGET *put, runProcessorsOnStartup rp)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(
									cunilogRunProcessorsOnStartup		== rp
								||	cunilogDontRunProcessorsOnStartup	== rp
							);

		switch (rp)
		{
			case cunilogRunProcessorsOnStartup:
				cunilogTargetSetRunProcessorsOnStartup (put);
				break;
			case cunilogDontRunProcessorsOnStartup:
				cunilogTargetClrRunProcessorsOnStartup (put);
				break;
			default:
				cunilogTargetSetRunProcessorsOnStartup (put);
				break;
		}
	}
#endif

#if defined (DEBUG) || defined (CUNILOG_BUILD_SHARED_LIBRARY)
	void ConfigCUNILOG_TARGETprocessorList	(
					CUNILOG_TARGET			*put
				,	CUNILOG_PROCESSOR		**cuProcessorList	// One or more post-processors.
				,	unsigned int			nProcessors			// Number of processors.
											)
	{
		ubf_assert_non_NULL	(put);

		prepareProcessors (put, cuProcessorList, nProcessors);
	}
#endif

/*
	This function has a declaration in cunilogevtcmds.c too. If its signature changes,
	please don't forget to change it there too.
*/
void ConfigCUNILOG_TARGETdisableTaskProcessors (CUNILOG_TARGET *put, enum cunilogprocesstask task)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL (put->cprocessors);

	unsigned int n = 0;
	while (n < put->nprocessors)
	{
		if (task == put->cprocessors [n]->task)
			optCunProcSetOPT_CUNPROC_DISABLED (put->cprocessors [n]->uiOpts);
		-- n;
	}
}

/*
	This function has a declaration in cunilogevtcmds.c too. If its signature changes,
	please don't forget to change it there too.
*/
void ConfigCUNILOG_TARGETenableTaskProcessors (CUNILOG_TARGET *put, enum cunilogprocesstask task)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL (put->cprocessors);

	unsigned int n = 0;
	while (n < put->nprocessors)
	{
		if (task == put->cprocessors [n]->task)
			optCunProcClrOPT_CUNPROC_DISABLED (put->cprocessors [n]->uiOpts);
		-- n;
	}
}

/*
	This function has a declaration in cunilogevtcmds.c too. If its signature changes,
	please don't forget to change it there too.
*/
void ConfigCUNILOG_TARGETdisableEchoProcessor (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL (put->cprocessors);

	ConfigCUNILOG_TARGETdisableTaskProcessors (put, cunilogProcessEchoToConsole);
}

/*
	This function has a declaration in cunilogevtcmds.c too. If its signature changes,
	please don't forget to change it there too.
*/
void ConfigCUNILOG_TARGETenableEchoProcessor (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL (put->cprocessors);

	ConfigCUNILOG_TARGETenableTaskProcessors (put, cunilogProcessEchoToConsole);
}

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	void EnterCUNILOG_TARGET (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		EnterCUNILOG_LOCKER (put);
	}

	void LeaveCUNILOG_TARGET (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		LeaveCUNILOG_LOCKER (put);
	}
#endif

static void DoneCUNILOG_TARGETprocessors (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	unsigned int				np = put->nprocessors;
	unsigned int				ui;
	CUNILOG_PROCESSOR			*cp;
	union
	{
		CUNILOG_ROTATION_DATA	*prd;
		CUNILOG_CUSTPROCESS		*up;
	} upCust;

	for (ui = 0; ui < np; ++ ui)
	{
		cp = put->cprocessors [ui];
		switch (cp->task)
		{
			case cunilogProcessNoOperation:
			case cunilogProcessEchoToConsole:
			case cunilogProcessUpdateLogFileName:
			case cunilogProcessFlushLogFile:
				break;
			case cunilogProcessCustomProcessor:
				upCust.up = cp->pData;
				if (upCust.up->procDone)
					upCust.up->procDone (cp);
				break;
			case cunilogProcessTargetRedirector:
			case cunilogProcessXAmountEnumValues:
				break;
			case cunilogProcessWriteToLogFile:
				cunilogCloseCUNILOG_LOGFILEifOpen (put);
				break;
			case cunilogProcessRotateLogfiles:
				upCust.prd = cp->pData;
				if (cunilogHasRotatorFlag_USE_MBSRCMASK (upCust.prd))
				{
					doneSMEMBUF (&upCust.prd->mbSrcMask);
					cunilogClrRotatorFlag_USE_MBSRCMASK (upCust.prd);
				}
				if (cunilogHasRotatorFlag_USE_MBDSTFILE (upCust.prd))
				{
					doneSMEMBUF (&upCust.prd->mbDstFile);
					cunilogClrRotatorFlag_USE_MBDSTFILE (upCust.prd);
				}
				break;
		}
	}

	if (cunilogTargetHasProcessorsAllocatedFlag (put))
	{
		ubf_free (put->cprocessors);
		zeroProcessors (put);
		cunilogTargetClrProcessorsAllocatedFlag (put);
	}
}

/*
	This function is a no-op.
*/
static void DoneCUNILOG_TARGETpsdump (CUNILOG_TARGET *psu)
{
	UNUSED (psu);

	ubf_assert_non_NULL (psu);

	/*
	SCUNILOGDUMP *pd = psu->psdump;
	if (pd)
	{
		if (pd->dump_prs)
			ubf_free (pd->dump_prs);
		if (pd->dump_sns)
			ubf_free (pd->dump_sns);
	}
	*/
}

static void DoneCUNILOG_TARGETmembers (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasLogPathAllocatedFlag (put))
		freeSMEMBUF (&put->mbLogPath);
	if (cunilogTargetHasAppNameAllocatedFlag (put))
		freeSMEMBUF (&put->mbAppName);
	if (cunilogTargetHasLogFileAllocatedFlag (put))
		freeSMEMBUF (&put->mbLogfileName);
	if (cunilogTargetHasLogFileMaskAllocatedFlag (put))
		freeSMEMBUF (&put->mbLogFileMask);
	if (cunilogTargetHasFileToRotateAllocatedFlag (put))
		freeSMEMBUF (&put->mbFilToRotate);

	freeSMEMBUF (&put->mbLogEventLine);

	#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
		freeSMEMBUF (&put->mbColEventLine);
	#endif

	DoneSCUNILOGNPI (&put->scuNPI);
	DoneCUNILOG_LOCKER (put);
	DoneCUNILOG_TARGETqueuesemaphore (put);
	vec_deinit (&put->fls);
	DoneSBULKMEM (&put->sbm);
	DoneCUNILOG_TARGETpsdump (put);
}

CUNILOG_TARGET *DoneCUNILOG_TARGET (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);
	ubf_assert (cunilogIsTargetInitialised (put));

	DoneCUNILOG_TARGETprocessors (put);
	DoneCUNILOG_TARGETmembers (put);
	if (cunilogTargetHasTargetAllocatedFlag (put))
	{
		ubf_free (put);
	}
	return NULL;
}

/*
	Copy from cunilogstructs.h.

enum cunilogeventseverity
{
		cunilogEvtSeverityNone									//  0
	,	cunilogEvtSeverityNonePass								//  1
	,	cunilogEvtSeveriiyNoneFail								//  2
	,	cunilogEvtSeverityBlanks								//  3
	,	cunilogEvtSeverityEmergency								//	4
	,	cunilogEvtSeverityNotice								//	5
	,	cunilogEvtSeverityInfo									//  6
	,	cunilogEvtSeverityMessage								//  7
	,	cunilogEvtSeverityWarning								//  8
	,	cunilogEvtSeverityError									//  9
	,	cunilogEvtSeverityPass									// 10
	,	cunilogEvtSeverityFail									// 11
	,	cunilogEvtSeverityCritical								// 12
	,	cunilogEvtSeverityFatal									// 13
	,	cunilogEvtSeverityDebug									// 14
	,	cunilogEvtSeverityTrace									// 15
	,	cunilogEvtSeverityDetail								// 16
	,	cunilogEvtSeverityVerbose								// 17
	,	cunilogEvtSeverityIllegal								// 18
	// Do not add anything below this line.
	,	cunilogEvtSeverityXAmountEnumValues						// Used for sanity checks.
	// Do not add anything below cunilogEvtSeverityXAmountEnumValues.
};
*/
static const char *EventSeverityTexts3 [] =
{
		""				// cunilogEvtSeverityNone		 0
	,	""				// cunilogEvtSeverityNonePass	 1
	,	""				// cunilogEvtSevertiyNoneFail	 2
	,	""				// cunilogEvtSevertiyNoneWarn	 3
	,	"   "			// cunilogEvtSeverityBlanks		 4
	,	"EMG"			// cunilogEvtSeverityEmergency	 5
	,	"NOT"			// cunilogEvtSeverityNotice		 6
	,	"INF"
	,	"MSG"
	,	"WRN"
	,	"ERR"
	,	"PAS"			// cunilogEvtSeverityPass		11
	,	"FAI"
	,	"CRI"
	,	"FTL"
	,	"DBG"
	,	"TRC"
	,	"DET"
	,	"VBS"
	,	"ILG"			// cunilogEvtSeverityIllegal	18
};
static const char *EventSeverityTexts5 [] =
{
		""				// cunilogEvtSeverityNone		 0
	,	""				// cunilogEvtSeverityNonePass	 1
	,	""				// cunilogEvtSevertiyNoneFail	 2
	,	""				// cunilogEvtSevertiyNoneWarn	 3
	,	"     "			// cunilogEvtSeverityBlanks		 4
	,	"EMRGY"			// cunilogEvtSeverityEmergency	 5
	,	"NOTE "			// cunilogEvtSeverityNotice		 6
	,	"INFO "
	,	"MESSG"
	,	"WARN "
	,	"ERROR"
	,	"PASS "			//cunilogEvtSeverityPass		11
	,	"FAIL "
	,	"CRIT "
	,	"FATAL"
	,	"DEBUG"
	,	"TRACE"
	,	"DETAI"
	,	"VERBO"
	,	"ILLEG"			// cunilogEvtSeverityIllegal	18
};
static const char *EventSeverityTexts5tgt [] =
{
		""				// cunilogEvtSeverityNone		 0
	,	""				// cunilogEvtSeverityNonePass	 1
	,	""				// cunilogEvtSevertiyNoneFail	 2
	,	""				// cunilogEvtSevertiyNoneWarn	 3
	,	""				// cunilogEvtSeverityBlanks		 4
	,	"EMRGY"			// cunilogEvtSeverityEmergency	 5
	,	"NOTE"			// cunilogEvtSeverityNotice		 6
	,	"INFO"
	,	"MESSG"
	,	"WARN"
	,	"ERROR"
	,	"PASS"			//cunilogEvtSeverityPass		11
	,	"FAIL"
	,	"CRIT"
	,	"FATAL"
	,	"DEBUG"
	,	"TRACE"
	,	"DETAI"
	,	"VERBO"
	,	"ILLEG"			// cunilogEvtSeverityIllegal	18
};
static const char *EventSeverityTexts9 [] =
{
		""				// cunilogEvtSeverityNone		 0
	,	""				// cunilogEvtSeverityNonePass	 1
	,	""				// cunilogEvtSevertiyNoneFail	 2
	,	""				// cunilogEvtSevertiyNoneWarn	 3
	,	"         "		// cunilogEvtSeverityBlanks		 4
	,	"EMERGENCY"		// cunilogEvtSeverityEmergency	 5
	,	"NOTICE   "		// cunilogEvtSeverityNotice		 6
	,	"INFO     "
	,	"MESSAGE  "
	,	"WARNING  "
	,	"ERROR    "
	,	"PASS     "		// cunilogEvtSeverityPass		11
	,	"FAIL     "
	,	"CRITICAL "
	,	"FATAL    "
	,	"DEBUG    "
	,	"TRACE    "
	,	"DETAIL   "
	,	"VERBOSE  "
	,	"ILLEGAL  "		// cunilogEvtSeverityIllegal	18
};
static const char *EventSeverityTexts9tgt [] =
{
		""				// cunilogEvtSeverityNone		 0
	,	""				// cunilogEvtSeverityNonePass	 1
	,	""				// cunilogEvtSevertiyNoneFail	 2
	,	""				// cunilogEvtSeverityBlanks		 3
	,	"EMERGENCY"		// cunilogEvtSeverityEmergency	 4
	,	"NOTICE"		// cunilogEvtSeverityNotice		 5
	,	"INFO"
	,	"MESSAGE"
	,	"WARNING"
	,	"ERROR"
	,	"PASS"			// cunilogEvtSeverityPass		10
	,	"FAIL"
	,	"CRITICAL"
	,	"FATAL"
	,	"DEBUG"
	,	"TRACE"
	,	"DETAIL"
	,	"VERBOSE"
	,	"ILLEGAL"		// cunilogEvtSeverityIllegal	17
};

#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
STRANSICOLOURSEQUENCE evtSeverityColours [cunilogEvtSeverityXAmountEnumValues] =
{
		{"",	0}														// cunilogEvtSeverityNone		 0
	,	{STR_ANSI_FGCOL_BRIGHT_GREEN,	LEN_ANSI_FGCOL_BRIGHT_GREEN}	// cunilogEvtSeverityNonePass	 1
	,	{STR_ANSI_FGCOL_BRIGHT_RED,		LEN_ANSI_FGCOL_BRIGHT_RED}		// cunilogEvtSevertiyNoneFail	 2
	,	{STR_ANSI_FGCOL_BRIGHT_MAGENTA,	LEN_ANSI_FGCOL_BRIGHT_MAGENTA}	// cunilogEvtSeverityNoneWarn	 3
	,	{"",	0}														// cunilogEvtSeverityBlanks		 4
	,	{STR_ANSI_FGCOL_BRIGHT_RED,		LEN_ANSI_FGCOL_BRIGHT_RED}		// cunilogEvtSeverityEmergency	 5
	,	{"",	0}														// cunilogEvtSeverityNotice		 6
	,	{"",	0}														// cunilogEvtSeverityInfo		 7
	,	{"",	0}														// cunilogEvtSeverityMessage	 8
	,	{STR_ANSI_FGCOL_BRIGHT_MAGENTA,	LEN_ANSI_FGCOL_BRIGHT_MAGENTA}	// cunilogEvtSeverityWarning	 9
	,	{STR_ANSI_FGCOL_BRIGHT_RED,		LEN_ANSI_FGCOL_BRIGHT_RED}		// cunilogEvtSeverityError		10
	,	{STR_ANSI_FGCOL_BRIGHT_GREEN,	LEN_ANSI_FGCOL_BRIGHT_GREEN}	// cunilogEvtSeverityPass		11
	,	{STR_ANSI_FGCOL_BRIGHT_RED,		LEN_ANSI_FGCOL_BRIGHT_RED}		// cunilogEvtSeverityFail		12
	,	{STR_ANSI_FGCOL_BRIGHT_RED,		LEN_ANSI_FGCOL_BRIGHT_RED}		// cunilogEvtSeverityCritical	13
	,	{STR_ANSI_FGCOL_BRIGHT_RED,		LEN_ANSI_FGCOL_BRIGHT_RED}		// cunilogEvtSeverityFatal		14
	,	{"",	0}														// cunilogEvtSeverityDebug		15
	,	{"",	0}														// cunilogEvtSeverityTrace		16
	,	{"",	0}														// cunilogEvtSeverityDetail		17
	,	{"",	0}														// cunilogEvtSeverityVerbose	18
	,	{STR_ANSI_FGCOL_BRIGHT_RED,		LEN_ANSI_FGCOL_BRIGHT_RED}		// cunilogEvtSeverityIllegal	19
																		// cunilogEvtSeverityXAmountEnumValues
};
#endif

#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
	/*
		The return value of this function not only returns the space required for the ANSI escape
		sequence to set the colour of the severity, but also the space required to reset the
		colour again.
		
		This means that both, cpyEvtSeverityColour () and cpyRstEvtSeverityColour () can safely be
		called on a buffer that reserves the size returned by evtSeverityColoursLen (), provided
		that the parameter sev is not changed.
	*/
	static inline size_t evtSeverityColoursLen (cueventseverity sev)
	{
		ubf_assert (0 <= sev);
		ubf_assert (sev < cunilogEvtSeverityXAmountEnumValues);

		size_t len = 0;
		len += evtSeverityColours [sev].lnColSequence;
		if (len)
			len += LEN_ANSI_RESET;
		return len;
	}
#else
	#define evtSeverityColoursLen(sev) (0)
#endif

#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
	static inline void cpyEvtSeverityColour (char **sz, cueventseverity sev)
	{
		ubf_assert_non_NULL	(sz);
		ubf_assert_non_NULL	(*sz);
		ubf_assert			(0 <= sev);
		ubf_assert			(sev < cunilogEvtSeverityXAmountEnumValues);

		if (evtSeverityColours [sev].lnColSequence)
		{
			memcpy (*sz, evtSeverityColours [sev].szColSequence, evtSeverityColours [sev].lnColSequence);
			*sz += evtSeverityColours [sev].lnColSequence;
		}
	}
	static inline void cpyRstEvtSeverityColour (char **sz, cueventseverity sev)
	{
		if (evtSeverityColours [sev].lnColSequence)
		{
			memcpy (*sz, STR_ANSI_RESET, LEN_ANSI_RESET);
			*sz += LEN_ANSI_RESET;
		}
	}
#else
	#define cpyEvtSeverityColour(s, sev)
	#define cpyRstEvtSeverityColour(s)
#endif

static inline size_t requiredEventSeverityChars (cueventseverity sev, cueventsevfmtpy tpy)
{
	// "" or "INF" + " ".
	if (cunilogEvtSeverityBlanks > sev)
			return 0;
	switch (tpy)
	{														// The + 1 is for a space character.
		case cunilogEvtSeverityTypeChars3:					return 3 + 1;
		case cunilogEvtSeverityTypeChars5:					return 5 + 1;
		case cunilogEvtSeverityTypeChars9:					return 9 + 1;
		case cunilogEvtSeverityTypeChars3InBrackets:		return 2 + 3 + 1;
		case cunilogEvtSeverityTypeChars5InBrackets:		return 2 + 5 + 1;
		case cunilogEvtSeverityTypeChars9InBrackets:		return 2 + 9 + 1;
		case cunilogEvtSeverityTypeChars5InTightBrackets:	return 2 + 5 + 1;
		case cunilogEvtSeverityTypeChars9InTightBrackets:	return 2 + 9 + 1;
	}
	ubf_assert_msg (false, "Internal error! We should never get here!");
	return 0;
}

static inline size_t writeEventSeverity (char *szOut, cueventseverity sev, cueventsevfmtpy tpy)
{
	ubf_assert_non_NULL (szOut);
	ubf_assert (0 <= sev);
	ubf_assert (cunilogEvtSeverityXAmountEnumValues > sev);

	if (cunilogEvtSeverityBlanks > sev)
			return 0;

	size_t len;

	switch (tpy)
	{
		case cunilogEvtSeverityTypeChars3:					// "EMG"
			memcpy (szOut, EventSeverityTexts3 [sev], 3);
			szOut [3] = ' ';
			return 3 + 1;
		case cunilogEvtSeverityTypeChars5:					// "EMRGY"
			memcpy (szOut, EventSeverityTexts5 [sev], 5);
			szOut [5] = ' ';
			return 5 + 1;
		case cunilogEvtSeverityTypeChars9:					// "EMERGENCY"
			memcpy (szOut, EventSeverityTexts9 [sev], 9);
			szOut [9] = ' ';
			return 9 + 1;
		case cunilogEvtSeverityTypeChars3InBrackets:		// "[EMG]"
			*szOut ++ = '[';
			memcpy (szOut, EventSeverityTexts3 [sev], 3);
			memcpy (szOut + 3, "] ", 2);
			return 2 + 3 + 1;
		case cunilogEvtSeverityTypeChars5InBrackets:		// "[FAIL ]"
			*szOut ++ = '[';
			memcpy (szOut, EventSeverityTexts5 [sev], 5);
			memcpy (szOut + 5, "] ", 2);
			return 2 + 5 + 1;
		case cunilogEvtSeverityTypeChars9InBrackets:		// "[FAIL     ]"
			*szOut ++ = '[';
			memcpy (szOut, EventSeverityTexts9 [sev], 9);
			memcpy (szOut + 9, "] ", 2);
			return 2 + 9 + 1;
		case cunilogEvtSeverityTypeChars5InTightBrackets:
			len = strlen (EventSeverityTexts5tgt [sev]);
			ubf_assert (len <= 5);
			*szOut ++ = '[';
			memcpy (szOut, EventSeverityTexts5tgt [sev], len);
			memcpy (szOut + len, "] ", 2);
			memset (szOut + len + 2, ' ', 5 - len);
			return 2 + 5 + 1;
		case cunilogEvtSeverityTypeChars9InTightBrackets:
			len = strlen (EventSeverityTexts9tgt [sev]);
			ubf_assert (len <= 9);
			*szOut ++ = '[';
			memcpy (szOut, EventSeverityTexts9tgt [sev], len);
			memcpy (szOut + len, "] ", 2);
			memset (szOut + len + 2, ' ', 9 - len);
			return 2 + 9 + 1;
	}
	ubf_assert_msg (false, "Internal error! We should never get here!");
	return 0;
}

static inline size_t requiredFullstopChars (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);

	return cunilogIsEventAutoFullstop (pev) ? 1 : 0;
};

static inline size_t writeFullStop (char *szOut, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (szOut);
	ubf_assert_non_NULL (pev);

	if (cunilogIsEventAutoFullstop (pev))
	{
		if (pev->lenDataToLog && '.' != pev->szDataToLog [pev->lenDataToLog - 1])
		{
			szOut [0] = '.';
			return 1;
		}
	}
	return 0;
}

static inline size_t eventLenNewline (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);

	return lnLineEnding (pev->pCUNILOG_TARGET->unilogNewLine);
}

static inline enum enLineEndings eventLineEnding (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);

	return pev->pCUNILOG_TARGET->unilogNewLine;
}

static inline void evtTSFormats_unilogEvtTS_ISO8601 (char *chISO, UBF_TIMESTAMP ts)
{
	ISO8601_from_UBF_TIMESTAMPc (chISO, ts);
	chISO [LEN_ISO8601DATETIMESTAMPMS]		= ' ';
}

static inline void evtTSFormats_unilogEvtTS_ISO8601T (char *chISO, UBF_TIMESTAMP ts)
{
	ISO8601T_from_UBF_TIMESTAMPc (chISO, ts);
	chISO [LEN_ISO8601DATETIMESTAMPMS]		= ' ';
}

static inline void evtTSFormats_unilogEvtTS_ISO8601_3spc (char *chISO, UBF_TIMESTAMP ts)
{
	ISO8601_from_UBF_TIMESTAMPc (chISO, ts);
	chISO [LEN_ISO8601DATETIMESTAMPMS]		= ' ';
	chISO [LEN_ISO8601DATETIMESTAMPMS + 1]	= ' ';
	chISO [LEN_ISO8601DATETIMESTAMPMS + 2]	= ' ';
}

static inline void evtTSFormats_unilogEvtTS_ISO8601T_3spc (char *chISO, UBF_TIMESTAMP ts)
{
	ISO8601T_from_UBF_TIMESTAMPc (chISO, ts);
	chISO [LEN_ISO8601DATETIMESTAMPMS]		= ' ';
	chISO [LEN_ISO8601DATETIMESTAMPMS + 1]	= ' ';
	chISO [LEN_ISO8601DATETIMESTAMPMS + 2]	= ' ';
}

static inline void evtTSFormats_unilogEvtTS_NCSADT (char *chNCSADT, UBF_TIMESTAMP ts)
{
	NCSADATETIME_from_UBF_TIMESTAMP (chNCSADT, ts);
}

/*
	Structure for the event timestamp table.
	First member is the length that'll be written; second member is a pointer to the
	function that is going to write this out.
*/
typedef struct seventTSformats
{
	size_t			len;									// Required length.
	void			(*fnc) (char *, UBF_TIMESTAMP);			// Function pointer.
} SeventTSformats;

SeventTSformats evtTSFormats [cunilogEvtTS_AmountEnumValues] =
{
		{	// unilogEvtTS_ISO8601
			LEN_ISO8601DATETIMESTAMPMS + 1,
			evtTSFormats_unilogEvtTS_ISO8601				// "YYYY-MM-DD HH:MI:SS.000+01:00 ".
		}
	,	{	// unilogEvtTS_ISO8601T
			LEN_ISO8601DATETIMESTAMPMS + 1,
			evtTSFormats_unilogEvtTS_ISO8601T				// "YYYY-MM-DDTHH:MI:SS.000+01:00 ".
		}
	,	{	// unilogEvtTS_ISO8601_3spc
			LEN_ISO8601DATETIMESTAMPMS + 3,
			evtTSFormats_unilogEvtTS_ISO8601_3spc			// "YYYY-MM-DD HH:MI:SS.000+01:00   ".
		}
	,	{	// unilogEvtTS_ISO8601T_3spc
			LEN_ISO8601DATETIMESTAMPMS + 3,
			evtTSFormats_unilogEvtTS_ISO8601T_3spc			// "YYYY-MM-DDTHH:MI:SS.000+01:00   ".
		}
	,	{	// cunilogEvtTS_NCSADT
			LEN_NCSA_COMMON_LOG_DATETIME + 1,				// "[10/Oct/2000:13:55:36 -0700] ".
			evtTSFormats_unilogEvtTS_NCSADT
		}
};

static inline size_t requiredEvtLineTimestampAndSeverityLength (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);

	size_t	r;

	// "YYYY-MM-DD HH:MI:SS.000+01:00" + " ".
	r = evtTSFormats [pev->pCUNILOG_TARGET->unilogEvtTSformat].len;
	// "WRN" + " "
	r += requiredEventSeverityChars (pev->evSeverity, pev->pCUNILOG_TARGET->evSeverityType);

	return r;
}

static inline size_t requiredEventLineSizeU8 (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);
	ubf_assert (cunilogEvtTypeNormalText == pev->evType);

	size_t	r;

	// "YYYY-MM-DD HH:MI:SS.000+01:00" + " " + "WRN" + " ".
	r = requiredEvtLineTimestampAndSeverityLength (pev);
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// Actual data.
	r += pev->lenDataToLog;
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// "."
	r += requiredFullstopChars (pev);
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// CR + LF max.
	r += eventLenNewline (pev);
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// Terminating NUL.
	r += 1;
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	return r;
}

/*
	writeEventLineFromSUNILOGEVENTU8

	The caller is responsible for szEventLine to point to a buffer big enough to hold the
	event line. The function requiredEventLineSize () obtains this size.
*/
static size_t writeEventLineFromSUNILOGEVENTU8 (char *szEventLine, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (szEventLine);
	ubf_assert_non_NULL (pev);
	ubf_assert (cunilogEvtTypeNormalText == pev->evType);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);
	ubf_assert (cunilogIsTargetInitialised (pev->pCUNILOG_TARGET));

	char	*szOrg = szEventLine;

	DBG_RESET_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker);

	// "YYYY-MM-DD HH:MI:SS.000+01:00" + " " (see table above).
	evtTSFormats [pev->pCUNILOG_TARGET->unilogEvtTSformat].fnc (szEventLine, pev->stamp);
	//ISO8601_from_UBF_TIMESTAMP (szEventLine, pue->stamp);

	szEventLine += evtTSFormats [pev->pCUNILOG_TARGET->unilogEvtTSformat].len;
	szEventLine += writeEventSeverity (szEventLine, pev->evSeverity, pev->pCUNILOG_TARGET->evSeverityType);
	DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, szEventLine - szOrg);

	memcpy (szEventLine, pev->szDataToLog, pev->lenDataToLog);
	szEventLine += pev->lenDataToLog;
	DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, szEventLine - szOrg);

	szEventLine += writeFullStop (szEventLine, pev);
	DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, szEventLine - szOrg);

	// We've reserved space for a new line character sequence in requiredEventLineSizeU8 ()
	//	but we're not going to add this here yet. Instead, we NUL-terminate. This simply lets
	//	us call puts (). We'll add the new line sequence later in the processor that writes
	//	to the logfile.

	// The NUL byte is useful for OS API functions.
	szEventLine [0] = ASCII_NUL;
	return szEventLine - szOrg;
}

/*
static bool obtainValidSCUNILOGDUMPinCUNILOG_TARGET (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	if (NULL == put->psdump)
	{
		put->psdump = ubf_malloc (sizeof (SCUNILOGDUMP));
		SCUNILOGDUMP *pd = put->psdump;
		if (pd)
		{
			pd->dump_prs = ubf_malloc (sizeof (SHEX_DUMP_PARS));
			pd->dump_sns = ubf_malloc (sizeof (SHEX_DUMP_SAN));
			if (pd->dump_prs && pd->dump_sns)
			{
				ubf_data_dump_def_SUBF_DUMP_PARS (pd->dump_prs);
				SHEX_DUMP_SAN *ps = pd->dump_sns;
				ps->bInitialised = false;
				return true;
			} else
			{
				if (NULL != pd->dump_prs)
					ubf_free (pd->dump_prs);
				if (NULL != pd->dump_sns)
					ubf_free (pd->dump_sns);
				pd->dump_prs = NULL;
				pd->dump_sns = NULL;
			}
		}
		return false;
	} else
		return true;
}
*/

static inline size_t widthOfCaptionLengthFromCunilogEventType (cueventtype type)
{
	switch (type)
	{
		case cunilogEvtTypeNormalText:				return 0;
		case cunilogEvtTypeCommand:					return 0;
		case cunilogEvtTypeHexDumpWithCaption8:		return 1;
		case cunilogEvtTypeHexDumpWithCaption16:	return 2;
		case cunilogEvtTypeHexDumpWithCaption32:	return 4;
		case cunilogEvtTypeHexDumpWithCaption64:	return 8;
		default:									return 0;
	}
}

static size_t readCaptionLengthFromData (unsigned char *pData, size_t ui)
{
	uint8_t			ui8;
	uint16_t		ui16;
	uint32_t		ui32;
	uint64_t		ui64;
	size_t			lnRet;

	switch (ui)
	{
		case 0:	lnRet = 0;									break;
		case 1:	memcpy (&ui8, pData, ui);	lnRet = ui8;	break;
		case 2:	memcpy (&ui16, pData, ui);	lnRet = ui16;	break;
		case 4:	memcpy (&ui32, pData, ui);	lnRet = ui32;	break;
		case 8:	
				ubf_assert_msg (false, "Really??? A caption length of more than 4 GiB??");
				memcpy (&ui64, pData, ui);
				lnRet = (size_t) ui64;						break;
		default:
				ubf_assert_msg (false, "Bug");
				lnRet = 0;									break;
	}
	return lnRet;
}

static char		scSummaryOctets []	= " octets/bytes.";
static size_t	lnSummaryOctets		= sizeof (scSummaryOctets) - 1;

static inline size_t requiredEventLineSizeHexDump	(
						unsigned char		**pDumpData,
						size_t				*width,
						size_t				*len,
						CUNILOG_EVENT		*pev
													)
{
	ubf_assert_non_NULL (pev);
	ubf_assert	(
						cunilogEvtTypeHexDumpWithCaption8	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption16	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption32	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption64	== pev->evType
				);
	ubf_assert_non_NULL (pDumpData);
	ubf_assert_non_NULL (width);
	ubf_assert_non_NULL (len);

	CUNILOG_TARGET *put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);

	size_t	r;

	/*
		Timestamp + Type + caption, etc.
	*/

	// "YYYY-MM-DD HH:MI:SS.000+01:00" + " " + "WRN" + " ".
	r = requiredEvtLineTimestampAndSeverityLength (pev);
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// The width of the caption length.
	size_t ui			= widthOfCaptionLengthFromCunilogEventType (pev->evType);
	// Its actual length. This needs to be added to our return value.
	size_t lenCaption	= readCaptionLengthFromData (pev->szDataToLog, ui);
	*pDumpData			= pev->szDataToLog + ui + lenCaption;
	*width				= ui;
	*len				= lenCaption;
	r += lenCaption;
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// "."
	r += requiredFullstopChars (pev);
	// New line.
	r += eventLenNewline (pev);
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// Actual dump data size. Includes NUL terminator.
	size_t sizHexDmp = hxdmpRequiredSize (pev->lenDataToLog, put->dumpWidth, eventLineEnding (pev));
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, sizHexDmp);
	r += sizHexDmp;
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// ASCII TAB.
	++ r;

	// To be considered in the future:
	//	It would be faster to just always reserve UBF_UINT64_LEN octets instead of
	//	performing the uint -> ASCII transformation.
	char cDmpOctets [UBF_UINT64_SIZE];
	//r += ubf_str_from_uint64 (cDmpOctets, pev->lenDataToLog);
	ubf_str__from_uint64 (cDmpOctets, 10, pev->lenDataToLog);
	r += 10;
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	r += lnSummaryOctets;
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	// New line.
	r += eventLenNewline (pev);
	DBG_TRACK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, r);

	#ifdef DEBUG
		++ r;												// Space for a debug marker.
	#endif

	return r;
}

static size_t createDumpEventLineFromSUNILOGEVENT (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);
	ubf_assert (isInitialisedSMEMBUF (&pev->pCUNILOG_TARGET->mbLogEventLine));
	ubf_assert	(
						cunilogEvtTypeHexDumpWithCaption8	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption16	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption32	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption64	== pev->evType
				);

	CUNILOG_TARGET *put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);

	unsigned char	*pDumpData;
	size_t			captionWidth;
	size_t			captionLen;
	size_t lenTotal = requiredEventLineSizeHexDump (&pDumpData, &captionWidth, &captionLen, pev);
	// pDumpData				Points to the data to dump.
	// pev->lenDataToLog		Its length.

	growToSizeSMEMBUF64aligned (&put->mbLogEventLine, lenTotal);
	if (isUsableSMEMBUF (&put->mbLogEventLine))
	{
		#ifdef DEBUG
			put->mbLogEventLine.buf.pch [lenTotal] = CUNILOG_DEFAULT_DBG_CHAR;
		#endif
		char *szOut = put->mbLogEventLine.buf.pch;
		char *szOrg = szOut;

		// Timestamp + severity.
		evtTSFormats [put->unilogEvtTSformat].fnc (szOut, pev->stamp);
		szOut += evtTSFormats [put->unilogEvtTSformat].len;
		szOut += writeEventSeverity (szOut, pev->evSeverity, put->evSeverityType);
		DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, szOut - szOrg);

		// Caption.
		memcpy (szOut, pev->szDataToLog + captionWidth, captionLen);
		szOut += captionLen;
		DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, szOut - szOrg);

		// "." + new line.
		size_t lenNewLine = eventLenNewline (pev);
		szOut += writeFullStop (szOut, pev);
		memcpy (szOut, ccLineEnding (eventLineEnding (pev)), lenNewLine);
		szOut += lenNewLine;
		DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, szOut - szOrg);

		put->lnLogEventLine = szOut - szOrg;
		char *szHexDmpOut = szOut;
		size_t sizHx = hxdmpWriteHexDump	(
						szHexDmpOut, pDumpData, pev->lenDataToLog,
						put->dumpWidth, put->unilogNewLine
											);
		DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, sizHx + 1);
		ubf_assert (CUNILOG_DEFAULT_DBG_CHAR == put->mbLogEventLine.buf.pch [lenTotal]);
		put->lnLogEventLine += sizHx;
		DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, put->lnLogEventLine + 1);

		szOut = szHexDmpOut + sizHx;
		szOut [0] = ASCII_TAB;
		++ szOut;
		++ put->lnLogEventLine;

		//size_t lnOctets = ubf_str_from_uint64 (szOut, pev->lenDataToLog);
		size_t lnOctets = 10;
		ubf_str__from_uint64 (szOut, 10, pev->lenDataToLog);
		put->lnLogEventLine += lnOctets;
		ubf_assert (CUNILOG_DEFAULT_DBG_CHAR == put->mbLogEventLine.buf.pch [lenTotal]);
		DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, put->lnLogEventLine + 1);

		szOut += lnOctets;
		memcpy (szOut, scSummaryOctets, lnSummaryOctets + 1);
		put->lnLogEventLine += lnSummaryOctets;
		DBG_TRACK_CHECK_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker, put->lnLogEventLine + 1);

		return put->lnLogEventLine;
	}
	return CUNILOG_SIZE_ERROR;
}

static size_t createU8EventLineFromSUNILOGEVENT (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);
	ubf_assert (isInitialisedSMEMBUF (&pev->pCUNILOG_TARGET->mbLogEventLine));
	ubf_assert (cunilogEvtTypeNormalText == pev->evType);

	size_t requiredEvtLineSize;

	requiredEvtLineSize = requiredEventLineSizeU8 (pev);
	growToSizeSMEMBUF64aligned (&pev->pCUNILOG_TARGET->mbLogEventLine, requiredEvtLineSize);
	if (isUsableSMEMBUF (&pev->pCUNILOG_TARGET->mbLogEventLine))
	{
		pev->pCUNILOG_TARGET->lnLogEventLine =
			writeEventLineFromSUNILOGEVENTU8 (pev->pCUNILOG_TARGET->mbLogEventLine.buf.pch, pev);
		return pev->pCUNILOG_TARGET->lnLogEventLine;
	}
	return CUNILOG_SIZE_ERROR;
}

static size_t createEventLineFromSUNILOGEVENT (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);
	ubf_assert (isInitialisedSMEMBUF (&pev->pCUNILOG_TARGET->mbLogEventLine));

	DBG_RESET_CNTTRACKER (pev->pCUNILOG_TARGET->evtLineTracker);

	switch (pev->evType)
	{
		case cunilogEvtTypeNormalText:
			return createU8EventLineFromSUNILOGEVENT	(pev);
	#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
		case cunilogEvtTypeCommand:
			ubf_assert_msg (false, "Cunilog bug! This function is not to be called in this case!");
			break;
	#endif
		case cunilogEvtTypeHexDumpWithCaption8:
		case cunilogEvtTypeHexDumpWithCaption16:
		case cunilogEvtTypeHexDumpWithCaption32:
		case cunilogEvtTypeHexDumpWithCaption64:
			return createDumpEventLineFromSUNILOGEVENT	(pev);
		default:
			break;
	}
	return CUNILOG_SIZE_ERROR;
}

/*
	Not to be called with 0 == ui.
*/
static inline void storeCaptionLength (unsigned char **pData, size_t ui, size_t lenCapt)
{
	ubf_assert_non_NULL (pData);
	ubf_assert_non_NULL (*pData);
	ubf_assert_non_0	(ui);

	uint8_t			ui8;
	uint16_t		ui16;
	uint32_t		ui32;
	uint64_t		ui64;

	switch (ui)
	{
		/*
		// We're not being called for 0 == ui.
		case 0:
			ubf_assert_msg (false, "Must be a bug!");
			return;
		*/
		case 1:
			ui8 = (uint8_t) (lenCapt & 0xFF);
			memcpy (*pData, &ui8, ui);
			break;
		case 2:
			ui16 = (uint16_t) (lenCapt & 0xFFFF);
			memcpy (*pData, &ui16, ui);
			break;
		case 4:
			ui32 = (uint32_t) (lenCapt & 0xFFFFFFFF);
			memcpy (*pData, &ui32, ui);
			break;
		case 8:
			ubf_assert_msg (false, "Really??? A caption length of more than 4 GiB??");
			ui64 = (uint64_t) lenCapt;
			memcpy (*pData, &ui64, ui);
			break;
		default:
			ubf_assert_msg (false, "Must be a bug!");
			break;
	}
	*pData += ui;
}

/*
	Note that ccData can be NULL for event type cunilogEvtTypeCommand,
	in which case a buffer of siz octets is reserved but not initialised!
*/
static CUNILOG_EVENT *CreateCUNILOG_EVENTandData	(
					CUNILOG_TARGET				*put,
					cueventseverity				sev,
					const char					*ccCapt,
					size_t						lenCapt,
					cueventtype					type,
					const char					*ccData,
					size_t						siz
													)
{
	ubf_assert_non_NULL	(put);
	if (ccData)
		ubf_assert (cunilogEvtTypeCommand != type && NULL != ccData);
	else
		ubf_assert (cunilogEvtTypeCommand == type && NULL == ccData);
	ubf_assert			(USE_STRLEN != siz);
	ubf_assert			(0 <= type);
	ubf_assert			(cunilogEvtTypeAmountEnumValues > type);

	size_t			wl		= widthOfCaptionLengthFromCunilogEventType (type);
	ubf_assert (wl || 0 == lenCapt);						// If 0 == wl we can't have a caption.
	size_t			aln		= ALIGNED_SIZE (sizeof (CUNILOG_EVENT), CUNILOG_DEFAULT_ALIGNMENT);
	size_t			ln		= aln + wl + lenCapt + siz;
	CUNILOG_EVENT	*pev	= ubf_malloc (ln);

	if (pev)
	{
		unsigned char *pData = (unsigned char *) pev + aln;
		
		if	(	
					cunilogHasEnqueueTimestamps	(put)
				&&	HAS_CUNILOG_TARGET_A_QUEUE	(put)
			)
		{
			FillCUNILOG_EVENT	(
				pev, put,
				CUNILOGEVENT_ALLOCATED,
				0,
				sev, type,
				pData, siz, ln
								);
		} else
		{
			FillCUNILOG_EVENT	(
				pev, put,
				CUNILOGEVENT_ALLOCATED,
				LocalTime_UBF_TIMESTAMP (),
				sev, type,
				pData, siz, ln
								);
		}
		if (wl)
		{
			storeCaptionLength (&pData, wl, lenCapt);
			memcpy (pData, ccCapt, lenCapt);
			pData += lenCapt;
		}
		if (ccData)
			memcpy (pData, ccData, siz);
		else
			ubf_assert (cunilogEvtTypeCommand == type);
	}
	return pev;
}

/*
	Note that ccData can be NULL for event type cunilogEvtTypeCommand,
	in which case a buffer of siz octets is reserved but not initialised!

	This function is identical to CreateCUNILOG_EVENTandData () but requires
	an additional UBF_TIMESTAMP parameter (ts).

	This is a separate function so that it could easily be removed via
	macro definitions later if required.
*/
static CUNILOG_EVENT *CreateCUNILOG_EVENTandDataTS	(
					CUNILOG_TARGET				*put,
					cueventseverity				sev,
					const char					*ccCapt,
					size_t						lenCapt,
					cueventtype					type,
					const char					*ccData,
					size_t						siz,
					UBF_TIMESTAMP				ts
													)
{
	ubf_assert_non_NULL	(put);
	if (ccData)
		ubf_assert (cunilogEvtTypeCommand != type && NULL != ccData);
	else
		ubf_assert (cunilogEvtTypeCommand == type && NULL == ccData);
	ubf_assert			(USE_STRLEN != siz);
	ubf_assert			(0 <= type);
	ubf_assert			(cunilogEvtTypeAmountEnumValues > type);

	size_t			wl		= widthOfCaptionLengthFromCunilogEventType (type);
	ubf_assert (wl || 0 == lenCapt);						// If 0 == wl we can't have a caption.
	size_t			aln		= ALIGNED_SIZE (sizeof (CUNILOG_EVENT), CUNILOG_DEFAULT_ALIGNMENT);
	size_t			ln		= aln + wl + lenCapt + siz;
	CUNILOG_EVENT	*pev	= ubf_malloc (ln);

	if (pev)
	{
		unsigned char *pData = (unsigned char *) pev + aln;
		
		FillCUNILOG_EVENT	(
			pev, put,
			CUNILOGEVENT_ALLOCATED,
			ts,
			sev, type,
			pData, siz, ln
							);
		if (wl)
		{
			storeCaptionLength (&pData, wl, lenCapt);
			memcpy (pData, ccCapt, lenCapt);
			pData += lenCapt;
		}
		if (ccData)
			memcpy (pData, ccData, siz);
		else
			ubf_assert (cunilogEvtTypeCommand == type);
	}
	return pev;
}

static inline cueventtype cunilogEventTypeFromLength (size_t len)
{
	ubf_assert (USE_STRLEN != len);

	if (len < UINT8_MAX)
		return cunilogEvtTypeHexDumpWithCaption8;
	if (len < UINT16_MAX)
		return cunilogEvtTypeHexDumpWithCaption16;
	if (len < UINT32_MAX)
		return cunilogEvtTypeHexDumpWithCaption32;
	ubf_assert_msg (false, "Really??? A caption length of more than 4 GiB??");
	return cunilogEvtTypeHexDumpWithCaption64;
}

CUNILOG_EVENT *CreateCUNILOG_EVENT_Data		(
					CUNILOG_TARGET				*put,
					cueventseverity				sev,
					const char					*ccData,
					size_t						siz,
					const char					*ccCapt,
					size_t						lenCapt
											)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(ccData);
	ubf_assert (USE_STRLEN != siz);

	if (ccCapt)
		lenCapt = USE_STRLEN == lenCapt ? strlen (ccCapt) : lenCapt;
	else
		lenCapt = 0;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENTandData	(
							put, sev, ccCapt, lenCapt,
							cunilogEventTypeFromLength (lenCapt),
							ccData, siz
													);
	return pev;
}

CUNILOG_EVENT *CreateCUNILOG_EVENT_Text		(
					CUNILOG_TARGET				*put,
					cueventseverity				sev,
					const char					*ccText,
					size_t						len
											)
{
	ubf_assert_non_NULL (put);
	ubf_assert_non_NULL (ccText);

	len = USE_STRLEN == len ? strlen (ccText) : len;

	/*
	while (len && ('\n' == ccText [len - 1] || '\r' == ccText [len - 1]))
		-- len;
	*/
	len = strRemoveLineEndingsFromEnd (ccText, len);

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENTandData	(
							put, sev, NULL, 0, cunilogEvtTypeNormalText,
							ccText, len
													);
	return pev;
}

CUNILOG_EVENT *CreateCUNILOG_EVENT_TextTS		(
					CUNILOG_TARGET				*put,
					cueventseverity				sev,
					const char					*ccText,
					size_t						len,
					UBF_TIMESTAMP				ts
											)
{
	ubf_assert_non_NULL (put);
	ubf_assert_non_NULL (ccText);

	len = USE_STRLEN == len ? strlen (ccText) : len;

	/*
	while (len && ('\n' == ccText [len - 1] || '\r' == ccText [len - 1]))
		-- len;
	*/
	len = strRemoveLineEndingsFromEnd (ccText, len);

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENTandDataTS	(
							put, sev, NULL, 0, cunilogEvtTypeNormalText,
							ccText, len, ts
														);
	return pev;
}

CUNILOG_EVENT *CreateSUNILOGEVENT_W	(
					cueventseverity		sev,
					size_t				lenDataW
									)
{
	UNREFERENCED_PARAMETER (sev);
	UNREFERENCED_PARAMETER (lenDataW);

	//return CreateSUNILOGSTRUCT_U8 (evType, lenDataW * sizeof (wchar_t));
	ubf_assert (false);
	return NULL;
}

CUNILOG_EVENT *DuplicateCUNILOG_EVENT (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);

	size_t size;

	if (pev->sizEvent)
		size = pev->sizEvent;
	else
		size = sizeof (CUNILOG_EVENT);
	CUNILOG_EVENT *pnev = ubf_malloc (size);
	if (pnev)
	{
		memcpy (pnev, pev, size);
		cunilogSetEventAllocated (pnev);
	}
	return pnev;
}

CUNILOG_EVENT *DoneCUNILOG_EVENT (CUNILOG_TARGET *put, CUNILOG_EVENT *pev)
{
	if (NULL == put || put == pev->pCUNILOG_TARGET)
	{
		if (pev->szDataToLog && cunilogIsEventDataAllocated (pev))
		{
			ubf_free (pev->szDataToLog);
		}
		if (cunilogIsEventAllocated (pev))
		{
			ubf_free (pev);
		}
	}
	return NULL;
}

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline CUNILOG_EVENT *DequeueAllCUNILOG_EVENTs (CUNILOG_TARGET *put);
#endif

#ifndef CUNILOG_BUILD_WITHOUT_ERROR_CALLBACK
	void cunilogSetTargetErrorAndInvokeErrorCallback	(
			CUNILOG_ERROR cunilog_error, CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev
														)
	{
		ubf_assert_non_NULL (cup);
		ubf_assert_non_NULL (pev);
		ubf_assert_non_NULL (pev->pCUNILOG_TARGET);

		#ifdef PLATFORM_IS_WINDOWS
			DWORD syserror = GetLastError ();
		#else
			int32_t syserror = errno;
		#endif
		SetCunilogError (pev->pCUNILOG_TARGET, cunilog_error, syserror);

		if (pev->pCUNILOG_TARGET->errorCB)
		{
			errCBretval rv = pev->pCUNILOG_TARGET->errorCB (pev->pCUNILOG_TARGET->error, cup, pev);
			ubf_assert (0 <= rv);
			ubf_assert (cunilogErrCB_AmountEnumValues > rv);

			CUNILOG_EVENT *pve;
			CUNILOG_EVENT *nxt;

			switch (rv)
			{
				case cunilogErrCB_ignore:
					break;
				case cunilogErrCB_next_processor:
					break;
				case cunilogErrCB_next_event:
					cunilogEventSetIgnoreRemainingProcessors (pev);
					break;
				case cunilogErrCB_shutdown:
					cunilogTargetSetShutdownInitiatedFlag (pev->pCUNILOG_TARGET);
					break;
				case cunilogErrCB_cancel:
					cunilogTargetSetShutdownInitiatedFlag (pev->pCUNILOG_TARGET);
					#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
						pve = DequeueAllCUNILOG_EVENTs (pev->pCUNILOG_TARGET);
						while (pve)
						{
							nxt = pve->next;
							DoneCUNILOG_EVENT (NULL, pve);
							pve = nxt;
						}
					#endif
					break;
				default:
					ubf_assert (false);
					break;
			}
		}
	}

	/*
		cunilogTestErrorCB
	*/
	#ifdef CUNILOG_BUILD_WITH_TEST_ERRORCB
		void cunilogTestErrorCB	(
				CUNILOG_ERROR cunilog_error, CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev
								)
		{
			ubf_assert_non_NULL (cup);
			ubf_assert_non_NULL (pev);
			ubf_assert_non_NULL (pev->pCUNILOG_TARGET);

			if (cunilogTargetHasAlwaysCallErrorCB (pev->pCUNILOG_TARGET))
			{
				errCBretval rv = pev->pCUNILOG_TARGET->errorCB	(
										cunilog_error, cup, pev
																);
				UNUSED (rv);
				ubf_assert (0 <= rv);
				ubf_assert (cunilogErrCB_AmountEnumValues > rv);
			}
		}
	#else
		#define cunilogTestErrorCB(error, cup, pev)
	#endif
#else
	#define cunilogInvokeErrorCallback(error, cup, pev)
	#define cunilogTestErrorCB(error, cup, pev)
#endif

/*
	The dummy/no-operation processor.
*/
static bool cunilogProcessNoneFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	UNREFERENCED_PARAMETER (cup);
	UNREFERENCED_PARAMETER (pev);

	return true;
}

#ifdef PLATFORM_IS_WINDOWS
	static inline int cunilogPutsWin (const char *szOutput, size_t len)
	{
		if (cunilogConsoleIsUninitialised == ourCunilogConsoleOutputCodePage)
			CunilogSetConsoleTo (cunilogConsoleIsUTF8);
		CunilogEnableANSIifNotInitialised ();

		if (len)
		{	// This function expects a NUL-terminated string.
			ubf_assert (strlen (szOutput) == len);
			ubf_assert (ASCII_NUL == szOutput [len]);

			switch (ourCunilogConsoleOutputCodePage)
			{
				case cunilogConsoleIsUTF8:		return puts					(szOutput);
				case cunilogConsoleIsUTF16:		return putsU8toU16stdout	(szOutput);
				case cunilogConsoleIsNeither:	return puts					(szOutput);
				default:						return puts					(szOutput);
			}
		} else
		{
			switch (ourCunilogConsoleOutputCodePage)
			{
				case cunilogConsoleIsUTF8:		return puts					("");
				case cunilogConsoleIsUTF16:		return putsU8toU16stdout	("");
				case cunilogConsoleIsNeither:	return puts					("");
				default:						return puts					("");
			}
		}
	}
#endif

#ifdef PLATFORM_IS_WINDOWS
	static inline int cunilogPrintWin (const char *szOutput, size_t len)
	{
		if (cunilogConsoleIsUninitialised == ourCunilogConsoleOutputCodePage)
			CunilogSetConsoleTo (cunilogConsoleIsUTF8);
		CunilogEnableANSIifNotInitialised ();

		if (len)
		{	// This function expects a NUL-terminated string.
			ubf_assert (strlen (szOutput) == len);
			ubf_assert (ASCII_NUL == szOutput [len]);

			switch (ourCunilogConsoleOutputCodePage)
			{
				case cunilogConsoleIsUTF8:		return printf				(szOutput);
				case cunilogConsoleIsUTF16:		return fprintfU8toU16stream	(stdout, szOutput);
				case cunilogConsoleIsNeither:	return printf				(szOutput);
				default:						return printf				(szOutput);
			}
		} else
		{
			switch (ourCunilogConsoleOutputCodePage)
			{
				case cunilogConsoleIsUTF8:		return printf				("");
				case cunilogConsoleIsUTF16:		return fprintfU8toU16stream	(stdout, "");
				case cunilogConsoleIsNeither:	return printf				("");
				default:						return printf				("");
			}
		}
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
	static inline void cunilogFillColouredEchoEvtLine	(
							char				**pszToOutput,
							size_t				*plnToOutput,
							CUNILOG_EVENT		*pev
														)
	{
		ubf_assert_non_NULL (pszToOutput);
		ubf_assert_non_NULL (plnToOutput);
		ubf_assert_non_NULL (pev);
		// Includes LEN_ANSI_RESET too.
		size_t	lnThisColour	= evtSeverityColoursLen (pev->evSeverity);

		if	(
					cunilogTargetHasUseColourForEcho (pev->pCUNILOG_TARGET)
				&&	lnThisColour
			)
		{
			size_t	lnEvtLine		= pev->pCUNILOG_TARGET->lnLogEventLine;
			size_t	lnColEcho		= lnThisColour
									+ lnEvtLine
									;

			growToSizeSMEMBUF (&pev->pCUNILOG_TARGET->mbColEventLine, lnColEcho + 1);
			if (isUsableSMEMBUF (&pev->pCUNILOG_TARGET->mbColEventLine))
			{
				ubf_assert	(
									strlen (pev->pCUNILOG_TARGET->mbLogEventLine.buf.pch)
								==	pev->pCUNILOG_TARGET->lnLogEventLine
							);
				char *sz = pev->pCUNILOG_TARGET->mbColEventLine.buf.pch;
				cpyEvtSeverityColour (&sz, pev->evSeverity);
				memcpy (sz, pev->pCUNILOG_TARGET->mbLogEventLine.buf.pch, lnEvtLine);
				sz += lnEvtLine;
				cpyRstEvtSeverityColour (&sz, pev->evSeverity);
				*sz = ASCII_NUL;
				pev->pCUNILOG_TARGET->lnColEventLine = lnColEcho;
				*pszToOutput = pev->pCUNILOG_TARGET->mbColEventLine.buf.pch;
				*plnToOutput = lnColEcho;
				return;
			}
		}
		*pszToOutput = pev->pCUNILOG_TARGET->mbLogEventLine.buf.pch;
		*plnToOutput = pev->pCUNILOG_TARGET->lnLogEventLine;
	}
#endif

static bool cunilogProcessEchoFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	UNREFERENCED_PARAMETER (cup);
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);

	if (cunilogIsNoEcho (pev->pCUNILOG_TARGET) || cunilogHasEventNoEcho (pev))
		return true;

	// The actual task of this processor: Echo the event line.
	//	Note that we can rely on the following conditions here:
	//		- The line to output is NUL-terminated.
	//		- It only consists of printable characters.
	//		- The length of the event line has been stored correctly.
	//		- If we require a lock, we have it already.

	int		ips;
	char	*szToOutput;
	size_t	lnToOutput;

	#ifndef CUNILOG_BUILD_WITHOUT_CONSOLE_COLOUR
		cunilogFillColouredEchoEvtLine (&szToOutput, &lnToOutput, pev);
	#else
		szToOutput = pev->pCUNILOG_TARGET->mbLogEventLine.buf.pch;
		lnToOutput = pev->pCUNILOG_TARGET->lnLogEventLine;
	#endif

	#ifdef PLATFORM_IS_WINDOWS
		ips = cunilogPutsWin (szToOutput, lnToOutput);
	#else
		if (lnToOutput)
			ips = puts (szToOutput);
		else
			ips = puts ("");
	#endif
	if (EOF == ips)
	{	// "Bad file descriptor" might not be the best error here but what's better?
		ubf_assert_msg (false, "Error writing to stdout.");
		cunilogSetTargetErrorAndInvokeErrorCallback (EBADF, cup, pev);
	}
	return true;
}

static bool cunilogProcessUpdateLogFileNameFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	UNREFERENCED_PARAMETER (cup);
	ubf_assert_non_NULL (pev);
	
	CUNILOG_TARGET	*put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);

	#ifdef DEBUG
		char *sz = put->mbLogfileName.buf.pch;
		UNREFERENCED_PARAMETER (sz);
	#endif

	switch (put->culogPostfix)
	{
		case cunilogPostfixNone:
			return true;

		case cunilogPostfixMinute:
		case cunilogPostfixMinuteT:
		case cunilogPostfixHour:
		case cunilogPostfixHourT:
		case cunilogPostfixDay:
		case cunilogPostfixWeek:
		case cunilogPostfixMonth:
		case cunilogPostfixYear:
			savePrevTimestamp (pev);
			return true;

		case cunilogPostfixLogMinute:
		case cunilogPostfixLogMinuteT:
		case cunilogPostfixLogHour:
		case cunilogPostfixLogHourT:
		case cunilogPostfixLogDay:
		case cunilogPostfixLogWeek:
		case cunilogPostfixLogMonth:
		case cunilogPostfixLogYear:
			return true;

		case cunilogPostfixDotNumberMinutely:
		case cunilogPostfixDotNumberHourly:
		case cunilogPostfixDotNumberDaily:
		case cunilogPostfixDotNumberWeekly:
		case cunilogPostfixDotNumberMonthly:
		case cunilogPostfixDotNumberYearly:
			return true;

		default:
			ubf_assert_msg (false, "Bug!");
			return true;
	}
}

/*
	Closes the previous file and opens the new one.
*/
static inline bool cunilogOpenNewLogFile (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert			(isInitialisedSMEMBUF (&put->mbLogfileName));

	#ifdef OS_IS_WINDOWS
		CloseHandle (put->logfile.hLogFile);
		return cunilogOpenLogFile (put);
	#else
		fclose (put->logfile.fLogFile);
		return cunilogOpenLogFile (put);
	#endif
}

static inline bool requiresOpenLogFile (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

#ifdef OS_IS_WINDOWS
		return NULL == put->logfile.hLogFile || INVALID_HANDLE_VALUE == put->logfile.hLogFile;
	#else
		return NULL == put->logfile.fLogFile;
	#endif
}

#ifdef CUNILOG_BUILD_WITH_TEST_ERRORCB
#ifndef CUNILOG_BUILD_WITHOUT_ERROR_CALLBACK
	static inline bool requiresNewLogFileCunilogTest	(
							CUNILOG_TARGET		*put,
							CUNILOG_PROCESSOR	*cup,
							CUNILOG_EVENT		*pev
														)
	{
		ubf_assert_non_NULL (put);

		cunilogTestErrorCB (CUNILOG_ERROR_TEST_BEFORE_REQUIRES_NEW_LOGFILE, cup, pev);
		int r = requiresNewLogFile (put);
		cunilogTestErrorCB (CUNILOG_ERROR_TEST_AFTER_REQUIRES_NEW_LOGFILE, cup, pev);

		return r > 0;
	}
#endif
#endif

#ifdef CUNILOG_BUILD_WITH_TEST_ERRORCB
	#ifndef CUNILOG_BUILD_WITHOUT_ERROR_CALLBACK
		#define REQUIRES_NEW_LOGFILE(put, cup, pev)		\
				requiresNewLogFileCunilogTest (put, cup, pev)
	#else
		#define REQUIRES_NEW_LOGFILE(put, cup, pev)		\
				requiresNewLogFile (put)
	#endif
#else
	#define REQUIRES_NEW_LOGFILE(put, cup, pev)			\
				requiresNewLogFile (put)
#endif

static inline size_t addNewLineToLogEventLine (char *pData, size_t lnData, enum enLineEndings nl)
{	// At least one octet has been reserved for a newline character, and one
	//	for NUL, hence we're definitely safe to write 2 more octets.
	size_t len;
	const char *cc = szLineEnding (nl, &len);
	ubf_assert (len <= 2);
	memcpy (pData + lnData, cc, len + 1);
	return lnData + len;
}

static bool cunilogWriteDataToLogFile (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	char				*pData	= put->mbLogEventLine.buf.pch;
	size_t				lnData	= put->lnLogEventLine;
	enum enLineEndings	nl		= put->unilogNewLine;

	#ifdef OS_IS_WINDOWS
		DWORD dwWritten;
		DWORD toWrite = addNewLineToLogEventLine (pData, lnData, nl) & 0xFFFFFFFF;
		// The file has been opened with FILE_APPEND_DATA, i.e. we don't need to
		//	seek ourselves.
		//	LARGE_INTEGER	z = {0, 0};
		//	SetFilePointerEx (pl->hLogFile, z, NULL, FILE_END);
		bool b = WriteFile (put->logfile.hLogFile, pData, toWrite, &dwWritten, NULL);
		pData [lnData] = ASCII_NUL;
		return b;
	#else
		long lToWrite = (long) addNewLineToLogEventLine (pData, lnData, nl);
		// See https://www.man7.org/linux/man-pages/man3/fopen.3.html .
		//	A call "fseek (pl->fLogFile, (long) 0, SEEK_END);" is not required
		//	because we opened the file in append mode.
		size_t st = fwrite (pData, 1, lToWrite, put->logfile.fLogFile);
		pData [lnData] = ASCII_NUL;
		return st == lnData;
	#endif
}

static bool cunilogProcessWriteToLogFileFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);

	CUNILOG_TARGET	*put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);
	ubf_assert (isUsableSMEMBUF (&put->mbLogfileName));

	if (cunilogHasDontWriteToLogfile (put))
		return true;

	/*
	CUNILOG_LOGFILE	*pl	= cup->pData;
	ubf_assert_non_NULL (pl);
	*/

	if (isUsableSMEMBUF (&put->mbLogfileName))
	{
		if (requiresOpenLogFile (put))
		{
			if (!cunilogOpenLogFile (put))
				cunilogSetTargetErrorAndInvokeErrorCallback (CUNILOG_ERROR_OPENING_LOGFILE, cup, pev);
		} else
		if (REQUIRES_NEW_LOGFILE (put, cup, pev))
		{
			if (!cunilogOpenNewLogFile (put))
				cunilogSetTargetErrorAndInvokeErrorCallback (CUNILOG_ERROR_OPENING_LOGFILE, cup, pev);
		}
		if (!cunilogWriteDataToLogFile (put))
				cunilogSetTargetErrorAndInvokeErrorCallback (CUNILOG_ERROR_WRITING_LOGFILE, cup, pev);
	}
	return true;
}

static bool cunilogProcessFlushLogFileFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pCUNILOG_TARGET);
	CUNILOG_TARGET	*put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);
	
	if (cunilogHasDontWriteToLogfile (put))
		return true;

	#ifdef OS_IS_WINDOWS
		if (!FlushFileBuffers (put->logfile.hLogFile))
			cunilogSetTargetErrorAndInvokeErrorCallback (CUNILOG_ERROR_FLUSHING_LOGFILE, cup, pev);
	#else
		if (0 != fflush (put->logfile.fLogFile))
			cunilogInvokeErrorCallback (CUNILOG_ERROR_FLUSHING_LOGFILE, cup, pev);
	#endif
	return true;
}

static bool cunilogProcessEventSingleThreaded (CUNILOG_EVENT *pev);
static bool enqueueAndTriggerSeparateLoggingThread (CUNILOG_EVENT *pev);

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY

	static inline void IncrementPendingNoRotationEvents (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		++ put->nPendingNoRotEvts;
		//printf ("%" PRIu64 "\n", put->nPendingNoRotEvts);
	}
	static inline void DecrementPendingNoRotationEvents (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		-- put->nPendingNoRotEvts;
		//printf ("%" PRIu64 "\n", put->nPendingNoRotEvts);
	}
#else
	#define IncrementPendingNoRotationEvents(put)
	#define DecrementPendingNoRotationEvents(put)
#endif

static bool logFromInsideRotatorTextU8fmt (CUNILOG_TARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	size_t		len;
	bool		bRet = false;

	// We're supposed to be inside a rotator. This means we should have a
	//	CUNILOG_ROTATOR_ARGS structure set in our target. It also means
	//	we should have a cunilog processor. Its pData member points to a
	//	rotation data structure. Anything else is a bug.
	ubf_assert_non_NULL (put->prargs);
	ubf_assert_non_NULL (put->prargs->cup);
	ubf_assert_non_NULL (put->prargs->cup->pData);

	CUNILOG_ROTATION_DATA	*prd = put->prargs->cup->pData;
	ubf_assert_non_NULL (prd);

	/*
		prd->plogCUNILOG_TARGET == NULL		-> Log to current CUNILOG_TARGET (put) but
												without rotation (because we currently *are*
												within a rotation).
		prd->plogCUNILOG_TARGET == put		-> Bug, as this should be NULL.
		prd->plogCUNILOG_TARGET == <Anything else>
											-> Log to this CUNILOG_TARGET normally.
	*/
	ubf_assert (put != prd->plogCUNILOG_TARGET);
	if (prd->plogCUNILOG_TARGET && put != prd->plogCUNILOG_TARGET)
	{
		va_start (ap, fmt);
		bRet = logTextU8vfmt (put, fmt, ap);
		va_end (ap);
		return bRet;
	}

	va_start (ap, fmt);
	len = (size_t) vsnprintf (NULL, 0, fmt, ap);
	va_end (ap);

	char *szTxtToLog = ubf_malloc (len + 1);
	if (szTxtToLog)
	{
		va_start (ap, fmt);
		vsnprintf (szTxtToLog, len + 1, fmt, ap);
		va_end (ap);

		CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Text (put, cunilogEvtSeverityNone, szTxtToLog, len);
		ubf_assert_non_NULL (pev);
		if (pev)
		{
			cunilogSetEventInternal		(pev);
			cunilogSetEventNoRotation	(pev);
			#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
				if (HAS_CUNILOG_TARGET_A_QUEUE (put))
				{
					IncrementPendingNoRotationEvents (put);
					enqueueAndTriggerSeparateLoggingThread (pev);
					bRet = true;
				} else
			#endif
					bRet = cunilogProcessEventSingleThreaded (pev);
		}
		ubf_free (szTxtToLog);
	}
	return bRet;
}

#ifdef OS_IS_WINDOWS
	static DWORD GetTextForLastError (char *szErrMsg)
	{
		ubf_assert_non_NULL (szErrMsg);

		// Note that we expect a buffer of CUNILOG_STD_MSG_SIZE octets.
		DWORD dwError = GetLastError ();
		GetWinErrorTextU8 (szErrMsg, CUNILOG_STD_MSG_SIZE, dwError);
		return dwError;
	}
#else
	static int GetTextForLastError (char *szErrMsg)
	{
		ubf_assert_non_NULL (szErrMsg);

		char szErr [] = "\"Not implemented\"";
		memcpy (szErrMsg, szErr, sizeof (szErr));
		return -1;
	}
#endif

/*
	Increments the dot number part of a filename, i.e. ".1234".
	Can also be "", which is the case for the original file.
	Assumes that there's always enough space available to write out
	the incremented value.

	Like:
	"file.log"		-> ""
	"file.log.1"	-> ".1"
	"file.log.2"	-> ".2"
	"file.log.900"	-> ".900"

	Returns the new length of the dot number part, including the dot.
*/
static inline size_t incrementDotNumberName (char *sz)
{
	ubf_assert_non_NULL	(sz);
	ubf_assert_non_0	(sz [0]);

	if ('.' == sz [0])
	{
		++ sz;
		uint64_t	ui = 0;;
		ubf_uint64_from_str (&ui, sz);
		++ ui;
		size_t written = ubf_str_from_uint64 (sz, ui);
		++ written;											// The '.'
		return written;
	} else
	{
		++ sz;
		memcpy (sz, ".1", 3);
		return 2;
	}
}

static inline char *findDotNumberPart (size_t *pln, CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (pln);
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->prargs);

	CUNILOG_ROTATION_DATA	*prd = put->prargs->cup->pData;

	// Obtain the dot number part.
	size_t ln = put->stFilToRotate - 1;
	-- ln;
	while (ln)
	{
		if (!isdigit (prd->mbDstFile.buf.pch [ln]))
			break;
		-- ln;
	}
	char *sz = prd->mbDstFile.buf.pch + ln;
	*pln = ln;
	return sz;
}

/*
	Renames the DotNumberPostfix file in our files list (FLS) to keep the files list in sync
	with the real directory on disk.
*/
static inline void renameDotNumberPostfixInFLS (CUNILOG_TARGET *put, size_t nLen, size_t oLen)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->prargs);

	CUNILOG_ROTATOR_ARGS	*prg = put->prargs;
	CUNILOG_ROTATION_DATA	*prd = put->prargs->cup->pData;

	char		*szDst;
	const char	*ccSrc;

	if (nLen > oLen)
	{
		char	*sz	= put->fls.data [prg->idx].chFilename;
		size_t	ln	= put->fls.data [prg->idx].stFilename - oLen + nLen;
		put->fls.data [prg->idx].chFilename = GetAlignedMemFromSBULKMEMgrow (&put->sbm, ln);
		char *szFls = put->fls.data [prg->idx].chFilename;
		if (NULL == szFls)
		{	// Bulk memory allocation failed. There's not much we can do.
			ubf_assert_msg (false, "Bulk memory allocation failed");
			return;
		}
		memcpy (szFls, sz, put->fls.data [prg->idx].stFilename);
		put->fls.data [prg->idx].stFilename = ln;

		// Since the new vector entry is empty, we also need to copy the NUL terminator.
		++ nLen;
	}

	szDst	=	put->fls.data [prg->idx].chFilename
			+	put->lnAppName
			+	lenCunilogLogFileNameExtension;

	ccSrc	=	prd->mbDstFile.buf.pcc
			+	put->lnLogPath
			+	put->lnAppName
			+	lenCunilogLogFileNameExtension;
	memcpy (szDst, ccSrc, nLen);
}

/*
	Renames the LogPostfix file in our files list (FLS) to keep the files list in sync
	with the real directory on disk.
*/
static inline void renameLogPostfixInFLS (CUNILOG_TARGET *put, const char *szNew, size_t lnNew)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->prargs);
	ubf_assert_non_NULL	(szNew);
	ubf_assert_non_0	(lnNew);
	ubf_assert			(strlen (szNew) == lnNew);

	CUNILOG_ROTATOR_ARGS	*prg	= put->prargs;
	size_t					siz		= lnNew + 1;

	put->fls.data [prg->idx].chFilename = GetAlignedMemFromSBULKMEMgrow (&put->sbm, siz);
	if (put->fls.data [prg->idx].chFilename)
	{
		memcpy (put->fls.data [prg->idx].chFilename, szNew, siz);
		put->fls.data [prg->idx].stFilename = siz;
		return;
	}
	// Bulk memory allocation failed. There's not much we can do.
	ubf_assert_msg (false, "Bulk memory allocation failed");
	put->fls.data [prg->idx].stFilename = 0;
}

/*
	The next rotator would have to read the directory listing from disk again,
	but we may not have logfile.log again yet, if no event has been written to it,
	which would mean the amount of files to ignore is one too high.

	We therefore need to insert the current/active logfile's name.
	Since the vector is sorted with the current logfile at index 0, vec_insert() is
	invoked instead of vec_push().

					-> logfile.log		<- Inserted by vec_insert() below.
	logfile.log		-> logfile.log.1
	logfile.log.1	-> logfile.log.2

	i == false	:	Insert at the beginning/top.
	i == true	:	Push to the end.

*/
static inline void cunilogAddActiveLogfile (bool bIsActiveLogfile, bool i, CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	if (bIsActiveLogfile)
	{
		CUNILOG_FLS	currFls;
		currFls.stFilename = put->lnAppName + sizCunilogLogFileNameExtension;
		currFls.chFilename = GetAlignedMemFromSBULKMEMgrow (&put->sbm, currFls.stFilename);
		if (currFls.chFilename)
		{
			memcpy (currFls.chFilename, put->mbAppName.buf.pcc, put->lnAppName);
			memcpy (currFls.chFilename + put->lnAppName, szCunilogLogFileNameExtension,
						sizCunilogLogFileNameExtension);
			if (i)
			{
				if (0 == vec_push (&put->fls, currFls))
					return;
			} else
			{
				if (0 == vec_insert (&put->fls, 0, currFls))
					return;
			}
		}
		SetCunilogSystemError (put, CUNILOG_ERROR_HEAP_ALLOCATION);
	}
}

static inline bool prepareDotNumberPostfixFileToRename	(
						CUNILOG_TARGET				*put,
						size_t						*poldLength,
						size_t						*pnewLength
												)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->prargs);
	ubf_assert_non_NULL	(put->prargs->cup);
	ubf_assert_non_NULL (poldLength);
	ubf_assert_non_NULL (pnewLength);
	
	CUNILOG_ROTATION_DATA	*prd = put->prargs->cup->pData;
	bool					bIsActiveLogfile;

	memcpy (prd->mbDstFile.buf.pch, put->mbFilToRotate.buf.pch, put->stFilToRotate);
	size_t ln;
	char *sz = findDotNumberPart (&ln, put);
	//printf ("Dot number part: >%s<\n", sz);
	// Either ".log.<number>" orr "g" from ".log".
	ubf_assert ('.' == sz [0] || 'g' == sz [0]);
	bIsActiveLogfile = 'g' == sz [0];
	*poldLength = put->stFilToRotate - 1 - ln;
	*pnewLength = incrementDotNumberName (sz);
	return bIsActiveLogfile;
}

static inline bool prepareLogPostfixFileToRename	(
						CUNILOG_TARGET				*put,
						char						**pszNewFileName,
						size_t						*poldLength,
						size_t						*pnewLength
													)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->prargs);
	ubf_assert_non_NULL	(put->prargs->cup);
	ubf_assert_non_NULL	(pszNewFileName);
	ubf_assert_non_NULL (poldLength);
	ubf_assert_non_NULL (pnewLength);
	
	UNUSED (poldLength);

	CUNILOG_ROTATION_DATA	*prd = put->prargs->cup->pData;

	ubf_assert	(put->prargs->siz == put->lnAppName + sizCunilogLogFileNameExtension);
	ubf_assert	(!memcmp (put->prargs->nam, put->mbAppName.buf.pcc, put->lnAppName));
	ubf_assert	(
		!memcmp (put->prargs->nam + put->lnAppName,
		szCunilogLogFileNameExtension,
		lenCunilogLogFileNameExtension)
				);
	char *szWrite = prd->mbDstFile.buf.pch;
	memcpy (szWrite, put->mbFilToRotate.buf.pcc, put->lnLogPath);
	szWrite += put->lnLogPath;
	memcpy (szWrite, put->mbAppName.buf.pcc, put->lnAppName);
	*pszNewFileName = szWrite;
	szWrite += put->lnAppName;
	*szWrite ++ = '_';
	obtainDateAndTimeStamp (szWrite, put->prargs->cup->cur, put->culogPostfix);
	szWrite += lenDateTimeStampFromPostfix (put->culogPostfix);
	memcpy (szWrite, szCunilogLogFileNameExtension, sizCunilogLogFileNameExtension);
	*pnewLength =		put->lnAppName
				+	1								// Underscore '_'.
				+	lenDateTimeStampFromPostfix (put->culogPostfix)
				+	lenCunilogLogFileNameExtension;
	return true;
}

static void cunilogRenameLogfile (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->prargs);
	ubf_assert_non_NULL	(put->prargs->cup);
	ubf_assert_non_NULL	(put->prargs->cup->pData);
	ubf_assert			(isInitialisedSMEMBUF (&put->mbFilToRotate));
	ubf_assert_non_0	(put->stFilToRotate);
	ubf_assert_non_0	(1 < put->stFilToRotate);
	ubf_assert			(hasDotNumberPostfix (put) || hasLogPostfix (put));

	CUNILOG_ROTATION_DATA	*prd = put->prargs->cup->pData;
	bool					bIsActiveLogfile;
	size_t					oldLen			= 0;
	size_t					newLen;
	char					*szNewFileName		= NULL;

	if (!cunilogHasRotatorFlag_USE_MBDSTFILE (prd))
	{														// The + 1 is for an underscore.
		initSMEMBUFtoSize	(
			&prd->mbDstFile,
			put->stFilToRotate + lenDateTimeStampFromPostfix (put->culogPostfix) + 1
							);
		cunilogSetRotatorFlag_USE_MBDSTFILE (prd);
	}
	growToSizeSMEMBUF	(
		&prd->mbDstFile,
		put->stFilToRotate + lenDateTimeStampFromPostfix (put->culogPostfix) + 1
						);
	if (isUsableSMEMBUF (&prd->mbDstFile))
	{
		if (hasDotNumberPostfix (put))
		{
			bIsActiveLogfile = prepareDotNumberPostfixFileToRename (put, &oldLen, &newLen);
		} else
		if (hasLogPostfix (put))
		{
			// We only have to deal with the current/active logfile here. All other file names
			//	stay the same throughout their existence.
			if (put->prargs->siz != put->lnAppName + sizCunilogLogFileNameExtension)
				return;
			bIsActiveLogfile = prepareLogPostfixFileToRename (put, &szNewFileName, &oldLen, &newLen);
		} else
		{
			ubf_assert_msg (false, "Bug!");
			return;
		}

		bool bMoved;

		#ifdef PLATFORM_IS_WINDOWS
			if (bIsActiveLogfile)
				cunilogCloseCUNILOG_LOGFILEifOpen (put);
			bMoved = MoveFileU8long (put->mbFilToRotate.buf.pch, prd->mbDstFile.buf.pcc);
			if (bIsActiveLogfile && requiresOpenLogFile (put))
			{
				if (!cunilogOpenLogFile (put))
				{
					SetCunilogSystemError (put, CUNILOG_ERROR_OPENING_LOGFILE);
					cunilogSetTargetErrorAndInvokeErrorCallback	(
						CUNILOG_ERROR_OPENING_LOGFILE,
						put->prargs->cup, put->prargs->pev
																);
				}
			}
			if (bMoved)
			{
				if (hasDotNumberPostfix (put))
				{
					renameDotNumberPostfixInFLS (put, newLen, oldLen);
					cunilogAddActiveLogfile (bIsActiveLogfile, false, put);
				} else
				if (hasLogPostfix (put))
				{
					renameLogPostfixInFLS (put, szNewFileName, newLen);
					cunilogAddActiveLogfile (bIsActiveLogfile, true, put);
				}
				logFromInsideRotatorTextU8fmt	(
					put, "File \"%s\" moved/renamed to \"%s\".",
					put->mbFilToRotate.buf.pcc,
					prd->mbDstFile.buf.pcc
												);
			} else
			{
				char szErr [CUNILOG_STD_MSG_SIZE];
				DWORD dwErr = GetTextForLastError (szErr);

				logFromInsideRotatorTextU8fmt	(
					put,
					"Error %s while attempting to move file \"%s\" to \"%s\".",
					szErr, put->mbFilToRotate.buf.pcc, prd->mbDstFile.buf.pcc
												);
				SetCunilogError (put, CUNILOG_ERROR_RENAMING_LOGFILE, dwErr);
			}
		#else
			bMoved = false;
		#endif
	}
}

static void cunilogFileSystemCompressLogfile (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	bool b;
	char szErr [CUNILOG_STD_MSG_SIZE];
	
	enfilecompressresult	cmprRes = IsFileCompressedByName (put->mbFilToRotate.buf.pch);
	switch (cmprRes)
	{
		case fscompress_uncompressed:
			logFromInsideRotatorTextU8fmt	(
				put, "Initiating file system compression for file \"%s\"...",
				put->mbFilToRotate.buf.pcc
											);
			b = FScompressFileByName (put->mbFilToRotate.buf.pcc);
			if (b)
			{
				logFromInsideRotatorTextU8fmt	(
					put, "File system compression for file \"%s\" initiated.",
				put->mbFilToRotate.buf.pcc
												);
			} else
			{
				GetTextForLastError (szErr);
				logFromInsideRotatorTextU8fmt	(
					put,
					"Error %s while attempting to initiate file system compression for file \"%s\".",
					szErr, put->mbFilToRotate.buf.pcc
										);
			}
			break;
		case fscompress_compressed:
			break;
		case fscompress_error:
			GetTextForLastError (szErr);
			logFromInsideRotatorTextU8fmt	(
				put,
				"Error %s while attempting to check file system compression for file \"%s\".",
				szErr, put->mbFilToRotate.buf.pcc
									);
			break;
	}
}

#if defined (OS_IS_WINDOWS) && defined (HAVE_SHELLAPI)

	static void MoveFileToRecycleBinWin (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert_non_NULL	(put->prargs);
		ubf_assert_non_NULL	(put->prargs->cup);
		ubf_assert_non_NULL	(put->prargs->cup->pData);

		logFromInsideRotatorTextU8fmt	(
			put, "Moving obsolete logfile \"%s\" to recycle bin...", put->mbFilToRotate.buf.pcc
										);
		bool b = MoveToRecycleBinU8 (put->mbFilToRotate.buf.pcc);
		if (b)
		{
			logFromInsideRotatorTextU8fmt	(
				put, "Obsolete logfile \"%s\" moved to recycle bin.", put->mbFilToRotate.buf.pcc
											);
			vec_splice (&put->fls, put->prargs->idx, 1);
		} else
		{
			char szErr [CUNILOG_STD_MSG_SIZE];
			GetTextForLastError (szErr);
			logFromInsideRotatorTextU8fmt	(
				put,
				"Error %s while attempting to move obsolete logfile \"%s\" to recycle bin.", szErr, put->mbFilToRotate.buf.pcc
											);
		}
	}

#elif OS_IS_MACOS

	#include "./../OS/Apple/TrashCan.h"

	// Our wrapper.
	static void MoveFileToRecycleBinMac (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert_non_NULL	(put->prargs);
		ubf_assert_non_NULL	(put->prargs->cup);
		ubf_assert_non_NULL	(put->prargs->cup->pData);

		logFromInsideRotatorTextU8fmt (put, "Moving obsolete logfile \"%s\" to recycle bin...\n", put->mbFilToRotate.buf.pcc);
		if (LIBTRASHCAN_SUCCESS == trashcan_soft_delete_apple (put->mbFilToRotate.buf.pcc))
		{
			logFromInsideRotatorTextU8fmt (put, "Obsolete logfile \"%s\" moved to recycle bin.\n", put->mbFilToRotate.buf.pcc);
			vec_splice (&put->fls, put->prargs->idx, 1);
		} else
		{
			logFromInsideRotatorTextU8fmt (put, "Error while attempting to move obsolete logfile \"%s\" to recycle bin.\n", put->mbFilToRotate.buf.pcc);
		}
	}

#elif defined (OS_IS_LINUX)

	static void MoveFileToRecycleBinLnx (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert_non_NULL	(put->prargs);
		ubf_assert_non_NULL	(put->prargs->cup);
		ubf_assert_non_NULL	(put->prargs->cup->pData);

		logFromInsideRotatorTextU8fmt (put, "Moving obsolete logfile \"%s\" to recycle bin...\n", put->mbFilToRotate.buf.pcc);
		bool b = MoveFileToTrashPOSIX (put->mbFilToRotate.buf.pcc);
		if (b)
		{
			logFromInsideRotatorTextU8fmt (put, "Obsolete logfile \"%s\" moved to recycle bin.\n", put->mbFilToRotate.buf.pcc);
			vec_splice (&put->fls, put->prargs->idx, 1);
		} else
		{
			char szErr [CUNILOG_STD_MSG_SIZE];
			GetTextForLastError (szErr);
			logFromInsideRotatorTextU8fmt (put, "Error %s while attempting to move obsolete logfile \"%s\" to recycle bin.\n", szErr, put->mbFilToRotate.buf.pcc);
		}
	}

#else

	#error To do!!!

#endif

/*
	Platform-independent wrapper function.
*/
static inline void cunilogMoveFileToRecycleBin (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	#if defined (OS_IS_WINDOWS) && defined (HAVE_SHELLAPI)
		MoveFileToRecycleBinWin (put);
	#elif defined (OS_IS_MACOS)
		MoveFileToRecycleBinMac (put);
	#elif defined (OS_IS_LINUX)
		MoveFileToRecycleBinLnx (put);
	#else
		#error Not implemented yet!
	#endif
}

#ifdef PLATFORM_IS_WINDOWS
	static void cunilogDeleteObsoleteLogfile (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert_non_NULL	(put->prargs);
		ubf_assert_non_NULL	(put->prargs->cup);
		ubf_assert_non_NULL	(put->prargs->cup->pData);
	
		logFromInsideRotatorTextU8fmt (put, "Deleting obsolete logfile \"%s\"...\n", put->mbFilToRotate.buf.pch);
		bool b = DeleteFileU8 (put->mbFilToRotate.buf.pch);
		if (b)
		{
			logFromInsideRotatorTextU8fmt (put, "Obsolete logfile \"%s\" deleted.\n", put->mbFilToRotate.buf.pch);
			vec_splice (&put->fls, put->prargs->idx, 1);
		} else
		{
			char szErr [CUNILOG_STD_MSG_SIZE];
			GetTextForLastError (szErr);
			logFromInsideRotatorTextU8fmt (put, "Error %s while attempting to delete obsolete logfile \"%s\".\n", szErr, put->mbFilToRotate.buf.pch);
		}
	}
#endif

#ifdef PLATFORM_IS_POSIX
	static void DeleteObsoleteLogfile (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert_non_NULL	(put->prargs);
		ubf_assert_non_NULL	(put->prargs->cup);
		ubf_assert_non_NULL	(put->prargs->cup->pData);
	
		logFromInsideRotatorTextU8fmt (put, "Deleting obsolete logfile \"%s\"...\n", put->mbFilToRotate.buf.pch);
		int i = unlink (put->mbFilToRotate.buf.pch);
		if (0 == i)
		{
			logFromInsideRotatorTextU8fmt (put, "Obsolete logfile \"%s\" deleted.\n", put->mbFilToRotate.buf.pch);
		} else
		{
			logFromInsideRotatorTextU8fmt (put, "Error %d while attempting to delete obsolete logfile \"%s\".\n", errno, put->mbFilToRotate.buf.pch);
		}
	}
#endif

#ifdef CUNILOG_BUILD_DEBUG_OUTPUT_FILES_LIST
	static void DebugOutputFilesList (const char *szText, vec_cunilog_fls *pvec)
	{
		ubf_assert_non_NULL (pvec);

		// Output from index 0 up to length. This is different from how the files
		//	might be processed by a rotator.
		size_t iFiles = 0;
		if (szText)
			printf ("\nDebugOutputFilesList (%s): %zu file(s)\n", szText, pvec->length);
		else
			printf ("\nDebugOutputFilesList: %zu file(s)\n", pvec->length);
		while (iFiles < pvec->length)
		{
			puts (pvec->data [iFiles].chFilename);
			++ iFiles;
		}
		puts ("");
	}
#else
	#define DebugOutputFilesList(text, pvec)
#endif

static inline void performActualRotation (CUNILOG_ROTATOR_ARGS *prg)
{
	ubf_assert_non_NULL (prg);

	CUNILOG_PROCESSOR		*cup = prg->cup;
	ubf_assert_non_NULL (cup);
	CUNILOG_ROTATION_DATA	*prd = cup->pData;
	ubf_assert_non_NULL (prd);
	CUNILOG_EVENT			*pev = prg->pev;
	ubf_assert_non_NULL	(pev);
	CUNILOG_TARGET			*put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);

	// Now the target knows exactly which rotator it is working on.
	put->prargs = prg;

	switch (prd->tsk)
	{
		case cunilogrotationtask_None:
			// This is a bug. We're normally not called in this case. See function
			//	cunilogProcessRotateLogfiles ().
			ubf_assert (false);
			break;
		case cunilogrotationtask_RenameLogfiles:
			cunilogRenameLogfile (put);
			break;
		case cunilogrotationtask_FScompressLogfiles:
			cunilogFileSystemCompressLogfile (put);
			break;
		case cunilogrotationtask_MoveToRecycleBinLogfiles:
			cunilogMoveFileToRecycleBin (put);
			break;
		case cunilogrotationtask_DeleteLogfiles:
			cunilogDeleteObsoleteLogfile (put);
			break;
	}

	// We're not in a rotator anymore.
	put->prargs = NULL;
}

static inline void prepareU8fullFileNameToRotate (CUNILOG_TARGET *put, size_t idx)
{
	ubf_assert_non_NULL (put);

	char	*strNam = put->fls.data [idx].chFilename;
	size_t	sizName = put->fls.data [idx].stFilename;

	growToSizeSMEMBUF (&put->mbFilToRotate, put->lnLogPath + sizName);
	if (isUsableSMEMBUF (&put->mbFilToRotate))
	{
		memcpy (put->mbFilToRotate.buf.pch + put->lnLogPath, strNam, sizName);
		put->stFilToRotate = put->lnLogPath + sizName;
	}
}

static inline int cmpflsnums (const char *sz1, size_t l1, const char *sz2, size_t l2)
{
	const char *szo1 = sz1;
	const char *szo2 = sz2;

	while (szo1 [0] == szo2 [0])
	{
		++ szo1;
		-- l1;
		++ szo2;
		-- l2;
	}
	if (l1 || l2)
	{
		// The shorter the greater, i.e. "file.9" is greater than "file.10".
		if (l1 < l2)
			return -1;
		if (l1 > l2)
			return 1;
		if (isdigit (szo1 [0]) && isdigit (szo2 [0]))
		{
			uint64_t	ui1;
			uint64_t	ui2;

			//printf ("%s - %s\n", szo1, szo2);
			ubf_uint64_from_str (&ui1, szo1);
			ubf_uint64_from_str (&ui2, szo2);
			if (ui1 == ui2)
				return 0;
			// "file.1" is greater than "file.2".
			return ui1 > ui2 ? 1 : -1;
		}
	}
	return 0;
}

/*
	Compare functions for vec_sort ().
*/
static int flscmp_dotnum (const void *p1, const void *p2)
{
	const CUNILOG_FLS	*fls1 = p1;
	const CUNILOG_FLS	*fls2 = p2;
	int					r = 0;

	ubf_assert_non_0 (fls1->stFilename);
	ubf_assert_non_0 (fls2->stFilename);

	r = cmpflsnums (fls1->chFilename, fls1->stFilename - 1, fls2->chFilename, fls2->stFilename - 1);
	return r;
}

static int flscmp_default (const void *p1, const void *p2)
{
	const CUNILOG_FLS	*fls1 = p1;
	const CUNILOG_FLS	*fls2 = p2;
	int					r = 0;

	ubf_assert_non_0 (fls1->stFilename);
	ubf_assert_non_0 (fls2->stFilename);

	if (fls1->stFilename == fls2->stFilename)
	{
		r = memcmp (fls1->chFilename, fls2->chFilename, fls1->stFilename - 1);
		return r;
	}
	if (fls1->stFilename < fls2->stFilename)
	{	
		r = memcmp (fls1->chFilename, fls2->chFilename, fls1->stFilename - 1);
		return r ? r : -1;
	}
	if (fls1->stFilename > fls2->stFilename)
	{
		r = memcmp (fls1->chFilename, fls2->chFilename, fls2->stFilename - 1);
		return r ? r : 1;
	}
	return r;
}

static inline bool needReverseFLS (CUNILOG_TARGET *put, CUNILOG_PROCESSOR *cup)
{
	ubf_assert_non_NULL (put);
	ubf_assert_non_NULL (cup);

	CUNILOG_ROTATION_DATA	*prd = cup->pData;
	
	bool bRet;
	bRet = hasDotNumberPostfix (put) && cunilogrotationtask_RenameLogfiles != prd->tsk;
	return bRet;
}

/*
	On NTFS, files are returned in descending alphabetical order.

	This means it is enough to just reverse the order, i.e. start with the last
	file first and walk downwards, and this is what prapareLogfilesListAndRotate ()
	does.
	
	Other file systems may also return files in descending alphabetical order,
	but on file systems that return the files randomly we got to sort the vector
	first. If sorting descending alphabetically is required, we expect the target option
	flag CUNILOGTARGET_FS_NEEDS_SORTING.

	For the dot number postfix we need to sort in any case.
*/
static inline void sortLogfilesList (CUNILOG_TARGET *put, CUNILOG_PROCESSOR *cup)
{
	/*
	CUNILOG_ROTATION_DATA	*prd = cup->pData;

	size_t iFiles = put->fls.length;
	printf ("\nList sort in: %d\n", prd->tsk);
	while (iFiles --)
	{
		puts (put->fls.data [iFiles].chFilename);
	}
	*/

	if (cunilogTargetHasFSneedsSorting (put))
	{
		if (!cunilogTargetHasFLSisSorted (put))
		{
			if (hasDotNumberPostfix (put))
				vec_sort (&put->fls, flscmp_dotnum);
			else
				vec_sort (&put->fls, flscmp_default);
			cunilogTargetSetFLSisSorted (put);
		}
	}

	// The processor may need the files in reverse order.
	bool bFLSreversedRequired = needReverseFLS (put, cup);
	if (bFLSreversedRequired)
	{
		if (!cunilogTargetHasFLSreversed (put))
		{
			vec_reverse (&put->fls);
			cunilogTargetSetFLSreversed (put);
		}
	} else
	{
		if (cunilogTargetHasFLSreversed (put))
		{
			vec_reverse (&put->fls);
			cunilogTargetClrFLSreversed (put);
		}
	}

	/*
	printf ("\nList sort out: %d\n", prd->tsk);
	iFiles = put->fls.length;
	while (iFiles --)
	{
		puts (put->fls.data [iFiles].chFilename);
	}
	;
	*/
}

static void prapareLogfilesListAndRotate (CUNILOG_ROTATOR_ARGS *prg)
{
	ubf_assert_non_NULL (prg);
	CUNILOG_EVENT			*pev = prg->pev;
	ubf_assert_non_NULL (pev);
	CUNILOG_TARGET			*put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);
	CUNILOG_PROCESSOR		*cup = prg->cup;
	ubf_assert_non_NULL (cup);
	CUNILOG_ROTATION_DATA	*prd = cup->pData;
	ubf_assert_non_NULL (prd);


	//DebugOutputFilesList ("List in", cup, pev);
	sortLogfilesList (put, cup);

	uint64_t nToIgnore = prd->nIgnore + put->scuNPI.nIgnoredTotal;
	uint64_t nMaxToRot;

	if (cunilogrotationtask_RenameLogfiles == prd->tsk && hasLogPostfix (put))
	{	// This should all have been taken care of before we're called.
		ubf_assert (cunilogProcessRotateLogfiles == cup->task);
		ubf_assert (hasLogPostfix (put));

		nMaxToRot = 1;										// Only "<appname>.log" needs rotation.
	} else
	{
		nMaxToRot =	CUNILOG_MAX_ROTATE_AUTO - nToIgnore <= prd->nMaxToRotate
					?	prd->nMaxToRotate
					:	nToIgnore + prd->nMaxToRotate;
	}
	DebugOutputFilesList ("List out", &put->fls);

	size_t iFiles = put->fls.length;
	while (iFiles --)
	{
		ubf_assert (iFiles < put->fls.length);
		//puts (put->fls.data [iFiles].chFilename);
		++ prd->nCnt;
		if (prd->nCnt > nToIgnore)
		{
			if (prd->nCnt <= nMaxToRot)
			{
				/*
				DebugOutputFilesList ("List", &put->fls);
				puts ("File:");
				puts (put->fls.data [iFiles].chFilename);
				*/
				prepareU8fullFileNameToRotate (put, iFiles);
				//puts (put->fls.data [iFiles].chFilename);

				prg->nam = put->fls.data [iFiles].chFilename;
				prg->siz = put->fls.data [iFiles].stFilename;
				prg->idx = iFiles;
				performActualRotation (prg);
			} else
				break;
		} else
		{	// This tells the next rotation processor how many we've ignored already.
			++ put->scuNPI.nIgnoredTotal;
		}
	}
	DebugOutputFilesList ("List out", &put->fls);
}

/*
	Returns true if the logfile name ends with ".log" or ".log.<number>".
*/
static inline bool endsLogFileNameWithDotNumber (CUNILOG_FLS *pfls)
{
	ubf_assert_non_NULL	(pfls);
	ubf_assert_non_NULL	(pfls->chFilename);
	ubf_assert_non_0	(pfls->stFilename);

	size_t o = pfls->stFilename;
	size_t d = 0;

	if (o > lenCunilogLogFileNameExtension + 1)
	{
		o -= 2;												// NULL and 0 index.
		while (o && isdigit (pfls->chFilename [o]))
		{
			-- o;
			++ d;
		}
		if (d && o && '.' == pfls->chFilename [o])
		{
			-- o;
			++ d;
		}
		char *szfn = pfls->chFilename + pfls->stFilename - sizCunilogLogFileNameExtension - d;
		if (o > lenCunilogLogFileNameExtension)
		{
			return	0 == memcmp	(
							szCunilogLogFileNameExtension,
							szfn,
							lenCunilogLogFileNameExtension
								);
		}
	}
	return false;
}

#if defined (PLATFORM_IS_WINDOWS)
	static bool obtainLogfilesListToRotateCallbackWin (SRDIRONEENTRYSTRUCT *psdE)
	{
		ubf_assert_non_NULL (psdE);
		CUNILOG_TARGET			*put = psdE->pCustom;
		ubf_assert_non_NULL (put);

		CUNILOG_FLS	fls;
		fls.stFilename = psdE->stFileNameU8;
		fls.chFilename = psdE->szFileNameU8;
		if (hasDotNumberPostfix (put) && !endsLogFileNameWithDotNumber (&fls))
			return true;

		fls.chFilename = GetAlignedMemFromSBULKMEMgrow (&put->sbm, fls.stFilename);
		ubf_assert_non_NULL (fls.chFilename);
		if (fls.chFilename)
		{
			memcpy (fls.chFilename, psdE->szFileNameU8, fls.stFilename);
			if (0 == put->fls.capacity)
			{
				if (-1 == vec_reserve (&put->fls, CUNILOG_STD_VECT_EXP_SIZE))
					return false;
			}
			vec_push (&put->fls, fls);
		}
		return true;
	}
#elif defined (PLATFORM_IS_POSIX)
	static bool obtainLogfilesListToRotateCallbackPsx (SONEDIRENT *pod)
	{
		ubf_assert_non_NULL (pod);
		CUNILOG_TARGET			*put = pod->pCustom;
		ubf_assert_non_NULL (put);

		CUNILOG_FLS fls;
		fls.stFilename = strlen (pod->dirEnt->d_name) + 1;
		if	(
				globMatch	(
					pod->dirEnt->d_name, fls.stFilename - 1,
					put->mbLogFileMask.buf.pcc, put->lnLogFileMask
							)
			)
		{
			fls.chFilename = GetAlignedMemFromSBULKMEMgrow (&put->sbm, fls.stFilename);
			ubf_assert_non_NULL (fls.chFilename);
			if (fls.chFilename)
			{
				memcpy (fls.chFilename, pod->dirEnt->d_name, fls.stFilename);
				if (0 == put->fls.capacity)
				{
					if (-1 == vec_reserve (&put->fls, CUNILOG_STD_VECT_EXP_SIZE))
						return false;
				}
				vec_push (&put->fls, fls);
			}
		}
		return true;
	}
#endif

#if defined (PLATFORM_IS_WINDOWS)
	static void obtainLogfilesListToRotateWin (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(strlen (put->mbLogPath.buf.pcc)		== put->lnLogPath);
		ubf_assert			(strlen (put->mbLogFileMask.buf.pcc)	== put->lnLogFileMask);

		uint64_t		n;
		/*
		n = ForEachDirectoryEntryU8	(
				put->mbLogFileMask.buf.pcc,					// Search mask.
				obtainLogfilesListToRotateCallbackWin, put, NULL
									);
		*/
		n = ForEachDirectoryEntryMaskU8	(
				put->mbLogPath.buf.pcc,			put->lnLogPath,
				put->mbLogFileMask.buf.pcc,		put->lnLogFileMask,
				obtainLogfilesListToRotateCallbackWin,
				put,
				NULL
										);
		UNUSED (n);
	}
#elif defined (PLATFORM_IS_POSIX)
	static void obtainLogfilesListToRotatePsx (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(strlen (put->mbLogPath.buf.pcc)		== put->lnLogPath);
		ubf_assert			(strlen (put->mbLogFileMask.buf.pcc)	== put->lnLogFileMask);

		uint64_t n;
		n = ForEachPsxDirEntry	(
				put->mbLogFold.buf.pcc,						// Directory name.
				obtainLogfilesListToRotateCallbackPsx, put, NULL
								);
		UNUSED (n);
	}
#endif

static inline void obtainLogfilesListToRotate (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	if (0 == put->fls.length)
	{
		#if defined (PLATFORM_IS_WINDOWS)
			obtainLogfilesListToRotateWin (put);
		#elif defined (PLATFORM_IS_POSIX)
			obtainLogfilesListToRotatePsx (put);
		#endif
		if (hasLogPostfix (put))
		{	// Our current logfile is missing because the search mask wouldn't
			//	pick it up. For hasDotNumberPostfix () the current logfile is
			//	included, as well as for all other postfixes.
			cunilogAddActiveLogfile (true, true, put);
			//DebugOutputFilesList ("obtain", &put->fls);
		}
		cunilogTargetClrFLSreversed (put);
	}
}

static inline void cunilogResetFilesList (CUNILOG_TARGET *put)
{
	ubf_assert_non_NULL (put);

	vec_clear (&put->fls);
	EmptySBULKMEM (&put->sbm);
	cunilogTargetClrFLSisSorted (put);
}

static bool cunilogProcessRotateLogfilesFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (cup);
	ubf_assert_non_NULL (pev);
	
	if (cunilogHasEventNoRotation (pev))
		return true;

	CUNILOG_TARGET			*put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);
	CUNILOG_ROTATION_DATA	*prd = cup->pData;
	ubf_assert_non_NULL (prd);

	prd->nCnt = 0;

	CUNILOG_ROTATOR_ARGS	args;
	args.cup = cup;
	args.pev = pev;

	cunilogTestErrorCB (CUNILOG_ERROR_TEST_BEFORE_ROTATOR, cup, pev);

	obtainLogfilesListToRotate		(put);
	DebugOutputFilesList ("cunilogProcessRotateLogfilesFnct", &put->fls);

	switch (prd->tsk)
	{
		case cunilogrotationtask_None:
			break;
		case cunilogrotationtask_RenameLogfiles:
			if (!hasDotNumberPostfix (put) && !hasLogPostfix (put))
				break;
		case cunilogrotationtask_FScompressLogfiles:
		case cunilogrotationtask_MoveToRecycleBinLogfiles:
		case cunilogrotationtask_DeleteLogfiles:
			prapareLogfilesListAndRotate (&args);
			break;
		default:
			ubf_assert_msg (false, "Internal error");
			break;
	}

	cunilogTestErrorCB (CUNILOG_ERROR_TEST_AFTER_ROTATOR, cup, pev);

	return true;
}

/*
	The custom processor. Its pData member is a pointer to a CUNILOG_CUSTPROCESS structure,
	which has two members: pData for custom data and the function pointer procFnc, which
	is the actual custom processor function.
*/
static bool cunilogProcessCustomProcessorFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (cup);
	ubf_assert_non_NULL (pev);
	
	#ifdef DEBUG
		CUNILOG_TARGET	*put = pev->pCUNILOG_TARGET;
		ubf_assert_non_NULL (put);
	#endif

	CUNILOG_CUSTPROCESS	*pcu = cup->pData;
	ubf_assert_non_NULL (pcu);
	ubf_assert_non_NULL (pcu->procFnct);
	return pcu->procFnct (cup, pev);
}

/*
	The redirector processor. The event is redirected to a different CUNILOG_TARGET,
	to which the pData member points to. When the event is redirected, the remaining
	processors of the current target are ignored.

	If pData is NULL, no redirection takes place and the remaining processors are worked
	through as usual. Since this is most likely not what the caller intended, a debug
	assertion expects pData not being NULL.
*/
static bool cunilogProcessTargetRedirectorFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (cup);
	ubf_assert_non_NULL (pev);

	CUNILOG_TARGET *put = cup->pData;
	ubf_assert_non_NULL (put);

	if (put)
	{
		logEv (put, pev);
		return false;
	}
	return true;
}

static bool cunilogProcessTargetForkFnct (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (cup);
	ubf_assert_non_NULL (pev);

	CUNILOG_TARGET *put = cup->pData;
	ubf_assert_non_NULL (put);

	if (put)
	{
		CUNILOG_EVENT *pnev = DuplicateCUNILOG_EVENT (pev);
		if (pnev)
		{
			logEv (put, pev);
		}
	}
	return true;
}

/*
static void cunilogProcessNotSupported (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);
	
	CUNILOG_TARGET	*pta = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (pta);

	ubf_assert_msg (false, "Not supported yet!");
}
*/

/*
	Queue functions.
*/

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline size_t nToTrigger (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (cunilogHasDebugQueueLocked (put));

		if (cunilogTargetHasIsPaused (put))
		{
			++ put->nPausedEvents;
			return 0;
		} else
		{	// Only the current event requires a trigger. The value of nPausedEvents
			//	should have been zeroed out by ResumeLogCUNILOG_TARGET ().
			ubf_assert_0 (put->nPausedEvents);
			return 1;
		}
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	// Returns how many times the semaphore must be triggered to empty the queue.
	static inline size_t EnqueueCUNILOG_EVENT (CUNILOG_EVENT *pev)
	{
		ubf_assert_non_NULL (pev);
		ubf_assert_non_NULL (pev->pCUNILOG_TARGET);

		CUNILOG_TARGET	*put = pev->pCUNILOG_TARGET;
		ubf_assert (HAS_CUNILOG_TARGET_A_QUEUE (put));

		size_t r;

		EnterCUNILOG_LOCKER (put);

		// In this case it's more of an enqueuing date/timestamp than a creation date/timestamp.
		if (0 == pev->stamp)
		{
			pev->stamp = LocalTime_UBF_TIMESTAMP ();
			ubf_assert_msg (false, "Actually not implemented yet");
		}
		ubf_assert_non_0 (pev->stamp);

		if (put->qu.first)
		{
			CUNILOG_EVENT *l	= put->qu.last;
			ubf_assert_non_NULL (l);
			ubf_assert_NULL (l->next);
			l->next				= pev;
			put->qu.last		= pev;
			put->qu.num			+= 1;
		} else
		{
			put->qu.first		= pev;
			put->qu.last		= pev;
			put->qu.num			= 1;
		}
		r = nToTrigger (put);
		LeaveCUNILOG_LOCKER (put);
		return r;
	}
#endif

/*
	We expect the caller to process the returned event only. However,
	for consistency, i.e. to hide the rest of the queue from the caller,
	the member ->next of the returned event is set to NULL.
	This is to ensure the caller won't try to iterate further to the next
	event accidentally.

	This function is currently not used because we always empty the entire
	queue with DequeueAllCUNILOG_EVENTs ().
*/
#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline CUNILOG_EVENT *DequeueCUNILOG_EVENT (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (HAS_CUNILOG_TARGET_A_QUEUE (put));

		CUNILOG_EVENT	*pev	= NULL;

		EnterCUNILOG_LOCKER (put);
		if (put->qu.first)
		{
			ubf_assert_non_0 (put->qu.num);

			pev				= put->qu.first;
			put->qu.first	= pev->next;
			pev->next		= NULL;
			put->qu.num		-= 1;
		} else
		{
			ubf_assert_0	(put->qu.num);
			ubf_assert_NULL	(put->qu.last);
		}
		LeaveCUNILOG_LOCKER (put);
		return pev;
	}
#endif

/*
	We expect the caller to process the returned event and all its successors
	until the end of the list. The member ->next of the last element of the
	singly-linked list of events is NULL.
*/
#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline CUNILOG_EVENT *DequeueAllCUNILOG_EVENTs (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (HAS_CUNILOG_TARGET_A_QUEUE (put));

		CUNILOG_EVENT	*pev	= NULL;
		CUNILOG_EVENT	*last;

		EnterCUNILOG_LOCKER (put);
		if (put->qu.first)
		{
			ubf_assert_non_0 (put->qu.num);

			pev				= put->qu.first;
			last			= put->qu.last;
			ubf_assert_NULL (last->next);

			// The queue is now empty. The caller is required to process
			//	all returned events.
			put->qu.first	= NULL;
			put->qu.last	= NULL;
			put->qu.num		= 0;
		} else
		{
			ubf_assert_0	(put->qu.num);
			ubf_assert_NULL	(put->qu.last);
		}
		LeaveCUNILOG_LOCKER (put);
		return pev;
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline void triggerCUNILOG_EVENTloggingThread (CUNILOG_TARGET *put, size_t releaseCount)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (HAS_CUNILOG_TARGET_A_QUEUE (put));
		ubf_assert (0 < releaseCount);						// Caller's responsibility.

		#ifdef OS_IS_WINDOWS
			LONG lPrevCount;
			LONG relCount = (long) releaseCount;
			bool b = ReleaseSemaphore (put->sm.hSemaphore, relCount, &lPrevCount);
			UNREFERENCED_PARAMETER (b);
			#ifdef DEBUG
				ubf_assert_true (b);
				if (false == b)
				{
					char szMsg [CUNILOG_STD_MSG_SIZE];
					GetTextForLastError (szMsg);
					szMsg [0] = szMsg [0];					// To have something to step to.
				}
			#endif
		#else
			int i;
			while (releaseCount)
			{
				i = sem_post (&put->sm.tSemaphore);
				ubf_assert (0 == i);
				-- releaseCount;
			}
			UNREFERENCED_PARAMETER (i);
		#endif
	}
#endif

/*
	The calling thread loops as long as we return true. The calling thread exits
	when we return false.
*/
#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static bool SepLogThreadWaitForEvents (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (HAS_CUNILOG_TARGET_A_QUEUE (put));

		#ifdef OS_IS_WINDOWS
			DWORD dw = WaitForSingleObject (put->sm.hSemaphore, INFINITE);
			ubf_assert (WAIT_OBJECT_0 == dw);
			return WAIT_OBJECT_0 == dw;
		#else
			int i = sem_wait (&put->sm.tSemaphore);
			ubf_assert (0 == i);
			return 0 == i;
		#endif
	}
#endif

/*
	The separate logging thread.
*/
#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	SEPARATE_LOGGING_THREAD_RETURN_TYPE SeparateLoggingThread (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (HAS_CUNILOG_TARGET_A_QUEUE (put));

		while (SepLogThreadWaitForEvents (put))
		{
			CUNILOG_EVENT *pev = DequeueAllCUNILOG_EVENTs (put);
			CUNILOG_EVENT *pnx;
			while (pev)
			{	// Remember the next pointer because pev is going to be destroyed
				//	by DoneSUNILOGEVENT ().
				pnx = pev->next;
				cunilogProcessEventSingleThreaded (pev);
				DoneCUNILOG_EVENT (put, pev);
				pev = pnx;
			}
			if (cunilogTargetHasShutdownInitiatedFlag (put) && 0 == put->nPendingNoRotEvts )
				goto ExitSeparateLoggingThread;
		}
	ExitSeparateLoggingThread:
		cunilogTargetSetShutdownCompleteFlag (put);
		return SEPARATE_LOGGING_THREAD_RETURN_SUCCESS;
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static bool StartSeparateLoggingThread_ifNeeded (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (requiresCUNILOG_TARGETseparateLoggingThread (put))
		{
			#ifdef OS_IS_WINDOWS
				HANDLE h = CreateThread (NULL, 0, SeparateLoggingThread, put, 0, NULL);
				ubf_assert_non_NULL (h);
				if (NULL == h)
					SetCunilogSystemError (put, CUNILOG_ERROR_SEPARATE_LOGGING_THREAD);
				put->th.hThread = h;
				return NULL != h;
			#else
				put->th.tThread = 0;
				int i = pthread_create (&put->th.tThread, NULL, (void * (*)(void *)) SeparateLoggingThread, put);
				ubf_assert_0 (i);
				if (0 != i)
					SetCunilogSystemError (put->error, CUNILOG_ERROR_SEPARATE_LOGGING_THREAD);
				return 0 == i;
			#endif
		}
		// If the separate logging thread is not required, this is considered success.
		return true;
	}
#else
	static bool StartSeparateLoggingThread_ifNeeded (CUNILOG_TARGET *put)
	{
		UNUSED (put);

		return true;
	}
#endif

static inline bool updateThresholdForAppliesToTimes (CUNILOG_PROCESSOR *cup)
{
	ubf_assert_non_NULL (cup);

	bool bRet = cup->thr && cup->cur > cup->thr;
	if (0 == cup->thr || cup->cur > cup->thr)
	{
		cup->thr = cup->cur;
	}
	return bRet;
}

static inline bool updateCurrentValueAndIsThresholdReached	(
								CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev
																	)
{
	ubf_assert_non_NULL (cup);
	ubf_assert_non_NULL (pev);
	ubf_assert (sizeof (uint64_t) == sizeof (UBF_TIMESTAMP));

	bool bRet = false;

	cunilogTestErrorCB (CUNILOG_ERROR_TEST_BEFORE_THRESHOLD_UPDATE, cup, pev);
	switch (cup->freq)
	{
		case cunilogProcessAppliesTo_nEvents:
			++ cup->cur;
			bRet = cup->cur >= cup->thr;
			cup->thr = bRet ? 0 : cup->cur;
			break;
		case cunilogProcessAppliesTo_nOctets:
			cup->cur += pev->lenDataToLog;
			bRet = cup->cur >= cup->thr;
			cup->thr = bRet ? 0 : cup->cur;
			break;
		case cunilogProcessAppliesTo_nAlways:
			return true;
		case cunilogProcessAppliesTo_SecondChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS;
			bRet = updateThresholdForAppliesToTimes (cup);
			break;
		case cunilogProcessAppliesTo_MinuteChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS;
			bRet = updateThresholdForAppliesToTimes (cup);
			break;
		case cunilogProcessAppliesTo_HourChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS;
			bRet = updateThresholdForAppliesToTimes (cup);
			break;
		case cunilogProcessAppliesTo_DayChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS;
			bRet = updateThresholdForAppliesToTimes (cup);
			break;
		case cunilogProcessAppliesTo_WeekChanged:
			cup->cur = (pev->stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS)
					 + 	GetISO8601WeekNumberFromDate (
							UBF_TIMESTAMP_YEAR	(pev->stamp),
							UBF_TIMESTAMP_MONTH	(pev->stamp),
							UBF_TIMESTAMP_DAY	(pev->stamp),
							NULL
													);
			bRet = updateThresholdForAppliesToTimes (cup);
			break;
		case cunilogProcessAppliesTo_MonthChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS;
			bRet = updateThresholdForAppliesToTimes (cup);
			break;
		case cunilogProcessAppliesTo_YearChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS;
			bRet = updateThresholdForAppliesToTimes (cup);
			break;
		case cunilogProcessAppliesTo_Auto:
			ubf_assert_msg (false, "Illegal value");
			bRet = true;
			break;
	}
	cunilogTestErrorCB (CUNILOG_ERROR_TEST_AFTER_THRESHOLD_UPDATE, cup, pev);

	// The flag OPT_CUNPROC_AT_STARTUP tells us to run the processor in any case.
	if (optCunProcHasOPT_CUNPROC_AT_STARTUP (cup->uiOpts))
	{
		optCunProcClrOPT_CUNPROC_AT_STARTUP (cup->uiOpts);
		return true;
	}

	// The same applies to the target.
	if (cunilogTargetHasRunProcessorsOnStartup (pev->pCUNILOG_TARGET))
		return true;

	return bRet;
}

static bool (*pickAndRunProcessor [cunilogProcessXAmountEnumValues]) (CUNILOG_PROCESSOR *cup, CUNILOG_EVENT *pev) =
{
	/* cunilogProcessNoOperation		*/		cunilogProcessNoneFnct
	/* cunilogProcessEchoToConsole		*/	,	cunilogProcessEchoFnct
	/* cunilogProcessUpdateLogFileName	*/	,	cunilogProcessUpdateLogFileNameFnct
	/* cunilogProcessWriteToLogFile		*/	,	cunilogProcessWriteToLogFileFnct
	/* cunilogProcessFlush				*/	,	cunilogProcessFlushLogFileFnct
	/* cunilogProcessRotateLogfiles		*/	,	cunilogProcessRotateLogfilesFnct
	/* cunilogProcessCustomProcessor	*/	,	cunilogProcessCustomProcessorFnct
	/* cunilogProcessTargetRedirector	*/	,	cunilogProcessTargetRedirectorFnct
	/* cunilogProcessTargetFork			*/	,	cunilogProcessTargetForkFnct
};

/*
	Returns true for further processing by the caller.
*/
static inline bool cunilogProcessProcessor (CUNILOG_EVENT *pev, CUNILOG_PROCESSOR *cup)
{
	ubf_assert_non_NULL	(pev);
	ubf_assert_non_NULL	(cup);
	ubf_assert_non_NULL	(pev->pCUNILOG_TARGET);
	ubf_assert			(cunilogIsTargetInitialised	(pev->pCUNILOG_TARGET));

	// If the processor is disabled we move on to the next one unconditionally.
	if (optCunProcHasOPT_CUNPROC_DISABLED (cup->uiOpts))
		return true;

	if (cunilogHasEventEchoOnly (pev) && cunilogProcessEchoToConsole != cup->task)
		return true;

	bool bRetProcessor = true;
	if	(updateCurrentValueAndIsThresholdReached (cup, pev))
	{
		// True tells the caller to carry on with the next processor.
		bRetProcessor = pickAndRunProcessor [cup->task] (cup, pev);
	}
	
	if (cunilogProcessEchoToConsole == cup->task && cunilogHasEventEchoOnly (pev))
		return false;
	// An error callback function told us to stop here and ignore the remaining processors.
	if (cunilogEventHasIgnoreRemainingProcessors (pev))
		return false;
	// The processor has the OPT_CUNPROC_FORCE_NEXT flag set.
	if (OPT_CUNPROC_FORCE_NEXT & cup->uiOpts)
		return true;
	if (cunilogTargetHasRunProcessorsOnStartup (pev->pCUNILOG_TARGET))
		return true;
	// Now the current processor decides if the next processor is allowed to run.
	 return bRetProcessor;
}

static void cunilogProcessProcessors (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pCUNILOG_TARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pCUNILOG_TARGET));
	ubf_assert_non_NULL						(pev->pCUNILOG_TARGET->cprocessors);

	CUNILOG_TARGET *put = pev->pCUNILOG_TARGET;
	if (!cunilogIsEventInternal (pev) && !cunilogHasEventNoRotation (pev))
	{
		pev->pCUNILOG_TARGET->scuNPI.nIgnoredTotal = 0;
		cunilogResetFilesList (put);
		cunilogEventClrIgnoreRemainingProcessors (pev);
	}

	CUNILOG_PROCESSOR *cup;
	unsigned int ui = 0;
	while (ui < pev->pCUNILOG_TARGET->nprocessors)
	{
		cup = pev->pCUNILOG_TARGET->cprocessors [ui];
		bool b = cunilogProcessProcessor (pev, cup);
		if (!b)
			break;
		++ ui;
	}

	if (!cunilogIsEventInternal (pev))
	{
		if (cunilogTargetHasRunProcessorsOnStartup (pev->pCUNILOG_TARGET))
			cunilogTargetClrRunProcessorsOnStartup (pev->pCUNILOG_TARGET);
	}
}

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
	static bool cunilogProcessEvtCommand (CUNILOG_EVENT *pev)
	{
		ubf_assert (pev->lenDataToLog >= sizeof (enum cunilogEvtCmd));
		#ifdef DEBUG
			enum cunilogEvtCmd cmd;
			memcpy (&cmd, pev->szDataToLog, sizeof (enum cunilogEvtCmd));
			ubf_assert (0 <= cmd);
			ubf_assert (cunilogCmdConfigXAmountEnumValues > cmd);
		#endif
		culCmdChangeCmdConfigFromCommand (pev);
		DoneCUNILOG_EVENT (NULL, pev);
		return true;
	}
#endif

static bool cunilogProcessEventSingleThreaded (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pCUNILOG_TARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pCUNILOG_TARGET));

	// Event command flag for shutting down the separate logging thread.
	if (cunilogIsEventShutdown (pev))
	{
		cunilogTargetSetShutdownInitiatedFlag (pev->pCUNILOG_TARGET);
		DoneCUNILOG_EVENT (NULL, pev);
		// The shutdown event has been processed. We treat this as being success.
		return true;
	}

	#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
		if (cunilogEvtTypeCommand == pev->evType)
			return cunilogProcessEvtCommand (pev);
	#endif

	size_t	eventLineSize = createEventLineFromSUNILOGEVENT (pev);
	if (CUNILOG_SIZE_ERROR != eventLineSize)
	{
		cunilogProcessProcessors (pev);
		if (cunilogHasEventNoRotation (pev))
			DecrementPendingNoRotationEvents (pev->pCUNILOG_TARGET);
		return true;
	}
	return false;
}

static bool enqueueAndTriggerSeparateLoggingThread (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pCUNILOG_TARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pCUNILOG_TARGET));

	#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
		size_t n = EnqueueCUNILOG_EVENT (pev);
		if (n)
			triggerCUNILOG_EVENTloggingThread (pev->pCUNILOG_TARGET, n);
		return n > 0;
	#else
		return cunilogProcessEventSingleThreaded (pev);
	#endif
}

static bool cunilogProcessEventSingleThreadedSeparateLoggingThread (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pCUNILOG_TARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pCUNILOG_TARGET));

	return enqueueAndTriggerSeparateLoggingThread (pev);
}

static bool cunilogProcessEventMultiThreaded (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pCUNILOG_TARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pCUNILOG_TARGET));

	EnterCUNILOG_LOCKER (pev->pCUNILOG_TARGET);
	bool b = cunilogProcessEventSingleThreaded (pev);
	LeaveCUNILOG_LOCKER (pev->pCUNILOG_TARGET);
	return b;
}

static bool cunilogProcessEventMultiThreadedSeparateLoggingThread (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pCUNILOG_TARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pCUNILOG_TARGET));

	return enqueueAndTriggerSeparateLoggingThread (pev);
}

static bool cunilogProcessOrQueueEventMultiProcesses (CUNILOG_EVENT *pev)
{
	UNREFERENCED_PARAMETER (pev);
	ubf_assert_msg (false, "Not implemented yet.");
	return false;
}

static bool (*cunilogProcOrQueueEvt [cunilogTypeAmountEnumValues]) (CUNILOG_EVENT *pev) =
{
	/* cunilogSingleThreaded				*/		cunilogProcessEventSingleThreaded
	/* cunilogSingleThreadedSeparateThread	*/	,	cunilogProcessEventSingleThreadedSeparateLoggingThread
	/* cunilogMultiThreaded					*/	,	cunilogProcessEventMultiThreaded
	/* cunilogMultiThreadedSeparateThread	*/	,	cunilogProcessEventMultiThreadedSeparateLoggingThread
	/* cunilogMultiProcesses				*/	,	cunilogProcessOrQueueEventMultiProcesses
};

/*
	Called by the logging functions.
*/
static bool cunilogProcessOrQueueEvent (CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (pev);

	CUNILOG_TARGET *put = pev->pCUNILOG_TARGET;
	ubf_assert_non_NULL (put);
	ubf_assert (cunilogIsTargetInitialised (put));

	// Sanity check for the type.
	ubf_assert (put->culogType >= 0);
	ubf_assert (put->culogType < cunilogTypeAmountEnumValues);

	#ifdef CUNILOG_BUILD_SINGLE_THREADED_ONLY
		// This is the only one possible in a single-threaded environment.
		ubf_assert (cunilogSingleThreaded == put->culogType);
	#endif

	return cunilogProcOrQueueEvt [put->culogType] (pev);
}

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static void WaitForEndOfSeparateLoggingThread (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

	#ifdef OS_IS_WINDOWS
			ubf_assert_non_NULL (put->th.hThread);
			DWORD dw = WaitForSingleObject (put->th.hThread, INFINITE);
			UNREFERENCED_PARAMETER (dw);
			ubf_assert (WAIT_OBJECT_0 == dw);
		#else
			ubf_assert_non_0 (put->th.tThread);
			void *threadRetValue;
			int i = pthread_join (put->th.tThread, &threadRetValue);
			UNREFERENCED_PARAMETER (threadRetValue);
			ubf_assert (0 == i);
		#endif
	}
#else
	#define WaitForEndOfSeparateLoggingThread()
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static CUNILOG_EVENT scueShutdown =
	{
		NULL, CUNILOGEVENT_SHUTDOWN, 0,						// Up to stamp.
		NULL, 0,											// Up to lenDataToLog
		NULL,												// Member *next.
		cunilogEvtSeverityNone, cunilogEvtTypeNormalText,
		0													// Member sizEvent.
	};
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	bool queueShutdownEvent (CUNILOG_TARGET *put)
	{
		CUNILOG_EVENT *pev = DuplicateCUNILOG_EVENT (&scueShutdown);
		if (pev)
		{
			pev->pCUNILOG_TARGET = put;
			cunilogSetEventAllocated (pev);
			pev->stamp = 1;
			cunilogProcessOrQueueEvent (pev);
			return true;
		}
		return false;
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	bool ShutdownCUNILOG_TARGET (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (HAS_CUNILOG_TARGET_A_QUEUE (put))
		{
			if (queueShutdownEvent (put))
			{
				WaitForEndOfSeparateLoggingThread (put);
				cunilogTargetSetShutdownCompleteFlag (put);
				return true;
			}
			return false;
		}
		cunilogTargetSetShutdownCompleteFlag (put);
		return true;
	}
#else
	bool ShutdownCUNILOG_TARGET (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		cunilogTargetSetShutdownCompleteFlag (put);
		return true;
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	bool CancelCUNILOG_TARGET (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (HAS_CUNILOG_TARGET_A_QUEUE (put))
		{
			cunilogTargetSetShutdownInitiatedFlag (put);

			// Empty the queue. While this would actually not be required here, it can
			//	speed up things significantly (well, maybe a few cycles) with busy queues as
			//	it takes some burden off the separate logging thread.
			CUNILOG_EVENT *pev = DequeueAllCUNILOG_EVENTs (put);
			CUNILOG_EVENT *nxt;
			while (pev)
			{
				nxt = pev->next;
				DoneCUNILOG_EVENT (NULL, pev);
				pev = nxt;
			}

			// Queue the shutdown command for the separate logging thread and wait
			//	for it to end.
			if (queueShutdownEvent (put))
			{
				WaitForEndOfSeparateLoggingThread (put);
				return true;
			}
			return false;
		}
		return true;
	}
#else
	bool CancelCUNILOG_TARGET (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		cunilogSetShutdownTarget (put);
		return true;
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	void PauseLogCUNILOG_TARGET (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		EnterCUNILOG_LOCKER (put);
		cunilogTargetSetIsPaused (put);
		LeaveCUNILOG_LOCKER (put);
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	size_t ResumeLogCUNILOG_TARGET (CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL (put);

		size_t n;

		EnterCUNILOG_LOCKER (put);
		cunilogTargetClrIsPaused (put);
		n = put->nPausedEvents;
		put->nPausedEvents = 0;
		LeaveCUNILOG_LOCKER (put);

		if (needsOrHasLocker (put))
			triggerCUNILOG_EVENTloggingThread (put, n);
		return n;
	}
#endif

/*
	User logging functions.
*/

bool logEv (CUNILOG_TARGET *put, CUNILOG_EVENT *pev)
{
	ubf_assert_non_NULL (put);
	ubf_assert_non_NULL (pev);
	ubf_assert (cunilogIsTargetInitialised (put));

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	pev->pCUNILOG_TARGET = put;
	return cunilogProcessOrQueueEvent (pev);
}

bool logTextU8sevl			(CUNILOG_TARGET *put, cueventseverity sev, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Text (put, sev, ccText, len);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logTextU8sevlts		(CUNILOG_TARGET *put, cueventseverity sev, const char *ccText, size_t len, UBF_TIMESTAMP ts)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_TextTS (put, sev, ccText, len, ts);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logTextU8sevlq			(CUNILOG_TARGET *put, cueventseverity sev, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Text (put, sev, ccText, len);
	if (pev)
	{
		cunilogSetEventNoRotation (pev);
		return cunilogProcessOrQueueEvent (pev);
	}
	return false;
}

bool logTextU8sevlqts		(CUNILOG_TARGET *put, cueventseverity sev, const char *ccText, size_t len, UBF_TIMESTAMP ts)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_TextTS (put, sev, ccText, len, ts);
	if (pev)
	{
		cunilogSetEventNoRotation (pev);
		return cunilogProcessOrQueueEvent (pev);
	}
	return false;
}

bool logTextU8sev			(CUNILOG_TARGET *put, cueventseverity sev, const char *ccText)
{
	return logTextU8sevl (put, sev, ccText, USE_STRLEN);
}

bool logTextU8sevq			(CUNILOG_TARGET *put, cueventseverity sev, const char *ccText)
{
	return logTextU8sevlq (put, sev, ccText, USE_STRLEN);
}

bool logTextU8l				(CUNILOG_TARGET *put, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Text (put, cunilogEvtSeverityNone, ccText, len);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logTextU8lq			(CUNILOG_TARGET *put, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Text (put, cunilogEvtSeverityNone, ccText, len);
	if (pev)
	{
		cunilogSetEventNoRotation (pev);
		return cunilogProcessOrQueueEvent (pev);
	}
	return false;
}

bool logTextU8				(CUNILOG_TARGET *put, const char *ccText)
{
	return logTextU8l (put, ccText, USE_STRLEN);
}

bool logTextU8q				(CUNILOG_TARGET *put, const char *ccText)
{
	return logTextU8lq (put, ccText, USE_STRLEN);
}

bool logTextU8vfmt			(CUNILOG_TARGET *put, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	char *ob = ubf_malloc (l + 1);
	if (ob)
	{
		vsnprintf (ob, l + 1, fmt, ap);
		bool b = logTextU8l (put, ob, l);
		ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8fmt			(CUNILOG_TARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	bool		b;

	va_start (ap, fmt);
	b = logTextU8vfmt (put, fmt, ap);
	va_end (ap);

	return b;
}

bool logTextU8qvfmt			(CUNILOG_TARGET *put, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	char *ob = ubf_malloc (l + 1);
	if (ob)
	{
		vsnprintf (ob, l + 1, fmt, ap);
		bool b = logTextU8lq (put, ob, l);
		ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8qfmt			(CUNILOG_TARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	bool		b;

	va_start (ap, fmt);
	b = logTextU8qvfmt (put, fmt, ap);
	va_end (ap);

	return b;
}

bool logTextU8svfmt			(CUNILOG_TARGET *put, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;

	char		cb [CUNILOG_DEFAULT_SFMT_SIZE];
	char		*ob;

	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	ob = l < CUNILOG_DEFAULT_SFMT_SIZE ? cb : ubf_malloc (l + 1);
	if (ob)
	{
		vsnprintf (ob, l + 1, fmt, ap);

		bool b = logTextU8l (put, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8sfmt			(CUNILOG_TARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	bool		b;

	va_start (ap, fmt);
	b = logTextU8svfmt (put, fmt, ap);
	va_end (ap);

	return b;
}

bool logTextU8sqvfmt		(CUNILOG_TARGET *put, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;

	char		cb [CUNILOG_DEFAULT_SFMT_SIZE];
	char		*ob;

	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	ob = l < CUNILOG_DEFAULT_SFMT_SIZE ? cb : ubf_malloc (l + 1);
	if (ob)
	{
		vsnprintf (ob, l + 1, fmt, ap);
		bool b = logTextU8lq (put, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8sqfmt			(CUNILOG_TARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	bool		b;

	va_start (ap, fmt);
	b = logTextU8sqfmt (put, fmt, ap);
	va_end (ap);

	return b;
}

bool logTextU8svfmtsev		(CUNILOG_TARGET *put, cueventseverity sev, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;

	char		cb [CUNILOG_DEFAULT_SFMT_SIZE];
	char		*ob;

	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	ob = l < CUNILOG_DEFAULT_SFMT_SIZE ? cb : ubf_malloc (l + 1);
	if (ob)
	{
		vsnprintf (ob, l + 1, fmt, ap);

		bool b = logTextU8sevl (put, sev, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8sfmtsev		(CUNILOG_TARGET *put, cueventseverity sev, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	bool		b;

	va_start (ap, fmt);
	b = logTextU8svfmtsev (put, sev, fmt, ap);
	va_end (ap);

	return b;
}

bool logTextU8smbvfmtsev	(CUNILOG_TARGET *put, SMEMBUF *smb, cueventseverity sev, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);
	ubf_assert (isInitialisedSMEMBUF (smb));

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	growToSizeSMEMBUF (smb, l + 1);
	if (isUsableSMEMBUF (smb))
	{
		if (smb->buf.pch)
		{
			vsnprintf (smb->buf.pch, l + 1, fmt, ap);

			bool b = logTextU8sevl (put, sev, smb->buf.pch, l);
			return b;
		}
	}
	return false;
}

bool logTextU8smbfmtsev		(CUNILOG_TARGET *put, SMEMBUF *smb, cueventseverity sev, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);
	ubf_assert (isInitialisedSMEMBUF (smb));

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	bool		b;

	va_start (ap, fmt);
	b = logTextU8smbvfmtsev (put, smb, sev, fmt, ap);
	va_end (ap);

	return b;
}

bool logTextU8smbvfmt		(CUNILOG_TARGET *put, SMEMBUF *smb, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);
	ubf_assert (isInitialisedSMEMBUF (smb));

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	return logTextU8smbvfmtsev (put, smb, cunilogEvtSeverityNone, fmt, ap);
}

bool logTextU8smbfmt		(CUNILOG_TARGET *put, SMEMBUF *smb, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);
	ubf_assert (isInitialisedSMEMBUF (smb));

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	bool		b;

	va_start (ap, fmt);
	b = logTextU8smbvfmt (put, smb, fmt, ap);
	va_end (ap);

	return b;
}

bool logHexDumpU8sevl		(CUNILOG_TARGET *put, cueventseverity sev, const void *pBlob, size_t size, const char *ccCaption, size_t lenCaption)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Data (put, sev, pBlob, size, ccCaption, lenCaption);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logHexDumpU8l			(CUNILOG_TARGET *put, const void *pBlob, size_t size, const char *ccCaption, size_t lenCaption)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Data	(
							put, cunilogEvtSeverityNone, pBlob, size,
							ccCaption, lenCaption
													);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logHexDump				(CUNILOG_TARGET *put, const void *pBlob, size_t size)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Data	(
							put, cunilogEvtSeverityNone, pBlob,
							size, NULL, 0
													);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logHexDumpq			(CUNILOG_TARGET *put, const void *pBlob, size_t size)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Data	(
							put, cunilogEvtSeverityNone, pBlob,
							size, NULL, 0
													);
	if (pev)
	{
		cunilogSetEventNoRotation (pev);
		return cunilogProcessOrQueueEvent (pev);
	}
	return false;
}

bool logHexOrText			(CUNILOG_TARGET *put, const void *szHexOrTxt, size_t lenHexOrTxt)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	if (str_has_only_printable_ASCII (szHexOrTxt, lenHexOrTxt))
		return logTextU8l (put, szHexOrTxt, lenHexOrTxt);

	return logHexDump (put, szHexOrTxt, lenHexOrTxt);
}

bool logHexOrTextq			(CUNILOG_TARGET *put, const void *szHexOrTxt, size_t lenHexOrTxt)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	if (str_has_only_printable_ASCII (szHexOrTxt, lenHexOrTxt))
		return logTextU8lq (put, szHexOrTxt, lenHexOrTxt);

	return logHexDumpq (put, szHexOrTxt, lenHexOrTxt);
}

bool logHexOrTextU8			(CUNILOG_TARGET *put, const void *szHexOrTxtU8, size_t lenHexOrTxtU8)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	if (c_check_utf8 (szHexOrTxtU8, lenHexOrTxtU8))
		return logTextU8l (put, szHexOrTxtU8, lenHexOrTxtU8);

	return logHexDump (put, szHexOrTxtU8, lenHexOrTxtU8);
}

#ifdef PLATFORM_IS_WINDOWS
bool logTextWU16sevl			(CUNILOG_TARGET *put, cueventseverity sev, const wchar_t *cwText, size_t len)
{
	ubf_assert_non_NULL (put);
	
	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	char s8 [CUNILOG_STD_MSG_SIZE * 4];
	char *p8;

	len = USE_STRLEN == len ? wcslen (cwText) : len;
	ubf_assert (len <= INT_MAX);
	if (len > INT_MAX)
		return false;

	int il = (int) len;
	if (0 == il)
		return logTextU8sevl (put, sev, "", 0);

	int siz = reqUTF8sizel (cwText, il);

	// Case already covered above.
	ubf_assert_non_0 (siz);

	if (siz < CUNILOG_STD_MSG_SIZE * 4)
		p8 = s8;
	else
		p8 = ubf_malloc ((size_t) siz + 1);
	if (p8)
	{
		UTF8_from_WinU16l (p8, siz, cwText, il);
		if (ASCII_NUL == p8 [siz])
			-- siz;
		bool b = logTextU8sevl (put, sev, p8, siz);

		if (p8 != s8)
			ubf_free (p8);

		return b;
	}
	return false;
}
#endif

#ifdef PLATFORM_IS_WINDOWS
bool logTextWU16sev			(CUNILOG_TARGET *put, cueventseverity sev, const wchar_t *cwText)
{
	ubf_assert_non_NULL (put);
	
	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	char s8 [CUNILOG_STD_MSG_SIZE * 4];
	char *p8;

	int siz = reqUTF8size (cwText);

	// We always need at least space for a NUL terminator, hence siz can actually never
	//	be 0 here.
	ubf_assert_non_0 (siz);

	if (siz <= CUNILOG_STD_MSG_SIZE * 4)
		p8 = s8;
	else
		p8 = malloc (siz);
	if (p8)
	{
		UTF8_from_WinU16 (p8, siz, cwText);
		if (ASCII_NUL == p8 [siz])
			-- siz;
		bool b = logTextU8sevl (put, sev, p8, siz);

		if (p8 != s8)
			free (p8);

		return b;
	}
	return false;
}
#endif

#ifdef PLATFORM_IS_WINDOWS
bool logTextWU16l				(CUNILOG_TARGET *put, const wchar_t *cwText, size_t len)
{
	ubf_assert_non_NULL (put);
	
	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	return logTextWU16sevl (put, cunilogEvtSeverityNone, cwText, len);
}
#endif

#ifdef PLATFORM_IS_WINDOWS
bool logTextWU16				(CUNILOG_TARGET *put, const wchar_t *cwText)
{
	ubf_assert_non_NULL (put);
	
	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	return logTextWU16sev (put, cunilogEvtSeverityNone, cwText);
}
#endif

bool logTextU8csevl			(CUNILOG_TARGET *put, cueventseverity sev, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	CUNILOG_EVENT *pev = CreateCUNILOG_EVENT_Text (put, sev, ccText, len);
	if (pev)
	{
		cunilogSetEventEchoOnly (pev);
		return cunilogProcessOrQueueEvent (pev);
	}
	return false;
}

bool logTextU8csev			(CUNILOG_TARGET *put, cueventseverity sev, const char *ccText)
{
	ubf_assert_non_NULL (put);

	return logTextU8csevl (put, sev, ccText, USE_STRLEN);
}

bool logTextU8cl			(CUNILOG_TARGET *put, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	return logTextU8csevl (put, cunilogEvtSeverityNone, ccText, len);
}

bool logTextU8c				(CUNILOG_TARGET *put, const char *ccText)
{
	ubf_assert_non_NULL (put);

	return logTextU8csevl (put, cunilogEvtSeverityNone, ccText, USE_STRLEN);
}

bool logTextU8cvfmt			(CUNILOG_TARGET *put, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	char *ob = ubf_malloc (l + 1);
	if (ob)
	{
		vsnprintf (ob, l + 1, fmt, ap);
		bool b = logTextU8cl (put, ob, l);
		ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8cfmt			(CUNILOG_TARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	bool		b;

	va_start (ap, fmt);
	b = logTextU8cvfmt (put, fmt, ap);
	va_end (ap);

	return b;
}

bool logTextU8csfmt			(CUNILOG_TARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	size_t		l;

	char		cb [CUNILOG_DEFAULT_SFMT_SIZE];
	char		*ob;

	va_start (ap, fmt);
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);
	va_end (ap);

	ob = l < CUNILOG_DEFAULT_SFMT_SIZE ? cb : ubf_malloc (l + 1);
	if (ob)
	{
		va_start (ap, fmt);
		vsnprintf (ob, l + 1, fmt, ap);
		va_end (ap);

		bool b = logTextU8cl (put, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8csmbvfmt		(CUNILOG_TARGET *put, SMEMBUF *smb, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);
	ubf_assert (isInitialisedSMEMBUF (smb));

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;

	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	growToSizeSMEMBUF (smb, l + 1);
	if (isUsableSMEMBUF (smb))
	{
		if (smb->buf.pch)
		{
			vsnprintf (smb->buf.pch, l + 1, fmt, ap);

			bool b = logTextU8cl (put, smb->buf.pch, l);
			return b;
		}
	}
	return false;
}

bool logTextU8csmbfmt		(CUNILOG_TARGET *put, SMEMBUF *smb, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);
	ubf_assert (isInitialisedSMEMBUF (smb));

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	size_t		l;

	va_start (ap, fmt);
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);
	va_end (ap);

	growToSizeSMEMBUF (smb, l + 1);
	if (isUsableSMEMBUF (smb))
	{
		if (smb->buf.pch)
		{
			va_start (ap, fmt);
			vsnprintf (smb->buf.pch, l + 1, fmt, ap);
			va_end (ap);

			bool b = logTextU8cl (put, smb->buf.pch, l);
			return b;
		}
	}
	return false;
}

bool logTextU8csvfmtsev		(CUNILOG_TARGET *put, cueventseverity sev, const char *fmt, va_list ap)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	size_t		l;

	char		cb [CUNILOG_DEFAULT_SFMT_SIZE];
	char		*ob;

	l = (size_t) vsnprintf (NULL, 0, fmt, ap);

	ob = l < CUNILOG_DEFAULT_SFMT_SIZE ? cb : ubf_malloc (l + 1);
	if (ob)
	{
		vsnprintf (ob, l + 1, fmt, ap);

		bool b = logTextU8csevl (put, sev, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8csfmtsev		(CUNILOG_TARGET *put, cueventseverity sev, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogTargetHasShutdownInitiatedFlag (put))
		return false;

	va_list		ap;
	size_t		l;

	char		cb [CUNILOG_DEFAULT_SFMT_SIZE];
	char		*ob;

	va_start (ap, fmt);
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);
	va_end (ap);

	ob = l < CUNILOG_DEFAULT_SFMT_SIZE ? cb : ubf_malloc (l + 1);
	if (ob)
	{
		va_start (ap, fmt);
		vsnprintf (ob, l + 1, fmt, ap);
		va_end (ap);

		bool b = logTextU8csevl (put, sev, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
	static inline CUNILOG_EVENT *CreateCUNILOG_EVENTforCommand (CUNILOG_TARGET *put, enum cunilogEvtCmd cmd)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= cmd);
		ubf_assert			(cunilogCmdConfigXAmountEnumValues > cmd);

		size_t rs = culCmdRequiredSize (cmd);
		ubf_assert (CUNILOG_CMD_INVALID_SIZE != rs);

		if (CUNILOG_CMD_INVALID_SIZE != rs)
		{
			CUNILOG_EVENT *pev = CreateCUNILOG_EVENTandData	(
									put, cunilogEvtSeverityNone,
									NULL, 0,
									cunilogEvtTypeCommand, NULL,
									rs
															);
			return pev;
		}
		return NULL;
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
	bool ChangeCUNILOG_TARGETuseColourForEcho (CUNILOG_TARGET *put, bool bUseColour)
	{
		CUNILOG_EVENT *pev = CreateCUNILOG_EVENTforCommand (put, cunilogCmdConfigUseColourForEcho);
		if (pev)
		{
			culCmdStoreCmdConfigUseColourForEcho (pev->szDataToLog, bUseColour);
			return cunilogProcessOrQueueEvent (pev);
		}
		return false;
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
	bool ChangeCUNILOG_TARGETcunilognewline (CUNILOG_TARGET *put, newline_t nl)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= nl);
		ubf_assert			(cunilogNewLineAmountEnumValues > nl);

		CUNILOG_EVENT *pev = CreateCUNILOG_EVENTforCommand (put, cunilogCmdConfigCunilognewline);
		if (pev)
		{
			culCmdStoreCmdConfigCunilognewline (pev->szDataToLog, nl);
			return cunilogProcessOrQueueEvent (pev);
		}
		return false;
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
	bool ChangeCUNILOG_TARGETdisableTaskProcessors (CUNILOG_TARGET *put, enum cunilogprocesstask task)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= task);
		ubf_assert			(cunilogProcessXAmountEnumValues > task);

		CUNILOG_EVENT *pev = CreateCUNILOG_EVENTforCommand (put, cunilogCmdConfigDisableTaskProcessors);
		if (pev)
		{
			culCmdStoreCmdConfigEnableTaskProcessors (pev->szDataToLog, task);
			return cunilogProcessOrQueueEvent (pev);
		}
		return false;
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
	bool ChangeCUNILOG_TARGETenableTaskProcessors (CUNILOG_TARGET *put, enum cunilogprocesstask task)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= task);
		ubf_assert			(cunilogProcessXAmountEnumValues > task);

		CUNILOG_EVENT *pev = CreateCUNILOG_EVENTforCommand (put, cunilogCmdConfigEnableTaskProcessors);
		if (pev)
		{
			culCmdStoreCmdConfigEnableTaskProcessors (pev->szDataToLog, task);
			return cunilogProcessOrQueueEvent (pev);
		}
		return false;
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
	bool ChangeCUNILOG_TARGETdisableEchoProcessor	(CUNILOG_TARGET *put)
	{
		ubf_assert_non_NULL	(put);

		enum cunilogEvtCmd	cmd		= cunilogCmdConfigDisableEchoProcessor;
		CUNILOG_EVENT		*pev	= CreateCUNILOG_EVENTforCommand (put, cmd);
		if (pev)
		{
			memcpy (pev->szDataToLog, &cmd, sizeof (cmd));
			return cunilogProcessOrQueueEvent (pev);
		}
		return false;
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
	bool ChangeCUNILOG_TARGETenableEchoProcessor	(CUNILOG_TARGET *put)
	{
		enum cunilogEvtCmd	cmd		= cunilogCmdConfigEnableEchoProcessor;
		CUNILOG_EVENT		*pev	= CreateCUNILOG_EVENTforCommand (put, cmd);
		if (pev)
		{
			memcpy (pev->szDataToLog, &cmd, sizeof (cmd));
			return cunilogProcessOrQueueEvent (pev);
		}
		return false;
	}
#endif

#ifndef CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS
#ifndef CUNILOG_BUILD_WITHOUT_EVENT_SEVERITY_TYPE
	bool ChangeCUNILOG_TARGETeventSeverityFormatType (CUNILOG_TARGET *put, cueventsevfmtpy sevTpy)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= sevTpy);
		ubf_assert			(cunilogEvtSeverityTypeXAmountEnumValues > sevTpy);

		CUNILOG_EVENT *pev = CreateCUNILOG_EVENTforCommand	(
								put, cunilogCmdConfigEventSeverityFormatType
															);
		if (pev)
		{
			culCmdStoreConfigEventSeverityFormatType (pev->szDataToLog, sevTpy);
			return cunilogProcessOrQueueEvent (pev);
		}
		return false;
	}
#endif
#endif

#if !defined (CUNILOG_BUILD_SINGLE_THREADED_ONLY) && !defined (CUNILOG_BUILD_WITHOUT_EVENT_COMMANDS)
	bool ChangeCUNILOG_TARGETlogPriority (CUNILOG_TARGET *put, cunilogprio prio)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= prio);
		ubf_assert			(prio < cunilogPrioAmountEnumValues);

		if (hasSeparateLoggingThread (put))
		{
			CUNILOG_EVENT *pev = CreateCUNILOG_EVENTforCommand (put, cunilogCmdConfigSetLogPriority);
			if (pev)
			{
				culCmdStoreConfigLogThreadPriority (pev->szDataToLog, prio);
				return cunilogProcessOrQueueEvent (pev);
			}
			return false;
		}
		return true;
	}
#endif

bool CunilogChangeCurrentThreadPriority (cunilogprio prio)
{
	ubf_assert			(0 <= prio);
	ubf_assert			(prio < cunilogPrioAmountEnumValues);

	if (0 <= prio && prio < cunilogPrioAmountEnumValues)
	{
		return culCmdSetCurrentThreadPriority (prio);
	}
	return false;
}

void cunilogSetDefaultPrintEventSeverityFormatType (cueventsevfmtpy fmtpy)
{
	ubf_assert			(0 <= fmtpy);
	ubf_assert			(cunilogEvtSeverityTypeXAmountEnumValues > fmtpy);

	cunilogEvtSeverityTypeDefault = fmtpy;
}

void cunilogUseColourForOutput (bool bUseColour)
{
	bUseCunilogDefaultOutputColour = bUseColour;
}

int cunilog_printf_sev_fmtpy_vl	(
		cueventseverity		sev,
		cueventsevfmtpy		sftpy,
		const char			*format,
		va_list				ap
)
{
	ubf_assert_non_NULL (format);

	bool bUseColour = bUseCunilogDefaultOutputColour;

	size_t lenRequired = 0;
	if (bUseColour)
		lenRequired += evtSeverityColoursLen (sev);
	lenRequired += requiredEventSeverityChars (sev, sftpy);

	int			iReq;

	iReq = vsnprintf (NULL, 0, format, ap);
	if (iReq < 0)
		return iReq;
	lenRequired += iReq;

	#ifdef DEBUG
		++ lenRequired;
	#endif

	int		iRet = -1;
	char	szToPrint [CUNILOG_DEFAULT_SFMT_SIZE];
	char	*pzToPrint;

	if (lenRequired < CUNILOG_DEFAULT_SFMT_SIZE)
		pzToPrint = szToPrint;
	else
		pzToPrint = ubf_malloc (lenRequired + 1);

	if (pzToPrint)
	{
		#ifdef DEBUG
			pzToPrint [lenRequired]	= UBF_ERROR_CHAR;
		#endif

		char *pz = pzToPrint;
		if (bUseColour)
			cpyEvtSeverityColour (&pz, sev);
		size_t st = writeEventSeverity (pz, sev, sftpy);
		pz += st;
		iReq = vsnprintf (pz, lenRequired + 1, format, ap);
		if (iReq < 0)
			goto Leave;
		ubf_assert (UBF_ERROR_CHAR == pz [lenRequired]);

		pz += iReq;
		if (bUseColour)
			cpyRstEvtSeverityColour (&pz, sev);
		pz [0] = ASCII_NUL;
		ubf_assert (UBF_ERROR_CHAR == pzToPrint [lenRequired]);
		#ifdef DEBUG
			-- lenRequired;
		#endif

		#ifdef PLATFORM_IS_WINDOWS
			iRet = cunilogPrintWin (pzToPrint, lenRequired);
		#else
			if (lenRequired)
				iRet = printf (pz);
			else
				iRet = printf ("");
		#endif
	}

	Leave:
	if (pzToPrint && pzToPrint != szToPrint)
		ubf_free (pzToPrint);
	return iRet;
}

int cunilog_printf_sev_fmtpy	(
		cueventseverity		sev,
		cueventsevfmtpy		sftpy,
		const char			*format,
		...
								)
{
	ubf_assert_non_NULL (format);

	int			iRet		= -1;
	va_list		args;

	va_start	(args, format);
	iRet = cunilog_printf_sev_fmtpy_vl (sev, sftpy, format, args);
	va_end		(args);

	return iRet;
}

int cunilog_printf_sev			(
		cueventseverity		sev,
		const char			*format,
		...
								)
{
	ubf_assert_non_NULL (format);

	int			iRet		= -1;
	va_list		args;

	va_start	(args, format);
	iRet = cunilog_printf_sev_fmtpy_vl (sev, cunilogEvtSeverityTypeDefault, format, args);
	va_end		(args);

	return iRet;
}

int cunilog_printf				(
		const char			*format,
		...
								)
{
	ubf_assert_non_NULL (format);

	int			iRet		= -1;
	va_list		args;

	va_start	(args, format);
	iRet = cunilog_printf_sev_fmtpy_vl (cunilogEvtSeverityNone, cunilogEvtSeverityTypeDefault, format, args);
	va_end		(args);

	return iRet;
}

int cunilog_puts_sev_fmtpy_l	(
		cueventseverity		sev,
		cueventsevfmtpy		sftpy,
		const char			*strU8,
		size_t				len
								)
{
	ubf_assert_non_NULL (strU8);

	if (NULL == strU8)
		return EOF;

	bool bUseColour = bUseCunilogDefaultOutputColour;

	len = USE_STRLEN == len ? strlen (strU8) : len;
	size_t lenRequired = 0;
	if (bUseColour)
		lenRequired += evtSeverityColoursLen (sev);			// Includes reset.

	#ifdef DEBUG
		size_t	sizDbgEvtSeverityColours = lenRequired;
		size_t	sizDbgEvtSeverityCol;
		size_t	sizDbgEvtSeverityRst;
		char	*szCpy;
		++ lenRequired;
	#endif

	size_t reqSevl = requiredEventSeverityChars (sev, sftpy);
	lenRequired += reqSevl;
	lenRequired += len;

	int		iRet = EOF;
	char	szToPrint [WINAPI_U8_HEAP_THRESHOLD];
	UBF_DEFINE_GUARD_VAR (testvar, 1234567);
	char	*pzToPrint;

	if (lenRequired < WINAPI_U8_HEAP_THRESHOLD)
		pzToPrint = szToPrint;
	else
		pzToPrint = ubf_malloc (lenRequired + 1);

	if (pzToPrint)
	{
		char *pz = pzToPrint;

		if (bUseColour)
			cpyEvtSeverityColour (&pz, sev);

		#ifdef DEBUG
			pzToPrint [lenRequired]	= UBF_ERROR_CHAR;
			sizDbgEvtSeverityCol	= pz - pzToPrint;
		#endif

		size_t st = writeEventSeverity (pz, sev, sftpy);
		ubf_assert (reqSevl == st);
		pz += st;
		memcpy (pz, strU8, len);
		pz += len;

		#ifdef DEBUG
			szCpy = pz;
		#endif

		if (bUseColour)
			cpyRstEvtSeverityColour (&pz, sev);
		ubf_assert (UBF_ERROR_CHAR == pzToPrint [lenRequired]);
		pz [0] = ASCII_NUL;

		#ifdef DEBUG
			sizDbgEvtSeverityRst = pz - szCpy;
			ubf_assert (sizDbgEvtSeverityCol + sizDbgEvtSeverityRst == sizDbgEvtSeverityColours);
			-- lenRequired;
		#endif

		#ifdef PLATFORM_IS_WINDOWS
			iRet = cunilogPutsWin (pzToPrint, lenRequired);
		#else
			if (lenRequired)
				iRet = puts (pz);
			else
				iRet = puts ("");
		#endif

		ubf_assert (ASCII_NUL == pzToPrint [lenRequired]);
		if (pzToPrint != szToPrint)
			ubf_free (pzToPrint);
	}

	UBF_CHECK_GUARD_VAR (testvar, 1234567);
	return iRet;
}

int cunilog_puts_sev_fmtpy		(
		cueventseverity		sev,
		cueventsevfmtpy		sftpy,
		const char			*strU8
								)
{
	ubf_assert_non_NULL (strU8);

	return cunilog_puts_sev_fmtpy_l (sev, sftpy, strU8, USE_STRLEN);
}

int cunilog_puts_sev_l			(
		cueventseverity		sev,
		const char			*strU8,
		size_t				len
								)
{
	ubf_assert_non_NULL (strU8);

	return cunilog_puts_sev_fmtpy_l (sev, cunilogEvtSeverityTypeDefault, strU8, len);
}

int cunilog_puts_sev			(
		cueventseverity		sev,
		const char			*strU8
								)
{
	ubf_assert_non_NULL (strU8);

	return cunilog_puts_sev_fmtpy_l (sev, cunilogEvtSeverityTypeDefault, strU8, USE_STRLEN);
}

int cunilog_puts_l				(
		const char			*strU8,
		size_t				len
								)
{
	ubf_assert_non_NULL (strU8);

	return cunilog_puts_sev_fmtpy_l	(
				cunilogEvtSeverityNone, cunilogEvtSeverityTypeDefault, strU8, len
									);
}

int cunilog_puts				(
		const char			*strU8
								)
{
	ubf_assert_non_NULL (strU8);

	return cunilog_puts_sev_fmtpy_l	(
				cunilogEvtSeverityNone, cunilogEvtSeverityTypeDefault, strU8, USE_STRLEN
									);
}

const uint64_t	uiCunilogVersion	=		((uint64_t) CUNILOG_VERSION_MAJOR	<< 48)
										|	((uint64_t) CUNILOG_VERSION_MINOR	<< 32)
										|	((uint64_t) CUNILOG_VERSION_SUB		<< 16)
										|	((uint64_t) CUNILOG_VERSION_BUILD);

int cunilogCheckVersionIntChk (uint64_t cunilogHdrVersion)
{
	ubf_assert_non_0 (cunilogHdrVersion);
	ubf_assert_msg	(
		uiCunilogVersion == cunilogHdrVersion,
		"Version mismatch libcunilog. If built as static library, the library doesn't match cunilog_combined.h"
					);

	if (uiCunilogVersion == cunilogHdrVersion)
		return 0;
	if (uiCunilogVersion > cunilogHdrVersion)
		return 1;
	//if (uiCunilogVersion < cunilogHdrVersion)
		return -1;
}

#ifdef CUNILOG_BUILD_TEST_FNCTS
	bool test_cunilog (void)
	{
		CUNILOG_TARGET	*pt;
		bool			bRet	= true;

		// Ensure we haven't forgotten to fill the arrays/jump tables.
		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (arrLengthTimeStampFromPostfix)	==	cunilogPostfixAmountEnumValues);
		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (arrPostfixWildcardMask)		==	cunilogPostfixAmountEnumValues);
		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (obtainTimeStampAsString)		==	cunilogPostfixAmountEnumValues);
		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (frequencyTbl)					==	cunilogPostfixAmountEnumValues);
		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (cunilogProcOrQueueEvt)			==	cunilogTypeAmountEnumValues);

		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (EventSeverityTexts3)			==	cunilogEvtSeverityXAmountEnumValues);
		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (EventSeverityTexts5)			==	cunilogEvtSeverityXAmountEnumValues);
		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (EventSeverityTexts9)			==	cunilogEvtSeverityXAmountEnumValues);
		ubf_expect_bool_AND (bRet, GET_ARRAY_LEN (evtSeverityColours)			==	cunilogEvtSeverityXAmountEnumValues);

		// Check that the length assignments are correct.
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATEHOURANDMINUTE	== lenDateTimeStampFromPostfix (cunilogPostfixMinute));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATEHOURANDMINUTE	== lenDateTimeStampFromPostfix (cunilogPostfixMinuteT));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATEANDHOUR		== lenDateTimeStampFromPostfix (cunilogPostfixHour));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATEANDHOUR		== lenDateTimeStampFromPostfix (cunilogPostfixHourT));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATE				== lenDateTimeStampFromPostfix (cunilogPostfixDay));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATE				== lenDateTimeStampFromPostfix (cunilogPostfixDefault));
		ubf_expect_bool_AND (bRet, LEN_ISO8601YEARANDWEEK		== lenDateTimeStampFromPostfix (cunilogPostfixWeek));
		ubf_expect_bool_AND (bRet, LEN_ISO8601YEARANDMONTH		== lenDateTimeStampFromPostfix (cunilogPostfixMonth));
		ubf_expect_bool_AND (bRet, LEN_ISO8601YEAR				== lenDateTimeStampFromPostfix (cunilogPostfixYear));

		ubf_expect_bool_AND (bRet, LEN_ISO8601DATEHOURANDMINUTE	== lenDateTimeStampFromPostfix (cunilogPostfixLogMinute));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATEHOURANDMINUTE	== lenDateTimeStampFromPostfix (cunilogPostfixLogMinuteT));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATEANDHOUR		== lenDateTimeStampFromPostfix (cunilogPostfixLogHour));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATEANDHOUR		== lenDateTimeStampFromPostfix (cunilogPostfixLogHourT));
		ubf_expect_bool_AND (bRet, LEN_ISO8601DATE				== lenDateTimeStampFromPostfix (cunilogPostfixLogDay));
		ubf_expect_bool_AND (bRet, LEN_ISO8601YEARANDWEEK		== lenDateTimeStampFromPostfix (cunilogPostfixLogWeek));
		ubf_expect_bool_AND (bRet, LEN_ISO8601YEARANDMONTH		== lenDateTimeStampFromPostfix (cunilogPostfixLogMonth));
		ubf_expect_bool_AND (bRet, LEN_ISO8601YEAR				== lenDateTimeStampFromPostfix (cunilogPostfixLogYear));

		ubf_expect_bool_AND (bRet, 1 + UBF_UINT64_LEN			== lenDateTimeStampFromPostfix (cunilogPostfixDotNumberMinutely));
		ubf_expect_bool_AND (bRet, 1 + UBF_UINT64_LEN			== lenDateTimeStampFromPostfix (cunilogPostfixDotNumberHourly));
		ubf_expect_bool_AND (bRet, 1 + UBF_UINT64_LEN			== lenDateTimeStampFromPostfix (cunilogPostfixDotNumberDaily));
		ubf_expect_bool_AND (bRet, 1 + UBF_UINT64_LEN			== lenDateTimeStampFromPostfix (cunilogPostfixDotNumberWeekly));
		ubf_expect_bool_AND (bRet, 1 + UBF_UINT64_LEN			== lenDateTimeStampFromPostfix (cunilogPostfixDotNumberMonthly));
		ubf_expect_bool_AND (bRet, 1 + UBF_UINT64_LEN			== lenDateTimeStampFromPostfix (cunilogPostfixDotNumberYearly));

		/*
			Check consistency between unilogstruct.h and the declarations in this code file.
		*/

		ubf_expect_bool_AND (bRet,  0 == cunilogEvtSeverityNone);
		ubf_expect_bool_AND (bRet,  1 == cunilogEvtSeverityNonePass);
		ubf_expect_bool_AND (bRet,  2 == cunilogEvtSeverityNoneFail);
		ubf_expect_bool_AND (bRet,  3 == cunilogEvtSeverityNoneWarn);
		ubf_expect_bool_AND (bRet,  4 == cunilogEvtSeverityBlanks);
		ubf_expect_bool_AND (bRet,  5 == cunilogEvtSeverityEmergency);
		ubf_expect_bool_AND (bRet,  6 == cunilogEvtSeverityNotice);
		ubf_expect_bool_AND (bRet,  7 == cunilogEvtSeverityInfo);
		ubf_expect_bool_AND (bRet,  8 == cunilogEvtSeverityMessage);
		ubf_expect_bool_AND (bRet,  9 == cunilogEvtSeverityWarning);
		ubf_expect_bool_AND (bRet, 10 == cunilogEvtSeverityError);
		ubf_expect_bool_AND (bRet, 11 == cunilogEvtSeverityPass);
		ubf_expect_bool_AND (bRet, 12 == cunilogEvtSeverityFail);
		ubf_expect_bool_AND (bRet, 13 == cunilogEvtSeverityCritical);
		ubf_expect_bool_AND (bRet, 14 == cunilogEvtSeverityFatal);
		ubf_expect_bool_AND (bRet, 15 == cunilogEvtSeverityDebug);
		ubf_expect_bool_AND (bRet, 16 == cunilogEvtSeverityTrace);
		ubf_expect_bool_AND (bRet, 17 == cunilogEvtSeverityDetail);
		ubf_expect_bool_AND (bRet, 18 == cunilogEvtSeverityVerbose);
		ubf_expect_bool_AND (bRet, 19 == cunilogEvtSeverityIllegal);
		ubf_expect_bool_AND (bRet, 20 == cunilogEvtSeverityXAmountEnumValues);

		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts3 [cunilogEvtSeverityNone]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts3 [cunilogEvtSeverityNonePass]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts3 [cunilogEvtSeverityNoneFail]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts3 [cunilogEvtSeverityNoneWarn]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityBlanks]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityEmergency]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityNotice]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityInfo]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityMessage]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityWarning]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityError]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityPass]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityFail]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityCritical]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityFatal]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityDebug]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityTrace]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityDetail]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityVerbose]));
		ubf_expect_bool_AND (bRet, 3 == strlen (EventSeverityTexts3 [cunilogEvtSeverityIllegal]));

		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts5 [cunilogEvtSeverityNone]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts5 [cunilogEvtSeverityNonePass]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts5 [cunilogEvtSeverityNoneFail]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts5 [cunilogEvtSeverityNoneWarn]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityBlanks]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityEmergency]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityNotice]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityInfo]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityMessage]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityWarning]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityError]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityPass]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityFail]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityCritical]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityFatal]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityDebug]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityTrace]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityDetail]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityVerbose]));
		ubf_expect_bool_AND (bRet, 5 == strlen (EventSeverityTexts5 [cunilogEvtSeverityIllegal]));

		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts9 [cunilogEvtSeverityNone]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts9 [cunilogEvtSeverityNonePass]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts9 [cunilogEvtSeverityNoneFail]));
		ubf_expect_bool_AND (bRet, 0 == strlen (EventSeverityTexts9 [cunilogEvtSeverityNoneWarn]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityBlanks]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityEmergency]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityNotice]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityInfo]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityMessage]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityWarning]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityError]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityPass]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityFail]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityCritical]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityFatal]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityDebug]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityTrace]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityDetail]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityVerbose]));
		ubf_expect_bool_AND (bRet, 9 == strlen (EventSeverityTexts9 [cunilogEvtSeverityIllegal]));

		#ifdef OS_IS_LINUX
			bool bTrash = MoveFileToTrashPOSIX ("/home/thomas/FS/OAN/Thomas/cunilog/logs/testcunilog_2024-11-05 20_14.log");
			ubf_assert_false (bTrash);
		#endif

		/*
			Static.
		*/
		pt = InitCUNILOG_TARGETstaticEx	(
					NULL,		0,
					"Unilog",	USE_STRLEN,
					cunilogPath_relativeToHomeDir,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0,
					cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup
										);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== pt);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== &CUNILOG_TARGETstatic);
		ubf_expect_bool_AND (bRet, cunilogSingleThreaded	== CUNILOG_TARGETstatic.culogType);
		ubf_expect_bool_AND (bRet, cunilogPostfixDay		== CUNILOG_TARGETstatic.culogPostfix);
		ubf_expect_bool_AND (bRet, cunilogEvtTS_Default	== CUNILOG_TARGETstatic.unilogEvtTSformat);
		ubf_expect_bool_AND (bRet, 6 == CUNILOG_TARGETstatic.lnAppName);
		ubf_expect_bool_AND (bRet, !memcmp (CUNILOG_TARGETstatic.mbAppName.buf.pch, "Unilog", CUNILOG_TARGETstatic.lnAppName));

		size_t lnAbsLogPath;
		const char *szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);

		DoneCUNILOG_TARGETstatic ();

		pt = InitCUNILOG_TARGETstaticEx	(
					NULL,				0,
					"////sub/Unilog",	USE_STRLEN,
					cunilogPath_relativeToHomeDir,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0,
					cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup
										);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== pt);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== &CUNILOG_TARGETstatic);
		ubf_expect_bool_AND (bRet, cunilogSingleThreaded	== CUNILOG_TARGETstatic.culogType);
		ubf_expect_bool_AND (bRet, cunilogPostfixDay		== CUNILOG_TARGETstatic.culogPostfix);
		ubf_expect_bool_AND (bRet, cunilogEvtTS_Default	== CUNILOG_TARGETstatic.unilogEvtTSformat);
		ubf_expect_bool_AND (bRet, 10 == CUNILOG_TARGETstatic.lnAppName);
		#ifdef PLATFORM_IS_WINDOWS
			ubf_expect_bool_AND (bRet, !memcmp (CUNILOG_TARGETstatic.mbAppName.buf.pch, "sub\\Unilog", CUNILOG_TARGETstatic.lnAppName));
		#else
			ubf_expect_bool_AND (bRet, !memcmp (CUNILOG_TARGETstatic.mbAppName.buf.pch, "sub/Unilog", CUNILOG_TARGETstatic.lnAppName));
		#endif

		szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);

		DoneCUNILOG_TARGETstatic ();

		pt = InitCUNILOG_TARGETstaticEx	(
					NULL,		0,
					"Unilog",	USE_STRLEN,
					cunilogPath_relativeToCurrentDir,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0,
					cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup
										);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== pt);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== &CUNILOG_TARGETstatic);
		ubf_expect_bool_AND (bRet, cunilogSingleThreaded	== CUNILOG_TARGETstatic.culogType);
		ubf_expect_bool_AND (bRet, cunilogPostfixDay		== CUNILOG_TARGETstatic.culogPostfix);
		ubf_expect_bool_AND (bRet, cunilogEvtTS_Default	== CUNILOG_TARGETstatic.unilogEvtTSformat);
		ubf_expect_bool_AND (bRet, 6 == CUNILOG_TARGETstatic.lnAppName);
		ubf_expect_bool_AND (bRet, !memcmp (CUNILOG_TARGETstatic.mbAppName.buf.pch, "Unilog", CUNILOG_TARGETstatic.lnAppName));

		szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);

		DoneCUNILOG_TARGETstatic ();

		pt = InitCUNILOG_TARGETstaticEx	(
					"C:/temp",	USE_STRLEN,
					"Unilog",	USE_STRLEN,
					cunilogPath_relativeToExecutable,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0,
					cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup
										);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== pt);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== &CUNILOG_TARGETstatic);
		ubf_expect_bool_AND (bRet, cunilogSingleThreaded	== CUNILOG_TARGETstatic.culogType);
		ubf_expect_bool_AND (bRet, cunilogPostfixDay		== CUNILOG_TARGETstatic.culogPostfix);
		ubf_expect_bool_AND (bRet, cunilogEvtTS_Default		== CUNILOG_TARGETstatic.unilogEvtTSformat);
		// Size is 8 + NUL.
		ubf_expect_bool_AND (bRet, 9 == CUNILOG_TARGETstatic.mbLogPath.size);
		ubf_assert	(
				!memcmp	(
					CUNILOG_TARGETstatic.mbLogPath.buf.pch,
					"C:" UBF_DIR_SEP_STR "temp" UBF_DIR_SEP_STR ASCII_NUL_STR,
					CUNILOG_TARGETstatic.mbLogPath.size
						)
					);
		ubf_assert	(
				// "C:\\temp\\Unilog".
				!memcmp	(
					CUNILOG_TARGETstatic.mbLogfileName.buf.pch,
					"C:" UBF_DIR_SEP_STR "temp" UBF_DIR_SEP_STR "Unilog",
					CUNILOG_TARGETstatic.mbLogPath.size + CUNILOG_TARGETstatic.lnAppName - 1
						)
					);
		ubf_expect_bool_AND (bRet, 6 == CUNILOG_TARGETstatic.lnAppName);
		// Should be NUL-terminated.
		ubf_expect_bool_AND (bRet, !memcmp (CUNILOG_TARGETstatic.mbAppName.buf.pch, "Unilog", CUNILOG_TARGETstatic.lnAppName + 1));

		szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);

		DoneCUNILOG_TARGETstatic ();

		pt = InitCUNILOG_TARGETstaticEx	(
					"../temp",	USE_STRLEN,
					"Unilog",	USE_STRLEN,
					cunilogPath_relativeToExecutable,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup

										);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic == pt);
		ubf_expect_bool_AND (bRet, pCUNILOG_TARGETstatic	== &CUNILOG_TARGETstatic);
		ubf_expect_bool_AND (bRet, cunilogSingleThreaded	== CUNILOG_TARGETstatic.culogType);
		ubf_expect_bool_AND (bRet, cunilogPostfixDay				== CUNILOG_TARGETstatic.culogPostfix);
		ubf_expect_bool_AND (bRet, 6 == CUNILOG_TARGETstatic.lnAppName);
		ubf_expect_bool_AND (bRet, !memcmp (CUNILOG_TARGETstatic.mbAppName.buf.pch, "Unilog", CUNILOG_TARGETstatic.lnAppName + 1));

		szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);

		DoneCUNILOG_TARGETstatic ();

		/*
			Dynamic.
		*/
		pt = CreateNewCUNILOG_TARGET (
				NULL, 0,
				NULL, 0,
				cunilogPath_relativeToExecutable,
				cunilogSingleThreaded,
				cunilogPostfixDay,
				NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
				cunilogRunProcessorsOnStartup
									);
		szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_expect_bool_AND (bRet, NULL != szAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_expect_bool_AND (bRet, 0 != lnAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_expect_bool_AND (bRet, 0 == szAbsLogPath [lnAbsLogPath]);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);
		bRet &= cunilogTargetHasRunProcessorsOnStartup (pt) ? true : false;
		DoneCUNILOG_TARGET (pt);

		pt = InitOrCreateCUNILOG_TARGET	(
				NULL,
				NULL, 0,
				"Unilog", 6,
				cunilogPath_relativeToExecutable,
				cunilogSingleThreaded,
				cunilogPostfixDay,
				NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
				cunilogDontRunProcessorsOnStartup
										);

		szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);

		DoneCUNILOG_TARGET (pt);

		pt = InitOrCreateCUNILOG_TARGET	(
				NULL,
				"C:/Temp", 7,
				"Unilog", 6,
				cunilogPath_relativeToExecutable,
				cunilogSingleThreaded,
				cunilogPostfixDay,
				NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
				cunilogDontRunProcessorsOnStartup
										);
		ubf_assert_non_NULL (pt);
		ubf_expect_bool_AND (bRet, pt != pCUNILOG_TARGETstatic);
		ubf_expect_bool_AND (bRet, cunilogSingleThreaded	== pt->culogType);
		ubf_expect_bool_AND (bRet, cunilogPostfixDay				== pt->culogPostfix);
		ubf_expect_bool_AND (bRet, 9 == pt->mbLogPath.size);				// "C:/Temp/" + NUL.
		ubf_assert	(
				!memcmp	(
					pt->mbLogPath.buf.pch,
					"C:" UBF_DIR_SEP_STR "Temp" UBF_DIR_SEP_STR ASCII_NUL_STR,
					pt->mbLogPath.size
						)
					);
		ubf_assert	(
				// "C:\\Temp\\Unilog".
				!memcmp	(
					pt->mbLogfileName.buf.pch,
					"C:" UBF_DIR_SEP_STR "Temp" UBF_DIR_SEP_STR "Unilog_",
					pt->lnLogPath + pt->lnAppName + 1
						)
					);
		ubf_expect_bool_AND (bRet, 6 == pt->lnAppName);
		ubf_expect_bool_AND (bRet, !memcmp (pt->mbAppName.buf.pch, "Unilog", pt->lnAppName));

		szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);

		DoneCUNILOG_TARGET (pt);

		// Should fail.
		CUNILOG_TARGET	cut;
		CUNILOG_TARGET	*put;
		put = InitCUNILOG_TARGET	(
				&cut,
				"temp",		USE_STRLEN,
				"OurApp",	USE_STRLEN,
				cunilogPath_isAbsolute,
				cunilogMultiThreaded
									);
		ubf_assert_NULL (put);
		ubf_expect_bool_AND (bRet, CUNILOG_ERROR_ABS_OR_REL_PATH == CunilogCunilogError (cut.error));

		/*
			Application name from executable name.
		*/
		pt = InitOrCreateCUNILOG_TARGET	(
				NULL,
				"C:/Temp", 7,
				NULL, 0,
				cunilogPath_relativeToExecutable,
				cunilogSingleThreaded,
				cunilogPostfixDay,
				NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
				cunilogDontRunProcessorsOnStartup
										);
		SMEMBUF mb;
		initSMEMBUF (&mb);
		ObtainAppNameFromExecutableModule (&mb);
		ubf_expect_bool_AND (bRet, !memcmp (pt->mbAppName.buf.pch, mb.buf.pch, strlen (mb.buf.pch)));
		ubf_assert_non_NULL (pt);
		ubf_expect_bool_AND (bRet, pt != pCUNILOG_TARGETstatic);
		ubf_expect_bool_AND (bRet, cunilogSingleThreaded	== pt->culogType);
		ubf_expect_bool_AND (bRet, cunilogPostfixDay		== pt->culogPostfix);
		ubf_expect_bool_AND (bRet, 9 == pt->mbLogPath.size);
		ubf_expect_bool_AND (bRet, 8 == pt->lnLogPath);
		ubf_assert	(
				!memcmp	(
					pt->mbLogPath.buf.pch,
					"C:" UBF_DIR_SEP_STR "Temp" UBF_DIR_SEP_STR ASCII_NUL_STR,
					pt->mbLogPath.size
						)
					);

		szAbsLogPath = GetAbsoluteLogPathCUNILOG_TARGET (pt, &lnAbsLogPath);
		ubf_assert_non_NULL (szAbsLogPath);
		ubf_assert_non_0 (lnAbsLogPath);
		ubf_assert_0 (szAbsLogPath [lnAbsLogPath]);

		DoneCUNILOG_TARGET (pt);

		return bRet;
	}
#endif
/****************************************************************************************

	File:		bottom.c
	Why:		Provides the bottom/foot of the combined cunilog.c code file.
	OS:			C99.
	Author:		Thomas
	Created:	2025-01-14

History
-------

When		Who				What
-----------------------------------------------------------------------------------------
2025-01-14	Thomas			Created.

****************************************************************************************/

#endif														// Of #ifndef CUNILOG_USE_LIBCUNILOG.
