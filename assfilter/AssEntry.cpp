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
        { &MEDIATYPE_Subtitle, &MEDIASUBTYPE_UTF8 },
        { &MEDIATYPE_Subtitle, &MEDIASUBTYPE_SSA },
        { &MEDIATYPE_Subtitle, &MEDIASUBTYPE_VOBSUB },
        { &MEDIATYPE_Subtitle, &MEDIASUBTYPE_HDMVSUB },
    };

    const AMOVIESETUP_PIN setupPin = {
        L"Input", TRUE, FALSE, TRUE, TRUE, &CLSID_NULL, nullptr, _countof(pinTypes), pinTypes
    };

    const AMOVIESETUP_FILTER setupFilter = {
        &__uuidof(AssFilter), name, MERIT_UNLIKELY, 1, &setupPin, &CLSID_LegacyAmFilterCategory
    };
}

CFactoryTemplate g_Templates[] = {
    { name, &__uuidof(AssFilter), AssFilter::CreateInstance, nullptr, &setupFilter },
    { L"AssFilterSettingsProp", &__uuidof(CAssFilterSettingsProp), CAssFilterSettingsProp::CreateInstance, nullptr, nullptr },
    { L"AssFilterStatusProp", &__uuidof(CAssFilterStatusProp), CAssFilterStatusProp::CreateInstance, nullptr, nullptr },
    { L"AssFilterAboutProp", &__uuidof(CAssFilterAboutProp), CAssFilterAboutProp::CreateInstance, nullptr, nullptr }
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
