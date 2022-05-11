#include "stdafx.h"
#include "Player_UI.h"
#include "UI_Inventory.h"
#include "UI_Equipment.h"

CPlayer_UI::CPlayer_UI(_DEVICE _pDevice)
	: CUIBase(_pDevice)
	, m_bViewInterface(false)
	, m_pIconTexture(nullptr)
	, m_pQuickEquip(nullptr), m_pQuickItem(nullptr)
	, m_pInvenUI(nullptr), m_pEquipUI(nullptr)
	, m_pGageTexture(nullptr), m_pHPBarTexture(nullptr)
	, m_pStaminaBarTexture(nullptr)
	, m_pNumTexture(nullptr)
	, m_iSoul(0)
{
}

CPlayer_UI::CPlayer_UI(const CPlayer_UI & _rOther)
	: CUIBase(_rOther)
	, m_bViewInterface(false)
	, m_pIconTexture(nullptr)
	, m_pQuickEquip(nullptr), m_pQuickItem(nullptr)
	, m_pInvenUI(nullptr), m_pEquipUI(nullptr)
	, m_pGageTexture(nullptr), m_pHPBarTexture(nullptr)
	, m_pStaminaBarTexture(nullptr)
	, m_pNumTexture(nullptr)
	, m_iSoul(_rOther.m_iSoul)
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

	return S_OK;
}

_uint CPlayer_UI::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;

	KeyInput();
	AddItem();
	CheckQuickItem();
	if(!m_bViewInterface)
		return (_uint)EEvent::None;


	m_tState = pManagement->Get_GameObject(m_iSceneIdx, L"Layer_Player", EFindGameObject::Proto, L"Player")->Get_Stat();


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
	if (!m_bViewInterface)
		return (_uint)EEvent::None;
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

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		return E_FAIL;
	}

	pEffect->EndPass();
	pEffect->End();
	Render_ItemIcon(pEffect);
	Render_HP_Bar(pEffect);
	Render_Stamina_Bar(pEffect);
	Render_Soul(pEffect);
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

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Player_Gage",
		L"Com_GageTexture", EComponentType::Static, (CComponent**)&m_pGageTexture), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Player_HP",
		L"Com_HPTexture", EComponentType::Static, (CComponent**)&m_pHPBarTexture), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Player_Stamina",
		L"Com_StaminaTexture", EComponentType::Static, (CComponent**)&m_pStaminaBarTexture), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Number",
		L"Com_NumberTexture", EComponentType::Static, (CComponent**)&m_pNumTexture), E_FAIL);
	
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
	m_pGageTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);

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
	



	_float4x4 matHPWorld;
	D3DXMatrixIdentity(&matHPWorld);
	matHPWorld.m[0][0] = fHPSizeX;
	matHPWorld.m[1][1] = 18.f;
	matHPWorld.m[2][2] = 0.f;
	memcpy(&matHPWorld.m[3][0],
		_float3(80.f + (fHPSizeX * 0.5f) - (WINCX >> 1),
			679.f - (WINCY >> 1),
			1.f), sizeof(_float3));


	_pEffect->SetMatrix("g_matWorld", &matHPWorld);
	m_pHPBarTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);

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
	return S_OK;
}

HRESULT CPlayer_UI::Render_Stamina_Bar(_EFFECT & _pEffect)
{
	_float fGageSizeX = 2.f * m_tState.fMaxStamina;
	_float fStaminaSizeX = 2.f * m_tState.fStamina;

	_float4x4 matGageWorld;
	D3DXMatrixIdentity(&matGageWorld);
	matGageWorld.m[0][0] = 160.f;
	matGageWorld.m[1][1] = 21.f;
	matGageWorld.m[2][2] = 0.f;
	memcpy(&matGageWorld.m[3][0],
		_float3(80.f + (fGageSizeX * 0.5f) - (WINCX >> 1),
			660.f - (WINCY >> 1),
			1.f), sizeof(_float3));


	_pEffect->SetMatrix("g_matWorld", &matGageWorld);
	m_pGageTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);

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
	



	_float4x4 matStaminaWorld;
	D3DXMatrixIdentity(&matStaminaWorld);
	matStaminaWorld.m[0][0] = fStaminaSizeX;
	matStaminaWorld.m[1][1] = 18.f;
	matStaminaWorld.m[2][2] = 0.f;
	memcpy(&matStaminaWorld.m[3][0],
		_float3(80.f + (fStaminaSizeX * 0.5f) - (WINCX >> 1),
			659.f - (WINCY >> 1),
			1.f), sizeof(_float3));


	_pEffect->SetMatrix("g_matWorld", &matStaminaWorld);
	m_pStaminaBarTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);

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
	m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iOneSoul);

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
		m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iTenSoul);

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
			m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iHunSoul);

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
				m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", 10);

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
				m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iSouSoul);

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
			}
		}
	}
	

	return S_OK;
}

void CPlayer_UI::KeyInput()
{
	if (m_pInvenUI&& m_pEquipUI)
	{
		if (CManagement::Get_Instance()->Key_Down(DIK_I))
		{
			if (!m_pInvenUI->Get_CheckViewInven())	//인벤토리 오픈
			{
				m_pInvenUI->Set_ViewInven(true);
				m_pEquipUI->Set_ViewEquip(false);
				m_bViewInterface = (false);
			}
			else if (m_pInvenUI->Get_CheckViewInven())	//인벤토리 오픈
			{
				m_pInvenUI->Set_ViewInven(false);
				m_pEquipUI->Set_ViewEquip(false);
				m_bViewInterface = (true);
			}
		}
		if (CManagement::Get_Instance()->Key_Down(DIK_T))
		{
			if (!m_pEquipUI->Get_CheckViewEquip())	//장비창 오픈
			{
				m_pInvenUI->Set_ViewInven(false);
				m_pEquipUI->Set_ViewEquip(true);
				m_bViewInterface =(false);
			}
			else if (m_pEquipUI->Get_CheckViewEquip())	//장비창 오픈
			{
				m_pInvenUI->Set_ViewInven(false);
				m_pEquipUI->Set_ViewEquip(false);
				m_bViewInterface = (true);
			}
		}
		if (CManagement::Get_Instance()->Key_Down(DIK_N))
		{
			m_iSoul += 1;
		}
		if (CManagement::Get_Instance()->Key_Down(DIK_M))
		{
			m_iSoul = 9999;
		}
		//if (CManagement::Get_Instance()->Key_Down(DIK_Y))
		//{
		//	m_pEquipUI->SwapQuickSlot();
		//}

	}
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
	}

	CGameObject::Free();
}
