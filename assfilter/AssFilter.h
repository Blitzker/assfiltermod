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

#include <ass.h>
#include "AssFilterSettings.h"
#include "BaseTrayIcon.h"
#include "ISpecifyPropertyPages2.h"
#include "Tools.h"

class AssPin;

class __declspec(uuid("8A3704F3-BE3B-4944-9FF3-EE8757FDBDA5"))
AssFilter final
    : public CCritSec
    , public CBaseFilter
    , public ISubRenderProvider
    , public ISpecifyPropertyPages2
    , public IAssFilterSettings
{
public:

    AssFilter(LPUNKNOWN pUnk, HRESULT* pResult);
    ~AssFilter();

    DECLARE_IUNKNOWN;

    static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT* pResult);

    void SetMediaType(const CMediaType& mt, IPin* pPin);
    void Receive(IMediaSample* pSample, REFERENCE_TIME tSegmentStart);

    // CUnknown
    STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv) override;

    // CBaseFilter
    int GetPinCount() override;
	CBasePin* GetPin(int n) override;
    STDMETHODIMP Pause() override;
    STDMETHODIMP Stop() override;
    STDMETHODIMP JoinFilterGraph(IFilterGraph* pGraph, LPCWSTR pName) override;

    // ISubRenderProvider
    STDMETHODIMP RequestFrame(REFERENCE_TIME start, REFERENCE_TIME stop, LPVOID context) override;
    STDMETHODIMP Disconnect() override;

    STDMETHODIMP GetBool(LPCSTR field, bool* value) override;
    STDMETHODIMP GetInt(LPCSTR field, int* value) override;
    STDMETHODIMP GetSize(LPCSTR field, SIZE* value) override;
    STDMETHODIMP GetRect(LPCSTR field, RECT* value) override;
    STDMETHODIMP GetUlonglong(LPCSTR field, ULONGLONG* value) override;
    STDMETHODIMP GetDouble(LPCSTR field, double* value) override;
    STDMETHODIMP GetString(LPCSTR field, LPWSTR* value, int* chars) override;
    STDMETHODIMP GetBin(LPCSTR field, LPVOID* value, int* size) override;
    STDMETHODIMP SetBool(LPCSTR field, bool value) override;
    STDMETHODIMP SetInt(LPCSTR field, int value) override;
    STDMETHODIMP SetSize(LPCSTR field, SIZE value) override;
    STDMETHODIMP SetRect(LPCSTR field, RECT value) override;
    STDMETHODIMP SetUlonglong(LPCSTR field, ULONGLONG value) override;
    STDMETHODIMP SetDouble(LPCSTR field, double value) override;
    STDMETHODIMP SetString(LPCSTR field, LPWSTR value, int chars) override;
    STDMETHODIMP SetBin(LPCSTR field, LPVOID value, int size) override;

    // ISpecifyPropertyPages2
    STDMETHODIMP GetPages(CAUUID *pPages) override;
    STDMETHODIMP CreatePage(const GUID& guid, IPropertyPage** ppPage) override;

    // IAssFilterSettings
    STDMETHODIMP GetTrackInfo(const WCHAR **pTrackName, const WCHAR **pTrackLang, const WCHAR **pSubType) override;
    STDMETHODIMP GetConsumerInfo(const WCHAR **pName, const WCHAR **pVersion) override;

private:

    struct ASS_LibraryDeleter
    {
        void operator()(ASS_Library* p);
    };

    struct ASS_RendererDeleter
    {
        void operator()(ASS_Renderer* p);
    };

    struct ASS_TrackDeleter
    {
        void operator()(ASS_Track* p);
    };

    HRESULT LoadDefaults();
    HRESULT ReadSettings(HKEY rootKey);
    HRESULT LoadSettings();
    HRESULT SaveSettings();

    STDMETHODIMP CreateTrayIcon();

    HRESULT ConnectToConsumer(IFilterGraph* pGraph);
    HRESULT LoadFonts(IPin* pPin);
    HRESULT LoadExternalFile();

    std::unique_ptr<ASS_Library, ASS_LibraryDeleter> m_ass;
    std::unique_ptr<ASS_Renderer, ASS_RendererDeleter> m_renderer;
    std::unique_ptr<ASS_Track, ASS_TrackDeleter> m_track;

    std::unique_ptr<AssPin> m_pin;
    ISubRenderConsumer2Ptr m_consumer;
    ULONGLONG m_consumerLastId = 0;
    std::map<std::string, std::wstring> m_stringOptions;
    std::map<std::string, bool> m_boolOptions;

    // Settings
    AssFSettings    m_settings;

    bool            m_bSrtHeaderDone;   // Is the private codec data already sent?
    bool            m_bNotFirstPause;   // Is it the first graph pause?
    bool            m_bNoExtFile;       // External file exists?
    bool            m_bExternalFile;    // True when there is an external sub available
    bool            m_bUnsupportedSub;  // Sub is not supported
    std::wstring    m_wsTrackName;      // Subtitle track name.
    std::wstring    m_wsTrackLang;      // Subtitle track language.
    std::wstring    m_wsSubType;        // Subtitle track type (ASS or SRT)
    REFERENCE_TIME  m_iSubLineCount;    // Subtitle line number id
    std::wstring    m_wsConsumerName;   // Consumer name
    std::wstring    m_wsConsumerVer;    // Consumer version

    // Subtitle data
    // Used for ReadOrder duplicate feature
    struct s_sub_line
    {
        REFERENCE_TIME tStart;
        REFERENCE_TIME tStop;
        int readOrder;
        std::string subLine;
    };

    std::multimap<int, s_sub_line> mapSubLine;

    std::unique_ptr<CBaseTrayIcon> m_pTrayIcon;
};
