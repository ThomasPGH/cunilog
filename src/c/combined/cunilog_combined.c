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
					p = ubf_malloc (siz);
				}
			} else
			{
				ubf_free (pb->buf.puc);
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

#ifdef DEBUG
	void doneSMEMBUF (SMEMBUF *pb)
	{
		ubf_assert_non_NULL (pb);
		ubf_assert_non_NULL (pb->buf.pvoid);

		if (pb->buf.pvoid)
		{
			ubf_assert (0 < pb->size);

			ubf_free (pb->buf.pvoid);
			initSMEMBUF (pb);
		}
	}
#endif

#ifdef DEBUG
	void freeSMEMBUF (SMEMBUF *pb)
	{
		ubf_assert_non_NULL (pb);
		ubf_assert_non_NULL (pb->buf.pvoid);

		if (pb->buf.pvoid)
		{
			ubf_assert (0 < pb->size);

			ubf_free (pb->buf.pvoid);
		}
		// Anything is Ok apart from 'A'.
		pb->initComplete = '#';
	}
#endif

void copySMEMBUF (SMEMBUF *dst, SMEMBUF *src)
{
	ubf_assert_non_NULL (dst);
	ubf_assert_non_NULL (src);
	ubf_assert (isInitialisedSMEMBUF (dst));
	ubf_assert (isInitialisedSMEMBUF (src));

	growToSizeSMEMBUF (dst, src->size);
	if (isUsableSMEMBUF (dst))
	{
		memcpy (dst->buf.pch, src->buf.pch, src->size);
	}
}

void copySMEMBUFsiz (SMEMBUF *dst, SMEMBUF *src, size_t siz)
{
	ubf_assert_non_NULL (dst);
	ubf_assert_non_NULL (src);
	ubf_assert (isInitialisedSMEMBUF (dst));
	ubf_assert (isInitialisedSMEMBUF (src));

	growToSizeSMEMBUF (dst, siz);
	if (isUsableSMEMBUF (dst))
	{
		memcpy (dst->buf.pch, src->buf.pch, siz);
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

#ifdef DEBUG
	int reqUTF8size (const WCHAR *wcU16)
	{
		return WideCharToMultiByte (CP_UTF8, 0, wcU16, -1, NULL, 0, NULL, NULL);
	}
#endif

#ifdef DEBUG
	int UTF8_from_WinU16 (char *chU8, int sizeU8, const WCHAR *wcU16)
	{
		return WideCharToMultiByte (CP_UTF8, 0, wcU16, -1, chU8, sizeU8, NULL, NULL);
	}
#endif

#ifdef DEBUG
	int reqWinU16wchars (const char *chU8)
	{
		return MultiByteToWideChar (CP_UTF8, 0, chU8, -1, NULL, 0);
	}
#endif

#ifdef DEBUG
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

#ifdef DEBUG
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

#ifdef DEBUG
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

#ifdef DEBUG
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
		stPath -= stWCflen - 1;								// We keep the dir separator.
		memcpy (ccRet, ccPath, stPath);
		UTF8_from_WinU16 (ccRet + stPath, iReq, wcFileName);
		if (ccWCfname && stWCflen)
		{	// We got a mask that needs to be added.
			memcpy (ccRet + stPath + iReq - 1, ccWCfname, stWCflen + 1);
		}
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
		char		*ccRet		= NULL;
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

#ifdef DEBUG
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
	#ifdef DEBUG
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

WCHAR **CreateWin16tringArrayFromU8 (const char **pu8, WORD numStrings)
{
	WCHAR		**pwcRet;
	
	pwcRet = ubf_malloc (numStrings * sizeof (WCHAR *));
	if (pwcRet)
	{
		WORD	n	= 0;
		while (n < numStrings)
		{
			pwcRet [n] = AllocWinU16_from_UTF8 (pu8 [n]);
			++ n;
		}
	}
	return pwcRet;
}

void DoneWin16StringArray (WCHAR **pwcStringArray, WORD numStrings)
{
	WORD n = 0;
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

#ifdef DEBUG
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
	bRet = DecryptFileW (pwcFileName, dwReserved);
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

BOOL IsFileSystemFAT (const char *chDriveRoot)
{
	return FS_TYPE_FAT == GetFileSystemType (chDriveRoot);
}

BOOL IsFileSystemFAT32 (const char *chDriveRoot)
{
	return FS_TYPE_FAT32 == GetFileSystemType (chDriveRoot);
}

BOOL IsFileSystemFATorFAT32 (const char *chDriveRoot)
{
	return IsFileSystemFAT32 (chDriveRoot) || IsFileSystemFAT (chDriveRoot);
}

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
	int		iu8;
	
	dwRet = GetModuleFileNameW (hModule, wcModuleName, WINAPI_U8_HEAP_THRESHOLD);
	if (dwRet == WINAPI_U8_HEAP_THRESHOLD)
	{	// Stack buffer (WINAPI_U8_HEAP_THRESHOLD) too small.
		ASSERT (FALSE);
	}
	iu8 = reqUTF8size (wcModuleName);
	ASSERT (0 < iu8);
	dwRet = (DWORD) iu8;
	if (dwRet < nSize)
	{	// Success.
		UTF8_from_WinU16 (lpFilenameU8, iu8, wcModuleName);
		// I can't think of any quicker way to obtain the length
		//	to return, so let's use strlen ().
		return (DWORD) (strlen (lpFilenameU8) & 0xFFFFFFFF);
	}
	// Buffer provided by the user too small.
	#ifdef GETMODULEFILENAMEU8_ASSERT_BUFFER_TOO_SMALL
		ASSERT (FALSE);
	#endif
	UTF8_from_WinU16 (lpFilenameU8, (int) nSize, wcModuleName);
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
	#if UNLEN < WINAPI_U8_HEAP_THRESHOLD
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
	#else
		// Not supported.
		#error UNLEN > WINAPI_U8_HEAP_THRESHOLD not supported
		ASSERT (FALSE);
		return FALSE;
	#endif
}
#endif

#ifdef HAVE_USERENV
BOOL GetDefaultUserProfileDirectoryU8(
	LPSTR   lpProfileDirU8,
	LPDWORD lpcchSize
)
{
	ASSERT (NULL != lpcchSize);

	BOOL bRet = false;
	WCHAR wcProfDir [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pcProfDir;

	DWORD	dw = 0;
	GetDefaultUserProfileDirectoryW (NULL, &dw);
	if (dw <= WINAPI_U8_HEAP_THRESHOLD)
		pcProfDir = wcProfDir;
	else
		pcProfDir = malloc (dw);
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

	BOOL bRet = false;
	WCHAR wcProfDir [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pcProfDir;

	DWORD	dw = 0;
	GetUserProfileDirectoryW (hToken, NULL, &dw);
	if (dw <= WINAPI_U8_HEAP_THRESHOLD)
		pcProfDir = wcProfDir;
	else
		pcProfDir = malloc (dw);
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
	DWORD	dwRet;
	WCHAR	wcErrText [WINAPI_U8_HEAP_THRESHOLD];

	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/stringapiset/nf-stringapiset-multibytetowidechar .
	//iReqSize = MultiByteToWideChar (CP_UTF8, 0, lpRootPathName, -1, NULL, 0);

	// See
	//	https://docs.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-formatmessagew .
	dwRet = FormatMessageW	(
					FORMAT_MESSAGE_FROM_SYSTEM
				|	FORMAT_MESSAGE_ARGUMENT_ARRAY
				|	FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dwError,
				0,
				wcErrText,
				WINAPI_U8_HEAP_THRESHOLD - 1,
				NULL
							);
	if (dwRet && dwRet > 2)
    {	// Remove the line ending (CR/LF).
		wcErrText [dwRet - 2] = L'\0';
		// Convert to UTF-8.
		iReqSize = reqUTF8size (wcErrText);
		if (iReqSize <= WINAPI_U8_HEAP_THRESHOLD)
		{
			char	chU8 [WINAPI_U8_HEAP_THRESHOLD];
			UTF8_from_WinU16 (chU8, iReqSize, wcErrText);
			snprintf (lpszBuf, dwSize, "\"%s\" (Error %lu, 0%lXh)", chU8, dwError, dwError);
			return TRUE;
		}
    }
	_ASSERT (FALSE);
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
	UNREFERENCED_PARAMETER (enHow);
	UNREFERENCED_PARAMETER (nRelevant);
	ASSERT (NULL != wcArgument);
	ASSERT (NULL != wcConstIs);
	ASSERT (0 != wcArgument [0]);
	ASSERT (0 != wcConstIs [0]);
	if (EN_IS_CMD_ARG_ALLOW_NO_DASH_ONLY == enCase && L'-' == wcConstIs [0])
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

HMODULE LoadLibraryU8(
  char *chLibFileNameU8
)
{
	HANDLE	hModule	= NULL;

	WCHAR *pwc = AllocWinU16_from_UTF8 (chLibFileNameU8);
	if (pwc)
	{
		hModule = LoadLibraryW (pwc);
		DoneWinU16 (pwc);
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
	BOOL SendSysCommandWPARAMtoThisConsole (WPARAM wparam)
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
		return SendSysCommandWPARAMtoThisConsole (SC_MAXIMIZE);
	}
#endif

#ifdef HAVE_WINUSER
	BOOL MiniMiseThisConsoleWindow (void)
	{
		return SendSysCommandWPARAMtoThisConsole (SC_MINIMIZE);
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

#ifdef HAVE_WINUSER
	BOOL RestoreThisConsoleWindow (void)
	{
		return SendSysCommandWPARAMtoThisConsole (SC_RESTORE);
	}
#endif

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

BOOL SetCurrentDirectoryU8(
  const char *lpPathNameU8
)
{
	WCHAR wcPathName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcPathName = AllocWinU16fromU8orUseThreshold (wcPathName, lpPathNameU8);
	BOOL bRet = SetCurrentDirectoryW (pwcPathName);
	DoneWinU16fromU8orUseThreshold (pwcPathName, wcPathName);
	return bRet;
}

BOOL SetCurrentDirectoryU8long(
  const char *lpPathNameU8
)
{
	WCHAR wcPathName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcPathName = AllocWinU16fromU8orUseThresholdLongFileName (wcPathName, lpPathNameU8);
	BOOL bRet = SetCurrentDirectoryW (pwcPathName);
	DoneWinU16fromU8orUseThreshold (pwcPathName, wcPathName);
	return bRet;
}

DWORD SetFileAttributesU8(
  LPCSTR	lpFileNameU8,
  DWORD		dwFileAttributes
)
{
	WCHAR wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcFileName = AllocWinU16fromU8orUseThreshold (wcFileName, lpFileNameU8);
	DWORD dwRet = SetFileAttributesW (pwcFileName, dwFileAttributes);
	DoneWinU16fromU8orUseThreshold (pwcFileName, wcFileName);
	return dwRet;
}

DWORD SetFileAttributesU8long(
  LPCSTR	lpFileNameU8,
  DWORD		dwFileAttributes
)
{
	WCHAR wcFileName [WINAPI_U8_HEAP_THRESHOLD];
	WCHAR *pwcFileName = AllocWinU16fromU8orUseThresholdLongFileName (wcFileName, lpFileNameU8);
	DWORD dwRet = SetFileAttributesW (pwcFileName, dwFileAttributes);
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

int fprintfU8 (FILE *stream, const char *format, ...)
{
	int			iReq;
	int			iRet		= -1;
	va_list		args;

	va_start (args, format);
	// Returns the required buffer size without terminating NUL.
	iReq = vsnprintf (NULL, 0, format, args);
	va_end (args);

	WCHAR	wcToPrint	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcToPrint;

	if (iReq < WINAPI_U8_HEAP_THRESHOLD)
	{
		char	szToPrint	[WINAPI_U8_HEAP_THRESHOLD];

		va_start (args, format);
		vsnprintf (szToPrint, WINAPI_U8_HEAP_THRESHOLD, format, args);
		va_end (args);
		pcToPrint = AllocWinU16fromU8orUseThreshold (wcToPrint, szToPrint);
		iRet = fwprintf (stream, pcToPrint);
		DoneWinU16fromU8orUseThreshold (pcToPrint, wcToPrint);
	} else
	{
		char	*szToPrint = ubf_malloc (((size_t) iReq) + 1);

		if (szToPrint)
		{
			pcToPrint = AllocWinU16fromU8orUseThreshold (wcToPrint, szToPrint);
			iRet = fwprintf (stream, pcToPrint);
			DoneWinU16fromU8orUseThreshold (pcToPrint, wcToPrint);
			ubf_free (szToPrint);
		}
	}
	return iRet;
}

int putsU8 (const char *strU8)
{
	WCHAR	wcToPrint	[WINAPI_U8_HEAP_THRESHOLD];
	WCHAR	*pcToPrint;
	int		iRet;

	pcToPrint = AllocWinU16fromU8orUseThreshold (wcToPrint, strU8);
	iRet = _putws (pcToPrint);
	DoneWinU16fromU8orUseThreshold (pcToPrint, wcToPrint);
	return iRet;
}

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
		#include "./ubfmem.h"
		#include "./strisdotordotdot.h"
		#include "./ubfdebug.h"
	#else
		#include "./../../mem/ubfmem.h"
		#include "./../../string/strisdotordotdot.h"
		#include "./../../dbg/ubfdebug.h"
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
uint64_t ForEachDirectoryEntryU8_Ex	(
				const char				*strPathU8,
				pForEachDirEntryU8		fedEnt,
				void					*pCustom,
				unsigned int			*pnSubLevels,
				SMEMBUF                 *pmb
									)
{
	HANDLE				hFind;
	WIN32_FIND_DATAW	wfdW;
	uint64_t			uiEnts				= 0;			// The return value.
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
					unsigned int nSubLevels = *pnSubLevels;
					uiEnts += ForEachDirectoryEntryU8_Ex	(
                                            pmb->buf.pch,
                                            fedEnt,
                                            pCustom,
                                            &nSubLevels,
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

uint64_t ForEachDirectoryEntryU8	(
				const char				*strPathU8,
				pForEachDirEntryU8		fedEnt,
				void					*pCustom,
				unsigned int			*pnSubLevels
									)
{
	HANDLE				hFind;
	WIN32_FIND_DATAW	wfdW;
	uint64_t			uiEnts				= 0;			// The return value.
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
					unsigned int nSubLevels = *pnSubLevels;
					uiEnts += ForEachDirectoryEntryU8	(
									subPath,
									fedEnt,
									pCustom,
									&nSubLevels
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
		char	*szExe		= (char *) &dw;						// Dummy value.

		while (szExe)
		{
			szExe = malloc (dwCurrSiz);
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
						free (szExe);
						return dw;
					}
				}
				free (szExe);
				if (0 == dw)
				{
					ubf_assert (false);
					return false;
				}
				dwCurrSiz += EXEFILENAME_MALLOC_BLOCKSIZE;
			}
		}
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

			enfilecompressresult r = IsFileNTFSCompressedByName (szFilename);
			ubf_assert (fscompress_compressed	== ntfscompress_compressed);
			ubf_assert (fscompress_uncompressed	== ntfscompress_uncompressed);
			ubf_assert (fscompress_error		== ntfscompress_error);
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
		bool			b		= false;
		SBULKMEM		sbm		= SBULKMEM_INITIALISER (128);
		//SBULKMEM		*pbm;
		void			*p;
		SBULKMEMBLOCK	*pbmb;

		// The SBULKMEM structure doesn't have any memory block attached.
		p = GetUnalignedMemFromSBULKMEMnogrow (&sbm, 1);
		ubf_assert_NULL (p);
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 1);
		ubf_assert_NULL (p);
		p = GetUnalignedMemFromSBULKMEMnogrow (&sbm, 1024);
		ubf_assert_NULL (p);
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 1024);
		ubf_assert_NULL (p);
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 1022);
		ubf_assert_NULL (p);

		pbmb = GrowSBULKMEM (&sbm, 0);
		ubf_assert_non_NULL (pbmb);
		pbmb = GrowSBULKMEM (&sbm, 12);
		ubf_assert_non_NULL (pbmb);
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 1022);
		ubf_assert_NULL (p);
		// This should succeed.
		p = GetAlignedMemFromSBULKMEMnogrow (&sbm, 16);
		ubf_assert_non_NULL (p);
		p = GetAlignedMemFromSBULKMEMgrow (&sbm, 12);
		ubf_assert_non_NULL (p);

		DoneSBULKMEM (&sbm);

		p = GetAlignedMemFromSBULKMEMgrow (&sbm, 1024);
		ubf_assert_non_NULL (p);
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

		puts ("Starting...");
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
		printf ("Diff - malloc (): %" PRIu64 " ms.\n", ut3);

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
			printf ("Diff - memblock:  %" PRIu64 " ms.\n", ut3);
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

/*
	Note: These comments are only here for convenience. Refer to the
	 ones in memstrstr.h. They might be more accurate.

	Replaces all occurrences of s1 in s with s2.
*/
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
	#include "./../pre/unref.h"

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./strintuint.h"
		#include "./ubfdebug.h"
	#else
		#include "./../string/strintuint.h"
		#include "./../dbg/ubfdebug.h"
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

void cpyFILETIME (FILETIME *fttarget, FILETIME *ftsource)
{
	ubf_assert_non_NULL (fttarget);
	ubf_assert_non_NULL (ftsource);

	fttarget->dwHighDateTime	= ftsource->dwHighDateTime;
	fttarget->dwLowDateTime		= ftsource->dwLowDateTime;
}

int cmpFILETIME (FILETIME *ft1, FILETIME *ft2)
{
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

bool IsLeapYear (uint32_t Y)
{	// Implements (2) and (3). See below.
	return (((Y % 4 == 0) && (Y % 100 != 0)) || (Y % 400 == 0)) ? true : false;
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

void ISO8601_from_UBF_TIMESTAMP_s (char *chISO, UBF_TIMESTAMP ts)
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

void ISO8601_from_UBF_TIMESTAMP_c (char *chISO, UBF_TIMESTAMP ts)
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

void ISO8601T_from_UBF_TIMESTAMP_s (char *chISO, UBF_TIMESTAMP ts)
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

void ISO8601T_from_UBF_TIMESTAMP_c (char *chISO, UBF_TIMESTAMP ts)
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

void ISO8601Date_from_UBF_TIMESTAMP (char *chISODateOnly, UBF_TIMESTAMP ts)
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

void ISO8601Date_from_UBF_TIMESTAMP_c (char *chISODateOnly, UBF_TIMESTAMP ts)
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

void ISO8601YearAndWeek_from_UBF_TIMESTAMP_s (char *chISO8601YearAndWeek, UBF_TIMESTAMP ts)
{
	unsigned int	week;
	uint32_t		newYear;

	week = GetISO8601WeekNumberFromDate	(
		UBF_TIMESTAMP_YEAR (ts), UBF_TIMESTAMP_MONTH (ts), UBF_TIMESTAMP_DAY (ts), &newYear
										);
	snprintf (chISO8601YearAndWeek, SIZ_ISO8601YEARANDWEEK, "%4.4u-W%2.2u", (unsigned int) newYear, week);
}

void ISO8601YearAndWeek_from_UBF_TIMESTAMP_c (char *chISO8601YearAndWeek, UBF_TIMESTAMP ts)
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

void ISO8601DateAndHour_from_UBF_TIMESTAMP_s (char *chISODateAndHour, UBF_TIMESTAMP ts)
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

void ISO8601DateAndHour_from_UBF_TIMESTAMP_c (char *chISODateAndHour, UBF_TIMESTAMP ts)
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

void ISO8601TDateAndHour_from_UBF_TIMESTAMP_s (char *chISODateAndHour, UBF_TIMESTAMP ts)
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

void ISO8601TDateAndHour_from_UBF_TIMESTAMP_c (char *chISODateAndHour, UBF_TIMESTAMP ts)
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

void ISO8601DateHourAndMinute_from_UBF_TIMESTAMP_s (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh
	// YYYY-MM-DD HH
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

void ISO8601DateHourAndMinute_from_UBF_TIMESTAMP_c (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh
	// YYYY-MM-DD HH
	/*
	snprintf	(
		chISODateHourAndMinute, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u-%2.2u-%2.2u %2.2u:%2.2u",
		(unsigned int) UBF_TIMESTAMP_YEAR (ts),
		(unsigned int) UBF_TIMESTAMP_MONTH (ts),
		(unsigned int) UBF_TIMESTAMP_DAY (ts),
		(unsigned int) UBF_TIMESTAMP_HOUR (ts),
		(unsigned int) UBF_TIMESTAMP_MINUTE (ts)
				);
	*/

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

void ISO8601TDateHourAndMinute_from_UBF_TIMESTAMP_s (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh
	// YYYY-MM-DD HH
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

void ISO8601TDateHourAndMinute_from_UBF_TIMESTAMP_c (char *chISODateHourAndMinute, UBF_TIMESTAMP ts)
{
	// yyyy-mm-dd hh
	// YYYY-MM-DD HH

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

void ISO8601Year_from_UBF_TIMESTAMP (char *chYear, UBF_TIMESTAMP ts)
{
	// YYYY
	snprintf	(
		chYear, SIZ_ISO8601DATETIMESTAMPMS,
		"%4.4u", (unsigned int) UBF_TIMESTAMP_YEAR (ts)
				);
}

void ISO8601Year_from_UBF_TIMESTAMP_c (char *chYear, UBF_TIMESTAMP ts)
{
	// YYYY
	ubf_str0_from_uint16 (chYear, 4, (unsigned int) UBF_TIMESTAMP_YEAR (ts));
}

void ISO8601YearAndMonth_from_UBF_TIMESTAMP (char *chISOYearAndMonth, UBF_TIMESTAMP ts)
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

void ISO8601YearAndMonth_from_UBF_TIMESTAMP_c (char *chISOYearAndMonth, UBF_TIMESTAMP ts)
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

void UBF_TIMESTAMP_to_ISO8601_no_ms_Holocene (char *chISO, UBF_TIMESTAMP ts)
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
	void Test_ubf_times_functions (void)
	{
		char			ch [FORMATTEDMILLISECONDS_SIZE];
		UBF_DEF_GUARD_VAR (var, "0123456789");
		uint64_t		u;
		bool			b;
		UBF_TIMESTAMP	ut;
		UBF_TIMESTAMP	ut2;
		SUBF_TIMESTRUCT	ts;
		char			cOut [1024];

		GetSystemTime_UBF_TIMESTAMP (&ut);
		ubf_assert (IsBuildYearOrNewer_UBF_TIMESTAMP (&ut));

		u = 1000;
		while (u --)
		{
			GetSystemTime_UBF_TIMESTAMP (&ut);
			SUBF_TIMESTRUCT_from_UBF_TIMESTAMP (&ts, ut);
			UBF_TIMESTAMP_from_SUBF_TIMESTRUCT (&ut2, &ts);
			ubf_assert (ut == ut2);
			GetLocalTime_UBF_TIMESTAMP (&ut);
			SUBF_TIMESTRUCT_from_UBF_TIMESTAMP (&ts, ut);
			UBF_TIMESTAMP_from_SUBF_TIMESTRUCT (&ut2, &ts);
			ubf_assert (ut == ut2);
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
		ubf_assert (0x84 == ut);
		ts.uOffsetHours			= 1;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		// 14h = 1 for uOffsetHours, 4 for 30 offset minutes.
		ubf_assert (0x14 == ut);
		u = UBF_TIMESTAMP_OFFSETMINUTES (ut);
		ubf_assert (30 == u);
		ts.uOffsetHours			= 0;
		ts.uOffsetMinutes		= 45;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		u = UBF_TIMESTAMP_OFFSETMINUTES (ut);
		ubf_assert (45 == u);
		ts.uOffsetMinutes		= 15;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		u = UBF_TIMESTAMP_OFFSETMINUTES (ut);
		ubf_assert (15 == u);
		ts.uOffsetMinutes		= 0;
		ut = UBF_TIMESTAMP_OFFSET_BITS (&ts);
		u = UBF_TIMESTAMP_OFFSETMINUTES (ut);
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
		ubf_assert (44 == uWeek);
		ubf_assert (2022 == uYear);
		ISO8601YearAndWeek_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601Year_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601Year_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601DateAndHour_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11-04 16", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601DateAndHour_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11-04 16", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11-04 16:20:07.400+08:30", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11-04 16:20:07.400+08:30", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndWeek_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndWeek_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndMonth_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11", SIZ_ISO8601YEARANDMONTH));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndMonth_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11", SIZ_ISO8601YEARANDMONTH));

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
		ISO8601Year_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601Year_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601DateAndHour_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11-04 16", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601DateAndHour_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11-04 16", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11-04 16:20:07.400-08:45", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-11-04 16:20:07.400-08:45", SIZ_ISO8601YEAR));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndWeek_from_UBF_TIMESTAMP (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));
		memset (cOut, 0, SIZ_ISO8601DATETIMESTAMPMS);
		ISO8601YearAndWeek_from_UBF_TIMESTAMP_c (cOut, ut);
		ubf_assert (!memcmp (cOut, "2022-W44", SIZ_ISO8601YEARANDWEEK));

		u = 0;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "00:00:00.000", FORMATTEDMILLISECONDS_SIZE));
		u = 500;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "00:00:00.500", FORMATTEDMILLISECONDS_SIZE));
		u = 999;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "00:00:00.999", FORMATTEDMILLISECONDS_SIZE));
		u = 1999;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "00:00:01.999", FORMATTEDMILLISECONDS_SIZE));
		u = 59999;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "00:00:59.999", FORMATTEDMILLISECONDS_SIZE));
		u = 50 * 1000 * 3600;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "50:00:00.000", FORMATTEDMILLISECONDS_SIZE));
		u = 99 * 1000 * 3600;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "99:00:00.000", FORMATTEDMILLISECONDS_SIZE));
		u = (99 * 1000 * 3600) + (1000 * 59);
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "99:00:59.000", FORMATTEDMILLISECONDS_SIZE));
		u = (99 * 1000 * 3600) + (1000 * 59 * 60) + (1000 * 59);
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "99:59:59.000", FORMATTEDMILLISECONDS_SIZE));
		u = (99 * 1000 * 3600) + (1000 * 59 * 60) + (1000 * 59) + 999;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (true == b);
		ubf_assert (!memcmp (ch, "99:59:59.999", FORMATTEDMILLISECONDS_SIZE));
		// Should cause an overflow and lead to a return value of FALSE.
		u = (99 * 1000 * 3600) + (1000 * 59 * 60) + (1000 * 59) + 999 + 1;
		b = FormattedMilliseconds (ch, u);
		UBF_TST_GUARD_VAR (var, "0123456789");
		ubf_assert (false == b);
		// Should not have been touched by the function due to it returning FALSE.
		ubf_assert (!memcmp (ch, "99:59:59.999", FORMATTEDMILLISECONDS_SIZE));

		// Test bit constants.
		UBF_TIMESTAMP	stamp;
		UBF_TIMESTAMP	maskd;
		GetLocalTime_UBF_TIMESTAMP (&stamp);
		uint64_t uio1 = UBF_TIMESTAMP_OFFSETMINUTES (stamp);
		uint64_t uio2 = UBF_TIMESTAMP_OFFSETMINUTES (stamp & UBF_TIMESTAMP_KEEP_OFFSET_BITS);
		ubf_assert (uio1 == uio2);
		uio1 = UBF_TIMESTAMP_OFFSETHOURS (stamp);
		uio2 = UBF_TIMESTAMP_OFFSETHOURS (stamp & UBF_TIMESTAMP_KEEP_OFFSET_BITS);
		ubf_assert (uio1 == uio2);
		uio1 = UBF_TIMESTAMP_SECOND (stamp);
		uio2 = UBF_TIMESTAMP_SECOND (stamp & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS);
		ubf_assert (uio1 == uio2);
		uio1 = UBF_TIMESTAMP_MINUTE (stamp);
		uio2 = UBF_TIMESTAMP_MINUTE (stamp & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS);
		ubf_assert (uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS;
		ubf_assert_0 (UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		uio1 = UBF_TIMESTAMP_HOUR (stamp);
		uio2 = UBF_TIMESTAMP_HOUR (stamp & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS);
		ubf_assert (uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS;
		ubf_assert_0 (UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		uio1 = UBF_TIMESTAMP_DAY (stamp);
		uio2 = UBF_TIMESTAMP_DAY (stamp & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS);
		ubf_assert (uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS;
		ubf_assert_0 (UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_HOUR (maskd & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS));
		uio1 = UBF_TIMESTAMP_MONTH (stamp);
		uio2 = UBF_TIMESTAMP_MONTH (stamp & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS);
		ubf_assert (uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS;
		ubf_assert_0 (UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_HOUR (maskd & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_DAY (maskd & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS));
		uio1 = UBF_TIMESTAMP_YEAR (stamp);
		uio2 = UBF_TIMESTAMP_YEAR (stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS);
		ubf_assert (uio1 == uio2);
		maskd = stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS;
		ubf_assert_0 (UBF_TIMESTAMP_SECOND (maskd & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_MINUTE (maskd & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_HOUR (maskd & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_DAY (maskd & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS));
		ubf_assert_0 (UBF_TIMESTAMP_MONTH (maskd & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS));

		char sz1 [1024];
		char sz2 [1024];

		GetISO8601Week_s (sz1);
		GetISO8601Week_c (sz2);
		if (memcmp (sz1, sz2, SIZ_ISO8601WEEK))
		{	// We assume the test ran just the moment the week number changed.
			GetISO8601Week_s (sz1);
			GetISO8601Week_c (sz2);
			ubf_assert (!memcmp (sz1, sz2, SIZ_ISO8601WEEK));
		} else
		{
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
		ISO8601_from_UBF_TIMESTAMP_c	(szTS2, tt);
		ubf_assert (!memcmp (szTS1, szTS2, SIZ_ISO8601DATETIMESTAMPMS));

		tt = LocalTime_UBF_TIMESTAMP ();
		ISO8601T_from_UBF_TIMESTAMP  (szTS1, tt);
		ISO8601T_from_UBF_TIMESTAMP_c (szTS2, tt);
		ubf_assert (!memcmp (szTS1, szTS2, SIZ_ISO8601DATETIMESTAMPMS));

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
		ubf_assert_0 (memcmp (szISO_1, szISO_2, LEN_ISO8601DATETIMESTAMP_NO_OFFS));
		ubf_assert ('X'		== szISO_1 [LEN_ISO8601DATETIMESTAMP_NO_OFFS]);
		ubf_assert ('\0'	== szISO_2 [LEN_ISO8601DATETIMESTAMP_NO_OFFS]);

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

static const char *ccMonths [12] =	{
			"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
									};
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
		if (!memcmp (ccMonths [m], cc__DATE__, 3))
			break;
	}
	// Year, like "2024".
	memcpy (szISO__DATE__, cc__DATE__ + 7, 4);
	szISO__DATE__ [4] = '-';
	// Month, like "10".
	memcpy (szISO__DATE__ + 5, ccDigMns [m], 2);
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

#ifdef DEBUG
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

#ifdef DEBUG
	#ifdef UBF_USE_DBG_ABRT_OUTPUT_FNCTS
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

	#ifdef UBF_USE_DBG_ABRT_OUTPUT_FNCTS
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

	#ifdef UBF_USE_DBG_ABRT_OUTPUT_FNCTS
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
		#include "./ubfdebug.h
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
			SDBGCOUNTANDTRACK *t = malloc (sizeof (SDBGCOUNTANDTRACK) * (pt->size + DBGCOUNTANDTRACK_RESERVE_NUM));
			if (t)
			{
				if (pt->size)
				{
					memcpy (t, pt->trackers, sizeof (SDBGCOUNTANDTRACK) * pt->size);
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

bool check_utf8(const char *str, size_t len)
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
		#include "./memstrstr.h"
		#include "./ubfdebug.h"
		#include "./unref.h"
	#else
		#include "./../mem/memstrstr.h"
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

	size_t	idx;
	
	while (len)
	{
		idx = 0;
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
		cp	+= cc - cp + pb->psstrcustfmt [idx].lenFormatSpecifier;
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
		len	-= cc - cp;
		cp	+= cc - cp + pb->psstrcustfmt [idx].lenFormatSpecifier;
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

		const char *ccCustomFormatArgumentExpanded = pCustFmtArg;
		SSTRCUSTFMT *p = pSSTRCUSTFMT;
		size_t size = *(size_t *)p->pCustom;
		size_t s2 = strlen (ccCustomFormatArgumentExpanded) + 1;
		ubf_assert (s2 == size);
		ubf_assert (s2 <= len);
		memcpy (szBuf, ccCustomFormatArgumentExpanded, s2);
		return size - 1;
	}

	void TestStrCustomFmt (void)
	{
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
		cc = strcustomfmtFindNextCust (&idx, &b, "{a}", 3);
		ubf_assert (!memcmp ("{a}", cc, 4));
		cc = strcustomfmtFindNextCust (&idx, &b, "   ", 3);
		ubf_assert_NULL (cc);
		cc = strcustomfmtFindNextCust (&idx, &b, "#1", 2);
		ubf_assert (!memcmp ("#1", cc, 2));

		// Let's check the callback functions.
		f [0].getRqSize				= ourReqSize;
		f [0].storeData				= ourStoData;
		size_t s1 = strcustomfmtGetRqSizeCust (&b, 3, "{a}", NULL);
		char ch [256];
		size_t s2 = strcustomfmtStoreDataCust (ch, 256, &b, 3, "{a}", NULL);
		ubf_assert (s1 == s2);
		ubf_assert (!memcmp (ch, "0123456789", 10));

		// A bit more complicated.
		f [1].getRqSize				= ourReqSize;
		f [1].storeData				= ourStoData;
		memset (ch, 0, 256);
		char	x = 0;
		s1 = strcustomfmtGetRqSizeCust (&b, USE_STRLEN, "None {a} // #1, {a}xxx", NULL, &x, NULL);
		s2 = strcustomfmtStoreDataCust (ch, 256, &b, USE_STRLEN, "None {a} // #1, {a}xxx", NULL, &x, NULL);
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

bool ubf_correct_directory_separators_str (char *chPath, size_t *plenPath, size_t *pReps)
{
	size_t			stRet	= 0;
	size_t			st;
	size_t			stLen;
	char			*ch		= chPath;
	char			ds [3]	= {UBF_DIR_SEP, UBF_DIR_SEP};		// Double separator.
	
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

void ubf_change_directory_separators (char *szPath, size_t len, char newSeparator)
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
	
	c1 = ubf_value_of_ASCII_hex (*chHx ++);
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
	
	c2 = ubf_value_of_ASCII_oct (*chOc ++);
	if (UBF_INVALID_OCT_CHAR == c2)
	{
		return 0;
	}
	ubf_assert (0 == ('\xF8' & c2));
	c1 = ubf_value_of_ASCII_oct (*chOc ++);
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
		uct = ubf_value_of_ASCII_bin (*chOb ++);
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

	memcpy (pc, &binASCII [((ui & 0xF0) >> 4) & 0x0F],		4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x0F))], 4);
}

void asc_bin_from_word (char *pc, uint16_t ui)
{
	ubf_assert_non_NULL (pc);

	memcpy (pc, &binASCII [((ui & 0xF000) >> 12) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x0F00) >>  8) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x00F0) >>  4) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x000F))],				4);
}

void asc_bin_from_dword (char *pc, uint32_t ui)
{
	ubf_assert_non_NULL (pc);

	memcpy (pc, &binASCII [((ui & 0xF0000000) >> 28) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x0F000000) >> 24) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x00F00000) >> 20) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x000F0000) >> 16) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x0000F000) >> 12) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x00000F00) >>  8) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x000000F0) >>  4) & 0x0F],	4);		pc += 4;
	memcpy (pc, &binASCII [((ui & 0x0000000F))],				4);
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
	void ubf_hex_test_function (void)
	{
		char			chT [32];
		unsigned char	o;
		size_t			st;

		ubf_assert (ubf_is_octal_digit ('0'));
		ubf_assert (ubf_is_octal_digit ('1'));
		ubf_assert (ubf_is_octal_digit ('2'));
		ubf_assert (ubf_is_octal_digit ('3'));
		ubf_assert (ubf_is_octal_digit ('4'));
		ubf_assert (ubf_is_octal_digit ('5'));
		ubf_assert (ubf_is_octal_digit ('6'));
		ubf_assert (ubf_is_octal_digit ('7'));
		ubf_assert (!ubf_is_octal_digit ('8'));
		ubf_assert (!ubf_is_octal_digit ('9'));
		ubf_assert (!ubf_is_octal_digit ('A'));
		ubf_assert (!ubf_is_octal_digit ('Z'));
		ubf_assert (!ubf_is_octal_digit ('.'));
		
		ubf_assert (ubf_is_hex_digit ('0'));
		ubf_assert (ubf_is_hex_digit ('1'));
		ubf_assert (ubf_is_hex_digit ('2'));
		ubf_assert (ubf_is_hex_digit ('3'));
		ubf_assert (ubf_is_hex_digit ('4'));
		ubf_assert (ubf_is_hex_digit ('5'));
		ubf_assert (ubf_is_hex_digit ('6'));
		ubf_assert (ubf_is_hex_digit ('7'));
		ubf_assert (ubf_is_hex_digit ('8'));
		ubf_assert (ubf_is_hex_digit ('9'));
		ubf_assert (ubf_is_hex_digit ('A'));
		ubf_assert (ubf_is_hex_digit ('B'));
		ubf_assert (ubf_is_hex_digit ('C'));
		ubf_assert (ubf_is_hex_digit ('D'));
		ubf_assert (ubf_is_hex_digit ('E'));
		ubf_assert (ubf_is_hex_digit ('F'));
		ubf_assert (ubf_is_hex_digit ('a'));
		ubf_assert (ubf_is_hex_digit ('b'));
		ubf_assert (ubf_is_hex_digit ('c'));
		ubf_assert (ubf_is_hex_digit ('d'));
		ubf_assert (ubf_is_hex_digit ('e'));
		ubf_assert (ubf_is_hex_digit ('f'));
		ubf_assert (!ubf_is_hex_digit ('g'));
		ubf_assert (!ubf_is_hex_digit ('z'));
		ubf_assert (!ubf_is_hex_digit ('G'));
		ubf_assert (!ubf_is_hex_digit ('Z'));
		ubf_assert (!ubf_is_hex_digit ('.'));
		
		// Pretty much 100 % test coverage for the hexadecimal characters.
		ubf_assert ( 0 == ubf_value_of_ASCII_hex ('0'));
		ubf_assert ( 1 == ubf_value_of_ASCII_hex ('1'));
		ubf_assert ( 2 == ubf_value_of_ASCII_hex ('2'));
		ubf_assert ( 3 == ubf_value_of_ASCII_hex ('3'));
		ubf_assert ( 4 == ubf_value_of_ASCII_hex ('4'));
		ubf_assert ( 5 == ubf_value_of_ASCII_hex ('5'));
		ubf_assert ( 6 == ubf_value_of_ASCII_hex ('6'));
		ubf_assert ( 7 == ubf_value_of_ASCII_hex ('7'));
		ubf_assert ( 8 == ubf_value_of_ASCII_hex ('8'));
		ubf_assert ( 9 == ubf_value_of_ASCII_hex ('9'));
		ubf_assert (10 == ubf_value_of_ASCII_hex ('a'));
		ubf_assert (10 == ubf_value_of_ASCII_hex ('A'));
		ubf_assert (11 == ubf_value_of_ASCII_hex ('b'));
		ubf_assert (11 == ubf_value_of_ASCII_hex ('B'));
		ubf_assert (12 == ubf_value_of_ASCII_hex ('c'));
		ubf_assert (12 == ubf_value_of_ASCII_hex ('C'));
		ubf_assert (13 == ubf_value_of_ASCII_hex ('d'));
		ubf_assert (13 == ubf_value_of_ASCII_hex ('D'));
		ubf_assert (14 == ubf_value_of_ASCII_hex ('e'));
		ubf_assert (14 == ubf_value_of_ASCII_hex ('E'));
		ubf_assert (15 == ubf_value_of_ASCII_hex ('f'));
		ubf_assert (15 == ubf_value_of_ASCII_hex ('F'));
		ubf_assert (UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex (' '));
		ubf_assert (UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('g'));
		ubf_assert (UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('G'));
		ubf_assert (UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('.'));
		ubf_assert (UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('H'));
		ubf_assert (UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('Z'));
		ubf_assert (UBF_INVALID_HEX_CHAR == ubf_value_of_ASCII_hex ('\0'));

		// Let's do the same with the octal digits.
		ubf_assert ( 0 == ubf_value_of_ASCII_oct ('0'));
		ubf_assert ( 1 == ubf_value_of_ASCII_oct ('1'));
		ubf_assert ( 2 == ubf_value_of_ASCII_oct ('2'));
		ubf_assert ( 3 == ubf_value_of_ASCII_oct ('3'));
		ubf_assert ( 4 == ubf_value_of_ASCII_oct ('4'));
		ubf_assert ( 5 == ubf_value_of_ASCII_oct ('5'));
		ubf_assert ( 6 == ubf_value_of_ASCII_oct ('6'));
		ubf_assert ( 7 == ubf_value_of_ASCII_oct ('7'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct (' '));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('8'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('9'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('A'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('B'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('C'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('D'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('E'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('.'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('H'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('Z'));
		ubf_assert (UBF_INVALID_OCT_CHAR == ubf_value_of_ASCII_oct ('\0'));
		
		// Binary.
		ubf_assert (0 == ubf_value_of_ASCII_bin ('0'));
		ubf_assert (1 == ubf_value_of_ASCII_bin ('1'));
		ubf_assert (UBF_INVALID_BIN_CHAR == ubf_value_of_ASCII_bin (' '));
		ubf_assert (UBF_INVALID_BIN_CHAR == ubf_value_of_ASCII_bin ('2'));
		ubf_assert (UBF_INVALID_BIN_CHAR == ubf_value_of_ASCII_bin ('\0'));
		
		strcpy (chT, "377");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_assert (3 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_assert (3 == st);
		ubf_assert (255 == o);

		strcpy (chT, "400");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_assert (4 == o);
		o = ubf_value_of_ASCII_oct (chT [1]);
		ubf_assert (0 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_assert (2 == st);
		// 40o == 32d.
		ubf_assert (32 == o);

		strcpy (chT, "401");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_assert (4 == o);
		o = ubf_value_of_ASCII_oct (chT [1]);
		ubf_assert (0 == o);
		o = ubf_value_of_ASCII_oct (chT [2]);
		ubf_assert (1 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_assert (2 == st);
		// 40o == 32d. The "1" in "401" is already ignored due to octet boundary.
		ubf_assert (32 == o);

		strcpy (chT, "477");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_assert (4 == o);
		o = ubf_value_of_ASCII_oct (chT [1]);
		ubf_assert (7 == o);
		o = ubf_value_of_ASCII_oct (chT [2]);
		ubf_assert (7 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_assert (2 == st);
		// 47o == 39d.
		ubf_assert (39 == o);

		strcpy (chT, "1110");
		o = ubf_value_of_ASCII_oct (chT [0]);
		ubf_assert (1 == o);
		o = ubf_value_of_ASCII_oct (chT [1]);
		ubf_assert (1 == o);
		o = ubf_value_of_ASCII_oct (chT [2]);
		ubf_assert (1 == o);
		o = ubf_value_of_ASCII_oct (chT [3]);
		ubf_assert (0 == o);
		st = ubf_octet_from_oct (&o, chT);
		ubf_assert (3 == st);
		// 111o == 73d == 49h == "I".
		ubf_assert (73 == o);
		
		st = ubf_octet_from_bin (&o, "0");
		ubf_assert (1 == st);
		ubf_assert (0 == o);
		st = ubf_octet_from_bin (&o, "1");
		ubf_assert (1 == st);
		ubf_assert (1 == o);
		st = ubf_octet_from_bin (&o, "01");
		ubf_assert (2 == st);
		ubf_assert (1 == o);
		st = ubf_octet_from_bin (&o, "10");
		ubf_assert (2 == st);
		ubf_assert (2 == o);
		st = ubf_octet_from_bin (&o, "11");
		ubf_assert (2 == st);
		ubf_assert (3 == o);
		st = ubf_octet_from_bin (&o, "100");
		ubf_assert (3 == st);
		ubf_assert (4 == o);
		st = ubf_octet_from_bin (&o, "101");
		ubf_assert (3 == st);
		ubf_assert (5 == o);
		st = ubf_octet_from_bin (&o, "110");
		ubf_assert (3 == st);
		ubf_assert (6 == o);
		st = ubf_octet_from_bin (&o, "111");
		ubf_assert (3 == st);
		ubf_assert (7 == o);
		st = ubf_octet_from_bin (&o, "1000");
		ubf_assert (4 == st);
		ubf_assert (8 == o);
		st = ubf_octet_from_bin (&o, "1001");
		ubf_assert (4 == st);
		ubf_assert (9 == o);
		st = ubf_octet_from_bin (&o, "1010");
		ubf_assert (4 == st);
		ubf_assert (10 == o);
		st = ubf_octet_from_bin (&o, "1011");
		ubf_assert (4 == st);
		ubf_assert (11 == o);
		st = ubf_octet_from_bin (&o, "1100");
		ubf_assert (4 == st);
		ubf_assert (12 == o);
		st = ubf_octet_from_bin (&o, "1101");
		ubf_assert (4 == st);
		ubf_assert (13 == o);
		st = ubf_octet_from_bin (&o, "1110");
		ubf_assert (4 == st);
		ubf_assert (14 == o);
		st = ubf_octet_from_bin (&o, "1111");
		ubf_assert (4 == st);
		ubf_assert (15 == o);
		st = ubf_octet_from_bin (&o, "00000001");
		ubf_assert (8 == st);
		ubf_assert (1 == o);
		st = ubf_octet_from_bin (&o, "000000001");				// Only 8 bits will be consumed.
		ubf_assert (8 == st);
		ubf_assert (0 == o);
		st = ubf_octet_from_bin (&o, "101010101");				// Only 8 bits will be consumed.
		ubf_assert (8 == st);
		ubf_assert (170 == o);
		st = ubf_octet_from_bin (&o, "00000010");
		ubf_assert (8 == st);
		ubf_assert (2 == o);
		st = ubf_octet_from_bin (&o, "00000011");
		ubf_assert (8 == st);
		ubf_assert (3 == o);
		st = ubf_octet_from_bin (&o, "00000100");
		ubf_assert (8 == st);
		ubf_assert (4 == o);
		st = ubf_octet_from_bin (&o, "00010000");
		ubf_assert (8 == st);
		ubf_assert (16 == o);
		st = ubf_octet_from_bin (&o, "00010001");
		ubf_assert (8 == st);
		ubf_assert (17 == o);
		st = ubf_octet_from_bin (&o, "00100000");
		ubf_assert (8 == st);
		ubf_assert (32 == o);
		st = ubf_octet_from_bin (&o, "00100001");
		ubf_assert (8 == st);
		ubf_assert (33 == o);
		st = ubf_octet_from_bin (&o, "00100010");
		ubf_assert (8 == st);
		ubf_assert (34 == o);
		st = ubf_octet_from_bin (&o, "00100011");
		ubf_assert (8 == st);
		ubf_assert (35 == o);
		st = ubf_octet_from_bin (&o, "01000000");
		ubf_assert (8 == st);
		ubf_assert (64 == o);
		st = ubf_octet_from_bin (&o, "10000000");
		ubf_assert (8 == st);
		ubf_assert (128 == o);
		
		// UCS2UTF16bin_from_hex.
		uint16_t		bin [128];
		char			*hex;
		bool			b;
		
		hex = "003100320033";
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 0, &st);
		// Should come back false.
		ubf_assert (false == b);
		ubf_assert (0 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 1, &st);
		ubf_assert (false == b);
		ubf_assert (0 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 2, &st);
		ubf_assert (false == b);
		ubf_assert (0 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 3, &st);
		ubf_assert (false == b);
		ubf_assert (0 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 4, &st);
		ubf_assert (true == b);
		ubf_assert (1 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 5, &st);
		ubf_assert (false == b);
		ubf_assert (0 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 6, &st);
		ubf_assert (false == b);
		ubf_assert (0 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 7, &st);
		ubf_assert (false == b);
		ubf_assert (0 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 8, &st);
		ubf_assert (true == b);
		ubf_assert (2 == st);
		b = UCS2UTF16bin_from_hex (NULL, SIZE_T_MAX, hex, 12, &st);
		ubf_assert (true == b);
		ubf_assert (3 == st);
		b = UCS2UTF16bin_from_hex (bin, 0, hex, 12, &st);
		ubf_assert (true == b);
		ubf_assert (0 == st);
		b = UCS2UTF16bin_from_hex (bin, 1, hex, 12, &st);
		ubf_assert (true == b);
		ubf_assert (1 == st);
		ubf_assert (0x31 == bin [0]);
		b = UCS2UTF16bin_from_hex (bin, 2, hex, 12, &st);
		ubf_assert (true == b);
		ubf_assert (2 == st);
		ubf_assert (0x31 == bin [0]);
		ubf_assert (0x32 == bin [1]);
		b = UCS2UTF16bin_from_hex (bin, 3, hex, 12, &st);
		ubf_assert (true == b);
		ubf_assert (3 == st);
		ubf_assert (0x31 == bin [0]);
		ubf_assert (0x32 == bin [1]);
		ubf_assert (0x33 == bin [2]);
		b = UCS2UTF16bin_from_hex (bin, 4, hex, 12, &st);
		ubf_assert (true == b);
		ubf_assert (3 == st);
		ubf_assert (0x31 == bin [0]);
		ubf_assert (0x32 == bin [1]);
		ubf_assert (0x33 == bin [2]);
		// "@AB";
		hex = "004000410042";
		b = UCS2UTF16bin_from_hex (bin, 4, hex, 12, &st);
		ubf_assert (true == b);
		ubf_assert (3 == st);
		ubf_assert (0x40 == bin [0]);
		ubf_assert (0x41 == bin [1]);
		ubf_assert (0x42 == bin [2]);
		
		// Tests for ubf_uint16_from_hex ().
		hex = "FFFF";
		uint16_t		ui16;
		uint32_t		ui32;
		uint64_t		ui64;
		
		ubf_assert (sizeof (uint16_t) == sizeof (wchar_t));
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_assert (4 == st);
		ubf_assert (65535 == ui16);
		hex = "FF";
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_assert (2 == st);
		ubf_assert (255 == ui16);
		hex = "10";
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_assert (2 == st);
		ubf_assert (16 == ui16);
		hex = "100";
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_assert (3 == st);
		ubf_assert (256 == ui16);
		hex = "FFF0";
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_assert (4 == st);
		ubf_assert (65520 == ui16);
		hex = "";
		ui16 = 17;	// Should stay because no hex digit means the function should not
					//	change it.
		st = ubf_uint16_from_hex (&ui16, hex);
		ubf_assert (0 == st);
		ubf_assert (17 == ui16);
		
		unsigned char c;
		char ch [10];
		for (c = (unsigned char) '0'; c <= (unsigned char) '9'; c ++)
		{
			ch [0] = c;
			ch [1] = '\0';
			st = ubf_uint16_from_hex (&ui16, ch);
			ubf_assert (1 == st);
			ubf_assert ((uint16_t) (c - (unsigned char) '0') == ui16);
		}
		for (c = (unsigned char) 'a'; c < (unsigned char) 'g'; c ++)
		{
			ch [0] = c;
			ch [1] = '\0';
			st = ubf_uint16_from_hex (&ui16, ch);
			ubf_assert (1 == st);
			ubf_assert ((uint16_t) (10 + c - (unsigned char) 'a') == ui16);
		}
		for (c = (unsigned char) '0'; c <= (unsigned char) '9'; c ++)
		{
			ch [0] = c;
			ch [1] = '\0';
			st = ubf_uint32_from_hex (&ui32, ch);
			ubf_assert (1 == st);
			ubf_assert ((uint32_t) (c - (unsigned char) '0') == ui32);
		}
		for (c = (unsigned char) 'a'; c < (unsigned char) 'g'; c ++)
		{
			ch [0] = c;
			ch [1] = '\0';
			st = ubf_uint64_from_hex (&ui64, ch);
			ubf_assert (1 == st);
			ubf_assert ((uint64_t) (10 + (uint64_t) c - (unsigned char) 'a') == ui64);
		}
		
		hex = "FFFF";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_assert (4 == st);
		ubf_assert (65535 == ui32);
		hex = "FF";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_assert (2 == st);
		ubf_assert (255 == ui32);
		hex = "10";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_assert (2 == st);
		ubf_assert (16 == ui32);
		hex = "100";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_assert (3 == st);
		ubf_assert (256 == ui32);
		hex = "FFF0";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_assert (4 == st);
		ubf_assert (65520 == ui32);
		hex = "";
		ui32 = 17;	// Should stay because no hex digit means the function should not
					//	change it.
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_assert (0 == st);
		ubf_assert (17 == ui32);
		hex = "FFFFFFFF";
		st = ubf_uint32_from_hex (&ui32, hex);
		ubf_assert (8 == st);
		ubf_assert (UINT32_MAX == ui32);

		hex = "FFFF";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (4 == st);
		ubf_assert (65535 == ui64);
		hex = "FF";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (2 == st);
		ubf_assert (255 == ui64);
		hex = "10";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (2 == st);
		ubf_assert (16 == ui64);
		hex = "100";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (3 == st);
		ubf_assert (256 == ui64);
		hex = "FFF0";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (4 == st);
		ubf_assert (65520 == ui64);
		hex = "";
		ui64 = 17;	// Should stay because no hex digit means the function should not
					//	change it.
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (0 == st);
		ubf_assert (17 == ui64);
		hex = "FFFFFFFF";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (8 == st);
		ubf_assert (UINT32_MAX == ui64);
		hex = "FFFFFFFFFFFFFFFF";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (16 == st);
		ubf_assert (UINT64_MAX == ui64);
		hex = "FFFFFFFFFFFFFFF0";
		st = ubf_uint64_from_hex (&ui64, hex);
		ubf_assert (16 == st);
		ubf_assert (UINT64_MAX - 15 == ui64);

	}
#endif
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
		*stJump				= 0;
	return uiRet;
}

size_t strIsLineEndings (char *ch, size_t stLen, size_t *stJump)
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

/*
	This should be:

	static const char *aszCunilogNewLine [unilogNewLineAmountEnumValues - 1] =

	But we want to check the amount of elements at runtime.
*/
#if defined (CUNILOG_NEWLINE_POSIX_ONLY)
	static const char *aszLineEndings [] =
	{
			"\n",				// 0 = cunilogNewLineSystem
			"\n",				// 1 = cunilogNewLinePOSIX
	};
	size_t lenLineEndings [] =
	{
			1,					// 0 = cunilogNewLineSystem		"\n"
			1,					// 1 = cunilogNewLinePOSIX		"\n"
	};
#elif defined (CUNILOG_NEWLINE_WINDOWS_ONLY)
	static const char *aszLineEndings [] =
	{
			"\r\n",				// 0 = cunilogNewLineSystem
			"\r\n",				// 1 = cunilogNewLineWindows
	};
	size_t lenLineEndings [] =
	{
			2,					// 0 = cunilogNewLineSystem		"\r\n"
			2,					// 1 = cunilogNewLineWindows	"\r\n"
	};
#elif defined (CUNILOG_NEWLINE_POSIX_AND_WINDOWS_ONLY)
	static const char *aszLineEndings [] =
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
	static const char *aszLineEndings [] =
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

const char *ccLineEnding (newline_t nl)
{
	ubf_assert (nl >= 0);
	ubf_assert (nl < cunilogNewLineAmountEnumValues);

	return aszLineEndings [nl];
}

size_t lnLineEnding (newline_t nl)
{
	ubf_assert (nl >= 0);
	ubf_assert (nl < cunilogNewLineAmountEnumValues);

	return lenLineEndings [nl];
}

const char *szLineEnding (newline_t nl, size_t *pln)
{
	ubf_assert (nl >= 0);
	ubf_assert (nl < cunilogNewLineAmountEnumValues);
	ubf_assert_non_NULL (pln);

	*pln = lenLineEndings [nl];
	return aszLineEndings [nl];
}

#ifdef STRNEWLINE_BUILD_TEST
	void test_strnewline (void)
	{
		ubf_assert (GET_ARRAY_LEN (aszLineEndings)				==	cunilogNewLineAmountEnumValues);
		ubf_assert (GET_ARRAY_LEN (lenLineEndings)				==	cunilogNewLineAmountEnumValues);

		size_t			st;
		size_t			us;
		char			sz [1024];

		strcpy (sz, "\r\n\r");
		us = strIsLineEndings (sz, strlen (sz), &st);
		ubf_assert (2 == us);
		strcpy (sz, "\na");
		us = strIsLineEndings (sz, strlen (sz), &st);
		ubf_assert (1 == us);
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
		bool		b;
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
		b = 1 == s;
		u = 9;
		s = ubf_str_from_int64 (c, u);
		ubf_assert (1 == s);
		b &= 1 == s;
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
		s = ubf_str0_from_uint64 (c, 1, u);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (1 == s);
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

		s = ubf_str0_from_uint64 (c, 10, 3000);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (4 == s);
		ubf_assert (!memcmp (c, "0000003000", 11));

		s = ubf_str0_from_uint64 (c, 10, 200);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (3 == s);
		ubf_assert (!memcmp (c, "0000000200", 11));

		s = ubf_str0_from_uint64 (c, 3, 555);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (3 == s);
		ubf_assert (!memcmp (c, "555", 4));

		s = ubf_str0_from_uint64 (c, 2, 200);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (STR0_NOT_ENOUGH_DIGITS == s);
		ubf_assert (!memcmp (c, "00", 3));

		s = ubf_str0_from_uint64 (c, 3, 1999);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (STR0_NOT_ENOUGH_DIGITS == s);
		ubf_assert (!memcmp (c, "999", 3));

		// The 16 bit version.
		s = ubf_str0_from_uint16 (c, 10, 3000);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (4 == s);
		ubf_assert (!memcmp (c, "0000003000", 11));

		s = ubf_str0_from_uint16 (c, 10, 200);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (3 == s);
		ubf_assert (!memcmp (c, "0000000200", 11));

		s = ubf_str0_from_uint16 (c, 3, 555);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (3 == s);
		ubf_assert (!memcmp (c, "555", 4));

		s = ubf_str0_from_uint16 (c, 2, 200);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (STR0_NOT_ENOUGH_DIGITS == s);
		ubf_assert (!memcmp (c, "00", 3));

		s = ubf_str0_from_uint16 (c, 3, 1999);
		UBF_CHK_GUARD_VAR (v, "012345");
		ubf_assert (STR0_NOT_ENOUGH_DIGITS == s);
		ubf_assert (!memcmp (c, "999", 3));

		// Min.
		ubf_str0_from_59max (c, 0);
		ubf_assert (!memcmp (c, "00", 2));
		ubf_str0_from_59max (c, 61);
		ubf_assert (!memcmp (c, "61", 2));
		// NUL-terminated.
		ubf_str0_from_59max_n (c, 0);
		ubf_assert (!memcmp (c, "00", 3));
		// Max.
		ubf_str0_from_59max_n (c, 61);
		ubf_assert (!memcmp (c, "61", 3));

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
	#else
		#include "./../pre/externC.h"
		#include "./../string/strhex.h"
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
size_t uri_encode_str (char *szURIencoded, const char *str, size_t len)
{

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

static inline enGl globCharAfterStar (char *c, size_t *g, const char *ccGlob, size_t *lnGlob)
{
	ubf_assert_non_NULL (c);
	ubf_assert_non_NULL (g);
	ubf_assert_non_NULL (lnGlob);

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
			||	('/' == c1 && '\\' == c2)				\
			||	('\\' == c1 && '/' == c2)				\
		)
#endif

// Neither r nor p are uninitialised.
DISABLE_WARNING_POTENTIALLY_UNINITIALISED_VARIABLE_USED ()
DISABLE_WARNING_POTENTIALLY_UNINITIALISED_LOCAL_POINTER_USED ()

static inline const char *handleGlobStars	(
							bool *r,
							size_t *s, size_t *g,
							const char *ccStri, size_t *lnStri,
							const char *ccGlob, size_t *lnGlob
											)
{
	char		c;
	const char	*p;
	enGl		gl;

	gl = globCharAfterStar (&c, g, ccGlob, lnGlob);
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

static inline bool globMatchInt (const char *ccStri, size_t *lnStri, const char *ccGlob, size_t *lnGlob)
{
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
	return s - *lnStri == g - *lnGlob;
}

DEFAULT_WARNING_POTENTIALLY_UNINITIALISED_VARIABLE_USED ()
DEFAULT_WARNING_POTENTIALLY_UNINITIALISED_LOCAL_POINTER_USED ()

bool globMatch (const char *ccStri, size_t lnStri, const char *ccGlob, size_t lnGlob)
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

#ifdef STRWILDCARDS_BUILD_TEST_FNCT
	bool strwildcards_test_function (void)
	{
		bool b = true;

		/*
			Note that in C99 a question mark can either be escaped (as in "\?") or
			stand alone (as in "?"). See C99 standard, 6.4.4.4 and 6.4.5.
			Some of the tests below escape it, others don't.
		*/
		ubf_expect_bool_AND (b, true	== globMatch ("a", 0, "b", 0));
		ubf_expect_bool_AND (b, true	== globMatch ("abc", 0, "b", 0));
		ubf_expect_bool_AND (b, true	== globMatch ("a", 1, "b", 0));
		ubf_expect_bool_AND (b, false	== globMatch ("a", 1, "b", 1));
		ubf_expect_bool_AND (b, true	== globMatch ("a", 1, "a", 1));
		ubf_expect_bool_AND (b, true	== globMatch ("a/b/c", USE_STRLEN, "\?/\?/\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("abc", 3, "a*", 2));
		ubf_expect_bool_AND (b, true	== globMatch ("abc", 3, "*bc", 3));
		ubf_expect_bool_AND (b, true	== globMatch ("abc", 3, "a*c", 3));
		ubf_expect_bool_AND (b, false	== globMatch ("abc", 3, "a*d", 3));
		ubf_expect_bool_AND (b, false	== globMatch ("abc", 3, "d*d", 3));
		ubf_expect_bool_AND (b, true	== globMatch ("abc", 3, "*", 1));
		ubf_expect_bool_AND (b, false	== globMatch ("abc", 3, "d", 1));
		ubf_expect_bool_AND (b, true	== globMatch ("abc", 3, "*x", 1));
		ubf_expect_bool_AND (b, false	== globMatch ("abc", 3, "*x", 2));
		ubf_expect_bool_AND (b, true	== globMatch ("a/b/c", USE_STRLEN, "*\\*\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("a/b/c", USE_STRLEN, "*\\b\\c", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("a/b/c", USE_STRLEN, "*\\d\\e", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("a/b/c", USE_STRLEN, "a\\b\\e", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("a/b/c", USE_STRLEN, "a\\b\\c", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("a/b/c", USE_STRLEN, "a\\*\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("a/b/c", USE_STRLEN, "a\\*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("abcdef", USE_STRLEN, "a\?c*f", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("abcdef", USE_STRLEN, "a\?c*fx", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("0123456789", USE_STRLEN, "0123456789", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("0123456789", USE_STRLEN, "0123\?56789", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("0123\0""456789", 11, "0123\0""\?56789", 11));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/home/user", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/home/use\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/home/us\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/home/u\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/home/\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/home\?\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/hom\?\?\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/h*/user", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/home/user", USE_STRLEN, "/h*/usxr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/*/us*r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/home/user", USE_STRLEN, "/*/us*x", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/home/user", USE_STRLEN, "/*/us*\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/*/us**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/**", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/***", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/****", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/*****", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/user", USE_STRLEN, "/*/\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"/*\?usr", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/home/usr", USE_STRLEN,	"/*\?/usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"/*\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"/*?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"\?*\?*", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"\?*\?\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"\\*\\\?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"/?\?\?\?/?\?\?", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"?home?usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN,	"?h*m*?u*r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/home/usr", USE_STRLEN, "/**x", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN, "/**sr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN, "/**r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN, "/**usr", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN, "/***r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN, "/****r", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN, "/*****r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/home/usr", USE_STRLEN, "/*****b", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/home/usr", USE_STRLEN, "/*********************b", USE_STRLEN));
		ubf_expect_bool_AND (b, true	== globMatch ("/home/usr", USE_STRLEN, "/*********************r", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/home/usr", USE_STRLEN, "/**?", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/", USE_STRLEN, "/*?", USE_STRLEN));
		ubf_expect_bool_AND (b, false	== globMatch ("/", USE_STRLEN, "/**?", USE_STRLEN));

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

#ifdef DEBUG
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

#ifdef _DEBUG
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
#ifdef DEBUG
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

#ifdef DEBUG
	bool isDirectorySeparator (char c)
	{
		return ('/' == c || '\\' == c);
	}
#endif

#ifdef DEBUG
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

#ifdef DEBUG
	size_t ubf_obtain_strlen (const char *sz, size_t providedLength)
	{
		return sz ? (USE_STRLEN == providedLength ? strlen (sz) : providedLength) : 0;
	}
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

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
	
		#include "./ubf_date_and_time.h"
		#include "./ubfdebug.h"
		#include "./ArrayMacros.h"
		#include "./membuf.h"
		#include "./check_utf8.h"
		#include "./ubfcharscountsandchecks.h"
		#include "./strfilesys.h"
		#include "./strintuint.h"
		#include "./strhexdump.h"
		#include "./strmembuf.h"
		#include "./strisabsolutepath.h"
		#include "./strnewline.h"
		#include "./CompressFile.h"
		#include "./ExeFileName.h"
		
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
		#include "./../string/strfilesys.h"
		#include "./../string/strintuint.h"
		#include "./../string/strhexdump.h"
		#include "./../string/strmembuf.h"
		#include "./../string/strisabsolutepath.h"
		#include "./../string/strnewline.h"
		#include "./../string/strwildcards.h"
		#include "./../OS/CompressFile.h"
		#include "./../OS/ExeFileName.h"
		
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

static SCUNILOGTARGET SCUNILOGTARGETstatic;
SCUNILOGTARGET *pSCUNILOGTARGETstatic	= &SCUNILOGTARGETstatic;

/*
	Copy from the structure header cunilogstructs.h to avoid switching between code and header
	during development. Can/could be deleted when finalised.

typedef struct cunilog_processor
{
	void						*pSCUNILOGTARGET;			// Pointer to the SCUNILOGTARGET
															//	structure.
	enum cunilogprocesstask		task;						// What to apply.
	enum cunilogprocesstype		thrtype;					// When to apply (theshold type).
	uint64_t					thr;						// Required value of cur before this
															//	processor is applied/run.
	uint64_t					cur;						// The current value.
	void						*pData;						// Pointer to processor-specific data.
	uint64_t					uiOpts;						// Option flags. See OPT_CUNPROC_
															//	definitions below.
} CUNILOG_PROCESSOR;

*/

/*
	Our default pData structures for our standard processors.
*/
CUNILOG_LOGFILE			stdcuppLogFile;

CUNILOG_ROTATION_DATA	stdcuppRotatorFS_compress =
{
	cunilogrotationtask_FScompressLogfiles,
	2, 0, CUNILOG_MAX_ROTATE_AUTO,
	SMEMBUF_INITIALISER, SMEMBUF_INITIALISER,
	CUNILOG_ROTATOR_FLAG_NONE
};

CUNILOG_ROTATION_DATA	stdcuppRotatorMove_to_recycle_bin =
{
	cunilogrotationtask_MoveToRecycleBinLogfiles,
	3, 0, CUNILOG_MAX_ROTATE_AUTO,
	SMEMBUF_INITIALISER, SMEMBUF_INITIALISER,
	CUNILOG_ROTATOR_FLAG_NONE
};

CUNILOG_ROTATION_DATA	stdcuppRotatorDelete =
{
	cunilogrotationtask_DeleteLogfiles,
	4, 0, CUNILOG_MAX_ROTATE_AUTO,
	SMEMBUF_INITIALISER, SMEMBUF_INITIALISER,
	CUNILOG_ROTATOR_FLAG_NONE
};

/*
	Our standard processors. Only used for our static SCUNILOGTARGET structure.
*/
CUNILOG_PROCESSOR	stdcuppEcho	=
{
	cunilogProcessEchoToConsole,
	cunilogProcessAppliesTo_nAlways,
	0, 0,
	NULL,
	OPT_CUNPROC_FORCE_NEXT
};
CUNILOG_PROCESSOR	stdcuppUpdateLogfileName =
{
	cunilogProcessUpdateLogFileName,
	cunilogProcessAppliesTo_nAlways,
	0, 0,
	NULL,
	OPT_CUNPROC_FORCE_NEXT
};
CUNILOG_PROCESSOR	stdcuppWriteToLogfile =
{
	cunilogProcessWriteToLogFile,
	cunilogProcessAppliesTo_nAlways,
	0, 0,
	&stdcuppLogFile,
	OPT_CUNPROC_NONE
};
CUNILOG_PROCESSOR	stdcuppFlushLogFile =
{
	cunilogProcessFlushLogFile,
	cunilogProcessAppliesTo_Auto,
	0, 0,
	&stdcuppLogFile,
	OPT_CUNPROC_FORCE_NEXT
};
CUNILOG_PROCESSOR	stdcuppRotateLogfilesFScompress =
{
	cunilogProcessRotateLogfiles,
	cunilogProcessAppliesTo_Auto,
	0, 0,
	&stdcuppRotatorFS_compress,
	OPT_CUNPROC_NONE | OPT_CUNPROC_AT_STARTUP
};
CUNILOG_PROCESSOR	stdcuppRotateLogfilesMoveToTrash =
{
	cunilogProcessRotateLogfiles,
	cunilogProcessAppliesTo_Auto,
	0, 0,
	&stdcuppRotatorMove_to_recycle_bin,
	OPT_CUNPROC_NONE | OPT_CUNPROC_AT_STARTUP
};
CUNILOG_PROCESSOR	*stdcupp [] =
{
	&stdcuppEcho,											// Writes to console.
	&stdcuppUpdateLogfileName,								// Updates the date/timestamp within
															//	the log file's name.
	&stdcuppWriteToLogfile,									// Writes out to log file.
	&stdcuppFlushLogFile,									// Flushes the log file.
	&stdcuppRotateLogfilesFScompress,						// Rotates the log files.
	&stdcuppRotateLogfilesMoveToTrash						// Rotates the log files.
};

#ifdef DEBUG
	void InitCUNILOG_PROCESSOR	(
			CUNILOG_PROCESSOR				*cup,
			enum cunilogprocesstask			task,
			enum cunilogprocessfrequency	freq,
			uint64_t						thrs
								)
	{
		ubf_assert_non_NULL (cup);
		cup->task	= task;
		cup->freq	= freq;
		cup->thr	= thrs;
		cup->cur	= 0;
		cup->pData	= NULL;
		cup->uiOpts	= OPT_CUNPROC_NONE;
	}
#endif

size_t arrLengthTimeStampFromPostfix [] =					// [unilogPostfixAmountEnumValues]
{
	/* cunilogPostfixNone		*/		0
	/* cunilogPostfixMinute		*/	,	LEN_ISO8601DATEHOURANDMINUTE
	/* cunilogPostfixMinuteT	*/	,	LEN_ISO8601DATEHOURANDMINUTE
	/* cunilogPostfixHour		*/	,	LEN_ISO8601DATEANDHOUR
	/* cunilogPostfixHourT		*/	,	LEN_ISO8601DATEANDHOUR
	/* cunilogPostfixDay		*/	,	LEN_ISO8601DATE
	/* cunilogPostfixWeek		*/	,	LEN_ISO8601YEARANDWEEK
	/* cunilogPostfixMonth		*/	,	LEN_ISO8601YEARANDMONTH
	/* cunilogPostfixYear		*/	,	LEN_ISO8601YEAR
};

#ifdef DEBUG
	size_t lenDateTimeStampFromPostfix (enum cunilogpostfix postfix)
	{
		ubf_assert (postfix <= cunilogPostfixYear);
		ubf_assert (postfix < cunilogPostfixAmountEnumValues);

		return arrLengthTimeStampFromPostfix [postfix];
	}
#endif

/*
	Note the wildcard mask for "YYYY-MM-DD HH_MI". Hours and minutes are separated by an
	underscore instead of a colon.
*/
const char *arrPostfixWildcardMask []	=		// [cunilogPostfixAmountEnumValues]
{
	"",											// cunilogPostfixNone
	"\?\?\?\?-\?\?-\?\? \?\?_\?\?",				// cunilogPostfixMinute		"YYYY-MM-DD HH_MI".
	"\?\?\?\?-\?\?-\?\?T\?\?_\?\?",				// cunilogPostfixMinuteT	"YYYY-MM-DDTHH_MI".
	"\?\?\?\?-\?\?-\?\? \?\?",					// cunilogPostfixHour		"YYYY-MM-DD HH".
	"\?\?\?\?-\?\?-\?\?T\?\?",					// cunilogPostfixHourT		"YYYY-MM-DDTHH".
	"\?\?\?\?-\?\?-\?\?",						// cunilogPostfixDay		"YYYY-MM-DD".
	"\?\?\?\?-W\?\?",							// cunilogPostfixWeek		"YYYY-Wnn".
	"\?\?\?\?-\?\?",							// cunilogPostfixMonth		"YYYY-MM".
	"\?\?\?\?"									// cunilogPostfixYear		"YYYY".
};

#ifdef DEBUG
	const char *postfixMaskFromLogPostfix (enum cunilogpostfix postfix)
	{
		ubf_assert (postfix >= 0);
		ubf_assert (postfix <= cunilogPostfixYear);
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
	/* cunilogPostfixNone		*/		noPostfix
	/* cunilogPostfixMinute		*/	,	ISO8601DateHourAndMinute_from_UBF_TIMESTAMP_c
	/* cunilogPostfixMinuteT	*/	,	ISO8601TDateHourAndMinute_from_UBF_TIMESTAMP_c
	/* cunilogPostfixHour		*/	,	ISO8601DateAndHour_from_UBF_TIMESTAMP_c
	/* cunilogPostfixHourT		*/	,	ISO8601TDateAndHour_from_UBF_TIMESTAMP_c
	/* cunilogPostfixDay		*/	,	ISO8601Date_from_UBF_TIMESTAMP_c
	/* cunilogPostfixWeek		*/	,	ISO8601YearAndWeek_from_UBF_TIMESTAMP_c
	/* cunilogPostfixMonth		*/	,	ISO8601YearAndMonth_from_UBF_TIMESTAMP_c
	/* cunilogPostfixYear		*/	,	ISO8601Year_from_UBF_TIMESTAMP_c
};

#ifdef DEBUG
	void obtainDateAndTimeStamp (char *szWhere, UBF_TIMESTAMP ts, enum cunilogpostfix postfix)
	{
		ubf_assert_non_NULL (szWhere);
		ubf_assert (postfix >= 0);
		ubf_assert (postfix <= cunilogPostfixYear);
		ubf_assert (postfix < cunilogPostfixAmountEnumValues);

		obtainTimeStampAsString [postfix] (szWhere, ts);
	}
#else
	#define obtainDateAndTimeStamp(sw, ts, pfx)			\
		obtainTimeStampAsString [(pfx)] (sw, ts)
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
		static inline void InitCriticalSection (SCUNILOGTARGET *put)
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
		static inline void InitCriticalSection (SCUNILOGTARGET *put)
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
	static inline void InitCUNILOG_LOCKER (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (needsOrHasLocker (put))
		{
			InitCriticalSection (put);
		}
		#ifdef DEBUG
			put->cl.bInitialised = true;
		#endif
	}
#else
	#define InitCUNILOG_LOCKER(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline void EnterCUNILOG_LOCKER (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (needsOrHasLocker (put));
		ubf_assert_true (put->cl.bInitialised);

		#ifdef OS_IS_WINDOWS
			EnterCriticalSection (&put->cl.cs);
		#else
			pthread_mutex_lock (&put->cl.mt);
		#endif
	}
#else
	#define EnterCUNILOG_LOCKER(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#ifdef PLATFORM_IS_WINDOWS
		static inline void LeaveCUNILOG_LOCKER (SCUNILOGTARGET *put)
		{
			ubf_assert_non_NULL (put);
			ubf_assert (needsOrHasLocker (put));
			ubf_assert_true (put->cl.bInitialised);

			LeaveCriticalSection (&put->cl.cs);
		}
	#else
		static inline void LeaveCUNILOG_LOCKER (SCUNILOGTARGET *put)
		{
			ubf_assert_non_NULL (put);
			ubf_assert (needsOrHasLocker (put));
			ubf_assert_true (put->cl.bInitialised);

			pthread_mutex_unlock (&put->cl.mt);
		}
	#endif
#else
	#define LeaveCUNILOG_LOCKER(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#ifdef OS_IS_WINDOWS
		static inline void DestroyCriticalSection (SCUNILOGTARGET *put)
		{
			DeleteCriticalSection (&put->cl.cs);
		}
	#else
		static inline void DestroyCriticalSection (SCUNILOGTARGET *put)
		{
			pthread_mutex_destroy (&put->cl.mt);
		}
	#endif
#endif
		
#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline void DoneCUNILOG_LOCKER (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert_true (put->cl.bInitialised);

		if (needsOrHasLocker (put))
			DestroyCriticalSection (put);
		#ifdef DEBUG
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

/*
	ObtainUserHomeDirectoy

	Obtains the user's home directory with an ending forward or backward slash as an
	SMEMBUF. It returns the length of the path excluding the terminating NUL character.

	This function should probably go in its own module at some point.
*/
static size_t ObtainUserHomeDirectoy (SMEMBUF *mb)
{
	ubf_assert_non_NULL (mb);
	ubf_assert (isInitialisedSMEMBUF (mb));

	size_t	r = 0;

	#if defined (PLATFORM_IS_WINDOWS)

		HANDLE hToken = 0;
		OpenProcessToken (GetCurrentProcess(), TOKEN_QUERY, &hToken );
		DWORD dw = 0;
		GetUserProfileDirectoryU8 (hToken, NULL, &dw);
		growToSizeSMEMBUF (mb, (size_t) dw + 1);
		if (isUsableSMEMBUF (mb))
		{
			bool b = GetUserProfileDirectoryU8 (hToken, mb->buf.pch, &dw);
			if (b && dw > 1)
			{
				// This check is normally not required since we actually already
				//	know that the profile folder comes without a directory separator.
				//	This function is only called from the initialiser and therefore not a
				//	hot path.
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

	#elif defined (PLATFORM_IS_POSIX)

		size_t lnH;
		const char *szH = UserHomeDirectoryPOSIX (&lnH);
		// Same here: This check is normally not required since we actually already
		//	know that the profile folder comes without a directory separator.
		//	This function is only called from the initialiser and therefore not a
		//	hot path.
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

static size_t ObtainRelativeLogPathBase (SMEMBUF *mb, enCunilogRelLogPath relLogPath)
{
	switch (relLogPath)
	{
		case cunilogLogPath_relativeToExecutable:
			return ObtainPathFromExecutableModule (mb);
		case cunilogLogPath_relativeToCurrentDir:
			return ObtainCurrentWorkingDirectoy (mb);
		case cunilogLogPath_relativeToHomeDir:
			return ObtainUserHomeDirectoy (mb);
	}
	return 0;
}

static bool ObtainLogPathFromArgument	(
				SMEMBUF						*smlp,
				size_t						*lnlp,
				const char					*szLogPath,
				size_t						len,
				enCunilogRelLogPath			relLogPath
										)
{
	ubf_assert_non_NULL (smlp);
	ubf_assert_non_NULL (lnlp);
	ubf_assert_non_NULL (szLogPath);
	ubf_assert_non_0 (len);
	ubf_assert (isInitialisedSMEMBUF (smlp));

	SMEMBUF b	= SMEMBUF_INITIALISER;
	size_t	ln	= len;

	if (is_absolute_path_l (szLogPath, ln))
	{
		if (!isDirSep (szLogPath [ln - 1]))
		{
			if (SMEMBUFfromStrReserveBytes (&b, szLogPath, ln, 1))
			{
				b.buf.pch [ln] = UBF_DIR_SEP;
				++ ln;
			}
		} else
		{
			SMEMBUFfromStr (&b, szLogPath, ln);
		}
	} else
	{	// The path is relative. It cannot be absolute.
		if (cunilogLogPath_isAbsolute == relLogPath)
		{
			#ifndef CUNILOG_BUILD_TEST_FNCTS
				ubf_assert_msg	(
					false,
					"szLogPath cannot be relative or NULL if cunilogLogPath_isAbsolute is given"
								);
			#endif
			return false;
		}
		SMEMBUF	t	= SMEMBUF_INITIALISER;
		size_t	lp	= ObtainRelativeLogPathBase (&t, relLogPath);
		ln = lp + len;
		if (!isDirSep (szLogPath [len - 1]))
		{
			growToSizeSMEMBUF (&b, ln + 1);
			if (isUsableSMEMBUF (&b))
			{
				copySMEMBUF (&b, &t);
				memcpy (b.buf.pch + lp, szLogPath, len);
				b.buf.pch [lp + len] = UBF_DIR_SEP;
				++ ln;
			}
		} else
		{
			growToSizeSMEMBUF (&b, ln);
			if (isUsableSMEMBUF (&b))
				memcpy (b.buf.pch + lp, szLogPath, len);
		}
		doneSMEMBUF (&t);
	}
	str_correct_dir_separators (b.buf.pch, ln);
	str_remove_path_navigators (b.buf.pch, &ln);
	copySMEMBUFsiz (smlp, &b, ln);
	doneSMEMBUF (&b);
	*lnlp = ln;													// The length of the destination.
	return true;
}

char *CreateLogPathInSUNILOGTARGET	(
		SCUNILOGTARGET *put, const char *szLogPath, size_t len, enCunilogRelLogPath relLogPath
									)
{
	ubf_assert_non_NULL (put);

	initSMEMBUF (&put->mbLogPath);
	if (szLogPath)
	{
		ubf_assert (0 != len);

		bool b;
		b = ObtainLogPathFromArgument	(
				&put->mbLogPath, &put->lnLogPath, szLogPath, len, relLogPath
										);
		// The function only fails if szLogPath is NULL or relative but cunilogLogPath_isAbsolute
		//	has been given.
		if (!b)
			return NULL;
	} else
	{
		// If szLogPath is NULL its length should be 0.
		ubf_assert_0 (len);

		// Cannot be absolute.
		if (cunilogLogPath_isAbsolute == relLogPath)
			return NULL;

		// No path given. We use the path specified with relLogPath.
		put->lnLogPath = ObtainRelativeLogPathBase (&put->mbLogPath, relLogPath);
	}
	put->uiOpts |= CUNILOGTARGET_LOGPATH_ALLOCATED;
	return put->mbLogPath.buf.pch;
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

char *CreateAppNameInSUNILOGTARGET (SCUNILOGTARGET *put, const char *szAppName, size_t len)
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

/*
	Probably not required, or maybe only as an example.

CUNILOG_PROCESSOR **NewStandardProcessors (SCUNILOGTARGET *put, unsigned int *pn)
{
	ubf_assert_non_NULL (put);

	CUNILOG_PROCESSOR **pp	= malloc (3 * sizeof (CUNILOG_PROCESSOR *));

	if (pp)
	{
		// Echo (output) to console.
		unsigned int ui = 0;
		pp [ui] = malloc (sizeof (CUNILOG_PROCESSOR));
		if (pp [ui])
		{
			pp [ui]->pSCUNILOGTARGET = put;
			pp [ui]->task		= cunilogProcessEchoToConsole;
			pp [ui]->thrtype	= cunilogProcessAppliesTo_nAlways;
			pp [ui]->thr		= 0;
			pp [ui]->cur		= 0;
			pp [ui]->pData		= NULL;
			pp [ui]->uiOpts		= OPT_CUNPROC_NONE;
			pp [ui]->uiOpts	|= OPT_CUNPROC_FORCE_NEXT | OPT_CUNPROC_ALLOCATED;
		}
		// Update the name of the logfile.
		++ ui;
		pp [ui] = malloc (sizeof (CUNILOG_PROCESSOR));
		if (pp [ui])
		{
			pp [ui]->pSCUNILOGTARGET = put;
			pp [ui]->task		= cunilogProcessUpdateLogFileName;
			pp [ui]->thrtype	= cunilogProcessAppliesTo_nAlways;
			pp [ui]->thr		= 0;
			pp [ui]->cur		= 0;
			pp [ui]->pData		= NULL;
			pp [ui]->uiOpts		= OPT_CUNPROC_NONE;
			pp [ui]->uiOpts	|= OPT_CUNPROC_ALLOCATED;
		}
		// Write to log file.
		++ ui;
		pp [ui] = malloc (sizeof (CUNILOG_PROCESSOR));
		if (pp [ui])
		{
			pp [ui]->pSCUNILOGTARGET = put;
			pp [ui]->task		= cunilogProcessWriteToLogFile;
			pp [ui]->thrtype	= cunilogProcessAppliesTo_nAlways;
			pp [ui]->thr		= 0;
			pp [ui]->cur		= 0;
			pp [ui]->pData		= NULL;
			pp [ui]->uiOpts		= OPT_CUNPROC_NONE;
			pp [ui]->uiOpts	|= OPT_CUNPROC_ALLOCATED;
		}
		// Flush log file.
		++ ui;
		pp [ui] = malloc (sizeof (CUNILOG_PROCESSOR));
		if (pp [ui])
		{
			pp [ui]->pSCUNILOGTARGET = put;
			pp [ui]->task		= cunilogProcessFlushLogFile;
			pp [ui]->thrtype	= cunilogProcessAppliesTo_HourChanged;
			pp [ui]->thr		= 0;
			pp [ui]->cur		= 0;
			pp [ui]->pData		= NULL;
			pp [ui]->uiOpts		= OPT_CUNPROC_NONE;
			pp [ui]->uiOpts	|= OPT_CUNPROC_FORCE_NEXT | OPT_CUNPROC_ALLOCATED;
		}
		// This is the last one.
		++ ui;
		if (pn)
			*pn = ui;
	}
	return pp;
}
*/

static inline enum cunilogprocessfrequency defaultFrequencyFromPostfix (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	switch (put->culogPostfix)
	{
		case cunilogPostfixNone:
			return cunilogProcessAppliesTo_nAlways;
		case cunilogPostfixMinute:
		case cunilogPostfixMinuteT:
			return cunilogProcessAppliesTo_MinuteChanged;
		case cunilogPostfixHour:
		case cunilogPostfixHourT:
			return cunilogProcessAppliesTo_HourChanged;
		case cunilogPostfixDay:
			return cunilogProcessAppliesTo_DayChanged;
		case cunilogPostfixWeek:
			return cunilogProcessAppliesTo_WeekChanged;
		case cunilogPostfixMonth:
			return cunilogProcessAppliesTo_MonthChanged;
		case cunilogPostfixYear:
			return cunilogProcessAppliesTo_YearChanged;
		case cunilogPostfixAmountEnumValues:
			ubf_assert_msg (false, "Illegal value");
			return cunilogProcessAppliesTo_nAlways;
	}
	return cunilogProcessAppliesTo_nAlways;
}

static inline void correctDefaultFrequency (CUNILOG_PROCESSOR *cp, SCUNILOGTARGET *put)
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
			case cunilogProcessUpdateLogFileName:
			case cunilogProcessWriteToLogFile:
			case cunilogProcessCustomProcessor:
				cp->freq = cunilogProcessAppliesTo_nAlways;
				break;
			case cunilogProcessFlushLogFile:
			case cunilogProcessRotateLogfiles:
				cp->freq = defaultFrequencyFromPostfix (put);
				break;
			case cunilogProcessAmountEnumValues:
				ubf_assert_msg (false, "Illegal value");
				break;
		}
	}
}

static inline void defaultFrequenciesAndMaxToRotate (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL	(put);
	ubf_assert_non_NULL	(put->cprocessors);
	ubf_assert_non_0	(put->nprocessors);

	CUNILOG_PROCESSOR		*cp;
	CUNILOG_ROTATION_DATA	*prPrev				= NULL;
	CUNILOG_ROTATION_DATA	*prCurr;

	unsigned int n = 0;
	while (n < put->nprocessors)
	{
		cp = put->cprocessors [n];
		correctDefaultFrequency (cp, put);
		if (cunilogProcessRotateLogfiles == cp->task)
		{
			prCurr = cp->pData;
			if (prPrev)
			{
				if (CUNILOG_MAX_ROTATE_AUTO == prPrev->nMaxToRotate)
					prPrev->nMaxToRotate = prCurr->nIgnore;
			}
			prPrev = prCurr;
		}
		++ n;
	}
}

static void prepareProcessors (SCUNILOGTARGET *put, CUNILOG_PROCESSOR **cp, unsigned int np)
{
	ubf_assert_non_NULL (put);

	if (NULL == cp || 0 == np)
	{
		put->cprocessors = stdcupp;
		put->nprocessors = GET_ARRAY_LEN (stdcupp);
	} else
	{
		/*
			This was originally for testing but is not required here. We'll provide the caller
			with an option to dynamically allocate standard processors at a later time.

		put->cprocessors = NewStandardProcessors (put, &put->nprocessors);
		if (put->cprocessors)
			cunilogSetProcessorsAllocated (put);
		*/

		// The caller obviously provided processors.
		//setProcessorsSCUNILOGTARGET (put, cp, np);
		put->cprocessors = cp;
		put->nprocessors = np;
	}
	defaultFrequenciesAndMaxToRotate (put);
}

static void prepareSCUNILOGTARGETinitFilenameBuffers (SCUNILOGTARGET *put, size_t lnTotal)
{
	ubf_assert_non_NULL (put);

	initSMEMBUFtoSize (&put->mbLogfileName, lnTotal);		// The actual log file.
	if (isUsableSMEMBUF (&put->mbLogfileName))
		cunilogSetLogFileAllocated (put);
	initSMEMBUFtoSize (&put->mbLogFileMask, lnTotal);		// Mask for logfile rotation.
	if (isUsableSMEMBUF (&put->mbLogFileMask))
		cunilogSetLogFileMaskAllocated (put);
	initSMEMBUFtoSize (&put->mbFilToRotate, lnTotal);
	if (isUsableSMEMBUF (&put->mbFilToRotate))
		cunilogSetFileToRotateAllocated (put);
}

static bool prepareSCUNILOGTARGETforLogging (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	size_t lnRoomForStamp	= lenDateTimeStampFromPostfix (put->culogPostfix);
	size_t lnUnderscore		= lnRoomForStamp ? 1 : 0;			// The underscore that separates
																//	the appname from the timestamp.
	ubf_assert (lnRoomForStamp <= LEN_ISO8601DATEHOURANDMINUTE);

	size_t lnTotal;
	lnTotal = put->lnLogPath + put->lnAppName + lnUnderscore;

	// The pointer to the ASCII timestamp within the filename allows us to later update only
	//	the timestamp within the log file's name. We add this index/length to the filename
	//	further down to get its position.
	size_t idxStamp = lnTotal;
	lnTotal += lnRoomForStamp
				+ lenCunilogLogFileNameExtension
				+ 1;	// A terminating NUL character so that we can use the log file's
						//	name directly in OS APIs.
	prepareSCUNILOGTARGETinitFilenameBuffers (put, lnTotal);

	if (isUsableSMEMBUF (&put->mbLogfileName) && isUsableSMEMBUF (&put->mbLogFileMask))
	{
		// Remember the position of the timestamp for quick and easy update.
		put->szDateTimeStamp = put->mbLogfileName.buf.pch + idxStamp;

		char *szWrite = put->mbLogfileName.buf.pch;
		memcpy (szWrite, put->mbLogPath.buf.pch, put->lnLogPath);
		szWrite += put->lnLogPath;
		memcpy (szWrite, put->mbAppName.buf.pch, put->lnAppName);
		szWrite += put->lnAppName;

		if (lnUnderscore)
		{
			szWrite [0] = '_';
			szWrite += 1;
		}

		szWrite += lnRoomForStamp;
		
		// Note that this memcpy () makes it NUL-terminated.
		memcpy (szWrite, szCunilogLogFileNameExtension, sizCunilogLogFileNameExtension);

		// Create the wildcard/search mask.
		copySMEMBUF (&put->mbLogFileMask, &put->mbLogfileName);
		memcpy	(
			put->mbLogFileMask.buf.pch + (put->szDateTimeStamp - put->mbLogfileName.buf.pcc),
			postfixMaskFromLogPostfix (put->culogPostfix),
			lenDateTimeStampFromPostfix (put->culogPostfix)
				);

		#ifdef PLATFORM_IS_POSIX
			// The function ForEachPsxDirEntry () returns every file and doesn't support a search
			//	mask. The callback function can use the szLogFileMask and its length to check if
			//	the returned filename fits the mask.
			put->szLogFileMask	= put->mbLogFileMask.buf.pch + put->lnLogPath;
			put->lnsLogFileMask	=		put->lnAppName
									+	lenDateTimeStampFromPostfix (put->culogPostfix)
									+	lenCunilogLogFileNameExtension
									+	lnUnderscore;
		#endif

		// Create name of the found file.
		copySMEMBUF (&put->mbFilToRotate, &put->mbLogPath);

		initSMEMBUFtoSize (&put->mbLogEventLine, CUNILOG_INITIAL_EVENTLINE_SIZE);
		if (isUsableSMEMBUF (&put->mbLogEventLine))
			cunilogSetEvtLineAllocated (put);
		cunilogSetTargetInitialised (put);
		return true;
	} else
	{
		put->szDateTimeStamp = NULL;
		ubf_assert_non_NULL (put->szDateTimeStamp);
		return false;
	}
}

static bool hasSCUNILOGTARGETqueue (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	return	(
					cunilogSingleThreadedSeparateLoggingThread	== put->culogType
				||	cunilogMultiThreadedSeparateLoggingThread	== put->culogType
			);
}

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	bool InitSCUNILOGTARGETqueuesemaphore (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (hasSCUNILOGTARGETqueue (put))
		{
			#ifdef OS_IS_WINDOWS
				// See
				//	https://learn.microsoft.com/en-us/windows/win32/api/synchapi/nf-synchapi-createsemaphorew .
				put->sm.hSemaphore = CreateSemaphoreW (NULL, 0, MAXLONG, NULL);
				return NULL != put->sm.hSemaphore;
			#else
				int i = sem_init (&put->sm.tSemaphore, 0, 0);
				ubf_assert (0 == i);
				return 0 == i;
			#endif
		}
		// If no semaphore is required, not creating it is seen as success.
		return true;
	}
#else
	#define InitSCUNILOGTARGETqueuesemaphore(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	void DoneSCUNILOGTARGETqueuesemaphore (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (hasSCUNILOGTARGETqueue (put))
		{
			#ifdef OS_IS_WINDOWS
				CloseHandle (put->sm.hSemaphore);
				put->sm.hSemaphore = NULL;
			#else
				int i = sem_destroy (&put->sm.tSemaphore);
				ubf_assert (0 == i);
				UNREFERENCED_PARAMETER (i);
			#endif
		}
	}
#else
	#define DoneSCUNILOGTARGETqueuesemaphore(x)
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static bool InitSCUNILOGTARGETqueue (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (hasSCUNILOGTARGETqueue (put))
		{
			put->qu.first	= NULL;
			put->qu.last	= NULL;
			put->qu.num		= 0;
		}
		// If no semaphore is required, not creating it is seen as success.
		return true;
	}
#else
	#define InitSCUNILOGTARGETqueue(x)
#endif

static bool StartSeparateLoggingThread_ifNeeded (SCUNILOGTARGET *put)
;

#ifdef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	// This is the only type possible in a single-threaded environment.
	#define unilogTypeFromArgument(type)					\
				(unilogSingleThreaded)
#else
	#define unilogTypeFromArgument(type)					\
				(type)
#endif

/*
	This function is not used anymore. The configurable hex dump is not supported
	anymore.
*/
static inline void InitSCUNILOGTARGETdumpstructs (SCUNILOGTARGET *put)
{
	UNUSED (put);

	ubf_assert_non_NULL (put);

	//put->psdump = NULL;
}

static inline void initSCUNILOGTARGEToptionFlags (SCUNILOGTARGET *put, runProcessorsOnStartup rp)
{
	ubf_assert_non_NULL (put);
	ubf_assert	(
						cunilogRunProcessorsOnStartup		== rp
					||	cunilogDontRunProcessorsOnStartup	== rp
				);

	put->uiOpts = CUNILOGTARGET_NO_FLAGS;
	if (cunilogRunProcessorsOnStartup == rp)
		cunilogSetRunAllProcessorsOnStartup (put);
}

static inline void initFilesListInSCUNILOGTARGET (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	vec_init (&put->fls);
	InitSBULKMEM (&put->sbm, CUNILOG_STD_BULK_ALLOC_SIZE);

	// Note that we do not allocate any bulk memory here yet. If for instance the caller
	//	hasn't provided a rotation processor we may not even need one.
}

static inline void initPrevTimestamp (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	size_t lenPostfixStamp = lenDateTimeStampFromPostfix (put->culogPostfix);
	memset (put->cPrevDateTimeStamp, 0, lenPostfixStamp);
}

#ifdef PLATFORM_IS_POSIX
	// The plain logging folder name is not used on Windows. It is only required
	//	for opendir () on POSIX. This might change in the future as I can imagine
	//	that some custom processor might also benefit from it.
	static void InitSCUNILOGTARGETmbLogFold (SCUNILOGTARGET *put)
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
	#define InitSCUNILOGTARGETmbLogFold(x)
#endif

static inline bool initCommonMembersAndPrepareSCUNILOGTARGET (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	str_remove_path_navigators (put->mbLogPath.buf.pch, &put->lnLogPath);
	InitSCUNILOGNPI							(&put->scuNPI);
	DBG_INIT_CNTTRACKER						(put->evtLineTracker);
	put->nPendingNoRotEvts					= 0;
	put->nPausedEvents						= 0;
	put->dumpWidth							= enDataDumpWidth16;
	initPrevTimestamp						(put);
	InitSCUNILOGTARGETmbLogFold				(put);
	InitSCUNILOGTARGETdumpstructs			(put);
	InitCUNILOG_LOCKER						(put);
	InitSCUNILOGTARGETqueuesemaphore		(put);
	InitSCUNILOGTARGETqueue					(put);
	initFilesListInSCUNILOGTARGET			(put);
	StartSeparateLoggingThread_ifNeeded		(put);
	return prepareSCUNILOGTARGETforLogging	(put);
}

#ifdef DEBUG
	void assertSaneParametersSCUNILOGTARGET	(
	  enCunilogRelLogPath		relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
											)
	{
		ubf_assert (0 <= relLogPath);
		ubf_assert (cunilogLogPath_AmountEnumValues > relLogPath);
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
	#define assertSaneParametersSCUNILOGTARGET(rP, tpy, psf, ts, nl, rp)
#endif

SCUNILOGTARGET *InitSCUNILOGTARGETex
(
	  SCUNILOGTARGET			*put				// Must not be NULL.
	, const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szAppName			// Application name.
	, size_t					lenAppName			// Length of szApplication.
	, enCunilogRelLogPath		relLogPath			// Rel. to home, exe, or current dir.
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
	assertSaneParametersSCUNILOGTARGET	(
		relLogPath, type, postfix, unilogTSformat, unilogNewLine, rp
										);

	size_t			lnLogPath		= (size_t) -1 != lenLogPath	? lenLogPath : strlen (szLogPath);
	size_t			lnAppName		= (size_t) -1 != lenAppName	? lenAppName : strlen (szAppName);

	initSCUNILOGTARGEToptionFlags (put, rp);
	put->culogPostfix		= postfix;
	put->culogType			= unilogTypeFromArgument (type);
	put->unilogEvtTSformat	= unilogTSformat;
	put->unilogNewLine		= unilogNewLine;
	char *szLP = CreateLogPathInSUNILOGTARGET (put, szLogPath, lnLogPath, relLogPath);
	if (NULL == szLP && cunilogLogPath_isAbsolute == relLogPath)
	{
		#ifndef CUNILOG_BUILD_TEST_FNCTS
			ubf_assert_msg	(
				false,
				"szLogPath cannot be relative or NULL if cunilogLogPath_isAbsolute is given"
							);
		#endif
		return NULL;
	}
	CreateAppNameInSUNILOGTARGET (put, szAppName, lnAppName);
	prepareProcessors (put, cuProcessorList, nProcessors);
	return initCommonMembersAndPrepareSCUNILOGTARGET (put) ? put : NULL;
}

#ifdef DEBUG
	SCUNILOGTARGET *InitSCUNILOGTARGET
	(
		  SCUNILOGTARGET			*put				// Must not be NULL.
		, const char				*szLogPath			// Path to the logging information.
		, size_t					lenLogPath			// Length of szLogPath
		, const char				*szAppName			// Application name.
		, size_t					lenAppName			// Length of szApplication.
		, enCunilogRelLogPath		relLogPath			// Rel. to home, exe, or current dir.
		, enum cunilogtype			type
	)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (0 <= relLogPath);
		ubf_assert (cunilogLogPath_AmountEnumValues > relLogPath);
		ubf_assert (0 <= type);
		ubf_assert (cunilogTypeAmountEnumValues > type);

		SCUNILOGTARGET	*prt;

		prt = InitSCUNILOGTARGETex	(
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

SCUNILOGTARGET *CreateNewSCUNILOGTARGET
(
	  const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szAppName			// Application name.
	, size_t					lenAppName			// Length of szApplication.
	, enCunilogRelLogPath		relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, CUNILOG_PROCESSOR			**cuProcessorList	// One or more post-processors.
	, unsigned int				nProcessors			// Number of processors.
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
)
{
	assertSaneParametersSCUNILOGTARGET	(
		relLogPath, type, postfix, unilogTSformat, unilogNewLine, rp
										);

	SCUNILOGTARGET	*pu;							// Return value.
	size_t			lnUNILOGTARGET	= ALIGNED_SIZE (sizeof (SCUNILOGTARGET), CUNILOG_DEFAULT_ALIGNMENT);
	size_t			lnTotal			= lnUNILOGTARGET;
	size_t			lnLP;
	size_t			lnAP;
	SMEMBUF			logpath			= SMEMBUF_INITIALISER;
	SMEMBUF			appname			= SMEMBUF_INITIALISER;

	size_t			lnLogPath		= (size_t) -1 != lenLogPath	? lenLogPath : strlen (szLogPath);
	size_t			lnAppName		= (size_t) -1 != lenAppName	? lenAppName : strlen (szAppName);

	if (szLogPath && lnLogPath)
	{
		if (!isDirSep (szLogPath [lnLogPath -1]))
			lnLP = ALIGNED_SIZE (lnLogPath + 1, CUNILOG_DEFAULT_ALIGNMENT);
		else 
			lnLP = ALIGNED_SIZE (lnLogPath, CUNILOG_DEFAULT_ALIGNMENT);
		lnTotal += lnLP;
	} else
	{	// No log path given.
		if (cunilogLogPath_isAbsolute == relLogPath)
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
		szLogPath = logpath.buf.pch;
		lnLP = ALIGNED_SIZE (lnLogPath, CUNILOG_DEFAULT_ALIGNMENT);
		lnTotal += lnLP;
	}
	if (szAppName && lnAppName)
	{
		lnAP = ALIGNED_SIZE (lnAppName, CUNILOG_DEFAULT_ALIGNMENT);
		lnTotal += lnAP;
	} else
	{	// No application name given.
		lnAppName = ObtainAppNameFromExecutableModule (&appname);
		ubf_assert_non_0 (lnAppName);
		szAppName = appname.buf.pch;
		lnAP = ALIGNED_SIZE (lnAppName, CUNILOG_DEFAULT_ALIGNMENT);
		lnTotal += lnAP;
	}
	pu = ubf_malloc (lnTotal);
	if (pu)
	{
		initSCUNILOGTARGEToptionFlags (pu, rp);
		pu->uiOpts |= CUNILOGTARGET_ALLOCATED;
		initSMEMBUF (&pu->mbLogPath);
		pu->mbLogPath.buf.pcc = (char *) pu + ALIGNED_SIZE (lnUNILOGTARGET, CUNILOG_DEFAULT_ALIGNMENT);
		memcpy (pu->mbLogPath.buf.pch, szLogPath, lnLogPath);
		if (!isDirSep (szLogPath [lnLogPath -1]))
		{
			pu->mbLogPath.buf.pch [lnLogPath]	= UBF_DIR_SEP;
			pu->mbLogPath.size					= lnLogPath + 1;
			pu->lnLogPath						= lnLogPath + 1;
		} else
		{
			pu->mbLogPath.size					= lnLogPath;
			pu->lnLogPath						= lnLogPath;
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
		prepareProcessors (pu, cuProcessorList, nProcessors);
		initCommonMembersAndPrepareSCUNILOGTARGET (pu);
	}
	if (isUsableSMEMBUF (&logpath))
		doneSMEMBUF (&logpath);
	if (isUsableSMEMBUF (&appname))
		doneSMEMBUF (&appname);
	return pu;
}

SCUNILOGTARGET *InitOrCreateSCUNILOGTARGET
(
	  SCUNILOGTARGET			*put				// If NULL, a new structure is allocated.
	, const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szAppName			// Application name.
	, size_t					lenAppName			// Length of szApplication.
	, enCunilogRelLogPath		relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, CUNILOG_PROCESSOR			**cuProcessorList	// One or more post-processors.
	, unsigned int				nProcessors			// Number of processors.
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
)
{
	assertSaneParametersSCUNILOGTARGET	(
		relLogPath, type, postfix, unilogTSformat, unilogNewLine, rp
										);

	SCUNILOGTARGET	*pu;

	if (put)
	{
		pu = put;
		InitSCUNILOGTARGETex	(
			pu,
			szLogPath, lenLogPath, szAppName, lenAppName, relLogPath,
			type, postfix, cuProcessorList, nProcessors,
			unilogTSformat, unilogNewLine, rp
							);
	} else
	{
		pu = CreateNewSCUNILOGTARGET	(
				szLogPath, lenLogPath, szAppName, lenAppName, relLogPath,
				type, postfix, cuProcessorList, nProcessors, 
				unilogTSformat, unilogNewLine, rp
									);
	}
	if (pu && pu->mbAppName.buf.pch)
	{
	} else
	{
		pu = DoneSCUNILOGTARGET (pu);
	}
	return pu;
}

SCUNILOGTARGET *InitSCUNILOGTARGETstaticEx
(
	  const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szApplication		// Application name.
	, size_t					lenApplication		// Length of szApplication.
	, enCunilogRelLogPath		relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
	, enum cunilogpostfix		postfix
	, CUNILOG_PROCESSOR			**cuProcessorList	// One or more post-processors.
	, unsigned int				nProcessors			// Number of processors.
	, enum cunilogeventTSformat	unilogTSformat		// The format of an event timestamp.
	, enum enLineEndings		unilogNewLine
	, runProcessorsOnStartup	rp					// Run/don't run all processors instantly.
)
{
	assertSaneParametersSCUNILOGTARGET	(
		relLogPath, type, postfix, unilogTSformat, unilogNewLine, rp
										);

	return InitSCUNILOGTARGETex	(
		pSCUNILOGTARGETstatic, szLogPath, lenLogPath, szApplication, lenApplication, relLogPath,
		type, postfix, cuProcessorList, nProcessors, unilogTSformat, unilogNewLine, rp
								);
}

SCUNILOGTARGET *InitSCUNILOGTARGETstatic
(
	  const char				*szLogPath			// Path to the logging information.
	, size_t					lenLogPath			// Length of szLogPath
	, const char				*szApplication		// Application name.
	, size_t					lenApplication		// Length of szApplication.
	, enCunilogRelLogPath		relLogPath			// Rel. to home, exe, or current dir.
	, enum cunilogtype			type
)
{
	ubf_assert (0 <= relLogPath);
	ubf_assert (cunilogLogPath_AmountEnumValues > relLogPath);
	ubf_assert (0 <= type);
	ubf_assert (cunilogTypeAmountEnumValues > type);

	return InitSCUNILOGTARGETex	(
				pSCUNILOGTARGETstatic,
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

#ifdef DEBUG
	void configSCUNILOGTARGETcunilogpostfix (SCUNILOGTARGET *put, enum cunilogeventTSformat tsf)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= tsf);
		ubf_assert			(cunilogEvtTS_AmountEnumValues > tsf);

		put->unilogEvtTSformat = tsf;		
	}
#endif

#ifdef DEBUG
	void configSCUNILOGTARGETcunilognewline (SCUNILOGTARGET *put, newline_t nl)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(0 <= nl);
		ubf_assert			(cunilogNewLineAmountEnumValues > nl);

		put->unilogNewLine = nl;
	}
#endif

#ifdef DEBUG
	void configSCUNILOGTARGETrunProcessorsOnStartup (SCUNILOGTARGET *put, runProcessorsOnStartup rp)
	{
		ubf_assert_non_NULL	(put);
		ubf_assert			(
									cunilogRunProcessorsOnStartup		== rp
								||	cunilogDontRunProcessorsOnStartup	== rp
							);

		switch (rp)
		{
			case cunilogRunProcessorsOnStartup:
				cunilogSetRunAllProcessorsOnStartup (put);
				break;
			case cunilogDontRunProcessorsOnStartup:
				cunilogClrRunAllProcessorsOnStartup (put);
				break;
			default:
				cunilogSetRunAllProcessorsOnStartup (put);
				break;
		}
	}
#endif

#ifdef DEBUG
	void configSCUNILOGTARGETprocessorList	(
					SCUNILOGTARGET			*put
				,	CUNILOG_PROCESSOR		**cuProcessorList	// One or more post-processors.
				,	unsigned int			nProcessors			// Number of processors.
											)
	{
		ubf_assert_non_NULL	(put);

		prepareProcessors (put, cuProcessorList, nProcessors);
	}
#endif

void EnterSCUNILOGTARGET (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	EnterCUNILOG_LOCKER (put);
}

void LeaveSCUNILOGTARGET (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	LeaveCUNILOG_LOCKER (put);
}

static void CloseCUNILOG_LOGFILEifOpen (CUNILOG_LOGFILE *cl)
{
	#ifdef OS_IS_WINDOWS
		if (cl->hLogFile)
		{
			CloseHandle (cl->hLogFile);
			cl->hLogFile = NULL;
		}
	#else
		if (cl->fLogFile)
		{
			fclose (cl->fLogFile);
			cl->fLogFile = 0;
		}
	#endif
}

static void DoneSCUNILOGTARGETprocessors (SCUNILOGTARGET *psu)
{
	ubf_assert_non_NULL (psu);

	unsigned int			np = psu->nprocessors;
	unsigned int			ui;
	CUNILOG_PROCESSOR		*cp;
	CUNILOG_ROTATION_DATA	*rt;

	for (ui = 0; ui < np; ++ ui)
	{
		cp = psu->cprocessors [ui];
		switch (cp->task)
		{
			case cunilogProcessNoOperation:
			case cunilogProcessEchoToConsole:
			case cunilogProcessUpdateLogFileName:
			case cunilogProcessFlushLogFile:
			case cunilogProcessCustomProcessor:
			case cunilogProcessAmountEnumValues:
				break;
			case cunilogProcessWriteToLogFile:
				CloseCUNILOG_LOGFILEifOpen (cp->pData);
				break;
			case cunilogProcessRotateLogfiles:
				rt = cp->pData;
				if (cunilogHasRotatorFlag_USE_MBSRCMASK (rt))
					doneSMEMBUF (&rt->mbSrcMask);
				if (cunilogHasRotatorFlag_USE_MBDSTFILE (rt))
					doneSMEMBUF (&rt->mbDstFile);
				break;
		}
	}

	if (cunilogIsProcessorsAllocated (psu))
	{
		for (ui = 0; ui < np; ++ ui)
		{
			cp = psu->cprocessors [ui];
			free (cp);
		}
	}
}

/*
	This function is a no-op.
*/
static void DoneSCUNILOGTARGETpsdump (SCUNILOGTARGET *psu)
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

static void DoneSCUNILOGTARGETmembers (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsLogPathAllocated (put))
		freeSMEMBUF (&put->mbLogPath);
	if (cunilogIsAppNameAllocated (put))
		freeSMEMBUF (&put->mbAppName);
	if (cunilogIsLogFileAllocated (put))
		freeSMEMBUF (&put->mbLogfileName);
	if (cunilogIsLogFileMaskAllocated (put))
		freeSMEMBUF (&put->mbLogFileMask);
	if (cunilogIsFileToRotateAllocated (put))
		freeSMEMBUF (&put->mbFilToRotate);
	if (cunilogIsEvtLineAllocated (put))
		freeSMEMBUF (&put->mbLogEventLine);
	DoneSCUNILOGNPI (&put->scuNPI);
	DoneCUNILOG_LOCKER (put);
	DoneSCUNILOGTARGETqueuesemaphore (put);
	vec_deinit (&put->fls);
	DoneSBULKMEM (&put->sbm);
	DoneSCUNILOGTARGETpsdump (put);
}

SCUNILOGTARGET *DoneSCUNILOGTARGET (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);
	ubf_assert (cunilogIsTargetInitialised (put));

	DoneSCUNILOGTARGETprocessors (put);
	DoneSCUNILOGTARGETmembers (put);
	if (cunilogIsTargetAllocated (put))
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
	,	cunilogEvtSeverityBlanks								//  1
	,	cunilogEvtSeverityEmergency								//	2
	,	cunilogEvtSeverityNotice								//	3
	,	cunilogEvtSeverityInfo									//  4
	,	cunilogEvtSeverityMessage								//  5
	,	cunilogEvtSeverityWarning								//  6
	,	cunilogEvtSeverityError									//  7
	,	cunilogEvtSeverityFail									//  8
	,	cunilogEvtSeverityCritical								//  9
	,	cunilogEvtSeverityFatal									// 10
	,	cunilogEvtSeverityDebug									// 11
	,	cunilogEvtSeverityTrace									// 12
	,	cunilogEvtSeverityDetail								// 13
	,	cunilogEvtSeverityVerbose = cunilogEvtSeverityDetail	// 13
	,	cunilogEvtSeverityIllegal								// 14
}
;
*/
const char *EventSeverityTexts []	=
{
	/*	 0	*/	"",
	/*	 1	*/	"   ",
	/*	 2	*/	"EMG",
	/*	 3	*/	"NTC",
	/*	 4	*/	"INF",
	/*	 5	*/	"MSG",
	/*	 6	*/	"WRN",
	/*	 7	*/	"ERR",
	/*	 8	*/	"FAI",
	/*	 9	*/	"CRI",
	/*	10	*/	"FTL",
	/*	11	*/	"DBG",
	/*	12	*/	"TRC",
	/*	13	*/	"DET",
	/*	14	*/	"ILG"
};

static const char *unilogTextFromEventSeverity (enum cunilogeventseverity evSeverity)
{
	// Check consistency between unilogstruct.h and the declarations here.
	ubf_assert (0 == cunilogEvtSeverityNone);
	ubf_assert (1 == cunilogEvtSeverityBlanks);
	ubf_assert (2 == cunilogEvtSeverityEmergency);
	ubf_assert (3 == cunilogEvtSeverityNotice);
	ubf_assert (4 == cunilogEvtSeverityInfo);
	ubf_assert (5 == cunilogEvtSeverityMessage);
	ubf_assert (6 == cunilogEvtSeverityWarning);
	ubf_assert (7 == cunilogEvtSeverityError);
	ubf_assert (8 == cunilogEvtSeverityFail);
	ubf_assert (9 == cunilogEvtSeverityCritical);
	ubf_assert (10 == cunilogEvtSeverityFatal);
	ubf_assert (11 == cunilogEvtSeverityDebug);
	ubf_assert (12 == cunilogEvtSeverityTrace);
	ubf_assert (13 == cunilogEvtSeverityDetail);
	ubf_assert (13 == cunilogEvtSeverityVerbose);
	ubf_assert (14 == cunilogEvtSeverityIllegal);
	ubf_assert (cunilogEvtSeverityIllegal > evSeverity);

	ubf_assert (0 == strlen (EventSeverityTexts [cunilogEvtSeverityNone]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityBlanks]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityEmergency]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityNotice]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityInfo]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityMessage]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityWarning]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityError]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityFail]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityCritical]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityFatal]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityDebug]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityTrace]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityDetail]));
	ubf_assert (3 == strlen (EventSeverityTexts [cunilogEvtSeverityVerbose]));

	return EventSeverityTexts [evSeverity];
}

static size_t requiredEventSeverityChars (cueventseverity sev)
{
	// "" or "INF" + " ".
	return cunilogEvtSeverityNone == sev ? 0 : 3 + 1;
}

static size_t writeEventSeverity (char *szOut, cueventseverity sev)
{
	ubf_assert_non_NULL (szOut);

	if (cunilogEvtSeverityNone != sev)
	{
		memcpy (szOut, unilogTextFromEventSeverity (sev), 3);
		szOut [3] = ' ';
		return 3 + 1;
	}
	return 0;
}

static inline size_t requiredFullstopChars (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);

	return cunilogIsEventAutoFullstop (pev) ? 1 : 0;
};

static inline size_t writeFullStop (char *szOut, SCUNILOGEVENT *pev)
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

static inline size_t eventLenNewline (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);

	return lnLineEnding (pev->pSCUNILOGTARGET->unilogNewLine);
}

static inline enum enLineEndings eventLineEnding (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);

	return pev->pSCUNILOGTARGET->unilogNewLine;
}

static inline void evtTSFormats_unilogEvtTS_ISO8601 (char *chISO, UBF_TIMESTAMP ts)
{
	ISO8601_from_UBF_TIMESTAMP_c (chISO, ts);
	chISO [LEN_ISO8601DATETIMESTAMPMS]		= ' ';
}

static inline void evtTSFormats_unilogEvtTS_ISO8601T (char *chISO, UBF_TIMESTAMP ts)
{
	ISO8601T_from_UBF_TIMESTAMP_c (chISO, ts);
	chISO [LEN_ISO8601DATETIMESTAMPMS]		= ' ';
}

static inline void evtTSFormats_unilogEvtTS_ISO8601_3spc (char *chISO, UBF_TIMESTAMP ts)
{
	ISO8601_from_UBF_TIMESTAMP_c (chISO, ts);
	chISO [LEN_ISO8601DATETIMESTAMPMS]		= ' ';
	chISO [LEN_ISO8601DATETIMESTAMPMS + 1]	= ' ';
	chISO [LEN_ISO8601DATETIMESTAMPMS + 2]	= ' ';
}

static inline void evtTSFormats_unilogEvtTS_ISO8601T_3spc (char *chISO, UBF_TIMESTAMP ts)
{
	ISO8601T_from_UBF_TIMESTAMP_c (chISO, ts);
	chISO [LEN_ISO8601DATETIMESTAMPMS]		= ' ';
	chISO [LEN_ISO8601DATETIMESTAMPMS + 1]	= ' ';
	chISO [LEN_ISO8601DATETIMESTAMPMS + 2]	= ' ';
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
};

static inline size_t requiredEvtLineTimestampAndSeverityLength (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);

	size_t	r;

	// "YYYY-MM-DD HH:MI:SS.000+01:00" + " ".
	r = evtTSFormats [pev->pSCUNILOGTARGET->unilogEvtTSformat].len;
	// "WRN" + " "
	r += requiredEventSeverityChars (pev->evSeverity);

	return r;
}

static inline size_t requiredEventLineSizeU8 (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);
	ubf_assert (cunilogEvtTypeNormalText == pev->evType);

	size_t	r;

	// "YYYY-MM-DD HH:MI:SS.000+01:00" + " " + "WRN" + " ".
	r = requiredEvtLineTimestampAndSeverityLength (pev);
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// Actual data.
	r += pev->lenDataToLog;
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// "."
	r += requiredFullstopChars (pev);
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// CR + LF max.
	r += eventLenNewline (pev);
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// Terminating NUL.
	r += 1;
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	return r;
}

/*
	writeEventLineFromSUNILOGEVENTU8

	The caller is responsible for szEventLine to point to a buffer big enough to hold the
	event line. The function requiredEventLineSize () obtains this size.
*/
static size_t writeEventLineFromSUNILOGEVENTU8 (char *szEventLine, SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (szEventLine);
	ubf_assert_non_NULL (pev);
	ubf_assert (cunilogEvtTypeNormalText == pev->evType);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);
	ubf_assert (cunilogIsTargetInitialised (pev->pSCUNILOGTARGET));

	char	*szOrg = szEventLine;

	DBG_RESET_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker);

	// "YYYY-MM-DD HH:MI:SS.000+01:00" + " " (see table above).
	evtTSFormats [pev->pSCUNILOGTARGET->unilogEvtTSformat].fnc (szEventLine, pev->stamp);
	//ISO8601_from_UBF_TIMESTAMP (szEventLine, pue->stamp);

	szEventLine += evtTSFormats [pev->pSCUNILOGTARGET->unilogEvtTSformat].len;
	szEventLine += writeEventSeverity (szEventLine, pev->evSeverity);
	DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, szEventLine - szOrg);

	memcpy (szEventLine, pev->szDataToLog, pev->lenDataToLog);
	szEventLine += pev->lenDataToLog;
	DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, szEventLine - szOrg);

	szEventLine += writeFullStop (szEventLine, pev);
	DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, szEventLine - szOrg);

	// We've reserved space for a new line character sequence in requiredEventLineSizeU8 ()
	//	but we're not going to add this here yet. Instead, we NUL-terminate. This simply lets
	//	us call puts (). We'll add the new line sequence later in the processor that writes
	//	to the logfile.

	// The NUL byte is useful for OS API functions.
	szEventLine [0] = ASCII_NUL;
	return szEventLine - szOrg;
}

