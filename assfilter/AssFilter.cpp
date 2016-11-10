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
#include "AssDebug.h"
#include "AssPin.h"
#include "AssFilterSettingsProps.h"
#include "registry.h"
#include "SubFrame.h"
#include "Tools.h"

AssFilter::AssFilter(LPUNKNOWN pUnk, HRESULT* pResult)
	: CBaseFilter("", pUnk, this, __uuidof(AssFilter))
{
    if (pResult) *pResult = S_OK;
    m_pin = std::make_unique<AssPin>(this, pResult);

    m_ass = decltype(m_ass)(ass_library_init());
    m_renderer = decltype(m_renderer)(ass_renderer_init(m_ass.get()));

    m_stringOptions["name"] = L"AssFilterMod";
    m_stringOptions["version"] = L"0.2.1.0";
    m_stringOptions["yuvMatrix"] = L"None";
    m_stringOptions["outputLevels"] = L"PC";
    m_boolOptions["combineBitmaps"] = false;
    m_boolOptions["isMovable"] = false;

    m_bSrtHeaderDone = false;

    LoadSettings();

#ifdef DEBUG
    DbgSetModuleLevel(LOG_ERROR, DWORD_MAX);
    DbgSetModuleLevel(LOG_TRACE, DWORD_MAX);
    DbgSetLogFileDesktop(L"AssFilterDbg.Log");
#endif
}

AssFilter::~AssFilter()
{
    if (m_consumer)
        m_consumer->Disconnect();

#ifdef DEBUG
    DbgCloseLogFile();
#endif
}

CUnknown* WINAPI AssFilter::CreateInstance(LPUNKNOWN pUnk, HRESULT* pResult)
{
    DbgLog((LOG_TRACE, 1, L"AssFilter::CreateInstance()"));

    try
    {
        return new AssFilter(pUnk, pResult);
    }
    catch (std::bad_alloc&)
    {
        if (pResult) *pResult = E_OUTOFMEMORY;
    }

    return nullptr;
}

void AssFilter::SetMediaType(const CMediaType& mt, IPin* pPin)
{
    DbgLog((LOG_TRACE, 1, L"AssFilter::SetMediaType()"));

    CAutoLock lock(this);

    // Check if there is already a track
    bool bTrackExist = false;
    if (m_track) bTrackExist = true;

    m_track = decltype(m_track)(ass_new_track(m_ass.get()));

    struct SUBTITLEINFO
    {
        DWORD dwOffset;
        CHAR  IsoLang[4];
        WCHAR TrackName[256];
    };

    auto psi = reinterpret_cast<const SUBTITLEINFO*>(mt.Format());

    m_wsTrackName.assign(psi->TrackName);
    m_sTrackLang.assign(MatchLanguage(std::string(psi->IsoLang)) + " (" + std::string(psi->IsoLang) + ")");
    m_sSubType.assign("ASS");

    // SRT Stuff
    if (mt.subtype == MEDIASUBTYPE_UTF8)
    {
        DbgLog((LOG_TRACE, 1, L"AssFilter::SetMediaType() -> SRT Mode"));
        m_sSubType.assign("SRT");

        m_bSrtHeaderDone = false;
        m_boolOptions["isMovable"] = true;
    }
    else
    {
        m_boolOptions["isMovable"] = false;
        ass_process_codec_private(m_track.get(), (char*)mt.Format() + psi->dwOffset, mt.FormatLength() - psi->dwOffset);
    }

    // If a track already exist, return cuz we don't need to allocate the fonts again.
    if (bTrackExist)
    {
        // If the splitter don't stop the graph when switching tracks,
        // we need to clear the consumer subtitles buffer.
        if (m_consumer)
            m_consumer->Clear();
        return;
    }

    IAMGraphStreamsPtr graphStreams;
    IDSMResourceBagPtr bag;
    if (SUCCEEDED(GetFilterGraph()->QueryInterface(IID_PPV_ARGS(&graphStreams))) &&
        SUCCEEDED(graphStreams->FindUpstreamInterface(pPin, IID_PPV_ARGS(&bag), AM_INTF_SEARCH_FILTER)))
    {
        for (DWORD i = 0; i < bag->ResGetCount(); i++)
        {
            _bstr_t name, desc, mime;
            BYTE* pData = nullptr;
            DWORD len = 0;
            if (SUCCEEDED(bag->ResGet(i, &name.GetBSTR(), &desc.GetBSTR(), &mime.GetBSTR(), &pData, &len, nullptr)))
            {
                if (wcscmp(mime.GetBSTR(), L"application/x-truetype-font") == 0 ||
                    wcscmp(mime.GetBSTR(), L"application/vnd.ms-opentype") == 0) // TODO: more mimes?
                {
                    ass_add_font(m_ass.get(), "", (char*)pData, len);
                    // TODO: clear these fonts somewhere?
                }
                CoTaskMemFree(pData);
            }
        }
    }

    ass_set_fonts(m_renderer.get(), 0, 0, ASS_FONTPROVIDER_DIRECTWRITE, 0, 0);
}

