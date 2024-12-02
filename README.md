![https://landenlabs.com/console/llwatch/llwatch64.png](https://landenlabs.com/console/llwatch/llwatch.jpg)

#LLWatch
Windows / DOS   Watch program execution similar to unix / linux watch command

LLWatch repeats execution of a DOS command and monitors its console output.
Optionally homing the cusor and highlighting changes.

 [![Build status](https://travis-ci.org/landenlabs/llwatch.svg?branch=master)](https://travis-ci.org/landenlabs/llwatch)


Visit home website

[https://landenlabs.com](https://landenlabs.com)


Help Banner:
<pre>
LLWatch v1.1 - Jan 23 2016
By: Dennis Lang
https://landenlabs.com/

SUMMARY:
  Watch - execute a program periodically, showing output

USAGE:
  llwatch [-dhv] [-t #lines][-b #lines] [-n <seconds>] -- <command>

DESCRIPTION:  Watch runs command repeatedly, displaying its output. This allows you to
  Watch the program output change over time. By default, the program is run
  every 2 seconds.

  -d  Disable highlighting the differences between successive updates.
  -h  Home cursor between updates
  -n <seconds> Specify update interval, default 2 seconds
  -t <#lines> Limit output to top # lines, default is 20
  -b <#lines> Limit output to bottom # lines, default is all
  -v  Toggle verbose output
  -g <pattern> Match grep pattern for line to show.
  -r <replace> Use with -g and perform replacement per line.

EXAMPLES:
    To watch the contents of a directory change, you could use:
       llwatch -- cmd /c dir *.txt

    Use find to filter command output
       llwatch -- cmd /c "c:\Windows\System32\tasklist.exe | find "Console""
    Use built-in grep filter to file command output
       llwatch -g Console -- c:\Windows\System32\tasklist.exe
</pre>

Help banner

![https://landenlabs.com/console/llwatch/help.png](https://landenlabs.com/console/llwatch/help.png)

Example of scrolling output and highlighted changes:

![https://landenlabs.com/console/llwatch/tasklist.png](https://landenlabs.com/console/llwatch/tasklist.png)

Example of updating screen with 'home screen' enabled:

![https://landenlabs.com/console/llwatch/refresh.gif](https://landenlabs.com/console/llwatch/refresh.gif)

