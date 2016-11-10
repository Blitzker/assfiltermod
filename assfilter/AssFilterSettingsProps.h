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
#include "Tools.h"

class __declspec(uuid("DB4911BF-7703-4529-8D87-4CA3AAB837C5"))
CAssFilterSettingsProp : public CBasePropertyPage
{
private:
    IAssFilterSettings *m_pAssFilterSettings = nullptr; // Pointer to the filter's custom interface.
    COLORREF            m_acrCustClr[16];   // Custom colors

    // Settings
    AssFSettings        m_settings;

    HRESULT LoadSettings();
    HRESULT LoadDefaults();
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

class __declspec(uuid("FFF8BE53-6145-4D19-BD7E-4D7C57030389"))
CAssFilterStatusProp : public CBasePropertyPage
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

class __declspec(uuid("7077B783-80B2-4D7B-B9CD-C4EA21622B61"))
CAssFilterAboutProp : public CBasePropertyPage
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
