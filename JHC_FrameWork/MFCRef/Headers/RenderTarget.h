#pragma once
#ifndef _AFXDLL

#ifndef __RANDERTARGET_H__
#define __RANDERTARGET_H__

#include "Base.h"

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	explicit CRenderTarget(_DEVICE _pDevice);
	virtual ~CRenderTarget() = default;

public:
	HRESULT Ready_RenderTarget(const _uint& _iWidth, const _uint& _iHeight, D3DFORMAT _Format, D3DXCOLOR _Color);
	void Clear_RenderTarget();

public:
	void SetUp_OnGraghicDevice(const _uint& _iIndex);
	void Release_OnGraghicDevice(const _uint& _iIndex);

public:
	HRESULT Ready_DebugBuffer(const _float& _fX, const _float& _fY, const _float& _fSizeX, const _float& _fSizeY);
	void Render_DebugBuffer();

public:
	void SetUp_OnShader(_EFFECT& _pEffect, const char* _pConstantTable);

public:
	static CRenderTarget* Create(_DEVICE _pDevice, const _uint& _iWidth, const _uint& _iHeight, D3DFORMAT _Format, D3DXCOLOR _Color);
	virtual void Free() override;

private:
	_DEVICE				m_pDevice;
	_TEXTURE9			m_pTargetTexture;
	_SURFACE9			m_pTargetSurface;
	_SURFACE9			m_pOldTargetSurface;
	D3DXCOLOR			m_ClearColor;

	_VertexBuffer		m_pVB;
	_IndexBuffer		m_pIB;
};

END

#endif // !__RANDERTARGET_H__

#endif // !_AFXDLL