/*
static bool obtainValidSCUNILOGDUMPinSCUNILOGTARGET (SCUNILOGTARGET *put)
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
						SCUNILOGEVENT		*pev
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

	SCUNILOGTARGET *put = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (put);

	size_t	r;

	/*
		Timestamp + Type + caption, etc.
	*/

	// "YYYY-MM-DD HH:MI:SS.000+01:00" + " " + "WRN" + " ".
	r = requiredEvtLineTimestampAndSeverityLength (pev);
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// The width of the caption length.
	size_t ui			= widthOfCaptionLengthFromCunilogEventType (pev->evType);
	// Its actual length. This needs to be added to our return value.
	size_t lenCaption	= readCaptionLengthFromData (pev->szDataToLog, ui);
	*pDumpData			= pev->szDataToLog + ui + lenCaption;
	*width				= ui;
	*len				= lenCaption;
	r += lenCaption;
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// "."
	r += requiredFullstopChars (pev);
	// New line.
	r += eventLenNewline (pev);
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// Actual dump data size. Includes NUL terminator.
	size_t sizHexDmp = hxdmpRequiredSize (pev->lenDataToLog, put->dumpWidth, eventLineEnding (pev));
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, sizHexDmp);
	r += sizHexDmp;
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// ASCII TAB.
	++ r;

	// To be considered in the future:
	//	It would be faster to just always reserve UBF_UINT64_LEN octets instead of
	//	performing the uint -> ASCII transformation.
	char cDmpOctets [UBF_UINT64_SIZE];
	//r += ubf_str_from_uint64 (cDmpOctets, pev->lenDataToLog);
	ubf_str__from_uint64 (cDmpOctets, 10, pev->lenDataToLog);
	r += 10;
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	r += lnSummaryOctets;
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	// New line.
	r += eventLenNewline (pev);
	DBG_TRACK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, r);

	#ifdef DEBUG
		++ r;												// Space for a debug marker.
	#endif

	return r;
}

