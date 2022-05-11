#pragma once

#ifndef __FRUSTUM_H__
#define __FRUSTUM_H__

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CFrustum final : public CBase
{
private:
	explicit CFrustum(_DEVICE _pDevice);
	explicit CFrustum(const CFrustum& _rOther);
	virtual ~CFrustum() = default;

public:
	HRESULT	Ready_Frustum();

public:
	void ConstructFrustum(_float _fFar);
	_bool CheckCube_Frustum(const _float3& _vMin, const _float3& _vMax, const _float4x4& _matWorld);
	_bool CheckSphere_Frustum(const _float& _fCenterX, const _float& _fCenterY, const _float& _fCenterZ, _float _fRadius);

public:
	static CFrustum* Create(_DEVICE _pDevice);
	virtual void Free() override;

private:
	_DEVICE			m_pDevice;
	D3DXPLANE		m_vPlanes[6];
};

END

#endif // !__FRUSTUM_H__