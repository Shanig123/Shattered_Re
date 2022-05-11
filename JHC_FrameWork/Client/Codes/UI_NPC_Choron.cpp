#include "stdafx.h"
#include "UI_NPC_Choron.h"
#include "Player.h"
#include "UI_Inventory.h"
#include "Player_UI.h"

/*
디졸브 필요함
*/

#define TALK				(0)
#define MAINQUEST			(1)
#define MAINQUESTLEAVE		(2)
#define SUBQUEST			(3)
#define SUBQUESTLEAVE		(4)
#define SELECT_OK			(5)
#define SELECT_NO			(6)

#define PowValue			(1.5f)

CUI_NPC_Choron::CUI_NPC_Choron(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_bUIView(false), m_bPlayScript(false)
	, m_iScriptFlag(0), m_iMainDrawId(0), m_iScriptId(0)
	, m_iMainRenderPass(0), m_iScriptRenderPass(0)
	, m_fScriptDelayTime(0.f), m_fBackDissolveTime(0.f), m_fScriptDissolveTime(0.f)
	, m_eUIMode(eUIMode::End)
{
}

CUI_NPC_Choron::CUI_NPC_Choron(const CUI_NPC_Choron & _rhs)
	: CUIBase(_rhs)
	, m_bUIView(false), m_bPlayScript(false)
	, m_iScriptFlag(0), m_iMainDrawId(0), m_iScriptId(0)
	, m_iMainRenderPass(0), m_iScriptRenderPass(0)
	, m_fScriptDelayTime(0.f), m_fBackDissolveTime(0.f), m_fScriptDissolveTime(0.f)
	, m_eUIMode(eUIMode::End)
{
}

HRESULT CUI_NPC_Choron::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();

}

HRESULT CUI_NPC_Choron::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	CManagement* pManagement = GET_MANAGEMENT;
	m_tUIDesc.vPos = { _float(0),_float(0) ,1.f };
	m_tUIDesc.vSize = { _float(WINCX),_float(WINCY) };
	lstrcpy(m_tUIDesc.szTexName, L"Component_Texture_ValnirUI");
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);
	m_eUIMode = eUIMode::Talk;
	//lstrcpy(tDesc.szTexName, L"Component_Texture_EquipBack");

	//FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_UI_Player_Equipment", (CGameObject**)&m_pEquipUI, &tDesc), E_FAIL);
	//FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pEquipUI, m_iSceneIdx, L"Layer_PlayerUI"), E_FAIL);
	//Safe_AddRef(m_pEquipUI);

	return S_OK;
}

_uint CUI_NPC_Choron::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	CUIBase::Update_GameObject(_fDeltaTime);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));

	if (pPlayer->Get_Npc_Choron_Interact())
	{
		if (!m_bUIView)
		{
			m_iScriptFlag = 1;
			m_fBackDissolveTime = 0.f;
			m_fScriptDissolveTime = 0.f;
			m_fScriptDelayTime = 0.f;
			m_bPlayScript = true;

			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
		}
			m_bUIView = true;
		//m_bPlayScript = true;
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
		if (m_eUIMode == eUIMode::Select)
		{
			const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
			const _uint& iStatic = Get_EventMgr()->Get_CurStaticEvent();

			if (m_iMainDrawId != MAINQUESTLEAVE && m_iMainDrawId != SUBQUESTLEAVE && m_iMainDrawId != SELECT_NO)
			{
				if (!(iQuest & QUEST_START_1) && m_iScriptFlag < 4)
					m_iMainDrawId = MAINQUEST;
				if (((iQuest & (QUEST_START_1|QUEST_END_1)) == (QUEST_START_1 | QUEST_END_1)) && m_iScriptFlag < 4)
					m_iMainDrawId = SUBQUEST;

				if ((iQuest & QUEST_START_1) && !(iQuest & QUEST_END_1) && (iStatic & EVENT_STATIC_GET_QUEST_ITEM1))
					m_eUIMode = eUIMode::Talk;
		/*		else 
					m_iMainDrawId = MAINQUEST;*/
			}
		}
		else if (m_eUIMode == eUIMode::Talk)
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
			m_iMainDrawId = 0;
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

_uint CUI_NPC_Choron::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_NPC_Choron::Render_GameObject()
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

	if (FAILED(CUI_NPC_Choron::MainBack_ConstantTable(pEffect)))
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
		if (FAILED(CUI_NPC_Choron::Render_Enter()))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}
	}
	if (m_eUIMode == eUIMode::Talk && !m_bPlayScript)
		Render_Script();

	return S_OK;
}