static size_t createDumpEventLineFromSUNILOGEVENT (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);
	ubf_assert (isInitialisedSMEMBUF (&pev->pSCUNILOGTARGET->mbLogEventLine));
	ubf_assert	(
						cunilogEvtTypeHexDumpWithCaption8	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption16	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption32	== pev->evType
					||	cunilogEvtTypeHexDumpWithCaption64	== pev->evType
				);

	SCUNILOGTARGET *put = pev->pSCUNILOGTARGET;
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
		szOut += writeEventSeverity (szOut, pev->evSeverity);
		DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, szOut - szOrg);

		// Caption.
		memcpy (szOut, pev->szDataToLog + captionWidth, captionLen);
		szOut += captionLen;
		DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, szOut - szOrg);

		// "." + new line.
		size_t lenNewLine = eventLenNewline (pev);
		szOut += writeFullStop (szOut, pev);
		memcpy (szOut, ccLineEnding (eventLineEnding (pev)), lenNewLine);
		szOut += lenNewLine;
		DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, szOut - szOrg);

		put->lnLogEventLine = szOut - szOrg;
		char *szHexDmpOut = szOut;
		size_t sizHx = hxdmpWriteHexDump	(
						szHexDmpOut, pDumpData, pev->lenDataToLog,
						put->dumpWidth, put->unilogNewLine
											);
		DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, sizHx + 1);
		ubf_assert (CUNILOG_DEFAULT_DBG_CHAR == put->mbLogEventLine.buf.pch [lenTotal]);
		put->lnLogEventLine += sizHx;
		DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, put->lnLogEventLine + 1);

		szOut = szHexDmpOut + sizHx;
		szOut [0] = ASCII_TAB;
		++ szOut;
		++ put->lnLogEventLine;

		//size_t lnOctets = ubf_str_from_uint64 (szOut, pev->lenDataToLog);
		size_t lnOctets = 10;
		ubf_str__from_uint64 (szOut, 10, pev->lenDataToLog);
		put->lnLogEventLine += lnOctets;
		ubf_assert (CUNILOG_DEFAULT_DBG_CHAR == put->mbLogEventLine.buf.pch [lenTotal]);
		DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, put->lnLogEventLine + 1);

		szOut += lnOctets;
		memcpy (szOut, scSummaryOctets, lnSummaryOctets + 1);
		put->lnLogEventLine += lnSummaryOctets;
		DBG_TRACK_CHECK_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker, put->lnLogEventLine + 1);

		return put->lnLogEventLine;
	}
	return CUNILOG_SIZE_ERROR;
}

