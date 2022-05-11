#pragma once

#ifndef __CALCUlATOR_H__
#define __CALCUlATOR_H__

#include "Component.h"

BEGIN(Engine)
class CTransform;
class ENGINE_DLL CCalculator final : public CComponent
{
private:
	typedef	struct tagOBB
	{
		_float3		vPoint[8];
		_float3		vCenter;
		_float3		vProjAxis[3];	// 중점에서 각 면으로 뻗어 나가는 방향 벡터
		_float3		vAxis[3];		// 박스에 평행한 축 벡터
	}OBB;

private:
	explicit CCalculator(_DEVICE _pDevice);
	explicit CCalculator(const CCalculator& _rOther);
	virtual ~CCalculator() = default;

public:
	virtual HRESULT Ready_Component_Prototype() override;
	virtual HRESULT Ready_Component_Clone(void * _pArg = nullptr) override;
	virtual _uint Update_Component(const _float& _fDeltaTime) override;

public:
	_bool Collision_AABB
	(
		const _float3* _rDestMin, const _float3* _rDestMax, const _float4x4* _rDestWorld,
		const _float3* _rSrcMin, const _float3* _rSrctMax, const _float4x4* _rSrcWorld
	);
	_bool Collision_OBB
	(
		const _float3* _rDestMin, const _float3* _rDestMax, const _float4x4* _rDestWorld,
		const _float3* _rSrcMin, const _float3* _rSrctMax, const _float4x4* _rSrcWorld
	);
	_bool Collision_Spher(_float4x4& _vStartWorld, _float _fStartRadius, _float4x4& _vEndWorld, _float _fEndRadius);
	_bool Picking_OnMesh(HWND hWnd, class CStaticMesh* pMesh, CTransform* pTransformCom,
						_float3* vOutResult = nullptr, _float3* vObjectPos = nullptr);//제네릭으로 픽킹 외에도 인자에 맞는 변수를 넣어서 위치를 받을 수 있음.

private:
	void Set_Point(OBB* pObb, const _float3* pMin, const _float3* pMax);
	void Set_Axis(OBB* pObb);

public:
	static CCalculator* Create(_DEVICE _pDevice);
	virtual CComponent * Clone(void * _pArg = nullptr) override;
	virtual void Free() override;
};

END

#endif // !__CALCUlATOR_H__