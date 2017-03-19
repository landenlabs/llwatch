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

#pragma once

template <typename tchar>
class GetOpts
{
public:
    // Pass in argc and argv from main()
    // optStr is optional switches
    //      "bd:eg:h"
    // colon indicates those switch letter which tag an argument
    //   -b  -d foo -e -g bar -h
    GetOpts(int argc,  const tchar* argv[], const tchar* optStr) :
        m_argc(argc),
        m_argv(argv),
        m_optStr(optStr),
        m_optArg(0),     // Argument associated with option 
        m_optIdx(1),        // Index into parent argv vector
        m_optOpt(0),        // Character checked for validity
        m_argSeq(""),
        m_error(false)
    { }
       
    int             m_argc;
    const tchar**   m_argv;
    const tchar*    m_optStr;

    const tchar*    m_optArg;   // Argument associated with option  
    int             m_optIdx;   // Index into parent argv vector 
    tchar           m_optOpt;   // Character option being processed.
	const tchar*    m_argSeq;   // Argv token of characters (sequence).
    bool            m_error;    // True if error detected.

    // Return true if option detected.
    bool GetOpt();

    // Return option character just processed by GetOpt().
    tchar Opt() const
    { return m_optOpt; }

    bool Error() const
    { return m_error; }

    // Return option's argument value.
    const tchar* OptArg() const
    { return m_optArg; }

    // Return next index after last processed by GetOpt();
    int NextIdx() const
    { return m_optIdx; }

    const tchar* FindChr(const tchar* str, tchar chr)
    {
        while (*str && *str != chr)
            str++;
        return (*str == chr) ? str : 0;
    }
};