HRESULT CUI_NPC_Choron::Render_Script()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUI_NPC_Choron::Script_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		Safe_Release(pEffect);

		return E_FAIL;
	}
	/*
		SetUp ConstantTable;

	*/
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

HRESULT CUI_NPC_Choron::Render_Enter()
{
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(CUI_NPC_Choron::Enter_ConstantTable(pEffect)))
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

HRESULT CUI_NPC_Choron::MainBack_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4 matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	_float4x4 matWorld;
	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	matWorld._41 = 0.f;
	matWorld._42 = 0.f;
	matWorld._43 = 1.0f;
	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);
	_pEffect->SetFloat("g_ftime", m_fBackDissolveTime);
	_pEffect->SetFloat("g_fDissolveSpeed", 1.f);
	_pEffect->SetVector("g_vColor", &_float4(0.f, 0.f, 0.f, 1.f));
	Get_Manager()->Set_Texture(0, L"Component_Texture_ChoronUI", 
		_pEffect, "g_BaseTexture", m_iMainDrawId);

	Get_Manager()->Set_Texture(0,
		L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", 0);

	return S_OK;
}

HRESULT CUI_NPC_Choron::Script_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4 matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	memcpy(&matWorld.m[0][0], &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float));
	//memcpy(&matWorld.m[3][0], &m_pTransform->Get_TransformDesc().matWorld.m[3][0], sizeof(_float3));
	matWorld.m[1][1] = 250.f;
	//matWorld.m[3][1] = (WINCY*0.5f) - 50.f;
	matWorld.m[3][1] = /*(WINCY*0.5f) -*/ -260.f;
	matWorld.m[3][0] = 110.f;
	matWorld.m[3][2] = 1.f;
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
	if ((m_iScriptFlag == 1) && (iQuest & QUEST_START_1) && !(iQuest & QUEST_END_1) && !(iStatic & EVENT_STATIC_GET_QUEST_ITEM1))
		Get_Manager()->Set_Texture(0, L"Component_Texture_ChoronScript", _pEffect, "g_BaseTexture", m_iScriptId);
	else if (m_iScriptFlag == 1)
		Get_Manager()->Set_Texture(0, L"Component_Texture_HelloScript", _pEffect, "g_BaseTexture", m_iScriptId);
	else
		Get_Manager()->Set_Texture(0, L"Component_Texture_ChoronScript", _pEffect, "g_BaseTexture", m_iScriptId);
	Get_Manager()->Set_Texture(0,
		L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);

	return S_OK;
}

