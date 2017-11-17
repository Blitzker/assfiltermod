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
#include "AssDebug.h"
#include "AssFilter.h"
#include "AssFilterAutoLoader.h"
#include "registry.h"

AFAutoLoaderDummyInputPin::AFAutoLoaderDummyInputPin(AssFilterAutoLoader *pFilter, CCritSec *pLock, HRESULT *pResult, LPCWSTR pName)
    : CBaseInputPin(NAME("DummyInputPin"), pFilter, pLock, pResult, pName)
    , m_filter(pFilter)
{
}

HRESULT AFAutoLoaderDummyInputPin::CheckMediaType(const CMediaType* mt)
{
    return m_filter->CheckInput(mt);
}

AssFilterAutoLoader::AssFilterAutoLoader(LPUNKNOWN pUnk, HRESULT* pResult)
    : CBaseFilter(NAME("AssFilterAutoLoader"), pUnk, &m_pLock, __uuidof(AssFilterAutoLoader))
{
#ifdef DEBUG
    DbgSetModuleLevel(LOG_ERROR, 10);
    DbgSetModuleLevel(LOG_LOCKING, 10);
    DbgSetModuleLevel(LOG_TRACE, 10);
    DbgSetLogFileDesktop(L"AssFilterDbg.Log");
#endif

    m_pin = std::make_unique<AFAutoLoaderDummyInputPin>(this, &m_pLock, pResult, L"");
    m_loaded = false;

}

AssFilterAutoLoader::~AssFilterAutoLoader()
{
}

CUnknown* WINAPI AssFilterAutoLoader::CreateInstance(LPUNKNOWN pUnk, HRESULT* pResult)
{
    try
    {
        return new AssFilterAutoLoader(pUnk, pResult);
    }
    catch (std::bad_alloc&)
    {
        if (pResult)
            *pResult = E_OUTOFMEMORY;
    }

    return nullptr;
}

CBasePin* AssFilterAutoLoader::GetPin(int n)
{
    if (n == 0)
        return m_pin.get();

    return NULL;
}

int AssFilterAutoLoader::GetPinCount()
{
    return 1;
}

STDMETHODIMP AssFilterAutoLoader::JoinFilterGraph(IFilterGraph* pGraph, LPCWSTR pName)
{
    m_loaded = false;
    if(pGraph)
    {
        if (pName)
        {
            DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::JoinFilterGraph() -> %s joined the graph!", pName));
        }

        IEnumFiltersPtr pEnumFilters;
        if (SUCCEEDED(pGraph->EnumFilters(&pEnumFilters)))
        {
            for (IBaseFilterPtr pBaseFilter; pEnumFilters->Next(1, &pBaseFilter, 0) == S_OK; pBaseFilter = NULL)
            {
                //FILTER_INFO pInfo;
                //pBaseFilter->QueryFilterInfo(&pInfo);

                //DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::JoinFilterGraph() -> Filter name: %s", pInfo.achName));

                //if (pInfo.pGraph != NULL)
                //    pInfo.pGraph->Release();

                if (pBaseFilter != (IBaseFilterPtr)this)
                {
                    CLSID clsid;
                    pBaseFilter->GetClassID(&clsid);
                    if (clsid == __uuidof(AssFilterAutoLoader))
                    {
                        DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::JoinFilterGraph() -> AssFilterAutoLoader already in the graph"));

                        return E_FAIL;
                    }
                    if (clsid == __uuidof(AssFilter))
                    {
                        DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::JoinFilterGraph() -> AssFilter already in the graph"));

                        return E_FAIL;
                    }
                }
            }
        }
    }

    return __super::JoinFilterGraph(pGraph, pName);
}

STDMETHODIMP AssFilterAutoLoader::QueryFilterInfo(FILTER_INFO* pInfo)
{
    CheckPointer(pInfo, E_POINTER);
    ValidateReadWritePtr(pInfo, sizeof(FILTER_INFO));

    HRESULT hr = __super::QueryFilterInfo(pInfo);
    if (SUCCEEDED(hr))
    {
        wcscpy_s(pInfo->achName, _countof(pInfo->achName) - 1, L"AssFilterModAutoLoader");
    }
    return hr;
}

