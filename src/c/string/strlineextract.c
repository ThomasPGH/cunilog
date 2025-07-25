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
