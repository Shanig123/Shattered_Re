#include "stdafx.h"
#include "UI_NPC_Valnir.h"
#include "Player.h"
#include "Player_UI.h"
#include "Component_Manager.h"

#define PowValue			(1.5f)

CUI_NPC_Valnir::CUI_NPC_Valnir(_DEVICE _pDevice)
	:CUIBase(_pDevice)
	, m_pSlotTexture(nullptr)
	, m_pBackTexture(nullptr)
	, m_pFlavorTexture(nullptr)
	, m_pItemTitleTexture(nullptr)
	, m_pScriptTexture(nullptr)
	, m_iArrSize(0)
	, m_iCapacity(VALNIR_SLOT_Y_COUNT)
	, m_iArrBegin(0)
	, m_iQuickItemIdx(-1)
	, m_bUIView(false)
	, m_fSlotInterval(0.f)
	, m_bShoppingMode(false)
	, m_iUIDiffuse(0)
	, m_iPlayerSoul(0)
	, m_fBackDissolveTime(0.f)
	, m_iMainRenderPass(0)
{
	ZeroMemory(m_arrShopSlot, sizeof(Client::SHOPSLOT_INFO) * VALNIR_SLOT_Y_COUNT);
}

CUI_NPC_Valnir::CUI_NPC_Valnir(const CUI_NPC_Valnir & _rhs)
	: CUIBase(_rhs)
	, m_pSlotTexture(nullptr)
	, m_pBackTexture(nullptr)
	, m_pFlavorTexture(nullptr)
	, m_pItemTitleTexture(nullptr)
	, m_pScriptTexture(nullptr)
	, m_iArrSize(0)
	, m_iCapacity(VALNIR_SLOT_Y_COUNT)
	, m_iArrBegin(0)
	, m_iQuickItemIdx(-1)
	, m_bUIView(false)
	, m_fSlotInterval(0.f)
	, m_bShoppingMode(false)
	, m_iUIDiffuse(0)
	, m_iPlayerSoul(0)
	, m_fBackDissolveTime(0.f)
	, m_iMainRenderPass(0)
{
	ZeroMemory(m_arrShopSlot, sizeof(Client::SHOPSLOT_INFO) * VALNIR_SLOT_Y_COUNT);
}

HRESULT CUI_NPC_Valnir::Ready_GameObject_Prototype()
{
	return CUIBase::Ready_GameObject_Prototype();

}

HRESULT CUI_NPC_Valnir::Ready_GameObject_Clone(const _uint & _iSceneIdx, void * _pArg)
{
	FAILED_CHECK_RETURN(CUIBase::Ready_GameObject_Clone(_iSceneIdx, _pArg), E_FAIL);
	CManagement* pManagement = GET_MANAGEMENT;
	m_tUIDesc.vPos = { _float(0),_float(0) ,1.f };
	m_tUIDesc.vSize = { _float(WINCX),_float(WINCY) };

	FAILED_CHECK_RETURN(CUI_NPC_Valnir::Add_Com(), E_FAIL);
	m_arrShopSlot[0].iCost = 100;
	m_arrShopSlot[0].ubyItemCount = 5;
	m_arrShopSlot[0].ubyItem_ID = (_ubyte)eItemID::Heal_Medium;

	m_arrShopSlot[1].iCost = 500;
	m_arrShopSlot[1].ubyItemCount = 5;
	m_arrShopSlot[1].ubyItem_ID = (_ubyte)eItemID::Heal_Large;

	m_arrShopSlot[2].iCost = 1000;
	m_arrShopSlot[2].ubyItemCount = 5;
	m_arrShopSlot[2].ubyItem_ID = (_ubyte)eItemID::Heal_Full;

	m_arrShopSlot[3].iCost = 100;
	m_arrShopSlot[3].ubyItemCount = 99;
	m_arrShopSlot[3].ubyItem_ID = (_ubyte)eItemID::Poise_Full;
	
	m_arrShopSlot[4].iCost = 9000;
	m_arrShopSlot[4].ubyItemCount = 99;
	m_arrShopSlot[4].ubyItem_ID = (_ubyte)eItemID::Poise_Large;

	m_fSlotInterval = 5.f;

	return S_OK;
}

