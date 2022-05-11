#pragma once
#ifndef __PARTICLEMANAGER_H__
#define __PARTICLEMANAGER_H__

#include "Base.h"
#include "Particle_Base.h"
#include "TrailEffect.h"

BEGIN(Engine)
typedef  CParticle_Base* LPCParticle_Base;
class ENGINE_DLL CParticle_Manager final : public CBase
{
	DECLARE_SINGLETON(CParticle_Manager)
private:
	explicit CParticle_Manager();
	virtual ~CParticle_Manager() = default;

public:
	HRESULT Ready_ParticleManager(_DEVICE _pDevice);
	_uint Update_ParticleManager(const _float& _fDeltaTime);
	_uint LateUpdate_ParticleManager(const _float& _fDeltaTime);
	void Clear_ForScene();

public:
	HRESULT Generate_Particle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		_float _fRotateAxisRadian = (D3DX_PI)
	);
	HRESULT Generate_Particle(const _uint& _iSceneIdx,
		_float3 _arrCreatePos[],
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		_float _fRotateAxisRadian = (D3DX_PI));
	HRESULT Generate_ExplosionParticle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		_float3* _pTargetPos,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		_float _fRotateAxisRadian = (0.f),
		_float _fSpeed = 0.f,
		EAxis _eAxisOption = EAxis::End);
	HRESULT Generate_SetRadian_ExplosionParticle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		_float3* _pTargetPos,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		const _float3& _vRotateAxisRadian = { 0.f,0.f,0.f },
		_float _fSpeed = 0.f);
	HRESULT Generate_ItemParticle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId,
		_float _fSpeed = 0.f);
	HRESULT Generate_Decal(const _uint& _iSceneIdx,
		const TRANSFORM_DESC& _tCreateInfo,
		const _float4& _vArgb,
		_uint _iTextureDrawId = 0,
		const ePartilcePatternType& _eParticleMovePattern = (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlinkPow | (_ulong)ePartilcePatternType::GoStraight | (_ulong)ePartilcePatternType::ScaleUp | (_ulong)ePartilcePatternType::x3),
		_float _fLifeTime = 1.6f);
	HRESULT Generate_SphereParticle(const _uint& _iSceneIdx,
		const _float3& _vCreatePos,
		const _float& _fCreateRange,
		_float3* _pTargetPos,
		const _ubyte& _iCreateCount,
		const _float4& _vColor,
		const ePartilcePatternType& _eParticleMovePattern,
		_uint _iTextureDrawId = 0,
		_float _fLifeTime = 4.f,
		_float _fRotateAxisRadian = (D3DX_PI),
		_float _fSpeed = 0.f);
public:
	HRESULT Set_Texture(_EFFECT& _pEffect,
		const char* _pConstantTable,
		_uint _iIndex = 0);
	HRESULT Render_VIBuffer(_EFFECT* _pEffect);

private:
	HRESULT Return_Particle(LPCParticle_Base _pParticle);

private:
	_DEVICE m_pDevice;

	class	CTexture*	m_pTexture;
	class CVIBuffer_RectTexture*	m_pVIBuffer;

	list<LPCParticle_Base> m_listUpdateParticle;
	list<LPCParticle_Base> m_listParticlePool;

public:
	virtual void Free() override;

};
END
#endif // !__PARTICLEMANAGER_H__

