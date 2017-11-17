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

#include "stdafx.h"
#include "AssFilter.h"
#include "AssFilterSettingsProps.h"
#include "AssFilterAutoLoader.h"

namespace
{
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

    const AMOVIESETUP_MEDIATYPE pinTypes2[] = {
        { &GUID_NULL, &GUID_NULL }
    };

    const AMOVIESETUP_PIN setupPin2 = {
        L"Input", FALSE, FALSE, TRUE, FALSE, &CLSID_NULL, nullptr, _countof(pinTypes2), pinTypes2
    };

    const AMOVIESETUP_FILTER setupFilter[] = {
        { &__uuidof(AssFilter), L"AssFilterMod", MERIT_PREFERRED+2, 1, &setupPin },
        { &__uuidof(AssFilterAutoLoader), L"AssFilterModAutoLoad", 0xffffffff, 1, &setupPin2 }
    };
}

CFactoryTemplate g_Templates[] = {
    { setupFilter[0].strName, &__uuidof(AssFilter), AssFilter::CreateInstance, nullptr, &setupFilter[0] },
    { setupFilter[1].strName, &__uuidof(AssFilterAutoLoader), AssFilterAutoLoader::CreateInstance, nullptr, &setupFilter[1] },
    { L"AssFilterGeneralProp", &__uuidof(CAssFilterGeneralProp), CAssFilterGeneralProp::CreateInstance, nullptr, nullptr },
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
