#include "stdafx.h"
#include "UI_Forge.h"
#include "Player.h"
#include "Player_UI.h"
#include "Component_Manager.h"
#include "UI_Select_Upgrade.h"
#include "UI_Equipment.h"
#include "UI_Inventory.h"

CUI_Forge::CUI_Forge(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_pSlotTexture(nullptr)
	, m_pFlavorTexture(nullptr)
	, m_iArrSize(0)
	, m_iSelectSlotNumber(0)
	, m_iCapacity(3)
	, m_bUIView(false)
	, m_iPlySoul(0)
	, m_ubyPlyItem(0)
	, m_iUpgradeChance(0)
	, m_iSlotSize(0)
{	 
	ZeroMemory(m_arrForgeSlot, sizeof(Client::FORGE_SLOT_INFO) * 3);
}

CUI_Forge::CUI_Forge(const CUI_Forge & _rhs)
	: CUIBase(_rhs)
	, m_pSlotTexture(nullptr)
	, m_pFlavorTexture(nullptr)
	, m_iSelectSlotNumber(0)
	, m_iCapacity(3)
	, m_bUIView(false)
	, m_iPlySoul(0)
	, m_ubyPlyItem(0)
	, m_iUpgradeChance(0)
	, m_iArrSize(0)
	, m_iSlotSize(0)
{
	ZeroMemory(m_arrForgeSlot, sizeof(Client::FORGE_SLOT_INFO) * 3);
}

HRESULT CUI_Forge::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();

}

HRESULT CUI_Forge::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	CManagement* pManagement = GET_MANAGEMENT;
	m_tUIDesc.vPos = { _float(0),_float(0) ,1.f };
	m_tUIDesc.vSize = { _float(WINCX),_float(WINCY) };
	lstrcpy(m_tUIDesc.szTexName, L"Component_Texture_ForgeBack");

	FAILED_CHECK_RETURN(CUI_Forge::Add_Com(), E_FAIL);
	m_iUpgradeChance = 800;

	//CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));

	return S_OK;
}

_uint CUI_Forge::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;

	CUIBase::Update_GameObject(_fDeltaTime);

	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
	m_iSlotSize = pPlyUI->Get_Equip()->Get_ArrSize();

	Check_Upgrade_Recipe();

	
	KeyInput();

	return m_iEvent;
}

_uint CUI_Forge::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_Forge::Render_GameObject()
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

	if (FAILED(SetUp_ConstantTable_Forge(pEffect)))
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

	pEffect->EndPass();
	pEffect->End();

	if (m_bUIView)
	{
		if (m_iArrSize != 0)
		{
			Render_Slot(pEffect);
			Render_Material(pEffect);
			Render_WeaponName(pEffect);

			Render_Counting		  (pEffect, _float3(380.f, 23.f, 1.f),	 _float3(10.f, 15.f, 1.f),  10.f, (_uint)(m_arrForgeSlot[m_iSelectSlotNumber].fDamage + ((m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_Level)*m_arrForgeSlot[m_iSelectSlotNumber].fDamage)));
			Render_Counting		  (pEffect, _float3(380.f, -9.f, 1.f),	 _float3(10.f, 15.f, 1.f),  10.f, (_uint)(m_arrForgeSlot[m_iSelectSlotNumber].fDamage + ((m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_Level+1.f)*m_arrForgeSlot[m_iSelectSlotNumber].fDamage)));
			Render_ReverseCounting(pEffect, _float3(-45.f, -135.f, 1.f), _float3(8.f, 12.f, 1.f), 10.f, (_uint)m_arrForgeSlot[m_iSelectSlotNumber].iCost);
			Render_ReverseCounting(pEffect, _float3(62.f, -135.f, 1.f),	 _float3(8.f, 12.f, 1.f), 10.f, (_uint)m_arrForgeSlot[m_iSelectSlotNumber].ubyMaterialCost);
			Render_ReverseCounting(pEffect, _float3(-45.f, -180.f, 1.f), _float3(8.f, 12.f, 1.f), 10.f, (_uint)m_iPlySoul);
			Render_ReverseCounting(pEffect, _float3(62.f, -180.f, 1.f),	 _float3(8.f, 12.f, 1.f), 10.f, (_uint)m_ubyPlyItem);

		}
	}

	Safe_Release(pEffect);

	return S_OK;
}

