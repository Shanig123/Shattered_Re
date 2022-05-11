#pragma once

#ifndef __RENDERTARGET_MANAGER_H__
#define __RENDERTARGET_MANAGER_H__

#include "Base.h"
#include "MyRenderTarget.h"

BEGIN(Engine)

class ENGINE_DLL CRenderTarget_Manager final : public CBase
{
	DECLARE_SINGLETON(CRenderTarget_Manager)

private:
	explicit CRenderTarget_Manager();
	virtual ~CRenderTarget_Manager() = default;

public:
	HRESULT Ready_RenderTarget_Manager(const wstring& _pTargetTag, _DEVICE _pDevice, const _uint& _iWidth, const _uint& _iHeight, D3DFORMAT _Format, D3DXCOLOR _Color);

public:
	HRESULT Ready_MRT(const wstring& _pMRTTag, const wstring& _pTargetTag);
	HRESULT Begin_MRT(const wstring& _pMRTTag);
	HRESULT End_MRT(const wstring& _pMRTTag);

public:
	HRESULT Ready_DebugBuffer(const wstring& _pTargetTag, const _float& _fX, const _float& _fY, const _float& _fSizeX, const _float& _fSizeY);
	void Render_DebugBuffer(const wstring& _pMRTTag);
	void SetUp_OnShader(_EFFECT& _pEffect, const wstring& _pTargetTag, const char* _pConstantTable);

private:
	CMyRenderTarget* Find_RenderTarget(const wstring& _pTargetTag);
	list<CMyRenderTarget*>* Find_MRT(const wstring& _pMRTTag);

public:
	virtual void Free() override;

private:
	map<const wstring, CMyRenderTarget*>			m_mapRenderTarget;
	map<const wstring, list<CMyRenderTarget*>>		m_mapMRT;
};

END

#endif // !__RENDERTARGET_MANAGER_H__