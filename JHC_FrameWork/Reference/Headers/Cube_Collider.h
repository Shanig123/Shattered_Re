#pragma once

#ifndef __CUBE_COLLIDER_H__
#define __CUBE_COLLIDER_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CCube_Collider final : public CComponent
{
private:
	explicit CCube_Collider(_DEVICE _pDevice);
	explicit CCube_Collider(const CCube_Collider& _rOther);
	virtual ~CCube_Collider() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void * _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;
	virtual HRESULT Render_Collider(EColliderType _eType, _float4x4* _pColliderMatrix = nullptr);

public:
	_uint Update_Transform();
	HRESULT Set_Transform();

public:
	const _float4x4* Get_WorldMatrix() { return &m_CubeColliderDesc.matWorld; }
	const _float3* Get_Min() { return &m_vMin; }
	const _float3* Get_Max() { return &m_vMax; }

public:
	void Set_ParentMatrix(const _float4x4* _pParentMatrix);

public:
	static CCube_Collider* Create(_DEVICE _pDevice);
	virtual CComponent * Clone(void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	CUBECOLLIDER_DESC		m_CubeColliderDesc;
	_TEXTURE9				m_pTexture[(_uint)EColliderType::End];
	_VertexBuffer			m_pVB;
	_IndexBuffer			m_pIB;

	_float3					m_vMin;
	_float3					m_vMax;
};

END

#endif // !__CUBE_COLLIDER_H__