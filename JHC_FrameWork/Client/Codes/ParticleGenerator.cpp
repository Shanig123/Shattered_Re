#include "stdafx.h"
#include "ParticleGenerator.h"

#define PLAYER_TRACKING_DISTANCE (30.f)

CParticleGenerator::CParticleGenerator(_DEVICE _pDevice)
	:CGameObject(_pDevice),
	m_tPaticleGenerator_Desc(), m_fThisAccTime(0.f)
	, m_pTarget(nullptr), m_bCheckProjection(false)
{
	ZeroMemory(&m_vPlayerPosTracking, sizeof(_float3));
}

CParticleGenerator::CParticleGenerator(const CParticleGenerator & _rhs)
	:CGameObject(_rhs), m_tPaticleGenerator_Desc(), m_fThisAccTime(0.f)
	, m_pTarget(nullptr), m_bCheckProjection(false)
{
	ZeroMemory(&m_vPlayerPosTracking, sizeof(_float3));
}

CParticleGenerator::~CParticleGenerator()
{
}

HRESULT CParticleGenerator::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CParticleGenerator::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	if (!_pArg)
		return E_FAIL;
	
	memcpy(&m_tPaticleGenerator_Desc, _pArg, sizeof(P_GENERATOR_DESC));
	if (eEffectType::End == m_tPaticleGenerator_Desc.eType)
		return E_FAIL;

	FAILED_CHECK_RETURN(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);

	//else if (eEffectType::Dust == m_tPaticleGenerator_Desc.eType)
	//{

	//}
	m_tTransformDesc.vPos = m_tPaticleGenerator_Desc.vCreateNormalPos;
	if (m_tPaticleGenerator_Desc.bPlayerStakingOnOff)
	{
		m_pTarget = CManagement::Get_Instance()->Get_GameObject(m_iSceneIdx, L"Layer_Player");
	}

	return S_OK;
}

_uint CParticleGenerator::Update_GameObject(const _float & _fDeltaTime)
{
	if(m_tPaticleGenerator_Desc.bPlayerStakingOnOff)
	{ 
	m_tTransformDesc.vPos = dynamic_cast<CTransform*>(m_pTarget->
		Get_Component(L"Com_Transform"))->
		Get_TransformDesc().vPos;
	}
	/*
		최적화 시 밀집하여 생성 할 수 있도록 하자.
	*/
	//_float4x4 matCam;
	//m_pDevice->GetTransform(D3DTS_VIEW, &matCam);
	//D3DXMatrixInverse(&matCam, nullptr, &matCam);
	//memcpy(&m_tTransformDesc.vPos,& matCam.m[3][0],sizeof(_float3));
	//m_tTransformDesc.vPos.y += 3.f;
	m_vPlayerPosTracking = dynamic_cast<CTransform*>((CManagement::Get_Instance()->
			Get_GameObject(m_iSceneIdx, L"Layer_Player"))->
			Get_Component(L"Com_Transform"))->
			Get_TransformDesc().vPos;

	m_vPlayerPosTracking = m_tPaticleGenerator_Desc.vCreateNormalPos - m_vPlayerPosTracking;

	CGameObject::Update_GameObject(_fDeltaTime);
	//if(!m_tPaticleGenerator_Desc.bCreateOnOff)
		Check_Projection();

	if (!m_tPaticleGenerator_Desc.bPlayerStakingOnOff)
	{

		if (((D3DXVec3Length(&m_vPlayerPosTracking)+ m_tPaticleGenerator_Desc.fCreateRange)< PLAYER_TRACKING_DISTANCE)
			&& m_bCheckProjection)//플레이어와 가깝거나 절두체 안에 있을 경우
		{
			m_tPaticleGenerator_Desc.bCreateOnOff = true;
		}
		else
			m_tPaticleGenerator_Desc.bCreateOnOff = false;
	}

	if (m_tPaticleGenerator_Desc.bCreateOnOff)
	{
		m_fThisAccTime += _fDeltaTime;

		if (m_fThisAccTime > m_tPaticleGenerator_Desc.fCreateCoolTime)
		{
			m_fThisAccTime = 0.f;
			//void(*func)(CParticle_Base::PARTICLE_PATTERN_DESC*);
		/*	void (CParticleGenerator::*func)(void) = NoAct;
			func = NoAct;
			pPointer = func;*/
			//생성함수 작성
			switch (m_tPaticleGenerator_Desc.eType)
			{
			case eEffectType::Blink:
				CreateBlinkEffect(_fDeltaTime);
				break;
			case eEffectType::Dust:
				CreateDustEffect(_fDeltaTime);
				break;
			//case eEffectType::PlayerDash:
				//PlayerDashEffect(_fDeltaTime);
				break;
			case eEffectType::Explosion0:
				Explosion0Effect(_fDeltaTime);
				break;
			default:
				break;
			}
		}

	}
	else
	{
		m_fThisAccTime = 0.f;
	}

	return 0;
}