void CUI_Forge::Start_WeaponUpgrad()
{
	m_bUIView = true;
}

HRESULT CUI_Forge::SetUp_ConstantTable_Forge(_EFFECT & _pEffect)
{
	_float4x4 matView, matProj;

	m_pDevice->GetTransform(D3DTS_VIEW, &matView);
	m_pDevice->GetTransform(D3DTS_PROJECTION, &matProj);

	_pEffect->SetMatrix("g_matWorld", &m_pTransform->Get_TransformDesc().matWorld);
	_pEffect->SetMatrix("g_matView", &matView);
	_pEffect->SetMatrix("g_matProj", &matProj);
	CManagement::Get_Instance()->Set_Texture(0, m_tUIDesc.szTexName, _pEffect, "g_BaseTexture");
	_pEffect->CommitChanges();

	return S_OK;
}

HRESULT CUI_Forge::Render_Slot(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);

	for (_uint i = 0; i < m_iArrSize; ++i)
	{
		matSlotWorld.m[0][0] = 300.f;
		matSlotWorld.m[1][1] = 80.f;
		matSlotWorld.m[2][2] = 1.f;

		memcpy(&matSlotWorld.m[3][0],
			_float3(-400.f, 150.f - (i*(90.f)), 1.f), sizeof(_float3));

		/*셰이더 값 설정 코드 필요함.*/

		if (m_arrForgeSlot[i].ubyWeapon_ID == 8)
			m_arrForgeSlot[i].ubyWeapon_Level;

		if (m_arrForgeSlot[i].ubyWeapon_ID == 9)
			m_arrForgeSlot[i].ubyWeapon_Level;


		_pEffect->SetMatrix("g_matWorld", &matSlotWorld);

		if (m_iSelectSlotNumber == i)
		{
			if (m_arrForgeSlot[i].ubyWeapon_Level == 5)
			{
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture");
				_pEffect->CommitChanges();
			}
			else
			{
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture", 1);
				_pEffect->CommitChanges();
			}
		}
		else
		{
			m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture");
			_pEffect->CommitChanges();
		}

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

		Render_Sign(_pEffect, _float3(281.f, 148.f - (i*(90.f)), 1.f), _float3(12.f, 12.f, 1.f), 11);
		Render_Counting(_pEffect, _float3(-272.f, 149.f - (i*(90.f)), 1.f), _float3(13.f, 19.5f, 1.f), 8.f, m_arrForgeSlot[i].ubyWeapon_Level);


		matSlotWorld.m[0][0] = 70.f;
		matSlotWorld.m[1][1] = 70.f;
		matSlotWorld.m[2][2] = 1.f;

		memcpy(&matSlotWorld.m[3][0], _float3(-510.f, 150.f - (i*90.f), 1.f), sizeof(_float3));

		/*셰이더 값 설정 코드 필요함.*/

		_pEffect->SetMatrix("g_matWorld", &matSlotWorld);


		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Item_Icon"
			, _pEffect, "g_BaseTexture", m_arrForgeSlot[i].ubyWeapon_ID);

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

		if (i == m_iSelectSlotNumber)
		{
			matSlotWorld.m[0][0] = 250.f;
			matSlotWorld.m[1][1] = 250.f;
			matSlotWorld.m[2][2] = 1.f;

			memcpy(&matSlotWorld.m[3][0], _float3(-25.f, 100.f, 1.f), sizeof(_float3));

			/*셰이더 값 설정 코드 필요함.*/
			_pEffect->SetMatrix("g_matWorld", &matSlotWorld);

			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Item_Icon"
				, _pEffect, "g_BaseTexture"
				, m_arrForgeSlot[i].ubyWeapon_ID);

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
	}
	return S_OK;
}