void AssFilter::Receive(IMediaSample* pSample, REFERENCE_TIME tSegmentStart)
{
    // TODO: resolve a race with RequestFrame()
    DbgLog((LOG_TRACE, 1, L"AssFilter::Receive() tSegmentStart: %I64d", tSegmentStart));

    REFERENCE_TIME tStart, tStop;
    BYTE* pData;

    CAutoLock lock(this);

    if (SUCCEEDED(pSample->GetTime(&tStart, &tStop)) &&
        SUCCEEDED(pSample->GetPointer(&pData)))
    {
        tStart += tSegmentStart;
        tStop += tSegmentStart;

        if (m_sSubType == "SRT")
        {
            // Send the codec private data
            if (!m_bSrtHeaderDone)
            {
                char outBuffer[1024] {};

                // Generate a standard ass header
                _snprintf_s(outBuffer, _TRUNCATE, "[Script Info]\n"
                    "; Script generated by ParseSRT\n"
                    "Title: ParseSRT generated file\n"
                    "ScriptType: v4.00+\n"
                    "WrapStyle: 0\n"
                    "ScaledBorderAndShadow: yes\n"
                    "YCbCr Matrix: TV.601\n"
                    "PlayResX: 1280\n"
                    "PlayResY: 720\n"
                    "Video Aspect Ratio: c1.777778\n\n"
                    "[V4+ Styles]\n"
                    "Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, "
                    "BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, "
                    "BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding\n"
                    "Style: Default,%s,%u,&H%X,&H%X,&H%X,&H%X,0,0,0,0,%u,%u,%u,0,1,%u,%u,%u,%u,%u,%u,1"
                    "\n\n[Events]\n"
                    "Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text\n\n",
                    ws2s(m_settings.FontName).c_str(), m_settings.FontSize, m_settings.ColorPrimary,
                    m_settings.ColorSecondary, m_settings.ColorOutline, m_settings.ColorShadow, 
                    m_settings.FontScaleX, m_settings.FontScaleY, m_settings.FontSpacing, m_settings.FontOutline, 
                    m_settings.FontShadow, m_settings.LineAlignment, m_settings.MarginLeft, m_settings.MarginRight, 
                    m_settings.MarginVertical);
                ass_process_codec_private(m_track.get(), outBuffer, strnlen_s(outBuffer, sizeof(outBuffer)));
                m_bSrtHeaderDone = true;
            }

            // Subtitle data is in UTF-8 format.
            char subLineData[1024] {};
            strncpy_s(subLineData, _countof(subLineData), (char*)pData, pSample->GetActualDataLength());
            std::string str = subLineData;

            // This is the way i use to get a unique id for the subtitle line
            // It will only fail in the case there is 2 or more lines with the same start timecode
            // (Need to check if the matroska muxer join lines in such a case)
            m_iSubLineCount = tStart / 10000;

            // Change srt tags to ass tags
            ParseSrtLine(str, m_settings.ColorPrimary, m_settings.ColorOutline);

            // Add the custom tags
            str.insert(0, ws2s(m_settings.CustomTags));

            // Add blur
            char blur[20] {};
            _snprintf_s(blur, _TRUNCATE, "{\\blur%u}", m_settings.FontBlur);
            str.insert(0, blur);

            // ASS in MKV: ReadOrder, Layer, Style, Name, MarginL, MarginR, MarginV, Effect, Text
            char outBuffer[1024] {};
            _snprintf_s(outBuffer, _TRUNCATE, "%lld,0,Default,Main,0,0,0,,%s", m_iSubLineCount, str.c_str());
            ass_process_chunk(m_track.get(), outBuffer, strnlen_s(outBuffer, sizeof(outBuffer)), tStart / 10000, (tStop - tStart) / 10000);
        }
        else
        {
            ass_process_chunk(m_track.get(), (char*)pData, pSample->GetActualDataLength(), tStart / 10000, (tStop - tStart) / 10000);
        }
    }
}

