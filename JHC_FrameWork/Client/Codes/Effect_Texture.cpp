#include "stdafx.h"
#include "Effect_Texture.h"
#include "Transform.h"
#include "Management.h"

#define _DISTANCE_		(50.f)

CEffect_Texture::CEffect_Texture(_DEVICE _pDevice)
	: CGameObject(_pDevice)
	, m_fAccTime(0.f)
	, m_pTransform(nullptr), m_pVIBuffer(nullptr), m_pEffectShader(nullptr)//, m_pNodeArr(nullptr)
	//, m_fTexAlpha(0.f)
	, m_bRenderOnOff(false), m_bFlowb(false), m_fTileling(0.f)
	//, m_iRenderId(0)
{
	ZeroMemory(&m_tTexObj_Desc, sizeof(TEXOBJ_DESC));
}

CEffect_Texture::CEffect_Texture(const CEffect_Texture & _rhs)
	: CGameObject(_rhs)
	, m_fAccTime(0.f)
	, m_pTransform(nullptr), m_pVIBuffer(nullptr), m_pEffectShader(nullptr)
	, m_bRenderOnOff(true), m_bFlowb(false), m_fTileling(0.f)

{
	ZeroMemory(&m_tTexObj_Desc, sizeof(TEXOBJ_DESC));
}

CEffect_Texture::~CEffect_Texture()
{
}

HRESULT CEffect_Texture::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CEffect_Texture::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg )
{
	if (!_pArg)
		return E_FAIL;
	if (_pArg)
	{
		memcpy(&m_tTexObj_Desc, _pArg, sizeof(TEXOBJ_DESC));
	}
	FAILED_CHECK_RETURN(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	FAILED_CHECK_RETURN(CEffect_Texture::AddCom(), E_FAIL);

	m_tTexObj_Desc.vArgb.w = 1.f;
	m_fTileling = 1.f;
	return S_OK;
}

_uint CEffect_Texture::Update_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUpdateInit)
		Update_Init(_fDeltaTime);


	Check_PlayerDistance(_fDeltaTime);
	m_pTransform->Update_Transform();
	
	return 0;
}

HRESULT CEffect_Texture::Render_GameObject()
{
	NULL_CHECK_RETURN(m_pEffectShader, E_FAIL);

	if (m_bDead)
		return E_FAIL;

	//if (FAILED(m_pTransform->Set_Transform()))
	//{
	//	ERROR_BOX(L"Failed to Set_Transform");
	//	return E_FAIL;
	//}

	_EFFECT pEffect = m_pEffectShader->Get_EffectHandle();
	NULL_CHECK_RETURN(pEffect, E_FAIL);
	pEffect->AddRef();

	for (_uint i = 0; i<2 ;++i)
	{
		m_bFlowb = i>0 ? true : false;
		FAILED_CHECK_RETURN(CEffect_Texture::SetUp_ConstantTable(pEffect),E_FAIL);
		pEffect->CommitChanges();
	

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(m_tTexObj_Desc.iRenderPass);

		m_pVIBuffer->Render_VIBuffer(&pEffect);
		//CParticle_Manager::Get_Instance()->Render_VIBuffer(&pEffect);

		pEffect->CommitChanges();
		pEffect->EndPass();
		pEffect->End();
	}
	Safe_Release(pEffect);

	return S_OK;
}

void CEffect_Texture::Update_Init(const _float & _fDeltaTime)
{
	m_bUpdateInit = true;

	//m_bRenderOnOff = true;

}

