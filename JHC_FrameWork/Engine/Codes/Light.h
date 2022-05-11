#pragma once

#ifndef __LIGHT_H__
#define __LIGHT_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLight final : public CBase
{
	NO_COPY(CLight)
private:
	explicit CLight(_DEVICE _pDevice);
	virtual ~CLight() = default;

private:
	HRESULT Ready_Light(const D3DLIGHT9* _pLightInfo, const _uint& _iIndex);
public:
	_uint	Update_Light(const _float& _fDeltaTime);
	void Render_Light(LPD3DXEFFECT& _pEffect);

public:
	void Set_LightPower(const _float& _fLightPower) { m_fLightPower = _fLightPower; }
	void Set_LightInfo(const D3DLIGHT9& _tLightInfo, const _float& _fLightPower) { m_tLightInfo = _tLightInfo; m_fLightPower = _fLightPower; }
	void Set_LightCamDesc(const CAMERA_DESC& _tCamDesc) {m_tLightCamDesc = _tCamDesc; }
public:
	const D3DLIGHT9* Get_Light() const { return &m_tLightInfo; }
	const _float4x4& Get_LightView()const { return m_matLightView; }
	const _float4x4& Get_LightProj()const { return m_matLightProj; }
	const CAMERA_DESC& Get_LightCamDesc()const { return m_tLightCamDesc; }
public:
	static CLight* Create(_DEVICE _pDevice, const D3DLIGHT9* _pLightInfo, const _uint& _iIndex);
	virtual void Free() override;

private:
	_DEVICE			m_pDevice;
	CAMERA_DESC		m_tLightCamDesc;
	D3DLIGHT9		m_tLightInfo;
	_float			m_fLightPower;
	_float			m_fLightAccTime;
	_float4x4		m_matLightView;
	_float4x4		m_matLightProj;
	_uint			m_iIndex;
	_VertexBuffer	m_pVB;
	_IndexBuffer	m_pIB;
};

END

#endif // !__LIGHT_H__