#include "stdafx.h"
#include "UI_BossMonster.h"
#include "JBaseBoss.h"

#define HP_TEX_ID		(1)
#define BACK_TEX_ID		(0)

CUI_BossMonster::CUI_BossMonster(_DEVICE _pDevice)
	: CUIBase(_pDevice)
	, m_fMonHp(0.f), m_fOriginSize(0.f), m_fOriginPos(0.f)
	, m_fAccTime(0.f)
	, m_pParentObject(nullptr)
	, m_eBossName(eBossName::End)
{
}

CUI_BossMonster::CUI_BossMonster(const CUI_BossMonster & _rOther)
	: CUIBase(_rOther), m_fOriginSize(0.f), m_fOriginPos(0.f)
	, m_fAccTime(0.f)
	, m_fMonHp(_rOther.m_fMonHp), m_pParentObject(nullptr)
	, m_eBossName(eBossName::End)
{
}

HRESULT CUI_BossMonster::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}
	return S_OK;
}

HRESULT CUI_BossMonster::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{

	if (FAILED(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	m_fOriginSize = m_tTransformDesc.vScale.x;
	m_fOriginPos = m_tTransformDesc.vPos.x;

	return S_OK;
}

_uint CUI_BossMonster::Update_GameObject(const _float & _fDeltaTime)
{
	if (m_bDead || !m_pParentObject)
		return m_iEvent = (_int)EEvent::Dead;
	if (dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Ready ||
		dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Sleep ||
		dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::Awake ||
		dynamic_cast<CJBaseBoss*> (m_pParentObject)->Get_MonsterPattern() == eBossMonsterPattern::End)
	{
		m_fAccTime = 0.f;
		return m_iEvent;
	}
	else
	{
		m_fAccTime += _fDeltaTime * 0.1f;
		if (FAILED(Get_Manager()->Add_RenderList(ERenderPriority::UIOrtho, this)))
		{
			ERROR_BOX(L"Failed to Add_RenderList");
			return 0;
		}

		m_tState.fHP = m_pParentObject->Get_Stat().fHP;
		m_tState.fMaxHP = m_pParentObject->Get_Stat().fMaxHP;
	}
	CGameObject::Update_GameObject(_fDeltaTime);

	return m_iEvent;
}

HRESULT CUI_BossMonster::Render_GameObject()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	_float4x4 matBackUIWorld, matView, matProj;
	D3DXMatrixIdentity(&matBackUIWorld);


	m_tUIDesc.vPos = { 20.f ,-300.f, 1.f };
	m_tUIDesc.vSize = { 900.f,120.f };

	//m_tTransformDesc.vPos = m_tUIDesc.vPos;
	//m_tTransformDesc.vScale = { m_tUIDesc.vSize.x,m_tUIDesc.vSize.y,1.f };
	matBackUIWorld.m[0][0] = m_tUIDesc.vSize.x;
	matBackUIWorld.m[1][1] = m_tUIDesc.vSize.y;
	memcpy(&matBackUIWorld.m[3][0], &m_tUIDesc.vPos, sizeof(_float3));

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	
	pEffect->SetMatrix("g_matWorld", &matBackUIWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);
	pEffect->SetFloat("g_fDissolve_per", 0.5f);

	CManagement::Get_Instance()->Set_Texture(0,
		L"Component_Texture_Dissolve", pEffect, "g_DissolveTexture", m_iDissolveId);

	CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_BossGage", pEffect,
		"g_BaseTexture", 0);

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(0);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}

	pEffect->EndPass();
	pEffect->End();
	
	FAILED_CHECK_RETURN(CUI_BossMonster::Render_Hp(pEffect),E_FAIL);
	FAILED_CHECK_RETURN(CUI_BossMonster::Render_Name(pEffect, m_tUIDesc.vPos), E_FAIL);

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CUI_BossMonster::Render_Hp(_EFFECT& _pEffect)
{

	//Bottom Boss UI Hp Bar
	m_tUIDesc.vPos = { 14.5f ,-314.f, 1.f };
	m_tUIDesc.vSize = { 790.f ,10.f };
	//Top Boss UI Hp Bar
	//m_tUIDesc.vPos = { 14.5f ,288.f, 1.f };
	//m_tUIDesc.vSize = { 790.f ,10.f };

	_float4x4 matHpWorld;
	D3DXMatrixIdentity(&matHpWorld);
	
	matHpWorld.m[0][0] = m_tUIDesc.vSize.x;
	matHpWorld.m[1][1] = m_tUIDesc.vSize.y;
	memcpy(&matHpWorld.m[3][0], m_tUIDesc.vPos, sizeof(_float3));

	_float4x4 matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matHpWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	_pEffect->SetFloat("g_ftime", m_fAccTime*0.5f);
	_pEffect->SetFloat("g_fU", m_tState.fHP / m_tState.fMaxHP);

	CManagement::Get_Instance()->Set_Texture(0,
	L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);


	CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_Player_HP", _pEffect,
		"g_BaseTexture", 1);

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(16);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}

	_pEffect->EndPass();
	_pEffect->End();

	return S_OK;
}

HRESULT CUI_BossMonster::Render_Name(_EFFECT & _pEffect, const _float3& _vPos)
{
	if (m_eBossName == eBossName::End)
		return S_OK;

	_float4x4 matNameUIWorld, matView, matProj;
	D3DXMatrixIdentity(&matNameUIWorld);

	matNameUIWorld.m[0][0] = 320.f;
	matNameUIWorld.m[1][1] = 24.f;
	memcpy(&matNameUIWorld.m[3][0], &_vPos, sizeof(_float3));
	matNameUIWorld.m[3][0] -= 240.f;
	matNameUIWorld.m[3][1] += 30.f;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matNameUIWorld);

	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	CManagement::Get_Instance()->Set_Texture(0, L"Component_Texture_BossName", _pEffect,
		"g_BaseTexture",(_uint)m_eBossName);

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(18);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}

	_pEffect->EndPass();
	_pEffect->End();

	return S_OK;
}

CUI_BossMonster * CUI_BossMonster::Create(_DEVICE _pDevice)
{
	CUI_BossMonster* pInstance = new CUI_BossMonster(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUI_BossMonster::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_BossMonster* pClone = new CUI_BossMonster(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}
//
//HRESULT CUI_BossMonster::Add_Com()
//{
//	return S_OK;
//}
//
//void CUI_BossMonster::Set_Hp(_uint iCheck)
//{
//	m_tTransformDesc.vPos = m_tUIDesc.vPos ;
//	m_tTransformDesc.vScale = { m_tUIDesc.vSize.x,m_tUIDesc.vSize.y,1.f };
//	//100ºÐÀ²
//	if (HP_TEX_ID == iCheck)
//	{
//		m_tTransformDesc.vScale.x = (m_tState.fHP / m_tState.fMaxHP)*m_tUIDesc.vSize.x;
//		m_tTransformDesc.vPos.x = m_tUIDesc.vPos.x + (m_tTransformDesc.vScale.x *0.5f - m_tUIDesc.vSize.x*0.5f);
//	}
//
//	if (m_pTransform)
//	{
//		m_pTransform->Set_Pos(m_tUIDesc.vPos);
//		m_pTransform->Set_Scale(m_tTransformDesc.vScale);
//		m_pTransform->Update_Transform();
//	}
//
//}
