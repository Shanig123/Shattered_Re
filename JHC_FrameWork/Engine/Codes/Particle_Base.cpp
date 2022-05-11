#include "Particle_Base.h"
#include "Transform.h"
#include "Management.h"


USING(Engine);

CParticle_Base::CParticle_Base(_DEVICE _pDevice)
	:CGameObject(_pDevice)
	,m_pTransform(nullptr), m_pEffectShader(nullptr), m_eMovePattern((_ulong)ePartilcePatternType::End)
	, m_bInitialize(false)
	, m_fThisAccTime(0.f), m_fDeltaTime(0.f)
	, m_ubyCreateCount(0), m_iRenderId(0)
{
	ZeroMemory(&m_vTargetPos, sizeof(_float3));
}

CParticle_Base::CParticle_Base(const CParticle_Base & _rhs)
	: CGameObject(_rhs)
	, m_pTransform(nullptr), m_pEffectShader(nullptr), m_eMovePattern((_ulong)ePartilcePatternType::End)
	, m_bInitialize(false)
	, m_fThisAccTime(0.f)
	, m_ubyCreateCount(0), m_iRenderId(0)
{
	ZeroMemory(&m_vTargetPos, sizeof(_float3));
}

HRESULT CParticle_Base::Ready_GameObject_Prototype()
{
	return S_OK;
}

HRESULT CParticle_Base::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg)
{
	if (_pArg)
		return E_FAIL;

	FAILED_CHECK_RETURN(CGameObject::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);

	return S_OK;
}

_uint CParticle_Base::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_bDead)
		return (_uint)EEvent::Dead;
	if (!m_bInitialize)
		CParticle_Base::AddCom();
	//if (!m_tParticleDesc.pFunction)
	//	return (_uint)EEvent::Dead;
	m_fDeltaTime = _fDeltaTime;
	//if ((_ulong)m_eMovePattern & (_ulong)ePartilcePatternType::x2)
	//{
	//	m_fDeltaTime *= 2.f;
	//}
	//if ((_ulong)m_eMovePattern & (_ulong)ePartilcePatternType::x3)
	//{
	//	m_fDeltaTime *= 3.f;
	//}

	//if ((_ulong)m_eMovePattern & (_ulong)ePartilcePatternType::x4)
	//{
	//	m_fDeltaTime *= 4.f;
	//}
	//if ((_ulong)m_eMovePattern & (_ulong)ePartilcePatternType::x5)
	//{
	//	m_fDeltaTime *= 5.f;
	//}
	m_fThisAccTime += m_fDeltaTime;
	//if (m_eMovePattern == ePartilcePatternType::OnStartBlinkAndGoStraight)
	//{
	//	m_fThisAccTime += _fDeltaTime;
	//}
	if (m_fThisAccTime > m_fLifeTime)
	{
		m_bDead = true;

		return (_uint)EEvent::Dead;
	}
	//FAILED_CHECK_RETURN(CManagement::Get_Instance()->Add_RenderList(ERenderPriority::Effect, this),E_FAIL);
	FAILED_CHECK_RETURN(CManagement::Get_Instance()->Add_RenderList(ERenderPriority::Alpha, this), E_FAIL);

	return (_uint)EEvent::None;
}

