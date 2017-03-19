// ---------------------------------------------------------------------------
// WinProcess.cpp - Windows Process api helper
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

#include "WinProcess.h"

#include <Windows.h>
#include <iostream>
#include <string>
#include <stdio.h> 
#include <strsafe.h>

const unsigned int BUFSIZE = 4096;

// ======================================================================================
bool WinProcess::Init(void)
{
	SECURITY_ATTRIBUTES saAttr;

	// Set the bInheritHandle flag so pipe handles are inherited. 
	saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
	saAttr.bInheritHandle = TRUE;
	saAttr.lpSecurityDescriptor = NULL;

	// Create a pipe for the child process's STDOUT. 
	HANDLE rd, wrt;
	if (!CreatePipe(&rd, &wrt, &saAttr, 0))
		ErrorExit("StdoutRd CreatePipe");
	m_hChildStd_OUT_Rd = rd;
	m_hChildStd_OUT_Wr = wrt;

	// Ensure the read handle to the pipe for STDOUT is not inherited.
	if (!SetHandleInformation(m_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
		ErrorExit("Stdout SetHandleInformation");
	DWORD pipeMode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
	if (!SetNamedPipeHandleState(m_hChildStd_OUT_Rd, &pipeMode, NULL, NULL))
		ErrorExit("Stdout SetHandleInformation");

	// Create a pipe for the child process's STDIN. 
	if (!CreatePipe(&rd, &wrt, &saAttr, 0))
		ErrorExit("Stdin CreatePipe");
	m_hChildStd_IN_Rd = rd;
	m_hChildStd_IN_Wr = wrt;

	// Ensure the write handle to the pipe for STDIN is not inherited. 
	if (!SetHandleInformation(m_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
		ErrorExit("Stdin SetHandleInformation");

	return true;
}

// ======================================================================================
// Has statics - not thread safe.
const char* WinProcess::GetRunExtension(std::string& exeName)
{
	// Cache results - return previous match
	if (m_lastExeName == exeName)
		return m_extn;
	m_lastExeName = exeName;

	/*
	static char ext[_MAX_EXT];
	_splitpath(exeName.c_str(), NULL, NULL, NULL, ext);

	if (ext[0] == '.')
	return ext;
	*/

	// Expensive - search PATH for executable.
	char fullPath[MAX_PATH];
	static const char* s_extns[] = { NULL, ".exe", ".com", ".cmd", ".bat", ".ps" };
	for (unsigned idx = 0; idx != ARRAYSIZE(s_extns); idx++)
	{
		m_extn = s_extns[idx];
		DWORD foundPathLen = SearchPath(NULL, exeName.c_str(), m_extn, ARRAYSIZE(fullPath), fullPath, NULL);
		if (foundPathLen != 0)
			return m_extn;
	}

	return NULL;
}

// ======================================================================================
std::string WinProcess::GetRunCommand(std::string& fullCommand, const std::string& command)
{
	fullCommand = command;
	const char* pEndExe = strchr(command.c_str(), ' ');
	if (pEndExe == NULL)
		pEndExe = strchr(command.c_str(), '\0');
	std::string exeName(command.c_str(), pEndExe);

	const char* exeExtn = GetRunExtension(exeName);
	static const char* s_extns[] = { ".cmd", ".bat", ".ps" };
	if (exeExtn != NULL)
	{
		for (unsigned idx = 0; idx != ARRAYSIZE(s_extns); idx++)
		{
			const char* extn = s_extns[idx];
			if (strcmp(exeExtn, extn) == 0)
			{
				// Add .bat or .cmd to executable name.
				fullCommand = exeName + extn + pEndExe;
				break;
			}
		}
	}

	return fullCommand;
}

// ======================================================================================
// Create a child process that uses the previously created pipes for STDIN and STDOUT.
void WinProcess::CreateChildProcess(const std::string& rawCommandLine, unsigned long waitMsec)
{
	Init();

	std::string commandLine;
	GetRunCommand(commandLine, rawCommandLine);

	BOOL bSuccess = FALSE;

	// Set up members of the PROCESS_INFORMATION structure. 
	ZeroMemory(&m_piProcInfo, sizeof(PROCESS_INFORMATION));

	// Set up members of the STARTUPINFO structure. 
	// This structure specifies the STDIN and STDOUT handles for redirection.

	ZeroMemory(&m_siStartInfo, sizeof(STARTUPINFO));
	m_siStartInfo.cb = sizeof(STARTUPINFO);
	m_siStartInfo.hStdError = m_hChildStd_OUT_Wr;
	m_siStartInfo.hStdOutput = m_hChildStd_OUT_Wr;
	m_siStartInfo.hStdInput = m_hChildStd_IN_Rd;
	m_siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	// Create the child process. 
	char* cmdPtr = (char*)commandLine.c_str();
	bSuccess = CreateProcessA(NULL,
		cmdPtr,			// command line 
		NULL,          // process security attributes 
		NULL,          // primary thread security attributes 
		TRUE,          // handles are inherited 
		0,             // creation flags 
		NULL,          // use parent's environment 
		NULL,          // use parent's current directory 
		&m_siStartInfo,  // STARTUPINFO pointer 
		&m_piProcInfo);  // receives PROCESS_INFORMATION 

					   // If an error occurs, exit the application. 
	if (!bSuccess)
	{
		ErrorExit("CreateProcess");
		std::cerr << " Make sure you proceed command with --\n"
			" And that executable is either in path or you specify \n"
			" full path to executable \n";
	}
	else
	{
		m_exitCode = 0;

		// Wait for process to start (and possibly exit)
		DWORD createStatus = WaitForSingleObject(m_piProcInfo.hProcess, waitMsec);

#if 0
		// Close handles to the child process and its primary thread.
		// Some applications might keep these handles to monitor the status
		// of the child process, for example. 
		CloseHandle(m_piProcInfo.hProcess);
		CloseHandle(m_piProcInfo.hThread);
#endif
	}
}

// ======================================================================================
void WinProcess::CloseProcess()
{
	CloseHandle(m_piProcInfo.hProcess);
	CloseHandle(m_piProcInfo.hThread);
}

// ======================================================================================
// Read from a file and write its contents to the pipe for the child's STDIN.
// Stop when there is no more data. 
void WinProcess::WriteToPipe(HANDLE inFile)
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = FALSE;

	for (;;)
	{
		bSuccess = ReadFile(inFile, chBuf, BUFSIZE, &dwRead, NULL);
		if (!bSuccess || dwRead == 0) 
			break;

		bSuccess = WriteFile(m_hChildStd_IN_Wr, chBuf, dwRead, &dwWritten, NULL);
		if (!bSuccess) 
			break;
	}

	m_hChildStd_IN_Wr.Close();
}

// ======================================================================================
// Read output from the child process's pipe for STDOUT
// and write to the parent process's pipe for STDOUT. 
// Stop when there is no more data. 
void WinProcess::ReadFromPipe(HANDLE outHnd, std::string* pBuffer)
{
	DWORD dwRead, dwWritten;
	CHAR chBuf[BUFSIZE];
	BOOL bSuccess = TRUE;
	DWORD exitError = STILL_ACTIVE;
	if (pBuffer)
		pBuffer->clear();

	while (exitError == STILL_ACTIVE)
	{
		bSuccess = ReadFile(m_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
		if (bSuccess && dwRead != 0)
		{
			bSuccess = WriteFile(outHnd, chBuf, dwRead, &dwWritten, NULL);
			if (pBuffer)
				pBuffer->append(chBuf, chBuf + dwRead);
		}

#if 0
		exitError = WaitForSingleObject(m_piProcInfo.hProcess, 1000);
		if (exitError == WAIT_FAILED)
		{
			exitError = GetLastError();
		}
#endif
		exitError = GetExitCodeThread(m_piProcInfo.hThread, &m_exitCode);
		if (exitError == 0)
			exitError = GetLastError();
		else
			exitError = m_exitCode;
	}
	
	GetExitCodeProcess(m_piProcInfo.hProcess, &m_exitCode);
}

// ======================================================================================
// Format a readable error message, display a message box, 
// and exit from the application.
void WinProcess::ErrorExit(PTSTR lpszFunction)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40)*sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(1);
}