_uint CUI_NPC_Valnir::Update_GameObject(const _float & _fDeltaTime)
{
	CManagement* pManagement = GET_MANAGEMENT;
	CUIBase::Update_GameObject(_fDeltaTime);
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));

	if (pPlayer->Get_Npc_Valnir_Interact())
	{
		if (!m_bUIView)
		{
			m_fBackDissolveTime = 0.f;

			m_bUIView = true;
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
			pPlayer->Set_Npc_Valnir(true);
		}		
	}

	if (m_bUIView)
	{
		if (m_fBackDissolveTime < 8.f)
			m_fBackDissolveTime += _fDeltaTime * powf(PowValue, m_fBackDissolveTime);

		m_bPlayerInteract = true;
		pPlyUI->Set_ViewInterface(false);
		KeyInput();
	}
	else
	{
		if (m_bPlayerInteract)
		{
			if (!pPlyUI->Get_CheckViewInterface())
			{
				pPlyUI->Set_ViewInterface(true);
			}
			pPlayer->Set_Npc_Valnir(false);
			m_bPlayerInteract = false;
		}
		
		return m_iEvent;
	}

	
	return m_iEvent;
}

_uint CUI_NPC_Valnir::LateUpdate_GameObject(const _float & _fDeltaTime)
{
	if (!m_bUIView)
		return 0;
	CManagement::Get_Instance()->Add_RenderList(ERenderPriority::UIOrtho, this);
	m_pTransform->Update_Transform();

	return CUIBase::LateUpdate_GameObject(_fDeltaTime);
}

HRESULT CUI_NPC_Valnir::Render_GameObject()
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

	if (FAILED(SetUp_ConstantTable_Valnir(pEffect)))
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


	if (m_bShoppingMode)
	{
		Render_ShopBack(pEffect);
		Render_Slot(pEffect);
		Render_Item(pEffect);
		Render_Soul(pEffect);
		Render_CostSoul(pEffect);
		Render_ItemCount(pEffect);
		Render_FlavorText(pEffect);
		Render_FlavorTitle(pEffect);
	}
	if (!m_bShoppingMode&&m_fBackDissolveTime >= 8.f)
		Render_Script(pEffect);

	Safe_Release(pEffect);

	return S_OK;
}


