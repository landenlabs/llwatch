// ------------------------------------------------------------------------------------------------
// Watch a program execute (similar to Unix 'watch' command)
//
// Project: LLWatch
// Author:  Dennis Lang   Jan-2016
// http:/LanDenLabs.com/
//
// ----- License ----
//
// Copyright (c) 2016 Dennis Lang
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
 
#pragma warning( disable : 4995 )

#include <Windows.h>

#include <iostream>
#include <string>
#include <stdio.h> 
#include <strsafe.h>

// Windows specific classes
#include "WinProcess.h"
#include "WinCursor.h"
#include "Colorize.h"
#include "GetOpts.h"

#include "llstring.h"



using namespace std;
typedef unsigned int uint;
typedef std::string String;

#define BUFSIZE 4096 

#define _VERSION "v1.1"
char sUsage[] =
"\n"
"LLWatch " _VERSION " - " __DATE__ "\n"
"By: Dennis Lang\n"
"https://landenlabs.com/\n"
"\n"
"SUMMARY:\n"
"  Watch - execute a program periodically, showing output \n"
"\n"
"USAGE: \n"
"  llwatch [-dhv] [-t #lines][-b #lines] [-n <seconds>] -- <command> \n"
"\n"
"DESCRIPTION:"
"  Watch runs command repeatedly, displaying its output. This allows you to \n"
"  Watch the program output change over time. By default, the program is run\n"
"  every 2 seconds. \n"
"\n"
"  -d  Disable highlighting the differences between successive updates. \n"
"  -h  Home cursor between updates \n"
"  -n <seconds> Specify update interval, default 2 seconds \n"
"  -t <#lines> Limit output to top # lines, default is 20 \n"
"  -b <#lines> Limit output to bottom # lines, default is all \n"
"  -v  Toggle verbose output \n"

#ifdef HAVE_REGEX
"  -g <pattern> Match grep pattern for line to show. \n"
"  -r <replace> Use with -g and perform replacement per line.  \n"
#endif

"\n"
"EXAMPLES:\n"
"    To watch the contents of a directory change, you could use: \n"
"       llwatch -- cmd /c dir *.txt \n"
"\n"
"    Use find to filter command output\n"
"       llwatch -- cmd /c \"c:\\Windows\\System32\\tasklist.exe | find \"Console\"\" \n"
"    Use built-in grep filter to file command output\n"
"       llwatch -g Console -- c:\\Windows\\System32\\tasklist.exe \n"
"\n"
"\n";

bool m_highlightDelta = true;
bool m_homeCursor = false;
uint m_seconds = 2;
uint m_maxRunCnt = -1;
uint m_topLines = 20;
uint m_bottomLines = 0;

#ifdef HAVE_REGEX
bool m_isGrepLinePat = false;
std::tr1::regex     m_grepLinePat;      // -G=<grepPattern>
std::string         m_replaceStr;       // -R=<replacePattern>
#endif

bool m_verbose = true;
const char MATCH_COLOR[] = "!07";
const char DIFF_COLOR[] = "!0e";

// ======================================================================================
void showDiffFast(const lstring& currBuffer, const lstring& prevBuffer)
{
	unsigned endIdx = (unsigned)min(currBuffer.length(), prevBuffer.length());
	unsigned startIdx = 0;
	unsigned idx = 0;
	while (idx != endIdx)
	{
		while (idx != endIdx && currBuffer[idx] == prevBuffer[idx])
			idx++;
		Colorize::write(std::cout, MATCH_COLOR);
		Colorize::write(std::cout, currBuffer + startIdx, idx - startIdx);
		startIdx = idx;
		while (idx != endIdx && currBuffer[idx] != prevBuffer[idx])
			idx++;
		Colorize::write(std::cout, DIFF_COLOR);
		Colorize::write(std::cout, currBuffer + startIdx, idx - startIdx);
		startIdx = idx;
	}

	Colorize::write(std::cout, MATCH_COLOR);
	idx = (unsigned)currBuffer.length();
	if (idx > startIdx)
		Colorize::write(std::cout, currBuffer + startIdx, idx - startIdx);
}

// ======================================================================================
void TrimTopBottom(lstring& currBuffer, uint topLines, uint bottomLines)
{
	lstring eol("\n");
	uint lineCnt = currBuffer.count(eol);
	if (lineCnt > bottomLines && bottomLines != 0)
	{
		uint skipLines = lineCnt - bottomLines;
		size_t offset = currBuffer.findCnt(eol, 0, skipLines);
		currBuffer.erase(0, offset);
	}
	else if (lineCnt > topLines && topLines != 0)
	{
		size_t offset = currBuffer.findCnt(eol, 0, topLines);
		currBuffer.resize(offset + eol.length());
	}
}

