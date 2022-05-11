#include "stdafx.h"
#include "Player_UI.h"
#include "UI_Inventory.h"
#include "UI_Equipment.h"
#include "UI_Status.h"
#include "Player.h"
#include "MainCamera.h"

CPlayer_UI::CPlayer_UI(_DEVICE _pDevice)
	: CUIBase(_pDevice)
	, m_bViewInterface(false)
	, m_pIconTexture(nullptr)
	, m_pQuickEquip(nullptr), m_pQuickItem(nullptr)
	, m_pInvenUI(nullptr), m_pEquipUI(nullptr), m_pStatusUI(nullptr)
	, m_pNumTexture(nullptr)
	, m_bViewMenu(false)
	, m_fHPSpectrum(0.f)
	, m_fSTSpectrum(0.f)
	, m_fDeltaTime(0.f)
	, m_eMenu(MENU_END)
	, m_iSoul(0)
	, m_eInteract(INTERACT::INTER_END)
	, m_fActTime(0.f)
	, m_iGetItem(0)
	, m_bGetItem(false)
	, m_bOnRenderGetItem(false)
	, m_fGet_ItemTime(0.f)
	, m_iPreArea(0), m_iCurArea(0)
	, m_bNewArea(false), m_fAreaTime(0.f)
	, m_fWaitTime(0.f),m_bAreaWait(false)
{
}

CPlayer_UI::CPlayer_UI(const CPlayer_UI & _rOther)
	: CUIBase(_rOther)
	, m_bViewInterface(false)
	, m_pIconTexture(nullptr)
	, m_pQuickEquip(nullptr), m_pQuickItem(nullptr)
	, m_pInvenUI(nullptr), m_pEquipUI(nullptr), m_pStatusUI(nullptr)
	, m_pNumTexture(nullptr)
	, m_bViewMenu(false)
	, m_fHPSpectrum(0.f)
	, m_fSTSpectrum(0.f),m_fAccTime(0.f)
	, m_fDeltaTime(0.f)
	, m_eMenu(MENU_END)
	, m_iSoul(_rOther.m_iSoul)
	, m_eInteract(INTERACT::INTER_END)
	, m_fActTime(0.f)
	, m_iGetItem(0)
	, m_bGetItem(false)
	, m_bOnRenderGetItem(false)
	, m_fGet_ItemTime(0.f)
	, m_iPreArea(0), m_iCurArea(0)
	, m_bNewArea(false), m_fAreaTime(0.f)
	, m_fWaitTime(0.f), m_bAreaWait(false)
{
	m_PrototypeName = L"Player_UI";
}