HRESULT CUI_Forge::Render_Material(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;

	D3DXMatrixIdentity(&matSlotWorld);

	for (_uint i = 0; i < 2; ++i)
	{
		matSlotWorld.m[0][0] = 50.f;
		matSlotWorld.m[1][1] = 50.f;
		matSlotWorld.m[2][2] = 1.f;

		memcpy(&matSlotWorld.m[3][0], _float3(-70.f + (i*(106.f)), -115.f, 1.f), sizeof(_float3));

		/*셰이더 값 설정 코드 필요함.*/
		_pEffect->SetMatrix("g_matWorld", &matSlotWorld);

		if (i)
		{
			if (m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_Level < 4)
			{
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Item_Icon"
					, _pEffect, "g_BaseTexture"
					, 4);
			}
			else
			{
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Item_Icon"
					, _pEffect, "g_BaseTexture"
					, 5);
			}
		}
		else
		{
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Item_Icon"
				, _pEffect, "g_BaseTexture"
				, 25);
		}


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

HRESULT CUI_Forge::Render_WeaponName(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);

	for (_uint i = 0; i < m_iArrSize; ++i)
	{

		m_arrForgeSlot[i].ubyWeapon_ID;
		_uint iNameTex = 0;

		matSlotWorld.m[0][0] = 200.f;
		matSlotWorld.m[1][1] = 60.f;
		matSlotWorld.m[2][2] = 1.f;

		memcpy(&matSlotWorld.m[3][0],
			_float3(-380.f, 150.f - (i*(90.f)), 1.f), sizeof(_float3));


		/*셰이더 값 설정 코드 필요함.*/

		if (m_arrForgeSlot[i].ubyWeapon_ID == 8)
			iNameTex = 0;

		if (m_arrForgeSlot[i].ubyWeapon_ID == 9)
			iNameTex = 1;

		if (m_arrForgeSlot[i].ubyWeapon_ID == 10)
			iNameTex = 0;

		_pEffect->SetMatrix("g_matWorld", &matSlotWorld);

		m_pFlavorTexture->Set_Texture(_pEffect, "g_BaseTexture", iNameTex);

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

HRESULT CUI_Forge::Render_Sign(_EFFECT & _pEffect, _float3 & vPos, _float3 & vSize, _uint iIndex)
{
	_float4x4 matSlotWorld;

	D3DXMatrixIdentity(&matSlotWorld);

	matSlotWorld.m[0][0] = vSize.x;
	matSlotWorld.m[1][1] = vSize.y;
	matSlotWorld.m[2][2] = 1.f;

	memcpy(&matSlotWorld.m[3][0],
		_float3(-vPos.x, vPos.y, 1.f), sizeof(_float3));


	/*셰이더 값 설정 코드 필요함.*/

	_pEffect->SetMatrix("g_matWorld", &matSlotWorld);

	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
		, _pEffect, "g_BaseTexture"
		, iIndex);


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

HRESULT CUI_Forge::Render_Counting(_EFFECT & _pEffect, _float3 & vPos, _float3 & vSize, const _float & fDist, const _uint & iCount)
{
	_uint iCost = iCount;
	_float fNum = 0.f;
	if (iCount / 10 > 0)
	{
		if (iCount / 100 > 0)
		{
			if (iCount / 1000 > 0)
			{
				fNum = 4.f;
			}
			else
				fNum = 3.f;
		}
		else
			fNum = 2.f;
	}
	else
		fNum = 1.f;

	_float4x4 matOneWorld;
	D3DXMatrixIdentity(&matOneWorld);
	matOneWorld.m[0][0] = vSize.x;
	matOneWorld.m[1][1] = vSize.y;
	matOneWorld.m[2][2] = 1.f;
	memcpy(&matOneWorld.m[3][0],
		_float3(vPos.x + fDist*fNum,
			vPos.y,
			vPos.z), sizeof(_float3));

	_uint iOneCost = iCost % 10;

	_pEffect->SetMatrix("g_matWorld", &matOneWorld);
	
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
		, _pEffect, "g_BaseTexture"
		, iOneCost);

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

	if (iCount >= 10)
	{
		_float4x4 matTenWorld;
		D3DXMatrixIdentity(&matTenWorld);
		matTenWorld.m[0][0] = vSize.x;
		matTenWorld.m[1][1] = vSize.y;
		matTenWorld.m[2][2] = 1.f;
		memcpy(&matTenWorld.m[3][0],
			_float3((vPos.x + fDist*(fNum - 1)), vPos.y, 1.f), sizeof(_float3));

		iCost = iCount;
		iCost = iCost % 100;
		_uint iTenCost = iCost / 10;

		_pEffect->SetMatrix("g_matWorld", &matTenWorld);

		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
			, _pEffect, "g_BaseTexture"
			, iTenCost);

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

		if (iCount >= 100)
		{
			_float4x4 matHunWorld;
			D3DXMatrixIdentity(&matHunWorld);
			matHunWorld.m[0][0] = vSize.x;
			matHunWorld.m[1][1] = vSize.y;
			matHunWorld.m[2][2] = 1.f;
			memcpy(&matHunWorld.m[3][0],
				_float3((vPos.x + fDist*(fNum - 2)),
					vPos.y,
					1.f), sizeof(_float3));

			iCost = iCount;
			iCost = iCost % 1000;
			_uint iHunCost = iCost / 100;

			_pEffect->SetMatrix("g_matWorld", &matHunWorld);
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
				, _pEffect, "g_BaseTexture"
				, iHunCost);
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


			if (iCount >= 1000)
			{
				_float4x4 matColWorld;
				D3DXMatrixIdentity(&matColWorld);
				matColWorld.m[0][0] = vSize.x;
				matColWorld.m[1][1] = vSize.y;
				matColWorld.m[2][2] = 1.f;
				memcpy(&matColWorld.m[3][0],
					_float3((vPos.x + fDist*(fNum - 3)),
						vPos.y,
						1.f), sizeof(_float3));

				_pEffect->SetMatrix("g_matWorld", &matColWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
					, _pEffect, "g_BaseTexture"
					, 10);
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
				matSouWorld.m[0][0] = vSize.x;
				matSouWorld.m[1][1] = vSize.y;
				matSouWorld.m[2][2] = 1.f;
				memcpy(&matSouWorld.m[3][0],
					_float3((vPos.x + fDist*(fNum - 4)),
						vPos.y,
						1.f), sizeof(_float3));

				iCost = iCount;
				iCost = iCost % 10000;
				_uint iSouCost = iCost / 1000;

				_pEffect->SetMatrix("g_matWorld", &matSouWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
					, _pEffect, "g_BaseTexture"
					, iSouCost);
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

HRESULT CUI_Forge::Render_ReverseCounting(_EFFECT & _pEffect, _float3 & vPos, _float3 & vSize, const _float & fDist, const _uint & iCount)
{
	_uint iCost = iCount;

	_float4x4 matOneWorld;
	D3DXMatrixIdentity(&matOneWorld);
	matOneWorld.m[0][0] = vSize.x;
	matOneWorld.m[1][1] = vSize.y;
	matOneWorld.m[2][2] = 1.f;
	memcpy(&matOneWorld.m[3][0],
		_float3(vPos.x,
			vPos.y,
			vPos.z), sizeof(_float3));

	_uint iOneCost = iCost % 10;

	_pEffect->SetMatrix("g_matWorld", &matOneWorld);

	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
		, _pEffect, "g_BaseTexture"
		, iOneCost);

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

	if (iCount >= 10)
	{
		_float4x4 matTenWorld;
		D3DXMatrixIdentity(&matTenWorld);
		matTenWorld.m[0][0] = vSize.x;
		matTenWorld.m[1][1] = vSize.y;
		matTenWorld.m[2][2] = 1.f;
		memcpy(&matTenWorld.m[3][0],
			_float3((vPos.x - fDist), vPos.y, 1.f), sizeof(_float3));

		iCost = iCount;
		iCost = iCost % 100;
		_uint iTenCost = iCost / 10;

		_pEffect->SetMatrix("g_matWorld", &matTenWorld);

		GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
			, _pEffect, "g_BaseTexture"
			, iTenCost);

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

		if (iCount >= 100)
		{
			_float4x4 matHunWorld;
			D3DXMatrixIdentity(&matHunWorld);
			matHunWorld.m[0][0] = vSize.x;
			matHunWorld.m[1][1] = vSize.y;
			matHunWorld.m[2][2] = 1.f;
			memcpy(&matHunWorld.m[3][0],
				_float3((vPos.x - fDist*2),
					vPos.y,
					1.f), sizeof(_float3));

			iCost = iCount;
			iCost = iCost % 1000;
			_uint iHunCost = iCost / 100;

			_pEffect->SetMatrix("g_matWorld", &matHunWorld);
			GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
				, _pEffect, "g_BaseTexture"
				, iHunCost);
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


			if (iCount >= 1000)
			{
				_float4x4 matColWorld;
				D3DXMatrixIdentity(&matColWorld);
				matColWorld.m[0][0] = vSize.x;
				matColWorld.m[1][1] = vSize.y;
				matColWorld.m[2][2] = 1.f;
				memcpy(&matColWorld.m[3][0],
					_float3((vPos.x - fDist*3),
						vPos.y,
						1.f), sizeof(_float3));

				_pEffect->SetMatrix("g_matWorld", &matColWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
					, _pEffect, "g_BaseTexture"
					, 10);
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
				matSouWorld.m[0][0] = vSize.x;
				matSouWorld.m[1][1] = vSize.y;
				matSouWorld.m[2][2] = 1.f;
				memcpy(&matSouWorld.m[3][0],
					_float3((vPos.x - fDist*4),
						vPos.y,
						1.f), sizeof(_float3));

				iCost = iCount;
				iCost = iCost % 10000;
				_uint iSouCost = iCost / 1000;

				_pEffect->SetMatrix("g_matWorld", &matSouWorld);
				GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Number"
					, _pEffect, "g_BaseTexture"
					, iSouCost);
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

void CUI_Forge::KeyInput()
{
	CManagement* pManagement = GET_MANAGEMENT;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));

	if (m_bUIView)
	{
		if (pManagement->Key_Down(DIK_RETURN))
		{
			CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
			
			if (!Check_ForgeAble() || m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_Level > 4|| pPlyUI->Get_Inven()==nullptr)
			{
				return;
			}

			if (m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_Level < 5)
			{
				for (_ubyte i = 0; i < m_arrForgeSlot[m_iSelectSlotNumber].ubyMaterialCost; i++)
				{
					if (m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_Level < 4)
					{
						pPlyUI->Get_Inven()->Min_ItemCount((_ubyte)eItemID::Poise_Full);
					}
					else
					{
						pPlyUI->Get_Inven()->Min_ItemCount((_ubyte)eItemID::Poise_Large);
					}
				}

				pPlyUI->Add_Souls(-m_arrForgeSlot[m_iSelectSlotNumber].iCost);
			}

			if (Mersen_ToInt(0, 1000) >= m_iUpgradeChance)
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_.ForgeSucess.ogg", CSound_Manager::ESoundID::UI, 1.f);
				Upgrade_Weapon();
				m_iUpgradeChance = 800;
			}
			else
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_.ForgeFail.ogg", CSound_Manager::ESoundID::UI, 1.f);
				m_iUpgradeChance -= 200;
			}

			
			Check_Damage_And_Level(m_iSelectSlotNumber);
			SlotMove(m_iSelectSlotNumber);
			Check_Damage_And_Level(m_iSelectSlotNumber);
		}
		if (CManagement::Get_Instance()->Key_Down(DIK_W))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);
			if (m_iSelectSlotNumber == 0)
			{
				m_iSelectSlotNumber = m_iArrSize-1;
				if (m_iSelectSlotNumber < 0)
				{
					m_iSelectSlotNumber = 0;
				}
				return;
			}

			--m_iSelectSlotNumber;
		}
		if (CManagement::Get_Instance()->Key_Down(DIK_S))
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemSlot.ogg", CSound_Manager::ESoundID::UI, 1.f);
			if (m_iSelectSlotNumber == m_iArrSize-1)
			{
				m_iSelectSlotNumber = 0;
				if ((_uint)m_iSelectSlotNumber > m_iArrSize)
				{
					m_iSelectSlotNumber = m_iArrSize;
				}
				return;
			}
			++m_iSelectSlotNumber;
		}
		if (CManagement::Get_Instance()->Key_Down(DIK_ESCAPE))
		{
			CUI_Select_Upgrade* pUpgrade = (CUI_Select_Upgrade*)pManagement->Get_GameObject(m_iSceneIdx, L"Layer_NPCUI", EFindGameObject::Proto, L"GameObject_Select_Upgrader");
			pUpgrade->Set_View_SelectUpgrade(true);
			m_bUIView = false;
			m_iSelectSlotNumber = 0;
			m_iArrSize = 0;

			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
		}
	}
}

