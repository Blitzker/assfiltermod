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
 *
 *  Initial design and concept by Gabest and the MPC-HC Team, copyright under GPLv2
 */

#include "stdafx.h"
#include "FontInstaller.h"

CFontInstaller::CFontInstaller()
{
}

CFontInstaller::~CFontInstaller()
{
	UninstallFonts();
}

bool CFontInstaller::InstallFont(const void* pData, UINT len)
{
	return InstallFontMemory(pData, len);
}

bool CFontInstaller::InstallFont(const std::wstring& font)
{
    return InstallFontResource(font);
}

void CFontInstaller::UninstallFonts()
{
    std::vector<HANDLE>::iterator itm;
    for(itm = m_fonts_memory.begin(); itm != m_fonts_memory.end(); ++itm)
      RemoveFontMemResourceEx(*itm);

    m_fonts_memory.clear();

    std::vector<std::wstring>::iterator itr;
    for (itr = m_fonts_resource.begin(); itr != m_fonts_resource.end(); ++itr)
        RemoveFontResourceEx(itr->c_str(), FR_PRIVATE, NULL);

    m_fonts_resource.clear();
}

bool CFontInstaller::InstallFontMemory(const void* pData, UINT len)
{
	DWORD nFonts = 0;
	HANDLE hFont = AddFontMemResourceEx((PVOID)pData, len, nullptr, &nFonts);
	if(hFont && nFonts > 0) {
		m_fonts_memory.push_back(hFont);
	}
	return hFont && nFonts > 0;
}

bool CFontInstaller::InstallFontResource(const std::wstring& font)
{
    if (AddFontResourceEx(font.c_str(), FR_PRIVATE, NULL))
    {
        m_fonts_resource.push_back(font);
        return true;
    }
    return false;
}