HRESULT CPlayer_UI::Ready_GameObject_Prototype()
{
	if (FAILED(CGameObject::Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CPlayer_UI::Ready_GameObject_Clone(const _uint& _iSceneIdx, void * _pArg/* = nullptr*/)
{
	if (FAILED(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Ready_GameObject_Clone");
		return E_FAIL;
	}

	if (FAILED(CPlayer_UI::Add_Com()))
	{
		ERROR_BOX(L"Failed to Add_Component");
		return E_FAIL;
	}
	CManagement* pManagement = GET_MANAGEMENT;

	
	
	CUIBase::UI_DESC tDesc;
	tDesc.vPos = { _float(0),_float(0) ,1.f };
	tDesc.vSize = { _float(WINCX),_float(WINCY) };
	lstrcpy(tDesc.szTexName, L"Component_Texture_InvenBack");

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_UI_Player_Inventory", (CGameObject**)&m_pInvenUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pInvenUI, m_iSceneIdx, L"Layer_PlayerUI"), E_FAIL);
	Safe_AddRef(m_pInvenUI);

	// m_pInvenUI->Add_ItemCount((_uint)eItemID::Heal_Small);

	lstrcpy(tDesc.szTexName, L"Component_Texture_EquipBack");

	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_UI_Player_Equipment", (CGameObject**)&m_pEquipUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pEquipUI, m_iSceneIdx, L"Layer_PlayerUI"), E_FAIL);
	Safe_AddRef(m_pEquipUI);

	//m_pEquipUI->Add_ItemCount((_uint)eWeaponID::Weapon_1);

	lstrcpy(tDesc.szTexName, L"Component_Texture_Status_Back");
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject(0, L"GameObject_Status_UI",(CGameObject**)&m_pStatusUI, &tDesc), E_FAIL);
	FAILED_CHECK_RETURN(pManagement->Clone_GameObject_ToLayer(m_pStatusUI, m_iSceneIdx, L"Layer_PlayerUI"), E_FAIL);
	Safe_AddRef(m_pStatusUI);

	
	return S_OK;
}

_uint CPlayer_UI::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	m_fDeltaTime = _fDeltaTime;
	m_fAccTime += m_fDeltaTime*0.1f;
	KeyInput();
	AddItem();
	CheckQuickItem();
	Update_GetItem(_fDeltaTime);
	Update_Interact(_fDeltaTime);
	Update_Area(_fDeltaTime);
	/*if(!m_bViewInterface)
		return (_uint)EEvent::None;*/


	m_tState = pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player", EFindGameObject::Proto, L"Player")->Get_Stat();
	m_pStatusUI->Set_Real_Status(m_tState);
	m_pStatusUI->Set_Cur_Soul(m_iSoul);
	m_iSoul = m_pStatusUI->Get_Soul();

	CGameObject::Update_GameObject(_fDeltaTime);

	if (nullptr == pManagement)
	{
		ERROR_BOX(L"pManagement is nullptr");
		return 0;
	}

	if (FAILED(pManagement->Add_RenderList(ERenderPriority::UIOrtho, this)))
	{
		ERROR_BOX(L"Failed to Add_RenderList");
		return 0;
	}

	m_pTransform->Update_Transform();
	return (_uint)EEvent::None;
}

HRESULT CPlayer_UI::Render_GameObject()
{ 
	if (Get_EventMgr()->Get_CurEvent()&EVENT_VOLATILE_ENDING)
	{
		return S_OK;
	}
	if (FAILED(m_pTransform->Set_Transform()))
	{
		ERROR_BOX(L"Failed to Set_Transform");
		return E_FAIL;
	}

	_EFFECT pEffect = m_pShader->Get_EffectHandle();
	pEffect->AddRef();

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		ERROR_BOX(L"failed to SetUp_ConstantTable");
		return E_FAIL;
	}

	_uint	iPassMax = 0;

	pEffect->Begin(&iPassMax, 0);
	pEffect->BeginPass(0);

	if (m_bViewInterface && !m_pEquipUI->Get_CheckViewEquip() && !m_pInvenUI->Get_CheckViewInven() &&!m_pStatusUI->Get_CheckViewStatus())
	{
		if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			return E_FAIL;
		}
	}

	pEffect->EndPass();
	pEffect->End();

	if (m_bViewInterface && !m_pEquipUI->Get_CheckViewEquip() && !m_pInvenUI->Get_CheckViewInven() && !m_pStatusUI->Get_CheckViewStatus())
	{
		Render_ItemIcon(pEffect);
		Render_HP_Bar(pEffect);
		Render_Stamina_Bar(pEffect);
		Render_Soul(pEffect);
	}

	if (m_bViewMenu && !m_pEquipUI->Get_CheckViewEquip() && !m_pInvenUI->Get_CheckViewInven() && !m_pStatusUI->Get_CheckViewStatus())
	{
		Render_Menu(pEffect);
		Render_Menu_Icon(pEffect);
	}

	if (m_bViewInterface && m_fActTime > 0.f)
		Render_Interact_Icon(pEffect);

	if (m_bViewInterface && m_bGetItem)
		Render_Get_Item(pEffect);

	if (m_fAreaTime != 0.f && !m_bViewMenu)
		Render_New_Area(pEffect);


	Safe_Release(pEffect);
	return S_OK;
}

void CPlayer_UI::SwapWeapon()
{
	m_pEquipUI->SwapQuickSlot();
}

_bool CPlayer_UI::Use_Item()
{
	return	m_pInvenUI->Min_QucikItemCount();
}

_bool CPlayer_UI::Add_Equip_Item(const eWeaponID& _eWeaponID)
{
	return m_pEquipUI->Add_ItemCount((_ubyte)_eWeaponID);
}

_bool CPlayer_UI::Add_Used_Item(const eItemID & _eItemID)
{
	return m_pInvenUI->Add_ItemCount((_ubyte)_eItemID);
}

HRESULT CPlayer_UI::Add_Com()
{
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Item_Icon",
		L"Com_IconTexture", EComponentType::Static, (CComponent**)&m_pIconTexture), E_FAIL);

	/*FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Player_Gage",
		L"Com_GageTexture", EComponentType::Static, (CComponent**)&m_pGageTexture), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Player_HP",
		L"Com_HPTexture", EComponentType::Static, (CComponent**)&m_pHPBarTexture), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Player_Stamina",
		L"Com_StaminaTexture", EComponentType::Static, (CComponent**)&m_pStaminaBarTexture), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Number",
		L"Com_NumberTexture", EComponentType::Static, (CComponent**)&m_pNumTexture), E_FAIL);

*/
	
	return S_OK;
}