_ubyte CUI_Forge::Find_Weapon(const _ubyte & _ubyItemID)
{
	for (_ubyte i = 0; i < m_iArrSize; ++i)
		if (m_arrForgeSlot[i].ubyWeapon_ID == _ubyItemID)
			return ++i;

	return 0;
}

_bool CUI_Forge::Check_ForgeAble()
{
	if (m_arrForgeSlot[m_iSelectSlotNumber].ubyMaterialCost <= m_ubyPlyItem && m_iPlySoul >= (_uint)m_arrForgeSlot[m_iSelectSlotNumber].iCost)
		return true;
	return false;
}

void CUI_Forge::Check_Upgrade_Recipe()
{
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
	////////////////////////////////임시 아이템 추가////////////////
	if (GET_MANAGEMENT->Key_Pressing(DIK_NUMPADMINUS))
	{
		pPlyUI->Add_Used_Item(eItemID::Poise_Full);
	}
	////////////////////////////////임시 아이템 추가////////////////

	for (_uint i = 0; i < m_iArrSize; i++)
	{
		if (m_arrForgeSlot[i].ubyWeapon_Level < 5)
		{
			if (m_arrForgeSlot[i].ubyWeapon_Level < 4)
			{
				m_arrForgeSlot[i].ubyMaterialID = 5;
				m_arrForgeSlot[i].ubyMaterialCost = 1 + ((m_arrForgeSlot[i].ubyWeapon_Level) * 2);
				if (m_iSelectSlotNumber == i)
					m_ubyPlyItem = pPlyUI->Get_Inven()->Get_ItemCount(5);
			}
			else
			{
				m_arrForgeSlot[i].ubyMaterialID = 6;
				m_arrForgeSlot[i].ubyMaterialCost = 1;
				if (m_iSelectSlotNumber == i)
					m_ubyPlyItem = pPlyUI->Get_Inven()->Get_ItemCount(6);
			}
			m_arrForgeSlot[i].iCost = 100 * ((m_arrForgeSlot[i].ubyWeapon_Level + 1) * 2);
		}
	}

	m_iPlySoul = pPlyUI->Get_Souls();
}

