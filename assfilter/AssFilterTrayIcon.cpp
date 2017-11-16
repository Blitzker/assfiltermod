/*
 *      Copyright (C) 2010-2017 Hendrik Leppkes
 *      http://www.1f0.de
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "stdafx.h"
#include "AssFilterTrayIcon.h"
#include "PopupMenu.h"

#define STREAM_CMD_OFFSET 100

CAssFilterTrayIcon::CAssFilterTrayIcon(IBaseFilter *pFilter, const WCHAR *wszName, int resIcon)
    : CBaseTrayIcon(pFilter, wszName, resIcon)
{
}

CAssFilterTrayIcon::CAssFilterTrayIcon(IBaseFilter *pFilter, const WCHAR *wszName, int resIcon, const std::vector<s_ext_sub> &extSubFiles)
    : CBaseTrayIcon(pFilter, wszName, resIcon),
    m_ExtSubFiles(extSubFiles)
{
}

CAssFilterTrayIcon::~CAssFilterTrayIcon(void)
{
}

HMENU CAssFilterTrayIcon::GetPopupMenu()
{
    CheckPointer(m_pFilter, nullptr);
    CPopupMenu menu;

    int curExtSub = 0;
    IAFMExtSubtitles *pExtSubtitles = nullptr;
    if (SUCCEEDED(m_pFilter->QueryInterface(&pExtSubtitles)))
    {
        curExtSub = pExtSubtitles->GetCurExternalSub();
        pExtSubtitles->Release();
    }

    for (auto i = 0; i < m_ExtSubFiles.size(); ++i)
    {
        BOOL bChecked = FALSE;
        if (curExtSub == i)
            bChecked = TRUE;

        std::wstring nameAndType;
        if (!m_ExtSubFiles[i].subAltName.empty())
            nameAndType.assign(m_ExtSubFiles[i].subAltName + L" (" + m_ExtSubFiles[i].subType + L")");
        else
            nameAndType.assign(m_ExtSubFiles[i].subLang + L" (" + m_ExtSubFiles[i].subType + L")");

        menu.AddItem(STREAM_CMD_OFFSET + i, (LPWSTR)nameAndType.c_str(), bChecked);

        if (i == m_ExtSubFiles.size() - 1)
            menu.AddSeparator();
    }

    menu.AddItem(STREAM_CMD_OFFSET - 1, L"Properties");

    HMENU hMenu = menu.Finish();
    return hMenu;
}

HRESULT CAssFilterTrayIcon::ProcessMenuCommand(HMENU hMenu, int cmd)
{
    CheckPointer(m_pFilter, E_FAIL);
    DbgLog((LOG_TRACE, 10, L"Menu Command %d", cmd));
    if (cmd >= STREAM_CMD_OFFSET && cmd < m_ExtSubFiles.size() + STREAM_CMD_OFFSET)
    {
        IAFMExtSubtitles *pExtSubtitles = nullptr;
        if (SUCCEEDED(m_pFilter->QueryInterface(&pExtSubtitles)))
        {
            pExtSubtitles->SetCurExternalSub(cmd - STREAM_CMD_OFFSET);
            pExtSubtitles->Release();
        }
    }
    else if (cmd == STREAM_CMD_OFFSET - 1)
        OpenPropPage();
    else
        return E_UNEXPECTED;

    return S_OK;
}
