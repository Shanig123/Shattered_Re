#include "stdafx.h"
#include "UI_Select_Upgrade.h"
#include "Player.h"
#include "Player_UI.h"
#include "Component_Manager.h"

CUI_Select_Upgrade::CUI_Select_Upgrade(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_pSelectTexture(nullptr)
	, m_pTextTexture(nullptr)
	, m_bUIView(false)
	, m_iUIDiffuse(0)
	, m_iOptionNumber(0)
	, m_pForge(nullptr)
	, m_fBackDissolveTime(0.f)
	, m_bSelected(false)
	, m_iMainRenderPass(0)
{
}

CUI_Select_Upgrade::CUI_Select_Upgrade(const CUI_Select_Upgrade & _rhs)
	: CUIBase(_rhs)
	, m_pSelectTexture(nullptr)
	, m_pTextTexture(nullptr)
	, m_bUIView(false)
	, m_iUIDiffuse(0)
	, m_iOptionNumber(0)
	, m_pForge(nullptr)
	, m_fBackDissolveTime(0.f)
	, m_bSelected(false)
	, m_iMainRenderPass(0)
{
}

HRESULT CUI_Select_Upgrade::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();

}

HRESULT CUI_Select_Upgrade::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	
	memcpy(&m_tUIDesc, _pArg, sizeof(CUIBase::UI_DESC));

	CManagement* pManagement = GET_MANAGEMENT;


	FAILED_CHECK_RETURN(CUI_Select_Upgrade::Add_Com(), E_FAIL);

	CUIBase::UI_DESC tDesc;
	tDesc.vPos = { 0.f,0.f,1.f };
	tDesc.vSize = { _float(WINCX),_float(WINCY) };
	_tchar szBuff[MAX_PATH] = L"Component_Texture_Status_Back";
	lstrcpy(tDesc.szTexName, szBuff);

	_tchar szBuff2[MAX_PATH] = L"Component_Texture_ForgeBack";

	lstrcpy(tDesc.szTexName, szBuff2);

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_Forge_UI",
		(CGameObject**)&m_pForge, &tDesc), E_FAIL);

	m_PrototypeName = L"GameObject_Select_Upgrader";

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pForge, m_iSceneIdx,
		L"Layer_NPCUI"), E_FAIL);

	Safe_AddRef(m_pForge);

	return S_OK;
}

_uint CUI_Select_Upgrade::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	CUIBase::Update_GameObject(_fDeltaTime);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));

	if (pPlayer->Get_LevelStone_Interact() && !m_pForge->Get_View() && !pPlyUI->Get_StatusUI()->Get_CheckViewStatus())
	{
		if (!m_bUIView)
		{
			m_bSelected = true;
			m_bUIView = true;
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
			pPlayer->Set_Npc_LevelStone(true);
		}		
	}
	if (m_pForge->Get_View() || pPlyUI->Get_StatusUI()->Get_CheckViewStatus() || m_bUIView)
	{
		if (m_fBackDissolveTime < 8.f)
			m_fBackDissolveTime += _fDeltaTime * powf(1.5f, m_fBackDissolveTime);

		pPlyUI->Set_ViewInterface(false);
		KeyInput();
	}
	else
	{
	/*	if (!pPlyUI->Get_CheckViewInterface() && !m_pForge->Get_View() && !pPlyUI->Get_StatusUI()->Get_View())
		{
			pPlyUI->Set_ViewInterface(true);
		}*/
	}

	return m_iEvent;
}

_uint CUI_Select_Upgrade::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_Select_Upgrade::Render_GameObject()
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

	if (FAILED(SetUp_ConstantTable_Upgrage(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		Safe_Release(pEffect);

		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);

	if (m_fBackDissolveTime < 8.f)
		m_iMainRenderPass = 4;
	else
		m_iMainRenderPass = 19;

	pEffect->BeginPass(m_iMainRenderPass);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(pEffect);

		return E_FAIL;
	}

	pEffect->EndPass();
	pEffect->End();

	if (m_fBackDissolveTime > .5f)
		Render_Option(pEffect);

	Safe_Release(pEffect);

	return S_OK;
}

