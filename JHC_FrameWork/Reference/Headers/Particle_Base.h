#pragma once
#ifndef __PARTICLE_BASE_H__
#define __PARTICLE_BASE_H__

#include "GameObject.h"
#include "ParticlePattern.h"

BEGIN(Engine)

//기본 파티클오브젝트의 베이스클래스입니다.
class ENGINE_DLL CParticle_Base : public CGameObject
{
public:
	typedef void(*PATTERN_FUNC) (void);
	//typedef PATTERN_FUNC (void*);


protected:
	explicit CParticle_Base(_DEVICE _pDevice);
	explicit CParticle_Base(const CParticle_Base& _rhs);
	virtual ~CParticle_Base() = default;

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;

public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:
	void Reset_Data();
	void Set_ParticleInfo(const PARTICLE_DESC& _tParticleInfo,
		_float3* _pTargetPos,
		const _ubyte& _ubyIndex,
		const _ubyte& _ubyCreateCount,
		const _float& _fLifeTime,
		const _uint& _iRenderID,
		const ePartilcePatternType& _eParticleMovePattern);
	void Set_AccTime(const _float& _fAccTime) { m_fThisAccTime = _fAccTime; };
	void Set_SceneIndex(const _uint& _iSceneIdx);

protected:
	virtual HRESULT AddCom();
	virtual HRESULT SetUp_ConstantTable(_EFFECT& _pEffect,const _ubyte& _ubyIndex);

protected:
	//Info
	PARTICLE_DESC	m_arrParticleDesc[255];
	//TRANSFORM_DESC	m_arrParticleTransformDesc[255];
	_ulong			m_eMovePattern;
	_bool			m_bInitialize;
	_float			m_fThisAccTime;
	_float			m_fLifeTime;
	_float			m_fDeltaTime;
	_ubyte			m_ubyCreateCount;
	_uint			m_iRenderId;
	_float3			m_vTargetPos;

	//class	CTexture*	m_pTexture;
	class	CTransform*	m_pTransform;
	//class	CVIBuffer_RectTexture* m_pVIBuffer;
	class	CShader*	m_pEffectShader;


public:
	static CParticle_Base* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint& _iSceneIdx, void * _pArg = nullptr) override;
	virtual void Free() override;
};

END

#endif // !__PARTICLE_BASE_H__