void CUI_Forge::Add_Damage_And_Level(_uint iInven)
{
	CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player_Weapon", EFindGameObject::Proto, L"Player_Weapon"));
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
	_ubyte ubyWeaponID = pPlyUI->Get_Equip()->Get_ItemInfo(iInven)->ubyItem_ID - 1;

	if (pPlyUI == nullptr || pPlyUI->Get_Equip()->Get_ArrSize() == 0 || m_iCapacity < m_iArrSize || pPlyUI->Get_Equip()->Get_ItemInfo(iInven) == nullptr || !ubyWeaponID)
	{
		return;
	}
	_uint iIndex = Find_Weapon(ubyWeaponID);
	if (!iIndex)	//index를 찾지 못함.
	{
		_uint iLevel = 0;
		_float fDamage = 0;

		if (ubyWeaponID == 8)
		{
			iLevel = pWeapon->Get_WeaponStat().iNormalLevel;
			fDamage = pWeapon->Get_WeaponStat().fNormalDamage;
		}
		if (ubyWeaponID == 9)
		{
			iLevel = pWeapon->Get_WeaponStat().iMoonLevel;
			fDamage = pWeapon->Get_WeaponStat().fMoonDamage;
		}
		if (ubyWeaponID == 10)
		{
			iLevel = 0;
			fDamage = 0;
		}

		if (m_iArrSize >= m_iCapacity|| iLevel == 5)	//사이즈가 카파시티를 넘으면 false반환
			return;
		if (!m_iArrSize)	//비어있을 경우
		{
			++m_iArrSize;
			m_arrForgeSlot[0].ubyWeapon_ID = ubyWeaponID;
			m_arrForgeSlot[0].ubyWeapon_Level = iLevel;
			m_arrForgeSlot[0].fDamage = fDamage;
		}
		else
		{
			m_arrForgeSlot[m_iArrSize].ubyWeapon_ID = ubyWeaponID;
			m_arrForgeSlot[m_iArrSize].ubyWeapon_Level = iLevel;
			m_arrForgeSlot[m_iArrSize].fDamage = fDamage;
			++m_iArrSize;
		}
	}
}