HRESULT CParticleGenerator::Render_GameObject()
{
	return S_OK;
}

void CParticleGenerator::Check_Projection()
{
	_float4x4 matWorld, matView, matProj;
	_float3 vProjPos;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	//D3DXMatrixTranslation(&matWorld, m_tTransformDesc.vPos.x, m_tTransformDesc.vPos.y, m_tTransformDesc.vPos.z);
	D3DXVec3TransformCoord(&vProjPos, &m_tTransformDesc.vPos,& matView);
	D3DXVec3TransformCoord(&vProjPos, &vProjPos, &matProj);
	//m_tTransformDesc.matWorld = matWorld*matView*matProj;

	//memcpy(vProjPos, &m_tTransformDesc.matWorld.m[3][0], sizeof(_float3));
	if (vProjPos.x < 1.1f && vProjPos.x >-1.1f && vProjPos.z >-1.1f && vProjPos.z < 1.f && vProjPos.y < 1.1f && vProjPos.y> -1.1f)
		m_bCheckProjection = true;
	else
		m_bCheckProjection = false;
}

void CParticleGenerator::CreateDustEffect(_float _fDeltaTime)
{
	CManagement::Get_Instance()->Generate_Particle(
		m_iSceneIdx,
		m_tTransformDesc.vPos,
		m_tPaticleGenerator_Desc.fCreateRange,
		m_tPaticleGenerator_Desc.iCreateCount,
	//	_ARGB(255, 64, 236, 255),
	//	_ARGB(255, 255, 0, 0),
		_float4(0.250980f,0.925490f,1.f,1.f)
		, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoUp | (_ulong)ePartilcePatternType::x2)
		//ePartilcePatternType::OnStratBlinkAndGoUp
		, 1,2.2f);


	//CManagement::Get_Instance()->Generate_SphereParticle(
	//	m_iSceneIdx,
	//	m_tTransformDesc.vPos,
	//	m_tPaticleGenerator_Desc.fCreateRange*0.5f,
	//	&m_tTransformDesc.vPos,
	//	1,
	//	//_ARGB(255, 64, 236, 255),
	//	_float4(0.250980f, 0.925490f, 1.f, 1.f),
	//	(ePartilcePatternType)((_ulong)ePartilcePatternType::OffStartBlink | (_ulong)ePartilcePatternType::RevRotX /*| (_ulong)ePartilcePatternType::RotateX*/),9);

}

void CParticleGenerator::CreateBlinkEffect(_float _fDeltaTime)
{
	CManagement::Get_Instance()->Generate_Particle(
		m_iSceneIdx,
		m_tTransformDesc.vPos,
		m_tPaticleGenerator_Desc.fCreateRange,
		m_tPaticleGenerator_Desc.iCreateCount,
		//_ARGB(255, 64, 236, 255),
		_float4(0.250980f, 0.925490f, 1.f, 1.f),
		ePartilcePatternType::OffStartBlink);



	//CManagement::Get_Instance()->Generate_ExplosionParticle(
	//	m_iSceneIdx,
	//	m_tTransformDesc.vPos,
	//	1.f,
	//	&vTargetPos,
	//	m_tPaticleGenerator_Desc.iCreateCount,
	//	//_ARGB(255, 153, 236, 255),
	//	_float4(0.607843f, 0.925490f, 1.f, 1.f)
	//	, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraight)
	//);



}