HRESULT CPlayer_UI::Render_ItemIcon(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);
	matSlotWorld.m[0][0] = 65.f;
	matSlotWorld.m[1][1] = matSlotWorld.m[0][0];
	matSlotWorld.m[2][2] = matSlotWorld.m[0][0];
	memcpy(&matSlotWorld.m[3][0],
		_float3(70.f-(WINCX>>1),
			115.f - (WINCY >> 1),
			1.f), sizeof(_float3));


	if (m_pQuickEquip && m_pQuickEquip->ubyItem_ID)	//무기
	{
		_pEffect->SetMatrix("g_matWorld", &matSlotWorld);
		m_pIconTexture->Set_Texture(_pEffect, "g_BaseTexture", (m_pQuickEquip->ubyItem_ID) - 1);

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
	matSlotWorld.m[0][0] = 70.f;
	matSlotWorld.m[1][1] = matSlotWorld.m[0][0];
	matSlotWorld.m[2][2] = matSlotWorld.m[0][0];
	memcpy(&matSlotWorld.m[3][0],
		_float3(150.f - (WINCX >> 1),
			115.f - (WINCY >> 1),
			1.f), sizeof(_float3));

	if (m_pQuickItem && m_pQuickItem->ubyItem_ID)	//소모품
	{
		_pEffect->SetMatrix("g_matWorld", &matSlotWorld);
		m_pIconTexture->Set_Texture(_pEffect, "g_BaseTexture", (m_pQuickItem->ubyItem_ID) - 1);

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

HRESULT CPlayer_UI::Render_HP_Bar(_EFFECT & _pEffect)
{
	_float fGageSizeX = 2.f * m_tState.fMaxHP;
	_float fHPSizeX = 2.f * m_tState.fHP;
	if (m_fHPSpectrum <= fHPSizeX)
		m_fHPSpectrum = fHPSizeX;

	// 게이지
	_float4x4 matGageWorld;
	D3DXMatrixIdentity(&matGageWorld);
	matGageWorld.m[0][0] = fGageSizeX;
	matGageWorld.m[1][1] = 21.f;
	matGageWorld.m[2][2] = 0.f;
	memcpy(&matGageWorld.m[3][0],
		_float3(80.f + (fGageSizeX * 0.5f) - (WINCX >> 1),
			680.f - (WINCY >> 1),
			1.f), sizeof(_float3));


	_pEffect->SetMatrix("g_matWorld", &matGageWorld);
	//m_pGageTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Gage", _pEffect, "g_BaseTexture", 0);

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(2);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	// 게이지


	// 잔상
	if (m_fHPSpectrum > fHPSizeX)
		m_fHPSpectrum -= 40.f * m_fDeltaTime;

	_float4x4 matSpecWorld;
	D3DXMatrixIdentity(&matSpecWorld);
	matSpecWorld.m[0][0] = m_fHPSpectrum;
	matSpecWorld.m[1][1] = 16.f;
	matSpecWorld.m[2][2] = 0.f;
	memcpy(&matSpecWorld.m[3][0],
		_float3(80.f + (m_fHPSpectrum * 0.5f) - (WINCX >> 1),
			680.f - (WINCY >> 1),
			1.f), sizeof(_float3));


	_pEffect->SetMatrix("g_matWorld", &matSpecWorld);
	//m_pGageSpectrumTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Gage_Spectrum", _pEffect, "g_BaseTexture", 0);

	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(2);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	// 잔상



	_float4x4 matHPWorld;
	D3DXMatrixIdentity(&matHPWorld);
	matHPWorld.m[0][0] = fHPSizeX;
	matHPWorld.m[1][1] = 18.f;
	matHPWorld.m[2][2] = 0.f;
	memcpy(&matHPWorld.m[3][0],
		_float3(80.f + (fHPSizeX * 0.5f) - (WINCX >> 1),
			679.f - (WINCY >> 1),
			1.f), sizeof(_float3));

	
	_pEffect->SetMatrix("g_matWorld", &matGageWorld);
	//m_pHPBarTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	_pEffect->SetFloat("g_ftime", m_fAccTime*0.5f);
	_pEffect->SetFloat("g_fU", fHPSizeX / fGageSizeX);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_HP", _pEffect, "g_BaseTexture", 2);

	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(16);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();



	//----------------------------Flow_UV--------------------------------------------//

	_pEffect->SetMatrix("g_matWorld", &matGageWorld);
	//m_pGageTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	_pEffect->SetFloat("g_ftime",- m_fAccTime);
	//_pEffect->SetVector("g_vColor", &_float4(0.258823f, 0.058823f, 0.109803f, 2.2f));
	//_pEffect->SetVector("g_vColor", &_float4(0.258823f, 0.058823f, 0.109803f, 1.5f));
	_pEffect->SetVector("g_vColor", &_float4(0.158823f, 0.058823f, 0.058823f, 2.f));
	_pEffect->SetFloat("g_fU", fHPSizeX / fGageSizeX);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Wire", _pEffect, "g_FlowTexture", 3);

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(17);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	//----------------------------Flow_UV--------------------------------------------//

	return S_OK;
}

HRESULT CPlayer_UI::Render_Stamina_Bar(_EFFECT & _pEffect)
{
	_float fGageSizeX = 2.f * m_tState.fMaxStamina;
	_float fStaminaSizeX = 2.f * m_tState.fStamina;
	if (m_fSTSpectrum <= fStaminaSizeX)
		m_fSTSpectrum = fStaminaSizeX;


	_float4x4 matGageWorld;
	D3DXMatrixIdentity(&matGageWorld);
	matGageWorld.m[0][0] = fGageSizeX;
	matGageWorld.m[1][1] = 21.f;
	matGageWorld.m[2][2] = 0.f;
	memcpy(&matGageWorld.m[3][0],
		_float3(80.f + (fGageSizeX * 0.5f) - (WINCX >> 1),
			660.f - (WINCY >> 1),
			1.f), sizeof(_float3));

	
	_pEffect->SetMatrix("g_matWorld", &matGageWorld);
//	m_pGageTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Gage", _pEffect, "g_BaseTexture", 0);

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(2);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	




	// 잔상
	if (m_fSTSpectrum > fStaminaSizeX)
		m_fSTSpectrum -= 40.f * m_fDeltaTime;

	_float4x4 matSpecWorld;
	D3DXMatrixIdentity(&matSpecWorld);
	matSpecWorld.m[0][0] = m_fSTSpectrum;
	matSpecWorld.m[1][1] = 18.f;
	matSpecWorld.m[2][2] = 0.f;
	memcpy(&matSpecWorld.m[3][0],
		_float3(80.f + (m_fSTSpectrum * 0.5f) - (WINCX >> 1),
			660.f - (WINCY >> 1),
			1.f), sizeof(_float3));


	_pEffect->SetMatrix("g_matWorld", &matSpecWorld);
	//m_pGageSpectrumTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Gage_Spectrum", _pEffect, "g_BaseTexture", 0);

	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(2);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	// 잔상






	_float4x4 matStaminaWorld;
	D3DXMatrixIdentity(&matStaminaWorld);
	matStaminaWorld.m[0][0] = fStaminaSizeX;
	matStaminaWorld.m[1][1] = 18.f;
	matStaminaWorld.m[2][2] = 0.f;
	memcpy(&matStaminaWorld.m[3][0],
		_float3(80.f + (fStaminaSizeX * 0.5f) - (WINCX >> 1),
			659.f - (WINCY >> 1),
			1.f), sizeof(_float3));


	_pEffect->SetMatrix("g_matWorld", &matGageWorld);
//	m_pStaminaBarTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	_pEffect->SetFloat("g_ftime", m_fAccTime * 1.1f);
	_pEffect->SetFloat("g_fU", fStaminaSizeX/ fGageSizeX);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Stamina", _pEffect, "g_BaseTexture", 2);

	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(16);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();


	//----------------------------Flow_UV--------------------------------------------//

	_pEffect->SetMatrix("g_matWorld", &matGageWorld);
	//m_pGageTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	_pEffect->SetFloat("g_ftime", -(m_fAccTime*0.5f)*1.1f);
	_pEffect->SetVector("g_vColor", &_float4(0.1f, 0.05f, 0.1f,1.f));
	_pEffect->SetFloat("g_fU", fStaminaSizeX / fGageSizeX);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Wire", _pEffect, "g_FlowTexture", 3);

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(17);
	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	//----------------------------Flow_UV--------------------------------------------//

	return S_OK;
}

HRESULT CPlayer_UI::Render_Soul(_EFFECT & _pEffect)
{
	_uint iSoul = m_iSoul;
	_float fSizeX, fSizeY;
	fSizeX = 10.f;
	fSizeY = 16.f;
	_float4x4 matOneWorld;
	D3DXMatrixIdentity(&matOneWorld);
	matOneWorld.m[0][0] = fSizeX;
	matOneWorld.m[1][1] = fSizeY;
	matOneWorld.m[2][2] = 0.f;
	memcpy(&matOneWorld.m[3][0],
		_float3(150.f - (WINCX >> 1),
			620.f - (WINCY >> 1),
			1.f), sizeof(_float3));

	_uint iOneSoul = iSoul % 10;

	_pEffect->SetMatrix("g_matWorld", &matOneWorld);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iOneSoul);


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




	if (m_iSoul >= 10)
	{
		_float4x4 matTenWorld;
		D3DXMatrixIdentity(&matTenWorld);
		matTenWorld.m[0][0] = fSizeX;
		matTenWorld.m[1][1] = fSizeY;
		matTenWorld.m[2][2] = 0.f;
		memcpy(&matTenWorld.m[3][0],
			_float3((150.f - 11.f) - (WINCX >> 1),
				620.f - (WINCY >> 1),
				1.f), sizeof(_float3));

		iSoul = m_iSoul;
		iSoul = iSoul % 100;
		_uint iTenSoul = iSoul / 10;


		_pEffect->SetMatrix("g_matWorld", &matTenWorld);
		//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iTenSoul);
		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iTenSoul);

		iPassMax = 0;

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

		if (m_iSoul >= 100)
		{
			_float4x4 matHunWorld;
			D3DXMatrixIdentity(&matHunWorld);
			matHunWorld.m[0][0] = fSizeX;
			matHunWorld.m[1][1] = fSizeY;
			matHunWorld.m[2][2] = 0.f;
			memcpy(&matHunWorld.m[3][0],
				_float3((150.f - 22.f) - (WINCX >> 1),
					620.f - (WINCY >> 1),
					1.f), sizeof(_float3));

			iSoul = m_iSoul;
			iSoul = iSoul % 1000;
			_uint iHunSoul = iSoul / 100;

			_pEffect->SetMatrix("g_matWorld", &matHunWorld);
			//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iHunSoul);
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iHunSoul);

			iPassMax = 0;

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


			if (m_iSoul >= 1000)
			{
				_float4x4 matColWorld;
				D3DXMatrixIdentity(&matColWorld);
				matColWorld.m[0][0] = fSizeX;
				matColWorld.m[1][1] = fSizeY;
				matColWorld.m[2][2] = 0.f;
				memcpy(&matColWorld.m[3][0],
					_float3((150.f - 31.f) - (WINCX >> 1),
						618.f - (WINCY >> 1),
						1.f), sizeof(_float3));

				

				_pEffect->SetMatrix("g_matWorld", &matColWorld);
				//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", 10);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", 10);

				iPassMax = 0;

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




				_float4x4 matSouWorld;
				D3DXMatrixIdentity(&matSouWorld);
				matSouWorld.m[0][0] = fSizeX;
				matSouWorld.m[1][1] = fSizeY;
				matSouWorld.m[2][2] = 0.f;
				memcpy(&matSouWorld.m[3][0],
					_float3((150.f - 36.f) - (WINCX >> 1),
						620.f - (WINCY >> 1),
						1.f), sizeof(_float3));

				iSoul = m_iSoul;
				iSoul = iSoul % 10000;
				_uint iSouSoul = iSoul / 1000;

				_pEffect->SetMatrix("g_matWorld", &matSouWorld);
				//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iSouSoul);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iSouSoul);

				iPassMax = 0;

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
		}
	}
	

	return S_OK;
}

