#pragma once

#ifndef __SPHERE_COLLIDER_H__
#define __SPHERE_COLLIDER_H__

#include "Component.h"
#include "Camera.h"

BEGIN(Engine)

class ENGINE_DLL CSphere_Collider final : public CComponent
{
private:
	explicit CSphere_Collider(_DEVICE _pDevice, _float _fRadius, _uint _iSlices, _uint _iStacks);
	explicit CSphere_Collider(const CSphere_Collider& _rOther);
	virtual ~CSphere_Collider() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void * _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;
	virtual HRESULT Render_Collider(EColliderType _eType, _float4x4* _pColliderMatrix = nullptr);
	virtual HRESULT Render_Collider(EColliderType _eType, _EFFECT& _pEffect);

public:
	HRESULT Set_Transform();
	_bool Sphere_Picking(HWND _hWnd, _uint _iWinCX, _uint _iWinCY, class CCamera * _pCamera, const _float4x4 * _pMatWorld);

public:
	TCHAR* Get_BoneName() { return m_SphereColliderDesc.szBoneName; }
	_float3& Get_Pos() { return m_SphereColliderDesc.vPos; }
	_float3& Get_Scale() { return m_SphereColliderDesc.vScale; }
	_float& Get_Radius() { return m_fRadius; }
	_float4x4& Get_WorldMatrix() { return m_SphereColliderDesc.matWorld; }

public:
	void Set_Desc(SPHERECOLLIDER_DESC& _Desc) { m_SphereColliderDesc = _Desc; }
	void Set_Pos(const _float3& _vPos) { m_SphereColliderDesc.vPos = _vPos; }
	void Set_Scale(const _float3& _vScale) { m_SphereColliderDesc.vScale = _vScale; }
	void Set_ParentMatrix(const _float4x4* _pParentMatrix);

public:
	static CSphere_Collider* Create(_DEVICE _pDevice, _float _fRadius, _uint _iSlices, _uint _iStacks);
	virtual CComponent * Clone(void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	SPHERECOLLIDER_DESC			m_SphereColliderDesc;
	_MESH						m_pMesh;
	_TEXTURE9					m_pTexture[(_uint)EColliderType::End];

	_float						m_fRadius;
	_uint						m_iSlices;
	_uint						m_iStacks;
};

END

#endif // !__SPHERE_COLLIDER_H__