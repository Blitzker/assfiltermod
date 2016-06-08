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

class AssFilter;

class AssPin final
    : public CCritSec
    , public CBaseInputPin
{
public:

    AssPin(AssFilter* pFilter, HRESULT* pResult);

    DECLARE_IUNKNOWN;

    // CBasePin
    HRESULT CheckMediaType(const CMediaType* pmt) override;
    HRESULT SetMediaType(const CMediaType* pmt) override;

    // IMemInputPin
    STDMETHODIMP Receive(IMediaSample* pSample) override;

private:

    AssFilter* const m_pAssFilter;
};