HRESULT CPlayer_UI::Render_Menu(_EFFECT & _pEffect)
{
	_float4x4 matMenuWorld;
	D3DXMatrixIdentity(&matMenuWorld);
	matMenuWorld.m[0][0] = 1280.f;
	matMenuWorld.m[1][1] = 720.f;
	matMenuWorld.m[2][2] = 0.f;
	memcpy(&matMenuWorld.m[3][0],
		_float3(0.,0.f,1.f), sizeof(_float3));

	_pEffect->SetMatrix("g_matWorld", &matMenuWorld);
	
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Menu", _pEffect, "g_BaseTexture", m_eMenu);

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(2);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();

	return S_OK;
}

HRESULT CPlayer_UI::Render_Menu_Icon(_EFFECT & _pEffect)
{
	_float3 vBackPos,vStatPos,vWeaponPos,vInvenPos, vIconSize;
	_uint iStatIcon, iWeaponIcon, iInvenIcon;
	switch (m_eMenu)
	{
	case CPlayer_UI::MENU_INVEN:
		vBackPos = _float3(0 - (162.f * 0.5f) - 19.f, 60.f - (163.f * 0.5f) - 19.f, 1.f);
		iInvenIcon = 0;
		iStatIcon = 3;
		iWeaponIcon = 5;
		break;
	case CPlayer_UI::MENU_STAT:
		vBackPos = _float3(0, 60.f, 1.f);
		iInvenIcon = 1;
		iStatIcon = 2;
		iWeaponIcon = 5;
		break;
	case CPlayer_UI::MENU_WEAPON:
		vBackPos = _float3(0 + (162.f * 0.5f) + 19.f, 60.f + (163.f * 0.5f) + 19.f, 1.f);
		iInvenIcon = 1;
		iStatIcon = 3;
		iWeaponIcon = 4;
		break;
	}


	
	
	_float4x4 matBackWorld;
	D3DXMatrixIdentity(&matBackWorld);
	matBackWorld.m[0][0] = 162.f;
	matBackWorld.m[1][1] = 163.f;
	matBackWorld.m[2][2] = 0.f;
	memcpy(&matBackWorld.m[3][0],&vBackPos, sizeof(_float3));

	//아이콘 백
	_pEffect->SetMatrix("g_matWorld", &matBackWorld);
//	m_pMenuIconBackTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Menu_Icon_Back", _pEffect, "g_BaseTexture");

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(8);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();

	vStatPos = _float3(0, 60.f, 1.f);
	vWeaponPos = _float3(0 + (162.f * 0.5f) + 19.f, 60.f + (163.f * 0.5f) + 19.f, 1.f);
	vInvenPos = _float3(0 - (162.f * 0.5f) - 19.f, 60.f - (163.f * 0.5f) - 19.f, 1.f);
	vIconSize = _float3(155.f, 155.f, 1.f);

	_float4x4 matStatWorld;
	D3DXMatrixIdentity(&matStatWorld);
	matStatWorld.m[0][0] = vIconSize.x;
	matStatWorld.m[1][1] = vIconSize.y;
	matStatWorld.m[2][2] = vIconSize.z;
	memcpy(&matStatWorld.m[3][0], &vStatPos, sizeof(_float3));
	// 아이콘 스텟
	_pEffect->SetMatrix("g_matWorld", &matStatWorld);
	//m_pMenuIconTexture->Set_Texture(_pEffect, "g_BaseTexture", iStatIcon);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Menu_Icon", _pEffect, "g_BaseTexture", iStatIcon);

	iPassMax = 0;

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


	_float4x4 matInvenWorld;
	D3DXMatrixIdentity(&matInvenWorld);
	matInvenWorld.m[0][0] = vIconSize.x;
	matInvenWorld.m[1][1] = vIconSize.y;
	matInvenWorld.m[2][2] = vIconSize.z;
	memcpy(&matInvenWorld.m[3][0], &vInvenPos, sizeof(_float3));
	//아이콘 인벤
	_pEffect->SetMatrix("g_matWorld", &matInvenWorld);
	//m_pMenuIconTexture->Set_Texture(_pEffect, "g_BaseTexture", iInvenIcon);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Menu_Icon", _pEffect, "g_BaseTexture", iInvenIcon);

	iPassMax = 0;

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

	_float4x4 matWeaponWorld;
	D3DXMatrixIdentity(&matWeaponWorld);
	matWeaponWorld.m[0][0] = vIconSize.x;
	matWeaponWorld.m[1][1] = vIconSize.y;
	matWeaponWorld.m[2][2] = vIconSize.z;
	memcpy(&matWeaponWorld.m[3][0], &vWeaponPos, sizeof(_float3));
	// 아이콘 웨폰
	_pEffect->SetMatrix("g_matWorld", &matWeaponWorld);
//	m_pMenuIconTexture->Set_Texture(_pEffect, "g_BaseTexture", iWeaponIcon);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Player_Menu_Icon", _pEffect, "g_BaseTexture", iWeaponIcon);

	iPassMax = 0;

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

	return S_OK;
}

