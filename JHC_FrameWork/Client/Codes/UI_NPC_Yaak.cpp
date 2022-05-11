#include "stdafx.h"
#include "UI_NPC_Yaak.h"
#include "Player.h"
#include "UI_Inventory.h"
#include "Player_UI.h"

/*
디졸브 필요함
*/

#define PowValue			(1.5f)

CUI_NPC_Yaak::CUI_NPC_Yaak(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_bUIView(false), m_bPlayScript(false)
	, m_iScriptFlag(0), m_iSelectIndex(0), m_iScriptId(0)
	, m_iMainRenderPass(0), m_iScriptRenderPass(0)
	, m_fScriptDelayTime(0.f), m_fBackDissolveTime(0.f), m_fScriptDissolveTime(0.f)
	, m_eUIMode(eUIMode::End)

	, m_eView(EView::Quee_Chess)
{
}

CUI_NPC_Yaak::CUI_NPC_Yaak(const CUI_NPC_Yaak & _rhs)
	: CUIBase(_rhs)
	, m_bUIView(false), m_bPlayScript(false)
	, m_iScriptFlag(0), m_iSelectIndex(0), m_iScriptId(0)
	, m_iMainRenderPass(0), m_iScriptRenderPass(0)
	, m_fScriptDelayTime(0.f), m_fBackDissolveTime(0.f), m_fScriptDissolveTime(0.f)
	, m_eUIMode(eUIMode::End)

	, m_eView(EView::Quee_Chess)
{
}

HRESULT CUI_NPC_Yaak::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();

}

HRESULT CUI_NPC_Yaak::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	CManagement* pManagement = GET_MANAGEMENT;
	m_tUIDesc.vPos = { _float(0),_float(0) ,1.f };
	m_tUIDesc.vSize = { _float(WINCX),_float(WINCY) };
	lstrcpy(m_tUIDesc.szTexName, L"Component_Texture_ValnirUI");
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);
	m_eUIMode = eUIMode::Select;

	return S_OK;
}

_uint CUI_NPC_Yaak::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	CUIBase::Update_GameObject(_fDeltaTime);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));

	if (pPlayer->Get_Npc_Yaak_Interact())
	{
		if (!m_bUIView)
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);

			m_iScriptFlag = 0;
			m_fBackDissolveTime = 0.f;
			m_fScriptDissolveTime = 0.f;
			m_fScriptDelayTime = 0.f;
			m_bPlayScript = false;
		}
			m_bUIView = true;

		pPlayer->Set_Npc_Chrono(true);
	}
	if (m_bUIView)
	{
		if (m_iMainRenderPass == 4 && m_fBackDissolveTime < 8.f)
			m_fBackDissolveTime += _fDeltaTime * powf(PowValue, m_fBackDissolveTime);

		m_bPlayerInteract = true;
		pPlyUI->Set_ViewInterface(false);
		KeyInput(_fDeltaTime);
		
		if (m_bPlayScript)
			Play_Script(_fDeltaTime);

		if (m_eUIMode == eUIMode::Talk)
		{
			if (m_fScriptDissolveTime < 8.f)
			{
				m_fScriptDissolveTime += _fDeltaTime * powf(PowValue, m_fScriptDissolveTime);
				m_iScriptRenderPass = 0;
			}
			else
			{
				m_iScriptRenderPass = 0;
			}
		}
	}
	else
	{
		if (m_bPlayerInteract)
		{
			if (!pPlyUI->Get_CheckViewInterface())
			{
				pPlyUI->Set_ViewInterface(true);
			}
			pPlayer->Set_Npc_Chrono(false);
			m_bPlayerInteract = false;
		}
	}
	return m_iEvent;
}

_uint CUI_NPC_Yaak::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_NPC_Yaak::Render_GameObject()
{
	if (!m_bUIView)
		return 0;
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUI_NPC_Yaak::MainBack_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		Safe_Release(pEffect);

		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	if (m_iScriptFlag == 1 && m_fBackDissolveTime < 8.f)
		m_iMainRenderPass = 4;
	else
		m_iMainRenderPass = 0;
	pEffect->BeginPass(m_iMainRenderPass);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(pEffect);

		return E_FAIL;
	}
	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	if (m_eUIMode == eUIMode::Talk && !m_bPlayScript)
	{
		if (FAILED(CUI_NPC_Yaak::Render_Enter()))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}
	}
	if (m_eUIMode == eUIMode::Talk && !m_bPlayScript)
		Render_Script();

	return S_OK;
}

HRESULT CUI_NPC_Yaak::Render_Script()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUI_NPC_Yaak::Script_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		Safe_Release(pEffect);

		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(m_iScriptRenderPass);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(pEffect);

		return E_FAIL;
	}
	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CUI_NPC_Yaak::Render_Enter()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUI_NPC_Yaak::Enter_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		Safe_Release(pEffect);

		return E_FAIL;
	}
	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(0);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(pEffect);

		return E_FAIL;
	}
	pEffect->CommitChanges();
	pEffect->EndPass();
	pEffect->End();

	Safe_Release(pEffect);
	return S_OK;
}

HRESULT CUI_NPC_Yaak::MainBack_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4 matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	_pEffect->SetFloat("g_ftime", m_fBackDissolveTime);
	_pEffect->SetFloat("g_fDissolveSpeed", 1.f);
	_pEffect->SetVector("g_vColor", &_float4(0.f, 0.f, 0.f, 1.f));
	Get_Manager()->Set_Texture(0, L"Component_Texture_YaakUI", _pEffect, "g_BaseTexture", (_uint)m_eView);

	Get_Manager()->Set_Texture(0,L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", 0);

	return S_OK;
}