bool AssFilterAutoLoader::AutoLoad(IFilterGraph* pGraph)
{
    // Find subtitle pin (MEDIASUBTYPE_ASS or MEDIASUBTYPE_UTF8) on the graph splitter
    bool have_subtitle_pin = false;
    IEnumFiltersPtr pEnumFilters;
    if (SUCCEEDED(pGraph->EnumFilters(&pEnumFilters)))
    {
        DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::AutoLoad -> Succeeded EnumFilters"));
        for (IBaseFilterPtr pBaseFilter; pEnumFilters->Next(1, &pBaseFilter, 0) == S_OK; pBaseFilter = NULL)
        {
            IFileSourceFilterPtr pFSF;
            if (SUCCEEDED(pBaseFilter->QueryInterface(IID_PPV_ARGS(&pFSF))))
            {
                DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::AutoLoad -> Succeeded QueryInterface"));
                IEnumPinsPtr pEnumPins;
                if (SUCCEEDED(pBaseFilter->EnumPins(&pEnumPins)))
                {
                    DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::AutoLoad -> Succeeded EnumPins"));
                    for (IPinPtr pPin; pEnumPins->Next(1, &pPin, 0) == S_OK; pPin = NULL)
                    {
                        IEnumMediaTypesPtr pEnumMediaTypes;
                        if (SUCCEEDED(pPin->EnumMediaTypes(&pEnumMediaTypes)))
                        {
                            DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::AutoLoad -> Succeeded EnumMediaTypes"));
                            AM_MEDIA_TYPE* pMediaType = NULL;
                            for (; pEnumMediaTypes->Next(1, &pMediaType, NULL) == S_OK; DeleteMediaType(pMediaType), pMediaType = NULL)
                            {
                                if (pMediaType->majortype == MEDIATYPE_Subtitle && ((pMediaType->subtype == MEDIASUBTYPE_ASS) ||
                                    (pMediaType->subtype == MEDIASUBTYPE_UTF8) || (pMediaType->subtype == MEDIASUBTYPE_VOBSUB) ||
                                    (pMediaType->subtype == MEDIASUBTYPE_HDMVSUB)))
                                {
                                    DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::AutoLoad -> Found subtitle pin on source filter"));

                                    have_subtitle_pin = true;
                                    break;
                                }
                            }
                            if (pMediaType)
                                DeleteMediaType(pMediaType);
                        }
                        if (have_subtitle_pin)
                            break;
                    }
                }
            }
            if (have_subtitle_pin)
                break;
        }
    }

    return !have_subtitle_pin;
}

bool AssFilterAutoLoader::DisableAutoLoad()
{
    HRESULT hr;
    BOOL bFlag;

    CRegistry reg = CRegistry(HKEY_CURRENT_USER, ASSFILTER_REGISTRY_KEY, hr, TRUE);
    if (SUCCEEDED(hr))
    {
        bFlag = reg.ReadBOOL(L"DisableAutoLoad", hr);
        if (!SUCCEEDED(hr))
            return false;
    }

    return bFlag ? true : false;
}

HRESULT AssFilterAutoLoader::CheckInput(const CMediaType* mt)
{
    HRESULT hr = NOERROR;
    if (!m_loaded)
    {
        m_loaded = true;
#ifdef DEBUG
        if (mt->majortype==MEDIATYPE_Video)
        {
            DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::CheckInput() -> MEDIATYPE_Video"));
        }
        else if (mt->majortype==MEDIATYPE_Audio)
        {
            DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::CheckInput() -> MEDIATYPE_Audio"));
        }
        else if (mt->majortype==MEDIATYPE_Subtitle)
        {
            DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::CheckInput() -> MEDIATYPE_Subtitle"));
        }
        else
        {
            DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::CheckInput() -> Other MEDIATYPE"));
        }
#endif // DEBUG

        if (mt->majortype == MEDIATYPE_Audio ||
            mt->majortype == MEDIATYPE_Subtitle ||
            mt->majortype == MEDIATYPE_Video)
        {
            if (AutoLoad(m_pGraph) && !DisableAutoLoad())
            {
                DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::CheckInput() -> Autoload"));

                IBaseFilterPtr filter;
                hr = CoCreateInstance(__uuidof(AssFilter), NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&filter));
                if (FAILED(hr))
                {
                    DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::CheckInput -> Failed to create AssFilterMod."));

                    return E_FAIL;
                }
                hr = m_pGraph->AddFilter(filter, L"AssFilterMod(AutoLoad)");
                if (FAILED(hr))
                {
                    DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::CheckInput -> Failed to AddFilter."));

                    return E_FAIL;
                }
                if (mt->majortype == MEDIATYPE_Subtitle)
                {
                    IGraphConfigPtr graph;
                    if (SUCCEEDED(filter->QueryInterface(IID_PPV_ARGS(&graph))))
                    {
                        hr = graph->AddFilterToCache(filter);
                        if (FAILED(hr))
                        {
                            DbgLog((LOG_TRACE, 1, L"AssFilterAutoLoader::CheckInput -> Failed to add filter to cache."));

                            return E_FAIL;
                        }
                    }
                }
            }
        }
    }
    return E_FAIL;
}