HRESULT CParticle_Base::Render_GameObject()
{
	NULL_CHECK_RETURN(m_pEffectShader,E_FAIL);

	if (m_bDead)
		return E_FAIL;
	PARTICLE_PATTERN_DESC tPatternInfo;
	CPaticlePattern	PatternFunc;

	for (_ubyte i = 0; i < m_ubyCreateCount; ++i)
	{
		ZeroMemory(&tPatternInfo, sizeof(PARTICLE_PATTERN_DESC));

		tPatternInfo.tParticleInfo = m_arrParticleDesc[i];
		tPatternInfo.fThisAccTime = m_fThisAccTime;
		m_pTransform->Set_MoveSpeed(m_arrParticleDesc[i].fMoveSpeed);

		tPatternInfo.pTransform = m_pTransform;
		tPatternInfo.pDevice = m_pDevice;

		tPatternInfo.fDeltaTime = m_fDeltaTime;
		//if (m_eMovePattern == ePartilcePatternType::OnStartBlinkAndGoStraight)
		//{
		//	tPatternInfo.fDeltaTime *= 2.f;
		//}

		tPatternInfo.fLifeTime = m_fLifeTime;
		tPatternInfo.ePatternType = m_eMovePattern;
		tPatternInfo.pTargetPos = &m_vTargetPos;

		m_arrParticleDesc[i] = tPatternInfo.tParticleInfo;
		if (m_iRenderId == 0)
		{
			m_arrParticleDesc[i].vScale = _float3(0.05f, 0.05f, 0.05f);
		}
		else if ((m_iRenderId >= 3 && m_iRenderId <= 5) || 27 ==m_iRenderId)
		{
			m_arrParticleDesc[i].vScale = _float3(0.1f, 0.1f, 0.1f);
		}
		else if (m_iRenderId >= 6 && m_iRenderId <= 8)
		{
			m_arrParticleDesc[i].vScale = _float3(0.3f, 0.3f, 0.3f);
		}
		//if(m_iRenderId != 0)
		m_pTransform->Set_Scale(m_arrParticleDesc[i].vScale);
		m_pTransform->Set_Rotate(m_arrParticleDesc[i].vRotateAngle);

		PatternFunc(&tPatternInfo);
	
		//m_fThisAccTime = tPatternInfo.fThisAccTime;
		m_arrParticleDesc[i].vArgb = tPatternInfo.tParticleInfo.vArgb;
		//if (m_iRenderId == 0)
		//{
		//	m_pTransform->Set_Scale(_float3(0.05f, 0.05f, 0.05f));
		//}
		//m_pTransform->Set_Pos(m_arrParticleDesc[i].vPos);
		m_pTransform->Update_Transform();
		m_pTransform->Get_Info(EInfo::Pos, &m_arrParticleDesc[i].vPos);
		
		if (FAILED(m_pTransform->Set_Transform()))
		{
			ERROR_BOX(L"Failed to Set_Transform");
			return E_FAIL;
		}

		_EFFECT pEffect = m_pEffectShader->Get_EffectHandle();
		NULL_CHECK_RETURN(pEffect, E_FAIL);
		pEffect->AddRef();
		SetUp_ConstantTable(pEffect, i);
		pEffect->CommitChanges();

//		_uint	iPassMax = 0;

		pEffect->Begin(nullptr, 0);		// 1인자 : 현재 쉐이더 파일이 갖고 있는 pass의 최대 개수, 2인자 : 시작하는 방식(default)
	/*	if (m_eMovePattern == ePartilcePatternType::Decal)
			pEffect->BeginPass(3);
		else*/
			pEffect->BeginPass(2);

		//m_pTexture->Set_Texture(pEffect, "g_EmissiveTexture", 0);
		
		//CParticle_Manager::Get_Instance()->Set_Texture(pEffect,
		//	"g_EmissiveTexture",
		//	m_iRenderId);
		CManagement::Get_Instance()->Set_Texture(0, L"Tex_DustParticle",
			pEffect,"g_EmissiveTexture", m_iRenderId);
		pEffect->CommitChanges();
		pEffect->SetTexture("g_BaseTexture", nullptr);
		pEffect->SetTexture("g_NormalTexture", nullptr);
		pEffect->SetTexture("g_OpacityTexture", nullptr);

		CParticle_Manager::Get_Instance()->Render_VIBuffer(&pEffect);
		//m_pVIBuffer->Render_VIBuffer(&pEffect);


		pEffect->CommitChanges();
		pEffect->EndPass();
		pEffect->End();

		Safe_Release(pEffect);
		//if (m_eMovePattern == ePartilcePatternType::OnStratBlinkAndGoUp ||
		//	m_eMovePattern == ePartilcePatternType::OnStratBlinkAndGoUpx2)
		//	if (m_arrParticleDesc[i].vArgb.w <= 0.f)
		//		m_bDead = true;
	}
	//m_fThisAccTime = m_arrParticleDesc[0].
	return S_OK;
}

void CParticle_Base::Reset_Data()
{
	ZeroMemory(m_arrParticleDesc, sizeof(PARTICLE_DESC)*255);
	//ZeroMemory(m_arrParticleTransformDesc, sizeof(TRANSFORM_DESC));
	m_bDead = false;
	m_bInitialize = false;
	m_iSceneIdx = 0;
	m_fThisAccTime = 0.f;
	m_ubyCreateCount = 0;
	m_fLifeTime = 0.f;
	m_iRenderId = 0;
	m_eMovePattern = (_ulong)ePartilcePatternType::End;
	m_pTransform->ResetData();
	ZeroMemory(&m_vTargetPos, sizeof(_float3));
}

