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

#include "AssFilterSettings.h"

// {DB4911BF-7703-4529-8D87-4CA3AAB837C5}
DEFINE_GUID(CLSID_AssFilterSettingsProp,
0xdb4911bf, 0x7703, 0x4529, 0x8d, 0x87, 0x4c, 0xa3, 0xaa, 0xb8, 0x37, 0xc5);

// {FFF8BE53-6145-4D19-BD7E-4D7C57030389}
DEFINE_GUID(CLSID_AssFilterStatusProp,
0xfff8be53, 0x6145, 0x4d19, 0xbd, 0x7e, 0x4d, 0x7c, 0x57, 0x3, 0x3, 0x89);

// {7077B783-80B2-4D7B-B9CD-C4EA21622B61}
DEFINE_GUID(CLSID_AssFilterAboutProp,
0x7077b783, 0x80b2, 0x4d7b, 0xb9, 0xcd, 0xc4, 0xea, 0x21, 0x62, 0x2b, 0x61);

class CAssFilterSettingsProp : public CBasePropertyPage
{
private:
    IAssFilterSettings *m_pAssFilterSettings = nullptr; // Pointer to the filter's custom interface.
    COLORREF            m_acrCustClr[16];   // Custom colors

    // Settings
    struct SrtSettings
    {
        BOOL TrayIcon;
        BOOL NativeSize;

        std::wstring FontName;
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

        std::wstring CustomTags;
    } m_settings;

    HRESULT LoadSettings();
    HRESULT SaveSettings();

    void DrawCustomButton(LPNMCUSTOMDRAW &lpnmCD, COLORREF crPen, COLORREF crBrush);

    void SetDirty()
    {
        m_bDirty = TRUE;
        if (m_pPageSite)
        {
            m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
        }
    }

public:
    CAssFilterSettingsProp(LPUNKNOWN pUnk, HRESULT *phr);
    ~CAssFilterSettingsProp();

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

    HRESULT OnConnect(IUnknown *pUnk) override;
    HRESULT OnDisconnect(void) override;
    HRESULT OnActivate(void) override;
    HRESULT OnApplyChanges(void) override;
    INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

};

class CAssFilterStatusProp : public CBasePropertyPage
{
private:
    IAssFilterSettings *m_pAssFilterSettings = nullptr; // Pointer to the filter's custom interface.

    void SetDirty()
    {
        m_bDirty = TRUE;
        if (m_pPageSite)
        {
            m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
        }
    }

public:
    CAssFilterStatusProp(LPUNKNOWN pUnk, HRESULT *phr);
    ~CAssFilterStatusProp();

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

    HRESULT OnConnect(IUnknown *pUnk) override;
    HRESULT OnDisconnect(void) override;
    HRESULT OnActivate(void) override;
    HRESULT OnApplyChanges(void) override;
    INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

};

class CAssFilterAboutProp : public CBasePropertyPage
{
private:
    IAssFilterSettings *m_pAssFilterSettings = nullptr; // Pointer to the filter's custom interface.

    void SetDirty()
    {
        m_bDirty = TRUE;
        if (m_pPageSite)
        {
            m_pPageSite->OnStatusChange(PROPPAGESTATUS_DIRTY);
        }
    }

public:
    CAssFilterAboutProp(LPUNKNOWN pUnk, HRESULT *phr);
    ~CAssFilterAboutProp();

    static CUnknown * WINAPI CreateInstance(LPUNKNOWN lpunk, HRESULT *phr);

    HRESULT OnConnect(IUnknown *pUnk) override;
    HRESULT OnDisconnect(void) override;
    HRESULT OnActivate(void) override;
    HRESULT OnApplyChanges(void) override;
    INT_PTR OnReceiveMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;

};
