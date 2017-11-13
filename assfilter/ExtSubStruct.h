/*
 *   Copyright(C) 2017 Blitzker
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

// External subtitles
struct s_ext_sub
{
    std::wstring subFile;
    std::wstring subLang;
    std::wstring subType;
    std::wstring yuvMatrix;
    UINT codePage;
    size_t vecPos;
};

// {41E2AAD5-7574-407D-9740-2596C3C4D7C9}
DEFINE_GUID(IID_IAFMExtSubtitles, 
0x41e2aad5, 0x7574, 0x407d, 0x97, 0x40, 0x25, 0x96, 0xc3, 0xc4, 0xd7, 0xc9);

interface __declspec(uuid("41E2AAD5-7574-407D-9740-2596C3C4D7C9"))
IAFMExtSubtitles : public IUnknown
{
    // Get the id of the current loaded external subtitle
    STDMETHOD_(int, GetCurExternalSub)() = 0;

    // Request a change of external subtitle
    STDMETHOD(SetCurExternalSub)(int iCurExtSub) = 0;
};
