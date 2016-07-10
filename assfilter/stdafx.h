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

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <initguid.h>

#include <comdef.h>
#include <Commctrl.h>

#include <streams.h>

#include <cassert>
#include <map>
#include <memory>
#include <string>

#include "SubRenderIntf.h"

#define ASSFILTER_REGISTRY_KEY L"Software\\AssFilter"

#define SAFE_DELETE(p)      if(p) { delete p; p = nullptr; }

_COM_SMARTPTR_TYPEDEF(IEnumFilters, __uuidof(IEnumFilters));
_COM_SMARTPTR_TYPEDEF(IBaseFilter, __uuidof(IBaseFilter));
_COM_SMARTPTR_TYPEDEF(ISubRenderProvider, __uuidof(ISubRenderProvider));
_COM_SMARTPTR_TYPEDEF(ISubRenderConsumer2, __uuidof(ISubRenderConsumer2));
_COM_SMARTPTR_TYPEDEF(ISubRenderFrame, __uuidof(ISubRenderFrame));
_COM_SMARTPTR_TYPEDEF(IAMGraphStreams, __uuidof(IAMGraphStreams));

// {00000000-0000-0000-0000-000000000000}
DEFINE_GUID(GUID_NULL, 0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

// {E487EB08-6B26-4be9-9DD3-993434D313FD}
DEFINE_GUID(MEDIATYPE_Subtitle, 0xe487eb08, 0x6b26, 0x4be9, 0x9d, 0xd3, 0x99, 0x34, 0x34, 0xd3, 0x13, 0xfd);

// {87C0B230-03A8-4fdf-8010-B27A5848200D}
DEFINE_GUID(MEDIASUBTYPE_UTF8, 0x87c0b230, 0x3a8, 0x4fdf, 0x80, 0x10, 0xb2, 0x7a, 0x58, 0x48, 0x20, 0xd);

// {326444F7-686F-47ff-A4B2-C8C96307B4C2}
DEFINE_GUID(MEDIASUBTYPE_ASS, 0x326444f7, 0x686f, 0x47ff, 0xa4, 0xb2, 0xc8, 0xc9, 0x63, 0x7, 0xb4, 0xc2);

// {A33D2F7D-96BC-4337-B23B-A8B9FBC295E9}
DEFINE_GUID(FORMAT_SubtitleInfo, 0xa33d2f7d, 0x96bc, 0x4337, 0xb2, 0x3b, 0xa8, 0xb9, 0xfb, 0xc2, 0x95, 0xe9);

#define ReturnIfFailed(x) { HRESULT hr = (x); if (FAILED(hr)) return hr; }
#define ReturnIfNotEquals(r, x) { HRESULT hr = (x); if (hr != r) return hr; }

interface __declspec(uuid("EBAFBCBE-BDE0-489A-9789-05D5692E3A93"))
IDSMResourceBag : public IUnknown {
    STDMETHOD_(DWORD, ResGetCount)() PURE;
    STDMETHOD(ResGet)(DWORD iIndex, BSTR* ppName, BSTR* ppDesc, BSTR* ppMime, BYTE** ppData, DWORD* pDataLen, DWORD_PTR* pTag) PURE;
    STDMETHOD(ResSet)(DWORD iIndex, LPCWSTR pName, LPCWSTR pDesc, LPCWSTR pMime, const BYTE* pData, DWORD len, DWORD_PTR tag) PURE;
    STDMETHOD(ResAppend)(LPCWSTR pName, LPCWSTR pDesc, LPCWSTR pMime, BYTE* pData, DWORD len, DWORD_PTR tag) PURE;
    STDMETHOD(ResRemoveAt)(DWORD iIndex) PURE;
    STDMETHOD(ResRemoveAll)(DWORD_PTR tag) PURE;
};
_COM_SMARTPTR_TYPEDEF(IDSMResourceBag, __uuidof(IDSMResourceBag));