HRESULT CPlayer_UI::Render_Interact_Icon(_EFFECT & _pEffect)
{
	if (m_eInteract != INTERACT::INTER_END)
		m_iInteract = (_uint)m_eInteract;

	_float4x4 matBackWorld;
	D3DXMatrixIdentity(&matBackWorld);
	matBackWorld.m[0][0] = 194.f;
	matBackWorld.m[1][1] = 28.f;
	matBackWorld.m[2][2] = 0.f;
	memcpy(&matBackWorld.m[3][0], _float3(0.f, -225.f, 1.f), sizeof(_float3));

	//아이콘 백
	_pEffect->SetMatrix("g_matWorld", &matBackWorld);
	//	m_pMenuIconBackTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Interact", _pEffect, "g_BaseTexture", m_iInteract);
	_pEffect->SetFloat("g_ftime", m_fActTime);
	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(7);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	return S_OK;
}

HRESULT CPlayer_UI::Render_Get_Item(_EFFECT & _pEffect)
{
	_float4x4 matBackWorld;
	D3DXMatrixIdentity(&matBackWorld);
	matBackWorld.m[0][0] = 358.f;
	matBackWorld.m[1][1] = 64.f;
	matBackWorld.m[2][2] = 0.f;
	memcpy(&matBackWorld.m[3][0], _float3(0.f, -161.f, 1.f), sizeof(_float3));

	//아이콘 백
	_pEffect->SetMatrix("g_matWorld", &matBackWorld);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Item_MSG", _pEffect, "g_BaseTexture", m_iGetItem);
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


	return S_OK;
}