HRESULT CUI_NPC_Choron::Enter_ConstantTable(_EFFECT & _pEffect)
{
	_float4x4 matWorld, matView, matProj;
	//D3DXMatrixIdentity(&matWorld);
	matWorld = m_pTransform->Get_TransformDesc().matWorld;
	
	matWorld.m[0][0] = 40;
	matWorld.m[1][1] = 30.f;
	////matWorld.m[3][1] = -(WINCY*0.5f) - 50.f;
	matWorld.m[3][1] = /*(WINCY*0.5f) -*/ -270.f;
	matWorld.m[3][0] = WINCX - 820.f;
	//matWorld.m[3][1] = 0.f;
	//matWorld.m[3][0] = 0.f;
	matWorld.m[3][2] = 1.f;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	//_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);

	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	_pEffect->SetFloat("g_fDissolve_per", 0.5f);

	Get_Manager()->Set_Texture(0,
		L"Component_Texture_KeyUI", _pEffect, "g_BaseTexture", 0);

	Get_Manager()->Set_Texture(0,
	L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", m_iDissolveId);

	return S_OK;
}

void CUI_NPC_Choron::KeyInput(const _float& _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	if (pManagement->Key_Down( DIK_ESCAPE))
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
		pPlayer->Set_Npc_Chrono(false);
		m_bUIView = false;
		m_iScriptFlag = 0;
		m_iMainDrawId = 0;
		m_iScriptId = 0;

		Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
		Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
	}
	if (pManagement->Key_Down(DIK_RETURN) && !m_bPlayScript)
	{
		if (m_iMainRenderPass == 4 && m_fScriptDissolveTime < 8.f)
		{
			m_fScriptDissolveTime += 8.f;
			++m_iScriptFlag;
			//m_fScriptDissolveTime = 0.f;
			return;
		}
		if (m_eUIMode == eUIMode::Talk)
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UITalking.ogg", CSound_Manager::ESoundID::UI, 1.f);

			m_bPlayScript = true;
			if (m_iScriptFlag == 1)
			{
				
				m_eUIMode = eUIMode::Select;
				const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
				const _uint& iStatic = Get_EventMgr()->Get_CurStaticEvent();
				if ((iQuest & QUEST_START_1) && !(iQuest & QUEST_END_1) && (iStatic & EVENT_STATIC_GET_QUEST_ITEM1))
				{
					m_eUIMode = eUIMode::Talk;
				}
			
			}
			++m_iScriptFlag;
			m_fScriptDissolveTime = 0.f;
			//m_iScriptFlag = m_iScriptFlag << 1;
		}
		else if (m_eUIMode == eUIMode::Select)
		{
			if ((m_iMainDrawId == MAINQUESTLEAVE) || (m_iMainDrawId == SUBQUESTLEAVE) || (m_iMainDrawId == SELECT_NO))
			{
				CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
				pPlayer->Set_Npc_Chrono(false);
				m_bUIView = false;
				m_iScriptFlag = 0;
				m_iMainDrawId = 0;
				m_iScriptId = 0;
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
			}
			else
			{
				if (m_iMainDrawId == SELECT_OK)
				{
					Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
					Get_Manager()->PlaySounds(L"Environment_UITalkSelect.ogg", CSound_Manager::ESoundID::UI, 1.f);

					const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
					const _uint& iStatic = Get_EventMgr()->Get_CurStaticEvent();
					const _uint& iEvent = Get_EventMgr()->Get_CurEvent();

					if (/*(m_iScriptFlag == 6)
						&&*/ (iQuest & QUEST_START_1)
						&& (iQuest & QUEST_END_1)
						&& !(iEvent &(EVENT_VOLATILE_QUEST_START_1))
						&& !(iEvent &(EVENT_VOLATILE_QUEST_END_1)))
					{
						Get_EventMgr()->Add_Event(EVENT_VOLATILE_QUEST_START_1);
						CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
						pPlayer->Set_Npc_Chrono(false);
						m_bUIView = false;
						m_iScriptFlag = 0;
						m_pTransform->Set_Pos((GET_MANAGEMENT->Set_GlobalPos(L"Player", _float3(13.f, 1.992630f, -6.f))));
					}

					if ((m_iScriptFlag >= 6) && !(iQuest & QUEST_START_1))
					{
						Get_EventMgr()->Add_Quest_Flag(QUEST_START_1);	//메인퀘 시작
						CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
						pPlayer->Set_Npc_Chrono(false);
						m_bUIView = false;
						m_iScriptFlag = 0;
						m_iMainDrawId = 0;
						m_iScriptId = 0;
						//m_pTransform->Set_Pos((GET_MANAGEMENT->Set_GlobalPos(L"Player", _float3(13.f, 1.992630f, -6.f))));

					}
				}
			
				else
				{
					m_eUIMode = eUIMode::Talk;
					m_bPlayScript = true;

					Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
					Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
				}
				if (m_iMainDrawId == SELECT_NO)
				{

					Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
					Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
				}
			}
			
		}
		/*Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
		Get_Manager()->PlaySounds(L"Test.wav", CSound_Manager::ESoundID::UI);*/
	}
	if (m_eUIMode == eUIMode::Select)
	{
		if (pManagement->Key_Down(DIK_W) || pManagement->Key_Down(DIK_S))
		{
			if (m_iMainDrawId == MAINQUEST)
				++m_iMainDrawId;
			else if (m_iMainDrawId  == MAINQUESTLEAVE)
				--m_iMainDrawId;
			else if(m_iMainDrawId == SUBQUEST)
				++m_iMainDrawId;
			else if (m_iMainDrawId == SUBQUESTLEAVE)
				--m_iMainDrawId;
			else if (m_iMainDrawId == SELECT_OK)
				++m_iMainDrawId;
			else if (m_iMainDrawId == SELECT_NO)
				--m_iMainDrawId;

			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UITalkSelect.ogg", CSound_Manager::ESoundID::UI, 1.f);
		}
		//if (pManagement->Key_Down(DIK_DOWN))
		//{
		//	if (m_iMainDrawId == MAINQUEST)
		//		++m_iMainDrawId;
		//	else if (m_iMainDrawId == MAINQUESTLEAVE)
		//		--m_iMainDrawId;
		//	else if (m_iMainDrawId == SUBQUEST)
		//		++m_iMainDrawId;
		//	else if (m_iMainDrawId == SUBQUESTLEAVE)
		//		--m_iMainDrawId;
		//}
	}
}

