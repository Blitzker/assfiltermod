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
#include "AssFilter.h"
#include "AssFilterSettingsProps.h"

namespace
{
    const WCHAR name[] = L"AssFilterMod";

    const AMOVIESETUP_MEDIATYPE pinTypes[] = {
        { &MEDIATYPE_Subtitle, &MEDIASUBTYPE_ASS },
        { &MEDIATYPE_Subtitle, &MEDIASUBTYPE_UTF8 }
    };

    const AMOVIESETUP_PIN setupPin = {
        L"", TRUE, FALSE, TRUE, TRUE, &CLSID_NULL, nullptr, _countof(pinTypes), pinTypes
    };

    const AMOVIESETUP_FILTER setupFilter = {
        &__uuidof(AssFilter), name, MERIT_UNLIKELY, 1, &setupPin, &CLSID_LegacyAmFilterCategory
    };
}

CFactoryTemplate g_Templates[] = {
    { name, &__uuidof(AssFilter), AssFilter::CreateInstance, nullptr, &setupFilter },
    { L"AssFilterSettingsProp", &CLSID_AssFilterSettingsProp, CAssFilterSettingsProp::CreateInstance, nullptr, nullptr },
    { L"AssFilterStatusProp", &CLSID_AssFilterStatusProp, CAssFilterStatusProp::CreateInstance, nullptr, nullptr },
    { L"AssFilterAboutProp", &CLSID_AssFilterAboutProp, CAssFilterAboutProp::CreateInstance, nullptr, nullptr }
};

int g_cTemplates = _countof(g_Templates);
//int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);

STDAPI DllRegisterServer()
{
	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	return AMovieDllRegisterServer2(FALSE);
}

extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD dwReason, LPVOID lpReserved)
{
	return DllEntryPoint(hDllHandle, dwReason, lpReserved);
}