void CParticle_Base::Set_ParticleInfo(const PARTICLE_DESC & _tParticleInfo,
	_float3* _pTargetPos,
	const _ubyte& _ubyIndex,
	const _ubyte& _ubyCreateCount,
	const _float& _fLifeTime,
	const _uint& _iRenderID,
	const ePartilcePatternType& _eParticleMovePattern)
{
	memcpy(&m_arrParticleDesc[_ubyIndex], &_tParticleInfo, sizeof(PARTICLE_DESC));
	m_arrParticleDesc[_ubyIndex].vInitPos = m_arrParticleDesc[_ubyIndex].vPos;
	//memcpy(&m_arrParticleTransformDesc[_ubyIndex],
	//	&m_arrParticleDesc[_ubyIndex].tTransformDesc, 
	//	sizeof(TRANSFORM_DESC)*255);
	m_ubyCreateCount = _ubyCreateCount;
	m_fLifeTime = _fLifeTime;
	m_eMovePattern = (_ulong)_eParticleMovePattern;
	m_iRenderId = _iRenderID;
	if(_pTargetPos)
		m_vTargetPos = *_pTargetPos;
	else if (_pTargetPos == nullptr)
	{
		m_vTargetPos = { Mersen_ToFloat(-1.f,1.f),Mersen_ToFloat(-1.f,1.f) ,Mersen_ToFloat(-1.f,1.f) };
		m_vTargetPos *= 100.f;
	}
	else
		ZeroMemory(&m_vTargetPos, sizeof(_float3));
	//m_bInitialize = false;
}

void CParticle_Base::Set_SceneIndex(const _uint & _iSceneIdx)
{
	m_iSceneIdx = _iSceneIdx;
}

HRESULT CParticle_Base::AddCom()
{
	if (!m_pTransform)
	{
		
		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Component_Transform", L"Com_Transform", EComponentType::Static,
			(CComponent**)(&m_pTransform), &m_tTransformDesc), E_FAIL);
	}
	//if (!m_pVIBuffer)
	//{
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
	//		L"Component_VIBuffer_RectTexture", L"Component_VIBuffer_RectTexture", EComponentType::Static,
	//		(CComponent**)(&m_pVIBuffer)), E_FAIL);
	//}
	if (!m_pEffectShader)
	{
		FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
			L"Shader_General_Effect", L"Shader_General_Effect", EComponentType::Static,
			(CComponent**)(&m_pEffectShader)), E_FAIL);
	}
	//if (!m_pTexture)
	//{
	//	FAILED_CHECK_RETURN(CGameObject::Add_Component(0,
	//		L"Tex_DustParticle", L"Tex_DustParticle", EComponentType::Static,
	//		(CComponent**)(&m_pTexture)), E_FAIL);
	//}
	if(!m_bInitialize)
	{
		m_bInitialize = true;
	}

	return S_OK;
}

HRESULT CParticle_Base::SetUp_ConstantTable(_EFFECT & _pEffect, const _ubyte& _ubyIndex)
{
	_float4x4			matWorld, matView, matProj;

	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);

	auto pManagement = GET_MANAGEMENT;

	const D3DLIGHT9* pLightInfo = pManagement->Get_Light();

	_pEffect->SetVector("g_vLightDiffuse", (_float4*)&pLightInfo->Diffuse);
	_pEffect->SetVector("g_vLightDir", &_float4(pLightInfo->Direction, 0.f));

	D3DXMatrixInverse(&matView, NULL, &matView);
	_pEffect->SetVector("g_vCamPos", (_float4*)&matView._41);
   	_pEffect->SetVector("g_vColor", (_float4*)&m_arrParticleDesc[_ubyIndex].vArgb);
	return S_OK;
}

CParticle_Base * CParticle_Base::Create(_DEVICE _pDevice)
{
	CParticle_Base* pInstance = new CParticle_Base(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject * CParticle_Base::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CParticle_Base* pClone = new CParticle_Base(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
		return nullptr;
	}

	return pClone;
}

void CParticle_Base::Free()
{
	CGameObject::Free();
}