HRESULT CUI_NPC_Valnir::Render_Soul(_EFFECT & _pEffect)
{
	_uint iSoul = m_iPlayerSoul;



	_float4x4 matOneWorld;
	D3DXMatrixIdentity(&matOneWorld);
	matOneWorld.m[0][0] = 10.f;
	matOneWorld.m[1][1] = 15.f;
	matOneWorld.m[2][2] = 1.f;
	memcpy(&matOneWorld.m[3][0],
		_float3(625.f - (WINCX >> 1),
			250.f - (WINCY >> 1),
			1.f), sizeof(_float3));

	_uint iOneSoul = iSoul % 10;

	_pEffect->SetMatrix("g_matWorld", &matOneWorld);
	//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", );
	Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iOneSoul);
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

	if (m_iPlayerSoul >= 10)
	{
		_float4x4 matTenWorld;
		D3DXMatrixIdentity(&matTenWorld);
		matTenWorld.m[0][0] = 10.f;
		matTenWorld.m[1][1] = 15.f;
		matTenWorld.m[2][2] = 1.f;
		memcpy(&matTenWorld.m[3][0],
			_float3((625.f - 10.f) - (WINCX >> 1),
				250.f - (WINCY >> 1),
				1.f), sizeof(_float3));

		iSoul = m_iPlayerSoul;
		iSoul = iSoul % 100;
		_uint iTenSoul = iSoul / 10;


		_pEffect->SetMatrix("g_matWorld", &matTenWorld);
		//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iTenSoul);
		Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iTenSoul);

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

		if (m_iPlayerSoul >= 100)
		{
			_float4x4 matHunWorld;
			D3DXMatrixIdentity(&matHunWorld);
			matHunWorld.m[0][0] = 10.f;
			matHunWorld.m[1][1] = 15.f;
			matHunWorld.m[2][2] = 1.f;
			memcpy(&matHunWorld.m[3][0],
				_float3((625.f - 20.f) - (WINCX >> 1),
					250.f - (WINCY >> 1),
					1.f), sizeof(_float3));

			iSoul = m_iPlayerSoul;
			iSoul = iSoul % 1000;
			_uint iHunSoul = iSoul / 100;

			_pEffect->SetMatrix("g_matWorld", &matHunWorld);
			//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iHunSoul);
			Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iHunSoul);

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


			if (m_iPlayerSoul >= 1000)
			{
				_float4x4 matColWorld;
				D3DXMatrixIdentity(&matColWorld);
				matColWorld.m[0][0] = 10.f;
				matColWorld.m[1][1] = 15.f;
				matColWorld.m[2][2] = 1.f;
				memcpy(&matColWorld.m[3][0],
					_float3((625.f - 30.f) - (WINCX >> 1),
						250.f - (WINCY >> 1),
						1.f), sizeof(_float3));



				_pEffect->SetMatrix("g_matWorld", &matColWorld);
				//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", );
				Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", 10);

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
				matSouWorld.m[0][0] = 10.f;
				matSouWorld.m[1][1] = 15.f;
				matSouWorld.m[2][2] = 1.f;
				memcpy(&matSouWorld.m[3][0],
					_float3((625.f - 40.f) - (WINCX >> 1),
						250.f - (WINCY >> 1),
						1.f), sizeof(_float3));

				iSoul = m_iPlayerSoul;
				iSoul = iSoul % 10000;
				_uint iSouSoul = iSoul / 1000;

				_pEffect->SetMatrix("g_matWorld", &matSouWorld);
				//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iSouSoul);
				Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iSouSoul);

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


HRESULT CUI_NPC_Valnir::Render_CostSoul(_EFFECT & _pEffect)
{
	for (_uint i = 0; i < VALNIR_SLOT_Y_COUNT; i++)
	{
		_uint iCost = m_arrShopSlot[i].iCost;

		_float4x4 matOneWorld;
		D3DXMatrixIdentity(&matOneWorld);
		matOneWorld.m[0][0] = 10.f;
		matOneWorld.m[1][1] = 15.f;
		matOneWorld.m[2][2] = 1.f;
		memcpy(&matOneWorld.m[3][0],
			_float3(-100.f,
				230.f - (i*(60.f + m_fSlotInterval)),
				1.f), sizeof(_float3));

		_uint iOneCost = iCost % 10;

		_pEffect->SetMatrix("g_matWorld", &matOneWorld);
		//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", );
		Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iOneCost);
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

		if (m_arrShopSlot[i].iCost >= 10)
		{
			_float4x4 matTenWorld;
			D3DXMatrixIdentity(&matTenWorld);
			matTenWorld.m[0][0] = 10.f;
			matTenWorld.m[1][1] = 15.f;
			matTenWorld.m[2][2] = 1.f;
			memcpy(&matTenWorld.m[3][0],
				_float3(-110.f,
					230.f - (i*(60.f + m_fSlotInterval)),
					1.f), sizeof(_float3));

			iCost = m_arrShopSlot[i].iCost;
			iCost = iCost % 100;
			_uint iTenCost = iCost / 10;


			_pEffect->SetMatrix("g_matWorld", &matTenWorld);
			//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", );
			Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iTenCost);
			
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

			if (m_arrShopSlot[i].iCost >= 100)
			{
				_float4x4 matHunWorld;
				D3DXMatrixIdentity(&matHunWorld);
				matHunWorld.m[0][0] = 10.f;
				matHunWorld.m[1][1] = 15.f;
				matHunWorld.m[2][2] = 1.f;
				memcpy(&matHunWorld.m[3][0],
					_float3(-120.f,
						230.f - (i*(60.f + m_fSlotInterval)),
						1.f), sizeof(_float3));

				iCost = m_arrShopSlot[i].iCost;
				iCost = iCost % 1000;
				_uint iHunCost = iCost / 100;

				_pEffect->SetMatrix("g_matWorld", &matHunWorld);
				//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iHunCost);
				Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iHunCost);

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


				if (m_arrShopSlot[i].iCost >= 1000)
				{
					_float4x4 matColWorld;
					D3DXMatrixIdentity(&matColWorld);
					matColWorld.m[0][0] = 10.f;
					matColWorld.m[1][1] = 15.f;
					matColWorld.m[2][2] = 1.f;
					memcpy(&matColWorld.m[3][0],
						_float3(-133.5f,
							230.f - (i*(60.f + m_fSlotInterval)),
							1.f), sizeof(_float3));

					_pEffect->SetMatrix("g_matWorld", &matColWorld);
					//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", );
					Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", 10);
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
					matSouWorld.m[0][0] = 10.f;
					matSouWorld.m[1][1] = 15.f;
					matSouWorld.m[2][2] = 1.f;
					memcpy(&matSouWorld.m[3][0],
						_float3(-140.f,
							230.f - (i*(60.f + m_fSlotInterval)),
							1.f), sizeof(_float3));

					iCost = m_arrShopSlot[i].iCost;
					iCost = iCost % 10000;
					_uint iSouCost = iCost / 1000;

					_pEffect->SetMatrix("g_matWorld", &matSouWorld);
					//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture",);
					Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture",  iSouCost);
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
	}

	return S_OK;
}