void CEffect_Texture::Check_PlayerDistance(const _float & _fDeltaTime)
{

	CGameObject* pObj =	Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player");
	NULL_CHECK_RETURN(pObj, );
	CComponent* pCom = 	pObj->Get_Component(L"Com_Transform");
	NULL_CHECK_RETURN(pCom, );
	memcpy(&m_vTargetPos, &dynamic_cast<CTransform*>(pCom)->Get_TransformDesc().matWorld.m[3][0], sizeof(_float3));

	_float fDistance = D3DXVec3Length(&(*((_float3*)(&m_pTransform->Get_TransformDesc().matWorld.m[3][0])) - m_vTargetPos));

	if ((_DISTANCE_ > fDistance) && m_bRenderOnOff) 
	{
		m_fAccTime += _fDeltaTime;
		m_fDeltaTime = _fDeltaTime;

		Get_Manager()->Add_RenderList(ERenderPriority::Environment, this);	/////렌더 추가 
		
		PARTICLE_PATTERN_DESC tPatternInfo;
		CPaticlePattern	PatternFunc;

		ZeroMemory(&tPatternInfo, sizeof(PARTICLE_PATTERN_DESC));
		m_pTransform->Set_Pos(m_tTexObj_Desc.tTransformDesc.vPos);
		tPatternInfo.tParticleInfo.fMoveSpeed		= m_tTexObj_Desc.tTransformDesc.fSpeedPerSecond;
		tPatternInfo.tParticleInfo.vArgb			= m_tTexObj_Desc.vArgb;
		tPatternInfo.tParticleInfo.vPos				= m_tTexObj_Desc.tTransformDesc.vPos;
		tPatternInfo.tParticleInfo.vRevRotateAngle	= m_tTexObj_Desc.tTransformDesc.vRev;
		tPatternInfo.tParticleInfo.vRotateAngle		= m_tTexObj_Desc.tTransformDesc.vRotate;
		tPatternInfo.tParticleInfo.vScale			= m_tTexObj_Desc.tTransformDesc.vScale;
		tPatternInfo.tParticleInfo.vTargetPos		= m_vTargetPos;

		tPatternInfo.fThisAccTime = m_fAccTime;
		m_pTransform->Set_MoveSpeed(m_tTexObj_Desc.tTransformDesc.fSpeedPerSecond);

		tPatternInfo.pTransform = m_pTransform;
		tPatternInfo.pDevice = m_pDevice;

		tPatternInfo.fDeltaTime = CManagement::Get_Instance()->Get_DeltaTime(TIMER_MAIN);

		tPatternInfo.fLifeTime = 10.f;
		tPatternInfo.ePatternType = (_ulong)m_tTexObj_Desc.eMovePattern;
		tPatternInfo.pTargetPos = &m_vTargetPos;


		PatternFunc(&tPatternInfo);

		m_tTexObj_Desc.tTransformDesc.fSpeedPerSecond	= tPatternInfo.tParticleInfo.fMoveSpeed;
		m_tTexObj_Desc.vArgb							= tPatternInfo.tParticleInfo.vArgb;
		m_tTexObj_Desc.tTransformDesc.vPos				= tPatternInfo.tParticleInfo.vPos;
		m_tTexObj_Desc.tTransformDesc.vRev				= tPatternInfo.tParticleInfo.vRevRotateAngle;
		m_tTexObj_Desc.tTransformDesc.vRotate			= tPatternInfo.tParticleInfo.vRotateAngle;
		m_tTexObj_Desc.tTransformDesc.vScale			= tPatternInfo.tParticleInfo.vScale;
		m_vTargetPos									= tPatternInfo.tParticleInfo.vTargetPos;

		//m_tParticleDesc.vScale = _float3(0.3f, 0.3f, 0.3f);
		//m_tTexObj_Desc.tTransformDesc.vScale.x 
		m_pTransform->Set_Scale(m_tTexObj_Desc.tTransformDesc.vScale);

		m_pTransform->Set_Rotate(m_tTexObj_Desc.tTransformDesc.vRotate);
		//m_pTransform->Get_Info(EInfo::Pos, &m_tTexObj_Desc.tTransformDesc.vPos);
	}
	else	//플레이어와 멀 때 
	{
		m_pTransform->Set_Pos(m_tTexObj_Desc.tTransformDesc.vPos);
		m_fAccTime = 0.f;
	}
}

void CEffect_Texture::CheckType()
{
	switch (m_tTexObj_Desc.eTexMode)
	{
	case eTexMode::Distortion:

		break;
	case eTexMode::Fog:

		break;
	}
}

HRESULT CEffect_Texture::AddCom()
{
	if (!m_pTransform)
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Component_Transform", L"Com_Transform", EComponentType::Static,
			(CComponent**)(&m_pTransform), &m_tTexObj_Desc.tTransformDesc), E_FAIL);
	}
	if (!m_pVIBuffer)
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Component_VIBuffer_RectTexture",
			L"Component_VIBuffer_RectTexture", EComponentType::Static,
			(CComponent**)(&m_pVIBuffer)), E_FAIL);
	}
	if (!m_pEffectShader)
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Shader_General_Environment", L"Shader_General_Environment", EComponentType::Static,
			(CComponent**)(&m_pEffectShader)), E_FAIL);
	}
	return S_OK;
}

HRESULT CEffect_Texture::SetUp_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4			matWorld, matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	
	_pEffect->SetMatrix("g_matWorld",&m_pTransform->Get_TransformDesc().matWorld);
	
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);

	const D3DLIGHT9* pLightInfo = Get_Manager()->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);

	_pEffect->SetFloat("g_ftime", m_fAccTime);
	_pEffect->SetFloat("g_fTiling", m_fTileling);
	_pEffect->SetFloat("g_fFlowOffset", -0.5f);
	_pEffect->SetFloat("g_fFlowStrength", 0.25f);
	_pEffect->SetVector("g_vColor", &m_tTexObj_Desc.vArgb);
	//

	_pEffect->SetBool("g_bFlowB", m_bFlowb);

	switch (m_tTexObj_Desc.eTexMode)
	{
	case eTexMode::Distortion:
		CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_Distortion",
			_pEffect, "g_DistortionTexture", m_tTexObj_Desc.iRenderId/*+ (_int)(m_bFlowb)*/);
		CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_Distortion",
			_pEffect, "g_FilterTexture", m_tTexObj_Desc.iRenderId/*+ (_int)(m_bFlowb)*/);
		//_pEffect->SetTexture("g_BaseTexture", nullptr);
		break;
	case eTexMode::DistortionFilter:
		CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_Distortion",
			_pEffect, "g_DistortionTexture", m_tTexObj_Desc.iRenderId/*+ (_int)(m_bFlowb)*/);
		CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_Filter",
			_pEffect, "g_FilterTexture", m_tTexObj_Desc.iRenderId_Filter/*+ (_int)(m_bFlowb)*/);
		//_pEffect->SetTexture("g_BaseTexture", nullptr);
		break;
	case eTexMode::Fog:
		//CManagement::Get_Instance()->Set_Texture(0, L"Tex_DustParticle",
		//	_pEffect, "g_EmissiveTexture", m_tTexObj_Desc.iRenderId);
		_pEffect->SetTexture("g_BaseTexture", nullptr);
		break;
	}

	return S_OK;
}

CEffect_Texture * CEffect_Texture::Create(_DEVICE _pDevice)
{
	CEffect_Texture* pInstance = new CEffect_Texture(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CEffect_Texture::Clone(const _uint & _iSceneIdx, void * _pArg )
{
	CEffect_Texture* pClone = new CEffect_Texture(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
		return nullptr;
	}

	return pClone;
}
//
//void CEffect_Texture::Free()
//{
//	CGameObject::Free();
//}
