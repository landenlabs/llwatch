// ------------------------------------------------------------------------------------------------
// Smart file handle class. Close when it goes out of scope.
//
// Project: NTFSfastFind
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

#include <Windows.h>


class Hnd
{
public:
    Hnd(HANDLE handle = INVALID_HANDLE_VALUE) : m_handle(handle)
    { }

    ~Hnd()
    { Close();  }

    Hnd& operator=(HANDLE other)
    {
		if (other != m_handle)
			Close();
        m_handle = other;
        return *this;
    }

#if 0
    HANDLE Duplicate() const
    {
        if (IsValid())
        {
            HANDLE handle;
            if (::DuplicateHandle(
                    ::GetCurrentProcess(), 
                    m_handle, 
                    ::GetCurrentProcess(),
                    &handle, 
                    0,
                    FALSE,
                    DUPLICATE_SAME_ACCESS))
            {
                return handle;
            }
        }

        return INVALID_HANDLE_VALUE;
    }
#endif

    bool IsValid() const
    { return m_handle != INVALID_HANDLE_VALUE; }

	void Close()
	{
		if (IsValid())
		{
			CloseHandle(m_handle);
			m_handle = INVALID_HANDLE_VALUE;
		}
	}

    operator HANDLE& ()
    {  return m_handle; }

    HANDLE m_handle;
};