HRESULT CPlayer_UI::Render_New_Area(_EFFECT & _pEffect)
{
	_float4x4 matBackWorld;
	D3DXMatrixIdentity(&matBackWorld);
	matBackWorld.m[0][0] = WINCX;
	matBackWorld.m[1][1] = WINCY;
	matBackWorld.m[2][2] = 0.f;
	memcpy(&matBackWorld.m[3][0], _float3(0.f, 100.f, 1.f), sizeof(_float3));

	//아이콘 백
	_pEffect->SetMatrix("g_matWorld", &matBackWorld);
	//	m_pMenuIconBackTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Area", _pEffect, "g_BaseTexture", m_iAreaCount);
	_pEffect->SetFloat("g_ftime", m_fAreaTime);
	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(7);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}
	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	return S_OK;
}



void CPlayer_UI::KeyInput()
{
	if (CManagement::Get_Instance()->Key_Down(DIK_ESCAPE))
	{
		Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
		Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);

		//if (!m_bViewMenu && !m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip())
		if (m_bViewInterface && !m_bViewMenu && !m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip())
		{
			m_bViewMenu = true;
			m_bViewInterface = false;
			m_eMenu = MENU_STAT;
		}
		else if (m_bViewMenu && !m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip())
		{
			m_bViewMenu = false;
			m_bViewInterface = true;
			m_eMenu = MENU_STAT;
		}

		if (m_bViewMenu && m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip() && !m_pStatusUI->Get_CheckViewStatus())
		{
			m_pInvenUI->Set_ViewInven(false);
			//m_pEquipUI->Set_ViewEquip(false);
			//m_bViewInterface = (true);
		}
		else if (m_bViewMenu && !m_pInvenUI->Get_CheckViewInven() && m_pEquipUI->Get_CheckViewEquip() && !m_pStatusUI->Get_CheckViewStatus())
		{
			//m_pInvenUI->Set_ViewInven(false);
			m_pEquipUI->Set_ViewEquip(false);
			//m_bViewInterface = (true);
		}
		//else if (m_bViewMenu && !m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip() && m_pStatusUI->Get_CheckViewStatus())
		//{
		//	//m_pInvenUI->Set_ViewInven(false);
		//	//m_bViewInterface = (true);
		//	m_pStatusUI->Set_ViewStatus(false);
		//}
	}
	if (m_bViewMenu && !m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip() && !m_pStatusUI->Get_CheckViewStatus())
	{
		if (GET_MANAGEMENT->Key_Down(DIK_A))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UISwitch.ogg", CSound_Manager::ESoundID::UI, 1.f);

			_int iMenu = m_eMenu;
			iMenu -= 1;
			if (iMenu <= -1)
				iMenu = 2;
			
			m_eMenu = (eMENU)iMenu;
		}
		else if (GET_MANAGEMENT->Key_Down(DIK_D))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UISwitch.ogg", CSound_Manager::ESoundID::UI, 1.f);

			_int iMenu = m_eMenu;
			iMenu += 1;
			if (iMenu >= (_int)eMENU::MENU_END)
				iMenu = 0;

			m_eMenu = (eMENU)iMenu;
		}

		if (GET_MANAGEMENT->Key_Down(DIK_RETURN))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);

			if (m_pInvenUI&& m_pEquipUI)
			{
				if (m_eMenu == eMENU::MENU_INVEN && !m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip() && !m_pStatusUI->Get_CheckViewStatus())	//인벤토리 오픈
				{
					m_pInvenUI->Set_ViewInven(true);
					m_pEquipUI->Set_ViewEquip(false);
					m_pStatusUI->Set_ViewStatus(false);
					//m_bViewInterface = (false);
				}
				else if (m_eMenu == eMENU::MENU_WEAPON && !m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip() && !m_pStatusUI->Get_CheckViewStatus())//장비창
				{
					m_pInvenUI->Set_ViewInven(false);
					m_pEquipUI->Set_ViewEquip(true);
					m_pStatusUI->Set_ViewStatus(false);
					//m_bViewInterface = (false);
				}
				else if(m_eMenu == eMENU::MENU_STAT && !m_pInvenUI->Get_CheckViewInven() && !m_pEquipUI->Get_CheckViewEquip() && !m_pStatusUI->Get_CheckViewStatus()) // 스텟
				{
					m_pInvenUI->Set_ViewInven(false);
					m_pEquipUI->Set_ViewEquip(false);
					m_pStatusUI->Set_ViewStatus(true);
				}
			}
		}
	}









	//if (m_pInvenUI&& m_pEquipUI)
	//{
	//	if (CManagement::Get_Instance()->Key_Down(DIK_I))
	//	{

	//	else if (m_pInvenUI->Get_CheckViewInven())	//인벤토리 오픈
	//	{
	//		m_pInvenUI->Set_ViewInven(false);
	//		m_pEquipUI->Set_ViewEquip(false);
	//		m_bViewInterface = (true);
	//	}
	//	}
	//	if (CManagement::Get_Instance()->Key_Down(DIK_T))
	//	{
	//		if (!m_pEquipUI->Get_CheckViewEquip())	//장비창 오픈
	//		{

	//		}
	//		else if (m_pEquipUI->Get_CheckViewEquip())	//장비창 오픈
	//		{
	//			m_pInvenUI->Set_ViewInven(false);
	//			m_pEquipUI->Set_ViewEquip(false);
	//			m_bViewInterface = (true);
	//		}
	//	}
	//}

		/*if (CManagement::Get_Instance()->Key_Down(DIK_N))
			m_iSoul += 1;*/
		if (CManagement::Get_Instance()->Key_Down(DIK_M))
			m_iSoul = 9999;
	
		//if (CManagement::Get_Instance()->Key_Down(DIK_Y))
		//{
		//	m_pEquipUI->SwapQuickSlot();
		//}
}

