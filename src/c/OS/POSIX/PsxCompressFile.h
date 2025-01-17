/****************************************************************************************

	File:		PsxCompressFile.h
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

#ifndef U_PSXCOMPRESS_FILE_H
#define U_PSXCOMPRESS_FILE_H

#ifndef CUNILOG_USE_COMBINED_MODULE

	#ifdef UBF_USE_FLAT_FOLDER_STRUCTURE
		#include "./externC.h"
		#include "./platform.h"
	#else
		#include "./../../pre/externC.h"
		#include "./../../pre/platform.h"
	#endif

#endif

EXTERN_C_BEGIN

#ifdef PLATFORM_IS_POSIX


#endif														// Of #ifdef PLATFORM_IS_POSIX.

EXTERN_C_END

#endif														// Of #ifndef U_PSXCOMPRESS_FILE_H.