STDMETHODIMP AssFilter::NonDelegatingQueryInterface(REFIID riid, void** ppv)
{
    CheckPointer(ppv, E_POINTER);

    if (riid == __uuidof(ISubRenderOptions))
        return GetInterface(static_cast<ISubRenderOptions*>(this), ppv);
    else if (riid == __uuidof(ISubRenderProvider))
        return GetInterface(static_cast<ISubRenderProvider*>(this), ppv);
    else if (riid == __uuidof(ISpecifyPropertyPages))
        return GetInterface(static_cast<ISpecifyPropertyPages*>(this), ppv);
    else if (riid == __uuidof(ISpecifyPropertyPages2))
        return GetInterface(static_cast<ISpecifyPropertyPages2*>(this), ppv);
    else if (riid == __uuidof(IAssFilterSettings))
        return GetInterface(static_cast<IAssFilterSettings*>(this), ppv);

    return __super::NonDelegatingQueryInterface(riid, ppv);
}

int AssFilter::GetPinCount()
{
	return 1;
}

CBasePin* AssFilter::GetPin(int n)
{
	return m_pin.get();
}

STDMETHODIMP AssFilter::Pause()
{
    DbgLog((LOG_TRACE, 1, L"AssFilter::Pause()"));

    CAutoLock lock(this);
    
    if (m_pGraph)
    {
        IEnumFiltersPtr filters;
        if (SUCCEEDED(m_pGraph->EnumFilters(&filters)))
        {
            ISubRenderConsumer2Ptr consumer;
            ISubRenderProviderPtr provider;

            for (IBaseFilterPtr filter; filters->Next(1, &filter, 0) == S_OK;)
            {
                if (SUCCEEDED(filter->QueryInterface(IID_PPV_ARGS(&consumer))) &&
                    SUCCEEDED(QueryInterface(IID_PPV_ARGS(&provider))) &&
                    SUCCEEDED(consumer->Connect(provider)))
                {
                    DbgLog((LOG_TRACE, 1, L"AssFilter::Pause() -> Connected to consumer"));
                    m_consumer = consumer;
                    m_consumerLastId = 0;
                }
            }
        }
    }

    return __super::Pause();
}

STDMETHODIMP AssFilter::Stop()
{
    DbgLog((LOG_TRACE, 1, L"AssFilter::Stop()"));

    CAutoLock lock(this);

    if (m_consumer)
    {
        m_consumer->Disconnect();
        m_consumer = nullptr;
    }

    return __super::Stop();
}

STDMETHODIMP AssFilter::RequestFrame(REFERENCE_TIME start, REFERENCE_TIME stop, LPVOID context)
{
    DbgLog((LOG_TRACE, 1, L"AssFilter::RequestFrame() start: %I64d, stop: %I64d", start, stop));

    CAutoLock lock(this);

    CheckPointer(m_consumer, E_UNEXPECTED);

    RECT videoOutputRect;
    m_consumer->GetRect("videoOutputRect", &videoOutputRect);
    DbgLog((LOG_TRACE, 1, L"AssFilter::RequestFrame() videoOutputRect: %u, %u, %u, %u", videoOutputRect.left, videoOutputRect.top, videoOutputRect.right, videoOutputRect.bottom));

    //RECT subtitleTargetRect;
    //m_consumer->GetRect("subtitleTargetRect", &subtitleTargetRect);
    //DbgLog((LOG_TRACE, 1, L"AssFilter::RequestFrame() subtitleTargetRect: %u, %u, %u, %u", subtitleTargetRect.left, subtitleTargetRect.top, subtitleTargetRect.right, subtitleTargetRect.bottom));

    // The video rect we render the subtitles on
    RECT videoRect{};

    // Check to draw subtitles at custom resolution
    if (m_settings.NativeSize)
    {
        SIZE originalVideoSize;
        m_consumer->GetSize("originalVideoSize", &originalVideoSize);

        switch (m_settings.CustomRes)
        {
        case 0:
            videoRect.right = originalVideoSize.cx;
            videoRect.bottom = originalVideoSize.cy;
            break;
        case 1:
            videoRect.right = 3840;
            videoRect.bottom = 2160;
            break;
        case 2:
            videoRect.right = 2560;
            videoRect.bottom = 1440;
            break;
        case 3:
            videoRect.right = 1920;
            videoRect.bottom = 1080;
            break;
        case 4:
            videoRect.right = 1440;
            videoRect.bottom = 900;
            break;
        case 5:
            videoRect.right = 1280;
            videoRect.bottom = 720;
            break;
        case 6:
            videoRect.right = 1024;
            videoRect.bottom = 768;
            break;
        case 7:
            videoRect.right = 800;
            videoRect.bottom = 600;
            break;
        case 8:
            videoRect.right = 640;
            videoRect.bottom = 480;
            break;
        default:
            videoRect.right = originalVideoSize.cx;
            videoRect.bottom = originalVideoSize.cy;
            break;
        }
    }
    else
        videoRect = videoOutputRect;

    ass_set_frame_size(m_renderer.get(), videoRect.right , videoRect.bottom);

    DbgLog((LOG_TRACE, 1, L"AssFilter::RequestFrame() videoRect: %u, %u, %u, %u", videoRect.left, videoRect.top, videoRect.right, videoRect.bottom));

    int frameChange = 0;
    ISubRenderFramePtr frame = new SubFrame(videoRect, m_consumerLastId++,
                                            ass_render_frame(m_renderer.get(), m_track.get(), start / 10000, &frameChange));
    return m_consumer->DeliverFrame(start, stop, context, frame);
}