void CPlayer_UI::AddItem()
{
	if (m_pInvenUI&& m_pEquipUI)
	{
		if (CManagement::Get_Instance()->Key_Down(DIK_4))
		{
			if (m_pInvenUI->Get_CheckViewInven())
				m_pInvenUI->Add_ItemCount(_uint(Mersen_ToInt((_uint)eItemID::Heal_Small, (_uint)8)));
			if (m_pEquipUI->Get_CheckViewEquip())
				m_pEquipUI->Add_ItemCount(_uint(Mersen_ToInt((_uint)eWeaponID::Weapon_1, (_uint)eWeaponID::Weapon_3)));
		}
	}
}

void CPlayer_UI::CheckQuickItem()
{
	m_pQuickEquip =	m_pEquipUI->Get_QuickItem(0);
	m_pQuickItem = m_pInvenUI->Get_QuickItem();
}

void CPlayer_UI::Update_Interact(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player", EFindGameObject::Proto, L"Player"));
	m_eInteract = pPlayer->Get_bCollAct();

	if (m_bOnRenderGetItem)
		m_eInteract = INTERACT::INTER_OK;

	if (m_eInteract != INTERACT::INTER_END)
		m_fActTime += _fDeltaTime * 5.f;
	else
		m_fActTime -= _fDeltaTime * 5.f;

	if (m_fActTime >= 1.f)
		m_fActTime = 1.f;
	else if (m_fActTime <= 0.f)
		m_fActTime = 0.f;

	
}

