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
#include "AssPin.h"

#include "AssFilter.h"

AssPin::AssPin(AssFilter* pFilter, HRESULT* pResult)
    : CBaseInputPin(NAME("AssPin"), pFilter, this, pResult, L"Input")
    , m_pAssFilter(pFilter)
{
    DbgLog((LOG_TRACE, 1, L"AssPin::AssPin -> Pin created!"));
}

HRESULT AssPin::CheckMediaType(const CMediaType* pmt)
{
    CheckPointer(pmt, E_POINTER);

    CAutoLock lock(this);

    // Check if the request is for ASS or SSA
    if (pmt->majortype == MEDIATYPE_Subtitle &&
        (pmt->subtype == MEDIASUBTYPE_ASS || pmt->subtype == MEDIASUBTYPE_SSA) &&
        pmt->formattype == FORMAT_SubtitleInfo)
    {
        DbgLog((LOG_TRACE, 1, L"AssPin::CheckMediaType -> MEDIASUBTYPE_ASS_SSA"));
        return S_OK;
    }

    // Check if the request is for SRT
    if (pmt->majortype == MEDIATYPE_Subtitle &&
        pmt->subtype == MEDIASUBTYPE_UTF8 &&
        pmt->formattype == FORMAT_SubtitleInfo)
    {
        DbgLog((LOG_TRACE, 1, L"AssPin::CheckMediaType -> MEDIASUBTYPE_UTF8"));
        return S_OK;
    }

    // Check if the request is for VOBSUB or HDMVSUB
    if (pmt->majortype == MEDIATYPE_Subtitle &&
        (pmt->subtype == MEDIASUBTYPE_VOBSUB || pmt->subtype == MEDIASUBTYPE_HDMVSUB) &&
        pmt->formattype == FORMAT_SubtitleInfo)
    {
        DbgLog((LOG_TRACE, 1, L"AssPin::CheckMediaType -> MEDIASUBTYPE_VOBSUB_HDMVSUB"));
        return S_OK;
    }

    DbgLog((LOG_TRACE, 1, L"AssPin::CheckMediaType -> Unsupported MEDIATYPE"));

    return S_FALSE;
}

HRESULT AssPin::SetMediaType(const CMediaType* pmt)
{
    assert(CritCheckIn(this));

    ReturnIfFailed(CBaseInputPin::SetMediaType(pmt));

    m_pAssFilter->SetMediaType(*pmt, m_Connected);

    return S_OK;
}

STDMETHODIMP AssPin::Receive(IMediaSample* pSample)
{
    ReturnIfNotEquals(S_OK, CBaseInputPin::Receive(pSample));

    m_pAssFilter->Receive(pSample, m_tStart);

    return S_OK;
}