// ======================================================================================
void RegexTrim(lstring& currBuffer, const std::tr1::regex& grepLinePat, const lstring& replaceStr)
{
	const char eol[] = "\n";
	Split lines(currBuffer, eol);
	bool changed = false;
	size_t size = 0;
	
	bool doReplace = !replaceStr.empty();
	for (size_t idx = lines.size() - 1; idx < lines.size(); idx--)
	{
		lstring& str = lines[idx];
		bool keep;
		if (doReplace)
			keep = str.regReplace(grepLinePat, replaceStr);
		else
			keep = str.regFind(grepLinePat);

		if (!keep || str.isSpace())
			lines.erase(lines.begin() + idx);
		else
			size += str.length();
	}

	std::string result;
	result.reserve(size);

	for (unsigned idx = 0; idx != lines.size(); idx++)
		result.append(lines[idx] + eol);

	currBuffer.swap(result);
}

// ======================================================================================
int main(int argc, const char *argv[])
{
	if (argc == 1)
	{
		std::cerr << sUsage;
		return -1;
	}

	lstring cmdLine = GetCommandLine();
    size_t eraseCnt = strlen(argv[0]) + 1 + ((cmdLine.at(0) == '"') ? 2 : 0);
    cmdLine.erase(0, eraseCnt);
	int off = (int)cmdLine.find("--");
	if (off != -1)
		cmdLine.erase(0, off + 2);

	cmdLine.trim();

	if (cmdLine.length() == 0)
	{
		std::cerr << "Specify a command to execute\n";
		return -1;
	}

#ifdef HAVE_REGEX
	const char opts[] = "b:dg:r:hn:t:v?";
#else
	const char opts[] = "b:dhn:t:v?";
#endif

	GetOpts<char> getOpts(argc, argv, opts);
	char* endPtr;
	while (getOpts.GetOpt())
	{
		switch (getOpts.Opt())
		{
		case 'b':	// keep bottom lines
			m_bottomLines = strtoul(getOpts.OptArg(), &endPtr, 10);
			if (endPtr == getOpts.OptArg())
			{
				std::cerr << "Invalid # bottom lines:" << getOpts.OptArg() << std::endl;
				return -1;
			}
			break;

		case 'd':	// toggle delta highlight. 
			m_highlightDelta = !m_highlightDelta;
			break;

#ifdef HAVE_REGEX
		case 'g':	// grep (match per line to show)
			m_isGrepLinePat = strlen(getOpts.OptArg()) != 0;
			if (m_isGrepLinePat)
				m_grepLinePat = getOpts.OptArg();
			
			break;
		case 'r':	// replace, used with grep to 
			m_replaceStr = getOpts.OptArg();
			break;
#endif

		case 'h':	// home cursor between runs
			m_homeCursor = true;
			break;

		case 'n':	// seconds between updates
			m_seconds = strtoul(getOpts.OptArg(), &endPtr, 10);
			if (endPtr == getOpts.OptArg())
			{
				std::cerr << "Invalid seconds:" << getOpts.OptArg() << std::endl;
				return -1;
			}
			break;

		case 't':	// keep top limes
			m_topLines = strtoul(getOpts.OptArg(), &endPtr, 10);
			if (endPtr == getOpts.OptArg())
			{
				std::cerr << "Invalid # top lines:" << getOpts.OptArg() << std::endl;
				return -1;
			}
			break;

		case 'v':	// verbose
			m_verbose = !m_verbose;
			break;

		default:
		case '?':	// show help banner
			std::cout << sUsage;
			return 0;
		}
	}


	WinProcess winProcess;
	HANDLE hParentStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hNull = INVALID_HANDLE_VALUE;
	lstring prevBuffer;
	lstring currBuffer;

	if (m_homeCursor)
		WinCursor::ClearScreen(" ");

	DWORD dwWritten;
	uint runCnt = 0;
	for (runCnt = 0; runCnt < m_maxRunCnt; runCnt++)
	{
		if (m_homeCursor)
			WinCursor::SetCursorPosition(0, 0);

		if (m_verbose)
			std::cerr << "---[Execute=" << cmdLine << "]---\n";
		winProcess.CreateChildProcess(cmdLine);
		if (m_highlightDelta)
		{
			winProcess.ReadFromPipe(hNull, &currBuffer);
#ifdef HAVE_REGEX
			if (m_isGrepLinePat)
				RegexTrim(currBuffer, m_grepLinePat, m_replaceStr);
#endif
			TrimTopBottom(currBuffer, m_topLines, m_bottomLines);

			if (prevBuffer.empty())
				 WriteFile(hParentStdOut, currBuffer.c_str(), (DWORD)currBuffer.length(), &dwWritten, NULL);
			else
			{
				// showDiffLcs(currBuffer, prevBuffer);
				showDiffFast(currBuffer, prevBuffer);
			}
			prevBuffer.swap(currBuffer);
		}
		else
		{
			winProcess.ReadFromPipe(hParentStdOut);
		}
		winProcess.CloseProcess();
		if (m_verbose)
			std::cerr << "\n---[Exit code=" << winProcess.m_exitCode << " RunCnt=" << runCnt << "]---\n";
		Sleep(m_seconds * 1000);
	}

	return 0;
}


