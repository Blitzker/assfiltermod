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

#pragma once

// {2E277FB7-75C0-453C-A1ED-A6B6FD6F4728}
DEFINE_GUID(IID_IAssFilterSettings,
0x2e277fb7, 0x75c0, 0x453c, 0xa1, 0xed, 0xa6, 0xb6, 0xfd, 0x6f, 0x47, 0x28);

// AssFilter Settings Interface
interface __declspec(uuid("2E277FB7-75C0-453C-A1ED-A6B6FD6F4728"))
IAssFilterSettings : public IUnknown
{
    // Get the current subtitle track name and language
    STDMETHOD(GetTrackInfo)(LPCWSTR *pTrackName, LPCSTR *pTrackLang, LPCSTR *pSubType) = 0;

    // Get info from the consumer
    STDMETHOD(GetConsumerInfo)(LPCWSTR *pName, LPCWSTR *pVersion) = 0;
};
