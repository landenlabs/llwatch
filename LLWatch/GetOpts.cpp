// ------------------------------------------------------------------------------------------------
// Standard unix like argument parsing.
//
// Author:  Dennis Lang   Apr-2011
// http://LanDenLabs.com
//
// This file is part of LLWatch project.
//
// ----- License ----
//
// Copyright (c) 2014 Dennis Lang
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
//
// ------------------------------------------------------------------------------------------------

#include "GetOpts.h"
#include <iostream>

// ------------------------------------------------------------------------------------------------
template <typename tchar>
bool GetOpts<tchar>::GetOpt()
{
    m_error = false;

	if (m_argSeq == 0 || !*m_argSeq) 
    { 
        // Update scanning pointer. 
		if (m_optIdx >= m_argc || *(m_argSeq = m_argv[m_optIdx]) != '-') 
        {
			m_argSeq = 0;
			return false;   // Done
		}

		if (m_argSeq[1] && *++m_argSeq == '-') 
        { 
            // Found "--", no more options allowed.
			++m_optIdx;
			m_argSeq = 0;
			return false;
		}
	}  

	const tchar* pOptLetterList;     // option letter list index 
    m_optOpt = *m_argSeq++;
	if (m_optOpt == ':' ||
		!(pOptLetterList = FindChr(m_optStr, m_optOpt))) 
    {
		//
		// For backwards compatibility: don't treat '-' as an
		// option letter unless caller explicitly asked for it.
		//
		if (m_optOpt == '-')
			return false;

		if (!*m_argSeq)
			++m_optIdx;

        m_error = true; // Illegal option.
		return false;   // Error.
	}

	if (*++pOptLetterList != ':') 
    { 
        // Don't need argument 
		m_optArg = NULL;
		if (!*m_argSeq)
			++m_optIdx;
	} 
    else 
    { 
        // Need an argument 
		if (*m_argSeq)   
        {
            //  No white space 
			m_optArg = m_argSeq;
        }
		else if (m_argc <= ++m_optIdx) 
        { 
			m_argSeq = 0;
            m_error = true;     // Missing option value
			return false;       // Error.
		} 
        else 
        {
            // White space
			m_optArg = m_argv[m_optIdx];
        }

		m_argSeq = 0;
		++m_optIdx;
	}

	return true; // Got a valid option.
}

// Force template to build.
template bool GetOpts<char>::GetOpt();