static size_t createU8EventLineFromSUNILOGEVENT (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);
	ubf_assert (isInitialisedSMEMBUF (&pev->pSCUNILOGTARGET->mbLogEventLine));
	ubf_assert (cunilogEvtTypeNormalText == pev->evType);

	size_t requiredEvtLineSize;

	requiredEvtLineSize = requiredEventLineSizeU8 (pev);
	growToSizeSMEMBUF64aligned (&pev->pSCUNILOGTARGET->mbLogEventLine, requiredEvtLineSize);
	if (isUsableSMEMBUF (&pev->pSCUNILOGTARGET->mbLogEventLine))
	{
		pev->pSCUNILOGTARGET->lnLogEventLine =
			writeEventLineFromSUNILOGEVENTU8 (pev->pSCUNILOGTARGET->mbLogEventLine.buf.pch, pev);
		return pev->pSCUNILOGTARGET->lnLogEventLine;
	}
	return CUNILOG_SIZE_ERROR;
}

static size_t createEventLineFromSUNILOGEVENT (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);
	ubf_assert (isInitialisedSMEMBUF (&pev->pSCUNILOGTARGET->mbLogEventLine));

	DBG_RESET_CNTTRACKER (pev->pSCUNILOGTARGET->evtLineTracker);

	switch (pev->evType)
	{
		case cunilogEvtTypeHexDumpWithCaption8:
		case cunilogEvtTypeHexDumpWithCaption16:
		case cunilogEvtTypeHexDumpWithCaption32:
		case cunilogEvtTypeHexDumpWithCaption64:
			return createDumpEventLineFromSUNILOGEVENT	(pev);
		case cunilogEvtTypeNormalText:
			return createU8EventLineFromSUNILOGEVENT	(pev);
		default:
			break;
	}
	return CUNILOG_SIZE_ERROR;
}

