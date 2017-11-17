/*   
 *   Copyright(C) 2016-2017 Blitzker
 *
 *   This program is free software : you can redistribute it and / or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#ifdef _DEBUG
#define DBG_TIMING(x,l,y) { DWORD start = timeGetTime(); y; DWORD end = timeGetTime(); if(end-start>l) DbgLog((LOG_CUSTOM5, 10, L"TIMING: %S took %u ms", x, end-start)); }
extern void DbgSetLogFile(LPCTSTR szLogFile);
extern void DbgSetLogFileDesktop(LPCTSTR szLogFile);
extern void DbgCloseLogFile();
#else
#define DBG_TIMING(x,l,y) y;
#define DbgSetLogFile(sz)
#define DbgSetLogFileDesktop(sz)
#define DbgCloseLogFile()
#endif
