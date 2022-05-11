#pragma once

#ifndef __OPTIMIZATION_H__
#define __OPTIMIZATION_H__

#include "Component.h"
#include "Frustum.h"

BEGIN(Engine)

class ENGINE_DLL COptimization final : public CComponent
{
private:
	explicit COptimization(_DEVICE _pDevice);
	explicit COptimization(const COptimization& _rOther);
	virtual ~COptimization() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void* _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;

public:
	void ConstructFrustum(_float _fFar);
	_bool CheckCube_Frustum(const _float3& _vMin, const _float3& _vMax, const _float4x4& _matWorld);
	_bool CheckSphere_Frustum(const _float& _fCenterX, const _float& _fCenterY, const _float& _fCenterZ, _float _fRadius);

public:
	static COptimization* Create(_DEVICE _pDevice);
	virtual CComponent* Clone(void* _pArg = nullptr) override;
	virtual void Free() override;

private:
	CFrustum*		m_pFrustum;
};

END

#endif // !__OPTIMIZATION_H__