static inline void storeCaptionLengthInData (unsigned char **pData, size_t ui, size_t lenCapt)
{
	uint8_t			ui8;
	uint16_t		ui16;
	uint32_t		ui32;
	uint64_t		ui64;

	switch (ui)
	{
		case 0:
			return;
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

static SCUNILOGEVENT *CreateSCUNILOGEVENTandData	(
					SCUNILOGTARGET				*put,
					cueventseverity				sev,
					const char					*ccCapt,
					size_t						lenCapt,
					cueventtype					type,
					const char					*ccData,
					size_t						siz
													)
{
	ubf_assert_non_NULL (put);
	ubf_assert_non_NULL (ccData);
	ubf_assert (USE_STRLEN != siz);

	size_t			ui		= widthOfCaptionLengthFromCunilogEventType (type);
	size_t			aln		= ALIGNED_SIZE (sizeof (SCUNILOGEVENT), CUNILOG_DEFAULT_ALIGNMENT);
	size_t			ln		= aln + ui + lenCapt + siz;
	SCUNILOGEVENT	*pev	= ubf_malloc (ln);

	if (pev)
	{
		unsigned char *pData = (unsigned char *) pev + aln;
		FillSCUNILOGEVENT	(
			pev, put,
			CUNILOGEVENT_ALLOCATED,
			LocalTime_UBF_TIMESTAMP (),
			sev, type,
			pData, siz
							);
		storeCaptionLengthInData (&pData, ui, lenCapt);
		if (ui)
		{
			memcpy (pData, ccCapt, lenCapt);
			pData += lenCapt;
		}
		memcpy (pData, ccData, siz);
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

SCUNILOGEVENT *CreateSCUNILOGEVENT_Data		(
					SCUNILOGTARGET				*put,
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

	SCUNILOGEVENT *pev = CreateSCUNILOGEVENTandData	(
							put, sev, ccCapt, lenCapt,
							cunilogEventTypeFromLength (lenCapt),
							ccData, siz
													);
	return pev;
}

SCUNILOGEVENT *CreateSCUNILOGEVENT_Text		(
					SCUNILOGTARGET				*put,
					cueventseverity				sev,
					const char					*ccText,
					size_t						len
											)
{
	ubf_assert_non_NULL (put);
	ubf_assert_non_NULL (ccText);

	len = USE_STRLEN == len ? strlen (ccText) : len;

	while (len && ('\n' == ccText [len - 1] || '\r' == ccText [len - 1]))
		-- len;

	SCUNILOGEVENT *pev = CreateSCUNILOGEVENTandData	(
							put, sev, NULL, 0, cunilogEvtTypeNormalText,
							ccText, len
													);
	return pev;
}

SCUNILOGEVENT *CreateSUNILOGEVENT_W	(
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

SCUNILOGEVENT *DoneSCUNILOGEVENT (SCUNILOGEVENT *pev)
{
	if (pev->szDataToLog && cunilogIsEventDataAllocated (pev))
	{
		ubf_free (pev->szDataToLog);
	}
	if (cunilogIsEventAllocated (pev))
	{
		ubf_free (pev);
	}
	return NULL;
}

/*
	The dummy/no-operation processor.
*/
static void cunilogProcessNoneFnct (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	UNREFERENCED_PARAMETER (cup);
	UNREFERENCED_PARAMETER (pev);
	
}

static void cunilogProcessEchoFnct (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	UNREFERENCED_PARAMETER (cup);
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);

	if (cunilogIsNoEcho (pev->pSCUNILOGTARGET))
		return;

	// The actual task of this processor: Echo the event line.
	//	Note that we can rely on the following conditions here:
	//		- The line to output is NUL-terminated.
	//		- It only consists of printable characters.
	//		- The length of the event line has been stored correctly.
	//		- If we require a lock, we have it already.
	cunilog_puts (pev->pSCUNILOGTARGET->mbLogEventLine.buf.pch);
}

static void unilogProcessUpdateLogPathFnct (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	UNREFERENCED_PARAMETER (cup);
	ubf_assert_non_NULL (pev);
	
	SCUNILOGTARGET	*put = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (put);

	#ifdef DEBUG
		char *sz = put->mbLogfileName.buf.pch;
		UNREFERENCED_PARAMETER (sz);
	#endif

	size_t lenPostfixStamp = lenDateTimeStampFromPostfix (put->culogPostfix);
	memcpy (put->cPrevDateTimeStamp, put->szDateTimeStamp, lenPostfixStamp);

	obtainDateAndTimeStamp (put->szDateTimeStamp, pev->stamp, put->culogPostfix);

	// The cunilogPostfixMinute postfix causes a colon to appear between hours and minutes.
	//	We simply replace it with an underscore.
	put->szDateTimeStamp [13] = cunilogPostfixMinute == put->culogPostfix ? '_' : put->szDateTimeStamp [13];

	put->szDateTimeStamp [lenPostfixStamp] = '.';
}

static bool cunilogOpenLogFile (CUNILOG_LOGFILE *pl, const char *szLogFileName)
{
	#ifdef PLATFORM_IS_WINDOWS
		pl->hLogFile = CreateFileU8	(
						szLogFileName, CUNILOG_DEFAULT_OPEN_MODE,
						FILE_SHARE_DELETE | FILE_SHARE_READ,
						NULL, OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
						NULL
									);
		return NULL == pl->hLogFile || INVALID_HANDLE_VALUE == pl->hLogFile;
	#else
		// We always (and automatically) append.
		pl->fLogFile = fopen (szLogFileName, CUNILOG_DEFAULT_OPEN_MODE);
		return NULL != pl->fLogFile;
	#endif
}

/*
	Closes the previous file and opens the new one.
*/
static bool cunilogOpenNewLogFile (CUNILOG_LOGFILE *pl, const char *szLogFileName)
{
	#ifdef OS_IS_WINDOWS
		CloseHandle (pl->hLogFile);
		return cunilogOpenLogFile (pl, szLogFileName);
	#else
		fclose (pl->fLogFile);
		return cunilogOpenLogFile (pl, szLogFileName);
	#endif
}

static inline bool requiresOpenLogFile (CUNILOG_LOGFILE *pl)
{
	#ifdef OS_IS_WINDOWS
		return NULL == pl->hLogFile || INVALID_HANDLE_VALUE == pl->hLogFile;
	#else
		return NULL == pl->fLogFile;
	#endif
}

static inline bool requiresNewLogFile (SCUNILOGTARGET *put)
{
	return memcmp	(
		put->cPrevDateTimeStamp, put->szDateTimeStamp,
		lenDateTimeStampFromPostfix (put->culogPostfix)
					);
}

static inline size_t addNewLineToLogEventLine (char *pData, size_t lnData, enum enLineEndings nl)
{	// At least one octet has been reserved for a newline character, and one
	//	for NUL, hence we're definitely safe to write 2 more octets.
	size_t len;
	const char *cc = szLineEnding (nl, &len);
	ubf_assert (len <= 2);
	memcpy (pData + lnData, cc, len + 1);
	return lnData + len;
}

static bool cunilogWriteDataToLogFile (CUNILOG_LOGFILE *pl, char *pData, size_t lnData, enum enLineEndings nl)
{
	#ifdef OS_IS_WINDOWS
		DWORD dwWritten;
		DWORD toWrite = addNewLineToLogEventLine (pData, lnData, nl) & 0xFFFFFFFF;
		// The file has been opened with FILE_APPEND_DATA, i.e. we don't need to
		//	seek ourselves.
		//LARGE_INTEGER	z = {0, 0};
		//SetFilePointerEx (pl->hLogFile, z, NULL, FILE_END);
		bool b = WriteFile (pl->hLogFile, pData, toWrite, &dwWritten, NULL);
		return b;
	#else
		long lToWrite = (long) addNewLineToLogEventLine (pData, lnData, nl);
		// See https://www.man7.org/linux/man-pages/man3/fopen.3.html .
		//	A call "fseek (pl->fLogFile, (long) 0, SEEK_END);" is not required
		//	because we opened the file in append mode.
		size_t st = fwrite (pData, 1, lToWrite, pl->fLogFile);
		return st == lnData;
	#endif
}

static void cunilogProcessWriteToLogFileFnct (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	ubf_assert_non_NULL (pev->pSCUNILOGTARGET);
	SCUNILOGTARGET	*put = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (put);
	ubf_assert (isUsableSMEMBUF (&put->mbLogfileName));

	CUNILOG_LOGFILE	*pl	= cup->pData;
	ubf_assert_non_NULL (pl);

	if (isUsableSMEMBUF (&put->mbLogfileName))
	{
		if (requiresOpenLogFile (pl))
			cunilogOpenLogFile (pl, put->mbLogfileName.buf.pcc);
		else
		if (requiresNewLogFile (put))
			cunilogOpenNewLogFile (pl, put->mbLogfileName.buf.pcc);
		cunilogWriteDataToLogFile (pl, put->mbLogEventLine.buf.pch, put->lnLogEventLine, put->unilogNewLine);
	}
}

static void cunilogProcessFlushFnct (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	UNREFERENCED_PARAMETER (pev);

	ubf_assert_non_NULL (pev);
	
	/*
		Not necessary for this processor.

		SCUNILOGTARGET	*put = pev->pSCUNILOGTARGET;
		ubf_assert_non_NULL (put);
	*/

	CUNILOG_LOGFILE *pcl = cup->pData;

	#ifdef OS_IS_WINDOWS
		#ifdef DEBUG
			bool b = FlushFileBuffers (pcl->hLogFile);
			UNREFERENCED_PARAMETER (b);
		#else
			FlushFileBuffers (pcl->hLogFile);
		#endif
	#else
		#ifdef DEBUG
			int r = fflush (pcl->fLogFile);
			ubf_assert (0 == r);
			UNREFERENCED_PARAMETER (r);
		#else
			fflush (pcl->fLogFile);
		#endif
	#endif
}

static bool cunilogProcessEventSingleThreaded (SCUNILOGEVENT *pev);
static void enqueueAndTriggerSeparateLoggingThread (SCUNILOGEVENT *pev);

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY

	static inline void IncrementPendingNoRotationEvents (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
		++ put->nPendingNoRotEvts;
		//printf ("%" PRIu64 "\n", put->nPendingNoRotEvts);
	}
	static inline void DecrementPendingNoRotationEvents (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
		-- put->nPendingNoRotEvts;
		//printf ("%" PRIu64 "\n", put->nPendingNoRotEvts);
	}
#else
	#define IncrementNoRotationEvents(p)
	#define DecrementNoRotationEvents(p)
#endif

static void logNoRotationTextU8fmt (SCUNILOGTARGET *put, const char *fmt, ...)
{
	va_list		ap;
	size_t		len;

	ubf_assert_non_NULL (put);

	va_start (ap, fmt);
	len = (size_t) vsnprintf (NULL, 0, fmt, ap);
	va_end (ap);

	char *szTxtToLog = ubf_malloc (len + 1);
	if (szTxtToLog)
	{
		va_start (ap, fmt);
		vsnprintf (szTxtToLog, len + 1, fmt, ap);
		va_end (ap);

		SCUNILOGEVENT *pev = CreateSCUNILOGEVENT_Text (put, cunilogEvtSeverityNone, szTxtToLog, len);
		ubf_assert_non_NULL (pev);
		if (pev)
		{
			cunilogSetEventNoRotation (pev);
			#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
				if (hasSCUNILOGTARGETqueue (put))
				{
					IncrementPendingNoRotationEvents (put);
					enqueueAndTriggerSeparateLoggingThread (pev);
				}else
			#endif
					cunilogProcessEventSingleThreaded (pev);
		}
		ubf_free (szTxtToLog);
	}
}

#ifdef OS_IS_WINDOWS
	static void GetTextForLastError (char *szErrMsg)
	{
		ubf_assert_non_NULL (szErrMsg);

		// Note that we expect a buffer of CUNILOG_STD_MSG_SIZE octets.
		DWORD dwError = GetLastError ();
		GetWinErrorTextU8 (szErrMsg, CUNILOG_STD_MSG_SIZE, dwError);
	}
#else
	static void GetTextForLastError (char *szErrMsg)
	{
		ubf_assert_non_NULL (szErrMsg);

		char szErr [] = "\"Not implemented\"";
		memcpy (szErrMsg, szErr, sizeof (szErr));
	}
#endif

static void fsComprLogIntTxt (SCUNILOGTARGET *put, const char *szStrArg)
{
	logNoRotationTextU8fmt (put, szStrArg, put->mbFilToRotate.buf.pch);
}

static void FileSystemCompressLogfile (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	bool b;
	char szErr [CUNILOG_STD_MSG_SIZE];
	
	enfilecompressresult	cmprRes = IsFileCompressedByName (put->mbFilToRotate.buf.pch);
	switch (cmprRes)
	{
		case fscompress_uncompressed:
			fsComprLogIntTxt (put, "Initiating file system compression for file \"%s\"...\n");
			b = FScompressFileByName (put->mbFilToRotate.buf.pch);
			if (b)
			{
				fsComprLogIntTxt (put, "File system compression for file \"%s\" initiated.\n");
			} else
			{
				GetTextForLastError (szErr);
				logNoRotationTextU8fmt	(
					put,
					"Error %s while attempting to initiate file system compression for file \"%s\".\n",
					szErr, put->mbFilToRotate.buf.pch
										);
			}
			break;
		case fscompress_compressed:
			break;
		case fscompress_error:
			GetTextForLastError (szErr);
			logNoRotationTextU8fmt	(
				put,
				"Error %s while attempting to check file system compression for file \"%s\".\n",
				szErr, put->mbFilToRotate.buf.pch
									);
			break;
	}
}

#if defined (OS_IS_WINDOWS) && defined (HAVE_SHELLAPI)

	static void MoveFileToRecycleBinWin (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		logNoRotationTextU8fmt (put, "Moving obsolete logfile \"%s\" to recycle bin...\n", put->mbFilToRotate.buf.pch);
		bool b = MoveToRecycleBinU8 (put->mbFilToRotate.buf.pch);
		if (b)
		{
			logNoRotationTextU8fmt (put, "Obsolete logfile \"%s\" moved to recycle bin.\n", put->mbFilToRotate.buf.pch);
		} else
		{
			char szErr [CUNILOG_STD_MSG_SIZE];
			GetTextForLastError (szErr);
			logNoRotationTextU8fmt (put, "Error %s while attempting to move obsolete logfile \"%s\" to recycle bin.\n", szErr, put->mbFilToRotate.buf.pch);
		}
	}

#elif OS_IS_MACOS

	#include "./../OS/Apple/TrashCan.h"

	// Our wrapper.
	static void MoveFileToRecycleBinMac (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		logNoRotationTextU8fmt (put, "Moving obsolete logfile \"%s\" to recycle bin...\n", put->mbFilToRotate.buf.pch);
		if (LIBTRASHCAN_SUCCESS == trashcan_soft_delete_apple (put->mbFilToRotate.buf.pch))
		{
			logNoRotationTextU8fmt (put, "Obsolete logfile \"%s\" moved to recycle bin.\n", put->mbFilToRotate.buf.pch);
		} else
		{
			logNoRotationTextU8fmt (put, "Error while attempting to move obsolete logfile \"%s\" to recycle bin.\n", put->mbFilToRotate.buf.pch);
		}
	}

#elif defined (OS_IS_LINUX)

	static void MoveFileToRecycleBinLnx (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		logNoRotationTextU8fmt (put, "Moving obsolete logfile \"%s\" to recycle bin...\n", put->mbFilToRotate.buf.pch);
		bool b = MoveFileToTrashPOSIX (put->mbFilToRotate.buf.pch);
		if (b)
		{
			logNoRotationTextU8fmt (put, "Obsolete logfile \"%s\" moved to recycle bin.\n", put->mbFilToRotate.buf.pch);
		} else
		{
			char szErr [CUNILOG_STD_MSG_SIZE];
			GetTextForLastError (szErr);
			logNoRotationTextU8fmt (put, "Error %s while attempting to move obsolete logfile \"%s\" to recycle bin.\n", szErr, put->mbFilToRotate.buf.pch);
		}
	}

#else

	#error To do!!!

#endif

/*
	Platform-independent wrapper function.
*/
static inline void cuMoveFileToRecycleBin (SCUNILOGTARGET *put)
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
	static void DeleteObsoleteLogfile (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
	
		logNoRotationTextU8fmt (put, "Deleting obsolete logfile \"%s\"...\n", put->mbFilToRotate.buf.pch);
		bool b = DeleteFileU8 (put->mbFilToRotate.buf.pch);
		if (b)
		{
			logNoRotationTextU8fmt (put, "Obsolete logfile \"%s\" deleted.\n", put->mbFilToRotate.buf.pch);
		} else
		{
			char szErr [CUNILOG_STD_MSG_SIZE];
			GetTextForLastError (szErr);
			logNoRotationTextU8fmt (put, "Error %s while attempting to delete obsolete logfile \"%s\".\n", szErr, put->mbFilToRotate.buf.pch);
		}
	}
#endif

#ifdef PLATFORM_IS_POSIX
	static void DeleteObsoleteLogfile (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
	
		logNoRotationTextU8fmt (put, "Deleting obsolete logfile \"%s\"...\n", put->mbFilToRotate.buf.pch);
		int i = unlink (put->mbFilToRotate.buf.pch);
		if (0 == i)
		{
			logNoRotationTextU8fmt (put, "Obsolete logfile \"%s\" deleted.\n", put->mbFilToRotate.buf.pch);
		} else
		{
			logNoRotationTextU8fmt (put, "Error %d while attempting to delete obsolete logfile \"%s\".\n", errno, put->mbFilToRotate.buf.pch);
		}
	}
#endif

static void performActualRotation (CUNILOG_ROTATOR_ARGS *prg)
{
	ubf_assert_non_NULL (prg);

	CUNILOG_PROCESSOR		*cup = prg->cup;
	ubf_assert_non_NULL (cup);
	CUNILOG_ROTATION_DATA	*prd = cup->pData;
	ubf_assert_non_NULL (prd);
	SCUNILOGEVENT			*pev = prg->pev;
	ubf_assert_non_NULL	(pev);
	SCUNILOGTARGET			*put = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (put);

	switch (prd->tsk)
	{
		case cunilogrotationtask_None:
			// This is a bug. We're normally not called in this case. See function
			//	cunilogProcessRotateLogfiles ().
			ubf_assert (false);
			break;
		case cunilogrotationtask_FScompressLogfiles:
			FileSystemCompressLogfile (put);
			break;
		case cunilogrotationtask_MoveToRecycleBinLogfiles:
			cuMoveFileToRecycleBin (put);
			break;
		case cunilogrotationtask_DeleteLogfiles:
			DeleteObsoleteLogfile (put);
			break;
	}
}

static void prepareU8fullFileNameToRotate (CUNILOG_ROTATOR_ARGS *prg)
{
	ubf_assert_non_NULL (prg);
	SCUNILOGEVENT			*pev = prg->pev;
	ubf_assert_non_NULL (pev);
	SCUNILOGTARGET			*put = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (put);

	growToSizeSMEMBUF (&put->mbFilToRotate, put->lnLogPath + prg->siz);
	if (isUsableSMEMBUF (&put->mbFilToRotate))
	{
		memcpy (put->mbFilToRotate.buf.pch + put->lnLogPath, prg->nam, prg->siz);
	}
}

/*
	Compare function for vec_sort ().
*/
static int flscmp (const void *p1, const void *p2)
{
	const CUNILOG_FLS	*fls1 = p1;
	const CUNILOG_FLS	*fls2 = p2;
	int					r = 0;

	if (fls1->stFilename == fls2->stFilename)
	{
		r = memcmp (fls1->chFilename, fls2->chFilename, fls1->stFilename - 1);
		return r;
	}
	// We should never get here. That's a bug, misconfiguration, or someone inserted
	//	a unicode character manually.
	ubf_assert (false);
	if (fls1->stFilename < fls2->stFilename)
	{	
		ubf_assert (false);
		r = memcmp (fls1->chFilename, fls2->chFilename, fls1->stFilename - 1);
	}
	if (fls1->stFilename > fls2->stFilename)
	{
		ubf_assert (false);
		r = memcmp (fls1->chFilename, fls2->chFilename, fls2->stFilename - 1);
	}
	return r;
}

static void prapareLogfilesListAndRotate (CUNILOG_ROTATOR_ARGS *prg)
{
	ubf_assert_non_NULL (prg);
	SCUNILOGEVENT			*pev = prg->pev;
	ubf_assert_non_NULL (pev);
	SCUNILOGTARGET			*put = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (put);
	CUNILOG_PROCESSOR		*cup = prg->cup;
	ubf_assert_non_NULL (cup);
	CUNILOG_ROTATION_DATA	*prd = cup->pData;
	ubf_assert_non_NULL (prd);

	/*
		On NTFS, files are returned in descending alphabetical order.
		This means it's enough to just reverse the order, i.e. start with the last
		file first and walk downwards. This may also be true for other file systems,
		though on file systems that return the files randomly we got to sort the vector
		first.
	*/
	if (cunilogIsFSneedsSorting (put))
		vec_sort (&put->fls, flscmp);
	size_t iFiles = put->fls.length;
	uint64_t nToIgnore = prd->nIgnore + put->scuNPI.nIgnoredTotal;
	uint64_t nMaxToRot =		CUNILOG_MAX_ROTATE_AUTO - nToIgnore <= prd->nMaxToRotate
							?	prd->nMaxToRotate
							:	nToIgnore + prd->nMaxToRotate;
	while (iFiles --)
	{
		//puts (put->fls.data [iFiles].chFilename);
		++ prd->nCnt;
		if (prd->nCnt > nToIgnore)
		{
			if (prd->nCnt <= nMaxToRot)
			{
				prg->nam = put->fls.data [iFiles].chFilename;
				prg->siz = put->fls.data [iFiles].stFilename;
				prepareU8fullFileNameToRotate (prg);
				performActualRotation (prg);
			} else
				return;
		} else
		{	// This tells the next rotation processor how many we've ignored already.
			++ put->scuNPI.nIgnoredTotal;
		}
	}
}

#if defined (PLATFORM_IS_WINDOWS)
	static bool obtainLogfilesListToRotateCallbackWin (SRDIRONEENTRYSTRUCT *psdE)
	{
		ubf_assert_non_NULL (psdE);
		SCUNILOGTARGET			*put = psdE->pCustom;
		ubf_assert_non_NULL (put);

		char	szU8FileNameOnly [UTF8_MAX_PATH];

		CUNILOG_FLS	fls;
		// Note that the return value of UTF8_from_WinU16 () already includes a NUL terminator.
		fls.stFilename = UTF8_from_WinU16 (szU8FileNameOnly, UTF8_MAX_PATH, psdE->pwfd->cFileName);
		fls.chFilename = GetAlignedMemFromSBULKMEMgrow (&put->sbm, fls.stFilename);
		ubf_assert_non_NULL (fls.chFilename);
		if (fls.chFilename)
		{
			memcpy (fls.chFilename, szU8FileNameOnly, fls.stFilename);
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
		SCUNILOGTARGET			*put = pod->pCustom;
		ubf_assert_non_NULL (put);

		CUNILOG_FLS fls;
		fls.stFilename = strlen (pod->dirEnt->d_name) + 1;
		if	(
				globMatch	(
					pod->dirEnt->d_name, fls.stFilename - 1,
					put->szLogFileMask, put->lnsLogFileMask
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
	static void obtainLogfilesListToRotateWin (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		//logNoRotationTextU8fmt (put, "Test for the internal log output (Windows)");

		uint64_t n;
		n = ForEachDirectoryEntryU8	(
				put->mbLogFileMask.buf.pcc,					// Search mask.
				obtainLogfilesListToRotateCallbackWin, put, NULL
									);
		UNREFERENCED_PARAMETER (n);
	}
#elif defined (PLATFORM_IS_POSIX)
	static void obtainLogfilesListToRotatePsx (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		//logNoRotationTextU8fmt (put, "Test for the internal log output (POSIX)");

		uint64_t n;
		n = ForEachPsxDirEntry	(
				put->mbLogFold.buf.pcc,						// Directory name.
				obtainLogfilesListToRotateCallbackPsx, put, NULL
								);
		UNREFERENCED_PARAMETER (n);
	}
#endif

static inline void obtainLogfilesListToRotate (SCUNILOGTARGET *put)
{
	ubf_assert_non_NULL (put);

	#if defined (PLATFORM_IS_WINDOWS)
		obtainLogfilesListToRotateWin (put);
	#elif defined (PLATFORM_IS_POSIX)
		obtainLogfilesListToRotatePsx (put);
	#endif
}

static void cunilogProcessRotateLogfilesFnct (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (cup);
	ubf_assert_non_NULL (pev);
	
	if (cunilogHasEventNoRotation (pev))
		return;

	SCUNILOGTARGET			*put = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (put);
	CUNILOG_ROTATION_DATA	*prd = cup->pData;
	ubf_assert_non_NULL (prd);

	// Reset.
	vec_clear (&put->fls);
	EmptySBULKMEM (&put->sbm);
	prd->nCnt = 0;

	CUNILOG_ROTATOR_ARGS	args;
	args.cup = cup;
	args.pev = pev;

	switch (prd->tsk)
	{
		case cunilogrotationtask_None:
			break;
		case cunilogrotationtask_FScompressLogfiles:
		case cunilogrotationtask_MoveToRecycleBinLogfiles:
		case cunilogrotationtask_DeleteLogfiles:
			obtainLogfilesListToRotate (put);
			prapareLogfilesListAndRotate (&args);
			break;
	}
}

/*
	The custom processor. Its pData member is a pointer to a CUNILOG_CUSTPROCESS structure,
	which has two members: pData for custom data and the function pointer procFnc, which
	is the actual custom processor function.
*/
static void cunilogProcessCustomProcessorFnct (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	
	#ifdef DEBUG
		SCUNILOGTARGET	*put = pev->pSCUNILOGTARGET;
		ubf_assert_non_NULL (put);
	#endif

	CUNILOG_CUSTPROCESS	*pcu = cup->pData;
	ubf_assert_non_NULL (pcu);
	ubf_assert_non_NULL (pcu->procFnc);
	pcu->procFnc (cup, pev);
}

/*
static void cunilogProcessNotSupported (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);
	
	SCUNILOGTARGET	*pta = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (pta);

	ubf_assert_msg (false, "Not supported yet!");
}
*/

/*
	Queue functions.
*/

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline size_t nToTrigger (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (cunilogIsPaused (put))
		{
			++ put->nPausedEvents;
			return 0;
		} else
		{	// Only the current event requires a trigger. The value of nPausedEvents
			//	should have been zeroed out by ResumeLogSCUNILOGTARGET ().
			ubf_assert_0 (put->nPausedEvents);
			return 1;
		}
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	// Returns how many times the semaphore must be triggered to empty the queue.
	static inline size_t EnqueueSCUNILOGEVENT (SCUNILOGEVENT *pev)
	{
		ubf_assert_non_NULL (pev);

		SCUNILOGTARGET	*put = pev->pSCUNILOGTARGET;
		ubf_assert_non_NULL (put);
		ubf_assert (hasSCUNILOGTARGETqueue (put));

		size_t r;

		EnterCUNILOG_LOCKER (put);
		if (put->qu.first)
		{
			SCUNILOGEVENT *l	= put->qu.last;
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
	queue with DequeueAllSCUNILOGEVENTs ().
*/
#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static inline SCUNILOGEVENT *DequeueSCUNILOGEVENT (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (hasSCUNILOGTARGETqueue (put));

		SCUNILOGEVENT	*pev	= NULL;

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
	static inline SCUNILOGEVENT *DequeueAllSCUNILOGEVENTs (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (hasSCUNILOGTARGETqueue (put));

		SCUNILOGEVENT	*pev	= NULL;
		SCUNILOGEVENT	*last;

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
	static inline void triggerSCUNILOGEVENTloggingThread (SCUNILOGTARGET *put, size_t releaseCount)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (hasSCUNILOGTARGETqueue (put));
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
	static bool SepLogThreadWaitForEvents (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (hasSCUNILOGTARGETqueue (put));

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
	SEPARATE_LOGGING_THREAD_RETURN_TYPE SeparateLoggingThread (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (hasSCUNILOGTARGETqueue (put));

		while (SepLogThreadWaitForEvents (put))
		{
			SCUNILOGEVENT *pev = DequeueAllSCUNILOGEVENTs (put);
			SCUNILOGEVENT *pnx;
			while (pev)
			{	// Remember the next pointer because pev is going to be destroyed
				//	by DoneSUNILOGEVENT ().
				pnx = pev->next;
				cunilogProcessEventSingleThreaded (pev);
				DoneSCUNILOGEVENT (pev);
				pev = pnx;
			}
			if (cunilogIsShutdownTarget (put) && 0 == put->nPendingNoRotEvts )
				goto ExitSeparateLoggingThread;
		}
	ExitSeparateLoggingThread:
		cunilogSetTargetHasShutdown (put);
		return SEPARATE_LOGGING_THREAD_RETURN_SUCCESS;
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static bool StartSeparateLoggingThread_ifNeeded (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (requiresSCUNILOGTARGETseparateLoggingThread (put))
		{
			#ifdef OS_IS_WINDOWS
				HANDLE h = CreateThread (NULL, 0, SeparateLoggingThread, put, 0, NULL);
				ubf_assert_non_NULL (h);
				put->th.hThread = h;
				return NULL != h;
			#else
				put->th.tThread = 0;
				int i = pthread_create (&put->th.tThread, NULL, (void * (*)(void *)) SeparateLoggingThread, put);
				ubf_assert_0 (i);
				return 0 == i;
			#endif
		}
		// If the separate logging thread is not required, this is considered success.
		return true;
	}
#else
	static bool StartSeparateLoggingThread_ifNeeded (SCUNILOGTARGET *put)
	{
		return true;
	}
#endif

static void cunilogUpdateCurrentValue (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (cup);
	ubf_assert_non_NULL (pev);

	switch (cup->freq)
	{
		case cunilogProcessAppliesTo_nEvents:
			++ cup->cur;
			break;
		case cunilogProcessAppliesTo_nOctets:
			cup->cur += pev->lenDataToLog;
			break;
		case cunilogProcessAppliesTo_nAlways:
			break;
		case cunilogProcessAppliesTo_SecondChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS;
			break;
		case cunilogProcessAppliesTo_MinuteChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS;
			break;
		case cunilogProcessAppliesTo_HourChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS;
			break;
		case cunilogProcessAppliesTo_DayChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS;
			break;
		case cunilogProcessAppliesTo_WeekChanged:
			cup->cur = (pev->stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS)
					 + 	GetISO8601WeekNumberFromDate (
							UBF_TIMESTAMP_YEAR	(pev->stamp),
							UBF_TIMESTAMP_MONTH	(pev->stamp),
							UBF_TIMESTAMP_DAY	(pev->stamp),
							NULL
													);
			break;
		case cunilogProcessAppliesTo_MonthChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS;
			break;
		case cunilogProcessAppliesTo_YearChanged:
			cup->cur = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS;
			break;
		case cunilogProcessAppliesTo_Auto:
			ubf_assert_msg (false, "Illegal value");
			break;
	}
}

static bool thresholdReached (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (cup);
	ubf_assert_non_NULL (pev);
	ubf_assert (sizeof (uint64_t) == sizeof (UBF_TIMESTAMP));

	bool bRet = false;

	switch (cup->freq)
	{
		case cunilogProcessAppliesTo_nEvents:
		case cunilogProcessAppliesTo_nOctets:
			bRet = cup->cur >= cup->thr;
			cup->cur = bRet ? 0 : cup->cur;
			break;
		case cunilogProcessAppliesTo_nAlways:
			return true;
		case cunilogProcessAppliesTo_SecondChanged:
			bRet = 0 < cup->thr && cup->cur != cup->thr;
			cup->thr = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_SECOND_BITS;
			break;
		case cunilogProcessAppliesTo_MinuteChanged:
			bRet = 0 < cup->thr && cup->cur != cup->thr;
			cup->thr = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_MINUTE_BITS;
			break;
		case cunilogProcessAppliesTo_HourChanged:
			bRet = 0 < cup->thr && cup->cur != cup->thr;
			cup->thr = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_HOUR_BITS;
			break;
		case cunilogProcessAppliesTo_DayChanged:
			bRet = 0 < cup->thr && cup->cur != cup->thr;
			cup->thr = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_DAY_BITS;
			break;
		case cunilogProcessAppliesTo_WeekChanged:
			bRet = 0 < cup->thr && cup->cur != cup->thr;
			// We keep the year and the week number.
			cup->thr = (pev->stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS)
					 +
						GetISO8601WeekNumberFromDate (
							UBF_TIMESTAMP_YEAR	(pev->stamp),
							UBF_TIMESTAMP_MONTH	(pev->stamp),
							UBF_TIMESTAMP_DAY	(pev->stamp),
							NULL
													);
			break;
		case cunilogProcessAppliesTo_MonthChanged:
			bRet = 0 < cup->thr && cup->cur != cup->thr;
			cup->thr = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_MONTH_BITS;
			break;
		case cunilogProcessAppliesTo_YearChanged:
			bRet = 0 < cup->thr && cup->cur != cup->thr;
			cup->thr = pev->stamp & UBF_TIMESTAMP_KEEP_FROM_YEAR_BITS;
			break;
		case cunilogProcessAppliesTo_Auto:
			ubf_assert_msg (false, "Illegal value");
			bRet = true;
			break;
	}

	// The flag OPT_CUNPROC_AT_STARTUP tells us to run the processor in any case.
	if (optCunProcHasOPT_CUNPROC_AT_STARTUP (cup->uiOpts))
	{
		optCunProcClrOPT_CUNPROC_AT_STARTUP (cup->uiOpts);
		bRet = true;
	}

	return bRet;
}

static void (*pickAndRunProcessor [cunilogProcessAmountEnumValues]) (CUNILOG_PROCESSOR *cup, SCUNILOGEVENT *pev) =
{
	/* cunilogProcessNoOperation		*/		cunilogProcessNoneFnct
	/* cunilogProcessEchoToConsole		*/	,	cunilogProcessEchoFnct
	/* cunilogProcessUpdateLogFileName	*/	,	unilogProcessUpdateLogPathFnct
	/* cunilogProcessWriteToLogFile		*/	,	cunilogProcessWriteToLogFileFnct
	/* cunilogProcessFlush				*/	,	cunilogProcessFlushFnct
	/* cunilogProcessRotateLogfiles		*/	,	cunilogProcessRotateLogfilesFnct
	/* cunilogProcessCustomProcessor	*/	,	cunilogProcessCustomProcessorFnct
};

/*
	Returns true for further processing by the caller.
*/
static bool cunilogProcessProcessor (SCUNILOGEVENT *pev, CUNILOG_PROCESSOR *cup)
{
	ubf_assert_non_NULL			(pev);
	ubf_assert_non_NULL			(cup);
	ubf_assert_non_NULL			(pev->pSCUNILOGTARGET);
	cunilogIsTargetInitialised	(pev->pSCUNILOGTARGET);

	cunilogUpdateCurrentValue (cup, pev);
	if	(
				!optCunProcHasOPT_CUNPROC_DISABLED (cup->uiOpts)
			&&	(
						thresholdReached (cup, pev)
					||	cunilogHasRunAllProcessorsOnStartup (pev->pSCUNILOGTARGET)
				)
		)
	{
		pickAndRunProcessor [cup->task] (cup, pev);
		// Tells the caller to carry on with the next processor.
		return true;
	}

	// Tells the caller not to bother with the next processor unless bForceNext is true
	//	or the caller explicitely wants us to run all processors on startup.
	return OPT_CUNPROC_FORCE_NEXT & cup->uiOpts || cunilogHasRunAllProcessorsOnStartup (pev->pSCUNILOGTARGET);
}

static void cunilogProcessProcessors (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pSCUNILOGTARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pSCUNILOGTARGET));
	ubf_assert_non_NULL						(pev->pSCUNILOGTARGET->cprocessors);

	pev->pSCUNILOGTARGET->scuNPI.nIgnoredTotal = 0;
	unsigned int ui = 0;
	while (ui < pev->pSCUNILOGTARGET->nprocessors)
	{
		bool b = cunilogProcessProcessor (pev, pev->pSCUNILOGTARGET->cprocessors [ui]);
		if (!b)
			break;
		++ ui;
	}

	if (cunilogHasRunAllProcessorsOnStartup (pev->pSCUNILOGTARGET))
		cunilogClrRunAllProcessorsOnStartup (pev->pSCUNILOGTARGET);
}

static bool cunilogProcessEventSingleThreaded (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pSCUNILOGTARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pSCUNILOGTARGET));

	// Event command flag for shutting down the separate logging thread.
	if (cunilogIsEventShutdown (pev))
	{
		cunilogSetShutdownTarget (pev->pSCUNILOGTARGET);
		DoneSCUNILOGEVENT (pev);
		return false;
	}

	size_t	eventLineSize = createEventLineFromSUNILOGEVENT (pev);
	if (CUNILOG_SIZE_ERROR != eventLineSize)
	{
		// For now we simply process the event's processors.
		cunilogProcessProcessors (pev);
		if (cunilogHasEventNoRotation (pev))
			DecrementPendingNoRotationEvents (pev->pSCUNILOGTARGET);
		return true;
	}
	return false;
}

static void enqueueAndTriggerSeparateLoggingThread (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pSCUNILOGTARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pSCUNILOGTARGET));

	#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
		size_t n = EnqueueSCUNILOGEVENT (pev);
		if (n)
			triggerSCUNILOGEVENTloggingThread (pev->pSCUNILOGTARGET, n);
	#else
		cunilogProcessEventSingleThreaded (pev);
	#endif
}

static bool cunilogProcessEventSingleThreadedSeparateLoggingThread (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pSCUNILOGTARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pSCUNILOGTARGET));

	enqueueAndTriggerSeparateLoggingThread (pev);
	return true;
}

static bool cunilogProcessEventMultiThreaded (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pSCUNILOGTARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pSCUNILOGTARGET));

	EnterCUNILOG_LOCKER (pev->pSCUNILOGTARGET);
	bool b = cunilogProcessEventSingleThreaded (pev);
	LeaveCUNILOG_LOCKER (pev->pSCUNILOGTARGET);
	return b;
}

static bool cunilogProcessEventMultiThreadedSeparateLoggingThread (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL						(pev);
	ubf_assert_non_NULL						(pev->pSCUNILOGTARGET);
	ubf_assert (cunilogIsTargetInitialised	(pev->pSCUNILOGTARGET));

	enqueueAndTriggerSeparateLoggingThread (pev);
	return true;
}

static bool cunilogProcessOrQueueEventMultiProcesses (SCUNILOGEVENT *pev)
{
	UNREFERENCED_PARAMETER (pev);
	ubf_assert_msg (false, "Not implemented yet.");
	return false;
}

static bool (*cunilogProcOrQueueEvt [cunilogTypeAmountEnumValues]) (SCUNILOGEVENT *pev) =
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
static bool cunilogProcessOrQueueEvent (SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (pev);

	SCUNILOGTARGET *put = pev->pSCUNILOGTARGET;
	ubf_assert_non_NULL (put);
	ubf_assert (cunilogIsTargetInitialised (put));

	// Sanity check for the type.
	ubf_assert (put->culogType >= cunilogSingleThreaded);
	ubf_assert (put->culogType <= cunilogMultiProcesses);
	ubf_assert (put->culogType < cunilogTypeAmountEnumValues);

	#ifdef CUNILOG_BUILD_SINGLE_THREADED_ONLY
		// This is the only one possible in a single-threaded environment.
		ubf_assert (unilogSingleThreaded == pta->unilogType);
	#endif

	return cunilogProcOrQueueEvt [put->culogType] (pev);
}

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	static void WaitForEndOfSeparateLoggingThread (SCUNILOGTARGET *put)
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
	SCUNILOGEVENT scueShutdown;
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	void ShutdownSCUNILOGTARGET (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (hasSCUNILOGTARGETqueue (put))
		{
			FillSCUNILOGEVENT	(
				&scueShutdown, put, CUNILOGEVENT_SHUTDOWN, 0,
				cunilogEvtSeverityNone, cunilogEvtTypeNormalText, NULL, 0
								);
			cunilogProcessOrQueueEvent (&scueShutdown);
			WaitForEndOfSeparateLoggingThread (put);
		}
	}
#else
	void ShutdownSCUNILOGTARGET (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		cunilogSetTargetShutdown (put);
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	void CancelSCUNILOGTARGET (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		if (hasSCUNILOGTARGETqueue (put))
		{
			cunilogSetShutdownTarget (put);

			// Empty the queue. While this would actually not be required here, it can
			//	speed up things significantly (well, maybe a few cycles) with busy queues as
			//	it takes some burden off the separate logging thread.
			SCUNILOGEVENT *pev = DequeueAllSCUNILOGEVENTs (put);
			SCUNILOGEVENT *nxt;
			while (pev)
			{
				nxt = pev->next;
				DoneSCUNILOGEVENT (pev);
				pev = nxt;
			}

			// Queue the shutdown command for the separate logging thread and wait
			//	for it to end.
			FillSCUNILOGEVENT	(
				&scueShutdown, put, CUNILOGEVENT_SHUTDOWN, 0,
				cunilogEvtSeverityNone, cunilogEvtTypeNormalText, NULL, 0
								);
			cunilogProcessOrQueueEvent (&scueShutdown);
			WaitForEndOfSeparateLoggingThread (put);
		}
	}
#else
	void CancelSCUNILOGTARGET (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		cunilogSetTargetShutdown (put);
	}
#endif

/*
	Copied from cunilog.h.

enum enCunilogLogPriority
{
	cunilogPrioNormal,
	cunilogPrioBelowNormal,
	cunilogPrioLow,
	cunilogPrioIdle,
	// Do not insert enum values below this line.
	cunilogPrioInvalid
};
*/

#ifdef OS_IS_WINDOWS
	int icuWinPrioTable [cunilogPrioInvalid] =
	{
			/* cunilogPrioNormal		*/	THREAD_PRIORITY_NORMAL
		,	/* cunilogPrioBelowNormal	*/	THREAD_PRIORITY_BELOW_NORMAL
		,	/* cunilogPrioLow			*/	THREAD_PRIORITY_LOWEST
		,	/* cunilogPrioIdle			*/	THREAD_PRIORITY_IDLE
	};
#else
	// These values haven't been tested yet! I (Thomas) just made them up in
	//	the hope they might do well enough.
	int icuPsxPrioTable [cunilogPrioInvalid] =
	{
			/* cunilogPrioNormal		*/	0
		,	/* cunilogPrioBelowNormal	*/	5
		,	/* cunilogPrioLow			*/	10
		,	/* cunilogPrioIdle			*/	19
	};
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	#ifdef OS_IS_WINDOWS
		static bool SetWinLogPriority (SCUNILOGTARGET *put, int prio)
		{
			ubf_assert_non_NULL (put);
			ubf_assert_non_NULL (put->th.hThread);
			ubf_assert (INVALID_HANDLE_VALUE != put->th.hThread);

			if (put->th.hThread && INVALID_HANDLE_VALUE != put->th.hThread)
			{
				return SetThreadPriority (put->th.hThread, prio);
			}
			return false;
		}
	#else
		static bool SetPsxLogPriority (SCUNILOGTARGET *put, int prio)
		{	// See https://man7.org/linux/man-pages/man3/pthread_setschedprio.3.html .
			return 0 == pthread_setschedprio (put->th.tThread, prio);
		}
	#endif
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	bool SetLogPrioritySCUNILOGTARGET (SCUNILOGTARGET *put, cunilogprio prio)
	{
		ubf_assert_non_NULL (put);
		ubf_assert (prio < cunilogPrioInvalid);

		if (hasSeparateLoggingThread (put))
		{
			#ifdef PLATFORM_IS_WINDOWS
				return SetWinLogPriority (put, icuWinPrioTable [prio]);
			#else
				return SetPsxLogPriority (put, icuPsxPrioTable [prio]);
			#endif
		}
		return true;
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	void PauseLogSCUNILOGTARGET (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		EnterCUNILOG_LOCKER (put);
		cunilogSetPaused (put);
		LeaveCUNILOG_LOCKER (put);
	}
#endif

#ifndef CUNILOG_BUILD_SINGLE_THREADED_ONLY
	size_t ResumeLogSCUNILOGTARGET (SCUNILOGTARGET *put)
	{
		ubf_assert_non_NULL (put);

		size_t n;

		EnterCUNILOG_LOCKER (put);
		cunilogClrPaused (put);
		n = put->nPausedEvents;
		put->nPausedEvents = 0;
		LeaveCUNILOG_LOCKER (put);

		triggerSCUNILOGEVENTloggingThread (put, n);
		return n;
	}
#endif

/*
	User logging functions.
*/

bool logEv (SCUNILOGTARGET *put, SCUNILOGEVENT *pev)
{
	ubf_assert_non_NULL (put);
	ubf_assert_non_NULL (pev);
	ubf_assert (cunilogIsTargetInitialised (put));

	if (cunilogIsShutdownTarget (put))
		return false;

	pev->pSCUNILOGTARGET = put;
	return cunilogProcessOrQueueEvent (pev);
}

bool logTextU8sevl			(SCUNILOGTARGET *put, cueventseverity sev, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	SCUNILOGEVENT *pl = CreateSCUNILOGEVENT_Text (put, sev, ccText, len);
	return cunilogProcessOrQueueEvent (pl);
}

bool logTextWsevl			(SCUNILOGTARGET *put, cueventseverity sev, const wchar_t *cwText, size_t len)
{
	UNREFERENCED_PARAMETER (sev);
	UNREFERENCED_PARAMETER (cwText);
	UNREFERENCED_PARAMETER (len);

	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	ubf_assert_msg (false, "Not implemented yet.");
	return false;
}

bool logTextU8sev			(SCUNILOGTARGET *put, cueventseverity sev, const char *ccText)
{
	return logTextU8sevl (put, sev, ccText, USE_STRLEN);
}

bool logTextWsev			(SCUNILOGTARGET *put, cueventseverity sev, const wchar_t *cwText)
{
	UNREFERENCED_PARAMETER (sev);
	UNREFERENCED_PARAMETER (cwText);

	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	ubf_assert_msg (false, "Not implemented yet.");
	return false;
}

bool logTextU8l				(SCUNILOGTARGET *put, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	SCUNILOGEVENT *pev = CreateSCUNILOGEVENT_Text (put, cunilogEvtSeverityNone, ccText, len);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logTextU8ql			(SCUNILOGTARGET *put, const char *ccText, size_t len)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	SCUNILOGEVENT *pev = CreateSCUNILOGEVENT_Text (put, cunilogEvtSeverityNone, ccText, len);
	if (pev)
	{
		cunilogSetEventNoRotation (pev);
		return cunilogProcessOrQueueEvent (pev);
	}
	return false;
}

bool logTextWl				(SCUNILOGTARGET *put, const wchar_t *cwText, size_t len)
{
	UNREFERENCED_PARAMETER (cwText);
	UNREFERENCED_PARAMETER (len);

	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	ubf_assert_msg (false, "Not implemented yet.");
	return false;
}

bool logTextU8				(SCUNILOGTARGET *put, const char *ccText)
{
	return logTextU8l (put, ccText, USE_STRLEN);
}

bool logTextU8q				(SCUNILOGTARGET *put, const char *ccText)
{
	return logTextU8ql (put, ccText, USE_STRLEN);
}

bool logTextW				(SCUNILOGTARGET *put, const wchar_t *cwText)
{
	UNREFERENCED_PARAMETER (cwText);

	ubf_assert_non_NULL (put);
	
	if (cunilogIsShutdownTarget (put))
		return false;

	ubf_assert_msg (false, "Not implemented yet.");
	return false;
}

bool logTextU8fmt			(SCUNILOGTARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	va_list		ap;
	size_t		l;

	va_start (ap, fmt);
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);
	va_end (ap);

	char *ob = ubf_malloc (l + 1);
	if (ob)
	{
		va_start (ap, fmt);
		vsnprintf (ob, l + 1, fmt, ap);
		va_end (ap);

		bool b = logTextU8l (put, ob, l);
		ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8qfmt			(SCUNILOGTARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	va_list		ap;
	size_t		l;

	va_start (ap, fmt);
	l = (size_t) vsnprintf (NULL, 0, fmt, ap);
	va_end (ap);

	char *ob = ubf_malloc (l + 1);
	if (ob)
	{
		va_start (ap, fmt);
		vsnprintf (ob, l + 1, fmt, ap);
		va_end (ap);

		bool b = logTextU8ql (put, ob, l);
		ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8sfmt			(SCUNILOGTARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
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

		bool b = logTextU8l (put, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8sqfmt			(SCUNILOGTARGET *put, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
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

		bool b = logTextU8ql (put, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8sfmtsev		(SCUNILOGTARGET *put, cueventseverity sev, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
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

		bool b = logTextU8sevl (put, sev, ob, l);
		if (ob != cb) ubf_free (ob);
		return b;
	}
	return false;
}

bool logTextU8smbfmtsev		(SCUNILOGTARGET *put, SMEMBUF *smb, cueventseverity sev, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);
	ubf_assert (isInitialisedSMEMBUF (smb));

	if (cunilogIsShutdownTarget (put))
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

			bool b = logTextU8sevl (put, sev, smb->buf.pch, l);
			return b;
		}
	}
	return false;
}

bool logTextU8smbfmt		(SCUNILOGTARGET *put, SMEMBUF *smb, const char *fmt, ...)
{
	ubf_assert_non_NULL (put);
	ubf_assert (isInitialisedSMEMBUF (smb));

	if (cunilogIsShutdownTarget (put))
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

			bool b = logTextU8l (put, smb->buf.pch, l);
			return b;
		}
	}
	return false;
}

bool logHexDumpU8sevl		(SCUNILOGTARGET *put, cueventseverity sev, const void *pBlob, size_t size, const char *ccCaption, size_t lenCaption)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	SCUNILOGEVENT *pev = CreateSCUNILOGEVENT_Data (put, sev, pBlob, size, ccCaption, lenCaption);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logHexDumpU8l			(SCUNILOGTARGET *put, const void *pBlob, size_t size, const char *ccCaption, size_t lenCaption)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	SCUNILOGEVENT *pev = CreateSCUNILOGEVENT_Data	(
							put, cunilogEvtSeverityNone, pBlob, size,
							ccCaption, lenCaption
													);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logHexDump				(SCUNILOGTARGET *put, const void *pBlob, size_t size)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	SCUNILOGEVENT *pev = CreateSCUNILOGEVENT_Data	(
							put, cunilogEvtSeverityNone, pBlob,
							size, NULL, 0
													);
	return pev && cunilogProcessOrQueueEvent (pev);
}

bool logHexOrText			(SCUNILOGTARGET *put, const void *szHexOrTxt, size_t lenHexOrTxt)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	if (str_has_only_printable_ASCII (szHexOrTxt, lenHexOrTxt))
		return logTextU8l (put, szHexOrTxt, lenHexOrTxt);

	return logHexDump (put, szHexOrTxt, lenHexOrTxt);
}

bool logHexOrTextU8			(SCUNILOGTARGET *put, const void *szHexOrTxtU8, size_t lenHexOrTxtU8)
{
	ubf_assert_non_NULL (put);

	if (cunilogIsShutdownTarget (put))
		return false;

	if (check_utf8 (szHexOrTxtU8, lenHexOrTxtU8))
		return logTextU8l (put, szHexOrTxtU8, lenHexOrTxtU8);

	return logHexDump (put, szHexOrTxtU8, lenHexOrTxtU8);
}

/* Do we need this?
const char		ccCunilogVersionText [] = CUNILOG_VERSION_STRING;
const char		ccCunilogVersionYear [] = CUNILOG_VERSION_YEAR;
*/
/*
const uint64_t	uiCunilogVersion		=		((uint64_t) CUNILOG_VERSION_MAJOR	<< 48)
											|	((uint64_t) CUNILOG_VERSION_MINOR	<< 32)
											|	((uint64_t) CUNILOG_VERSION_SUB		<< 16)
											|	((uint64_t) CUNILOG_VERSION_BUILD);
*/
const uint64_t	uiCunilogVersion		=		((uint64_t) CUNILOG_VERSION_BUILD);
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
		SCUNILOGTARGET	*pt;
		bool			bRet	= true;

		//ASSERT (false);

		// Ensure we haven't forgotten to fill the arrays/jump tables.
		ubf_assert (GET_ARRAY_LEN (arrLengthTimeStampFromPostfix)	==	cunilogPostfixAmountEnumValues);
		ubf_assert (GET_ARRAY_LEN (arrPostfixWildcardMask)			==	cunilogPostfixAmountEnumValues);
		ubf_assert (GET_ARRAY_LEN (obtainTimeStampAsString)			==	cunilogPostfixAmountEnumValues);
		ubf_assert (GET_ARRAY_LEN (cunilogProcOrQueueEvt)			==	cunilogTypeAmountEnumValues);

		// Check that the length assignments are correct.
		ubf_assert (LEN_ISO8601DATEHOURANDMINUTE	== lenDateTimeStampFromPostfix (cunilogPostfixMinute));
		ubf_assert (LEN_ISO8601DATEANDHOUR			== lenDateTimeStampFromPostfix (cunilogPostfixHour));
		ubf_assert (LEN_ISO8601DATE					== lenDateTimeStampFromPostfix (cunilogPostfixDay));
		ubf_assert (LEN_ISO8601DATE					== lenDateTimeStampFromPostfix (cunilogPostfixDefault));
		ubf_assert (LEN_ISO8601YEARANDWEEK			== lenDateTimeStampFromPostfix (cunilogPostfixWeek));
		ubf_assert (LEN_ISO8601YEARANDMONTH			== lenDateTimeStampFromPostfix (cunilogPostfixMonth));
		ubf_assert (LEN_ISO8601YEAR					== lenDateTimeStampFromPostfix (cunilogPostfixYear));

		#ifdef OS_IS_LINUX
			bool bTrash = MoveFileToTrashPOSIX ("/home/thomas/FS/OAN/Thomas/cunilog/logs/testcunilog_2024-11-05 20_14.log");
			ubf_assert_false (bTrash);
		#endif

		/*
			Static.
		*/
		pt = InitSCUNILOGTARGETstaticEx	(
					NULL,		0,
					"Unilog",	USE_STRLEN,
					cunilogLogPath_relativeToHomeDir,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0,
					cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup
										);
		ubf_assert (pSCUNILOGTARGETstatic	== pt);
		ubf_assert (pSCUNILOGTARGETstatic	== &SCUNILOGTARGETstatic);
		ubf_assert (cunilogSingleThreaded	== SCUNILOGTARGETstatic.culogType);
		ubf_assert (cunilogPostfixDay		== SCUNILOGTARGETstatic.culogPostfix);
		ubf_assert (cunilogEvtTS_Default	== SCUNILOGTARGETstatic.unilogEvtTSformat);
		ubf_assert (6 == SCUNILOGTARGETstatic.lnAppName);
		ubf_assert (!memcmp (SCUNILOGTARGETstatic.mbAppName.buf.pch, "Unilog", SCUNILOGTARGETstatic.lnAppName));
		DoneSCUNILOGTARGETstatic ();

		pt = InitSCUNILOGTARGETstaticEx	(
					NULL,				0,
					"////sub/Unilog",	USE_STRLEN,
					cunilogLogPath_relativeToHomeDir,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0,
					cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup
										);
		ubf_assert (pSCUNILOGTARGETstatic	== pt);
		ubf_assert (pSCUNILOGTARGETstatic	== &SCUNILOGTARGETstatic);
		ubf_assert (cunilogSingleThreaded	== SCUNILOGTARGETstatic.culogType);
		ubf_assert (cunilogPostfixDay		== SCUNILOGTARGETstatic.culogPostfix);
		ubf_assert (cunilogEvtTS_Default	== SCUNILOGTARGETstatic.unilogEvtTSformat);
		ubf_assert (10 == SCUNILOGTARGETstatic.lnAppName);
		#ifdef PLATFORM_IS_WINDOWS
			ubf_assert (!memcmp (SCUNILOGTARGETstatic.mbAppName.buf.pch, "sub\\Unilog", SCUNILOGTARGETstatic.lnAppName));
		#else
			ubf_assert (!memcmp (SCUNILOGTARGETstatic.mbAppName.buf.pch, "sub/Unilog", SCUNILOGTARGETstatic.lnAppName));
		#endif
		DoneSCUNILOGTARGETstatic ();

		pt = InitSCUNILOGTARGETstaticEx	(
					NULL,		0,
					"Unilog",	USE_STRLEN,
					cunilogLogPath_relativeToCurrentDir,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0,
					cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup
										);
		ubf_assert (pSCUNILOGTARGETstatic	== pt);
		ubf_assert (pSCUNILOGTARGETstatic	== &SCUNILOGTARGETstatic);
		ubf_assert (cunilogSingleThreaded	== SCUNILOGTARGETstatic.culogType);
		ubf_assert (cunilogPostfixDay		== SCUNILOGTARGETstatic.culogPostfix);
		ubf_assert (cunilogEvtTS_Default	== SCUNILOGTARGETstatic.unilogEvtTSformat);
		ubf_assert (6 == SCUNILOGTARGETstatic.lnAppName);
		ubf_assert (!memcmp (SCUNILOGTARGETstatic.mbAppName.buf.pch, "Unilog", SCUNILOGTARGETstatic.lnAppName));
		DoneSCUNILOGTARGETstatic ();

		pt = InitSCUNILOGTARGETstaticEx	(
					"C:/temp",	USE_STRLEN,
					"Unilog",	USE_STRLEN,
					cunilogLogPath_relativeToExecutable,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0,
					cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup
										);
		ubf_assert (pSCUNILOGTARGETstatic	== pt);
		ubf_assert (pSCUNILOGTARGETstatic	== &SCUNILOGTARGETstatic);
		ubf_assert (cunilogSingleThreaded	== SCUNILOGTARGETstatic.culogType);
		ubf_assert (cunilogPostfixDay		== SCUNILOGTARGETstatic.culogPostfix);
		ubf_assert (cunilogEvtTS_Default		== SCUNILOGTARGETstatic.unilogEvtTSformat);
		ubf_assert (8 == SCUNILOGTARGETstatic.mbLogPath.size);
		ubf_assert	(
				!memcmp	(
					SCUNILOGTARGETstatic.mbLogPath.buf.pch,
					"C:" UBF_DIR_SEP_STR "temp" UBF_DIR_SEP_STR,
					SCUNILOGTARGETstatic.mbLogPath.size
						)
					);
		ubf_assert	(
				// "C:\\temp\\Unilog".
				!memcmp	(
					SCUNILOGTARGETstatic.mbLogfileName.buf.pch,
					"C:" UBF_DIR_SEP_STR "temp" UBF_DIR_SEP_STR "Unilog",
					SCUNILOGTARGETstatic.mbLogPath.size + SCUNILOGTARGETstatic.lnAppName
						)
					);
		ubf_assert (6 == SCUNILOGTARGETstatic.lnAppName);
		ubf_assert (!memcmp (SCUNILOGTARGETstatic.mbAppName.buf.pch, "Unilog", SCUNILOGTARGETstatic.lnAppName));
		DoneSCUNILOGTARGETstatic ();

		pt = InitSCUNILOGTARGETstaticEx	(
					"../temp",	USE_STRLEN,
					"Unilog",	USE_STRLEN,
					cunilogLogPath_relativeToExecutable,
					cunilogSingleThreaded,
					cunilogPostfixDay,
					NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
					cunilogDontRunProcessorsOnStartup

										);
		ubf_assert (pSCUNILOGTARGETstatic == pt);
		ubf_assert (pSCUNILOGTARGETstatic	== &SCUNILOGTARGETstatic);
		ubf_assert (cunilogSingleThreaded	== SCUNILOGTARGETstatic.culogType);
		ubf_assert (cunilogPostfixDay				== SCUNILOGTARGETstatic.culogPostfix);
		ubf_assert (6 == SCUNILOGTARGETstatic.lnAppName);
		ubf_assert (!memcmp (SCUNILOGTARGETstatic.mbAppName.buf.pch, "Unilog", SCUNILOGTARGETstatic.lnAppName));
		DoneSCUNILOGTARGETstatic ();

		/*
			Dynamic.
		*/
		pt = CreateNewSCUNILOGTARGET (
				NULL, 0,
				NULL, 0,
				cunilogLogPath_relativeToExecutable,
				cunilogSingleThreaded,
				cunilogPostfixDay,
				NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
				cunilogDontRunProcessorsOnStartup
									);
		DoneSCUNILOGTARGET (pt);

		pt = InitOrCreateSCUNILOGTARGET	(
				NULL,
				NULL, 0,
				"Unilog", 6,
				cunilogLogPath_relativeToExecutable,
				cunilogSingleThreaded,
				cunilogPostfixDay,
				NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
				cunilogDontRunProcessorsOnStartup
										);
		DoneSCUNILOGTARGET (pt);

		pt = InitOrCreateSCUNILOGTARGET	(
				NULL,
				"C:/Temp", 7,
				"Unilog", 6,
				cunilogLogPath_relativeToExecutable,
				cunilogSingleThreaded,
				cunilogPostfixDay,
				NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
				cunilogDontRunProcessorsOnStartup
										);
		ubf_assert_non_NULL (pt);
		ubf_assert (pt != pSCUNILOGTARGETstatic);
		ubf_assert (cunilogSingleThreaded	== pt->culogType);
		ubf_assert (cunilogPostfixDay				== pt->culogPostfix);
		ubf_assert (8 == pt->mbLogPath.size);
		ubf_assert	(
				!memcmp	(
					pt->mbLogPath.buf.pch,
					"C:" UBF_DIR_SEP_STR "Temp" UBF_DIR_SEP_STR,
					pt->mbLogPath.size
						)
					);
		ubf_assert	(
				// "C:\\Temp\\Unilog".
				!memcmp	(
					pt->mbLogfileName.buf.pch,
					"C:" UBF_DIR_SEP_STR "Temp" UBF_DIR_SEP_STR "Unilog",
					pt->mbLogPath.size + pt->mbAppName.size
						)
					);
		ubf_assert (6 == pt->mbAppName.size);
		ubf_assert (!memcmp (pt->mbAppName.buf.pch, "Unilog", pt->mbAppName.size));
		DoneSCUNILOGTARGET (pt);

		// Should fail.
		SCUNILOGTARGET	cut;
		SCUNILOGTARGET	*put;
		put = InitSCUNILOGTARGET	(
				&cut,
				"temp",		USE_STRLEN,
				"OurApp",	USE_STRLEN,
				cunilogLogPath_isAbsolute,
				cunilogMultiThreaded
									);
		ubf_assert_NULL (put);

		/*
			Application name from executable name.
		*/
		pt = InitOrCreateSCUNILOGTARGET	(
				NULL,
				"C:/Temp", 7,
				NULL, 0,
				cunilogLogPath_relativeToExecutable,
				cunilogSingleThreaded,
				cunilogPostfixDay,
				NULL, 0, cunilogEvtTS_Default, cunilogNewLineSystem,
				cunilogDontRunProcessorsOnStartup
										);
		SMEMBUF mb;
		initSMEMBUF (&mb);
		ObtainAppNameFromExecutableModule (&mb);
		ubf_assert (!memcmp (pt->mbAppName.buf.pch, mb.buf.pch, strlen (mb.buf.pch)));
		ubf_assert_non_NULL (pt);
		ubf_assert (pt != pSCUNILOGTARGETstatic);
		ubf_assert (cunilogSingleThreaded	== pt->culogType);
		ubf_assert (cunilogPostfixDay				== pt->culogPostfix);
		ubf_assert (8 == pt->mbLogPath.size);
		ubf_assert	(
				!memcmp	(
					pt->mbLogPath.buf.pch,
					"C:" UBF_DIR_SEP_STR "Temp" UBF_DIR_SEP_STR,
					pt->mbLogPath.size
						)
					);
		DoneSCUNILOGTARGET (pt);

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