STDMETHODIMP AssFilter::Disconnect(void)
{
    DbgLog((LOG_TRACE, 1, L"AssFilter::Disconnect()"));

    CAutoLock lock(this);
    m_consumer = nullptr;
    return S_OK;
}

STDMETHODIMP AssFilter::GetBool(LPCSTR field, bool* value)
{
    CheckPointer(value, E_POINTER);
    *value = m_boolOptions[field];
    return S_OK;
}

STDMETHODIMP AssFilter::GetInt(LPCSTR field, int* value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::GetSize(LPCSTR field, SIZE* value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::GetRect(LPCSTR field, RECT* value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::GetUlonglong(LPCSTR field, ULONGLONG* value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::GetDouble(LPCSTR field, double* value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::GetString(LPCSTR field, LPWSTR* value, int* chars)
{
    CheckPointer(value, E_POINTER);
    auto str = m_stringOptions[field];
    size_t len = str.length();
    *value = (LPWSTR)LocalAlloc(0, (len + 1) * sizeof(WCHAR));
    memcpy(*value, str.data(), len * sizeof(WCHAR));
    (*value)[len] = '\0';
    if (chars)
        *chars = static_cast<int>(len);
    DbgLog((LOG_TRACE, 1, L"AssFilter::GetString() field: %S, value: %s, chars: %d", field, *value, *chars));
    return S_OK;
}

STDMETHODIMP AssFilter::GetBin(LPCSTR field, LPVOID* value, int* size)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::SetBool(LPCSTR field, bool value)
{
    if (!strcmp(field, "combineBitmaps"))
        m_boolOptions[field] = value;
    return S_OK;
}

STDMETHODIMP AssFilter::SetInt(LPCSTR field, int value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::SetSize(LPCSTR field, SIZE value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::SetRect(LPCSTR field, RECT value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::SetUlonglong(LPCSTR field, ULONGLONG value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::SetDouble(LPCSTR field, double value)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::SetString(LPCSTR field, LPWSTR value, int chars)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::SetBin(LPCSTR field, LPVOID value, int size)
{
    return S_FALSE;
}

STDMETHODIMP AssFilter::GetPages(CAUUID *pPages)
{
    DbgLog((LOG_TRACE, 1, L"AssFilter::GetPages()"));

    CheckPointer(pPages, E_POINTER);
    pPages->cElems = 3;
    pPages->pElems = (GUID*)CoTaskMemAlloc(sizeof(GUID) * pPages->cElems);
    if (pPages->pElems == nullptr)
    {
        return E_OUTOFMEMORY;
    }
    pPages->pElems[0] = __uuidof(CAssFilterSettingsProp);
    pPages->pElems[1] = __uuidof(CAssFilterStatusProp);
    pPages->pElems[2] = __uuidof(CAssFilterAboutProp);

    return S_OK;
}

STDMETHODIMP AssFilter::CreatePage(const GUID& guid, IPropertyPage** ppPage)
{
    DbgLog((LOG_TRACE, 1, L"AssFilter::CreatePage()"));

    CheckPointer(ppPage, E_POINTER);
    HRESULT hr = S_OK;

    if (*ppPage != nullptr)
        return E_INVALIDARG;

    if (guid == __uuidof(CAssFilterSettingsProp))
        *ppPage = new CAssFilterSettingsProp(nullptr, &hr);
    else if (guid == __uuidof(CAssFilterStatusProp))
        *ppPage = new CAssFilterStatusProp(nullptr, &hr);
    else if (guid == __uuidof(CAssFilterAboutProp))
        *ppPage = new CAssFilterAboutProp(nullptr, &hr);

    if (SUCCEEDED(hr) && *ppPage)
    {
        (*ppPage)->AddRef();
        return S_OK;
    }
    else
    {
        SAFE_DELETE(*ppPage);
        return E_FAIL;
    }
}

STDMETHODIMP AssFilter::GetTrackInfo(const WCHAR **pTrackName, const char **pTrackLang, const char **pSubType)
{
    if (!m_pin || m_pin->IsConnected() == FALSE)
    {
        return E_UNEXPECTED;
    }

    if (pTrackName)
        *pTrackName = m_wsTrackName.c_str();

    if (pTrackLang)
        *pTrackLang = m_sTrackLang.c_str();

    if (pSubType)
        *pSubType = m_sSubType.c_str();

    return S_OK;
}

STDMETHODIMP AssFilter::GetConsumerInfo(const WCHAR **pName, const WCHAR **pVersion)
{
    CheckPointer(m_consumer, E_UNEXPECTED);
    if (!m_pin || m_pin->IsConnected() == FALSE)
    {
        return E_UNEXPECTED;
    }

    if (pName)
    {
        LPWSTR cName;
        int cChars;

        m_consumer->GetString("name", &cName, &cChars);
        *pName = cName;
        //LocalFree(cName);
    }

    if (pVersion)
    {
        LPWSTR cVersion;
        int cChars;

        m_consumer->GetString("version", &cVersion, &cChars);
        *pVersion = cVersion;
        //LocalFree(cVersion);
    }

    return S_OK;
}

HRESULT AssFilter::LoadDefaults()
{
    m_settings.TrayIcon = FALSE;
    m_settings.NativeSize = FALSE;

    m_settings.FontName = L"Arial";
    m_settings.FontSize = 50;
    m_settings.FontScaleX = 100;
    m_settings.FontScaleY = 100;
    m_settings.FontSpacing = 0;
    m_settings.FontBlur = 0;

    m_settings.FontOutline = 2;
    m_settings.FontShadow = 0;
    m_settings.LineAlignment = 2;
    m_settings.MarginLeft = 120;
    m_settings.MarginRight = 120;
    m_settings.MarginVertical = 20;
    m_settings.ColorPrimary = 0x00FFFFFF;
    m_settings.ColorSecondary = 0x00FFFF;
    m_settings.ColorOutline = 0;
    m_settings.ColorShadow = 0;
    m_settings.CustomRes = 0;

    m_settings.CustomTags = L"";

    return S_OK;
}

HRESULT AssFilter::ReadSettings(HKEY rootKey)
{
    HRESULT hr;
    DWORD dwVal;
    BOOL bFlag;
    std::wstring strVal;

    CRegistry reg = CRegistry(rootKey, ASSFILTER_REGISTRY_KEY, hr, TRUE);
    if (SUCCEEDED(hr))
    {
        bFlag = reg.ReadBOOL(L"TrayIcon", hr);
        if (SUCCEEDED(hr)) m_settings.TrayIcon = bFlag;

        bFlag = reg.ReadBOOL(L"NativeSize", hr);
        if (SUCCEEDED(hr)) m_settings.NativeSize = bFlag;

        strVal = reg.ReadString(L"FontName", hr);
        if (SUCCEEDED(hr)) m_settings.FontName = strVal;

        dwVal = reg.ReadDWORD(L"FontSize", hr);
        if (SUCCEEDED(hr)) m_settings.FontSize = dwVal;

        dwVal = reg.ReadDWORD(L"FontScaleX", hr);
        if (SUCCEEDED(hr)) m_settings.FontScaleX = dwVal;

        dwVal = reg.ReadDWORD(L"FontScaleY", hr);
        if (SUCCEEDED(hr)) m_settings.FontScaleY = dwVal;

        dwVal = reg.ReadDWORD(L"FontSpacing", hr);
        if (SUCCEEDED(hr)) m_settings.FontSpacing = dwVal;

        dwVal = reg.ReadDWORD(L"FontBlur", hr);
        if (SUCCEEDED(hr)) m_settings.FontBlur = dwVal;

        dwVal = reg.ReadDWORD(L"FontOutline", hr);
        if (SUCCEEDED(hr)) m_settings.FontOutline = dwVal;

        dwVal = reg.ReadDWORD(L"FontShadow", hr);
        if (SUCCEEDED(hr)) m_settings.FontShadow = dwVal;

        dwVal = reg.ReadDWORD(L"LineAlignment", hr);
        if (SUCCEEDED(hr)) m_settings.LineAlignment = dwVal;

        dwVal = reg.ReadDWORD(L"MarginLeft", hr);
        if (SUCCEEDED(hr)) m_settings.MarginLeft = dwVal;

        dwVal = reg.ReadDWORD(L"MarginRight", hr);
        if (SUCCEEDED(hr)) m_settings.MarginRight = dwVal;

        dwVal = reg.ReadDWORD(L"MarginVertical", hr);
        if (SUCCEEDED(hr)) m_settings.MarginVertical = dwVal;

        dwVal = reg.ReadDWORD(L"ColorPrimary", hr);
        if (SUCCEEDED(hr)) m_settings.ColorPrimary = dwVal;

        dwVal = reg.ReadDWORD(L"ColorSecondary", hr);
        if (SUCCEEDED(hr)) m_settings.ColorSecondary = dwVal;

        dwVal = reg.ReadDWORD(L"ColorOutline", hr);
        if (SUCCEEDED(hr)) m_settings.ColorOutline = dwVal;

        dwVal = reg.ReadDWORD(L"ColorShadow", hr);
        if (SUCCEEDED(hr)) m_settings.ColorShadow = dwVal;

        dwVal = reg.ReadDWORD(L"CustomRes", hr);
        if (SUCCEEDED(hr)) m_settings.CustomRes = dwVal;

        strVal = reg.ReadString(L"CustomTags", hr);
        if (SUCCEEDED(hr)) m_settings.CustomTags = strVal;
    }
    else
        SaveSettings();

    return S_OK;
}

HRESULT AssFilter::LoadSettings()
{
    LoadDefaults();

    ReadSettings(HKEY_CURRENT_USER);
    return S_OK;
}

HRESULT AssFilter::SaveSettings()
{
    HRESULT hr;
    CreateRegistryKey(HKEY_CURRENT_USER, ASSFILTER_REGISTRY_KEY);
    CRegistry reg = CRegistry(HKEY_CURRENT_USER, ASSFILTER_REGISTRY_KEY, hr);
    if (SUCCEEDED(hr))
    {
        reg.WriteBOOL(L"TrayIcon", m_settings.TrayIcon);
        reg.WriteBOOL(L"NativeSize", m_settings.NativeSize);
        reg.WriteString(L"FontName", m_settings.FontName.c_str());
        reg.WriteDWORD(L"FontSize", m_settings.FontSize);
        reg.WriteDWORD(L"FontScaleX", m_settings.FontScaleX);
        reg.WriteDWORD(L"FontScaleY", m_settings.FontScaleY);
        reg.WriteDWORD(L"FontSpacing", m_settings.FontSpacing);
        reg.WriteDWORD(L"FontBlur", m_settings.FontBlur);
        reg.WriteDWORD(L"FontOutline", m_settings.FontOutline);
        reg.WriteDWORD(L"FontShadow", m_settings.FontShadow);
        reg.WriteDWORD(L"LineAlignment", m_settings.LineAlignment);
        reg.WriteDWORD(L"MarginLeft", m_settings.MarginLeft);
        reg.WriteDWORD(L"MarginRight", m_settings.MarginRight);
        reg.WriteDWORD(L"MarginVertical", m_settings.MarginVertical);
        reg.WriteDWORD(L"ColorPrimary", m_settings.ColorPrimary);
        reg.WriteDWORD(L"ColorSecondary", m_settings.ColorSecondary);
        reg.WriteDWORD(L"ColorOutline", m_settings.ColorOutline);
        reg.WriteDWORD(L"ColorShadow", m_settings.ColorShadow);
        reg.WriteDWORD(L"CustomRes", m_settings.CustomRes);
        reg.WriteString(L"CustomTags", m_settings.CustomTags.c_str());
    }

    return S_OK;
}

void AssFilter::ASS_LibraryDeleter::operator()(ASS_Library* p)
{
    if (p) ass_library_done(p);
}

void AssFilter::ASS_RendererDeleter::operator()(ASS_Renderer* p)
{
    if (p) ass_renderer_done(p);
}

void AssFilter::ASS_TrackDeleter::operator()(ASS_Track* p)
{
    if (p) ass_free_track(p);
}