void CUI_Forge::SlotMove(const _int & _iIndex)
{
	if (m_arrForgeSlot[_iIndex].ubyWeapon_Level < 5 || _iIndex < 0)
		return;
	Client::FORGE_SLOT_INFO m_tempArr[6];

	memcpy(&m_tempArr[0], &m_arrForgeSlot[_iIndex + 1], sizeof(Client::FORGE_SLOT_INFO)*(m_iArrSize - (_iIndex + 1)));
	memcpy(&m_arrForgeSlot[_iIndex], &m_tempArr[0], sizeof(Client::FORGE_SLOT_INFO)*(m_iArrSize - (_iIndex + 1)));
	
	m_arrForgeSlot[m_iArrSize - 1].fDamage = 0;
	m_arrForgeSlot[m_iArrSize - 1].iCost = 0;
	m_arrForgeSlot[m_iArrSize - 1].ubyMaterialCost = 0;
	m_arrForgeSlot[m_iArrSize - 1].ubyMaterialID = 0;
	m_arrForgeSlot[m_iArrSize - 1].ubyWeapon_ID = 0;
	m_arrForgeSlot[m_iArrSize - 1].ubyWeapon_Level = 0;

	--m_iArrSize;
	m_iSelectSlotNumber = 0;
}

void CUI_Forge::Check_Damage_And_Level(_uint iInven)
{
	_uint iLevel = 0;
	_float fDamage = 0;

	CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player_Weapon", EFindGameObject::Proto, L"Player_Weapon"));
	
	_ubyte ubyWeaponID = m_arrForgeSlot[iInven].ubyWeapon_ID;

	if (ubyWeaponID == 8)
	{
		iLevel = pWeapon->Get_WeaponStat().iNormalLevel;
		fDamage = pWeapon->Get_WeaponStat().fNormalDamage;
	}
	if (ubyWeaponID == 9)
	{
		iLevel = pWeapon->Get_WeaponStat().iMoonLevel;
		fDamage = pWeapon->Get_WeaponStat().fMoonDamage;
	}
	if (ubyWeaponID == 10)
	{
		iLevel = 0;
		fDamage = 0;
	}

	m_arrForgeSlot[iInven].ubyWeapon_ID = ubyWeaponID;
	m_arrForgeSlot[iInven].ubyWeapon_Level = iLevel;
	m_arrForgeSlot[iInven].fDamage = fDamage;
}