void CUI_NPC_Choron::Play_Script(const _float& _fDeltaTime)
{
	if(::CoolTime(_fDeltaTime, m_fScriptDelayTime, 0.5f))
	{
		m_fScriptDelayTime = 0.f;
		//m_fScriptDissolveTime = 0.f;
		m_bPlayScript = false;
		CUI_NPC_Choron::Event_Control(_fDeltaTime);
	}
	else
	{
		//스크립트 재생 m_iScriptFlag와 이벤트 플래그에 따라 다르게 재생 필요함.
		if(m_eUIMode == eUIMode::Talk)
			Script_UI_Control(_fDeltaTime);
	}
}

void CUI_NPC_Choron::Event_Control(const _float & _fDeltaTime)
{
	const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
	const _uint& iStatic = Get_EventMgr()->Get_CurStaticEvent();
	const _uint& iEvent = Get_EventMgr()->Get_CurEvent();


	if ((m_iScriptFlag == 5) && (iQuest & QUEST_START_1) && (iQuest & QUEST_END_1) && (!(iEvent &(EVENT_VOLATILE_QUEST_START_1 | EVENT_VOLATILE_QUEST_END_1))))
	{
		//Get_EventMgr()->Add_Event(EVENT_VOLATILE_QUEST_START_1);
	}
	if ((m_iScriptFlag == 6)
		&& (iQuest & QUEST_START_1)
		&& (iQuest & QUEST_END_1)
		&& !(iEvent &(EVENT_VOLATILE_QUEST_START_1))
		&& !(iEvent &(EVENT_VOLATILE_QUEST_END_1)))
	{
	/*	Get_EventMgr()->Add_Event(EVENT_VOLATILE_QUEST_START_1);
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
		pPlayer->Set_Npc_Interacting(false);
		m_bUIView = false;
		m_iScriptFlag = 0;
		m_pTransform->Set_Pos((GET_MANAGEMENT->Set_GlobalPos(L"Player", _float3(13.f, 1.992630f, -6.f))));*/
		m_eUIMode = eUIMode::Select;
		m_iMainDrawId = 5;
	}

	//if ((m_iScriptFlag == 5) && !(iQuest & QUEST_START_1))
	//	Get_EventMgr()->Add_Quest_Flag(QUEST_START_1);	//메인퀘 시작
	if ((m_iScriptFlag >= 6)
		&& !(iQuest & QUEST_START_1)
		&& !(iQuest& QUEST_END_1)
		&& !(iStatic & EVENT_STATIC_GET_QUEST_ITEM1))	//시작 후 ui닫힘.
	{
		//CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
		//pPlayer->Set_Npc_Interacting(false);
		//m_bUIView = false;
		//m_iScriptFlag = 0;
		m_eUIMode = eUIMode::Select;
		m_iMainDrawId = 5;
	}
	if ((m_iScriptFlag == 2)
		&& (iQuest & QUEST_START_1)
		&& !(iQuest& QUEST_END_1)
		&& !(iStatic & EVENT_STATIC_GET_QUEST_ITEM1))	//시작 후 ui닫힘.
	{
		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
		pPlayer->Set_Npc_Chrono(false);
		m_bUIView = false;
		m_iScriptFlag = 0;
	}
	if ((m_iScriptFlag == 4)
		&& (iQuest & QUEST_START_1)
		&& !(iQuest & QUEST_END_1)
		&& (iStatic & EVENT_STATIC_GET_QUEST_ITEM1))
	{
		Get_EventMgr()->Add_Quest_Flag(QUEST_END_1);
		
		//인벤토리에 아이템 삭제
		CGameObject* pUI = Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI");
		NULL_CHECK_RETURN(pUI, );
		dynamic_cast<CUI_Inventory*>(pUI)->Min_ItemCount(27);

		CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
		pPlayer->Set_Npc_Chrono(false);
		m_bUIView = false;
		m_iScriptFlag = 0;
		m_iMainDrawId = 0;
		m_iScriptId = 0;
	}
}