//void CParticleGenerator::PlayerDashEffect(_float _fDeltaTime)
//{
//
//	//_float4x4* m_pPlayerBoneMatrix = nullptr;
//	//_ulong	ulongCount = 0;
//	//_float3 vCreatePos;
//	//_float4x4 matPlayerWorld;
//	//CDynamicMesh* pMesh = nullptr;
//	//pMesh = dynamic_cast<CDynamicMesh*>(m_pTarget->Get_Mesh(L"Mesh_Mesh"));
//	//if (!pMesh)
//	//	__debugbreak();
//	//pMesh->Get_AllBoneMetrix(&m_pPlayerBoneMatrix, &ulongCount);
//	//for (_ulong i = 0; i < ulongCount; ++i)
//	//{
//	//	matPlayerWorld = *((dynamic_cast<CTransform*>(m_pTarget->
//	//	Get_Component(L"Com_Transform"))->Get_WorldMatrix()));
//	//	memcpy(vCreatePos, &(m_pPlayerBoneMatrix[i] * matPlayerWorld).m[3][0], sizeof(_float3));
//	//	CManagement::Get_Instance()->Generate_ExplosionParticle(
//	//		m_iSceneIdx,
//	//		vCreatePos,
//	//		1.f,
//	//		nullptr,
//	//		m_tPaticleGenerator_Desc.iCreateCount,
//	//		_ARGB(255, 153, 236, 255),
//	//		ePartilcePatternType::OnStartBlink, 0, 4.f, 0.f);
//	//}
//
//}

void CParticleGenerator::Explosion0Effect(_float _fDeltaTime)
{
	if (m_pTarget)
	{
		_float3 vTargetPos = (dynamic_cast<CTransform*>(m_pTarget->
			Get_Component(L"Com_Transform"))->
			Get_TransformDesc().vPos);
		CManagement::Get_Instance()->Generate_ExplosionParticle(
			m_iSceneIdx,
			m_tTransformDesc.vPos,
			1.f,
			&vTargetPos,
			m_tPaticleGenerator_Desc.iCreateCount,
			//_ARGB(255, 153, 236, 255),
			_float4(0.607843f,0.925490f,1.f,1.f)
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraight)
			);
	}
	else
	{
		CManagement::Get_Instance()->Generate_ExplosionParticle(
			m_iSceneIdx,
			m_tTransformDesc.vPos,
			1.f,
			nullptr,
			m_tPaticleGenerator_Desc.iCreateCount,
			_float4(0.607843f, 0.925490f, 1.f, 1.f)
			, (ePartilcePatternType)((_ulong)ePartilcePatternType::OnStartBlink | (_ulong)ePartilcePatternType::GoStraight)
		);

	}

	/*_float3 vPos = m_tTransformDesc.vPos;
	vPos.x += 5.f;
	CManagement::Get_Instance()->Generate_ExplosionParticle(
		m_iSceneIdx,
		vPos,
		1.f,
		&vTargetPos,
		m_tPaticleGenerator_Desc.iCreateCount,
		_ARGB(255, 153, 236, 255),
		ePartilcePatternType::OnStartBlinkAndGoStraight
		, 0);
	vPos = m_tTransformDesc.vPos;
	vPos.x -= 5.f;
	CManagement::Get_Instance()->Generate_ExplosionParticle(
		m_iSceneIdx,
		vPos,
		1.f,
		&vTargetPos,
		m_tPaticleGenerator_Desc.iCreateCount,
		_ARGB(255, 153, 236, 255),
		ePartilcePatternType::OnStartBlinkAndGoStraight
		, 0);
	vPos = m_tTransformDesc.vPos;
	vPos.z += 5.f;
	CManagement::Get_Instance()->Generate_ExplosionParticle(
		m_iSceneIdx,
		vPos,
		1.f,
		&vTargetPos,
		m_tPaticleGenerator_Desc.iCreateCount,
		_ARGB(255, 153, 236, 255),
		ePartilcePatternType::OnStartBlinkAndGoStraight
		, 0);
	vPos = m_tTransformDesc.vPos;
	vPos.z -= 5.f;
	CManagement::Get_Instance()->Generate_ExplosionParticle(
		m_iSceneIdx,
		vPos,
		1.f,
		&vTargetPos,
		m_tPaticleGenerator_Desc.iCreateCount,
		_ARGB(255, 153, 236, 255),
		ePartilcePatternType::OnStartBlinkAndGoStraight
		, 0);*/


}

CParticleGenerator * CParticleGenerator::Create(_DEVICE _pDevice)
{
	CParticleGenerator* pInstance = nullptr;
	pInstance = new CParticleGenerator(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CParticleGenerator.");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

CGameObject * CParticleGenerator::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CParticleGenerator* pInstance = nullptr;
	pInstance = new CParticleGenerator(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CParticleGenerator.");
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

void CParticleGenerator::Free()
{
	CGameObject::Free();
}