void CUI_Forge::Upgrade_Weapon()
{
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
	CPlayer_Weapon* pWeapon = dynamic_cast<CPlayer_Weapon*>(GET_MANAGEMENT->Get_GameObject(m_iSceneIdx, L"Layer_Player_Weapon", EFindGameObject::Proto, L"Player_Weapon"));
	if (pWeapon == nullptr&&pPlyUI == nullptr&&pPlyUI->Get_Equip() == nullptr)
	{
		return;
	}
	if (m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_ID== 8)
	{
		pWeapon->Set_WeaponLevelUp(CPlayer_Weapon::NORMAL,1);
	}

	if (m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_ID == 9)
	{
		pWeapon->Set_WeaponLevelUp(CPlayer_Weapon::MOON, 1);
	}

	if (m_arrForgeSlot[m_iSelectSlotNumber].ubyWeapon_ID == 10)
	{
		return;
	}
}

void CUI_Forge::ScrollingSlot()
{
	//향후 무기가 2종 이상 추가될경우 스크롤 만들어야 함
}

HRESULT CUI_Forge::Add_Com()
{
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_Forge_Slot",
		L"Com_SlotTexture", EComponentType::Static, (CComponent**)&m_pSlotTexture), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_WeaponName",
		L"Com_WeaponNameTexture", EComponentType::Static, (CComponent**)&m_pFlavorTexture), E_FAIL);

	return S_OK;
}

CUI_Forge * CUI_Forge::Create(_DEVICE _pDevice)
{
	CUI_Forge* pInstance = new CUI_Forge(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_Forge::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_Forge* pInstance = new CUI_Forge(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}
