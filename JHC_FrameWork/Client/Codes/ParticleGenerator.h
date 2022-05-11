#pragma once
#ifndef __PARTICLE_GENERATOR_H__
#define __PARTICLE_GENERATOR_H__

#include "GameObject.h"
USING(Engine)
typedef enum class eEffectType
{
	Dust,
	Blink,
	//PlayerDash,
	Explosion0,
	End

}eEffectType;
class CParticleGenerator : public CGameObject
{
public:
	typedef struct tagParticleGeneratorDesc
	{
		tagParticleGeneratorDesc():
			iCreateCount(0),
			fCreateRange(0),
			fCreateCoolTime(0), 
			eType(eEffectType::End),
			bCreateOnOff(true),
			bPlayerStakingOnOff(false)
		{ZeroMemory(vCreateNormalPos, sizeof(_float3)); }
		_float3 vCreateNormalPos;
		_float	fCreateRange, fCreateCoolTime;
		_uint	iCreateCount;
		_bool	bCreateOnOff;
		_bool	bPlayerStakingOnOff;
		eEffectType eType;
	}PGENERATORDESC,P_GENERATOR_DESC;

protected:
	explicit CParticleGenerator(_DEVICE _pDevice);
	explicit CParticleGenerator(const CParticleGenerator& _rhs);
	virtual ~CParticleGenerator();

protected:
	virtual HRESULT Ready_GameObject_Prototype() override;
	virtual HRESULT Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
public:
	virtual _uint Update_GameObject(const _float & _fDeltaTime) override;
	//virtual _uint LateUpdate_GameObject(const _float & _fDeltaTime) override;
	virtual HRESULT Render_GameObject() override;

public:

private:
	void Check_Projection();
private:
	void CreateDustEffect(_float _fDeltaTime);
	void CreateBlinkEffect(_float _fDeltaTime);
	//void PlayerDashEffect(_float _fDeltaTime);
	void Explosion0Effect(_float _fDeltaTime);

private:
	P_GENERATOR_DESC m_tPaticleGenerator_Desc;
	_float	m_fThisAccTime;
	_float3 m_vPlayerPosTracking;
	CGameObject*		m_pTarget;
	_bool	m_bCheckProjection;
public:
	static CParticleGenerator* Create(_DEVICE _pDevice);
	virtual CGameObject * Clone(const _uint & _iSceneIdx, void * _pArg = nullptr) override;
protected:
	virtual void Free()override;
};

#endif // !__PARTICLE_GENERATOR_H__