HRESULT CUI_Select_Upgrade::SetUp_ConstantTable_Upgrage(_EFFECT & _pEffect)
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
	_pEffect->SetVector("g_vColor", &_float4(0.f, 0.f, 0.f, 0.f));
	GET_MANAGEMENT->Set_Texture(0, m_tUIDesc.szTexName, _pEffect, "g_BaseTexture", m_iOptionNumber);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", 0);

	return S_OK;
}

void CUI_Select_Upgrade::Select_Upgrade()
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));

	switch (m_iOptionNumber)
	{
	case 0:
		pPlyUI->Get_StatusUI()->Set_UpgradeMode();
		pPlyUI->Get_StatusUI()->Start_StatusUpgrade();
		m_bUIView = false;
		break;
	case 1:
		m_pForge->Start_WeaponUpgrad();
		m_bUIView = false;
		break;
	default:
		break;
	}
}

void CUI_Select_Upgrade::KeyInput()
{
	CManagement* pManagement = GET_MANAGEMENT;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));

	if (m_bUIView)
	{

		if (CManagement::Get_Instance()->Key_Down(DIK_A))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UISwitch.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iOptionNumber == 0)
			{
				m_iOptionNumber = 1;
				return;
			}
			else
			{
				m_iOptionNumber = 0;
				return;
			}			
		}
		if (CManagement::Get_Instance()->Key_Down(DIK_D))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UISwitch.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_iOptionNumber == 1)
			{
				m_iOptionNumber = 0;
				return;
			}
			else
			{
				m_iOptionNumber = 1;
				return;
			}			
		}
		if (pManagement->Key_Down(DIK_RETURN))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
			Select_Upgrade();
		}
		if (pManagement->Key_Down(DIK_ESCAPE))
		{
			m_iOptionNumber = 0;
			pPlayer->Set_Npc_LevelStone(false);
			pPlyUI->Set_ViewInterface(true);
			m_fBackDissolveTime = 0.f;
			m_bUIView = false;
			m_bSelected = false;
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
		}
	}
}

HRESULT CUI_Select_Upgrade::Render_Option(_EFFECT & _pEffect)
{
	_float4x4 matOption;
	_float3 m_vScale;
	D3DXMatrixIdentity(&matOption);

	for (_uint i = 0; i < 2; i++)
	{
		_uint iTexIndex = i;

		if (m_iOptionNumber == i)
		{
			iTexIndex += 2;
			m_vScale = { 300.f,300.f ,300.f };
		}
		else
		{
			m_vScale = { 200.f,200.f ,200.f };
		}

		matOption.m[0][0] = m_vScale.x;
		matOption.m[1][1] = m_vScale.y;
		matOption.m[2][2] = m_vScale.z;

		memcpy(&matOption.m[3][0],
			_float3(-290.f + (i*565.f),
				0.f,
				1.f), sizeof(_float3));
		/*셰이더 값 설정 코드 필요함.*/
		_pEffect->SetMatrix("g_matWorld", &matOption);

		m_pSelectTexture->Set_Texture(_pEffect, "g_BaseTexture", iTexIndex);
		_uint	iPassMax = 0;
		_pEffect->Begin(&iPassMax, 0);
		_pEffect->BeginPass(0);
		if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}
		_pEffect->CommitChanges();
		_pEffect->EndPass();
		_pEffect->End();

	}
	return S_OK;
}

HRESULT CUI_Select_Upgrade::Add_Com()
{
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Select_Upgrade",
		L"Com_SlotTexture", EComponentType::Static, (CComponent**)&m_pSelectTexture), E_FAIL);
	
	//FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Item_Icon",
	//	L"Com_IconTexture", EComponentType::Static, (CComponent**)&m_pTextTexture), E_FAIL);

	return S_OK;
}

CUI_Select_Upgrade * CUI_Select_Upgrade::Create(_DEVICE _pDevice)
{
	CUI_Select_Upgrade* pInstance = new CUI_Select_Upgrade(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_Select_Upgrade::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_Select_Upgrade* pInstance = new CUI_Select_Upgrade(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

void CUI_Select_Upgrade::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pForge);
	}
	CUIBase::Free();
}
