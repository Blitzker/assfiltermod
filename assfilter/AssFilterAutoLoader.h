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

class AssFilterAutoLoader;

class AFAutoLoaderDummyInputPin: public CBaseInputPin
{
public:
    AFAutoLoaderDummyInputPin(
        AssFilterAutoLoader* pFilter,
        CCritSec* pLock,
        HRESULT* phr,
        LPCWSTR pName);

    HRESULT CheckMediaType(const CMediaType*) override;

private:
    AssFilterAutoLoader* const m_filter;
};

// {8A6DFC6A-0A79-4790-85DA-0688B8093B54}
class __declspec(uuid("8A6DFC6A-0A79-4790-85DA-0688B8093B54"))
AssFilterAutoLoader : public CBaseFilter
{
public:
    AssFilterAutoLoader(LPUNKNOWN pUnk, HRESULT* pResult);
    virtual ~AssFilterAutoLoader();

    DECLARE_IUNKNOWN;

    static CUnknown* WINAPI CreateInstance(LPUNKNOWN pUnk, HRESULT* pResult);

    //CBaseFilter
    CBasePin* GetPin(int n) override;
    int GetPinCount() override;
    STDMETHODIMP JoinFilterGraph(IFilterGraph* pGraph, LPCWSTR pName) override;
    STDMETHODIMP QueryFilterInfo(FILTER_INFO* pInfo) override;

    bool AutoLoad(IFilterGraph* pGraph);
    bool DisableAutoLoad();
    HRESULT CheckInput(const CMediaType* mt);

private:
    std::unique_ptr<AFAutoLoaderDummyInputPin>  m_pin;

    CCritSec    m_pLock;
    bool        m_loaded;
};