HRESULT CUI_NPC_Yaak::Script_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4 matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	memcpy(&matWorld.m[0][0], &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float));
	memcpy(&matWorld.m[3][0], &m_pTransform->Get_TransformDesc().matWorld.m[3][0], sizeof(_float3));
	matWorld.m[1][1] = 250.f;
	matWorld.m[3][1] = -240.f;
	matWorld.m[3][0] = 110.f;
	matWorld.m[3][3] = 1.f;
	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	_pEffect->SetFloat("g_ftime", m_fScriptDissolveTime);
	_pEffect->SetFloat("g_fDissolveSpeed", 1.f);
	_pEffect->SetVector("g_vColor", &_float4(0.f, 0.f, 0.f, 0.f));
	const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
	const _uint& iStatic = Get_EventMgr()->Get_CurStaticEvent();
	Get_Manager()->Set_Texture(0, L"Component_Texture_YaakScript", _pEffect, "g_BaseTexture", m_iScriptId);
	Get_Manager()->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);

	return S_OK;
}

HRESULT CUI_NPC_Yaak::Enter_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4 matWorld, matView, matProj;
	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	
	matWorld.m[0][0] = 40;
	matWorld.m[1][1] = 30.f;
	matWorld.m[3][1] = -270.f;
	matWorld.m[3][0] = WINCX - 820.f;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);

	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	Get_Manager()->Set_Texture(0, L"Component_Texture_KeyUI", _pEffect, "g_BaseTexture", 0);

	Get_Manager()->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);

	return S_OK;
}

void CUI_NPC_Yaak::KeyInput(const _float& _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (pManagement->Key_Down( DIK_ESCAPE))
	{
		Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
		Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
		pPlayer->Set_Npc_Yaak(false);
		m_bUIView = false;
		m_iScriptFlag = 0;
		m_iSelectIndex = 0;
		m_iScriptId = 0;
	}

	if (pManagement->Key_Down(DIK_RETURN))
	{
		if (eUIMode::Select == m_eUIMode)
		{
			switch (m_eView)
			{
			case EView::Quee_Chess:
			case EView::Knight_Tour:
				m_eUIMode = eUIMode::Talk;
				m_bPlayScript = true;
				break;
			case EView::Select_Start:
			{
				Get_EventMgr()->Add_Event(EVENT_VOLATILE_STAGE_CHESSSTART);
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
				CTransform* pPlayerTransform = dynamic_cast<CTransform*>(pPlayer->Get_Component(L"Com_Transform"));
				pPlayer->Set_Npc_Yaak(false);
				pPlayer->Min_ObstacleType(EObstacle::Yaak);
				m_bUIView = false;
				m_iScriptFlag = 0;
				GET_MANAGEMENT->Set_GlobalPos(L"Player", _float3(120.4f, 31.682110f, 228.3f));
				pPlayerTransform->Set_Rotate(EAxis::Y, D3DXToRadian(130.f));				
			}				
				return;
			case EView::Select_Leave:
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);

				CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
				pPlayer->Set_Npc_Yaak(false);
				m_bUIView = false;
				m_iScriptFlag = 0;
				m_iSelectIndex = 0;
				m_iScriptId = 0;
			}				
				return;
			}

			m_eView = EView::Talk;
		}
		else
		{
			FindSelect();
			m_eUIMode = eUIMode::Select;
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UITalking.ogg", CSound_Manager::ESoundID::UI, 1.f);
		}
	}

	if (m_eUIMode == eUIMode::Select)
	{
		

		if (pManagement->Key_Down(DIK_W))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UITalkSelect.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (0 <= m_iSelectIndex - 1)
				--m_iSelectIndex;
		}

		if (pManagement->Key_Down(DIK_S))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UITalkSelect.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (4 > m_iSelectIndex + 1)
				++m_iSelectIndex;
		}

		FindSelect();
	}
}

void CUI_NPC_Yaak::Play_Script(const _float& _fDeltaTime)
{
	if(::CoolTime(_fDeltaTime, m_fScriptDelayTime, 0.5f))
	{
		m_fScriptDelayTime = 0.f;
		m_bPlayScript = false;
	}
	else
	{
		if(m_eUIMode == eUIMode::Talk)
			Script_UI_Control(_fDeltaTime);
	}
}

void CUI_NPC_Yaak::Script_UI_Control(const _float & _fDeltaTime)
{
	if ((_int)EView::Quee_Chess == m_iSelectIndex)
		m_iScriptId = 0;

	if ((_int)EView::Knight_Tour == m_iSelectIndex)
		m_iScriptId = 1;
}

void CUI_NPC_Yaak::FindSelect()
{
	switch (m_iSelectIndex)
	{
	case 0:
		m_eView = EView::Quee_Chess;
		break;
	case 1:
		m_eView = EView::Knight_Tour;
		break;
	case 2:
		m_eView = EView::Select_Start;
		break;
	case 3:
		m_eView = EView::Select_Leave;
		break;
	}
}

CUI_NPC_Yaak * CUI_NPC_Yaak::Create(_DEVICE _pDevice)
{
	CUI_NPC_Yaak* pInstance = new CUI_NPC_Yaak(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_NPC_Yaak::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_NPC_Yaak* pInstance = new CUI_NPC_Yaak(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

