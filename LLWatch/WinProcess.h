// ---------------------------------------------------------------------------
// WinProcess.h - Windows Process api helper
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

#include <Windows.h>
#include <string>

#include "Hnd.h"

// ======================================================================================
class WinProcess
{
public:
	WinProcess() { m_extn = NULL;  }

	Hnd m_hChildStd_IN_Rd;
	Hnd m_hChildStd_IN_Wr;
	Hnd m_hChildStd_OUT_Rd;
	Hnd m_hChildStd_OUT_Wr;

	PROCESS_INFORMATION m_piProcInfo;
	STARTUPINFO			m_siStartInfo;

	const char* m_extn;
	std::string m_lastExeName;

	DWORD m_exitCode;

	bool Init(void);
	const char* GetRunExtension(std::string& exeName);
	std::string WinProcess::GetRunCommand(std::string& fullCommand, const std::string& command);
	void CreateChildProcess(const std::string& commandLine, unsigned long waitMsec = 10);
	void WriteToPipe(HANDLE inFile);
	void ReadFromPipe(HANDLE outHnd, std::string* pBuffer = NULL);
	void CloseProcess();
	void ErrorExit(PTSTR);
};