void CUI_NPC_Choron::Script_UI_Control(const _float & _fDeltaTime)
{
	const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
	const _uint& iStatic = Get_EventMgr()->Get_CurStaticEvent();

	//퀘스트 완료 후 반복퀘 수령
	if ((m_iScriptFlag == 2) && (iQuest & QUEST_START_1) && (iQuest & QUEST_END_1))
	{
		m_iScriptId = 8;
		return;
	}
	else if ((m_iScriptFlag == 3) && (iQuest & QUEST_START_1) && (iQuest & QUEST_END_1))
	{
		m_iScriptId = 9;
		return;
	}
	else if ((m_iScriptFlag == 4) && (iQuest & QUEST_START_1) && (iQuest & QUEST_END_1))
	{
		m_iScriptId = 10;
		return;
	}
	else if ((m_iScriptFlag == 5) && (iQuest & QUEST_START_1) && (iQuest & QUEST_END_1))
	{
		m_iScriptId = 11;
		return;
	}
	//퀘스트 수락 후 아이템을 가져왔을 때
	if ((m_iScriptFlag == 2) && (iQuest & QUEST_START_1) && !(iQuest & QUEST_END_1) && (iStatic & EVENT_STATIC_GET_QUEST_ITEM1))
	{
		m_iScriptId = 6;
		//return;
	}
	else if ((m_iScriptFlag == 3) && (iQuest & QUEST_START_1) && !(iQuest & QUEST_END_1) && (iStatic & EVENT_STATIC_GET_QUEST_ITEM1))
	{
		m_iScriptId = 7;
		return;
	}
	////퀘스트받을 때
	if ((m_iScriptFlag == 2) && !(iQuest & QUEST_START_1))
	{
		m_iScriptId = 0;
		return;
	}
	else 	if ((m_iScriptFlag == 3) && !(iQuest & QUEST_START_1))
	{
		m_iScriptId = 1;
		return;
	}
	else 	if ((m_iScriptFlag == 4) && !(iQuest & QUEST_START_1))
	{
		m_iScriptId = 2;
		return;
	}
	else 	if ((m_iScriptFlag == 5) && !(iQuest & QUEST_START_1))
	{
		m_iScriptId = 3;
		return;
	}

	if ((m_iScriptFlag == 1) && (iQuest & QUEST_START_1) && !(iQuest & QUEST_END_1) && !(iStatic & EVENT_STATIC_GET_QUEST_ITEM1))
	{
		if (m_iScriptId != 4 && m_iScriptId != 5)
			m_iScriptId = Mersen_ToInt(4, 5);
		return;
	}
	if (m_iScriptFlag == 1)
	{
		if (iQuest & QUEST_END_1)
			m_iScriptId = 1;
		else
			m_iScriptId = 0;
		return;
	}


}

CUI_NPC_Choron * CUI_NPC_Choron::Create(_DEVICE _pDevice)
{
	CUI_NPC_Choron* pInstance = new CUI_NPC_Choron(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_NPC_Choron::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_NPC_Choron* pInstance = new CUI_NPC_Choron(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

