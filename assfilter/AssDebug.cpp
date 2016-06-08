/*
 *   Copyright(C) 2016 Blitzker
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

#include "stdafx.h"
#include "AssDebug.h"

#ifdef DEBUG

#include <Shlobj.h>
#include <Shlwapi.h>

extern HANDLE m_hOutput;
volatile LONG hOutputCounter = 0;
extern HRESULT  DbgUniqueProcessName(LPCTSTR inName, LPTSTR outName);
void DbgSetLogFile(LPCTSTR szFile)
{
    HANDLE hOutput = CreateFile(szFile, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);

    if (INVALID_HANDLE_VALUE == hOutput &&
        GetLastError() == ERROR_SHARING_VIOLATION)
    {
        TCHAR uniqueName[MAX_PATH] = { 0 };
        if (SUCCEEDED(DbgUniqueProcessName(szFile, uniqueName)))
        {
            hOutput = CreateFile(uniqueName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        }
    }

    if (hOutput != INVALID_HANDLE_VALUE) {
        if (InterlockedCompareExchangePointer(&m_hOutput, hOutput, INVALID_HANDLE_VALUE) != INVALID_HANDLE_VALUE)
            CloseHandle(hOutput);
    }

    InterlockedIncrement(&hOutputCounter);
}

void DbgSetLogFileDesktop(LPCTSTR szFile)
{
    TCHAR szLogPath[512];
    SHGetFolderPath(nullptr, CSIDL_DESKTOPDIRECTORY, nullptr, 0, szLogPath);
    PathAppend(szLogPath, szFile);
    DbgSetLogFile(szLogPath);
}

void DbgCloseLogFile()
{
    LONG count = InterlockedDecrement(&hOutputCounter);
    if (count == 0 && m_hOutput != INVALID_HANDLE_VALUE) {
        FlushFileBuffers(m_hOutput);
        CloseHandle(m_hOutput);
        m_hOutput = INVALID_HANDLE_VALUE;
    }
}
#endif