void CPlayer_UI::On_Item(const eItemID & _eItemID)
{
	switch (_eItemID)
	{
	case eItemID::NO_ITEM:
		m_iGetItem = (_uint)eItemInfo::Item_End;
		break;
	case eItemID::Heal_Medium:
		m_iGetItem = (_uint)eItemInfo::Heal_Medium;
		break;
	case eItemID::Heal_Large:
		m_iGetItem = (_uint)eItemInfo::Heal_Large;
		break;
	case eItemID::Heal_Full:
		m_iGetItem = (_uint)eItemInfo::Heal_Full;
		break;
	case eItemID::StaminaRegen_Medium:
		m_iGetItem = (_uint)eItemInfo::StaminaRegen_Medium;
		break;
	case eItemID::StaminaRegen_Large:
		m_iGetItem = (_uint)eItemInfo::StaminaRegen_Large;
		break;
	case eItemID::Poise_Large:
		m_iGetItem = (_uint)eItemInfo::Poise_Large;
		break; 
	case eItemID::Poise_Full:
		m_iGetItem = (_uint)eItemInfo::Poise_Full;
		break;
	case eItemID::Soul_Small_1:
		m_iGetItem = (_uint)eItemInfo::Soul_Small_1;
		break;
	case eItemID::Soul_Small_2:
		m_iGetItem = (_uint)eItemInfo::Soul_Small_2;
		break;
	case eItemID::Soul_Medium:
		m_iGetItem = (_uint)eItemInfo::Soul_Medium;
		break;
	case eItemID::Soul_Preto:
		m_iGetItem = (_uint)eItemInfo::Soul_Preto;
		break;
	case eItemID::Soul_Mal:
		m_iGetItem = (_uint)eItemInfo::Soul_Mal;
		break;
	case eItemID::MainQuest_Key:
		m_iGetItem = (_uint)eItemInfo::MainQuest_Key;
		break;
	}

	m_bGetItem = true;
}

void CPlayer_UI::Update_GetItem(const _float & _fDeltaTime)
{
	if (m_bGetItem && m_fGet_ItemTime < 1.f)
		m_fGet_ItemTime += _fDeltaTime;
	else if (!m_bGetItem)
		m_fGet_ItemTime = 0.f;

	if (m_fGet_ItemTime >= 1.f)
	{
		m_bOnRenderGetItem = true;
		m_fGet_ItemTime = 1.f;
	}
}

void CPlayer_UI::Update_Area(const _float & _fDeltaTime)
{
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player", EFindGameObject::Proto, L"Player"));
	if (pPlayer->Get_PlayerAni() != (_uint)PlayerAniID::WAKE_UP && pPlayer->Get_PlayerAni() != (_uint)PlayerAniID::SLEEPING)
	{
		m_iCurArea = m_iSceneIdx;

		if (m_iPreArea != m_iCurArea)
		{
			switch (m_iCurArea)
			{
			case (_uint)ESceneType::Loading:
				m_bNewArea = false;
				break;
			case (_uint)ESceneType::Tutorial:
				m_iAreaCount = 0;
				m_bNewArea = true;
				break;
			case (_uint)ESceneType::Shrine:
				m_iAreaCount = 1;
				m_bNewArea = true;
				
				break;
			case (_uint)ESceneType::Stage2:
				m_iAreaCount = 2;
				m_bNewArea = true;
				break;
			}
			GET_MANAGEMENT->StopSound(CSound_Manager::ESoundID::Area);
			GET_MANAGEMENT->PlaySounds(L"New_Area.mp3", CSound_Manager::ESoundID::Area);
			m_iPreArea = m_iCurArea;
		}
	}

	if (m_bNewArea && m_fAreaTime < 1.f)
	{
		m_fAreaTime += _fDeltaTime;
	}
	else if (m_bNewArea && m_fAreaTime > 1.f)
	{
	
		m_fAreaTime = 1.f;
		m_bAreaWait = true;
	}
	if (m_bAreaWait)
	{
		m_fWaitTime += _fDeltaTime;
		if (m_fWaitTime >= 2.f)
		{
			m_fWaitTime = 0.f;
			m_bAreaWait = false;
			m_bNewArea = false;
		}
	}
	if (!m_bNewArea && m_fAreaTime > 0.f)
	{
		m_fAreaTime -= _fDeltaTime;
	}
	else if (!m_bNewArea && m_fAreaTime <= 0.f)
	{
		m_fAreaTime = 0.f;
	}
}

void CPlayer_UI::OK()
{
	m_bOnRenderGetItem = false;
	m_bGetItem = false;
}

CPlayer_UI * CPlayer_UI::Create(_DEVICE _pDevice)
{
	CPlayer_UI* pInstance = new CPlayer_UI(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		ERROR_BOX(L"Failed to Create");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_UI::Clone(const _uint& _iSceneIdx, void * _pArg /*= nullptr*/)
{
	CPlayer_UI* pClone = new CPlayer_UI(*this);
	if (FAILED(pClone->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		ERROR_BOX(L"Failed to Clone");
		Safe_Release(pClone);
	}

	return pClone;
}

void CPlayer_UI::Free()
{
	if (m_bCheckClone)
	{
		Safe_Release(m_pInvenUI);
		Safe_Release(m_pEquipUI);
		Safe_Release(m_pStatusUI);
	}

	CGameObject::Free();
}
