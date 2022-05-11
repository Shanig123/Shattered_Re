#pragma once

#ifndef __MyRenderTarget_H__
#define __MyRenderTarget_H__

#include "Base.h"

BEGIN(Engine)

class CMyRenderTarget final : public CBase
{
private:
	explicit CMyRenderTarget(_DEVICE _pDevice);
	virtual ~CMyRenderTarget() = default;

public:
	HRESULT Ready_MyRenderTarget(const _uint& _iWidth, const _uint& _iHeight, D3DFORMAT _Format, D3DXCOLOR _Color);
	void Clear_MyRenderTarget();

public:
	void SetUp_OnGraghicDevice(const _uint& _iIndex);
	void Release_OnGraghicDevice(const _uint& _iIndex);

public:
	HRESULT Ready_DebugBuffer(const _float& _fX, const _float& _fY, const _float& _fSizeX, const _float& _fSizeY);
	void Render_DebugBuffer();

public:
	void SetUp_OnShader(_EFFECT& _pEffect, const char* _pConstantTable);

public:
	static CMyRenderTarget* Create(_DEVICE _pDevice, const _uint& _iWidth, const _uint& _iHeight, D3DFORMAT _Format, D3DXCOLOR _Color);
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

#endif // !__MyRenderTarget_H__