HRESULT CUI_NPC_Valnir::Render_Slot(_EFFECT & _pEffect)
{
	_float4x4 matSlotWorld;
	D3DXMatrixIdentity(&matSlotWorld);
	matSlotWorld.m[0][0] = 500.f;
	matSlotWorld.m[1][1] = 60.f;
	matSlotWorld.m[2][2] = 1.f;

	_uint iIndex = 0;

	for (_uint i = 0; i < VALNIR_SLOT_Y_COUNT; ++i)
	{
		for (_uint j = 0; j < 1; ++j)
		{
			memcpy(&matSlotWorld.m[3][0],
				_float3(-263.f + (j*(530.f + m_fSlotInterval)),
					230.f - (i*(60.f + m_fSlotInterval)),
					1.f), sizeof(_float3));
			iIndex = i * 1 + j;
			/*셰이더 값 설정 코드 필요함.*/
			_pEffect->SetMatrix("g_matWorld", &matSlotWorld);
			if (m_iArrBegin == iIndex)
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture", 1);
			else
				m_pSlotTexture->Set_Texture(_pEffect, "g_BaseTexture");

			_uint	iPassMax = 0;
			_pEffect->Begin(&iPassMax, 0);
			_pEffect->BeginPass(21);
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


HRESULT CUI_NPC_Valnir::Render_Item(_EFFECT & _pEffect)
{
	_float4x4 matItemWorld;
	D3DXMatrixIdentity(&matItemWorld);

	_uint iIndex = 0;

	for (_uint i = 0; i < VALNIR_SLOT_Y_COUNT; ++i)
	{
		for (_uint j = 0; j < 1; ++j)
		{
			matItemWorld.m[0][0] = 60.f;
			matItemWorld.m[1][1] = 60.f;
			matItemWorld.m[2][2] = 1.f;

			memcpy(&matItemWorld.m[3][0],
				_float3(-450.f + (j*(60.f + m_fSlotInterval)),
					230.f - (i*(60.f + m_fSlotInterval)),
					1.f), sizeof(_float3));
			iIndex = i + j;
			/*셰이더 값 설정 코드 필요함.*/
			_pEffect->SetMatrix("g_matWorld", &matItemWorld);

			//m_pIconTexture->Set_Texture(_pEffect, "g_BaseTexture", i + 1);
			Get_Manager()->Set_Texture(0, L"Component_Texture_Item_Icon", _pEffect, "g_BaseTexture", i + 1);
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

			if (iIndex == m_iArrBegin)
			{
				matItemWorld.m[0][0] = 200.f;
				matItemWorld.m[1][1] = 200.f;
				matItemWorld.m[2][2] = 1.f;

				memcpy(&matItemWorld.m[3][0],
					_float3(450.f,10.f,	1.f), sizeof(_float3));
				/*셰이더 값 설정 코드 필요함.*/
				_pEffect->SetMatrix("g_matWorld", &matItemWorld);

				//m_pIconTexture->Set_Texture(_pEffect, "g_BaseTexture", i + 1);
				Get_Manager()->Set_Texture(0, L"Component_Texture_Item_Icon", _pEffect, "g_BaseTexture", i + 1);

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
	}
	return S_OK;
}

HRESULT CUI_NPC_Valnir::Render_ItemCount(_EFFECT & _pEffect)
{
	for (_uint i = 0; i < VALNIR_SLOT_Y_COUNT; i++)
	{
		_uint iItemCount = m_arrShopSlot[i].ubyItemCount;

		_float4x4 matOneWorld;
		D3DXMatrixIdentity(&matOneWorld);
		matOneWorld.m[0][0] = 10.f;
		matOneWorld.m[1][1] = 15.f;
		matOneWorld.m[2][2] = 1.f;
		memcpy(&matOneWorld.m[3][0],
			_float3(-420.f,
				215.f - (i*(60.f + m_fSlotInterval)),
				1.f), sizeof(_float3));

		_uint iOneCost = iItemCount % 10;

		_pEffect->SetMatrix("g_matWorld", &matOneWorld);
		//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", );
		Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iOneCost);
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

		if (m_arrShopSlot[i].ubyItemCount >= 10)
		{
			_float4x4 matTenWorld;
			D3DXMatrixIdentity(&matTenWorld);
			matTenWorld.m[0][0] = 10.f;
			matTenWorld.m[1][1] = 15.f;
			matTenWorld.m[2][2] = 1.f;
			memcpy(&matTenWorld.m[3][0],
				_float3(-430.f,
					215.f - (i*(60.f + m_fSlotInterval)),
					1.f), sizeof(_float3));

			iItemCount = m_arrShopSlot[i].ubyItemCount;
			iItemCount = iItemCount % 100;
			_uint iTenCost = iItemCount / 10;


			_pEffect->SetMatrix("g_matWorld", &matTenWorld);
			//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", );
			Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iTenCost);

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

			if (m_arrShopSlot[i].ubyItemCount >= 100)
			{
				_float4x4 matHunWorld;
				D3DXMatrixIdentity(&matHunWorld);
				matHunWorld.m[0][0] = 10.f;
				matHunWorld.m[1][1] = 15.f;
				matHunWorld.m[2][2] = 1.f;
				memcpy(&matHunWorld.m[3][0],
					_float3(-440.f,
						215.f - (i*(60.f + m_fSlotInterval)),
						1.f), sizeof(_float3));

				iItemCount = m_arrShopSlot[i].ubyItemCount;
				iItemCount = iItemCount % 1000;
				_uint iHunCost = iItemCount / 100;

				_pEffect->SetMatrix("g_matWorld", &matHunWorld);
				//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", iHunCost);
				Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iHunCost);

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


				if (m_arrShopSlot[i].ubyItemCount >= 1000)
				{
					_float4x4 matColWorld;
					D3DXMatrixIdentity(&matColWorld);
					matColWorld.m[0][0] = 10.f;
					matColWorld.m[1][1] = 15.f;
					matColWorld.m[2][2] = 1.f;
					memcpy(&matColWorld.m[3][0],
						_float3(-453.5f,
							215.f - (i*(60.f + m_fSlotInterval)),
							1.f), sizeof(_float3));

					_pEffect->SetMatrix("g_matWorld", &matColWorld);
					//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture", );
					Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", 10);
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
					matSouWorld.m[0][0] = 10.f;
					matSouWorld.m[1][1] = 15.f;
					matSouWorld.m[2][2] = 1.f;
					memcpy(&matSouWorld.m[3][0],
						_float3(-460.f,
							215.f - (i*(60.f + m_fSlotInterval)),
							1.f), sizeof(_float3));

					iItemCount = m_arrShopSlot[i].ubyItemCount;
					iItemCount = iItemCount % 10000;
					_uint iSouCost = iItemCount / 1000;

					_pEffect->SetMatrix("g_matWorld", &matSouWorld);
					//m_pNumTexture->Set_Texture(_pEffect, "g_BaseTexture",);
					Get_Manager()->Set_Texture(0, L"Component_Texture_Number", _pEffect, "g_BaseTexture", iSouCost);
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
	}
	return S_OK;
}

HRESULT CUI_NPC_Valnir::Render_FlavorText(_EFFECT & _pEffect)
{
	_float4x4 matBackWorld;
	D3DXMatrixIdentity(&matBackWorld);

	matBackWorld.m[0][0] = WINCX;
	matBackWorld.m[1][1] = WINCY;
	matBackWorld.m[2][2] = 1.f;
	_float3 vFlavorPos = { 0.f ,0.f, 1.f };
	memcpy(&matBackWorld.m[3][0], vFlavorPos, sizeof(_float3));
	/*셰이더 값 설정 코드 필요함.*/
	_pEffect->SetMatrix("g_matWorld", &matBackWorld);
	m_pFlavorTexture->Set_Texture(_pEffect, "g_BaseTexture",m_iArrBegin);

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(0);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(_pEffect);

		return E_FAIL;
	}

	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();

	return S_OK;
}


HRESULT CUI_NPC_Valnir::Render_FlavorTitle(_EFFECT & _pEffect)
{
	_float4x4 matItemWorld;
	D3DXMatrixIdentity(&matItemWorld);

	_uint iIndex = 0;

	for (_uint i = 0; i < VALNIR_SLOT_Y_COUNT; ++i)
	{
		for (_uint j = 0; j < 1; ++j)
		{
			matItemWorld.m[0][0] = 480.f;
			matItemWorld.m[1][1] = 60.f;
			matItemWorld.m[2][2] = 1.f;

			memcpy(&matItemWorld.m[3][0],
				_float3(-280.f,
					230.f - (i*(60.f + m_fSlotInterval)),
					1.f), sizeof(_float3));
			/*셰이더 값 설정 코드 필요함.*/
			_pEffect->SetMatrix("g_matWorld", &matItemWorld);

			m_pItemTitleTexture->Set_Texture(_pEffect, "g_BaseTexture", i);

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

HRESULT CUI_NPC_Valnir::Render_Script(_EFFECT & _pEffect)
{
	_float4x4 matWorld;
	D3DXMatrixIdentity(&matWorld);
	memcpy(&matWorld.m[0][0], &m_pTransform->Get_TransformDesc().matWorld.m[0][0], sizeof(_float));
	memcpy(&matWorld.m[3][0], &m_pTransform->Get_TransformDesc().matWorld.m[3][0], sizeof(_float3));
	matWorld.m[1][1] = 250.f;
	matWorld.m[3][1] = -260.f;
	matWorld.m[3][0] = 110.f;
	matWorld.m[3][3] = 1.f;

	_pEffect->SetMatrix("g_matWorld", &matWorld);
	_pEffect->SetVector("g_vColor", &_float4(0.f, 0.f, 0.f, 0.f));
	const _uint& iQuest = Get_EventMgr()->Get_QuestFlag();
	if ((iQuest & QUEST_START_1) && (iQuest & QUEST_END_1))
		m_pScriptTexture->Set_Texture(_pEffect, "g_BaseTexture", 1);
	else
		m_pScriptTexture->Set_Texture(_pEffect, "g_BaseTexture", 0);

	_uint	iPassMax = 0;

	_pEffect->Begin(&iPassMax, 0);
	_pEffect->BeginPass(0);

	if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
	{
		ERROR_BOX(L"Failed to Render_VIBuffer");
		Safe_Release(_pEffect);

		return E_FAIL;
	}

	_pEffect->CommitChanges();
	_pEffect->EndPass();
	_pEffect->End();
	return S_OK;
}

HRESULT CUI_NPC_Valnir::Render_ShopBack(_EFFECT & _pEffect)
{
	for (_uint i = 0; i < 2; i++)
	{
		_float4x4 matBackWorld;
		D3DXMatrixIdentity(&matBackWorld);
		matBackWorld.m[0][0] = (_float)WINCX;
		matBackWorld.m[1][1] = (_float)WINCY;
		matBackWorld.m[2][2] = 1.f;
		_float3 vShopBack = { 0.f , 0.f, 1.f };
		memcpy(&matBackWorld.m[3][0], vShopBack, sizeof(_float3));
		/*셰이더 값 설정 코드 필요함.*/
		_pEffect->SetMatrix("g_matWorld", &matBackWorld);
		m_pBackTexture->Set_Texture(_pEffect, "g_BaseTexture", i);

		_uint	iPassMax = 0;

		_pEffect->Begin(&iPassMax, 0);
		_pEffect->BeginPass(0);
		if (FAILED(m_pVIBuffer->Render_VIBuffer(&_pEffect)))
		{
			ERROR_BOX(L"Failed to Render_VIBuffer");
			Safe_Release(_pEffect);

			return E_FAIL;
		}

		_pEffect->CommitChanges();
		_pEffect->EndPass();
		_pEffect->End();
	}
	return S_OK;
}

HRESULT CUI_NPC_Valnir::SetUp_ConstantTable_Valnir(_EFFECT & _pEffect)
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

	GET_MANAGEMENT->Set_Texture(0, m_tUIDesc.szTexName, _pEffect, "g_BaseTexture", m_iUIDiffuse);
	GET_MANAGEMENT->Set_Texture(0, L"Component_Texture_Dissolve", _pEffect, "g_DissolveTexture", 0);

	return S_OK;
}

void CUI_NPC_Valnir::Buy_Item()
{
	CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));

	if (m_iPlayerSoul >= m_arrShopSlot[m_iArrBegin].iCost&&m_arrShopSlot[m_iArrBegin].ubyItemCount > 0)
	{
		_int iCost = m_arrShopSlot[m_iArrBegin].iCost;
	//	6

		if (pPlyUI->Add_Used_Item((eItemID)(m_arrShopSlot[m_iArrBegin].ubyItem_ID)))
		{
			pPlyUI->Add_Souls(-iCost);
			m_arrShopSlot[m_iArrBegin].ubyItemCount--;

			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemBuySuccess.ogg", CSound_Manager::ESoundID::UI, 1.f);
		}
		else
		{
			Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
			Get_Manager()->PlaySounds(L"Environment_ItemBuyFail.ogg", CSound_Manager::ESoundID::UI, 1.f);
		}

	}
	else
	{
		Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
		Get_Manager()->PlaySounds(L"Environment_ItemBuyFail.ogg", CSound_Manager::ESoundID::UI, 1.f);
	}
}

void CUI_NPC_Valnir::KeyInput()
{
	CManagement* pManagement = GET_MANAGEMENT;
	CPlayer* pPlayer = dynamic_cast<CPlayer*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_Player"));

	if (m_bUIView)
	{
		if (m_bShoppingMode)
		{
			CPlayer_UI* pPlyUI = dynamic_cast<CPlayer_UI*>(Get_Manager()->Get_GameObject(m_iSceneIdx, L"Layer_PlayerUI", EFindGameObject::Proto, L"Player_UI"));
			m_iPlayerSoul = pPlyUI->Get_Souls();
			if (CManagement::Get_Instance()->Key_Down(DIK_W))
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_Button.wav", CSound_Manager::ESoundID::UI, 1.f);
				if (m_iArrBegin < 1)
				{
					m_iArrBegin += (1 * (VALNIR_SLOT_Y_COUNT - 1));
					return;
				}
				m_iArrBegin -= 1;
			}
			if (CManagement::Get_Instance()->Key_Down(DIK_S))
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_Button.wav", CSound_Manager::ESoundID::UI, 1.f);
				if (m_iArrBegin >= 1 * (VALNIR_SLOT_Y_COUNT - 1))
				{
					m_iArrBegin -= (1 * (VALNIR_SLOT_Y_COUNT - 1));
					return;
				}
				m_iArrBegin += 1;
			}
			if (pManagement->Key_Down(DIK_ESCAPE))
			{
				m_iArrBegin = 0;
				pPlayer->Set_Npc_Valnir(false);
				m_bUIView = false;
				m_bShoppingMode = false;
				m_iUIDiffuse = 0;
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
			}
			if (pManagement->Key_Down(DIK_RETURN))
			{
				Buy_Item();
			}
		}
		else
		{
			if (pManagement->Key_Down(DIK_RETURN))
			{
				if (m_fBackDissolveTime < 8.f)
				{
					m_fBackDissolveTime += 8.f;
					m_iUIDiffuse=1;
					return;
				}
				if (m_iUIDiffuse == 0)
					m_iUIDiffuse++;
				if (m_iUIDiffuse == 1)
				{
					m_iUIDiffuse = 0;
					m_bShoppingMode = true;
				}
				if (m_iUIDiffuse == 2)
				{
					pPlayer->Set_Npc_Valnir(false);
					m_bUIView = false;
					m_bShoppingMode = false;
					m_iUIDiffuse =0;
				}
			}
			if (CManagement::Get_Instance()->Key_Down(DIK_W))
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_Button.wav", CSound_Manager::ESoundID::UI, 1.f);
				if (m_iUIDiffuse != 0)
				{
					if (m_iUIDiffuse == 1)
						m_iUIDiffuse = 2;
					else
						m_iUIDiffuse = 1;
				}

			}
			if (CManagement::Get_Instance()->Key_Down(DIK_S))
			{
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_Button.wav", CSound_Manager::ESoundID::UI, 1.f);
				if (m_iUIDiffuse != 0)
				{
					if (m_iUIDiffuse == 2)
						m_iUIDiffuse = 1;
					else
						m_iUIDiffuse = 2;
				}
			}
			if (pManagement->Key_Down(DIK_ESCAPE))
			{
				pPlayer->Set_Npc_Valnir(false);
				pPlayer->Set_Npc_Interacting(false);
				m_bUIView = false;
				m_bShoppingMode = false;
				m_iUIDiffuse = 0;
				Get_Manager()->StopSound(CSound_Manager::ESoundID::UI);
				Get_Manager()->PlaySounds(L"Environment_UI.ogg", CSound_Manager::ESoundID::UI, 1.f);
			}
		}
	}
}

HRESULT CUI_NPC_Valnir::Add_Com()
{
	FAILED_CHECK_RETURN(CUIBase::Add_Com(), E_FAIL);

	/*Texture*/
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_ShopSlot",
		L"Com_SlotTexture", EComponentType::Static, (CComponent**)&m_pSlotTexture), E_FAIL);
	
	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_ShopBack",
		L"Com_ShopBackTexture", EComponentType::Static, (CComponent**)&m_pBackTexture), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_FlavorTexture",
		L"Com_FlavorTexture", EComponentType::Static, (CComponent**)&m_pFlavorTexture), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_FlavorTitle",
		L"Com_ItemTitleTexture", EComponentType::Static, (CComponent**)&m_pItemTitleTexture), E_FAIL);

	FAILED_CHECK_RETURN(CGameObject::Add_Component(0, L"Component_Texture_ValnirScript",
		L"Com_ScriptTexture", EComponentType::Static, (CComponent**)&m_pScriptTexture), E_FAIL);

	return S_OK;
}

CUI_NPC_Valnir * CUI_NPC_Valnir::Create(_DEVICE _pDevice)
{
	CUI_NPC_Valnir* pInstance = new CUI_NPC_Valnir(_pDevice);
	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		MSG_BOX(L"Failed to Create CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}

CGameObject * CUI_NPC_Valnir::Clone(const _uint & _iSceneIdx, void * _pArg)
{
	CUI_NPC_Valnir* pInstance = new CUI_NPC_Valnir(*this);
	if (FAILED(pInstance->Ready_GameObject_Clone(_iSceneIdx, _pArg)))
	{
		MSG_BOX(L"Failed to Clone CUI_Equipment.");
		return nullptr;
	}
	return pInstance;
}
