#pragma once

#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
private:
	explicit CTransform(_DEVICE _pDevice);
	explicit CTransform(const CTransform& _other);
	virtual ~CTransform() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void * _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;

public:
	_uint Update_Transform();
	HRESULT Set_Transform();

public:
	const TRANSFORM_DESC& Get_TransformDesc() const;
	const _float4x4* Get_WorldMatrix() ;
	void Get_Info(EInfo _eType, _float3* _vInfo) const;
	//const _float4x4 Get_WorldMatrixAfterUpdate();
public:
	void Set_TransformDesc(TRANSFORM_DESC& _rDesc) { m_TransformDesc = _rDesc; }
	void Set_WorldMatrix(const _float4x4* _matWorld) { m_TransformDesc.matWorld = *_matWorld; }
	void Set_RotateMatrix(const _float4x4* _matRotate) { m_TransformDesc.matRotate = *_matRotate; }
	void Set_Pos(const _float3& _vPos) { m_TransformDesc.vPos = _vPos; }
	void Set_Scale(const _float3& _vScale) { m_TransformDesc.vScale = _vScale; }
	void Set_Rotate(EAxis _eType, const _float & _fAngle);
	void Set_Rotate(const _float3& _vRadianAngle);
	void Set_Revolution(EAxis _eType, const _float & _fAngle);
	void Set_Parent(const _float3& _vParent) { m_TransformDesc.vParent = _vParent; }
	void Set_ParentMatrix(const _float4x4* _pParent);
	void Set_MoveSpeed(const _float _fMoveSpeed) { m_TransformDesc.fSpeedPerSecond = _fMoveSpeed; }
	void Set_RevolutionSpeed(const _float _fRevSpeed) { m_TransformDesc.fRevPerSecond = _fRevSpeed; }

	void Set_Info(EInfo _eType, const _float3& _vInfo);
public:
	void Go_Straight(const _float& _fSpeed, const _float& _fDeltaTime);
	void Go_Side(const _float& _fSpeed, const _float& _fDeltaTime);
	void Go_Up(const _float& _fSpeed, const _float& _fDeltaTime);
	void Move_Pos(const _float3* _pDir, const _float& _fSpeed, const _float& _fDeltaTime);
	void Rotate(EAxis _eType, const _float & _fAngle, const _float& _fSpeed = 1.f, const _float& _fDeltaTime = 1.f);
	void Revolution(EAxis _eType, const _float & _fAngle, const _float& _fSpeed = 1.f, const _float& _fDeltaTime = 1.f);
	void Chase_Target(const _float3& _vTargetPos, const _float& _fSpeed, const _float& _fDeltaTime);

private:
	_float4x4* Compute_LookAtTarget(const _float3& pTargetPos);

public:
	static CTransform* Create(_DEVICE _pDevice);
	virtual CComponent * Clone(void * _pArg = nullptr) override;
	virtual void Free() override;

private:
	TRANSFORM_DESC	m_TransformDesc;
	//test
};

END

#endif // !__TRANSFORM_H__