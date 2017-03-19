// ---------------------------------------------------------------------------
// llstring - Simple string wrapper
//
// Author: Dennis Lang - 2015
// http://LanDenLabs.com
//
// This file is part of LLWatch project.
//
// ----- License ----
//
// Copyright (c) 2015 Dennis Lang
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
// of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// ---------------------------------------------------------------------------


#pragma once

#include <assert.h>
#include "llstring.h"


// ---------------------------------------------------------------------------
char* lstring::PadRight(char* str, size_t maxLen, size_t padLen, char padChr)
{
    if (padLen > maxLen)
        padLen = maxLen;

    size_t wlen = strlen(str);
    if (padLen > wlen)
    {
        memset(str + wlen, padChr, padLen - wlen);
        str[padLen] = '\0';
    }
    return str;
}

// ---------------------------------------------------------------------------
char* lstring::PadLeft(char* str, size_t maxLen, size_t padLen, char padChr)
{
    if (padLen > maxLen)
        padLen = maxLen;

    size_t wlen = strlen(str);
    if (padLen > wlen)
    {
        memmove(str + padLen-wlen, str, wlen+1);
        memset(str, padChr, padLen - wlen);
    }
    return str;
}

//  warning C4706: assignment within conditional expression, line #56
#pragma warning (disable : 4706)

//-----------------------------------------------------------------------------
char* lstring::TrimString(char* str)
{
    char* pDst = str;
    char* pSrc = str;

    while (*pSrc && isspace(*pSrc))
        pSrc++;         // remove leader

    while (*pDst++ = *pSrc++)
    {}

    pDst--;
    while (pDst != str && isspace(pDst[-1]))
        *--pDst = '\0'; // remove trailer

    return str;
}

//-----------------------------------------------------------------------------
lstring& lstring::trim()
{
	const char* str = c_str();
	char* pDst = (char*)c_str();
	char* pSrc = (char*)c_str();

	while (*pSrc && isspace(*pSrc))
		pSrc++;         // remove leader

	while (*pDst++ = *pSrc++)
	{
	}

	pDst--;
	while (pDst != str && isspace(pDst[-1]))
		*--pDst = '\0'; // remove trailer
	resize(pDst - str + 1);
	return *this;
}


//-----------------------------------------------------------------------------
bool lstring::regFind(const std::tr1::regex&  grepLinePat) const
{
	std::string::const_iterator begIter = begin();
	std::string::const_iterator endIter = end();
	std::tr1::smatch match;
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default;

	return std::tr1::regex_search(begIter, endIter, match, grepLinePat, flags);
}

//-----------------------------------------------------------------------------
bool lstring::regReplace(const std::tr1::regex& grepLinePat, const std::string& replaceStr)
{
	bool itemMatches = false;
	std::tr1::smatch match;
	std::regex_constants::match_flag_type flags = std::regex_constants::match_default;

	// Loop to get multiple matches on a line.
	size_t off = 0;
	do
	{
		std::string::const_iterator begIter = this->begin();
		std::string::const_iterator endIter = this->end();
		std::advance(begIter, off);

		if (begIter < endIter &&
			std::tr1::regex_search(begIter, endIter, match, grepLinePat, flags | std::regex_constants::format_first_only))
		{
			std::string subStr = this->substr(off);
			std::string newStr = std::regex_replace(subStr, grepLinePat, replaceStr, flags | std::regex_constants::format_first_only);
			size_t repLen = match.length() + newStr.length() - this->length();
			if (newStr != subStr)
			{
				// str.swap(newStr);
				size_t begPos = off + match.position();
				this->replace(begPos, this->length() - begPos, newStr, match.position(), newStr.length() - match.position());

				itemMatches = true;
				off += match.position() + 1;
			}
			else
				off = 0;
		}
		else
			off = 0;
	} while (off != 0);

	return itemMatches;
}