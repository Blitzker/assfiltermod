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

 // Settings
struct AssFSettings
{
    BOOL TrayIcon;

    std::wstring FontName;
    BOOL NativeSize;
    BOOL ScaledBorderAndShadow;
    BOOL DisableFontLigatures;
    BOOL DisableAutoLoad;
    BOOL Kerning;
    DWORD FontSize;
    DWORD FontScaleX;
    DWORD FontScaleY;
    DWORD FontSpacing;
    DWORD FontBlur;
    DWORD FontOutline;
    DWORD FontShadow;
    DWORD LineAlignment;
    DWORD MarginLeft;
    DWORD MarginRight;
    DWORD MarginVertical;
    DWORD ColorPrimary;
    DWORD ColorSecondary;
    DWORD ColorOutline;
    DWORD ColorShadow;
    DWORD CustomRes;
    DWORD SrtResX;
    DWORD SrtResY;

    std::wstring CustomTags;
    std::wstring ExtraFontsDir;
    std::wstring ExtraSubsDir;
};

// AssFilter Settings Interface
interface __declspec(uuid("2E277FB7-75C0-453C-A1ED-A6B6FD6F4728"))
IAssFilterSettings : public IUnknown
{
    // Get the current subtitle track name and language
    STDMETHOD(GetTrackInfo)(LPCWSTR *pTrackName, LPCWSTR *pTrackLang, LPCWSTR *pSubType) = 0;

    // Get info from the consumer
    STDMETHOD(GetConsumerInfo)(LPCWSTR *pName, LPCWSTR *pVersion) = 0;
};
