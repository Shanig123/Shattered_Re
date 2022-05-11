#pragma once

#ifndef __LIGHT_MANAGER_H__
#define __LIGHT_MANAGER_H__

#include "Base.h"
#include "Light.h"
BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)

private:
	explicit CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	HRESULT Ready_Light(_DEVICE _pDevice, const D3DLIGHT9* _pLightInfo, const _uint& _iIndex);
	_uint Update_Light(const _float& _fDeltaTime);
	void Render_Light(LPD3DXEFFECT& _pEffect);
	void Clear_ForScene();

public:
	void Set_LightPower(const _float& _fLightPower, const _uint& _iIndex = 0);
	void Set_LightInfo(const D3DLIGHT9& _tLightInfo, const _float& _fLightPower, const _uint& _iIndex = 0);
	void Set_LightCamDesc(const CAMERA_DESC& _tCamDesc, const _uint& _iIndex = 0);
	void Set_CubeTexture(class CTexture* _pCubeTexture) { m_pCubeTexture = _pCubeTexture; }
	void Set_FogTexture(class CTexture* _pCubeTexture) { m_pFogTexture = _pCubeTexture; }
	HRESULT Set_Texture(_EFFECT& _pEffect, const char* _pConstantTable, const _uint& _iIndex = 0);
	HRESULT Set_FogTexture(_EFFECT& _pEffect, const char* _pConstantTable, const _uint& _iIndex = 0);
	void Set_SkyDir(const _float3& _vDir) { m_vSkyReflectDir = _vDir; }
	void Set_FogOnOff() { m_bFogOnOff = !m_bFogOnOff; }
public:
	const D3DLIGHT9* Get_Light(const _uint& _iIndex = 0);
	const _float3&	Get_SkyDir()const { return m_vSkyReflectDir; }
	const _float4x4* Get_LightView(const _uint& _iIndex = 0);
	const _float4x4* Get_LightProj(const _uint& _iIndex = 0);
	const CAMERA_DESC* Get_LightCamDesc(const _uint& _iIndex = 0)const;
	const _bool& Get_FogOnOff()const { return m_bFogOnOff; }
public:
	virtual void Free() override;

private:
	list<CLight*>		m_listLight;
	class CTexture*		m_pCubeTexture;
	class CTexture*		m_pFogTexture;
	_bool				m_bFogOnOff;
	_float3				m_vSkyReflectDir;
};

END

#endif // !__LIGHT_MANAGER_H__